#include <algorithm>
#include <iostream>
#include <list>
#include <mutex>
#include <thread>

using namespace std;

// 保护共享数据结构的最基本的方式，是使用C++标准库提供的互斥量。

// 当访问共享数据前，将数据锁住，在访问结束后，再将数据解锁。线程库需要保证，当一个线程使用特定互斥量锁住共享数据时，其他的线程想要访问锁住的数据，
// 都必须等到之前那个线程对数据进行解锁后，才能进行访问。这就保证了所有线程都能看到共享数据，并而不破坏不变量。
// 互斥量一种数据保护通用机制，但它不是什么“银弹”；需要编排代码来保护数据的正确性，并避免接口间的竞争条件也非常重要。不过，互斥量自身也有问题，也会造成死锁，
// 或对数据保护的太多(或太少)。

// C++中使用互斥量:
// C++中通过实例化std::mutex创建互斥量实例，通过成员函数lock()对互斥量上锁，unlock()进行解锁。不过，实践中不推荐直接去调用成员函数，调用成员函数就意味着，
// 必须在每个函数出口都要去调用unlock()，也包括异常的情况。C++标准库为互斥量提供了一个RAII语法的模板类std::lock_guard，在构造时就能提供已锁的互斥量，并在
// 析构的时候进行解锁，从而保证了一个已锁互斥量能被正确解锁。
std::list<int> some_list;
std::mutex some_mutex;
void add_to_list(int new_value) {
  std::lock_guard<std::mutex> guard(some_mutex);
  some_list.push_back(new_value);
}
bool list_contains(int value_to_find) {
  std::lock_guard<std::mutex> guard(some_mutex);
  return std::find(some_list.begin(), some_list.end(), value_to_find) !=
         some_list.end();
}
// 某些情况下使用全局变量没问题，但在大多数情况下，互斥量通常会与需要保护的数据放在同一类中，而不是定义成全局变量。这是面向对象设计的准则：将其放在一个类中，
// 就可让他们联系在一起，也可对类的功能进行封装，并进行数据保护。这种情况下，函数add_to_list和list_contains可以作为这个类的成员函数。互斥量和需要保护的数
// 据，在类中都定义为private成员，这会让访问数据的代码更清晰，并且容易看出在什么时候对互斥量上锁。当所有成员函数都会在调用时对数据上锁，结束时对数据解锁，这
// 就保证了访问时数据不变量不被破坏

// 当其中一个成员函数返回的是保护数据的指针或引用时，会破坏数据。具有访问能力的指针或引用可以访问(并可能修改)被保护的数据，而不会被互斥锁限制。这就需要对接口有
// 相当谨慎的设计，要确保互斥量能锁住数据的访问，并且不留后门。
class some_data {
  int a;
  std::string b;

 public:
  void do_something() {
    a = 5;
    b = "llll";
  }
  void display() { cout << a << " " << b << endl; }
};
class data_wrapper {
 private:
  some_data data;
  std::mutex m;

 public:
  template <typename Function>
  void process_data(Function func) {
    std::lock_guard<std::mutex> l(m);
    func(data);  // 1 传递“保护”数据给用户函数
  }
  void display() {
    std::lock_guard<std::mutex> l(m);
    data.display();
  }
};
some_data* unprotected;
void malicious_function(some_data& protected_data) {
  unprotected = &protected_data;
}
data_wrapper x;
void foo() {
  x.display();                         // 0
  x.process_data(malicious_function);  // 2 传递一个恶意函数
  unprotected->do_something();  // 3 在无保护的情况下访问保护数据
  unprotected->display();       // 5 llll
  x.display();                  // 5 llll
}
// 切勿将受保护数据的指针或引用传递到互斥锁作用域之外，无论是函数返回值，还是存储在外部可见内存，亦或是以参数的形式传递到用户提供的函数中去。
// 这是在使用互斥量保护共享数据时常犯的错误，但绝不仅仅是一个潜在的陷阱而已。

int main() {
  foo();

  return 0;
}