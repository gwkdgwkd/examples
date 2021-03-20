#include <iostream>

using namespace std;

// C++不允许通过对象来访问private、protected属性的成员变量。无论通过对象变量还是对象指针，都不行。
// 不过C++的这种限制仅仅是语法层面的，通过某种“蹩脚”的方法，我们能够突破访问权限的限制，访问到private、protected属性的成员变量，赋予我们这种“特异功能”的，
// 正是强大而又灵活的指针（Pointer）。
// 在对象的内存模型中，成员变量和对象的开头位置会有一定的距离。
// 将这段距离称为偏移（Offset）。一旦知道了对象的起始地址，再加上偏移就能够求得成员变量的地址，知道了成员变量的地址和类型，也就能够轻而易举地知道它的值。
// 当通过对象指针访问成员变量时，编译器实际上也是使用这种方式来取得它的值。
// 当成员变量的访问权限为private或protected时，可以手动转换，只要能正确计算偏移即可，这样就突破了访问权限的限制。
// C++的成员访问权限仅仅是语法层面上的，是指访问权限仅对取成员运算符.和->起作用，而无法防止直接通过指针来访问。你可以认为这是指针的强大，也可以认为是C++语言设计的瑕疵。

class A {
 public:
  A(int a, int b, int c);
  void showAddr();
  int m_a;

 protected:
  int m_b;

 private:
  int m_c;
};
A::A(int a, int b, int c) : m_a(a), m_b(b), m_c(c) {}
void A::showAddr() {
  cout << "a.addr = " << &m_a << ", b.addr = " << &m_b << ", c.addr = " << &m_c
       << endl;
}

int main() {
  A obj(10, 20, 30);
  int a1 = obj.m_a;
  // int b = obj.m_b;  // Compile Error
  A *p = new A(40, 50, 60);
  int a2 = p->m_a;
  // 此时编译器内部会发生类似下面的转换：
  // int a2 = *(int*)((int)p + 0); // 如果是b，0->sizeof(int)
  // int c = p->m_c;  // Compile Error

  cout << "sizeof(int) = " << sizeof(int) << endl;  // 4
  int aa1 = *(int *)((long)&obj + 0);
  int b1 = *(int *)((long)&obj + sizeof(int));
  int c1 = *(int *)((long)&obj + sizeof(int) * 2);
  cout << "a1 = " << aa1 << ", b1 = " << b1 << ", c1 = " << c1 << endl;
  cout << "obj.addr = " << &obj << endl;
  cout << "obj.m_a.addr = " << hex << (long)&obj + 0
       << ", obj.m_b.addr = " << hex << (long)&obj + sizeof(int)
       << ", obj.m_c.addr = " << hex << (long)&obj + sizeof(int) * 2 << endl;
  obj.showAddr();
  // a1 = 10, b1 = 20, c1 = 30
  // obj.addr = 0x7ffdb36c2c40
  // obj.m_a.addr = 7ffdb36c2c40, obj.m_b.addr = 7ffdb36c2c44, obj.m_c.addr = 7ffdb36c2c48
  // a.addr = 0x7ffdb36c2c40, b.addr = 0x7ffdb36c2c44, c.addr = 0x7ffdb36c2c48
  // int aa2 = *(int *)((long)p + 0);
  // int b2 = *(int *)((long)p + sizeof(int));
  // int c2 = *(int *)((long)p + sizeof(int) * 2);
  int aa2 = *((int *)p + 0);
  int b2 = *((int *)p + 1);
  int c2 = *((int *)p + 2);
  cout << dec << "a2 = " << aa2 << ", b2 = " << b2 << ", c2 = " << c2 << endl;
  cout << "obj.addr = " << p << endl;
  cout << "p.m_a.addr = " << hex << (long)p + 0 << ", p.m_b.addr = " << hex
       << (long)p + sizeof(int) << ", p.m_c.addr = " << hex
       << (long)p + sizeof(int) * 2 << endl;
  p->showAddr();
  // a2 = 40, b2 = 50, c2 = 60
  // obj.addr = 0x1fddc20
  // p.m_a.addr = 1fddc20, p.m_b.addr = 1fddc24, p.m_c.addr = 1fddc28
  // a.addr = 0x1fddc20, b.addr = 0x1fddc24, c.addr = 0x1fddc28

  return 0;
}