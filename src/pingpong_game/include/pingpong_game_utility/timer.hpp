#ifndef PINGPONG_GAME_UTILITY_TIMER_HPP
#define PINGPONG_GAME_UTILITY_TIMER_HPP

#include <SFML/System/Sleep.hpp>
#include <spdlog/spdlog.h>
#include <future>
#include <chrono>
#include <functional>
#include <cstdint>
#include <type_traits>
#include <utility>
#include <atomic>
#include <mutex>
#include <condition_variable>
#include "threadpool.hpp"

template <typename Period_ = std::ratio<1, 1>>
class Timer
{
public:
    using sclock = std::chrono::steady_clock;

private:
    void sleep() noexcept
    {
        std::this_thread::sleep_for(interval2_);
    }

    void condition_lock()
    {
        std::unique_lock<std::mutex> lock(mt_);
        cv_.wait(lock, [this]() { return stopped_.load() || running_.load(); });
    }

    auto getElapsedTime() noexcept
    {
        return sclock::now() - start_time_;
    }

    void update()
    {
        if (getElapsedTime() < interval2_) {
            return;
        }
        callback_();
        if (repeat_ > 0) {
            --repeat_;
        }
    }

    void threadFunction()
    {
        while (repeat_ != 0) {
            update();
            if (repeat_ == 0) {
                stopped_.store(true);
                running_.store(false);
                return;
            }
            if (running_) {
                sleep();
            }
            if (!running_ && !stopped_) {
                condition_lock();
            }
        }
    }

public:
    // Helper trait to detect Timer specializations
    template <typename T>
    struct is_timer : std::false_type
    {};

    template <typename P>
    struct is_timer<Timer<P>> : std::true_type
    {};

    // Constructor for free functions and callable objects
    template <
        typename Function,
        std::enable_if_t<std::is_invocable_r_v<void, Function> && !is_timer<std::decay_t<Function>>::value, int> = 0>
    Timer(Function&& callback)
        : callback_(std::forward<Function>(callback)), start_time_(sclock::now()), pause_time_(sclock::now())
    {}

    // Constructor for member functions
    template <typename Class, typename MemberFunction>
    Timer(Class* obj, MemberFunction mem_func)
        : callback_([obj, mem_func]() { (obj->*mem_func)(); }), start_time_(sclock::now()), pause_time_(sclock::now())
    {}

    ~Timer()
    {
        try {
            stop();
        }
        catch (...) {
            // Suppress exceptions in destructor to maintain noexcept guarantee
            (void)0;
        }
    }

    template <typename Period2_ = std::ratio<1, 1>>
    void configure(std::chrono::duration<long long, Period2_> interval, uint64_t repeat = 0)
    {
        if (interval.count() <= 0) {
            throw std::invalid_argument("Interval must be greater than zero.");
        }
        interval2_ = std::chrono::duration_cast<std::chrono::duration<long long, Period_>>(interval);
        repeat_ = repeat;
    }

    void start()
    {
        std::unique_lock<std::mutex> lock(mt_);
        if (running_)
            return;
        stopped_.store(false);
        running_.store(true);
        start_time_ = sclock::now();
        lock.unlock();
        thread_future_ = ThreadPool::getInstance().submit(ThreadPriority::Highest, [this]() { threadFunction(); });
    }

    void stop()
    {
        stopped_.store(true);
        running_.store(false);
        repeat_ = 0;
        cv_.notify_all();
        if (thread_future_.valid()) {
            try {
                thread_future_.wait();
            }
            catch (...) {
                (void)0;
            }
        }
    }

    void pause() noexcept
    {
        std::unique_lock<std::mutex> lock(mt_);
        if (!running_)
            return;

        pause_time_ = sclock::now();
        running_.store(false);
    }

    void stop_pause() noexcept
    {
        if (running_)
            return;

        start_time_ += (sclock::now() - pause_time_);
        running_.store(true);
        cv_.notify_all();
    }

    template <typename Period2_ = std::ratio<1, 1>>
    auto leftTime() noexcept
    {
        if (!running_ || repeat_ == 0) {
            return std::chrono::duration_cast<std::chrono::duration<long long, Period2_>>(std::chrono::seconds(0));
        }
        // Calculate remaining time: repeat_ is the number of remaining callbacks
        // Each callback happens every interval2_ seconds
        // The timer was configured with an initial repeat count, and we need to calculate
        // the remaining time based on elapsed time and remaining callbacks
        auto elapsed = getElapsedTime();
        auto intervalDuration = std::chrono::duration_cast<sclock::duration>(interval2_);

        // Calculate how many complete intervals have elapsed (this approximates callbacks executed)
        auto elapsedIntervals = elapsed / intervalDuration;

        // The initial repeat count was approximately: current repeat_ + elapsedIntervals
        // But we need to be more precise. The remaining time should be:
        // (repeat_ callbacks remaining) * interval2_ + time left in current interval
        // However, we need to account for the fact that callbacks are called at interval boundaries

        // Calculate time elapsed in the current (incomplete) interval
        auto elapsedInCurrentInterval = elapsed - (elapsedIntervals * intervalDuration);

        // Time remaining in current interval (until next callback fires)
        auto remainingInCurrentInterval = intervalDuration - elapsedInCurrentInterval;

        // After the next callback fires, there will be (repeat_ - 1) callbacks remaining
        // Each callback takes interval2_ time
        // So total remaining time = time until next callback + time for remaining callbacks after that
        auto remainingAfterNextCallback = (repeat_ > 1)
                                              ? std::chrono::duration_cast<sclock::duration>(interval2_ * (repeat_ - 1))
                                              : sclock::duration::zero();

        // Total remaining time
        auto remaining = remainingInCurrentInterval + remainingAfterNextCallback;

        // Ensure we don't return negative time
        if (remaining.count() < 0) {
            return std::chrono::duration_cast<std::chrono::duration<long long, Period2_>>(std::chrono::seconds(0));
        }
        return std::chrono::duration_cast<std::chrono::duration<long long, Period2_>>(remaining);
    }

    bool running() noexcept
    {
        return stopped_.load() == false;
    }

private:

    std::function<void()> callback_;
    std::chrono::duration<long long, Period_> interval2_;
    uint64_t repeat_ = 0;
    std::atomic_bool running_{false};
    std::atomic_bool stopped_{false};
    std::future<void> thread_future_;
    sclock::time_point start_time_;
    sclock::time_point pause_time_;
    std::mutex mt_;
    std::condition_variable cv_;
};
#endif //PINGPONG_GAME_UTILITY_TIMER_HPP
