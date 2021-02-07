#include <iostream>
#include <memory>

using namespace std;

// 和shared_ptr、unique_ptr一样，weak_ptr智能指针也是以模板类的方式实现的。weak_ptr<T>（T为指针所指数据的类型）定义在<memory>头文件，并位于std命名空间中。

// C++11标准虽然将weak_ptr定位为智能指针的一种，但该类型指针通常不单独使用（没有实际用处），只能和shared_ptr类型指针搭配使用。甚至于，可以将weak_ptr类型指针视
// 为shared_ptr指针的一种辅助工具，借助weak_ptr类型指针， 我们可以获取shared_ptr指针的一些状态信息，比如有多少指向相同的shared_ptr指针、shared_ptr指针指向
// 的堆内存是否已经被释放等等。
// 当weak_ptr类型指针的指向和某一shared_ptr指针相同时，weak_ptr指针并不会使所指堆内存的引用计数加1；同样，当weak_ptr指针被释放时，之前所指堆内存的引用计数也不
// 会因此而减1。也就是说，weak_ptr类型指针并不会影响所指堆内存空间的引用计数。

// weak_ptr<T>模板类中没有重载*和->运算符，这也就意味着，weak_ptr类型指针只能访问所指的堆内存，而无法修改它。

int main() {
  // weak_ptr指针的创建:
  // 1)可以创建一个空weak_ptr指针:
  std::weak_ptr<int> wp1;

  // 2)凭借已有的weak_ptr指针，可以创建一个新的weak_ptr指针:
  std::weak_ptr<int> wp2(wp1);
  // 若wp1为空指针，则wp2也为空指针；反之，如果wp1指向某一shared_ptr指针拥有的堆内存，则wp2也指向该块存储空间（可以访问，但无所有权）。

  // 3)weak_ptr指针更常用于指向某一shared_ptr指针拥有的堆内存，因为在构建weak_ptr指针对象时，可以利用已有的shared_ptr指针为其初始化:
  std::shared_ptr<int> sp(new int);
  std::weak_ptr<int> wp3(sp);
  // wp3指针和sp指针有相同的指针。再次强调，weak_ptr类型指针不会导致堆内存空间的引用计数增加或减少。

  // weak_ptr模板类提供的成员方法:
  // operator=()  重载=赋值运算符，是的weak_ptr指针可以直接被weak_ptr或者shared_ptr类型指针赋值。
  // swap(x) 	    其中x表示一个同类型的weak_ptr类型指针，该函数可以互换2个同类型weak_ptr指针的内容。
  // reset() 	    将当前weak_ptr指针置为空指针。
  // use_count()  查看指向和当前weak_ptr指针相同的shared_ptr指针的数量。
  // expired() 	  判断当前weak_ptr指针为否过期（指针为空，或者指向的堆内存已经被释放）。
  // lock() 	    如果当前weak_ptr已经过期，则该函数会返回一个空的shared_ptr指针；反之，该函数返回一个和当前weak_ptr指向相同的shared_ptr指针。
  // weak_ptr<T>模板类没有重载*和->运算符，因此weak_ptr类型指针只能访问某一shared_ptr指针指向的堆内存空间，无法对其进行修改。

  std::shared_ptr<int> sp1(new int(10));
  std::shared_ptr<int> sp2(sp1);
  std::weak_ptr<int> wp(sp2);
  // 输出和wp同指向的shared_ptr类型指针的数量
  cout << wp.use_count() << endl;  // 2
  // 释放sp2
  sp2.reset();
  cout << wp.use_count() << endl;  // 1
  // 借助lock()函数，返回一个和wp同指向的shared_ptr类型指针，获取其存储的数据
  cout << *(wp.lock()) << endl;  // 10

  return 0;
}