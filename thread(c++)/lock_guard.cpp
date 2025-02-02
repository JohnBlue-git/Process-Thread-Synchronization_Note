#include <iostream>
#include <thread>
#include <vector>
#include <mutex>
#include <atomic>

// Spinlock class definition
class Spinlock {
    std::atomic_flag flag = ATOMIC_FLAG_INIT;
public:
    void lock() {
        while (flag.test_and_set(std::memory_order_acquire)) {
            // Spin-wait (do nothing)
        }
    }
    void unlock() {
        flag.clear(std::memory_order_release);
    }
};
Spinlock spinlock;


int counter = 0;
void incrementCounter(int id) {
    for (int i = 0; i < 100; ++i) {
        // normal
        //std::lock_guard<std::mutex> guard(mtx);
        // more finner
        std::lock_guard<Spinlock> guard(spinlock);
        ++counter;
        std::cout << "Thread " << id << " incremented counter to " << counter << std::endl;
    }
}

int main() {
    const int numThreads = 5;
    std::vector<std::thread> threads;

    // Create and start multiple threads
    for (int i = 0; i < numThreads; ++i) {
        threads.emplace_back(incrementCounter, i);
    }

    // Join all threads
    for (auto& th : threads) {
        th.join();
    }

    std::cout << "Final counter value: " << counter << std::endl;

    return 0;
}
