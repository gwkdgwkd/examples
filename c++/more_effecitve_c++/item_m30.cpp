#include <cstring>
#include <iostream>

using namespace std;

// 代理类

int data[10][20];
void processInput(int dim1, int dim2) {
  // 使用变量作为的大小的话，是不可以的：
  int data[dim1][dim2];  // 现在应该可以了，不报错了
  // int *data1 = new int[dim1][dim2];  // 错误
}

template <class T>
class Array2D {
 public:
  Array2D(int dim1, int dim2) {}
  T& operator()(int index1, int index2) {}
  const T& operator()(int index1, int index2) const {}

  class Array1D {
   public:
    T& operator[](int index) {}
    const T& operator[](int index) const {}
  };
  Array1D operator[](int index) {}
  const Array1D operator[](int index) const {}
};
void processInput1(int dim1, int dim2) {
  Array2D<int> data1(10, 20);                          // fine
  Array2D<float>* data2 = new Array2D<float>(10, 20);  // fine
  Array2D<int> data3(dim1, dim2);                      // fine
}

// 然而，使用这些array对象并不直接了当。
// 根据C和C++中的语法习惯，我们应该能够使用[]来索引数组：cout << data1[3][6];
// 但是没有这个操作符，我们不能重载。
// 如果你能容忍奇怪的语法，你可能会学其它语言使用()来索引数组。
// 这么做，你只需重载operator()。
// 这很容易实现，并很容易推广到任意多维的数组。
// 缺点是你的Array2D对象看起来和内嵌数组一点都不象。
// 实际上，上面访问元素(3,6)的操作看起来相函数调用。
// 变量data不是真正的二维数组，它是一个10元素的一维数组。
// 其中每一个元素又都是一个20元素的数组，所以表达式data[3][6]实际上是(data[3])[6]，
// 也就是data的第四个元素这个数组的第7个元素。
// 简而言之，第一个[]返回的是一个数组，第二个[]从这个返回的数组中再去取一个元素。
// 我们可以通过重载Array2D类的operator[]来玩同样的把戏。
// Array2D的operator[]返回一个新类Array1D的对象。
// 再重载Array1D的operator[]来返回所需要的二维数组中的元素。
// Array2D的用户并不需要知道Array1D类的存在。
// 这个背后的“一维数组”对象从概念上来说，并不是为Array2D类的用户而存在的。
// 其用户编程时就象他们在使用真正的二维数组一样。
// 对于Array2D类的用户这样做是没有意义的：
// 为了满足C++的反复无常，这些对象必须在语法上兼容于其中的元素是另一个一维数组的一个一维数组。
// 每个Array1D对象扮演的是一个一维数组，而这个一维数组没有在使用Array2D的程序中出现。
// 扮演其它对象的对象通常被称为代理类。
// 在这个例子里，Array1D是一个代理类。它的实例扮演的是一个在概念上不存在的一维数组。
// 术语代理对象（proxy object）和代理类（proxy classs）还不是很通用；
// 这样的对象有时被叫做surrogate。
void func1() {
  Array2D<int> d(10, 20);
  cout << d(3, 6) << endl;

  processInput1(4, 5);

  Array2D<float> f1(10, 20);
  cout << f1[3][4] << endl;
}

// 区分通过operator[]进行的是读操作还是写操作
// 使用代理来实现多维数组是很通用的的方法，但代理类的用途远不止这些。
// Item M5中展示了代理类可以怎样用来阻止单参数的构造函数被误用为类型转换函数。
// 在代理类的各中用法中，最神奇的是帮助区分通过operator[]进行的是读操作还是写操作。
// operator[]可以在两种不同的情况下调用：读一个字符或写一个字符。
// 读是个右值操作；写是个左值操作。
// 这个名词来自于编译器，左值出现在赋值运算的左边，右值出现在赋值运算的右边。
// 通常，将一个对象做左值使用意味着它可能被修改，做右值用意味着它不能够被修改。
// 想区分将operator[]用作左值还是右值，
// 因为，对于有引用计数的数据结构，读操作的代价可以远小于写操作的代价。
// 引用计数对象的写操作将导致整个数据结构的拷贝，而读不需要，只要简单地返回一个值。
// 不幸的是，在operator[]内部，没有办法确定它是怎么被调用的，不可能区分出它是做左值还是右值。
// 可以基于const属性重载operator[]，这样就可以区分读还是写了。
// 换句话说，你建议我们这么解决问题：
// class String {
// public:
//  const char& operator[](int index) const; // for reads
//  char& operator[](int index); // for writes
// };
// 这不能工作，编译器根据调用成员函数的对象的const属性来选择此成员函数的const和非const版本，
// 而不考虑调用时的环境。

// 也许不可能在operator[]内部区分左值还是右值操作，但我们仍然能区别对待读操作和写操作，
// 如果我们将判断读还是写的行为推迟到我们知道operator[]的结果被怎么使用之后的话。
// 我们所需要的是有一个方法将读或写的判断推迟到operator[]返回之后。
// proxy类可以让我们得到我们所需要的时机，
// 因为我们可以修改operator[]让它返回一个（代理字符的）proxy对象而不是字符本身。
// 我们可以等着看这个proxy怎么被使用。
// 如果是读它，我们可以断定operator[]的调用是读。
// 如果它被写，我们必须将operator[]的调用处理为写。
// 在proxy类上只能做三件事：
// 1.创建它，也就是指定它扮演哪个字符。
// 2.将它作为赋值操作的目标，在这种情况下可以将赋值真正作用在它扮演的字符上。
//   这样被使用时，proxy类扮演的是左值。
// 3.用其它方式使用它。这时，代理类扮演的是右值。
template <class T>
class RCPtr {
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
class RCObject {
 public:
  void addReference();
  void removeReference();
  void markUnshareable();
  bool isShareable() const;
  bool isShared() const;

 protected:
  RCObject();
  RCObject(const RCObject& rhs);
  RCObject& operator=(const RCObject& rhs);
  virtual ~RCObject() = 0;

 private:
  int refCount;
  bool shareable;
};
RCObject::RCObject() : refCount(0), shareable(true) {}
RCObject::RCObject(const RCObject&) : refCount(0), shareable(true) {}
RCObject& RCObject::operator=(const RCObject&) { return *this; }
RCObject::~RCObject() {}
void RCObject::addReference() { ++refCount; }
void RCObject::removeReference() {
  if (--refCount == 0) delete this;
}
void RCObject::markUnshareable() { shareable = false; }
bool RCObject::isShareable() const { return shareable; }
bool RCObject::isShared() const { return refCount > 1; }
class String {
 public:
  String(const char* value = "");

  class CharProxy {
   public:
    CharProxy(String& str, int index) : theString(str), charIndex(index) {}
    CharProxy& operator=(const CharProxy& rhs);
    CharProxy& operator=(char c);
    operator char() const;
    char* operator&();
    const char* operator&() const;

   private:
    String& theString;
    int charIndex;
  };
  const CharProxy operator[](int index) const;
  CharProxy operator[](int index);

 private:
  struct StringValue : public RCObject {
    char* data;
    StringValue(const char* initValue);
    StringValue(const StringValue& rhs);
    void init(const char* initValue);
    ~StringValue();
  };
  RCPtr<StringValue> value;
};
void String::StringValue::init(const char* initValue) {
  data = new char[strlen(initValue) + 1];
  strcpy(data, initValue);
}
String::StringValue::StringValue(const char* initValue) { init(initValue); }
String::StringValue::StringValue(const StringValue& rhs) { init(rhs.data); }
String::StringValue::~StringValue() { delete[] data; }
String::String(const char* initValue) : value(new StringValue(initValue)) {}
// 每个函数都创建和返回一个proxy对象来代替字符。
// 根本没有对那个字符作任何操作：我们将它推迟到直到我们知道是读操作还是写操作。
const String::CharProxy String::operator[](int index) const {
  // operator[]的const版本返回一个const的proxy对象。
  // 因为CharProxy::operator=是个非const的成员函数，这样的proxy对象不能作赋值的目标使用。
  // 因此，不管是从operator[]的const版本返回的proxy对象，还是它所扮演的字符都不能作左值使用。
  // 很方便啊，它正好是我们想要的const版本的operator[]的行为。
  return CharProxy(const_cast<String&>(*this), index);
  // 同样要注意在const的operator[]返回而创建CharProxy对象时，对*this使用的const_cast。
  // 这使得它满足了CharProxy类的构造函数的需要，它的构造函数只接受一个非const的String类。
  // 类型转换通常是领人不安的，但在此处，operator[]返回的CharProxy对象自己是const的，
  // 所以不用担心String内部的字符可能被通过proxy类被修改。
}
String::CharProxy String::operator[](int index) {
  // 通过operator[]返回的proxy对象记录了它属于哪个string对象以及所扮演的字符的下标：
  return CharProxy(*this, index);
}
String::CharProxy::operator char() const {
  // 将proxy对象作右值使用时很简，只需返回它所扮演的字符就可以了：
  cout << "char()" << endl;
  return theString.value->data[charIndex];
  // 因为这个函数返回了一个字符的值，并且又因为C++限定这样通过值返回的对象只能作右值使用，
  // 所以这个转换函数只能出现在右值的位置。
}
// 再看CahrProxy的赋值操作的实现，
// 这是我们必须处理proxy对象所扮演的字符作赋值的目标（即左值）使用的地方。
String::CharProxy& String::CharProxy::operator=(const CharProxy& rhs) {
  // 我们将写操作的实现移入CharProxy的赋值操作中，
  // 于是可以避免非const的operator[]的调用只是作右值时所多付出的写操作的代价。
  // 随便提一句，这个函数需要访问string的私有数据成员value。
  // 这是前面将CharProxy申明为string的友元的原因。
  if (theString.value->isShared()) {
    theString.value = new StringValue(theString.value->data);
  }
  theString.value->data[charIndex] = rhs.theString.value->data[rhs.charIndex];
  cout << "const operator=" << endl;
  return *this;
}
String::CharProxy& String::CharProxy::operator=(char c) {
  // 第二个CharProxy的赋值操作是类似的：
  // 作为一个资深的软件工程师， 你当然应该消除这两个赋值操作中的代码重复，
  // 应该将它们放入一个私有成员函数中供二者调用。
  if (theString.value->isShared()) {
    theString.value = new StringValue(theString.value->data);
  }
  theString.value->data[charIndex] = c;
  cout << "operator=" << endl;
  return *this;
}

// 局限性
// 使用proxy类是个区分operator[]作左值还是右值的好方法，但它不是没有缺点的。
// 我们很喜欢proxy对象对其所扮演的对象的无缝替代，但这很难实现。
// 这是因为，右值不只是出现在赋值运算的情况下，那时，proxy对象的行为就和实际的对象不一致了。
// 如果String::operator[]返回一个CharProxy而不是char&，下面的代码将不能编译：
// char* p = &s1[1];  // error!
// 表达式s1[1]返回一个CharProxy，于是“=”的右边是一个 CharProxy*。
// 没有从CharProxy*到char *的转换函数，所以p的初始化过程编译失败了。
// 通常，取proxy对象地址的操作与取实际对象地址的操作得到的指针，其类型是不同的。
// 要消除这个不同，你需要重载CharProxy类的取地址运算。
const char* String::CharProxy::operator&() const {
  // const版本返回其扮演的字符的 const型的指针：
  return &(theString.value->data[charIndex]);
}
char* String::CharProxy::operator&() {
  // 非const版本有多一些操作，因为它返回的指针指项的字符可以被修改。
  // 它和Item M29中的非const的String::operator[]行为相似，实现也很接近：
  if (theString.value->isShared()) {
    theString.value = new StringValue(theString.value->data);
  }
  theString.value->markUnshareable();
  return &(theString.value->data[charIndex]);
}

void func2() {
  String s1 = "1234567", s2 = "abcdefghijk";
  cout << s1[5] << endl;  // calls non-const operator[], because s1 isn't const
  s2[5] = 'x';            // also calls non-const operator[]: s2 isn't const
  s1[3] = s2[8];  // both calls are to non-const operator[], because both s1
  // char()
  // 6
  // operator=
  // const operator=

  char* p = &s1[1];
}

// char和代理它的CharProxy的第二个不同之处，
// 出现带引用计数的数组模板中如果我们想用proxy类来区分其operator[]作左值还是右值时：
template <class T>  // reference-counted array
class Array {       // using proxies
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

// 一个类似的问题必须面对：通过proxy对象调用实际对象的成员函数，想避开它是不可能的。
class Rational {
 public:
  Rational(int numerator = 0, int denominator = 1);
  int numerator() const;
  int denominator() const;
};
void func3() {
  Array<int> intArray;
  intArray[5] = 22;  // fine
  // intArray[5] += 5;  // error!
  // ++intArray[5];     // error!
  // 当operator[]作最简单的赋值操作的目标时，是成功的，
  // 但当它出现operator+=和operator++的左侧时，失败了。
  // 因为operator[]返回一个proxy对象，而它没有operator+=和operator++操作。
  // 同样的情况存在于其它需要左值的操作中，包括operator*=、operator<<=、operator--等等。
  // 如果你想让这些操作你作用在operator[]上，必须为Arrar<T>::Proxy类定义所有这些函数。
  // 这是一个极大量的工作，你可能不愿意去做的。
  // 不幸的是，你要么去做这些工作，要么没有这些操作，不能两全。

  Array<Rational> array;
  // 这是我们所期望的使用方式，但我们很失望：
  // cout << array[4].numerator();         // error!
  // int denom = array[22].denominator();  // error!
  // operator[]返回一个proxy对象而不是实际的Rational对象。
  // 但成员函数numerator()和denominator()只存在于Rational对象上，而不是其proxy对象。
  // 因此，你的编译器发出了抱怨。
  // 要使得proxy对象的行为和它们所扮演的对象一致，你必须重载可作用于实际对象的每一个函数。
}

// 另一个proxy对象替代实际对象失败的情况是作为非const的引用传给函数：
void swap(char& a, char& b);
void func3() {
  String s = "+C+";
  // swap(s[0], s[1]);
  // String::operator[]返回一个CharProxy对象，但swap()函数要求它所参数是char &类型。
  // 一个CharProxy对象可以印式地转换为一个char，但没有转换为char&的转换函数。
  // 而它可能转换成的char并不能成为swap的char&参数，
  // 因为这个char是一个临时对象（它是operator char()的返回值），
  // 根据Item M19的解释，拒绝将临时对象绑定为非const的引用的形参是有道理的。
}

// 最后一种proxy对象不能无缝替换实际对象的情况是隐式类型转换。
// 当proxy对象隐式转换为它所扮演的实际对象时，一个用户自定义的转换函数被调用了。
// 例如，一个CharProxy对象可以转换为它扮演的char，通过调用operator char()函数。
// 如Item M5解释的，编译器在调用函数而将参数转换为此函数所要的类型时，只调用一个用户自定义的转换函数。
// 于是，很可能在函数调用时，传实际对象是成功的而传proxy对象是失败的。、
class TVStation {
 public:
  TVStation(int channel) {}
};
void watchTV(const TVStation& station, float hoursToWatch) {}
// 由于问题发生在隐式类型转换上，它很难解决。
// 实际上，更好的设计应该是申明它的构造函数为explicit，以使得第一次的调用watchTV()的行为都编译失败。
void func4() {
  // 借助于int到TVStation的隐式类型转换，我们可以这么做：
  watchTV(10, 2.5);
  // 然而，当使用那个用proxy类区分operator[]作左右值的带引用计数的数组模板时，就不能这么做了：
  Array<int> intArray1;
  intArray1[4] = 10;
  // watchTV(intArray1[4], 2.5);
}

// Proxy类可以完成一些其它方法很难甚至不可能实现的行为。
// 多维数组是一个例子，左/右值的区分是第二个，限制隐式类型转换是第三个。
// 同时，proxy类也有缺点。作为函数返回值，proxy对象是临时对象，它们必须被构造和析构。
// 这不是免费的，虽然此付出能从具备了区分读写的能力上得到更多的补偿。
// Proxy对象的存在增加了软件的复杂度，因为额外增加的类使得事情更难设计、实现、理解和维护。
// 最后，从一个处理实际对象的类改换到处理proxy对象的类经常改变了类的语义，
// 因为proxy对象通常表现出的行为与实际对象有些微妙的区别。
// 有时，这使得在设计系统时无法选择使用proxy对象，
// 但很多情况下很少有操作需要将proxy对象暴露给用户。
// 例如，很少有用户取上面的二维数组例子中的Array1D对象的地址，
// 也不怎么有可能将下标索引的对象作参数传给一个期望其它类型的函数。
// 在很多情况下，proxy对象可以完美替代实际对象。
// 当它们可以工作时，通常也是没有其它方法可采用的情况。

int main() {
  func1();
  func2();
  func3();
  func4();

  return 0;
}