#include <limits.h>
#include <algorithm>
#include <deque>
#include <iostream>
#include <list>
#include <mutex>
#include <stack>
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

// 假设有一个stack<vector<int>>，vector是一个动态容器，当你拷贝一个vetcor，标准库会从堆上分配很多内存来完成这次拷贝。当这个系统处在重度负荷，
// 或有严重的资源限制的情况下，这种内存分配就会失败，所以vector的拷贝构造函数可能会抛出一个std::bad_alloc异常。当vector中存有大量元素时，这种情
// 况发生的可能性更大。当pop()函数返回“弹出值”时(也就是从栈中将这个值移除)，会有一个潜在的问题：这个值被返回到调用函数的时候，栈才被改变；但当拷贝
// 数据的时候，调用函数抛出一个异常会怎么样？ 如果事情真的发生了，要弹出的数据将会丢失；它的确从栈上移出了，但是拷贝失败了！std::stack的设计人员将
// 这个操作分为两部分：先获取顶部元素(top())，然后从栈中移除(pop())。这样，在不能安全的将元素拷贝出去的情况下，栈中的这个数据还依旧存在，没有丢失。
// 当问题是堆空间不足，应用可能会释放一些内存，然后再进行尝试。
// 不幸的是，这样的分割却制造了本想避免或消除的条件竞争。幸运的是，我们还有的别的选项，但是使用这些选项是要付出代价的:
// 1 传入一个引用
// 将变量的引用作为参数，传入pop()函数中获取想要的“弹出值”：
// std::vector<int> result;
// some_stack.pop(result);
// 大多数情况下，这种方式还不错，但缺点很明显：需要构造出一个栈中类型的实例，用于接收目标值。对于一些类型，这样做是不现实的，因为临时构造一个实例，
// 从时间和资源的角度上来看，都是不划算。对于其他的类型，这样也不总能行得通，因为构造函数需要的一些参数，在这个阶段的代码不一定可用。最后，需要可赋
// 值的存储类型，这是一个重大限制：即使支持移动构造，甚至是拷贝构造(从而允许返回一个值)，很多用户自定义类型可能都不支持赋值操作。
// 2 无异常抛出的拷贝构造函数或移动构造函数
// 对于有返回值的pop()函数来说，只有“异常安全”方面的担忧(当返回值时可以抛出一个异常)。很多类型都有拷贝构造函数，它们不会抛出异常，并且随着新标准中
// 对“右值引用”的支持，很多类型都将会有一个移动构造函数，即使他们和拷贝构造函数做着相同的事情，它也不会抛出异常。一个有用的选项可以限制对线程安全的
// 栈的使用，并且能让栈安全的返回所需的值，而不会抛出异常。
// 虽然安全，但非可靠。尽管能在编译时可使用std::is_nothrow_copy_constructible和std::is_nothrow_move_constructible类型特征，让拷贝或移动
// 构造函数不抛出异常，但是这种方式的局限性太强。用户自定义的类型中，会有不抛出异常的拷贝构造函数或移动构造函数的类型， 那些有抛出异常的拷贝构造函数，
// 但没有移动构造函数的类型往往更多（这种情况会随着人们习惯于C++11中的右值引用而有所改变)。如果这些类型不能被存储在线程安全的栈中，那将是多么的不幸。
// 3 返回指向弹出值的指针
// 返回一个指向弹出元素的指针，而不是直接返回值。指针的优势是自由拷贝，并且不会产生异常，这样你就能避免Cargill提到的异常问题了。缺点就是返回一个指针
// 需要对对象的内存分配进行管理，对于简单数据类型(比如：int)，内存管理的开销要远大于直接返回值。对于选择这个方案的接口，使用std::shared_ptr是个不
// 错的选择；不仅能避免内存泄露(因为当对象中指针销毁时，对象也会被销毁)，而且标准库能够完全控制内存分配方案，也就不需要new和delete操作。这种优化是很
// 重要的：因为堆栈中的每个对象，都需要用new进行独立的内存分配，相较于非线程安全版本，这个方案的开销相当大。
// 4 1 + 2 或 1 + 3
// 对于通用的代码来说，灵活性不应忽视。当选择了选项2或3时，再去选择1也是很容易的。这些选项提供给用户，让用户选择对于他们自己来说最合适，最经济的方案。

// 定义线程安全的堆栈
// 实现了选项1和选项3：重载了pop()，使用一个局部引用去存储弹出值，并返回一个std::shared_ptr<>对象。
struct empty_stack : std::exception {
  const char* what() const throw() { return "empty stack!"; };
};
template <typename T>
class threadsafe_stack {
 private:
  std::stack<T> data;
  mutable std::mutex m;

 public:
  threadsafe_stack() : data(std::stack<T>()) {}
  threadsafe_stack(const threadsafe_stack& other) {
    std::lock_guard<std::mutex> lock(other.m);
    data = other.data;  // 1 在构造函数体中的执行拷贝
  }
  threadsafe_stack& operator=(const threadsafe_stack&) = delete;
  void push(T new_value) {
    std::lock_guard<std::mutex> lock(m);
    data.push(new_value);
  }
  std::shared_ptr<T> pop() {
    std::lock_guard<std::mutex> lock(m);
    if (data.empty()) throw empty_stack();  // 在调用pop前，检查栈是否为空
    std::shared_ptr<T> const res(
        std::make_shared<T>(data.top()));  // 在修改堆栈前，分配出返回值
    data.pop();
    return res;
  }
  void pop(T& value) {
    std::lock_guard<std::mutex> lock(m);
    if (data.empty()) throw empty_stack();
    value = data.top();
    data.pop();
  }
  bool empty() const {
    std::lock_guard<std::mutex> lock(m);
    return data.empty();
  }
};

// 一个给定操作需要两个或两个以上的互斥量时，另一个潜在的问题将出现：死锁。与条件竞争完全相反——不同的两个线程会互相等待，从而什么都没做。
// 避免死锁的一般建议，就是让两个互斥量总以相同的顺序上锁：总在互斥量B之前锁住互斥量A，就永远不会死锁。
// 某些情况下是可以这样用，因为不同的互斥量用于不同的地方。不过，事情没那么简单，比如：当有多个互斥量保护同一个类的独立实例时，一个操作对同一个类的两个不同实
// 例进行数据的交换操作，为了保证数据交换操作的正确性，就要避免数据被并发修改，并确保每个实例上的互斥量都能锁住自己要保护的区域。不过，选择一个固定的顺序(例如，
// 实例提供的第一互斥量作为第一个参数，提供的第二个互斥量为第二个参数)，可能会适得其反：在参数交换了之后，两个线程试图在相同的两个实例间进行数据交换时，程序又死锁了！
// 很幸运，C++标准库有办法解决这个问题，std::lock可以一次性锁住多个(两个以上)的互斥量，并且没有副作用(死锁风险)。
class some_big_object {};
void swap(some_big_object& lhs, some_big_object& rhs);
class X {
 private:
  some_big_object some_detail;
  std::mutex m;

 public:
  X(some_big_object const& sd) : some_detail(sd) {}
  friend void swap(X& lhs, X& rhs) {
    if (&lhs == &rhs) return;
    std::lock(lhs.m, rhs.m);
    std::lock_guard<std::mutex> lock_a(lhs.m, std::adopt_lock);
    std::lock_guard<std::mutex> lock_b(rhs.m, std::adopt_lock);
    swap(lhs.some_detail, rhs.some_detail);
  }
};

// 虽然std::lock(和std::scoped_lock<>)可以在这情况下(获取两个以上的锁)避免死锁，但它没办法帮助你获取其中一个锁。这时，依赖于开发者的纪律性(也就是经验)，来确
// 保你的程序不会死锁。这并不简单：死锁是多线程编程中一个令人相当头痛的问题，并且死锁经常是不可预见的，因为在大多数时间里，所有工作都能很好的完成。不过，一些相对简
// 单的规则能帮助写出“无死锁”的代码。
// 1 避免嵌套锁
// 第一个建议往往是最简单的：一个线程已获得一个锁时，再别去获取第二个。因为每个线程只持有一个锁，锁上就不会产生死锁。即使互斥锁造成死锁的最常见原因，也可能会在其他
// 方面受到死锁的困扰(比如：线程间的互相等待)。当你需要获取多个锁，使用一个std::lock来做这件事(对获取锁的操作上锁)，避免产生死锁。
// 2 避免在持有锁时调用用户提供的代码
// 第二个建议是次简单的：因为代码是用户提供的，你没有办法确定用户要做什么；用户程序可能做任何事情，包括获取锁。你在持有锁的情况下，调用用户提供的代码；如果用户代码
// 要获取一个锁，就会违反第一个指导意见，并造成死锁(有时，这是无法避免的)。当你正在写一份通用代码，每一个操作的参数类型，都在用户提供的代码中定义，就需要其他指导意
// 见来帮助你。
// 3 使用固定顺序获取锁
// 当硬性条件要求你获取两个或两个以上的锁，并且不能使用std::lock单独操作来获取它们；那么最好在每个线程上，用固定的顺序获取它们(锁)。
// 当需要获取两个互斥量时，避免死锁的方法，关键是如何在线程之间，以一定的顺序获取锁。一些情况下，这种方式相对简单。
// 其他情况下，这就没那么简单了，例如交换操作，这种情况下你可能同时锁住多个互斥量(有时不会发生)。
// 4 使用锁的层次结构
// 虽然，定义锁的顺序是一种特殊情况，但锁的层次的意义在于提供对运行时约定是否被坚持的检查。这个建议需要对你的应用进行分层，并且识别在给定层上所有可上锁的互斥量。当
// 代码试图对一个互斥量上锁，在该层锁已被低层持有时，上锁是不允许的。你可以在运行时对其进行检查，通过分配层数到每个互斥量上，以及记录被每个线程上锁的互斥量。
class hierarchical_mutex {
  std::mutex internal_mutex;
  unsigned long const hierarchy_value;
  unsigned long previous_hierarchy_value;
  static thread_local unsigned long this_thread_hierarchy_value;  // 1
  void check_for_hierarchy_violation() {
    if (this_thread_hierarchy_value <= hierarchy_value)  // 2
    {
      throw std::logic_error("mutex hierarchy violated");
    }
  }
  void update_hierarchy_value() {
    previous_hierarchy_value = this_thread_hierarchy_value;  // 3
    this_thread_hierarchy_value = hierarchy_value;
  }

 public:
  explicit hierarchical_mutex(unsigned long value)
      : hierarchy_value(value), previous_hierarchy_value(0) {}
  void lock() {
    check_for_hierarchy_violation();
    internal_mutex.lock();     // 4
    update_hierarchy_value();  // 5
  }
  void unlock() {
    if (this_thread_hierarchy_value != hierarchy_value)
      throw std::logic_error("mutex hierarchy violated");    // 9
    this_thread_hierarchy_value = previous_hierarchy_value;  // 6
    internal_mutex.unlock();
  }
  bool try_lock() {
    check_for_hierarchy_violation();
    if (!internal_mutex.try_lock())  // 7
      return false;
    update_hierarchy_value();
    return true;
  }
};
thread_local unsigned long hierarchical_mutex::this_thread_hierarchy_value(
    ULONG_MAX);  // 8
// 5 超越锁的延伸扩展
// std::lock()和std::lock_guard可组成简单的锁，并覆盖大多数情况，但是有时需要更多的灵活性。在这种情况下，可以使用标准库提供的std::unique_lock模板。
// 如std::lock_guard，这是一个参数化的互斥量模板类，并且它提供很多RAII类型锁用来管理std::lock_guard类型，可以让代码更加灵活。

// unique_lock是个类模板，工作中，一般lock_guard(推荐使用)；lock_guard取代了mutex的lock()和unlock();
// unique_lock比lock_guard灵活很多，效率上差一点，内存占用多一点。
// lock_guard可以带第二个参数：
// std::lock_guard<std::mutex> sbguard1(my_mutex1, std::adopt_lock);  // std::adopt_lock标记作用；

int main() {
  foo();

  return 0;
}