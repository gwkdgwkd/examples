#include <iostream>

// 构造函数不能是虚函数，因为派生类不能继承基类的构造函数，将构造函数声明为虚函数没有意义。
// 这是原因之一，另外还有一个原因：
// C++中的构造函数用于在创建对象时进行初始化工作，在执行构造函数之前对象尚未创建完成，
// 虚函数表尚不存在，也没有指向虚函数表的指针，所以此时无法查询虚函数表，也就不知道要调用哪一个构造函数。
// 析构函数用于在销毁对象时进行清理工作，可以声明为虚函数，而且有时候必须要声明为虚函数。

// 在实际开发中，一旦我们自己定义了析构函数，就是希望在对象销毁时用它来进行清理工作，
// 比如释放内存、关闭文件等，如果这个类又是一个基类，那么就必须将该析构函数声明为虚函数，否则就有内存泄露的风险。
// 也就是说，大部分情况下都应该将基类的析构函数声明为虚函数。
// 注意，这里强调的是基类，如果一个类是最终的类，那就没必要再声明为虚函数了。

namespace n1 {
class Base {
 public:
  Base() {
    str = new char[100];
    std::cout << "Base constructor" << std::endl;
  }
  ~Base() {
    delete[] str;
    std::cout << "Base destructor" << std::endl;
  }

 protected:
  char *str;
};
class Derived : public Base {
 public:
  Derived() {
    name = new char[100];
    std::cout << "Derived constructor" << std::endl;
  }
  ~Derived() {
    delete[] name;
    std::cout << "Derived destructor" << std::endl;
  }

 private:
  char *name;
};

void func1() {
  Base *pb = new Derived();
  delete pb;  // 只调用了基类的析构函数，没有调用派生类的析构函数
  // Base constructor
  // Derived constructor
  // Base destructor

  // 不调用派生类的析构函数会导致内存空间得不到释放；
  // 除非程序运行结束由操作系统回收，否则就再也没有机会释放这些内存，这是典型的内存泄露。
  // 为什么delete pb;不会调用派生类的析构函数呢？
  // 因为这里的析构函数是非虚函数，通过指针访问非虚函数时，编译器会根据指针的类型来确定要调用的函数；
  // 也就是说，指针指向哪个类就调用哪个类的函数。
  // pb是基类的指针，所以不管它指向基类的对象还是派生类的对象，始终都是调用基类的析构函数。
}
void func2() {
  Derived *pd = new Derived();
  delete pd;  // 同时调用了派生类和基类的析构函数
  // Base constructor
  // Derived constructor
  // Derived destructor
  // Base destructor

  // 为什么delete pd;会同时调用派生类和基类的析构函数呢？
  // pd是派生类的指针，编译器会根据它的类型匹配到派生类的析构函数，
  // 在执行派生类的析构函数的过程中，又会调用基类的析构函数。
  // 派生类析构函数始终会调用基类的析构函数，并且这个过程是隐式完成的。
}

void testN1() {
  func1();
  func2();
}
}  // namespace n1

namespace n2 {
class Base {
 public:
  Base() {
    str = new char[100];
    std::cout << "Base constructor" << std::endl;
  }
  virtual ~Base() {
    delete[] str;
    std::cout << "Base destructor" << std::endl;
  }

 protected:
  char *str;
};
class Derived : public Base {
 public:
  Derived() {
    name = new char[100];
    std::cout << "Derived constructor" << std::endl;
  }
  ~Derived() {
    delete[] name;
    std::cout << "Derived destructor" << std::endl;
  }

 private:
  char *name;
};

void func1() {
  Base *pb = new Derived();
  delete pb;  // 同时调用了派生类和基类的析构函数
  // Base constructor
  // Derived constructor
  // Derived destructor
  // Base destructor

  // 将基类的析构函数声明为虚函数后，派生类的析构函数也会自动成为虚函数。
  // 这个时候编译器会忽略指针的类型，而根据指针的指向来选择函数；
  // 也就是说，指针指向哪个类的对象就调用哪个类的函数。
  // pb指向了派生类的对象，所以会调用派生类的析构函数，继而再调用基类的析构函数。
  // 如此一来也就解决了内存泄露的问题。
}
void func2() {
  Derived *pd = new Derived();
  delete pd;  // 同时调用了派生类和基类的析构函数
  // Base constructor
  // Derived constructor
  // Derived destructor
  // Base destructor

  // 同上，pd也指向来派生类对象，所以也会现调用派生类的析构函数，继而再调用基类的析够函数。
}

void testN2() {
  func1();
  func2();
}
}  // namespace n2

int main(int argc, char *argv[]) {
  if (argc < 2) {
    std::cout << argv[0] << " i [0 - 1]" << std::endl;
    return 0;
  }
  int type = argv[1][0] - '0';
  switch (type) {
    case 0:
      n1::testN1();
      break;
    case 1:
      n2::testN2();
      break;
    default:
      std::cout << "invalid type" << std::endl;
      break;
  }

  return 0;
}