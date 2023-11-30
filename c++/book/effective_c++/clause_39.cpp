#include <iostream>

// 明智而审慎地使用private继承

namespace n1 {
// private继承意味is-implemented-in-terms-of，根据某物实现出。
// 如果class之间的继承关系是private，则和public不同，
// 编译器不会自动将一个derived class对象转换为一个base class对象。
// 由private base class继承而来的所有成员，
// 在derived class中都会变成private属性，
// 纵使它们在base class中原本是protected或public属性。
// private继承意味着只有实现部分被继承，接口部分应略去。
// private继承在软件设计层面上没有意义，其意义只及于软件实现层面。

// private继承和复合都意味着is-implemented-in-terms-of。
// 如何选择？
// 答案很简单，尽可能使用复合，必要时才使用private继承。
// 何时才是必要的？
// 1.主要是当protected成员和/或virtual函数牵扯进来的时候；
// 2.其实还会一种激进情况，那是当空间方面的厉害关系足以踢翻private继承的支柱时。

class Person {};
// 显然private继承并不意味着is-a关系：
class Student : private Person {};
void eat(const Person& p) {}
void study(const Student& s) {}

void func() {
  Person p;
  Student s;
  eat(p);
  // eat(s);  // 错误，难道学生不是人？
}
}  // namespace n1

namespace n2 {
// 要定时统计Widget class中成员函数被调用的次数：
class Timer {
 public:
  explicit Timer(int tickFrequency);
  virtual void onTick() const;
};

namespace test1 {
// 为了让Widget重新定义Timer内的virtual函数，Widget必须继承自Timer。
// 但public继承并不适当，因为Widget并不是个Timer。
// Widget客户不应该对着一个Widget调用onTick吧，因为那并不是Widget的接口。
// 必须以private形式继承Timer：
class Widget : private Timer {
 private:
  virtual void onTick() const;
};
// Timer的public onTick在Widget内变成private，而重新定义时仍是private。
}  // namespace test1

namespace test2 {
// 上面是个好设计，但不值几文钱，因为private继承并非决定必要。
// 如果决定以复合取而代之，是可以的，这个设计只比private继承要复杂一点。
class Widget {
 private:
  class WidgetTimer : public Timer {
   public:
    virtual void onTick() const;
  };
  WidgetTimer timer;
};
// 相比于比private继承的优点：
// 1.要使Widget拥有derived class，同时还想阻止derived class重新定义onTick。
//   如果继承自Timer，这个想法不可能实现，private继承也不可能。
//   但如果WidgetTimer是Widget内部的一个private成员并继承Timer，
//   Widget的derived将无法取用WidgetTimer，因此无法继承或重新定义它的virtual函数。
//   Java或C#中有阻止derived重新定义virtual函数的能力，现在可以在C++中模拟它了。
// 2.或许会想要将Widget的编译依赖性降至最低。
//   但如果Widget继承Timer，当Widget被编译时Timer的定义必须可见，
//   所以定义Widget的那个文件恐怕必须#include "timer.h"。
//   但如果Widget移除Widget之外而Widget内含指针指向一个WidgetTimer，
//   Widget可以只带一个简单的WidgetTimer声明就可以，不需要#include "timer.h"。
//   对于大型系统而言，如此解耦可能是重要的措施。
}  // namespace test2
}  // namespace n2

namespace n3 {
// private继承主要用于：
// 当一个derived class子类访问base class的protected成分，
// 或为了重新定义一个或多个virtual函数。
// 有一种情况涉及空间最优化，可能会促使选择private继承而不是继承加复合。
// 只适用于处理的class不带任何数据时，这样的class没有non-static成员变量，
// 没有virtual函数（会有vptr），也没有virtual base class。
// 这种所谓的empty class对象不使用任何空间，因为没有任何数据需要存储。
// 然而由于技术上的理由，C++规定凡是独立（非附属）对象都必须有非零大小，
// 所以如果这样做：
class Empty {};  // 没有数据，所以其对象不使用任何内存
namespace test1 {
class HoldsAnInt {  // 应该只需要一个int空间
 private:
  int x;
  Empty e;  // 应该不需要内存
};

void func() {
  // sizeof(HoldsAnInt)>sizeof(int)，一个Empty成员变量竟然要求内存。
  // 大多数编译器中sizeof(Empty)为１，因为面对大小为零的独立（非附属）对象，
  // C++官方勒令默默安插一个char到空对象内：
  std::cout << "Empty: " << sizeof(Empty) << std::endl;  // 1
  // 由于对齐，使得sizeof(HoldsAnInt)为８：
  std::cout << "HoldsAnInt: " << sizeof(HoldsAnInt) << std::endl;  // 8
  std::cout << "int: " << sizeof(int) << std::endl;                // 4
}
}  // namespace test1

namespace test2 {
// 很小心地说独立（非附属）对象的大小一定不为零。
// 也就是说，这个约束不适用于derived class对象内的base class成分，
// 因为它们并非独立（非附属）。
// 如果你继承Empty，而不是内含一个那种类型的对象：
class HoldsAnInt : private Empty {
 private:
  int x;
};
void func() {
  // 这是所谓的EBO（empty base optimization空白类最优化）。
  // EBO一般只在单一继承（而非多重继承）下才可行。
  // 现实中的empty class并不是真的empty，虽然从未拥有non-static成员变量，
  // 却往往内含typedef，enum，static成员变量，或non-virtual函数。
  // STL就有许多用途的empty class，其中含有用的成员（通常是typedef），
  // 包括base class unary_function和binary_function，
  // 这些是用户自定义的函数对象通常会继承的class。
  // 感谢EBO，使这样的继承很少增加derived class的大小。
  // 大多数class并非empty，所以EBO很少成为private继承的正当理由。

  std::cout << "HoldsAnInt: " << sizeof(HoldsAnInt) << std::endl;  // 4
  // sizeof(HoldsAnInt) > sizeof(int)
}

// 当面对并不存在is-a关系的两个class，其中一个需要访问另一个的protected成员，
// 或需要重新定义其一个或多个virtual函数，private继承极有可能成为正统的设计策略。
}  // namespace test2
}  // namespace n3

// 请记住：
// 1.private继承意味着is-implemented-in-terms-of，
//   根据某物实现出，它通常比复合的成本低。
// 2.但当derived class需要访问protected base class的成员，
//   或需要重新定义继承而来的virtual函数时，这么设计是合理的。
// 3.和复合不同，private继承可以造成empty base最优化。
//   这对致力于对象尺寸最小化的程序库开发者而言，可能很重要。

int main(int argc, char* argv[]) {
  if (argc < 2) {
    std::cout << argv[0] << " i [0 - 2]" << std::endl;
    return 0;
  }
  int type = argv[1][0] - '0';
  switch (type) {
    case 0:
      n1::func();
      break;
    case 1:
      n3::test1::func();
      break;
    case 2:
      n3::test2::func();
      break;
    default:
      std::cout << "invalid type" << std::endl;
      break;
  }

  return 0;
}