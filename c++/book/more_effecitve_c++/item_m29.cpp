#include <cstring>
#include <iostream>
#include <string>

// 引用计数

// 引用计数是这样一个技巧，它允许多个有相同值的对象共享这个值的实现，两个常用的动机：
// 1.简化跟踪堆中的对象的过程。
//   一旦一个对象通过调用new被分配出来，最要紧的就是记录谁拥有这个对象，
//   因为其所有者并且只有其所有者负责对这个对象调用delete。
//   引用计数可以免除跟踪对象所有权的担子，因为当使用引用计数后，对象自己拥有自己。
//   当没人再使用它时，它自己自动销毁自己，因此，引用计数是个简单的垃圾回收体系。
// 2.如果很多对象有相同的值，不用存储多次。
//   更好的办法是让所有的对象共享这个值的实现，这么做不但节省内存，
//   而且可以使得程序运行更快，因为不需要构造和析构这个值的拷贝。

namespace n1 {
// 没有引用计数：
class String {
 public:
  String(const char* value = "") {
    data = new char[strlen(value) + 1];
    strcpy(data, value);
  }
  String& operator=(const String& rhs) {
    if (this == &rhs) {
      return *this;
    }
    delete[] data;
    data = new char[strlen(rhs.data) + 1];
    strcpy(data, rhs.data);
    return *this;
  }

  void print() { std::cout << (void*)data << ":" << data << std::endl; }

 private:
  char* data;
};

void func() {
  // 5个对象的内容相同，但是保存了5份Hello：
  String a, b, c, d, e;
  a = b = c = d = e = "Hello";

  a.print();
  b.print();
  c.print();
  d.print();
  e.print();

  // 0x5636efd10eb0:Hello
  // 0x5636efd10ed0:Hello
  // 0x5636efd10ef0:Hello
  // 0x5636efd10f10:Hello
  // 0x5636efd10f30:Hello
}
}  // namespace n1

namespace n2 {
// 实现引用计数：
// 引用计数不能在String对象内部，因为需要的是每个String值一个引用计数值，
// 而不是每个String对象一个引用计数，创建StringValue类来保存引用计数及其跟踪的值。
// 因为它唯一的用处就是帮助实现String类，所以将它嵌套在String类的私有区内。
// 另外，为了便于Sting的所有成员函数读取其数据，将StringValue声明为struct。
// 将一个struct内嵌在类的私有区内，能便于这个类的所有成员访问这个结构，
// 同时又阻止了其它任何人（除了友元）对它的访问。
class String {
 public:
  String(const char* initValue = "") : value(new StringValue(initValue)) {}
  String(const String& rhs) : value(rhs.value) { ++value->refCount; }
  ~String() {
    if (--value->refCount == 0) {
      delete value;
    }
  }
  String& operator=(const String& rhs) {
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

  const char& operator[](int index) const { return value->data[index]; }
  char& operator[](int index) {
    // 这个函数可能是被调用了来读一个字符，也可能被调用了来写一个字符。
    // 不幸的是，C++编译器没有办法知道一个特定的operator[]是用作读的还是写的，
    // 所以必须保守地假设所有调用非const operator[]的行为都是为了写操作。
    // Proxy类可以帮助区分读还是写，见Item M30。

    // 为了安全地实现非const的operator[]，
    // 必须确保没有其它String对象在共享这个可能被修改的StringValue对象。
    // 简而言之，当返回StringValue对象中的一个字符的引用时，
    // 必须确保这个StringValue的引用计数是1。

    // 这个与其它对象共享一个值直到写操作时才拥有自己的拷贝的想法，
    // 在计算机科学中已经有了悠久而著名的历史了，尤其是在操作系统中：
    // 进程共享内存页直到它们想在自己的页拷贝中修改数据为止。
    // 这个技巧如此常用，以至于有一个名字：写时拷贝。
    // 它是提高效率的一个更通用方法，Lazy原则的特例。

    if (value->refCount > 1) {
      --value->refCount;
      value = new StringValue(value->data);
    }

    return value->data[index];
  }

  void print() {
    std::cout << (void*)value->data << ":" << value->data << std::endl;
  }

 private:
  struct StringValue {
    int refCount;
    char* data;
    StringValue(const char* initValue) : refCount(1) {
      data = new char[strlen(initValue) + 1];
      strcpy(data, initValue);
    }
    ~StringValue() { delete[] data; }
  };
  StringValue* value;
};

void func1() {
  String a, b, c, d, e;
  a = b = c = d = e = "Hello";

  a.print();
  b.print();
  c.print();
  d.print();
  e.print();

  // 0x560e01408010:Hello
  // 0x560e01408010:Hello
  // 0x560e01408010:Hello
  // 0x560e01408010:Hello
  // 0x560e01408010:Hello
}
void func2() {
  // 大部分情况下，写时拷贝可以同时保证效率和正确性，只有一个挥之不去的问题：
  String s1 = "World";
  char* p = &s1[1];  // 放到s2定义的后面，[]调用导致有新的副本，没问题
  char& r = s1[2];
  String s2 = s1;
  s1.print();  // 0x5556f80cef30:World
  s2.print();  // 0x5556f80cef30:World

  // String的拷贝构造函数没有办法检测这样的问题，
  // 因为它不知道指向s1拥有的StringValue对象的指针的存在：
  *p = 'x';
  s1.print();  // 0x5556f80cef30:Wxrld
  s2.print();  // 0x5556f80cef30:Wxrld

  // 不光是指针，如果保存了String的非const operator[]的返回值的引用也有问题：
  r = 'z';
  s1.print();  // 0x5556f80cef30:Wxzld
  s2.print();  // 0x5556f80cef30:Wxzld
}

void func() {
  func1();
  func2();
}
}  // namespace n2

namespace n3 {
// n2中的问题至少有三种方法来应付：
// 1.忽略它，假装它不存在。
//   这是实现带引用计数的String类的类库中令人痛苦的常见问题。
//   如果有带引用计数的String类，试一下上面的例子，看是否存在。
//   即使不能确定操作的是否是带引用计数的String类，也无论如何应该试一下这个例子。
//   由于封装，可能使用了一个这样的类型而不自知。
// 2.不是所以的实现都忽略这个问题，稍微好些的方法是明确说明它的存在。
//   通常是将它写入文档，或多或少地说明，别这么做，如果这么做了，结果为未定义。
//   无论以哪种方式这么做了（有意地或无意地），并抱怨其结果时，他们辩解道已经告知了。
//   这样的实现通常很方便，但它们在可用性方面留下了太多的期望。
// 3.排除这个问题，它不难实现，但它将降低一个值共享于对象间的次数。
//   本质是：在每个StringValue对象中增加一个标志以指出它是否为可共享的。
//   在最初（对象可共享时）将标志打开，在非const的operator[]被调用时将它关闭。
//   一旦标志被设为false，它将永远保持在这个状态。
class String {
 public:
  String(const char* initValue = "") : value(new StringValue(initValue)) {}
  String(const String& rhs) {
    // 所有其它的成员函数也都必须以类似的方法检查这个共享标志：
    if (rhs.value->shareable) {
      value = rhs.value;
      ++value->refCount;
    } else {
      value = new StringValue(rhs.value->data);
    }
  }
  ~String() {
    if (--value->refCount == 0) {
      delete value;
    }
  }
  String& operator=(const String& rhs) {
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

  const char& operator[](int index) const { return value->data[index]; }
  char& operator[](int index) {
    if (value->refCount > 1) {
      --value->refCount;
      value = new StringValue(value->data);
    }

    // 唯一将共享标志设为false的地方：
    value->shareable = false;
    // 如果使用Item M30中的proxy类的技巧以区分读写操作，
    // 通常可以降低必须被设为不可共享的StringValue对象的数目。

    return value->data[index];
  }

  void print() {
    std::cout << (void*)value->data << ":" << value->data << std::endl;
  }

 private:
  struct StringValue {
    int refCount;
    bool shareable;  // 新增标志位
    char* data;
    StringValue(const char* initValue) : refCount(1), shareable(true) {
      data = new char[strlen(initValue) + 1];
      strcpy(data, initValue);
    }
    ~StringValue() { delete[] data; }
  };
  StringValue* value;
};

void func1() {
  String s1 = "World";
  char* p = &s1[1];
  char& r = s1[2];
  String s2 = s1;
  s1.print();  // 0x5556416eded0:World
  s2.print();  // 0x5556416edf10:World
  *p = 'x';
  s1.print();  // 0x5556416eded0:Wxrld
  s2.print();  // 0x5556416edf10:World
  r = 'z';
  s1.print();  // 0x5556416eded0:Wxzld
  s2.print();  // 0x5556416edf10:World
}

// 标准C++标准库的string（见Item E49和Item M35）同时使用了2和3。
// 从非const的operator[]中返回的引用，
// 直到下一次的可能修改这个string的函数的调用为止都是有效的。
// 在此之后，使用这个引用（或它指向的字符），其结果未定义。
// 这样就它允许了：string的可共享标志在调用可能修改string的函数时被重设为true。
void func2() {
  std::string s1("nihao");
  char* p = &s1[1];
  char& r = s1[2];
  std::string s2(s1);
  std::cout << (void*)s1.data() << ":" << s1 << std::endl;
  std::cout << (void*)s2.data() << ":" << s2 << std::endl;
  // 0x7fff11509f90:nihao
  // 0x7fff11509fb0:nihao

  *p = 'w';
  std::cout << (void*)s1.data() << ":" << s1 << std::endl;
  std::cout << (void*)s2.data() << ":" << s2 << std::endl;
  // 0x7fff11509f90:nwhao
  // 0x7fff11509fb0:nihao

  r = 'q';
  std::cout << (void*)s1.data() << ":" << s1 << std::endl;
  std::cout << (void*)s2.data() << ":" << s2 << std::endl;
  // 0x7fff11509f90:nwqao
  // 0x7fff11509fb0:nihao

  std::string s3 = "world";
  std::string s4(s3);
  std::cout << (void*)s3.data() << ":" << s3 << std::endl;
  std::cout << (void*)s4.data() << ":" << s4 << std::endl;
  // 0x7ffd7a94ebf0:world
  // 0x7ffd7a94ec10:world
  // g++-4.8 item_m29.cpp:
  // 0x1f813b8:world
  // 0x1f813b8:world

  std::string s5 = "1234567890123456";
  std::string s6(s5);
  std::cout << (void*)s5.data() << ":" << s5 << std::endl;
  std::cout << (void*)s6.data() << ":" << s6 << std::endl;
  // 0x55f3bc5e1eb0:1234567890123456
  // 0x55f3bc5e1ed0:1234567890123456
  // g++-4.8 item_m29.cpp
  // 0x1f813e8:1234567890123456
  // 0x1f813e8:1234567890123456

  // https://blog.51cto.com/u_15891800/5995662
}

void func() {
  func1();
  func2();
}
}  // namespace n3

namespace n4 {
// 带引用计数的基类：
// 引用计数不只用在字符串类上，只要是多个对象具有相同值的类都可以使用引用计数。
// 改写一个类以获得引用计数需要大量的工作，而已经有太多的工作需要做了。
// 如果能将引用计数的代码写成与运行环境无关的，并能在需要时将它嫁接到其它类上。
// 将是非常幸运的，有一个方法可以完成它（至少完成了绝大部分必须的工作）：
// 1.构建一个基类RCObject，任何需要引用计数的类都必须从它继承。
//   RCObject封装了引用计数功能，如增加和减少引用计数的函数。
//   它还包含了当这个值不再被需要时摧毁值对象的代码（也就是引用计数为0时）。
// 2.包含了一个字段以跟踪这个值对象是否可共享，并提供查询这个值和将它设为false的函数。
//   不需将可共享标志设为true的函数，因为所有的值对象默认都是可共享的。
//   如上面说过的，一旦一个对象变成了不可共享，将没有办法使它再次成为可共享。
class RCObject {
 public:
  // 在所有的构造函数中都将refCount设为了0，这看起来违反直觉。
  // 确实，最少，构造这个RCObject对象的对象引用它！
  // 在它构造后，只需构造它的对象简单地将refCount设为1就可以了，
  // 所以没有将这个工作放入RCObject内部，这使得最终的代码看起来很简短。
  RCObject() : refCount(0), shareable(true) {}

  // 另一个奇怪之处是拷贝构造函数也将refCount设为0，
  // 而不管被拷贝的RCObject对象的refCount的值。
  // 这是因为正在构造新的值对象，而这个对象总是未被共享的，只被它的构造者引用。
  // 再一次，构造者负责将refCount设为正确的值。
  RCObject(const RCObject& rhs) : refCount(0), shareable(true) {}

  // RCObject的赋值运算看起来完全出乎意料：它没有做任何事情。
  // 这个函数不太可能被调用的，RCObject是基于引用计数来共享的值对象的基类，
  // 它不该被从一个赋给另外一个，而应该是拥有这个值的对象被从一个赋给另外一个。
  // 在这个设计里，不期望StringValue对象被从一个赋给另外一个，
  // 期望在赋值过程中只有String对象被涉及。
  // 在String参与的赋值语句中，StringValue的值没有发生变化，只是它的引用计数被修改了。
  // 不过，可以想象，一些还没有写出来的类在将来某天可能从RCObject派生出来，
  // 并希望允许被引用计数的值被赋值（见Item M23和Item E16）。
  // 如果这样的话，RCObject的赋值操作应该做正确的事情，而这个正确的事情就是什么都不做。
  // 假设希望允许在StringValue对象间赋值：
  // 对于给定的StringValue对象sv1和sv2，在赋值过程中，它们的引用计数值上发生什么？
  // sv1 = sv2;
  // 在赋值之前，已经有一定数目的String对象指向sv1。
  // 这个值在赋值过程中没有被改变，因为只是sv1的值被改变了。
  // 同样的，一定数目的String对象在赋值之前指向前v2，在赋值后，sv2的引用计数同样没有改变。
  // 当RCObject在赋值过程中被涉及时，指向它的对象的数目没有受影响，
  // 因此RCObject::operator=不应该改变引用计数值。
  RCObject& operator=(const RCObject& rhs) { return *this; }

  // removeReference不但负责减少对象的refCount值，还负责当refCount值为0时析构对象。
  // 后者是通过delete this来实现的，如Item
  // M27中解释的，这只当知道*this是堆对象时才安全。
  // 要让这个类正确，必须确保RCObject只能被构建在堆中，实现这一点的常用方法见Item
  // M27。 另外一个习惯方法：RCObject被设计为只作被引用计数的值对象的基类使用，
  // 而这些值对象应该只通过灵巧指针RCPtr引用。
  // 此外，值对象应该只能由值会共享的对象来实例化，它们不能被按通常的方法使用。
  // 在例子中，值对象的类是StringValue，通过将它申明为String的私有而限制其使用。
  // 只有String可以创建StringValue，所以String的作者应该确保这些对象都是通过new产成的。
  // 于是，限制RCObject只能在堆上创建的方法就是指定一组满足这个要求的类，
  // 并确保只有这些类能创建RCObject对象。
  // 用户不可能无意地（或有意地）用一种不恰当的方法创建RCObject对象。
  // 限制了创建被引用计数对象的权力，当交出这个权力时，
  // 必须明确其附带条件是满足创建对象的限制条件。
  void removeReference() {
    if (--refCount == 0) {
      delete this;
    }
  }

  virtual ~RCObject() {}
  void addReference() { ++refCount; }
  void markUnshareable() { shareable = false; }
  bool isShareable() const { return shareable; }
  bool isShared() const { return refCount > 1; }

 private:
  int refCount;
  bool shareable;
};

// 自动的引用计数处理
// RCObject类给了一个存储引用计数的地方，并提供了成员函数供操作引用计数，
// 但调用这些函数的动作还必须被手工加入其它类中。
// 仍要在String的拷贝构造函数和赋值操作符时，
// 调用StringValue的addReference和removeReference函数，这很笨拙。
// 想将这些调用放到可重用的类中，以使得String的作者不用再担心引用计数的任何细节。
// 没有一个简单的方法将所有引用计数方面的工作从所有的类中移出来；
// 但有一个方法可以从大部分类中将大部分工作移出来。
// 在一些类中，该方法可以消除所有引用计数方面的代码，但String类不是其中之一。
// 有一个成员函数搞坏了这件事：非const版本的operator[]。
// 供引用计数对象使用的智能指针模板：
template <class T>
class RCPtr {
 public:
  RCPtr(T* realPtr = 0) : pointee(realPtr) { init(); }
  RCPtr(const RCPtr& rhs) : pointee(rhs.pointee) { init(); }
  ~RCPtr() {
    if (pointee) {
      pointee->removeReference();
    }
  }
  RCPtr& operator=(const RCPtr& rhs) {
    if (pointee != rhs.pointee) {
      if (pointee) {
        pointee->removeReference();
      }
      pointee = rhs.pointee;
      init();
    }
    return *this;
  }
  T* operator->() const { return pointee; }
  T& operator*() const { return *pointee; }

 private:
  T* pointee;
  void init() {
    if (pointee == 0) return;
    if (pointee->isShareable() == false) {
      pointee = new T(*pointee);
    }
    // 当一个RCPtr构建时，它指向的对象需要增加引用计数值：
    pointee->addReference();
  }
};

class String {
 public:
  // 不用实现拷贝构造函数、赋值运算在哪里和析构函数：不再需要那些函数了！
  // 确实，String对象的拷贝仍然被支持，
  // 并且，这个拷贝将正确处理藏在后面的被引用计数的StringValue对象，
  // 但String类不需要写下哪怕一行代码来让它发生。
  // 因为编译器为String自动生成的拷贝构造函数将自动调用其RCPtr成员的拷贝构造函数，
  // 而这个拷贝构造函数完成所有必须的对StringValue对象的操作，包括它的引用计数。
  // RCPtr是一个智能指针，所以这是它将完成的工作。
  // 它同样处理赋值和析构，所以String类同样不需要写出这些函数。
  // 最初目的是将不可重用的引用计数代码，
  // 从自己写的String类中移到一个与运行环境无关的类中以供任何其它类使用。
  // 现在，通过用RCObject和RCPtr两个类完成了这一点。

  String(const char* value = "") : value(new StringValue(value)) {}
  const char& operator[](int index) const { return value->data[index]; }
  char& operator[](int index) {
    if (value->isShared()) {
      value = new StringValue(value->data);
    }
    value->markUnshareable();
    return value->data[index];
  }

  void print() {
    std::cout << (void*)value->data << ":" << value->data << std::endl;
  }

 private:
  struct StringValue : public RCObject {
    char* data;
    StringValue(const char* initValue) { init(initValue); }
    StringValue(const StringValue& rhs) { init(rhs.data); }
    void init(const char* initValue) {
      data = new char[strlen(initValue) + 1];
      strcpy(data, initValue);
    }
    ~StringValue() { delete[] data; }
  };
  RCPtr<StringValue> value;
};

// 和用内建指针实现的版本相比较，会受到两件事的打击：
// 1.代码有很多的减少，因为RCPtr完成了大量以前在String内部完成的处理引用计数的担子。
// 2.剩下的代码几乎没有变化：智能指针无缝替换了内建指针。
//   实际上，唯一的变化是在operator[]里，用调用isShared函数代替了直接检查refCount的值，
//   并用灵巧指针RCPtr对象消除了写时拷贝时手工维护引用计数值的工作。
// 这当然全都很漂亮，谁能反对减少代码？谁能反对成功的封装？
// 然而，这个全新的String类本身对用户的冲击远胜过它的实现细节，这才是真正的闪光点。
// 如果没有什么消息是好消息的话，这本身就是最好的消息，String的接口没有改变！
// 增加了引用计数，增加了标记某个String的值为不可共享的能力，
// 将引用计数功能移入一个新类，增加了智能指针来自动处理引用计数，但用户的一行代码都不需要修改。
// 当然，改变了String类的定义，所以需要重新编译和链接，但他们在代码上的投资受到了完全的保护。
// 看到了吗？封装确实是个很好的东西。

void func1() {
  String a, b, c, d, e;
  a = b = c = d = e = "Hello";

  a.print();
  b.print();
  c.print();
  d.print();
  e.print();

  // 0x5586f6a59010:Hello
  // 0x5586f6a59010:Hello
  // 0x5586f6a59010:Hello
  // 0x5586f6a59010:Hello
  // 0x5586f6a59010:Hello
}
void func2() {
  String s1 = "World";
  char* p = &s1[1];
  char& r = s1[2];
  String s2 = s1;
  s1.print();  // 0x5641cc3aaed0:World
  s2.print();  // 0x5641cc3aaf10:World
  *p = 'x';
  s1.print();  // 0x5641cc3aaed0:Wxrld
  s2.print();  // 0x5641cc3aaf10:World
  r = 'z';
  s1.print();  // 0x5641cc3aaed0:Wxzld
  s2.print();  // 0x5641cc3aaf10:World
}

void func() {
  func1();
  func2();
}
}  // namespace n4

namespace n5 {
// 在现存类上增加引用计数：到现在为止，所讨论的都假设能够访问有关类的源码。
// 但如果想让一个位于支撑库中而无法修改的类获得引用计数的好处呢？
// 不可能让它们从RCObject继承的，所以也不能对它们使用灵巧指针RCPtr。

// 只要对设计作小小的修改，就可以将引用计数加到任意类型上：
// 增加一个类RCWidget以供用户使用，而所有的事情都和String/StringValue的例子一样，
// RCWidget和String相同，Widget和StringValue相同。
// 计算机科学中的绝大部分问题都可以通过增加一个中间层次来解决。
// 增加一个新类CountHolder以处理引用计数，它从RCObject继承。
// 让CountHolder包含一个指针指向Widget，然后用等价的智能指针RCIPter替代RCPtr模板，
// 它知道CountHolder类的存在，名字中的i表示间接indirect。

template <class T>
class RCIPtr {
 public:
  RCIPtr(T* realPtr = 0) : counter(new CountHolder) {
    counter->pointee = realPtr;
    init();
  }
  RCIPtr(const RCIPtr& rhs) : counter(rhs.counter) { init(); }
  ~RCIPtr() { counter->removeReference(); }
  RCIPtr& operator=(const RCIPtr& rhs) {
    if (counter != rhs.counter) {
      counter->removeReference();
      counter = rhs.counter;
      init();
    }
    return *this;
  }
  const T* operator->() const { return counter->pointee; }
  T* operator->() {
    makeCopy();
    return counter->pointee;
  }
  const T& operator*() const { return *(counter->pointee); }
  T& operator*() {
    makeCopy();
    return *(counter->pointee);
  }

 private:
  struct CountHolder : public n4::RCObject {
    ~CountHolder() { delete pointee; }
    T* pointee;
  };
  CountHolder* counter;
  void init() {
    if (counter->isShareable() == false) {
      T* oldValue = counter->pointee;
      counter = new CountHolder;
      counter->pointee = new T(*oldValue);
    }
    counter->addReference();
  }
  void makeCopy() {
    if (counter->isShared()) {
      T* oldValue = counter->pointee;
      counter->removeReference();
      counter = new CountHolder;
      counter->pointee = new T(*oldValue);
      counter->addReference();
    }
  }

  // RCIPtr与RCPtr只两处不同：
  // 1.RCPtr对象直接指向值对象，而RCIptr对象通过中间层的CountHolder对象指向值对象；
  // 2.RCIPtr重载了operator->和operator*，当有对被指向的对象的非const的操作时，
  //   写时拷贝自动被执行。
};

// 有了RCIPtr，很容易实现RCWidget，
// 因为RCWidget的每个函数都是将调用传递给RCIPtr以操作Widget对象。
class Widget {
 public:
  Widget(int size) : data(size) {}
  Widget(const Widget& rhs) : data(rhs.data) {}
  ~Widget() {}
  Widget& operator=(const Widget& rhs) {
    data = rhs.data;
    return *this;
  }
  void doThis() {}
  int showThat() const {
    std::cout << (void*)&data << ":" << data << std::endl;
    return 0;
  }

 private:
  int data;
};
class RCWidget {
 public:
  RCWidget(int size) : value(new Widget(size)) {}
  void doThis() { value->doThis(); }
  int showThat() const { return value->showThat(); }

 private:
  RCIPtr<Widget> value;
};
// 注意RCWidget的构造函数是怎么用它的参数调用Widget的构造函数的，通过new，见Item M8；
// RCWidget的doThis怎么调用Widget的doThis函数的；
// 以及RCWidget的showThat怎么返回Widget的showThat的返回值的。
// 同样要注意RCWidget没有申明拷贝构造函数和赋值操作函数，也没有析构函数。
// 如同String类一样，它不需要这些函数，感谢于RCIPtr，RCWidget的默认版本将完成正确的事情。

void func() {
  RCWidget w1(1);
  RCWidget w2(1);
  RCWidget w3(w1);
  RCWidget w4 = w1;

  w1.showThat();
  w2.showThat();
  w3.showThat();
  w4.showThat();

  // 0x55806e200eb0:1
  // 0x55806e200ef0:1
  // 0x55806e200eb0:1
  // 0x55806e200eb0:1
}
}  // namespace n5

// 什么时候使用引用计数？实现引用计数不是没有代价的。
// 每个被引用的值带一个引用计数，其大部分操作都需要以某种形式检查或操作引用计数。
// 对象的值需要更多的内存，而在处理它们时需要执行更多的代码。
// 此外，就内部的源代码而言，带引用计数的类的复杂度比不带的版本高。
// 没有引用计数的String类只依赖于自己，而最终的String类如果没有三个辅助类就无法使用，
// StringValue、RCObject和RCPtr，这个更复杂的设计确保在值可共享时的更高的效率；
// 免除了跟踪对象所有权的需要，提高了引用计数的想法和实现的可重用性。
// 但，这四个类必须写出来、被测试、文档化、和被维护，比单个类要多做更多的工作。
// 即使是管理人员也能看出这点，引用计数是基于对象通常共享相同的值的假设的优化技巧。
// 如果假设不成立的话，引用计数将比通常的方法使用更多的内存和执行更多的代码。
// 另一方面，如果对象确实有具体相同值的趋势，那么引用计数将同时节省时间和空间。
// 共享的值所占内存越大，同时共享的对象数目越多，节省的内存也就越大。
// 创建和销毁这个值的代价越大，节省的时间也越多，总之，引用计数在下列情况下对提高效率很有用：
// 1.少量的值被大量的对象共享；
// 2.这样的共享通常通过调用赋值操作和拷贝构造而发生；
// 3.对象/值的比例越高，越是适宜使用引用计数；
// 4.对象的值的创建和销毁代价很高昂，或它们占用大量的内存，
//   即使这样，如果不是多个对象共享相同的值，引用计数仍然帮不了任何东西。
// 只有一个方法来确认这些条件是否满足，而不是猜测或依赖直觉，使用profiler或其它工具来分析。
// 使用这种方法，可以发现是否创建和销毁值的行为是性能瓶颈，并能得出对象/值的比例。
// 只有当手里有了这些数据，才能得出是否从引用计数上得到的好处超过其缺点。
// 即使上面的条件满足了，使用引用计数仍然可能是不合适的。
// 有些数据结构（如有向图）将导致自我引用或环状结构。
// 这样的数据结构可能导致孤立的自引用对象，它没有被别人使用，而其引用计数又绝不会降到零。
// 因为这个无用的结构中的每个对象被同结构中的至少一个对象所引用。
// 商用化的垃圾收集体系使用特别的技术来查找这样的结构并消除它们，
// 但现在使用的这个简单的引用计数技术不是那么容易扩充出这个功能的。
// 即使效率不是主要问题，引用计数仍然很吸引人，如果不放心谁应该去执行删除动作，
// 那么引用计数正是这种放下担子的技巧，很多程序员只因为这个原因就使用引用计数。

int main(int argc, char* argv[]) {
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
      n4::func();
      break;
    case 4:
      n5::func();
      break;
    default:
      std::cout << "invalid type" << std::endl;
      break;
  }

  return 0;
}