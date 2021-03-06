#include <iostream>

using namespace std;

// vector和string优先于动态分配的数组

// 当决定用new来动态分配内存时，意味着要承担以下责任：
// 1 确保以后会有人用delete来删除所分配的内存。否则导致资源泄露。
// 2 必须确保使用了正确的delete形式。如果使用了不正确的delete形式，结果不确定。某些平台会崩溃。
// 3 必须确保只delete了一次。如果一次分配多次delete，结果同样不确定。

// 每当要动态分配一个数组时，都应该考虑用vector和string来代替。一般情况下，类型是字符型用string，否则用vector。
// vector和string消除了上述的负担，它们自己管理内存。
// 当元素被加入到容器中时，它们的内存会增长；而当vector和string被析构时，它们的析构函数会自动析构容器中的元素并释放内存。
// vector和string是功能完全的STL系列容器，所以，凡是适合于序列容器的STL算法，都可以使用。
// 数组也能用STL算法，但数组没有提供begin、end及size这样的成员函数，也没有像iterator、reverse_iterator和value_type这样的嵌套定义。
// vector和string和旧代码集成（把数据传递给数组）通常不是一个问题。

// 如果string是以引用计数方式来实现的，而又运行在多线程环境中，并认为引用计数会影响效率，那么有三种可行的选择（都没有舍弃STL）：
// 1 检查库的实现，看看是否有可以禁止引用计数的方法，通常是通过改变某个预处理变量的值。这是不可移植的办法。
// 2 寻找或开发另一个不使用引用计数的string实现。
// 3 考虑使用vector<char>而不是string。vector的实现不允许使用引用计数，所以不会发生隐藏的多线程性能问题。
//   使用vector<char>，那么就舍弃了使用string的成员函数的机会，但大多数成员函数的功能可以通过STL算法来实现。

int main() { return 0; }