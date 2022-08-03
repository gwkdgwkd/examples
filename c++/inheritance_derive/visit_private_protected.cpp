#include <iostream>

// C++不允许通过对象来访问private、protected属性的成员变量，
// 无论通过对象变量还是对象指针，都不行。
// 不过C++的这种限制仅仅是语法层面的，通过某种蹩脚的方法，
// 能够突破访问权限的限制，访问到private、protected属性的成员变量，
// 赋予我们这种特异功能的，正是强大而又灵活的指针。
// 在对象的内存模型中，成员变量和对象的开头位置会有一定的距离，将这段距离称为偏移。
// 一旦知道了对象的起始地址，再加上偏移就能够求得成员变量的地址，
// 知道了成员变量的地址和类型，也就能够知道它的值。
// 当通过对象指针访问成员变量时，编译器实际上也是使用这种方式来取得它的值。
// 当成员变量的访问权限为private或protected时，可以手动转换，
// 只要能正确计算偏移即可，这样就突破了访问权限的限制。
// C++的成员访问权限仅仅是语法层面上的，是指访问权限仅对取成员运算符.和->起作用，
// 而无法防止直接通过指针来访问。
// 你可以认为这是指针的强大，也可以认为是C++语言设计的瑕疵。

class A {
 public:
  A(int a, int b, int c) : m_a(a), m_b(b), m_c(c) {}
  void showAddr() {
    std::cout << "a.addr = " << &m_a << ", b.addr = " << &m_b
              << ", c.addr = " << &m_c << std::endl;
  }
  int m_a;

 protected:
  int m_b;

 private:
  int m_c;
};

void func1() {
  A obj(10, 20, 30);
  int a = obj.m_a;
  // int b = obj.m_b;  // Compile Error
  // int c = obj.m_c;

  std::cout << "sizeof(int) = " << sizeof(int) << std::endl;  // 4

  int a1 = *(int *)((long)&obj + 0);
  int b1 = *(int *)((long)&obj + sizeof(int));
  int c1 = *(int *)((long)&obj + sizeof(int) * 2);
  std::cout << "a1 = " << a1 << ", b1 = " << b1 << ", c1 = " << c1 << std::endl;
  // a1 = 10, b1 = 20, c1 = 30

  std::cout << "obj.addr = 0x" << &obj
            << std::endl;  // obj.addr = 0x0x7ffd1629a00c
  std::cout << "obj.m_a.addr = 0x" << std::hex << (long)&obj + 0
            << ", obj.m_b.addr = 0x" << std::hex << (long)&obj + sizeof(int)
            << ", obj.m_c.addr = 0x" << std::hex << (long)&obj + sizeof(int) * 2
            << std::endl;
  // obj.m_a.addr = 0x7ffd1629a00c, obj.m_b.addr = 0x7ffd1629a010, obj.m_c.addr = 0x7ffd1629a014
  obj.showAddr();
  // a.addr = 0x7ffd1629a00c, b.addr = 0x7ffd1629a010, c.addr = 0x7ffd1629a014
}

void func2() {
  A *p = new A(40, 50, 60);
  // 编译器内部会发生类似转换：int a2 = *(int*)((int)p + 0);
  int a1 = p->m_a;
  // int b1 = p->m_b;  // int a2 = *(int*)((int)p + sizeof(int));

  int a2 = *((int *)p + 0);
  int b2 = *((int *)p + 1);
  int c2 = *((int *)p + 2);
  std::cout << std::dec << "a2 = " << a2 << ", b2 = " << b2 << ", c2 = " << c2
            << std::endl;
  // a2 = 40, b2 = 50, c2 = 60

  std::cout << "obj.addr = 0x" << p << std::endl;
  // obj.addr = 0x0x55ec99bbc2c0
  std::cout << "p.m_a.addr = 0x" << std::hex << (long)p + 0
            << ", p.m_b.addr = 0x" << std::hex << (long)p + sizeof(int)
            << ", p.m_c.addr = 0x" << std::hex << (long)p + sizeof(int) * 2
            << std::endl;
  // p.m_a.addr = 0x55ec99bbc2c0, p.m_b.addr = 0x55ec99bbc2c4, p.m_c.addr = 0x55ec99bbc2c8
  p->showAddr();
  // a.addr = 0x55ec99bbc2c0, b.addr = 0x55ec99bbc2c4, c.addr = 0x55ec99bbc2c8
}

int main(int argc, char *argv[]) {
  func1();
  func2();
  return 0;
}