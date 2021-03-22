#include <iostream>

using namespace std;

// 在C/C++中经常会发生数据类型的转换，例如将int类型的数据赋值给float类型的变量时，编译器会先把int类型的数据转换为float类型再赋值；
// 反过来，float类型的数据在经过类型转换后也可以赋值给int类型的变量。
// 数据类型转换的前提是，编译器知道如何对数据进行取舍。
// 类其实也是一种数据类型，也可以发生数据类型转换，不过这种转换只有在基类和派生类之间才有意义，并且只能将派生类赋值给基类，包括将派生类对象赋值给基类对象、将派
// 生类指针赋值给基类指针、将派生类引用赋值给基类引用，这在C++中称为向上转型（Upcasting）。相应地，将基类赋值给派生类称为向下转型（Downcasting）。
// 向上转型非常安全，可以由编译器自动完成；向下转型有风险，需要程序员手动干预。向上转型和向下转型是面向对象编程的一种通用概念，它们也存在于Java、C#等编程语言中。
// 向上转型后通过基类的对象、指针、引用只能访问从基类继承过去的成员（包括成员变量和成员函数），不能访问派生类新增的成员。

// 赋值的本质是将现有的数据写入已分配好的内存中，对象的内存只包含了成员变量，所以对象之间的赋值是成员变量的赋值，成员函数不存在赋值问题。
// 对象之间的赋值不会影响成员函数，也不会影响this指针。
// 将派生类对象赋值给基类对象时，会舍弃派生类新增的成员，也就是“大材小用”。
// 这种转换关系是不可逆的，只能用派生类对象给基类对象赋值，而不能用基类对象给派生类对象赋值。
// 理由很简单，基类不包含派生类的成员变量，无法对派生类的成员变量赋值。同理，同一基类的不同派生类对象之间也不能赋值。
class A {
 public:
  A(int a);

 public:
  void display();

 public:
  int m_a;
};
A::A(int a) : m_a(a) {}
void A::display() { cout << "Class A: m_a=" << m_a << endl; }
class B : public A {
 public:
  B(int a, int b);

 public:
  void display();

 public:
  int m_b;
};
B::B(int a, int b) : A(a), m_b(b) {}
void B::display() { cout << "Class B: m_a=" << m_a << ", m_b=" << m_b << endl; }

// 除了可以将派生类对象赋值给基类对象（对象变量之间的赋值），还可以将派生类指针赋值给基类指针（对象指针之间的赋值）。
// 将派生类指针赋值给基类指针时，通过基类指针只能使用派生类的成员变量，但不能使用派生类的成员函数。
// 编译器通过指针来访问成员变量，指针指向哪个对象就使用哪个对象的数据；编译器通过指针的类型来访问成员函数，指针属于哪个类的类型就使用哪个类的函数。
class A1 {
 public:
  A1(int a);

 public:
  void display();

 protected:
  int m_a;
};
A1::A1(int a) : m_a(a) {}
void A1::display() { cout << "Class A: m_a=" << m_a << endl; }
class B1 : public A1 {
 public:
  B1(int a, int b);

 public:
  void display();

 protected:
  int m_b;
};
B1::B1(int a, int b) : A1(a), m_b(b) {}
void B1::display() {
  cout << "Class B: m_a=" << m_a << ", m_b=" << m_b << endl;
}
class C1 {
 public:
  C1(int c);

 public:
  void display();

 protected:
  int m_c;
};
C1::C1(int c) : m_c(c) {}
void C1::display() { cout << "Class C: m_c=" << m_c << endl; }
// class D1 : public C1, public B1 {
class D1 : public B1, public C1 {
 public:
  D1(int a, int b, int c, int d);

 public:
  void display();

 private:
  int m_d;
};
D1::D1(int a, int b, int c, int d) : B1(a, b), C1(c), m_d(d) {}
void D1::display() {
  cout << "Class D: m_a=" << m_a << ", m_b=" << m_b << ", m_c=" << m_c
       << ", m_d=" << m_d << endl;
}

// 引用在本质上是通过指针的方式实现的，既然基类的指针可以指向派生类的对象，那么就有理由推断：基类的引用也可以指向派生类的对象，并且它的表现和指针是类似的。
// 基类引用派生类时，使用了派生类对象的成员变量，但是却没有使用派生类的成员函数，这和指针的表现是一样的。

int main() {
  A a(10);
  B b(66, 99);
  a.display();
  b.display();
  a = b;
  a.display();
  b.display();
  // Class A: m_a=10
  // Class B: m_a=66, m_b=99
  // Class A: m_a=66
  // Class B: m_a=66, m_b=99

  A1 *pa = new A1(1);
  B1 *pb = new B1(2, 20);
  C1 *pc = new C1(3);
  D1 *pd = new D1(4, 40, 400, 4000);
  pa = pd;
  // 调用display()函数时虽然使用了派生类的成员变量，但是display()函数本身却是基类的。
  // pa本来是基类A的指针，现在指向了派生类D的对象，这使得隐式指针this发生了变化，也指向了D类的对象，所以最终在display()内部使用的是D类对象的成员变量。
  // 编译器虽然通过指针的指向来访问成员变量，但是却不通过指针的指向来访问成员函数：编译器通过指针的类型来访问成员函数。
  // 对于pa，它的类型是A，不管它指向哪个对象，使用的都是A类的成员函数。
  pa->display();  // Class A: m_a=4
  pb = pd;
  pb->display();  // Class B: m_a=4, m_b=40
  pc = pd;        // 执行pc = pd;语句后，pc和pd的值并不相等。
  pc->display();  // Class C: m_c=400
  cout << "pa=" << pa << endl;  // 0x557f13d68320
  cout << "pb=" << pb << endl;  // 0x557f13d68320
  cout << "pc=" << pc << endl;  // 0x557f13d68328
  cout << "pd=" << pd << endl;  // 0x557f13d68320
  // 将最终派生类的指针pd分别赋值给了基类指针pa、pb、pc，按理说它们的值应该相等，都指向同一块内存，
  // 但是运行结果却有力地反驳了这种推论，只有pa、pb、pd三个指针的值相等，pc的值比它们都大。
  // 赋值就是将一个变量的值交给另外一个变量，这种想法虽然没错，但是有一点要注意，就是赋值以前编译器可能会对现有的值进行处理。
  // 例如将double类型的值赋给int类型的变量，编译器会直接抹掉小数部分，导致赋值运算符两边变量的值不相等。
  // 将派生类的指针赋值给基类的指针时也是类似的道理，编译器也可能会在赋值前进行处理。
  // pd的内存模型：   (pa pb pd)            pc
  //                    B(A(m_a)   m_b)   C(m_c)   m_d
  // 对象的指针必须要指向对象的起始位置。对于A类和B类来说，它们的子对象的起始地址和D类对象一样，所以将pd赋值给pa、pb时不需要做任何调整，直接传递现有的值即可；
  // 而C类子对象距离D类对象的开头有一定的偏移，将pd赋值给pc时要加上这个偏移，这样pc才能指向C类子对象的起始位置。
  // 也就是说，执行pc = pd;语句时编译器对pd的值进行了调整，才导致pc、pd的值不同。
  // 调整过程：pc = (C*)((int)pd + sizeof(B));
  // 如果我们把B、C类的继承顺序调整一下，让C在B前面：class D: public C, public B
  //  pa=0x55b80e0a0324
  //  pb=0x55b80e0a0324
  //  pc=0x55b80e0a0320
  //  pd=0x55b80e0a0320

  D1 d(4, 40, 400, 4000);
  A1 &ra = d;
  B1 &rb = d;
  C1 &rc = d;
  ra.display();  // Class A: m_a=4
  rb.display();  // Class B: m_a=4, m_b=40
  rc.display();  // Class C: m_c=400
  // ra、rb、rc是基类的引用，它们都引用了派生类对象d，但调用了基类display()函数。

  return 0;
}