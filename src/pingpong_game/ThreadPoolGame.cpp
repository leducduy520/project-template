#include "ThreadPoolGame.hpp"

ThreadPool* ThreadPool::pool;
std::once_flag ThreadPool::creat_flag;

ThreadPool::ThreadPool(size_t threads) : stop_flag(false), idle_threads(threads)
{
    for (size_t i = 0; i < threads; ++i)
    {
        workers.emplace_back(&ThreadPool::worker, this);
    }
}

ThreadPool::~ThreadPool()
{
    shutdown();
}

void ThreadPool::shutdown()
{
    {
        std::unique_lock<std::mutex> lock(queue_mutex);
        stop_flag.store(true);
    }
    condition.notify_all();
    for (std::thread& worker : workers)
    {
        worker.join();
    }
}

void ThreadPool::worker()
{
    while (true)
    {
        Task task([] {}, 0); // Initialize with an empty task

        {
            std::unique_lock<std::mutex> lock(queue_mutex);
            condition.wait(lock, [this] { return stop_flag || !tasks.empty(); });

            if (stop_flag && tasks.empty())
                return;

            task = std::move(tasks.top());
            tasks.pop();
        }

        idle_threads--;
        try
        {
            task.func();
        }
        catch (const std::exception& e)
        {
            std::cerr << "Exception in thread pool task: " << e.what() << std::endl;
        }
        idle_threads++;
    }
}
