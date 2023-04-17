#include <iostream>
#include <vector>
#include <thread>

void do_some_thing() {
    std::cout << "do some thing. " << std::endl;
}

void do_some_thing(int id) {
    std::cout << "do some thing. id: " << id << std::endl;
}

void do_other_thing() {
    std::cout << "do other thing. " << std::endl;
}

void thread2_3() {
    std::thread t1(static_cast<void(*)()>(do_some_thing));
    std::thread t2(std::move(t1));
    t1 = std::thread(do_other_thing);
    std::thread t3;
    t3 = std::move(t2);
    // t1 = std::move(t3);  // 会调用std::terminate()，不能通过赋新值的方式来丢弃一个线程对象

    t1.join();
    t3.join();
}

std::thread fun1() {
    std::thread t1(static_cast<void(*)()>(do_some_thing));
    return t1;
}

void fun2(std::thread t) {
    std::cout << "get a thread." << std::endl;
    t.join();  // 这里需要设计者记得去合并或分离线程
}

void thread2_3_1() {
    auto t1 = fun1();
    fun2(std::move(t1));
}

class scoped_thread {
    std::thread t;
public:
    explicit scoped_thread(std::thread t_):t(std::move(t_)) {
        if (!t.joinable()) {
            throw std::logic_error("No thread");
        }
    }

    ~scoped_thread() {
        t.join();
    }

    scoped_thread(const scoped_thread &)=delete;
    scoped_thread &operator=(const scoped_thread &)=delete;
};

void thread2_3_2() {
    // 使用类对象来管理线程的合并/分离
    scoped_thread t1((std::thread(static_cast<void(*)()>(do_some_thing))));
    std::cout << "main thread." << std::endl;
}

// 与scope_thread类似
class joining_thread {
    std::thread t;
public:
    joining_thread() noexcept=default;

    template<typename Callable, typename ... Args>
    explicit joining_thread(Callable &&func, Args && ... args)
    :t(std::forward<Callable>(func), std::forward<Args>(args)...) {}

    explicit joining_thread(std::thread t_) noexcept
    :t(std::move(t_)) {}

    joining_thread(joining_thread &&other) noexcept
    :t(std::move(other.t)) {}

    joining_thread &operator=(joining_thread &&other) noexcept {
        if (joinable()) {
            join();
        }
        t = std::move(other.t);
        return *this;
    }

    joining_thread &operator=(std::thread other) noexcept {
        if (joinable()) {
            join();
        }
        t = std::move(other);
        return *this;
    }

    ~joining_thread() noexcept {
        if (joinable()) {
            join();
        }
    }

    void swap(joining_thread &other) noexcept {
        t.swap(other.t);
    }

    std::thread::id get_id() const noexcept {
        return t.get_id();
    }

    bool joinable() const noexcept {
        return t.joinable();
    }

    void join() {
        t.join();
    }

    void detach() {
        t.detach();
    }

    std::thread &as_thread() noexcept {
        return t;
    }

    const std::thread &as_thread() const noexcept {
        return t;
    }
};

// 量产线程
void thread2_3_3() {
    std::vector<std::thread> threads;
    for (unsigned int i = 0; i < 20; ++i) {
        // 注意这里为了让线程引用重载的函数
        threads.emplace_back(static_cast<void(*)(int)>(do_some_thing), i);
    }

    for (auto &entry : threads) {
        entry.join();
    }
}