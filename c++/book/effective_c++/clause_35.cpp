#include <functional>
#include <iostream>

// 考虑virtual函数以外的其他选择

namespace n1 {
// 再明白不过的设计，但从某个角度说，却反而成了弱点：
class GameCharacter {
 public:
  // 返回人物的健康指数，派生类可重新定义，不是纯虚函数，意味着有缺省算法：
  virtual int healValue() const;
};
}  // namespace n1

// 考虑其他代替方案：

namespace n2 {
// 1.non-virtual Interface手法实现template Method模式：
//   有一个有趣的思想流派，主张virtual函数应该几乎总是private。
class GameCharacter {
 public:
  // 通过public non-virtual成员函数间接调用private virtual函数，
  // 称为non-virtual interface(NVI)手法。
  // 它是所谓Template Method设计模式的一种独特表现形式，与C++ template并无关系。
  // non-virtual函数healValue称为virtual函数的外覆器。
  int healValue() const {  // 自动成为了inline
    // 做一些事前的工作
    int retVal = doHealthValue();  // 做真正的工作
    // 做一些事后的工作
    return retVal;
  }

  // NVI手法的一个优点在于，外覆器确保在调用virtual函数之前设定好适当的场景，
  // 并在调用结束后清理场景，事前工作包括锁定互斥锁、制造日志记录项、
  // 验证class约束条件、验证函数先决条件等。
  // 事后工作包括解除互斥锁、验证函数的事后条件、再次验证class约束条件等等。
  // 如果让客户直接调用virtual函数，就没有任何办法可以做这些事。

  // NVI手法涉及在derived class内重新定义private virtual函数。
  // 重新定义了若干个derived class并不能调用的函数，这里并不矛盾。
  // derived class重新定义virtual函数，从而赋予它们如何实现机能的控制能力，
  // 但base class保留函数何时被调用的权利。

  // 在NVI手法下，其实没必要让virtual函数一定是private。
  // 某些class继承体系要求derived class，
  // 在virtual函数的实现内必须调用其base class的对应兄弟，
  // 为了调用合法，virtual必须为protected。
  // 有时候virtual函数甚至一定得是public，
  // 比如有多态性质的base class的析构函数，这样就不能实施NVI手法了。

 private:
  // derived class可以重新定义它：
  virtual int doHealthValue() const {
    // 缺省算法，计算健康指数
  }
};
}  // namespace n2

namespace n3 {
// 2.由Function Pointer实现Strategy模式：
//   NVI手法对public virtual函数而言是一个有趣的代替方案，但从某种设计角度来看，
//   它只比窗花更强一些而已，另一个思路是人物健康指数的计算与人物类型无关，
//   计算时完全不需要人物这个成分。
class GameCharacter;
int defaultHealthCalc(const GameCharacter& gc) {
  std::cout << "defaultHealthCalc" << std::endl;
}
// 这个做法是常见的Strategy设计模式的简单应用：
class GameCharacter {
 public:
  typedef int (*HealthCalcFunc)(const GameCharacter&);
  explicit GameCharacter(HealthCalcFunc hcf = defaultHealthCalc)
      : healthFunc(hcf) {}
  int healthValue() const { return healthFunc(*this); }
  int setHealthCalculator(HealthCalcFunc hcf) { healthFunc = hcf; }

 private:
  HealthCalcFunc healthFunc;
};
// 与植于继承体系内的virtual函数比较，Strategy模式提供了某些有趣的弹性：
// a.同一人物类型之不同实体可以有不同的健康计算函数。
class EvilBadGuy : public GameCharacter {
 public:
  explicit EvilBadGuy(HealthCalcFunc hcf = defaultHealthCalc)
      : GameCharacter(hcf) {}
};
int loseHealthQuickly(const GameCharacter&) {
  std::cout << "loseHealthQuickly" << std::endl;
};
int loseHealthSlowly(const GameCharacter&) {
  std::cout << "loseHealthSlowly" << std::endl;
};
// b.某已知人物之健康指数计算函数可在运行期间变更，
//   GameCharacter可提供一个成员函数，用来替换当前的健康指数计算函数。
int loseHealthQuickly1(const GameCharacter&) {
  std::cout << "loseHealthQuickly1" << std::endl;
};
// 换句话说，健康指数计算函数不再是GameCharacter继承体系内的成员函数，
// 这意味着这些函数并未访问对象的内部成分。
// 如果人物的健康可纯粹根据该人物public接口得来的信息加以计算，
// 这就没问题，但如果需要non-public信息进行计算，就有问题了。
// 一般而言，只有弱化class的封装能够解决：
// 需要以non-member函数访问class的non-public成员。
// 可声明哪个non-member函数为friend，或是为其实现的某一部分public访问函数。
// 运用函数指针替换virtual函数，其优点：
// 1.每个对象可各自拥有自己的健康计算函数;
// 2.可在运行期改变计算函数。
// 是否可以弥补缺点（必须降低GameCharacter的封装性），
// 是必须根据每个设计情况的不同而抉择的。

void func() {
  EvilBadGuy ebg1(loseHealthQuickly);
  ebg1.healthValue();  // loseHealthQuickly
  EvilBadGuy ebg2(loseHealthSlowly);
  ebg2.healthValue();  // loseHealthSlowly
  EvilBadGuy ebg3;
  ebg3.healthValue();  // defaultHealthCalc

  ebg1.setHealthCalculator(loseHealthQuickly1);
  ebg1.healthValue();  // loseHealthQuickly1
}
}  // namespace n3

namespace n4 {
// 3.由function完成Strategy模式
class GameCharacter;
int defaultHealthCalc(const GameCharacter& gc);

// 这个做法是常见的Strategy设计模式的简单应用：
class GameCharacter {
 public:
  typedef std::function<int(const GameCharacter&)> HealthCalcFunc;
  explicit GameCharacter(HealthCalcFunc hcf = defaultHealthCalc)
      : healthFunc(hcf) {}
  int healthValue() const { return healthFunc(*this); }

  // 不使用函数指针，改用一个类型为function对象，可以解决指针的一些约束：
  // 可以持有任何可调用物（函数指针、函数对象或成员函数指针），只要其签名一致就可以。

 private:
  HealthCalcFunc healthFunc;
};
short calcHealth(const GameCharacter&) {
  std::cout << "calcHealth" << std::endl;
}
struct HealthCalculator {
  int operator()(const GameCharacter&) const {
    std::cout << "HealthCalculator::operator()" << std::endl;
  }
};
class GameLevel {
 public:
  float health(const GameCharacter&) const {
    std::cout << "GameLevel::health" << std::endl;
  }
};
class EvilBadGuy : public GameCharacter {
 public:
  explicit EvilBadGuy(HealthCalcFunc hcf = defaultHealthCalc)
      : GameCharacter(hcf) {}
};
class EyeCandyCharacter : public GameCharacter {
 public:
  explicit EyeCandyCharacter(HealthCalcFunc hcf = defaultHealthCalc)
      : GameCharacter(hcf) {}
};

void func() {
  EvilBadGuy ebg1(calcHealth);
  ebg1.healthValue();  // calcHealth

  HealthCalculator healthCalculator;
  EyeCandyCharacter ecc(healthCalculator);
  ecc.healthValue();  // HealthCalculator::operator()

  GameLevel currentLevel;
  EvilBadGuy ebg2(
      std::bind(&GameLevel::health, currentLevel, std::placeholders::_1));
  ebg2.healthValue();  // GameLevel::health
}
}  // namespace n4

namespace n5 {
// 4.古典的Strategy模式，传统（典型）的Strategy做法，
//   会将健康计算函数做成一个分离的继承体系中的virtual成员函数。
class GameCharacter;
class HealthCalcFunc {
 public:
  virtual int calc(const GameCharacter& gc) const {
    std::cout << "calc" << std::endl;
  }
};
HealthCalcFunc defaultHealthCalc;
class GameCharacter {
 public:
  explicit GameCharacter(HealthCalcFunc* phcf = &defaultHealthCalc)
      : pHealthCalc(phcf) {}
  int healthValue() const { return pHealthCalc->calc(*this); }

 private:
  HealthCalcFunc* pHealthCalc;
};
// 这个解法的吸引力在于，熟悉标准Strategy模式的人很容易辨认它，
// 而且提供将一个既有的健康算法纳入使用的可能性：
// 只要为HealthCalcFunc继承体系添加一个derived class即可。

void func() {
  HealthCalcFunc f;
  GameCharacter g(&f);
  g.healthValue();  // calc
}
}  // namespace n5

// 本条款的忠告是，当为解决问题而寻找设计方案时，不妨考虑virtual函数的替代方案：
// 1.使用non-virtual interface(NVI)手法，那是Template Method设计模式的特殊形式，
//   它以public非虚成员函数，包裹较低访问性（private或protected）的virtual函数；
// 2.将virtual函数替换为函数指针成员变量，这是Strategy设计模式的一种分解表现形式；
// 3.以function成员变量替换virtual函数，因而允许使用任何可调用物搭配一个兼容的签名式，
//   这也是Strategy设计模式的某种形式；
// 4.将继承体系内的virtual函数替换为另一个继承体系内的virtual函数，
//   这是Strategy设计模式的传统实现手法。
// 以上并未彻底而详尽地列出virtual函数的所有替换方案，各有优缺点，要全方面考虑。

// 请记住：
// 1.virtual函数的替代方案包括NVI手法及Strategy设计模式的多种形式；
// 2.NVI手法自身是一个特殊形式的Template Method设计模式；
// 3.将机能从成员函数移到class外部函数，带来的一个缺点是，
//   非成员函数无法访问class的non-public成员；
// 4.function对象的行为就像一般函数指针，
//   这样的对象可接纳与给定之目标签名式兼容的所有可调用物。

int main(int argc, char* argv[]) {
  if (argc < 2) {
    std::cout << argv[0] << " i [0 - 2]" << std::endl;
    return 0;
  }
  int type = atoi(argv[1]);
  switch (type) {
    case 0:
      n3::func();
      break;
    case 1:
      n4::func();
      break;
    case 2:
      n5::func();
      break;
    default:
      std::cout << "invalid type" << std::endl;
      break;
  }
}