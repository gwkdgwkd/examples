#include <iostream>

// 智能（smart）指针
// 智能指针是一种外观和行为都被设计成与内建指针相类似的对象，不过它能提供更多的功能。
// 它们有许多应用的领域，包括资源管理和重复代码任务的自动化。
// 当使用智能指针替代C++的内建指针，就能控制下面这些方面的指针的行为：
// 1.构造和析构，可以决定建立智能指针时应该怎么做。
//   通常赋给智能指针缺省值0，避免出现令人头疼的未初始化的指针。
//   当指向某一对象的最后一个智能指针被释放时，
//   某些智能指针被设计成负责删除它们指向的对象，这样做对防止资源泄漏很有帮助。
// 2.拷贝和赋值，能对拷贝智能指针或有智能指针参与的赋值操作进行控制。
//   对于某些类型的智能指针来说，
//   期望的行为是自动拷贝它们所指向的对象或用对这些对象进行赋值操作，
//   也就是进deep copy，深层拷贝。
//   对于其它的一些智能指针来说，仅仅拷贝指针本身或对指针进行赋值操作。
//   还有一部分类型的智能指针根本就不允许这些操作。
//   无论认为应该如何去做，智能指针始终受控制。
// 3.Dereferencing，取出指针所指东西的内容。
//   当用户引用被智能指针所指的对象，会发生什么事情呢？可以自行决定。
//   例如可以用智能指针实现条款M17提到的lazy fetching方法。

namespace n1 {
// 智能指针从模板中生成，因为要与内建指针类似，必须是强类型的；
// 模板参数确定指向对象的类型，大多数智能指针模板看起来都象这样：
template <class T>
class SmartPtr {
 public:
  // 智能指针的的构造通常很简单：
  // 1.找到指向的对象（一般由智能指针构造函数的参数给出），
  //   让智能指针的内部成员dumb pointer指向它。
  // 2.如果没有找到对象，把内部指针设为0或发出一个错误信号（可以是异常）。
  SmartPtr(T* realPtr = 0) : pointee(realPtr) {}

  // 如果一个智能指针拥有它指向的对象，当它被释放时必须负责删除这个对象：
  ~SmartPtr() { delete pointee; }

  // 对于智能指针类来说，如果不允许进行拷贝和赋值操作，它们应该被声明为private。
  // 对于拷贝构造函数和赋值操作符来说，如果只拷贝内部的dumb pointer，
  // 会导致两个智能指针指向一个相同的对象，这是一个灾难。
  // 因为当释放时每个智能指针都会删除它们所指的对象。
  // 这意味着一个对象会被删除两次，结果是不可预测的，通常是灾难性的。
  // 另一种方法是通过调用new，建立一个所指对象的新拷贝。
  // 这确保了不会有许多指向同一个对象的智能指针，
  // 但是建立（以后还得释放）新对象会造成不可接受的性能损耗。
  // 并且不知道要建立什么类型的对象，
  // 因为SmartPtr<T>对象不一定指向类型T的对象，也可以指向T的派生类对象。
  // 虚拟构造函数可能帮助解决这个问题，
  // 但是好象不能把它们用在SmartPtr这样的通用类中。
  // 如果SmartPtr禁止拷贝和赋值，就可以消除这个问题，
  // 但是采用当SmartPtr被拷贝和赋值时，对象所有权随之被传递的方法，
  // 是一个更具灵活性的解决方案，auto_ptr就是这样做的：
  SmartPtr(SmartPtr& rhs) {  // 形参没有const
    pointee = rhs.pointee;   // 转移所有权
    rhs.pointee = 0;         // rhs不再拥有任何东西
  }
  SmartPtr& operator=(SmartPtr& rhs) {  // 形参没有const
    if (this == rhs) {
      return *this;  // 自我赋值，什么都不做
    }
    delete pointee;  // 一定要有，不然之前的对象将泄露
    pointee = rhs.pointee;
    rhs.pointee = 0;
    return *this;
  }

  // 两个dereference操作符被声明为const，
  // 是因为dereference一个智能指针时不会对其自身进行修改，
  // const型的智能指针才可以使用这两个操作符：
  T& operator*() const {
    // 注意返回类型是一个引用。
    // 如果返回对象，尽管编译器允许这么做，却可能导致灾难性后果。
    // 必须时刻牢记：
    // pointee不用必须指向T类型对象；它也可以指向T的派生类对象。
    // 如果此时operator*函数返回的是T类型对象而不是派生类对象的引用，
    // 函数实际上返回的是一个错误类型的对象，是多态失效。
    // 也就是说智能指针将不能支持虚拟函数，象这样的指针再智能也没有用。
    // 而返回一个引用还能够具有更高的效率，兼顾正确性与效率当然是一件好事。
    return *pointee;
  }
  T* operator->() const {
    // operator->的情况与operator*是相同的。
    // operator->仅能返回两种东西：
    // 一个指向某对象的dumb pointer或另一个智能指针。
    // 多数情况下，会返回一个普通dumb pointer。
    // 因为返回一个指针，通过operator->调用虚拟函数，其行为也是正确的。
    return pointee;
  }

  // 如果智能指针为null，返回0，否则返回非0：
  operator void*() { return (void*)pointee; }

 private:
  T* pointee;  // 智能指针所指的真正对象
};

class C {
 public:
  C(int d = 0) : data(d) { std::cout << "C" << std::endl; }
  ~C() { std::cout << "~C" << std::endl; }
  void print() { std::cout << data << std::endl; }

 private:
  int data;
};

void func1() {
  // 使用智能指针与使用dump pointer没有很大的差别，这表明了封装是非常有效的。
  SmartPtr<C> sp1(new C(3));
  sp1->print();
  (*sp1).print();

  SmartPtr<C> sp2(sp1);
  sp2->print();
  // sp1->print();  // 崩溃

  SmartPtr<C> sp3 = sp2;
  sp3->print();
  // sp2->print();  // 崩溃

  // C
  // 3
  // 3
  // 3
  // 3
  // ~C
}

// 因为当调用SmartPtr的拷贝构造函数时，对象的所有权被传递出去，
// 所以通过传值方式传递SmartPtr对象是一个很糟糕的方法：
void print1(std::ostream& s, SmartPtr<int> p) { s << *p << '\n'; }
// 当print1的参数p被初始化时（调用SmartPtr的拷贝构造函数），
// 实参指向对象的所有权被传递到给了p。
// 当print1结束执行后，p离开了作用域，
// 它的析构函数删除它指向的对象（就是原来ptr指向的对象）。
// 然而ptr已不再指向任何对象（它的dumb pointer是null），
// 所以调用print1以后任何试图使用它的操作都将产生未定义的行为。
// 只有在确实想把对象的所有权传递给一个临时的函数参数时，
// 才能通过传值方式传递SmartPtr，这种情况很少见。
// 这不是说不能把SmartPtr做为参数传递，这只意味着不能使用传值的方法。
// 可以通过const引用参数传递的SmartPtr：
void print2(std::ostream& s, const SmartPtr<int>& p) { s << *p << '\n'; }
// 在函数里，p是一个引用，而不是一个对象，所以不会调用拷贝构造函数初始化p。
// 当实参被传递到上面这个print2时，它还保留着所指对象的所有权，
// 调用print2以后还可以安全地使用实参。
// 所以通过const引用传递SmartPtr可以避免传值所产生的风险。
// 在拷贝和赋值中，把对象的所有权从一个智能指针传递到另一个中去，这种思想很有趣，
// 而且可能已经注意到拷贝构造函数和赋值操作符不同寻常的声明方法同样也很有趣。
// 这些函数的参数通常会带有const，但是SmartPtr的这些函数则没有。
// 实际上在拷贝和赋值中上述这些函数的代码修改了这些参数。
void func2() {
  SmartPtr<int> sp(new int(5));
  print2(std::cout, sp);          // 5
  std::cout << *sp << std::endl;  // 5
  print1(std::cout, sp);          // 5
  // std::cout << *sp << std::endl;  // 崩溃
}

void func3() {
  SmartPtr<int> sp1;
  SmartPtr<int> sp2(new int(6));

  if (sp1) {
    std::cout << "sp1 not null" << std::endl;
  } else {
    std::cout << "sp1 is null" << std::endl;
  }
  // sp1 is null

  if (sp2 == nullptr) {
    std::cout << "sp2 is null" << std::endl;
  } else {
    std::cout << "sp2 not null" << std::endl;
  }
  // sp2 not null

  // 如果没有operator void*()，下面的都编译不过：
  if (sp1 == 0)
    ;
  if (sp1)
    ;
  if (!sp1)
    ;

  SmartPtr<double> sp3;
  if (sp1 == sp3)
    ;
  // 即使在SmartPtr<int>和SmartPtr<double>之间没有operator==函数，
  // 也能够编译，因为智能指针被隐式地转换为void*指针，
  // 而对于内建指针类型有内建的比较函数，这种进行隐式类型转换的行为特性很危险。
}

// 对于很多程序来说，这就是需要了解智能指针的全部东西。
}  // namespace n1

namespace n2 {
// 测试智能指针是否为nullptr
// 有一件做不到的事情是判断智能指针为nullptr，这是一个严重的限制。
// 在智能指针类里加入一个isNull成员函数是一件很容易的事，
// 但会导致智能指针与dumb pointer行为不一致的问题。
// 另一种方法是提供隐式类型转换操作符，允许编译上述的测试。
// 一般应用于这种目的的类型转换是void*，象所有的类型转换函数一样，它有缺点：
// 在一些情况下虽然大多数程序员希望它调用失败，但是函数确实能够成功地被调用。
// 特别是它允许智能指针与完全不同的类型之间进行比较。
// 在void*类型转换方面，也有一些变化。
// 有些设计者采用到const void*的类型转换，还有一些采取转换到bool的方法。
// 这些变化都没有消除混合类型比较的问题。
// 有一种两全之策可以提供合理的测试nullptr值的语法形式，
// 同时把不同类型的智能指针之间进行比较的可能性降到最低。
// 这就是在智能指针类中重载operator!，
// 当且仅当智能指针是一个空指针时，operator!返回true：
template <class T>
class SmartPtr {
 public:
  SmartPtr(T* realPtr = 0) : pointee(realPtr) {}
  ~SmartPtr() { delete pointee; }

  // 当且仅当智能指针是空值，返回true：
  bool operator!() const { return pointee == nullptr; }
  operator bool() const { return pointee; }

 private:
  T* pointee;
};
void func() {
  SmartPtr<int> sp1;
  SmartPtr<int> sp2(new int(6));

  auto test1 = [](const SmartPtr<int>& sp, int i) {
    std::cout << "sp" << i << (!sp ? " == " : " != ") << "nullptr" << std::endl;
  };
  test1(sp1, 1);  // sp1 == nullptr
  test1(sp2, 2);  // sp2 != nullptr

  auto test2 = [](const SmartPtr<int>& sp, int i) {
    std::cout << "sp" << i << (sp.operator bool() ? " != " : " == ")
              << "nullptr" << std::endl;
  };
  test2(sp1, 1);  // sp1 == nullptr
  test2(sp2, 2);  // sp2 != nullptr

  // 如果没有operator bool()的话：
  SmartPtr<double> sp3;
  // if (sp3 == 0) ;// 错误
  // if (sp3)  // 错误
  // if (sp1 == sp3)  // 错误
  // 仅在这种情况下会存在不同类型之间进行比较：
  if (!sp1 == !sp3)  // 能够编译，幸好程序员不会经常这样编写代码
    ;
}

// 有趣的是，iostream库的实现除了提供void*隐式的类型转换，
// 也有operator!函数，不过这两个函数通常测试的流状态有些不同。
// 在C++类库标准中，void*隐式的类型转换已经被bool类型的转换所替代，
// operator bool总是返回与operator!相反的值。
}  // namespace n2

namespace n3 {
// 把智能指针转变成dumb指针
// 有时你要在一个程序里或已经使用dumb指针的程序库中添加智能指针。
template <class T>
class SmartPtr {
 public:
  SmartPtr(T* realPtr = 0) : pointee(realPtr) {}
  ~SmartPtr() { delete pointee; }
  operator T*() { return pointee; }

 private:
  T* pointee;
};
void print(int* p) { std::cout << *p << std::endl; }
void func1() {
  int i = 5;
  print(&i);  // 5

  SmartPtr<int> sp(new int(6));
  print(&*sp);  // 6，可以但繁琐
  print(sp);    // 6
}

void func2() {
  // 也消除了测试空值的问题：
  SmartPtr<int> sp1;
  SmartPtr<int> sp2(new int(7));
  if (sp1 == 0) {
    std::cout << "sp1 == nullptr" << std::endl;
  }
  if (!sp1) {
    std::cout << "sp1 == nullptr" << std::endl;
  }
  if (sp2) {
    std::cout << "sp2 != nullptr" << std::endl;
  }
  if (sp2 != nullptr) {
    std::cout << "sp2 != nullptr" << std::endl;
  }
  // sp1 == nullptr
  // sp1 == nullptr
  // sp2 != nullptr
  // sp2 != nullptr

  // 然而，类型转换函数也有类型转换函数所具有的缺点。
  // 它使得用户能够很容易地直接访问dumb指针，绕过了类指针对象所提供的智能特性：
  int* p = sp1;
}

class C {
 public:
  C(const int* p) {}
};
C merge(const C& c1, const C& c2) {}
void func3() {
  // 通常，智能指针提供的智能行为特性是设计中的主要组成部分，
  // 所以允许用户使用dumb指针会导致灾难性的后果。
  // 甚至即使提供一个从智能指针到dumb指针的隐式转换操作符，
  // 智能指针也不能真正地做到与dumb指针互换。
  // 因为从智能指针到dumb指针的转换是用户定义类型转换，
  // 在同一时间编译器进行这种转换的次数不能超过一次。
  int *p1, *p2;
  merge(p1, p2);
  SmartPtr<int>*sp1, *sp2;
  // merge(sp1, sp2);  // 报错
  // 因为SmartPtr<int>到C的转换要调用两次用户定义类型转换：
  // 1次SmartPtr<int>到int*，1次从int*到C，编译器不会进行这种序列的转换。
}

void func4() {
  // 提供到dumb指针的隐式类型转换的智能指针类也暴露了一个非常有害的bug：
  SmartPtr<int> sp = new int(5);
  delete sp;
  // free(): double free detected in tcache 2

  // 正常来说，这段代码应该不能被编译，sp是对象不是指针，不能delete一个对象。
  // 但由于定义了隐世类型转换符，编译器使用隐式类型转换来尽可能使函数调用成功；
  // 再回想一下条款M8：
  // 使用delete会调用析构函数和operator delete，两者都是函数。
  // 编译器为了使在delete sp语句里的两个函数成功调用，
  // 就把sp隐式转换为int*，然后删除它。
  // 本来是写错了代码，而现在却编译过了，这样做必然会破坏程序。
  // 如果sp拥有它指向的对象，对象就会被删除两次，
  // 一次在调用delete时，第二次在sp的析构函数被调用时。
  // 当sp不拥有对象，而是其它人拥有时，如果拥有者同时负责删除sp的则情况还好，
  // 但是如果拥有者不是负责删除sp的人，可以预料它以后还会再次删除该对象。
  // 不论是最前者所述的情况还是最后者的情况都会导致一个对象被删除两次，
  // 这样做会产生不能预料的后果。
  // 这个bug极为有害，因为隐藏在智能指针后面的全部思想就是：
  // 让它们不论是在外观上还是在使用感觉上都与dumb指针尽可能地相似。
  // 你越接近这种思想，你的用户就越可能忘记正在使用智能指针。
  // 如果他们忘记了正在使用智能指针，肯定会在调用new之后调用delete，
  // 以防止资源泄漏，谁又能责备他们这样做不对呢？
  // 底线很简单：
  // 除非有一个让人非常信服的原因去这样做，
  // 否则绝对不要提供转换到dumb指针的隐式类型转换操作符。
}

void func() {
  func1();
  func2();
  func3();
  func4();
}
}  // namespace n3

namespace n4 {
// 智能指针和继承类到基类的类型转换

class Base {
 public:
  virtual void show() const = 0;
};
class D1 : public Base {
 public:
  virtual void show() const { std::cout << "D1::show" << std::endl; }
};
class D2 : public Base {
 public:
  virtual void show() const { std::cout << "D2::show" << std::endl; }
};

namespace test1 {
void show(const Base* pmp, int numTimes) {
  for (int i = 1; i <= numTimes; ++i) {
    pmp->show();
  }
}
void func1() {
  D1* d1 = new D1;
  D2* d2 = new D2;
  show(d1, 2);
  show(d2, 2);

  // D1::show
  // D1::show
  // D2::show
  // D2::show
}

void show(const n1::SmartPtr<Base>& pmp, int numTimes) {
  for (int i = 1; i <= numTimes; ++i) {
    pmp->show();
  }
}
void func2() {
  n1::SmartPtr<D1> d1ptr(new D1);
  n1::SmartPtr<D2> d2ptr(new D2);
  // show(d1tr, 2);  // 报错
  // show(d2ptr, 2);  // 报错

  // 既然智能指针这么聪明，为什么不能编译这些代码呢？
  // 因为不能把SmartPtr<D1>或SmartPtr<D2>转换成SmartPtr<Base>。
  // 从编译器的观点来看，这些类之间没有任何关系。
  // 为什么编译器的会这样认为呢？
  // 毕竟SmartPtr<D1>或SmartPtr<D2>不是从SmartPtr<Base>继承过来的，
  // 这些类之间没有继承关系，编译器不能把对象转换成完全不同的另一种类型的对象。
}

void func() {
  test1::func1();
  test1::func2();
}
}  // namespace test1

namespace test2 {
// 幸运的是，有办法避开这种限制，这种方法的核心思想（不是实际操作）很简单：
// 对于可以进行隐式转换的每个智能指针类型都提供一个隐式类型转换操作符。
template <class T>
class SmartPtr {
 public:
  SmartPtr(T* realPtr = 0) : pointee(realPtr) {}
  SmartPtr(const SmartPtr& rhs) {}
  ~SmartPtr() { delete pointee; }
  SmartPtr& operator=(const SmartPtr& rhs) {}
  T* operator->() const { return pointee; }
  T& operator*() const { return *pointee; }
  operator T*() { return pointee; }

 private:
  T* pointee;
};
template <>
class SmartPtr<D1> {
 public:
  SmartPtr(D1* realPtr = 0) : pointee(realPtr) {}
  operator SmartPtr<Base>() { return SmartPtr<Base>(pointee); }

 private:
  D1* pointee;
};
template <>
class SmartPtr<D2> {
 public:
  SmartPtr(D2* realPtr = 0) : pointee(realPtr) {}
  operator SmartPtr<Base>() { return SmartPtr<Base>(pointee); }

 private:
  D2* pointee;
};
void show(const SmartPtr<Base>& pmp, int numTimes) {
  for (int i = 1; i <= numTimes; ++i) {
    pmp->show();
  }
}
void func() {
  SmartPtr<D1> d1ptr(new D1);
  SmartPtr<D2> d2ptr(new D2);
  show(d1ptr, 2);
  show(d2ptr, 2);

  // D1::show
  // D1::show
  // D2::show
  // D2::show
}

// 这种方法有两个缺点：
// 1.必须人为地特化SmartPtr类，所以加入隐式类型转换操作符也就破坏了模板的通用性。
// 2.可能必须添加许多类型转换符，因为指向的对象可以位于继承层次中很深的位置，
//   必须为直接或间接继承的每一个基类提供一个类型转换符。
//   如果认为能够克服这个缺点，方法是仅仅为转换到直接基类而提供一个隐式类型转换符，
//   那么请再想想这样做行么？
//   因为编译器在同一时间调用用户定义类型转换函数的次数不能超过一次，
//   它们不能把指向T的智能指针转换为指向cT的间接基类的智能指针，除非一步就能完成。
}  // namespace test2

namespace test3 {
template <class T>
class SmartPtr {
 public:
  SmartPtr(T* realPtr = 0) : pointee(realPtr) {}
  SmartPtr(const SmartPtr& rhs) {}
  ~SmartPtr() { delete pointee; }
  SmartPtr& operator=(const SmartPtr& rhs) {}
  T* operator->() const { return pointee; }
  T& operator*() const { return *pointee; }
  operator T*() { return pointee; }

  // 感谢最近的语言扩展，可以声明（非虚）成员函数模板，通常就叫成员模板。
  // 这让编译器能编写所有的类型转换函数，这会节省很多时间：
  template <class newType>
  operator SmartPtr<newType>() {
    return SmartPtr<newType>(pointee);
  }
  // 编译器首先检查SmartPtr<T>的类定义，看其有没有声明明确的类型转换符，但是没有。
  // 这不是指：在上面的模板没有声明类型转换符。
  // 编译器然后检查是否存在一个成员函数模板，并可以被实例化成它所期望的类型转换。
  // 它发现了一个这样的模板，所以它把newType绑定成T的基类类型来实例化模板。
  // 这时，惟一的问题是实例化的成员函数代码能否被编译：
  // 传递dumb pointee到指向T的基类的智能指针的构造函数，必须合法的。
  // 指针pointee是指向T类型的，
  // 把它转变成指向其基类（public或protected）对象的指针必然是合法的，
  // 因此类型转换操作符能够被编译，
  // 可以成功地把指向T的智能指针隐式地类型转换为指向T的基类的智能指针。

 private:
  T* pointee;
};
void show(const SmartPtr<Base>& pmp, int numTimes) {
  for (int i = 1; i <= numTimes; ++i) {
    pmp->show();
  }
}
void func1() {
  SmartPtr<D1> d1ptr(new D1);
  SmartPtr<D2> d2ptr(new D2);
  show(d1ptr, 2);
  show(d2ptr, 2);

  // D1::show
  // D1::show
  // D2::show
  // D2::show

  // 复制构造函数没写好，导致崩溃：
  // free(): double free detected in tcache 2
}

// 不要被这个例子误导，而认为这种方法只能用于把指针在继次中向上进行类型转换。
// 这种方法可以成功地用于任何合法的指针类型转换。
// 如果有dumb指针T1*和另一种dumb指针T2*，当且仅当能隐式地把T1*转换为T2*时，
// 就能够隐式地把指向T1的智能指针类型转换为指向T2的智能指针类型。
// 这种技术能给出几乎所有想要的行为特性。
class DD : public D1 {
 public:
  virtual void show() const { std::cout << "DD::show" << std::endl; }
};
void show(const SmartPtr<D1>& pmp, int numTimes) {
  for (int i = 1; i <= numTimes; ++i) {
    pmp->show();
  }
}
void func2() {
  SmartPtr<DD> dd(new DD);
  // show(dd, 2);  // 报错

  // 期望调用SmartPtr<D1>，因为DD是直接从D1继承的，而间接继承自Base。
  // 当然这是dumb指针时的工作方法。
  // 智能指针不会这么智能，它们的转换操作符是成员函数，
  // 对C++编译器而言，所有类型转换操作符是同等地位的。
  // 因此show的调用具有二义性，因为从SmartPtr<DD>到SmartPtr<D1>的类型转换，
  // 并不比到SmartPtr<Base>的类型转换优先。
}
void func() {
  func1();
  func2();
}

// 通过成员模板来实现智能指针的类型转换有两个缺点：
// 1.支持成员模板的编译器较少，所以这种技术不具有可移植性。
//   以后情况会有所改观，但是没有人知道这会等到什么时候。
// 2.这种方法的工作原理不很明了，要理解它必须先要深入理解函数调用时的参数匹配，
//   隐式类型转换函数，模板函数隐式实例化和成员函数模板。
//   有些程序员以前从来没有看到过这种技巧，
//   而却被要求维护使用了这种技巧的代码，真是很可怜他们。
//   这种技巧确实很巧妙，这自然是肯定，但是过于的巧妙可是一件危险的事情。
// 直接了当地说，如何能够让智能指针的行为与dumb指针一样呢？
// 答案很简单：不可能，智能指针固然智能，但不是指针。
// 最好的方法是使用成员模板生成类型转换函数，在会产生二义性结果的地方使用cast。
// 这不是一个完美的方法，不过已经很不错了，在一些情况下需去除二义性，
// 所付出的代价与智能指针提供复杂的功能相比还是值得的。
}  // namespace test3
}  // namespace n4

namespace n5 {
// 智能指针和const

int i = 4;
namespace test1 {
// 对于dumb指针来说，const既可以描述指针有可以描述所指向的东西：
const int* p1;
int const* p2;
int* const p3 = &i;
const int* const p4 = &i;
int const* const p5 = &i;

using n1::SmartPtr;
// 自然想要让智能指针具有同样的灵活性，不幸的是只能在一个地方放置const，
// 并只能对指针本身起作用，而不能针对于所指对象：
const SmartPtr<int> p6(&i);
// p6是一个const智能指针指向non-const对象。

// 好像有一个简单的补救方法，就是建立一个指向cosnt的智能指针：
SmartPtr<const int> p7(&i);
// p7是一个non-const智能指针指向const对象。

// 现在可以建立const和non-const对象和指针的四种不同组合：
SmartPtr<int> p8;                   // non-const对象，non-const指针
SmartPtr<const int> p9;             // const对象，non-const指针
const SmartPtr<int> p10(&i);        // non-const对象，const指针
const SmartPtr<const int> p11(&i);  // const对象，const指针

// 但是美中不足的是，使用dumb指针能够用non-const指针初始化const指针，
// 也能用指向non-cosnt对象的指针初始化指向const对象的指针，
// 就像进行赋值一样，例如：
int* p12 = new int(5);
const int* p13 = p12;  // 正确
// 但是如果试图把这种方法用在智能指针上，情况会怎么样呢？
SmartPtr<int> p14(new int(6));
// SmartPtr<const int> p15 = p14; // 报错
// SmartPtr<const int> p15(p14); // 报错
// SmartPtr<int>与SmartPtr<const int>是完全不同的类型。
// 在编译器看来，它们是毫不相关的，所以没有理由相信它们是赋值兼容的。
}  // namespace test1

namespace test2 {
// 到目前为止这是一个老问题了，把它们变成赋值兼容的唯一方法是必须提供函数，
// 用来把SmartPtr<int>类型的对象转换成SmartPtr<const int>类型。
// 如果使用的编译器支持成员模板，就可以自动生成需要的隐式类型转换操作。
// 带const的类型转换是单向的：
// 从non-const到const的转换是安全的，但是从const到non-const是不安全的。
// 而且用const指针能做的事情，用non-const指针也能做，
// 但是用non-const指针还能做其它一些事情（例如，赋值操作）。
// 同样，用指向const对象的指针能做的任何事情，用指向non-const对象的指针也能做到，
// 但是用指向non-const对象的指针能够完成一些，
// 指向const对象的指针所不能完成的事情（例如，赋值操作）。
// 这些规则看起来与public继承的规则相类似。
// 能够把一个派生类对象转换成基类对象，但是反之则不是这样，
// 对基类所做的任何事情对派生类也能做，但是还能对派生类做另外一些事情。
// 能够利用这一点来实现智能指针，就是说可以让每个指向T的智能指针类，
// public派生自一个对应的指向const-T的智能指针类：
template <class T>  // 指向const对象的智能指针
class SmartPtrToConst {
 protected:
  union {
    const T* constPointee;  // 让SmartPtrToConst访问
    T* pointee;             // 让SmartPtr访问
  };
};
template <class T>  // 指向non-const对象的智能指针
class SmartPtr : public SmartPtrToConst<T> {
 public:
  SmartPtr(T* realPtr = 0) {}
};

// 使用这种方法，指向non-const-T对象的智能指针包含一个指向const-T的dumb指针，
// 指向const-T的智能指针需要包含一个指向const-T的dumb指针。
// 最方便的方法是把指向const-T的dumb指针放在基类里，
// 把指向non-const-T的dumb指针放在派生类里，
// 然而这样做有些浪费，因为SmartPtr对象包含两个dumb指针:
// 一个是从SmartPtrToConst继承来的，一个是SmartPtr自己的。
// 一种在C世界里的老式武器可以解决这个问题，这就是union，它在C++中同样有用。
// union在protected中，所以所以两个类都可以访问它，
// 它包含两个必须的dumb指针类型，这就是union美丽的地方。
// 当然两个类的成员函数必须约束它们自己仅仅使用适合的指针，这是使用union所冒的风险。
// 利用这种设计，能够获得所要的行为特性：
SmartPtr<int> p1(new int(6));
// SmartPtrToConst<const int> p2(p1);
}  // namespace test2
}  // namespace n5

// 智能指针如此繁琐麻烦，是否值得使用，特别是如果编译器缺乏支持成员函数模板时。
// 通常是值得的，例如通过使用智能指针极大地简化了条款M29中的引用计数代码。
// 而且正如该例子所显示的，智能指针的使用在一些领域受到极大的限制，
// 例如测试空值、转换到dumb指针、继承类向基类转换和对指向const的指针的支持。
// 同时智能指针的实现、理解和维护需要大量的技巧。
// 调试使用智能指针的代码也比调试使用dumb指针的代码困难。
// 无论如何也不可能设计出一种通用目的的智能指针，能够替代dumb指针。
// 达到同样的代码效果，使用智能指针更方便。
// 智能指针应该谨慎使用，不过每个C++程序员最终都会发现它们是有用的。

int main(int argc, char* argv[]) {
  if (argc < 2) {
    std::cout << argv[0] << " i [0 - 8]" << std::endl;
    return 0;
  }
  int type = argv[1][0] - '0';
  switch (type) {
    case 0:
      n1::func1();
      break;
    case 1:
      n1::func2();
      break;
    case 2:
      n1::func3();
      break;
    case 3:
      n2::func();
      break;
    case 4:
      n3::func();
      break;
    case 5:
      n4::test1::func();
      break;
    case 6:
      n4::test2::func();
      break;
    case 7:
      n4::test3::func();
      break;
    default:
      std::cout << "invalid type" << std::endl;
      break;
  }

  return 0;
}