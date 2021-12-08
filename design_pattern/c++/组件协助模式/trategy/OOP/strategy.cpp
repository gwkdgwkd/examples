
class TaxStrategy {
 public:
  virtual double Calculate(const Context& context) = 0;
  virtual ~TaxStrategy() {}
};

class CNTax : public TaxStrategy {
 public:
  virtual double Calculate(const Context& context) {
    // CN
  }
};

class USTax : public TaxStrategy {
 public:
  virtual double Calculate(const Context& context) {
    // US
  }
};

class DETax : public TaxStrategy {
 public:
  virtual double Calculate(const Context& context) {
    // DE
  }
};

// 更改
class FRTax : public TaxStrategy {
 public:
  virtual double Calculate(const Context& context) {
    // FR
  }
};

// 使用了扩展的方式支持法国，符合开放封闭原则
class SalesOrder {
 private:
  TaxStrategy* strategy;  // 不可以用对象。可以用引用，但有其他问题，一般不用。

 public:
  SalesOrder(StrategyFactory* strategyFactory) {
    // 添加了法国后，SalesOrder不需要变化，但需要工厂类可以支持法国
    this->strategy = strategyFactory->NewStrategy();  // 工厂模式
  }
  ~SalesOrder() { delete this->strategy; }

 public
  double CalculateTax() {
    Context context();
    double val = strategy->Calculate(context);  // 多态调用
  }
};
