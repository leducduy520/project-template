#ifndef CHILD_PROCESS_HPP
#define CHILD_PROCESS_HPP

#include "IPCManager.hpp"
#include <string>
#include <atomic>
#include <memory>
#include <thread>

namespace ipc {

/**
 * @brief Child process implementation
 * 
 * This class represents a child process that can communicate with the main process
 * via IPC. It handles commands from the main process and sends responses/status updates.
 */
class ChildProcess {
public:
    /**
     * @brief Constructor
     * @param process_id Unique process ID
     * @param queue_name Base name for message queues
     */
    ChildProcess(uint32_t process_id, const std::string& queue_name = "ipc_queue");
    
    /**
     * @brief Destructor
     */
    ~ChildProcess();
    
    // Non-copyable
    ChildProcess(const ChildProcess&) = delete;
    ChildProcess& operator=(const ChildProcess&) = delete;
    
    // Movable
    ChildProcess(ChildProcess&&) = default;
    ChildProcess& operator=(ChildProcess&&) = default;
    
    /**
     * @brief Initialize the child process
     * @return true if successful, false otherwise
     */
    bool initialize();
    
    /**
     * @brief Run the child process main loop
     */
    void run();
    
    /**
     * @brief Stop the child process
     */
    void stop();
    
    /**
     * @brief Get the process ID
     * @return Process ID
     */
    uint32_t getProcessId() const { return process_id_; }
    
    /**
     * @brief Get the current status
     * @return Current process status
     */
    ProcessStatus getStatus() const { return status_; }

private:
    uint32_t process_id_;
    std::string queue_name_;
    std::unique_ptr<IPCManager> ipc_manager_;
    std::atomic<ProcessStatus> status_;
    std::atomic<bool> running_;
    std::atomic<uint32_t> message_counter_;
    
    /**
     * @brief Handle incoming messages
     * @param message The received message
     */
    void handleMessage(const IPCMessage& message);
    
    /**
     * @brief Handle a command message
     * @param message The command message
     */
    void handleCommand(const IPCMessage& message);
    
    /**
     * @brief Send status update to main process
     * @param status The status to send
     */
    void sendStatus(ProcessStatus status);
    
    /**
     * @brief Send response to main process
     * @param original_message_id Original message ID
     * @param success Whether the operation was successful
     * @param payload Optional payload data
     */
    void sendResponse(uint32_t original_message_id, bool success, const std::string& payload = "");
    
    /**
     * @brief Process work (override in derived classes)
     */
    virtual void doWork();
    
    /**
     * @brief Process initialization (override in derived classes)
     */
    virtual void onInitialize() {}
    
    /**
     * @brief Process cleanup (override in derived classes)
     */
    virtual void onCleanup() {}
};

} // namespace ipc

#endif // CHILD_PROCESS_HPP
