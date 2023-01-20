#include <iostream>
#include <string>

using namespace std;

// 复制对象时不要忘了每一个成分

// 默认的copying函数，将被拷贝对象的所有成员变量都做一份拷贝。
// 如果自己声明了copying函数，意思告诉编译器，
// 不喜欢缺省实现中的某些行为。
// 编译器仿佛被冒犯似的，会以一种奇怪的方式回敬：
// 实现的代码几乎必然出错时却不告诉你。
class Date {};
class Base {
 public:
  Base(const Base &rhs) : name(rhs.name) {
    cout << "Base copy constructor" << endl;
  }
  Base &operator=(const Base &rhs) {
    cout << "Base copy constructor" << endl;
    name = rhs.name;
    return *this;
  }

 private:
  string name;
  Date date;  // copying函数中没有处理date，编译器不会提示。
};
class Derived : public Base {
  // 为derived class撰写copying函数，
  // 必须很小心地复制其base class成分，往往是private，
  // 无法直接访问，需要调用base class对应的copying函数。
  Derived(const Derived &rhs) : Base(rhs), proiority(rhs.proiority) {
    cout << "Derived copy constructor" << endl;
  }
  Derived &operator=(const Derived &rhs) {
    cout << "Derived copy constructor" << endl;
    Base::operator=(rhs);  // 对base class成分进行赋值操作
    proiority = rhs.proiority;
    return *this;
  }

 private:
  int proiority;
};

// 当编写一个copying函数，请确保：
// 1.复制所有local成员变量；
// 2.调用所有base class内的适当的copy函数。

// copy assignment操作符和copy构造函数往往有相近的实现。
// copy assignment操作符调用copy构造函数是不合理的，
// 因为这就像试图构造一个已经存在的对象。
// copy构造函数调用copy assignment操作符同样无意义。
// 构造函数用来初始化新对象，而assignment操作符只用于已初始化对象身上。
// 消除重复代码的做法是，建立一个新的成员函数给两者调用，
// 这样的函数往往是private而且常被命名为init。

// 请记住：
// 1.copying函数应该确保复制对象内的所有成员变量及所有base class成分。
// 2.不要尝试以某个copying函数实现另一个copying函数，
//   应该将共同机制放进第三个函数中，并由两个copying函数共同调用。

int main() { return 0; }