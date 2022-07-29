#include <iostream>
#include <memory>

// 开发中，常会遇到诸如程序运行中突然崩溃、程序运行所用内存越来越多最终不得不重启等问题，
// 这些问题往往都是内存资源管理不当造成的。比如：
// 1.内存资源已经被释放，但指向它的指针并没有改变指向（野指针），并且后续还在使用；
// 2.内存资源已经被释放，后期又试图再释放一次（重复释放同一块内存会导致程序运行崩溃）；
// 3.没有及时释放不再使用的内存资源，造成内存泄漏，程序占用的内存资源越来越多。

// 早在1959年前后，就有人提出了垃圾自动回收机制。
// 所谓垃圾，指的是那些不再使用或者没有任何指针指向的内存空间，
// 而回收则指的是将这些垃圾收集起来以便再次利用。
// 如今，垃圾回收机制已经大行其道，得到了Java、Python、C#、PHP等的支持。
// C++虽然从来没有公开得支持过垃圾回收机制，但C++98/03标准中，
// 支持使用auto_ptr智能指针来实现堆内存的自动回收；
// C++11在废弃auto_ptr的同时，
// 增添了unique_ptr、shared_ptr以及weak_ptr这3个智能指针来实现堆内存的自动回收。
// 所谓智能指针，可以从字面上理解为“智能”的指针。
// 具体来讲，智能指针和普通指针的用法是相似的，不同之处在于，
// 智能指针可以在适当时机自动释放分配的内存。
// 也就是说，使用智能指针可以很好地避免忘记释放内存而导致内存泄漏问题出现。
// 由此可见，C++也逐渐开始支持垃圾回收机制了，尽管目前支持程度还有限。

// C++智能指针底层是采用引用计数的方式实现的。
// 简单的理解，在申请堆内存空间的同时，会为其配备一个整形值（初值为1），
// 每当有新对象使用此堆内存时，该整形值+1；
// 反之，每当使用此堆内存的对象被释放时，该整形值减1。
// 当堆空间对应的整形值为0时，即表明不再有对象使用它，该堆空间就会被释放掉。

// C++11 shared_ptr智能指针:
// 每种智能指针都是以类模板的方式实现的，shared_ptr也不例外。
// shared_ptr<T>的定义位于<memory>头文件，并位于std命名空间中。
// 值得一提的是，和unique_ptr、weak_ptr不同之处在于，
// 多个shared_ptr智能指针可以共同使用同一块堆内存。
// 并且，由于该类型智能指针在实现上采用的是引用计数机制，
// 即便有一个shared_ptr指针放弃了堆内存的使用权（引用计数减1），
// 也不会影响其他指向同一堆内存的shared_ptr指针（只有引用计数为0时，堆内存才被释放）。

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

namespace create {
// shared_ptr智能指针的创建, shared_ptr<T>类模板中，提供了多种实用的构造函数：
void func1() {
  // 1.构造出shared_ptr<T>类型的空智能指针，
  //   空的shared_ptr指针，其初始引用计数为0，而不是1：
  std::shared_ptr<A> p1;           // 不传入任何实参
  std::shared_ptr<A> p2(nullptr);  // 传入空指针nullptr
  std::cout << std::boolalpha << "p1 is Empty: " << (p1 == nullptr)
            << ", count: " << p1.use_count() << std::endl;
  std::cout << std::boolalpha << "p2 is Empty: " << (p2 == NULL)
            << ", count: " << p2.use_count() << std::endl;

  // p1 is Empty: true, count: 0
  // p2 is Empty: true, count: 0
}

void func2() {
  // 2.在构建shared_ptr智能指针，也可以明确其指向：
  std::shared_ptr<A> p1(new A(3));
  // 同时，C++11标准中还提供了std::make_shared<T>模板函数，可以初始化shared_ptr：
  std::shared_ptr<A> p2 = std::make_shared<A>(5);  // 与p1是完全相同的
  std::cout << std::boolalpha << "p1 is Empty: " << (p1 == nullptr)
            << ", count: " << p1.use_count() << std::endl;
  std::cout << std::boolalpha << "p2 is Empty: " << (p2 == NULL)
            << ", count: " << p2.use_count() << std::endl;

  // create A, i = 3
  // create A, i = 5
  // p1 is Empty: false, count: 1
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
  // p1、p2、p3都是shared_ptr类型的智能指针，因此可以用p1来初始化p2和p3，
  // 由于p1是左值，因此会调用拷贝构造函数。
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
  // 需要注意的是，如果p1，p2为空智能指针，则p3和p4也为空智能指针，其引用计数初始值为0；
  std::shared_ptr<A> p1;
  std::shared_ptr<A> p2(nullptr);
  std::cout << std::boolalpha << p1 << " " << p1.operator bool() << std::endl;
  std::cout << std::boolalpha << p2 << " " << p2.operator bool() << std::endl;
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
  // 4.调用移动构造函数
  std::shared_ptr<A> p1(new A());
  std::cout << "p1 count: " << p1.use_count() << std::endl;  // 1
  std::shared_ptr<A> p2(std::move(p1));
  std::cout << "p1 count: " << p1.use_count() << std::endl;  // 0
  std::cout << "p2 count: " << p2.use_count() << std::endl;  // 1
  std::shared_ptr<A> p3 = std::move(p1);  // p1已结移动到了p2，从p1移来了nullptr
  std::cout << "p1 count: " << p1.use_count() << std::endl;  // 0
  std::cout << "p2 count: " << p2.use_count() << std::endl;  // 1
  std::cout << "p3 count: " << p3.use_count() << std::endl;  // 0
  // 对于std::move(p1)来说，该函数会强制将p1转换成对应的右值，
  // 因此初始化p2和p3调用的是移动构造函数。
  // 另外和调用拷贝构造函数不同，p2和p3拥有了p1的堆内存，而p1则变成了空智能指针。
}

void testCreate() {
  func1();
  func2();
  func3();
  func4();
  func5();
}
}  // namespace create

namespace func {
// shared_ptr<T>模板类提供的成员方法:
// operator=() 	  重载赋值号，使得同一类型shared_ptr智能指针可以相互赋值。
// operator*() 	  重载*号，获取当前shared_ptr智能指针对象指向的数据。
// operator->()   重载->号，当智能指针指向的数据类型为自定义的结构体时，
//                通过->运算符可以获取其内部的指定成员。
// swap() 	      交2个相同类型shared_pt智能指针的内容。
// reset() 	      当函数没有实参时，该函数会使当前shared_ptr所指堆内存的引用计数减1，
//                同时将当前对象重置为一个空指针；
//                当为函数传递一个新申请的堆内存时，
//                则调用该函数的shared_ptr对象会获得该存储空间的所有权，并且引用计数的初始值为1。
// get() 	        获得shared_ptr对象内部包含的普通指针。
// use_count() 	  返回同当前shared_ptr对象（包括它）指向相同的所有shared_ptr对象的数量。
// unique() 	    判断当前shared_ptr对象指向的堆内存，是否不再有其它shared_ptr对象再指向它。
// operator bool()判断当前shared_ptr对象是否为空智能指针，如果是空指针，返回false；
//                反之，返回true。

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
}

void func2() {
  std::shared_ptr<A> p1 = std::make_shared<A>(66);
  std::shared_ptr<A> p2 = std::make_shared<A>(77);
  std::shared_ptr<A> p3(p2);
  std::cout << p1->GetI() << " (" << p1.use_count() << ")" << std::endl;
  std::cout << p2->GetI() << " (" << p2.use_count() << ")" << std::endl;
  std::cout << p3->GetI() << " (" << p3.use_count() << ")" << std::endl;
  p1.reset();  // p1引用计数减1，p1为nullptr
  std::cout << std::boolalpha << (p1 == nullptr) << std::endl;
  p2.reset(new A(88));
  std::cout << p2->GetI() << " (" << p2.use_count() << ")" << std::endl;
  std::cout << p3->GetI() << " (" << p3.use_count() << ")" << std::endl;

  // create A, i = 66
  // create A, i = 77
  // 66 (1)
  // 77 (2)
  // 77 (2)
  // delete A
  // true
  // create A, i = 88
  // 88 (1)
  // 77 (1)
  // delete A
  // delete A
}

void func3() {
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

void testFunc() {
  func1();
  func2();
  func3();
}
}  // namespace func

namespace deletor {
// 在初始化shared_ptr智能指针时，还可以自定义所指堆内存的释放规则，
// 引用计数为0时，优先调用自定义的释放规则。

// 自定义释放规则
void deleteA(A* p) {
  std::cout << "run func deletorA" << std::endl;
  delete[] p;
}

void testDeletor() {
  // 在某些场景中，自定义释放规则是很有必要的。
  // 比如，对于申请的动态数组来说，shared_ptr指针默认的释放规则是不支持释放数组的。
  // 只能自定义对应的释放规则，才能正确地释放申请的堆内存。

  // std::shared_ptr<A> p1(new A[3]);
  // create A
  // create A
  // create A
  // delete A
  // munmap_chunk(): invalid pointer
  // 已放弃 (核心已转储)

  // 对于申请的动态数组，释放规则可以使用C++11标准中提供的default_delete<T>模板类，
  // 也可以自定义释放规则：
  std::shared_ptr<A> p1(new A[2], std::default_delete<A[]>());
  p1.reset();
  // create A
  // create A
  // delete A
  // delete A

  // 初始化智能指针，并自定义释放规则：
  std::shared_ptr<A> p2(new A[2], deleteA);
  p2.reset();
  // create A
  // create A
  // run func deletorA
  // delete A
  // delete A

  // 借助lambda表达式
  std::shared_ptr<A> p3(new A[2], [](A* p) {
    std::cout << "run lambda deletor A[]" << std::endl;
    delete[] p;
  });
  p3.reset();
  // create A
  // create A
  // run lambda deletor A[]
  // delete A
  // delete A
}
}  // namespace deletor

namespace badcase {
void testBadcase() {
  // 同一普通指针不能同时为多个shared_ptr对象赋值，否则会导致程序发生异常:
  int* ptr = new int;
  std::shared_ptr<int> p1(ptr);
  // std::shared_ptr<int> p2(ptr);  // 错误，导致两次释放同一块内存
  // free(): double free detected in tcache 2

  // 没有定义下面的操作符
  // p1++;
  // p1--;
  // p1 = p1 + 1;
  // int a = p1[0];
}
}  // namespace badcase

namespace test {
A* getA() { return new A(); }
void func() {
  std::shared_ptr<A> p1(getA());  // 可以管理指针
  A* p2 = getA();                 // 内存泄漏

  // create A
  // create A
  // delete A
}
}  // namespace test

int main(int argc, char* argv[]) {
  if (argc < 2) {
    std::cout << argv[0] << " i [0 - 4]" << std::endl;
    return 0;
  }
  int type = argv[1][0] - '0';
  switch (type) {
    case 0:
      create::testCreate();
      break;
    case 1:
      func::testFunc();
      break;
    case 2:
      deletor::testDeletor();
      break;
    case 3:
      badcase::testBadcase();
      break;
    case 4:
      test::func();
      break;
    default:
      std::cout << "invalid type" << std::endl;
      break;
  }
}