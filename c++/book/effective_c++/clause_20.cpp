#include <iostream>
#include <string>

// 宁以pass-by-reference-to-const替换pass-by-value

namespace n1 {
// 缺省情况下C++以by value方式（继承自C）传递对象至函数。
// 除非另外指定，否则函数都是以实际实参的副本为初值。
// 而调用端获得的也是函数返回值的一个副本，这些都是由拷贝构造函数生成的。
// 这使得pass-by-value成为昂贵的操作。
class Person {
 public:
  Person() { std::cout << "Person" << std::endl; }
  Person(const Person& rhs) { std::cout << "Person copy" << std::endl; }
  virtual ~Person() { std::cout << "~Person" << std::endl; }

 private:
  std::string name;
  std::string address;
};
class Student : public Person {
 public:
  Student() { std::cout << "Student" << std::endl; }
  Student(const Student& rhs) { std::cout << "Student copy" << std::endl; }
  ~Student() { std::cout << "~Student" << std::endl; }

 private:
  std::string schoolName;
  std::string schoolAddress;
};

namespace test1 {
void validateStudent(Student s) {}
void func() {
  Student p;
  std::cout << "call validateStudend E:" << std::endl;
  validateStudent(p);
  std::cout << "call validateStudend X:" << std::endl;

  // Person
  // Student
  // call validateStudend E:
  // Person
  // Student copy
  // ~Student
  // ~Person
  // call validateStudend X:
  // ~Student
  // ~Person

  // validateStudend调用时，发生了什么事？
  // 无疑Student的copy构造函数会被调用，以p来初始化s。
  // 当validateStudend返回时，s会被销毁。
  // 因此，对此函数而言，参数的传递成本是：
  // 1.一次Student copy构造函数调用；
  // 2.一次Student析构函数调用；
  // 3.Student对象内有两个string对象，
  //   一次构造一个Student也就构造了两个string对象；
  // 4.Student对象继承自Person对象，
  //   所以每次构造Student对象也必须构造出一个Person对象；
  // 5.一个Person对象又有两个string对象，
  //   因此一次Person构造动作又伴随两个string构造。
  // 最终结果是，以by value方式传递一个Student对象会导致调用：
  // 一次Student copy、一次Person copy、四次string copy构造函数。
  // 当函数内的Student复件被销毁，每一个构造动作都对应一个析构函数的调用动作。
  // 因此，以by value方式传递一个student对象，总成本是六次构造和六次析构。
  // 这是正确且值得拥有的行为，毕竟希望所有的对象都能够被正确地构造和析构。
}
}  // namespace test1
namespace test2 {
void validateStudent(const Student& s) {}
void func() {
  Student s;
  std::cout << "call validateStudend E:" << std::endl;
  validateStudent(s);
  std::cout << "call validateStudend X:" << std::endl;

  // Person
  // Student
  // call validateStudend E:
  // call validateStudend X:
  // ~Student
  // ~Person

  // 以常量引用的方式传递参数要高效的多：
  // 没有任何构造函数或析构函数被调用，因为没有任何新对象被创建。
  // const很重要，以by value方式，调用者不会修改实参，只会修改副本。
  // 以by reference方式，调用者需要考虑会不会改变实参数。
}
}  // namespace test2
}  // namespace n1

namespace n2 {
class Window {
 public:
  std::string name() const { return "Window"; }
  virtual void display() const { std::cout << "Window display" << std::endl; }
};
class WindowWithScroollBars : public Window {
 public:
  std::string name() const { return "WindowWithScroollBars"; }
  virtual void display() const {
    std::cout << "WindowWithScroollBars display" << std::endl;
  }
};

// 以by reference方式也可以避免对象切割问题：
// 当一个派生类对象以by value方式传递并被视为一个基类对象，
// 基类的copy构造函数会被调用，派生类属性被切割，仅仅留下基类对象。
void printNameAndDisplay1(Window w) {
  // 不论传递过来的对象原本是什么类型，参数w就是一个Window对象，
  // 因此，函数内调用的是基类版本的函数：
  std::cout << w.name() << std::endl;
  w.display();
}
// 解决切割问题的办法，就是以by reference-to-const的方式传递w：
void printNameAndDisplay2(const Window& w) {
  // 传进来的窗口是什么类型，w就表现出那种类型：
  std::cout << w.name() << std::endl;
  w.display();
}

void func() {
  WindowWithScroollBars wwsb;
  printNameAndDisplay1(wwsb);
  // Window
  // Window display

  printNameAndDisplay2(wwsb);
  // Window
  // WindowWithScroollBars display
}
}  // namespace n2

// 引用往往通过指针实现，因此pass by reference通常意味着真正传递的是指针。
// 对于内置类型，pass by value往往比pass by reference的效率高些，
// 如果可以选择，就用pass by value。
// 对于STL的迭代器和函数对象也一样，习惯上它们都被设计为pass by value。
// 内置类型都很小，但不是小的类型就可以使用pass by value。
// 即使小型对象拥有并不昂贵的copy构造函数，还是可能有效率上的争议。
// 编译器对待内置类型和自定义类型的态度截然不同，哪怕两者拥有相同的底层类型。

// 请记住：
// 1.尽量以pass-by-reference-to-const替换pass-by-value，
//   前者通常比较高效，并可避免切割问题；
// 2.以上规则并不适用于内置类型，以及STL的迭代器和函数对象，
//   对它们而言，pass-by-value往往比较适当。

int main(int argc, char* argv[]) {
  if (argc < 2) {
    std::cout << argv[0] << " i [0 - 2]" << std::endl;
    return 0;
  }
  int type = argv[1][0] - '0';
  switch (type) {
    case 0:
      n1::test1::func();
      break;
    case 1:
      n1::test2::func();
      break;
    case 2:
      n2::func();
      break;
    default:
      std::cout << "invalid type" << std::endl;
      break;
  }

  return 0;
}