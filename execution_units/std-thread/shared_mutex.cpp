#include <iostream>
#include <mutex>
#include <shared_mutex>
#include <thread>
#include <vector>

int main()
{
    std::shared_mutex mutex;
    int shared_value = 0;
    std::vector<std::thread> readers;
    for (int i = 0; i < 3; ++i) {
        readers.emplace_back([&, i] {
            for (int n = 0; n < 3; ++n) {
                std::shared_lock lock(mutex);
                std::cout << "reader " << i << " observed " << shared_value << '\n';
            }
        });
    }

    std::thread writer([&] {
        for (int n = 0; n < 5; ++n) {
            std::unique_lock lock(mutex);
            ++shared_value;
        }
    });
    for (auto& reader : readers) {
        reader.join();
    }
    writer.join();
    std::cout << "final value=" << shared_value << '\n';
}
