#include <iostream>
#include <semaphore>
#include <thread>
#include <vector>

int main()
{
    std::counting_semaphore<2> permits(2);
    std::vector<std::thread> workers;
    for (int i = 0; i < 6; ++i) {
        workers.emplace_back([&, i] {
            permits.acquire();
            std::cout << "worker " << i << " acquired a permit\n";
            permits.release();
        });
    }
    for (auto& worker : workers) {
        worker.join();
    }
}
