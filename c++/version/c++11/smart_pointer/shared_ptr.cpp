#include <iostream>
#include <memory>

// 开发中，常会遇到诸如程序运行中突然崩溃、内存越来越多最终不得不重启等问题，
// 这些问题往往都是内存资源管理不当造成的，比如：
// 1.内存资源已经被释放，但指向它的指针并未改变（野指针），后续还在使用；
// 2.内存资源已经被释放，又试图再释放一次（重复释放同一块内存会导致崩溃）；
// 3.没有及时释放不再使用的内存资源，造成内存泄漏，程序占用的内存越来越多。

// 早在1959年前后，就有人提出了垃圾自动回收机制。
// 所谓垃圾，指的是那些不再使用或者没有任何指针指向的内存空间，
// 而回收则指的是将这些垃圾收集起来以便再次利用。
// 如今，垃圾回收机制已经大行其道，得到了Java、Python、C#、PHP等的支持。
// C++虽然从来没有公开得支持过垃圾回收机制，但C++98/03标准中，
// 支持使用auto_ptr智能指针来实现堆内存的自动回收，C++11在废弃auto_ptr的同时，
// 增添了unique_ptr、shared_ptr以及weak_ptr来实现堆内存的自动回收。
// 具体来讲，智能指针和普通指针的用法是相似的，不同之处在于，
// 智能指针可以在适当时机自动释放分配的内存。
// 也就是说，使用智能指针可以很好地避免忘记释放内存而导致内存泄漏问题出现。
// 由此可见，C++也逐渐开始支持垃圾回收机制了，尽管目前支持程度还有限。

// C++智能指针底层是采用引用计数的方式实现的。
// 简单的理解，在申请堆内存空间的同时，会为其配备一个整形值（初值为1），
// 每当有新对象使用此堆内存时，该值+1；
// 反之，每当使用此堆内存的对象被释放时，该整形值减1。
// 当堆空间对应的值为0时，即表明不再有对象使用它，该堆空间就会被释放掉。

// C++11 shared_ptr智能指针：
// 每种智能指针都是以类模板的方式实现的，shared_ptr也不例外。
// shared_ptr<T>的定义位于<memory>头文件，并位于std命名空间中。
// 值得一提的是，和unique_ptr、weak_ptr不同之处在于，
// 多个shared_ptr智能指针可以共同使用同一块堆内存。
// 并且，由于该类型智能指针在实现上采用的是引用计数机制，
// 即便有一个shared_ptr指针放弃了堆内存的使用权（引用计数减1），
// 也不会影响其他指向同一堆内存的shared_ptr指针，只有计数为0时，才被释放。

class A {
 public:
  A() {
    i_ = 0;
    std::cout << "create A" << std::endl;
  }
  A(int i) : i_(i) { std::cout << "create A, i = " << i << std::endl; }
  ~A() { std::cout << "delete A" << std::endl; }
  int GetI() { return i_; }

 private:
  int i_;
};

namespace n1 {
// shared_ptr智能指针的创建，shared_ptr<T>类模板中，提供了多种构造函数：
void func1() {
  // 1.构造出shared_ptr<T>类型的空智能指针，其初始引用计数为0，而不是1：
  std::shared_ptr<A> p1;  // 不传入任何实参
  std::cout << std::boolalpha << "p1 is Empty: " << (p1 == nullptr)
            << ", count: " << p1.use_count() << std::endl;
  std::shared_ptr<A> p2(nullptr);  // 传入空指针nullptr
  std::cout << std::boolalpha << "p2 is Empty: " << (p2 == NULL)
            << ", count: " << p2.use_count() << std::endl;

  // p1 is Empty: true, count: 0
  // p2 is Empty: true, count: 0
}

void func2() {
  // 2.在构建shared_ptr智能指针，也可以明确其指向：
  std::shared_ptr<A> p1(new A(3));
  std::cout << std::boolalpha << "p1 is Empty: " << (p1 == nullptr)
            << ", count: " << p1.use_count() << std::endl;
  // create A, i = 3
  // p1 is Empty: false, count: 1

  // C++11标准中还提供了std::make_shared<T>，可以初始化shared_ptr：
  std::shared_ptr<A> p2 = std::make_shared<A>(5);  // 与p1是完全相同的
  std::cout << std::boolalpha << "p2 is Empty: " << (p2 == NULL)
            << ", count: " << p2.use_count() << std::endl;
  // create A, i = 5
  // p2 is Empty: false, count: 1

  // delete A
  // delete A
}

void func3() {
  // 3.除此之外，shared_ptr<T>模板还提供有相应的拷贝构造函数：
  std::shared_ptr<A> p1(new A());
  std::shared_ptr<A> p2(p1);
  std::cout << std::boolalpha << p1 << " " << p1.operator bool() << std::endl;
  std::cout << std::boolalpha << p2 << " " << p2.operator bool() << std::endl;
  std::cout << "p2 count: " << p2.use_count() << std::endl;
  std::shared_ptr<A> p3 = p1;
  std::cout << "p1 count: " << p1.use_count() << std::endl;
  std::cout << "p2 count: " << p2.use_count() << std::endl;
  std::cout << "p3 count: " << p3.use_count() << std::endl;
  // p1、p2、p3都是shared_ptr类型的智能指针，
  // 因此可以用p1来初始化p2和p3，由于p1是左值，因此会调用拷贝构造函数。
  // p1、p2和p3指向同一块堆内存，同时该堆空间的引用计数会加1。
  std::cout << "p1 == p2 : " << (p1 == p2) << std::endl;
  std::cout << "p2 == p3 : " << (p2 == p3) << std::endl;
  std::cout << "p1 == p3 : " << (p1.get() == p3.get()) << std::endl;

  // create A
  // 0x55a1d086d2e0 true
  // 0x55a1d086d2e0 true
  // p2 count: 2
  // p1 count: 3
  // p2 count: 3
  // p3 count: 3
  // p1 == p2 : true
  // p2 == p3 : true
  // p1 == p3 : true
  // delete A
}

void func4() {
  std::shared_ptr<A> p1;
  std::shared_ptr<A> p2(nullptr);
  std::cout << std::boolalpha << p1 << " " << p1.operator bool() << std::endl;
  std::cout << std::boolalpha << p2 << " " << p2.operator bool() << std::endl;
  // p1，p2为空智能指针，则p3和p4也为空智能指针，其引用计数初始值为0：
  std::shared_ptr<A> p3(p1);
  std::shared_ptr<A> p4 = p2;
  std::cout << "p1 count: " << p1.use_count() << std::endl;
  std::cout << "p2 count: " << p2.use_count() << std::endl;
  std::cout << "p3 count: " << p3.use_count() << std::endl;
  std::cout << "p4 count: " << p4.use_count() << std::endl;

  // 0 false
  // 0 false
  // p1 count: 0
  // p2 count: 0
  // p3 count: 0
  // p4 count: 0
}

void func5() {
  // 4.调用移动构造函数：
  std::shared_ptr<A> p1(new A());
  std::cout << "p1 count: " << p1.use_count() << std::endl;  // 1
  std::shared_ptr<A> p2(std::move(p1));
  std::cout << "p1 count: " << p1.use_count() << std::endl;  // 0
  std::cout << "p2 count: " << p2.use_count() << std::endl;  // 1
  std::shared_ptr<A> p3 = std::move(p1);
  // p1已经移动到了p2，p3从p1移来了nullptr：
  std::cout << "p1 count: " << p1.use_count() << std::endl;  // 0
  std::cout << "p2 count: " << p2.use_count() << std::endl;  // 1
  std::cout << "p3 count: " << p3.use_count() << std::endl;  // 0
}

void func6() {
  std::shared_ptr<A> p(new A());
  std::shared_ptr<A> p1 = p;
  std::cout << "p count: " << p.use_count() << std::endl;    // 2
  std::cout << "p1 count: " << p1.use_count() << std::endl;  // 2
  p = nullptr;
  std::cout << "p count: " << p.use_count() << std::endl;    // 0
  std::cout << "p1 count: " << p1.use_count() << std::endl;  // 1
}
}  // namespace n1

namespace n2 {
// shared_ptr<T>模板类提供的成员方法：
// operator=() 	  重载赋值号，使得同一类型shared_ptr智能指针可以相互赋值。
// operator*() 	  重载*号，获取当前shared_ptr智能指针对象指向的数据。
// operator->()   重载->号，当智能指针指向的数据类型为自定义的结构体时，
//                通过->运算符可以获取其内部的指定成员。
// swap() 	      交2个相同类型shared_pt智能指针的内容。
// reset() 	      当函数没有实参时会使shared_ptr所指堆内存的引用计数减1，
//                同时将当前对象重置为一个空指针；当为函数传递一个新申请的堆内存时，
//                则调用shared_ptr的对象会获得该存储空间的所有权，并且引用计数为1。
// get() 	        获得shared_ptr对象内部包含的普通指针。
// use_count() 	  返回同当前shared_ptr对象，包括它，
//                指向相同的所有shared_ptr对象的数量。
// unique() 	    判断当前shared_ptr对象指向的堆内存，
//                是否不再有其它shared_ptr对象再指向它。
// operator bool()判断当前shared_ptr对象是否为空智能指针，
//                如果是空指针，返回false，反之，返回true。

// 除此之外，C++11标准还支持同一类型的shared_ptr对象，
// 或者shared_ptr和nullptr之间，进行==，!=，<，<=，>，>=运算。

void func1() {
  std::shared_ptr<A> p1(new A(6));
  std::cout << p1->GetI() << std::endl;
  std::cout << (*p1).GetI() << std::endl;
  std::cout << p1.get()->GetI() << std::endl;
  std::cout << (*p1.get()).GetI() << std::endl;

  // create A, i = 6
  // 6
  // 6
  // 6
  // 6
  // delete A

  // 没有定义下面的操作符：
  // p1++;
  // p1--;
  // p1 = p1 + 1;
  // A a = p1[0];
}

void func2() {
  std::shared_ptr<A> p1 = std::make_shared<A>(22);
  std::shared_ptr<A> p2(p1);
  std::cout << p1->GetI() << " (" << p1.use_count() << ")" << std::endl;
  p2.reset();  // p1引用计数减1，p1为nullptr
  std::cout << std::boolalpha << (p2 == nullptr) << std::endl;
  std::cout << p1->GetI() << " (" << p1.use_count() << ")" << std::endl;

  // create A, i = 22
  // 22 (2)
  // true
  // 22 (1)
  // delete A
}

void func3() {
  std::shared_ptr<A> p1 = std::make_shared<A>(33);
  std::shared_ptr<A> p2(p1);
  std::shared_ptr<A> p3 = p1;
  std::cout << p1->GetI() << " (" << p1.use_count() << ")" << std::endl;
  std::cout << p2->GetI() << " (" << p2.use_count() << ")" << std::endl;
  p1.reset(new A(34));
  std::cout << p1->GetI() << " (" << p1.use_count() << ")" << std::endl;
  std::cout << p2->GetI() << " (" << p2.use_count() << ")" << std::endl;

  // create A, i = 33
  // 33 (3)
  // 33 (3)
  // create A, i = 34
  // 34 (1)
  // 33 (2)
  // delete A
  // delete A
}

void func4() {
  std::shared_ptr<A> p1 = std::make_shared<A>(66);
  std::shared_ptr<A> p2 = std::make_shared<A>(77);
  std::cout << "p1 : " << p1.get()->GetI() << std::endl;
  std::cout << "p2 : " << p2.get()->GetI() << std::endl;
  p1.swap(p2);
  std::cout << "p1 : " << p1.get()->GetI() << std::endl;
  std::cout << "p2 : " << p2.get()->GetI() << std::endl;

  // create A, i = 66
  // create A, i = 77
  // p1 : 66
  // p2 : 77
  // p1 : 77
  // p2 : 66
  // delete A
  // delete A
}

void func5() {
  std::shared_ptr<A> p1 = std::make_shared<A>(99);
  std::shared_ptr<A> p2 = p1;
  std::cout << p1->GetI() << " (" << p1.use_count()
            << "), unique : " << std::boolalpha << p1.unique() << std::endl;
  p2.reset();
  std::cout << p1->GetI() << " (" << p1.use_count()
            << "), unique : " << std::boolalpha << p1.unique() << std::endl;

  // 99 (2), unique : false
  // 99 (1), unique : true
}
}  // namespace n2

namespace n3 {
// 在初始化shared_ptr智能指针时，还可以自定义所指堆内存的释放规则，
// 引用计数为0时，优先调用自定义的释放规则。

void func1() {
  // 在某些场景中，自定义释放规则是很有必要的，比如，对于动态数组来说，
  // shared_ptr指针默认的释放规则是不支持释放数组。
  // 只能自定义对应的释放规则，才能正确地释放申请的堆内存。

  std::shared_ptr<A> p(new A[2]);

  // create A
  // create A
  // delete A
  // munmap_chunk(): invalid pointer
  // 已放弃 (核心已转储)
}

void func2() {
  // 对于申请的动态数组，可以使用C++11中提供的default_delete<T>模板类：
  std::shared_ptr<A> p(new A[2], std::default_delete<A[]>());

  // create A
  // create A
  // delete A
  // delete A
}

void deleteA(A* p) {  // 自定义释放规则
  std::cout << "run func deletorA" << std::endl;
  delete[] p;
}
void func3() {
  // 对于申请的动态数组，还可以使用定义释放规则：
  std::shared_ptr<A> p(new A[2], deleteA);

  // create A
  // create A
  // run func deletorA
  // delete A
  // delete A
}

void func4() {
  // 使用lambda表达式：
  std::shared_ptr<A> p(new A[2], [](A* p) {
    std::cout << "run lambda deletor A[]" << std::endl;
    delete[] p;
  });

  // create A
  // create A
  // run lambda deletor A[]
  // delete A
  // delete A
}
}  // namespace n3

namespace n4 {
void func1() {
  // 一个普通指针不能同时为多个shared_ptr对象赋值，会导致程序发生异常：
  int* ptr = new int;
  std::shared_ptr<int> p1(ptr);
  std::shared_ptr<int> p2(ptr);
  // 错误，导致两次释放同一块内存：
  // free(): double free detected in tcache 2
}

A* getA() { return new A(); }
void func2() {
  std::shared_ptr<A> p1(getA());  // 可以管理指针
  A* p2 = getA();                 // 内存泄漏

  // create A
  // create A
  // delete A
}
}  // namespace n4

namespace n5 {
class B {
 public:
  B() { std::cout << "B" << std::endl; }
  ~B() { std::cout << "~B" << std::endl; }
  virtual void show() = 0;
};
class D1 : public B {
 public:
  D1() { std::cout << "D1" << std::endl; }
  ~D1() { std::cout << "~D1" << std::endl; }
  void show() { std::cout << "D1::show" << std::endl; }
};
class D2 : public B {
 public:
  D2() { std::cout << "D2" << std::endl; }
  ~D2() { std::cout << "~D2" << std::endl; }
  void show() { std::cout << "D2::show" << std::endl; }
};

void func() {
  // shared_ptr用于多态：
  std::shared_ptr<B> p(new D1);
  // B
  // D1
  p->show();  // D1::show

  p = std::make_shared<D2>();
  // B
  // D2
  // ~D1
  // ~B
  p->show();  // D2::show

  // ~D2
  // ~B
}
}  // namespace n5

int main(int argc, char* argv[]) {
  if (argc < 2) {
    std::cout << argv[0] << " i [0 - 16]" << std::endl;
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
      n1::func5();
      n1::func6();
      break;
    case 5:
      n2::func1();
      break;
    case 6:
      n2::func2();
      break;
    case 7:
      n2::func3();
      break;
    case 8:
      n2::func4();
      break;
    case 9:
      n2::func5();
      break;
    case 10:
      n3::func1();
      break;
    case 11:
      n3::func2();
      break;
    case 12:
      n3::func3();
      break;
    case 13:
      n3::func4();
      break;
    case 14:
      n4::func1();
      break;
    case 15:
      n4::func2();
      break;
    case 16:
      n5::func();
      break;
    default:
      std::cout << "invalid type" << std::endl;
      break;
  }
}