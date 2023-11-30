#include <iostream>
#include <functional>

// C++11标准库中预定义的函数对象：
//  binary_negateNegate        二进制函数对象类
//  function                   函数包装器
//  reference_wrapper          参考包装
//  unary_negateNegate         一元函数对象类
//  bit_and                    按位AND函数对象类
//  bit_or                     按位或函数对象类
//  bit_xor                    按位XOR函数对象类
//  dividesDivision            函数对象类
//  equal_toFunction           用于相等性比较的对象类
//  greaterFunction            大于不平等比较的对象类
//  greater_equalFunction      大于或等于比较的对象类
//  lessFunction               小于不等式比较的对象类
//  less_equalFunction         小于或等于比较的对象类
//  logical_andLogical         AND函数对象类
//  logical_notLogical         NOT函数对象类
//  logical_orLogical          OR函数对象类
//  minusSubtraction           函数对象类
//  modulusModulus 	           函数对象类
//  multipliesMultiplication   函数对象类
//  negateNegative 	           函数对象类
//  not_equal_toFunction       非相等比较的对象类
//  plusAddition               函数对象类
//  bad_function_call          错误调用引发异常（类）
//  hash                       默认哈希函数对象类
//  is_bind_expression         是绑定表达式
//  is_placeholder             是占位符

// std::function和函数指针的优劣：
// 1.std::function允许任意可转换的函数；
// 2.std::function可以和其他参数或函数绑定对象库配合使用；
// 3.当空函数结构调用的时候std::function的行为可以预见，类型安全；
// 4.函数指针更小；
// 5.函数指针更快，std::function在析构时可能会释放函数对象；
// 6.函数指针对C语言库的向后兼容性更好；
// 7.函数指针的错误信息更容易理解。

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

std::function<int(int)> functor;
void func() {
  functor = normal;
  std::cout << functor(1) << std::endl;

  functor = lambda;
  std::cout << functor(2) << std::endl;

  Functor f;
  functor = f;
  std::cout << functor(3) << std::endl;

  functor = C::static_func;
  std::cout << functor(4) << std::endl;

  C c;
  functor = std::bind(&C::non_static_func, c, std::placeholders::_1);
  std::cout << functor(5) << std::endl;

  // normal func:1
  // lambda:2
  // Functor:3
  // C::static_func:4
  // C::non_static_func:5
}
}  // namespace n1

namespace n2 {
// 被std::function包装的可调用对象被称为std::function的target，
// std::function可以存储、复制、调用target。
// 如果std::function不包含target，调用会抛出std::bad_function_call异常。
void func() {
  std::function<void()> bad;
  try {
    bad();
  } catch (const std::bad_function_call &b) {
    std::cout << b.what() << std::endl;
  }
  // bad_function_call
}
}  // namespace n2

namespace n3 {
// std::function仅能和nullptr做比较，因为operator==和operator!=只有该版本，
// 如果operator bool()为false，其和nullptr相等。
void func() {
  std::cout << std::boolalpha;

  std::function<void()> null;
  std::cout << null.operator bool() << std::endl;  // false

  std::function<void()> notnull = [] {};
  std::cout << notnull.operator bool() << std::endl;  // true
}
}  // namespace n3

int main(int argc, char *argv[]) {
  if (argc < 2) {
    std::cout << argv[0] << " i [0 - 2]" << std::endl;
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
    default:
      std::cout << "invalid type" << std::endl;
      break;
  }

  return 0;
}