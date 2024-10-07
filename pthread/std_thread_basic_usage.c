#include <iostream>
#include <thread>

void threadFunction() {
  return;
}

int main() {
    
    std::thread th(&threadFunction);
    // or
    //std::thread(&threadFunction).join();
    // or
    //std::thread(&threadFunction).detach();

    //th.detach();
    // or
    th.join();

    // cannot call pthread_cancel() !!!

    return 0;
}
