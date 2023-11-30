#include <iostream>
#include <utility>

// 考虑到键值对并不是普通类型数据，C++ STL标准库提供了pair类模板，
// 用来将2个普通元素first和second创建成一个新元素<first,second>，
// 普通元素可以是基本数据类型、结构体或者类，通过其构成的元素格式不难看出，
// 使用pair类模板来创建键值对形式的元素，再合适不过，类模板定义在<utility>中。

// 在C++11标准之前，pair类模板中提供了以下3种构造函数：
// 1.pair();
//   默认构造函数，即创建空的pair对象。
// 2.pair(const first_type& a, const second_type& b);
//   直接使用2个元素初始化成pair对象。
// 3.template<class U, class V> pair(const pair<U,V>& pr);
//   拷贝（复制）构造函数。
// 4.在C++11标准中，在引入右值引用的基础上，pair类模板中又增添了如下2个构造函数：
//   template<class U, class V> pair(pair<U,V>&& pr); 移动构造函数
//   template<class U, class V> pair(U&& a, V&& b); 使用右值引用参数创建pair对象
// 5.除此之外，C++11标准中pair类模板还新增加了如下一种构造函数，
//   该构造pair类模板的方式很少用到：
//   pair(piecewise_construct_t pwc, tuple<Args1...> first_args,
//        tuple<Args2...> second_args);

int main() {
  // 调用构造函数默认构造函数：
  std::pair<std::string, double> pair1;

  // 调用第2种构造函数：
  std::pair<std::string, std::string> pair2("hello1", "world1");

  // 调用拷贝构造函数：
  std::pair<std::string, std::string> pair3(pair2);

  // 调用移动构造函数：
  // make_pair()函数，它也是<utility>头文件提供的，其功能是生成一个pair对象。
  // 因此，当将make_pair()的返回的临时对象，作为参数传递给pair()构造函数时，
  // 其调用的是移动构造函数，而不是拷贝构造函数。
  std::pair<std::string, std::string> pair4(std::make_pair("hello2", "world2"));
  std::pair<std::string, std::string> pair5 =
      std::make_pair("hello3", "world3");

  // 调用第5种构造函数：
  std::pair<std::string, std::string> pair6(std::string("Hello4"),
                                            std::string("world4"));

  std::cout << "pair1: " << pair1.first << " " << pair1.second << std::endl;
  std::cout << "pair2: " << pair2.first << " " << pair2.second << std::endl;
  std::cout << "pair3: " << pair3.first << " " << pair3.second << std::endl;
  std::cout << "pair4: " << pair4.first << " " << pair4.second << std::endl;
  std::cout << "pair5: " << pair5.first << " " << pair5.second << std::endl;
  std::cout << "pair6: " << pair6.first << " " << pair6.second << std::endl;
  // pair1:  0
  // pair2: hello1 world1
  // pair3: hello1 world1
  // pair4: hello2 world2
  // pair5: hello3 world3
  // pair6: Hello4 world4

  // C++11还允许手动为pair对象赋值：
  std::pair<std::string, std::string> pair7;
  pair7.first = "hello5";
  pair7.second = "world5";
  std::cout << pair7.first << " " << pair7.second << std::endl;
  // hello5 world5

  // <utility>还为pair对象重载了<、<=、>、>=、==、!=这6的运算符，其运算规则是：
  // 对于进行比较的2个pair对象，先比较pair.first元素的大小，
  // 如果相等则继续比较pair.second元素的大小。
  // 对于进行比较的2个pair对象，其对应的键和值的类型比较相同，否则将没有可比性，
  // 同时编译器提示没有相匹配的运算符，即找不到合适的重载运算符。
  std::pair<std::string, int> pair8("STL教程", 20);
  std::pair<std::string, int> pair9("C++教程", 20);
  std::pair<std::string, int> pair10("C++教程", 30);
  if (pair8 != pair9) {  // key不同，value相同
    std::cout << "pair8 != pair9" << std::endl;
  }
  if (pair9 != pair10) {  // key相同，value不同
    std::cout << "pair9 != pair10" << std::endl;
  }
  // pair8 != pair9
  // pair9 != pair10

  // pair类模板还提供有一个swap()成员函数，能够互换2个pair对象的键值对，
  // 其操作成功的前提是这2个pair对象的键和值的类型要相同。
  std::pair<std::string, int> pair11("pair", 10);
  std::pair<std::string, int> pair12("pair2", 20);

  pair11.swap(pair12);
  std::cout << pair11.first << " " << pair11.second << std::endl;  // pair2 20
  std::cout << pair12.first << " " << pair12.second << std::endl;  // pair 10

  return 0;
}