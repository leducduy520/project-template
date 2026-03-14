#ifndef MAIN_PROCESS_MANAGER_HPP
#define MAIN_PROCESS_MANAGER_HPP

#include "IPCManager.hpp"
#include "ChildProcess.hpp"
#include <vector>
#include <memory>
#include <unordered_map>
#include <thread>
#include <atomic>
#include <mutex>
#include <string>
#include <chrono>

namespace ipc {

/**
 * @brief Information about a managed child process
 */
struct ChildProcessInfo {
    uint32_t process_id;
    ProcessStatus status;
    std::string executable_path;
    std::chrono::steady_clock::time_point last_heartbeat;
    bool is_alive;
    
    ChildProcessInfo(uint32_t id, const std::string& exec_path)
        : process_id(id)
        , status(ProcessStatus::IDLE)
        , executable_path(exec_path)
        , last_heartbeat(std::chrono::steady_clock::now())
        , is_alive(false)
    {}
};

/**
 * @brief Main process manager
 * 
 * This class manages multiple child processes and handles IPC communication
 * with them. It can spawn, monitor, and control child processes.
 */
class MainProcessManager {
public:
    /**
     * @brief Constructor
     * @param queue_name Base name for message queues
     */
    explicit MainProcessManager(const std::string& queue_name = "ipc_queue");
    
    /**
     * @brief Destructor
     */
    ~MainProcessManager();
    
    // Non-copyable
    MainProcessManager(const MainProcessManager&) = delete;
    MainProcessManager& operator=(const MainProcessManager&) = delete;
    
    // Movable
    MainProcessManager(MainProcessManager&&) = default;
    MainProcessManager& operator=(MainProcessManager&&) = default;
    
    /**
     * @brief Initialize the main process manager
     * @return true if successful, false otherwise
     */
    bool initialize();
    
    /**
     * @brief Cleanup and stop all child processes
     */
    void cleanup();
    
    /**
     * @brief Spawn a new child process
     * @param executable_path Path to the child process executable
     * @return Process ID if successful, 0 otherwise
     */
    uint32_t spawnChildProcess(const std::string& executable_path);
    
    /**
     * @brief Send a command to a child process
     * @param process_id Target process ID
     * @param command Command to send
     * @return true if successful, false otherwise
     */
    bool sendCommand(uint32_t process_id, CommandType command);
    
    /**
     * @brief Send a command to all child processes
     * @param command Command to send
     * @return Number of processes the command was sent to
     */
    size_t broadcastCommand(CommandType command);
    
    /**
     * @brief Get status of a child process
     * @param process_id Process ID
     * @return Process status, or ProcessStatus::ERROR if not found
     */
    ProcessStatus getChildStatus(uint32_t process_id) const;
    
    /**
     * @brief Get all child process IDs
     * @return Vector of process IDs
     */
    std::vector<uint32_t> getChildProcessIds() const;
    
    /**
     * @brief Start monitoring child processes
     */
    void startMonitoring();
    
    /**
     * @brief Stop monitoring child processes
     */
    void stopMonitoring();
    
    /**
     * @brief Check if a child process is alive
     * @param process_id Process ID
     * @return true if alive, false otherwise
     */
    bool isChildAlive(uint32_t process_id) const;

private:
    std::string queue_name_;
    std::unique_ptr<IPCManager> ipc_manager_;
    std::atomic<uint32_t> next_process_id_;
    std::atomic<bool> monitoring_;
    
    // Child process management
    mutable std::mutex children_mutex_;
    std::unordered_map<uint32_t, std::unique_ptr<ChildProcessInfo>> children_;
    
    // Monitoring thread
    std::thread monitoring_thread_;
    
    /**
     * @brief Handle incoming messages from child processes
     * @param message The received message
     */
    void handleMessage(const IPCMessage& message);
    
    /**
     * @brief Monitoring thread function
     */
    void monitoringThreadFunction();
    
    /**
     * @brief Spawn a child process (platform-specific)
     * @param executable_path Path to executable
     * @param process_id Process ID to assign
     * @return true if successful, false otherwise
     */
    bool spawnProcess(const std::string& executable_path, uint32_t process_id);
    
    /**
     * @brief Check if a process is still running (platform-specific)
     * @param process_id Process ID
     * @return true if running, false otherwise
     */
    bool isProcessRunning(uint32_t process_id) const;
};

} // namespace ipc

#endif // MAIN_PROCESS_MANAGER_HPP
