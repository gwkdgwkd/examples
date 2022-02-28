#include <iostream>

using namespace std;

// C++11标准中借助右值引用可以为指定类添加移动构造函数，
// 这样当使用该类的右值对象（可以理解为临时对象）初始化同类对象时，编译器会优先选择移动构造函数。

// 移动构造函数的调用时机是：用同类的右值对象初始化新对象。
// 那么，用当前类的左值对象（有名称，能获取其存储地址的实例对象）初始化同类对象时，
// 是否就无法调用移动构造函数了呢？当然不是，C++11标准中已经给出了解决方案，即调用move()函数。

// move本意为"移动"，但该函数并不能移动任何数据，它的功能很简单，就是将某个左值强制转化为右值。
// 其语法格式为：move(arg) 其中，arg表示指定的左值对象。该函数会返回arg对象的右值形式。

class movedemo {
 public:
  movedemo() : num(new int(0)) { cout << "construct!" << endl; }
  movedemo(const movedemo &d) : num(new int(*d.num)) {  // 拷贝构造函数
    cout << "copy construct!" << endl;
  }
  movedemo(movedemo &&d) : num(d.num) {  // 移动构造函数
    d.num = NULL;
    cout << "move construct!" << endl;
  }

 public:  // 这里应该是private，使用public是为了更方便说明问题
  int *num;
};

class first {
 public:
  first() : num(new int(0)) { cout << "construct!" << endl; }
  first(first &&d) : num(d.num) {  // 移动构造函数
    d.num = NULL;
    cout << "first move construct!" << endl;
  }

 public:
  int *num;
};
class second {
 public:
  second() : fir() {}
  // 用first类的移动构造函数初始化fir
  second(second &&sec) : fir(move(sec.fir)) {
    cout << "second move construct" << endl;
  }

 public:
  first fir;
};

int main() {
  movedemo demo;  // construct!

  // demo对象作为左值，直接用于初始化demo2对象，其底层调用的是拷贝构造函数。
  movedemo demo2 = demo;                       // copy construct!
  cout << "demo2.num " << *demo2.num << endl;  // 0
  cout << "demo.num " << *demo.num << endl;    // 0

  // 通过调用move()函数可以得到demo对象的右值形式，用其初始化demo3对象，编译器会优先调用移动构造函数。
  movedemo demo3 = std::move(demo);            // move construct!
  cout << "demo3.num " << *demo3.num << endl;  // 0
  // cout << *demo.num << endl;  // 此时demo.num = NULL，段错误
  // 注意，调用拷贝构造函数，并不影响demo对象，但如果调用移动构造函数，
  // 由于函数内部会重置demo.num指针的指向为NULL，所以上面代码会导致程序运行时发生错误。

  second oth;  // construct!
  second oth2 = move(oth);
  // first move construct!
  // second move construct

  // cout << *oth.fir.num << endl;  // 程序报运行时错误
  // 由于oth为左值，如果想调用移动构造函数为oth2初始化，需先利用move()函数生成一个oth的右值版本；
  // oth对象内部还包含一个first类对象，对于oth.fir来说，其也是一个左值，
  // 所以在初始化oth.fir时，还需要再调用一次move()函数。

  return 0;
}