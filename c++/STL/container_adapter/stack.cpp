#include <iostream>
#include <list>
#include <queue>
#include <set>
#include <stack>
#include <vector>

using namespace std;

// stack栈适配器是一种单端开口的容器，实际上该容器模拟的就是栈存储结构，即无论是向里存数据还是从中取数据，都只能从这一个开口实现操作。
// stack适配器的开头端通常称为栈顶。由于数据的存和取只能从栈顶处进行操作，因此对于存取数据，stack适配器有这样的特性，即每次只能访问适配器中位于最顶端的元素，
// 也只有移除stack顶部的元素之后，才能访问位于栈中的元素。
// 栈中存储的元素满足“后进先出（简称LIFO）”的准则，stack适配器也同样遵循这一准则。

// stack适配器以模板类stack<T,Container=deque<T>>（其中T为存储元素的类型，Container表示底层容器的类型）的形式位于<stack>头文件中，并定义在std命名空间里。
// stack容器适配器支持的成员函数(和其他序列容器相比，stack是一类存储机制简单、提供成员函数较少的容器):
// empty() 	当stack栈中没有元素时，该成员函数返回true；反之，返回false。
// size() 	返回stack栈中存储元素的个数。
// top() 	返回一个栈顶元素的引用，类型为T&。如果栈为空，程序会报错。
// push(const T& val) 先复制val，再将val副本压入栈顶。这是通过调用底层容器的push_back()函数完成的。
// push(T&& obj) 以移动元素的方式将其压入栈顶。这是通过调用底层容器的有右值引用参数的push_back()函数完成的。
// pop() 	弹出栈顶元素。
// emplace(arg...) 	arg...可以是一个参数，也可以是多个参数，但它们都只用于构造一个对象，并在栈顶直接生成该对象，作为新的栈顶元素。
// swap(stack<T> & other_stack) 将两个stack适配器中的元素进行互换，需要注意的是，进行互换的2个stack适配器中存储的元素类型以及底层采用的基础容器类型，都必须相同。

int main() {
  // 创建stack适配器，大致分为如下几种方式：
  // 1 创建一个不包含任何元素的stack适配器，并采用默认的deque基础容器
  std::stack<int> values1;
  // 2 通过指定第二个模板类型参数，可以使用除deque容器外的其它序列式容器，只要该容器支持empty()、size()、back()、push_back()、pop_back()这5个成员函数即可。
  std::stack<std::string, std::list<int>> values21;
  std::stack<std::string, std::vector<int>> values22;
  std::stack<std::string, std::deque<int>> values23;
  // std::stack<std::string, std::set<int>> values24;  // 不报错？
  // 3 可以用一个基础容器来初始化stack适配器，只要该容器的类型和stack底层使用的基础容器类型相同即可
  std::list<int> l{1, 2, 3};
  std::stack<int, std::list<int>> my_stack3(l);
  // 初始化后的my_stack适配器中，栈顶元素为3，而不是1。stack第2个模板参数必须显式指定为list<int>（必须为int类型，和存储类型保持一致），否则stack底层将默认使用
  // deque容器，也就无法用lsit容器的内容来初始化stack适配器。
  // 4 还可以用一个stack适配器来初始化另一个stack适配器，只要它们存储的元素类型以及底层采用的基础容器类型相同即可
  std::stack<int, std::list<int>> my_stack41 = my_stack3;
  std::stack<int, std::list<int>> my_stack42(my_stack3);
  // 第3、4种初始化方法中，my_stack适配器的数据是经过拷贝得来的，也就是说，操作my_stack适配器，并不会对l容器以及my_stack1适配器有任何影响；反过来也是如此。

  list<int> values{1, 2, 3};
  stack<int, list<int>> my_stack(values);
  cout << "size of my_stack: " << my_stack.size() << endl;  // 3
  while (!my_stack.empty()) {
    cout << my_stack.top() << endl;
    my_stack.pop();
  }
  // 3
  // 2
  // 1

  return 0;
}