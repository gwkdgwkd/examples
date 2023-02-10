#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <typeinfo>

// 让函数根据一个以上的对象来决定怎么虚拟

// void doShow(B &b1, B &b2) { b1.show(b2); }
// 当调用doShow时，并不知道其b1和b2的真实类型，所知道的就只有它们是Base对象。
// 如果显示内容只取决于b1的动态类型，可以将show设为虚函数，并调用b1.show(b2)。
// 如果只取决于b2的动态类型，也可以同样处理。
// 但现在，取决于两个对象的动态类型，
// 虚函数体系只能作用在一个对象身上，它不足以解决问题。
// 需要的是一种作用在多个对象上的虚函数，C++没有提供这样的函数。
// 必须找到一个方法来解决这个被称为二重调度（double dispatch）的问题。
// 这个名字来自于object-oriented programming community，
// 在那里虚函数调用的术语是message dispatch，
// 而基两个参数的虚调用是通过double dispatch实现的，
// 推而广之，在多个参数上的虚函数叫multiple dispatch。
// 有几个方法可以考虑，但没有哪个是没有缺点的，这不该奇怪。
// C++没有直接提供double dispatch，
// 所以必须自己完成编译器在实现虚函数时所做的工作。
// 如果容易的话，可能就什么都自己做了，并用C语言编程了，
// 没有这么做也做不到，所以系紧安全带，有一个坎途了。

namespace n1 {
// 用虚函数加RTTI
// 虚函数实现了一个单一调度，这只是所需要的一半；
// 编译器实现虚函数，所以在Base中声明一个虚函数show。
// 实现二重调度的最常见方法：
// 就是和虚函数体系格格不入的if...then...else链。
// 在这种刺眼的体系下，首先是发现b2的真实类型，然后处理所有的可能：
class B {
 public:
  virtual ~B() {}
  virtual void show(B &b) = 0;
  virtual std::string get() = 0;
};
class DA : public B {
 public:
  virtual void show(B &b);
  virtual std::string get() { return "DA"; }
};
class DB : public B {
 public:
  virtual void show(B &b);
  virtual std::string get() { return "DB"; }
};
class DC : public B {
 public:
  virtual void show(B &b);
  virtual std::string get() { return "DC"; }
};
class UnknownD {
 public:
  UnknownD(B &b) : b_(b) {}
  std::string what() const { return "unknown " + b_.get(); }

 private:
  B &b_;
};
void doShow(B &b1, B &b2) { b1.show(b2); }

// 这儿的代码一点都不复杂，它很容易实现，也很容易让它工作。
// RTTI只有一点令人不安：
// 它只是看起来无害，实际的危险来自于最后一个else语句，在这儿抛了一个异常。
// 这种实现方式的代价是几乎放弃了封装，
// 因为每个show函数都必须知道所有其它同胞类中的版本。
// 尤其是，如果增加一个新的类时，
// 必须更新每一个基于RTTI的if...then...else链以处理这个新的类型。
// 即使只是忘了一处，程序都将有一个bug，而且它还不显眼。
// 编译器也没办法帮助检查这种疏忽，因为它们根本不知道代码在做什么。
// 这种类型相关的程序在C语言中已经很有一段历史了，
// 而且谁都知道这样的程序本质上是没有可维护性的。
// 扩充这样的程序最终是不可想象的，这是引入虚函数的主意原因：
// 将产生和维护类型相关的函数调用的担子由程序员转给编译器。
// 当用RTTI实现二重调度时，正退回到过去的苦日子中。
// 这种过时的技巧在C语言中导致了错误，它们在C++语言也仍然导致错误。
// 认识到自己的脆弱，在show函数中加上了最后的那个else语句，
// 以处理如果遇到一个未知类型。
// 这种情况原则上说是不可能发生的，但在决定使用RTTI时又怎么知道呢？
// 有很多种方法来处理这种未曾预料的相互作用，但没有一个令人非常满意。
// 在这个例子里，选择了抛出一个异常，
// 但无法想象调用者对这个错误的处理能够比开发者好多少，
// 因为遇到了一个开发者不知道其存在的东西。
void DA::show(B &b) {
  std::string str;
  const std::type_info &type = typeid(b);
  if (type == typeid(DA)) {
    DA &da = static_cast<DA &>(b);
    str = get() + " " + da.get();
  } else if (type == typeid(DB)) {
    DB &db = static_cast<DB &>(b);
    str = get() + " " + db.get();
  } else if (type == typeid(DC)) {
    DC &dc = static_cast<DC &>(b);
    str = get() + " " + dc.get();
  } else {
    throw UnknownD(b);
  }

  std::cout << str << std::endl;
}
void DB::show(B &b) {
  std::string str;
  const std::type_info &type = typeid(b);
  if (type == typeid(DA)) {
    DA &da = static_cast<DA &>(b);
    str = get() + " " + da.get();
  } else if (type == typeid(DB)) {
    DB &db = static_cast<DB &>(b);
    str = get() + " " + db.get();
  } else if (type == typeid(DC)) {
    DC &dc = static_cast<DC &>(b);
    str = get() + " " + dc.get();
  } else {
    throw UnknownD(b);
  }

  std::cout << str << std::endl;
}
void DC::show(B &b) {
  std::string str;
  const std::type_info &type = typeid(b);
  if (type == typeid(DA)) {
    DA &da = static_cast<DA &>(b);
    str = get() + " " + da.get();
  } else if (type == typeid(DB)) {
    DB &db = static_cast<DB &>(b);
    str = get() + " " + db.get();
  } else if (type == typeid(DC)) {
    DC &dc = static_cast<DC &>(b);
    str = get() + " " + dc.get();
  } else {
    throw UnknownD(b);
  }

  std::cout << str << std::endl;
}

class DD : public B {
 public:
  virtual void show(B &b) {}
  virtual std::string get() { return "DD"; }
};
void func() {
  DA da1, da2;
  DB db1, db2;
  DC dc1, dc2;
  doShow(da1, da2);  // DA DA
  doShow(da1, db2);  // DA DB
  doShow(da1, dc2);  // DA DC
  doShow(db1, da2);  // DB DA
  doShow(db1, db2);  // DB DB
  doShow(db1, dc2);  // DB DC
  doShow(dc1, da2);  // DC DA
  doShow(dc1, db2);  // DC DB
  doShow(dc1, dc2);  // DC DC

  DD dd;
  try {
    doShow(da1, dd);
  } catch (const UnknownD &e) {
    std::cout << e.what() << std::endl;
  }
  // unknown DD
}
}  // namespace n1

namespace n2 {
// 只使用虚函数
// 其实有一个方法可以将用RTTI实现二重调度固有风险降到最低的，
// 不过在此之前让看一下怎么只用虚函数来解决二重调度问题。
// 这个方法和RTTI方法有着同样的基本构架。
// show函数被申明为虚，并被所有派生类重定义，此外，
// 它还被每个类重载，每个重载处理一个派生类型：
class DA;
class DB;
class DC;
class B {
 public:
  virtual ~B() {}
  virtual void show(B &b) = 0;
  virtual void show(DA &da) = 0;
  virtual void show(DB &db) = 0;
  virtual void show(DC &dc) = 0;
  virtual std::string get() = 0;
};
class DA : public B {
 public:
  virtual void show(B &b) { b.show(*this); }
  // 粗一看，它象依据参数的顺序进行循环调用，
  // 也就是开始的b变成了调用成员函数的对象，而*this成了它的参数。
  // 但再仔细看一下啦，它不是循环调用。
  // 编译器根据参数b的静态类型决定调那一组函数中的哪一个。
  // 有四个不同的show函数可以被调用，但根据*this的静态类型来选中其中一个。
  // 现在的静态类型是什么？
  // 因为是在DA的成员函数中，所以*this肯定是DA类型。
  // 调用的将是接受DA参数的show函数，而不是带B类型参数的show函数。
  // 一点都不混乱，也不麻烦，没有RTTI，也不需要为意料之外的对象类型抛异常。
  // 不会有意料之外的类型的，这就是使用虚函数的好处。
  // 实际上，如果没有那个致命缺陷的话，它就是实现二重调度问题的完美解决方案。
  // 这个缺陷是，和前面看到的RTTI方法一样：每个类都必须知道它的同胞类。
  // 当增加新类时，所有的代码都必须更新，不过，更新方法和前面不一样。
  // 确实，没有if...then...else需要修改，但通常是更差：
  // 每个类都需要增加一个新的虚函数。
  // 如果决定增加一个新类DD（继承于B），必须为每个现存类增加一个show函数。
  // 修改现存类经常是做不到的，比如，不是在写整个游戏，
  // 只是在完成程序框架下的一个支撑库，可能无权修改B类或从其经常的框架类。
  // 此时，增加一个新的成员函数（虚的或不虚的），都是不可能的。
  // 也就说，理论上有操作需要被修改的类的权限，但实际上没有。

  virtual void show(DA &da);
  virtual void show(DB &db);
  virtual void show(DC &dc);
  virtual std::string get() { return "DA"; }
};
class DB : public B {
 public:
  virtual void show(B &b) { b.show(*this); }
  virtual void show(DA &da);
  virtual void show(DB &db);
  virtual void show(DC &dc);
  virtual std::string get() { return "DB"; }
};
class DC : public B {
 public:
  virtual void show(B &b) { b.show(*this); }
  virtual void show(DA &da);
  virtual void show(DB &db);
  virtual void show(DC &dc);
  virtual std::string get() { return "DC"; }
};
void DA::show(DA &da) { std::cout << da.get() + " " + get() << std::endl; }
void DA::show(DB &db) { std::cout << db.get() + " " + get() << std::endl; }
void DA::show(DC &dc) { std::cout << dc.get() + " " + get() << std::endl; }
void DB::show(DA &da) { std::cout << da.get() + " " + get() << std::endl; }
void DB::show(DB &db) { std::cout << db.get() + " " + get() << std::endl; }
void DB::show(DC &dc) { std::cout << dc.get() + " " + get() << std::endl; }
void DC::show(DA &da) { std::cout << da.get() + " " + get() << std::endl; }
void DC::show(DB &db) { std::cout << db.get() + " " + get() << std::endl; }
void DC::show(DC &dc) { std::cout << dc.get() + " " + get() << std::endl; }
void doShow(B &b1, B &b2) { b1.show(b2); }
void func() {
  DA da1, da2;
  DB db1, db2;
  DC dc1, dc2;
  doShow(da1, da2);  // DA DA
  doShow(da1, db2);  // DA DB
  doShow(da1, dc2);  // DA DC
  doShow(db1, da2);  // DB DA
  doShow(db1, db2);  // DB DB
  doShow(db1, dc2);  // DB DC
  doShow(dc1, da2);  // DC DA
  doShow(dc1, db2);  // DC DB
  doShow(dc1, dc2);  // DC DC
}

// 总结一下就是：
// 如果需要实现二重调度，最好的办法是修改设计以取消这个需要。
// 如果做不到的话，虚函数的方法比RTTI的方法安全，
// 但它限制了程序的可控制性（取决于是否有权修改头文件）。
// 另一方面，RTTI的方法不需要重编译，但通常会导致代码无法维护，需要抉择。
}  // namespace n2

namespace n3 {
// 模拟虚函数表
// 有一个方法来增加选择，编译器通常创建一个函数指针数组（vtbl）来实现虚函数，
// 并在虚函数被调用时在这个数组中进行下标索引。
// 使用vtbl，编译器避免了使用if...then...else链，
// 并能在所有调用虚函数的地方生成同样的代码：
// 确定正确的vtbl下标，然后调用vtbl这个位置上存储的指针所指向的函数。
// 没理由说不能这么做，如果这么做了，不但使得基于RTTI的代码更具效率，
// 下标索引加函数指针的反引用通常比if...then...else高效，产生的代码也少。
// 同样也将RTTI的使用范围限定在一处：初始化函数指针数组的地方。
class B {
 public:
  virtual ~B() {}
  virtual void show(B &b) = 0;
  virtual std::string get() = 0;
};
class UnknownD {
 public:
  UnknownD(B &b) : b_(b) {}
  std::string what() const { return "unknown " + b_.get(); }

 private:
  B &b_;
};
void doShow(B &b1, B &b2) { b1.show(b2); }

class DA : public B {
 public:
  virtual void show(B &b) {
    ShowFunctionPtr hfp = lookup(b);
    if (hfp) {
      (this->*hfp)(b);
    } else {
      // 如果能保持映射表和B的继承层次的同步，
      // lookup就总能找到传入对象对应的有效函数指针。
      // 人终究只是人，就算再仔细，错误也会钻入软件。
      // 这就是为什么检查lookup的返回值并在其失败时抛异常的原因。
      throw UnknownD(b);
    }
  }

  // 和n2中基于虚函数的方法相似，每种show都由一个独立的函数处理；
  // 不过不同的是，这次，这些函数有着不同的名字，而不是都叫show。
  void showDA(B &da);
  void showDB(B &db);
  void showDC(B &dc);
  virtual std::string get() { return "DA"; }

 private:
  typedef void (DA::*ShowFunctionPtr)(B &);
  typedef std::map<std::string, ShowFunctionPtr> ShowMap;
  // 在DA::show中，需要一个方法来映射参数b的动态类型到一个成员函数指针。
  // 一个简单的方法是创建一个映射表，给定的类名对应恰当的成员函数指针。
  // 直接使用一个这样的映射表来实现show是可行的，
  // 但如果增加一个中间函数lookup时，将更好理解。
  // lookup函数接受一个B参数，返回相应的成员函数指针。
  static ShowFunctionPtr lookup(const B &b) {
    // 不过这意味着要付出拷贝赋值的代价，不想这么做。
    // 如果initializeMap()返回指针的话，就不需要付出这个代价，
    // 但这样就需要担心指针指向的map对象是否能在恰当的时候被析构了。
    // 幸好，有个两全的方法，可以将map改为一个智能指针，
    // 它将在自己被析构时delete所指向的对象。
    static std::shared_ptr<ShowMap> map(initializeMap());
    // std::cout << typeid(b).name() << std::endl;
    ShowMap::iterator it = map->find(typeid(b).name());
    if (it == map->end()) return 0;
    return (*it).second;
  }
  static ShowMap *initializeMap() {
    ShowMap *phm = new ShowMap;
    (*phm)["N2n32DAE"] = &DA::showDA;
    (*phm)["N2n32DBE"] = &DA::showDB;
    (*phm)["N2n32DCE"] = &DA::showDC;
    return phm;
  }
};
class DB : public B {
 public:
  virtual void show(B &b) {
    ShowFunctionPtr hfp = lookup(b);
    if (hfp) {
      (this->*hfp)(b);
    } else {
      throw UnknownD(b);
    }
  }
  void showDA(B &da);
  void showDB(B &db);
  void showDC(B &dc);
  virtual std::string get() { return "DB"; }

 private:
  typedef void (DB::*ShowFunctionPtr)(B &);
  typedef std::map<std::string, ShowFunctionPtr> ShowMap;
  static ShowFunctionPtr lookup(const B &b) {
    static std::shared_ptr<ShowMap> map(initializeMap());
    // std::cout << typeid(b).name() << std::endl;
    ShowMap::iterator it = map->find(typeid(b).name());
    if (it == map->end()) return 0;
    return (*it).second;
  }
  static ShowMap *initializeMap() {
    ShowMap *phm = new ShowMap;
    (*phm)["N2n32DAE"] = &DB::showDA;
    (*phm)["N2n32DBE"] = &DB::showDB;
    (*phm)["N2n32DCE"] = &DB::showDC;
    return phm;
  }
};
class DC : public B {
 public:
  virtual void show(B &b) {
    ShowFunctionPtr hfp = lookup(b);
    if (hfp) {
      (this->*hfp)(b);
    } else {
      throw UnknownD(b);
    }
  }
  void showDA(B &b);
  void showDB(B &b);
  void showDC(B &b);
  virtual std::string get() { return "DC"; }

 private:
  typedef void (DC::*ShowFunctionPtr)(B &);
  typedef std::map<std::string, ShowFunctionPtr> ShowMap;
  static ShowFunctionPtr lookup(const B &b) {
    static std::shared_ptr<ShowMap> map(initializeMap());
    // std::cout << typeid(b).name() << std::endl;
    ShowMap::iterator it = map->find(typeid(b).name());
    if (it == map->end()) return 0;
    return (*it).second;
  }
  static ShowMap *initializeMap() {
    ShowMap *phm = new ShowMap;
    (*phm)["N2n32DAE"] = &DC::showDA;
    (*phm)["N2n32DBE"] = &DC::showDB;
    (*phm)["N2n32DCE"] = &DC::showDC;
    return phm;
  }
};
void DA::showDA(B &b) {
  // 如果转换失败，dynamic_cast会抛出一个bad_cast异常。
  // 当然，它们从不会失败，
  // 因为碰撞函数被调用时不会带一个错误的参数类型的，只是谨慎一些更好。
  DA &da = dynamic_cast<DA &>(b);
  std::cout << get() + " " + da.get() << std::endl;
}
void DA::showDB(B &b) {
  DB &db = dynamic_cast<DB &>(b);
  std::cout << get() + " " + db.get() << std::endl;
}
void DA::showDC(B &b) {
  DC &dc = dynamic_cast<DC &>(b);
  std::cout << get() + " " + dc.get() << std::endl;
}
void DB::showDA(B &b) {
  DA &da = dynamic_cast<DA &>(b);
  std::cout << get() + " " + da.get() << std::endl;
}
void DB::showDB(B &b) {
  DB &db = dynamic_cast<DB &>(b);
  std::cout << get() + " " + db.get() << std::endl;
}
void DB::showDC(B &b) {
  DC &dc = dynamic_cast<DC &>(b);
  std::cout << get() + " " + dc.get() << std::endl;
}
void DC::showDA(B &b) {
  DA &da = dynamic_cast<DA &>(b);
  std::cout << get() + " " + da.get() << std::endl;
}
void DC::showDB(B &b) {
  DB &db = dynamic_cast<DB &>(b);
  std::cout << get() + " " + db.get() << std::endl;
}
void DC::showDC(B &b) {
  DC &dc = dynamic_cast<DC &>(b);
  std::cout << get() + " " + dc.get() << std::endl;
}

void func() {
  DA da1, da2;
  DB db1, db2;
  DC dc1, dc2;
  doShow(da1, da2);  // DA DA
  doShow(da1, db2);  // DA DB
  doShow(da1, dc2);  // DA DC
  doShow(db1, da2);  // DB DA
  doShow(db1, db2);  // DB DB
  doShow(db1, dc2);  // DB DC
  doShow(dc1, da2);  // DC DA
  doShow(dc1, db2);  // DC DB
  doShow(dc1, dc2);  // DC DC
}
}  // namespace n3

namespace n4 {
// 使用非成员的处理函数
// 现在知道了怎么构造一个类似vtbl的映射表以实现二重调度的第二部分，
// 并且也知道了怎么将映射表的实现细节封装在lookup函数中。
// 因为这张表包含的是指向成员函数的指针，
// 所以在增加新的B类型时仍然需要修改类的定义，
// 这还是意味着所有人都必须重新编译，即使他们根本不关心这个新的类型。
// 如果映射表中包含的指针指向非成员函数，那么就没有重编译问题了。
// 而且，转到非成员的处理函数将发现一个一直被忽略的设计上的问题，
// 就是，应该在哪个类里处理不同类型的对象间的处理，在前面的设计中，
// 对于对象1和对象2，如果对象1是show的左边的参数，将在对象1的类中处理；
// 如果对象2正巧是左边的参数，就在对象2的类中处理。
// 这个有特别的含义吗？是不是这样更好些：
// 类型A和类型B的对象间的碰撞应该既不在A中也不在B中处理，
// 而在两者之外的某个中立的地方处理？
// 如果将处理函数从类里移出来，在给用户提供类定义的头文件时，
// 不用带上任何碰撞处理函数。
// 理论上，这个实现和使用成员函数的版本是相同的，只有几个轻微区别：
// 1.ShowFunctionPtr现在是一个指向非成员函数的指针类型的重定义;
// 2.意料之外的类UnknownD增加一个参数;
// 3.其构造函数需要两个对象作参数而不再是一个了。
//   这也意味着映射需要三个消息了：两个类型名，一个ShowFunctionPtr。
// 最终达到了目的：
// 1.如果增加了新的B的子类，现存类不需要重新编译（除非它们用到了新类）。
// 2.没有了RTTI的混乱和if...then...else的不可维护。
// 3.增加一个新类只需要做明确定义了的局部修改：
//   在initializeMap中增加一个或多个新的映射关系，
//   在show所在的无名的命名空间中申明一个新的碰撞处理函数。
class B {
 public:
  virtual ~B() {}
  virtual std::string get() = 0;
};
class UnknownD {
 public:
  UnknownD(B &b1, B &b2) : b1_(b1), b2_(b2) {}
  std::string what() const { return "unknown " + b1_.get() + " " + b2_.get(); }

 private:
  B &b1_;
  B &b2_;
};
class DA : public B {
 public:
  virtual std::string get() { return "DA"; }
};
class DB : public B {
 public:
  virtual std::string get() { return "DB"; }
};
class DC : public B {
 public:
  virtual std::string get() { return "DC"; }
};
namespace {
// 用了无名的命名空间来包含实现碰撞处理函数所需要的函数。
// 无名命名空间中的东西是当前编译单元（其实就是当前文件）私有的，
// 很象被申明为文件范围内static的函数一样。
// 有了命名空间后，文件范围内的static已经不赞成使用了，
// 应该尽快让自己习惯使用无名的命名空间（只要编译器支持）。
void showAandA(B &b1, B &b2) {
  DA &da1 = dynamic_cast<DA &>(b1);
  DA &da2 = dynamic_cast<DA &>(b2);
  std::cout << da1.get() + " " + da2.get() << std::endl;
}
void showAandB(B &b1, B &b2) {
  DA &da = dynamic_cast<DA &>(b1);
  DB &db = dynamic_cast<DB &>(b2);
  std::cout << da.get() + " " + db.get() << std::endl;
}
void showAandC(B &b1, B &b2) {
  DA &da = dynamic_cast<DA &>(b1);
  DC &dc = dynamic_cast<DC &>(b2);
  std::cout << da.get() + " " + dc.get() << std::endl;
}
void showBandA(B &b1, B &b2) {
  DB &db = dynamic_cast<DB &>(b1);
  DA &da = dynamic_cast<DA &>(b2);
  std::cout << db.get() + " " + da.get() << std::endl;
}
void showBandB(B &b1, B &b2) {
  DB &db1 = dynamic_cast<DB &>(b1);
  DB &db2 = dynamic_cast<DB &>(b2);
  std::cout << db1.get() + " " + db2.get() << std::endl;
}
void showBandC(B &b1, B &b2) {
  DB &db = dynamic_cast<DB &>(b1);
  DC &dc = dynamic_cast<DC &>(b2);
  std::cout << db.get() + " " + dc.get() << std::endl;
}
void showCandA(B &b1, B &b2) {
  DC &dc = dynamic_cast<DC &>(b1);
  DA &da = dynamic_cast<DA &>(b2);
  std::cout << dc.get() + " " + da.get() << std::endl;
}
void showCandB(B &b1, B &b2) {
  DC &dc = dynamic_cast<DC &>(b1);
  DB &db = dynamic_cast<DB &>(b2);
  std::cout << dc.get() + " " + db.get() << std::endl;
}
void showCandC(B &b1, B &b2) {
  DC &dc1 = dynamic_cast<DC &>(b1);
  DC &dc2 = dynamic_cast<DC &>(b2);
  std::cout << dc1.get() + " " + dc2.get() << std::endl;
}
}  // namespace
namespace test1 {
typedef void (*ShowFunctionPtr)(B &, B &);
typedef std::map<std::pair<std::string, std::string>, ShowFunctionPtr> ShowMap;
ShowMap *initializeMap() {
  ShowMap *phm = new ShowMap;
  (*phm)[std::make_pair("N2n42DAE", "N2n42DAE")] = &showAandA;
  (*phm)[std::make_pair("N2n42DAE", "N2n42DBE")] = &showAandB;
  (*phm)[std::make_pair("N2n42DAE", "N2n42DCE")] = &showAandC;
  (*phm)[std::make_pair("N2n42DBE", "N2n42DAE")] = &showBandA;
  (*phm)[std::make_pair("N2n42DBE", "N2n42DBE")] = &showBandB;
  (*phm)[std::make_pair("N2n42DBE", "N2n42DCE")] = &showBandC;
  (*phm)[std::make_pair("N2n42DCE", "N2n42DAE")] = &showCandA;
  (*phm)[std::make_pair("N2n42DCE", "N2n42DBE")] = &showCandB;
  (*phm)[std::make_pair("N2n42DCE", "N2n42DCE")] = &showCandC;
  return phm;
}
ShowFunctionPtr lookup(const std::string &b1, const std::string &b2) {
  static std::shared_ptr<ShowMap> show_map(initializeMap());
  ShowMap::iterator it = show_map->find(std::make_pair(b1, b2));
  if (it == show_map->end()) return 0;
  return (*it).second;
}
void doShow(B &b1, B &b2) {
  // std::cout << typeid(b1).name() << "," << typeid(b2).name() << std::endl;
  ShowFunctionPtr phf = lookup(typeid(b1).name(), typeid(b2).name());
  if (phf)
    phf(b1, b2);
  else
    throw UnknownD(b1, b2);
}

void func1() {
  DA da1, da2;
  DB db1, db2;
  DC dc1, dc2;
  doShow(da1, da2);  // DA DA
  doShow(da1, db2);  // DA DB
  doShow(da1, dc2);  // DA DC
  doShow(db1, da2);  // DB DA
  doShow(db1, db2);  // DB DB
  doShow(db1, dc2);  // DB DC
  doShow(dc1, da2);  // DC DA
  doShow(dc1, db2);  // DC DB
  doShow(dc1, dc2);  // DC DC
}
class DD : public B {  // unknown
 public:
  virtual std::string get() { return "DD"; }
};
class DB1 : public B {
 public:
  virtual std::string get() { return "DB1"; }
};
void func2() {
  DA da;
  DD dd;
  try {
    doShow(da, dd);
  } catch (const UnknownD &e) {
    std::cout << e.what() << std::endl;
  }
  // unknown DA DD

  // 继承与模拟虚函数表
  DB1 db1;
  try {
    // 期望调用showBandA，实际上抛出了异常：
    // 因为lookup在根据类型名DB1和DA，在map中查找函数时没有找到。
    // 虽然BB1可以被转换为一个BB，但lookup却不知道这一点。
    // 而且，没有没有一个简单的办法来告诉它。
    // 如果需要实现二重调度，并且需要这儿的向上类型映射，
    // 只能采用前面讨论的二次虚函数调用的方法，
    // 同时也意味着增加新类的时候，所有人都必须重新编译。
    doShow(da, db1);
  } catch (const UnknownD &e) {
    std::cout << e.what() << std::endl;
  }
  // unknown DA DB1
}
void func() {
  func1();
  func2();
}
}  // namespace test1

namespace test2 {
// 按目前情况来看，test1的设计完全是静态的。
// 每次注册一个碰撞处理函数，就不得不永远留着它。
// 如果想在运行过程中增加、删除或修改处理函数，将怎么样？不提供。
// 但是是可以做到的，可以将映射表放入一个类，
// 并由它提供动态修改映射关系的成员函数。

// 用户于是可以使用此类型的一个全局对象来自动地注册他们所需要的函数：
// 这个类允许在映射表中增加和删除操作，以及根据类型名对查找相应的碰撞处理函数。
// 它也使用了Item E26中讲的技巧来限制ShowMap对象的个数为1，
// 因为系统中只有一个映射表。
// 最后，它允许简化在映射表中增加对称性的碰撞的过程，
// 也就是说，b1和b2谁在前，谁在后，其效果是相同的。
// 它自动增加对称的映射关系，只要addEntry被调用时可选参数symmetric被设为true。
class ShowMap {
 public:
  typedef void (*ShowFunctionPtr)(B &, B &);
  void addEntry(const std::string &type1, const std::string &type2,
                ShowFunctionPtr collisionFunction, bool symmetric = true) {
    show_map[std::make_pair(type1, type2)] = collisionFunction;
  }
  void removeEntry(const std::string &type1, const std::string &type2) {
    show_map.erase(std::make_pair(type1, type2));
  }
  ShowFunctionPtr lookup(const std::string &type1, const std::string &type2) {
    auto it = show_map.find(std::make_pair(type1, type2));
    if (it != show_map.end()) {
      return it->second;
    }

    return nullptr;
  }
  static ShowMap &theShowMap() {
    static ShowMap showMap;
    return showMap;
  }

 private:
  ShowMap() {}
  ShowMap(const ShowMap &) {}

  // C++标准并没有规定type_info::name的返回值，不同的实现，其行为会有区别。
  // 更好的设计是通过它所关联的type_info对象的地址了鉴别一个类，
  // 因为每个类关联的type_info对象肯定是不同的。
  // SHOWMAP于是应该被声明为 map<cont type_info*, ShowFunctionPtr>。
  typedef std::map<std::pair<std::string, std::string>, ShowFunctionPtr>
      SHOWMAP;
  SHOWMAP show_map;
};

// 必须确保在发生碰撞前就将映射关系加入了映射表，
// 一个方法是让B的子类在构造函数中进行确认，这将导致在运行期的一个小小的性能开销。
// 另外一个方法是创建一个RegisterShowFunction类：
class RegisterShowFunction {
 public:
  RegisterShowFunction(const std::string &type1, const std::string &type2,
                       ShowMap::ShowFunctionPtr showFunction,
                       bool symmetric = true) {
    ShowMap::theShowMap().addEntry(type1, type2, showFunction, symmetric);
  }
};
auto tryShow = [](B &b1, B &b2) {
  // std::cout << typeid(b1).name() << "," << typeid(b2).name() << std::endl;
  auto show =
      ShowMap::theShowMap().lookup(typeid(b1).name(), typeid(b2).name());
  if (show == nullptr) {
    std::cout << "unknown " << b1.get() << " " << b2.get() << std::endl;
  } else {
    show(b1, b2);
  }
};
void func1() {
  DA da1, da2;
  DB db1, db2;
  DC dc1, dc2;
  tryShow(da1, da2);  // unknown DA DA
  RegisterShowFunction("N2n42DAE", "N2n42DAE", &showAandA);
  tryShow(da1, da2);  // DA DA
  ShowMap::theShowMap().removeEntry("N2n42DAE", "N2n42DAE");
  tryShow(da1, da2);  // unknown DA DA

  RegisterShowFunction("N2n42DAE", "N2n42DAE", &showAandA);
  RegisterShowFunction("N2n42DAE", "N2n42DBE", &showAandB);
  RegisterShowFunction("N2n42DAE", "N2n42DCE", &showAandC);
  RegisterShowFunction("N2n42DBE", "N2n42DAE", &showBandA);
  RegisterShowFunction("N2n42DBE", "N2n42DBE", &showBandB);
  RegisterShowFunction("N2n42DBE", "N2n42DCE", &showBandC);
  RegisterShowFunction("N2n42DCE", "N2n42DAE", &showCandA);
  RegisterShowFunction("N2n42DCE", "N2n42DBE", &showCandB);
  RegisterShowFunction("N2n42DCE", "N2n42DCE", &showCandC);

  tryShow(da1, da2);  // DA DA
  tryShow(da1, db2);  // DA DB
  tryShow(da1, dc2);  // DA DC
  tryShow(db1, da2);  // DB DA
  tryShow(db1, db2);  // DB DB
  tryShow(db1, dc2);  // DB DC
  tryShow(dc1, da2);  // DC DA
  tryShow(dc1, db2);  // DC DB
  tryShow(dc1, dc2);  // DC DC
}
class DD : public B {  // unknown
 public:
  virtual std::string get() { return "DD"; }
};
class DB1 : public B {
 public:
  virtual std::string get() { return "DB1"; }
};
void func2() {
  DA da;
  DD dd;
  DB1 db1;
  tryShow(da, dd);   // unknown DA DD
  tryShow(da, db1);  // unknown DA DD

  // 这些新函数可以用同样方法加入映射表而不需要修改之前的代码：
  auto showAandD = [](B &b1, B &b2) {
    DA &da = dynamic_cast<DA &>(b1);
    DD &dd = dynamic_cast<DD &>(b2);
    std::cout << da.get() + " " + dd.get() << std::endl;
  };
  RegisterShowFunction("N2n42DAE", "N2n45test22DDE", showAandD);
  tryShow(da, dd);  // DA DD

  // 这不会改变实现多重调度没有完美解决方法的事实，
  // da和db1期望使用showAandB，但不行，必须使用新的函数重新注册：
  auto showAandB1 = [](B &b1, B &b2) {
    DA &da = dynamic_cast<DA &>(b1);
    DB1 &db1 = dynamic_cast<DB1 &>(b2);
    std::cout << da.get() + " " + db1.get() << std::endl;
  };
  RegisterShowFunction("N2n42DAE", "N2n45test23DB1E", showAandB1);
  tryShow(da, db1);  // DA DB1
}
void func() {
  func1();
  func2();
}
}  // namespace test2
}  // namespace n4

int main(int argc, char *argv[]) {
  if (argc < 2) {
    std::cout << argv[0] << " i [0 - 4]" << std::endl;
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
    case 3:
      n4::test1::func();
      break;
    case 4:
      n4::test2::func();
      break;
    default:
      std::cout << "invalid type" << std::endl;
      break;
  }

  return 0;
}