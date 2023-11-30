#include <iostream>

// 在类中，如果不希望某些数据被修改，可以使用const关键字加以限定。
// const可以用来修饰成员变量和成员函数。

namespace n1 {
class A {
 public:
  // 初始化const成员变量只有一种方法，就是通过构造函数的初始化列表：
  A() : c(5) { i = 10; }
  // A() : { c = 5 }  // assignment of read-only member ‘n1::A::c’

  // const成员变量和普通const变量的用法相似，只需要在声明时加上const关键字：
  const int c;

  // const成员函数也称为常成员函数，声明也要const关键字：
  int Get() const;

  // 函数开头的const用来修饰函数的返回值，表示返回值是const类型，不能被修改：
  const char* Str() { return "nihao"; }

 private:
  int i;
};

// 和这个函数是两个不同的原型，如果定义时不加const，会冲突：
// int Get();
int A::Get() const {  // 常成员函数声明和定义时都要加上const关键字
  // const成员函数可以使用类中的所有成员变量，
  // 但是不能修改它们的值，主要是为了保护数据而设置的：
  // i = 5;  // assignment of member ‘n1::A::i’ in read-only object
  return c + i;
}

void func1() {
  A a;
  std::cout << a.c << std::endl;      // 5
  std::cout << a.Get() << std::endl;  // 15

  const char* s = a.Str();
  // s[0] = 'i';  // assignment of read-only location ‘* s’
}

// 在C++中，const也可以用来修饰对象，称为常对象。
// 一旦将对象定义为常对象之后，该对象就只能访问被const修饰的成员了，
// 因为非const成员可能会修改对象的数据（编译器也会这样假设），C++禁止这样做。
// 非const对象可以访问const成员函数。
void func2() {
  const A a1;
  A const a2;
  A a3;
  std::cout << a1.c << " " << a1.Get() << std::endl;  // 5 15
  std::cout << a2.c << " " << a2.Get() << std::endl;  // 5 15
  std::cout << a3.c << " " << a3.Get() << std::endl;  // 5 15
  // const char* s1 = a1.Str();
  // const char* s2 = a2.Str();
  std::cout << a3.Str() << std::endl;  // nihao

  const A* p1 = new A();
  A const* p2 = new A();
  A* p3 = new A();
  std::cout << p1->c << " " << p1->Get() << std::endl;  // 5 15
  std::cout << p2->c << " " << p2->Get() << std::endl;  // 5 15
  std::cout << p3->c << " " << p3->Get() << std::endl;  // 5 15
  // const char* s3 = p1->Str();
  // const char* s4 = p2->Str();
  std::cout << p3->Str() << std::endl;  // nihao
}
}  // namespace n1

int main(int argc, char* argv[]) {
  if (argc < 2) {
    std::cout << argv[0] << " i [0 - 1]" << std::endl;
    return 0;
  }
  int type = argv[1][0] - '0';
  switch (type) {
    case 0:
      n1::func1();
      break;
    case 1:
      n1::func2();
      break;
    default:
      std::cout << "invalid type" << std::endl;
      break;
  }
}