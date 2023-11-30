#include <iostream>

// C++不允许通过对象来访问private、protected属性的成员变量，
// 无论通过对象变量还是对象指针，都不行。
// 不过C++的这种限制仅仅是语法层面的，通过某种蹩脚的方法，
// 能够突破访问权限的限制，访问到private、protected属性的成员变量，
// 赋予这种特异功能的，正是强大而又灵活的指针。
// 在对象内存模型中，成员变量和对象的开头位置会有一定的距离，称为偏移。
// 一旦知道了对象的起始地址，再加上偏移就能够求得成员变量的地址，
// 知道了成员变量的地址和类型，也就能够知道它的值。
// 当通过对象指针访问成员变量时，编译器实际上也是使用这种方式来获取值。
// 当成员变量的访问权限为private或protected时，可以手动转换，
// 只要能正确计算偏移即可，这样就突破了访问权限的限制。
// C++的成员访问权限仅仅是语法层面上的，仅对取成员运算符.和->起作用，
// 而无法防止直接通过指针来访问。
// 可以认为这是指针的强大，也可以认为是C++语言设计的瑕疵。

class A {
 public:
  A(int a, int b, int c) : m_a(a), m_b(b), m_c(c) {}
  void showAddr() {
    std::cout << "a.addr = " << &m_a << std::endl;
    std::cout << "b.addr = " << &m_b << std::endl;
    std::cout << "c.addr = " << &m_c << std::endl;
  }
  int m_a;

 protected:
  int m_b;

 private:
  int m_c;
};

void func1() {
  A obj(10, 20, 30);
  obj.m_a;
  // obj.m_b;  // 报错
  // obj.m_c;

  std::cout << "sizeof(int) = " << sizeof(int) << std::endl;  // 4

  int *pa = (int *)((long)&obj + 0);
  int *pb = (int *)((long)&obj + sizeof(int));
  int *pc = (int *)((long)&obj + sizeof(int) * 2);
  std::cout << "obj.addr = " << &obj << std::endl;
  std::cout << "a = " << *pa << "," << pa << std::endl;
  std::cout << "b = " << *pb << "," << pb << std::endl;
  std::cout << "c = " << *pc << "," << pc << std::endl;
  // obj.addr = 0x7fffb995b7bc
  // a = 10,0x7fffb995b7bc
  // b = 20,0x7fffb995b7c0
  // c = 30,0x7fffb995b7c4

  obj.showAddr();
  // a.addr = 0x7fffb995b7bc
  // b.addr = 0x7fffb995b7c0
  // c.addr = 0x7fffb995b7c4
}

void func2() {
  A *p = new A(40, 50, 60);
  // 编译器内部会发生类似转换：int a2 = *(int*)((int)p + 0);
  p->m_a;
  // p->m_b;  // 报错
  // p->m_c;

  int *pa = (int *)p + 0;
  int *pb = (int *)p + 1;
  int *pc = (int *)p + 2;
  std::cout << "obj.addr = " << p << std::endl;
  std::cout << "a = " << *pa << "," << pa << std::endl;
  std::cout << "b = " << *pb << "," << pb << std::endl;
  std::cout << "c = " << *pc << "," << pc << std::endl;
  // obj.addr = 0x55cd475a4eb0
  // a = 40,0x55cd475a4eb0
  // b = 50,0x55cd475a4eb4
  // c = 60,0x55cd475a4eb8

  p->showAddr();
  // a.addr = 0x55cd475a4eb0
  // b.addr = 0x55cd475a4eb4
  // c.addr = 0x55cd475a4eb8
}

int main(int argc, char *argv[]) {
  if (argc < 2) {
    std::cout << argv[0] << " i [0 - 1]" << std::endl;
    return 0;
  }
  int type = atoi(argv[1]);
  switch (type) {
    case 0:
      func1();
      break;
    case 1:
      func2();
      break;
    default:
      std::cout << "invalid type" << std::endl;
      break;
  }

  return 0;
}