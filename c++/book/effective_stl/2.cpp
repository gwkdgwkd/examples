#include <algorithm>
#include <iostream>
#include <iterator>
#include <list>
#include <vector>

// 不要试图编写独立于容器类型的代码

// STL是以泛化原则为基础的：
// 1.数组被泛化为以其包含的对象的类型为参数的容器；
// 2.函数被泛化为以其使用的迭代器的类型为参数的算法；
// 3.指针被泛化为以其指向的对象的类型为参数的迭代器。

// 当编写自己的容器、迭代器和算法时，很多程序员以一种不同的方式泛化。
// 不是针对具体的容器，而是把容器的概念泛化，这样当从vector变为deque或list时，
// 代码不用改变，这类泛化，出发点好，却几乎总是误入歧途。

// 试图编写对序列容器和关联容器都适用的代码几乎毫无意义，很多成员函数不通用：
// 1.只有序列容器才支持push_front或push_back；
// 2.只有关联容器才支持count和lower_bound，等等；
// 3.即使insert和erase这样的基本操作，容器类型不同原型和语义也不同。

// 想编写对于大多数序列容器vector、deque和list都适用的代码，只能用它们功能的交集：
// 1.意味着不能使用reserve或capacity，因为deque和list中没有这样的操作；
// 2.有list意味着要放弃operator[]，而且要把操作限制在双向迭代器的能力范围之内；
// 3.要放弃随机迭代器的操作，包括sort、stable_sort、partial_sort和nth_elemment；
// 4.为了要支持vector，就不能使用push_front和pop_front；
// 5.对于vector和deque而言，splice和成员函数形式的sort又是被禁用的。

// 这些限制的根源在于，对不同类型的序列容器，使用迭代器、指针和引用无效的规则不同。
// 要想使代码对vector、deque和list都能工作，必须假定，对任何一种容器，
// 使迭代器、指针和引用无效的任何操作将在使用的容器上使他们无效。
// 所以：
// 1.每个insert都使所有迭代器、指针和引用无效，因为deque::insert使所有迭代器失效；
// 2.不能调用capacity，因此vector::insert必须保证所有的指针和引用也无效；
// 3.对erase的每次调用都要假定使一切变为无效；
// 4.不能把容器中的数据传递到C接口中，因为只有vector支持这一点；
// 5.不能使用bool作为要存储的对象类型来实例化容器，
//   因为vector<bool>并不总是表现的像个vector，实际上并没有存储bool类型的对象；
// 6.不能假定list的常数时间的插入和删除操作，因为vector和deque是线性时间。

// 想要编写对于set和map都适用的代码几乎不可能，面对现实吧，不同的容器是不同的，
// 它们有着非常明显的优缺点，它们并不是被设计来交换使用的，不要尝试这样做。

// 考虑到有时候不可避免的要从一种容器转到另一种，可以使用封装技术来实现这种转变。
// 最简单的方式是通过对容器类型和其迭代器类型使用typedef。
class Widget {
 public:
  bool operator==(const Widget& rhs) { return true; }
};
typedef std::vector<Widget> WidgetContainer;
typedef WidgetContainer::iterator WCIterator;
// 这使得改变容器要容易得多，尤其当仅仅是增加一个自定义分配子时，就显得更为方便。
template <typename T>
class SpecialAllocator {};
// typedef vector<Widget, SpecialAllocator<Widget>> WidgetContainer;
// typedef WidgetContainer::iterator WCIterator;
// 即使没有意识到这些类型定义所带来的封装效果，但也会欣赏所节省的工作。

// 如果不想把自己选择的容器暴露给客户，就得多费点劲，需要使用类。
// 要减少在替换容器类型时所需要修改的代码，可以把容器隐藏到一个类中，
// 并尽量减少那些通过类型接口可见的、与容器相关的信息。
class CustomerList {
 private:
  typedef std::list<Widget> CustomerContainer;
  typedef CustomerContainer::iterator CCIterator;

  CustomerContainer customers;
};

int main() {
  std::vector<Widget> vw;
  Widget bestWidget;
  // 不要这样写：
  std::vector<Widget>::iterator i1 = find(vw.begin(), vw.end(), bestWidget);
  // 这样写：
  WidgetContainer cw;
  WCIterator i2 = std::find(cw.begin(), cw.end(), bestWidget);

  return 0;
}