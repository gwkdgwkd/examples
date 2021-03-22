#include <iostream>

using namespace std;

// 当通过指针访问类的成员函数时：
//  如果该函数是非虚函数，那么编译器会根据指针的类型找到该函数；也就是说，指针是哪个类的类型就调用哪个类的函数。
//  如果该函数是虚函数，并且派生类有同名的函数遮蔽它，那么编译器会根据指针的指向找到该函数；也就是说，指针指向的对象属于哪个类就调用哪个类的函数。这就是多态。
// 编译器之所以能通过指针指向的对象找到虚函数，是因为在创建对象时额外地增加了虚函数表。
// 如果一个类包含了虚函数，那么在创建该类的对象时就会额外地增加一个数组，数组中的每一个元素都是虚函数的入口地址。不过数组和对象是分开存储的，为了将对象和数组关联起来，
// 编译器还要在对象中安插一个指针，指向数组的起始位置。这里的数组就是虚函数表（Virtual function table），简写为vtable。

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
// 如果派生类有同名的虚函数遮蔽（覆盖）了基类的虚函数，那么将使用派生类的虚函数替换基类的虚函数，这样具有遮蔽关系的虚函数在vtable中只会出现一次。
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
// 转换后的表达式是固定的，只要调用display()函数，不管它是哪个类的，都会使用这个表达式。换句话说，编译器不管p指向哪里，一律转换为相同的表达式。
// 转换后的表达式没有用到与p的类型有关的信息，只要知道p的指向就可以调用函数，这跟名字编码（Name Mangling）算法有着本质上的区别。
// eating()函数，它在vtable中的索引为1，通过p调用时会把p->eating();转换为(*(*(p+0)+1))(p);对于不同的虚函数，仅仅改变索引（下标）即可。
// 以上是针对单继承进行的讲解。当存在多继承时，虚函数表的结构就会变得复杂，尤其是有虚继承时，还会增加虚基类表，更加让人抓狂。

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

  return 0;
}
