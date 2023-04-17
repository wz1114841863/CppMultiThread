#include <iostream>
#include <thread>

void hello() {
    std::cout << "Hello concurrent world. " << std::endl;
}

int thread_1_4() {
    std::thread t1(hello);  // 创建一个新线程时需要传递一个可执行对象（函数或类）
    t1.join();              // 阻塞主线程，等待线程结束
    return 0;
}