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
    // 或者switch
    if (tax == CN_Tax) {
      // CN
    } else if (tax == US_Tax) {
      // US
    } else if (tax == DE_Tax) {
      // DE
    } else if (tax == FR_Tax) {  // 变化
      // FR
      // 违反了开放封闭原则，应该用扩展的方式
    }
  }
};
