#include <iostream>

using namespace std;

// 当通过指针访问类的成员函数时：
// 1.如果该函数是非虚函数，那么编译器会根据指针的类型找到该函数；
//   也就是说，指针是哪个类的类型就调用哪个类的函数。
// 2.如果该函数是虚函数，并且派生类有同名的函数遮蔽它，那么编译器会根据指针的指向找到该函数；
//   也就是说，指针指向的对象属于哪个类就调用哪个类的函数，这就是多态。
// 编译器之所以能通过指针指向的对象找到虚函数，是因为在创建对象时额外地增加了虚函数表。
// 如果一个类包含了虚函数，那么在创建该类的对象时就会额外地增加一个数组，数组中的每一个元素都是虚函数的入口地址。
// 不过数组和对象是分开存储的，为了将对象和数组关联起来，
// 编译器还要在对象中安插一个指针，指向数组的起始位置。
// 这里的数组就是虚函数表（Virtual function table），简写为vtable。

class People {
 public:
  People(string name, int age);

 public:
  virtual void display();
  virtual void eating();

 protected:
  string m_name;
  int m_age;
};
People::People(string name, int age) : m_name(name), m_age(age) {}
void People::display() {
  cout << "Class People：" << m_name << "今年" << m_age << "岁了。" << endl;
}
void People::eating() {
  cout << "Class People：我正在吃饭，请不要跟我说话..." << endl;
}

class Student : public People {
 public:
  Student(string name, int age, float score);

 public:
  virtual void display();
  virtual void examing();

 protected:
  float m_score;
};
Student::Student(string name, int age, float score)
    : People(name, age), m_score(score) {}
void Student::display() {
  cout << "Class Student：" << m_name << "今年" << m_age << "岁了，考了"
       << m_score << "分。" << endl;
}
void Student::examing() {
  cout << "Class Student：" << m_name << "正在考试，请不要打扰T啊！" << endl;
}

class Senior : public Student {
 public:
  Senior(string name, int age, float score, bool hasJob);

 public:
  virtual void display();
  virtual void partying();

 private:
  bool m_hasJob;
};
Senior::Senior(string name, int age, float score, bool hasJob)
    : Student(name, age, score), m_hasJob(hasJob) {}
void Senior::display() {
  if (m_hasJob) {
    cout << "Class Senior：" << m_name << "以" << m_score
         << "的成绩从大学毕业了，并且顺利找到了工作，Ta今年" << m_age << "岁。"
         << endl;
  } else {
    cout << "Class Senior：" << m_name << "以" << m_score
         << "的成绩从大学毕业了，不过找工作不顺利，Ta今年" << m_age << "岁。"
         << endl;
  }
}
void Senior::partying() {
  cout << "Class Senior：快毕业了，大家都在吃散伙饭..." << endl;
}

// 各个类的对象内存模型如下所示：
// People对象         vtable             Student对象         vtable                Senior对象          vtable
// XXX *vfptr     0  &People::display    XXX *vfptr      0  &Student::display     XXX *vfptr      0  &Senior::display
// m_name         1  &People::eating     m_name          1  &People::eating       m_name          1  &People::eating
// m_age                                 m_age           2  &Student::examing     m_age           2  &Student::examing
//                                       m_score                                  m_score         3  &Senior::partying
//                                                                                m_hasJob
// 仔细观察虚函数表，可以发现:
// 基类的虚函数在vtable中的索引（下标）是固定的，不会随着继承层次的增加而改变，派生类新增的虚函数放在vtable的最后。
// 如果派生类有同名的虚函数遮蔽（覆盖）了基类的虚函数，那么将使用派生类的虚函数替换基类的虚函数，
// 这样具有遮蔽关系的虚函数在vtable中只会出现一次。
// 当通过指针调用虚函数时，先根据指针找到vfptr，再根据vfptr找到虚函数的入口地址。
// 以虚函数display()为例，它在vtable中的索引为0，通过p调用时：
// p->display();
// 编译器会转换为:
// (*(*(p+0)+0))(p);
// 这个表达式的具体分析：
//  0是vfptr在对象中的偏移，p+0是vfptr的地址；
//  *(p+0)是vfptr的值，而vfptr是指向vtable的指针，所以*(p+0)也就是vtable的地址；
//  display()在vtable中的索引（下标）是0，所以(*(p+0)+0)也就是display()的地址；
//  知道了display()的地址，(*(*(p+0)+0))(p)也就是对display()的调用了，这里的p就是传递的实参，它会赋值给this指针。
// 转换后的表达式是固定的，只要调用display()函数，不管它是哪个类的，都会使用这个表达式。
// 换句话说，编译器不管p指向哪里，一律转换为相同的表达式。
// 转换后的表达式没有用到与p的类型有关的信息，只要知道p的指向就可以调用函数，
// 这跟名字编码（Name Mangling）算法有着本质上的区别。
// eating()函数，它在vtable中的索引为1，通过p调用时会把p->eating();
// 转换为(*(*(p+0)+1))(p);对于不同的虚函数，仅仅改变索引（下标）即可。
// 以上是针对单继承进行的讲解。当
// 存在多继承时，虚函数表的结构就会变得复杂，尤其是有虚继承时，还会增加虚基类表，更加让人抓狂。

typedef void (*Fun)(void);
class Base {
 public:
  virtual void f() { cout << "Base::f" << endl; }
  virtual void g() { cout << "Base::g" << endl; }
  virtual void h() { cout << "Base::h" << endl; }
};
class Derive : public Base {
 public:
  virtual void g() { cout << "Derive::g" << endl; }
  virtual void i() { cout << "Derive::i" << endl; }
};

// 多继承的虚函数表：
//  在多继承情况下，有多少个有虚函数的基类就有多少个虚函数表指针。
//  当子类有多出来的虚函数时，添加在第一个虚函数表中，父类指针不能调用
//  当有多个虚函数表时，虚函数表的结果是0代表没有下一个虚函数表。
//  不同操作系统中代表有下一个虚函数表的标识不同。
class BaseA {
  // int i;

 public:
  virtual void A() { cout << "BaseA::A" << endl; }
  virtual void B() { cout << "BaseA::B" << endl; }
  virtual void C() { cout << "BaseA::C" << endl; }
};
class BaseB {
  // double d;

 public:
  virtual void D() { cout << "BaseB::D" << endl; }
  virtual void E() { cout << "BaseB::E" << endl; }
  virtual void F() { cout << "BaseB::F" << endl; }
};
class DeriveAB : public BaseA, BaseB {
  // char a;

 public:
  virtual void B() { cout << "DeriveAB::B" << endl; }
  virtual void F() { cout << "DeriveAB::F" << endl; }
  virtual void G() { cout << "DeriveAB::G" << endl; }
};

typedef void (*VFPTR)();
void PrintVTable(VFPTR vTable[]) {
  cout << "虚表地址>" << vTable << endl;
  // for (int i = 0; vTable[i] != nullptr; ++i) {
  // 多个虚函数表时，判断第一个表结束的标识有问题，怎么判断？
  for (int i = 0; vTable[i] != nullptr && i < 5; ++i) {
    printf("第%d个虚函数地址 :0X%x,->", i, vTable[i]);
    VFPTR f = vTable[i];
    f();
  }
  cout << endl;
}

int main() {
  People *p = new People("赵红", 29);
  p->display();
  p = new Student("王刚", 16, 84.5);
  p->display();
  p = new Senior("李智", 22, 92.0, true);
  p->display();
  // Class People：赵红今年29岁了。
  // Class Student：王刚今年16岁了，考了84.5分。
  // Class Senior：李智以92的成绩从大学毕业了，并且顺利找到了工作，Ta今年22岁。

  Base b;  // ubuntu中，第二维步长为2，不然段错误。2要换成4，why？
  cout << sizeof(b) << endl;  // 8
  Fun fptr1 = (Fun) * ((int *)*(int *)((int *)&b + 0) + 0);
  Fun fptr2 = (Fun) * ((int *)*(int *)((int *)&b + 0) + 2);
  Fun fptr3 = (Fun) * ((int *)*(int *)((int *)&b + 0) + 4);
  fptr1();  // Base::f
  fptr2();  // Base::g
  fptr3();  // Base::h
  int **pVtab = (int **)&b;
  Fun fptr4 = (Fun)pVtab[0][0];
  Fun fptr5 = (Fun)pVtab[0][2];
  Fun fptr6 = (Fun)pVtab[0][4];
  fptr4();                      // Base::f
  fptr5();                      // Base::g
  fptr6();                      // Base::h
  cout << pVtab[0][5] << endl;  // 0
  VFPTR *vTableb = (VFPTR *)(*(int *)&b);
  PrintVTable(vTableb);
  // 虚表地址>0x402108
  // 第0个虚函数地址 :0X401b66,->Base::f
  // 第1个虚函数地址 :0X401b92,->Base::g
  // 第2个虚函数地址 :0X401bbe,->Base::h

  Derive d;
  cout << sizeof(d) << endl;  // 8
  Fun fptr11 = (Fun) * ((int *)*(int *)((int *)&d + 0) + 0);
  Fun fptr12 = (Fun) * ((int *)*(int *)((int *)&d + 0) + 2);
  Fun fptr13 = (Fun) * ((int *)*(int *)((int *)&d + 0) + 4);
  Fun fptr14 = (Fun) * ((int *)*(int *)((int *)&d + 0) + 6);
  fptr11();  // Base::f
  fptr12();  // Derive::g
  fptr13();  // Base::h
  fptr14();  // Derive::i
  int **pVtab1 = (int **)&d;
  Fun fptr15 = (Fun)pVtab1[0][0];
  Fun fptr16 = (Fun)pVtab1[0][2];
  Fun fptr17 = (Fun)pVtab1[0][4];
  Fun fptr18 = (Fun)pVtab1[0][6];
  fptr15();                      // Base::f
  fptr16();                      // Derive::g
  fptr17();                      // Base::h
  fptr18();                      // Derive::i
  cout << pVtab1[0][8] << endl;  // 0
  VFPTR *vTableb1 = (VFPTR *)(*(int *)&d);
  PrintVTable(vTableb1);
  // 虚表地址>0x4020f8
  // 第0个虚函数地址 :0X401b88,->Base::f
  // 第1个虚函数地址 :0X401c0c,->Derive::g
  // 第2个虚函数地址 :0X401be0,->Base::h
  // 第3个虚函数地址 :0X401c38,->Derive::i

  DeriveAB d1;
  cout << sizeof(d1) << endl;  // 16　(两个指针么？)
  // 第一维的偏移怎么算？类中有成员变量会段错误
  Fun fptr21 = (Fun) * ((int *)*(int *)((int *)&d1 + 0) + 0);
  Fun fptr22 = (Fun) * ((int *)*(int *)((int *)&d1 + 0) + 2);
  Fun fptr23 = (Fun) * ((int *)*(int *)((int *)&d1 + 0) + 4);
  Fun fptr24 = (Fun) * ((int *)*(int *)((int *)&d1 + 0) + 6);
  Fun fptr25 = (Fun) * ((int *)*(int *)((int *)&d1 + 0) + 8);
  Fun fptr26 = (Fun) * ((int *)*(int *)((int *)&d1 + 2) + 0);
  Fun fptr27 = (Fun) * ((int *)*(int *)((int *)&d1 + 2) + 2);
  Fun fptr28 = (Fun) * ((int *)*(int *)((int *)&d1 + 2) + 4);
  fptr21();  // BaseA::A
  fptr22();  // DeriveAB::B
  fptr23();  // BaseA::C
  fptr24();  // DeriveAB::F
  fptr25();  // DeriveAB::G
  fptr26();  // BaseB::D
  fptr27();  // BaseB::E
  fptr28();  // DeriveAB::F
  int **pVtab2 = (int **)&d1;
  Fun fptr31 = (Fun)pVtab2[0][0];
  Fun fptr32 = (Fun)pVtab2[0][2];
  Fun fptr33 = (Fun)pVtab2[0][4];
  Fun fptr34 = (Fun)pVtab2[0][6];
  Fun fptr35 = (Fun)pVtab2[0][8];
  cout << pVtab2[0][10] << endl;  // -8
  Fun fptr36 = (Fun)pVtab2[1][0];
  Fun fptr37 = (Fun)pVtab2[1][2];
  Fun fptr38 = (Fun)pVtab2[1][4];
  fptr31();  // BaseA::A
  fptr32();  // DeriveAB::B
  fptr33();  // BaseA::C
  fptr34();  // DeriveAB::F
  fptr35();  // DeriveAB::G
  fptr36();  // BaseB::D
  fptr37();  // BaseB::E
  fptr38();  // DeriveAB::F
  VFPTR *vTableb21 = (VFPTR *)(*(int *)&d1);
  PrintVTable(vTableb21);
  // 虚表地址>0x402270
  // 第0个虚函数地址 :0X401e32,->BaseA::A
  // 第1个虚函数地址 :0X401ee2,->DeriveAB::B
  // 第2个虚函数地址 :0X401e5e,->BaseA::C
  // 第3个虚函数地址 :0X401f0e,->DeriveAB::F
  // 第4个虚函数地址 :0X401f40,->DeriveAB::G
  VFPTR *vTableb22 = (VFPTR *)(*(int *)((char *)&d1 + sizeof(BaseA)));
  PrintVTable(vTableb22);
  // 虚表地址>0x4022a8
  // 第0个虚函数地址 :0X401e8a,->BaseB::D
  // 第1个虚函数地址 :0X401eb6,->BaseB::E
  // 第2个虚函数地址 :0X401f39,->DeriveAB::F

  return 0;
}
