#ifndef THREAD_POOL_GAME_H
#define THREAD_POOL_GAME_H

#include <future>
#include <memory>
#include <type_traits>
#include <atomic>
#include <boost/asio/thread_pool.hpp>
#include <boost/asio/post.hpp>

/**
 * @brief Wrapper class for boost::asio::thread_pool
 * 
 * Provides a singleton thread pool interface with priority-based task submission.
 * Thread-safe singleton implementation.
 */
class ThreadPool
{
public:
    /**
     * @brief Get the singleton instance of ThreadPool
     * @param num_threads Number of threads in the pool (default: hardware_concurrency)
     * @return Reference to the singleton ThreadPool instance
     */
    static ThreadPool& getInstance(size_t num_threads = 0);

    /**
     * @brief Submit a task to the thread pool (member function version)
     * @tparam ClassType Class type
     * @tparam Ret Return type (must be void)
     * @param priority Priority level (currently not used, reserved for future use)
     * @param func Member function pointer
     * @param obj Object pointer
     * @return std::future<void> Future object to track task completion
     */
    template <typename ClassType, typename Ret>
    std::future<void> submit(int priority, Ret (ClassType::*func)(), ClassType* obj);

    /**
     * @brief Submit a task to the thread pool (free function version)
     * @tparam Function Function type (must not be a member function pointer)
     * @tparam Args Argument types
     * @param priority Priority level (currently not used, reserved for future use)
     * @param func Function to execute
     * @param args Arguments to pass to the function
     * @return std::future<void> Future object to track task completion
     */
    template <typename Function, typename... Args>
    typename std::enable_if<!std::is_member_function_pointer<typename std::decay<Function>::type>::value,
                            std::future<void>>::type
    submit(int priority, Function&& func, Args&&... args);

    /**
     * @brief Get the number of threads in the pool
     * @return Number of threads
     */
    size_t getThreadCount() const noexcept;

    /**
     * @brief Wait for all tasks to complete
     */
    void wait();

    // Delete copy constructor and assignment operator
    ThreadPool(const ThreadPool&) = delete;
    ThreadPool& operator=(const ThreadPool&) = delete;
    ThreadPool(ThreadPool&&) = delete;
    ThreadPool& operator=(ThreadPool&&) = delete;

private:
    /**
     * @brief Private constructor for singleton pattern
     * @param num_threads Number of threads in the pool
     */
    explicit ThreadPool(size_t num_threads);

    /**
     * @brief Destructor
     */
    ~ThreadPool();

    std::unique_ptr<boost::asio::thread_pool> m_pool;
    size_t m_thread_count;
    mutable std::atomic<bool> m_joined{false};
};

// Template implementation for member functions
template <typename ClassType, typename Ret>
std::future<void> ThreadPool::submit(int priority, Ret (ClassType::*func)(), ClassType* obj)
{
    (void)priority; // Reserved for future use
    auto promise = std::make_shared<std::promise<void>>();
    auto future = promise->get_future();

    // Wrap the member function call in a lambda
    auto task = [promise, func, obj]() {
        try {
            (obj->*func)();
            promise->set_value();
        }
        catch (...) {
            promise->set_exception(std::current_exception());
        }
    };

    // Post the task to the thread pool
    boost::asio::post(*m_pool, std::move(task));

    return future;
}

// Template implementation for free functions
template <typename Function, typename... Args>
typename std::enable_if<!std::is_member_function_pointer<std::decay_t<Function>>::value, std::future<void>>::type
ThreadPool::submit(int priority, Function&& func, Args&&... args)
{
    (void)priority; // Reserved for future use
    auto promise = std::make_shared<std::promise<void>>();
    auto future = promise->get_future();

    // Wrap the function call in a lambda that captures arguments by value
    auto task = [promise, func = std::forward<Function>(func), args...]() mutable {
        try {
            func(args...);
            promise->set_value();
        }
        catch (...) {
            promise->set_exception(std::current_exception());
        }
    };

    // Post the task to the thread pool
    boost::asio::post(*m_pool, std::move(task));

    return future;
}

#endif // THREAD_POOL_GAME_H
