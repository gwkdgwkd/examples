#include <iostream>
#include <vector>

// 别让异常逃离析构函数

class Widget {
 public:
  ~Widget() {}  // C++并不禁止析构函数抛出异常，但不鼓励这样做
};

void doSomething() {
  std::vector<Widget> v;
  // 函数退出时v被自动销毁，当v被销毁时，又责任销毁其内含的所有Widget。
  // 假设析构第一个时有异常，其他九个还应被销毁。
}

// 如果析构函数中发生了异常，可以：
// 1.结束程序，通常用abort完成；
// 2.吞下异常，但这是个坏主意，压制了某些动作失败的重要信息；
// 3.把产生异常的操作放到普通函数中，提供给使用者，由使用者决定对异常的处理。
//   这么做不会带来负担，而是给使用者一个处理错误的机会。

// 请记住：
// 1.析构函数决不要抛出异常；
// 2.如果一个析构函数调用的函数可能抛出异常，
//   析构函数应该捕获任何异常，然后吞下它们（不传播）或结束程序；
// 3.如果客户需要对某个操作函数运行间抛出的异常做出反应，
//   那么class应该提供一个普通函数（而非在析构函数中）执行操作。

int main() { return 0; }