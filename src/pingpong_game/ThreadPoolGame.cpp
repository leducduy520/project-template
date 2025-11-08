#include "ThreadPoolGame.hpp"
#include <thread>

ThreadPool::ThreadPool(size_t num_threads)
    : m_thread_count(num_threads == 0 ? (std::thread::hardware_concurrency() * 2) / 5 : num_threads)
{
    if (m_thread_count == 0) {
        m_thread_count = 1; // At least one thread
    }
    m_pool = std::make_unique<boost::asio::thread_pool>(m_thread_count);
}

ThreadPool::~ThreadPool()
{
    // Only join if we haven't already joined via wait()
    // This prevents hanging during static destruction if wait() was already called
    if (m_pool && !m_joined.load()) {
        m_pool->join();
        m_joined.store(true);
    }
    // Reset will call thread_pool destructor, but if we've already joined,
    // the destructor won't block
    m_pool.reset();
}

ThreadPool& ThreadPool::getInstance(size_t num_threads)
{
    // Modern C++11+ thread-safe singleton using static local variable
    // The C++11 standard guarantees thread-safe initialization of static locals
    static ThreadPool instance(num_threads);
    return instance;
}

size_t ThreadPool::getThreadCount() const
{
    return m_thread_count;
}

void ThreadPool::wait()
{
    if (m_pool && !m_joined.load()) {
        m_pool->join();
        m_joined.store(true);
    }
}

