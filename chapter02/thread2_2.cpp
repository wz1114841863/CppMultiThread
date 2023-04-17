#include <iostream>
#include <thread>
#include <memory>

void func(int i, const std::string &s) {
    std::cout << "child thread s: " << s << std::endl;
}

void func1(int &num) {
    // num = 11;
    std::cout << "child thread num: " << num << std::endl;
}

class X {
public:
    void do_something(int val) {
        std::cout << "do something. val = " << val << std::endl;
    }
};

void test_move(std::unique_ptr<std::string> s) {
    std::cout << "child thread, s: " << *s << std::endl;
}


void thread_2_2() {
    char buffer[1024] = "abcdefghijklmn";
    std::cout << "main thread: " << buffer << std::endl;
    std::thread t1(func, 3, buffer);  // 可能会出现错误，主线程结束，但转换还未执行
    t1.detach();
}

void thread_2_2_1() {
    char buffer[1024] = "abcdefghijklmn";
    std::cout << "main thread: " << buffer << std::endl;
    std::thread t1(func, 3, std::string(buffer));
    t1.detach();
}

void thread_2_2_2() {
    int num = 10;
    std::cout << "main thread: " << num << std::endl;
    std::thread t2(func1, std::ref(num));  // 需要添加std::ref, 否则编译报错
    t2.join();
}

void thread_2_2_3() {
    X tmp_x;
    int num = 10;
    std::thread t3(&X::do_something, &tmp_x, num);  // 类似于std::bind
    t3.join();
}

void thread_2_2_4() {
    std::unique_ptr<std::string> uni(new std::string("abcde"));
    std::cout << "main thread uni: " << *uni << std::endl;
    std::thread t4(test_move, std::move(uni));
    std::cout << "uni is nullptr? " << (uni == nullptr) << std::endl;
    t4.join();
}



