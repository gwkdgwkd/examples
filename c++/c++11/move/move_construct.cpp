#include <iostream>

using namespace std;

class demo {
 public:
  demo() : num(new int(0)) { cout << "construct!" << endl; }
  //拷贝构造函数
  demo(const demo &d) : num(new int(*d.num)) {
    cout << "copy construct!" << endl;
  }
  ~demo() { cout << "class destruct!" << endl; }

 private:
  int *num;
};
demo get_demo() { return demo(); }

class demo1 {
 public:
  demo1() : num(new int(0)) { cout << "construct!" << endl; }
  demo1(const demo1 &d) : num(new int(*d.num)) {
    cout << "copy construct!" << endl;
  }
  // 此构造函数使用右值引用形式的参数，又称为移动构造函数。
  demo1(demo1 &&d) : num(d.num) {
    // 并且在此构造函数中，num指针变量采用的是浅拷贝的复制方式，同时在函数内部重置了d.num，有效避免了“同一块对空间被释放多次”情况的发生。
    d.num = NULL;
    cout << "move construct!" << endl;
  }
  ~demo1() { cout << "class destruct!" << endl; }

 private:
  int *num;
};
demo1 get_demo1() { return demo1(); }

int main() {
  // C++11移动语义是什么:
  // 在C++11标准之前（C++ 98/03标准中），如果想用其它对象初始化一个同类的新对象，只能借助类中的复制（拷贝）构造函数。
  // 拷贝构造函数的实现原理很简单，就是为新对象复制一份和其它对象一模一样的数据。
  // 当类中拥有指针类型的成员变量时，拷贝构造函数中需要以深拷贝（而非浅拷贝）的方式复制该指针成员。即拷贝该指针成员本身的同时，还要拷贝指针指向的内存资源
  // 否则一旦多个对象中的指针成员指向同一块堆空间，这些对象析构时就会对该空间释放多次，这是不允许的。

  // 其整个初始化的流程包含以下几个阶段：
  // 执行get_demo()函数内部的demo()语句，即调用demo类的默认构造函数生成一个匿名对象；
  // 执行return demo()语句，会调用拷贝构造函数复制一份之前生成的匿名对象，并将其作为get_demo()函数的返回值（函数体执行完毕之前，匿名对象会被析构销毁）；
  // 执行a = get_demo()语句，再调用一次拷贝构造函数，将之前拷贝得到的临时对象复制给a（此行代码执行完毕，get_demo()函数返回的对象会被析构）；
  // 程序执行结束前，会自行调用demo类的析构函数销毁a。
  demo a = get_demo();
  // 目前多数编译器都会对程序中发生的拷贝操作进行优化:
  // construct!
  // class destruct!
  // 使用g++的-fno-elide-constructors命令运行:g++ -std=c++11 move.cpp -fno-elide-constructors
  // construct!
  // copy construct!
  // class destruct!
  // copy construct!
  // class destruct!
  // class destruct!
  // 利用拷贝构造函数实现对a对象的初始化，底层实际上进行了2次拷贝（而且是深拷贝）操作。当然，对于仅申请少量堆空间的临时对象来说，深拷贝的执行效率依旧可以接受，
  // 但如果临时对象中的指针成员申请了大量的堆空间，那么2次深拷贝操作势必会影响a对象初始化的执行效率。
  // 此问题一直存留在以C++98/03标准编写的C++程序中。由于临时变量的产生、销毁以及发生的拷贝操作本身就是很隐晦的（编译器对这些过程做了专门的优化），且并不会影响
  // 程序的正确性，因此很少进入程序员的视野。

  cout << "============" << endl;

  // 那么当类中包含指针类型的成员变量，使用其它对象来初始化同类对象时，怎样才能避免深拷贝导致的效率问题呢？
  // C++11标准引入了解决方案，该标准中引入了右值引用的语法，借助它可以实现移动语义。
  // C++移动构造函数（移动语义的具体实现）:
  // 所谓移动语义，指的就是以移动而非深拷贝的方式初始化含有指针成员的类对象。简单的理解，移动语义指的就是将其他对象（通常是临时对象）拥有的内存资源“移为已用”。
  // 对于程序执行过程中产生的临时对象，往往只用于传递数据（没有其它的用处），并且会很快会被销毁。因此在使用临时对象初始化新对象时，我们可以将其包含的指针成员指
  // 向的内存资源直接移给新对象所有，无需再新拷贝一份，这大大提高了初始化的执行效率。
  demo1 b = get_demo1();
  // 使用临时对象初始化b对象过程中产生的2次拷贝操作，都转由移动构造函数完成。
  // g++ move.cpp -std=c++0x -fno-elide-constructors
  // construct!
  // move construct!
  // class destruct!
  // move construct!
  // class destruct!
  // class destruct!

  // 非const右值引用只能操作右值，程序执行结果中产生的临时对象（例如函数返回值、lambda 表达式等）既无名称也无法获取其存储地址，所以属于右值。
  // 当类中同时包含拷贝构造函数和移动构造函数时，如果使用临时对象初始化当前类的对象，编译器会优先调用移动构造函数来完成此操作。
  // 只有当类中没有合适的移动构造函数时，编译器才会退而求其次，调用拷贝构造函数。

  // 通常在类中自定义移动构造函数的同时，会再为其自定义一个适当的拷贝构造函数，由此当用户利用右值初始化类对象时，会调用移动构造函数；
  // 使用左值（非右值）初始化类对象时，会调用拷贝构造函数。

  // 如果使用左值初始化同类对象，但也想调用移动构造函数完成，有没有办法可以实现呢？
  // 默认情况下，左值初始化同类对象只能通过拷贝构造函数完成，如果想调用移动构造函数，则必须使用右值进行初始化。C++11标准中为了满足用户使用左值初始化同类对象时也
  // 通过移动构造函数完成的需求，新引入了std::move()函数，它可以将左值强制转换成对应的右值，由此便可以使用移动构造函数。

  return 0;
}