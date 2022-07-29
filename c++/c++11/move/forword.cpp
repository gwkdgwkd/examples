#include <iostream>

// 完美转发指的是函数模板可以将自己的参数完美地转发给内部调用的其它函数。
// 所谓完美，即不仅能准确地转发参数的值，还能保证被转发参数的左、右值属性不变。
// C++11标准为C++引入右值引用语法的同时，还解决了一个C++98/03标准长期存在的短板，
// 即使用简单的方式即可在函数模板中实现参数的完美转发。

void otherdef(int& t) { std::cout << " lvalue" << std::endl; }
void otherdef(const int& t) { std::cout << " rvalue" << std::endl; }

namespace n1 {
// 显然，function()函数模板并没有实现完美转发:
// 1.参数t为非引用类型，这意味着在调用时，
//   实参将值传递给形参的过程就需要额外进行一次拷贝操作；
// 2.无论调用function()函数模板时传递给参数t的是左值还是右值，
//   对于函数内部的参数t来说，它有自己的名称，也可以获取它的存储地址，
//   因此它永远都是左值，也就是说，传递给otherdef()函数的参数t永远都是左值。
// 总之，无论从那个角度看，function()函数的定义都不完美。
template <typename T>
void function(T t) {
  otherdef(t);
}

void testN1() {
  function(5);  // lvalue
  int i = 4;
  function(i);             // lvalue
  function(std::move(i));  // lvalue
}
}  // namespace n1

namespace n2 {
// 完美转发这样严苛的参数传递机制，很常用吗？
// C++98/03标准中几乎不会用到，但C++11标准为C++引入了右值引用和移动语义，
// 因此很多场景中是否实现完美转发，
// 直接决定了该参数的传递过程使用的是拷贝语义还是移动语义。
// 事实上，C++98/03标准下的C++也可以实现完美转发，只是实现方式比较笨拙。
// C++98/03标准中只有左值引用，并且可以细分为非const引用和const引用。
// 其中，使用非const引用作为函数模板参数时，只能接收左值，无法接收右值；
// 而const左值引用既可以接收左值，也可以接收右值，但考虑到其const属性，
// 除非被调用函数的参数也是const属性，否则将无法直接传递。
// 这也就意味着，单独使用任何一种引用形式，可以实现转发，但无法保证完美。
// 因此如果使用C++98/03标准下的C++语言，可以采用函数模板重载的方式实现完美转发:
template <typename T>
void function(const T& t) {  // 接收右值参数
  otherdef(t);
}
template <typename T>
void function(T& t) {  // 接收左值参数
  otherdef(t);
}

void testN2() {
  function(5);  // rvalue
  int i = 4;
  function(i);             // lvalue
  function(std::move(i));  // rvalue
}
}  // namespace n2

namespace n3 {
// 显然，使用重载的模板函数实现完美转发也是有弊端的，
// 此实现方式仅适用于模板函数仅有少量参数的情况，
// 否则就需要编写大量的重载函数模板，造成代码的冗余。
// 为了方便用户更快速地实现完美转发，
// C++11标准中允许在函数模板中使用右值引用来实现完美转发。
// C++11标准中规定，通常情况下右值引用形式的参数只能接收右值，不能接收左值。
// 但对于函数模板中使用右值引用语法定义的参数来说，它不再遵守这一规定，
// 既可以接收右值，也可以接收左值，此时的右值引用又被称为万能引用。
// 在C++11标准中实现完美转发，只需要编写如下一个模板函数即可：
template <typename T>
void function(T&& t) {
  otherdef(t);
}

void testN3() {
  function(5);  // lvalue
  int i = 4;
  function(i);             // lvalue
  function(std::move(i));  // lvalue
}
}  // namespace n3

namespace n4 {
// 通过将函数模板的形参类型设置为T&&，可以很好地解决接收左、右值的问题。
// 但除此之外，还需要解决一个问题，即无论传入的形参是左值还是右值，
// 对于函数模板内部来说，形参既有名称又能寻址，因此它都是左值。
// 那么如何才能将函数模板接收到的形参连同其左、右值属性，一起传递给被调用的函数呢？
// C++11标准引入了模板函数forword<T>()，只需要调用该函数，就可以很方便地解决此问题。

// 实现完美转发的函数模板的最终版本:
template <typename T>
void function(T&& t) {
  // forword()用于修饰被调用函数中需要维持参数左、右值属性的参数
  otherdef(std::forward<T>(t));
}

// 总的来说，在定义模板函数时应该：
// 1.采用右值引用定义参数类型，由此该函数既可以接收外界传入的左值，也可以接收右值；
// 2.使用C++11提供的forword()模板函数修饰被调用函数中需要维持左、右值属性的参数。
// 由此即可轻松实现函数模板中参数的完美转发。

void testN4() {
  function(5);  // rvalue
  int i = 4;
  function(i);             // lvalue
  function(std::move(i));  // rvalue
}
}  // namespace n4

int main(int argc, char* argv[]) {
  if (argc < 2) {
    std::cout << argv[0] << " i [0 - 2]" << std::endl;
    return 0;
  }
  int type = argv[1][0] - '0';
  switch (type) {
    case 0:
      n1::testN1();
      break;
    case 1:
      n2::testN2();
      break;
    case 2:
      n3::testN3();
      break;
    case 3:
      n4::testN4();
      break;
    default:
      std::cout << "invalid type" << std::endl;
      break;
  }

  return 0;
}