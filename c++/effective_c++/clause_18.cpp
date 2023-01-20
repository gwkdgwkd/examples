#include <iostream>
#include <memory>

using namespace std;

// 让接口容易被正确使用，不易被误用

// C++在接口之海漂浮。
// function接口、class接口、template接口，
// 每一种接口都是客户与你的代码互动的手段。
// 理论上，如果客户企图使用某个接口而却没有获得他所预期的行为，
// 这个代码应该编译不通过；
// 如果通过了，那就是客户想要的。

// 欲开发一个容易被正确使用，不易被误用的接口，
// 首先必须考虑客户可能出什么样的错误。
class Date1 {
 public:
  Date1(int month, int day, int year) {}
  // ...
  // 1.也许会以错误的次序传递参数：Date(30,3,1995);
  // 2.可能传递无效的月份或天数：Date(2,30,1995);
};

// 导入简单的外覆类型来区别天数、月份和年份：
struct Day {
  explicit Day(int d) : val(d) {}
  int val;
};
struct Month {
  explicit Month(int m) : val(m) {}
  int val;
};
struct Year {
  explicit Year(int y) : val(y) {}
  int val;
};
class Date2 {
 public:
  Date2(const Month& m, const Day& d, const Year& y) {}
  // ...
};
// 令Day、Month和Year成为成熟且经过充分锻炼的class，
// 并封装其内部数据，比简单的struct要好。
// 但struct也已经足够示范：
// 明智而审慎地导入新类型对预防接口被误用有神奇疗效。

// 一旦正确的类型已确定，限制其值有时候是通情达理的。
// 一个办法是使用enums，但不具备类型安全性。
class Month1 {
 public:
  static Month1 Jan() { return Month1(1); }
  static Month1 Feb() { return Month1(2); }
  static Month1 Mar() { return Month1(3); }
  static Month1 Dec() { return Month1(12); }

 private:
  explicit Month1(int m) : val(m) {}
  int val;
};
class Date3 {
 public:
  Date3(const Month1& m, const Day& d, const Year& y) {}
  // ...
};
// 如果以函数替换对象，表现某个特定月份让你诧异，
// 或许是因为你忘记了non-local static对象的初始化次序可能出问题（条款4）。

// 预防客户错误的另一个办法是，限制类型内什么事可做，
// 什么事不能做，常见的限制是加上const。
// 除非有好的理由，否则应该尽量令class的行为与内置type一致。
// 避免无端与内置类型不兼容，真正的理由是为了提供行为一致的接口，
// STL容器的接口是否一致。

// 任何接口如果要求客户必须记得做某些事情，
// 就是有着不正确使用的倾向，因为客户可能会忘记做那件事。
// 没使用对象管理资源时，为避免资源泄露，要求必须释放资源。
// 给客户两个犯错的机会：
// 没有删除指针，或删除同一指针超过一次。
// 许多时候，较好的接口设计原则是先发制人，直接返回一个智能指针，
// 强迫客户将返回值保存在shared_ptr内，消除了忘记释放资源的可能性。

// shared_ptr允许当智能指针被创建时，
// 指定一个资源释放函数（删除器）绑定于智能指针上，
// auto_ptr就没有这个功能。
// shared_ptr提供的某个构造函数接受两个实参：
// 一个是被管理的指针，另一个是引用次数为0时将被调用的删除器。

// shared_ptr有一个特别好的性质是：
// 会自动使用它的每个指针专属的删除器，因而消除潜在的客户错误：
// 所谓的cross-DLL problem。
// 这个问题发生于对象在动态链接库中被new创建，却在另一个库中被delete。
// 在许多平台上，会导致运行期错误。
// shared_ptr没有这个问题，
// 因为缺省的删除器是来自shared_ptr诞生的那个库的delete。

// 常见的shared_ptr来自Boost。
// shared_ptr是原始指针的两倍大，
// 以动态分配内存作为计算或删除器的专属数据，以virtual形式调用删除器。
// 只要定义一个预处理符合就可以关闭多线程支持。
// 总之，它比原始指针大且慢，而且使用辅助动态内存。
// 在许多应用中这些额外成本并不显著，然而其降低客户错误的成效确是显著的。

// 请记住：
// 1.好的接口很容易被正确使用，不容易被误用。
// 2.应该在所有接口中努力达到这些性质。
// 3.促进正确使用的办法包括接口一致性，以及与内置类型的行为兼容。
// 4.阻止误用的办法包括建立新类型、限制类型上的操作，
//   束缚对象值，以及消除客户的资源管理责任。
// 5.shared_ptr支持定制型删除器，可防范DLL问题，
//   可被用来自动解除互斥锁等等。

int main() {
  Date1(30, 3, 1995);
  Date1(2, 30, 1995);

  // Date2(30, 3, 1995); // 错误，不正确的类型
  // Date2(Day(30), Month(3), Year(1995));  // 错误，不正确的类型
  Date2(Month(3), Day(30), Year(1995));  // 类型正确

  Date3(Month1::Mar(), Day(30), Year(1995));

  return 0;
}