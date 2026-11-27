#include <iostream>
#include <mutex>
#include <shared_mutex>
#include <thread>
#include <vector>

class SharedValue {
public:
    int read() const
    {
        std::shared_lock lock(mutex_);
        return value_;
    }

    void increment()
    {
        std::unique_lock lock(mutex_);
        ++value_;
    }

private:
    mutable std::shared_mutex mutex_;
    int value_ = 0;
};

int main()
{
    SharedValue value;
    std::vector<std::thread> readers;
    for (int i = 0; i < 3; ++i) {
        readers.emplace_back([&value, i] {
            for (int n = 0; n < 3; ++n) {
                std::cout << "reader " << i << ": " << value.read() << '\n';
            }
        });
    }

    std::thread writer([&value] {
        for (int n = 0; n < 5; ++n) {
            value.increment();
        }
    });

    for (auto& reader : readers) {
        reader.join();
    }
    writer.join();
    std::cout << "final value: " << value.read() << '\n';
}
