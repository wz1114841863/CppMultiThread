#include <iostream>
#include <thread>

void print_thread_id() {
    std::cout << "id: " << std::this_thread::get_id() << std::endl;
}

void thread2_5() {
    std::thread::id master_thread = std::this_thread::get_id();
    std::cout << "main thread id: " << master_thread << std::endl;
    std::thread t1(print_thread_id);
    std::thread t2(print_thread_id);
    std::thread t3(print_thread_id);

    t1.join();
    t2.join();
    t3.join();
}