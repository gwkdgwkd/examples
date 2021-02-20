#include <iostream>

using namespace std;

// 所谓常量表达式，指的就是由多个（≥1）常量组成的表达式。换句话说，如果表达式中的成员都是常量，那么该表达式就是一个常量表达式。这也意味着，常量表达式一旦确定，其值将无法修改。
// 常量表达式的应用场景还有很多，比如匿名枚举、switch-case结构中的case表达式等。

// C++程序的执行过程大致要经历编译、链接、运行这3个阶段。值得一提的是，常量表达式和非常量表达式的计算时机不同，非常量表达式只能在程序运行阶段计算出结果；
// 而常量表达式的计算往往发生在程序的编译阶段，这可以极大提高程序的执行效率，因为表达式只需要在编译阶段计算一次，节省了每次程序运行时都需要计算一次的时间。

// 对于用C++编写的程序，性能往往是永恒的追求。那么如何才能判定一个表达式是否为常量表达式，进而获得在编译阶段即可执行的“特权”呢？除了人为判定外，C++11标准还提供有constexpr关键字。
// constexpr关键字的功能是使指定的常量表达式获得在程序编译阶段计算出结果的能力，而不必等到程序运行阶段。C++11标准中，constexpr可用于修饰普通变量、函数（包括模板函数）以及类的构造函数。
// 注意，获得在编译阶段计算出结果的能力，并不代表constexpr修饰的表达式一定会在程序编译阶段被执行，具体的计算时机还是编译器说了算。

// 当常量表达式中包含浮点数时，考虑到程序编译和运行所在的系统环境可能不同，常量表达式在编译阶段和运行阶段计算出的结果精度很可能会受到影响，因此C++11标准规定，
// 浮点常量表达式在编译阶段计算的精度要至少等于（或者高于）运行阶段计算出的精度。

// constexpr修饰函数:
// constexpr还可以用于修饰函数的返回值，这样的函数又称为“常量表达式函数”。
// 注意，constexpr并非可以修改任意函数的返回值。换句话说，一个函数要想成为常量表达式函数，必须满足如下4个条件:
// 1)整个函数的函数体中，除了可以包含using指令、typedef语句以及static_assert断言外，只能包含一条return返回语句。
// 注意，这个函数是无法通过编译的，因为该函数的返回值用constexpr修饰，但函数内部包含多条语句。
// constexpr int display(int x) {
//   int ret = 1 + 2 + x;
//   return ret;
// }
constexpr int display(int x) {
  //可以添加 using 执行、typedef 语句以及 static_assert 断言
  return 1 + 2 + x;
}

// 2)该函数必须有返回值，即函数的返回值类型不能是void。
// constexpr void display() {}
// 像上面这样定义的返回值类型为void的函数，不属于常量表达式函数。原因很简单，因为通过类似的函数根本无法获得一个常量。

// 3)函数在使用之前，必须有对应的定义语句。函数的使用分为“声明”和“定义”两部分，普通的函数调用只需要提前写好该函数的声明部分即可（函数的定义
// 部分可以放在调用位置之后甚至其它文件中），但常量表达式函数在使用前，必须要有该函数的定义。
// 普通函数在调用时，只需要保证调用位置之前有相应的声明即可；而常量表达式函数则不同，调用位置之前必须要有该函数的定义，否则会导致程序编译失败。
int noconst_dis(int x);         // 普通函数的声明
constexpr int display3(int x);  // 常量表达式函数的声明
// 放在main后面也可以编译通过，和描述的不太一样 ???
constexpr int display3(int x) { return 1 + 2 + x; }  // 常量表达式函数的定义

// 4)return返回的表达式必须是常量表达式，举个例子：
int num = 3;
// constexpr int display4(int x) { return num + x; }
// 常量表达式函数的返回值必须是常量表达式的原因很简单，如果想在程序编译阶段获得某个函数返回的常量，则该函数的return语句中就不能包含程序运行阶段才能确定值的变量。
// 注意，在常量表达式函数的return语句中，不能包含赋值的操作。另外，用constexpr修改函数时，函数本身也是支持递归的。

// constexpr修饰类的构造函数:
// 对于C++内置类型的数据，可以直接用constexpr修饰，但如果是自定义的数据类型（用struct或者class实现），直接用constexpr修饰是不行的。
// ‘constexpr’ cannot be used for type declarations
// constexpr struct myType {
//   const char *name;
//   int age;
// };
// 当我们想自定义一个可产生常量的类型时，正确的做法是在该类型的内部添加一个常量构造函数。例如，修改上面的错误示例如下：
struct myType {
  constexpr myType(char *name, int age) : name(name), age(age){};
  // constexpr可用于修饰函数，而类中的成员方法完全可以看做是“位于类这个命名空间中的函数”，所以constexpr也可以修饰类中的成员函数，只不过此函数必须满足前面提到的4个条件。
  constexpr const char *getname() { return name; }
  constexpr int getage() { return age; }
  const char *name;
  int age;
};
// 注意，constexpr修饰类的构造函数时，要求该构造函数的函数体必须为空，且采用初始化列表的方式为各个成员赋值时，必须使用常量表达式。
// 注意，C++11标准中，不支持用constexpr修饰带有virtual的成员方法。

// constexpr修饰模板函数:
struct myType1 {
  const char *name;
  int age;
};
template <typename T>
constexpr T dispaly5(T t) {
  return t;
}

int main() {
  // 数组的长度就必须是一个常量表达式：
  int url1[10];     //正确
  int url2[6 + 4];  //正确
  int length = 6;
  int url3[length];  //错误，length是变量(现在也不会报错了)

  // constexpr修饰普通变量:
  // C++11标准中，定义变量时可以用constexpr修饰。使用constexpr修改普通变量时，变量必须经过初始化且初始值必须是一个常量表达式。
  constexpr int num = 1 + 2 + 3;
  int url4[num] = {1, 2, 3, 4, 5, 6};
  cout << url4[1] << endl;
  // constexpr用const关键字替换也可以正常执行，这是因为num的定义同时满足“num是const常量且使用常量表达式为其初始化”这2个条件，由此编译器会认定num是一个常量表达式。

  int a[display3(3)] = {1, 2, 3, 4};  // 调用常量表达式函数
  cout << a[2] << endl;
  cout << noconst_dis(3) << endl;  // 调用普通函数

  // the value of ‘num’ is not usable in a constant expression
  // int a[display4(3)] = {1, 2, 3, 4};

  constexpr struct myType mt { "zhangsan", 10 };
  constexpr const char *name = mt.getname();
  cout << name << endl;
  constexpr int age = mt.getage();
  cout << age << endl;
  cout << mt.name << " " << mt.age << endl;

  struct myType1 stu {
    "zhangsan", 10
  };
  // 当模板函数中以自定义结构体myType类型进行实例化时，由于该结构体中没有定义常量表达式构造函数，所以实例化后的函数不是常量表达式函数，此时constexpr是无效的；
  struct myType1 ret = dispaly5(stu);
  cout << ret.name << " " << ret.age << endl;
  // 模板函数的类型T为int类型，实例化后的函数符合常量表达式函数的要求，所以该函数的返回值就是一个常量表达式。
  constexpr int ret1 = dispaly5(10);
  cout << ret1 << endl;

  return 0;
}

int noconst_dis(int x) { return 1 + 2 + x; }  // 普通函数的定义