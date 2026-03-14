#include "IPCManager.hpp"
#include <boost/interprocess/shared_memory_object.hpp>
#include <boost/interprocess/mapped_region.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <iostream>
#include <stdexcept>

namespace ipc {

IPCManager::IPCManager(const std::string& queue_name, uint32_t process_id, bool is_main_process)
    : queue_name_(queue_name)
    , process_id_(process_id)
    , is_main_process_(is_main_process)
    , running_(false)
{
}

IPCManager::~IPCManager() {
    cleanup();
}

bool IPCManager::initialize() {
    try {
        // Generate queue name for this process
        std::string my_queue_name = generateQueueName(queue_name_, process_id_);
        
        // Create or open the receive queue
        if (is_main_process_) {
            // Main process creates its own queue
            receive_queue_ = createOrOpenQueue(my_queue_name, true);
        } else {
            // Child process creates its own queue
            receive_queue_ = createOrOpenQueue(my_queue_name, true);
        }
        
        if (!receive_queue_) {
            std::cerr << "Failed to create/open message queue: " << my_queue_name << std::endl;
            return false;
        }
        
        running_ = true;
        return true;
    } catch (const std::exception& e) {
        std::cerr << "IPCManager initialization error: " << e.what() << std::endl;
        return false;
    }
}

void IPCManager::cleanup() {
    stopReceiving();
    
    {
        std::lock_guard<std::mutex> lock(send_queues_mutex_);
        send_queues_.clear();
    }
    
    receive_queue_.reset();
    
    // Try to remove the message queue
    try {
        if (!queue_name_.empty()) {
            std::string my_queue_name = generateQueueName(queue_name_, process_id_);
            boost::interprocess::message_queue::remove(my_queue_name.c_str());
        }
    } catch (...) {
        // Ignore cleanup errors
    }
    
    running_ = false;
}

bool IPCManager::sendMessage(const IPCMessage& message, uint32_t target_process_id) {
    if (!running_) {
        return false;
    }
    
    try {
        std::string target_queue_name = generateQueueName(queue_name_, target_process_id);
        boost::interprocess::message_queue* target_queue = nullptr;
        
        {
            std::lock_guard<std::mutex> lock(send_queues_mutex_);
            auto it = send_queues_.find(target_process_id);
            if (it == send_queues_.end()) {
                // Try to open the target queue
                auto queue = createOrOpenQueue(target_queue_name, false);
                if (!queue) {
                    return false;
                }
                send_queues_[target_process_id] = std::move(queue);
                target_queue = send_queues_[target_process_id].get();
            } else {
                target_queue = it->second.get();
            }
        }
        
        if (target_queue) {
            target_queue->send(&message, sizeof(IPCMessage), 0);
            return true;
        }
    } catch (const std::exception& e) {
        std::cerr << "Error sending message: " << e.what() << std::endl;
    }
    
    return false;
}

bool IPCManager::receiveMessage(IPCMessage& message) {
    if (!running_ || !receive_queue_) {
        return false;
    }
    
    try {
        std::size_t received_size;
        unsigned int priority;
        if (receive_queue_->try_receive(&message, sizeof(IPCMessage), received_size, priority)) {
            if (received_size == sizeof(IPCMessage)) {
                return true;
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "Error receiving message: " << e.what() << std::endl;
    }
    
    return false;
}

bool IPCManager::receiveMessage(IPCMessage& message, uint32_t timeout_ms) {
    if (!running_ || !receive_queue_) {
        return false;
    }
    
    try {
        std::size_t received_size;
        unsigned int priority;
        
        if (timeout_ms == 0) {
            // Infinite wait
            receive_queue_->receive(&message, sizeof(IPCMessage), received_size, priority);
        } else {
            // Wait with timeout
            boost::posix_time::ptime timeout = boost::posix_time::microsec_clock::universal_time() +
                boost::posix_time::milliseconds(timeout_ms);
            if (receive_queue_->timed_receive(&message, sizeof(IPCMessage), received_size, priority, timeout)) {
                if (received_size == sizeof(IPCMessage)) {
                    return true;
                }
            } else {
                return false; // Timeout
            }
        }
        
        if (received_size == sizeof(IPCMessage)) {
            return true;
        }
    } catch (const std::exception& e) {
        std::cerr << "Error receiving message: " << e.what() << std::endl;
    }
    
    return false;
}

void IPCManager::startReceiving(MessageHandler handler) {
    if (running_ && !receive_thread_.joinable()) {
        message_handler_ = std::move(handler);
        receive_thread_ = std::thread(&IPCManager::receiveThreadFunction, this);
    }
}

void IPCManager::stopReceiving() {
    running_ = false;
    if (receive_thread_.joinable()) {
        receive_thread_.join();
    }
    message_handler_ = nullptr;
}

std::unique_ptr<boost::interprocess::message_queue> IPCManager::createOrOpenQueue(
    const std::string& queue_name, bool create) {
    try {
        if (create) {
            // Remove existing queue if it exists
            boost::interprocess::message_queue::remove(queue_name.c_str());
            
            // Create new queue with max 100 messages
            return std::make_unique<boost::interprocess::message_queue>(
                boost::interprocess::create_only,
                queue_name.c_str(),
                100,  // max messages
                sizeof(IPCMessage)  // message size
            );
        } else {
            // Open existing queue
            return std::make_unique<boost::interprocess::message_queue>(
                boost::interprocess::open_only,
                queue_name.c_str()
            );
        }
    } catch (const boost::interprocess::interprocess_exception& e) {
        if (create) {
            // If create failed, try to open
            try {
                return std::make_unique<boost::interprocess::message_queue>(
                    boost::interprocess::open_only,
                    queue_name.c_str()
                );
            } catch (...) {
                return nullptr;
            }
        }
        return nullptr;
    }
}

void IPCManager::receiveThreadFunction() {
    IPCMessage message;
    
    while (running_) {
        if (receiveMessage(message, 100)) {  // 100ms timeout
            if (message_handler_) {
                try {
                    message_handler_(message);
                } catch (const std::exception& e) {
                    std::cerr << "Error in message handler: " << e.what() << std::endl;
                }
            }
        }
    }
}

std::string IPCManager::generateQueueName(const std::string& base_name, uint32_t process_id) {
    return base_name + "_" + std::to_string(process_id);
}

} // namespace ipc
