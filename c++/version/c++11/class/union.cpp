#include <cstring>
#include <iostream>
#include <string>

// 在C/C++中，联合体（Union）是一种构造数据类型。
// 联合体内可以定义多个不同类型的成员，这些成员将会共享同一块内存空间。
// 老版C++为了和C保持兼容，对联合体的数据成员的类型进行了很大程度的限制，
// 这些限制在今天看来并没有必要：
// C++98不允许联合体的成员是非POD类型，但是C++11取消了这种限制。
// C++11允许联合体有静态成员，C++11删除了联合体不允许拥有静态成员的限制。
union U {
  static int func() {
    int n = 3;
    return n;
  }
};

namespace n1 {
// C++11标准规定，任何非引用类型都可以成为联合体的数据成员，
// 这种联合体也被称为非受限联合体，例如：
class Student {
 public:
  Student(bool g, int a) : gender(g), age(a) {}

 private:
  bool gender;
  int age;
};

union T {
  // 因为Student类带有自定义的构造函数，所以是一个非POD类型的，
  // 这导致编译器报错，gcc-5.1.0版本报错。
  // 只是为了兼容C而制定，在长期的编程实践中发现，这种规定是没有必要的。
  Student s;
  char name[10];
  // int& r;  // 不能有引用类型
};
}  // namespace n1

namespace n2 {
// 非受限联合体的赋值注意事项：
// C++11规定，如果非受限联合体内有一个非POD的成员，而该成员拥有自定义的构造函数，
// 那么这个非受限联合体的默认构造函数将被编译器删除，其他的特殊成员函数，
// 如默认拷贝构造函数、拷贝赋值操作符以及析构函数等也将被删除。
union U1 {
  std::string s;
  int n;
};

// 解决上面问题的一般需要用到placement new，是new关键字的一种进阶用法，
// 既可以在栈（stack）上生成对象，也可以在堆（heap）上生成对象。
// 相对应地，把常见的new的用法称为operator new，它只能在heap上生成对象。
// placement new的语法格式如下：
// new(address) ClassConstruct(...)
// address表示已有内存的地址，该内存可以在栈上，也可以在堆上；
// ClassConstruct(...)表示调用类的构造函数，如果没有参数，也可以省略括号。
// placement_new利用已经申请好的内存来生成对象，
// 它不再为对象分配新的内存，而是将对象数据放在address指定的内存中。
using namespace std;
union U2 {
  std::string s;
  int n;

 public:
  U2() {
    // 构造时，采用placement_new将s构造在其地址&s上，
    // 唯一作用只是调用了一下string类的构造函数：
    new (&s) std::string;
  }

  ~U2() {
    // 在析构时还需要调用string类的析构函数：
    s.~string();
  }
};

void func() {
  // 因为string类拥有自定义的构造函数，所以U1的默认构造函数被删除；
  // 定义U1的类型变量u需要调用默认构造函数，所以u也就无法定义成功：
  // U1 u; // use of deleted function ‘U1::U1()’

  U2 u;
}
}  // namespace n2

namespace n3 {
// 非受限联合体的匿名声明和枚举式类

// 匿名联合体是指不具名的联合体（也即没有名字的联合体），一般定义如下：
union U {
  union {  // 匿名联合体
    int x;
  };
};
// 匿名联合体没有名称，只能定义在一个结构体或类的内部，
// 并且可以与该结构体或类的其他成员共享相同的命名空间。

class Student {
 public:
  Student(bool g, int a) : gender(g), age(a) {}
  bool gender;
  int age;
};
// 非受限联合体也可以匿名，当用于类的声明时，这样的类被称为枚举式类：
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
  // 使用了一个匿名非受限联合体，它作为类Singer的变长成员来使用，
  // 这样的变长成员给类的编写带来了更大的灵活性，这是C++98标准中无法达到的：
  union {
    // member ‘Student T::s’ with constructor not allowed in union
    Student s;
    int id;
    char name[10];
  };
};

void func() {
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
    std::cout << argv[0] << " i [0 - 1]" << std::endl;
    return 0;
  }
  int type = argv[1][0] - '0';
  switch (type) {
    case 0:
      n2::func();
      break;
    case 1:
      n3::func();
      break;
    default:
      std::cout << "invalid type" << std::endl;
      break;
  }

  return 0;
}