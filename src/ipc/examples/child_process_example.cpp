#include "ChildProcess.hpp"
#include <iostream>
#include <cstdlib>
#include <string>

int main(int argc, char* argv[]) {
    // Get process ID from command line argument
    uint32_t process_id = 1;
    if (argc > 1) {
        process_id = static_cast<uint32_t>(std::atoi(argv[1]));
    }
    
    std::cout << "=== Child Process Example (ID: " << process_id << ") ===" << std::endl;
    
    ipc::ChildProcess child(process_id, "ipc_queue");
    
    if (!child.initialize()) {
        std::cerr << "Failed to initialize child process" << std::endl;
        return 1;
    }
    
    // Run the child process
    // This will block until stop() is called
    child.run();
    
    std::cout << "Child process " << process_id << " exiting" << std::endl;
    return 0;
}
