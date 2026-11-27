#include <iostream>
#include <string>
#include <thread>

int main()
{
    std::thread worker([](const std::string& message) {
        std::cout << "worker: " << message << '\n';
    }, "hello");

    worker.join();
    std::cout << "worker joined\n";
}
