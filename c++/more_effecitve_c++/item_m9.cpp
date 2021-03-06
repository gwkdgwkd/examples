#include <iostream>
#include <memory>

using namespace std;

// 使用析构函数防止资源泄漏

// 对指针说再见。必须得承认：你永远都不会喜欢使用指针。
// 不用对所有的指针说再见，但是你需要对用来操纵局部资源（local resources）的指针说再见。

class ALA {
 public:
  virtual void processAdoption() = 0;
};
class Puppy : public ALA {
 public:
  virtual void processAdoption();
};
class Kitten : public ALA {
 public:
  virtual void processAdoption();
};
ALA* readALA(istream& s) {}
void processAdoptions1(istream& dataSource) {
  while (dataSource) {
    ALA* pa = readALA(dataSource);
    pa->processAdoption();
    // 在每次循环结尾处删除pa。这是必须的，因为每次调用readALA都建立一个堆对象。如果不删除对象，循环将产生资源泄漏。
    delete pa;
  }
}
// 如果pa->processAdoption抛出了一个异常，将会发生什么？
// processAdoptions没有捕获异常，所以异常将传递给processAdoptions的调用者。
// 传递中，processAdoptions函数中的调用pa->processAdoption语句后的所有语句都被跳过，这就是说pa没有被删除。
// 结果，任何时候pa->processAdoption抛出一个异常都会导致processAdoptions内存泄漏。
// 堵塞泄漏很容易：
void processAdoptions2(istream& dataSource) {
  while (dataSource) {
    ALA* pa = readALA(dataSource);
    try {
      pa->processAdoption();
    } catch (...) {  // 捕获所有异常
      delete pa;     // 当异常抛出时，避免内存泄漏
      throw;         // 传送异常给调用者
    }
    delete pa;  // 当没有异常抛出时，避免资源泄漏
  }
}
// 必须用try和catch对你的代码进行小改动。更重要的是你必须写双份清除代码，一个为正常的运行准备，一个为异常发生时准备。

// 可以把总被执行的清除代码放入processAdoptions函数内的局部对象的析构函数里，这样可以避免重复书写清除代码。
// 因为当函数返回时局部对象总是被释放，无论函数是如何退出的。
// 仅有一种例外就是当你调用longjmp时。Longjmp的这个缺点是C++率先支持异常处理的主要原因。
// 具体方法是用一个对象代替指针pa，这个对象的行为与指针相似。
void processAdoptions(istream& dataSource) {
  while (dataSource) {
    shared_ptr<ALA> pa(readALA(dataSource));
    pa->processAdoption();
  }
}
// 用一个对象存储需要被自动释放的资源，然后依靠对象的析构函数来释放资源，这种思想不只是可以运用在指针上，还能用在其它资源的分配和释放上。

// 资源应该被封装在一个对象里，遵循这个规则，你通常就能避免在存在异常环境里发生资源泄漏。

int main() { return 0; }