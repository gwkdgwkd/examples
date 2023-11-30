#include <iostream>
#include <memory>

// 让接口容易被正确使用，不易被误用

// C++在接口之海漂浮，function、class、template，每种接口都是客户与代码互动的手段。
// 理论上，如果客户企图使用某个接口而却没有获得他所预期的行为，这个代码应该编译不通过；
// 如果通过了，那就是客户想要的。

namespace n1 {
// 欲开发一个容易被正确使用，不易被误用的接口，首先必须考虑客户可能出什么样的错误。
class Date {
 public:
  Date(int month, int day, int year) {}
};

void func() {
  Date(30, 3, 1995);  // 错误的次序传递参数
  Date(2, 30, 1995);  // 传递无效的月份或天数
}
}  // namespace n1

// 导入简单的外覆类型来区别天数、月份和年份：
struct Day {
  explicit Day(int d) : val(d) {}
  int val;
};
struct Year {
  explicit Year(int y) : val(y) {}
  int val;
};

namespace n2 {
struct Month {
  explicit Month(int m) : val(m) {}
  int val;
};
class Date {
 public:
  Date(const Month& m, const Day& d, const Year& y) {}
};
// 令Day、Month和Year成为成熟且充分锻炼的类，并封装内部数据，比struct要好。
// 但struct也已经足够示范：明智而审慎地导入新类型对预防接口被误用有神奇疗效。

void func() {
  // Date(30, 3, 1995);  // 错误，不正确的类型
  // Date(Day(30), Month(3), Year(1995));  // 错误，不正确的类型
  Date(Month(3), Day(30), Year(1995));  // 类型正确
}
}  // namespace n2

namespace n3 {
// 一旦类型正确，限制其值有时候是通情达理的，可以使用enums，但不具备类型安全性。
class Month {
 public:
  static Month Jan() { return Month(1); }
  static Month Feb() { return Month(2); }
  static Month Mar() { return Month(3); }
  static Month Dec() { return Month(12); }

 private:
  explicit Month(int m) : val(m) {}
  int val;
};
class Date {
 public:
  Date(const Month& m, const Day& d, const Year& y) {}
};
// 如果以函数替换对象，表现某个特定月份让诧异，
// 或许是因为忘记了non-local static对象的初始化次序可能出问题（条款4）。

void func() { Date(Month::Mar(), Day(30), Year(1995)); }
}  // namespace n3

// 预防客户错误的另一个办法是，限制类型内什么事可做，什么事不能做，常见的是加上const。
// 除非有好的理由，否则应该尽量令class的行为与内置type一致。
// 避免无端与内置类型不兼容，真正的理由是为了提供行为一致的接口，STL容器的接口是否一致。

// 任何接口如果要求客户必须记得做某些事情，就是有着不正确使用的倾向，可能会忘记做那件事。
// 没使用对象管理资源时，为避免资源泄露，要求必须释放资源。
// 给客户两个犯错的机会：没有删除指针，或删除同一指针超过一次。
// 许多时候，较好的接口设计原则是先发制人，直接返回一个智能指针，
// 强迫客户将返回值保存在shared_ptr内，消除了忘记释放资源的可能性。

// shared_ptr允许当智能指针被创建时，指定一个资源释放函数（删除器）绑定于智能指针上，
// auto_ptr就没有这个功能，shared_ptr提供的某个构造函数接受两个实参：
// 一个是被管理的指针，另一个是引用次数为0时将被调用的删除器。

// shared_ptr有一个特别好的性质是，会自动使用它的每个指针专属的删除器，
// 因而消除潜在的客户错误：所谓的cross-DLL problem。
// 这个问题发生于对象在动态链接库中被new创建，却在另一个库中被delete。
// 在许多平台上，会导致运行期错误，shared_ptr没有这个问题，
// 因为缺省的删除器是来自shared_ptr诞生的那个库的delete。

// 常见的shared_ptr来自Boost，shared_ptr是原始指针的两倍大，
// 以动态分配内存作为计算或删除器的专属数据，以virtual形式调用删除器。
// 只要定义一个预处理符合就可以关闭多线程支持。
// 总之，它比原始指针大且慢，而且使用辅助动态内存。
// 在许多应用中这些额外成本并不显著，然而其降低客户错误的成效确是显著的。

// 请记住：
// 1.好的接口很容易被正确使用，不容易被误用；
// 2.应该在所有接口中努力达到这些性质；
// 3.促进正确使用的办法包括接口一致性，以及与内置类型的行为兼容；
// 4.阻止误用的办法包括建立新类型、限制类型上的操作，束缚对象值，
//   以及消除客户的资源管理责任；
// 5.shared_ptr支持定制型删除器，可防范DLL问题，可被用来自动解除互斥锁等等。

int main(int argc, char* argv[]) {
  if (argc < 2) {
    std::cout << argv[0] << " i [0 - 2]" << std::endl;
    return 0;
  }
  int type = argv[1][0] - '0';
  switch (type) {
    case 0:
      n1::func();
      break;
    case 1:
      n2::func();
      break;
    case 2:
      n3::func();
      break;
    default:
      std::cout << "invalid type" << std::endl;
      break;
  }

  return 0;
}