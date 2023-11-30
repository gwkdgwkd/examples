#include <iostream>

// 运用成员函数模板接受所有兼容类型

// 智能指针是行为像指针的对象，并提供指针没有的能力，STL容器的迭代器几乎总是智能指针。

class Top {};
class Middle : public Top {};
class Bottom : public Middle {};

namespace n1 {
void func() {
  // 真实指针做到很好的一件事是，支持隐式转换：
  // 1.Derived class指针可以隐式转换为base class指针；
  Top *pt1 = new Middle;
  Top *pt2 = new Bottom;

  // 2.指向non-const对象的指针可以转换为指向const对象。
  const Top *pct = pt1;
}
}  // namespace n1

namespace n2 {
// 但如果想在用户自定义的智能指针中模拟上述转换，稍稍有点麻烦：
template <typename T>
class SmartPtr {
 public:
  explicit SmartPtr(T *realPtr) {}
};
void func() {
  // 同一个template的不同具现体之间并不存在什么与生俱来的固有关系。
  // 带有base-derived关系的两个类型分别具现化某个template，
  // 产生出来的两个具现体并不带有base-derived关系。
  // 所以SmartPtr<Top>和SmartPtr<Middle>为完全不同的class，
  // 它们之间的关系并不比vector<float>和Widget更密切。

  // SmartPtr<Top> pst1 = SmartPtr<Middle>(new Middle);
  // SmartPtr<Top> pst2 = SmartPtr<Bottom>(new Bottom);
  // SmartPtr<const Top> psct2 = pst1;
}
}  // namespace n2

namespace n3 {
// 为了获得class SmartPtr之间的转换能力，必须明确地编写出来。
// 应该关注如何编写智能指针的构造函数，使其行为能够满足转型的需要。
// 但如果后面扩充来类，BelowBottom，所以永远无法写出需要的所有构造函数。
// 理论上需要的构造函数数量没有尽头，因为template可以被无限量具现化。
// 因此，需要的不是为class SmartPtr写一个构造函数，
// 而是为它写一个构造模板member function template，简称member template：
template <typename T>
class SmartPtr {
 public:
  explicit SmartPtr(T *realPtr) {}

  // 因为原始指针之间的转换是隐式的，所以没有explicit也合理：
  template <typename U>
  SmartPtr(const SmartPtr<U> &other) {}  // 泛化copy构造函数
};
void func() {
  SmartPtr<Top> pst1 = SmartPtr<Middle>(new Middle);
  SmartPtr<Top> pst2 = SmartPtr<Bottom>(new Bottom);
  SmartPtr<const Top> psct = pst1;

  // SmartPtr泛化的copy构造函数提供的比需要的更多：
  // 1.希望根据一个SmartPtr<Bottom>创建一个SmartPtr<Top>，
  //   而不希望根据SmartPtr<Top>创建SmartPtr<Bottom>，对public继承是矛盾的。
  SmartPtr<Bottom> pb = SmartPtr<Top>(new Top);
  // 2.也不希望根据SmartPtr<double>创建一个SmartPtr<int>，
  //   因为现实中没有将int*转换为double*的隐式转换行为。
  SmartPtr<int> pi = SmartPtr<double>(new double);
  // 所以必成从某方面对member template所创建的成员函数进行筛选。
}
}  // namespace n3

namespace n4 {
template <typename T>
class SmartPtr {
 public:
  // 使用成员初始值列表来初始化SmartPtr<T>的T*成员变量，并以U*指针作为初值。
  // 这个转换只有当：存在某个隐式转换可将U*转换为T*时才能通过编译，正是想要的。
  template <typename U>
  SmartPtr(const SmartPtr<U> &other) : heldPtr(other.get()) {}
  explicit SmartPtr(T *realPtr) {}
  T *get() const { return heldPtr; }

 private:
  T *heldPtr;
};
void func() {
  SmartPtr<Top> pst1 = SmartPtr<Middle>(new Middle);
  SmartPtr<Top> pst2 = SmartPtr<Bottom>(new Bottom);
  SmartPtr<const Top> psct = pst1;

  // 报错：
  // SmartPtr<Bottom> pb = SmartPtr<Top>(new Top);
  // SmartPtr<int> pi = SmartPtr<double>(new double);
}
}  // namespace n4

// 成员函数模板member function template的效用不限于构造函数，
// 常扮演的另一个角色是支持赋值操作。
// shared_prt支持所有来自兼容的内置指针、shared_ptr、
// auto_ptr和weak_ptr的构造行为，以及上述各物的赋值操作。

// member function template是个奇妙的东西，但它们并不改变语言基本规则：
// 如果需要一个copy构造函数，却没有声明，编译器会暗自生成一个。
// 声明泛化copy构造函数，并不会阻止编译器生成non-template的copy构造函数。
// 要控制copy的方方面面，必须同时声明泛化copy构造函数和正常的copy构造函数。
// 相同的规则也适用于赋值操作。

// 请记住：
// 1.使用member function template生成可接受所有兼容类型的函数；
// 2.如果声明member template用于泛化copy构造或泛化assignment操作，
//   还是需要声明正常的copy构造函数和copy assignmet操作符。

int main(int argc, char *argv[]) {
  if (argc < 2) {
    std::cout << argv[0] << " i [0 - 3]" << std::endl;
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
    default:
      std::cout << "invalid type" << std::endl;
      break;
  }

  return 0;
}