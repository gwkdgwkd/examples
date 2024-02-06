#ifndef VISITOR_H
#define VISITOR_H

class ElementA;
class ElementB;

class Visitor {
 public:
  virtual void visitElementA(ElementA& element) = 0;
  virtual void visitElementB(ElementB& element) = 0;

  virtual ~Visitor() {}
};
#endif  // VISITOR_H