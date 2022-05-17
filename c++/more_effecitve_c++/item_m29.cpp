#include <cstring>
#include <iostream>

using namespace std;

// 引用计数

// 引用计数是这样一个技巧，它允许多个有相同值的对象共享这个值的实现。这个技巧有两个常用动机：
// 1.是简化跟踪堆中的对象的过程。
//   一旦一个对象通过调用new被分配出来，最要紧的就是记录谁拥有这个对象，
//   因为其所有者并且只有其所有者负责对这个对象调用delete。
//   引用计数可以免除跟踪对象所有权的担子，因为当使用引用计数后，对象自己拥有自己。
//   当没人再使用它时，它自己自动销毁自己。
//   因此，引用计数是个简单的垃圾回收体系。
// 2.如果很多对象有相同的值，不用存储多次。
//   更好的办法是让所有的对象共享这个值的实现。
//   这么做不但节省内存，而且可以使得程序运行更快，因为不需要构造和析构这个值的拷贝。

#define SHAREABLE

class String {
 public:
  String(const char* initValue = "");
  String(const String& rhs);
  ~String();
  String& operator=(const String& rhs);
  const char& operator[](int index) const;
  char& operator[](int index);

  void display() { cout << value->data << endl; }

 private:
  struct StringValue {
    // StringValue的主要目的是提供一个空间将一个特别的值和共享此值的对象的数目联系起来。
    int refCount;
#ifdef SHAREABLE
    bool shareable;
#endif
    char* data;
    StringValue(const char* initValue);
    ~StringValue();
  };
  StringValue* value;

  char* data;
};
String::String(const char* initValue) : value(new StringValue(initValue)) {}

// String的拷贝构造函数很高效：新生成的String对象与被拷贝的对象共享相同的StringValue对象：
#ifdef SHAREABLE
String::String(const String& rhs) {
  if (rhs.value->shareable) {
    value = rhs.value;
    ++value->refCount;
  } else {
    value = new StringValue(rhs.value->data);
  }
}
#else
String::String(const String& rhs) : value(rhs.value) { ++value->refCount; }
#endif
// 这肯定比通常的（不带引用计数的）string类高效，
// 因为不需要为新生成的string值分配内存、释放内存以及将内容拷贝入这块内存。
// 现在，我们只不过是拷贝了一个指针并增加了一次引用计数。

// String类的析构函数同样容易实现，因为大部分情况下它不需要做任何事情。
// 只要引用计数值不是0，也就是至少有一个String对象使用这个值，这个值就不可以被销毁。
// 只有当唯一的使用者被析构了（也就是引用计数在进入函数前已经为1时），
// String的析构函数才摧毁StringValue对象：
String::~String() {
  if (--value->refCount == 0) delete value;
}

// 不使用引用计数的版本
// String& String::operator=(const String& rhs) {
//   if (this == &rhs) return *this;
//   delete[] data;
//   data = new char[strlen(rhs.data) + 1];
//   strcpy(data, rhs.data);
//   return *this;
// }
String& String::operator=(const String& rhs) {
  if (value == rhs.value) {
    return *this;
  }
  if (--value->refCount == 0) {
    delete value;
  }
  value = rhs.value;
  ++value->refCount;
  return *this;
}

// 这个函数实现了C++传统意义上的下标索引（根本不会说“不”）。
// 如果你想加上参数检查，这是非常容易的。
const char& String::operator[](int index) const { return value->data[index]; }
// 当我们修改一个String对象的值时，必须小心防止修改了与它共享相同StringValue对象的其它String对象的值。
// 不幸的是，C++编译器没有办法告诉我们一个特定的operator[]是用作读的还是写的，
// 所以我们必须保守地假设“所有”调用非const operator[]的行为都是为了写操作。
// 为了安全地实现非const的operator[]，我们必须确保没有其它String对象在共享这个可能被修改的StringValue对象。
// 简而言之，当我们返回StringValue对象中的一个字符的引用时，必须确保这个StringValue的引用计数是1。
char& String::operator[](int index) {
  if (value->refCount > 1) {
    --value->refCount;
    value = new StringValue(value->data);
  }
#ifdef SHAREABLE
  value->shareable = false;  // add this
#endif
  return value->data[index];
}

// 这个“与其它对象共享一个值直到写操作时才拥有自己的拷贝”的想法在计算机科学中已经有了悠久而著名的历史了，
// 尤其是在操作系统中：进程共享内存页直到它们想在自己的页拷贝中修改数据为止。
// 这个技巧如此常用，以至于有一个名字：写时拷贝，它是提高效率的一个更通用方法Lazy原则的特例。
// 大部分情况下，写时拷贝可以同时保证效率和正确性，只有一个挥之不去的问题，见代码。
// 至少有三种方法来应付这个问题:
// 1.忽略它，假装它不存在。这是实现带引用计数的String类的类库中令人痛苦的常见问题。
// 2.稍微好些的方法是明确说明它的存在。通常是将它写入文档，或多或少地说明“别这么做。
// 3.排除这个问题。它不难实现，但它将降低一个值共享于对象间的次数。
//   它的本质是这样的：在每个StringValue对象中增加一个标志以指出它是否为可共享的。
//   在最初（对象可共享时）将标志打开，在非const的operator[]被调用时将它关闭。
//   一旦标志被设为false，它将永远保持在这个状态。
// 标准C++运行库中的string类型同时使用了方法2和方法3。
// 从非const的operator[]中返回的引用直到下一次的可能修改这个string的函数的调用为止都是有效的。
// 在此之后，使用这个引用（或它指向的字符），其结果未定义。
// 这样就它允许了：string的可共享标志在调用可能修改string的函数时被重设为true。

// 创建一个带引用计数的String类并不困难，但需要注意一些细节。
// 认识到“我们需要一个地方来存储这个计数值”是很重要的。
// 这个地方不能在String对象内部，因为需要的是每个String值一个引用计数值，而不是每个String对象一个引用计数。
// 这意味着String值和引用计数间是一一对应的关系，所以我们将创建一个类来保存引用计数及其跟踪的值。
// 我们叫这个类StringValue，又因为它唯一的用处就是帮助我们实现String类，所以我们将它嵌套在String类的私有区内。
// 另外，为了便于Sting的所有成员函数读取其数据区，我们将StringValue申明为struct。
// 需要知道的是：将一个struct内嵌在类的私有区内，能便于这个类的所有成员访问这个结构，
// 但阻止了其它任何人对它的访问（当然，除了友元）。
String::StringValue::StringValue(const char* initValue)
    : refCount(1)
#ifdef SHAREABLE
      ,
      shareable(true)
#endif
{
  data = new char[strlen(initValue) + 1];
  strcpy(data, initValue);
}
String::StringValue::~StringValue() { delete[] data; }

// 带引用计数的基类
// RCObject封装了引用计数功能，如增加和减少引用计数的函数。
// 它还包含了当这个值不再被需要时摧毁值对象的代码（也就是引用计数为0时）。
// 最后，它包含了一个字段以跟踪这个值对象是否可共享，并提供查询这个值和将它设为false的函数。
// 不需将可共享标志设为true的函数，因为所有的值对象默认都是可共享的。
// 一旦一个对象变成了不可共享，将没有办法使它再次成为可共享。
class RCObject {
  // RCObjcet可以被构造（作为派生类的基类部分）和析构；
  // 可以有新的引用加在上面以及移除当前引用；
  // 其可共享性可以被查询以及被禁止；
  // 它们可以报告当前是否被共享了。
 public:
  RCObject();
  RCObject(const RCObject& rhs);
  RCObject& operator=(const RCObject& rhs);
  virtual ~RCObject() = 0;
  void addReference();
  void removeReference();
  void markUnshareable();
  bool isShareable() const;
  bool isShared() const;

 private:
  int refCount;
  bool shareable;
};
// 在它构造后，只需构造它的对象简单地将refCount设为1就可以。
RCObject::RCObject() : refCount(0), shareable(true) {}
// 拷贝构造函数也将refCount设为0，而不管被拷贝的RCObject对象的refCount的值。
// 这是因为我们正在构造新的值对象，而这个新的值对象总是未被共享的，只被它的构造者引用。
// 再一次，构造者负责将refCount设为正确的值。
RCObject::RCObject(const RCObject&) : refCount(0), shareable(true) {}
// 赋值运算看起来完全出乎意料：它没有做任何事情，这个函数不太可能被调用的。
// RCObject 是基于引用计数来共享的值对象的基类，它不该被从一个赋给另外一个，
// 而应该是拥有这个值的对象被从一个赋给另外一个。
RCObject& RCObject::operator=(const RCObject&) { return *this; }
RCObject::~RCObject() {}
void RCObject::addReference() { ++refCount; }
// 不但负责减少对象的refCount值，还负责当refCount值降到0时析构对象。
void RCObject::removeReference() {
  // 只当我们知道*this是一个堆对象时才安全。要让这个类正确，必须确保RCObject只能被构建在堆中。
  // 实现这一点的常用方法见Item M27，但我们这次采用一个特别的方法。
  if (--refCount == 0) delete this;
  // 我们用习惯方法来解决。
  // RCObject被设计为只作被引用计数的值对象的基类使用，而这些值对象应该只通过灵巧指针RCPtr引用。
  // 此外，值对象应该只能由值会共享的对象来实例化；
  // 它们不能被按通常的方法使用。
  // 在我们的例子中，值对象的类是StringValue，我们通过将它申明为String的私有而限制其使用。
  // 只有String可以创建StringValue对象，所以String类的作者应该确保这些值对象都是通过new操作产成的。
  // 于是，我们限制RCObject只能在堆上创建的方法就是指定一组满足这个要求的类，并确保只有这些类能创建RCObject对象。
  // 用户不可能无意地（或有意地）用一种不恰当的方法创建RCObject对象。
  // 我们限制了创建被引用计数对象的权力，当我们交出这个权力时，必须明确其附带条件是满足创建对象的限制条件。
}
void RCObject::markUnshareable() { shareable = false; }
bool RCObject::isShareable() const { return shareable; }
bool RCObject::isShared() const { return refCount > 1; }
class String1 {
 private:
  struct StringValue : public RCObject {
    char* data;
    StringValue(const char* initValue);
    ~StringValue();
  };
};
String1::StringValue::StringValue(const char* initValue) {
  data = new char[strlen(initValue) + 1];
  strcpy(data, initValue);
}
String1::StringValue::~StringValue() { delete[] data; }
// 这个版本的StringValue和前面的几乎一样，唯一改变的就是StringValue的成员函数不再处理refCount字段。
// RCObject现在接管了这个工作。

// 自动的引用计数处理
// RCObject类给了我们一个存储引用计数的地方，并提供了成员函数供我们操作引用计数，
// 但调用这些函数的动作还必须被手工加入其它类中。
// 仍要在String的拷贝构造函数和赋值操作符调用StringValue的addReference和removeReference函数，这很笨拙。
// 我们想将这些调用也移入一个可重用的类中，以使得String这样的类的作者不用再担心引用计数的任何细节。
template <class T>
class RCPtr {
  // 供引用计数对象使用的灵巧指针模板
 public:
  RCPtr(T* realPtr = 0);
  RCPtr(const RCPtr& rhs);
  ~RCPtr();
  RCPtr& operator=(const RCPtr& rhs);
  T* operator->() const;
  T& operator*() const;

 private:
  T* pointee;
  void init();
};
template <class T>
void RCPtr<T>::init() {
  if (pointee == 0) return;
  if (pointee->isShareable() == false) {
    pointee = new T(*pointee);
  }
  // 当一个RCPtr构建时，它指向的对象需要增加引用计数值。
  pointee->addReference();
}
template <class T>
RCPtr<T>::RCPtr(T* realPtr) : pointee(realPtr) {
  init();
}
template <class T>
RCPtr<T>::RCPtr(const RCPtr& rhs) : pointee(rhs.pointee) {
  init();
}
template <class T>
RCPtr<T>::~RCPtr() {
  if (pointee) pointee->removeReference();
}
template <class T>
RCPtr<T>& RCPtr<T>::operator=(const RCPtr& rhs) {
  if (pointee != rhs.pointee) {
    if (pointee) pointee->removeReference();
    pointee = rhs.pointee;
    init();
  }
  return *this;
}
template <class T>
T* RCPtr<T>::operator->() const {
  return pointee;
}
template <class T>
T& RCPtr<T>::operator*() const {
  return *pointee;
}
class RCObject1 {
 public:
  void addReference();
  void removeReference();
  void markUnshareable();
  bool isShareable() const;
  bool isShared() const;

 protected:
  RCObject1();
  RCObject1(const RCObject1& rhs);
  RCObject1& operator=(const RCObject1& rhs);
  virtual ~RCObject1() = 0;

 private:
  int refCount;
  bool shareable;
};
RCObject1::RCObject1() : refCount(0), shareable(true) {}
RCObject1::RCObject1(const RCObject1&) : refCount(0), shareable(true) {}
RCObject1& RCObject1::operator=(const RCObject1&) { return *this; }
RCObject1::~RCObject1() {}
void RCObject1::addReference() { ++refCount; }
void RCObject1::removeReference() {
  if (--refCount == 0) delete this;
}
void RCObject1::markUnshareable() { shareable = false; }
bool RCObject1::isShareable() const { return shareable; }
bool RCObject1::isShared() const { return refCount > 1; }
class String2 {
  // 拷贝构造函数在哪里？赋值运算在哪里？析构函数在哪里？
  // 我们不再需要那些函数了！确实，String对象的拷贝仍然被支持，
  // 并且，这个拷贝将正确处理藏在后面的被引用计数的StringValue对象，
  // 但String类不需要写下哪怕一行代码来让它发生。
  // 因为编译器为String自动生成的拷贝构造函数将自动调用其RCPtr成员的拷贝构造函数，
  // 而这个拷贝构造函数完成所有必须的对StringValue对象的操作，包括它的引用计数。
  // RCPtr是一个灵巧指针，所以这是它将完成的工作。
  // 它同样处理赋值和析构，所以String类同样不需要写出这些函数。
 public:
  String2(const char* value = "");
  const char& operator[](int index) const;
  char& operator[](int index);

 private:
  struct StringValue : public RCObject1 {
    char* data;
    StringValue(const char* initValue);
    StringValue(const StringValue& rhs);
    void init(const char* initValue);
    ~StringValue();
  };
  RCPtr<StringValue> value;
};
void String2::StringValue::init(const char* initValue) {
  data = new char[strlen(initValue) + 1];
  strcpy(data, initValue);
}
String2::StringValue::StringValue(const char* initValue) { init(initValue); }
String2::StringValue::StringValue(const StringValue& rhs) { init(rhs.data); }
String2::StringValue::~StringValue() { delete[] data; }
String2::String2(const char* initValue) : value(new StringValue(initValue)) {}
const char& String2::operator[](int index) const { return value->data[index]; }
char& String2::operator[](int index) {
  if (value->isShared()) {
    value = new StringValue(value->data);
  }
  value->markUnshareable();
  return value->data[index];
}
// 和我们用内建指针实现的版本相比较，你会受到两件事的打击:
// 1.代码有很多的减少，因为RCPtr完成了大量以前在String内部完成的处理引用计数的担子。
// 2.剩下的代码几乎没有变化：灵巧指针无缝替换了内建指针。
//   实际上，唯一的变化是在operator[]里，我们用调用isShared函数代替了直接检查refCount的值，
//   并用灵巧指针RCPtr对象消除了写时拷贝时手工维护引用计数值的工作。
// 这当然全都很漂亮。谁能反对减少代码？谁能反对成功的封装？
// 然而，这个全新的String类本身对用户的冲击远胜过它的实现细节，这才是真正的闪光点。
// 如果没有什么消息是好消息的话，这本身就是最好的消息。
// String的接口没有改变！我们增加了引用计数，我们增加了标记某个String的值为不可共享的能力，
// 我们将引用计数功能移入一个新类，我们增加了灵巧指针来自动处理引用计数，但用户的一行代码都不需要修改。
// 当然，我们改变了String类的定义，所以用户需要重新编译和链接，但他们在自己代码上的投资受到了完全的保护。
// 你看到了吗？封装确实是个很好的东西。

// 在现存类上增加引用计数
// 如果我们想让一个位于支撑库中而无法修改的类获得引用计数的好处呢？
// 不可能让它们从RCObject继承的，所以也不能对它们使用灵巧指针RCPtr。
// 只要对我们的设计作小小的修改，我们就可以将引用计数加到任意类型上。
// 计算机科学中的绝大部分问题都可以通过增加一个中间层次来解决。
// 我们增加一个新类CountHolder以处理引用计数，它从RCObject继承。
// 我们让CountHolder包含一个指针指向Widget。
// 然后用等价的灵巧指针RCIPter模板替代RCPtr模板，它知道CountHolder类的存在。
// 在这种情况下，我们需要增加一个类RCWidget以供用户使用，而所有的事情都和String/StringValue的例子一样，
// RCWidget和String相同，Widget和StringValue相同。
// 设计看起来是这样的：
template <class T>
class RCIPtr {
 public:
  RCIPtr(T* realPtr = 0);
  RCIPtr(const RCIPtr& rhs);
  ~RCIPtr();
  RCIPtr& operator=(const RCIPtr& rhs);
  const T* operator->() const;
  T* operator->();
  const T& operator*() const;
  T& operator*();

 private:
  struct CountHolder : public RCObject {
    ~CountHolder() { delete pointee; }
    T* pointee;
  };
  CountHolder* counter;
  void init();
  void makeCopy();
};
template <class T>
void RCIPtr<T>::init() {
  if (counter->isShareable() == false) {
    T* oldValue = counter->pointee;
    counter = new CountHolder;
    counter->pointee = new T(*oldValue);
  }
  counter->addReference();
}
template <class T>
RCIPtr<T>::RCIPtr(T* realPtr) : counter(new CountHolder) {
  counter->pointee = realPtr;
  init();
}
template <class T>
RCIPtr<T>::RCIPtr(const RCIPtr& rhs) : counter(rhs.counter) {
  init();
}
template <class T>
RCIPtr<T>::~RCIPtr() {
  counter->removeReference();
}
template <class T>
RCIPtr<T>& RCIPtr<T>::operator=(const RCIPtr& rhs) {
  if (counter != rhs.counter) {
    counter->removeReference();
    counter = rhs.counter;
    init();
  }
  return *this;
}
template <class T>
void RCIPtr<T>::makeCopy() {
  if (counter->isShared()) {
    T* oldValue = counter->pointee;
    counter->removeReference();
    counter = new CountHolder;
    counter->pointee = new T(*oldValue);
    counter->addReference();
  }
}
template <class T>
const T* RCIPtr<T>::operator->() const {
  return counter->pointee;
}
template <class T>
T* RCIPtr<T>::operator->() {
  makeCopy();
  return counter->pointee;
}
template <class T>
const T& RCIPtr<T>::operator*() const {
  return *(counter->pointee);
}
template <class T>
T& RCIPtr<T>::operator*() {
  makeCopy();
  return *(counter->pointee);
}
// RCIPtr与RCPtr只两处不同:
// 1.RCPtr对象直接指向值对象，而RCIptr对象通过中间层的CountHolder对象指向值对象。
// 2.RCIPtr重载了operator->和operator*，当有对被指向的对象的非const的操作时，写时拷贝自动被执行。
class Widget {
 public:
  Widget(int size);
  Widget(const Widget& rhs);
  ~Widget();
  Widget& operator=(const Widget& rhs);
  void doThis();
  int showThat() const;
};
class RCWidget {
 public:
  RCWidget(int size) : value(new Widget(size)) {}
  void doThis() { value->doThis(); }
  int showThat() const { return value->showThat(); }

 private:
  RCIPtr<Widget> value;
};

// 实现引用计数不是没有代价的。
// 每个被引用的值带一个引用计数，其大部分操作都需要以某种形式检查或操作引用计数。
// 对象的值需要更多的内存，而我们在处理它们时需要执行更多的代码。
// 此外，就内部的源代码而言，带引用计数的类的复杂度比不带的版本高。
// 没有引用计数的String类只依赖于自己，
// 而我们最终的String类如果没有三个辅助类（StringValue、RCObject和RCPtr）就无法使用。
// 确实，我们这个更复杂的设计确保在值可共享时的更高的效率；
// 免除了跟踪对象所有权的需要，提高了引用计数的想法和实现的可重用性。
// 但，这四个类必须写出来、被测试、文档化、和被维护，比单个类要多做更多的工作。
// 即使是管理人员也能看出这点。
// 引用计数是基于对象通常共享相同的值的假设的优化技巧。
// 如果假设不成立的话，引用计数将比通常的方法使用更多的内存和执行更多的代码。
// 另一方面，如果你的对象确实有具体相同值的趋势，那么引用计数将同时节省时间和空间。
// 共享的值所占内存越大，同时共享的对象数目越多，节省的内存也就越大。
// 创建和销毁这个值的代价越大，你节省的时间也越多。
// 总之，引用计数在下列情况下对提高效率很有用：
// 1.少量的值被大量的对象共享。
// 2.这样的共享通常通过调用赋值操作和拷贝构造而发生。
// 3.对象/值的比例越高，越是适宜使用引用计数。
// 4.对象的值的创建和销毁代价很高昂，或它们占用大量的内存。
//   即使这样，如果不是多个对象共享相同的值，引用计数仍然帮不了你任何东西。
// 只有一个方法来确认这些条件是否满足，而这个方法不是猜测或依赖直觉，这个方法是使用profiler或其它工具来分析。
// 使用这种方法，你可以发现是否创建和销毁值的行为是性能瓶颈，并能得出对象/值的比例。
// 只有当你手里有了这些数据，你才能得出是否从引用计数上得到的好处超过其缺点。
// 即使上面的条件满足了，使用引用计数仍然可能是不合适的。
// 有些数据结构（如有向图）将导致自我引用或环状结构。
// 这样的数据结构可能导致孤立的自引用对象，它没有被别人使用，而其引用计数又绝不会降到零。
// 因为这个无用的结构中的每个对象被同结构中的至少一个对象所引用。
// 商用化的垃圾收集体系使用特别的技术来查找这样的结构并消除它们，
// 但我们现在使用的这个简单的引用计数技术不是那么容易扩充出这个功能的。
// 即使效率不是主要问题，引用计数仍然很吸引人。
// 如果你不放心谁应该去执行删除动作，那么引用计数正是这种让你放下担子的技巧。
// 很多程序员只因为这个原因就使用引用计数。

int main() {
  // 将可能如何遇到多个对象有相同的值。这儿有一个：
  String a, b, c, d, e;
  a = b = c = d = e = "Hello";
  // 对象a到e都有相同的值“Hello”。
  // 其值的形态取决于String类是怎么实现的，
  // 但通常的实现（不使用引用计数）是每个string对象有一个这个值的拷贝。

  String s1("More Effective C++");
  String s2 = s1;
  // 应该是s1和s2指向相同的StringValue对象。
  // 对象的引用计数应该在赋值时被增加。
  // 并且，s1原来指向的StringValue对象的引用计数应该减少，因为s1不再具有这个值了。
  // 如果s1是拥有原来的值的唯一对象，这个值应该被销毁。

  cout << s1[3] << endl;  // e
  s1[5] = 'x';
  s1.display();  // More xffective C++

  String s3 = "Hello";
  char* p = &s3[1];
  String s4 = s3;
  s3.display();       // Hello
  s4.display();       // Hello
  cout << p << endl;  // ello
  *p = 'x';           // 有不受欢迎的结果：修改了s3和s4
  s3.display();       // Hxllo
  s4.display();       // Hxllo,使用了SHAREABLE结果是Hello
  cout << p << endl;  // xllo
  // String的拷贝构造函数没有办法检测这样的问题，
  // 因为它不知道指向s1拥有的StringValue对象的指针的存在。
  // 并且，这个问题不局限于指针：
  // 它同样存在于有人保存了一个String的非const operator[]的返回值的引用的情况下。
  char& c1 = p[1];
  cout << c1 << endl;  // l
  p[1] = 'a';
  cout << c1 << endl;  // a

  return 0;
}