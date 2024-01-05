#include <array>
#include <iostream>

// 条款15：尽可能的使用constexpr

// 如果要给C++11颁一个最令人困惑新词奖，constexpr十有八九会折桂。
// 当用于对象上面，它本质上就是const的加强形式，但是当它用于函数上，意思就大不相同了。
// 有必要消除困惑，因为绝对会用它的，特别是当发现constexpr正合吾意的时候。

// 从概念上来说，constexpr表明一个值不仅仅是常量，还是编译期可知的。
// 这个表述并不全面，因为当constexpr被用于函数的时候，事情就有一些细微差别了。
// 不能假设constexpr函数的结果是const，也不能保证它们的（返回）值是在编译期可知的。

namespace n1 {
// constexpr对象实际上和const一样，它们是编译期（编译和链接）可知的。
// 编译期可知的值享有特权，它们可能被存放到只读存储空间中。
// 对于那些嵌入式系统的开发者，这个特性是相当重要的。
// 更广泛的应用是其值编译期可知的常量整数会出现在需要整型常量表达式的上下文中，包括：
// 1.括数组大小；
// 2.整数模板参数，包括std::array对象的长度；
// 3.枚举名的值；
// 4.对齐修饰符，等等。

// 如果想在这些上下文中使用变量，一定会希望为constexpr，因为编译器会确保是编译期可知的：
void func1() {
  int sz;  // non-constexpr变量
  // constexpr auto arraySize1 = sz;     // 错误，sz的值在编译期不可知
  // std::array<int, sz> data1;          // 错误，同上

  constexpr auto arraySize2 = 10;     // 没问题，10是编译期可知常量
  std::array<int, arraySize2> data2;  // 没问题, arraySize2是constexpr
}

// const不提供constexpr所能保证之事，因为const对象不需要在编译期初始化它的值：
void func2() {
  int sz;
  const auto arraySize = sz;  // 没问题，arraySize是sz的const复制
  // std::array<int, arraySize> data;  // 错误，arraySize值在编译期不可知
}

// 简而言之，所有constexpr对象都是const，但不是所有const对象都是constexpr。
// 如果想编译器保证一个变量有一个值，这个值可以放到那些需要编译期常量的上下文的地方，
// 需要的工具是constexpr而不是const。
}  // namespace n1

namespace n2 {
// 涉及到constexpr函数时，constexpr对象的使用情况就更有趣了：
// 1.如果实参是编译期常量，这些函数将产出编译期常量；
// 2.如果实参是运行时才能知道的值，它们就将产出运行时值。
// 这听起来就像你不知道它们要做什么一样，那么想是错误的，请这么看：
// 1.constexpr函数可以用于需求编译期常量的上下文，
//   如果传给constexpr函数的实参在编译期可知，那么结果将在编译期计算，
//   如果实参的值在编译期不知道，代码就会被拒绝；
// 2.当一个constexpr函数被一个或者多个编译期不可知值调用时，它就像普通函数一样，
//   运行时计算它的结果，这意味着不需要用于编译期和运行期的两个函数，constexpr全做了。

// 假设需要一个数据结构来存储一个实验的结果，而这个实验可能以各种方式进行。
// 实验期间风扇转速，温度等等都可能导致亮度值改变，亮度值可以是高，低，或者无。
// 如果有n个实验相关的环境条件，它们每一个都有三个状态，最终可以得到的组合有3n个。
// 储存所有实验结果的所有组合需要足够存放3n个值的数据结构。
// 假设每个结果都是int并且n是编译期已知的或者可以被计算出的，std::array是个合理的选择。
// 需要一个方法在编译期计算3n，C++标准库提供了std::pow，它的数学功能正是所需要的，
// 但是这里还有两个问题：
// 1.std::pow是为浮点类型设计的，需要整型结果；
// 2.std::pow不是constexpr，即不保证使用编译期可知值调用而得到编译期可知的结果，
//   所以不能用它作为std::array的大小。

// pow前面的constexpr不表明pow返回一个const值：
// 1.它只说了如果base和exp是编译期常量，pow的值可以被当成编译期常量使用；
// 2.如果base和/或exp不是编译期常量，pow结果将会在运行时计算。
// 这意味着pow不止可以用于像std::array的大小这种需要编译期常量的地方，它也可以用于运行时。

// 因为constexpr函数必须能在编译期值调用的时候返回编译期结果，
// 就必须对它的实现施加一些限制，这些限制在C++11和C++14标准间有所出入。

constexpr int pow(int base, int exp) noexcept {
#if __cplusplus == 201103L  // C++11
  // C++11中，constexpr函数的代码不超过一行语句：一个return。
  // 听起来很受限，但实际上有两个技巧可以扩展constexpr函数的表达能力：
  // 1.使用三元运算符?:来代替if-else语句；
  // 2.使用递归代替循环。
  return (exp == 0 ? 1 : base * pow(base, exp - 1));
  // 这样没问题，但是很难想象除了使用函数式语言的程序员外会觉得这样硬核的编程方式更好。
#elif __cplusplus == 201402L
  // 在C++14中，constexpr函数的限制变得非常宽松了，所以下面的函数实现成为了可能：
  auto result = 1;
  for (int i = 0; i < exp; ++i) {
    result *= base;
  }
  return result;
#endif
}

void func() {
  constexpr auto numConds = 5;
  std::array<int, pow(3, numConds)> results;
  std::cout << results.size() << std::endl;  // 243

  int base, exp;
  std::cin >> base;                          // 3
  std::cin >> exp;                           // 5
  std::cout << pow(base, exp) << std::endl;  // 243
}
}  // namespace n2

namespace n3 {
// constexpr函数限制为只能获取和返回字面值类型，这意味着哪些值的类型能在编译期决定。

class Point {
 public:
  // 在C++11中，除了void外的所有内置类型，以及一些用户定义类型都可以是字面值类型，
  // 因为构造函数和其他成员函数可能是constexpr：
  constexpr Point(double xVal = 0, double yVal = 0) noexcept
      : x(xVal), y(yVal) {}

  // xValue和yValue函数也能是constexpr，因为如果对编译期已知的Point对象调用getter，
  // 数据成员x和y的值也能在编译期知道：
  constexpr double xValue() const noexcept { return x; }
  constexpr double yValue() const noexcept { return y; }

#if __cplusplus == 201103L  // C++11
  // 在C++11中，有两个限制使得Point的成员函数setX和setY不能声明为constexpr：
  // 1.它们修改它们操作的对象的状态，并且在C++11中，constexpr成员函数是隐式的const；
  // 2.它们有void返回类型，void类型不是C++11中的字面值类型。
  void setX(double newX) noexcept { x = newX; }
  void setY(double newY) noexcept { y = newY; }
#elif __cplusplus == 201402L
  // C++11的两个限制在C++14中放开了，所以C++14中Point的setter也能声明为constexpr：
  constexpr void setX(double newX) noexcept { x = newX; }
  constexpr void setY(double newY) noexcept { y = newY; }
#endif

 private:
  double x, y;
};

// 可以写一个constexpr函数，里面调用Point的getter并初始化constexpr的对象：
constexpr Point midpoint(const Point& p1, const Point& p2) noexcept {
  return {(p1.xValue() + p2.xValue()) / 2, (p1.yValue() + p2.yValue()) / 2};
}

#if __cplusplus == 201402L
constexpr Point reflection(const Point& p) noexcept {
  Point result;
  result.setX(-p.xValue());
  result.setY(-p.yValue());
  return result;
}
#endif

void func() {
  // Point的构造函数声明为constexpr，因为如果传入的参数在编译期可知，
  // Point的数据成员也能在编译器可知：
  constexpr Point p1(9.4, 27.7);  // 没问题，constexpr构造函数会在编译期执行
  constexpr Point p2(28.8, 5.3);  // 也没问题

  constexpr auto mid = midpoint(p1, p2);  // 使用constexpr函数始化constexpr对象
  // mid对象通过调用构造函数，成员函数和非成员函数来进行初始化过程就能在只读内存中被创建出来，
  // 它也意味着可以在模板实参或者需要枚举名的值的表达式里面使用像mid.xValue()*10的表达式，
  // 因为Point::xValue返回double，mid.xValue()*10也是个double。
  // 浮点数类型不可被用于实例化模板或者说明枚举名的值，但是它们可以被用来作为表达式的一部分。
  // 比如，static_cast<int>(mid.xValue()*10)可以被用来实例化模板或者说明枚举名的值。
  // 它也意味着以前相对严格的编译期完成的工作和运行时完成的工作的界限变得模糊，
  // 一些运行时的计算过程能并入编译阶段，越多这样的代码并入，程序就越快，编译时间会变长。

#if __cplusplus == 201402L
  constexpr auto reflectedMid = reflection(mid);  // reflectedMid编译期可知
#endif
}
}  // namespace n3

// 本条款的建议是尽可能的使用constexpr，现在希望大家已经明白缘由：
// constexpr对象和constexpr函数可以使用的范围比non-constexpr对象和函数大得多。
// 使用constexpr关键字可以最大化你的对象和函数可以使用的场景。

// 还有个重要的需要注意的是constexpr是对象和函数接口的一部分。
// 加上constexpr相当于宣称能被用在C++要求常量表达式的地方。
// 如果声明一个对象或者函数是constexpr，客户端程序员就可能会在那些场景中使用它。
// 如果后面认为使用constexpr是一个错误并想移除它，可能造成大量客户端代码不能编译。
// 为了debug或者性能优化而添加I/O到一个函数中这样简单的动作可能就导致这样的问题，
// 因为I/O语句一般不被允许出现在constexpr函数里。
// 尽可能的使用constexpr表示需要长期坚持对某个对象或者函数施加这种限制。

// 要记住的东西：
// 1.constexpr对象是const，它被在编译期可知的值初始化；
// 2.当传递编译期可知的值时，constexpr函数可以产出编译期可知的结果；
// 3.constexpr对象和函数可以使用的范围比non-constexpr对象和函数要大；
// 4.constexpr是对象和函数接口的一部分。

int main(int argc, char* argv[]) {
  if (argc < 2) {
    std::cout << argv[0] << " i [0 - 1]" << std::endl;
    return 0;
  }
  int type = argv[1][0] - '0';
  switch (type) {
    case 0:
      n2::func();
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