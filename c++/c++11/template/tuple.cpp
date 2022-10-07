#include <iostream>
#include <map>
#include <stdexcept>
#include <tuple>

// c++11中的tuple看似简单，其实它是简约而不简单，
// 可以说它是c++11中一个既简单又复杂的东东，
// 关于它简单的一面是它很容易使用，复杂的一面是它内部隐藏了太多细节，
// 要揭开它神秘的面纱时又比较困难。
// tuple是一个固定大小的不同类型值的集合，是泛化的std::pair。
// 和c#中的tuple类似，但是比c#中的tuple强大得多。
// 可以把他当做一个通用的结构体来用，
// 不需要创建结构体又获取结构体的特征，
// 在某些情况下可以取代结构体使程序更简洁，直观。

namespace n1 {
struct A {
  char* p;
  int len;
};
// 用tuple<const char*, int>tp就可以不用创建这个结构体了，
// 而作用是一样的，更简洁直观了：
std::tuple<const char*, int> tp = std::make_tuple("nihao", 6);

std::tuple<double, char, std::string> get_student(int id) {
  // C++11构建tuple的写法：
  if (id == 0) return std::make_tuple(3.8, 'A', "Lisa Simpson");

  // C++17提供了更方便的构建tuple的写法：
  //if (id == 0) return { 3.8, 'A', "Lisa Simpson" };

  if (id == 1) return std::make_tuple(2.9, 'C', "Milhouse Van Houten");
  if (id == 2) return std::make_tuple(1.7, 'D', "Ralph Wiggum");
  throw std::invalid_argument("id");
}

// 相比较于std::tuple，std::pair如果想要支持多于2个的变量，
// 需要多层嵌套或结合struct使用：
std::pair<double, std::pair<char, std::string>> get_student1(int id) {
  return std::pair<double, std::pair<char, std::string>>{3.8,
                                                         {'A', "Lisa Simpson"}};
}
// tuple和vector比较：
// vector只能容纳同一种类型的数据，tuple可以容纳任意类型的数据；
// tuple和c++17的variant比较：
// 二者都可以容纳不同类型的数据，但是variant的类型个数是固定的，
// 而tuple的类型个数不是固定的，是变长的，更为强大。

void func1() {
  auto student = get_student(0);
  // 通过下标位置获取tuple中对应的元素：
  std::cout << "ID: 0, "
            << "GPA: " << std::get<0>(student) << ", "
            << "grade: " << std::get<1>(student) << ", "
            << "name: " << std::get<2>(student) << std::endl;
  // ID: 0, GPA: 3.8, grade: A, name: Lisa Simpson
}

void func2() {
  // C++11已经可以有多重返回值了，也就是可以像lua那样这样从函数返回值：
  // 从要返回多重返回值的函数中，
  // 返回一个std::tuple，里面包含多个具体要返回的值，
  // 接收端使用std::tie（会创建一个元组的左值引用）来解包tuple。
  double gpa;
  char grade;
  std::string name;
  std::tie(gpa, grade, name) = get_student(1);
  std::cout << "ID: 1, "
            << "GPA: " << gpa << ", "
            << "grade: " << grade << ", "
            << "name: " << name << std::endl;
  // ID: 1, GPA: 2.9, grade: C, name: Milhouse Van Houten
}

void func3() {
  // 解包时，如果只想解某个位置的值时，
  // 可以用std::ignore占位符来表示不解某个位置的值。
  double gpa;
  char grade;
  std::tie(gpa, grade, std::ignore) = get_student(2);
  std::cout << "ID: 2, "
            << "GPA: " << gpa << ", "
            << "grade: " << grade << std::endl;
  // ID: 1, GPA: 2.9, grade: C

  // C++17提供了更方便的解构写法：
  // auto [ gpa2, grade2, name2 ] = get_student(2);
  // std::cout << "ID: 2, "
  //           << "GPA: " << gpa2 << ", "
  //           << "grade: " << grade2 << ", "
  //           << "name: " << name2 << '\n';
}

void func4() {
  // 还有一个创建右值的引用元组方法：forward_as_tuple
  double gpa;
  char grade;
  std::string name;
  std::forward_as_tuple(gpa, grade, name) = get_student(2);
  std::cout << "ID: 2, "
            << "GPA: " << gpa << ", "
            << "grade: " << grade << ", "
            << "name: " << name << std::endl;
  // ID: 2, GPA: 1.7, grade: D, name: Ralph Wiggum
}

void testN1() {
  func1();
  func2();
  func3();
  func4();
}
}  // namespace n1

namespace n2 {
// tuple高级用法，模板元编程
// 因为tuple的参数是变长的，也没有for_each函数，
// 如果想遍历tuple中的每个元素，需要自己写代码实现：
template <class Tuple, std::size_t N>
struct TuplePrinter {
  static void print(const Tuple& t) {
    TuplePrinter<Tuple, N - 1>::print(t);
    std::cout << ", " << std::get<N - 1>(t);
  }
};
template <class Tuple>
struct TuplePrinter<Tuple, 1> {
  static void print(const Tuple& t) { std::cout << std::get<0>(t); }
};
// 打印tuple中的每个元素：
template <class... Args>
void PrintTuple(const std::tuple<Args...>& t) {
  std::cout << "(";
  TuplePrinter<decltype(t), sizeof...(Args)>::print(t);
  std::cout << ")" << std::endl;
}

void testN2() {
  std::tuple<int, std::string, float> t1(11, "Test", 3.14);
  int n = 7;
  auto t2 = std::tuple_cat(t1, std::make_pair("Foo", "bar"), t1, std::tie(n));
  PrintTuple(t2);  // (11, Test, 3.14, Foo, bar, 11, Test, 3.14, 7)
  n = 10;
  PrintTuple(t2);  // (11, Test, 3.14, Foo, bar, 11, Test, 3.14, 10)

  // 获取tuple中元素的个数：
  std::cout << "t2 : " << (std::tuple_size<decltype(t2)>::value)
            << std::endl;  // t2 : 9

  // 获取tuple中某个位置元素的类型（通过std::tuple_element获取元素类型）：
  std::tuple_element<1, decltype(t2)>::type cnt = std::get<1>(t2);
  std::cout << "cnt = " << cnt << std::endl;  // cnt = Test
}
}  // namespace n2

namespace n3 {
// 根据tuple元素值获取其对应的索引位置：
namespace detail {
template <int I, typename T, typename... Args>
struct find_index {
  static int call(std::tuple<Args...> const& t, T&& val) {
    return (std::get<I - 1>(t) == val)
               ? I - 1
               : find_index<I - 1, T, Args...>::call(t, std::forward<T>(val));
  }
};
template <typename T, typename... Args>
struct find_index<0, T, Args...> {
  static int call(std::tuple<Args...> const& t, T&& val) {
    return (std::get<0>(t) == val) ? 0 : -1;
  }
};
}  // namespace detail
template <typename T, typename... Args>
int find_index(std::tuple<Args...> const& t, T&& val) {
  // return detail::find_index<0, sizeof...(Args) - 1, T, Args...>::call(
  //     t, std::forward<T>(val));
  // return detail::find_index<0, T, Args...>::call(t, std::forward<T>(val));
  return 0;
}

void testN3() {
  std::tuple<int, std::string, int> tp(1, "Test", 5);
  // std::cout << find_index(tp, 1) << std::endl;
  // std::cout << find_index(tp, "Test") << std::endl;
  // std::cout << find_index(tp, 5) << std::endl;
}
}  // namespace n3

namespace n4 {
// 展开tuple，并将tuple元素作为函数的参数。
// 这样就可以根据需要对tuple元素进行处理了：
template <size_t N>
struct Apply {
  template <typename F, typename T, typename... A>
  static inline auto apply(F&& f, T&& t, A&&... a)
      -> decltype(Apply<N - 1>::apply(::std::forward<F>(f),
                                      ::std::forward<T>(t),
                                      ::std::get<N - 1>(::std::forward<T>(t)),
                                      ::std::forward<A>(a)...)) {
    return Apply<N - 1>::apply(::std::forward<F>(f), ::std::forward<T>(t),
                               ::std::get<N - 1>(::std::forward<T>(t)),
                               ::std::forward<A>(a)...);
  }
};
template <>
struct Apply<0> {
  template <typename F, typename T, typename... A>
  static inline auto apply(F&& f, T&&, A&&... a)
      -> decltype(::std::forward<F>(f)(::std::forward<A>(a)...)) {
    return ::std::forward<F>(f)(::std::forward<A>(a)...);
  }
};
template <typename F, typename T>
inline auto apply(F&& f, T&& t) -> decltype(
    Apply<::std::tuple_size<typename ::std::decay<T>::type>::value>::apply(
        ::std::forward<F>(f), ::std::forward<T>(t))) {
  return Apply<::std::tuple_size<typename ::std::decay<T>::type>::value>::apply(
      ::std::forward<F>(f), ::std::forward<T>(t));
}
void func1(int i, double d) {
  std::cout << "function func1(" << i << ", " << d << ");\n";
}
int func2(int i) {
  std::cout << "function func2(" << i << ");\n";
  return i;
}

void testN4() {
  std::tuple<int, double> tup(23, 4.5);
  apply(func1, tup);
  int d = apply(func2, std::make_tuple(2));

  // function func1(23, 4.5);
  // function func2(2);
}
}  // namespace n4

int main(int argc, char* argv[]) {
  if (argc < 2) {
    std::cout << argv[0] << " i [0 - 3]" << std::endl;
    return 0;
  }
  int type = argv[1][0] - '0';
  switch (type) {
    case 0:
      n1::testN1();
      break;
    case 1:
      n2::testN2();
      break;
    case 2:
      n3::testN3();
      break;
    case 3:
      n4::testN4();
      break;
    default:
      std::cout << "invalid type" << std::endl;
      break;
  }
}
