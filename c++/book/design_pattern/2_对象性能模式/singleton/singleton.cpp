#include <atomic>
#include <iostream>
#include <memory>
#include <mutex>

// 动机：
// 1.在软件系统中，经常有这样一些特殊的类，必须保证它们在系统中只存在一个实例，
//   才能确保它们的逻辑正确性，以及良好的效率。
// 2.如何绕过常规的构造器，提供一种机制来保证一个类只有一个实例？
// 3.这应该是类设计者的责任，而不是使用者的责任。

// 定义：保证一个类仅有一个实例，并提供一个该实例的全局访问点。

// 要点总结：
// 1.Singleton模式中的实例构造器可以设置为protected以允许子类派生；
// 2.Singleton模式一般不要支持拷贝构造函数和Clone接口，
//   因为这有可能导致多个实例，与Singleton模式的初衷违背。
// 3.如何实现多线程环境下安全的Singleton？注意对双检查锁的正确实现。

// 通过单例模式，可以做到：
// 1.确保一个类只有一个实例被建立；
// 2.提供了一个对对象的全局访问指针；
// 3.在不影响单例类的客户端的情况下允许将来有多个实例。

// 单例模式分为两种：
// 1.懒汉模式，第一次调用时才初始化，有延迟加载的效果，以时间换空间。
//   优点：第一次使用实例对象时，创建对象，进程启动无负载，
//        多个单例实例启动顺序自由控制。
//   缺点：复杂。
// 2.饿汉模式，程序开始就初始化，以空间换时间。
//   优点：简单。
//   缺点：可能会导致进程启动慢，且如果有多个单例类对象实例启动顺序不确定。

// 懒汉式设计模式实现方式：
// 1.静态指针+用到时初始化；
// 2.局部静态变量。
// 饿汉式设计模式实现方式：
// 1.直接定义静态对象；
// 2.静态指针+类外初始化时new空间实现。

// 懒汉模式的特点是延迟加载，比如配置文件，采用懒汉式的方法，
// 配置文件的实例直到用到的时候才会加载，不到万不得已就不会去实例化类，
// 也就是说在第一次用到类实例的时候才会去实例化。

// 饿汉模式单例类定义的时候就进行实例化，因为main函数执行之前，
// 全局作用域的类成员静态变量m_Instance已经初始化，故没有多线程的问题。

// Singleton模式VS全局变量：
// 很多情况下，使用Singleton模式达到的效果和全局变量达到的效果类似。
// 但是，全局变量不能防止实例化多个对象。
// Singleton模式的意图保证一个类仅有一个对象，并提供一个访问它的全局访问点，
// 因此全局变量可以达到后面半句的效果，但是却不能保证仅有一个对象被实例化。
// 另外，使用全局变量将使得对象在无论是否用到都要被创建，而Singleton则没有这个瑕疵。

namespace n1 {
// 线程非安全版本

// 这是单例模式最经典的实现方式，将构造函数和拷贝构造函数都设为私有的，
// 而且采用了延迟初始化的方式，在第一次调用getInstance()的时候才会生成对象，
// 不调用就不会生成对象，不占据内存，然而，在多线程的情况下，这种方法是不安全的。
class Singleton {
 private:
  Singleton(){};
  Singleton(const Singleton& other){};

 public:
  static Singleton* getInstance();

 private:
  static Singleton* m_instance;
};
Singleton* Singleton::m_instance = nullptr;
Singleton* Singleton::getInstance() {
  // 正常情况下，如果线程A调用getInstance()时，将m_instance初始化了，
  // 那么线程B再调用getInstance()时，就不会再new了，直接返回之前构造好的对象。
  // 然而存在这种情况，线程A执行m_instance = new Singleton()还没完成，
  // 此时m_instance仍为nullptr，线程B也正在执行m_instance=new Singleton()，
  // 这是就会产生两个对象，线程A和B可能使用的是同一个对象，也可能是两个对象，
  // 这样就可能导致程序错误，同时，还会发生内存泄漏。
  if (m_instance == nullptr) {
    m_instance = new Singleton();
  }

  return m_instance;
}
}  // namespace n1

namespace n2 {
// 线程安全版本，但锁的代价过高

class Singleton {
 private:
  Singleton(){};
  Singleton(const Singleton& other){};

 public:
  static Singleton* getInstance();

 private:
  static Singleton* m_instance;
  static std::mutex m_mutex;
};
Singleton* Singleton::m_instance = nullptr;
std::mutex Singleton::m_mutex;
Singleton* Singleton::getInstance() {
  // 这种写法不会出现n1两个线程都执行new的情况，当线程A执行new Singleton()时，
  // 线程B如果调用了getInstance()，会阻塞到线程A结束后释锁，从而是线程安全的。
  // 但这种写法的性能不高，因为每次调用getInstance()都会加锁释放锁，
  // 只有在第一次new Singleton()才需要锁，只要m_instance被创建出来了，
  // 不管多少线程同时访问，使用if(m_instance == nullptr)进行判断都是足够的，
  // 因为只是读操作，不需要加锁，从而没有线程安全问题，加了锁之后反而存在性能问题。
  std::lock_guard<std::mutex> lock(m_mutex);
  if (m_instance == nullptr) {
    m_instance = new Singleton();
  }

  return m_instance;
}
}  // namespace n2

namespace n3 {
// 双检查锁，但由于内存读写reorder不安全

// 在相当长的一段时间，迷惑了很多人，在2000年的时候才被人发现漏洞，
// 而且在每种语言上都发现了，原因是内存读写的乱序执行（编译器的问题）。
class Singleton {
 private:
  Singleton(){};
  Singleton(const Singleton& other){};

 public:
  static Singleton* getInstance();

 private:
  static Singleton* m_instance;
  static std::mutex m_mutex;
};
Singleton* Singleton::m_instance = nullptr;
std::mutex Singleton::m_mutex;
Singleton* Singleton::getInstance() {
  // 线程A执行到new那行，被reorder，申请了内存并赋值给m_instance，
  // 此时线程B执行到这里，m_instance不为空，直接返回，
  // 但此时线程B得到的m_instance不可用，因为还没调用构造函数。
  if (m_instance == nullptr) {
    std::lock_guard<std::mutex> lock(m_mutex);
    if (m_instance == nullptr) {
      // reorder：计算机指令序列执行顺序与假设的不一致。
      // 大多数人认为：
      // 1.申请内存；
      // 2.调用构造函数；
      // 3.把内存地址赋值给m_instance。
      // 在指令层面，可能不一样：
      // 1.申请内存；
      // 2.把内存地址赋值给m_instance;
      // 3.调用构造函数。
      m_instance = new Singleton();
      // 这句话可以分成三个步骤来执行：
      // 1.分配了一个Singleton类型对象所需要的内存；
      // 2.在分配的内存处构造Singleton类型的对象；
      // 3.把分配的内存的地址赋给指针m_instance。
      // 可能会认为是按顺序执行的，但实际上只能确定1是最先执行的，2，3却不一定。

      // 问题就出现在这：
      // 假如线程A在调用执行m_instance=new Singleton()的时按照1，3，2的顺序的，
      // 那么刚刚执行完步骤3给Singleton类型分配了内存就切换到了线程B，
      // 由于m_instance已经不是nullptr了，所以B直接执行return m_instance，
      // 而这个对象并没有真正的被构造，严重bug发生了。
    }
  }

  return m_instance;
}

// java和c#发现这个问题后，就加了关键字volatile，在声明m_instance变量的时候，
// 要加上volatile修饰，编译器看到之后， 就知道这个地方不能够reorder，
// 一定要先分配内存，在执行构造器，都完成之后再赋值。
// 而对于c++标准却一直没有改正，所以VC++在2005版本也加入了这个关键字，
// 但是这并不能够跨平台，只支持微软平台。
}  // namespace n3

namespace n4 {
// C++11版本之后的跨平台实现（volatile）
class Singleton {
 private:
  Singleton(){};
  Singleton(const Singleton& other){};

 public:
  static Singleton* getInstance();

 private:
  static std::atomic<Singleton*> m_instance;
  static std::mutex m_mutex;
};
std::atomic<Singleton*> Singleton::m_instance;
std::mutex Singleton::m_mutex;
Singleton* Singleton::getInstance() {
  Singleton* tmp = m_instance.load(std::memory_order_relaxed);

  // std::atomic_thread_fence(std::memory_order_acquire);
  // std::atomic_thread_fence(std::memory_order_release);
  // 这两句话可以保证他们之间的语句不会发生乱序执行。
  std::atomic_thread_fence(std::memory_order_acquire);  // 获取内存fence
  if (tmp == nullptr) {
    std::lock_guard<std::mutex> lock(m_mutex);
    if (tmp == nullptr) {
      tmp = new Singleton();
      std::atomic_thread_fence(std::memory_order_release);  // 释放内存fence
      m_instance.store(tmp, std::memory_order_relaxed);
    }
  }
  return tmp;
}
}  // namespace n4

namespace n5 {
// 在linux中，pthread_once()函数可以保证某个函数只执行一次，不能跨平台。
class Singleton {
 public:
  static Singleton* getInstance() {
    // init函数只会执行一次
    pthread_once(&ponce_, &Singleton::init);
    return m_instance;
  }

 private:
  Singleton();  // 私有构造函数，不允许使用者自己生成对象
  Singleton(const Singleton& other);
  // 要写成静态方法的原因：类成员函数隐含传递this指针（第一个参数）
  static void init() { m_instance = new Singleton(); }
  static pthread_once_t ponce_;
  static Singleton* m_instance;  // 静态成员变量
};
pthread_once_t Singleton::ponce_ = PTHREAD_ONCE_INIT;
Singleton* Singleton::m_instance = nullptr;  // 静态成员需要先初始化
}  // namespace n5

namespace n6 {
// c++11版本最简洁的跨平台方案

// c++11中已经提供了std::call_once方法来保证函数在多线程环境中只被调用一次，
// 需要一个once_flag的参数，用法和pthread_once类似，并且支持跨平台。

// 还有一种最为简单的方案，局部静态变量不仅只会初始化一次，而且还是线程安全的。
// 这种单例被称为Meyers' Singleton，这种方法很简洁，也很完美，但是注意：
// 1.gcc4.0之后的编译器支持这种写法；
// 2.C++11及以后的版本（如C++14）的多线程下，正确；
// 3.C++11之前不能这么写。
class Singleton {
 public:
  // 注意返回的是引用，返回指针也行
  static Singleton& getInstance() {
    static Singleton m_instance;  // 局部静态变量
    return m_instance;
  }

 private:
  Singleton() {}  // 私有构造函数，不允许使用者自己生成对象
  Singleton(const Singleton& other) {}
};
}  // namespace n6

namespace n7 {
// 饿汉模式一

// 在C++中非局部静态对象的初始化顺序的不确定性可能会导致饿汉模式工作不正常。
// 假设有两个这样的单例类，将分别生成单例对象A，单例对象B。
// 它们分别定义在不同的编译单元（cpp中），而A的初始化依赖于B，
// 即A的构造函数中要调用B::GetInstance()，而此时B::m_instance可能还未初始化，
// 显然调用结果就是非法的，所以说只有B在A之前完成初始化程序才能正确运行，
// 而这种跨编译单元的初始化顺序编译器是无法保证的。
class Singleton {
 public:
  static Singleton* GetInstance() { return &m_instance; }

 private:
  Singleton(){};
  Singleton(Singleton const&);
  Singleton& operator=(Singleton const&);

  static Singleton m_instance;
};
Singleton Singleton::m_instance;  // 在程序入口之前就完成单例对象的初始化
}  // namespace n7

namespace n8 {
// 饿汉模式二（boost实现）

// 单例对象作为静态局部变量，然后增加一个辅助类，
// 并声明一个该辅助类的类静态成员变量，在辅助类的构造函数中，初始化单例对象。
class Singleton {
 public:
  static Singleton* GetInstance() {
    static Singleton instance;
    return &instance;
  }

 protected:
  // 辅助代理类
  struct Object_Creator {
    Object_Creator() { Singleton::GetInstance(); }
  };
  static Object_Creator _object_creator;

  Singleton() {}
  ~Singleton() {}
};
Singleton::Object_Creator Singleton::_object_creator;
}  // namespace n8

namespace n9 {
// C++11中借助可变参数模板实现的单例
template <typename T>
class Singleton {
 public:
  template <typename... Args>
  static T* Instance(Args&&... args) {
    if (m_pInstance == nullptr)
      m_pInstance = new T(std::forward<Args>(args)...);  // 完美转发
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
  Singleton(void);
  ~Singleton(void);
  Singleton(const Singleton&) = delete;
  Singleton& operator=(const Singleton&) = delete;

 private:
  static Singleton* m_pInstance;
};
}  // namespace n9

namespace n10 {
// 工作中用到的：
#define DISALLOW_COPY_AND_ASSIGN(type) \
  type(const type&);                   \
  void operator=(const type&);
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
  DISALLOW_COPY_AND_ASSIGN(SingletonInterface)
};
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
  DISALLOW_COPY_AND_ASSIGN(SingletonInterfaceSp)
};

class A : public SingletonInterface<A> {
 public:
  ~A() = default;
  void show() { std::cout << "A" << std::endl; }

 private:
  A() = default;

  friend SingletonInterface<A>;
  DISALLOW_COPY_AND_ASSIGN(A);
};  // class Business

void func() { A::GetInstance()->show(); }
}  // namespace n10

int main() { return 0; }