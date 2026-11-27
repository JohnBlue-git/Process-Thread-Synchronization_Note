#include <condition_variable>
#include <cstddef>
#include <iostream>
#include <mutex>
#include <optional>
#include <queue>
#include <thread>

class BoundedQueue {
public:
    explicit BoundedQueue(std::size_t capacity) : capacity_(capacity) {}

    void push(int value)
    {
        std::unique_lock lock(mutex_);
        not_full_.wait(lock, [this] { return closed_ || queue_.size() < capacity_; });
        if (closed_) {
            return;
        }
        queue_.push(value);
        not_empty_.notify_one();
    }

    std::optional<int> pop()
    {
        std::unique_lock lock(mutex_);
        not_empty_.wait(lock, [this] { return closed_ || !queue_.empty(); });
        if (queue_.empty()) {
            return std::nullopt;
        }
        const int value = queue_.front();
        queue_.pop();
        not_full_.notify_one();
        return value;
    }

    void close()
    {
        {
            std::lock_guard lock(mutex_);
            closed_ = true;
        }
        not_empty_.notify_all();
        not_full_.notify_all();
    }

private:
    const std::size_t capacity_;
    std::queue<int> queue_;
    bool closed_ = false;
    std::mutex mutex_;
    std::condition_variable not_empty_;
    std::condition_variable not_full_;
};

int main()
{
    BoundedQueue queue(4);
    std::thread producer([&queue] {
        for (int item = 1; item <= 10; ++item) {
            queue.push(item);
        }
        queue.close();
    });

    std::thread consumer([&queue] {
        while (const auto item = queue.pop()) {
            std::cout << "Consumed " << *item << '\n';
        }
    });

    producer.join();
    consumer.join();
}
