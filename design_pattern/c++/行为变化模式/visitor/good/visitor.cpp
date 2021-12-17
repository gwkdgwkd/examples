#include <iostream>
using namespace std;

class ElementA;
class ElementB;
class Visitor {
 public:
  virtual void visitElementA(ElementA& element) = 0;
  virtual void visitElementB(ElementB& element) = 0;

  virtual ~Visitor() {}
};

class Element {
 public:
  virtual void accept(Visitor& visitor) = 0;  // 第一次多态辨析
  virtual ~Element() {}
};

class ElementA : public Element {
 public:
  void accept(Visitor& visitor) override { visitor.visitElementA(*this); }
};

class ElementB : public Element {
 public:
  void accept(Visitor& visitor) override {
    visitor.visitElementB(*this);  // 第二次多态辨析
  }
};

// ==================================

// 扩展1
class Visitor1 : public Visitor {
 public:
  void visitElementA(ElementA& element) override {
    cout << "Visitor1 is processing ElementA" << endl;
  }

  void visitElementB(ElementB& element) override {
    cout << "Visitor1 is processing ElementB" << endl;
  }
};

// 扩展2
class Visitor2 : public Visitor {
 public:
  void visitElementA(ElementA& element) override {
    cout << "Visitor2 is processing ElementA" << endl;
  }

  void visitElementB(ElementB& element) override {
    cout << "Visitor2 is processing ElementB" << endl;
  }
};

int main() {
#if 0
  Visitor1 visitor;
  // Visitor1 is processing ElementB
  // Visitor1 is processing ElementA
#else
  Visitor2 visitor;
  // Visitor2 is processing ElementB
  // Visitor2 is processing ElementA
#endif
  ElementB elementB;
  elementB.accept(visitor);  // double dispatch

  ElementA elementA;
  elementA.accept(visitor);

  return 0;
}