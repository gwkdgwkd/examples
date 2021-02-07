#include <iostream>
#include <memory>

using namespace std;

// 开发中，常会遇到诸如程序运行中突然崩溃、程序运行所用内存越来越多最终不得不重启等问题，这些问题往往都是内存资源管理不当造成的。比如：
// 有些内存资源已经被释放，但指向它的指针并没有改变指向（成为了野指针），并且后续还在使用；
// 有些内存资源已经被释放，后期又试图再释放一次（重复释放同一块内存会导致程序运行崩溃）；
// 没有及时释放不再使用的内存资源，造成内存泄漏，程序占用的内存资源越来越多。

// 早在1959年前后，就有人提出了“垃圾自动回收”机制。所谓垃圾，指的是那些不再使用或者没有任何指针指向的内存空间，而“回收”则指的是将这些“垃圾”收集起来以便再次利用。
// 如今，垃圾回收机制已经大行其道，得到了诸多编程语言的支持，例如Java、Python、C#、PHP 等。而C++ 虽然从来没有公开得支持过垃圾回收机制，但C++98/03标准中，支持
// 使用auto_ptr智能指针来实现堆内存的自动回收；C++11在废弃auto_ptr的同时，增添了unique_ptr、shared_ptr以及weak_ptr这3个智能指针来实现堆内存的自动回收。
// 所谓智能指针，可以从字面上理解为“智能”的指针。具体来讲，智能指针和普通指针的用法是相似的，不同之处在于，智能指针可以在适当时机自动释放分配的内存。也就是说，使用
// 智能指针可以很好地避免“忘记释放内存而导致内存泄漏”问题出现。由此可见，C++也逐渐开始支持垃圾回收机制了，尽管目前支持程度还有限。

// C++智能指针底层是采用引用计数的方式实现的。简单的理解，智能指针在申请堆内存空间的同时，会为其配备一个整形值（初始值为1），每当有新对象使用此堆内存时，该整形值
// +1；反之，每当使用此堆内存的对象被释放时，该整形值减1。当堆空间对应的整形值为0 时，即表明不再有对象使用它，该堆空间就会被释放掉。

// C++11 shared_ptr智能指针:
// 每种智能指针都是以类模板的方式实现的，shared_ptr也不例外。shared_ptr<T>（其中T表示指针指向的具体数据类型）的定义位于<memory>头文件，并位于std命名空间中。
// 值得一提的是，和unique_ptr、weak_ptr不同之处在于，多个shared_ptr智能指针可以共同使用同一块堆内存。并且，由于该类型智能指针在实现上采用的是引用计数机制，即
// 便有一个shared_ptr指针放弃了堆内存的“使用权”（引用计数减 1），也不会影响其他指向同一堆内存的shared_ptr指针（只有引用计数为0时，堆内存才会被自动释放）。

//自定义释放规则
void deleteInt(int* p) { delete[] p; }

int main() {
  // shared_ptr智能指针的创建, shared_ptr<T>类模板中，提供了多种实用的构造函数:
  // 1)构造出shared_ptr<T>类型的空智能指针(空的shared_ptr指针，其初始引用计数为0，而不是1)：
  std::shared_ptr<int> p1;           // 不传入任何实参
  std::shared_ptr<int> p2(nullptr);  // 传入空指针nullptr

  // 2)在构建shared_ptr智能指针，也可以明确其指向:
  std::shared_ptr<int> p3(new int(10));
  // 同时，C++11标准中还提供了std::make_shared<T>模板函数，其可以用于初始化shared_ptr智能指针:
  std::shared_ptr<int> p4 = std::make_shared<int>(10);  // 与p3是完全相同的

  // 3)除此之外，shared_ptr<T>模板还提供有相应的拷贝构造函数和移动构造函数:
  // 调用拷贝构造函数
  std::shared_ptr<int> p5(p3);
  std::shared_ptr<int> p6 = p3;
  // p3和p56都是 shared_ptr类型的智能指针，因此可以用p3来初始化p56,由于p3是左值，因此会调用拷贝构造函数。
  // 需要注意的是，如果p3为空智能指针，则p56也为空智能指针，其引用计数初始值为0；反之，则表明p56和p3指向同一块堆内存，同时该堆空间的引用计数会加1。

  // 调用移动构造函数
  std::shared_ptr<int> p7(std::move(p4));
  std::shared_ptr<int> p8 = std::move(p4);
  // 对于std::move(p4)来说，该函数会强制将p4转换成对应的右值，因此初始化p78调用的是移动构造函数。
  // 另外和调用拷贝构造函数不同，用std::move(p4)初始化p78，会使得p78拥有了p4的堆内存，而p4则变成了空智能指针。

  // 同一普通指针不能同时为多个shared_ptr对象赋值，否则会导致程序发生异常:
  int* ptr = new int;
  std::shared_ptr<int> pp1(ptr);
  std::shared_ptr<int> pp2(ptr);  // 错误

  // 4)在初始化shared_ptr智能指针时，还可以自定义所指堆内存的释放规则，这样当堆内存的引用计数为0时，会优先调用我们自定义的释放规则:
  // 在某些场景中，自定义释放规则是很有必要的。比如，对于申请的动态数组来说，shared_ptr指针默认的释放规则是不支持释放数组的，只能自定义对应的释放规则，
  // 才能正确地释放申请的堆内存。
  // 对于申请的动态数组，释放规则可以使用C++11标准中提供的default_delete<T> 模板类，我们也可以自定义释放规则：
  // 指定default_delete作为释放规则
  std::shared_ptr<int> pq1(new int[10], std::default_delete<int[]>());
  // 初始化智能指针，并自定义释放规则
  std::shared_ptr<int> pq2(new int[10], deleteInt);
  // 借助lambda表达式，我们还可以像如下这样初始化pq1，它们是完全相同的：
  std::shared_ptr<int> pq3(new int[10], [](int* p) { delete[] p; });

  // shared_ptr<T>模板类提供的成员方法:
  // operator=() 	  重载赋值号，使得同一类型 shared_ptr智能指针可以相互赋值。
  // operator*() 	  重载*号，获取当前shared_ptr智能指针对象指向的数据。
  // operator->()   重载->号，当智能指针指向的数据类型为自定义的结构体时，通过->运算符可以获取其内部的指定成员。
  // swap() 	      交2个相同类型shared_pt智能指针的内容。
  // reset() 	      当函数没有实参时，该函数会使当前shared_ptr所指堆内存的引用计数减1，同时将当前对象重置为一个空指针；
  //                当为函数传递一个新申请的堆内存时，则调用该函数的shared_ptr对象会获得该存储空间的所有权，并且引用计数的初始值为1。
  // get() 	        获得shared_ptr对象内部包含的普通指针。
  // use_count() 	  返回同当前shared_ptr对象（包括它）指向相同的所有shared_ptr对象的数量。
  // unique() 	    判断当前shared_ptr对象指向的堆内存，是否不再有其它shared_ptr对象再指向它。
  // operator bool()判断当前shared_ptr对象是否为空智能指针，如果是空指针，返回false；反之，返回true。
  // 除此之外，C++11标准还支持同一类型的shared_ptr对象，或者shared_ptr和nullptr之间，进行==，!=，<，<=，>，>=运算。

  // 构建2个智能指针
  std::shared_ptr<int> pw1(new int(10));
  std::shared_ptr<int> pw2(pw1);
  // 输出pw2指向的数据
  cout << *pw2 << endl;  // 10
  pw1.reset();           // 引用计数减1,p1为空指针
  if (pw1) {
    cout << "p1 不为空" << endl;
  } else {
    cout << "p1 为空" << endl;  // p1 为空
  }
  // 以上操作，并不会影响pw2
  cout << *pw2 << endl;  // 10
  // 判断当前和pw2同指向的智能指针有多少个
  cout << pw2.use_count() << endl;  // 1

  return 0;
}