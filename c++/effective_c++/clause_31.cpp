#include <iostream>

using namespace std;

// 将文件间的编译依存关系降至最低

/*
// class的定义不只描述了class接口，还包括十足的实现细节：
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
// 如果这些头文件中任何一个被改变，或这些头文件所依赖的其他头文件有任何改变，
// 那么每一个含入Person class的文件都要重新编译。
// 这样连串编译依存关系会对许多项目造成难以形容的灾难。
*/

/*
// 将实现细节分开描述，如果可以这么做，
// Person的客户就只需要在Person接口被修改时才重新编译：
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
// 这样想法存在两个问题：
// 1.string不是class，它是个typedef，前置声明错了，
//   正确的前置声明比较复杂，因为涉及额外的template。
//   然而那并不要紧，因为不来就不该尝试手工声明一部分标准程序库。
//   你应该仅仅使用适当的#incldue完成目标。
// 2.编译器必须在编译期间知道对象的大小。
//   怎么知道对象有多大呢？唯一办法就是查看对象的定义。
int main() {
  int x;
  Person p(params);
}
*/

/*
// 把Person分割为两个class，一个只提供接口，另一个负责实现该接口：
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
// main class（Person）只含一个指针成员，
// 这般设计常被称为pimpl idiom（pointer to implementation）。
// 这种class内的指针名往往就是pImpl。

// 这样的设计下，Person的客户就完全与Date，
// Address以及Person的实现细节分离了。
// 客户也无法看到Person的实现细节，也就不可能写出什么取决于实现的代码。
// 这真正是接口与实现分离！
// 这个分离的关键在于以声明的依赖性替换定义的依赖性，
// 那正是编译依赖性最小化的本质：
// 让头文件尽可能自我满足，万一做不到，
// 则让它与其他文件内的声明（非定义）依赖。

// 像Person这个使用pimpl idiom的class，往往被称为Handle class。
// 并不会改变Person做的事，只会改变它做事的方法。
#include "Person.h"
// PersonImpl有着和Person完全相同的成员函数，两者接口完全相同：
#include "PersonImpl.h"  
Person::Person(const string& name, const Date& birthday, const Address& addr)
       : pImpl(new PersonImpl(name, birthday, addr)) {}
string Person::name const { return pImpl->name(); }
*/

/*
// 另一个制作Handle class的办法是，
// 令Person成为一种特殊的抽象基类，称为Interface class。
// 这种class的目地是详细描述derived class的接口，
// 因此通常不带成员变量，也没有构造函数，
// 只有一个virtual析构函数以及一组pure virtual函数，
// 用来描述整个接口。
// Interface class类似Java和.NET的Interfaces，
// 但C++的Interface class，
// 并不需要负担Java和.NET的Interfac所需负担的责任。
// Java和.NET都不允许在Interface内实现成员变量或成员函数，
// 但C++不禁止这两样东西。
class Person{
 public:
  virtual ~Person();
  virtual string name() const = 0;
  virtual string birthday() const = 0;
  virtual string address() const = 0;

  // Interface class的客户必须有办法为这种class创建新的对象。
  // 通常调用一个特殊函数（factory函数或virtual构造函数），
  // 返回指针（或智能指针），指向动态分配的对象，
  // 而该对象支持Interface class的接口。
  // 这样的函数又往往在Interface class内被声明为static。
  static shared_ptr<Person> create(const string& name, 
                                   const Date& birthday, 
                                   const Address& addr);
};
// 这个class的客户必须以Person的pointer和reference来编写应用程序。
// 就像Handle classes的客户一样，
// 除非Interface class的接口被修改否则其客户不需要重新编译。

// 客户这样使用：
string name;
Date dateOfBirth;
Address address;
shared_ptr<Person> pp(Person::create(name,dateOfBirth,address));
cout << pp->name() << pp->birthDate() << pp->address();

// 支持Interface class接口的那个具体类必须被定义出来，
// 而且真正的构造函数必须被调用。
// 一切都在virtual构造函数实现代码所在的文件内秘密发生。
class RealPerson : public Person{
 public:
  RealPerson(const string& name, const Date& birthday, 
             const Address& addr)
            : theName(name), theBirthDate(birthday), 
              theAddress(addr) {}
  virtual ~RealPerson() {}
  string name() const;
  string birthday() const;
  string address() const;
 private:
  string theName;
  Date theBirthDate;
  Address theAddress;
};

有了RealPerson之后，Person::create定义如下：
shared_ptr<Person> create(const string& name, 
                          const Date& birthday, 
                          const Address& addr) {
  return shared_ptr<Person>(new RealPerson(name, birthday, addr));
}
// 更现实的Person::create实现会根据额外参数、读自文件或数据库的数据、
// 环境变量来创建不同类型的derived class对象。

// RealPerson示范实现Interface class的两个常见机制之一，
// 另一个涉及多重继承，参考条款40：
// 从Interface class继承接口，然后实现出接口所覆盖的函数。
*/

// 设计策略：
// 1.如果使用object reference或object pointer可以完成认为，
//   就不要使用object。
//   可以依靠一个类型声明就定义出该类型的reference和pointer；
//   但如果定义某类型的object，就需要用到该类型的定义。
// 2.如果能够，尽量以class声明替换class定义。
//   声明一个函数用到一个class时，并不需要该class的定义；
//   以by value方式传参也不需要定义。
//   class Date;
//   Date today();
//   void clearAppointments(Date d);
//   声明today和clearAppointments函数无需定义Date。
//   一旦任何人调用那些函数，调用之前一定要找到Date的定义。
//   何必费心声明一个没人调用的函数呢？
//   并非没人调用，而是并非每个人都调用。
//   如果能将提供class定义（通过#include完成）的义务，
//   从函数声明所在的头文件移到内含函数调用的客户文件，
//   便可将并非正真必要的类型定义与客户端的编译依赖性去除掉。
// 3.为声明和定义提供不同的头文件。
//   需要两个头文件，一个用于声明，一个用于定义。
//   如果有个声明被改来，两个文件都得改。
//   程序库客户应该总是#include一个声明文件而非前置声明若干函数。
//   #include "datefwd.h"  // 参考C++标准程序库文件的<iosfwd>。
//   Date today();
//   void clearAppointments(Date d);
//   iosfwd深具启发意义的另一个原因，
//   分外彰显本条款适用于template也适用于non-template。
//   C++也提供export关键字，允许将template声明和定义分割于不同的文件内。
//   不幸的是支持这个关键在的编译器目前非常少。

// Handle class和Interface class解除来接口和实现之间的耦合关系，
// 从而降低文件间的编译依赖。
// 在Handle class身上，成员函数必须通过implementation pointer取得对象数据。
// 那会为每一次访问增加一层间接性。
// 而每一个对象消耗的内存数量必须增加implementation pointer的大小。
// 最后，implementation pointer必须初始化（在Handle class构造函数内），
// 指向一个动态分配得来的implementation object。
// 需要承受因动态分配内存而带来的额外开销，以及遭遇bad_alloc异常的可能性。
// 至于Interface class，由于每个函数都是virtual，
// 所以必须为每次函数调用付出一个间接跳跃成本。
// 此外Interface class派生的对象必须包含一个vptr，
// 这个指针可能会增加存放对象所需的内存数量。

// 最后，不论Handle class还是Interface class，
// 一旦脱离inline函数都无法有太大作为。
// 条款30解释了函数本体为了被inline必须置于头文件内，
// 但Handle class和Interface class这是特别被设计出来的，
// 用来隐藏实现细节的，如函数本体。

// 如果因为若干额外成本便不考虑Handle class和Interface class，
// 将是严重的错误。

// 请记住：
// 1.支持编译依存性最小化的一般构想是：相依于声明，不要相依于定义。
// 2.基于此构想的两个手段是handle class和interface class。
// 3.程序库头文件应该以完全且仅有声明的形式存在，
//   这种做法不论是否涉及template都适用。

int main() { return 0; }