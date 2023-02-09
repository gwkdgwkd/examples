#include <iostream>
#include <string>
#include <typeinfo>

// 当通过指针访问类的成员函数时：
// 1.如果该函数是非虚函数，那么编译器会根据指针的类型找到该函数；
//   也就是说，指针是哪个类的类型就调用哪个类的函数。
// 2.如果该函数是虚函数，并且派生类有同名的函数遮蔽它，
//   那么编译器会根据指针的指向找到该函数；
//   也就是说，指针指向的对象属于哪个类就调用哪个类的函数，这就是多态。
// 编译器之所以能通过指针指向的对象找到虚函数，
// 是因为在创建对象时额外地增加了虚函数表。
// 如果一个类包含了虚函数，那么在创建该类的对象时就会额外地增加一个数组，
// 数组中的每一个元素都是虚函数的入口地址。
// 不过数组和对象是分开存储的，为了将对象和数组关联起来，
// 编译器还要在对象中增加一个指针，指向数组的起始位置。
// 这里的数组就是虚函数表（virtual function table），简写为vtable。

// 查看内存布局：
// g++-9 -c -fdump-lang-class polymorphism.cpp

using FunPtr = std::string (*)(void);

using VFPTR = void (*)(void);  // typedef void (*VFPTR)();
// void PrintVTable(VFPTR vTable[]) {
//   std::cout << "虚表地址：" << vTable << std::endl;
//   for (int i = 0; vTable[i] != nullptr; ++i) {
//     // 多个虚函数表时，判断第一个表结束的标识有问题，怎么判断，不是空么？
//     std::cout << "第" << i + 1 << "个虚函数地址" << (long *)vTable[i]
//               << std::endl;
//     VFPTR f = vTable[i];
//     f();
//   }
//   std::cout << std::endl;
// }

namespace n1 {
// 存在虚函数时，单继承的内存模型
class A {
 public:
  virtual std::string func_a() { return "A::func_a"; }

 protected:
  virtual std::string func_b() { return "A::func_b"; }
  int i = 11;
  int j = 12;
};
void print(A &obj, int fn, int mn, int size) {
  // 64位计算机，计算虚函数相关地址时，使用long *
  // 32位计算机，计算虚函数相关地址时，使用int *

  std::cout << "obj addr  : " << &obj << " (" << size << ")" << std::endl;

  long *vptr = (long *)&obj;  // 虚函数表的指针放到对象的开头，与对象地址相同
  long *vtbl = (long *)*vptr;  // 虚函数表的指针指向虚函数表
  std::cout << "#1 vptr   : " << vptr << " --> " << vtbl << std::endl;

  // 在真正的虚函数表中：
  // 1.第一项为0：
  void *ptr = (void *)*((long *)*(vptr + 0) - 2);
  std::cout << "  slot -2 : " << *((long *)*(vptr + 0) - 2) << std::endl;

  // 2.第二项为type_info指针，指向type_info对象：
  const std::type_info *ti = (const std::type_info *)*((long *)*(vptr + 0) - 1);
  std::cout << "  slot -1 : name " << ti->name() << "   --> " << (long *)ti
            << std::endl;

  // 3.第三项开始为虚函数所在地址，vptr指向的不是虚函数的第一项，而是这里：
  for (int i = 0; i < fn; ++i) {
    // 虚函数的地址 = 虚函数表的地址 + 偏移量
    FunPtr func = (FunPtr) * ((long *)*(vptr + 0) + i);
    std::cout << "  slot  " << i << " : " << func() << "      --> "
              << (long *)func << std::endl;
  }

  // 也可以用下面的方法：
  // long **pVtab = (long **)&obj;
  // std::cout << ((FunPtr)pVtab[0][0])()<< std::endl;  // A::func_a
  // std::cout << ((FunPtr)pVtab[0][1])()<< std::endl;  // A::func_a

  // 第一个成员变量的地址 = 对象的地址 + 虚函数表的指针大小
  int *mem_base = (int *)(vptr) + 2;
  for (int i = 0; i < mn; ++i) {
    int *mem_ptr = mem_base + i;
    std::cout << "#" << i + 2 << " mem  " << i + 1 << " : " << mem_ptr
              << " --> " << *mem_ptr << std::endl;
  }
}
void func1() {
  A a;
  print(a, 2, 2, sizeof(a));
  // obj addr  : 0x7ffcc1d69fc0 (16)
  // #1 vptr   : 0x7ffcc1d69fc0 --> 0x5576850a2ac8
  //   slot -2 : 0
  //   slot -1 : name N2n11AE   --> 0x5576850a2ce0
  //   slot  0 : A::func_a      --> 0x55768509e8a8
  //   slot  1 : A::func_b      --> 0x55768509e93a
  // #2 mem  1 : 0x7ffcc1d69fc8 --> 11
  // #3 mem  2 : 0x7ffcc1d69fcc --> 12

  // Vtable for n1::A
  // n1::A::_ZTVN2n11AE: 4 entries
  // 0     (int (*)(...))0
  // 8     (int (*)(...))(& _ZTIN2n11AE)
  // 16    (int (*)(...))n1::A::func_a
  // 24    (int (*)(...))n1::A::func_b
}

class B : public A {
 public:
  virtual std::string func_a() { return "B::func_a"; }

 protected:
  int k = 21;

 private:
  virtual std::string func_c() { return "B::func_c"; }
};
void func2() {
  B b;
  print(b, 3, 3, sizeof(B));
  // obj addr  : 0x7ffd806c1560 (24)
  // #1 vptr   : 0x7ffd806c1560 --> 0x55aac57c2aa0
  //   slot -2 : 0
  //   slot -1 : name N2n11BE   --> 0x55aac57c2cc8
  //   slot  0 : B::func_a      --> 0x55aac57be9cc
  //   slot  1 : A::func_b      --> 0x55aac57be93a
  //   slot  2 : B::func_c      --> 0x55aac57bea5e
  // #2 mem  1 : 0x7ffd806c1568 --> 11
  // #3 mem  2 : 0x7ffd806c156c --> 12
  // #4 mem  3 : 0x7ffd806c1570 --> 21

  // Vtable for n1::B
  // n1::B::_ZTVN2n11BE: 5 entries
  // 0     (int (*)(...))0
  // 8     (int (*)(...))(& _ZTIN2n11BE)
  // 16    (int (*)(...))n1::B::func_a
  // 24    (int (*)(...))n1::A::func_b
  // 32    (int (*)(...))n1::B::func_c
}

class C : public B {
 public:
  virtual std::string func_a() { return "C::func_a"; }
  virtual std::string func_d() { return "C::func_d"; }

 protected:
  int m = 31;
};
void func3() {
  C c;
  print(c, 4, 4, sizeof(c));
  // obj addr  : 0x7ffe0be876a0 (24)
  // #1 vptr   : 0x7ffe0be876a0 --> 0x557418579a70
  //   slot -2 : 0
  //   slot -1 : name N2n11CE   --> 0x557418579cb0
  //   slot  0 : C::func_a      --> 0x557418575af0
  //   slot  1 : A::func_b      --> 0x55741857593a
  //   slot  2 : B::func_c      --> 0x557418575a5e
  //   slot  3 : C::func_d      --> 0x557418575b82
  // #2 mem  1 : 0x7ffe0be876a8 --> 11
  // #3 mem  2 : 0x7ffe0be876ac --> 12
  // #4 mem  3 : 0x7ffe0be876b0 --> 21
  // #5 mem  4 : 0x7ffe0be876b4 --> 31

  // Vtable for n1::C
  // n1::C::_ZTVN2n11CE: 6 entries
  // 0     (int (*)(...))0
  // 8     (int (*)(...))(& _ZTIN2n11CE)
  // 16    (int (*)(...))n1::C::func_a
  // 24    (int (*)(...))n1::A::func_b
  // 32    (int (*)(...))n1::B::func_c
  // 40    (int (*)(...))n1::C::func_d
}

class D : public A {
 public:
  // 派生类中新增加的虚函数，排列在虚函数表的末尾，
  // 哪怕声明的的顺序比基类的虚函数早，也放最后：
  virtual std::string func_e() { return "D::func_e"; }
  virtual std::string func_a() { return "D::func_a"; }
  virtual std::string func_f() { return "D::func_f"; }

 protected:
  int n = 22;
};
void func4() {
  D d;
  print(d, 4, 3, sizeof(d));
  // obj addr  : 0x7ffe1ed02b90 (24)
  // #1 vptr   : 0x7ffe1ed02b90 --> 0x55d1d1d54a40
  //   slot -2 : 0
  //   slot -1 : name N2n11DE   --> 0x55d1d1d54c98
  //   slot  0 : D::func_a      --> 0x55d1d1d50ca6
  //   slot  1 : A::func_b      --> 0x55d1d1d5093a
  //   slot  2 : D::func_e      --> 0x55d1d1d50c14
  //   slot  3 : D::func_f      --> 0x55d1d1d50d38
  // #2 mem  1 : 0x7ffe1ed02b98 --> 11
  // #3 mem  2 : 0x7ffe1ed02b9c --> 12
  // #4 mem  3 : 0x7ffe1ed02ba0 --> 22

  // Vtable for n1::D
  // n1::D::_ZTVN2n11DE: 6 entries
  // 0     (int (*)(...))0
  // 8     (int (*)(...))(& _ZTIN2n11DE)
  // 16    (int (*)(...))n1::D::func_a
  // 24    (int (*)(...))n1::A::func_b
  // 32    (int (*)(...))n1::D::func_e
  // 40    (int (*)(...))n1::D::func_f
}

// 基类的虚函数在vtable中的索引是固定的，不会随着继承层次的增加而改变，
// 派生类新增的虚函数放在vtable的最后。
// 如果派生类有同名的虚函数覆盖了基类的虚函数，
// 那么将使用派生类的虚函数替换基类的虚函数，
// 这样具有遮蔽关系的虚函数在vtable中只会出现一次。
// 当通过指针调用虚函数时，先由指针找到vfptr，再由vfptr找到虚函数的入口地址。
// 以虚函数func_a()为例，它在vtable中的索引为0，通过p调用时：
// p->func_a();
// 编译器会转换为：
// (*(*(p+0)+0))(p);
// 这个表达式的具体分析：
// 1.0是vfptr在对象中的偏移，p+0是vfptr的地址；
// 2.*(p+0)是vfptr的值，而vfptr是指向vtable的指针，
//   所以*(p+0)也就是vtable的地址；
// 3.func_a()在vtable中的索引是0，所以(*(p+0)+0)也就是display()的地址；
// 4.知道了func_a()的地址，(*(*(p+0)+0))(p)也就是对func_a()的调用了，
//   这里的p就是传递的实参，它会赋值给this指针。
// 转换后的表达式是固定的，只要调用func_a()，不管它是哪个类都会使用这个表达式。
// 换句话说，编译器不管p指向哪里，一律转换为相同的表达式。
// 转换后的表达式没有用到与p的类型有关的信息，只要知道p的指向就可以调用函数，
// 因为在基类和派生类中，p指向地址所代表的对象，
// 虚函数表中相同偏移量保存的函数的地址不一样，从而实现了多态。
}  // namespace n1

namespace n2 {
// 当存在多继承时，虚函数表的结构就会变得复杂。
// 在多继承情况下，有多少个有虚函数的基类就有多少个虚函数表指针。
// 当子类有多出来的虚函数时，添加在第一个虚函数表中，父类指针不能调用。
// 当有多个虚函数表时，虚函数表的结果是0代表没有下一个虚函数表，
// 不同操作系统中代表有下一个虚函数表的标识不同。

namespace test1 {
// 基类无虚函数，派生类也没有虚函数：
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

  // 基类成员变量存储顺序与声明顺序有关，
  // 从靠近派生类的基类开始，先基类baseA，然后是基类baseB。
  // 规律：先基类再派生类，先声明，先存储。
}
}  // namespace test1

namespace test2 {
// 基类均无虚函数，派生类有虚函数：
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
  virtual std::string print() { return "Derived::print()"; }
  int c = 3;
};
void func() {
  Derived obj;
  long *vt_ptr = (long *)&obj + 0;
  int *mem1_addr = (int *)vt_ptr + 2;
  int *mem2_addr = (int *)mem1_addr + 1;
  int *mem3_addr = (int *)mem1_addr + 2;
  long **pVtab = (long **)&obj;

  std::cout << "obj begin addr: " << &obj << " (" << sizeof(Derived) << ","
            << sizeof(obj) << ")" << std::endl;
  std::cout << "vp_ptr: " << vt_ptr << std::endl;
  std::cout << "  slot -2: " << *((long *)(pVtab[0] - 2)) << std::endl;
  std::cout << "  slot -1: "
            << ((const std::type_info *)*(pVtab[0] - 1))->name() << std::endl;
  std::cout << "  slot  0: " << ((FunPtr)(pVtab[0][0]))() << std::endl;
  std::cout << "mem1 a: " << mem1_addr << " (" << *mem1_addr << ")"
            << std::endl;
  std::cout << "mem2 b: " << mem2_addr << " (" << *mem2_addr << ")"
            << std::endl;
  std::cout << "mem3 c: " << mem3_addr << " (" << *mem3_addr << ")"
            << std::endl;

  // obj begin addr: 0x7ffdb5726650 (24,24)
  // vp_ptr: 0x7ffdb5726650
  //   slot -2: 0
  //   slot -1: N2n25test27DerivedE
  //   slot  0: Derived::print()
  // mem1 a: 0x7ffdb5726658 (1)
  // mem2 b: 0x7ffdb572665c (2)
  // mem3 c: 0x7ffdb5726660 (3)

  // Vtable for n2::test2::Derived
  // n2::test2::Derived::_ZTVN2n25test27DerivedE: 3 entries
  // 0     (int (*)(...))0
  // 8     (int (*)(...))(& _ZTIN2n25test27DerivedE)
  // 16    (int (*)(...))n2::test2::Derived::print

  // 因为派生类存在虚函数，故排在最前的是虚函数表指针，
  // 此时，虚函数表指针属于派生类，而非基类，接着才是基类成员变量，
  // 这里先是基类baseA,然后才是基类baseB,最后才是派生类成员变量。
  // 由于只有派生类存在虚函数，故虚函数表中只有派生类的虚函数地址。
}
}  // namespace test2

namespace test3 {
// 两个基类中只有一个类有虚函数，派生类有虚函数：
class BaseA {
 public:
  virtual std::string show() { return "virtual BaseA::show()"; }
  int a = 1;
};
class BaseB {
 public:
  int b = 2;
};
class Derived1 : public BaseA, public BaseB {
 public:
  virtual std::string print() { return "virtual Derived1::print()"; }
  int c = 3;
};
void func1() {
  Derived1 obj;
  long *vt_ptr = (long *)&obj + 0;
  int *mem1_addr = (int *)vt_ptr + 2;
  int *mem2_addr = (int *)mem1_addr + 1;
  int *mem3_addr = (int *)mem1_addr + 2;
  long **pVtab = (long **)&obj;

  std::cout << "obj begin addr: " << &obj << " (" << sizeof(Derived1) << ","
            << sizeof(obj) << ")" << std::endl;
  std::cout << "vp_ptr: " << vt_ptr << std::endl;
  std::cout << "  slot -2: " << *((long *)(pVtab[0] - 2)) << std::endl;
  std::cout << "  slot -1: "
            << ((const std::type_info *)*(pVtab[0] - 1))->name() << std::endl;
  std::cout << "  slot  0: " << ((FunPtr)(pVtab[0][0]))() << std::endl;
  std::cout << "  slot  1: " << ((FunPtr)(pVtab[0][1]))() << std::endl;
  std::cout << "mem1 a: " << mem1_addr << " (" << *mem1_addr << ")"
            << std::endl;
  std::cout << "mem2 b: " << mem2_addr << " (" << *mem2_addr << ")"
            << std::endl;
  std::cout << "mem3 c: " << mem3_addr << " (" << *mem3_addr << ")"
            << std::endl;

  // obj begin addr: 0x7fff7efc4ca0 (24,24)
  // vp_ptr: 0x7fff7efc4ca0
  //   slot -2: 0
  //   slot -1: N2n25test38Derived1E
  //   slot  0: virtual BaseA::show()
  //   slot  1: virtual Derived1::print()
  // mem1 a: 0x7fff7efc4ca8 (1)
  // mem2 b: 0x7fff7efc4cac (2)
  // mem3 c: 0x7fff7efc4cb0 (3)

  // Vtable for n2::test3::Derived1
  // n2::test3::Derived1::_ZTVN2n25test38Derived1E: 4 entries
  // 0     (int (*)(...))0
  // 8     (int (*)(...))(& _ZTIN2n25test38Derived1E)
  // 16    (int (*)(...))n2::test3::BaseA::show
  // 24    (int (*)(...))n2::test3::Derived1::print

  // 虚函数表也按照先声明先存储的规则，依次为：
  // &baseA::show，&deriveA::print
}
class Derived2 : public BaseB, public BaseA {  // 修改BaseB和BaseA的顺序
 public:
  virtual std::string print() { return "virtual Derived2::print()"; }
  int c = 3;
};
void func2() {
  Derived2 obj;
  long *vt_ptr = (long *)&obj + 0;
  int *mem1_addr = (int *)vt_ptr + 2;
  int *mem2_addr = (int *)mem1_addr + 1;
  int *mem3_addr = (int *)mem1_addr + 2;
  long **pVtab = (long **)&obj;

  std::cout << "obj begin addr: " << &obj << " (" << sizeof(Derived1) << ","
            << sizeof(obj) << ")" << std::endl;
  std::cout << "vp_ptr: " << vt_ptr << std::endl;
  std::cout << "  slot -2: " << *((long *)(pVtab[0] - 2)) << std::endl;
  std::cout << "  slot -1: "
            << ((const std::type_info *)*(pVtab[0] - 1))->name() << std::endl;
  std::cout << "  slot  0: " << ((FunPtr)(pVtab[0][0]))() << std::endl;
  std::cout << "  slot  1: " << ((FunPtr)(pVtab[0][1]))() << std::endl;
  std::cout << "mem1 a: " << mem1_addr << " (" << *mem1_addr << ")"
            << std::endl;
  std::cout << "mem2 b: " << mem2_addr << " (" << *mem2_addr << ")"
            << std::endl;
  std::cout << "mem3 c: " << mem3_addr << " (" << *mem3_addr << ")"
            << std::endl;

  // obj begin addr: 0x7ffeb4decbc0 (24,24)
  // vp_ptr: 0x7ffeb4decbc0
  //   slot -2: 0
  //   slot -1: N2n25test38Derived2E
  //   slot  0: virtual BaseA::show()
  //   slot  1: virtual Derived2::print()
  // mem1 a: 0x7ffeb4decbc8 (1)
  // mem2 b: 0x7ffeb4decbcc (2)
  // mem3 c: 0x7ffeb4decbd0 (3)

  // Vtable for n2::test3::Derived2
  // n2::test3::Derived2::_ZTVN2n25test38Derived2E: 4 entries
  // 0     (int (*)(...))0
  // 8     (int (*)(...))(& _ZTIN2n25test38Derived2E)
  // 16    (int (*)(...))n2::test3::BaseA::show
  // 24    (int (*)(...))n2::test3::Derived2::print

  // 虽然改变了继承的顺序，但是内存布局是一样的，
  // 存储顺序：先基类，再派生，先声明，先存储。
  // 尽管BaseB更靠近派生类，但是BaseA的优先级更高，
  // 因为基类BaseA存在虚函数而基类BaseB不存在。
  // 虚表依然仅按照先声明先存储的顺序存储虚函数：基类优先级>派生类优先级。
}

// 两个基类只有一个有虚函数，派生类没有虚函数，内存布局和派生类有虚函数是一样的。
// 交换派生类的顺序，结论也一样，内存布局不变。
}  // namespace test3

namespace test4 {
// 两个基类中都含有虚函数，派生类没有虚函数：
class BaseA {
 public:
  virtual std::string show() { return "virtual BaseA::show()"; }
  int a = 1;
};
class BaseB {
 public:
  virtual std::string play() { return "virtual BaseB::play()"; }
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
  long **pVtab1 = (long **)&obj;
  long **pVtab2 = (long **)((char *)&obj + sizeof(BaseA));

  std::cout << "obj begin addr: " << &obj << " (" << sizeof(Derived) << ","
            << sizeof(obj) << ")"
            << "," << alignof(Derived) << std::endl;
  std::cout << "vp_ptr1: " << vt_ptr1 << std::endl;
  std::cout << "  slot -2: " << *((long *)(pVtab1[0] - 2)) << std::endl;
  std::cout << "  slot -1: "
            << ((const std::type_info *)*(pVtab1[0] - 1))->name() << std::endl;
  std::cout << "  slot  0: " << ((FunPtr)(pVtab1[0][0]))() << std::endl;
  std::cout << "mem1 a : " << mem1_addr << " (" << *mem1_addr << ")"
            << std::endl;
  std::cout << "vp_ptr2: " << vt_ptr2 << std::endl;
  std::cout << "  slot -2: " << *((long *)(pVtab2[0] - 2)) << std::endl;
  std::cout << "  slot -1: "
            << ((const std::type_info *)*(pVtab2[0] - 1))->name() << std::endl;
  std::cout << "  slot  0: " << ((FunPtr)(pVtab2[0][0]))() << std::endl;
  std::cout << "mem2 b : " << mem2_addr << " (" << *mem2_addr << ")"
            << std::endl;
  std::cout << "mem3 c : " << mem3_addr << " (" << *mem3_addr << ")"
            << std::endl;

  // obj begin addr: 0x7ffe78405a60 (32,32),8
  // vp_ptr1: 0x7ffe78405a60
  //   slot -2: 0
  //   slot -1: N2n25test47DerivedE
  //   slot  0: virtual BaseA::show()
  // mem1 a : 0x7ffe78405a68 (1)
  // vp_ptr2: 0x7ffe78405a70
  //   slot -2: -16
  //   slot -1: N2n25test47DerivedE
  //   slot  0: virtual BaseB::play()
  // mem2 b : 0x7ffe78405a78 (2)
  // mem3 c : 0x7ffe78405a7c (3)

  // Vtable for n2::test4::Derived
  // n2::test4::Derived::_ZTVN2n25test47DerivedE: 6 entries
  // 0     (int (*)(...))0
  // 8     (int (*)(...))(& _ZTIN2n25test47DerivedE)
  // 16    (int (*)(...))n2::test4::BaseA::show
  // 24    (int (*)(...))-16
  // 32    (int (*)(...))(& _ZTIN2n25test47DerivedE)
  // 40    (int (*)(...))n2::test4::BaseB::play

  // 多继承，每个基类都有虚函数，所以每个类有一个虚函数表，
  // 每张表各自存储自己的虚函数表的信息。
}
}  // namespace test4

namespace test5 {
// 两个基类中都含有虚函数，派生类有虚函数：
class BaseA {
 public:
  virtual std::string show() { return "virtual BaseA::show()"; }
  int a = 1;
};

class BaseB {
 public:
  virtual std::string play() { return "virtual BaseB::play()"; }
  int b = 2;
};

class Derived : public BaseA, public BaseB {
 public:
  virtual std::string print() { return "virtual Derived::print()"; }
  int c = 3;
};
void func() {
  Derived obj;
  long *vt_ptr1 = (long *)&obj + 0;
  int *mem1_addr = (int *)vt_ptr1 + 2;
  long *vt_ptr2 = (long *)((char *)&obj + sizeof(BaseA));
  int *mem2_addr = (int *)vt_ptr2 + 2;
  int *mem3_addr = (int *)mem2_addr + 1;
  long **pVtab1 = (long **)&obj;
  long **pVtab2 = (long **)((char *)&obj + sizeof(BaseA));

  std::cout << "obj begin addr: " << &obj << " (" << sizeof(Derived) << ","
            << sizeof(obj) << ")"
            << "," << alignof(Derived) << ", BaseA:" << sizeof(BaseA)
            << std::endl;
  std::cout << "vp_ptr1: " << vt_ptr1 << std::endl;
  std::cout << "  slot -2: " << *((long *)(pVtab1[0] - 2)) << std::endl;
  std::cout << "  slot -1: "
            << ((const std::type_info *)*(pVtab1[0] - 1))->name() << std::endl;
  std::cout << "  slot  0: " << ((FunPtr)(pVtab1[0][0]))() << std::endl;
  std::cout << "  slot  1: " << ((FunPtr)(pVtab1[0][1]))() << std::endl;
  std::cout << "mem1 a : " << mem1_addr << " (" << *mem1_addr << ")"
            << std::endl;
  std::cout << "vp_ptr2: " << vt_ptr2 << std::endl;
  std::cout << "  slot -2: " << *((long *)(pVtab2[0] - 2)) << std::endl;
  std::cout << "  slot -1: "
            << ((const std::type_info *)*(pVtab2[0] - 1))->name() << std::endl;
  std::cout << "  slot  0: " << ((FunPtr)(pVtab2[0][0]))() << std::endl;
  std::cout << "mem2 b : " << mem2_addr << " (" << *mem2_addr << ")"
            << std::endl;
  std::cout << "mem3 c : " << mem3_addr << " (" << *mem3_addr << ")"
            << std::endl;

  // obj begin addr: 0x7ffcb755a7b0 (32,32),8, BaseA:16
  // vp_ptr1: 0x7ffcb755a7b0
  //   slot -2: 0
  //   slot -1: N2n25test57DerivedE
  //   slot  0: virtual BaseA::show()
  //   slot  1: virtual Derived::print()
  // mem1 a : 0x7ffcb755a7b8 (1)
  // vp_ptr2: 0x7ffcb755a7c0
  //   slot -2: -16
  //   slot -1: N2n25test57DerivedE
  //   slot  0: virtual BaseB::play()
  // mem2 b : 0x7ffcb755a7c8 (2)
  // mem3 c : 0x7ffcb755a7cc (3)

  // Vtable for n2::test5::Derived
  // n2::test5::Derived::_ZTVN2n25test57DerivedE: 7 entries
  // 0     (int (*)(...))0
  // 8     (int (*)(...))(& _ZTIN2n25test57DerivedE)
  // 16    (int (*)(...))n2::test5::BaseA::show
  // 24    (int (*)(...))n2::test5::Derived::print
  // 32    (int (*)(...))-16
  // 40    (int (*)(...))(& _ZTIN2n25test57DerivedE)
  // 48    (int (*)(...))n2::test5::BaseB::play

  // 多重继承下，子类不再有自己的虚函数表，它的虚函数表与第一个父类的合并了。
}
}  // namespace test5

namespace test6 {
// 两个基类中都含有虚函数，派生类有虚函数并重写基类函数：
class BaseA {
 public:
  virtual std::string show() { return "virtual BaseA::show()"; }
  int a = 1;
};

class BaseB {
 public:
  virtual std::string play() { return "virtual BaseB::play()"; }
  int b = 2;
};

class Derived : public BaseA, public BaseB {
 public:
  virtual std::string print() { return "virtual Derived::print()"; }
  virtual std::string show() { return "virtual Derived::show()"; }
  virtual std::string play() { return "virtual Derived::play()"; }
  int c = 3;
};
void func() {
  Derived obj;
  long *vt_ptr1 = (long *)&obj + 0;
  int *mem1_addr = (int *)vt_ptr1 + 2;
  long *vt_ptr2 = (long *)((char *)&obj + sizeof(BaseA));
  int *mem2_addr = (int *)vt_ptr2 + 2;
  int *mem3_addr = (int *)mem2_addr + 1;
  long **pVtab1 = (long **)&obj;
  long **pVtab2 = (long **)((char *)&obj + sizeof(BaseA));

  std::cout << "obj begin addr: " << &obj << " (" << sizeof(Derived) << ","
            << sizeof(obj) << ")"
            << "," << alignof(Derived) << ", BaseA:" << sizeof(BaseA)
            << std::endl;
  std::cout << "vp_ptr1: " << vt_ptr1 << std::endl;
  std::cout << "  slot -2: " << *((long *)(pVtab1[0] - 2)) << std::endl;
  std::cout << "  slot -1: "
            << ((const std::type_info *)*(pVtab1[0] - 1))->name() << std::endl;
  std::cout << "  slot  0: " << ((FunPtr)(pVtab1[0][0]))() << std::endl;
  std::cout << "  slot  1: " << ((FunPtr)(pVtab1[0][1]))() << std::endl;
  std::cout << "  slot  2: " << ((FunPtr)(pVtab1[0][2]))() << std::endl;
  std::cout << "mem1 a : " << mem1_addr << " (" << *mem1_addr << ")"
            << std::endl;
  std::cout << "vp_ptr2: " << vt_ptr2 << std::endl;
  std::cout << "  slot -2: " << *((long *)(pVtab2[0] - 2)) << std::endl;
  std::cout << "  slot -1: "
            << ((const std::type_info *)*(pVtab2[0] - 1))->name() << std::endl;
  std::cout << "  slot  0: " << ((FunPtr)(pVtab2[0][0]))() << std::endl;
  std::cout << "mem2 b : " << mem2_addr << " (" << *mem2_addr << ")"
            << std::endl;
  std::cout << "mem3 c : " << mem3_addr << " (" << *mem3_addr << ")"
            << std::endl;

  // obj begin addr: 0x7ffdc95657e0 (32,32),8, BaseA:16
  // vp_ptr1: 0x7ffdc95657e0
  //   slot -2: 0
  //   slot -1: N2n25test67DerivedE
  //   slot  0: virtual Derived::show()
  //   slot  1: virtual Derived::print()
  //   slot  2: virtual Derived::play()
  // mem1 a : 0x7ffdc95657e8 (1)
  // vp_ptr2: 0x7ffdc95657f0
  //   slot -2: -16
  //   slot -1: N2n25test67DerivedE
  //   slot  0: virtual Derived::play()
  // mem2 b : 0x7ffdc95657f8 (2)
  // mem3 c : 0x7ffdc95657fc (3)

  // Vtable for n2::test6::Derived
  // n2::test6::Derived::_ZTVN2n25test67DerivedE: 8 entries
  // 0     (int (*)(...))0
  // 8     (int (*)(...))(& _ZTIN2n25test67DerivedE)
  // 16    (int (*)(...))n2::test6::Derived::show
  // 24    (int (*)(...))n2::test6::Derived::print
  // 32    (int (*)(...))n2::test6::Derived::play
  // 40    (int (*)(...))-16
  // 48    (int (*)(...))(& _ZTIN2n25test67DerivedE)
  // 56    (int (*)(...))n2::test6::Derived::_ZThn16_N2n25test67Derived4playB5cxx11Ev

  // 如果子类重写了任意父类的虚函数，都会覆盖对应的函数地址记录。
}
}  // namespace test6
}  // namespace n2

namespace n3 {
// 如果说没有虚函数的虚继承只是一个噩梦的话，
// 那么既有虚函数又有虚继承就是真正的炼狱。
// 总结：
// 1.如有虚函数，则内存中加入vptr指针，放在内存中最高位置，
//   其余应继承的成员变量按就近原则在内存中从近到远分布；
// 2.如有虚继承，则在内存汇总加入vbptr指针，指向虚表，
//   表中是距离基类成员变量的偏移量，指针放在vptr（如有）的下面，
//   如有重复拷贝则只继承一份成员变量，放到对象最后面。

namespace test1 {
class Base {
  int b = 100;

 public:
  std::string funB() { return "Base::funB"; }
};
class Derived : virtual public Base {
  int d = 101;

 public:
  std::string funD() { return "Derived::funD"; }
};
void func() {
  Derived obj;
  long **vptr = (long **)&obj;
  int *mem1 = (int *)((long *)&obj + 1);
  int *mem2 = mem1 + 1;
  std::cout << "Base size    : " << sizeof(Base) << std::endl;
  std::cout << "Derived size : " << sizeof(Derived) << std::endl;
  std::cout << "obj addr     : " << &obj << std::endl;
  std::cout << "#1 vptr      : " << vptr << std::endl;
  std::cout << "   slot -3   : " << *((long *)(vptr[0] - 3)) << std::endl;
  std::cout << "   slot -2   : " << *((long *)(vptr[0] - 2)) << std::endl;
  std::cout << "   slot -1   : "
            << ((const std::type_info *)*(vptr[0] - 1))->name() << std::endl;
  std::cout << "#2 Derived::d: " << mem1 << "," << *mem1 << std::endl;
  std::cout << "#3 Base::b   : " << mem2 << "," << *mem2 << std::endl;

  // Base size    : 4
  // Derived size : 16
  // obj addr     : 0x7ffdf2d85be0
  // #1 vptr      : 0x7ffdf2d85be0
  //    slot -3   : 12
  //    slot -2   : 0
  //    slot -1   : N2n35test17DerivedE
  // #2 Derived::d: 0x7ffdf2d85be8,101
  // #3 Base::b   : 0x7ffdf2d85bec,100

  // Vtable for n3::test1::Derived
  // n3::test1::Derived::_ZTVN2n35test17DerivedE: 3 entries
  // 0     12
  // 8     (int (*)(...))0
  // 16    (int (*)(...))(& _ZTIN2n35test17DerivedE)
  //
  // VTT for n3::test1::Derived
  // n3::test1::Derived::_ZTTN2n35test17DerivedE: 1 entries
  // 0     ((& n3::test1::Derived::_ZTVN2n35test17DerivedE) + 24)
}
}  // namespace test1

namespace test2 {
class Base {
  int v = 100;

 public:
  virtual std::string fun() { return "Base::fun"; }
};
class A : public Base {
  int a = 101;

 public:
  virtual std::string fun() { return "A::fun"; }
  virtual std::string funA() { return "A::funA"; }
};
class B : virtual public Base {
  int b = 102;

 public:
  virtual std::string fun() { return "B::fun"; }
  virtual std::string funB() { return "B::funB"; }
};
void func1() {
  B obj;
  long **vptr1 = (long **)&obj;
  long **vptr2 = (long **)((int *)&obj + 2 + 1 + 1);  // obj+vptr+B::b+align
  // 对于基类来说，新增了函数funcB，所以虚函数表中新加了一项，
  // 用这个地址减去offset，就是对于的函数地址（不知道理解的对不对）：
  FunPtr fptr = (FunPtr)((char *)vptr2[0][0] - *((long *)(vptr2[0] - 3)));
  int *mem1 = (int *)((long *)&obj + 1);
  int *mem2 = (int *)((long *)vptr2 + 1);
  std::cout << "Base size    : " << sizeof(Base) << std::endl;
  std::cout << "B size       : " << sizeof(B) << std::endl;
  std::cout << "obj addr     : " << &obj << std::endl;
  std::cout << "#1 vptr1     : " << vptr1 << std::endl;
  std::cout << "   slot -3   : " << *((long *)(vptr1[0] - 3)) << std::endl;
  std::cout << "   slot -2   : " << *((long *)(vptr1[0] - 2)) << std::endl;
  std::cout << "   slot -1   : "
            << ((const std::type_info *)*(vptr1[0] - 1))->name() << std::endl;
  std::cout << "   slot  0   : " << ((FunPtr)(vptr1[0][0]))() << std::endl;
  std::cout << "   slot  1   : " << ((FunPtr)(vptr1[0][1]))() << std::endl;
  std::cout << "#2 B:b       : " << mem1 << "," << *mem1 << std::endl;
  std::cout << "#3 vptr2     : " << vptr2 << std::endl;
  std::cout << "   slot -3   : " << *((unsigned long *)(vptr2[0] - 3)) << " ("
            << *((long *)(vptr2[0] - 3)) << ")" << std::endl;
  std::cout << "   slot -2   : " << *((long *)(vptr2[0] - 2)) << std::endl;
  std::cout << "   slot -1   : "
            << ((const std::type_info *)*(vptr2[0] - 1))->name() << std::endl;
  std::cout << "   slot  0   : " << fptr() << std::endl;
  std::cout << "#3 Base::v   : " << mem2 << "," << *mem2 << std::endl;

  // Base size    : 16
  // B size       : 32
  // obj addr     : 0x7ffddb1f1860
  // #1 vptr1     : 0x7ffddb1f1860
  //    slot -3   : 16
  //    slot -2   : 0
  //    slot -1   : N2n35test21BE
  //    slot  0   : B::fun
  //    slot  1   : B::funB
  // #2 B:b       : 0x7ffddb1f1868,102
  // #3 vptr2     : 0x7ffddb1f1870
  //    slot -3   : 18446744073709551600 (-16)
  //    slot -2   : -16
  //    slot -1   : N2n35test21BE
  //    slot  0   : B::funB
  // #3 Base::v   : 0x7ffddb1f1878,100

  // Vtable for n3::test2::B
  // n3::test2::B::_ZTVN2n35test21BE: 9 entries
  // 0     16
  // 8     (int (*)(...))0
  // 16    (int (*)(...))(& _ZTIN2n35test21BE)
  // 24    (int (*)(...))n3::test2::B::fun
  // 32    (int (*)(...))n3::test2::B::funB
  // 40    18446744073709551600
  // 48    (int (*)(...))-16
  // 56    (int (*)(...))(& _ZTIN2n35test21BE)
  // 64    (int (*)(...))n3::test2::B::_ZTv0_n24_N2n35test21B3funB5cxx11Ev
  //
  // VTT for n3::test2::B
  // n3::test2::B::_ZTTN2n35test21BE: 2 entries
  // 0     ((& n3::test2::B::_ZTVN2n35test21BE) + 24)
  // 8     ((& n3::test2::B::_ZTVN2n35test21BE) + 64)
}
class C : public A, public B {
  int c = 104;

 public:
  virtual std::string funB() { return "C::funB"; }
  virtual std::string funC() { return "C::funC"; }
};
void func2() {
  C obj;
  long **vptr1 = (long **)&obj;
  int *mem1 = (int *)((long *)&obj + 1);
  int *mem2 = mem1 + 1;
  long **vptr2 = (long **)((int *)&obj + 2 + 2);
  int *mem3 = (int *)((long *)vptr2 + 1);
  int *mem4 = mem3 + 1;
  long **vptr3 = (long **)(mem3 + 1 + 1);
  FunPtr fptr = (FunPtr)((char *)vptr3[0][0] - *((long *)(vptr3[0] - 3)));
  int *mem5 = (int *)((long *)vptr3 + 1);

  std::cout << "A size       : " << sizeof(A) << std::endl;
  std::cout << "B size       : " << sizeof(B) << std::endl;
  std::cout << "C size       : " << sizeof(C) << std::endl;
  std::cout << "obj addr     : " << &obj << std::endl;
  std::cout << "#1 vptr1     : " << vptr1 << std::endl;
  std::cout << "   slot -3   : " << *((long *)(vptr1[0] - 3)) << std::endl;
  std::cout << "   slot -2   : " << *((long *)(vptr1[0] - 2)) << std::endl;
  std::cout << "   slot -1   : "
            << ((const std::type_info *)*(vptr1[0] - 1))->name() << std::endl;
  std::cout << "   slot  0   : " << ((FunPtr)(vptr1[0][0]))() << std::endl;
  std::cout << "   slot  1   : " << ((FunPtr)(vptr1[0][1]))() << std::endl;
  std::cout << "   slot  2   : " << ((FunPtr)(vptr1[0][2]))() << std::endl;
  std::cout << "   slot  3   : " << ((FunPtr)(vptr1[0][3]))() << std::endl;
  std::cout << "#2 A::Base::v: " << mem1 << "," << *mem1 << std::endl;
  std::cout << "#3 A::a      : " << mem2 << "," << *mem2 << std::endl;
  std::cout << "#4 vptr2     : " << vptr2 << std::endl;
  std::cout << "   slot -3   : " << *((long *)(vptr2[0] - 3)) << std::endl;
  std::cout << "   slot -2   : " << *((long *)(vptr2[0] - 2)) << std::endl;
  std::cout << "   slot -1   : "
            << ((const std::type_info *)*(vptr2[0] - 1))->name() << std::endl;
  std::cout << "   slot  0   : " << ((FunPtr)(vptr2[0][0]))() << std::endl;
  std::cout << "   slot  1   : " << ((FunPtr)(vptr2[0][1]))() << std::endl;
  std::cout << "#5 A::a      : " << mem3 << "," << *mem3 << std::endl;
  std::cout << "#6 C::c      : " << mem4 << "," << *mem4 << std::endl;
  std::cout << "#7 vptr3     : " << vptr3 << std::endl;
  std::cout << "   slot -3   : " << *((unsigned long *)(vptr3[0] - 3)) << " ("
            << *((long *)(vptr3[0] - 3)) << ")" << std::endl;
  std::cout << "   slot -2   : " << *((long *)(vptr3[0] - 2)) << std::endl;
  std::cout << "   slot -1   : "
            << ((const std::type_info *)*(vptr3[0] - 1))->name() << std::endl;
  std::cout << "   slot  0   : " << fptr() << std::endl;
  std::cout << "#8 B::Base::v: " << mem5 << "," << *mem5 << std::endl;

  // A size       : 16
  // B size       : 32
  // C size       : 48
  // obj addr     : 0x7fff0d0d0f10
  // #1 vptr1     : 0x7fff0d0d0f10
  //    slot -3   : 32
  //    slot -2   : 0
  //    slot -1   : N2n35test21CE
  //    slot  0   : A::fun
  //    slot  1   : A::funA
  //    slot  2   : C::funB
  //    slot  3   : C::funC
  // #2 A::Base::v: 0x7fff0d0d0f18,100
  // #3 A::a      : 0x7fff0d0d0f1c,101
  // #4 vptr2     : 0x7fff0d0d0f20
  //    slot -3   : 16
  //    slot -2   : -16
  //    slot -1   : N2n35test21CE
  //    slot  0   : B::fun
  //    slot  1   : C::funB
  // #5 A::a      : 0x7fff0d0d0f28,102
  // #6 C::c      : 0x7fff0d0d0f2c,104
  // #7 vptr3     : 0x7fff0d0d0f30
  //    slot -3   : 18446744073709551600 (-16)
  //    slot -2   : -32
  //    slot -1   : N2n35test21CE
  //    slot  0   : B::funB
  // #8 B::Base::v: 0x7fff0d0d0f38,100

  // Vtable for n3::test2::C
  // n3::test2::C::_ZTVN2n35test21CE: 16 entries
  // 0     32
  // 8     (int (*)(...))0
  // 16    (int (*)(...))(& _ZTIN2n35test21CE)
  // 24    (int (*)(...))n3::test2::A::fun
  // 32    (int (*)(...))n3::test2::A::funA
  // 40    (int (*)(...))n3::test2::C::funB
  // 48    (int (*)(...))n3::test2::C::funC
  // 56    16
  // 64    (int (*)(...))-16
  // 72    (int (*)(...))(& _ZTIN2n35test21CE)
  // 80    (int (*)(...))n3::test2::B::fun
  // 88    (int (*)(...))n3::test2::C::_ZThn16_N2n35test21C4funBB5cxx11Ev
  // 96    18446744073709551600
  // 104   (int (*)(...))-32
  // 112   (int (*)(...))(& _ZTIN2n35test21CE)
  // 120   (int (*)(...))n3::test2::B::_ZTv0_n24_N2n35test21B3funB5cxx11Ev
  //
  // VTT for n3::test2::C
  // n3::test2::C::_ZTTN2n35test21CE: 5 entries
  // 0     ((& n3::test2::C::_ZTVN2n35test21CE) + 24)
  // 8     ((& n3::test2::C::_ZTCN2n35test21CE16_NS0_1BE) + 24)
  // 16    ((& n3::test2::C::_ZTCN2n35test21CE16_NS0_1BE) + 64)
  // 24    ((& n3::test2::C::_ZTVN2n35test21CE) + 80)
  // 32    ((& n3::test2::C::_ZTVN2n35test21CE) + 120)
}
}  // namespace test2

namespace test3 {
class Base {
  int v = 100;

 public:
  virtual std::string fun() { return "Base::fun"; }

  // Vtable for n3::test3::Base
  // n3::test3::Base::_ZTVN2n35test34BaseE: 3 entries
  // 0     (int (*)(...))0
  // 8     (int (*)(...))(& _ZTIN2n35test34BaseE)
  // 16    (int (*)(...))n3::test3::Base::fun
};
class A : virtual public Base {
  int a = 101;

 public:
  virtual std::string funA() { return "A::funA"; }

  // Vtable for n3::test3::A
  // n3::test3::A::_ZTVN2n35test31AE: 8 entries
  // 0     16
  // 8     (int (*)(...))0
  // 16    (int (*)(...))(& _ZTIN2n35test31AE)
  // 24    (int (*)(...))n3::test3::A::funA
  // 32    0
  // 40    (int (*)(...))-16
  // 48    (int (*)(...))(& _ZTIN2n35test31AE)
  // 56    (int (*)(...))n3::test3::Base::fun

  // VTT for n3::test3::A
  // n3::test3::A::_ZTTN2n35test31AE: 2 entries
  // 0     ((& n3::test3::A::_ZTVN2n35test31AE) + 24)
  // 8     ((& n3::test3::A::_ZTVN2n35test31AE) + 56)
};
class B : virtual public Base {
  int b = 102;

 public:
  virtual std::string fun() { return "B::fun"; }
  virtual std::string funB() { return "B::funB"; }

  // Vtable for n3::test3::B
  // n3::test3::B::_ZTVN2n35test31BE: 9 entries
  // 0     16
  // 8     (int (*)(...))0
  // 16    (int (*)(...))(& _ZTIN2n35test31BE)
  // 24    (int (*)(...))n3::test3::B::fun
  // 32    (int (*)(...))n3::test3::B::funB
  // 40    18446744073709551600
  // 48    (int (*)(...))-16
  // 56    (int (*)(...))(& _ZTIN2n35test31BE)
  // 64    (int (*)(...))n3::test3::B::_ZTv0_n24_N2n35test31B3funEv

  // VTT for n3::test3::B
  // n3::test3::B::_ZTTN2n35test31BE: 2 entries
  // 0     ((& n3::test3::B::_ZTVN2n35test31BE) + 24)
  // 8     ((& n3::test3::B::_ZTVN2n35test31BE) + 64)
};
class C : public A, public B {
  int c = 103;

 public:
  virtual std::string fun() { return "C::fun"; }
  virtual std::string funB() { return "C::funB"; }
  virtual std::string funC() { return "C::funC"; }
  virtual std::string funD() { return "C::funD"; }

  // Vtable for n3::test3::C
  // n3::test3::C::_ZTVN2n35test31CE: 17 entries
  // 0     32
  // 8     (int (*)(...))0
  // 16    (int (*)(...))(& _ZTIN2n35test31CE)
  // 24    (int (*)(...))n3::test3::A::funA
  // 32    (int (*)(...))n3::test3::C::fun
  // 40    (int (*)(...))n3::test3::C::funB
  // 48    (int (*)(...))n3::test3::C::funC
  // 56    (int (*)(...))n3::test3::C::funD
  // 64    16
  // 72    (int (*)(...))-16
  // 80    (int (*)(...))(& _ZTIN2n35test31CE)
  // 88    (int (*)(...))n3::test3::C::_ZThn16_N2n35test31C3funEv
  // 96    (int (*)(...))n3::test3::C::_ZThn16_N2n35test31C4funBEv
  // 104   18446744073709551584
  // 112   (int (*)(...))-32
  // 120   (int (*)(...))(& _ZTIN2n35test31CE)
  // 128   (int (*)(...))n3::test3::C::_ZTv0_n24_N2n35test31C3funEv

  // VTT for n3::test3::C
  // n3::test3::C::_ZTTN2n35test31CE: 7 entries
  // 0     ((& n3::test3::C::_ZTVN2n35test31CE) + 24)
  // 8     ((& n3::test3::C::_ZTCN2n35test31CE0_NS0_1AE) + 24)
  // 16    ((& n3::test3::C::_ZTCN2n35test31CE0_NS0_1AE) + 56)
  // 24    ((& n3::test3::C::_ZTCN2n35test31CE16_NS0_1BE) + 24)
  // 32    ((& n3::test3::C::_ZTCN2n35test31CE16_NS0_1BE) + 64)
  // 40    ((& n3::test3::C::_ZTVN2n35test31CE) + 128)
  // 48    ((& n3::test3::C::_ZTVN2n35test31CE) + 88)
};
void func() {
  C obj;

  long **vptr1 = (long **)&obj;
  int *mem1 = (int *)((long *)&obj + 1);
  long **vptr2 = (long **)((int *)&obj + 2 + 2);
  int *mem2 = (int *)((long *)vptr2 + 1);
  int *mem3 = mem2 + 1;
  long **vptr3 = (long **)(mem3 + 1);
  // FunPtr fptr = (FunPtr)((char *)vptr3[0][0] - *((long *)(vptr3[0] - 3)));
  FunPtr fptr = (FunPtr)((char *)vptr3[0][0] - 146);  // 不知道146是怎么来的
  int *mem4 = (int *)((long *)vptr3 + 1);

  std::cout << "A size       : " << sizeof(A) << std::endl;
  std::cout << "B size       : " << sizeof(B) << std::endl;
  std::cout << "C size       : " << sizeof(C) << std::endl;
  std::cout << "obj addr     : " << &obj << std::endl;
  std::cout << "#1 vptr1     : " << vptr1 << std::endl;
  std::cout << "   slot -3   : " << *((long *)(vptr1[0] - 3)) << std::endl;
  std::cout << "   slot -2   : " << *((long *)(vptr1[0] - 2)) << std::endl;
  std::cout << "   slot -1   : "
            << ((const std::type_info *)*(vptr1[0] - 1))->name() << std::endl;
  std::cout << "   slot  0   : " << ((FunPtr)(vptr1[0][0]))() << std::endl;
  std::cout << "   slot  1   : " << ((FunPtr)(vptr1[0][1]))() << std::endl;
  std::cout << "   slot  2   : " << ((FunPtr)(vptr1[0][2]))() << std::endl;
  std::cout << "   slot  3   : " << ((FunPtr)(vptr1[0][3]))() << std::endl;
  std::cout << "   slot  4   : " << ((FunPtr)(vptr1[0][4]))() << std::endl;
  std::cout << "#2 A::a      : " << mem1 << "," << *mem1 << std::endl;
  std::cout << "#3 vptr2     : " << vptr2 << std::endl;
  std::cout << "   slot -3   : " << *((long *)(vptr2[0] - 3)) << std::endl;
  std::cout << "   slot -2   : " << *((long *)(vptr2[0] - 2)) << std::endl;
  std::cout << "   slot -1   : "
            << ((const std::type_info *)*(vptr2[0] - 1))->name() << std::endl;
  std::cout << "   slot  0   : " << ((FunPtr)(vptr2[0][0]))() << std::endl;
  std::cout << "   slot  1   : " << ((FunPtr)(vptr2[0][1]))() << std::endl;
  std::cout << "#4 B::b      : " << mem2 << "," << *mem2 << std::endl;
  std::cout << "#5 C::c      : " << mem3 << "," << *mem3 << std::endl;
  std::cout << "#6 vptr3     : " << vptr3 << std::endl;
  std::cout << "   slot -3   : " << *((unsigned long *)(vptr3[0] - 3)) << " ("
            << *((long *)(vptr3[0] - 3)) << ")" << std::endl;
  std::cout << "   slot -2   : " << *((long *)(vptr3[0] - 2)) << std::endl;
  std::cout << "   slot -1   : "
            << ((const std::type_info *)*(vptr3[0] - 1))->name() << std::endl;
  std::cout << "   slot  0   : " << fptr() << std::endl;
  std::cout << "#7 Base::v   : " << mem4 << "," << *mem4 << std::endl;

  // A size       : 32
  // B size       : 32
  // C size       : 48
  // obj addr     : 0x7ffe010c7e60
  // #1 vptr1     : 0x7ffe010c7e60
  //    slot -3   : 32
  //    slot -2   : 0
  //    slot -1   : N2n35test31CE
  //    slot  0   : A::funA
  //    slot  1   : C::fun
  //    slot  2   : C::funB
  //    slot  3   : C::funC
  //    slot  4   : C::funD
  // #2 A::a      : 0x7ffe010c7e68,101
  // #3 vptr2     : 0x7ffe010c7e70
  //    slot -3   : 16
  //    slot -2   : -16
  //    slot -1   : N2n35test31CE
  //    slot  0   : C::fun
  //    slot  1   : C::funB
  // #4 B::b      : 0x7ffe010c7e78,102
  // #5 C::c      : 0x7ffe010c7e7c,103
  // #6 vptr3     : 0x7ffe010c7e80
  //    slot -3   : 18446744073709551584 (-32)
  //    slot -2   : -32
  //    slot -1   : N2n35test31CE
  //    slot  0   : C::fun
  // #7 Base::v   : 0x7ffe010c7e88,100
}
}  // namespace test3
}  // namespace n3

int main(int argc, char *argv[]) {
  if (argc < 2) {
    std::cout << argv[0] << " i [0 - 14]" << std::endl;
    return 0;
  }
  int type = atoi(argv[1]);
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
      n1::func4();
      break;
    case 4:
      n2::test1::func();
      break;
    case 5:
      n2::test2::func();
      break;
    case 6:
      n2::test3::func1();
      break;
    case 7:
      n2::test3::func2();
      break;
    case 8:
      n2::test4::func();
      break;
    case 9:
      n2::test5::func();
      break;
    case 10:
      n2::test6::func();
      break;
    case 11:
      n3::test1::func();
      break;
    case 12:
      n3::test2::func1();
      break;
    case 13:
      n3::test2::func2();
      break;
    case 14:
      n3::test3::func();
      break;
    default:
      std::cout << "invalid type" << std::endl;
      break;
  }

  return 0;
}
