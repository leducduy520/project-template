#ifndef IPC_MANAGER_HPP
#define IPC_MANAGER_HPP

#include "IPCMessage.hpp"
#include <boost/interprocess/ipc/message_queue.hpp>
#include <boost/interprocess/sync/named_mutex.hpp>
#include <boost/interprocess/sync/scoped_lock.hpp>
#include <memory>
#include <string>
#include <thread>
#include <atomic>
#include <functional>
#include <mutex>
#include <unordered_map>

namespace ipc {

/**
 * @brief Callback function type for message handlers
 */
using MessageHandler = std::function<void(const IPCMessage&)>;

/**
 * @brief IPC Manager for inter-process communication
 * 
 * This class manages communication between processes using Boost.Interprocess
 * message queues. It supports both main process and child process modes.
 */
class IPCManager {
public:
    /**
     * @brief Constructor
     * @param queue_name Name of the message queue
     * @param process_id Unique process ID (0 for main, >0 for children)
     * @param is_main_process Whether this is the main process
     */
    IPCManager(const std::string& queue_name, uint32_t process_id, bool is_main_process = false);
    
    /**
     * @brief Destructor
     */
    ~IPCManager();
    
    // Non-copyable
    IPCManager(const IPCManager&) = delete;
    IPCManager& operator=(const IPCManager&) = delete;
    
    // Movable
    IPCManager(IPCManager&&) = default;
    IPCManager& operator=(IPCManager&&) = default;
    
    /**
     * @brief Initialize the IPC manager
     * @return true if successful, false otherwise
     */
    bool initialize();
    
    /**
     * @brief Cleanup and close IPC resources
     */
    void cleanup();
    
    /**
     * @brief Send a message to another process
     * @param message The message to send
     * @param target_process_id Target process ID (0 for main, >0 for specific child)
     * @return true if successful, false otherwise
     */
    bool sendMessage(const IPCMessage& message, uint32_t target_process_id);
    
    /**
     * @brief Receive a message (non-blocking)
     * @param message Output message
     * @return true if message received, false otherwise
     */
    bool receiveMessage(IPCMessage& message);
    
    /**
     * @brief Receive a message (blocking with timeout)
     * @param message Output message
     * @param timeout_ms Timeout in milliseconds (0 for infinite)
     * @return true if message received, false on timeout
     */
    bool receiveMessage(IPCMessage& message, uint32_t timeout_ms);
    
    /**
     * @brief Start the message receiving thread
     * @param handler Message handler callback
     */
    void startReceiving(MessageHandler handler);
    
    /**
     * @brief Stop the message receiving thread
     */
    void stopReceiving();
    
    /**
     * @brief Check if the manager is running
     * @return true if running, false otherwise
     */
    bool isRunning() const { return running_; }
    
    /**
     * @brief Get the process ID
     * @return Process ID
     */
    uint32_t getProcessId() const { return process_id_; }
    
    /**
     * @brief Get the queue name
     * @return Queue name
     */
    const std::string& getQueueName() const { return queue_name_; }

private:
    std::string queue_name_;
    uint32_t process_id_;
    bool is_main_process_;
    std::atomic<bool> running_;
    
    // Message queue for receiving messages
    std::unique_ptr<boost::interprocess::message_queue> receive_queue_;
    
    // Message queues for sending to other processes (main process only)
    std::unordered_map<uint32_t, std::unique_ptr<boost::interprocess::message_queue>> send_queues_;
    std::mutex send_queues_mutex_;
    
    // Receiving thread
    std::thread receive_thread_;
    MessageHandler message_handler_;
    
    /**
     * @brief Create or open a message queue
     * @param queue_name Name of the queue
     * @param create Whether to create the queue (true) or open existing (false)
     * @return Pointer to message queue or nullptr on error
     */
    std::unique_ptr<boost::interprocess::message_queue> createOrOpenQueue(
        const std::string& queue_name, bool create);
    
    /**
     * @brief Receiving thread function
     */
    void receiveThreadFunction();
    
    /**
     * @brief Generate queue name for a process
     * @param base_name Base queue name
     * @param process_id Process ID
     * @return Generated queue name
     */
    static std::string generateQueueName(const std::string& base_name, uint32_t process_id);
};

} // namespace ipc

#endif // IPC_MANAGER_HPP
