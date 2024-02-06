#include <iostream>

// Strategy 策略模式

// 动机（Motivation）：
// 1.在软件构建过程中，某些对象使用的算法可能多种多样，经常改变，
//   如果将这些算法都编码到对象中，将会使对象变得异常复杂，
//   而且有时候支持不使用的算法也是一个性能负担；
// 2.如何在运行时根据需要透明地更改对象的算法？将算法与对象本身解耦，从而避免上述问题？

// 定义：
// 定义一系列算法，把它们一个个封装起来，并且使它们可互相替换（变化）。
// 该模式使得算法可独立于使用它的客户程序(稳定)而变化（扩展，子类化）。

// 要点总结：
// 1.Strategy及其子类为组件提供了一系列可重用的算法，
//   从而可以使得类型在运行时方便地根据需要在各个算法之间进行切换；
// 2.Strategy模式提供了用条件判断语句以外的另一种选择，消除条件判断语句，
//   就是在解耦合，含有许多条件判断语句的代码通常都需要Strategy模式；
// 3.如果Strategy对象没有实例变量，那么各个上下文可以共享同一个Strategy对象，
//   从而节省对象开销。

namespace n1 {
enum TaxBase {
  CN_Tax,
  US_Tax,
  DE_Tax,
  FR_Tax  // 更改
};

class SalesOrder {
  TaxBase tax;

 public:
  double CalculateTax() {
    // 违反了开放封闭原则，应该用扩展的方式：
    if (tax == CN_Tax) {  // 或者switch
      std::cout << "CN" << std::endl;
    } else if (tax == US_Tax) {
      std::cout << "US" << std::endl;
    } else if (tax == DE_Tax) {
      std::cout << "DE" << std::endl;
    } else if (tax == FR_Tax) {  // 变化
      std::cout << "FR" << std::endl;
    }
  }
};
}  // namespace n1

namespace n2 {
struct Context {};
class TaxStrategy {
 public:
  virtual double Calculate(const Context& context) = 0;
  virtual ~TaxStrategy() {}
};

class CNTax : public TaxStrategy {
 public:
  virtual double Calculate(const Context& context) {
    std::cout << "CN" << std::endl;
  }
};

class USTax : public TaxStrategy {
 public:
  virtual double Calculate(const Context& context) {
    std::cout << "US" << std::endl;
  }
};

class DETax : public TaxStrategy {
 public:
  virtual double Calculate(const Context& context) {
    std::cout << "DE" << std::endl;
  }
};

class StrategyFactory {
 public:
  virtual ~StrategyFactory() = default;
  virtual TaxStrategy* NewStrategy() = 0;
};

class CNStrategyFactory : public StrategyFactory {
 public:
  TaxStrategy* NewStrategy() { return new CNTax; }
};
class USStrategyFactory : public StrategyFactory {
 public:
  TaxStrategy* NewStrategy() { return new USTax; }
};
class DEStrategyFactory : public StrategyFactory {
 public:
  TaxStrategy* NewStrategy() { return new DETax; }
};

// 使用了扩展的方式支持法国，符合开放封闭原则：
class SalesOrder {
 public:
  SalesOrder(StrategyFactory* strategyFactory) {
    // 添加了法国后，SalesOrder不需要变化，但需要工厂类可以支持法国
    this->strategy = strategyFactory->NewStrategy();  // 工厂模式
  }
  ~SalesOrder() { delete this->strategy; }

  double CalculateTax() {
    Context context;
    double val = strategy->Calculate(context);  // 多态调用
  }

 private:
  TaxStrategy* strategy;  // 不可以用对象，可以用引用，但有其他问题，一般不用
};

void func1() {
  SalesOrder s1(new CNStrategyFactory);
  s1.CalculateTax();  // CN

  SalesOrder s2(new USStrategyFactory);
  s2.CalculateTax();  // US

  SalesOrder s3(new DEStrategyFactory);
  s3.CalculateTax();  // DE
}

// 新增法国：
class FRTax : public TaxStrategy {
 public:
  virtual double Calculate(const Context& context) {
    std::cout << "FR" << std::endl;
  }
};
class FRStrategyFactory : public StrategyFactory {
 public:
  TaxStrategy* NewStrategy() { return new FRTax; }
};

void func2() {
  SalesOrder s(new FRStrategyFactory);
  s.CalculateTax();  // FR
}

void func() {
  func1();
  func2();
}
}  // namespace n2

int main(int argc, char* argv[]) {
  if (argc < 2) {
    std::cout << argv[0] << " i [0 - 0]" << std::endl;
    return 0;
  }
  int type = argv[1][0] - '0';

  switch (type) {
    case 0:
      n2::func();
      break;
    default:
      std::cout << "invalid type" << std::endl;
      break;
  }

  return 0;
}