#include <iostream>

using namespace std;

// 引用虽然是基于指针实现的，但它比指针更加易用，从上面的两个例子也可以看出来，通过指针获取数据时需要加*，书写麻烦，而引用不需要，它和普通变量的使用方式一样。
// C++的发明人Bjarne Stroustrup也说过，他在C++中引入引用的直接目的是为了让代码的书写更加漂亮，尤其是在运算符重载中，不借助引用有时候会使得运算符的使用很麻烦。

// 引用和指针的其他区别:
//  1) 引用必须在定义时初始化，并且以后也要从一而终，不能再指向其他数据；而指针没有这个限制，指针在定义时不必赋值，以后也能指向任意数据。
//  2) 可以有const指针，但是没有const引用。也就是说，引用变量不能定义为下面的形式：
//     int a = 20;
//     int & const r = a;
//     因为r本来就不能改变指向，加上const是多此一举。
// 3) 指针可以有多级，但是引用只能有一级，例如，int **p是合法的，而int &&r是不合法的。如果希望定义一个引用变量来指代另外一个引用变量，那么也只需要加一个&，如下所示：
//    int a = 10;
//    int &r = a;
//    int &rr = r;
// 4) 指针和引用的自增（++）自减（--）运算意义不一样。对指针使用++表示指向下一份数据，对引用使用++表示它所指代的数据本身加1；自减（--）也是类似的道理。

int num = 99;
class A {
 public:
  A();
  void show() {
    cout << "n addr: " << hex << showbase << &n << endl;
    cout << "r addr: " << hex << showbase << &r << endl;
    cout << "p addr: " << hex << showbase << p << endl;
  }

 private:
  int n;
  int &r;
  int *p;
};
A::A() : n(0), r(num), p(&num) {}

int main() {
  int a = 99;
  int &r = a;
  cout << a << ", " << r << endl;    // 99, 99
  cout << &a << ", " << &r << endl;  // 0x7ffe7c31f69c, 0x7ffe7c31f69c
  // 变量是要占用内存的，虽然我们称r为变量，但是通过&r获取到的却不是r的地址，而是a的地址，感觉r这个变量不占用独立的内存，它和a指代的是同一份内存。

  A *aa = new A();
  cout << sizeof(A) << endl;  // 16
  cout << "aa addr: " << hex << showbase << aa << endl;
  cout << "num addr: " << &num << endl;
  aa->show();
  // aa addr: 0x16c1030
  // num addr: 0x602090
  // n addr: 0x16c1030
  // r addr: 0x602090
  // p addr: 0x602090
  // 从运行结果可以看出：
  //  成员变量r是占用内存的，如果不占用的话，sizeof(A)的结果应该为8。
  //  r与num地址是一样的。
  // 使用&r取地址时，编译器会对代码进行隐式的转换，使得代码输出的是r的内容（a的地址），而不是r的地址，这就是为什么获取不到引用变量的地址的原因。
  // 也就是说，不是变量r不占用内存，而是编译器不让获取它的地址。
  // 其实引用只是对指针进行了简单的封装，它的底层依然是通过指针实现的，引用占用的内存和指针占用的内存长度一样，在32位环境下是4个字节，在64位环境下是8个字节，
  // 之所以不能获取引用的地址，是因为编译器进行了内部转换。
  // int a = 99;
  // int &r = a;
  // r = 18;
  // cout<<&r<<endl;
  // 编译时转换为：
  // int a = 99;
  // int *r = &a;
  // *r = 18;
  // cout<<r<<endl;
  // 当引用作为函数参数时，也会有类似的转换。
  // void swap(int &r1, int &r2){}
  // swap(num1, num2);
  // 编译时转换为：
  // void swap(int *r1, int *r2){}
  // swap(&num1, &num2);

  int a2 = 10;
  int &r2 = a2;
  r2++;
  cout << noshowbase << dec << r2 << endl;  // 11
  int arr[2] = {27, 84};
  int *p2 = arr;
  p2++;
  cout << *p2 << endl;  // 84

  return 0;
}