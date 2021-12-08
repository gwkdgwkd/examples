#include <iostream>

class Library {
 public:
  ~Library() {}

  // Library开发人员：开发步骤1、3、5
  void step1() { std::cout << "Library::step1 run" << std::endl; }
  void step3() { std::cout << "Library::step3 run" << std::endl; }
  void step5() { std::cout << "Library::step5 run" << std::endl; }

  // Library开发人员：程序主流程
  void run() {
    step1();

    if (step2()) {
      step3();
    }

    for (int i = 0; i < 3; ++i) {
      step4();
    }

    step5();
  }

 protected:
  virtual bool step2() = 0;
  virtual void step4() = 0;
};