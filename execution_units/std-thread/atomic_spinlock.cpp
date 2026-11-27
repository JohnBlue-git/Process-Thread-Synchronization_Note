#include <atomic>
#include <iostream>
#include <mutex>
#include <thread>
#include <vector>

class SpinLock {
public:
    void lock() noexcept
    {
        while (flag_.test_and_set(std::memory_order_acquire)) {
            flag_.wait(true, std::memory_order_relaxed);
        }
    }

    void unlock() noexcept
    {
        flag_.clear(std::memory_order_release);
        flag_.notify_one();
    }

private:
    std::atomic_flag flag_ = ATOMIC_FLAG_INIT;
};

int main()
{
    constexpr int thread_count = 4;
    constexpr int increments = 10000;
    SpinLock lock;
    int counter = 0;
    std::vector<std::thread> threads;

    for (int i = 0; i < thread_count; ++i) {
        threads.emplace_back([&] {
            for (int n = 0; n < increments; ++n) {
                const std::lock_guard guard(lock);
                ++counter;
            }
        });
    }
    for (auto& thread : threads) {
        thread.join();
    }
    std::cout << "counter=" << counter << '\n';
}
