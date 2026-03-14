#ifndef IPC_MESSAGE_HPP
#define IPC_MESSAGE_HPP

#include <cstdint>
#include <string>
#include <array>
#include <chrono>

namespace ipc {

/**
 * @brief Message types for IPC communication
 */
enum class MessageType : uint8_t {
    COMMAND = 0,      // Command from main to child
    RESPONSE = 1,     // Response from child to main
    STATUS = 2,       // Status update from child
    HEARTBEAT = 3,    // Heartbeat message
    TERMINATE = 4     // Termination request
};

/**
 * @brief Command types that can be sent to child processes
 */
enum class CommandType : uint8_t {
    START = 0,
    STOP = 1,
    PAUSE = 2,
    RESUME = 3,
    GET_STATUS = 4,
    CUSTOM = 5
};

/**
 * @brief Status of a child process
 */
enum class ProcessStatus : uint8_t {
    IDLE = 0,
    RUNNING = 1,
    PAUSED = 2,
    STOPPED = 3,
    ERROR = 4
};

/**
 * @brief IPC Message structure
 * 
 * This structure is designed to be used with Boost.Interprocess message queues.
 * It contains a fixed-size payload to ensure efficient communication.
 */
struct IPCMessage {
    MessageType type;                    // Message type
    uint32_t process_id;                 // Process ID (0 for main, >0 for children)
    uint32_t message_id;                 // Unique message ID
    uint64_t timestamp;                  // Timestamp in milliseconds
    union {
        CommandType command;             // Command type (if type == COMMAND)
        ProcessStatus status;            // Process status (if type == STATUS)
        uint8_t reserved[4];            // Reserved bytes
    } data;
    std::array<char, 256> payload;       // Payload data (can contain JSON, text, etc.)
    
    IPCMessage() 
        : type(MessageType::HEARTBEAT)
        , process_id(0)
        , message_id(0)
        , timestamp(0)
    {
        payload.fill(0);
        data.reserved[0] = 0;
    }
    
    IPCMessage(MessageType msg_type, uint32_t pid, uint32_t msg_id)
        : type(msg_type)
        , process_id(pid)
        , message_id(msg_id)
        , timestamp(0)
    {
        payload.fill(0);
        data.reserved[0] = 0;
    }
};

/**
 * @brief Get current timestamp in milliseconds
 */
inline uint64_t getCurrentTimestamp() {
    return std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch()
    ).count();
}

} // namespace ipc

#endif // IPC_MESSAGE_HPP
