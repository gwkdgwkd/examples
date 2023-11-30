#include <iostream>

namespace n1 {
class A {
 public:
  A(int i) : a(i) { std::cout << "A(int)" << std::endl; }
  A(const A& a) { std::cout << "A(const A&)" << std::endl; }
  A& operator=(const A& a) { std::cout << "operator =(const A&)" << std::endl; }
  A() { std::cout << "A()" << std::endl; }

 private:
  int a;
};

class B {
 public:
  B(int i) : a(i) { std::cout << "B(int)" << std::endl; }
  B(const B& a) { std::cout << "B(const B&)" << std::endl; }
  B& operator=(const B& a) { std::cout << "operator =(const B&)" << std::endl; }
  B() { std::cout << "B()" << std::endl; }

 private:
  int a;
};

class AB {
 public:
  // 对于a_，直接调用拷贝构造函数
  AB(A a, B b) : a_(a), i(5) {
    // 对于b_，先调用了默认构造函数，然后调用了赋值操作符号
    b_ = b;

    // 对大多数类型而言，只调用一次构造函数效率高，
    // 比先调用默认构造函数再调用赋值操作符要高效。

    // 对于内置类型，初始化和赋值成本相同，但为了一致性，最好也使用初始化列表。
    j = 6;
  }

 private:
  A a_;
  B b_;
  int i;
  int j;
};

void func() {
  A a;  // A()
  B b;  // B()

  std::cout << "create AB" << std::endl;  // create AB

  // B(const B&)
  // A(const A&)
  AB(a, b);
  // A(const A&)
  // B()
  // operator =(const B&)
}
}  // namespace n1

namespace n2 {
class A {
 public:
  A(int a) {
    i = a;
    i = 6;  // 是赋值不是初始化，构造函数体内可以多次赋值
  }

  // 初始化列表是初始化，所以一个变量只能出现一次：
  // A() : i(4), i(5) {}
  // error: multiple initializations given for ‘n2::A::i’

  A() : i(4) {  // 初始化也只能使用一次
    i = 5;  // 编译器允许构造函数赋值和初始化列表初始化混用
  }

 private:
  int i;
};
}  // namespace n2

namespace n3 {
class A {
 public:
  A(int i) : a(i) { std::cout << "A(int)" << std::endl; }

 private:
  int a;
};

int g = 6;
class C {
 public:
  C(A a) : i(2), j(g), a_(a) {
    // i = 2;  // assignment of read-only member ‘n2::A::i’
  }

  // 下面三种类型的成员变量只能在初始化列表初始化：
  const int i;  // const成员变量
  int& j;       // 引用成员变量
  A a_;         // 没有默认构造函数的自定义类型成员变量
};

void func() {
  A a(6);  // A(int)
  C c(a);
  std::cout << g << std::endl;  // 6
  c.j = 8;
  std::cout << g << std::endl;  // 8
}
}  // namespace n3

// 总结：
// 1.构造函数的一项重要功能是对成员变量进行初始化，为了达到这个目的，
//   可以在构造函数的函数体中对成员变量一一赋值，还可以采用初始化列表；
// 2.初始化列表是对象的成员变量定义的地方；
// 3.对象的内置类型成员变量在初始化列表定义时没有要求必须初始化，
//   因此既可以在初始化列表进行初始化，也可以在构造函数体内初始化；
// 4.而const成员变量、引用成员变量、没有默认构造函数的自定义类型成员变量不能先定义再初始化，
//   它们在初始化列表内定义，并且必须在定义时就初始化，因此必须在初始化列表内初始化；
// 5.尽量使用初始化列表初始化，因为不管是否使用初始化列表，虽然对于内置类型没有差别，
//   但是对于自定义类型成员变量，一定会先使用初始化列表初始化。

int main(int argc, char* argv[]) {
  if (argc < 2) {
    std::cout << argv[0] << " i [0 - 1]" << std::endl;
    return 0;
  }
  int type = argv[1][0] - '0';
  switch (type) {
    case 0:
      n1::func();
      break;
    case 1:
      n3::func();
      break;
    default:
      std::cout << "invalid type" << std::endl;
      break;
  }

  return 0;
}