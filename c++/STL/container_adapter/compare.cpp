#include <iostream>
#include <queue>
#include <vector>

using namespace std;

// 无论priority_queue中存储的是基础数据类型（int、double等），还是string类对象或者自定义的类对象，都可以使用函数对象的方式自定义排序规则。
template <typename T>
class cmp {  // struct也行
 public:
  bool operator()(T a, T b) { return a > b; }
};

// 当priority_queue容器适配器中存储的数据类型为结构体或者类对象（包括string类对象）时，还可以通过重载其>或者<运算符，间接实现自定义排序规则的目的。
// 此方式仅适用于priority_queue容器中存储的为类对象或者结构体变量，也就是说，当存储类型为类的指针对象或者结构体指针变量时，此方式将不再适用，而只能使用函数对象的方式。
// <function>头文件中的std::less<T> 和 std::greater<T>，各自底层实现采用的都是函数对象的方式。比如，std::less<T>的底层实现代码为：
//     template <typename T>
//     struct less {
//         // 定义新的排序规则
//         bool operator()(const T &_lhs, const T &_rhs) const {
//             return _lhs < _rhs;
//         }
//     };
// std::greater<T>的底层实现代码为：
//     template <typename T>
//     struct greater {
//         bool operator()(const T &_lhs, const T &_rhs) const {
//             return _lhs > _rhs;
//         }
//     };
// 可以看到，std::less<T>和std::greater<T>底层实现的唯一不同在于，前者使用<号实现从大到小排序，后者使用>号实现从小到大排序。
class node {
 public:
  node(int x = 0, int y = 0) : x(x), y(y) {}
  int x, y;
};
// 新的排序规则为：先按照x值排序，如果x相等，则按y的值排序
bool operator<(const node &a, const node &b) {
  if (a.x > b.x)
    return 1;
  else if (a.x == b.x)
    if (a.y >= b.y) return 1;
  return 0;
}

// 也可以以友元函数或者成员函数的方式重载>或者<运算符。需要注意的是，以成员函数的方式重载>或者<运算符时，该成员函数必须声明为const类型，且参数也必须为const类型，
// 至于参数的传值方式是采用按引用传递还是按值传递，都可以（建议采用按引用传递，效率更高）。
class node1 {
 public:
  node1(int x = 0, int y = 0) : x(x), y(y) {}
  int x, y;
  bool operator<(const node1 &b) const {
    if ((*this).x > b.x)
      return 1;
    else if ((*this).x == b.x)
      if ((*this).y > b.y) return 1;
    return 0;
  }
};
// 在以友元函数的方式重载<或者>运算符时，要求参数必须使用const修饰
class node2 {
 public:
  node2(int x = 0, int y = 0) : x(x), y(y) {}
  int x, y;
  friend bool operator<(const node2 &a, const node2 &b);
};
bool operator<(const node2 &a, const node2 &b) {
  if (a.x > b.x)
    return 1;
  else if (a.x == b.x)
    if (a.y < b.y) return 1;
  return 0;
}

// 总的来说，以函数对象的方式自定义priority_queue的排序规则，适用于任何情况；而以重载>或者<运算符间接实现priority_queue自定义排序的方式，仅适用于priority_queue
// 中存储的是结构体变量或者类对象（包括string类对象）。

int main() {
  int a[] = {4, 2, 3, 5, 6};
  // 通过在cmp类（结构体）重载的()运算符中自定义排序规则，并将其实例化后作为priority_queue模板的第3个参数传入，即可实现为priority_queue容器适配器自定义比较函数。
  priority_queue<int, vector<int>, cmp<int> > pq(a, a + 5);
  while (!pq.empty()) {
    cout << pq.top() << " ";
    pq.pop();
  }
  cout << endl;  // 2 3 4 5 6

  priority_queue<node> pq1;
  pq1.push(node(1, 2));
  pq1.push(node(2, 2));
  pq1.push(node(3, 4));
  pq1.push(node(3, 3));
  pq1.push(node(2, 3));
  cout << "x y" << endl;
  while (!pq1.empty()) {
    cout << pq1.top().x << " " << pq1.top().y << endl;
    pq1.pop();
  }
  // x y
  // 1 2
  // 2 2
  // 2 3
  // 3 3
  // 3 4

  priority_queue<node1> pq2;
  pq2.push(node1(1, 2));
  pq2.push(node1(2, 2));
  pq2.push(node1(3, 4));
  pq2.push(node1(3, 3));
  pq2.push(node1(2, 3));
  cout << "x y" << endl;
  while (!pq2.empty()) {
    cout << pq2.top().x << " " << pq2.top().y << endl;
    pq2.pop();
  }
  // x y
  // 1 2
  // 2 2
  // 2 3
  // 3 3
  // 3 4

  priority_queue<node2> pq3;
  pq3.push(node2(1, 2));
  pq3.push(node2(2, 2));
  pq3.push(node2(3, 4));
  pq3.push(node2(3, 3));
  pq3.push(node2(2, 3));
  cout << "x y" << endl;
  while (!pq3.empty()) {
    cout << pq3.top().x << " " << pq3.top().y << endl;
    pq3.pop();
  }
  // x y
  // 1 2
  // 2 3
  // 2 2
  // 3 4
  // 3 3

  return 0;
}