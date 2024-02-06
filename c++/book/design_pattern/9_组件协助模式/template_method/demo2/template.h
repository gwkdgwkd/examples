#ifndef TEMPLATE_H
#define TEMPLATE_H

class AbstractClass {
 public:
  virtual ~AbstractClass() = default;
  void TemplateMethod() {
    this->PrimitiveOperation1();
    this->PrimitiveOperation2();
  }

 protected:
  virtual void PrimitiveOperation1() = 0;
  virtual void PrimitiveOperation2() = 0;
  AbstractClass() = default;

 private:
};
#endif  // TEMPLATE_H