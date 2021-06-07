#include <iostream>

using namespace std;

// 将文件间的编译依存关系降至最低

/*
class的定义不只描述了class接口，还包括十足的实现细目。
#include <string>
#incldue "date.h"
#include "address.h"
using namespace std;
class Person {
 public:
  Person(const string& name, const Date& birthday, const Address& addr);
  string name() const;
  string birthday() const;
  string address() const;

 private:
  string theName;
  Date theBirthDate;
  Address theAddress;
};
如果这些头文件中任何一个被改变，或这些头文件所依赖的其他头文件有任何改变，那么每一个含入Person class的文件都要重新编译。
这样连串编译依存关系会对许多项目造成难以形容的灾难。
*/

/*
将实现细目分开描述。如果可以这么做，Person的客户就只需要在Person接口被修改时才重新编译。
namespace std {
  class string;
}
class Date;
class Address;
class Person {
 public:
  Person(const string& name, const Date& birthday, const Address& addr);
  string name() const;
  string birthday() const;
  string address() const;

 private:
  string theName;
  Date theBirthDate;
  Address theAddress;
};
这样想法存在两个问题：
1 string不是class，它是个typedef，前置声明错了，正确的前置声明比较复杂，因为涉及额外的template。
  然而那并不要紧，因为不来就不该尝试手工声明一部分标准程序库。你应该仅仅使用适当的#incldue完成目标。
2 编译器必须在编译期间知道对象的大小。怎么知道对象有多大呢？唯一办法就是查看对象的定义。
int main() {
  int x;
  Person p(params);
}
*/

/*
把Person分割为两个class，一个只提供接口，另一个负责实现该接口。
#include <string>
#incldue <memory>
using namespace std;
class PersonImpl;
class Date;
class Address;
class Person {
 public:
  Person(const string& name, const Date& birthday, const Address& addr);
  string name() const;
  string birthday() const;
  string address() const;
 private:
  shared_ptr<PersonImpl> pImpl;
};
main class（Person）只含一个指针成员，这般设计常被称为pimpl idiom（pointer to implementation）。
这种class内的指针名往往就是pImpl。

这样的设计下，Person的客户就完全与Date，Address以及Person的实现细节分离了。
客户也无法看到Person的实现细节，也就不可能写出什么取决于实现的代码。这真正是“接口与实现分离”！
这个分离的关键在于以“声明的依赖性”替换“定义的依赖性”，那正是编译依赖性最小化的本质：
让头文件尽可能自我满足，万一做不到，则让它与其他文件内的声明（非定义）依赖。

设计策略：
1 如果使用object reference或object pointer可以完成认为，就不要使用object。
  可以依靠一个类型声明就定义出该类型的reference和pointer；但如果定义某类型的object，就需要用到该类型的定义。
2 如果能够，尽量以class声明替换class定义。
  声明一个函数用到一个class时，并不需要该class的定义；以by value方式传参也不需要定义。
*/

// 请记住：
// 支持”编译依存性最小化的一般构想是：相依于声明，不要相依于定义。基于此构想的两个手段是handle class和interface class。
// 程序库头文件应该以“完全且仅有声明”的形式存在。这种做法不论是否涉及template都适用。

int main() { return 0; }