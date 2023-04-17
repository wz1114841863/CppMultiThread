#include <iostream>
#include <thread>
#include <windows.h>

struct func{
    int &_i;
    func(int &i):_i(i) {}
    void operator()() {
        for (unsigned j=0 ; j<10000 ; ++j) {
            std::cout << "j: " << j << " i: " << _i << std::endl;  // error!
        }
    }
};

class thread_guard {
    std::thread &t1;
public:
    thread_guard(std::thread &t1):t1(t1) {}
    ~thread_guard() {
        if (t1.joinable()) {
            t1.join();
        }
    }

    thread_guard(const std::thread &)=delete;
    thread_guard& operator=(const std::thread &) = delete;
};

void thread2_1() {
    int i = 10;
    std::thread t1{func(i)};  // 这里要么使用{}要么多加一个括号，否则会解释为函数声明
    t1.detach();  // 线程分离，目标线程变为守护线程
    // delay
    Sleep(100);
}

void thread2_1_3() {
    int i = 10;
    std::thread t2{func(i)};
    try {
        std::cout << "i: " << i << std::endl;
    }catch (...) {
        t2.join();  // 防止因为异常倒是线程未正常结束
        throw;
    }
    t2.join();
}

void do_something() {
    std::cout << " " << std::endl;
}

void thread2_1_4() {
    int i = 10;
    std::thread t3{func(i)};
    thread_guard g(t3);
    do_something();
}

