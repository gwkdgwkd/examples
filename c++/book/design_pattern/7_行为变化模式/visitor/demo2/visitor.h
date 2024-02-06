#ifndef VISITOR_H
#define VISITOR_H

class Element;
class Visitor {
 public:
  virtual ~Visitor() = default;
  virtual void VisitConcreteElementA(Element* elm) = 0;
  virtual void VisitConcreteElementB(Element* elm) = 0;

  /*
  // 应该用这个函数代替上面两个：
  void Visitor::Visit(Element* elm) {
    if (typeid(*elm) == typeid(ConcreteElementA)) {
      // 提供对于ConcreteElementA的访问实现：
      std::cout << (typeid(*elm)).name() << std::endl;
      std::cout << "i will visit element A" << std::endl;
    } else if (typeid(*elm) == typeid(ConcreteElementB)) {
      // 提供对于ConcreteElementB的访问实现：
      std::cout << (typeid(*elm)).name() << std::endl;
      std::cout << "i will visit element B" << std::endl;
    } else if (typeid(*elm) == typeid(Element)) {
      // 可以在这里提供对所有Element的默认的访问实现：
      std::cout << (typeid(*elm)).name() << std::endl;
      std::cout << "i will visit element" << std::endl;
    }
  }
  //*/

 protected:
  Visitor() = default;
};
#endif  // VISITOR_H