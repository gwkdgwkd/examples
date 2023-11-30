#include <iostream>
#include <list>
#include <string>

// 确定对象被使用前已先被初始化

namespace n1 {
// 永远在使用对象之前先将它初始化，对于内置类型，必须手工完成初始化。
// 对于内置类型以外的任何其他东西，初始化责任落在构造函数身上，
// 确保每一个构造函数都将对象的每一个成员初始化。
class A {};
namespace test1 {
class B {
 public:
  // 这会正确初始化对象，但不是最佳做法。
  // 首先调用default构造函数，然后立刻再赋值，default构造函数浪费了。
  B(const std::string& name, const std::string& address,
    const std::list<A>& phones) {
    // 下面都不是被初始化，初始化发生时间比进入构造函数更早，而是被赋值：
    s1 = name;
    s2 = address;
    la = phones;
    // 内置类型除外，不保证一定在赋值动作的时间点之前获得初值：
    num = 0;
  }

 private:
  std::string s1;
  std::string s2;
  std::list<A> la;
  int num;
};
}  // namespace test1

namespace test2 {
class B {
 public:
  // B构造函数的较佳写法是，使用初始化列表替换赋值动作，结果相同，但效率较高。
  // 初始化列表是针对各个成员变量而设的实参，当做各成员变量拷贝构造的实参。
  // 对大多数类型而言，只调用一次copy构造函数效率高，
  // 比先调用default在调用copy assignment操作符要高效。
  // 对于内置类型，初始化和赋值成本相同，但为了一致性，最好也使用初始化列表。
  B(const std::string& name, const std::string& address,
    const std::list<A>& phones)
      : s1(name), s2(address), la(phones), num(0) {
    // 想要default构造成员变量，也可以使用初始化列表，指定作为初始化实参即可。
    // 总是在初始化列表中列出所有成员变量，不然不确定是忘记了，还是不需要初值。
    // 有些情况内置类型（初始化和赋值成本相同）也需要使用初始化列表。
    // 如果成员变量是const或reference，就一定需要初值，不能被赋值。

    // 有多个构造函数，会导致重复代码，
    // 可以在初始化列表中遗漏那些（赋值和初始化一样好）的成员变量，
    // 改用赋值操作，并将赋值放到一个函数内（private）。
  }

 private:
  std::string s1;
  std::string s2;
  std::list<A> la;
  int num;
};
}  // namespace test2
}  // namespace n1

namespace n2 {
// C++有着十分固定的成员初始化次序：
// 1.base class更早于derived class被初始化；
// 2.class的成员变量总是以其声明次序被初始化，
//   即使在成员初始化列表中以不同的次序（合法的），也不会有任何影响，
//   所以初始化列表最好和声明顺序一致。

// 不同编译单元内定义的non-local static对象的初始化次序：
// static对象，从被构造出来直到程序结束为止，与stack和heap对象不同。
// 程序结束时static对象会被自动销毁，析构函数会在main()结束时被自动调用。
// 包括：
// 1.global static对象；
// 2.定义namespace内的static对象；
// 3.class内的static成员；
// 4.函数内的static对象；
// 5.文件作用域内的static对象。
// 函数内的static对象被称为local static对象，
// 其他static对象称为non-local static对象。

// 编译单元是指产出单一目标文件的源码，基本上是一个源码文件加上包含的头文件。
// 如果某个编译单元内的某个non-local static对象的初始化，
// 使用了其他编译单元内的non-local static对象，用到的对象可能尚未被初始化。
// 对定义于不同编译单元内的non-local static对象，
// C++并未明确定义它们的初始化次序，因为决定次序非常困难，根本无解。
// 消除这个问题需要做的是，将每个non-local static对象搬到自己的专属函数内，
// 这些函数返回一个引用指向所包含的对象，然后用户通过函数而不直接接触这些对象。
// 换句话说，non-local static被local static替换了，单例常见的实现手法。
// C++保证，函数内的local static对象会在被调用期间，首次遇上该对象的定义时被初始化。
// 使用函数调用代替访问non-local static对象，就可以保证获取已经初始化的对象。
// 更棒的是，如果从未调用non-local static对象的仿真函数，就不会构造和析构。
// 真正的non-local static对象可没这等便宜。

class Base {
 public:
  Base() { std::cout << "Base" << std::endl; }
};
class Arg1 {
 public:
  Arg1() { std::cout << "Arg1" << std::endl; }
  Arg1(const Arg1& rhs) { std::cout << "Arg1 copy" << std::endl; }
};
class Arg2 {
 public:
  Arg2() { std::cout << "Arg2" << std::endl; }
  Arg2(const Arg2& rhs) { std::cout << "Arg2 copy" << std::endl; }
};
class Arg3 {
 public:
  Arg3() { std::cout << "Arg3" << std::endl; }
  Arg3& operator=(const Arg3& rhs) {
    std::cout << "Arg3 assignment" << std::endl;
  }
};
class GlobalStatic {
 public:
  GlobalStatic() { std::cout << "GlobalStatic" << std::endl; }
};
class LocalStatic {
 public:
  LocalStatic() { std::cout << "LocalStatic" << std::endl; }
};
class Derived : public Base {
 public:
  Derived(Arg2 aa2, Arg1 aa1, Arg3 aa3) : a2(aa2), a1(aa1) {
    std::cout << "Derived" << std::endl;
    a3 = aa3;
  }

 private:
  Arg1 a1;
  Arg2 a2;
  Arg3 a3;
};
static GlobalStatic gs;
LocalStatic& localStatic() {
  static LocalStatic ls;
  return ls;
}

void func() {
  // GlobalStatic

  Arg1 a1;  // Arg1
  Arg2 a2;  // Arg2
  Arg3 a3;  // Arg3

  Derived d(a2, a1, a3);
  // Arg1 copy
  // Arg2 copy
  // Base
  // Arg1 copy
  // Arg2 copy
  // Arg3
  // Derived
  // Arg3 assignment

  std::cout << "before call localStatic" << std::endl;
  // before call localStatic
  localStatic();
  // LocalStatic
}
}  // namespace n2

// 请记住：
// 1.为内置类型对象进行手工初始化，因为C++不保证初始化它们；
// 2.构造函数最好使用成员初始化列表，而不要在构造函数本体内使用赋值操作；
// 3.初始化列表的成员变量，其排列次序应该和在class中的声明次序相同；
// 4.为免除跨编译单元的初始化次序问题，
//   应该用local static对象替换non-local static对象。

int main(int argc, char* argv[]) {
  if (argc < 2) {
    std::cout << argv[0] << " i [0 - 0]" << std::endl;
    return 0;
  }
  int type = argv[1][0] - '0';
  switch (type) {
    case 0:
      n2::func();
      break;
    default:
      std::cout << "invalid type" << std::endl;
      break;
  }

  return 0;
}