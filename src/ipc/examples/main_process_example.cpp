#include "MainProcessManager.hpp"
#include <iostream>
#include <thread>
#include <chrono>
#include <string>

int main(int argc, char* argv[]) {
    std::cout << "=== Main Process Example ===" << std::endl;
    
    ipc::MainProcessManager manager("ipc_queue");
    
    if (!manager.initialize()) {
        std::cerr << "Failed to initialize main process manager" << std::endl;
        return 1;
    }
    
    // Start monitoring
    manager.startMonitoring();
    
    // Get the executable path for child processes
    std::string child_executable = "./ChildProcessExample";
    if (argc > 1) {
        child_executable = argv[1];
    }
    
    std::cout << "Spawning child processes..." << std::endl;
    
    // Spawn 3 child processes
    std::vector<uint32_t> child_ids;
    for (int i = 0; i < 3; ++i) {
        uint32_t pid = manager.spawnChildProcess(child_executable);
        if (pid > 0) {
            child_ids.push_back(pid);
            std::cout << "Spawned child process with ID: " << pid << std::endl;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
    
    if (child_ids.empty()) {
        std::cerr << "No child processes were spawned" << std::endl;
        manager.cleanup();
        return 1;
    }
    
    std::this_thread::sleep_for(std::chrono::seconds(2));
    
    // Send commands to child processes
    std::cout << "\nSending START command to all children..." << std::endl;
    manager.broadcastCommand(ipc::CommandType::START);
    
    std::this_thread::sleep_for(std::chrono::seconds(3));
    
    std::cout << "\nSending PAUSE command to first child..." << std::endl;
    if (!child_ids.empty()) {
        manager.sendCommand(child_ids[0], ipc::CommandType::PAUSE);
    }
    
    std::this_thread::sleep_for(std::chrono::seconds(2));
    
    std::cout << "\nSending RESUME command to first child..." << std::endl;
    if (!child_ids.empty()) {
        manager.sendCommand(child_ids[0], ipc::CommandType::RESUME);
    }
    
    std::this_thread::sleep_for(std::chrono::seconds(3));
    
    // Check status of all children
    std::cout << "\nChecking status of all children..." << std::endl;
    for (uint32_t pid : child_ids) {
        auto status = manager.getChildStatus(pid);
        std::cout << "Process " << pid << " status: " 
                  << static_cast<int>(status) << std::endl;
    }
    
    std::this_thread::sleep_for(std::chrono::seconds(2));
    
    std::cout << "\nSending STOP command to all children..." << std::endl;
    manager.broadcastCommand(ipc::CommandType::STOP);
    
    std::this_thread::sleep_for(std::chrono::seconds(2));
    
    std::cout << "\nCleaning up..." << std::endl;
    manager.cleanup();
    
    std::cout << "Main process example completed" << std::endl;
    return 0;
}
