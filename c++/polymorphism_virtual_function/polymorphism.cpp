#include <iostream>

// 当通过指针访问类的成员函数时：
// 1.如果该函数是非虚函数，那么编译器会根据指针的类型找到该函数；
//   也就是说，指针是哪个类的类型就调用哪个类的函数。
// 2.如果该函数是虚函数，并且派生类有同名的函数遮蔽它，那么编译器会根据指针的指向找到该函数；
//   也就是说，指针指向的对象属于哪个类就调用哪个类的函数，这就是多态。
// 编译器之所以能通过指针指向的对象找到虚函数，是因为在创建对象时额外地增加了虚函数表。
// 如果一个类包含了虚函数，那么在创建该类的对象时就会额外地增加一个数组，
// 数组中的每一个元素都是虚函数的入口地址。
// 不过数组和对象是分开存储的，为了将对象和数组关联起来，
// 编译器还要在对象中安插一个指针，指向数组的起始位置。
// 这里的数组就是虚函数表（Virtual function table），简写为vtable。

// 查看内存布局：
// g++ -c -fdump-lang-class polymorphism.cpp

using FunPtr = void (*)(void);

using VFPTR = void (*)(void);  // typedef void (*VFPTR)();
void PrintVTable(VFPTR vTable[]) {
  std::cout << "虚表地址：" << vTable << std::endl;
  for (int i = 0; vTable[i] != nullptr; ++i) {
    // 多个虚函数表时，判断第一个表结束的标识有问题，怎么判断？
    std::cout << "第" << i + 1 << "个虚函数地址" << (long *)vTable[i]
              << std::endl;
    VFPTR f = vTable[i];
    f();
  }
  std::cout << std::endl;
}

namespace n1 {
// 存在虚函数时，单继承的内存模型
class A {
 public:
  virtual void func_a() { std::cout << "A::func_a" << std::endl; }

 protected:
  virtual void func_b() { std::cout << "A::func_b" << std::endl; }
  int i = 11;
  int j = 12;
};
class B : public A {
 public:
  virtual void func_a() { std::cout << "B::func_a" << std::endl; }

 protected:
  int k = 21;

 private:
  virtual void func_c() { std::cout << "B::func_c" << std::endl; }
};
class C : public B {
 public:
  virtual void func_a() { std::cout << "C::func_a" << std::endl; }
  virtual void func_d() { std::cout << "C::func_d" << std::endl; }

 protected:
  int m = 31;
};
class D : public A {
 public:
  virtual void func_e() { std::cout << "D::func_e" << std::endl; }
  virtual void func_a() { std::cout << "D::func_a" << std::endl; }
  virtual void func_f() { std::cout << "D::func_f" << std::endl; }

 protected:
  int n = 22;
};

void func1() {
  // 64位计算机，计算虚函数相关地址时，使用long *
  // 32位计算机，计算虚函数相关地址时，使用int *
  A obj;

  // 对象的大小 = 虚函数表的指针(long *，8字节) + i(4字节) + j(4字节)
  std::cout << "obj begin addr: " << &obj << " (" << sizeof(A) << ","
            << sizeof(obj) << ")" << std::endl;

  long *vt_ptr = (long *)&obj;  // 虚函数表的指针放到对象的开头，与对象地址相同
  long *f_vt_addr = (long *)*((long *)&obj + 0);  // 虚函数表的指针指向虚函数表
  std::cout << "#1 v_table ptr: " << vt_ptr
            << "     --> first v_table addr: " << f_vt_addr << std::endl;

  // 第一个成员变量的地址 = 对象的地址 + 虚函数表的指针大小
  int *mem1_ptr = (int *)((long *)&obj + 1);
  // 第一个虚函数的地址 = 虚函数表的地址 + 偏移量
  long *func_a_addr = f_vt_addr + 0;  // 第一个函数偏移量为0
  std::cout << "#2 mem1 i addr: " << mem1_ptr << "(" << *mem1_ptr
            << ")      func_a in v_table: " << (long *)*(func_a_addr)
            << std::endl;

  // 第一个成员的地址加上偏移量1，是第二个成员的地址
  int *mem2_ptr = mem1_ptr + 1;
  long *func_b_addr = f_vt_addr + 1;  // 第二个函数偏移量为1
  std::cout << "#3 mem2 j addr: " << mem2_ptr << "(" << *mem2_ptr
            << ")      func_b in v_table: " << (long *)*(func_b_addr)
            << std::endl;

  // 内存模型：
  // obj begin addr: 0x7ffe9e89d6e0 (16,16)
  // #1 v_table ptr: 0x7ffe9e89d6e0     --> first v_table addr: 0x56186b3ccd38
  // #2 mem1 i addr: 0x7ffe9e89d6e8(11)      func_a in v_table: 0x56186b3caa84
  // #3 mem2 j addr: 0x7ffe9e89d6ec(12)      func_b in v_table: 0x56186b3caac0

  FunPtr func_a = (FunPtr) * ((long *)*((long *)&obj + 0) + 0);
  FunPtr func_b = (FunPtr) * ((long *)*((long *)&obj + 0) + 1);
  std::cout << (long *)func_a << std::endl;  // 0x56186b3caa84
  std::cout << (long *)func_b << std::endl;  // 0x56186b3caac0
  func_a();                                  // A::func_a
  func_b();                                  // A::func_b

  long **pVtab = (long **)&obj;
  ((FunPtr)pVtab[0][0])();  // A::func_a
  ((FunPtr)pVtab[0][1])();  // A::func_b

  // 虚函数表的最后一个位置，不是nullptr么？
  if ((void *)pVtab[0][2] == nullptr) {
    std::cout << "nullptr" << std::endl;
  } else {
    std::cout << "not nullptr" << std::endl;
  }
  // not nullptr

  // 使用下面的函数，但是怎么判断虚函数表结束了？
  // VFPTR *vTableb = (VFPTR *)(*(long *)&obj);
  // PrintVTable(vTableb);
}

void func2() {
  B obj;

  std::cout << "obj begin addr: " << &obj << " (" << sizeof(B) << ","
            << sizeof(obj) << ")" << std::endl;

  long *vt_ptr = (long *)&obj;
  long *f_vt_addr = (long *)*((long *)&obj + 0);
  std::cout << "#1 v_table ptr: " << vt_ptr
            << "     --> first v_table addr: " << f_vt_addr << std::endl;

  int *mem1_ptr = (int *)((long *)&obj + 1);
  long *func_a_addr = f_vt_addr + 0;
  std::cout << "#2 mem1 i addr: " << mem1_ptr << "(" << *mem1_ptr
            << ")      func_a in v_table: " << (long *)*func_a_addr
            << std::endl;

  int *mem2_ptr = mem1_ptr + 1;
  long *func_b_addr = f_vt_addr + 1;
  std::cout << "#3 mem2 j addr: " << mem2_ptr << "(" << *mem2_ptr
            << ")      func_b in v_table: " << (long *)*func_b_addr
            << std::endl;

  int *mem3_ptr = mem2_ptr + 1;
  long *func_c_addr = f_vt_addr + 2;
  std::cout << "#4 mem3 k addr: " << mem3_ptr << "(" << *mem3_ptr
            << ")      func_c in v_table: " << (long *)*func_c_addr
            << std::endl;

  // 内存模型：
  // obj begin addr: 0x7ffe5a394ad0 (24,24)
  // #1 v_table ptr: 0x7ffe5a394ad0     --> first v_table addr: 0x55a629ae1d10
  // #2 mem1 i addr: 0x7ffe5a394ad8(11)      func_a in v_table: 0x55a629adfb34
  // #3 mem2 j addr: 0x7ffe5a394adc(12)      func_b in v_table: 0x55a629adfaf8
  // #4 mem3 k addr: 0x7ffe5a394ae0(21)      func_c in v_table: 0x55a629adfb70

  FunPtr func_a = (FunPtr) * ((long *)*((long *)&obj + 0) + 0);
  FunPtr func_b = (FunPtr) * ((long *)*((long *)&obj + 0) + 1);
  FunPtr func_c = (FunPtr) * ((long *)*((long *)&obj + 0) + 2);
  std::cout << (long *)func_a << std::endl;  // 0x55a629adfb34
  std::cout << (long *)func_b << std::endl;  // 0x55a629adfaf8
  std::cout << (long *)func_c << std::endl;  // 0x55a629adfb70
  func_a();                                  // B::func_a
  func_b();                                  // A::func_b
  func_c();                                  // B::func_c

  long **pVtab = (long **)&obj;
  ((FunPtr)pVtab[0][0])();  // B::func_a
  ((FunPtr)pVtab[0][1])();  // A::func_b
  ((FunPtr)pVtab[0][2])();  // B::func_c
}

void func3() {
  C obj;

  std::cout << "obj begin addr: " << &obj << " (" << sizeof(C) << ","
            << sizeof(obj) << ")" << std::endl;

  long *vt_ptr = (long *)&obj;
  long *f_vt_addr = (long *)*((long *)&obj + 0);
  std::cout << "#1 v_table ptr: " << vt_ptr
            << "     --> first v_table addr: " << f_vt_addr << std::endl;

  int *mem1_ptr = (int *)((long *)&obj + 1);
  long *func_a_addr = f_vt_addr + 0;
  std::cout << "#2 mem1 i addr: " << mem1_ptr << "(" << *mem1_ptr
            << ")      func_a in v_table: " << (long *)*func_a_addr
            << std::endl;

  int *mem2_ptr = mem1_ptr + 1;
  long *func_b_addr = f_vt_addr + 1;
  std::cout << "#3 mem2 j addr: " << mem2_ptr << "(" << *mem2_ptr
            << ")      func_b in v_table: " << (long *)*func_b_addr
            << std::endl;

  int *mem3_ptr = mem2_ptr + 1;
  long *func_c_addr = f_vt_addr + 2;
  std::cout << "#4 mem3 k addr: " << mem3_ptr << "(" << *mem3_ptr
            << ")      func_c in v_table: " << (long *)*func_c_addr
            << std::endl;

  int *mem4_ptr = mem3_ptr + 1;
  long *func_d_addr = f_vt_addr + 3;
  std::cout << "#5 mem3 m addr: " << mem4_ptr << "(" << *mem4_ptr
            << ")      func_d in v_table: " << (long *)*func_d_addr
            << std::endl;

  // 内存模型：
  // obj begin addr: 0x7ffc240fbd20 (24,24)
  // #1 v_table ptr: 0x7ffc240fbd20     --> first v_table addr: 0x5637d95f5cc8
  // #2 mem1 i addr: 0x7ffc240fbd28(11)      func_a in v_table: 0x5637d95f30bc
  // #3 mem2 j addr: 0x7ffc240fbd2c(12)      func_b in v_table: 0x5637d95f3008
  // #4 mem3 k addr: 0x7ffc240fbd30(21)      func_c in v_table: 0x5637d95f3080
  // #4 mem3 m addr: 0x7ffc240fbd34(31)      func_c in v_table: 0x5637d95f30f8

  FunPtr func_a = (FunPtr) * ((long *)*((long *)&obj + 0) + 0);
  FunPtr func_b = (FunPtr) * ((long *)*((long *)&obj + 0) + 1);
  FunPtr func_c = (FunPtr) * ((long *)*((long *)&obj + 0) + 2);
  FunPtr func_d = (FunPtr) * ((long *)*((long *)&obj + 0) + 3);
  std::cout << (long *)func_a << std::endl;  // 0x5637d95f30bc
  std::cout << (long *)func_b << std::endl;  // 0x5637d95f3008
  std::cout << (long *)func_c << std::endl;  // 0x5637d95f3080
  std::cout << (long *)func_d << std::endl;  // 0x5637d95f30f8
  func_a();                                  // C::func_a
  func_b();                                  // A::func_b
  func_c();                                  // B::func_c
  func_d();                                  // C::func_d

  long **pVtab = (long **)&obj;
  ((FunPtr)pVtab[0][0])();  // C::func_a
  ((FunPtr)pVtab[0][1])();  // A::func_b
  ((FunPtr)pVtab[0][2])();  // B::func_c
  ((FunPtr)pVtab[0][3])();  // C::func_d
}

// 仔细观察虚函数表，可以发现:
// 基类的虚函数在vtable中的索引是固定的，不会随着继承层次的增加而改变，
// 派生类新增的虚函数放在vtable的最后。
// 如果派生类有同名的虚函数覆盖了基类的虚函数，那么将使用派生类的虚函数替换基类的虚函数，
// 这样具有遮蔽关系的虚函数在vtable中只会出现一次。
// 当通过指针调用虚函数时，先根据指针找到vfptr，再根据vfptr找到虚函数的入口地址。
// 以虚函数func_a()为例，它在vtable中的索引为0，通过p调用时：
// p->func_a();
// 编译器会转换为:
// (*(*(p+0)+0))(p);
// 这个表达式的具体分析：
//  0是vfptr在对象中的偏移，p+0是vfptr的地址；
//  *(p+0)是vfptr的值，而vfptr是指向vtable的指针，所以*(p+0)也就是vtable的地址；
//  func_a()在vtable中的索引是0，所以(*(p+0)+0)也就是display()的地址；
//  知道了func_a()的地址，(*(*(p+0)+0))(p)也就是对func_a()的调用了，
//  这里的p就是传递的实参，它会赋值给this指针。
// 转换后的表达式是固定的，只要调用func_a()函数，不管它是哪个类的，都会使用这个表达式。
// 换句话说，编译器不管p指向哪里，一律转换为相同的表达式。
// 转换后的表达式没有用到与p的类型有关的信息，只要知道p的指向就可以调用函数，
// 因为在基类和派生类中，p指向地址所代表的对象，
// 虚函数表中相同偏移量保存的函数的地址不一样，从而实现了多态。

void func4() {
  D obj;

  std::cout << "obj begin addr: " << &obj << " (" << sizeof(D) << ","
            << sizeof(obj) << ")" << std::endl;

  long *vt_ptr = (long *)&obj;
  long *f_vt_addr = (long *)*((long *)&obj + 0);
  std::cout << "#1 v_table ptr: " << vt_ptr
            << "     --> first v_table addr: " << f_vt_addr << std::endl;

  int *mem1_ptr = (int *)((long *)&obj + 1);
  long *func_a_addr = f_vt_addr + 0;
  std::cout << "#2 mem1 i addr: " << mem1_ptr << "(" << *mem1_ptr
            << ")      func_a in v_table: " << (long *)*func_a_addr
            << std::endl;

  int *mem2_ptr = mem1_ptr + 1;
  long *func_b_addr = f_vt_addr + 1;
  std::cout << "#3 mem2 j addr: " << mem2_ptr << "(" << *mem2_ptr
            << ")      func_b in v_table: " << (long *)*func_b_addr
            << std::endl;

  int *mem3_ptr = mem2_ptr + 1;
  long *func_e_addr = f_vt_addr + 2;
  std::cout << "#4 mem3 n addr: " << mem3_ptr << "(" << *mem3_ptr
            << ")      func_e in v_table: " << (long *)*func_e_addr
            << std::endl;

  long *func_f_addr = f_vt_addr + 3;
  std::cout << "#5                                      func_f in v_table: "
            << (long *)*func_f_addr << std::endl;

  // 内存模型：
  // obj begin addr: 0x7fffc16a2760 (24,24)
  // #1 v_table ptr: 0x7fffc16a2760     --> first v_table addr: 0x5610f3d05a58
  // #2 mem1 i addr: 0x7fffc16a2768(11)      func_a in v_table: 0x5610f3d01bf4
  // #3 mem2 j addr: 0x7fffc16a276c(12)      func_b in v_table: 0x5610f3d01a8c
  // #4 mem3 n addr: 0x7fffc16a2770(22)      func_e in v_table: 0x5610f3d01bb8
  // #5                                      func_f in v_table: 0x5610f3d01c30

  // 派生类中新增加的虚函数，排列在虚函数表的末尾，哪怕声明的的顺序比基类的虚函数早，也放最后
  FunPtr func_a = (FunPtr) * ((long *)*((long *)&obj + 0) + 0);
  FunPtr func_b = (FunPtr) * ((long *)*((long *)&obj + 0) + 1);
  FunPtr func_e = (FunPtr) * ((long *)*((long *)&obj + 0) + 2);
  FunPtr func_f = (FunPtr) * ((long *)*((long *)&obj + 0) + 3);
  std::cout << (long *)func_a << std::endl;  // 0x5610f3d01bf4
  std::cout << (long *)func_b << std::endl;  // 0x5610f3d01a8c
  std::cout << (long *)func_e << std::endl;  // 0x5610f3d01bb8
  std::cout << (long *)func_f << std::endl;  // 0x5610f3d01c30
  func_a();                                  // D::func_a
  func_b();                                  // A::func_b
  func_e();                                  // D::func_e
  func_f();                                  // D::func_f

  long **pVtab = (long **)&obj;
  ((FunPtr)pVtab[0][0])();  // D::func_a
  ((FunPtr)pVtab[0][1])();  // A::func_b
  ((FunPtr)pVtab[0][2])();  // D::func_e
  ((FunPtr)pVtab[0][3])();  // D::func_f
}

void testN1() {
  func1();
  func2();
  func3();
  func4();
}
}  // namespace n1

namespace n2 {
// 当存在多继承时，虚函数表的结构就会变得复杂
// 在多继承情况下，有多少个有虚函数的基类就有多少个虚函数表指针。
// 当子类有多出来的虚函数时，添加在第一个虚函数表中，父类指针不能调用。
// 当有多个虚函数表时，虚函数表的结果是0代表没有下一个虚函数表，
// 不同操作系统中代表有下一个虚函数表的标识不同。

namespace test1 {  // 基类无虚函数，派生类也没有虚函数
class BaseA {
 public:
  int a = 1;
};
class BaseB {
 public:
  int b = 2;
};
class Derived : public BaseA, public BaseB {
 public:
  int c = 3;
};
void func() {
  Derived obj;
  int *mem1_addr = (int *)&obj + 0;
  int *mem2_addr = (int *)mem1_addr + 1;
  int *mem3_addr = (int *)mem1_addr + 2;

  std::cout << "obj begin addr: " << &obj << " (" << sizeof(Derived) << ","
            << sizeof(obj) << ")" << std::endl;
  std::cout << "mem1 a: " << mem1_addr << " (" << *mem1_addr << ")"
            << std::endl;
  std::cout << "mem2 b: " << mem2_addr << " (" << *mem2_addr << ")"
            << std::endl;
  std::cout << "mem3 c: " << mem3_addr << " (" << *mem3_addr << ")"
            << std::endl;

  // 内存模型
  // obj begin addr: 0x7ffdde33155c (12,12)
  // mem1 a: 0x7ffdde33155c (1)
  // mem2 b: 0x7ffdde331560 (2)
  // mem3 c: 0x7ffdde331564 (3)

  // 基类成员变量存储顺序与声明顺序有关（从靠近派生类的基类开始，先基类baseA，然后是基类baseB）。
  // 这个顺序与之前总结的规律一致：先基类再派生类，先声明，先存储。
}
}  // namespace test1

namespace test2 {  // 基类均无虚函数，派生类有虚函数
class BaseA {
 public:
  int a = 1;
};
class BaseB {
 public:
  int b = 2;
};
class Derived : public BaseA, public BaseB {
 public:
  virtual void print() { std::cout << "derived::print()" << std::endl; }
  int c = 3;
};
void func() {
  Derived obj;
  long *vt_ptr = (long *)&obj + 0;
  int *mem1_addr = (int *)vt_ptr + 2;
  int *mem2_addr = (int *)mem1_addr + 1;
  int *mem3_addr = (int *)mem1_addr + 2;

  std::cout << "obj begin addr: " << &obj << " (" << sizeof(Derived) << ","
            << sizeof(obj) << ")" << std::endl;
  std::cout << "vp_ptr: " << vt_ptr << std::endl;
  std::cout << "mem1 a: " << mem1_addr << " (" << *mem1_addr << ")"
            << std::endl;
  std::cout << "mem2 b: " << mem2_addr << " (" << *mem2_addr << ")"
            << std::endl;
  std::cout << "mem3 c: " << mem3_addr << " (" << *mem3_addr << ")"
            << std::endl;

  // 内存模型
  // obj begin addr: 0x7ffdde331550 (24,24)
  // vp_ptr: 0x7ffdde331550
  // mem1 a: 0x7ffdde331558 (1)
  // mem2 b: 0x7ffdde33155c (2)
  // mem3 c: 0x7ffdde331560 (3)

  ((FunPtr) * (long *)(*vt_ptr + 0))();  // derived::print()

  // 因为派生类存在虚函数，故排在最前的是虚函数表指针（此时，虚函数表指针属于派生类，而非基类），
  // 接着才是基类成员变量，这里先是基类baseA,然后才是基类baseB,最后才是派生类成员变量。
  // 由于只有派生类存在虚函数，故虚函数表中只有派生类的虚函数地址。
}
}  // namespace test2

namespace test3 {  // 两个基类中只有一个类有虚函数，派生类有虚函数
class BaseA {
 public:
  virtual void show() { std::cout << "virtual BaseA::show()" << std::endl; }
  int a = 1;
};
class BaseB {
 public:
  int b = 2;
};
class Derived1 : public BaseA, public BaseB {
 public:
  virtual void print() { std::cout << "virtual Derived::print()" << std::endl; }
  int c = 3;
};
void func1() {
  Derived1 obj;
  long *vt_ptr = (long *)&obj + 0;
  int *mem1_addr = (int *)vt_ptr + 2;
  int *mem2_addr = (int *)mem1_addr + 1;
  int *mem3_addr = (int *)mem1_addr + 2;

  std::cout << "obj begin addr: " << &obj << " (" << sizeof(Derived1) << ","
            << sizeof(obj) << ")" << std::endl;
  std::cout << "vp_ptr: " << vt_ptr << std::endl;
  std::cout << "mem1 a: " << mem1_addr << " (" << *mem1_addr << ")"
            << std::endl;
  std::cout << "mem2 b: " << mem2_addr << " (" << *mem2_addr << ")"
            << std::endl;
  std::cout << "mem3 c: " << mem3_addr << " (" << *mem3_addr << ")"
            << std::endl;

  // 内存模型
  // obj begin addr: 0x7ffdde331550 (24,24)
  // vp_ptr: 0x7ffdde331550
  // mem1 a: 0x7ffdde331558 (1)
  // mem2 b: 0x7ffdde33155c (2)
  // mem3 c: 0x7ffdde331560 (3)

  ((FunPtr) * (long *)((long *)(*vt_ptr) + 0))();  // virtual BaseA::show()
  ((FunPtr) * (long *)((long *)(*vt_ptr) + 1))();  // virtual Derived::print()

  // 虚函数表也按照先声明先存储的规则，依次为：&baseA::show，&deriveA::print
}
class Derived2 : public BaseB, public BaseA {  // 修改BaseB和BaseA的顺序
 public:
  virtual void print() { std::cout << "virtual Derived::print()" << std::endl; }
  int c = 3;
};
void func2() {
  Derived2 obj;
  long *vt_ptr = (long *)&obj + 0;
  int *mem1_addr = (int *)vt_ptr + 2;
  int *mem2_addr = (int *)mem1_addr + 1;
  int *mem3_addr = (int *)mem1_addr + 2;

  std::cout << "obj begin addr: " << &obj << " (" << sizeof(Derived1) << ","
            << sizeof(obj) << ")" << std::endl;
  std::cout << "vp_ptr: " << vt_ptr << std::endl;
  std::cout << "mem1 a: " << mem1_addr << " (" << *mem1_addr << ")"
            << std::endl;
  std::cout << "mem2 b: " << mem2_addr << " (" << *mem2_addr << ")"
            << std::endl;
  std::cout << "mem3 c: " << mem3_addr << " (" << *mem3_addr << ")"
            << std::endl;

  // 内存模型
  // obj begin addr: 0x7ffdde331550 (24,24)
  // vp_ptr: 0x7ffdde331550
  // mem1 a: 0x7ffdde331558 (1)
  // mem2 b: 0x7ffdde33155c (2)
  // mem3 c: 0x7ffdde331560 (3)

  long **pVtab = (long **)&obj;
  ((FunPtr)pVtab[0][0])();  // virtual BaseA::show()
  ((FunPtr)pVtab[0][1])();  // virtual Derived::print()

  // 虽然改变了继承的顺序，但是内存布局是一样的，存储顺序：先基类，再派生，先声明，先存储。
  // 尽管BaseB更靠近派生类，但是BaseA的优先级更高，因为基类BaseA存在虚函数而基类BaseB不存在。
  // 虚表依然仅按照先声明先存储的顺序存储虚函数：基类优先级>派生类优先级。
}

// 两个基类中只有一个类有虚函数，派生类没有虚函数，内存布局和派生类有虚函数是一样的。
// 交换派生类的顺序，结论也一样，内存布局不变。
}  // namespace test3

namespace test4 {  // 两个基类中都含有虚函数，派生类没有虚函数
class BaseA {
 public:
  virtual void show() { std::cout << "virtual BaseA::show()" << std::endl; }
  int a = 1;
};
class BaseB {
 public:
  virtual void play() { std::cout << "virtual BaseB::play()" << std::endl; }
  int b = 2;
};
class Derived : public BaseA, public BaseB {
 public:
  int c = 3;
};
void func() {
  Derived obj;
  long *vt_ptr1 = (long *)&obj + 0;
  int *mem1_addr = (int *)vt_ptr1 + 2;
  long *vt_ptr2 = (long *)((char *)&obj + sizeof(BaseA));
  int *mem2_addr = (int *)vt_ptr2 + 2;
  int *mem3_addr = (int *)mem2_addr + 1;

  std::cout << "obj begin addr: " << &obj << " (" << sizeof(Derived) << ","
            << sizeof(obj) << ")"
            << "," << alignof(Derived) << std::endl;
  std::cout << "vp_ptr1: " << vt_ptr1 << std::endl;
  std::cout << "mem1 a : " << mem1_addr << " (" << *mem1_addr << ")"
            << std::endl;
  std::cout << "vp_ptr2: " << vt_ptr2 << std::endl;
  std::cout << "mem2 b : " << mem2_addr << " (" << *mem2_addr << ")"
            << std::endl;
  std::cout << "mem3 c : " << mem3_addr << " (" << *mem3_addr << ")"
            << std::endl;

  // 内存模型：
  // obj begin addr: 0x7ffdb1e5c600 (32,32),8
  // vp_ptr1: 0x7ffdb1e5c600
  // mem1 a : 0x7ffdb1e5c608 (1)
  // vp_ptr2: 0x7ffdb1e5c610
  // mem2 b : 0x7ffdb1e5c618 (2)
  // mem3 c : 0x7ffdb1e5c61c (3)

  ((FunPtr) * (long *)((long *)(*vt_ptr1) + 0))();  // virtual BaseA::show()
  ((FunPtr) * (long *)((long *)(*vt_ptr2) + 0))();  // virtual BaseB::play()

  // 多继承，每个基类都有虚函数，所以每个类有一个虚函数表，每张表各自存储自己的虚函数表的信息。
}
}  // namespace test4

namespace test5 {  // 两个基类中都含有虚函数，派生类有虚函数
class BaseA {
 public:
  virtual void show() { std::cout << "virtual BaseA::show()" << std::endl; }
  int a = 1;
};

class BaseB {
 public:
  virtual void play() { std::cout << "virtual BaseB::play()" << std::endl; }
  int b = 2;
};

class Derived : public BaseA, public BaseB {
 public:
  virtual void print() { std::cout << "virtual Derived::print()" << std::endl; }
  int c = 3;
};
void func() {
  Derived obj;
  long *vt_ptr1 = (long *)&obj + 0;
  int *mem1_addr = (int *)vt_ptr1 + 2;
  long *vt_ptr2 = (long *)((char *)&obj + sizeof(BaseA));
  int *mem2_addr = (int *)vt_ptr2 + 2;
  int *mem3_addr = (int *)mem2_addr + 1;

  std::cout << "obj begin addr: " << &obj << " (" << sizeof(Derived) << ","
            << sizeof(obj) << ")"
            << "," << alignof(Derived) << std::endl;
  std::cout << "vp_ptr1: " << vt_ptr1 << std::endl;
  std::cout << "mem1 a : " << mem1_addr << " (" << *mem1_addr << ")"
            << std::endl;
  std::cout << "vp_ptr2: " << vt_ptr2 << std::endl;
  std::cout << "mem2 b : " << mem2_addr << " (" << *mem2_addr << ")"
            << std::endl;
  std::cout << "mem3 c : " << mem3_addr << " (" << *mem3_addr << ")"
            << std::endl;

  // 内存模型：
  // obj begin addr: 0x7ffcfcfb9110 (32,32),8
  // vp_ptr1: 0x7ffcfcfb9110
  // mem1 a : 0x7ffcfcfb9118 (1)
  // vp_ptr2: 0x7ffcfcfb9120
  // mem2 b : 0x7ffcfcfb9128 (2)
  // mem3 c : 0x7ffcfcfb912c (3)

  ((FunPtr) * (long *)((long *)(*vt_ptr1) + 0))();  // virtual BaseA::show()
  ((FunPtr) * (long *)((long *)(*vt_ptr1) + 1))();  // virtual Derived::print()
  ((FunPtr) * (long *)((long *)(*vt_ptr2) + 0))();  // virtual BaseB::play()

  long **pVtab = (long **)&obj;
  ((FunPtr)pVtab[0][0])();  // virtual BaseA::show()
  ((FunPtr)pVtab[0][1])();  // virtual Derived::print()
  pVtab = (long **)((char *)&obj + sizeof(BaseA));
  ((FunPtr)pVtab[0][0])();  // virtual BaseB::play()

  // 多重继承下，子类不再具有自身的虚函数表，它的虚函数表与第一个父类的虚函数表合并了。
}
}  // namespace test5

namespace test6 {  // 两个基类中都含有虚函数，派生类有虚函数并重写基类函数
class BaseA {
 public:
  virtual void show() { std::cout << "virtual BaseA::show()" << std::endl; }
  int a = 1;
};

class BaseB {
 public:
  virtual void play() { std::cout << "virtual BaseB::play()" << std::endl; }
  int b = 2;
};

class Derived : public BaseA, public BaseB {
 public:
  virtual void print() { std::cout << "virtual Derived::print()" << std::endl; }
  virtual void show() { std::cout << "virtual Derived::show()" << std::endl; }
  virtual void play() { std::cout << "virtual Derived::play()" << std::endl; }
  int c = 3;
};
void func() {
  Derived obj;
  long *vt_ptr1 = (long *)&obj + 0;
  int *mem1_addr = (int *)vt_ptr1 + 2;
  long *vt_ptr2 = (long *)((char *)&obj + sizeof(BaseA));
  int *mem2_addr = (int *)vt_ptr2 + 2;
  int *mem3_addr = (int *)mem2_addr + 1;

  std::cout << "obj begin addr: " << &obj << " (" << sizeof(Derived) << ","
            << sizeof(obj) << ")"
            << "," << alignof(Derived) << std::endl;
  std::cout << "vp_ptr1: " << vt_ptr1 << std::endl;
  std::cout << "mem1 a : " << mem1_addr << " (" << *mem1_addr << ")"
            << std::endl;
  std::cout << "vp_ptr2: " << vt_ptr2 << std::endl;
  std::cout << "mem2 b : " << mem2_addr << " (" << *mem2_addr << ")"
            << std::endl;
  std::cout << "mem3 c : " << mem3_addr << " (" << *mem3_addr << ")"
            << std::endl;

  // 内存模型：
  // obj begin addr: 0x7ffe153c3f20 (32,32),8
  // vp_ptr1: 0x7ffe153c3f20
  // mem1 a : 0x7ffe153c3f28 (1)
  // vp_ptr2: 0x7ffe153c3f30
  // mem2 b : 0x7ffe153c3f38 (2)
  // mem3 c : 0x7ffe153c3f3c (3)

  ((FunPtr) * (long *)((long *)(*vt_ptr1) + 0))();  // virtual Derived::show()
  ((FunPtr) * (long *)((long *)(*vt_ptr1) + 1))();  // virtual Derived::print()
  ((FunPtr) * (long *)((long *)(*vt_ptr1) + 2))();  // virtual Derived::play()
  ((FunPtr) * (long *)((long *)(*vt_ptr2) + 0))();  // virtual Derived::play()

  long **pVtab = (long **)&obj;
  ((FunPtr)pVtab[0][0])();  // virtual Derived::show()
  ((FunPtr)pVtab[0][1])();  // virtual Derived::print()
  ((FunPtr)pVtab[0][2])();  // virtual Derived::play()
  pVtab = (long **)((char *)&obj + sizeof(BaseA));
  ((FunPtr)pVtab[0][0])();  // virtual Derived::play()

  // 如果子类重写了任意父类的虚函数，都会覆盖对应的函数地址记录。
}
}  // namespace test6

void testN2() {
  test1::func();
  test2::func();
  test3::func1();
  test3::func2();
  test4::func();
  test5::func();
  test6::func();
}
}  // namespace n2

namespace n3 {
// 如果说没有虚函数的虚继承只是一个噩梦的话，那么既有虚函数又有虚继承就是真正的炼狱。
// 总结：
// 1.如有虚函数，则内存中加入vptr指针，放在内存中最高位置，
//   其余应继承的成员变量按就近原则在内存中从近到远分布；
// 2.如有虚继承，则在内存汇总加入vbptr指针，指向虚表，表中是距离基类成员变量的偏移量，
//   指针放在vptr（如有）的下面，如有重复拷贝则只继承一份成员变量，放到对象最后面。
class Base {
  int var = 100;

 public:
  virtual void fun() { std::cout << " Base::fun" << std::endl; }
};
class A : public Base {
  int a = 101;

 public:
  virtual void fun() { std::cout << " A::fun" << std::endl; }
  virtual void funA() { std::cout << " A::funA" << std::endl; }
};
class B : virtual public Base {
  int b = 102;

 public:
  virtual void fun() { std::cout << " B::fun" << std::endl; }
  virtual void funB() { std::cout << " B::funB" << std::endl; }
};
class C : public A, public B {
  int c = 104;

 public:
  virtual void funB() { std::cout << " C::funB" << std::endl; }
  virtual void funC() { std::cout << " C::funC" << std::endl; }
};
void testN3() {
  C obj;

  std::cout << "obj addr   : " << &obj << ", size :" << sizeof(C) << std::endl;
  long *vfptr1 = (long *)&obj;
  std::cout << "#1 vfptr1  : " << vfptr1 << std::endl;
  ((FunPtr) * (long *)((long *)*vfptr1 + 0))();
  ((FunPtr) * (long *)((long *)*vfptr1 + 1))();
  ((FunPtr) * (long *)((long *)*vfptr1 + 2))();
  ((FunPtr) * (long *)((long *)*vfptr1 + 3))();
  std::cout << " " << *(long *)((long *)*vfptr1 + 4) << std::endl;
  std::cout << " " << *(long *)((long *)*vfptr1 + 5) << std::endl;

  int *mem1 = (int *)&obj + 2;
  std::cout << "#2 mem1 var: " << mem1 << "," << *mem1 << std::endl;
  int *mem2 = (int *)&obj + 3;
  std::cout << "#3 mem2  a : " << mem2 << "," << *mem2 << std::endl;

  long *vfptr2 = (long *)(mem2 + 1);
  std::cout << "#4 vfptr2  : " << vfptr2 << std::endl;
  ((FunPtr) * (long *)((long *)*vfptr2 + 0))();
  ((FunPtr) * (long *)((long *)*vfptr2 + 1))();
  std::cout << " " << *(long *)((long *)*vfptr2 + 2) << std::endl;
  std::cout << " " << *(long *)((long *)*vfptr2 + 3) << std::endl;

  int *mem3 = (int *)&obj + 6;
  std::cout << "#5 mem3  b : " << mem3 << "," << *mem3 << std::endl;
  int *mem4 = (int *)&obj + 7;
  std::cout << "#6 mem4  c : " << mem4 << "," << *mem4 << std::endl;

  // obj addr   : 0x7ffc57da5580, size :48
  // #1 vfptr1  : 0x7ffc57da5580
  //  A::fun
  //  A::funA
  //  C::funB
  //  C::funC
  //  16
  //  -16
  // #2 mem1 var: 0x7ffc57da5588,100
  // #3 mem2  a : 0x7ffc57da558c,101
  // #4 vfptr2  : 0x7ffc57da5590
  //  B::fun
  //  C::funB
  //  -16
  //  -32
  // #5 mem3  b : 0x7ffc57da5598,102
  // #6 mem4  c : 0x7ffc57da559c,104

  // 才32个字节，剩下的16个字节用到哪里了？
}
}  // namespace n3

namespace n4 {
class Base {
  int var = 100;

 public:
  virtual void fun() { std::cout << " Base::fun" << std::endl; }
};
class A : virtual public Base {
  int a = 101;

 public:
  // virtual void fun() { std::cout << " A::fun" << std::endl; }
  virtual void funA() { std::cout << " A::funA" << std::endl; }
};
class B : virtual public Base {
  int b = 102;

 public:
  // virtual void fun() { std::cout << " B::fun" << std::endl; }
  virtual void funB() { std::cout << " B::funB" << std::endl; }
};
class C : public A, public B {
  int c = 103;

 public:
  virtual void fun() { std::cout << " C::fun" << std::endl; }
  virtual void funB() { std::cout << " C::funB" << std::endl; }
  virtual void funC() { std::cout << " C::funC" << std::endl; }
  virtual void funD() { std::cout << " C::funD" << std::endl; }
};
void testN4() {
  C obj;

  std::cout << "obj addr   : " << &obj << ", size :" << sizeof(C) << std::endl;
  long *vfptr1 = (long *)&obj;
  std::cout << "#1 vfptr1  : " << vfptr1 << std::endl;
  ((FunPtr) * (long *)((long *)*vfptr1 + 0))();
  ((FunPtr) * (long *)((long *)*vfptr1 + 1))();
  ((FunPtr) * (long *)((long *)*vfptr1 + 2))();
  ((FunPtr) * (long *)((long *)*vfptr1 + 3))();
  ((FunPtr) * (long *)((long *)*vfptr1 + 4))();
  std::cout << " " << *(long *)((long *)*vfptr1 + 5) << std::endl;
  std::cout << " " << *(long *)((long *)*vfptr1 + 6) << std::endl;

  int *mem1 = (int *)&obj + 2;
  std::cout << "#2 mem1  a : " << mem1 << "," << *mem1 << std::endl;

  long *vfptr2 = (long *)(mem1 + 2);
  std::cout << "#4 vfptr2  : " << vfptr2 << std::endl;
  ((FunPtr) * (long *)((long *)*vfptr2 + 0))();
  std::cout << " " << *(long *)((long *)*vfptr2 + 1) << std::endl;
  std::cout << " " << *(long *)((long *)*vfptr2 + 2) << std::endl;

  // B重写来fun
  // ((FunPtr) * (long *)((long *)*vfptr2 + 0))();
  // ((FunPtr) * (long *)((long *)*vfptr2 + 1))();
  // std::cout << " " << *(long *)((long *)*vfptr2 + 2) << std::endl;
  // std::cout << " " << *(long *)((long *)*vfptr2 + 3) << std::endl;

  int *mem2 = (int *)&obj + 6;
  std::cout << "#5 mem2  b : " << mem2 << "," << *mem2 << std::endl;
  int *mem3 = (int *)&obj + 7;
  std::cout << "#6 mem3  c : " << mem3 << "," << *mem3 << std::endl;
  int *mem4 = (int *)&obj + 10;  // 共享的元素，放到最后。为什么空了好多位置？
  std::cout << "#7 mem4 var: " << mem4 << "," << *mem4 << std::endl;

  // obj addr   : 0x7ffcfaa5d0e0, size :48
  // #1 vfptr1  : 0x7ffcfaa5d0e0
  //  A::funA
  //  C::fun
  //  C::funB
  //  C::funC
  //  C::funD
  //  16
  //  -16
  // #2 mem1  a : 0x7ffcfaa5d0e8,101
  // #4 vfptr2  : 0x7ffcfaa5d0f0
  //  C::funB
  //  -32
  //  -32
  // #5 mem2  b : 0x7ffcfaa5d0f8,102
  // #6 mem3  c : 0x7ffcfaa5d0fc,103
  // #7 mem4 var: 0x7ffcfaa5d108,100

  // 如果A重写了fun：
  // #1 vfptr1  : 0x7ffc5c3c8ba0
  //  C::fun
  //  A::funA
  //  C::funB
  //  C::funC
  //  C::funD

  //  如果B重写来func：
  // #1 vfptr1  : 0x7fffebdf2340
  //  A::funA
  //  C::fun
  //  C::funB
  //  C::funC
  //  C::funD
  // #4 vfptr2  : 0x7fffebdf2350
  //  C::fun
  //  C::funB
}
}  // namespace n4

int main(int argc, char *argv[]) {
  if (argc < 2) {
    std::cout << argv[0] << " i [0 - 3]" << std::endl;
    return 0;
  }
  int type = argv[1][0] - '0';
  switch (type) {
    case 0:
      n1::testN1();
      break;
    case 1:
      n2::testN2();
      break;
    case 2:
      n3::testN3();
      break;
    case 3:
      n4::testN4();
      break;
    default:
      std::cout << "invalid type" << std::endl;
      break;
  }

  return 0;
}
