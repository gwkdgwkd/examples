#include <iostream>
#include <string>

using namespace std;

// 成对使用new和delete时要采用相同形式

class Demo {
 public:
  Demo() { cout << "Demo" << endl; }
  ~Demo() { cout << "~Demo" << endl; }
};

// 使用new生成一个对象，有两件事发生：
// 1.内存被分配出来（通过operator new函数）；
// 2.针对此内存会有一个（或多个）构造函数被调用。
// 使用delete，也有两件事发生：
// 1.针对此内存会有一个（或更多）析构函数被调用;
// 2.然后内存被释放（通过operator delete函数）。
// delete的最大问题在于：即将被删除的内存之内究竟有多少对象？
// 答案决定了有多少个析构函数必须被调用起来。

// 即将被删除的那个指针，所指的是单一对象或对象数组？
// 这是必不可缺的问题，因为单一对象的内存布局一般与数组的内存布局不同。

// 请记住：
// 如果在new表达式中使用[]，必须在相应的delete表达式中也使用[]。
// 如果在new表达式中不使用[]，一定不要在相应的delete表达式中使用[]。

int main() {
  Demo *d = new Demo[5];
  // delete d;
  // Demo
  // Demo
  // Demo
  // Demo
  // Demo
  // ~Demo

  delete[] d;
  // Demo
  // Demo
  // Demo
  // Demo
  // Demo
  // ~Demo
  // ~Demo
  // ~Demo
  // ~Demo
  // ~Demo

  // 最好尽量不要对数组做typedef动作，
  // 可以使用string，vector等模板代替数组。
  typedef string AddressLines[4];  // 使用vector<string>代替
  string *pal = new AddressLines;
  // typedef后很自然使用这种形式的delete，但会导致行为未定义：
  // delete pal;
  delete[] pal;  // 正常

  return 0;
}