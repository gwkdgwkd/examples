#include <iostream>
#include <memory>

// 作为智能指针的一种，unique_ptr指针自然也具备“在适当时机自动释放堆内存空间”的能力。
// 和shared_ptr指针最大的不同之处在于，unique_ptr指针指向的堆内存无法同其它unique_ptr共享，
// 也就是说，每个unique_ptr指针都独自拥有对其所指堆内存空间的所有权。
// 这也就意味着，每个unique_ptr指针指向的堆内存空间的引用计数，都只能为1，
// 一旦该unique_ptr指针放弃对所指堆内存空间的所有权，则该空间会被立即释放回收。
// unique_ptr智能指针是以模板类的形式提供的，unique_ptr<T>定义在<memory>头文件，并位于std命名空间中。

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
void testCreate() {
  // unique_ptr智能指针的创建:
  // 1.通过以下2种方式，可以创建出空的unique_ptr指针：
  std::unique_ptr<A> p1();
  std::unique_ptr<A> p2(nullptr);
  std::unique_ptr<A> p3 = nullptr;

  // 2.创建unique_ptr指针的同时，也可以明确其指向:
  std::unique_ptr<A> p4(new A(5));  // create A, i = 5
  // 和可以用make_shared<T>()模板函数初始化shared_ptr指针不同，
  // C++11标准中并没有为unique_ptr类型指针添加类似的模板函数。

  // 3.基于unique_ptr类型指针不共享各自拥有的堆内存:
  // std::unique_ptr<int> p5(p4);   // 错误，没有提供拷贝构造函数
  // std::unique_ptr<int> p6 = p4;  // 错误

  // 4.提供了移动构造函数和移动赋值操作符
  std::unique_ptr<A> p7(std::move(p4));  // 正确，调用移动构造函数
  std::unique_ptr<A> p8 = std::move(p4);

  std::cout << std::boolalpha << p4.operator bool() << std::endl;  // false
  std::cout << std::boolalpha << p7.operator bool() << std::endl;  // true
  std::cout << std::boolalpha << p8.operator bool() << std::endl;  // false

  int *ip = new int(9);
  std::unique_ptr<int> p9(ip);  // 不能两个unique_ptr引用同一个内存地址
  // std::unique_ptr<int> p10(ip);  // free(): double free detected in tcache 2
  // std::unique_ptr<int> p11(ip);
  // p11.release();  // 虽然不会崩溃，但是不应该这样用

  // delete A
}
}  // namespace create

namespace func {
// unique_ptr<T>模板类提供的成员方法:
// operator*() 	  获取当前unique_ptr指针指向的数据。
// operator->() 	重载->号，当智能指针指向的数据类型为自定义的结构体时，通过->运算符可以获取其内部的指定成员。
// operator =() 	重载了=赋值号，从而可以将nullptr或者一个右值unique_ptr指针直接赋值给当前同类型的unique_ptr指针。
// operator []() 	重载了[]运算符，当unique_ptr指针指向一个数组时，可以直接通过[]获取指定下标位置处的数据。
// get() 	        获取当前unique_ptr指针内部包含的普通指针。
// get_deleter() 	获取当前unique_ptr指针释放堆内存空间所用的规则。
// operator bool()unique_ptr指针可直接作为if语句的判断条件，以判断该指针是否为空，如果为空，则为false；反之为true。
// release() 	    释放当前unique_ptr指针对所指堆内存的所有权，但该存储空间并不会被销毁。
// reset(p) 	    其中p表示一个普通指针，如果p为nullptr，则当前unique_ptr也变成空指针；
//                反之，则该函数会释放当前unique_ptr指针指向的堆内存（如果有），然后获取p所指堆内存的所有权（p为nullptr）。
// swap(x) 	      交换当前unique_ptr指针和同类型的x指针。
// 除此之外，C++11标准还支持同类型的unique_ptr指针之间，以及unique_ptr和nullptr之间，做==，!=，<，<=，>，>=运算。

void func1() {
  std::unique_ptr<A> p1(new A(6));             // create A, i = 6
  std::cout << (*p1).GetI() << std::endl;      // 6
  std::cout << p1->GetI() << std::endl;        // 6
  std::cout << p1.get()->GetI() << std::endl;  // 6
  // delete A
}

void func2() {
  std::unique_ptr<A> p1(new A(7));  // create A, i = 7
  A *tmp = p1.release();
  std::cout << tmp->GetI() << std::endl;                           // 7
  std::cout << std::boolalpha << p1.operator bool() << std::endl;  // false
  // delete tmp;  // release后，如果不用了，需要delete

  *tmp = 8;
  std::unique_ptr<A> p2(tmp);  // create A, i = 8
  // delete A
  std::cout << p2.operator bool() << std::endl;  // true
  std::cout << p2->GetI() << std::endl;          // 8
  // delete A
}

void func3() {
  std::unique_ptr<A> p1(new A(9));  // create A, i = 9
  A *tmp = p1.get();
  p1.reset(nullptr);                           // delete A
  std::cout << (p1 == nullptr) << std::endl;   // true
  std::cout << (tmp == nullptr) << std::endl;  // false
  std::cout << tmp->GetI() << std::endl;       // 0
  // delete tmp;  // free(): double free detected in tcache 2
}

void func4() {
  std::unique_ptr<A> p1(new A(10));      // create A, i = 10
  std::unique_ptr<A> p2(new A(11));      // create A, i = 11
  std::cout << p1->GetI() << std::endl;  // 10
  std::cout << p2->GetI() << std::endl;  // 11
  A *tmp = p2.release();                 // delete A
  p1.reset(tmp);
  std::cout << p1->GetI() << std::endl;       // 11
  std::cout << (p2 == nullptr) << std::endl;  // true
  p1.reset(p1.release());                     // delete A
}

void func5() {
  std::unique_ptr<A> p1(new A(12));      // create A, i = 12
  std::unique_ptr<A> p2(new A(13));      // create A, i = 13
  std::cout << p1->GetI() << std::endl;  // 12
  std::cout << p2->GetI() << std::endl;  // 13
  p1.swap(p2);
  std::cout << p1->GetI() << std::endl;  // 13
  std::cout << p2->GetI() << std::endl;  // 12
  // delete A
  // delete A
}

void testFunc() {
  func1();
  func2();
  func3();
  func4();
  func5();
}
}  // namespace func

namespace deletor {
// 自定义的释放规则
struct myDel {
  void operator()(A *p) { delete[] p; }
};

void myDelFunc(A *p) { delete[] p; }

void testDeletor() {
  // 默认情况下，unique_ptr指针采用std::default_delete<T>方法释放堆内存。
  // 当然，也可以自定义符合实际场景的释放规则。
  // 值得一提的是，和shared_ptr指针不同，为unique_ptr自定义释放规则，只能采用函数对象的方式。
  std::unique_ptr<A[], myDel> p1(new A[2], myDel());  // 和下面有啥不一样？
  std::unique_ptr<A, myDel> p2(new A[2], p1.get_deleter());  // 为啥不能用[]
  std::unique_ptr<A, std::default_delete<A[]>> p3(new A[2]);
  std::unique_ptr<A[], void (*)(A *)> p4(new A[2], [](A *p) { delete[] p; });
  std::unique_ptr<A[], void (*)(A *)> p5(new A[2], myDelFunc);

  // create A
  // create A
  // create A
  // create A
  // create A
  // create A
  // create A
  // create A
  // create A
  // create A
  p4.get_deleter()(p4.release());  // 不等函数退出时，手动删除
  // delete A
  // delete A
  std::cout << p1[1].GetI() << std::endl;  // 0
  // std::cout << p2[1].GetI() << std::endl;
  // delete A
  // delete A
  // delete A
  // delete A
  // delete A
  // delete A
  // delete A
  // delete A
}
}  // namespace deletor

int main(int argc, char *argv[]) {
  if (argc < 2) {
    std::cout << argv[0] << " i [0 - 2]" << std::endl;
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
    default:
      std::cout << "invalid type" << std::endl;
      break;
  }
}