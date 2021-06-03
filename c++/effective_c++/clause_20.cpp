#include <iostream>
#include <string>

using namespace std;

// 宁以pass-by-reference-to-const替换pass-by-value

// 缺省情况下C++以by value方式（继承自C）传递对象至函数。除非另外指定，否则函数都是以实际实参的副本为初值。
// 而调用端获得的也是函数返回值的一个副本，这些副本都是由对象的copy构造函数生成的。这使得pass-by-value成为昂贵的操作。
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
void validateStudend(Student s) {}

// 请记住：
// 尽量以pass-by-reference-to-const替换pass-by-value。前者通常比较高效，并可避免切割问题。
// 以上规则并不适用于内置类型，以及STL的迭代器和函数对象。对它们而言，pass-by-value往往比较适当。

int main() {
  Student p;
  cout << "call validateStudend E:" << endl;
  validateStudend(p);
  cout << "call validateStudend X:" << endl;
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
  // 无疑Student的copy构造函数会被调用，以p来初始化s。当validateStudend返回时，s会被销毁。
  // 因此，对此函数而言，参数的传递成本是“一次Student copy构造函数调用，加上一次Student析构函数调用”。
  // Student对象内有两个string对象，一次构造一个Student也就构造了两个string对象。
  // Student对象继承自Person对象，所以每次构造Student对象也必须构造出一个Person对象。
  // 一个Person对象又有两个string对象，因此一次Person构造动作又伴随两个string构造。
  // 最终结果是，以by value方式传递一个Student对象会导致调用一次Student copy、一次Person copy、四次string copy构造函数。
  // 当函数内的Student复件被销毁，每一个构造动作都对应一个析构函数的调用动作。
  // 因此，以by value方式传递一个student对象，总体成本是“六次构造函数和六次析构函数”！
  // 这是正确且值得拥有的行为，毕竟希望所有的对象都能够被正确地构造和析构。

  return 0;
}