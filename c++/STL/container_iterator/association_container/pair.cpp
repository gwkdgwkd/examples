#include <iostream>
#include <utility>

using namespace std;

// 考虑到“键值对”并不是普通类型数据，C++ STL标准库提供了pair类模板，其专门用来将2个普通元素first和second（可以是C++基本数据类型、结构体、类自定的类型）创建成
// 一个新元素<first,second>。通过其构成的元素格式不难看出，使用pair类模板来创建“键值对”形式的元素，再合适不过。
// pair类模板定义在<utility>头文件中

// 在C++11标准之前，pair类模板中提供了以下3种构造函数：
//  pair(); 默认构造函数，即创建空的pair对象
//  pair(const first_type& a, const second_type& b); 直接使用2个元素初始化成pair对象
//  template<class U, class V> pair (const pair<U,V>& pr);拷贝（复制）构造函数，即借助另一个pair对象，创建新的pair对象
// 在C++11标准中，在引入右值引用的基础上，pair类模板中又增添了如下2个构造函数：
//  template<class U, class V> pair (pair<U,V>&& pr); 移动构造函数
//  template<class U, class V> pair (U&& a, V&& b); 使用右值引用参数，创建pair对象
// 除此之外，C++11标准中pair类模板还新增加了如下一种构造函数,该构造pair类模板的方式很少用到：
//  pair (piecewise_construct_t pwc, tuple<Args1...> first_args, tuple<Args2...> second_args);

int main() {
  // 调用构造函数1，也就是默认构造函数
  pair<string, double> pair1;
  // 调用第2种构造函数
  pair<string, string> pair2("STL教程", "http://c.biancheng.net/stl/");
  // 调用拷贝构造函数
  pair<string, string> pair3(pair2);
  // 调用移动构造函数
  // make_pair()函数，它也是<utility>头文件提供的，其功能是生成一个pair对象。
  // 因此，当将make_pair()函数的返回值（是一个临时对象）作为参数传递给pair()构造函数时，其调用的是移动构造函数，而不是拷贝构造函数。
  pair<string, string> pair4(
      make_pair("C++教程", "http://c.biancheng.net/cplus/"));
  // pair <string, string> pair4 = make_pair("C++教程", "http://c.biancheng.net/cplus/");  // 与上面的写法等价
  // 调用第5种构造函数
  pair<string, string> pair5(string("Python教程"),
                             string("http://c.biancheng.net/python/"));

  cout << "pair1: " << pair1.first << " " << pair1.second << endl;
  cout << "pair2: " << pair2.first << " " << pair2.second << endl;
  cout << "pair3: " << pair3.first << " " << pair3.second << endl;
  cout << "pair4: " << pair4.first << " " << pair4.second << endl;
  cout << "pair5: " << pair5.first << " " << pair5.second << endl;
  // pair1:  0
  // pair2: STL教程 http://c.biancheng.net/stl/
  // pair3: STL教程 http://c.biancheng.net/stl/
  // pair4: C++教程 http://c.biancheng.net/cplus/
  // pair5: Python教程 http://c.biancheng.net/python/

  // C++11还允许我们手动为pair1对象赋值
  pair<string, string> pair6;
  pair6.first = "Java教程";
  pair6.second = "http://c.biancheng.net/java/";
  cout << "new pair6: " << pair6.first << " " << pair6.second << endl;
  // new pair6: Java教程 http://c.biancheng.net/java/

  // <utility>头文件中除了提供创建pair对象的方法之外，还为pair对象重载了 <、<=、>、>=、==、!= 这6的运算符，
  // 其运算规则是：对于进行比较的2个pair对象，先比较pair.first元素的大小，如果相等则继续比较pair.second元素的大小。
  // 对于进行比较的2个pair 对象，其对应的键和值的类型比较相同，否则将没有可比性，同时编译器提示没有相匹配的运算符，即找不到合适的重载运算符。
  pair<string, int> pair11("STL教程", 20);
  pair<string, int> pair22("C++教程", 20);
  pair<string, int> pair33("C++教程", 30);
  // pair1和pair2的key不同，value相同
  if (pair11 != pair22) {
    cout << "pair11 != pair22" << endl;
  }
  // pair2和pair3的key相同，value不同
  if (pair22 != pair33) {
    cout << "pair23 != pair33" << endl;
  }
  // pair11 != pair22
  // pair23 != pair33

  // pair类模板还提供有一个swap()成员函数，能够互换2个pair对象的键值对，其操作成功的前提是这2个pair对象的键和值的类型要相同。
  pair<string, int> pair111("pair", 10);
  pair<string, int> pair222("pair2", 20);

  pair111.swap(pair222);
  cout << "pair1: " << pair111.first << " " << pair111.second << endl;
  cout << "pair2: " << pair222.first << " " << pair222.second << endl;
  // pair1: pair2 20
  // pair2: pair 10

  return 0;
}