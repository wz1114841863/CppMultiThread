#include <list>
#include <mutex>
#include <algorithm>
#include <iostream>
#include <deque>
#include <exception>
#include <memory>
#include <stack>

std::list<int> some_list;  // 全局变量
std::mutex some_mutex;     // 互斥量

// 3.2.1
void add_to_list(int new_value) {
    std::lock_guard<std::mutex> guard(some_mutex);  // RAII
    some_list.push_back(new_value);
}

bool list_contains(int value_to_find) {
    std::lock_guard<std::mutex> guard(some_mutex);
    // C++17中，模板类参数推导，std::lock_guard guard(some_mutex);
    return std::find(some_list.begin(), some_list.end(), value_to_find) != some_list.end();
}

class some_data {
    int a;
    std::string b;
public:
    void do_something() {
        std::cout << "Class some_data do_some_thing." << std::endl;
    }
};

// 3.2.2
class data_wrapper {
private:
    some_data data;  // 设置为类的私有变量
    std::mutex m;
public:
    template<typename Function>
    void porcess_data(Function func) {
        std::lock_guard<std::mutex> l(m);
        func(data);  // 传递“保护”数据给用户函数
    }
};

some_data *unprotected;

void malicious_function(some_data &protected_data) {
    unprotected = &protected_data;
}

data_wrapper x;
void foo() {
    x.porcess_data(malicious_function);  // 传递一个恶意函数
    unprotected->do_something();  // 在无保护的情况下访问保护数据
}

// 3.2.3
// 非线程安全的栈
template <typename T, typename Container=std::deque<T>>
class stack {
public:
    explicit stack(const Container&);
    explicit stack(Container && = Container());

    template<typename Alloc>
    explicit stack(const Alloc&);

    template<typename Alloc>
    explicit stack(const Container&, const Alloc&);

    template<typename Alloc>
    explicit stack(stack &&, const Alloc&);

    bool empty() const;
    size_t size() const;
    T &top();
    T const &top() const;
    void push(const T &);
    void push(T &&);
    void pop();
    void swap(stack &&);
    template <typename ... Args>
    void emplace(Args&&... args);
};

// 定义线程安全的堆栈
struct empty_stack: std::exception {
    const char *what() const throw() {
        return "empty stack!";
    };
};

template<typename T>
class threadsafe_stack {
    std::stack<T> data;
    mutable std::mutex m;

public:
    threadsafe_stack()
    :data(std::stack<T>()) {}

    threadsafe_stack(const threadsafe_stack &other) {
        std::lock_guard<std::mutex> lock(other.m);
        data = other.data;  // 在构造函数体中的执行拷贝
    }

    threadsafe_stack &operator=(const threadsafe_stack &) = delete;

    void push(T new_value) {
        std::lock_guard<std::mutex> lock(m);
        data.push(new_value);
    }

    std::shared_ptr<T> pop() {
        std::lock_guard<std::mutex> lock(m);
        if (data.empty()) throw empty_stack();  // 在调用pop之前检查栈是否为空
        // 在修改堆栈前，分配出返回值
        std::shared_ptr<T> const res(std::make_shared<T>(data.top()));
        data.pop();
        return res;
    }
    void pop(T &value) {
        std::lock_guard<std::mutex> lock(m);
        if (data.empty()) throw empty_stack();  // 在调用pop之前检查栈是否为空
        value = data.top();
        data.pop();
    }

    bool empty() const {
        std::lock_guard<std::mutex> lock(m);
        return data.empty();
    }
};

// 3.2.4
class some_big_object {

};

void swap(some_big_object &lhs, some_big_object &rhs);

class X {
    some_big_object some_detail;
    std::mutex m;
public:
    X(const some_big_object &sd): some_detail(sd) {}

    friend void swap(X &lhs, X &rhs) {
        if (&lhs == &rhs) {  // 地址判断
            return ;
        }

        std::lock(lhs.m, rhs.m);  // 同时锁住两个变量
        std::lock_guard<std::mutex> lock_a(lhs.m, std::adopt_lock);
        std::lock_guard<std::mutex> lock_b(rhs.m, std::adopt_lock);

        // C++17
        // std::scoped_lock guard(lhs.m, rhs.m);
        swap(lhs.some_detail, rhs.some_detail);
    }
};

// 3.2.5 层次锁
class hierarchical_mutex {
public:
    hierarchical_mutex(int ) {

    }
};
hierarchical_mutex high_level_mutex(10000);
hierarchical_mutex low_level_mutex(50000);
hierarchical_mutex other_level_mutex(60000);

int do_low_level_stuff() {
    std::cout << "do_low_level_stuff. " << std::endl;
    return 0;
}

int low_level_func() {
    std::lock_guard<hierarchical_mutex> lk(low_level_mutex); // 4
    return do_low_level_stuff();
}

void high_level_stuff(int some_param);

void high_level_func()
{
    std::lock_guard<hierarchical_mutex> lk(high_level_mutex); // 6
    high_level_stuff(low_level_func()); // 5
}
void thread_a() // 7
{
    high_level_func();
}
void do_other_stuff();
void other_stuff()
{
    high_level_func(); // 10
    do_other_stuff();
}
void thread_b() // 8
{
    std::lock_guard<hierarchical_mutex> lk(other_mutex); // 9
    other_stuff();
}
