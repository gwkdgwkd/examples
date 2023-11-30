#include <iostream>

// 继承的一般语法为：
// class 派生类名:［继承方式］ 基类名{
//     派生类新增加的成员
// };
// 继承方式包括public、private和protected，
// 此项是可选的，如果不写，那么默认为private。

// 继承方式/基类成员 	  public成员 	 protected成员  private成员
// public继承 	      public 	    protected 	   不可见
// protected继承 	    protected 	protected 	   不可见
// private继承 	      private 	  private 	     不可见
// 规律：
// 1.基类成员在派生类中的访问权限不得高于继承方式中指定的权限。
//   也就是说，继承方式中的public、protected、private，
//   是用来指明基类成员在派生类中的最高访问权限的；
// 2.不管继承方式如何，基类中的private成员在派生类中始终不能使用，
//   不能在派生类的成员函数中访问或调用；
// 3.如果希望基类的成员能够被派生类继承并且毫无障碍地使用，
//   那么这些成员只能声明为public或protected，
//   只有那些不希望在派生类中使用的成员才声明为private；
// 4.如果希望基类的成员既不向外暴露（不能通过对象访问），
//   还能在派生类中使用，那么只能声明为protected。

// 由于private和protected继承方式会改变基类成员在派生类中的访问权限，
// 导致继承关系复杂，所以一般使用public。

namespace n1 {
class B {
 public:
  int a = 1;

 protected:
  int b = 2;

 private:
  int c = 3;
};

namespace test1 {
class D : public B {
 public:
  void display() {
    std::cout << "display:" << a << std::endl;
    std::cout << "display:" << b << std::endl;
    // std::cout << "display:" << c << std::endl;
  }
};

class DD : public D {
 public:
  void display() {
    std::cout << "display:" << a << std::endl;
    std::cout << "display:" << b << std::endl;
    // std::cout << "display:" << c << std::endl;
  }
};

void func() {
  D d;
  d.display();
  std::cout << "d.a:" << d.a << std::endl;
  // std::cout << "d.b:" << d.b << std::endl;
  // std::cout << "d.c:" << d.c << std::endl;

  // display:1
  // display:2
  // d.a:1

  DD dd;
  dd.display();
  // display:1
  // display:2
}
}  // namespace test1

namespace test2 {
class D : protected B {
 public:
  void display() {
    std::cout << "display:" << a << std::endl;
    std::cout << "display:" << b << std::endl;
    // std::cout << "display:" << c << std::endl;
  }
};
class DD : public D {
 public:
  void display() {
    std::cout << "display:" << a << std::endl;
    std::cout << "display:" << b << std::endl;
    // std::cout << "display:" << c << std::endl;
  }
};

void func() {
  D d;
  d.display();
  // std::cout << "d.a:" << d.a << std::endl;
  // std::cout << "d.b:" << d.b << std::endl;
  // std::cout << "d.c:" << d.c << std::endl;

  // display:1
  // display:2

  DD dd;
  dd.display();
  // display:1
  // display:2
}
}  // namespace test2

namespace test3 {
class D : private B {
 public:
  void display() {
    std::cout << "display:" << a << std::endl;
    std::cout << "display:" << b << std::endl;
    // std::cout << "display:" << c << std::endl;
  }
};
class DD : public D {
 public:
  void display() {
    // std::cout << "display:" << a << std::endl;
    // std::cout << "display:" << b << std::endl;
    // std::cout << "display:" << c << std::endl;
  }
};

void func() {
  D d;
  d.display();
  // std::cout << "d.a:" << d.a << std::endl;
  // std::cout << "d.b:" << d.b << std::endl;
  // std::cout << "d.c:" << d.c << std::endl;

  // display:1
  // display:2

  DD dd;
  dd.display();
}
}  // namespace test3
}  // namespace n1

namespace n2 {
// 基类的private成员不能在派生类中使用，并没有说不能被继承。
// 实际上，基类的private成员是被继承的，并且会占用派生类对象的内存，
// 它只是在派生类中不可见，导致无法使用罢了。
// 派生类访问基类private成员的唯一方法是借助基类非private成员函数，
// 如果基类没有非private成员函数，那么该成员在派生类中将无法访问。

class B {
 public:
  void dispaly() { std::cout << b << std::endl; }

 private:
  int b = 6;
};

class D : public B {
 public:
  // void dispaly() { std::cout << b << std::endl; }

 private:
  int d = 8;
};

void func() {
  // 派生类继承来基类的私有数据成员：
  std::cout << sizeof(B) << std::endl;  // 4
  std::cout << sizeof(D) << std::endl;  // 8

  // 派生类对象通过基类的非private成员函数访问基类的private数据成员：
  D d;
  d.dispaly();  // 6
}
}  // namespace n2

namespace n3 {
// 使用using关键字可以改变基类成员在派生类中的访问权限，
// 例如将public改为private、将protected改为public。
// using只能改变基类中public和protected成员的访问权限，
// 不能改变private成员的访问权限，因为基类中private成员在派生类中不可见，
// 根本不能使用，所以基类中的private成员在派生类中无论如何都不能访问。

class B {
 public:
  int a = 1;
  int b = 2;
  int c = 3;

 protected:
  int d = 4;
  int e = 5;
};
class D : public B {
 public:
  using B::d;  // 将protected改为public

 protected:
  using B::b;  // 将public改为protected

 private:
  using B::c;  // 将public改为private
};

void func() {
  B b;
  std::cout << b.a << std::endl;  // 1
  std::cout << b.b << std::endl;  // 2
  std::cout << b.c << std::endl;  // 3
  // std::cout << b.d << std::endl;

  D d;
  std::cout << d.a << std::endl;  // 1
  std::cout << d.d << std::endl;  // 4
  // std::cout << d.c << std::endl;
}
}  // namespace n3

namespace n4 {
// 如果派生类中的成员（包括成员变量和成员函数）和基类中的成员重名，
// 那么就会遮蔽从基类继承过来的成员。
// 所谓遮蔽，就是在派生类中使用该成员时，包括在定义派生类时使用，
// 也包括通过派生类对象访问该成员，实际上使用的是派生类新增而不是继承来的。
// 基类成员函数和派生类成员函数不构成重载：
// 基类成员和派生类成员的名字一样时会造成遮蔽，这句话对于成员变量很好理解，
// 对于成员函数要引起注意，不管函数的参数如何，只要名字一样就会造成遮蔽。
// 换句话说，基类成员函数和派生类成员函数不会构成重载，如果派生类有同名函数，
// 那么就会遮蔽基类中的所有同名函数，不管它们的参数是否一样。
class B {
 public:
  void show() { std::cout << "B::show()" << std::endl; }
  int a = 1;
};

class D1 : public B {
 public:
  void show() { std::cout << "D1::show()" << std::endl; }
  void show(int) { std::cout << "D1::show(int)" << std::endl; }
  void show(float) { std::cout << "D1::show(float)" << std::endl; }
  int a = 2;
};

class D2 : public B {
 public:
  void show(int) { std::cout << "D2::show(int)" << std::endl; }
  int a = 3;
};

void func() {
  B b;
  b.show();                       // B::show()
  std::cout << b.a << std::endl;  // 1

  D1 d1;
  d1.show();                       // D1::show()
  d1.show(5);                      // D1::show(int)
  d1.show(3.4f);                   // D1::show(float)
  std::cout << d1.a << std::endl;  // 2

  D2 d2;
  // d2.show();  // no matching function for call to ‘n4::D2::show()’
  d2.show(1);                      // D2::show(int)
  std::cout << d2.a << std::endl;  // 3
}
}  // namespace n4

int main(int argc, char *argv[]) {
  if (argc < 2) {
    std::cout << argv[0] << " i [0 - 5]" << std::endl;
    return 0;
  }
  int type = argv[1][0] - '0';
  switch (type) {
    case 0:
      n1::test1::func();
      break;
    case 1:
      n1::test2::func();
      break;
    case 2:
      n1::test3::func();
      break;
    case 3:
      n2::func();
      break;
    case 4:
      n3::func();
      break;
    case 5:
      n4::func();
      break;
    default:
      std::cout << "invalid type" << std::endl;
      break;
  }
  return 0;
}