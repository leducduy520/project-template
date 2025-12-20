#include "threadpool.hpp"
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

size_t ThreadPool::getThreadCount() const noexcept
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

void ThreadPool::set_thread_priority(ThreadPriority priority) noexcept
{
#ifdef _WIN32
    // Windows: Map enum to Windows thread priority levels
    int win_priority;
    switch (priority) {
    case ThreadPriority::Lowest:
        win_priority = THREAD_PRIORITY_LOWEST;
        break;
    case ThreadPriority::Low:
        win_priority = THREAD_PRIORITY_BELOW_NORMAL;
        break;
    case ThreadPriority::Normal:
        win_priority = THREAD_PRIORITY_NORMAL;
        break;
    case ThreadPriority::High:
        win_priority = THREAD_PRIORITY_ABOVE_NORMAL;
        break;
    case ThreadPriority::Highest:
        win_priority = THREAD_PRIORITY_HIGHEST;
        break;
    default:
        win_priority = THREAD_PRIORITY_NORMAL;
        break;
    }
    SetThreadPriority(GetCurrentThread(), win_priority);
#else
    // Linux: Map enum to nice values and scheduling policies
    struct sched_param param;
    int policy = SCHED_OTHER; // Default time-sharing policy
    
    switch (priority) {
    case ThreadPriority::Lowest:
        // Use nice value +19 (lowest priority)
        setpriority(PRIO_PROCESS, 0, 19);
        return; // Early return, no need to set scheduling policy
    case ThreadPriority::Low:
        // Use nice value +10
        setpriority(PRIO_PROCESS, 0, 10);
        return;
    case ThreadPriority::Normal:
        // Use nice value 0 (default)
        setpriority(PRIO_PROCESS, 0, 0);
        return;
    case ThreadPriority::High:
        // Try real-time scheduling with medium priority
        policy = SCHED_RR;
        param.sched_priority = (sched_get_priority_max(SCHED_RR) + sched_get_priority_min(SCHED_RR)) / 2;
        if (pthread_setschedparam(pthread_self(), policy, &param) != 0) {
            // Fallback to nice value -5 if real-time fails
            setpriority(PRIO_PROCESS, 0, -5);
        }
        return;
    case ThreadPriority::Highest:
        // Try real-time scheduling with highest priority
        policy = SCHED_FIFO;
        param.sched_priority = sched_get_priority_max(SCHED_FIFO);
        if (pthread_setschedparam(pthread_self(), policy, &param) != 0) {
            // Fallback to SCHED_RR if FIFO fails
            policy = SCHED_RR;
            param.sched_priority = sched_get_priority_max(SCHED_RR);
            if (pthread_setschedparam(pthread_self(), policy, &param) != 0) {
                // Final fallback to nice value -10
                setpriority(PRIO_PROCESS, 0, -10);
            }
        }
        return;
    default:
        setpriority(PRIO_PROCESS, 0, 0);
        return;
    }
#endif
}
