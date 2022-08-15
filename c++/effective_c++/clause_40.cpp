#include <string.h>
#include <iostream>
#include <memory>
#include <string>

using namespace std;

// 明智而审慎地使用多重继承

// 一旦涉及多重继承（MI），C++社群便分为两个基本阵营：
// 1.其中之一如果单一继承（SI）是好的，多重继承一定更好。
// 2.令一派则主张，单一继承是好的，但多重继承不值得拥有。

// 当MI进入设计框架，会导致较多的歧义，
// 因为程序有可能从一个以上的base class继承相同的名称。
class BorrowableItem {
 public:
  void checkOut() {}
};
class ElectronicGadget {
 private:
  void checkOut() {}
};
class MP3Player : public BorrowableItem, public ElectronicGadget {};

// 多重继承是继承一个以上的base class，
// 一般base class没有有更高级的base class，
// 否则会导致要命的钻石型多重继承：
class File {};
class InputFile : public File {};
class OutputFile : public File {};
class IOFile : public InputFile, public OutputFile {};
// 某个base class和某个derived class之间有一条以上的相同路线。
// 如果继承体系中有上面的情况，就必须面对这样一个问题：
// 是否打算让base class内的成员变量经由每一条路径被复制？
// C++并没有倾斜立场，两个方案（复制和不复制）都支持。
// 缺省的做法是执行复制，否则必须使用virtual继承。
class InputFile1 : virtual public File {};
class OutputFile1 : virtual public File {};
class IOFile1 : public InputFile, public OutputFile1 {};
// 从正确行为的观点来看，public继承应该总是virtual。
// 但为了避免继承得来的成员变量重复，
// 编译必须提供若干幕后戏法，后果是：
// 1.使用virtual继承的class产生的对象，
//   往往比使用non-virtual继承的对象体积大；
// 2.访问virtual base class的成员变量时，
//   也比访问non-virtual base class的成员变量速度慢。
// virtual继承的成本还包括其他方面，
// 支持virtual base class初始化规则，
// 比起non-virual base的情况远为复杂且不直观。
// virtual base的初始化责任是由继承体系中的最底层class负责，这意味着：
// 1.class若派生自virtual base而需要初始化，
//   必须认知其virtual base，不论那些base距离多远；
// 2.当一个新的derived class加入继承体系中，
//   它必须承担其virtual base（不论直接或间接）的初始化责任。

// 建议：
// 1.非必要不使用virtual base。
//   平时请使用non-virtual继承。
// 2.如果必须使用virtual base class，尽可能避免在其中放置数据。
//   这样就不需担心初始化和赋值所带来的诡异事情了。
// Java和.NET的Interface值得注意，
// 许多方面兼容C++的virtual base class，而且也不允许含有任何数据。

class IPerson {
 public:
  virtual ~IPerson();
  virtual string name() const = 0;
  virtual string birthDate() const = 0;
};
class DatabaseID {};
class PersonInfo {
  // 用来协助以各种格式打印数据库字段
 public:
  explicit PersonInfo(DatabaseID pid);
  virtual ~PersonInfo();
  virtual const char* theName() const {
    static char value[100];
    strcpy(value, valueDelimOpen());
    strcat(value, valueDelimOpen());
    return value;
  }
  virtual const char* theBirthDate() const;
  virtual const char* valueDelimOpen() const { return "["; }
  virtual const char* valueDelimClose() const { return "]"; }
};
// 多重继承一个通情达理的应用场景：
// 将public继承自某接口和private继承自某实现结合在一起：
class CPerson : public IPerson, private PersonInfo {
  // CPerson和PersonInfo的关系是，
  // PersonInfo刚好有若干函数可以帮助CPerson比较容易实现出来。
  // 它们的关系是is-implemented-in-terms-of，
  // 这种关系有两种技术实现：复合和private继承。
  // 通常复合比较受欢迎，但如果需要重新定义virtual函数，那么继承是必要的。
  // 需要重新定义valueDelimOpen和valueDelimClose，
  // 复合不合适，所以使用private继承。

  // CPerson也必须实现IPerson接口，那需要public继承才能完成。
 public:
  explicit CPerson(DatabaseID pid) : PersonInfo(pid) {}
  virtual string name() const { return PersonInfo::theName(); }
  virtual string birthDate() const { return PersonInfo::theBirthDate(); }

 private:
  const char* valueDelimOpen() const { return ""; }
  const char* valueDelimClose() const { return ""; }
};
shared_ptr<IPerson> makePerson(DatabaseID personIdentifier) {}
DatabaseID askUserForDatabaseID() {}

// 多重继承只是面向对象工具箱里的一个工具而已。
// 和单一继承比，它通常比较复杂，使用上也比较难以理解，
// 所以如果同样的设计，单一继承一定比较受欢迎。
// 多继承有时的确是完成任务最简洁、最易维护、最合理的做法，此时也别怕使用它。

// 请记住：
// 多重继承比单一继承复杂。
// 它可能导致新的歧义性，以及对virtual继承的需要。
// virtual继承会增加大小、速度、初始化（及赋值）复杂度等等成本。
// 如果virtual base class不带任何数据，将是最具实用价值的情况。
// 多重继承的确有正当用途，其中一个场景为下面两种情况的组合：
// public继承某个Interface class；
// private继承某个协助实现的class。

int main() {
  MP3Player mp;
  // mp.checkOut();  // 歧义，不知道调用哪个checkOut
  // checkOut的调用是模棱两可的，
  // 即使两个函数中只有一个可用的（有一个private）。
  // 这与C++用来解析重载函数调用的规则相符：
  // 在看到是否有个函数可用之前，C++首先确认这个函数对此调用是最佳匹配。
  // 找到最佳匹配函数后才检验可取用性。
  // 要解决歧义，需要加上base class作用域：
  mp.BorrowableItem::checkOut();
  // mp.ElectronicGadget::checkOut();  // 私有的

  DatabaseID id(askUserForDatabaseID());
  shared_ptr<IPerson> pp(makePerson(id));

  return 0;
}