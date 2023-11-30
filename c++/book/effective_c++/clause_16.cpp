#include <iostream>

// 成对使用new和delete时要采用相同形式

// 使用new生成一个对象，有两件事发生：
// 1.内存被分配出来（通过operator new函数）；
// 2.针对此内存会有一个（或多个）构造函数被调用。
// 使用delete也有两件事发生：
// 1.针对此内存会有一个（或多个）析构函数被调用；
// 2.然后内存被释放（通过operator delete函数）。
// delete的最大问题在于：即将被删除的内存之内究竟有多少对象？
// 答案决定了有多少个析构函数必须被调用。

// 即将被删除的那个指针，所指的是单一对象或对象数组？
// 这是必不可缺的问题，因为单一对象的内存布局一般与数组的内存布局不同。

class A {
 public:
  A() { std::cout << "A" << std::endl; }
  ~A() { std::cout << "~A" << std::endl; }
};

namespace n1 {
void func1() {
  A *pa = new A;  // A

  delete pa;  // ~A
}

void func2() {
  A *pa = new A[3];
  // A
  // A
  // A

  delete[] pa;
  // ~A
  // ~A
  // ~A
}

void func3() {
  A *pa = new A[3];
  // A
  // A
  // A

  delete pa;  // 崩溃
  // ~A
  // munmap_chunk(): invalid pointer
}

}  // namespace n1

namespace n2 {
// 最好尽量不要对数组做typedef动作，可以使用string，vector等代替数组：
typedef A Array[3];
void func1() {
  A *pa = new Array;
  // A
  // A
  // A

  // typedef后很自然使用这种形式的delete，但会导致行为未定义：
  delete pa;  // 崩溃
  // ~A
  // munmap_chunk(): invalid pointer
}
void func2() {
  A *pa = new Array;
  // A
  // A
  // A

  delete[] pa;
  // ~A
  // ~A
  // ~A
}
}  // namespace n2

// 请记住：
// 1.如果在new表达式中使用[]，必须在相应的delete表达式中也使用[]；
// 2.如果在new表达式中不使用[]，一定不要在相应的delete表达式中使用[]。

int main(int argc, char *argv[]) {
  if (argc < 2) {
    std::cout << argv[0] << " i [0 - 4]" << std::endl;
    return 0;
  }
  int type = argv[1][0] - '0';
  switch (type) {
    case 0:
      n1::func1();
      break;
    case 1:
      n1::func2();
      break;
    case 2:
      n1::func3();
      break;
    case 3:
      n2::func1();
      break;
    case 4:
      n2::func2();
      break;
    default:
      std::cout << "invalid type" << std::endl;
      break;
  }

  return 0;
}