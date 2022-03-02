#include <cstring>
#include <iostream>
#include <string>

using namespace std;

// 在C/C++中，联合体（Union）是一种构造数据类型。
// 在一个联合体内，可以定义多个不同类型的成员，这些成员将会共享同一块内存空间。
// 老版本的C++为了和C语言保持兼容，对联合体的数据成员的类型进行了很大程度的限制，
// 这些限制在今天看来并没有必要，因此C++11取消了这些限制。
// C++98不允许联合体的成员是非POD类型，但是C++11取消了这种限制。
// C++11允许联合体有静态成员，C++11删除了联合体不允许拥有静态成员的限制。
union U {
  static int func() {
    int n = 3;
    return n;
  }
};

namespace n1 {
// C++11标准规定，任何非引用类型都可以成为联合体的数据成员，这种联合体也被称为非受限联合体。例如:
class Student {
 public:
  Student(bool g, int a) : gender(g), age(a) {}

 private:
  bool gender;
  int age;
};
// 因为Student类带有自定义的构造函数，所以是一个非POD类型的，这导致编译器报错。
// 这种规定只是C++为了兼容C语言而制定，然而在长期的编程实践中发现，这种规定是没有必要的。
union T {
  // 含有非POD类型的成员，gcc-5.1.0版本报错
  Student s;  // member ‘Student T::s’ with constructor not allowed in union
  char name[10];
};

void testN1() {}
}  // namespace n1

namespace n2 {
// POD是C++中一个比较重要的概念，英文PlainOldData的缩写，用来描述一个类型的属性。
// POD类型一般具有以下几种特征（包括class、union和struct等）：
// 1)没有用户自定义的构造函数、析构函数、拷贝构造函数和移动构造函数。
// 2)不能包含虚函数和虚基类。
// 3)非静态成员必须声明为public。
// 4)类中的第一个非静态成员的类型与其基类不同，例如：
class A1 {};
class A2 : A1 {  // 第一个非静态成员b是基类类型，所以它不是POD类型。
  A1 b;
};
// 5)在类或者结构体继承时，满足以下两种情况之一：
//   派生类中有非静态成员，且只有一个仅包含静态成员的基类；
//   基类有非静态成员，而派生类没有非静态成员。
class B1 {
  static int n;
};
class B2 : B1 {  // 有非静态成员，且只有一个仅包含静态成员的基类B1，是POD类型
  int n1;
};
class B3 : B2 {  // 基类B2有非静态成员，而派生类B3没有非静态成员，也是POD类型
  static int n2;
};
// 6)所有非静态数据成员均和其基类也符合上述规则（递归定义），也就是说POD类型不能包含非POD类型的数据。
// 7)此外，所有兼容C语言的数据类型都是POD类型（struct、union等不能违背上述规则）。

// placement new是什么？
// 是new关键字的一种进阶用法，既可以在栈（stack）上生成对象，也可以在堆（heap）上生成对象。
// 相对应地，把常见的new的用法称为operator new，它只能在heap上生成对象。
// placement new的语法格式如下：
// new(address) ClassConstruct(...)
// address表示已有内存的地址，该内存可以在栈上，也可以在堆上；
// ClassConstruct(...)表示调用类的构造函数，如果构造函数没有参数，也可以省略括号。
// placement_new利用已经申请好的内存来生成对象，它不再为对象分配新的内存，
// 而是将对象数据放在address指定的内存中。

// 非受限联合体的赋值注意事项:
// C++11规定，如果非受限联合体内有一个非POD的成员，而该成员拥有自定义的构造函数，
// 那么这个非受限联合体的默认构造函数将被编译器删除；
// 其他的特殊成员函数，例如默认拷贝构造函数、拷贝赋值操作符以及析构函数等，也将被删除。
union U1 {
  string s;
  int n;
};
// 解决上面问题的一般需要用到placement new
union U2 {
  string s;
  int n;

 public:
  // 构造时，采用placement_new将s构造在其地址&s上，唯一作用只是调用了一下string类的构造函数。
  U2() { new (&s) string; }
  ~U2() { s.~string(); }  // 在析构时还需要调用string类的析构函数。
};

void testN2() {
  // 因为string类拥有自定义的构造函数，所以U1的构造函数被删除；
  // 定义U1的类型变量u需要调用默认构造函数，所以u也就无法定义成功。
  // U1 u; // use of deleted function ‘U1::U1()’
  U2 u;
}
}  // namespace n2

namespace n3 {
// 非受限联合体的匿名声明和“枚举式类”:
// 匿名联合体是指不具名的联合体（也即没有名字的联合体），一般定义如下：
union U1 {
  // 联合体U内定义了一个不具名的联合体，该联合体包含一个int类型的成员变量，称这个联合体为匿名联合体。
  union {
    int x;
  };
};
// 同样的，非受限联合体也可以匿名，而当非受限的匿名联合体运用于类的声明时，这样的类被称为“枚举式类”:
class Student {
 public:
  Student(bool g, int a) : gender(g), age(a) {}
  bool gender;
  int age;
};
class Singer {
 public:
  enum Type { STUDENT, NATIVE, FOREIGENR };
  Singer(bool g, int a) : s(g, a) { t = STUDENT; }
  Singer(int i) : id(i) { t = NATIVE; }
  Singer(const char* n, int s) {
    int size = (s > 9) ? 9 : s;
    memcpy(name, n, size);
    name[s] = '\0';
    t = FOREIGENR;
  }
  ~Singer() {}

  void print() {
    if (t == STUDENT) {
      std::cout << s.gender << " " << s.age << std::endl;
    } else if (t == NATIVE) {
      std::cout << id << std::endl;
    } else if (t == FOREIGENR) {
      std::cout << name << std::endl;
    }
  }

 private:
  Type t;
  // 使用了一个匿名非受限联合体，它作为类Singer的“变长成员”来使用，
  // 这样的变长成员给类的编写带来了更大的灵活性，这是C++98标准中无法达到的。
  union {
    Student s;  // member ‘Student T::s’ with constructor not allowed in union
    int id;
    char name[10];
  };
};

void testN3() {
  Singer s1(true, 13);
  Singer s2(310217);
  Singer s3("J Michael", 9);

  s1.print();  // 1 13
  s2.print();  // 310217
  s3.print();  // J Michael
}
}  // namespace n3

int main(int argc, char* argv[]) {
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
    case 2:
      n3::testN3();
      break;
    default:
      std::cout << "invalid type" << std::endl;
      break;
  }

  return 0;
}