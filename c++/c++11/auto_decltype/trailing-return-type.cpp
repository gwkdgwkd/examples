#include <iostream>

// 在泛型编程中，可能需要通过参数的运算来得到返回值的类型。
template <typename R, typename T, typename U> R add1(T t, U u) { return t + u; }

// 在add函数的定义上能不能直接通过decltype拿到返回值呢？
// template <typename T, typename U>
// decltype(t + u) add2(T t, U u) { // error: t、u尚未定义
//   return t + u;
// }
// 直接像上面这样写是编译不过的。因为t、u在参数列表中，而C++的返回值是前置语法，在返回值定义的时候参数变量还不存在。

// 可行的写法如下：
template <typename T, typename U> decltype(T() + U()) add3(T t, U u) {
  return t + u;
}
// 考虑到T、U可能是没有无参构造函数的类，正确的写法应该是这样：
template <typename T, typename U>
decltype((*(T *)0) + (*(U *)0)) add4(T t, U u) {
  return t + u;
} // 虽然成功地使用decltype完成了返回值的推导，但写法过于晦涩，会大大增加decltype在返回值类型推导上的使用难度并降低代码的可读性。

// 在C++11中增加了返回类型后置（trailing-return-type，又称跟踪返回类型）语法，将decltype和auto结合起来完成返回值类型的推导。
template <typename T, typename U> auto add5(T t, U u) -> decltype(t + u) {
  return t + u;
}

// 如果说前一个例子中的add使用C++98/03的返回值写法还勉强可以完成，那么这个例子对于C++而言就是不可能完成的任务了。
int &foo(int &i);
float foo(float &f);
template <typename T> auto func(T &val) -> decltype(foo(val)) {
  return foo(val);
}

// 返回值类型后置语法，是为了解决函数返回值类型依赖于参数而导致难以确定返回值类型的问题。
// 有了这种语法以后，对返回值类型的推导就可以用清晰的方式（直接通过参数做运算）描述出来，而不需要像C++98/03那样使用晦涩难懂的写法。

int main() {
  int a = 1;
  float b = 2.0;

  // 我们并不关心a+b的类型是什么，因此，只需要通过decltype(a+b)直接得到返回值类型即可。
  auto c1 = add1<decltype(a + b)>(a, b);
  // 但是像上面这样使用十分不方便，因为外部其实并不知道参数之间应该如何运算，只有add函数才知道返回值应当如何推导。
  std::cout << c1 << std::endl;

  auto c3 = add3<decltype(a + b)>(a, b);
  std::cout << c3 << std::endl;

  auto c4 = add4<decltype(a + b)>(a, b);
  std::cout << c4 << std::endl;

  auto c5 = add5<decltype(a + b)>(a, b);
  std::cout << c5 << std::endl;

  return 0;
}

// 几种方式调用的方法都一样吗？没有别的写法么？