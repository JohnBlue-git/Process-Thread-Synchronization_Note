#include <iostream>
#include <mutex>
#include <thread>
#include <vector>

int main()
{
    constexpr int thread_count = 4;
    constexpr int increments = 10000;
    std::mutex mutex;
    int counter = 0;
    std::vector<std::thread> threads;

    for (int i = 0; i < thread_count; ++i) {
        threads.emplace_back([&] {
            for (int n = 0; n < increments; ++n) {
                const std::lock_guard lock(mutex);
                ++counter;
            }
        });
    }
    for (auto& thread : threads) {
        thread.join();
    }
    std::cout << "counter=" << counter << '\n';
}
