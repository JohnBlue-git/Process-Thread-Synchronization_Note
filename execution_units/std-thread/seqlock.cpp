#include <atomic>
#include <cstdint>
#include <iostream>
#include <mutex>
#include <thread>

struct Snapshot {
    std::uint32_t version;
    std::uint32_t value;
};

class SequenceSnapshot {
public:
    void write(std::uint32_t value)
    {
        const std::lock_guard writer_lock(writer_mutex_);
        sequence_.fetch_add(1, std::memory_order_seq_cst);
        const auto version = static_cast<std::uint32_t>(value * 2U);
        const auto packed = (static_cast<std::uint64_t>(version) << 32U) | value;
        payload_.store(packed, std::memory_order_seq_cst);
        sequence_.fetch_add(1, std::memory_order_seq_cst);
    }

    Snapshot read() const
    {
        for (;;) {
            const auto before = sequence_.load(std::memory_order_seq_cst);
            if ((before & 1U) != 0U) {
                std::this_thread::yield();
                continue;
            }
            const auto packed = payload_.load(std::memory_order_seq_cst);
            const Snapshot copy{
                static_cast<std::uint32_t>(packed >> 32U),
                static_cast<std::uint32_t>(packed),
            };
            const auto after = sequence_.load(std::memory_order_seq_cst);
            if (before == after) {
                return copy;
            }
        }
    }

private:
    std::mutex writer_mutex_;
    std::atomic<std::uint64_t> sequence_{0};
    std::atomic<std::uint64_t> payload_{0};
};

int main()
{
    SequenceSnapshot snapshot;
    std::atomic<bool> consistent{true};
    std::thread writer([&] {
        for (std::uint32_t value = 1; value <= 10000; ++value) {
            snapshot.write(value);
        }
    });
    std::thread reader([&snapshot, &consistent] {
        for (int i = 0; i < 1000; ++i) {
            const auto copy = snapshot.read();
            if (copy.version != copy.value * 2U) {
                std::cerr << "inconsistent snapshot\n";
                consistent.store(false, std::memory_order_relaxed);
            }
        }
    });

    writer.join();
    reader.join();
    if (!consistent.load(std::memory_order_relaxed)) {
        return 1;
    }
    const auto copy = snapshot.read();
    std::cout << "version=" << copy.version << ", value=" << copy.value << '\n';
}
