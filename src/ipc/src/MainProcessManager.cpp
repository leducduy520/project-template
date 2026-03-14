#include "MainProcessManager.hpp"
#include <iostream>
#include <sstream>
#include <chrono>

#ifdef _WIN32
#include <windows.h>
#include <process.h>
#else
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#endif

namespace ipc {

MainProcessManager::MainProcessManager(const std::string& queue_name)
    : queue_name_(queue_name)
    , next_process_id_(1)
    , monitoring_(false)
{
    ipc_manager_ = std::make_unique<IPCManager>(queue_name_, 0, true);
}

MainProcessManager::~MainProcessManager() {
    cleanup();
}

bool MainProcessManager::initialize() {
    if (!ipc_manager_->initialize()) {
        std::cerr << "MainProcessManager: Failed to initialize IPC" << std::endl;
        return false;
    }
    
    // Start receiving messages
    ipc_manager_->startReceiving([this](const IPCMessage& msg) {
        handleMessage(msg);
    });
    
    std::cout << "MainProcessManager: Initialized successfully" << std::endl;
    return true;
}

void MainProcessManager::cleanup() {
    stopMonitoring();
    
    // Send terminate command to all children
    broadcastCommand(CommandType::STOP);
    
    // Wait a bit for processes to terminate
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    
    {
        std::lock_guard<std::mutex> lock(children_mutex_);
        children_.clear();
    }
    
    if (ipc_manager_) {
        ipc_manager_->stopReceiving();
        ipc_manager_->cleanup();
    }
    
    std::cout << "MainProcessManager: Cleaned up" << std::endl;
}

uint32_t MainProcessManager::spawnChildProcess(const std::string& executable_path) {
    uint32_t process_id = next_process_id_++;
    
    if (!spawnProcess(executable_path, process_id)) {
        std::cerr << "MainProcessManager: Failed to spawn child process " << process_id << std::endl;
        return 0;
    }
    
    {
        std::lock_guard<std::mutex> lock(children_mutex_);
        children_[process_id] = std::make_unique<ChildProcessInfo>(process_id, executable_path);
        children_[process_id]->is_alive = true;
    }
    
    std::cout << "MainProcessManager: Spawned child process " << process_id << std::endl;
    return process_id;
}

bool MainProcessManager::sendCommand(uint32_t process_id, CommandType command) {
    {
        std::lock_guard<std::mutex> lock(children_mutex_);
        if (children_.find(process_id) == children_.end()) {
            std::cerr << "MainProcessManager: Process " << process_id << " not found" << std::endl;
            return false;
        }
    }
    
    static uint32_t message_counter = 0;
    IPCMessage message(MessageType::COMMAND, 0, ++message_counter);
    message.timestamp = getCurrentTimestamp();
    message.data.command = command;
    
    std::string cmd_str = "Command: " + std::to_string(static_cast<int>(command));
    std::copy(cmd_str.begin(), cmd_str.end(), message.payload.begin());
    message.payload[cmd_str.length()] = '\0';
    
    bool success = ipc_manager_->sendMessage(message, process_id);
    if (success) {
        std::cout << "MainProcessManager: Sent command " << static_cast<int>(command) 
                  << " to process " << process_id << std::endl;
    }
    
    return success;
}

size_t MainProcessManager::broadcastCommand(CommandType command) {
    std::vector<uint32_t> process_ids;
    {
        std::lock_guard<std::mutex> lock(children_mutex_);
        for (const auto& [id, info] : children_) {
            if (info->is_alive) {
                process_ids.push_back(id);
            }
        }
    }
    
    size_t count = 0;
    for (uint32_t pid : process_ids) {
        if (sendCommand(pid, command)) {
            count++;
        }
    }
    
    return count;
}

ProcessStatus MainProcessManager::getChildStatus(uint32_t process_id) const {
    std::lock_guard<std::mutex> lock(children_mutex_);
    auto it = children_.find(process_id);
    if (it != children_.end()) {
        return it->second->status;
    }
    return ProcessStatus::ERROR;
}

std::vector<uint32_t> MainProcessManager::getChildProcessIds() const {
    std::lock_guard<std::mutex> lock(children_mutex_);
    std::vector<uint32_t> ids;
    ids.reserve(children_.size());
    for (const auto& [id, info] : children_) {
        ids.push_back(id);
    }
    return ids;
}

void MainProcessManager::startMonitoring() {
    if (!monitoring_) {
        monitoring_ = true;
        monitoring_thread_ = std::thread(&MainProcessManager::monitoringThreadFunction, this);
        std::cout << "MainProcessManager: Started monitoring" << std::endl;
    }
}

void MainProcessManager::stopMonitoring() {
    if (monitoring_) {
        monitoring_ = false;
        if (monitoring_thread_.joinable()) {
            monitoring_thread_.join();
        }
        std::cout << "MainProcessManager: Stopped monitoring" << std::endl;
    }
}

bool MainProcessManager::isChildAlive(uint32_t process_id) const {
    std::lock_guard<std::mutex> lock(children_mutex_);
    auto it = children_.find(process_id);
    if (it != children_.end()) {
        return it->second->is_alive;
    }
    return false;
}

void MainProcessManager::handleMessage(const IPCMessage& message) {
    uint32_t process_id = message.process_id;
    
    {
        std::lock_guard<std::mutex> lock(children_mutex_);
        auto it = children_.find(process_id);
        if (it == children_.end()) {
            // Unknown process, ignore
            return;
        }
        
        auto& info = it->second;
        info->last_heartbeat = std::chrono::steady_clock::now();
        
        switch (message.type) {
            case MessageType::STATUS:
                info->status = message.data.status;
                std::cout << "MainProcessManager: Process " << process_id 
                          << " status: " << static_cast<int>(info->status) << std::endl;
                break;
                
            case MessageType::HEARTBEAT:
                info->is_alive = true;
                break;
                
            case MessageType::RESPONSE:
                std::cout << "MainProcessManager: Response from process " << process_id 
                          << ": " << message.payload.data() << std::endl;
                break;
                
            default:
                break;
        }
    }
}

void MainProcessManager::monitoringThreadFunction() {
    while (monitoring_) {
        std::this_thread::sleep_for(std::chrono::seconds(2));
        
        auto now = std::chrono::steady_clock::now();
        std::vector<uint32_t> dead_processes;
        
        {
            std::lock_guard<std::mutex> lock(children_mutex_);
            for (auto& [id, info] : children_) {
                auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(
                    now - info->last_heartbeat).count();
                
                // If no heartbeat for 10 seconds, mark as dead
                if (elapsed > 10) {
                    if (info->is_alive) {
                        std::cout << "MainProcessManager: Process " << id 
                                  << " appears to be dead (no heartbeat)" << std::endl;
                        info->is_alive = false;
                        info->status = ProcessStatus::ERROR;
                    }
                }
            }
        }
    }
}

bool MainProcessManager::spawnProcess(const std::string& executable_path, uint32_t process_id) {
#ifdef _WIN32
    std::stringstream ss;
    ss << executable_path << " " << process_id;
    std::string cmd = ss.str();
    
    STARTUPINFOA si;
    PROCESS_INFORMATION pi;
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));
    
    if (!CreateProcessA(
        nullptr,
        const_cast<char*>(cmd.c_str()),
        nullptr,
        nullptr,
        FALSE,
        0,
        nullptr,
        nullptr,
        &si,
        &pi)) {
        return false;
    }
    
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
    return true;
#else
    pid_t pid = fork();
    if (pid == 0) {
        // Child process
        std::stringstream ss;
        ss << process_id;
        std::string pid_str = ss.str();
        
        char* args[] = {
            const_cast<char*>(executable_path.c_str()),
            const_cast<char*>(pid_str.c_str()),
            nullptr
        };
        
        execv(executable_path.c_str(), args);
        exit(1);  // execv failed
    } else if (pid > 0) {
        // Parent process
        return true;
    } else {
        // Fork failed
        return false;
    }
#endif
}

bool MainProcessManager::isProcessRunning(uint32_t process_id) const {
    // This is a simplified check - in a real implementation,
    // you would check the actual process status
    std::lock_guard<std::mutex> lock(children_mutex_);
    auto it = children_.find(process_id);
    if (it != children_.end()) {
        return it->second->is_alive;
    }
    return false;
}

} // namespace ipc
