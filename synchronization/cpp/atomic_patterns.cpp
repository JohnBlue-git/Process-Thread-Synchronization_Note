#include <array>
#include <atomic>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <thread>

class SpscQueue {
public:
    static constexpr std::size_t capacity = 64;

    bool try_push(int value)
    {
        const auto write = write_index_.load(std::memory_order_relaxed);
        const auto read = read_index_.load(std::memory_order_acquire);
        if (write - read == capacity) {
            return false;
        }

        items_[write % capacity] = value;
        write_index_.store(write + 1, std::memory_order_release);
        return true;
    }

    bool try_pop(int& value)
    {
        const auto read = read_index_.load(std::memory_order_relaxed);
        const auto write = write_index_.load(std::memory_order_acquire);
        if (read == write) {
            return false;
        }

        value = items_[read % capacity];
        read_index_.store(read + 1, std::memory_order_release);
        return true;
    }

private:
    std::array<int, capacity> items_{};
    std::atomic<std::size_t> write_index_{0};
    std::atomic<std::size_t> read_index_{0};
};

int main()
{
    constexpr int item_count = 100000;
    SpscQueue queue;
    std::thread producer([&queue] {
        for (int item = 1; item <= item_count; ++item) {
            while (!queue.try_push(item)) {
                std::this_thread::yield();
            }
        }
    });

    std::uint64_t sum = 0;
    std::thread consumer([&queue, &sum] {
        for (int count = 0; count < item_count; ++count) {
            int item;
            while (!queue.try_pop(item)) {
                std::this_thread::yield();
            }
            sum += static_cast<std::uint64_t>(item);
        }
    });

    producer.join();
    consumer.join();
    constexpr std::uint64_t expected_sum =
        static_cast<std::uint64_t>(item_count) * (item_count + 1U) / 2U;
    std::cout << "SPSC sum=" << sum << " (expected " << expected_sum << ")\n";
    return sum == expected_sum ? 0 : 1;
}
