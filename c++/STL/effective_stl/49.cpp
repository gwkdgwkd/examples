#include <algorithm>
#include <iostream>
#include <iterator>
#include <list>
#include <string>
#include <vector>

using namespace std;

// 学会分析与STL相关的编译器诊断信息

// 一些技巧，对分析与STL相关的诊断信息会有所帮助：
//  1 vector和string的迭代器通常就是指针，所以当错误地使用了iterator的时候，编译器的诊断信息中可能会引用到指针类型。
//    例如，如果代码中引用了vector<double>::iterator，那么编译器的诊断信息中极有可能会提交double*指针。
//  2 如果诊断信息中提高了back_insert_iterator、front_insert_iterator或者insert_iterator，则几乎总是意味着你错误地
//    调用了back_insert_iterator、front_insert_iterator或者insert_iterator。
//  3 类似地，如果诊断信息中提到了binder1st或者binder2nd，那么可能是错误地使用了binder1st或者binder2nd。
//  4 输出迭代器（如ostream_iterator、ostreambuf_iterator，以及那些由back_inserter、front_inserter和inserter函数返回的迭
//    代器）在赋值操作符内不完成其输出或者插入工作，所以，如果在使用这些迭代器的时候犯了错误，那么错误信息可能会提到与赋值操作符有关的内容。
//  5 如果得到的错误信息来源于某一个STL算法的内部实现，也许是你在调用算法的时候使用了错误的类型。例如，使用了不恰当的迭代器类型。
//  6 如果正在使用一个很常见的STL组件，比如vector、string或者for_each算法，但从错误信息来看，编译器好像对此一无所知，
//    那么可能是你没有包含相应的头文件。在一个STL平台上顺序编译，在移植到一个新的STL平台上时可能会出现这样的问题。

int main() {
  vector<int> v(10);
  // string s(10);  // string没有这样的带单个int实参的构造函数

  // 4
  // vector<string *> v;
  // copy(v.begin(), v.end(), ostream_iterator<string>(cout, "\n"));

  // 5
  //   list<int>::iterator i1, i2;  // 双向迭代器
  //   sort(i1, i2);                // 需要随机访问迭代器

  return 0;
}
