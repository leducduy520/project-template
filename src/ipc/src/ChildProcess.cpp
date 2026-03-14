#include "ChildProcess.hpp"
#include <iostream>
#include <thread>
#include <chrono>

namespace ipc {

ChildProcess::ChildProcess(uint32_t process_id, const std::string& queue_name)
    : process_id_(process_id)
    , queue_name_(queue_name)
    , status_(ProcessStatus::IDLE)
    , running_(false)
    , message_counter_(0)
{
    ipc_manager_ = std::make_unique<IPCManager>(queue_name_, process_id_, false);
}

ChildProcess::~ChildProcess() {
    stop();
}

bool ChildProcess::initialize() {
    if (!ipc_manager_->initialize()) {
        std::cerr << "ChildProcess " << process_id_ << ": Failed to initialize IPC" << std::endl;
        return false;
    }
    
    // Start receiving messages
    ipc_manager_->startReceiving([this](const IPCMessage& msg) {
        handleMessage(msg);
    });
    
    status_ = ProcessStatus::IDLE;
    onInitialize();
    
    std::cout << "ChildProcess " << process_id_ << ": Initialized successfully" << std::endl;
    return true;
}

void ChildProcess::run() {
    running_ = true;
    status_ = ProcessStatus::RUNNING;
    
    std::cout << "ChildProcess " << process_id_ << ": Started running" << std::endl;
    
    // Send initial status
    sendStatus(ProcessStatus::RUNNING);
    
    // Main work loop
    while (running_) {
        if (status_ == ProcessStatus::RUNNING) {
            doWork();
        }
        
        // Send periodic heartbeat
        static auto last_heartbeat = std::chrono::steady_clock::now();
        auto now = std::chrono::steady_clock::now();
        if (std::chrono::duration_cast<std::chrono::seconds>(now - last_heartbeat).count() >= 5) {
            IPCMessage heartbeat(MessageType::HEARTBEAT, process_id_, ++message_counter_);
            heartbeat.timestamp = getCurrentTimestamp();
            ipc_manager_->sendMessage(heartbeat, 0);  // Send to main process (ID 0)
            last_heartbeat = now;
        }
        
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    
    status_ = ProcessStatus::STOPPED;
    sendStatus(ProcessStatus::STOPPED);
    onCleanup();
    
    std::cout << "ChildProcess " << process_id_ << ": Stopped" << std::endl;
}

void ChildProcess::stop() {
    if (!running_) {
        return;
    }
    
    running_ = false;
    status_ = ProcessStatus::STOPPED;
    
    if (ipc_manager_) {
        ipc_manager_->stopReceiving();
        ipc_manager_->cleanup();
    }
}

void ChildProcess::handleMessage(const IPCMessage& message) {
    switch (message.type) {
        case MessageType::COMMAND:
            handleCommand(message);
            break;
        case MessageType::TERMINATE:
            std::cout << "ChildProcess " << process_id_ << ": Received terminate command" << std::endl;
            stop();
            break;
        default:
            // Ignore other message types
            break;
    }
}

void ChildProcess::handleCommand(const IPCMessage& message) {
    CommandType cmd = message.data.command;
    uint32_t msg_id = message.message_id;
    
    std::cout << "ChildProcess " << process_id_ << ": Received command " 
              << static_cast<int>(cmd) << std::endl;
    
    bool success = true;
    std::string payload;
    
    switch (cmd) {
        case CommandType::START:
            if (status_ == ProcessStatus::IDLE || status_ == ProcessStatus::STOPPED) {
                status_ = ProcessStatus::RUNNING;
                sendStatus(ProcessStatus::RUNNING);
            }
            break;
            
        case CommandType::STOP:
            if (status_ == ProcessStatus::RUNNING || status_ == ProcessStatus::PAUSED) {
                status_ = ProcessStatus::STOPPED;
                sendStatus(ProcessStatus::STOPPED);
            }
            break;
            
        case CommandType::PAUSE:
            if (status_ == ProcessStatus::RUNNING) {
                status_ = ProcessStatus::PAUSED;
                sendStatus(ProcessStatus::PAUSED);
            }
            break;
            
        case CommandType::RESUME:
            if (status_ == ProcessStatus::PAUSED) {
                status_ = ProcessStatus::RUNNING;
                sendStatus(ProcessStatus::RUNNING);
            }
            break;
            
        case CommandType::GET_STATUS:
            sendStatus(status_);
            return;  // Status already sent
            
        case CommandType::CUSTOM:
            // Handle custom commands (override in derived classes)
            payload = "Custom command received";
            break;
            
        default:
            success = false;
            payload = "Unknown command";
            break;
    }
    
    sendResponse(msg_id, success, payload);
}

void ChildProcess::sendStatus(ProcessStatus status) {
    IPCMessage status_msg(MessageType::STATUS, process_id_, ++message_counter_);
    status_msg.timestamp = getCurrentTimestamp();
    status_msg.data.status = status;
    
    std::string status_str = "Status: " + std::to_string(static_cast<int>(status));
    std::copy(status_str.begin(), status_str.end(), status_msg.payload.begin());
    status_msg.payload[status_str.length()] = '\0';
    
    ipc_manager_->sendMessage(status_msg, 0);  // Send to main process
}

void ChildProcess::sendResponse(uint32_t original_message_id, bool success, const std::string& payload) {
    IPCMessage response(MessageType::RESPONSE, process_id_, ++message_counter_);
    response.timestamp = getCurrentTimestamp();
    response.message_id = original_message_id;  // Reference to original message
    
    std::string response_str = success ? "OK" : "ERROR";
    if (!payload.empty()) {
        response_str += ": " + payload;
    }
    
    std::copy(response_str.begin(), response_str.end(), response.payload.begin());
    response.payload[response_str.length()] = '\0';
    
    ipc_manager_->sendMessage(response, 0);  // Send to main process
}

void ChildProcess::doWork() {
    // Default implementation - override in derived classes
    // This is where the actual work of the child process happens
    static int counter = 0;
    if (++counter % 50 == 0) {
        std::cout << "ChildProcess " << process_id_ << ": Working... (iteration " << counter << ")" << std::endl;
    }
}

} // namespace ipc
