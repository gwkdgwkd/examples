#include <algorithm>
#include <array>
#include <iostream>
#include <vector>

using namespace std;

// vector容器是STL中最常用的容器之一，它和array容器非常类似，都可以看做是对C++普通数组的“升级版”。
// 不同之处在于，array实现的是静态数组（容量固定的数组），而vector实现的是一个动态数组，
// 即可以进行元素的插入和删除，在此过程中，vector会动态调整所占用的内存空间，整个过程无需人工干预。
// vector常被称为向量容器，因为该容器擅长在尾部插入或删除元素，在常量时间内就可以完成，时间复杂度为O(1)；
// 而对于在容器头部或者中部插入或删除元素，则花费时间要长一些（移动元素需要耗费时间），时间复杂度为线性阶O(n)。
// vector容器以类模板vector<T>（T 表示存储元素的类型）的形式定义在<vector>头文件中，并位于std命名空间中

// 相比array容器，vector提供了更多了成员函数:
// begin() 	        返回指向容器中第一个元素的迭代器。
// end() 	          返回指向容器最后一个元素所在位置后一个位置的迭代器，通常begin()结合使用。
// rbegin() 	      返回指向最后一个元素的迭代器。
// rend() 	        返回指向第一个元素所在位置前一个位置的迭代器。
// cbegin() 	      和begin()功能相同，只不过在其基础上，增加了const属性，不能用于修改元素。
// cend() 	        和end()功能相同，只不过在其基础上，增加了const属性，不能用于修改元素。
// crbegin() 	      和rbegin()功能相同，只不过在其基础上，增加了const属性，不能用于修改元素。
// crend() 	        和rend()功能相同，只不过在其基础上，增加了const属性，不能用于修改元素。
// size() 	        返回实际元素个数。
// max_size()       返回元素个数的最大值。这通常是一个很大的值，一般是232-1，所以我们很少会用到这个函数。
// resize() 	      改变实际元素的个数。
// capacity()       返回当前容量。
// empty() 	        判断容器中是否有元素，若无元素，则返回true；反之，返回false。
// reserve() 	      增加容器的容量。
// shrink_to_fit() 将内存减少到等于当前元素实际所使用的大小。
// operator[]       重载了[]运算符，可以向访问数组中元素那样，通过下标即可访问甚至修改vector容器中的元素。
// at() 	          使用经过边界检查的索引访问元素。
// front() 	        返回第一个元素的引用。
// back() 	        返回最后一个元素的引用。
// data() 	        返回指向容器中第一个元素的指针。
// assign() 	      用新元素替换原有内容。
// push_back()      在序列的尾部添加一个元素。
// pop_back()       移出序列尾部的元素。
// insert() 	      在指定的位置插入一个或多个元素。
// erase() 	        移出一个元素或一段元素。
// clear() 	        移出所有的元素，容器大小变为0。
// swap() 	        交换两个容器的所有元素。
// emplace() 	      在指定的位置直接生成一个元素。
// emplace_back()   在序列尾部生成一个元素。
// 除此之外，C++11标准库还新增加了begin()和end()这2个函数
// vector容器还有一个std::swap(x,y)非成员函数（其中x和y是存储相同类型元素的vector容器），
// 它和swap()成员函数的功能完全相同，仅使用语法上有差异。

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
// 其中，_Myfirst指向的是vector容器对象的起始字节位置；
// _Mylast指向当前最后一个元素的末尾字节；
// _myend指向整个vector容器所占用内存空间的末尾字节。
// 在此基础上，将3个迭代器两两结合，还可以表达不同的含义，例如：
//  _Myfirst和_Mylast可以用来表示vector容器中目前已被使用的内存空间；
//  _Mylast和_Myend可以用来表示vector容器目前空闲的内存空间；
//  _Myfirst和_Myend可以用表示vector容器的容量。
// 对于空的vector容器，由于没有任何元素的空间分配，因此_Myfirst、_Mylast和_Myend均为null。
// 通过灵活运用这3个迭代器，vector容器可以轻松的实现诸如首尾标识、大小、容器、空容器判断等几乎所有的功能：
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
// 当vector的大小和容量相等（size==capacity）也就是满载时，如果再向其添加元素，那么vector就需要扩容。
// vector容器扩容的过程需要经历以下3步：
// 1.完全弃用现有的内存空间，重新申请更大的内存空间；
// 2.将旧内存空间中的数据，按原有顺序移动到新的内存空间中；
// 3.最后将旧的内存空间释放。
// 这也就解释了，为什么vector容器在进行扩容后，与其相关的指针、引用以及迭代器可能会失效的原因。
// 由此可见，vector扩容是非常耗时的。
// 为了降低再次分配内存空间时的成本，每次扩容时vector都会申请比用户需求量更多的内存空间，以便后期使用。
// vector容器扩容时，不同的编译器申请更多内存空间的量是不同的。以VS为例，它会扩容现有容器容量的50%。

void func1() {  // 创建vector容器的几种方式
  // 1.空的vector容器：
  std::vector<double> v1;  // 因为容器中没有元素，所以没有为其分配空间
  std::cout << v1.size() << "/" << v1.capacity() << std::endl;  // 0/0
  v1.reserve(20);  // 在创建好空容器上，通过调用reserve()来增加容器的容量
  std::cout << v1.size() << "/" << v1.capacity() << std::endl;  // 0/20
  v1.emplace_back(6);  // 当添加第一个元素时，vector会自动分配内存
  std::cout << v1.size() << "/" << v1.capacity() << std::endl;  // 1/20

  // 2.在创建的同时指定初始值以及元素个数, 创建了一个含有8个素数的vector容器：
  std::vector<int> v2{2, 3, 5, 7, 11, 13, 17, 19};
  std::cout << v2.size() << "/" << v2.capacity() << std::endl;  // 8/8

  // 3.在创建vector容器时，也可以指定元素个数，values容器开始时就有2个元素，它们的默认初始值都为0：
  std::vector<double> v3(2);
  std::cout << v3.size() << "/" << v3.capacity() << std::endl;  // 2/2
  std::cout << v3[0] << "," << v3[1] << std::endl;              // 0,0

  // 圆括号()和大括号{}是有区别的，()表示元素的个数，而{}则表示vector容器中只有一个元素：
  std::vector<double> v4{2};
  std::cout << v4.size() << "/" << v4.capacity() << std::endl;  // 1/1
  std::cout << v4[0] << std::endl;                              // 2
  // 如果不想用0作为默认值，也可以指定一个其它值：
  std::vector<char> v5(5, 'c');
  std::cout << v5.size() << "/" << v5.capacity() << std::endl;  // 5/5
  // 圆括号()中的2个参数，既可以是常量，也可以用变量来表示：
  int num = 20;
  double value = 1.0;
  std::vector<double> v6(num, value);
  std::cout << v6.size() << "/" << v6.capacity() << std::endl;  // 20/20

  // 4.通过存储元素类型相同的其它vector容器，也可以创建新的vector容器：
  std::vector<char> v7(v5);
  std::cout << v7.size() << "/" << v7.capacity() << std::endl;  // 5/5
  std::vector<char> v8 = v5;
  std::cout << v8.size() << "/" << v8.capacity() << std::endl;  // 5/5
  // 如果不想复制其它容器中所有的元素，可以用一对指针或者迭代器来指定初始值的范围：
  int array[] = {1, 2, 3};
  std::vector<int> v9(array, array + 2);
  std::vector<int> v10{1, 2, 3, 4, 5};
  std::vector<int> v11(std::begin(v10), std::begin(v10) + 3);
  std::cout << v9.size() << "/" << v9.capacity() << std::endl;    // 2/2
  std::cout << v10.size() << "/" << v10.capacity() << std::endl;  // 5/5
  std::cout << v11.size() << "/" << v11.capacity() << std::endl;  // 3/3
}

void func2() {  // vector容器访问元素
  std::vector<int> v{1, 2, 3, 4, 5};

  // 1.vector容器可以向普通数组那样访问存储的元素，甚至对指定下标处的元素进行修改：
  std::cout << v[4]++ << "," << v[4] << std::endl;  // 5,6
  // v[40] = 8;  // 超过下标范围，会发生越界访问的错误
  // 为什么vector容器在重载[]运算符时，没有实现边界检查的功能呢？
  // 答案很简单，因为性能。
  // 如果每次访问元素，都去检查索引值，无疑会产生很多开销。

  // 2.使用at()成员函数，当传给at()的索引会造成越界时，会抛出std::out_of_range异常：
  std::cout << v.at(4)++ << "," << v.at(4) << std::endl;  // 6,7
  try {
    std::cout << v.at(5) << std::endl;  // out_of_range
  } catch (std::out_of_range e) {
    std::cout << e.what() << std::endl;
    // vector::_M_range_check: __n (which is 5) >= this->size() (which is 5
  }

  // 3.vector容器还提供了2个成员函数，即front()和back()，它们分别返回vector容器中第一个和最后一个元素的引用，
  //  通过利用这2个函数返回的引用，可以访问（甚至修改）容器中的首尾元素：
  std::cout << v.front() << "," << v.back() << std::endl;  // 1,7
  v.front()++;
  v.back()--;
  std::cout << v.front() << "," << v.back() << std::endl;  // 2,6

  // 4.vector容器还提供了data()成员函数，该函数的功能是返回指向容器中首个元素的指针，
  //  通过该指针也可以访问甚至修改容器中的元素：
  std::cout << *(v.data() + 1) << std::endl;  // 2
  *(v.data() + 1) = 100;
  std::cout << *(v.data() + 1) << std::endl;  // 100
}

void func3() {  // 遍历vector元素
  std::vector<int> v{1, 2, 3, 4, 5};

  // 1.借助size()成员函数，该函数可以返回vector容器中实际存储的元素个数：
  for (int i = 0; i < v.size(); i++) {
    std::cout << v[i] << " ";
  }
  std::cout << std::endl;  // 2 100 3 4 6

  // 2.使用基于范围的循环，此方式将会逐个遍历容器中的元素：
  for (auto&& value : v) {
    cout << value << " ";
  }
  std::cout << std::endl;  // 2 100 3 4 6

  // 3.还可以使用vector迭代器遍历vector容器：
  for (auto first = v.begin(); first < v.end(); ++first) {
    cout << *first << " ";
  }
  std::cout << std::endl;  // 2 100 3 4 6
  for (auto first = v.rbegin(); first < v.rend(); ++first) {
    cout << *first << " ";
  }
  std::cout << std::endl;  // 6 4 3 100 2
  // 也可以使用cbegin()/cend()、crbegin()/crend()

  // 4.全局函数begin()/end()，它们都可以实现对容器中元素的访问：
  for (auto first = std::begin(v); first != std::end(v); ++first) {
    cout << *first << " ";
  }
  std::cout << std::endl;  // 2 100 3 4 6
}

void func4() {  // vector容器迭代器的独特之处
  // 和array容器不同，vector容器可以随着存储元素的增加，自行申请更多的存储空间。
  // 因此，在创建vector对象时，可以直接创建一个空的vector容器，并不会影响后续使用该容器。
  // 但这会产生一个问题，即在初始化空的vector容器时，不能使用迭代器。
  // 也就是说，如下初始化vector容器的方法是不行的：
  std::vector<int> v;
  for (auto first = v.begin(); first < v.end(); ++first) {
    *first = 5;
    std::cout << *first << " ";  // 初始化的同时输出值
  }
  std::cout << endl;
  // 没有打印，这是因为，对于空的vector容器来说，begin()和end()成员函数返回的迭代器是相等的，即它们指向的是同一个位置。
  // 所以，对于空的vector容器来说，可以通过调用push_back()或者借助resize()成员函数实现初始化容器的目的。
}

void func5() {  // 迭代器失效
  // 如果调用reserve()来增加容器容量，之前创建好的任何迭代器（例如开始迭代器和结束迭代器）都可能会失效，
  // 这是因为，为了增加容器的容量，vector<T>容器的元素可能已经被复制或移到了新的内存地址。
  // 所以后续再使用这些迭代器时，最好重新生成一下：
  std::vector<int> v{1, 2, 3};
  std::cout << "v addr[" << v.size() << "/" << v.capacity()
            << "] : " << v.data() << std::endl;
  auto first = v.begin();
  auto end = v.end();
  for (auto i = v.begin(); i < v.end(); ++i) {
    std::cout << *i << " ";
  }
  std::cout << std::endl;
  // v addr[3/3] : 0x55d86295ceb0
  // 1 2 3

  v.reserve(20);  // 增加values的容量，不会影响已存储的元素，也不会生成任何元素
  std::cout << "v addr[" << v.size() << "/" << v.capacity()
            << "] : " << v.data() << std::endl;
  while (first != end) {
    std::cout << *first << " ";
    ++first;
  }
  std::cout << endl;
  // v addr[3/20] : 0x55d86295d2e0
  // 0 0 1653911568

  for (auto i = v.begin(); i < v.end(); ++i) {
    std::cout << *i << " ";
  }
  std::cout << endl;  // 1 2 3

  // 调用reserve前，如果vector的容量已经大于或等于要设置的大小，那么这条语句什么也不做：
  v.reserve(10);
  std::cout << "v addr[" << v.size() << "/" << v.capacity()
            << "] : " << v.data() << std::endl;
  // v addr[3/20] : 0x55d86295d2e0
}

void func6() {  // reserve和resize
  // 仅通过reserve()成员函数增加value容器的容量，其大小并没有改变；
  // 但通过resize()成员函数改变value容器的大小，它的容量可能会发生改变。
  // 另外需要注意的是，通过resize()成员函数减少容器的大小（多余的元素会直接被删除），不会影响容器的容量。
  std::vector<int> v{2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37, 41, 43, 47};
  std::cout << v.size() << "/" << v.capacity() << " : " << v.data()
            << std::endl;
  v.reserve(20);
  // vector容器的大小不能超出它的容量，在大小等于容量的基础上，只要增加一个元素，就必须分配更多的内存：
  v.push_back(53);
  std::cout << v.size() << "/" << v.capacity() << " : " << v.data()
            << std::endl;
  // 15/15 : 0x55f0bd6b0eb0
  // 16/20 : 0x55f0bd6b1310

  // 同时vector会一次性申请多个存储空间（具体多少，取决于底层算法的实现）。
  // 这样做的好处是，可以很大程度上减少vector申请空间的次数，当后续再添加元素时，就可以节省申请空间耗费的时间。
  // 因此，对于vector容器而言，当增加新的元素时，有可能很快完成；也有可能会慢一些。
  // 可以调用reserve()成员函数来增加容器的容量（但并不会改变存储元素的个数）；

  // 通过调用成员函数resize()可以改变容器的大小，并且该函数也可能会导致vector容器容量的增加：
  v.resize(21);  // 将元素个数改变为21个，所以会增加5个默认初始化的元素
  std::cout << v.size() << "/" << v.capacity() << " : " << v.data()
            << std::endl;
  for (auto i = v.begin(); i < v.end(); ++i) {
    std::cout << *i << ' ';
  }
  std::cout << std::endl;
  // 21/32 : 0x55f0bd6b1370
  // 2 3 5 7 11 13 17 19 23 29 31 37 41 43 47 53 0 0 0 0 0

  v.resize(24, 99);  // 将元素个数改变为24个，新增加的3个元素默认值为99
  std::cout << v.size() << "/" << v.capacity() << " : " << v.data()
            << std::endl;
  for (auto i = v.begin(); i < v.end(); ++i) {
    std::cout << *i << ' ';
  }
  std::cout << std::endl;
  // 24/32 : 0x55f0bd6b1370
  // 2 3 5 7 11 13 17 19 23 29 31 37 41 43 47 53 0 0 0 0 0 99 99 99

  v.resize(20);  // 当需要减小容器的大小时，会移除多余的元素
  std::cout << v.size() << "/" << v.capacity() << " : " << v.data()
            << std::endl;
  for (auto i = v.begin(); i < v.end(); ++i) {
    std::cout << *i << ' ';
  }
  std::cout << std::endl;
  // 20/32 : 0x55f0bd6b1370
  // 2 3 5 7 11 13 17 19 23 29 31 37 41 43 47 53 0 0 0 0

  // vector<T>对象的容量和大小类型都是vector<T>::size_type类型。
  // size_type类型是定义在由vector类模板生成的vecotr类中的，它和操作系统有关，
  // 在32位架构下普遍表示的是unsigned int类型，而在64位架构下普通表示unsigned long类型。
  vector<int>::size_type cap = v.capacity();
  vector<int>::size_type size = v.size();
}

class testDemo {
 public:
  testDemo(int num) : num(num) { std::cout << "调用构造函数" << std::endl; }
  testDemo(const testDemo& other) : num(other.num) {
    std::cout << "调用拷贝构造函数" << std::endl;
  }
  testDemo(testDemo&& other) : num(other.num) {
    std::cout << "调用移动构造函数" << std::endl;
  }
  testDemo& operator=(const testDemo& other);

 private:
  int num;
};
testDemo& testDemo::operator=(const testDemo& other) {
  this->num = other.num;
  return *this;
}
void func7() {
  // 向vector容器中添加元素的唯一方式就是使用它的成员函数，如果不调用成员函数，非成员函数既不能添加也不能删除元素。
  // 这意味着，vector容器对象必须通过它所允许的函数去访问，迭代器显然不行。
  // 可以用来给容器中添加元素的函数有2个，分别是push_back()和emplace_back()函数。
  // emplace_back()函数是C++ 11新增加的，其功能和push_back()相同，都是在vector容器的尾部添加一个元素。
  // insert()和emplace()成员函数，严格意义上讲，这2个成员函数的功能是向容器中的指定位置插入元素
  vector<int> values45{};
  values45.push_back(1);
  values45.push_back(2);
  values45.emplace_back(3);
  values45.emplace_back(4);
  for (int i = 0; i < values45.size(); i++) {
    std::cout << values45[i] << " ";
  }
  std::cout << std::endl;  // 1 2 3 4
  // emplace_back()和push_back()的区别，就在于底层实现的机制不同。push_back()向容器尾部添加元素时，首先会创建这个元素，然后再将这个元素拷贝或者移动到容器中（如果是
  // 拷贝的话，事后会自行销毁先前创建的这个元素）；而emplace_back()在实现时，则是直接在容器尾部创建这个元素，省去了拷贝或移动元素的过程。
  std::cout << "emplace_back:" << std::endl;
  std::vector<testDemo> demo1;
  demo1.emplace_back(2);
  std::cout << "push_back:" << std::endl;
  std::vector<testDemo> demo2;
  demo2.push_back(2);
  // emplace_back:
  // 调用构造函数
  // push_back:
  // 调用构造函数
  // 调用移动构造函数
  // 如果移动构造函数被删除，那么调用拷贝构造函数
  // push_back()在底层实现时，会优先选择调用移动构造函数，如果没有才会调用拷贝构造函数。
  // 显然完成同样的操作，push_back()的底层实现过程比emplace_back()更繁琐，换句话说，emplace_back()的执行效率比push_back()高。
  // 因此，在实际使用时，建议优先选用emplace_back()。要兼顾之前的版本，还是应该使用push_back()。

  // vector容器提供了insert()和emplace()这2个成员函数，用来实现在容器指定位置处插入元素
  // insert()函数的功能是在vector容器的指定位置插入一个或多个元素。该函数的语法格式有多种:
  // iterator insert(pos,elem) 在迭代器pos指定的位置之前插入一个新元素elem，并返回表示新插入元素位置的迭代器。
  // iterator insert(pos,n,elem) 在迭代器pos指定的位置之前插入n个元素elem，并返回表示第一个新插入元素位置的迭代器。
  // iterator insert(pos,first,last) 在迭代器pos指定的位置之前，插入其他容器（不仅限于vector）中位于[first,last)区域的所有元素，并返回表示第一个新插入元素位置的迭代器。
  // iterator insert(pos,initlist) 在迭代器pos指定的位置之前，插入初始化列表（用大括号{}括起来的多个元素，中间有逗号隔开）中所有的元素，并返回表示第一个新插入元素位置的迭代器。
  std::vector<int> demo{1, 2};
  demo.insert(demo.begin() + 1, 3);  // {1,3,2}
  demo.insert(demo.end(), 2, 5);     // {1,3,2,5,5}
  std::array<int, 3> test{7, 8, 9};
  demo.insert(demo.end(), test.begin(), test.end());  // {1,3,2,5,5,7,8,9}
  demo.insert(demo.end(), {10, 11});                  // {1,3,2,5,5,7,8,9,10,11}
  for (int i = 0; i < demo.size(); i++) {
    std::cout << demo[i] << " ";
  }
  std::cout << std::endl;  // 1 3 2 5 5 7 8 9 10 11
  // emplace()是C++11标准新增加的成员函数，用于在vector容器指定位置之前插入一个新的元素。emplace()每次只能插入一个元素，而不是多个。
  // iterator emplace (const_iterator pos, args...);
  // 其中，pos为指定插入位置的迭代器；args...表示与新插入元素的构造函数相对应的多个参数；该函数会返回表示新插入元素位置的迭代器。
  std::vector<int> demo3{1, 2};
  demo3.emplace(demo3.begin(), 3);
  for (int i = 0; i < demo3.size(); i++) {
    std::cout << demo3[i] << " ";
  }
  std::cout << std::endl;  // 3 1 2
  std::cout << "insert:" << std::endl;
  std::vector<testDemo> demo22{};
  demo22.insert(demo22.begin(), testDemo(1));
  std::cout << "emplace:" << std::endl;
  std::vector<testDemo> demo11{};
  demo11.emplace(demo11.begin(), 1);
  // insert:
  // 调用构造函数
  // 调用移动构造函数
  // emplace:
  // 调用构造函数
  // insert()需要调用类的构造函数和移动构造函数（或拷贝构造函数）；当拷贝构造函数和移动构造函数同时存在时，insert()会优先调用移动构造函数。
  // emplace()只需要调用构造函数即可。
  // emplace()在插入元素时，是在容器的指定位置直接构造元素，而不是先单独生成，再将其复制（或移动）到容器中。因此，在实际使用中，推荐优先使用emplace()。
}
void func8() {}
void func9() {  // 无论是向现有vector容器中访问元素、添加元素还是插入元素，都只能借助vector模板类提供的成员函数，但删除vector容器的元素例外，完成此操作除了可以借助本身提供的成员函数，
  // 还可以借助一些全局函数。
  // pop_back() 删除vector容器中最后一个元素，该容器的大小（size）会减1，但容量（capacity）不会发生改变。
  // erase(pos) 删除vector容器中pos迭代器指定位置处的元素，并返回指向被删除元素下一个位置元素的迭代器。该容器的大小（size）会减1，但容量（capacity）不会发生改变。
  // swap(beg)、pop_back() 先调用swap()函数交换要删除的目标元素和容器最后一个元素的位置，然后使用pop_back()删除该目标元素。
  // erase(beg,end) 删除vector容器中位于迭代器[beg,end)指定区域内的所有元素，并返回指向被删除区域下一个位置元素的迭代器。该容器size会减小，但capacity不会发生改变。
  // remove() 删除容器中所有和指定元素值相等的元素，并返回指向最后一个元素下一个位置的迭代器。值得一提的是，调用该函数不会改变容器的大小和容量。
  // clear() 删除vector容器中所有的元素，使其变成空的vector容器。该函数会改变vector的大小（变为0），但不是改变其容量。
  vector<int> d{1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
  for (int i = 0; i < d.size(); i++) {
    std::cout << d[i] << " ";
  }
  std::cout << "    size : " << d.size() << ",capacity : " << d.capacity()
            << std::endl;
  // 1 2 3 4 5 6 7 8 9 10     size : 10,capacity : 10
  d.pop_back();
  for (int i = 0; i < d.size(); i++) {
    std::cout << d[i] << " ";
  }
  std::cout << "    size : " << d.size() << ",capacity : " << d.capacity()
            << std::endl;
  // 1 2 3 4 5 6 7 8 9     size : 9,capacity : 10
  // 如果想删除vector容器中指定位置处的元素，可以使用erase()成员函数，该函数的语法格式为：iterator erase(pos);
  auto ii = d.erase(d.begin() + 1);
  std::cout << *ii << std::endl;  // 3
  for (int i = 0; i < d.size(); i++) {
    std::cout << d[i] << " ";
  }
  std::cout << "    size : " << d.size() << ",capacity : " << d.capacity()
            << std::endl;
  // 1 3 4 5 6 7 8 9     size : 8,capacity : 10
  // 如果不在意容器中元素的排列顺序，可以结合swap()和pop_back()函数，同样可以实现删除容器中指定位置元素的目的。
  // swap()函数在头文件<algorithm>和<utility>中都有定义，使用时引入其中一个即可。
  swap(*(begin(d) + 1), *(end(d) - 1));
  // swap(d[1],d[7])
  d.pop_back();
  for (int i = 0; i < d.size(); i++) {
    std::cout << d[i] << " ";
  }
  std::cout << "    size : " << d.size() << ",capacity : " << d.capacity()
            << std::endl;
  // 1 9 4 5 6 7 8     size : 7,capacity : 10
  // 删除容器中某个指定区域内的所有元素，同样可以使用erase()成员函数实现
  ii = d.erase(d.begin(), d.begin() + 3);
  for (int i = 0; i < d.size(); i++) {
    std::cout << d[i] << " ";
  }
  std::cout << "    size : " << d.size() << ",capacity : " << d.capacity()
            << std::endl;
  // 5 6 7 8     size : 4,capacity : 10
  // 如果要删除容器中和指定元素值相同的所有元素，可以使用remove()函数，该函数定义在<algorithm>头文件中
  d.insert(d.begin(), 3, 6);
  for (int i = 0; i < d.size(); i++) {
    std::cout << d[i] << " ";
  }
  std::cout << "    size : " << d.size() << ",capacity : " << d.capacity()
            << std::endl;
  // 6 6 6 5 6 7 8     size : 7,capacity : 10
  ii = remove(d.begin(), d.end(), 6);
  std::cout << *ii << std::endl;  // 5
  // remove()函数之后，由于该函数并没有改变容器原来的大小和容量，因此无法使用之前的方法遍历容器
  for (int i = 0; i < d.size(); i++) {
    std::cout << d[i] << " ";
  }
  std::cout << "    size : " << d.size() << ",capacity : " << d.capacity()
            << std::endl;
  // 5 7 8 5 6 7 8     size : 7,capacity : 10
  // 需要借助remove()返回的迭代器完成正确的遍历
  for (auto first = d.begin(); first < ii; ++first) {
    std::cout << *first << " ";
  }
  std::cout << std::endl;  // 5 7 8
  // remove()的实现原理是，在遍历容器中的元素时，一旦遇到目标元素，就做上标记，然后继续遍历，直到找到一个非目标元素，即用此元素将最先做标记的位置覆盖掉，同时将此非目标元素所
  // 在的位置也做上标记，等待找到新的非目标元素将其覆盖。
  // 既然通过remove()函数删除掉demo容器中的多个指定元素，该容器的大小和容量都没有改变，其剩余位置还保留了之前存储的元素。我们可以使用erase()成员函数删掉这些"无用"的元素。
  // remove()用于删除容器中指定元素时，常和erase()成员函数搭配使用。
  d.erase(ii, d.end());
  for (int i = 0; i < d.size(); i++) {
    std::cout << d[i] << " ";
  }
  std::cout << "    size : " << d.size() << ",capacity : " << d.capacity()
            << std::endl;
  // 5 7 8     size : 3,capacity : 10
  // 如果想删除容器中所有的元素，则可以使用clear()成员函数
  d.clear();
  for (int i = 0; i < d.size(); i++) {
    std::cout << d[i] << " ";
  }
  std::cout << "    size : " << d.size() << ",capacity : " << d.capacity()
            << std::endl;
  // size : 0,capacity : 10
}
void func10() {
  // 可以将vector容器看做是一个动态数组。换句话说，在不超出vector最大容量限制（max_size()成员方法的返回值）的前提下，该类型容器可以自行扩充容量来满足用户存储更多元素的需求。
  // vector容器扩容的整个过程，和realloc()函数的实现方法类似，大致分为以下4个步骤：
  //  1 分配一块大小是当前vector容量几倍的新存储空间。注意，多数STL版本中的vector容器，其容器都会以2的倍数增长，也就是说，每次vector容器扩容，它们的容量都会提高到之前的2倍；
  //  2 将vector容器存储的所有元素，依照原有次序从旧的存储空间复制到新的存储空间中；
  //  3 析构掉旧存储空间中存储的所有元素；
  //  4 释放旧的存储空间。
  // vector容器的扩容过程是非常耗时的，并且当容器进行扩容后，之前和该容器相关的所有指针、迭代器以及引用都会失效。因此在使用vector容器过程中，我们应尽量避免执行不必要的扩容操作。
  // vector模板类中功能类似的成员方法:
  // size() 获取vector容器中已经存有多少个元素，但仅通过此方法，无法得知vector容器有多少存储空间。
  // capacity() 获取vector容器总共可以容纳多少个元素。如果想知道当前vector容器有多少未被使用的存储空间，可以通过capacity()-size()得知。注意，如果size()和capacity()返回
  //            的值相同，则表明当前vector容器中没有可用存储空间了，这意味着，下一次向vector容器中添加新元素，将导致vector容器扩容。
  // resize(n) 强制vector容器必须存储n个元素，注意，如果n比size()的返回值小，则容器尾部多出的元素将会被析构（删除）；如果n比size()大，则vector会借助默认构造函数创建出更多的
  //           默认值元素，并将它们存储到容器末尾；如果n比capacity()的返回值还要大，则vector会先扩增，在添加一些默认值元素。
  // reserve(n) 强制vector容器的容量至少为n。注意，如果n比当前vector容器的容量小，则该方法什么也不会做；反之如果n比当前vector容器的容量大，则vector容器就会扩容。
  // 只要有新元素要添加到vector容器中而恰好此时vector容器的容量不足时，该容器就会自动扩容。
  // 因此，避免vector容器执行不必要的扩容操作的关键在于，在使用vector容器初期，就要将其容量设为足够大的值。换句话说，在vector容器刚刚构造出来的那一刻，就应该借助reserve()成员
  // 方法为其扩充足够大的容量。
  vector<int> myvector;
  for (int i = 1; i <= 1000; i++) {
    myvector.push_back(i);
  }
  // vector容器会进行2~10次自动扩容（多数的STL标准库版本中，vector容器通常会扩容至当前容量的2倍，而这里1000≈2 10），程序的执行效率可想而知。
  vector<int> myvector1;
  myvector.reserve(1000);
  std::cout << myvector.capacity() << std::endl;
  for (int i = 1; i <= 1000; i++) {
    myvector.push_back(i);
  }
  // vector容器的容量仅扩充了1次，执行效率大大提高。
  // 当然在实际场景中，我们可能并不知道vector容器到底要存储多少个元素。这种情况下，可以先预留出足够大的空间，当所有元素都存储到vector容器中之后，再去除多余的容量。
  // 关于怎样去除vector容器多余的容量，可以借助该容器模板类提供的shrink_to_fit()成员方法，还可以使用swap()成员方法去除vector容器多余的容量，两种方法都可以。

  // vector swap()成员方法还可以这样用:
  // 在使用vector容器的过程中，其容器会根据需要自行扩增。比如，使用push_back()、insert()、emplace()等成员方法向vector容器中添加新元素时，如果当前容器已满（即
  // size()==capacity()），则它会自行扩容以满足添加新元素的需求。当然，还可以调用reserve()成员方法来手动提升当前vector容器的容量。
  // 除了可以添加元素外，vector模板类中还提供了pop_back()、erase()、clear()等成员方法，可以轻松实现删除容器中已存储的元素。但需要注意得是，借助这些成员方法只能
  // 删除指定的元素，容器的容量并不会因此而改变。
  vector<int> myvector2;
  std::cout << "1、当前 myvector 拥有 " << myvector2.size()
            << " 个元素，容量为 " << myvector2.capacity() << std::endl;
  for (int i = 1; i <= 10; i++) {
    myvector2.push_back(i);
  }
  std::cout << "2、当前 myvector 拥有 " << myvector2.size()
            << " 个元素，容量为 " << myvector2.capacity() << std::endl;
  myvector2.reserve(1000);
  std::cout << "3、当前 myvector 拥有 " << myvector2.size()
            << " 个元素，容量为 " << myvector2.capacity() << std::endl;
  myvector2.erase(myvector2.begin());
  std::cout << "4、当前 myvector 拥有 " << myvector2.size()
            << " 个元素，容量为 " << myvector2.capacity() << std::endl;
  myvector2.pop_back();
  std::cout << "5、当前 myvector 拥有 " << myvector2.size()
            << " 个元素，容量为 " << myvector2.capacity() << std::endl;
  myvector2.clear();
  std::cout << "6、当前 myvector 拥有 " << myvector2.size()
            << " 个元素，容量为 " << myvector2.capacity() << std::endl;
  // 1、当前 myvector 拥有 0 个元素，容量为 0
  // 2、当前 myvector 拥有 10 个元素，容量为 16
  // 3、当前 myvector 拥有 10 个元素，容量为 1000
  // 4、当前 myvector 拥有 9 个元素，容量为 1000
  // 5、当前 myvector 拥有 8 个元素，容量为 1000
  // 6、当前 myvector 拥有 0 个元素，容量为 1000
  // myvector模板类中提供有一个shrink_to_fit()成员方法，该方法的功能是将当前vector容器的容量缩减至和实际存储元素的个数相等。
  myvector2.shrink_to_fit();
  std::cout << "7、当前 myvector 拥有 " << myvector2.size()
            << " 个元素，容量为 " << myvector2.capacity() << std::endl;
  // 7、当前 myvector 拥有 0 个元素，容量为 0
  // vector模板类中还提供有swap()成员方法，该方法的基础功能是交换2个相同类型的vector容器（交换容量和存储的所有元素），但其也能用于去除vector容器多余的容量。
  // vector<T>(x).swap(x);  // 其中，x指当前要操作的容器，T为该容器存储元素的类型。
  vector<int> myvector3;
  myvector3.reserve(1000);
  std::cout << "1、当前 myvector 拥有 " << myvector3.size()
            << " 个元素，容量为 " << myvector3.capacity() << std::endl;
  for (int i = 1; i <= 10; i++) {
    myvector3.push_back(i);
  }
  vector<int>(myvector3).swap(myvector3);
  std::cout << "2、当前 myvector 拥有 " << myvector3.size()
            << " 个元素，容量为 " << myvector3.capacity() << std::endl;
  // 1、当前 myvector 拥有 0 个元素，容量为 1000
  // 2、当前 myvector 拥有 10 个元素，容量为 10
  // 将 myvector 容器的容量 1000 修改为 10，此行代码的执行流程可细分为以下 3 步：
  // 1 先执行vector<int>(myvector)，此表达式会调用vector模板类中的拷贝构造函数，从而创建出一个临时的vector容器（后续称其为tempvector）。
  // 值得一提的是，tempvector临时容器并不为空，因为将myvector作为参数传递给了复制构造函数，会将myvector容器中的所有元素拷贝一份，并存储到tempvector临时容器中。
  // 注意，vector模板类中的拷贝构造函数只会为拷贝的元素分配存储空间。换句话说，tempvector临时容器中没有空闲的存储空间，其容量等于存储元素的个数。
  // 2 然后借助swap()成员方法对tempvector临时容器和myvector容器进行调换，此过程不仅会交换2个容器存储的元素，还会交换它们的容量。换句话说经过swap()操作，
  // myvetor容器具有了tempvector临时容器存储的所有元素和容量，同时tempvector也具有了原myvector容器存储的所有元素和容量。
  // 3 当整条语句执行结束时，临时的tempvector容器会被销毁，其占据的存储空间都会被释放。注意，这里释放的其实是原myvector容器占用的存储空间。
  // swap()成员方法用于清空vector容器:vector<T>().swap(x);
  vector<int>().swap(myvector3);
  std::cout << "3、当前 myvector 拥有 " << myvector3.size()
            << " 个元素，容量为 " << myvector3.capacity() << std::endl;
  // 3、当前 myvector 拥有 0 个元素，容量为 0
}
void func11() {
  // 在使用vector容器时，要尽量避免使用该容器存储bool类型的元素，即避免使用vector<bool>:
  //  1 严格意义上讲，vector<bool>并不是一个STL容器；
  //  2 vector<bool>底层存储的并不是bool类型值。
  // 如果vector<bool>是一个STL容器，则下面这段代码是可以通过编译的：
  // vector<bool> cont{0, 1};
  // bool* p = &cont[0];
  // vector<bool>底层采用了独特的存储机制。
  // 为了节省空间，vector<bool>底层在存储各个bool类型值时，每个bool值都只使用一个比特位（二进制位）来存储。也就是说在vector<bool>底层，一个字节可以存储8个bool类型值。
  // 在这种存储机制的影响下，operator[] 势必就需要返回一个指向单个比特位的引用，但显然这样的引用是不存在的。对于指针来说，其指向的最小单位是字节，也无法另其指向单个比特位。
  // 由于vector<bool>并不完全满足C++标准中对容器的要求，所以严格意义上来说它并不是一个STL容器。
  // 既然vector<bool>不完全是一个容器，为什么还会出现在C++标准中呢？
  // 这和一个雄心勃勃的试验有关，还要从前面提到的代理对象开始说起。由于代理对象在C++软件开发中很受欢迎，引起了C++标准委员会的注意，他们决定以开发vector<bool>作为一个样例，
  // 来演示STL中的容器如何通过代理对象来存取元素，这样当用户想自己实现一个基于代理对象的容器时，就会有一个现成的参考模板。
  // 然而开发人员在实现vector<bool>的过程中发现，既要创建一个基于代理对象的容器，同时还要求该容器满足C++标准中对容器的所有要求，是不可能的。由于种种原因，这个试验最终失败了，
  // 但是他们所做过的尝试（即开发失败的vector<bool>）遗留在了C++标准中。
  // 如果在实际场景中需要使用vector<bool>这样的存储结构，该怎么办呢？很简单，可以选择使用deque<bool>或者bitset来替代vector<bool>。
  // deque容器几乎具有vecotr容器全部的功能（拥有的成员方法也仅差reserve()和capacity()），而且更重要的是，deque容器可以正常存储bool类型元素。
  // 还可以考虑用bitset代替vector<bool>，其本质是一个模板类，可以看做是一种类似数组的存储结构。和后者一样，bitset只能用来存储bool类型值，且底层存储机制也采用的是用一个比
  // 特位来存储一个bool值。和vector容器不同的是，bitset的大小在一开始就确定了，因此不支持插入和删除元素；另外bitset不是容器，所以不支持使用迭代器。
}

int main() {
  // func1();
  // func2();
  // func3();
  // func4();
  // func5();
  func6();
  // func7();
  // func8();
  // func9();
  // func10();
  // func11();
}