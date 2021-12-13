#include <atomic>
#include <iostream>
#include <mutex>

// 通过单例模式， 可以做到：
// 1. 确保一个类只有一个实例被建立
// 2. 提供了一个对对象的全局访问指针
// 3. 在不影响单例类的客户端的情况下允许将来有多个实例

class Singleton {
 private:
  Singleton(){};
  Singleton(const Singleton& other){};

 public:
  static Singleton* getInstance1();
  static Singleton* getInstance2();
  static Singleton* getInstance3();

 private:
  static Singleton* m_instance;
  static std::mutex m_mutex;
};

Singleton* Singleton::m_instance = nullptr;

// 线程非安全版本
// 这是单例模式最经典的实现方式，将构造函数和拷贝构造函数都设为私有的，而且采用了延迟初始化的方式，在第一次调用getInstance()的时候才会生成对象，
// 不调用就不会生成对象，不占据内存。然而，在多线程的情况下，这种方法是不安全的。
// 正常情况下，如果线程A调用getInstance()时，将m_instance初始化了，那么线程B再调用getInstance()时，就不会再执行new了，直接返回之前构造好的对象。
// 然而存在这种情况，线程A执行m_instance = new
// Singleton()还没完成，这个时候m_instance仍然为nullptr，线程B也正在执行m_instance
// = new Singleton()，
// 这是就会产生两个对象，线程A和B可能使用的是同一个对象，也可能是两个对象，这样就可能导致程序错误，同时，还会发生内存泄漏。
Singleton* Singleton::getInstance1() {
  if (m_instance == nullptr) {
    m_instance = new Singleton();
  }

  return m_instance;
}

// 线程安全版本，但锁的代价过高
// 这种写法不会出现上面两个线程都执行new的情况，当线程A在执行m_instance = new
// Singleton()的时候，线程B如果调用了getInstance()，一定会被阻塞在加锁处，
// 等待线程A执行结束后释放这个锁。从而是线程安全的。
// 但这种写法的性能不高，因为每次调用getInstance()都会加锁释放锁，而这个步骤只有在第一次new
// Singleton()才是有必要的，只要m_instance被创建出来了，
// 不管多少线程同时访问，使用if(m_instance ==
// nullptr)进行判断都是足够的（只是读操作，不需要加锁），没有线程安全问题，加了锁之后反而存在性能问题。
std::mutex Singleton::m_mutex;
Singleton* Singleton::getInstance2() {
  // Lock lock；
  std::lock_guard<std::mutex> lock(m_mutex);
  if (m_instance == nullptr) {
    m_instance = new Singleton();
  }

  return m_instance;
}

// 双检查锁，但由于内存读写reorder不安全。
// 在相当长的一段时间，迷惑了很多人，在2000年的时候才被人发现漏洞，而且在每种语言上都发现了。原因是内存读写的乱序执行（编译器的问题）。
Singleton* Singleton::getInstance3() {
  // 线程A执行到new那行，被reorder，申请了内存并赋值给m_instance，此时线程B执行到这里，m_instance不为空，直接返回，但此时m_instance不可用（没调用构造函数）。
  if (m_instance == nullptr) {
    // Lock lock；
    std::lock_guard<std::mutex> lock(m_mutex);
    if (m_instance == nullptr) {
      // reorder：计算机指令序列执行顺序与假设的不一致。
      // 我们认为：1、申请内存；2、调用构造函数；3、把内存地址赋值给m_instance。
      // 在指令层面，可能不一样：1、申请内存；2、把内存地址赋值给m_instance;3、调用构造函数。
      m_instance = new Singleton();
      // 这句话可以分成三个步骤来执行：
      //  1 分配了一个Singleton类型对象所需要的内存。
      //  2 在分配的内存处构造Singleton类型的对象。
      //  3 把分配的内存的地址赋给指针m_instance。
      // 可能会认为这三个步骤是按顺序执行的，但实际上只能确定步骤1是最先执行的，步骤2，3却不一定。问题就出现在这。
      // 假如某个线程A在调用执行m_instance = new
      // Singleton()的时候是按照1,3,2的顺序的，那么刚刚执行完步骤3
      // 给Singleton类型分配了内存（此时m_instance就不是nullptr了）就切换到了线程B，由于m_instance已经不是
      // nullptr了，所以B直接执行return
      // m_instance得到一个对象，而这个对象并没有真正的被构造！严重bug发生了。
    }
  }

  return m_instance;
}

// java和c#发现这个问题后，就加了一个关键字volatile，在声明m_instance变量的时候，要加上volatile修饰，编译器看到之后，
// 就知道这个地方不能够reorder（一定要先分配内存，在执行构造器，都完成之后再赋值）。
// 而对于c++标准却一直没有改正，所以VC++在2005版本也加入了这个关键字，但是这并不能够跨平台（只支持微软平台）。

class Singleton1 {
 private:
  Singleton1(){};
  Singleton1(const Singleton1& other){};

 public:
  static Singleton1* getInstance();

 private:
  static std::atomic<Singleton1*> m_instance;
  static std::mutex m_mutex;
};

std::atomic<Singleton1*> Singleton1::m_instance;
std::mutex Singleton1::m_mutex;

// C++11版本之后的跨平台实现（volatile）
Singleton1* Singleton1::getInstance() {
  Singleton1* tmp = m_instance.load(std::memory_order_relaxed);

  // std::atomic_thread_fence(std::memory_order_acquire);
  // std::atomic_thread_fence(std::memory_order_release);
  // 这两句话可以保证他们之间的语句不会发生乱序执行。
  std::atomic_thread_fence(std::memory_order_acquire);  // 获取内存fence
  if (tmp == nullptr) {
    std::lock_guard<std::mutex> lock(m_mutex);
    if (tmp == nullptr) {
      tmp = new Singleton1();
      std::atomic_thread_fence(std::memory_order_release);  // 释放内存fence
      m_instance.store(tmp, std::memory_order_relaxed);
    }
  }
  return tmp;
}

// 在linux中，pthread_once()函数可以保证某个函数只执行一次。不能跨平台。
class Singleton2 {
 public:
  static Singleton2* getInstance() {
    // init函数只会执行一次
    pthread_once(&ponce_, &Singleton2::init);
    return m_instance;
  }

 private:
  Singleton2();  // 私有构造函数，不允许使用者自己生成对象
  Singleton2(const Singleton2& other);
  // 要写成静态方法的原因：类成员函数隐含传递this指针（第一个参数）
  static void init() { m_instance = new Singleton2(); }
  static pthread_once_t ponce_;
  static Singleton2* m_instance;  // 静态成员变量
};
pthread_once_t Singleton2::ponce_ = PTHREAD_ONCE_INIT;
Singleton2* Singleton2::m_instance = nullptr;  // 静态成员需要先初始化

// c++11版本最简洁的跨平台方案
// c++11中已经提供了std::call_once方法来保证函数在多线程环境中只被调用一次，同样，他也需要一个once_flag的参数。用法和pthread_once类似，并且支持跨平台。
// 实际上，还有一种最为简单的方案！局部静态变量不仅只会初始化一次，而且还是线程安全的。
// 这种单例被称为Meyers' Singleton 。这种方法很简洁，也很完美，但是注意：
//  gcc 4.0之后的编译器支持这种写法。
//  C++11及以后的版本（如C++14）的多线程下，正确。
//  C++11之前不能这么写。
class Singleton3 {
 public:
  // 注意返回的是引用
  static Singleton3& getInstance() {
    static Singleton3 m_instance;  // 局部静态变量
    return m_instance;
  }

 private:
  Singleton3();  // 私有构造函数，不允许使用者自己生成对象
  Singleton3(const Singleton3& other);
};

// 单例模式分为两种
//  懒汉模式：第一次调用时才初始化，有延迟加载的效果，以时间换空间。
//   优点：第一次使用实例对象时，创建对象。进程启动无负载。多个单例实例启动顺序自由控制。
//   缺点：复杂
//  饿汉模式：程序开始就初始化，以空间换时间
//   优点：简单
//   缺点：可能会导致进程启动慢，且如果有多个单例类对象实例启动顺序不确定。

// 饿汉模式一
// 在C++中非局部静态对象的初始化顺序的不确定性可能会导致上述饿汉模式工作不正常。考虑：如果有两个这样的单例类，将分别生成单例对象A,单例对象B.
// 它们分别定义在不同的编译单元（cpp中），而A的初始化依赖于B【即A的构造函数中要调用B::GetInstance()，而此时B::m_instance可能还未初始化，
// 显然调用结果就是非法的】，所以说只有B在A之前完成初始化程序才能正确运行，而这种跨编译单元的初始化顺序编译器是无法保证的。
class Singleton4 {
 public:
  static Singleton4* GetInstance() { return &m_instance; }

 private:
  Singleton4(){};
  Singleton4(Singleton4 const&);
  Singleton4& operator=(Singleton4 const&);

  static Singleton4 m_instance;
};
Singleton4 Singleton4::m_instance;  // 在程序入口之前就完成单例对象的初始化

// 饿汉模式二（boost实现）
// 单例对象作为静态局部变量，然后增加一个辅助类，并声明一个该辅助类的类静态成员变量，在该辅助类的构造函数中，初始化单例对象。
class Singleton5 {
 public:
  static Singleton5* GetInstance() {
    static Singleton5 instance;
    return &instance;
  }

 protected:
  // 辅助代理类
  struct Object_Creator {
    Object_Creator() { Singleton5::GetInstance(); }
  };
  static Object_Creator _object_creator;

  Singleton5() {}
  ~Singleton5() {}
};
Singleton5::Object_Creator Singleton5::_object_creator;

// C++11中借助可变参数模板实现的单例
template <typename T>
class Singleton6 {
 public:
  template <typename... Args>
  static T* Instance(Args&&... args) {
    if (m_pInstance == nullptr)
      m_pInstance = new T(std::forward<Args>(args)...);  //完美转发
    return m_pInstance;
  }
  // 获取单例
  static T* GetInstance() {
    if (m_pInstance == nullptr)
      throw std::logic_error(
          "the instance is not init,please initialzer the instance first!");
    return m_pInstance;
  }
  static void DestoryInstance() {
    delete m_pInstance;
    m_pInstance = nullptr;
  }

 private:
  Singleton6(void);
  ~Singleton6(void);
  Singleton6(const Singleton&) = delete;
  Singleton6& operator=(const Singleton&) = delete;

 private:
  static Singleton6* m_pInstance;
};

// 工作中用到的：
// A macro to disallow the copy constructor and operator= functions.
// This should be used in the private: declarations for a class.
#define DISALLOW_COPY_AND_ASSIGN(type) \
  type(const type&);                   \
  void operator=(const type&)
// A macro to disallow all the implicit constructors, namely the
// default constructor, copy constructor and operator= functions.
//
// This should be used in the private: declarations for a class
// that wants to prevent anyone from instantiating it. This is
// especially useful for classes containing only static methods.
#define DISALLOW_IMPLICIT_CONSTRUCTORS(type) \
  type();                                    \
  DISALLOW_COPY_AND_ASSIGN(type)
template <typename T>
class SingletonInterface {
 public:
  static T* GetInstance() {
    static T s_instance;
    return &s_instance;
  }

 protected:
  SingletonInterface() {}
  ~SingletonInterface() {}

 private:
  DISALLOW_COPY_AND_ASSIGN(SingletonInterface);
};
// class RadioState : public SingletonInterface<RadioState>
template <typename T>
class SingletonInterfaceSp {
 public:
  static std::shared_ptr<T> GetInstance() {
    static std::shared_ptr<T> s_instance = std::shared_ptr<T>(new T());
    return s_instance;
  }

 protected:
  SingletonInterfaceSp() {}
  ~SingletonInterfaceSp() {}

 private:
  DISALLOW_COPY_AND_ASSIGN(SingletonInterfaceSp);
};
// class RadioState : public SingletonInterfaceSp<RadioState>

int main() { return 0; }