#include <array>
#include <iostream>
#include <iterator>
#include <list>

// list容器，又称双向链表容器，即该容器的底层是以双向链表的形式实现的。
// 这意味着，list容器中的元素可以分散存储在内存空间里，
// 而不是必须存储在一整块连续的内存空间中。
// list容器中各个元素的前后顺序是靠指针来维系的，
// 每个元素都配备了2个指针，分别指向它的前一个元素和后一个元素。
// 其中第一个元素的前向指针总为null，因为它前面没有元素；
// 同样，尾部元素的后向指针也总为null。
// 基于这样的存储结构，
// list容器具有一些其它容器（array、vector和deque）所不具备的优势，
// 即它可以在序列已知的任何位置快速插入或删除元素，时间复杂度为O(1)。
// 并且在list容器中移动元素，也比其它容器的效率高。
// 使用list容器的缺点是，它不能像array和vector那样，通过位置直接访问元素。
// 正确的做法是从容器中第一个元素或最后一个元素开始遍历容器，直到找到该位置。
// 实际场景中，如何需要对序列进行大量添加或删除元素的操作，
// 而直接访问元素的需求却很少，这种情况建议使用list容器存储序列。

// list容器可用的成员函数：
// begin() 	       返回指向容器中第一个元素的双向迭代器。
// end() 	         返回指向容器中最后一个元素所在位置的下一个位置的双向迭代器。
// rbegin()        返回指向最后一个元素的反向双向迭代器。
// rend() 	       返回指向第一个元素所在位置前一个位置的反向双向迭代器。
// cbegin()        和begin()功能相同，只不过在其基础上，
//                 增加了const属性，不能用于修改元素。
// cend() 	       和end()功能相同，只不过在其基础上，
//                 增加了const属性，不能用于修改元素。
// crbegin()       和rbegin()功能相同，只不过在其基础上，
//                 增加了const属性，不能用于修改元素。
// crend() 	       和rend()功能相同，只不过在其基础上，
//                 增加了const属性，不能用于修改元素。
// empty() 	       判断容器中是否有元素，若无元素，则返回true；
//                 反之，返回false。
// size() 	       返回当前容器实际包含的元素个数。
// max_size()      返回容器所能包含元素个数的最大值。
//                 这通常是一个很大的值，一般是2^32-1，所以很少会用到这个函数。
// front() 	       返回第一个元素的引用。
// back() 	       返回最后一个元素的引用。
// assign()        用新元素替换容器中原有内容。
// emplace_front() 在容器头部生成一个元素。
//                 该函数和push_front()的功能相同，但效率更高。
// push_front()    在容器头部插入一个元素。
// pop_front() 	   删除容器头部的一个元素。
// emplace_back()  在容器尾部直接生成一个元素。
//                 该函数和push_back()的功能相同，但效率更高。
// push_back() 	   在容器尾部插入一个元素。
// pop_back() 	   删除容器尾部的一个元素。
// emplace() 	     在容器中的指定位置插入元素。
//                 该函数和insert()功能相同，但效率更高。
// insert()  	     在容器中的指定位置插入元素。
// erase() 	       删除容器中一个或某区域内的元素。
// swap() 	       交换两个容器中的元素，
//                 必须保证这两个容器中存储的元素类型是相同的。
// resize()        调整容器的大小。
// clear() 	       删除容器存储的所有元素。
// splice()        将一个list容器中的元素插入到另一个容器的指定位置。
// remove(val) 	   删除容器中所有等于val的元素。
// remove_if() 	   删除容器中满足条件的元素。
// unique() 	     删除容器中相邻的重复元素，只保留一个。
// merge()         合并两个事先已排好序的list容器，
//                 并且合并之后的list容器依然是有序的。
// sort() 	       通过更改容器中元素的位置，将它们进行排序。
// reverse()       反转容器中元素的顺序。

// list容器还有一个std::swap(x,y)非成员函数，
// 它和swap()成员函数的功能完全相同，仅使用语法上有差异。
// C++11新添加的begin()和end()全局函数也同样适用于list容器。
// 即当操作对象为list容器时，其功能分别begin()、end()成员函数相同。

// 和array、vector、deque容器的迭代器相比，
// list容器迭代器最大的不同在于，其配备的迭代器类型为双向迭代器，
// 而不再是随机访问迭代器。
// 这意味着，假设p1和p2都是双向迭代器，
// 则支持使用++p1、p1++、p1--、p1++、*p1、p1==p2以及p1!=p2运算符，
// 但不支持以下操作（其中i为整数）：
// 1.p1[i]：不能通过下标访问list容器中指定位置处的元素。
// 2.p1-=i、p1+=i、p1+i 、p1-i：
//   双向迭代器p1不支持使用 -=、+=、+、- 运算符。
// 3.p1<p2、p1>p2、p1<=p2、p1>=p2：
//   双向迭代器p1、p2不支持使用<、>、<=、>=比较运算符。

// list模板类中，与添加或插入新元素相关的成员方法有如下几个：
// 1.push_front()：向list容器首个元素前添加新元素；
// 2.push_back()：向list容器最后一个元素后添加新元素；
// 3.emplace_front()：在容器首个元素前直接生成新的元素；
// 4.emplace_back()：在容器最后一个元素后直接生成新的元素；
// 5.emplace()：在容器的指定位置直接生成新的元素；
// 6.insert()：在指定位置插入新元素；
// 7.splice()：将其他list容器的多个元素添加到当前list的指定位置处。
// 除了insert()和splice()方法有多种语法格式外，
// 其它成员方法都仅有1种语法格式，同样是实现插入元素的功能，
// 无论是push_front()、push_back()还是insert()，
// 都有以emplace为名且功能和前者相同的成员函数。
// 后者是C++11标准新添加的，在大多数场景中，
// 都可以完全替代前者实现同样的功能。
// 更重要的是，实现同样的功能，emplace系列方法的执行效率更高。

// list insert()成员方法：
// 1.iterator insert(pos,elem)
//   在迭代器pos指定的位置之前插入一个新元素elem，
//   并返回表示新插入元素位置的迭代器。
// 2.iterator insert(pos,n,elem)
//   在迭代器pos指定的位置之前插入n个元素elem，
//   并返回表示第一个新插入元素位置的迭代器。
// 3.iterator insert(pos,first,last)
//   在pos位置之前，插入array、vector、deque等容器中，
//   位于[first,last)区域的所有元素，
//   并返回表示第一个新插入元素位置的迭代器。
// iterator insert(pos,initlist)
//   在迭代器pos指定的位置之前，插入初始化列表中所有的元素，
//   并返回表示第一个新插入元素位置的迭代器。

// 对list容器存储的元素执行删除操作，需要借助该容器模板类提供的成员函数。
// 幸运的是，相比其它STL容器模板类，
// list模板类提供了更多用来实现此操作的成员函数
// pop_front() 	删除位于list容器头部的一个元素。
// pop_back() 	删除位于list容器尾部的一个元素。
// erase() 	    该成员函数既可以删除list容器中指定位置处的元素，
//              也可以删除容器中某个区域内的多个元素。
// clear() 	    删除list容器存储的所有元素。
// remove(val) 	删除容器中所有等于val的元素。
// unique() 	  删除容器中相邻的重复元素，只保留一份。
// remove_if() 	删除容器中满足条件的元素。

// list容器底层存储结构：
// list容器时提到，该容器的底层是用双向链表实现的，
// 甚至一些STL版本中（比如SGI STL），
// list容器的底层实现使用的是双向循环链表。
// 使用链表存储数据，并不会将它们存储到一整块连续的内存空间中。
// 恰恰相反，各元素占用的存储空间（又称为节点）是独立的、分散的，
// 它们之间的线性关系通过指针来维持。
// 双向链表的各个节点中存储的不仅仅是元素的值，
// 还应包含2个指针，分别指向前一个元素和后一个元素。
// template<typename T,...>
// struct __List_node{
//     //...
//     __list_node<T>* prev;
//     __list_node<T>* next;
//     T myval;
//     //...
// }
// list容器定义的每个节点中，都包含*prev、*next和myval。其中：
// 1.prev指针用于指向前一个节点；
// 2.next指针用于指向后一个节点；
// 3.myval用于存储当前元素的值。
// 和array、vector这些容器迭代器的实现方式不同，
// 由于list容器的元素并不是连续存储的，所以该容器迭代器中，
// 必须包含一个可以指向list容器的指针，
// 并且该指针还可以借助重载的*、++、--、==、!=等运算符，
// 实现迭代器正确的递增、递减、取值等操作。
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
// SGI STL标准库中，list容器的底层实现为双向循环链表，
// 相比双向链表结构的好处是在构建list容器时，
// 只需借助一个指针即可轻松表示list容器的首尾元素。
// template <class T,...>
// class list
// {
//     // ...
//     // 指向链表的头节点，并不存放数据
//     __list_node<T>* node;
//     // ...以下还有list容器的构造函数以及很多操作函数
// }
// 为了更方便的实现list模板类提供的函数，该模板类在构建容器时，
// 会刻意在容器链表中添加一个空白节点，并作为list链表的首个节点（又称头节点）。
// 使用双向链表实现的list容器，其内部通常包含2个指针，
// 并分别指向链表中头部的空白节点和尾部的空白节点（也就是说，其包含2个空白节点）。
// 经常构造空的list容器，其用到的构造函数如下所示，
// 即便是创建空的list容器，它也包含有1个节点：
// list() {
//   empty_initialize();
// }
// void empty_initialize() {
//   node = get_node();  // 初始化节点
//   node->next = node;  // 前置节点指向自己
//   node->prev = node;  // 后置节点指向自己
// }
// 除此之外，list模板类中还提供有带参的构造函数，
// 它们的实现过程大致分为以下2步：
// 1.调用empty_initialize()函数，构造带有头节点的空list容器链表；
// 2.将各个参数按照次序插入到空的list容器链表中。
// 由此可以总结出，list容器实际上就是一个带有头节点的双向循环链表。
// 在此基础上，通过借助node头节点，就可以实现list容器中的所有成员函数：
// __list_iterator<T> begin(){return (*node).next;}
// __list_iterator<T> end(){return node;}
// bool empty() const{return (*node).next == node;}
// T& front() {return *begin();}
// T& back() {return *(--end();)}

// if(cont.size() == 0)
// if(cont.empty())
// 本质上上面两行代码是等价的，在实际场景中，到底应该使用哪一种呢？
// 建议使用empty()成员方法。
// 理由很简单，无论是哪种容器，只要其模板类中提供了empty()成员方法，
// 使用此方法都可以保证在O(1)时间复杂度内完成对容器是否为空的判断；
// 但对于list容器来说，使用size()成员方法判断容器是否为空，
// 可能要消耗O(n)的时间复杂度，这个结论适用于很多容器。
// 为什么list容器这么特殊呢？
// 这和list模板类提供了独有的splice()成员方法有关。
// 在实现list容器的过程中发现，
// 用户经常需要知道当前list容器中存有多少个元素，
// 于是想让size()成员方法的执行效率达到O(1)。
// 为了实现这个目的，必须重新设计list，
// 使它总能以最快的效率知道自己含有多少个元素。
// 要想令size()的执行效率达到O(1)，最直接的实现方式是：
// 在list模板类中设置一个专门用于统计存储元素数量的size变量，
// 其位于所有成员方法的外部。
// 与此同时，在每一个可用来为容器添加或删除元素的成员方法中，
// 添加对size变量值的更新操作。
// 由此，size()成员方法只需返回size这个变量即可达到时间复杂度O(1)。
// 不仅如此，由于list容器底层采用的是链式存储结构（也就是链表），
// 该结构最大的特点就是，某一链表中存有元素的节点，
// 无需经过拷贝就可以直接链接到其它链表中，
// 且整个过程只需要消耗O(1)的时间复杂度。
// 考虑到之所以选用list容器，就是看中了其底层存储结构的这一特性。
// 因此，作为list容器设计者，
// 自然也想将splice()方法的时间复杂度设计为O(1)。
// 这里就产生了一个矛盾，即如果将size()设计为O(1)时间复杂度，
// 则由于splice()成员方法会修改list容器存储元素的个数，
// 因此该方法中就需要添加更新size变量的代码（无疑是通过遍历链表来实现），
// 意味着splice()成员方法的执行效率将无法达到O(1)；
// 反之，如果将splice()成员方法的执行效率提高到O(1)，
// 则size()成员方法将无法实现O(1)的时间复杂度。
// 也就是说，list容器中的size()和splice()总有一个要做出让步，
// 即只能实现其中一个方法的执行效率达到O(1)。
// 值得一提的是，不同版本的STL标准库，其底层解决此冲突的抉择是不同的。
// 有些版本选择将splice()成员方法的执行效率达到O(1)，
// 而size()成员方法的执行效率为O(n)。
// 当然，有些版本的STL标准库中，选择将size()方法的执行效率设计为O(1)。
// 但不论怎样，选用empty()判断容器是否为空，效率总是最高的。
// 所以，如果程序中需要判断当前容器是否为空，应优先考虑使用empty()。

template <typename T>
void print(std::list<T>& l) {
  std::copy(l.begin(), l.end(), std::ostream_iterator<T>(std::cout, " "));
  std::cout << "[" << l.size() << "]" << std::endl;
}

void func1() {
  // list容器的创建：
  // 1.创建一个没有任何元素的空list容器：
  std::list<int> l1;
  print(l1);  // [0]

  // 2.创建一个包含n个元素的list容器：
  std::list<int> l2(10);  // 包含10个元素，每个值都为相应类型的默认值
  print(l2);              // 0 0 0 0 0 0 0 0 0 0 [10]

  // 3.创建一个包含n个元素的list容器，并为每个元素指定初始值：
  std::list<int> l3(10, 5);  // 创建了一个包含10个元素并且值都为5的容器
  print(l3);                 // 5 5 5 5 5 5 5 5 5 5 [10]
  std::list<int> l4{1, 2, 3};
  print(l4);  // 1 2 3 [3]

  // 4.在已有list容器的情况下，通过拷贝该容器可以创建新的list容器，
  //   采用此方式，必须保证新旧容器存储的元素类型一致：
  std::list<int> l5(l3);
  std::list<int> l6 = l4;
  print(l5);  // 5 5 5 5 5 5 5 5 5 5 [10]
  print(l6);  // 1 2 3 [3]

  // 5.通过拷贝其他类型容器（或者普通数组）中指定区域内的元素创建新的list容器：
  int a[] = {1, 2, 3, 4, 5};
  std::list<int> l7(a, a + 5);
  print(l7);  // 1 2 3 4 5 [5]
  std::array<int, 5> arr{11, 12, 13, 14, 15};
  std::list<int> l8(arr.begin() + 2, arr.end());
  print(l8);  // 13 14 15 [3]
}

void func2() {
  // 遍历：
  std::list<char> l{'a', 'b', 'c', 'd', 'e'};
  for (std::list<char>::iterator it = l.begin(); it != l.end(); ++it) {
    std::cout << *it;
  }
  std::cout << std::endl;  // abcde

  // 程序中比较迭代器用的是!=运算符，因为它不支持<等运算符：
  for (std::list<char>::reverse_iterator it = l.rbegin(); it != l.rend();
       ++it) {
    std::cout << *it;
  }
  std::cout << std::endl;  // edcba

  for (char& c : l) {
    std::cout << c;
  }
  std::cout << std::endl;  // abcde

  for (auto it = std::begin(l); it != std::end(l); ++it) {
    std::cout << *it;
  }
  std::cout << std::endl;  // abcde
}

void func3() {
  // 不同于之前学过的STL容器，访问list容器中存储元素的方式很有限，
  // 要么使用front()和back()成员函数，要么使用list容器迭代器。
  // list容器不支持随机访问，未提供下标操作符[]和at()成员函数，
  // 也没有提供data()成员函数。
  // 通过front()和back()成员函数，
  // 可以分别获得list容器中第一个元素和最后一个元素的引用形式。
  std::list<int> l{1, 2, 3, 4};
  int& first = l.front();
  int& last = l.back();
  std::cout << first << " " << last << std::endl;  // 1 4
  first = 10;
  last = 20;
  std::cout << l.front() << " " << l.back() << std::endl;  // 10 20

  // 如果想访问list容存储的其他元素，就只能使用list容器的迭代器：
  auto it = l.begin();
  while (it != l.end()) {
    (*it)++;
    ++it;
  }
  print(l);  // 11 3 4 21 [4]

  l.assign({100, 200, 300});
  print(l);  // 100 200 300 [3]

  std::cout << l.max_size() << std::endl;                 // 384307168202282325
  std::cout << std::boolalpha << l.empty() << std::endl;  // false
}

void func4() {
  std::list<int> l{1, 2, 3};

  l.push_front(0);
  print(l);  // 0 1 2 3 [4]

  l.push_back(4);
  print(l);  // 0 1 2 3 4 [5]

  l.emplace_front(-1);
  print(l);  // -1 0 1 2 3 4 [6]

  l.emplace_back(5);
  print(l);  // -1 0 1 2 3 4 5 [7]

  // emplace(pos,value)
  // 其中pos表示指明位置的迭代器，value为要插入的元素值
  l.emplace(l.end(), 6);
  print(l);  // -1 0 1 2 3 4 5 6 [8]

  l.insert(l.begin(), -2);
  print(l);  // -2 -1 0 1 2 3 4 5 6 [9]

  l.insert(l.end(), 2, 7);
  print(l);  // -2 -1 0 1 2 3 4 5 6 7 7 [11]

  std::array<int, 3> a{8, 9, 10};
  l.insert(l.end(), a.begin(), a.end());
  print(l);  // -2 -1 0 1 2 3 4 5 6 7 7 8 9 10 [14]

  l.insert(l.end(), {11, 12});
  print(l);  // -2 -1 0 1 2 3 4 5 6 7 7 8 9 10 11 12 [16]

  l.pop_front();
  print(l);  // -1 0 1 2 3 4 5 6 7 7 8 9 10 11 12 [15]

  l.pop_back();
  print(l);  // -1 0 1 2 3 4 5 6 7 7 8 9 10 11 [14]

  l.clear();
  print(l);  // [0]
}

void func5() {
  // 删除list容器中position迭代器所指位置处的元素：
  std::list<int> l{1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11};
  print(l);  // 1 2 3 4 5 6 7 8 9 10 11 [11]
  auto del = l.begin();
  ++del;
  l.erase(del);
  print(l);  // 1 3 4 5 6 7 8 9 10 11 [10]

  // 可实现删除list容器中first迭代器和last迭代器限定区域内的所有元素，
  // 包括first指向的元素，但不包括last指向的元素：
  del = l.begin();
  del++;
  del++;
  auto del1 = l.end();
  --del1;
  --del1;
  --del1;
  std::cout << "d:[" << *del << " " << *del1 << ")" << std::endl;  // d:[4 9)
  l.erase(del, del1);
  print(l);  // 1 3 9 10 11 [5]

  // erase()成员函数是按照被删除元素所在的位置来执行删除操作，
  // 如果想根据元素的值来执行删除操作，可以使用remove()成员函数：
  l.remove(3);
  print(l);  // 1 9 10 11 [4]

  // 通过将自定义的谓词函数（不限定参数个数）传给remove_if()成员函数，
  // list容器中能使谓词函数成立的元素都会被删除：
  l.remove_if([](int v) { return (v < 10); });
  print(l);  // 10 11 [2]
}

bool demo(int first, int second) { return (first == 2 && second == 3); }
void func6() {
  std::list<int> l{1, 2, 2, 3, 4, 7, 5, 2, 6, 7, 7};
  print(l);  // 1 2 2 3 4 7 5 2 6 7 7 [11]

  l.unique();  // 删除相邻重复的元素，仅保留一份
  print(l);    // 1 2 3 4 7 5 2 6 7 [9]

  l.unique(demo);  // demo为二元谓词函数，自定义的去重规则
  print(l);        // 1 2 4 7 5 2 6 7 [8]

  l.unique([](int i, int j) -> bool { return i * 2 == j; });
  print(l);  // 1 4 7 5 2 6 7 [7]
}

void func7() {
  // list splice()成员方法：
  // 1.void splice(iterator position, list& x);
  //   position为迭代器，用于指明插入位置；
  //   x为另一个list容器。
  //   此格式的splice()方法的功能是，
  //   将x容器中存储的所有元素全部移动当前list容器中position指明的位置处。
  // 2.void splice(iterator position, list& x, iterator i);
  //   position为迭代器，用于指明插入位置；
  //   x为另一个list容器；
  //   i也是一个迭代器，用于指向x容器中某个元素。
  //   此格式的splice()方法的功能是将x容器中i指向的元素，
  //   移动到当前容器中position指明的位置处。
  // 3.void splice(iterator position, list& x, iterator first, iterator last);
  //   position为迭代器，用于指明插入位置；
  //   x为另一个list容器；
  //   first和last都是迭代器，[fist,last)用于指定x容器中的某个区域。
  //   此格式的splice()方法的功能是将x容器[first,last)范围内所有的元素，
  //   移动到当前容器position指明的位置处。

  std::list<int> l1{1, 2, 3, 4};
  std::list<int> l2{10, 20, 30};
  std::list<int>::iterator it = ++l1.begin();
  l1.splice(it, l2);
  print(l1);  // 1 10 20 30 2 3 4 [7]
  print(l2);  // [0]
  l2.splice(l2.begin(), l1, it);
  print(l1);  // 1 10 20 30 3 4 [6]
  print(l2);  // 2 [1]
  l2.splice(l2.begin(), l1, l1.begin(), l1.end());
  print(l1);  // [0]
  print(l2);  // 1 10 20 30 3 4 2 [7]
}

void func8() {
  // list容器在进行插入insert()、接合splice()等操作时，
  // 都不会造成原有的list迭代器失效，甚至进行删除操作，
  // 而只有指向被删除元素的迭代器失效，其他迭代器不受任何影响。
  std::list<char> l{'h', 'e', 'l', 'l', 'o'};
  std::list<char>::iterator begin = l.begin();
  std::list<char>::iterator end = l.end();
  l.insert(begin, '1');
  l.insert(end, '1');
  while (begin != end) {
    std::cout << *begin;
    ++begin;
  }
  std::cout << std::endl;  // hello1
  print(l);                // 1 h e l l o 1 [7]
  // 可以看到，在进行插入操作之后，仍使用先前创建的迭代器遍历容器，
  // 虽然程序不会出错，但由于插入位置的不同，可能会遗漏新插入的元素。
}

void func9() {
  std::list<int> l1{2, 1, 3, 4, 6, 5};
  std::list<int> l2{6, 7, 9, 8, 10};

  l1.merge(l2);
  print(l1);  // 2 1 3 4 6 5 6 7 9 8 10 [11]
  print(l2);  // [0]

  l1.sort();
  print(l1);  // 1 2 3 4 5 6 6 7 8 9 10 [11]

  l1.reverse();
  print(l1);  // 10 9 8 7 6 6 5 4 3 2 1 [11]

  l2.resize(5);
  print(l2);  // 0 0 0 0 0 [5]

  l1.swap(l2);
  print(l1);  // 0 0 0 0 0 [5]
  print(l2);  // 10 9 8 7 6 6 5 4 3 2 1 [11]

  std::swap(l1, l2);
  print(l1);  // 10 9 8 7 6 6 5 4 3 2 1 [11]
  print(l2);  // 0 0 0 0 0 [5]
}

int main(int argc, char* argv[]) {
  if (argc < 2) {
    std::cout << argv[0] << " i [0 - 8]" << std::endl;
    return 0;
  }
  int type = argv[1][0] - '0';
  switch (type) {
    case 0:
      func1();
      break;
    case 1:
      func2();
      break;
    case 2:
      func3();
      break;
    case 3:
      func4();
      break;
    case 4:
      func5();
      break;
    case 5:
      func6();
      break;
    case 6:
      func7();
      break;
    case 7:
      func8();
      break;
    case 8:
      func9();
      break;
    default:
      std::cout << "invalid type" << std::endl;
      break;
  }

  return 0;
}