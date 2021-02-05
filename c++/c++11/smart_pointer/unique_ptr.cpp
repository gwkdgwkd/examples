#include <iostream>
#include <memory>

using namespace std;

// 作为智能指针的一种，unique_ptr指针自然也具备“在适当时机自动释放堆内存空间”的能力。和shared_ptr指针最大的不同之处在于，unique_ptr指针指向的堆内
// 存无法同其它unique_ptr共享，也就是说，每个unique_ptr指针都独自拥有对其所指堆内存空间的所有权。
// 这也就意味着，每个unique_ptr指针指向的堆内存空间的引用计数，都只能为1，一旦该unique_ptr指针放弃对所指堆内存空间的所有权，则该空间会被立即释放回收。
// unique_ptr智能指针是以模板类的形式提供的，unique_ptr<T>（T为指针所指数据的类型）定义在<memory>头文件，并位于std命名空间中。

// 自定义的释放规则
struct myDel {
  void operator()(int *p) { delete p; }
};

int main() {
  // unique_ptr智能指针的创建:
  // 1)通过以下2种方式，可以创建出空的unique_ptr指针：
  std::unique_ptr<int> p1();
  std::unique_ptr<int> p2(nullptr);

  // 2)创建unique_ptr指针的同时，也可以明确其指向:
  std::unique_ptr<int> p3(new int);
  // 和可以用make_shared<T>()模板函数初始化shared_ptr指针不同，C++11标准中并没有为unique_ptr类型指针添加类似的模板函数。

  // 3)基于unique_ptr类型指针不共享各自拥有的堆内存，因此C++11标准中的unique_ptr模板类没有提供拷贝构造函数，只提供了移动构造函数:
  std::unique_ptr<int> p4(new int);
  // std::unique_ptr<int> p5(p4);  // 错误，堆内存不共享
  std::unique_ptr<int> p6(std::move(p4));  // 正确，调用移动构造函数
  // 对于调用移动构造函数的p4和p6来说，p6将获取p4所指堆空间的所有权，而p4将变成空指针（nullptr）。

  // 4)默认情况下，unique_ptr指针采用std::default_delete<T>方法释放堆内存。当然，我们也可以自定义符合实际场景的释放规则。值得一提的是，
  // 和shared_ptr指针不同，为unique_ptr自定义释放规则，只能采用函数对象的方式。
  std::unique_ptr<int, myDel> p7(new int);
  std::unique_ptr<int, myDel> p8(new int, myDel());

  // unique_ptr<T>模板类提供的成员方法:
  // operator*() 	获取当前unique_ptr指针指向的数据。
  // operator->() 	重载->号，当智能指针指向的数据类型为自定义的结构体时，通过->运算符可以获取其内部的指定成员。
  // operator =() 	重载了=赋值号，从而可以将nullptr或者一个右值unique_ptr指针直接赋值给当前同类型的unique_ptr指针。
  // operator []() 	重载了[]运算符，当unique_ptr指针指向一个数组时，可以直接通过[]获取指定下标位置处的数据。
  // get() 	        获取当前unique_ptr指针内部包含的普通指针。
  // get_deleter() 	获取当前unique_ptr指针释放堆内存空间所用的规则。
  // operator bool()unique_ptr指针可直接作为if语句的判断条件，以判断该指针是否为空，如果为空，则为false；反之为true。
  // release() 	    释放当前unique_ptr指针对所指堆内存的所有权，但该存储空间并不会被销毁。
  // reset(p) 	    其中p表示一个普通指针，如果p为nullptr，则当前unique_ptr也变成空指针；
  //                反之，则该函数会释放当前unique_ptr指针指向的堆内存（如果有），然后获取p所指堆内存的所有权（p为nullptr）。
  // swap(x) 	    交换当前unique_ptr指针和同类型的x指针。
  // 除此之外，C++11标准还支持同类型的unique_ptr指针之间，以及unique_ptr和nullptr之间，做==，!=，<，<=，>，>=运算。

  std::unique_ptr<int> pp(new int);
  *pp = 10;
  // q接收pp释放的堆内存
  int *q = pp.release();
  cout << *q << endl;  // 10
  // 判断pp是否为空指针
  if (pp) {
    cout << "pp is not nullptr" << endl;
  } else {
    cout << "pp is nullptr" << endl;  // pp is nullptr
  }
  std::unique_ptr<int> pq;
  // pq获取q的所有权
  pq.reset(q);
  cout << *pq << endl;  // 10

  return 0;
}