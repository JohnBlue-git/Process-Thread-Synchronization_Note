#include <condition_variable>
#include <iostream>
#include <mutex>
#include <optional>
#include <queue>
#include <thread>

int main()
{
    constexpr std::size_t capacity = 3;
    std::queue<int> queue;
    std::mutex mutex;
    std::condition_variable changed;
    bool finished = false;

    std::thread consumer([&] {
        for (;;) {
            std::unique_lock lock(mutex);
            changed.wait(lock, [&] { return finished || !queue.empty(); });
            if (queue.empty() && finished) {
                break;
            }
            const int item = queue.front();
            queue.pop();
            std::cout << "consumed " << item << '\n';
            lock.unlock();
            changed.notify_one();
        }
    });

    std::thread producer([&] {
        for (int item = 1; item <= 8; ++item) {
            std::unique_lock lock(mutex);
            changed.wait(lock, [&] { return queue.size() < capacity; });
            queue.push(item);
            std::cout << "produced " << item << '\n';
            lock.unlock();
            changed.notify_one();
        }
        {
            const std::lock_guard lock(mutex);
            finished = true;
        }
        changed.notify_all();
    });

    producer.join();
    consumer.join();
}
