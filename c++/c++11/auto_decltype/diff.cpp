#include <iostream>
#include <vector>

namespace diff {
void func1() {
  int i = 5;

  // 语法格式的区别:
  // auto和decltype都是C++11新增的关键字，都用于自动类型推导，
  // 但是它们的语法格式是有区别的，如下所示：
  // auto varname = value;  // auto的语法格式
  auto i1 = i;
  std::cout << i1 << std::endl;  // 5
  // decltype(exp) varname [= value];  // decltype的语法格式
  // 其中，varname表示变量名，value表示赋给变量的值，exp表示一个表达式，方括号[]表示可有可无。
  decltype(i) i2 = 4;
  decltype(i) i3;
  std::cout << i2 << std::endl;  // 4
  std::cout << i3 << std::endl;  // 1

  // auto和decltype都会自动推导出变量varname的类型：
  // auto根据=右边的初始值value推导出变量的类型；
  // decltype根据exp表达式推导出变量的类型，跟=右边的value没有关系。
  // 另外，auto要求变量必须初始化，也就是在定义变量的同时必须给它赋值；
  // 而decltype不要求，初始化与否都不影响变量的类型。
  // 这很容易理解，因为auto是根据变量的初始值来推导出变量类型的，
  // 如果不初始化，变量的类型也就无法推导了。
  // auto将变量的类型和初始值绑定在一起，而decltype将变量的类型和初始值分开；
  // 虽然auto的书写更加简洁，但decltype的使用更加灵活。
}

void func2() {
  // 对cv限定符的处理:
  // cv限定符是const和volatile关键字的统称：
  // const关键字用来表示数据是只读的，也就是不能被修改；
  // volatile和const是相反的，它用来表示数据是可变的、易变的，
  // 目的是不让CPU将数据缓存到寄存器，而是从原始的内存中读取。

  // 在推导变量类型时，auto和decltype对cv限制符的处理是不一样的。
  // 1.decltype会保留cv限定符;
  // 2.auto有可能会去掉cv限定符，以下是auto关键字对cv限定符的推导规则：
  //   (1).如果表达式的类型不是指针或者引用，auto会把cv限定符直接抛弃，
  //       推导成non-const或者non-volatile类型；
  //   (2).如果表达式的类型是指针或者引用，auto将保留cv限定符。

  // 非指针非引用类型
  const int n1 = 0;
  auto n2 = 10;
  // n2赋值成功，说明不带const，也就是const被auto抛弃了，这验证了auto的第一条推导规则
  n2 = 99;  // 非const变量，可以赋值
  decltype(n1) n3 = 20;
  // n3 = 5; // const变量，不能赋值。说明decltype不会去掉表达式的const属性。

  // 指针类型
  const int *p1 = &n1;
  auto p2 = p1;
  // p2赋值失败，说明是带const的，也就是const没有被auto抛弃，这验证了auto的第二条推导规则。
  // *p2 = 66; // 指向const变量的指针，不能修改指向的值
  decltype(p1) p3 = p1;
  // *p3 = 19; // 指向const变量的指针，不能修改指向的值。decltype不会去掉const属性。

  // 引用类型
  const int &r1 = n1;
  auto r2 = r1;
  // r2赋值失败，说明是带const的，也就是说const没有被auto抛弃，这验证了auto的第二条规则。
  // r2 = 5;  // const引用，不能修改
  decltype(r1) r3 = r1;
  // r3 = 6;  // const引用，不能修改。说明decltype不会去掉表达式的const属性。
}

void func3() {
  // 对引用的处理:
  // 当表达式的类型为引用时，auto和decltype的推导规则也不一样；
  // decltype会保留引用类型，而auto会抛弃引用类型，直接推导出它的原始类型。
  int n = 10;
  int &r1 = n;

  auto r2 = r1;
  r2 = 20;
  // 给r2赋值并没有改变n的值，这说明r2没有指向n，而是自立门户，单独拥有了一块内存，
  // 这就证明r2不再是引用类型，它的引用类型被auto抛弃了。
  std::cout << n << ", " << r1 << ", " << r2 << std::endl;  // 10, 10, 20

  decltype(r1) r3 = n;
  r3 = 99;
  // 给r3赋值，n的值也跟着改变了，这说明r3仍然指向n，它的引用类型被decltype保留了。
  std::cout << n << ", " << r1 << ", " << r3 << std::endl;  // 99, 99, 99
}

void func4() {
  // 总结:
  // auto虽然在书写格式上比decltype简单，但是它的推导规则复杂，有时候会改变表达式的原始类型；
  // 而decltype比较纯粹，它一般会坚持保留原始表达式的任何类型，让推导的结果更加原汁原味。

  // 从代码是否健壮的角度考虑，推荐使用decltype，它没有那么多是非；
  // 但是decltype总是显得比较麻烦，尤其是当表达式比较复杂时，例如：
  std::vector<int> nums;
  decltype(nums.begin()) it1 = nums.begin();
  // 而如果使用auto就会清爽很多：
  auto it2 = nums.begin();

  // 在实际开发中人们仍然喜欢使用auto关键字，因为它用起来简单直观，更符合人们的审美。
  // 如果你的表达式类型不复杂，还是推荐使用auto关键字，优雅的代码总是叫人赏心悦目，沉浸其中。
}

void testDiff() {
  func1();
  func2();
  func3();
  func4();
}
}  // namespace diff

namespace trailingreturntype {
// 在泛型编程中，可能需要通过参数的运算来得到返回值的类型。

template <typename R, typename T, typename U>
R add1(T t, U u) {
  return t + u;
}
void func1() {
  int a = 1;
  float b = 2.0;

  // 并不关心a+b的类型是什么，因此，只需要通过decltype(a+b)直接得到返回值类型即可。
  // 但是这样使用十分不方便，因为外部其实并不知道参数之间应该如何运算，
  // 只有add函数才知道返回值应当如何推导。
  auto c = add1<decltype(a + b)>(a, b);
  std::cout << c << " " << typeid(c).name() << std::endl;  // 3 f
}

// 在add函数的定义上能不能直接通过decltype拿到返回值呢？
// template <typename T, typename U>
// decltype(t + u) add2(T t, U u) { // error: t、u尚未定义
//   return t + u;
// }
// 直接像上面这样写是编译不过的。
// 因为t、u在参数列表中，而C++的返回值是前置语法，在返回值定义的时候参数变量还不存在。
// 可行的写法如下：
template <typename T, typename U>
decltype(T() + U()) add3(T t, U u) {
  return t + u;
}
void func2() {
  int a = 1;
  double b = 2.0;

  auto c = add3(a, b);
  std::cout << c << " " << typeid(c).name() << std::endl;  // 3 d
}

// 考虑到T、U可能是没有无参构造函数的类，正确的写法应该是这样：
template <typename T, typename U>
decltype((*(T *)0) + (*(U *)0)) add4(T t, U u) {
  return t + u;
}
// 虽然成功地使用decltype完成了返回值的推导，但写法过于晦涩，
// 会大大增加decltype在返回值类型推导上的使用难度并降低代码的可读性。
void func3() {
  int a = 1;
  int b = 3;

  auto c = add4(a, b);
  std::cout << c << " " << typeid(c).name() << std::endl;  // 4 i
}

// 在C++11中增加了返回类型后置（trailing-return-type，又称跟踪返回类型）语法，
// 将decltype和auto结合起来完成返回值类型的推导。
// 返回值类型后置语法，是为了解决函数返回值类型依赖于参数而导致难以确定返回值类型的问题。
// 有了这种语法以后，对返回值类型的推导就可以用清晰的方式（直接通过参数做运算）描述出来，
// 而不需要像C++98/03那样使用晦涩难懂的写法。
template <typename T, typename U>
auto add5(T t, U u) -> decltype(t + u) {
  return t + u;
}
void func4() {
  double a = 1;
  float b = 3;

  auto c = add5(a, b);
  std::cout << c << " " << typeid(c).name() << std::endl;  // 4 d
}

// 如果说add使用C++98/03的返回值写法还勉强可以完成，那下面的例子只能用C++11来实现了。
int &foo(int &i) { return i; }
float foo(float &f) { return f; }
template <typename T>
auto func(T &val) -> decltype(foo(val)) {
  return foo(val);
}
void func5() {
  int i = 6;
  std::cout << func<int>(i) << std::endl;  // 6
  float f = 2.45;
  std::cout << func<float>(f) << std::endl;  // 2.45
}

void testTrailingReturnType() {
  func1();
  func2();
  func3();
  func4();
  func5();
}
}  // namespace trailingreturntype

int main(int argc, char *argv[]) {
  if (argc < 2) {
    std::cout << argv[0] << " i [0 - 1]" << std::endl;
    return 0;
  }
  int type = argv[1][0] - '0';
  switch (type) {
    case 0:
      diff::testDiff();
      break;
    case 1:
      trailingreturntype::testTrailingReturnType();
      break;
    default:
      std::cout << "invalid type" << std::endl;
      break;
  }

  return 0;
}