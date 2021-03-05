#include <array>
#include <iostream>
#include <list>

using namespace std;

// list容器，又称双向链表容器，即该容器的底层是以双向链表的形式实现的。这意味着，list容器中的元素可以分散存储在内存空间里，而不是必须存储在一整块连续的内存空间中。
// list容器中各个元素的前后顺序是靠指针来维系的，每个元素都配备了2个指针，分别指向它的前一个元素和后一个元素。其中第一个元素的前向指针总为null，因为它前面没有元素；
// 同样，尾部元素的后向指针也总为null。
// 基于这样的存储结构，list容器具有一些其它容器（array、vector 和 deque）所不具备的优势，即它可以在序列已知的任何位置快速插入或删除元素（时间复杂度为O(1)）。
// 并且在list容器中移动元素，也比其它容器的效率高。
// 使用list容器的缺点是，它不能像array和vector那样，通过位置直接访问元素。如果要访问list容器中的第6个元素，它不支持容器对象名[6]这种语法格式，正确的做法是从容器
// 中第一个元素或最后一个元素开始遍历容器，直到找到该位置。
// 实际场景中，如何需要对序列进行大量添加或删除元素的操作，而直接访问元素的需求却很少，这种情况建议使用list容器存储序列。
// list容器以模板类list<T>（T为存储元素的类型）的形式在<list>头文件中，并位于std命名空间中。

// list容器可用的成员函数:
// begin() 	返回指向容器中第一个元素的双向迭代器。
// end() 	返回指向容器中最后一个元素所在位置的下一个位置的双向迭代器。
// rbegin() 返回指向最后一个元素的反向双向迭代器。
// rend() 	返回指向第一个元素所在位置前一个位置的反向双向迭代器。
// cbegin() 和begin()功能相同，只不过在其基础上，增加了const属性，不能用于修改元素。
// cend() 	和end()功能相同，只不过在其基础上，增加了const属性，不能用于修改元素。
// crbegin() 和rbegin()功能相同，只不过在其基础上，增加了const属性，不能用于修改元素。
// crend() 	和rend()功能相同，只不过在其基础上，增加了const属性，不能用于修改元素。
// empty() 	判断容器中是否有元素，若无元素，则返回true；反之，返回false。
// size() 	返回当前容器实际包含的元素个数。
// max_size() 返回容器所能包含元素个数的最大值。这通常是一个很大的值，一般是232-1，所以我们很少会用到这个函数。
// front() 	返回第一个元素的引用。
// back() 	返回最后一个元素的引用。
// assign() 用新元素替换容器中原有内容。
// emplace_front() 在容器头部生成一个元素。该函数和push_front()的功能相同，但效率更高。
// push_front() 在容器头部插入一个元素。
// pop_front() 	删除容器头部的一个元素。
// emplace_back() 在容器尾部直接生成一个元素。该函数和push_back()的功能相同，但效率更高。
// push_back() 	在容器尾部插入一个元素。
// pop_back() 	删除容器尾部的一个元素。
// emplace() 	在容器中的指定位置插入元素。该函数和insert()功能相同，但效率更高。
// insert()  	在容器中的指定位置插入元素。
// erase() 	删除容器中一个或某区域内的元素。
// swap() 	交换两个容器中的元素，必须保证这两个容器中存储的元素类型是相同的。
// resize() 调整容器的大小。
// clear() 	删除容器存储的所有元素。
// splice() 将一个list容器中的元素插入到另一个容器的指定位置。
// remove(val) 	删除容器中所有等于val的元素。
// remove_if() 	删除容器中满足条件的元素。
// unique() 	删除容器中相邻的重复元素，只保留一个。
// merge() 	合并两个事先已排好序的list容器，并且合并之后的list容器依然是有序的。
// sort() 	通过更改容器中元素的位置，将它们进行排序。
// reverse() 反转容器中元素的顺序。
// C++11标准库还新增加了begin()和end()这2个函数，和list容器包含的begin()和end()成员函数不同，标准库提供的这2个函数的操作对象，既可以是容器，还可以是普通数组。
// 当操作对象是容器时，它和容器包含的begin()和end()成员函数的功能完全相同；如果操作对象是普通数组，则begin()函数返回的是指向数组第一个元素的指针，同样end()返回
// 指向数组中最后一个元素之后一个位置的指针（注意不是最后一个元素）。
// list容器还有一个std::swap(x,y)非成员函数（其中x和y是存储相同类型元素的list容器），它和swap()成员函数的功能完全相同，仅使用语法上有差异。
// C++11新添加的begin()和end()全局函数也同样适用于list容器。即当操作对象为list容器时，其功能分别begin()、end()成员函数相同。

// 和array、vector、deque容器的迭代器相比，list容器迭代器最大的不同在于，其配备的迭代器类型为双向迭代器，而不再是随机访问迭代器。
// 这意味着，假设 p1 和 p2 都是双向迭代器，则它们支持使用 ++p1、p1++、p1--、p1++、*p1、p1==p2以及p1!=p2运算符，但不支持以下操作（其中i为整数）：
//   p1[i]：不能通过下标访问list容器中指定位置处的元素。
//   p1-=i、p1+=i、p1+i 、p1-i：双向迭代器p1不支持使用 -=、+=、+、- 运算符。
//   p1<p2、p1>p2、p1<=p2、p1>=p2：双向迭代器p1、p2不支持使用<、>、<=、>=比较运算符。

// list模板类中，与“添加或插入新元素”相关的成员方法有如下几个：
//  push_front()：向list容器首个元素前添加新元素；
//  push_back()：向list容器最后一个元素后添加新元素；
//  emplace_front()：在容器首个元素前直接生成新的元素；
//  emplace_back()：在容器最后一个元素后直接生成新的元素；
//  emplace()：在容器的指定位置直接生成新的元素；
//  insert()：在指定位置插入新元素；
//  splice()：将其他list容器存储的多个元素添加到当前list容器的指定位置处。
// 除了insert()和splice()方法有多种语法格式外，其它成员方法都仅有1种语法格式
// 同样是实现插入元素的功能，无论是push_front()、push_back()还是insert()，都有以emplace为名且功能和前者相同的成员函数。后者是C++11标准新添加的，在大多数场景中，
// 都可以完全替代前者实现同样的功能。更重要的是，实现同样的功能，emplace系列方法的执行效率更高。

// list insert()成员方法
// iterator insert(pos,elem) 在迭代器pos指定的位置之前插入一个新元素elem，并返回表示新插入元素位置的迭代器。
// iterator insert(pos,n,elem) 在迭代器pos指定的位置之前插入n个元素 elem，并返回表示第一个新插入元素位置的迭代器。
// iterator insert(pos,first,last) 在pos位置之前，插入（array、vector、deque等）中位于[first,last)区域的所有元素，并返回表示第一个新插入元素位置的迭代器。
// iterator insert(pos,initlist) 在迭代器pos指定的位置之前，插入初始化列表（ { }括起来的多个元素）中所有的元素，并返回表示第一个新插入元素位置的迭代器。

// list splice()成员方法
// void splice(iterator position, list& x); position为迭代器，用于指明插入位置；x为另一个list容器。
//  此格式的splice()方法的功能是，将x容器中存储的所有元素全部移动当前list容器中position指明的位置处。
// void splice (iterator position, list& x, iterator i); position为迭代器，用于指明插入位置；x为另一个list容器；i也是一个迭代器，用于指向x容器中某个元素。
//  此格式的splice()方法的功能是将x容器中i指向的元素移动到当前容器中position指明的位置处。
// void splice (iterator position, list& x, iterator first, iterator last); position为迭代器，用于指明插入位置；x为另一个list容器；first和last都是迭代器，
//  [fist,last)用于指定x容器中的某个区域。此格式的splice()方法的功能是将x容器[first, last)范围内所有的元素移动到当前容器position指明的位置处。

// 对list容器存储的元素执行删除操作，需要借助该容器模板类提供的成员函数。幸运的是，相比其它STL容器模板类，list模板类提供了更多用来实现此操作的成员函数
// pop_front() 	删除位于list容器头部的一个元素。
// pop_back() 	删除位于list容器尾部的一个元素。
// erase() 	    该成员函数既可以删除list容器中指定位置处的元素，也可以删除容器中某个区域内的多个元素。
// clear() 	    删除list容器存储的所有元素。
// remove(val) 	删除容器中所有等于val的元素。
// unique() 	删除容器中相邻的重复元素，只保留一份。
// remove_if() 	删除容器中满足条件的元素。

// list容器底层存储结构:
// list容器时提到，该容器的底层是用双向链表实现的，甚至一些STL版本中（比如SGI STL），list容器的底层实现使用的是双向循环链表。
// 使用链表存储数据，并不会将它们存储到一整块连续的内存空间中。恰恰相反，各元素占用的存储空间（又称为节点）是独立的、分散的，它们之间的线性关系通过指针来维持。
// 双向链表的各个节点中存储的不仅仅是元素的值，还应包含2个指针，分别指向前一个元素和后一个元素
// template<typename T,...>
// struct __List_node{
//     //...
//     __list_node<T>* prev;
//     __list_node<T>* next;
//     T myval;
//     //...
// }
// list容器定义的每个节点中，都包含*prev、*next和myval。其中，prev指针用于指向前一个节点；next指针用于指向后一个节点；myval用于存储当前元素的值。
// 和array、vector这些容器迭代器的实现方式不同，由于list容器的元素并不是连续存储的，所以该容器迭代器中，必须包含一个可以指向list容器的指针，并且该指针还可
// 以借助重载的*、++、--、==、!=等运算符，实现迭代器正确的递增、递减、取值等操作。
// template<tyepname T,...>
// struct __list_iterator{
//     __list_node<T>* node;
//     ...
//     // 重载 == 运算符
//     bool operator==(const __list_iterator& x){return node == x.node;}
//     // 重载 != 运算符
//     bool operator!=(const __list_iterator& x){return node != x.node;}
//     // 重载 * 运算符，返回引用类型
//     T* operator *() const {return *(node).myval;}
//     // 重载前置 ++ 运算符
//     __list_iterator<T>& operator ++(){
//         node = (*node).next;
//         return *this;
//     }
//     // 重载后置 ++ 运算符
//     __list_iterator<T>& operator ++(int){
//         __list_iterator<T> tmp = *this;
//         ++(*this);
//         return tmp;
//     }
//     // 重载前置 -- 运算符
//     __list_iterator<T>& operator--(){
//         node = (*node).prev;
//         return *this;
//     }
//     // 重载后置 -- 运算符
//     __list_iterator<T> operator--(int){
//         __list_iterator<T> tmp = *this;
//         --(*this);
//         return tmp;
//     }
//     //...
// }
// 不同版本的STL标准库中，list容器的底层实现并不完全一致，但原理基本相同。
// SGI STL标准库中，list容器的底层实现为双向循环链表，相比双向链表结构的好处是在构建list容器时，只需借助一个指针即可轻松表示list容器的首尾元素。
// template <class T,...>
// class list
// {
//     // ...
//     // 指向链表的头节点，并不存放数据
//     __list_node<T>* node;
//     // ...以下还有list容器的构造函数以及很多操作函数
// }
// 为了更方便的实现list模板类提供的函数，该模板类在构建容器时，会刻意在容器链表中添加一个空白节点，并作为list链表的首个节点（又称头节点）。
// 使用双向链表实现的list容器，其内部通常包含2个指针，并分别指向链表中头部的空白节点和尾部的空白节点（也就是说，其包含2个空白节点）。
// 经常构造空的list容器，其用到的构造函数如下所示(即便是创建空的list容器，它也包含有1个节点)：
// list() {
//   empty_initialize();
// }
// void empty_initialize() {
//   node = get_node();  // 初始化节点
//   node->next = node;  // 前置节点指向自己
//   node->prev = node;  // 后置节点指向自己
// }
// 除此之外，list模板类中还提供有带参的构造函数，它们的实现过程大致分为以下2步：
//  调用empty_initialize()函数，构造带有头节点的空list容器链表；
//  将各个参数按照次序插入到空的list容器链表中。
// 由此可以总结出，list容器实际上就是一个带有头节点的双向循环链表。在此基础上，通过借助node头节点，就可以实现list容器中的所有成员函数
// __list_iterator<T> begin(){return (*node).next;}
// __list_iterator<T> end(){return node;}
// bool empty() const{return (*node).next == node;}
// T& front() {return *begin();}
// T& back() {return *(--end();)}

// if(cont.size() == 0)
// if(cont.empty())
// 本质上上面两行代码是等价的,在实际场景中，到底应该使用哪一种呢？
// 建议使用empty()成员方法。理由很简单，无论是哪种容器，只要其模板类中提供了empty()成员方法，使用此方法都可以保证在O(1) 时间复杂度内完成对“容器是否为空”的判断；
// 但对于list容器来说，使用size()成员方法判断“容器是否为空”，可能要消耗O(n)的时间复杂度。这个结论适用于很多容器。
// 为什么list容器这么特殊呢？这和list模板类提供了独有的splice()成员方法有关。
// 在实现list容器的过程中我们发现，用户经常需要知道当前list容器中存有多少个元素，于是我们想让size()成员方法的执行效率达到O(1)。
// 为了实现这个目的，我们必须重新设计list，使它总能以最快的效率知道自己含有多少个元素。
// 要想令size()的执行效率达到O(1)，最直接的实现方式是：在list模板类中设置一个专门用于统计存储元素数量的size变量，其位于所有成员方法的外部。与此同时，在每一个可用来为容
// 器添加或删除元素的成员方法中，添加对size变量值的更新操作。由此，size()成员方法只需返回size这个变量即可（时间复杂度为O(1)）。
// 不仅如此，由于list容器底层采用的是链式存储结构（也就是链表），该结构最大的特点就是，某一链表中存有元素的节点，无需经过拷贝就可以直接链接到其它链表中，且整个过程只需要
// 消耗O(1)的时间复杂度。考虑到之所以选用list容器，就是看中了其底层存储结构的这一特性。因此，作为list容器设计者的我们，自然也想将splice()方法的时间复杂度设计为O(1)。
// 这里就产生了一个矛盾，即如果将size()设计为O(1)时间复杂度，则由于splice()成员方法会修改list容器存储元素的个数，因此该方法中就需要添加更新size变量的代码（无疑是通过
// 遍历链表来实现），意味着splice()成员方法的执行效率将无法达到O(1)；反之，如果将splice()成员方法的执行效率提高到O(1)，则size()成员方法将无法实现O(1)的时间复杂度。
// 也就是说，list容器中的size()和splice()总有一个要做出让步，即只能实现其中一个方法的执行效率达到O(1)。
// 值得一提的是，不同版本的STL标准库，其底层解决此冲突的抉择是不同的。以本教程所用的C++ STL标准模板库为例，其选择将splice()成员方法的执行效率达到O(1)，而size()成员方
// 法的执行效率为O(n)。当然，有些版本的STL标准库中，选择将size()方法的执行效率设计为O(1)。
// 但不论怎样，选用empty()判断容器是否为空，效率总是最高的。所以，如果程序中需要判断当前容器是否为空，应优先考虑使用empty()。

// 二元谓词函数
bool demo(int first, int second) { return (first == 2 && second == 3); }

int main() {
  // list容器的创建:
  // 1. 创建一个没有任何元素的空list容器
  // 和空array容器不同，空的list容器在创建之后仍可以添加元素，因此创建list容器的方式很常用。
  std::list<int> values1;
  // 2. 创建一个包含n个元素的list容器
  // 通过此方式创建values容器，其中包含10个元素，每个元素的值都为相应类型的默认值（int类型的默认值为0）。
  std::list<int> values2(10);
  // 3. 创建一个包含n个元素的list容器，并为每个元素指定初始值。创建了一个包含10个元素并且值都为5个values容器
  std::list<int> values3(10, 5);
  // 4. 在已有list容器的情况下，通过拷贝该容器可以创建新的list容器。采用此方式，必须保证新旧容器存储的元素类型一致。
  std::list<int> value4(10);
  std::list<int> value5(value4);
  // 5. 通过拷贝其他类型容器（或者普通数组）中指定区域内的元素，可以创建新的list容器
  int a[] = {1, 2, 3, 4, 5};
  std::list<int> values6(a, a + 5);
  std::array<int, 5> arr{11, 12, 13, 14, 15};
  std::list<int> values7(arr.begin() + 2, arr.end());

  std::list<double> values;
  values.push_back(3.1);
  values.push_back(2.2);
  values.push_back(2.9);
  cout << "values size：" << values.size() << endl;
  values.sort();
  for (std::list<double>::iterator it = values.begin(); it != values.end();
       ++it) {
    std::cout << *it << " ";
  }
  cout << endl;  // 2.2 2.9 3.1

  std::list<char> l1{'h', 't', 't', 'p', ':', '/', '/', 'c', '.', 'b', 'i',
                     'a', 'n', 'c', 'h', 'e', 'n', 'g', '.', 'n', 'e', 't'};
  for (std::list<char>::iterator it = l1.begin(); it != l1.end(); ++it) {
    std::cout << *it;
  }
  cout << endl;  // http://c.biancheng.net
  for (std::list<char>::reverse_iterator it = l1.rbegin(); it != l1.rend();
       ++it) {
    std::cout << *it;
  }
  cout << endl;  // ten.gnehcnaib.c//:ptth
  // 程序中比较迭代器之间的关系，用的是!=运算符，因为它不支持<等运算符。
  // 另外在实际场景中，所有迭代器函数的返回值都可以传给使用auto关键字定义的变量，因为编译器可以自行判断出该迭代器的类型。

  // list容器在进行插入（insert()）、接合（splice()）等操作时，都不会造成原有的list迭代器失效，甚至进行删除操作，而只有指向被删除元素的迭代器失效，其他迭代器不受任何影响。
  std::list<char>::iterator begin = l1.begin();
  std::list<char>::iterator end = l1.end();
  l1.insert(begin, '1');
  l1.insert(end, '1');
  while (begin != end) {
    std::cout << *begin;
    ++begin;
  }
  cout << endl;  // http://c.biancheng.net1
  // 可以看到，在进行插入操作之后，仍使用先前创建的迭代器遍历容器，虽然程序不会出错，但由于插入位置的不同，可能会遗漏新插入的元素。

  // 不同于之前学过的STL容器，访问list容器中存储元素的方式很有限，即要么使用front()和back()成员函数，要么使用list容器迭代器。
  // list容器不支持随机访问，未提供下标操作符[]和at()成员函数，也没有提供data()成员函数。
  // 通过front()和back()成员函数，可以分别获得list容器中第一个元素和最后一个元素的引用形式。
  // 通过front()和back()的返回值，我们不仅能分别获取当前list容器中的首尾元素，必要时还能修改它们的值。
  std::list<int> mylist{1, 2, 3, 4};
  int &first = mylist.front();
  int &last = mylist.back();
  cout << first << " " << last << endl;  // 1 4
  first = 10;
  last = 20;
  cout << mylist.front() << " " << mylist.back() << endl;  // 10 20
  // 如果想访问list容存储的其他元素，就只能使用list容器的迭代器
  auto it = mylist.begin();
  cout << *it << " - ";
  ++it;
  while (it != mylist.end()) {
    cout << *it << " ";
    ++it;
  }
  cout << endl;  // 10 - 2 3 20
  // 对于非const类型的list容器，迭代器不仅可以访问容器中的元素，也可以对指定元素的值进行修改。
  // 对于修改容器指定元素的值，list模板类提供有专门的成员函数assign()

  std::list<int> values22{1, 2, 3};
  values22.push_front(0);      // {0,1,2,3}
  values22.push_back(4);       // {0,1,2,3,4}
  values22.emplace_front(-1);  // {-1,0,1,2,3,4}
  values22.emplace_back(5);    // {-1,0,1,2,3,4,5}
  // emplace(pos,value),其中pos表示指明位置的迭代器，value为要插入的元素值
  values22.emplace(values22.end(), 6);  //{-1,0,1,2,3,4,5,6}
  for (auto p = values22.begin(); p != values22.end(); ++p) {
    cout << *p << " ";
  }
  cout << endl;  // -1 0 1 2 3 4 5 6

  std::list<int> values33{1, 2};
  values33.insert(values33.begin(), 3);   // {3,1,2}
  values33.insert(values33.end(), 2, 5);  // {3,1,2,5,5}
  std::array<int, 3> tt{7, 8, 9};
  values33.insert(values33.end(), tt.begin(), tt.end());  // {3,1,2,5,5,7,8,9}
  values33.insert(values33.end(), {10, 11});  // {3,1,2,5,5,7,8,9,10,11}
  for (auto p = values33.begin(); p != values33.end(); ++p) {
    cout << *p << " ";
  }
  cout << endl;  // 3 1 2 5 5 7 8 9 10 11

  auto print_list = [](list<int> &l) -> void {
    for (auto iter = l.begin(); iter != l.end(); ++iter) {
      cout << *iter << " ";
    }
    cout << endl;
  };

  list<int> mylist1{1, 2, 3, 4}, mylist2{10, 20, 30};
  list<int>::iterator itt = ++mylist1.begin();  // 指向mylist1容器中的元素2
  mylist1.splice(itt, mylist2);
  cout << "mylist1: ";
  print_list(mylist1);  // mylist1: 1 10 20 30 2 3 4
  cout << "mylist2: ";
  print_list(mylist2);  // mylist2:
  mylist2.splice(mylist2.begin(), mylist1, itt);
  cout << "mylist1: ";
  print_list(mylist1);  // mylist1: 1 10 20 30 3 4
  cout << "mylist2: ";
  print_list(mylist2);  // mylist2: 2
  mylist2.splice(mylist2.begin(), mylist1, mylist1.begin(), mylist1.end());
  print_list(mylist1);  // mylist1:
  cout << "mylist2: ";
  print_list(mylist2);  // mylist2: 1 10 20 30 3 4 2

  list<int> v11{1, 2, 3, 4};
  v11.pop_front();
  print_list(v11);  // {2,3,4}
  v11.pop_back();
  print_list(v11);  // {2,3}
  v11.clear();
  print_list(v11);  // {}

  // erase()成员函数有以下2种语法格式：
  // iterator erase (iterator position);
  // iterator erase (iterator first, iterator last);
  // 删除list容器中position迭代器所指位置处的元素
  v11.insert(v11.end(), {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11});
  print_list(v11);  // 1 2 3 4 5 6 7 8 9 10 11
  auto del = v11.begin();
  ++del;
  v11.erase(del);
  print_list(v11);  // 1 3 4 5 6 7 8 9 10 11
  // 可实现删除list容器中first迭代器和last迭代器限定区域内的所有元素,包括first指向的元素，但不包括last指向的元素。
  del = v11.begin();
  del++;
  del++;
  auto del1 = v11.end();
  --del1;
  --del1;
  --del1;
  v11.erase(del, del1);
  print_list(v11);  // 1 3 9 10 11

  // erase()成员函数是按照被删除元素所在的位置来执行删除操作，如果想根据元素的值来执行删除操作，可以使用remove()成员函数
  list<int> v22{1, 2, 3, 4, 3};
  print_list(v22);  // 1 2 3 4 3
  v22.remove(3);
  print_list(v22);  // 1 2 4

  // unique()函数也有以下2种语法格式：
  // void unique()
  // void unique（BinaryPredicate）  // 传入一个二元谓词函数
  // 以上2种格式都能实现去除list容器中相邻重复的元素，仅保留一份。但第2种格式的优势在于，能自定义去重的规则
  list<int> v33{1, 2, 2, 3, 4, 7, 5, 2, 6, 7, 7};
  print_list(v33);  // 1 2 2 3 4 7 5 2 6 7 7
  v33.unique();     // 删除相邻重复的元素，仅保留一份
  print_list(v33);  // 1 2 3 4 7 5 2 6 7
  v33.unique(demo);  // demo为二元谓词函数，是我们自定义的去重规则
  print_list(v33);  // 1 2 4 7 5 2 6 7
  // 除了以上一定谓词函数的方式，还可以使用lamba表达式以及函数对象的方式定义。
  // 通过调用无参的unique()，仅能删除相邻重复（也就是相等）的元素，而通过我们自定义去重的规则，可以更好的满足在不同场景下去重的需求。

  // 通过将自定义的谓词函数（不限定参数个数）传给remove_if()成员函数，list容器中能使谓词函数成立的元素都会被删除。
  std::list<int> v44{15, 36, 7, 17, 20, 39, 4, 1};
  v44.remove_if([](int value) { return (value < 10); });
  print_list(v44);  // 15 36 17 20 39

  return 0;
}