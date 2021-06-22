#include <iostream>

using namespace std;

// 考虑用运算符的赋值形式（op=）取代其单独形式（op）

// 大多数程序员认为如果他们能这样写代码：
// x = x + y; x = x - y;
// 那他们也能这样写：
// x += y; x -= y;
// 如果x和y是用户定义的类型（user-defined type），就不能确保这样。
// 就C++来说，operator+、operator=和operator+=之间没有任何关系，因此如果你想让这三个operator同时存在并具有你所期望的关系，就必须自己实现它们。
// 同理，operator -, *, /,等等也一样。

// 确保operator的赋值形式（assignment version）（例如operator+=）与一个operator的单独形式（stand-alone）(例如operator+)之间存在正常的关系，
// 一种好方法是后者（operator+）根据前者（operator+=）来实现。这很容易：
class Rational {
 public:
  Rational& operator+=(const Rational& rhs) {}
  Rational& operator-=(const Rational& rhs) {}
};
const Rational operator+(const Rational& lhs, const Rational& rhs) {
  return Rational(lhs) += rhs;
}
const Rational operator-(const Rational& lhs, const Rational& rhs) {
  return Rational(lhs) -= rhs;
}
// 从零开始实现operator+=和-=，而operator+和operator-则是通过调用前述的函数来提供自己的功能。
// 使用这种设计方法，只用维护operator的赋值形式就行了。
// 而且如果假设operator赋值形式在类的public接口里，这就不用让operator的单独形式成为类的友元。

// 如果你不介意把所有的operator的单独形式放在全局域里，那就可以使用模板来替代单独形式的函数的编写：
template <class T>
const T operator+(const T& lhs, const T& rhs) {
  return T(lhs) += rhs;  // 比大多数编译器希望进行的返回值优化更复杂。
}
template <class T>
const T operator-(const T& lhs, const T& rhs) {
  return T(lhs) -= rhs;
}
// 使用这些模板，只要为operator赋值形式定义某种类型，一旦需要，其对应的operator单独形式就会被自动生成。

// 在这里值得指出的是三个效率方面的问题:
// 1 总的来说operator的赋值形式比其单独形式效率更高，因为单独形式要返回一个新对象，从而在临时对象的构造和释放上有一些开销。
//   operator的赋值形式把结果写到左边的参数里，因此不需要生成临时对象来容纳operator的返回值。
// 2 提供operator的赋值形式的同时也要提供其标准形式，允许类的客户端在便利与效率上做出折衷选择。
//   客户端可以决定是这样编写：
//   Rational a, b, c, d, result;
//   result = a + b + c + d;// 可能用了3个临时对象每个operator+调用使用1个
//   还是这样编写：
//   result = a; //不用临时对象
//   result += b; //不用临时对象
//   result += c; //不用临时对象
//   result += d; //不用临时对象
//   前者比较容易编写、debug 和维护，并且在80％的时间里它的性能是可以被接受的后者具有更高的效率。
//   你可以让客户端开发人员用更容易阅读的单独形式的operator来开发和debug代码，同时保留用效率更高的operator赋值形式替代单独形式的权力。
//   而且根据operator的赋值形式实现其单独形式，这样你能确保当客户端从一种形式切换到另一种形式时，操作的语义可以保持不变。
// 3 涉及到operator单独形式的实现。
//   return T(lhs) += rhs;表达式T(lhs)调用了T的拷贝构造函数。它建立一个临时对象，其值与lhs一样。
//   这个临时对象用来与rhs一起调用operator+=，操作的结果被从operator+返回。
//   template <class T>
//   const T operator+(const T& lhs, const T& rhs) {
//     T result(lhs);         // 拷贝lhs到result中
//     return result += rhs;  // rhs与它相加并返回结果
//   }
//   这个模板几乎与前面的程序相同，但是它们之间还是存在重要的差别。
//   第二个模板包含一个命名对象，result。这个命名对象意味着不能在operator+里使用返回值优化。
//   第一种实现方法总可以使用返回值优化，所以编译器为其生成优化代码的可能就会更大。
//   上面第一个函数实现也有这样的临时对象开销，就象你为使用命名对象result而耗费的开销一样。然
//   而未命名的对象在历史上比命名对象更容易清除，因此当我们面对在命名对象和临时对象间进行选择时，用临时对象更好一些。
//   它使你耗费的开销不会比命名的对象还多，特别是使用老编译器时，它的耗费会更少。

// operator的赋值形式（operator+=）比单独形式(operator+)效率更高。
// 做为一个库程序设计者，应该两者都提供，做为一个应用程序的开发者，在优先考虑性能时你应该考虑考虑用operator赋值形式代替单独形式。

int main() { return 0; }