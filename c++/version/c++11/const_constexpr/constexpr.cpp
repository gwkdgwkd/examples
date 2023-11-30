#include <iostream>

// 所谓常量表达式，指的就是由多个（≥1）常量组成的表达式。
// 换句话说，如果表达式中的成员都是常量，那么该表达式就是一个常量表达式。
// 这也意味着，常量表达式一旦确定，其值将无法修改。
// 常量表达式的应用场景还有很多，比如匿名枚举、case表达式等。

// C++程序的执行过程大致要经历编译、链接、运行这3个阶段。
// 值得一提的是，常量表达式和非常量表达式的计算时机不同：
// 1.非常量表达式只能在程序运行阶段计算出结果；
// 2.常量表达式的计算往往发生在程序的编译阶段，这可以极大提高程序的执行效率，
//   因为表达式只在编译阶段计算一次，每次程序运行时都无需再计算。

// 对于用C++编写的程序，性能往往是永恒的追求。
// 那么如何才能判定一个表达式是否为常量表达式，从而获得在编译阶段计算的特权呢？
// 除了人为判定外，C++11标准还提供有constexpr关键字，
// 使指定的常量表达式获得在程序编译阶段计算能力，而不必等到程序运行阶段。
// C++11中，constexpr可用于普通变量、函数（包括模板函数）以及类的构造函数。
// 注意，constexpr修饰的表达式获得在编译阶段计算出结果的能力，
// 并不代表一定会在程序编译阶段被执行，具体的计算时机还是编译器说了算。

// 当常量表达式中包含浮点数时，考虑到程序编译和运行所在的系统环境可能不同，
// 常量表达式在编译阶段和运行阶段计算出的结果精度很可能会受到影响，
// 因此C++11标准规定，浮点常量表达式在编译阶段计算的精度，
// 要至少等于（或者高于）运行阶段计算出的精度。

namespace n1 {
// constexpr修饰普通变量
void func() {
  // 使用constexpr修改普通变量时，变量必须初始化且初始值必须是常量表达式：
  constexpr int num1 = 1 + 2 + 3;
  int a1[num1] = {1, 2, 3, 4, 5, 6};

  // constexpr用const关键字替换也可以正常执行，
  // 这是因为定义同时满足是const常量且使用常量表达式为其初始化这2个条件，
  // 由此编译器会认定num2是一个常量表达式。
  const int num2 = 1 + 2 + 3;
  int a2[num2] = {1, 2, 3, 4, 5, 6};

  // int i = 5;
  // constexpr int num3 = i;  // 报错
}
}  // namespace n1

namespace n2 {
// constexpr还可以用于修饰函数的返回值，这样的函数又称为常量表达式函数。

// 注意，constexpr并非可以修饰任意函数的返回值。
// 换句话说，一个函数要想成为常量表达式函数，必须满足如下4个条件：
// 1.整个函数的函数体中，除了可以包含using、typedef以及static_assert外，
//   只能包含一条return返回语句；
// 2.该函数必须有返回值，即函数的返回值类型不能是void；
// 3.函数在使用之前，必须有对应的定义语句：
//   普通函数在调用时，只需要保证调用位置之前有相应的声明即可；
//   而常量表达式函数则不同，调用位置之前必须要有该函数的定义，否则会编译失败。
// 4.return返回的必须是常量表达式，如果想在程序编译阶段获得函数返回的常量，
//   则该函数的return语句中就不能包含程序运行阶段才能确定值的变量。
//   注意，在常量表达式函数的return语句中，不能包含赋值的操作。
//   另外，用constexpr修饰函数时，函数本身也是支持递归的。

constexpr int f(int x) {
  using namespace std;
  typedef int a;
  static_assert(5 > 0);

  return 1 + 2 + x;
}

void func() {
  std::cout << f(5) << std::endl;  // 8
}
}  // namespace n2

namespace n3 {
// constexpr修饰类的构造函数
// 对于C++内置类型的数据，可以直接用constexpr修饰；
// 但如果是自定义的数据类型（struct或class），直接用constexpr修饰是不行的。
// constexpr struct myType {
//   const char *name;
//   int age;
// };
// ‘constexpr’ cannot be used for type declarations

struct myType {
  // 定义可产生常量的类型时，正确的做法是在该类型的内部添加一个常量构造函数：
  constexpr myType(char *name, int age)
      : name(name),
        age(age){
 // constexpr修饰类的构造函数时，要求该构造函数的函数体必须为空，
            // 且采用初始化列表的方式为各个成员赋值时，必须使用常量表达式。
        };
  // constexpr可用于修饰函数，而类中的成员方法就是位于类这命名空间中的函数，
  // 所以constexpr也可以修饰类中的成员函数，只不过必须满足前面提的4个条件：
  constexpr const char *getname() { return name; }
  constexpr int getage() { return age; }
  // C++11标准中，不支持用constexpr修饰带有virtual的成员方法。
  const char *name;
  int age;
};

void func() {
  constexpr struct myType mt { "zhangsan", 10 };
  constexpr const char *name = mt.getname();
  std::cout << name << std::endl;  // zhangsan
  constexpr int age = mt.getage();
  std::cout << age << std::endl;                       // 10
  std::cout << mt.name << " " << mt.age << std::endl;  // zhangsan 10

  // int i = 20;
  // constexpr struct myType mt1 { "test", i };
}
}  // namespace n3

namespace n4 {
struct myType {
  const char *name;
  int age;
};
template <typename T>
constexpr T f(T t) {
  return t;
}

void func() {
  struct myType stu {
    "zhangsan", 10
  };
  // 当模板函数中以自定义结构体myType类型进行实例化时，
  // 由于该结构体中没有定义常量表达式构造函数，
  // 所以实例化后的函数不是常量表达式函数，此时constexpr是无效的：
  struct myType ret = f(stu);
  std::cout << ret.name << " " << ret.age << std::endl;  // zhangsan 10
  ret.age = 8;  // constexpr是无效的
  std::cout << ret.name << " " << ret.age << std::endl;  // zhangsan 8

  // 模板函数的类型T为int类型，实例化后的函数符合常量表达式函数的要求，
  // 所以该函数的返回值就是一个常量表达式。
  constexpr int ret1 = f(10);
  std::cout << ret1 << std::endl;  // 10
  // ret1 = 6;  // assignment of read-only variable ‘ret1’
}
}  // namespace n4

int main(int argc, char *argv[]) {
  if (argc < 2) {
    std::cout << argv[0] << " i [0 - 3]" << std::endl;
    return 0;
  }
  int type = argv[1][0] - '0';
  switch (type) {
    case 0:
      n1::func();
      break;
    case 1:
      n2::func();
      break;
    case 2:
      n3::func();
      break;
    case 3:
      n4::func();
      break;
    default:
      std::cout << "invalid type" << std::endl;
      break;
  }

  return 0;
}
