#include "library.h"

// 结构化软件设计流程:
// Library <--(早绑定) Application

class Application {
 public:
  // Application开发人员：开发步骤2、4
  bool step2() {
    std::cout << "Application::step2 run" << std::endl;
    return true;
  }
  void step4() { std::cout << "Application::step4 run" << std::endl; }
};

int main() {
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
