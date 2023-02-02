#include <cstring>
#include <iostream>

// 代理类

namespace n1 {
// 实现二维数组：

// 在BASIC甚至是COBOL中，可以创建二维、三维乃至n维数组，
// FORTRAN只能创最多7维数组。
// 但在C++中呢？只是有时可以，而且也只是某种程度上的。

namespace test1 {

void processInput(int dim1, int dim2) {
  int data1[dim1][dim2];  // 现在可以，以前报错
  // int *data2 = new int[dim1][dim2];  // 报错
}
void func() {
  int data[10][20];
  processInput(10, 20);
}
}  // namespace test1

namespace test2 {
// 多维数组在C++中的有用程度和其它语言相同，
// 所以找到一个象样的支持方法是很重要的。
// 常用方法是C++中的标准方法：
// 用一个类来实现所需要的而C++语言中并没有提供的东西。
// 因此，可以定义一个类模板来实现二维数组：
template <class T>
class Array2D {
 public:
  Array2D(int dim1, int dim2) {}

  // 最初的冲动可能是申明一个operator[][]函数：
  // T& operator[][](int index1, int index2);
  // const T& operator[][](int index1, int index2) const;
  // 然而并没有operator[][]这种东西，所有可以重载的运算符见Item M7。

  // 如果能容忍奇怪的语法，可能会学其它语言使用()来索引数组。
  // 这么做，只需重载operator()：
  T& operator()(int index1, int index2) {}
  const T& operator()(int index1, int index2) const {}
  // 这很容易实现，并很容易推广到任意多维的数组。
  // 缺点是Array2D对象看起来和内嵌数组一点都不象，
  // 访问元素(5,6)的操作看起来相函数调用。
};
void processInput(int dim1, int dim2) { Array2D<int> a(dim1, dim2); }
void func() {
  Array2D<int> a1(10, 20);
  Array2D<int>* a2 = new Array2D<int>(10, 20);
  processInput(10, 20);

  // 然而，使用这些Array2D对象并不直接了当。
  // 根据C和C++中的语法习惯，应该能够使用[]来索引数组：
  // std::cout << a1[5][6];  // 报错

  std::cout << a1(5, 6) << std::endl;
}
}  // namespace test2

namespace test3 {
// 如果拒绝让访问数组行为看起来象FORTRAN语言，将再次会到使用[]上来。
// 虽然没有operator[][]，但写出下面这样的代码是合法的：
void func1() {
  int a[10][20];
  std::cout << a[3][6] << std::endl;
  // a不是真正的二维数组，它是一个10元素的一维数组。
  // 其中每一个元素又都是一个20元素的数组，
  // 所以表达式data[3][6]实际上是(data[3])[6]，
  // 也就是data的第四个元素这个数组的第7个元素。
  // 简而言之，第一个[]返回的是一个数组，
  // 第二个[]从这个返回的数组中再去取一个元素。
}

// 可以通过重载Array2D类的operator[]来玩同样的把戏。
// Array2D的operator[]返回一个新类Array1D的对象。
// 再重载Array1D的operator[]来返回所需要的二维数组中的元素：
template <typename T>
class Array2D {
  // Array2D的用户并不需要知道Array1D类的存在。
  // 这个背后的一维数组对象从概念上来说，并不是为Array2D类的用户而存在的。
  // 其用户编程时就象他们在使用真正的二维数组一样。
  // 对于Array2D类的用户这样做是没有意义的：
  // 为了满足C++的反复无常，
  // 这些对象必须在语法上兼容于其中的元素是另一个一维数组的一个一维数组。

  // 每个Array1D对象扮演的是一个一维数组，
  // 而这个一维数组没有在使用Array2D的程序中出现。
  // 扮演其它对象的对象通常被称为代理类。
  // 在这个例子里，Array1D是一个代理类。
  // 它的实例扮演的是一个在概念上不存在的一维数组。
  // 术语代理对象（proxy object）和代理类（proxy classs）还不是很通用；
  // 这样的对象有时被叫做surrogate。
 public:
  class Array1D {
   public:
    T& operator[](int index) {}
    const T& operator[](int index) const {}
  };
  Array2D(int dim1, int dim2) {}
  Array1D& operator[](int index) {}
  const Array1D& operator[](int index) const {}
};
void func2() {
  Array2D<int> a(10, 20);
  std::cout << a[3][6] << std::endl;
}

void func() {
  func1();
  func2();
}
}  // namespace test3

void func() {
  test1::func();
  test2::func();
  test3::func();
}
}  // namespace n1

namespace n2 {
// 使用代理来实现多维数组是很通用的的方法，但代理类的用途远不止这些。
// Item M5中展示了代理类可以怎样用来阻止单参数的构造函数被误用为类型转换函数。
namespace test1 {
template <class T>
class Array {
 public:
  // 不能做为类型转换函数：
  Array(int lowBound, int highBound);
  // 能做为类型转换函数使用，能导致无穷的痛苦：
  Array(int size) {}
  T& operator[](int index) {}
  friend bool operator==(const Array<int>& lhs, const Array<int>& rhs);
};
bool operator==(const Array<int>& lhs, const Array<int>& rhs) {}
void func() {
  Array<int> a(10);
  Array<int> b(10);
  for (int i = 0; i < 10; ++i) {
    // 希望编译器能报出各种各样的警告信息，但是它根本没有。
    // 编译器当成下面这样：
    // if (a == static_cast<Array<int>>(b[i]))
    if (a == b[i]) {  // 拼写错误，a应该是a[i]
      std::cout << "a[" << i << "] == b[" << i << "]" << std::endl;
    } else {
      std::cout << "a[" << i << "] != b[" << i << "]" << std::endl;
    }
  }
}
}  // namespace test1

// 想给调用者提供一个单参数构造函数。
// 同时也希望防止编译器不加鉴别地调用这个构造函数。
// 有两个方法：
// 1.容易的方法，利用一个最新编译器的特性，explicit关键字。
//   为了解决隐式类型转换而特别引入的这个特性，它的使用方法很好理解。
//   构造函数用explicit声明，如果这样做，
//   编译器会拒绝为了隐式类型转换而调用构造函数，显式类型转换依然合法。
namespace test2 {
template <class T>
class Array {
 public:
  explicit Array(int size) {}
  T& operator[](int index) {}
  friend bool operator==(const Array<int>& lhs, const Array<int>& rhs);
};
bool operator==(const Array<int>& lhs, const Array<int>& rhs) {}

void func() {
  // 正确, explicit构造函数在建立对象时能正常使用：
  Array<int> a(10);
  Array<int> b(10);
  for (int i = 0; i < 10; ++i) {
    // if (a == b[i])  // 错误，没有办法隐式转换int到Array<int>
    if (a == Array<int>(b[i])) {
      // 正确,显式从int到Array<int>转换，但是代码的逻辑不合理
    }
    if (a == static_cast<Array<int> >(b[i])) {
      // 同样正确，同样不合理
    }
    if (a == (Array<int>)b[i]) {
      // C风格的转换也正确，但是逻辑依旧不合理
    }
  }
}
}  // namespace test2

// 2.如果编译器不支持explicit，
//   不得不回到不使用成为隐式类型转换函数的单参数构造函数。
//   必须修改Array的单参数构造函数，用一个ArraySize对象来代替int。
namespace test3 {
template <class T>
class Array {
 public:
  // 这里把ArraySize嵌套入Array中，为了强调它总是与Array一起使用。
  // 你也必须声明ArraySize为公有，为了让任何人都能使用它。
  class ArraySize {
   public:
    ArraySize(int numElements) : theSize(numElements) {}
    int size() const { return theSize; }

   private:
    int theSize;
  };
  Array(int lowBound, int highBound) {}
  Array(ArraySize size) {}
  T& operator[](int index) {}
  friend bool operator==(const Array<int>& lhs, const Array<int>& rhs);
};
bool operator==(const Array<int>& lhs, const Array<int>& rhs) {}

void func() {
  // 编译器要求用int参数调用Array<int>里的构造函数，
  // 但是没有这样的构造函数。
  // 编译器意识到它能从int参数转换成一个临时ArraySize对象，
  // ArraySize对象只是Array<int>构造函数所需要的，
  // 这样编译器进行了转换。
  Array<int> a(10);
  // 事实上仍旧能够安心地构造Array对象，不过这样做能够使你避免类型转换:
  for (int i = 0; i < 10; ++i) {
    // 为了调用operator==函数，编译器要求Array<int>对象在==右侧，
    // 但是不存在一个参数为int的单参数构造函数。
    // 而且编译器无法把int转换成一个临时ArraySize对象，
    // 然后通过这个临时对象建立必须的Array<int>对象，
    // 因为这将调用两个用户定义（user-defined）的类型转换，
    // 一个从int到ArraySize，一个从ArraySize到Array<int>。
    // 这种转换顺序被禁止的，所以当试图进行比较时编译器肯定会产生错误。
    // if (a == b[i]);  // 报错
  }
}
}  // namespace test3

void func() {
  test1::func();
  test2::func();
  test3::func();
}
}  // namespace n2

namespace n3 {
// 在代理类的各中用法中，最神奇的是帮助区分operator[]进行的是读操作还是写操作。

namespace test1 {
// operator[]可以在两种不同的情况下调用：
// 1.读一个字，右值操作，意味着不能被修改；
// 2.写一个字符，左值操作，意味着可能被修改。

// 想区分将operator[]用作左值还是右值，因为，
// 对于有引用计数的数据结构，读操作的代价可以远小于写操作的代价。
// 如Item M29解释的，引用计数对象的写操作将导致整个数据结构的拷贝，
// 而读不需要，只要简单地返回一个值。
// 不幸的是，在operator[]内部，没有办法确定它是怎么被调用的，
// 不可能区分出它是做左值还是右值。
class String {
 public:
  String() : value(new char[10]) {}

  // 编译器根据调用对象的const属性来选择此成员函数的const和非const版本，
  // 而不考虑调用时的环境，因此基于const属性重载operator[]，
  // 还是不能区分operator[]用于读操作还是写操作。
  char& operator[](int index) {
    std::cout << "char &operator[](int index)" << std::endl;
    return value[index];
  }
  const char& operator[](int index) const {
    std::cout << "const char &operator[](int index) const" << std::endl;
    return value[index];
  }

 private:
  char* value;
};
void func() {
  // 对象不是const的，所以对于operator[]，
  // 不论是读还是写，都调用non const版本的operator[]：
  String s1, s2;
  s1[5];  // 读s1
  // char &operator[](int index)
  s2[6] = 'x';  // 写s2
  // char &operator[](int index)
  s1[3] = s2[8];  // 读s2写s1
  // char &operator[](int index)
  // char &operator[](int index)

  // 只有const对象，才能调用const版本的operator[]：
  const String s3;
  s3[5];  // 读s3
  // const char &operator[](int index) const
}
}  // namespace test1

namespace test2 {
// 也许不可能在operator[]内部区分左值还是右值操作，
// 但仍然能区别对待读操作和写操作，
// 如果将判断读还是写的行为推迟到我们知道operator[]的结果被怎么使用之后的话。
// 所需要的是有一个方法将读或写的判断推迟到operator[]返回之后。
// 这是lazy原则（见Item M17）的一个例子。

// proxy类可以让得到所需要的时机，
// 因为可以修改operator[]让它返回一个（代理字符的）proxy对象而不是字符本身：
// 如果是读它，可以断定operator[]的调用是读；
// 如果它被写，必须将operator[]的调用处理为写。

// 在proxy类上只能做三件事：
// 1.创建它，也就是指定它扮演哪个字符；
// 2.将它作为赋值操作的目标，在这种情况下可以将赋值真正作用在它扮演的字符上，
//   这样被使用时，proxy类扮演的是左值；
// 3.用其它方式使用它，这时，代理类扮演的是右值。

// 带引用计数的String类用proxy类以区分operator[]是作左值还是右值使用：
class RCObject {
 public:
  RCObject() : refCount(0), shareable(true) {}
  RCObject(const RCObject& rhs) : refCount(0), shareable(true) {}
  RCObject& operator=(const RCObject& rhs) { return *this; }
  void removeReference() {
    if (--refCount == 0) {
      delete this;
    }
  }
  virtual ~RCObject(){};
  void addReference() { ++refCount; }
  void markUnshareable() { shareable = false; }
  bool isShareable() const { return shareable; }
  bool isShared() const { return refCount > 1; }

 private:
  int refCount;
  bool shareable;
};
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
    pointee->addReference();
  }
};
class String {
 public:
  String(const char* value = "") : value(new StringValue(value)) {}
  class CharProxy {
   public:
    CharProxy(String& str, int index) : theString(str), charIndex(index) {}

    // 再看CharProxy的赋值操作的实现，
    // 这是必须处理proxy对象所扮演的字符作赋值的目标（即左值）使用的地方：
    CharProxy& operator=(const CharProxy& rhs) {
      // 在Item M29中，悲观地假设所有非const的operator[]的调用都是写操作。
      // 现在，将写操作的实现移入CharProxy的赋值操作中，
      // 于是可以避免非const的operator[]的调用只是作右值时所多付出的写操作的代价。
      // 随便提一句，这个函数需要访问String的私有数据成员value，
      // 这是将CharProxy申明为String的友元的原因。
      std::cout << "CharProxy& operator=(const CharProxy& rhs)" << std::endl;
      if (theString.value->isShared()) {
        theString.value = new StringValue(theString.value->data);
      }
      theString.value->data[charIndex] =
          rhs.theString.value->data[rhs.charIndex];
      return *this;
    }
    CharProxy& operator=(char c) {
      std::cout << "CharProxy& operator=(char c)" << std::endl;
      if (theString.value->isShared()) {
        theString.value = new StringValue(theString.value->data);
      }
      theString.value->data[charIndex] = c;
      return *this;
    }

    // 将proxy对象作右值使用时很简单，只需返回它所扮演的字符就可以了：
    operator char() const {
      // 因为这个函数返回了一个字符的值，
      // 并且又因为C++限定这样通过值返回的对象只能作右值使用，
      // 所以这个转换函数只能出现在右值的位置。
      std::cout << "operator char() const" << std::endl;
      return theString.value->data[charIndex];
    }
    const char* operator&() const {
      // const版本返回其扮演的字符的const型的指针：
      return &(theString.value->data[charIndex]);
    }
    char* operator&() {
      // 非const版本有多一些操作，因为它返回的指针指项的字符可以被修改。
      // 它和Item M29中的非const的String::operator[]行为相似，
      // 实现也很接近：
      if (theString.value->isShared()) {
        theString.value = new StringValue(theString.value->data);
      }
      theString.value->markUnshareable();
      return &(theString.value->data[charIndex]);
    }

   private:
    String& theString;
    int charIndex;
  };
  // 这个String类与Item M29中的最终版本相比，
  // 唯一不同之处就是所有的operator[]函数现在返回的是CharProxy对象。
  // 然而，String类的用户可以忽略这一点，
  // 并当作operator[]返回的仍然是通常形式的字符（或其引用，见Item M1）来编程。
  const CharProxy operator[](int index) const {
    // operator[]的const版本返回一个const的proxy对象。
    // 因为CharProxy::operator=是个非const的成员函数，
    // 这样的const proxy对象不能作赋值的目标使用。
    // 因此，不管是从operator[]的const版本返回的proxy对象，
    // 还是它所扮演的字符都不能作左值使用。
    // 很方便啊，它正好是想要的const版本的operator[]的行为。

    // 同样要注意在const的operator[]返回而创建CharProxy对象时，
    // 对*this使用的const_cast。
    // 这使得它满足了CharProxy类的构造函数的需要，
    // 它的构造函数只接受一个非const的String类。
    // 类型转换通常是领人不安的，但在此处，
    // operator[]返回的CharProxy对象自己是const的，
    // 所以不用担心String内部的字符可能被通过proxy类被修改。

    std::cout << "const CharProxy operator[](int index) const" << std::endl;
    return CharProxy(const_cast<String&>(*this), index);
  }
  CharProxy operator[](int index) {
    // 通过operator[]返回的proxy对象，
    // 记录了它属于哪个string对象以及所扮演的字符的下标：
    std::cout << "CharProxy operator[](int index)" << std::endl;
    return CharProxy(*this, index);
  }
  // operator[]每个函数都创建和返回一个proxy对象来代替字符。
  // 根本没有对那个字符作任何操作：将它推迟到直到知道是读操作还是写操作。

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

void func1() {
  String s1("1234567890");
  String s2(s1);

  s1[5];
  // CharProxy operator[](int index)

  // 表达式s1[2]返回的是一CharProxy对象。
  // 没有为这样的对象定义输出流操作，
  // 所以编译器努力地寻找一个隐式的类型转换以使得operator<<调用成功（见Item M5）。
  // 它们找到一个：在CharProxy类内部申明了一个隐式转换到char的操作。
  // 于是自动调用这个转换操作，结果就是CharProxy类扮演的字符被打印输出了。
  // 这个CharProxy到char的转换是所有代理对象作右值使用时发生的典型行为：
  std::cout << s1[2] << std::endl;
  // CharProxy operator[](int index)
  // operator char() const
  // 3

  // 表达式s2[6]返回的是一个CharProxy对象，但这次它是赋值操作的目标。
  // 由于赋值的目标是CharProxy类，所以调用的是CharProxy类中的赋值操作。
  // 这至关重要，因为在CharProxy的赋值操作中，
  // 知道被赋值的CharProxy对象是作左值使用的。
  // 因此，知道proxy类扮演的字符是作左值使用的，
  // 必须执行一些必要的操作以实现字符的左值操作：
  s2[6] = 'x';
  // CharProxy operator[](int index)
  // CharProxy& operator=(char c)

  // 调用作用于两个CharProxy对象间的赋值操作，
  // 在此操作内部，知道左边一个是作左值，右边一个作右值：
  s1[3] = s2[8];
  // CharProxy operator[](int index)
  // CharProxy operator[](int index)
  // CharProxy& operator=(const CharProxy& rhs)

  const String s3("1234567890");
  std::cout << s3[6] << std::endl;
  // const CharProxy operator[](int index) const
  // operator char() const
  // 7
}

void func2() {
  // 使用proxy类是个区分operator[]作左值还是右值的好方法，但它不是没有缺点的。
  // 很喜欢proxy对象对其所扮演的对象的无缝替代，但这很难实现。
  // 这是因为，右值不只是出现在赋值运算的情况下，
  // 那时，proxy对象的行为就和实际的对象不一致了。
  String s1 = "Hello";
  char* p = &s1[1];  // 如果没有重载CharProxy类的取地址运算则报错
  // 表达式s1[1]返回一个CharProxy，于是=的右边是一个CharProxy *。
  // 没有从CharProxy *到char *的转换函数，所以p的初始化过程编译失败了。
  // 通常，取proxy对象地址的操作与取实际对象地址的操作得到的指针，其类型是不同的。
  // 要消除这个不同，需要重载CharProxy类的取地址运算。
}

void func() {
  func1();
  func2();
}
}  // namespace test2
}  // namespace n3

namespace n4 {
// char和代理它的CharProxy的不同之处出现在带引用计数的数组模板中，
// 如果想用proxy来区分operator[]作左值还是右值时：
template <class T>
class Array {
 public:
  class Proxy {
   public:
    Proxy(Array<T>& array, int index) {}
    Proxy& operator=(const T& rhs) {}
    operator T() const;
  };
  const Proxy operator[](int index) const { return *this; }
  Proxy operator[](int index) {}
};
void func1() {
  Array<int> a;

  // 当operator[]作最简单的赋值操作的目标时，是成功的：
  a[5] = 22;

  // 但当它出现operator+=和operator++的左侧时，失败了：
  // a[5] += 5;  // 报错
  // ++a[5];     // 报错

  // 因为operator[]返回一个proxy对象，
  // 而它没有operator+=和operator++操作。
  // 同样的情况存在于其它需要左值的操作中，
  // 包括operator*=、operator<<=、operator--等等。
  // 如果想让这些操作作用在operator[]上，
  // 必须为Arrar<T>::Proxy类定义所有这些函数。
  // 这是一个极大量的工作，可能不愿意去做的。
  // 不幸的是，要么去做这些工作，要么没有这些操作，不能两全。
}

// 一个类似的问题必须面对：
// 通过proxy对象调用实际对象的成员函数，想避开它是不可能的。
class Rational {
 public:
  Rational(int numerator = 0, int denominator = 1);
  int numerator() const;
  int denominator() const;
};
void func2() {
  Array<Rational> a;
  // std::cout << a[4].numerator();    // 报错
  // int denom = a[22].denominator();  // 报错

  // operator[]返回一个proxy对象而不是实际的Rational对象。
  // 但成员函数numerator()和denominator()只存在于Rational对象上，
  // 而不是其proxy对象，因此，的编译器发出了抱怨。
  // 要使得proxy对象的行为和它们所扮演的对象一致，
  // 必须重载可作用于实际对象的每一个函数。
}
void func() {
  func1();
  func2();
}
}  // namespace n4

namespace n5 {
// 另一个proxy对象替代实际对象失败的情况是作为非const的引用传给函数：
void swap(char& a, char& b);
void func1() {
  n3::test2::String s = "+C+";
  // swap(s[0], s[1]);  // 报错

  // String::operator[]返回一个CharProxy对象，
  // 但swap()函数要求它所参数是char &类型。
  // 一个CharProxy对象可以隐式地转换为一个char，
  // 但没有转换为char &的转换函数。
  // 而它可能转换成的char并不能成为swap的char &参数，
  // 因为这个char是一个临时对象（它是operator char()的返回值），
  // 根据Item M19的解释，拒绝将临时对象绑定为非const的引用的形参是有道理的。
}

// 最后一种proxy对象不能无缝替换实际对象的情况是隐式类型转换。
// 当proxy对象隐式转换为它所扮演的实际对象时，
// 一个用户自定义的转换函数被调用了。
// 例如，一个CharProxy对象可以转换为它扮演的char，通过调用operator char()函数。
// 如Item M5解释的，编译器在调用函数而将参数转换为此函数所要的类型时，
// 只调用一个用户自定义的转换函数。
// 于是，很可能在函数调用时，传实际对象是成功的而传proxy对象是失败的。
class TVStation {
 public:
  TVStation(int channel) {}
};
void watchTV(const TVStation& station, float hoursToWatch) {}
void func2() {
  // 借助于int到TVStation的隐式类型转换，可以这么做：
  watchTV(10, 2.5);

  // 然而，当用proxy类区分operator[]作左右值的带引用计数的数组模板时，
  // 就不能这么做了：
  n4::Array<int> a;
  a[4] = 10;
  // watchTV(a[4], 2.5);  // 报错

  // 问题发生在隐式类型转换上，它很难解决。
  // 实际上，更好的设计应该是申明它的构造函数为explicit，
  // 以使得第一次的调用watchTV()的行为都编译失败。
  // 关于隐式类型转换的细节和explicit对此的影响见Item M5。
}
void func() {
  func1();
  func2();
}
}  // namespace n5

// Proxy类可以完成一些其它方法很难甚至不可能实现的行为：
// 1.多维数组；
// 2.左/右值的区分；
// 3.限制隐式类型转换（见Item M5）。
// 同时，proxy类也有缺点：
// 1.作为函数返回值，proxy对象是临时对象（见Item 19），
//   它们必须被构造和析构，这不是免费的，
//   虽然此付出能从具备了区分读写的能力上得到更多的补偿。
// 2.Proxy对象的存在增加了软件的复杂度，
//   因为额外增加的类使得事情更难设计、实现、理解和维护。
// 最后，从一个处理实际对象的类改换到处理proxy对象的类经常改变了类的语义，
// 因为proxy对象通常表现出的行为与实际对象有些微妙的区别。
// 有时，这使得在设计系统时无法选择使用proxy对象，
// 但很多情况下很少有操作需要将proxy对象暴露给用户。
// 例如，很少有用户取上面的二维数组例子中的Array1D对象的地址，
// 也不怎么有可能将下标索引的对象（见Item M5）作参数传给一个期望其它类型的函数。
// 在很多情况下，proxy对象可以完美替代实际对象。
// 当它们可以工作时，通常也是没有其它方法可采用的情况。

int main(int argc, char* argv[]) {
  if (argc < 2) {
    std::cout << argv[0] << " i [0 - 5]" << std::endl;
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
      n3::test1::func();
      break;
    case 3:
      n3::test2::func();
      break;
    case 4:
      n4::func();
      break;
    case 5:
      n5::func();
      break;
    default:
      std::cout << "invalid type" << std::endl;
      break;
  }

  return 0;
}