#include <array>
#include <deque>
#include <iostream>
#include <iterator>
#include <vector>

// deque是double-ended queue的缩写，又称双端队列容器。
// deque容器和vecotr容器有很多相似之处，比如：
// 1.deque容器也擅长在序列尾部添加或删除元素（时间复杂度为O(1)），而不擅长在序列中间添加或删除元素。
// 2.deque容器也可以根据需要修改自身的容量和大小。
// 和vector不同的是：
// 1.deque还擅长在序列头部添加或删除元素，所耗费的时间复杂度也为常数阶O(1)。
// 2.当需要向序列两端频繁的添加或删除元素时，应首选deque容器。
// 并且更重要的一点是，deque容器中存储元素并不能保证所有元素都存储到连续的内存空间中。

// deque容器可利用的成员函数:
// begin() 	       返回指向容器中第一个元素的迭代器。
// end() 	         返回指向容器最后一个元素所在位置后一个位置的迭代器，通常和begin()结合使用。
// rbegin()        返回指向最后一个元素的迭代器。
// rend() 	       返回指向第一个元素所在位置前一个位置的迭代器。
// cbegin()        和begin()功能相同，只不过在其基础上，增加了const属性，不能用于修改元素。
// cend() 	       和end()功能相同，只不过在其基础上，增加了const属性，不能用于修改元素。
// crbegin()       和rbegin()功能相同，只不过在其基础上，增加了const属性，不能用于修改元素。
// crend() 	       和rend()功能相同，只不过在其基础上，增加了const属性，不能用于修改元素。
// size() 	       返回实际元素个数。
// max_size()      返回容器所能容纳元素个数的最大值。这通常是一个很大的值，一般是232-1，很少会用这个函数。
// resize()        改变实际元素的个数。
// empty() 	       判断容器中是否有元素，若无元素，则返回true；反之，返回false。
// shrink_to_fit() 将内存减少到等于当前元素实际所使用的大小。
// at() 	         使用经过边界检查的索引访问元素。
// front() 	       返回第一个元素的引用。
// back() 	       返回最后一个元素的引用。
// assign()        用新元素替换原有内容。
// push_back()     在序列的尾部添加一个元素。
// push_front()    在序列的头部添加一个元素。
// pop_back()      移除容器尾部的元素。
// pop_front()     移除容器头部的元素。
// insert() 	     在指定的位置插入一个或多个元素。
// erase() 	       移除一个元素或一段元素。
// clear() 	       移出所有的元素，容器大小变为0。
// swap() 	       交换两个容器的所有元素。
// emplace()       在指定的位置直接生成一个元素。
// emplace_front() 在容器头部生成一个元素。和push_front()的区别是，该函数直接在容器头构造元素，省了复制移动的过程。
// emplace_back()  在容器尾部生成一个元素。和push_back()的区别是，该函数直接在容器尾构造元素，省了复制移动的过程。
// 和vector相比，额外增加了实现在容器头部添加和删除元素的成员函数，同时删除了capacity()、reserve()和data()成员函数。

// 和array、vector相同，C++11标准库新增的begin()和end()这2个全局函数也适用于deque容器。
// 这2个函数的操作对象既可以是容器，也可以是普通数组。
// 当操作对象是容器时，它和容器包含的begin()和end()成员函数的功能完全相同；
// 如果操作对象是普通数组，则begin()函数返回的是指向数组第一个元素的指针，
// 同样end()返回指向数组中最后一个元素之后一个位置的指针。

// deque容器还有一个std::swap(x,y)非成员函数（其中x和y是存储相同类型元素的deque容器），
// 它和swap()成员函数的功能完全相同，仅使用语法上有差异。

// deque容器底层实现原理:
// 和vector容器采用连续的线性空间不同，deque容器存储数据的空间是由一段一段等长的连续空间构成，
// 各段空间之间并不一定是连续的，可以位于在内存的不同区域。
// 为了管理这些连续空间，deque容器用数组（数组名假设为map）存储着各个连续空间的首地址。
// 也就是说，map数组中存储的都是指针，指向那些真正用来存储数据的各个连续空间通过建立map数组，
// deque容器申请的这些分段的连续空间就能实现“整体连续”的效果。
// 换句话说，当deque容器需要在头部或尾部增加存储空间时，它会申请一段新的连续空间，
// 同时在map数组的开头或结尾添加指向该空间的指针，由此该空间就串接到了deque容器的头部或尾部。
// 如果map数组满了怎么办？
// 很简单，再申请一块更大的连续空间供map数组使用，
// 将原有数据（很多指针）拷贝到新的map数组中，然后释放旧的空间。
// deque容器的分段存储结构，提高了在序列两端添加或删除元素的效率，但也使该容器迭代器的底层实现变得更复杂。
// 由于deque容器底层将序列中的元素分别存储到了不同段的连续空间中，
// 因此要想实现迭代器的功能，必须先解决如下2个问题：
// 1.迭代器在遍历deque容器时，必须能够确认各个连续空间在map数组中的位置；
// 2.迭代器在遍历某个具体的连续空间时，必须能够判断自己是否已经处于空间的边缘位置。
//   如果是，则一旦前进或者后退，就需要跳跃到上一个或者下一个连续空间中。
// 为了实现遍历deque容器的功能，deque迭代器定义了如下的结构：
// template<class T,...>
// struct __deque_iterator{
//     ...
//     T* cur;  // 指向当前正在遍历的元素；
//     T* first;  // 指向当前连续空间的首地址；
//     T* last;  // 指向当前连续空间的末尾地址；
//     map_pointer node;  // map_pointer等价于T**,它是一个二级指针，用于指向map数组中存储的指向当前连续空间的指针。
// }
// 借助这4个指针，deque迭代器对随机访问迭代器支持的各种运算符进行了重载，
// 能够对deque分段连续空间中存储的元素进行遍历。
// 当迭代器处于当前连续空间边缘的位置时，如果继续遍历，
// 就需要跳跃到其它的连续空间中，该函数可用来实现此功能：
// void set_node(map_pointer new_node) {
//   node = new_node;    // 记录新的连续空间在 map 数组中的位置
//   first = *new_node;  // 更新first指针
//   // 更新last指针，difference_type(buffer_size())表示每段连续空间的长度
//   last = first + difference_type(buffer_size());
// }
// 重载*运算符
// reference operator*() const { return *cur; }
// pointer operator->() const { return &(operator*()); }
// 重载前置++运算符
// self& operator++() {
//   ++cur;
//   // 处理cur处于连续空间边缘的特殊情况
//   if (cur == last) {
//     // 调用该函数，将迭代器跳跃到下一个连续空间中
//     set_node(node + 1);
//     // 对cur重新赋值
//     cur = first;
//   }
//   return *this;
// }
// 重置前置--运算符
// self& operator--() {
//   // 如果cur位于连续空间边缘，则先将迭代器跳跃到前一个连续空间中
//   if (cur == first) {
//     set_node(node - 1);
//     cur == last;
//   }
//   --cur;
//   return *this;
// }
// deque容器除了维护先前讲过的map数组，还需要维护start、finish这2个deque迭代器。以下为deque容器的定义：
// template<class _Ty,
//     class _Alloc = allocator<_Ty>>
// class deque{
//     ...
// protected:
//     iterator start;  // start迭代器记录着map数组中首个连续空间的信息
//     iterator finish;  // finish迭代器记录着map数组中最后一个连续空间的信息
//     map_pointer map;
// ...
// }
// 和普通deque迭代器不同，start迭代器中的cur指针指向的是连续空间中首个元素；
// 而finish迭代器中的cur指针指向的是连续空间最后一个元素的下一个位置。
// 借助start和finish，以及deque迭代器中重载的诸多运算符，就可以实现deque容器提供的大部分成员函数:
// iterator begin() { return start; }
// iterator end() { return finish; }
// reference front() { return *start; }
// reference back() {
//   iterator tmp = finish;
//   --tmp;
//   return *tmp;
// }
// size_type size() const {
//   return finish - start;
// }
// bool empty() const { return finish == start; }

void print(std::deque<int>& d) {
  std::copy(d.begin(), d.end(), std::ostream_iterator<int>(std::cout, " "));
  std::cout << "[" << d.size() << "]" << std::endl;
}

void func1() {  // 创建deque容器
  // 1.创建一个没有任何元素的空deque容器：
  std::deque<int> d1;
  print(d1);  // [0]
  // 和空array容器不同，空的deque容器在创建之后可以做添加或删除，
  // 因此这种简单创建deque容器的方式比较常见。

  // 2.创建一个具有n个元素的deque容器，其中每个元素都采用对应类型的默认值：
  std::deque<int> d2(10);  // 创建一个具有10个元素（默认都为0）的deque容器
  print(d2);               // 0 0 0 0 0 0 0 0 0 0 [10]

  // 3.创建一个具有n个元素的deque容器，并为每个元素都指定初始值：
  std::deque<int> d3(4, 5);  // 创建了一个包含10个元素（都为5）的deque容器
  print(d3);                 // 5 5 5 5 [4]

  // 4.在已有deque容器的情况下，可以通过拷贝该容器创建一个新的deque容器：
  std::deque<int> d4(d3);  // 采用此方式，必须保证新旧容器存储的元素类型一致。
  std::deque<int> d5 = d2;
  print(d4);  // 5 5 5 5 [4]
  print(d5);  // 0 0 0 0 0 0 0 0 0 0 [10]

  // 5.通过拷贝其他类型容器中指定区域内的元素（也可以是普通数组），可以创建一个新容器：
  int a[] = {1, 2, 3, 4, 5};
  std::deque<int> d6(a, a + 5);  // 拷贝普通数组，创建deque容器
  std::array<int, 5> arr{11, 12, 13, 14, 15};  // 所有类型的容器都可以拷贝array
  std::deque<int> d7(arr.begin() + 2, arr.end());  // 拷贝arr容器中的{13,14,15}
  print(d6);                                       // 1 2 3 4 5 [5]
  print(d7);                                       // 13 14 15 [3]

  // 6.在创建的同时指定初始值以及元素个数：
  std::deque<int> d8{1, 2, 3};
  print(d8);  // 1 2 3 [3]
  std::deque<int> d9 = {4, 5, 6};
  print(d9);  // 4 5 6 [3]
}

void func2() {  // 遍历
  // begin()和end()分别用于指向首元素和尾元素+1的位置
  std::deque<int> d{1, 2, 3, 4, 5};
  for (auto i = d.begin(); i < d.end(); i++) {
    std::cout << *i << " ";
  }
  std::cout << std::endl;  // 1 2 3 4 5

  // 全局的begin()和end()函数，当操作对象为容器时，它们的功能是上面的begin()/end()成员函数一样
  for (auto i = begin(d); i < end(d); i++) {
    std::cout << *i << " ";
  }
  std::cout << std::endl;  // 1 2 3 4 5

  // cbegin()/cend()成员函数和begin()/end()唯一不同的是，前者返回的是const类型的正向迭代器，
  // 这就意味着，由cbegin()和cend()成员函数返回的迭代器，可以用来遍历容器内的元素，
  // 也可以访问元素，但是不能对所存储的元素进行修改。
  auto first = d.cbegin();
  auto end = d.cend();
  // 常量迭代器不能用来修改容器中的元素值
  // *(first + 1) = 6;  // 尝试修改容器中元素2的值
  // *(end - 1) = 10;  // 尝试修改容器中元素5的值
  // 常量迭代器可以用来遍历容器、访问容器中的元素
  while (first < end) {
    std::cout << *first << " ";
    ++first;
  }
  std::cout << std::endl;  // 1 2 3 4 5

  // deque模板类中还提供了rbegin()和rend()成员函数。
  for (auto i = d.rbegin(); i < d.rend(); i++) {
    std::cout << *i << " ";
  }
  std::cout << std::endl;  // 5 4 3 2 1
  // crbegin()/crend()组合和rbegin()/crend()组合唯一的区别在于，
  // 前者返回的迭代器为const类型迭代器，不能用来修改容器中的元素，除此之外在使用上和后者完全相同。

  for (auto i : d) {
    std::cout << i << " ";
  }
  std::cout << std::endl;  // 1 2 3 4 5
}

void func3() {  // 访问和修改元素
  // 1.和array、vector容器一样，可以采用普通数组访问存储元素的方式，访问deque容器中的元素
  std::deque<int> d{1, 2, 3, 4};
  std::cout << d[1] << std::endl;  // 2
  d[1] = 5;
  std::cout << d[1] << std::endl;  // 5
  // d[8] = 6;  // 越界，可能崩溃
  // 效率高，不用每次都检查是否越界

  // 2.如果想有效地避免越界访问，可以使用deque模板类提供的at()成员函数：
  std::cout << d.at(1) << std::endl;  // 5
  d.at(1) = 4;
  std::cout << d.at(1) << std::endl;  // 4
  try {
    d.at(8) = 6;
  } catch (std::out_of_range e) {
    std::cout << e.what() << std::endl;
    // deque::_M_range_check: __n (which is 8)>= this->size() (which is 4)
  }

  // 3.deque提供了2个成员函数，即front()和back()，
  //   它们返回vector容器中第一个和最后一个元素的引用，
  //   通过利用它们的返回值，可以访问（甚至修改）容器中的首尾元素：
  std::cout << d.front() << std::endl;  // 1
  std::cout << d.back() << std::endl;   // 4
  d.front() = 10;
  d.back() = 20;
  std::cout << d.front() << std::endl;  // 10
  std::cout << d.back() << std::endl;   // 20
  // 和vector容器不同，deque容器没有提供data()成员函数，
  // 同时deque容器在存储元素时，也无法保证其会将元素存储在连续的内存空间中，
  // 因此尝试使用指针去访问deque容器中指定位置处的元素，是非常危险的。

  // 另外，结合deque模板类中和迭代器相关的成员函数，可以实现遍历deque容器中指定区域元素的方法：
  print(d);  // 10 4 3 20 [4]
  auto first = d.begin() + 1;
  auto end = d.end() - 1;
  while (first < end) {
    std::cout << *first << " ";
    ++first;
  }
  std::cout << std::endl;  // 4 3
}

void func4() {
  std::deque<int> d;

  // 调用push_back()向容器尾部添加数据
  d.push_back(3);
  d.push_back(4);
  print(d);  // 3 4 [2]

  // 调用push_front()向容器头部添加数据
  d.push_front(2);
  d.push_front(1);
  print(d);  // 1 2 3 4 [4]

  // 调用pop_back()移除容器尾部的一个数据
  d.pop_back();
  print(d);  // 1 2 3 [3]

  // 调用pop_front()移除容器头部的一个数据
  d.pop_front();
  print(d);  // 2 3 [2]

  // 调用emplace系列函数，向容器中直接生成数据
  d.emplace_back(4);
  d.emplace_front(1);
  print(d);  // 1 2 3 4 [4]
}

void func5() {
  std::deque<int> d{1, 5};

  // emplace()需要2个参数，第一个为指定插入位置的迭代器，第二个是插入的值
  d.emplace(d.begin() + 1, 4);
  print(d);  // 1 4 5 [3]

  d.insert(d.begin() + 1, 3);
  print(d);  // 1 3 4 5 [4]
  d.insert(d.end(), 2, 6);
  print(d);  // 1 3 4 5 6 6 [6]

  std::array<int, 3> a{7, 8, 9};
  d.insert(d.end(), a.begin(), a.end());
  print(d);  // 1 3 4 5 6 6 7 8 9 [9]
  d.insert(d.end(), {10, 11});
  print(d);  // 1 3 4 5 6 6 7 8 9 10 11 [11]

  // erase()可以接受一个迭代器表示要删除元素所在位置：
  d.erase(d.begin());
  print(d);  // 3 4 5 6 6 7 8 9 10 11 [10]
  // 也可以接受2个迭代器，表示要删除元素所在的区域：
  d.erase(d.begin() + 3, d.end() - 2);
  print(d);                     // 3 4 5 10 11 [5]
  d.erase(d.begin(), d.end());  // 等同于d.clear()
  print(d);                     // [0]
}

class A {
 public:
  A(int num) : num(num) { std::cout << "create" << std::endl; }
  A(const A& other) : num(other.num) { std::cout << "copy" << std::endl; }
  A(A&& other) : num(other.num) { std::cout << "move" << std::endl; }
  A& operator=(const A& other) {
    this->num = other.num;
    return *this;
  }

 private:
  int num;
};
class B {
 public:
  B(int num) : num(num) { std::cout << "create" << std::endl; }
  B(const B& other) : num(other.num) { std::cout << "copy" << std::endl; }
  B& operator=(const B& other) {
    this->num = other.num;
    return *this;
  }

 private:
  int num;
};
void func6() {
  std::deque<A> da;

  // emplace系列函数都只调用了构造函数，而没有调用移动构造函数，提高了代码的运行效率
  da.emplace(da.begin(), 2);
  // create
  da.emplace_front(2);
  // create
  da.emplace_back(2);
  // create

  da.insert(da.begin(), 2);
  // create
  // move
  da.push_front(2);
  // create
  // move
  da.push_back(2);
  // create
  // move

  std::deque<B> db;
  db.emplace(db.begin(), 2);
  // create
  db.emplace_front(2);
  // create
  db.emplace_back(2);
  // create

  db.insert(db.begin(), 2);
  // create
  // copy
  db.push_front(2);
  // create
  // copy
  db.push_back(2);
  // create
  // copy
}

void func7() {
  // 当向deque容器添加元素时，deque容器会申请更多的内存空间，
  // 同时其包含的所有元素可能会被复制或移动到新的内存地址（原来的内存会释放），
  // 这会导致之前创建的迭代器失效。
  std::deque<int> d;
  auto db = d.begin();

  // 迭代器的功能是遍历容器，在遍历的同时可以访问（甚至修改）容器中的元素，
  // 但迭代器不能用来初始化空的deque容器：
  // *db = 2;
  d.push_back(1);
  print(d);                       // 1 [1]
  std::cout << *db << std::endl;  // 1

  d.push_back(2);
  d.push_front(3);
  print(d);                       // 3 1 2 [3]
  std::cout << *db << std::endl;  // 1, 没失效？
}

void func8() {
  std::deque<int> d{1, 2, 3, 4, 5};
  std::cout << std::boolalpha << d.empty() << std::endl;  // false
  std::cout << d.max_size() << std::endl;                 // 2305843009213693951

  d.resize(3);
  print(d);  // 1 2 3 [3]
  d.resize(8);
  print(d);           // 1 2 3 0 0 0 0 0 [8]
  d.shrink_to_fit();  // 有什么用？
  print(d);           // 1 2 3 0 0 0 0 0 [8]
  d.clear();
  print(d);  // [0]

  // 要在clear后用么？虽然元素为0，但是也有额外的内存被占用
  d.shrink_to_fit();
}

void func9() {
  std::deque<int> d1{1, 2, 3, 4, 5};
  std::deque<int> d2{6, 7, 8, 9, 10};
  print(d1);  // 1 2 3 4 5 [5]
  print(d2);  // 6 7 8 9 10 [5]

  d1.swap(d2);
  print(d1);  // 6 7 8 9 10 [5]
  print(d2);  // 1 2 3 4 5 [5]

  std::swap(d1, d2);
  print(d1);  // 1 2 3 4 5 [5]
  print(d2);  // 6 7 8 9 10 [5]
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