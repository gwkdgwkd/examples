#include <iostream>
#include <vector>

using namespace std;

// vector容器是STL中最常用的容器之一，它和array容器非常类似，都可以看做是对C++普通数组的“升级版”。不同之处在于，array实现的是静态数组（容量固定的数组），
// 而vector实现的是一个动态数组，即可以进行元素的插入和删除，在此过程中，vector会动态调整所占用的内存空间，整个过程无需人工干预。
// vector常被称为向量容器，因为该容器擅长在尾部插入或删除元素，在常量时间内就可以完成，时间复杂度为O(1)；而对于在容器头部或者中部插入或删除元素，则花费时间要
// 长一些（移动元素需要耗费时间），时间复杂度为线性阶O(n)。
// vector容器以类模板vector<T>（T 表示存储元素的类型）的形式定义在<vector>头文件中，并位于std命名空间中

// 相比array容器，vector提供了更多了成员函数:
// begin() 	    返回指向容器中第一个元素的迭代器。
// end() 	    返回指向容器最后一个元素所在位置后一个位置的迭代器，通常begin()结合使用。
// rbegin() 	返回指向最后一个元素的迭代器。
// rend() 	    返回指向第一个元素所在位置前一个位置的迭代器。
// cbegin() 	和begin()功能相同，只不过在其基础上，增加了const属性，不能用于修改元素。
// cend() 	    和end()功能相同，只不过在其基础上，增加了const属性，不能用于修改元素。
// crbegin() 	和rbegin()功能相同，只不过在其基础上，增加了const属性，不能用于修改元素。
// crend() 	    和rend()功能相同，只不过在其基础上，增加了const属性，不能用于修改元素。
// size() 	    返回实际元素个数。
// max_size()   返回元素个数的最大值。这通常是一个很大的值，一般是232-1，所以我们很少会用到这个函数。
// resize() 	改变实际元素的个数。
// capacity()   返回当前容量。
// empty() 	    判断容器中是否有元素，若无元素，则返回true；反之，返回false。
// reserve() 	增加容器的容量。
// shrink _to_fit() 将内存减少到等于当前元素实际所使用的大小。
// operator[ ]  重载了[ ]运算符，可以向访问数组中元素那样，通过下标即可访问甚至修改vector容器中的元素。
// at() 	    使用经过边界检查的索引访问元素。
// front() 	    返回第一个元素的引用。
// back() 	    返回最后一个元素的引用。
// data() 	    返回指向容器中第一个元素的指针。
// assign() 	用新元素替换原有内容。
// push_back()  在序列的尾部添加一个元素。
// pop_back()   移出序列尾部的元素。
// insert() 	在指定的位置插入一个或多个元素。
// erase() 	    移出一个元素或一段元素。
// clear() 	    移出所有的元素，容器大小变为0。
// swap() 	    交换两个容器的所有元素。
// emplace() 	在指定的位置直接生成一个元素。
// emplace_back() 在序列尾部生成一个元素。
// 除此之外，C++ 11标准库还新增加了begin()和end()这2个函数
// vector容器还有一个std::swap(x, y)非成员函数（其中x和y是存储相同类型元素的vector容器），它和swap()成员函数的功能完全相同，仅使用语法上有差异。

int main() {
  // 创建vector容器的几种方式:
  // 1 空的vector容器
  // 这是一个空的vector容器，因为容器中没有元素，所以没有为其分配空间。当添加第一个元素（比如使用push_back()函数）时，vector会自动分配内存。
  std::vector<double> values1;
  values1.reserve(20);  // 在创建好空容器上，通过调用reserve()来增加容器的容量
  // 这样就设置了容器的内存分配，即至少可以容纳20个元素。注意，如果vector的容量在执行此语句之前，已经大于或等于20个元素，那么这条语句什么也不做；
  // 另外，调用reserve()不会影响已存储的元素，也不会生成任何元素，即values容器内此时仍然没有任何元素。
  // 如果调用reserve()来增加容器容量，之前创建好的任何迭代器（例如开始迭代器和结束迭代器）都可能会失效，这是因为，为了增加容器的容量，vector<T>容器的元素可能
  // 已经被复制或移到了新的内存地址。所以后续再使用这些迭代器时，最好重新生成一下。
  // 2 在创建的同时指定初始值以及元素个数, 创建了一个含有8个素数的 vector 容器
  std::vector<int> primes{2, 3, 5, 7, 11, 13, 17, 19};
  // 3 在创建vector容器时，也可以指定元素个数，values容器开始时就有20个元素，它们的默认初始值都为0
  std::vector<double> values2(20);
  // 圆括号()和大括号{}是有区别的，前者(20)表示元素的个数，而后者{20}则表示vector容器中只有一个元素20。
  // 如果不想用0作为默认值，也可以指定一个其它值。圆括号()中的2个参数，既可以是常量，也可以用变量来表示
  std::vector<double> values3(20, 1.0);
  int num = 20;
  double value = 1.0;
  std::vector<double> values4(num, value);
  // 4 通过存储元素类型相同的其它vector容器，也可以创建新的vector容器
  std::vector<char> value4(5, 'c');
  std::vector<char> value5(value4);
  // 如果不想复制其它容器中所有的元素，可以用一对指针或者迭代器来指定初始值的范围
  int array[] = {1, 2, 3};
  std::vector<int> values6(array, array + 2);  // values将保存{1,2}
  std::vector<int> value7{1, 2, 3, 4, 5};
  std::vector<int> value8(std::begin(value7), std::begin(value7) + 3);  // 1,2,3

  vector<char> value11;  // 初始化一个空vector容量
  // 向value容器中的尾部依次添加S、T、L字符
  value11.push_back('S');
  value11.push_back('T');
  value11.push_back('L');
  // 调用size()成员函数容器中的元素个数
  printf("元素个数为：%d\n", value11.size());
  // 使用迭代器遍历容器
  for (auto i = value11.begin(); i < value11.end(); i++) {
    cout << *i << " ";
  }
  cout << endl;
  // 向容器开头插入字符
  value11.insert(value11.begin(), 'C');
  cout << "首个元素为：" << value11.at(0) << endl;
  // 元素个数为：3
  // S T L
  // 首个元素为：C

  // begin()和end()成员函数，它们分别用于指向「首元素」和「尾元素+1」的位置,迭代器对象是由vector对象的成员函数begin()和end()返回的。可以像使用普通指针那样上使用它们。
  vector<int> values33{1, 2, 3, 4, 5};
  auto first2 = values33.begin();
  auto end2 = values33.end();
  // 还可以使用全局的begin()和end()函数来从容器中获取迭代器：
  // auto first = std::begin(values);
  // auto end = std::end (values);
  while (first2 != end2) {
    cout << *first2 << " ";
    ++first2;
  }
  cout << endl;
  // 1 2 3 4 5

  // cbegin()/cend()成员函数和begin()/end()唯一不同的是，前者返回的是const类型的正向迭代器，这就意味着，由cbegin()和cend()成员函数返回的迭代器，可以用来遍历容器内的
  // 元素，也可以访问元素，但是不能对所存储的元素进行修改。
  auto first3 = values33.cbegin();
  auto end3 = values33.cend();
  while (first3 != end3) {
    // *first3 = 10;  // 不能修改元素
    cout << *first3 << " ";
    ++first3;
  }
  cout << endl;
  // 1 2 3 4 5

  // vector模板类中还提供了rbegin()和rend()成员函数，分别表示指向最后一个元素和第一个元素前一个位置的随机访问迭代器，又称它们为反向迭代器。
  // 需要注意的是，在使用反向迭代器进行++或--运算时，++指的是迭代器向左移动一位，--指的是迭代器向右移动一位，即这两个运算符的功能也“互换”了。
  // auto first4 = values33.rbegin();
  // auto end4 = values33.rend();
  // while (first4 != end4) {
  //   cout << *first4 << " ";
  //   ++first4;
  // }
  for (auto first = values33.rbegin(); first != values33.rend(); ++first) {
    cout << *first << " ";
  }
  cout << endl;
  // 5 4 3 2 1

  // crbegin()/crend()组合和rbegin()/crend()组合唯一的区别在于，前者返回的迭代器为const类型，即不能用来修改容器中的元素，除此之外在使用上和后者完全相同。

  // vector容器迭代器的独特之处:
  // 和array容器不同，vector容器可以随着存储元素的增加，自行申请更多的存储空间。因此，在创建vector对象时，我们可以直接创建一个空的vector容器，并不会影响后续使用该容器。
  // 但这会产生一个问题，即在初始化空的vector容器时，不能使用迭代器。也就是说，如下初始化vector容器的方法是不行的：
  vector<int> v1;
  int val = 1;
  for (auto first = v1.begin(); first < v1.end(); ++first, val++) {
    *first = val;
    cout << *first;  // 初始化的同时输出值
  }
  // 没有打印，这是因为，对于空的vector容器来说，begin()和end()成员函数返回的迭代器是相等的，即它们指向的是同一个位置。
  // 所以，对于空的vector容器来说，可以通过调用push_back()或者借助resize()成员函数实现初始化容器的目的。

  // vector容器在申请更多内存的同时，容器中的所有元素可能会被复制或移动到新的内存地址，这会导致之前创建的迭代器失效。
  // 每当vector容器的容量发生变化时，我们都要对之前创建的迭代器重新初始化一遍：
  vector<int> v2{1, 2, 3};
  cout << "values 容器首个元素的地址：" << v2.data() << endl;
  auto first4 = v2.begin();
  auto end4 = v2.end();
  for (auto i = v2.begin(); i < v2.end(); ++i) {
    cout << *i;
  }
  cout << endl;  // 123
  // 增加values的容量
  v2.reserve(20);
  cout << "values 容器首个元素的地址：" << v2.data() << endl;
  while (first4 != end4) {
    cout << *first4;
    ++first4;
  }
  cout << endl;  // 003
  for (auto i = v2.begin(); i < v2.end(); ++i) {
    cout << *i;
  }
  cout << endl;  // 123

  // vector容器访问元素的几种方式:
  // 一、访问vector容器中单个元素
  // 1.vector容器可以向普通数组那样访问存储的元素，甚至对指定下标处的元素进行修改
  // vector的索引从0开始，这和普通数组一样。通过使用索引，总是可以访问到vector容器中现有的元素。
  vector<int> v3{1, 2, 3, 4, 5};
  cout << v3[0] << endl;  // 1
  v3[0] = v3[1] + v3[2] + v3[3] + v3[4];
  cout << v3[0] << endl;  // 14
  // 2.容器名[n]这种获取元素的方式，需要确保下标n的值不会超过容器的容量（可以通过capacity()成员函数获取），否则会发生越界访问的错误。
  // 幸运的是，和array容器一样，vector容器也提供了at()成员函数，当传给at()的索引会造成越界时，会抛出std::out_of_range异常
  vector<int> v4{1, 2, 3, 4, 5};
  cout << v4.at(0) << endl;  // 1
  v4.at(0) = v4.at(1) + v4.at(2) + v4.at(3) + v4.at(4);
  cout << v4.at(0) << endl;  // 4
  // cout << v4.at(5) << endl;  // out_of_range
  // 为什么vector容器在重载[]运算符时，没有实现边界检查的功能呢？答案很简单，因为性能。如果每次访问元素，都去检查索引值，无疑会产生很多开销。
  // 3.vector容器还提供了2个成员函数，即front()和back()，它们分别返回vector容器中第一个和最后一个元素的引用，通过利用这2个函数返回的引用，可以访问（甚至修改）容器中的首尾元素。
  vector<int> values66{1, 2, 3, 4, 5};
  cout << "values 首元素为：" << values66.front() << endl;  // 1
  cout << "values 尾元素为：" << values66.back() << endl;   // 5
  values66.front() = 10;
  cout << "values 新的首元素为：" << values66.front() << endl;  // 10
  values66.back() = 20;
  cout << "values 新的尾元素为：" << values66.back() << endl;  // 20
  // 4.vector容器还提供了data()成员函数，该函数的功能是返回指向容器中首个元素的指针。通过该指针也可以访问甚至修改容器中的元素
  vector<int> values77{1, 2, 3, 4, 5};
  cout << *(values77.data() + 2) << endl;  // 3
  *(values77.data() + 1) = 10;
  cout << *(values77.data() + 1) << endl;  // 10

  // 二.访问vector容器中多个元素
  // 1.借助size()成员函数，该函数可以返回vector容器中实际存储的元素个数
  vector<int> values88{1, 2, 3, 4, 5};
  //从下标0一直遍历到size()-1处
  for (int i = 0; i < values88.size(); i++) {
    cout << values88[i] << " ";
  }
  cout << endl;  // 1 2 3 4 5
  // 2.使用基于范围的循环，此方式将会逐个遍历容器中的元素
  for (auto&& value : values88) {
    cout << value << " ";
  }
  cout << endl;  // 1 2 3 4 5
  // 3.还可以使用vector迭代器遍历vector容器
  for (auto first = values88.begin(); first < values88.end(); ++first) {
    cout << *first << " ";
  }
  cout << endl;  // 1 2 3 4 5
                 // 也可以使用 rbegin()/rend()、cbegin()/cend()、crbegin()/crend()以及全局函数begin()/end()，它们都可以实现对容器中元素的访问。

  // vector容器的容量（用capacity表示），指的是在不分配更多内存的情况下，容器可以保存的最多元素个数；而vector容器的大小（用size表示），指的是它实际所包含的元素个数。
  std::vector<int> value99{2,  3,  5,  7,  11, 13, 17, 19,
                           23, 29, 31, 37, 41, 43, 47};
  value99.reserve(20);
  cout << "value 容量是：" << value99.capacity() << endl;  // 20
  cout << "value 大小是：" << value99.size() << endl;      // 15
  // vector容器的大小不能超出它的容量，在大小等于容量的基础上，只要增加一个元素，就必须分配更多的内存。注意，这里的“更多”并不是1个。
  // 换句话说，当vector容器的大小和容量相等时，如果再向其添加（或者插入）一个元素，vector往往会申请多个存储空间，而不仅仅只申请1个。
  // 一旦vector容器的内存被重新分配，则和vector容器中元素相关的所有引用、指针以及迭代器，都可能会失效，最稳妥的方法就是重新生成。
  vector<int> value91{2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37, 41, 43, 47};
  cout << "value 容量是：" << value91.capacity() << endl;  // 15
  cout << "value 大小是：" << value91.size() << endl;      // 15
  printf("value首地址：%p\n", value91.data());  // value首地址：0x1cc7500
  value91.push_back(53);
  cout << "value 容量是(2)：" << value91.capacity() << endl;  // 30
  cout << "value 大小是(2)：" << value91.size() << endl;      // 16
  printf("value首地址： %p\n", value91.data());               // 0x1cc75b0
  // 向“已满”的vector容器再添加一个元素，整个value容器的存储位置发生了改变，同时vector会一次性申请多个存储空间（具体多少，取决于底层算法的实现）。
  // 这样做的好处是，可以很大程度上减少vector申请空间的次数，当后续再添加元素时，就可以节省申请空间耗费的时间。
  // 因此，对于vector容器而言，当增加新的元素时，有可能很快完成（即直接存在预留空间中）；也有可能会慢一些（扩容之后再放新元素）。
  // 可以调用reserve()成员函数来增加容器的容量（但并不会改变存储元素的个数）；而通过调用成员函数resize()可以改变容器的大小，并且该函数也可能会导致vector容器容量的增加。

  vector<int> value92{2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37, 41, 43, 47};
  cout << "value 容量是：" << value92.capacity() << endl;  // 15
  cout << "value 大小是：" << value92.size() << endl;      // 15
  value92.reserve(20);
  cout << "value 容量是(2)：" << value92.capacity() << endl;  // 20
  cout << "value 大小是(2)：" << value92.size() << endl;      // 15
  value92.resize(21);  // 将元素个数改变为21个，所以会增加6个默认初始化的元素
  cout << "value 容量是(3)：" << value92.capacity() << endl;  // 30
  cout << "value 大小是(3)：" << value92.size() << endl;      // 21
  for (auto i = value92.begin(); i < value92.end(); ++i) {
    cout << *i << ' ';
  }  // 2 3 5 7 11 13 17 19 23 29 31 37 41 43 47 0 0 0 0 0 0
  cout << endl;
  value92.resize(24, 99);  // 将元素个数改变为21个，新增加的6个元素默认值为99。
  cout << "value 容量是(3)：" << value92.capacity() << endl;  // 30
  cout << "value 大小是(3)：" << value92.size() << endl;      // 24
  for (auto i = value92.begin(); i < value92.end(); ++i) {
    cout << *i << ' ';
  }  // 2 3 5 7 11 13 17 19 23 29 31 37 41 43 47 0 0 0 0 0 0 99 99 99
  cout << endl;
  value92.resize(20);  // 当需要减小容器的大小时，会移除多余的元素。
  cout << "value 容量是(3)：" << value92.capacity() << endl;  // 30
  cout << "value 大小是(3)：" << value92.size() << endl;      // 20
  for (auto i = value92.begin(); i < value92.end(); ++i) {
    cout << *i << ' ';
  }  // 2 3 5 7 11 13 17 19 23 29 31 37 41 43 47 0 0 0 0 0
  cout << endl;
  // 仅通过reserve()成员函数增加value容器的容量，其大小并没有改变；
  // 但通过resize()成员函数改变value容器的大小，它的容量可能会发生改变。
  // 另外需要注意的是，通过resize()成员函数减少容器的大小（多余的元素会直接被删除），不会影响容器的容量。
  // vector<T>对象的容量和大小类型都是vector<T>::size_type类型。可以使用auto关键字代替vector<int>::size_type
  vector<int>::size_type cap = value92.capacity();
  vector<int>::size_type size = value92.size();
  // size_type类型是定义在由vector类模板生成的vecotr类中的，它和操作系统有关，在32位架构下普遍表示的是unsigned int类型，而在64位架构下普通表示unsigned long类型。

  // vector容器的底层实现机制:
  // STL众多容器中，vector是最常用的容器之一，其底层所采用的数据结构非常简单，就只是一段连续的线性内存空间。
  // 就是使用3个迭代器（可以理解成指针）来表示的：
  // template <class _Ty, class _Alloc = allocator<_Ty>>  // _Alloc表示内存分配器，此参数几乎不需要我们关心
  // class vector{
  //     ...
  // protected:
  //     pointer _Myfirst;
  //     pointer _Mylast;
  //     pointer _Myend;
  // };
  // 其中，_Myfirst指向的是vector容器对象的起始字节位置；_Mylast指向当前最后一个元素的末尾字节；_myend指向整个vector容器所占用内存空间的末尾字节。
  // 在此基础上，将3个迭代器两两结合，还可以表达不同的含义，例如：
  //  _Myfirst和_Mylast可以用来表示vector容器中目前已被使用的内存空间；
  //  _Mylast和_Myend可以用来表示vector容器目前空闲的内存空间；
  //  _Myfirst和_Myend可以用表示vector容器的容量。
  // 对于空的vector容器，由于没有任何元素的空间分配，因此_Myfirst、_Mylast和_Myend均为null。
  // 通过灵活运用这3个迭代器，vector容器可以轻松的实现诸如首尾标识、大小、容器、空容器判断等几乎所有的功能
  // template <class _Ty, class _Alloc = allocator<_Ty>>
  // class vector{
  // public：
  //     iterator begin() {return _Myfirst;}
  //     iterator end() {return _Mylast;}
  //     size_type size() const {return size_type(end() - begin());}
  //     size_type capacity() const {return size_type(_Myend - begin());}
  //     bool empty() const {return begin() == end();}
  //     reference operator[] (size_type n) {return *(begin() + n);}
  //     reference front() { return *begin();}
  //     reference back() {return *(end()-1);}
  //     ...
  // };
  // vector扩大容量的本质:
  // 当vector的大小和容量相等（size==capacity）也就是满载时，如果再向其添加元素，那么vector就需要扩容。vector容器扩容的过程需要经历以下3步：
  //  1 完全弃用现有的内存空间，重新申请更大的内存空间；
  //  2 将旧内存空间中的数据，按原有顺序移动到新的内存空间中；
  //  3 最后将旧的内存空间释放。
  // 这也就解释了，为什么vector容器在进行扩容后，与其相关的指针、引用以及迭代器可能会失效的原因。
  // 由此可见，vector扩容是非常耗时的。为了降低再次分配内存空间时的成本，每次扩容时vector都会申请比用户需求量更多的内存空间（这也就是vector容量的由来，
  // 即capacity>=size），以便后期使用。
  // vector容器扩容时，不同的编译器申请更多内存空间的量是不同的。以VS为例，它会扩容现有容器容量的50%。

  return 0;
}