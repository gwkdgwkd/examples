#include <iostream>

// C++11移动语义是什么：
// 在C++98/03中，如果想用其它对象初始化同类的对象，只能用拷贝构造函数。
// 拷贝构造函数的实现很简单，就是为新对象复制一份和其它对象一样的数据。
// 当类拥有指针类型成员时，拷贝构造函数中需要以深拷贝的方式复制该指针成员。
// 即拷贝该指针成员本身的同时，还要拷贝指针指向的内存资源，
// 否则一旦多个对象的指针指向同一块堆空间时，析构时就会释放多次，是不对的。

// 非const右值引用只能操作右值，程序中的函数返回值、lambda表达式等，
// 都是临时对象，既无名称也无法获取其存储地址，所以属于右值。
// C++11标准中借助右值引用可以为指定类添加移动构造函数。
// 当类中同时包含拷贝构造函数和移动构造函数时，如果使用临时对象初始化对象：
// 1.编译器会优先调用移动构造函数来完成此操作；
// 2.只有当类中没有移动构造函数时，编译器才会退而求其次，调用拷贝构造函数。

// 通常在类中自定义移动构造函数的同时，会再为其自定义适当的拷贝构造函数：
// 1.当利用右值初始化类对象时，会调用移动构造函数；
// 2.使用左值（非右值）初始化类对象时，会调用拷贝构造函数。

// 如果使用左值初始化同类对象，但也想调用移动构造函数完成，可以么？
// C++11标准新引入了std::move()函数，它可以将左值强制转换成对应的右值，
// 便可以使用移动构造函数，其语法格式为：move(arg);
// 其中，arg表示指定的左值对象，该函数会返回arg对象的右值形式。

class A {  // no move
 public:
  A() : p(new int(0)) { std::cout << "A construct!" << std::endl; }
  ~A() {
    std::cout << "A destruct!" << std::endl;
    delete p;
  }

  A(const A &a) : p(new int(*a.p)) {
    std::cout << "A copy construct!" << std::endl;
  }
  A &operator=(const A &a) {
    p = new int(*a.p);
    std::cout << "A operator =!" << std::endl;
    return *this;
  }

  int Get() { return *p; }
  void Set(int i) { *p = i; }

 private:
  int *p;
};

class B {  // has move
 public:
  B() : p(new int(0)) { std::cout << "B construct!" << std::endl; }
  ~B() {
    std::cout << "B destruct!" << std::endl;
    delete p;
  }

  B(const B &b) : p(new int(*b.p)) {  // 深拷贝
    std::cout << "B copy construct!" << std::endl;
  }

  // 此构造函数使用右值引用形式的参数，又称为移动构造函数：
  B(B &&b) : p(b.p) {  // 浅拷贝
    b.p = nullptr;  // 避免了同一块对空间被释放多次情况的发生
    std::cout << "B move construct!" << std::endl;
  }

  B &operator=(const B &b) {  // 深拷贝
    p = new int(*b.p);
    std::cout << "B operator =!" << std::endl;
    return *this;
  }

  B &operator=(B &&b) {  // 浅拷贝
    p = b.p;
    b.p = nullptr;
    std::cout << "B move operator =!" << std::endl;
    return *this;
  }

  int Get() { return *p; }
  void Set(int i) { *p = i; }

 private:
  int *p;
};

namespace n1 {
void func1() {
  A a;  // A construct!
  a.Set(8);

  // a是左值，调用拷贝构造函数和赋值操作符：
  A a1(a);   // A copy construct!
  A a2 = a;  // A copy construct!
  A a3;      // A construct!
  a3 = a;    // A operator =!

  // 使用move把左值a转换为对应的右值：
  A a4(std::move(a));   // A copy construct!
  A a5 = std::move(a);  // A copy construct!
  A a6;                 // A construct!
  a6 = std::move(a);    // A operator =!

  // 没有调用移动相关函数，所以都是复制的，是各自独立的对象：
  std::cout << a4.Get() << std::endl;  // 8
  std::cout << a5.Get() << std::endl;  // 8
  std::cout << a6.Get() << std::endl;  // 8

  // A destruct!
  // A destruct!
  // A destruct!
  // A destruct!
  // A destruct!
  // A destruct!
  // A destruct!
}

void func2() {
  B b;  // B construct!
  b.Set(9);

  B b1(b);   // B copy construct!
  B b2 = b;  // B copy construct!
  B b3;      // B construct!
  b3 = b;    // B operator =!

  // 调用了移动构造函数和移动赋值操作符，但是b只能被移动一次：
  B b4(std::move(b));   // B move construct!
  B b5 = std::move(b);  // B move construct!
  B b6;                 // B construct!
  b6 = std::move(b);    // B move operator =!

  // 只有第一个b4的p指向堆地址，b5和b6的p都指向了nullptr：
  std::cout << b4.Get() << std::endl;  // 9
  // std::cout << b5.Get() << std::endl;  // 崩溃
  // std::cout << b6.Get() << std::endl;  // 崩溃

  b1.Set(1);
  std::cout << b1.Get() << std::endl;  // 1
  b5 = std::move(b1);                  // B move operator =!
  std::cout << b5.Get() << std::endl;  // 1
  // std::cout << b1.Get() << std::endl;  // 崩溃

  b2.Set(2);
  std::cout << b2.Get() << std::endl;  // 2
  B b7(std::move(b2));                 // B move construct!
  std::cout << b7.Get() << std::endl;  // 2
  // std::cout << b2.Get() << std::endl;  // 崩溃

  // B destruct!
  // B destruct!
  // B destruct!
  // B destruct!
  // B destruct!
  // B destruct!
  // B destruct!
  // B destruct!
}
}  // namespace n1

namespace n2 {
A getA() { return A(); }
void func1() {
  A a = getA();
  // g++ -std=c++11 move.cpp -fno-elide-constructors
  // 关闭优化，执行流程：
  // 1.执行getA()函数内部的A()语句，即调用类的默认构造函数生成一个匿名对象；
  //   A construct!
  // 2.执行return A()语句，会调用拷贝构造函数复制一份之前生成的匿名对象，
  //   并将其作为getA()函数的返回值，
  //   A copy construct!
  //   函数体执行完毕之前，匿名对象（步骤1中生成的）会被析构销毁；
  //   A destruct!
  // 3.执行a=get_A()语句，再调用一次拷贝构造函数，将之前拷贝的临时对象复制给a，
  //   A copy construct!
  //   get_A()执行完毕，getA()函数返回的对象（步骤2中生成的）会被析构；
  //   A destruct!
  // 4.程序执行结束前，会自行调用类的析构函数销毁a。
  //   A destruct!

  // 利用拷贝构造函数实现对a对象的初始化，实际上进行了2次深拷贝操作。
  // 当然，对于仅申请少量堆空间的临时对象来说，深拷贝的执行效率依旧可以接受，
  // 但如果临时对象中的指针成员申请了大量的堆空间，势必会影响执行效率。
  // 此问题一直存留在以C++98/03标准编写的C++程序中。
  // 由于临时变量的产生、销毁以及发生的拷贝操作很隐晦的且不会影响程序的正确性，
  // 因此很少进入程序员的视野。

  // g++ -std=c++11 move.cpp
  // 目前多数编译器都会对程序中发生的拷贝操作进行优化：
  // A construct!
  // A destruct!
}

B getB() { return B(); }
void func2() {
  // 那么当类中包含指针类型的成员变量，使用其它对象来初始化同类对象时，
  // 怎样才能避免深拷贝导致的效率问题呢？
  // C++11标准中引入了右值引用的语法，借助它可以实现移动语义。

  // 所谓移动语义，指的就是以移动而非深拷贝的方式初始化含有指针成员的类对象。
  // 移动语义指的就是将其他对象（通常是临时对象）拥有的内存资源移为已用。
  // 对于程序执行过程中产生的临时对象，往往只用于传递数据（没有其它的用处），
  // 并且会很快会被销毁，因此在使用临时对象初始化新对象时，
  // 可以将其包含的指针成员指向的内存资源直接移给新对象所有，
  // 无需再新拷贝一份，这大大提高了初始化的执行效率。

  B b = getB();
  // g++ -std=c++11 move.cpp -fno-elide-constructors
  // 关闭优化：
  // 使用临时对象初始化b对象过程中产生的2次深拷贝操作，都转由移动构造函数完成。
  // B construct!
  // B move construct!
  // B destruct!
  // B move construct!
  // B destruct!
  // B destruct!

  // g++ -std=c++11 move.cpp
  // 目前多数编译器都会对程序中发生的拷贝操作进行优化：
  // B construct!
  // B destruct!
}
}  // namespace n2

int main(int argc, char *argv[]) {
  if (argc < 2) {
    std::cout << argv[0] << " i [0 - 3]" << std::endl;
    return 0;
  }
  int type = argv[1][0] - '0';
  switch (type) {
    case 0:
      n1::func1();
      break;
    case 1:
      n1::func2();
      break;
    case 2:
      n2::func1();
      break;
    case 3:
      n2::func2();
      break;
    default:
      std::cout << "invalid type" << std::endl;
      break;
  }

  return 0;
}