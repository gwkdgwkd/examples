#include <iostream>

// Template Method

// 动机（Motivation）：
// 1.在软件构建过程中，对于某一项任务，它常常有稳定的整体操作结构，
//   但各个子步骤却有很多改变的需求，或者由于固有的原因，
//   比如框架与应用之间的关系，而无法和任务的整体结构同时实现；
// 2.如何在确定稳定操作结构的前提下，来灵活应对各个子步骤的变化或者晚期实现需求？

// 定义：
// 定义一个操作中的算法的骨架（稳定），而将一些步骤延迟（变化）到子类中。
// Template Method使得子类可以不改变算法的结构即可重定义该算法的某些特定步骤。

// 要点总结：
// 1.Template Method是一种非常基础性的设计模式，在面向对象系统中有着大量的应用，
//   它用最简洁的机制（虚函数的多态性）为很多应用程序框架提供了灵活的扩展点，
//   是代码复用方面的基本实现结构；
// 2.除了可以灵活应对子步骤的变化外，不要调用我，让我来调用你，
//   这样的反向控制结构是Template Method的典型应用；
// 3.在具体实现方面，被Template Method调用的虚方法可以具有实现，
//   也可以没有任何实现（抽象方法、纯虚方法），但一般推荐将它们设置为protected方法。

namespace n1 {
// POP

class Library {
 public:
  // Library开发人员：开发步骤1、3、5
  void step1() { std::cout << "Library::step1 run" << std::endl; }
  void step3() { std::cout << "Library::step3 run" << std::endl; }
  void step5() { std::cout << "Library::step5 run" << std::endl; }
};

class Application {
 public:
  // Application开发人员：开发步骤2、4
  bool step2() {
    std::cout << "Application::step2 run" << std::endl;
    return true;
  }
  void step4() { std::cout << "Application::step4 run" << std::endl; }
};

// 面向对象软件设计流程：Library (晚绑定)--> Application

void func() {
  // Application开发人员：程序主流程
  Library lib;
  Application app;

  lib.step1();

  if (app.step2()) {
    lib.step3();
  }

  for (int i = 0; i < 3; ++i) {
    app.step4();
  }

  lib.step5();

  // Library::step1 run
  // Application::step2 run
  // Library::step3 run
  // Application::step4 run
  // Application::step4 run
  // Application::step4 run
  // Library::step5 run
}
}  // namespace n1

namespace n2 {
// OOP

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

class Application : public Library {
 public:
  // Application开发人员：开发步骤2、4
  virtual bool step2() {
    std::cout << "Application::step2 run" << std::endl;
    return true;
  }
  virtual void step4() { std::cout << "Application::step4 run" << std::endl; }
};

void func() {
  Library* libptr = new Application();
  libptr->run();

  // Library::step1 run
  // Application::step2 run
  // Library::step3 run
  // Application::step4 run
  // Application::step4 run
  // Application::step4 run
  // Library::step5 run
}
}  // namespace n2

int main(int argc, char* argv[]) {
  if (argc < 2) {
    std::cout << argv[0] << " i [0 - 1]" << std::endl;
    return 0;
  }
  int type = argv[1][0] - '0';

  switch (type) {
    case 0:
      n1::func();
      break;
    case 1:
      n2::func();
      break;
    default:
      std::cout << "invalid type" << std::endl;
      break;
  }

  return 0;
}