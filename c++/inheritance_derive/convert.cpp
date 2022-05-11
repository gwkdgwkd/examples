#include <iostream>

// 数据类型转换的前提是，编译器知道如何对数据进行取舍。
// 类也是一种数据类型，也可以发生数据类型转换。
// 不过这种转换只有在基类和派生类之间才有意义，并且只能将派生类赋值给基类，包括：
// 1.将派生类对象赋值给基类对象；
// 2.将派生类指针赋值给基类指针；
// 3.将派生类引用赋值给基类引用。
// 这在C++中称为向上转型（Upcasting）。
// 相应地，将基类赋值给派生类称为向下转型（Downcasting）。
// 向上转型非常安全，可以由编译器自动完成；
// 向下转型有风险，需要程序员手动干预。
// 向上转型和向下转型是面向对象编程的一种通用概念，它们也存在于Java、C#等编程语言中。
// 向上转型后通过基类的对象、指针、引用只能访问从基类继承过去的成员，不能访问派生类新增的成员。

// 赋值的本质是将现有的数据写入已分配好的内存中，对象的内存只包含了成员变量，
// 所以对象之间的赋值是成员变量的赋值，成员函数不存在赋值问题。
// 对象之间的赋值不会影响成员函数，也不会影响this指针。
// 将派生类对象赋值给基类对象时，会舍弃派生类新增的成员，也就是“大材小用”。
// 这种转换关系是不可逆的，只能用派生类对象给基类对象赋值，而不能用基类对象给派生类对象赋值。
// 理由很简单，基类不包含派生类的成员变量，无法对派生类的成员变量赋值。
// 同理，同一基类的不同派生类对象之间也不能赋值。

namespace n1 {
class B {
 public:
  B() { std::cout << "B()" << std::endl; }
  B(const B &) { std::cout << "B(B&)" << std::endl; }
  void display() { std::cout << "B::display" << std::endl; }
};

class D : public B {
 public:
  D() { std::cout << "D()" << std::endl; }
  void display() { std::cout << "D::display" << std::endl; }
  void display1() { std::cout << "D::display1" << std::endl; }
};

void func1() {  // 将派生类对象赋值给基类对象
  D d;
  d.display();
  // B()
  // D()
  // D::display

  B b = d;
  b.display();
  // B(B&)
  // B::display

  // b.display1(); // 不能调用派生类对象
}

void func2() {  // 将派生类指针赋值给基类指针
  D *pd = new D();
  pd->display();
  // B()
  // D()
  // D::display

  B *pb = pd;
  pb->display();
  // B::display

  // pb->display1();  // 不能调用派生类对象

  delete pd;
}

void func3() {  // 将派生类引用赋值给基类引用
  D d;
  d.display();
  // B()
  // D()
  // D::display

  B &b = d;
  b.display();
  // B::display

  // b.display1();  // 不能调用派生类对象
}
void testN1() {
  func1();
  func2();
  func3();
}
}  // namespace n1

namespace n2 {
// 将派生类指针赋值给基类指针时，通过基类指针只能使用派生类的成员变量，但不能使用派生类的成员函数。
// 编译器通过指针来访问成员变量，指针指向哪个对象就使用哪个对象的数据；
// 编译器通过指针的类型来访问成员函数，指针属于哪个类的类型就使用哪个类的函数。
// 通过引用或对象实现向上转型时，也是一样的，使用基类成员函数，成员函数中引用派生类成员变量。

class A {
 public:
  A(int a) : m_a(a) {}
  void display() { std::cout << "Class A: m_a=" << m_a << std::endl; }

 protected:
  int m_a;
};

class B : public A {
 public:
  B(int a, int b) : A(a), m_b(b) {}
  void display() {
    std::cout << "Class B: m_a=" << m_a << ", m_b=" << m_b << std::endl;
  }

 protected:
  int m_b;
};

class C {
 public:
  C(int c) : m_c(c) {}
  void display() { std::cout << "Class C: m_c=" << m_c << std::endl; }

 protected:
  int m_c;
};

// class D : public C, public B {
class D1 : public B, public C {
 public:
  D1(int a, int b, int c, int d) : B(a, b), C(c), m_d(d) {}
  void display() {
    std::cout << "Class D: m_a=" << m_a << ", m_b=" << m_b << ", m_c=" << m_c
              << ", m_d=" << m_d << std::endl;
  }

 public:
  int m_d;
};

void func1() {
  A *pa = new A(1);
  B *pb = new B(2, 20);
  C *pc = new C(3);
  D1 *pd = new D1(4, 40, 400, 4000);

  pa = pd;
  pa->display();  // Class A: m_a=4
  // 调用display()函数时虽然使用了派生类的成员变量，但是display()函数本身却是基类的。
  // pa本来是基类A的指针，现在指向了派生类D的对象，这使得隐式指针this发生了变化，
  // 也指向了D1类的对象，所以最终在display()内部使用的是D1类对象的成员变量。
  // 编译器虽然通过指针的指向来访问成员变量，但是却不通过指针的指向来访问成员函数：
  // 编译器通过指针的类型来访问成员函数。
  // 对于pa，它的类型是A，不管它指向哪个对象，使用的都是A类的成员函数。

  pb = pd;
  pb->display();  // Class B: m_a=4, m_b=40
  pc = pd;        // 执行pc = pd;语句后，pc和pd的值并不相等。
  pc->display();  // Class C: m_c=400
  std::cout << "pa=" << pa << std::endl;           // pa=0x5647e4fd1f10
  std::cout << "pb=" << pb << std::endl;           // pb=0x5647e4fd1f10
  std::cout << "pc=" << pc << std::endl;           // pc=0x5647e4fd1f18
  std::cout << "pd=" << pd << std::endl;           // pd=0x5647e4fd1f10
  std::cout << "m_d=" << &(pd->m_d) << std::endl;  // m_d=0x5647e4fd1f1c
  // 将最终派生类的指针pd分别赋值给了基类指针pa、pb、pc，按理说它们的值应该相等，都指向同一块内存，
  // 但是运行结果却有力地反驳了这种推论，只有pa、pb、pd三个指针的值相等，pc的值比它们都大。
  // 赋值就是将一个变量的值交给另外一个变量，这种想法虽然没错，但是赋值以前编译器可能会对现有的值进行处理。
  // 例如将double类型的值赋给int类型的变量，编译器会直接抹掉小数部分，导致赋值运算符两边变量的值不相等。
  // 将派生类的指针赋值给基类的指针时也是类似的道理，编译器也可能会在赋值前进行处理。
  // pd的内存模型：   (pa pb pd)            pc
  //                    B(A(m_a)   m_b)   C(m_c)   m_d
  // 对象的指针必须要指向对象的起始位置。
  // 对于A类和B类来说，它们的子对象的起始地址和D1类对象一样，
  // 所以将pd赋值给pa、pb时不需要做任何调整，直接传递现有的值即可；
  // 而C类子对象距离D类对象的开头有一定的偏移，将pd赋值给pc时要加上这个偏移，
  // 这样pc才能指向C类子对象的起始位置。
  // 也就是说，执行pc = pd;语句时编译器对pd的值进行了调整，才导致pc、pd的值不同。
  // 调整过程：pc = (C*)((int)pd + sizeof(B));
  std::cout << "(C *)(reinterpret_cast<long>(pd) + sizeof(B)) = "
            << (C *)(reinterpret_cast<long>(pd) + sizeof(B)) << std::endl;
  // (C *)(reinterpret_cast<long>(pd) + sizeof(B)) = 0x5647e4fd1f18
}

// 把B、C类的继承顺序调整一下，让C在B前面：
class D2 : public C, public B {
 public:
  D2(int a, int b, int c, int d) : B(a, b), C(c), m_d(d) {}

 public:
  int m_d;
};
void func2() {
  A *pa = new A(1);
  B *pb = new B(2, 20);
  C *pc = new C(3);
  D2 *pd = new D2(4, 40, 400, 4000);

  std::cout << "pa=" << pa << std::endl;  // pa=0x5647e4fd2340
  std::cout << "pb=" << pb << std::endl;  // pb=0x5647e4fd2360
  std::cout << "pc=" << pc << std::endl;  // pc=0x5647e4fd2380
  std::cout << "pd=" << pd << std::endl;  // pd=0x5647e4fd23a0

  pa = pd;
  pb = pd;
  pc = pd;
  std::cout << "pc=" << pc << std::endl;           // pc=0x5647e4fd23a0
  std::cout << "pd=" << pd << std::endl;           // pd=0x5647e4fd23a0
  std::cout << "pa=" << pa << std::endl;           // pa=0x5647e4fd23a4
  std::cout << "pb=" << pb << std::endl;           // pb=0x5647e4fd23a4
  std::cout << "m_d=" << &(pd->m_d) << std::endl;  // m_d=0x5647e4fd23ac

  // pd的内存模型： (pc pd)    (pa pb)
  //                 C(m_c)     B(A(m_a) m_b)   m_d
  std::cout << "pd + sizeof(C) = "
            << (B *)(reinterpret_cast<long>(pd) + sizeof(C)) << std::endl;
  // pd + sizeof(C) = 0x5647e4fd23a4
}

void func3() {
  D1 d(4, 40, 400, 4000);
  A &ra = d;
  B &rb = d;
  C &rc = d;
  ra.display();  // Class A: m_a=4
  rb.display();  // Class B: m_a=4, m_b=40
  rc.display();  // Class C: m_c=400
  d.display();   // Class D: m_a=4, m_b=40, m_c=400, m_d=4000
  // ra、rb、rc是基类的引用，它们都引用了派生类对象d，但调用了基类display()函数。
}

void func4() {
  A a(1);
  B b(2, 20);
  C c(3);
  D1 d(4, 40, 400, 4000);

  a = d;
  b = d;
  c = d;
  a.display();  // Class A: m_a=4
  b.display();  // Class B: m_a=4, m_b=40
  c.display();  // Class C: m_c=400
  d.display();  // Class D: m_a=4, m_b=40, m_c=400, m_d=4000
}

void testN2() {
  func1();
  func2();
  func3();
  func4();
}
}  // namespace n2

int main(int argc, char *argv[]) {
  if (argc < 2) {
    std::cout << argv[0] << " i [0 - 2]" << std::endl;
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
    default:
      std::cout << "invalid type" << std::endl;
      break;
  }

  return 0;
}