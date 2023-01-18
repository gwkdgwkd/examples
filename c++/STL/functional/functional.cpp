#include <algorithm>
#include <deque>
#include <functional>
#include <iostream>
#include <iterator>
#include <set>
#include <string>
#include <vector>

// 仿函数（Functor）又称为函数对象（Function Object），
// 是一个能行使函数功能的类。
// 仿函数是定义了一个含有operator()成员函数的对象，
// 可以视为一个一般的函数，
// 只不过这个函数功能是在一个类中的运算符operator()中实现，
// 是一个函数对象，它将函数作为参数传递的方式来使用。

// 在标准库中，函数对象被广泛地使用以获得弹性，
// 标准库中的很多算法都可以使用函数对象或者函数来作为自定的回调行为。

// STL中也大量涉及到仿函数，有时仿函数的使用是为了函数拥有类的性质，
// 以达到安全传递函数指针、依据函数生成对象、甚至是让函数之间有继承关系、
// 对函数进行运算和操作的效果。

// 比如STL中的容器set就使用了仿函数less，而less继承的binary_function，
// 就可以看作是对于一类函数的总体声明，这是函数做不到的。

// 仿函数还给出了static的替代方案，函数内的静态变量可以改成类的私有成员，
// 这样可以明确地在析构函数中清除所用的内容，
// 如果用到了指针，那么这个是不错的选择。
// 有人说这样的类已经不是仿函数了，但其实，封装后从外界观察，
// 可以明显地发现，它依然有函数的性质。

// 仿函数优点：
// 如果可以用仿函数实现，那么你应该用仿函数，而不要用CallBack。
// 原因在于：
// 1.仿函数可以不带痕迹地传递上下文参数；
// 2.而CallBack技术通常使用一个额外的void*参数传递，
//   这也是多数人认为CallBack技术丑陋的原因。
// 通常来说，仿函数比普通函数快，尤其是在大量调用中。

// 仿函数通常有下面四个作用：
// 1.作为排序规则，在一些特殊情况下排序是不能直接使用运算符<或者>时，
//   可以使用仿函数；
// 2.作为判别式使用，即返回值为bool类型；
// 3.同时拥有多种内部状态，比如返回一个值得同时并累加；
// 4.作为算法for_each的返回值使用。

// C++98标准库中预定义的函数对象：
//  plus          加法函数对象类
//  minus         减法函数对象类
//  multiplies 	  乘法函数对象类
//  divides 	  分割函数对象类
//  modulus 	  模量函数对象类
//  negate 	      负函数对象类
//  equal_to      用于相等性比较的函数对象类
//  not_equal_to  非相等比较的函数对象类
//  greater 	  函数对象类，用于大于不等式的比较
//  less 	      函数对象类，用于小于不等式的比较
//  greater_equal 函数对象类，用于大于或等于比较
//  less_equal 	  函数对象类，用于小于或等于比较
//  logical_and   逻辑AND函数对象类
//  logical_or    逻辑或函数对象类
//  logical_not   逻辑非函数对象类
// C++11标准库中预定义的函数对象：
//  binary_negateNegate        二进制函数对象类（类模板）
//  function                   函数包装器（类模板）
//  reference_wrapper          参考包装（类模板）
//  unary_negateNegate         一元函数对象类（类模板）
//  bit_and                    按位AND函数对象类（类模板）
//  bit_or                     按位或函数对象类（类模板）
//  bit_xor                    按位XOR函数对象类（类模板）
//  dividesDivision            函数对象类（类模板）
//  equal_toFunction           用于相等性比较的对象类（类模板）
//  greaterFunction            大于不平等比较的对象类（类模板）
//  greater_equalFunction      大于或等于比较的对象类（类模板）
//  lessFunction               小于不等式比较的对象类（类模板）
//  less_equalFunction         小于或等于比较的对象类（类模板）
//  logical_andLogical         AND函数对象类（类模板）
//  logical_notLogical         NOT函数对象类（类模板）
//  logical_orLogical          OR函数对象类（类模板）
//  minusSubtraction           函数对象类（类模板）
//  modulusModulus 	           函数对象类（类模板）
//  multipliesMultiplication   函数对象类（类模板）
//  negateNegative 	           函数对象类（类模板）
//  not_equal_toFunction       非相等比较的对象类（类模板）
//  plusAddition               函数对象类（类模板）
//  bad_function_call          错误调用引发异常（类）
//  hash                       默认哈希函数对象类（类模板）
//  is_bind_expression         是绑定表达式（类模板）
//  is_placeholder             是占位符（类模板）

namespace n1 {
// C++11中，经过std::function来实现对C++中各类可调用实体的封装，
// 造成一个新的可调用的std::function对象：
int normal(int i) {
  std::cout << "normal func:";
  return i;
}

auto lambda = [](int i) {
  std::cout << "lambda:";
  return i;
};

class Functor {
 public:
  int operator()(int i) {
    std::cout << "Functor:";
    return i;
  }
};

class C {
 public:
  static int static_func(int i) {
    std::cout << "C::static_func:";
    return i;
  }
  int non_static_func(int i) {
    std::cout << "C::non_static_func:";
    return i;
  }
};

std::function<int(int)> func;
void test() {
  func = normal;
  std::cout << func(1) << std::endl;

  func = lambda;
  std::cout << func(2) << std::endl;

  Functor f;
  func = f;
  std::cout << func(3) << std::endl;

  func = C::static_func;
  std::cout << func(4) << std::endl;

  C c;
  func = std::bind(&C::non_static_func, c, std::placeholders::_1);
  std::cout << func(5) << std::endl;

  // normal func:1
  // lambda:2
  // Functor:3
  // C::static_func:4
  // C::non_static_func:5
}
}  // namespace n1

namespace n2 {
// 既然用函数对象与调用普通函数有相同的效果，
// 为什么还要搞这么麻烦定义一个类来使用函数对象？
// 主要在于函数对象有以下的优势：
// 1.函数对象可以有自己的状态。
//   可以在类中定义状态变量，这样一个函数对象在多次调用中可以共享这个状态。
//   但是函数调用没这种优势，除非使用全局变量或局部静态变量来保存状态。
class C {
  int i_;

 public:
  C(int i = 0) : i_(i) {}
  int operator()() { return i_++; }
};
void func1() {
  std::vector<int> v;
  std::generate_n(std::back_inserter(v), 10, C(3));
  std::copy(v.begin(), v.end(), std::ostream_iterator<int>(std::cout, " "));
  std::cout << std::endl;  // 3 4 5 6 7 8 9 10 11 12
}
void func2() {
  std::deque<int> dq;
  std::generate_n(std::front_inserter(dq), 5, C(20));
  std::copy(dq.begin(), dq.end(), std::ostream_iterator<int>(std::cout, " "));
  std::cout << std::endl;  // 24 23 22 21 20
}
// 2.函数对象有自己特有的类型，而普通函数无类型可言。
//   这种特性对于使用C++标准库来说是至关重要的。
//   这样在使用STL中的函数时，可以传递相应的类型作为参数来实例化相应的模板，
//   从而实现自己定义的规则，比如容器的排序规则。
class StringSort {
 public:
  bool operator()(const std::string &s1, const std::string &s2) {
    return s1.size() < s2.size();
  };
};
void func3() {
  std::set<std::string, StringSort> s{"222", "1111", "44", "5"};
  std::copy(s.begin(), s.end(),
            std::ostream_iterator<std::string>(std::cout, "\n"));
  // 5
  // 44
  // 222
  // 1111
}
void test() {
  func1();
  func2();
  func3();
}
}  // namespace n2

namespace n3 {
// 被std::function包装的可调用对象被称为std::function的target，
// std::function可以存储、复制、调用target。
// 如果一个std::function不包含target，
// 调用时会抛出std::bad_function_call异常。
void test() {
  std::function<void()> bad;
  try {
    bad();
  } catch (const std::bad_function_call &b) {
    std::cout << b.what() << std::endl;
  }
  // bad_function_call
}
}  // namespace n3

namespace n4 {
// std::function对象仅能和nullptr做比较，
// 因为operator==和operator!=只有该版本，
// 如果operator bool()为true，其和nullptr相等。
void test() {
  std::cout << std::boolalpha;

  std::function<void()> null;
  std::cout << null.operator bool() << std::endl;  // false

  std::function<void()> notnull = [] {};
  std::cout << notnull.operator bool() << std::endl;  // true
}
}  // namespace n4

namespace n5 {
// 谓词predicate其实就是一个判断式，
// 是指普通函数或重载的operator()返回值是bool类型的仿函数。
// 如果operator()接受一个参数，那么叫做一元谓词；
// 如果接受两个参数，那么叫做二元谓词。

int a = 9;
int b = 6;
void func1() {
  std::greater<int> g;
  std::cout << a << " > " << b << " : " << g(a, b) << std::endl;
  std::cout << b << " > " << a << " : " << g(b, a) << std::endl;
  std::cout << a << " > " << a << " : " << g(a, a) << std::endl;
  // 9 > 6 : true
  // 6 > 9 : false
  // 9 > 9 : false
}
void func2() {
  std::greater_equal<int> ge;
  std::cout << a << " >= " << b << " : " << ge(a, b) << std::endl;
  std::cout << b << " >= " << a << " : " << ge(b, a) << std::endl;
  std::cout << a << " >= " << a << " : " << ge(a, a) << std::endl;
  // 9 >= 6 : true
  // 6 >= 9 : false
  // 9 >= 9 : true
}
void func3() {
  std::less<int> l;
  std::cout << a << " < " << b << " : " << l(a, b) << std::endl;
  std::cout << b << " < " << a << " : " << l(b, a) << std::endl;
  std::cout << a << " < " << a << " : " << l(a, a) << std::endl;
  // 9 < 6 : false
  // 6 < 9 : true
  // 9 < 9 : false
}
void func4() {
  std::less_equal<int> le;
  std::cout << a << " <= " << b << " : " << le(a, b) << std::endl;
  std::cout << b << " <= " << a << " : " << le(b, a) << std::endl;
  std::cout << a << " <= " << a << " : " << le(a, a) << std::endl;
  // 9 <= 6 : false
  // 6 <= 9 : true
  // 9 <= 9 : true
}
void func5() {
  std::equal_to<int> e;
  std::cout << a << " == " << b << " : " << e(a, b) << std::endl;
  std::cout << b << " == " << a << " : " << e(b, a) << std::endl;
  std::cout << a << " == " << a << " : " << e(a, a) << std::endl;
  // 9 == 6 : false
  // 6 == 9 : false
  // 9 == 9 : true
}
void func6() {
  std::not_equal_to<int> ne;
  std::cout << a << " != " << b << " : " << ne(a, b) << std::endl;
  std::cout << b << " != " << a << " : " << ne(b, a) << std::endl;
  std::cout << a << " != " << a << " : " << ne(a, a) << std::endl;
  // 9 != 6 : true
  // 6 != 9 : true
  // 9 != 9 : false
}
void test() {
  std::cout << std::boolalpha;
  func1();
  func2();
  func3();
  func4();
  func5();
  func6();
}
}  // namespace n5

namespace n6 {
// 自定义一元谓词：
bool Greater20(int i) { return i > 20; }
class Greater50 {  // 仿函数，应该是这个最好吧
 public:
  bool operator()(int i) { return i > 50; }
};
class G {
 public:
  static bool Greater10(int i) { return i > 10; }
  bool Greater40(int i) { return i > 40; }
};

std::vector<int> v{10, 20, 30, 40, 50};
void func1() {
  // &G::Greater10也行
  auto it = std::find_if(v.begin(), v.end(), G::Greater10);
  if (it != v.end()) {
    std::cout << "find : " << *it << std::endl;
  }
  // find : 20
}
void func2() {
  auto it = std::find_if(v.begin(), v.end(), Greater20);
  if (it != v.end()) {
    std::cout << "find : " << *it << std::endl;
  }
  // find : 30
}
void func3() {
  auto it = std::find_if(v.begin(), v.end(), [](int i) { return i > 30; });
  if (it != v.end()) {
    std::cout << "find : " << *it << std::endl;
  }
  // find : 40
}
void func4() {
  G g;
  // G::Greater40不行
  auto it = std::find_if(v.begin(), v.end(),
                         std::bind(&G::Greater40, g, std::placeholders::_1));
  if (it != v.end()) {
    std::cout << "find : " << *it << std::endl;
  }
  // find : 50
}
void func5() {
  auto it = std::find_if(v.begin(), v.end(), Greater50());
  if (it == v.end()) {
    std::cout << "not find!" << std::endl;
  }
  // not find!
}

void test() {
  func1();
  func2();
  func3();
  func4();
  func5();
}
}  // namespace n6

namespace n7 {
// 自定义二元谓词：
bool Greater(int a, int b) { return a > b; }
class Less {
 public:
  bool operator()(int a, int b) { return a < b; }
};
void test() {
  std::vector<int> v{40, 30, 10, 50, 20};
  auto print = [&v]() {
    std::for_each(v.begin(), v.end(), [](int i) { std::cout << i << " "; });
    std::cout << std::endl;
  };

  print();  // 40 30 10 50 20
  std::sort(v.begin(), v.end(), Greater);
  print();  // 50 40 30 20 10
  std::sort(v.begin(), v.end(), Less());
  print();  // 10 20 30 40 50
}
}  // namespace n7

int main(int argc, char *argv[]) {
  if (argc < 2) {
    std::cout << argv[0] << " i [0 - 1]" << std::endl;
    return 0;
  }
  int type = argv[1][0] - '0';
  switch (type) {
    case 0:
      n1::test();
      break;
    case 1:
      n2::test();
      break;
    case 2:
      n3::test();
      break;
    case 3:
      n4::test();
      break;
    case 4:
      n5::test();
      break;
    case 5:
      n6::test();
      break;
    case 6:
      n7::test();
      break;
    default:
      std::cout << "invalid type" << std::endl;
      break;
  }

  return 0;
}