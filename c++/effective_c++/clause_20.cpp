#include <iostream>
#include <string>

using namespace std;

// 宁以pass-by-reference-to-const替换pass-by-value

// 缺省情况下C++以by value方式（继承自C）传递对象至函数。
// 除非另外指定，否则函数都是以实际实参的副本为初值。
// 而调用端获得的也是函数返回值的一个副本，这些副本都是由对象的copy构造函数生成的。
// 这使得pass-by-value成为昂贵的操作。
class Person {
 public:
  Person() { cout << "Person" << endl; }
  Person(const Person& rhs) { cout << "Person copy" << endl; }
  virtual ~Person() { cout << "~Person" << endl; }

 private:
  string name;
  string address;
};
class Student : public Person {
 public:
  Student() { cout << "Student" << endl; }
  Student(const Student& rhs) { cout << "Student copy" << endl; }
  ~Student() { cout << "~Student" << endl; }

 private:
  string schoolName;
  string schoolAddress;
};
void validateStudend1(Student s) {}
void validateStudend2(const Student& s) {}

class Window {
 public:
  string name() const { return "Window"; }
  virtual void display() const { cout << "Window display" << endl; }
};
class WindowWithScroollBars : public Window {
 public:
  string name() const { return "WindowWithScroollBars"; }
  virtual void display() const {
    cout << "WindowWithScroollBars display" << endl;
  }
};

// 以by reference方式也可以避免对象切割问题：
//  当一个派生类对象以by value方式传递并被视为一个基类对象，基础类copy构造函数会被调用，
//  派生类属性被切割，仅仅留下基类对象。
void printNameAndDisplay1(Window w) {
  // 不论传递过来的对象原本是什么类型，参数w就是一个Window对象。
  // 因此，函数内调用的是基类版本的函数。
  std::cout << w.name() << endl;
  w.display();
}
// 解决切割问题的办法，就是以by reference-to-const的方式传递w
void printNameAndDisplay2(const Window& w) {
  // 传进来的窗口是什么类型，w就表现出那种类型。
  std::cout << w.name() << endl;
  w.display();
}

// 引用往往通过指针实现，因此pass by reference通常意味着真正传递的是指针。
// 对于内置类型，pass by vlaue往往比pass by reference的效率高些。如果可以选择，就用pass by vlaue。
// 上面的忠告也适用于STL的迭代器和函数对象，习惯上它们都被设计为pass by vlaue。
// 内置类型都很小，但不是小的类型就可以使用pass by vlaue。
// 即使小型对象拥有并不昂贵的copy构造函数，还是可能有效率上的争议。
// 编译器对待内置类型和用户自定义类型的态度截然不同，哪怕两者拥有相同的底层类型。

// 请记住：
// 尽量以pass-by-reference-to-const替换pass-by-value。前者通常比较高效，并可避免切割问题。
// 以上规则并不适用于内置类型，以及STL的迭代器和函数对象。对它们而言，pass-by-value往往比较适当。

int main() {
  Student p;
  cout << "call validateStudend1 E:" << endl;
  validateStudend1(p);
  cout << "call validateStudend1 X:" << endl;

  // validateStudend调用时，发生了什么事？
  // 无疑Student的copy构造函数会被调用，以p来初始化s。当validateStudend返回时，s会被销毁。
  // 因此，对此函数而言，参数的传递成本是“一次Student copy构造函数调用，加上一次Student析构函数调用”。
  // Student对象内有两个string对象，一次构造一个Student也就构造了两个string对象。
  // Student对象继承自Person对象，所以每次构造Student对象也必须构造出一个Person对象。
  // 一个Person对象又有两个string对象，因此一次Person构造动作又伴随两个string构造。
  // 最终结果是，以by value方式传递一个Student对象会导致调用一次Student copy、
  // 一次Person copy、四次string copy构造函数。
  // 当函数内的Student复件被销毁，每一个构造动作都对应一个析构函数的调用动作。
  // 因此，以by value方式传递一个student对象，总体成本是“六次构造函数和六次析构函数”！
  // 这是正确且值得拥有的行为，毕竟希望所有的对象都能够被正确地构造和析构。

  cout << "call validateStudend2 E:" << endl;
  validateStudend2(p);
  cout << "call validateStudend2 X:" << endl;
  // 这种传递方式的效率高很多：没有任何构造函数或析构函数被调用，因为没有任何新对象被创建。
  // const很重要，以by value方式，调用者不会修改实参，只会修改副本。
  // 以by reference方式，调用者需要考虑会不会改变实参数。

  // Person
  // Student
  // call validateStudend1 E:
  // Person
  // Student copy
  // ~Student
  // ~Person
  // call validateStudend1 X:
  // call validateStudend2 E:
  // call validateStudend2 X:
  // ~Student
  // ~Person

  WindowWithScroollBars wwsb;
  printNameAndDisplay1(wwsb);
  printNameAndDisplay2(wwsb);
  // Window
  // Window display
  // Window
  // WindowWithScroollBars display

  return 0;
}