#include <iostream>

using namespace std;

// 灵巧（smart）指针
// 灵巧指针是一种外观和行为都被设计成与内建指针相类似的对象，不过它能提供更多的功能。它们有许多应用的领域，包括资源管理和重复代码任务的自动化。
// 当你使用灵巧指针替代C++的内建指针（也就是dumb pointer）,你就能控制下面这些方面的指针的行为：
// 1 构造和析构。你可以决定建立灵巧指针时应该怎么做。通常赋给灵巧指针缺省值0，避免出现令人头疼的未初始化的指针。
//   当指向某一对象的最后一个灵巧指针被释放时，某些灵巧指针被设计成负责删除它们指向的对象。
//   这样做对防止资源泄漏很有帮助。
// 2 拷贝和赋值。你能对拷贝灵巧指针或有灵巧指针参与的赋值操作进行控制。
//   对于某些类型的灵巧指针来说，期望的行为是自动拷贝它们所指向的对象或用对这些对象进行赋值操作，也就是进 deep copy(深层拷贝)。
//   对于其它的一些灵巧指针来说，仅仅拷贝指针本身或对指针进行赋值操作。还有一部分类型的灵巧指针根本就不允许这些操作。
//   无论你认为应该如何去做，灵巧指针始终受你的控制。
// 3 Dereferencing(取出指针所指东西的内容)。当用户引用被灵巧指针所指的对象，会发生什么事情呢？
//   你可以自行决定。例如你可以用灵巧指针实现条款M17提到的lazy fetching方法。

// 灵巧指针从模板中生成，因为要与内建指针类似，必须是 strongly typed(强类型)的；模板参数确定指向对象的类型。大多数灵巧指针模板看起来都象这样：
template <class T>
class SmartPtr {
  // 拷贝构造函数和赋值操作符都被展现在这里。
 public:
  // 建立一个灵巧指针指向dumb pointer所指的对象。未初始化的指针缺省值为0(null)
  SmartPtr(T* realPtr = 0) {}
  SmartPtr(const SmartPtr& rhs);  // 拷贝一个灵巧指针
  ~SmartPtr();                    // 释放灵巧指针
  SmartPtr& operator=(const SmartPtr& rhs);
  T* operator->() const;  // dereference一个灵巧指针以访问所指对象的成员
  T& operator*() const;  // dereference灵巧指针
  operator void*() {}  // 如果灵巧指针为null，返回0，否则返回非0
 private:
  T* pointee;  // 灵巧指针所指的对象
};
// 对于灵巧指针类来说，不能允许进行拷贝和赋值操作，它们应该被声明为 private。
// 两个dereference操作符被声明为const，是因为dereference一个灵巧指针时不会对其自身进行修改（尽管可以修改指针所指的对象）（const型的灵巧指针才可以使用这两个操作符）。
// 最后，每个指向T对象的灵巧指针包含一个指向T的dumb pointer。这个dumb pointer指向的对象才是灵巧指针指向的真正对象。

// 使用灵巧指针与使用dump pointer没有很大的差别。这表明了封装是非常有效的。灵巧指针的用户可以象使用dumb pointer一样使用灵巧指针。有时这种替代会更透明化。

// 灵巧指针的构造、赋值和析构
// 灵巧指针的的构造通常很简单：
//  找到指向的对象（一般由灵巧指针构造函数的参数给出），让灵巧指针的内部成员dumb pointer指向它。
//  如果没有找到对象，把内部指针设为0或发出一个错误信号（可以是抛出一个异常）。

// 灵巧指针拷贝构造函数、赋值操作符函数和析构函数的实现由于（所指对象的）所有权的问题所以有些复杂。
//  如果一个灵巧指针拥有它指向的对象，当它被释放时必须负责删除这个对象。这里假设灵巧指针指向的对象是动态分配的。
template <class T>
class auto_ptr {
 public:
  auto_ptr(T* ptr = 0) : pointee(ptr) {}
  ~auto_ptr() { delete pointee; }
  auto_ptr(auto_ptr<T>& rhs);                // 拷贝构造函数
  auto_ptr<T>& operator=(auto_ptr<T>& rhs);  // 赋值操作符
  T* operator->() const {}  // dereference一个灵巧指针以访问所指对象的成员
  T& operator*() const {}  // dereference灵巧指针

 private:
  T* pointee;
};
//  如果我们只拷贝内部的dumb pointer，会导致两个auto_ptr指向一个相同的对象。这是一个灾难，因为当释放quto_ptr时每个auto_ptr都会删除它们所指的对象。
//  这意味着一个对象会被我们删除两次。这种两次删除的结果将是不可预测的（通常是灾难性的）。
//  另一种方法是通过调用new，建立一个所指对象的新拷贝。这确保了不会有许多指向同一个对象的auto_ptr，但是建立（以后还得释放）新对象会造成不可接受的性能损耗。
//  并且我们不知道要建立什么类型的对象，因为 auto_ptr<T>对象不用必须指向类型为T的对象，它也可以指向T的派生类型对象。虚拟构造函数可能帮助我们解决这个问题，
//  但是好象不能把它们用在auto_ptr这样的通用类中。
//  如果quto_ptr禁止拷贝和赋值，就可以消除这个问题，但是采用“当auto_ptr被拷贝和赋值时，对象所有权随之被传递”的方法，是一个更具灵活性的解决方案：
template <class T>
auto_ptr<T>::auto_ptr(auto_ptr<T>& rhs) {
  pointee = rhs.pointee;  // 把*pointee的所有权传递到*this
  rhs.pointee = 0;        // rhs不再拥有任何东西
}
template <class T>
auto_ptr<T>& auto_ptr<T>::operator=(auto_ptr<T>& rhs) {
  if (this == &rhs)  // 如果这个对象自我赋值什么也不要做
    return *this;
  // 注意赋值操作符在接受新对象的所有权以前必须删除原来拥有的对象。如果不这样做，原来拥有的对象将永远不会被删除。
  delete pointee;         // 删除现在拥有的对象
  pointee = rhs.pointee;  // 把*pointee的所有权从rhs传递到*this
  rhs.pointee = 0;
  return *this;
}
//  因为当调用auto_ptr的拷贝构造函数时，对象的所有权被传递出去，所以通过传值方式传递auto_ptr对象是一个很糟糕的方法。
void printTreeNode(ostream& s, auto_ptr<int> p) { s << *p; }
//  当printTreeNode的参数p被初始化时（调用auto_ptr的拷贝构造函数），ptn指向对象的所有权被传递到给了p。
//  当printTreeNode结束执行后，p离开了作用域，它的析构函数删除它指向的对象（就是原来ptr指向的对象）。
//  然而ptr已不再指向任何对象（它的dumb pointer是null），所以调用printTreeNode以后任何试图使用它的操作都将产生未定义的行为。
//  只有在你确实想把对象的所有权传递给一个临时的函数参数时，才能通过传值方式传递auto_ptr。这种情况很少见。
//  这不是说你不能把auto_ptr做为参数传递，这只意味着不能使用传值的方法。通过const引用传递（Pass-by-reference-to-const）的方法是这样的：
void printTreeNode1(ostream& s, const auto_ptr<int>& p) { s << *p; }
//  在函数里，p是一个引用，而不是一个对象，所以不会调用拷贝构造函数初始化p。
//  当ptn被传递到上面这个printTreeNode时，它还保留着所指对象的所有权，调用printTreeNode以后还可以安全地使用ptn。
//  所以通过const引用传递auto_ptr可以避免传值所产生的风险。
//  在拷贝和赋值中，把对象的所有权从一个灵巧指针传递到另一个中去，这种思想很有趣，而且你可能已经注意到拷贝构造函数和赋值操作符不同寻常的声明方法同样也很有趣。
//  这些函数的参数通常会带有const，但是上面这些函数则没有。实际上在拷贝和赋值中上述这些函数的代码修改了这些参数。
//  也就是说，如果auto_ptr对象被拷贝或做为赋值操作的数据源，就会修改auto_ptr对象！
//  灵巧指针的析构函数通常是这样的：
template <class T>
SmartPtr<T>::~SmartPtr() {
  // if的条件我写的，不对
  if ((void*)this == (void*)pointee) {  // *this owns * pointee
    delete pointee;
  }
}

// 实现Dereference操作符
// 把注意力转向灵巧指针的核心部分，operator*和operator->函数。前者返回所指的对象。理论上，这很简单：
template <class T>
T& SmartPtr<T>::operator*() const {
  // perform "smart pointer" processing;
  return *pointee;
}
// 注意返回类型是一个引用。如果返回对象，尽管编译器允许这么做，却可能导致灾难性后果。
// 必须时刻牢记：pointee不用必须指向T类型对象；它也可以指向T的派生类对象。
// 如果在这种情况下operator*函数返回的是T类型对象而不是派生类对象的引用，你的函数实际上返回的是一个错误类型的对象！
// 在返回的这种对象上调用虚拟函数，不会触发与（原先）所指对象的动态类型相符的函数。实际上就是说你的灵巧指针将不能支持虚拟函数，象这样的指针再灵巧也没有用。
// 而返回一个引用还能够具有更高的效率。能够兼顾正确性与效率当然是一件好事。
// operator->的情况与operator*是相同的。
// operator->仅能返回两种东西：一个指向某对象的dumb pointer或另一个灵巧指针。多数情况下，你想返回一个普通dumb pointer。在此情况下，你这样实现operator->:
template <class T>
T* SmartPtr<T>::operator->() const {
  // perform "smart pointer" processing;
  return pointee;
}
// 这样做运行良好。因为该函数返回一个指针，通过operator->调用虚拟函数，其行为也是正确的。
// 对于很多程序来说，这就是你需要了解灵巧指针的全部东西。

// 如果你想更深入地了解灵巧指针，你必须知道更多的有关dumb pointer的知识和灵巧指针如何能或不能进行模拟dumb pointer。

// 测试灵巧指针是否为NULL
// 有一件我们做不到的事情是“发现灵巧指针为NULL”。这是一个严重的限制。
// 在灵巧指针类里加入一个isNull 成员函数是一件很容易的事，但是没有解决当测试NULL时灵巧指针的行为与dumb pointer不相似的问题。
// 另一种方法是提供隐式类型转换操作符，允许编译上述的测试。一般应用于这种目的的类型转换是void*。
// 象所有的类型转换函数一样，它有一个缺点：在一些情况下虽然大多数程序员希望它调用失败，但是函数确实能够成功地被调用。
// 特别是它允许灵巧指针与完全不同的类型之间进行比较。
// 在void*类型转换方面，也有一些变化。有些设计者采用到const void*的类型转换，还有一些采取转换到bool的方法。这些变化都没有消除混合类型比较的问题。
// 有一种两全之策可以提供合理的测试null值的语法形式，同时把不同类型的灵巧指针之间进行比较的可能性降到最低。
// 这就是在灵巧指针类中重载operator!，当且仅当灵巧指针是一个空指针时，operator!返回true：
template <class T>
class SmartPtr1 {
 public:
  bool operator!() const {}  // 当且仅当灵巧指针是空值，返回true。
 private:
  T* pointee;  // 灵巧指针所指的对象
};
// 有趣的是，iostream库的实现除了提供void*隐式的类型转换，也有operator!函数，不过这两个函数通常测试的流状态有些不同。
// 在C++类库标准中，void*隐式的类型转换已经被bool类型的转换所替代，operator bool总是返回与operator!相反的值。

// 把灵巧指针转变成dumb指针
// 有时你要在一个程序里或已经使用dumb指针的程序库中添加灵巧指针。
class Tuple {};
void normalize(Tuple* pt) {}
// 在灵巧指针模板中增加指向T的dumb指针的隐式类型转换操作符，可以让以上函数使用灵巧指针调用成功运行：
template <class T>
class SmartPtr2 {
 public:
  SmartPtr2(T* realPtr = 0) {}
  SmartPtr2(const SmartPtr2& rhs) {}  // 拷贝一个灵巧指针
  ~SmartPtr2() {}                     // 释放灵巧指针
  SmartPtr2& operator=(const SmartPtr2& rhs) {}
  T* operator->() const {}
  T& operator*() const {}
  operator T*() { return pointee; }

 private:
  T* pointee;  // 灵巧指针所指的对象
};
// 并且这个函数也消除了测试空值的问题。
// 然而，它也有类型转换函数所具有的缺点。它使得用户能够很容易地直接访问dumb指针，绕过了“类指针（pointer-like）”对象所提供的“灵巧”特性:
void processTuple(SmartPtr2<Tuple>& pt) {
  Tuple* rawTuplePtr = pt;  // SmartPtr2<Tuple>转变成Tuple*
  // 使用raw TuplePtr修改tuple;
}
// 通常，灵巧指针提供的“灵巧”行为特性是设计中的主要组成部分，所以允许用户使用dumb指针会导致灾难性的后果。
// 甚至即使你提供一个从灵巧指针到dumb指针的隐式转换操作符，灵巧指针也不能真正地做到与dumb指针互换。
// 因为从灵巧指针到dumb指针的转换是“用户定义类型转换”，在同一时间编译器进行这种转换的次数不能超过一次。
class TupleAccessors {
 public:
  TupleAccessors(const Tuple* pt) {}  // pt identifies the
};
TupleAccessors merge(const TupleAccessors& ta1, const TupleAccessors& ta2) {}

// 灵巧指针和继承类到基类的类型转换
class MusicProduct {
 public:
  MusicProduct() {}
  MusicProduct(const string& title) {}
  virtual void play() const = 0;
  virtual void displayTitle() const = 0;
};
class Cassette : public MusicProduct {
 public:
  Cassette() {}
  Cassette(const string& title) {}
  virtual void play() const {}
  virtual void displayTitle() const {}
};
class CD : public MusicProduct {
 public:
  CD(const string& title) {}
  virtual void play() const {}
  virtual void displayTitle() const {}
};
void displayAndPlay(const MusicProduct* pmp, int numTimes) {
  for (int i = 1; i <= numTimes; ++i) {
    pmp->displayTitle();
    pmp->play();
  }
}
void displayAndPlay1(const SmartPtr<MusicProduct>& pmp, int numTimes) {
  for (int i = 1; i <= numTimes; ++i) {
    pmp->displayTitle();
    pmp->play();
  }
}

// 幸运的是，有办法避开这种限制，这种方法的核心思想很简单：对于可以进行隐式转换的每个灵巧指针类型都提供一个隐式类型转换操作符。
// 例如在music类层次内，在Cassette和CD的灵巧指针类内你可以加入operator SmartPtr<MusicProduct>函数：
template <class T>
class SmartPtr3 {
 public:
  SmartPtr3(T* realPtr = 0) {}
  SmartPtr3(const SmartPtr3& rhs) {}  // 拷贝一个灵巧指针
  ~SmartPtr3() {}                     // 释放灵巧指针
  SmartPtr3& operator=(const SmartPtr3& rhs) {}
  T* operator->() const {}
  T& operator*() const {}
  operator T*() { return pointee; }

 private:
  T* pointee;  // 灵巧指针所指的对象
};
template <>
class SmartPtr3<Cassette> {
 public:
  operator SmartPtr3<MusicProduct>() {
    return SmartPtr3<MusicProduct>(pointee);
  }

 private:
  Cassette* pointee;
};
template <>
class SmartPtr3<CD> {
 public:
  operator SmartPtr3<MusicProduct>() {
    return SmartPtr3<MusicProduct>(pointee);
  }

 private:
  CD* pointee;
};
void displayAndPlay2(const SmartPtr3<MusicProduct>& pmp, int howMany) {}
// 这种方法有两个缺点:
//  第一，你必须人为地特化（specialize）SmartPtr类，所以你加入隐式类型转换操作符也就破坏了模板的通用性。
//  第二，你可能必须添加许多类型转换符，因为你指向的对象可以位于继承层次中很深的位置，你必须为直接或间接继承的每一个基类提供一个类型转换符。
//       如果你认为你能够克服这个缺点，方法是仅仅为转换到直接基类而提供一个隐式类型转换符，那么请再想想这样做行么？
//       因为编译器在同一时间调用用户定义类型转换函数的次数不能超过一次，它们不能把指向T的灵巧指针转换为指向cT的间接基类的灵巧指针，除非只要一步就能完成。
template <class T>
class SmartPtr4 {
 public:
  SmartPtr4(T* realPtr = 0) {}
  SmartPtr4(const SmartPtr4& rhs) {}  // 拷贝一个灵巧指针
  ~SmartPtr4() {}                     // 释放灵巧指针
  SmartPtr4& operator=(const SmartPtr4& rhs) {}
  T* operator->() const {}
  T& operator*() const {}
  operator T*() { return pointee; }

  // 如果你能让编译器为你编写所有的类型转换函数，这会节省很多时间。
  // 感谢最近的语言扩展，让你能够做到，这个扩展能声明（非虚）成员函数模板（通常就叫成员模板（member template))，你能使用它来生成灵巧指针类型转换函数，如下：
  template <class newType>
  operator SmartPtr4<newType>() {  // 为了实现隐式类
    return SmartPtr4<newType>(pointee);
  }
  // 编译器首先检查SmartPtr<T>的类定义，看其有没有声明明确的类型转换符，但是它没有声明。（这不是指：在上面的模板没有声明类型转换符。）
  // 编译器然后检查是否存在一个成员函数模板，并可以被实例化成它所期望的类型转换。
  // 它发现了一个这样的模板（带有形式类型参数newType），所以它把newType绑定成T的基类类型来实例化模板。这时，惟一的问题是实例化的成员函数代码能否被编译：
  // 传递（dumb）指针pointee到指向“T的基类”的灵巧指针的构造函数，必须合法的。指针pointee是指向T类型的，把它转变成指向其基类（public或protected）对象的指针
  // 必然是合法的，因此类型转换操作符能够被编译，可以成功地把指向T的灵巧指针隐式地类型转换为指向“T 的基类”的灵巧指针。

 private:
  T* pointee;  // 灵巧指针所指的对象
};
void displayAndPlay3(const SmartPtr4<MusicProduct>& pmp, int howMany) {}
// 不要被这个例子误导，而认为这种方法只能用于把指针在继次中向上进行类型转换。这种方法可以成功地用于任何合法的指针类型转换。
// 如果你有dumb指针T1*和另一种dumb指针T2*，当且仅当你能隐式地把T1*转换为T2*时，你就能够隐式地把指向T1的灵巧指针类型转换为指向T2的灵巧指针类型。
// 这种技术能给我们几乎所有想要的行为特性。假设我们用一个新类CasSingle来扩充MusicProduct类层次：
class CasSingle : public Cassette {
 public:
  CasSingle(const string& title) {}
};
// void displayAndPlay3(const SmartPtr4<Cassette>& pmp, int howMany) {}
// displayAndPlay被重载，一个函数带有SmartPtr<Cassette> 对象参数,其他函数的参数为SmartPtr<CasSingle>，我们期望调用SmartPtr<Cassette>，
// 因为CasSingle是直接从Cassette上继承下来的，而间接继承自MusicProduct。
// 当然这是dumb指针时的工作方法。我们的灵巧指针不会这么灵巧，它们的转换操作符是成员函数，对C++编译器而言，所有类型转换操作符是同等地位的。
// 因此displayAndPlay的调用具有二义性，因为 从SmartPtr<CasSingle>到SmartPtr<Cassette>的类型转换并不比到SmartPtr<MusicProduct>的类型转换优先。
// 通过成员模板来实现灵巧指针的类型转换有两个缺点：
//  第一，支持成员模板的编译器较少，所以这种技术不具有可移植性。以后情况会有所改观，但是没有人知道这会等到什么时候。
//  第二，这种方法的工作原理不很明了，要理解它必须先要深入理解函数调用时的参数匹配，隐式类型转换函数，模板函数隐式实例化和成员函数模板。
//       有些程序员以前从来没有看到过这种技巧，而却被要求维护使用了这种技巧的代码，我真是很可怜他们。这种技巧确实很巧妙，这自然是肯定，但是过于的巧妙可是一件危险的事情。
// 直接了当地说，如何能够让灵巧指针的行为与dumb指针一样呢？答案很简单：不可能。
// 灵巧指针固然灵巧，但不是指针。最好的方法是使用成员模板生成类型转换函数，在会产生二义性结果的地方使用cast。
// 这不是一个完美的方法，不过已经很不错了，在一些情况下需去除二义性，所付出的代价与灵巧指针提供复杂的功能相比还是值得的。

// 灵巧指针和const
// 对于dumb指针来说，const既可以针对指针所指向的东西，也可以针对于指针本身，或者兼有两者的含义：
//  CD goodCD("Flood");
//  const CD *p; // p是一个non-const指针指向const CD对象
//  CD * const p = &goodCD; // p是一个const指针指向non-const CD对象;因为p是const, 它必须被初始化
//  const CD * const p = &goodCD; // p是一个const指针指向一个const CD对象
// 我们自然想要让灵巧指针具有同样的灵活性。不幸的是只能在一个地方放置const，并只能对指针本身起作用，而不能针对于所指对象：
//  const SmartPtr<CD> p = &goodCD;  // p是一个const灵巧指针指向non-const CD对象
// 好像有一个简单的补救方法，就是建立一个指向cosnt CD的灵巧指针：
//  SmartPtr<const CD> p = &goodCD;  // p是一个non-const灵巧指针指向const CD对象
// 现在我们可以建立const和non-const对象和指针的四种不同组合：
//  SmartPtr<CD> p; // non-const对象，non-const指针
//  SmartPtr<const CD> p; // const对象,non-const指针
//  const SmartPtr<CD> p = &goodCD; // non-const对象，const指针
//  const SmartPtr<const CD> p = &goodCD; // const对象，const指针
// 但是美中不足的是，使用dumb指针我们能够用non-const指针初始化const指针，我们也能用指向non-cosnt对象的指针初始化指向const对象的指针；就像进行赋值一样。例如：
//  CD *pCD = new CD("Famous Movie Themes");
//  const CD * pConstCD = pCD; // 正确
// 但是如果我们试图把这种方法用在灵巧指针上，情况会怎么样呢？
//  SmartPtr<CD> pCD = new CD("Famous Movie Themes");
//  SmartPtr<const CD> pConstCD = pCD; // 正确么?
// SmartPtr<CD>与SmartPtr<const CD>是完全不同的类型。在编译器看来，它们是毫不相关的，所以没有理由相信它们是赋值兼容的。
// 到目前为止这是一个老问题了，把它们变成赋值兼容的唯一方法是你必须提供函数，用来把SmartPtr<CD>类型的对象转换成SmartPtr<const CD>类型。
// 如果你使用的编译器支持成员模板，就可以利用前面所说的技巧自动生成你需要的隐式类型转换操作。
// 带const的类型转换是单向的：从non-const到const的转换是安全的，但是从const到non-const则不是安全的。
// 而且用const指针能做的事情，用non-const指针也能做，但是用non-const指针还能做其它一些事情（例如，赋值操作）。
// 同样，用指向const对象的指针能做的任何事情，用指向non-const对象的指针也能做到，
// 但是用指向non-const对象的指针能够完成一些指向const对象的指针所不能完成的事情（例如，赋值操作）。
// 这些规则看起来与public继承的规则相类似。
// 你能够把一个派生类对象转换成基类对象，但是反之则不是这样，你对基类所做的任何事情对派生类也能做，但是还能对派生类做另外一些事情。
// 我们能够利用这一点来实现灵巧指针，就是说可以让每个指向T的灵巧指针类public派生自一个对应的指向const-T的灵巧指针类：
template <class T>  // 指向const对象的灵巧指针
class SmartPtrToConst {
 protected:
  union {
    const T* constPointee;  //让 SmartPtrToConst访问
    T* pointee;             // 让SmartPtr访问
  };
};
template <class T>  // 指向non-const对象的灵巧指针
class SmartPtr5 : public SmartPtrToConst<T> {};
// 使用这种设计方法，指向non-const-T对象的灵巧指针包含一个指向const-T的dumb指针，指向const-T的灵巧指针需要包含一个指向const-T的dumb指针。
// 最方便的方法是把指向const-T的dumb指针放在基类里，把指向non-const-T的dumb指针放在派生类里，然而这样做有些浪费，因为SmartPtr对象包含两个dumb指针:
// 一个是从SmartPtrToConst继承来的，一个是SmartPtr自己的。
// 一种在C世界里的老式武器可以解决这个问题，这就是union，它在C++中同样有用。Union在protected中，所以所以两个类都可以访问它，它包含两个必须的dumb指针类型，
// 这就是union美丽的地方。当然两个类的成员函数必须约束它们自己仅仅使用适合的指针。这是使用union所冒的风险。
// 利用这种设计，能够获得所要的行为特性：
//  SmartPtr5<CD> pCD = new CD("Famous Movie Themes");
//  SmartPtrToConst<CD> pConstCD = pCD;  // 正确

// 灵巧指针如此繁琐麻烦，是否值得使用，特别是如果你的编译器缺乏支持成员函数模板时。
// 通常是值得的。例如通过使用灵巧指针极大地简化了条款M29中的引用计数代码。而且正如该例子所显示的，灵巧指针的使用在一些领域受到极大的限制，
// 例如测试空值、转换到dumb指针、继承类向基类转换和对指向const的指针的支持。同时灵巧指针的实现、理解和维护需要大量的技巧。
// 调试使用灵巧指针的代码也比调试使用dumb指针的代码困难。无论如何你也不可能设计出一种通用目的的灵巧指针，能够替代dumb指针。
// 达到同样的代码效果，使用灵巧指针更方便。灵巧指针应该谨慎使用, 不过每个C++程序员最终都会发现它们是有用的。

int main() {
  auto_ptr<int> ptn1(new int);
  auto_ptr<int> ptn2 = ptn1;  // 调用拷贝构造函数会发生什么情况？
  auto_ptr<int> ptn3;
  ptn3 = ptn2;  // 调用operator=;会发生什么情况?

  auto_ptr<int> ptn(new int);
  printTreeNode(cout, ptn);  // 通过传值方式传递auto_ptr
  printTreeNode1(cout, ptn);

  SmartPtr<int> ptn4;
  // 没有operator void*()，下面的都编译不过
  if (ptn4 == 0)
    ;
  if (ptn4)
    ;
  if (!ptn4)
    ;

  SmartPtr<double> ptn5;
  if (ptn4 == ptn5)
    ;
  // 即使在SmartPtr<int> 和 SmartPtr<double>之间没有operator= 函数，也能够编译，因为灵巧指针被隐式地转换为void*指针，而对于内建指针类型有内建的比较函数。
  // 这种进行隐式类型转换的行为特性很危险。

  SmartPtr1<int> ptn6;
  SmartPtr1<double> ptn7;
  if (!ptn6) {  // 正确
    // ptn是空值
  } else {
    // ptn不是空值
  }
  // if (ptn6 == 0) // 错误
  // if (ptn6)  // 错误
  // if (ptn6 == ptn7)  // 错误
  // 仅在这种情况下会存在不同类型之间进行比较：
  if (!ptn6 == !ptn7)  // 能够编译，幸好程序员不会经常这样编写代码。
    ;

  SmartPtr<Tuple> pt;
  // normalize(pt);  // 这种调用不能够编译，因为不能把DBPtr<Tuple>转换成Tuple*。
  normalize(&*pt);  // 繁琐, 但合法

  SmartPtr2<Tuple> pt1;
  normalize(pt1);
  normalize(&*pt1);
  if (pt1 == 0)
    ;  // 正确, 把pt转变成Tuple*
  if (pt1)
    ;  // 同上
  if (!pt1)
    ;  // 同上(reprise)

  Tuple *pt11, *pt12;
  merge(pt11, pt12);
  SmartPtr2<Tuple> *pt21, *pt22;
  // merge(pt21, pt22);
  // 因为从DBPtr<Tuple>到TupleAccessors的转换要调用两次用户定义类型转换:
  // 一次SmartPtr2<Tuple>到Tuple*，一次从Tuple*到TupleAccessors，编译器不会进行这种序列的转换。

  // 提供到dumb指针的隐式类型转换的灵巧指针类也暴露了一个非常有害的bug。
  SmartPtr2<Tuple> pt3 = new Tuple;
  delete pt3;  // 这段代码应该不能被编译，pt不是指针，它是一个对象，你不能删除一个对象。只有指针才能被删除，对么？
  // 编译器使用隐式类型转换来尽可能使函数调用成功；再回想一下条款M8：使用delete会调用析构函数和operator delete，两者都是函数。
  // 编译器为了使在delete pt语句里的两个函数成功调用，就把pt隐式转换为Tuple*，然后删除它。
  // 本来是你写错了代码，而现在却编译过了，这样做必然会破坏你的程序。
  // 如果pt拥有它指向的对象，对象就会被删除两次，一次在调用delete时，第二次在pt的析构函数被调用时。
  // 当pt不拥有对象，而是其它人拥有时，如果拥有者同时负责删除pt的则情况还好，但是如果拥有者不是负责删除pt的人，可以预料它以后还会再次删除该对象。
  // 不论是最前者所述的情况还是最后者的情况都会导致一个对象被删除两次，这样做会产生不能预料的后果。
  // 这个bug极为有害，因为隐藏在灵巧指针后面的全部思想就是让它们不论是在外观上还是在使用感觉上都与dumb指针尽可能地相似。
  // 你越接近这种思想，你的用户就越可能忘记正在使用灵巧指针。如果他们忘记了正在使用灵巧指针，肯定会在调用new之后调用delete，以防止资源泄漏，谁又能责备他们这样做不对呢？
  // 底线很简单：除非有一个让人非常信服的原因去这样做，否则绝对不要提供转换到dumb指针的隐式类型转换操作符。

  Cassette* funMusic = new Cassette("Alapalooza");
  CD* nightmareMusic = new CD("Disco Hits of the 70s");
  displayAndPlay(funMusic, 10);
  displayAndPlay(nightmareMusic, 0);
  SmartPtr2<Cassette> funMusic1(new Cassette("Alapalooza"));
  SmartPtr2<CD> nightmareMusic1(new CD("Disco Hits of the 70s"));
  // displayAndPlay1(funMusic1, 10);       // 错误!
  // displayAndPlay1(nightmareMusic1, 0);  // 错误!
  // 不能进行编译的原因是不能把SmartPtr<CD>或SmartPtr2<Cassette>转换成SmartPtr2<MusicProduct>。
  // 从编译器的观点来看，这些类之间没有任何关系。为什么编译器的会这样认为呢？
  // 毕竟SmartPtr<CD>或SmartPtr<Cassette>不是从SmartPtr<MusicProduct>继承过来的，这些类之间没有继承关系，
  // 我们不可能要求编译器把一种对象转换成（完全不同的）另一种类型的对象。

  // SmartPtr3<Cassette> funMusic2(new Cassette("Alapalooza"));
  // SmartPtr3<CD> nightmareMusic2(new CD("Disco Hits of the 70s"));
  // displayAndPlay2(funMusic2, 10);
  // displayAndPlay2(nightmareMusic2, 0);

  SmartPtr4<Cassette> funMusic3(new Cassette("Alapalooza"));
  SmartPtr4<CD> nightmareMusic3(new CD("Disco Hits of the 70s"));
  displayAndPlay3(funMusic3, 10);       // 以前是一个错误
  displayAndPlay3(nightmareMusic3, 0);  // 以前是一个错误
  // funMusic对象的类型是SmartPtr<Cassette>。
  // 函数displayAndPlay期望的参数是SmartPtr<MusicProduct>地对象。编译器侦测到类型不匹配，于是寻找把funMusic转换成SmartPtr<MusicProduct>对象的方法。
  // 它在SmartPtr<MusicProduct>类里寻找带有SmartPtr<Cassette>类型参数的单参数构造函数，但是没有找到。
  // 然后它们又寻找成员函数模板，以实例化产生这样的函数。它们在SmartPtr<Cassette>发现了模板，把newType绑定到MusicProduct上，生成了所需的函数。
  // 实例化函数，生成这样的代码：
  // SmartPtr<Cassette>:: operator SmartPtr<MusicProduct>() {
  //  return SmartPtr<MusicProduct>(pointee);
  // }
  // 实际上这段代码就是用pointee做为参数调用SmartPtr<MusicProduct>的构造函数，所以真正的问题是能否用一个Cassettte*指针构造一个SmartPtr<MusicProduct>对象。

  SmartPtr4<CasSingle> dumbMusic4(new CasSingle("Achy Breaky Heart"));
  // displayAndPlay3(dumbMusic4, 1);  // 错误!

  return 0;
}