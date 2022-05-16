#include <iostream>

using namespace std;

// 运用成员函数模板接受所有兼容类型

// 所谓智能指针是“行为像指针”的对象，并提供指针没有的能力。
// STL容器的迭代器几乎总是智能指针。

// 真实指针做到很好的一件事是，支持隐式转换：
// 1.Derived class指针可以隐式转换为base class指针;
// 2.指向non-const对象的指针可以转换为指向const对象。
// ...
class Top {};
class Middle : public Top {};
class Bottom : public Middle {};

// 但如果想在用户自定义的智能指针中模拟上述转换，稍稍有点麻烦。
template <typename T>
class SmartPtr {
 public:
  explicit SmartPtr(T *realPtr) {}
};

// 为了获得class SmartPtr之间的转换能力，必须明确地编写出来。
// 应该关注如何编写智能指针的构造函数，使其行为能够满足我们的转型需要。
// 但如果后面扩充来类，BelowBottom，所以我们永远无法写出我们需要的所有构造函数。
// 理论上将，我们需要的构造函数数量没有尽头，因为template可以被无限量具现化。
// 因此，我们需要的不是为class SmartPtr写一个构造函数，
// 而是为它写一个构造模板（member function template，简称member template）：
template <typename T>
class SmartPtr1 {
 public:
  explicit SmartPtr1(T *realPtr) {}
  template <typename U>
  SmartPtr1(const SmartPtr1<U> &other) {}  // 泛化copy构造函数。
  // explicit故意没写。
  // 因为原始指针之间的转换是隐式的，所以让智能指针模仿这种行为也属合理。
  // 这个泛化copy构造函数提供的比我们需要的更多：
  // 1.希望根据一个SmartPtr<Bottom>创建一个SmartPtr<Top>，
  //   而不希望根据SmartPtr<Top>创建一个SmartPtr<Bottom>，对public继承是矛盾的。
  // 2.也不希望根据SmartPtr<doublie>创建一个SmartPtr<int>，
  //   因为现实中没有将int*转换为double*的隐式转换行为。
  // 所以必成从某方面对member template所创建的成员函数进行筛选。
};

template <typename T>
class SmartPtr2 {
 public:
  // 使用成员初始值列表来初始化SmartPtr<T>的类型为T*的成员变量，并以类型U*的指针作为初值。
  // 这个转换只有当“存在某个隐式转换可将一个U*转换为一个T*指针”时才能通过编译，正是我们想要的。
  // 最终受益的是SmartPtr2<T>，它的泛化copy构造函数只在其所获得的实参隶属适当类型时才能通过编译。
  template <typename U>
  SmartPtr2(const SmartPtr2<U> &other) : heldPtr(other.get()) {}
  explicit SmartPtr2(T *realPtr) {}
  T *get() const { return heldPtr; }

 private:
  T *heldPtr;
};

// member function template(成员函数模板)的效用不限于构造函数，常扮演的另一个角色是支持赋值操作。
// shared_prt支持所有“来自兼容的内置指针、shared_ptr、auto_ptr和weak_ptr”的构造行为，
// 以及上述各物的赋值操作。

// member function template是个奇妙的东西，但它们并不改变语言基本规则：
// 如果需要一个copy构造函数，却没有声明，编译器会为你暗自生成一个。
// 在class内声明泛化copy构造函数并不会阻止编译器生成它们自己的copy构造函数（non-template)。
// 所以想要控制copy构造的方方面面，必须同时声明泛化copy构造函数和正常的copy构造函数。
// 相同的规则也适用于赋值操作。

// 请记住：
// 使用member function template（成员函数模板）生成“可接受所有兼容类型”的函数。
// 如果声明member template用于“泛化copy构造”或“泛化assignment操作”，
// 还是需要声明正常的copy构造函数和copy assignmet操作符。

int main() {
  Top *pt1 = new Middle;
  Top *pt2 = new Bottom;
  const Top *pct2 = pt1;

  // 希望下面的代码通过编译：
  // SmartPtr<Top> pst1 = SmartPtr<Middle>(new Middle);
  // SmartPtr<Top> pst2 = SmartPtr<Bottom>(new Bottom);
  // SmartPtr<const Top> psct2 = pst1;
  // 同一个template的不同具现体之间并不存在什么与生俱来的固有关系。
  // 带有base-derived关系的两个类型分别具现化某个template，
  // 产生出来的两个具现体并不带有base-derived关系。
  // 所以SmartPtr<Top>和SmartPtr<Middle>为完全不同的class，
  // 它们之间的关系并不比vector<float>和Widget更密切。

  SmartPtr1<Top> pst11 = SmartPtr1<Middle>(new Middle);
  SmartPtr1<Top> pst21 = SmartPtr1<Bottom>(new Bottom);
  SmartPtr1<const Top> psct21 = pst11;

  SmartPtr2<Top> pst12 = SmartPtr2<Middle>(new Middle);
  SmartPtr2<Top> pst22 = SmartPtr2<Bottom>(new Bottom);
  SmartPtr2<const Top> psct22 = pst12;

  return 0;
}