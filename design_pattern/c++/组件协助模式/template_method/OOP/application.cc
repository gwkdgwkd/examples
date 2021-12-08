#include "library.h"

// 面向对象软件设计流程：
// Library (晚绑定)--> Application

class Application : public Library {
 public:
  // Application开发人员：开发步骤2、4
  virtual bool step2() {
    std::cout << "Application::step2 run" << std::endl;
    return true;
  }
  virtual void step4() { std::cout << "Application::step4 run" << std::endl; }
};

int main() {
  Library *libptr = new Application();
  libptr->run();

  // Library::step1 run
  // Application::step2 run
  // Library::step3 run
  // Application::step4 run
  // Application::step4 run
  // Application::step4 run
  // Library::step5 run
}