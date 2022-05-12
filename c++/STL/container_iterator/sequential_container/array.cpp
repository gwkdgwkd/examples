#include <algorithm>
#include <array>
#include <cstring>
#include <iostream>
#include <iterator>
#include <vector>

// array容器是C++11标准中新增的序列容器，简单地理解，
// 它就是在C++普通数组的基础上，添加了一些成员函数和全局函数。
// 在使用上，它比普通数组更安全，且效率并没有因此变差。
// 和其它容器不同，array容器的大小是固定的，无法动态的扩展或收缩，这也就意味着，
// 在使用该容器的过程无法借由增加或移除元素而改变其大小，它只允许访问或者替换存储的元素。
// 在array<T,N>类模板中，T用于指明容器中的存储的具体数据类型，N用于指明容器的大小，
// 需要注意的是，这里的N必须是常量，不能用变量表示。
void func1() {            // array容器有多种初始化方式
  std::array<int, 5> ia;  // 各个值是不确定的,不会做默认初始化操作
  std::array<double, 4> da{};  // 所有的元素都会被初始化为0.0
  std::array<char, 5> ca{'a', 'b', 'c', 'd', '3'};  // 像创建常规数组一样
  std::array<float, 5> fa{2.1f, 3.4f};              // 只初始化前2个元素

  std::copy(ia.begin(), ia.end(), std::ostream_iterator<int>{std::cout, " "});
  std::cout << std::endl;  // -1603350191 21866 -1603350520 21866 -296823800
  std::copy(da.begin(), da.end(),
            std::ostream_iterator<double>{std::cout, " "});
  std::cout << std::endl;  // 0 0 0 0
  std::copy(ca.begin(), ca.end(), std::ostream_iterator<char>{std::cout, " "});
  std::cout << std::endl;  // a b c d 3
  std::copy(fa.begin(), fa.end(), std::ostream_iterator<float>{std::cout, " "});
  std::cout << std::endl;  // 2.1 3.4 0 0 0
}

// array容器还提供有很多功能实用的成员函数:
// begin() 	 返回指向容器中第一个元素的随机访问迭代器。
// end() 	   返回指向容器最后一个元素之后一个位置的随机访问迭代器，通常和begin()结合使用。
// rbegin()  返回指向最后一个元素的随机访问迭代器。
// rend() 	 返回指向第一个元素之前一个位置的随机访问迭代器。
// cbegin()  和begin()功能相同，只不过在其基础上增加了const属性，不能用于修改元素。
// cend() 	 和end()功能相同，只不过在其基础上，增加了const属性，不能用于修改元素。
// crbegin() 和rbegin()功能相同，只不过在其基础上，增加了const属性，不能用于修改元素。
// crend() 	 和rend()功能相同，只不过在其基础上，增加了const属性，不能用于修改元素。
// size() 	 返回容器中当前元素的数量，其值始终等于初始化array类的第二个模板参数N。
// max_size()返回容器可容纳元素的最大数量，其值始终等于初始化array类的第二个模板参数N。
// empty() 	 判断容器是否为空，和通过size()==0的判断条件功能相同，但其效率可能更快。
// at(n) 	   返回容器中n位置处元素的引用，该函数自动检查n是否在有效的范围内，如果不是则抛出out_of_range异常。
// front() 	 返回容器中第一个元素的直接引用，该函数不适用于空的array容器。
// back() 	 返回容器中最后一个元素的直接应用，该函数同样不适用于空的array容器。
// data() 	 返回一个指向容器首个元素的指针。利用该指针，可实现复制容器中所有元素等类似功能。
// fill(val) 将val这个值赋值给容器中的每个元素。
// array1.swap(array2) 交换array1和array2容器中的所有元素，但前提是它们具有相同的长度和类型。
void func2() {
  std::array<int, 5> a{1, 2, 3, 4, 5};
  std::copy(a.begin(), a.end(), std::ostream_iterator<int>(std::cout, " "));
  std::cout << std::endl;  // 1 2 3 4 5

  // 在使用反向迭代器进行++或--运算时，++指的是迭代器向左移动一位，
  // --指的是迭代器向右移动一位，即这两个运算符的功能也“互换”了。
  std::copy(a.rbegin(), a.rend(), std::ostream_iterator<int>(std::cout, " "));
  std::cout << std::endl;  // 5 4 3 2 1

  (*(a.begin()))++;
  (*(a.end() - 1))++;
  (*(a.rbegin()))++;
  (*(a.rend() - 1))++;
  std::cout << *(a.cbegin()) << std::endl;     // 3
  std::cout << *(a.cend() - 1) << std::endl;   // 7
  std::cout << *(a.crbegin()) << std::endl;    // 7
  std::cout << *(a.crend() - 1) << std::endl;  // 3

  std::cout << a.size() << std::endl;      // 5
  std::cout << a.max_size() << std::endl;  // 5

  // 访问单个元素方法一，通过容器名[]的方式直接访问和使用容器中的元素，
  // 这和C++标准数组访问元素的方式相同：
  for (int i = 0; i < a.size(); ++i) {
    a[i] *= 10;
  }
  // 访问单个元素方法二，为了能够有效地避免越界访问的情况，可以使用array容器提供的at()成员函数：
  for (int i = 0; i < a.size(); ++i) {
    std::cout << a.at(i) << " ";
  }
  std::cout << std::endl;  // 30 20 30 40 70
  // a[6] = 8;  // 崩溃
  try {
    a.at(6) = 8;
  } catch (std::out_of_range e) {
    std::cout << e.what() << std::endl;
    // array::at: __n (which is 6) >= _Nm (which is 5)
  }
  // 当传给at()的索引是一个越界值时，程序会抛出std::out_of_range异常。
  // 因此当需要访问容器中某个指定元素时，建议大家使用at()，除非确定索引没有越界。
  // 为什么array容器在重载[]运算符时，没有实现边界检查的功能呢？答案很简单，因为性能。
  // 如果每次访问元素，都去检查索引值，无疑会产生很多开销。
  // 当不存在越界访问的可能时，就能避免这种开销。

  a.front() += 5;
  a.back() += 6;
  // 访问单个元素方法三，通过调用data函数可以得到指向容器首个元素的指针，
  // 通过该指针，可以获得容器中的各个元素：
  int* data = a.data();
  *(data + 3) += 9;
  for (int i = 0; i < a.size(); ++i) {
    std::cout << *(data + i) << " ";
  }
  std::cout << std::endl;  // 35 20 30 49 76

  // 很少会创建空的array容器，因为当生成一个array容器时，它的元素个数就固定了，而且无法改变，
  // 所以生成空array容器的唯一方法是将模板的第二个参数指定为0，但这种情况基本不可能发生：
  std::array<int, 0> zero;
  // std::cout << zero.front() << std::endl;  // 崩溃
  // std::cout << zero.back() << std::endl;  // 崩溃

  // array容器之所以提供empty()，对于其他元素可变或者元素可删除的容器（vector、deque等）来说，
  // 它们使用empty()时的机制是一样的，因此为它们提供了一个一致性的操作：
  std::cout << std::boolalpha << a.empty() << std::endl;     // false
  std::cout << std::boolalpha << zero.empty() << std::endl;  // true

  a.fill(8);
  std::copy(a.begin(), a.end(), std::ostream_iterator<int>(std::cout, " "));
  std::cout << std::endl;  // 8 8 8 8 8

  std::array<int, 5> b{1, 2, 3};
  a.swap(b);
  std::copy(a.begin(), a.end(), std::ostream_iterator<int>(std::cout, " "));
  std::cout << std::endl;  // 1 2 3 0 0
}

// 除此之外，C++11标准库还新增加了begin()和end()这2个函数，
// 和array容器包含的begin()和end()成员函数不同的是，
// 标准库提供的这2个函数的操作对象，既可以是容器，还可以是普通数组。
// 当操作对象是容器时，它和容器包含的begin()和end()成员函数的功能完全相同；
// 如果操作对象是普通数组，则begin()函数返回的是指向数组第一个元素的指针，
// 同样end()返回指向数组中最后一个元素之后一个位置的指针（注意不是最后一个元素）。
// 另外，在<array>头文件中还重载了get()全局函数，
// 该重载函数的功能是访问容器中指定的元素，并返回该元素的引用。
// 正是由于array容器中包含了at()这样的成员函数，使得操作元素时比普通数组更安全。
void func3() {
  // 当迭代器指向容器中的一个特定元素时，它们不会保留任何关于容器本身的信息，
  // 所以无法从迭代器中判断，它是指向array容器还是指向vector容器。
  std::array<int, 5> a{1, 2, 3, 4, 5};
  for (auto i = std::begin(a); i != std::end(a); ++i) {
    std::cout << *i << " ";
  }
  std::cout << std::endl;  // 1 2 3 4 5

  int b[4] = {6, 7, 8, 9};
  for (auto i = std::begin(b); i != std::end(b); ++i) {
    std::cout << *i << " ";
  }
  std::cout << std::endl;  // 6 7 8 9

  std::vector<int> v = {10, 11, 12};
  for (auto i = std::begin(v); i != std::end(v); ++i) {
    std::cout << *i << " ";
  }
  std::cout << std::endl;  // 10 11 12

  // 访问单个元素方法四，array容器还提供了get<n>模板函数，
  // 它是一个辅助函数，能够获取到容器的第n个元素。
  // 需要注意的是，该模板函数中，参数的实参必须是一个在编译时可以确定的常量表达式，
  // 所以它不能是一个循环变量。
  // 也就是说，它只能访问模板参数指定的元素，编译器在编译时会对它进行检查。
  std::get<4>(a) = 8;
  std::cout << std::get<0>(a) << std::get<1>(a) << std::get<2>(a)
            << std::get<3>(a) << std::get<4>(a) << std::endl;  // 12348
  // std::get<6>(a);  // get是编译时获取的值，不用[]和at，超限时编译报错
}

// 在实际编程过程中，完全有理由去尝试，在原本使用普通数组的位置，改由array容器去实现。
// array模板类中已经封装好了大量实用的方法：
// 1.at()成员函数，可以有效防止越界操纵数组的情况；
// 2.fill()函数可以实现数组的快速初始化；
// 3.swap()函数可以轻松实现两个相同数组（类型相同，大小相同）中元素的互换。
// 总之，array容器就是普通数组的“升级版”，使用普通数组能实现的，使用array容器都可以实现，
// 而且无论是代码功能的实现效率，还是程序执行效率，都比普通数组更高。
void func4() {
  // 和C++普通数组存储数据的方式一样，
  // C++标准库保证使用array容器存储的所有元素一定会位于连续且相邻的内存中：
  std::array<int, 5> a{1, 2, 3};
  std::cout << &a[2] << " " << &a[0] + 2 << std::endl;
  // 0x7ffcd2e08dd8 0x7ffcd2e08dd8

  // 完全可以使用array容器去存储char*或const char*类型的字符串：
  // array容器的大小必须保证能够容纳复制进来的数据，而且如果是存储字符串的话，
  // 还要保证在存储整个字符串的同时，在其最后放置一个\0作为字符串的结束符。
  std::array<char, 50> c{1, 2, 3};
  strcpy(&c[0], "hello,world");  // &a[0]还可以用data()成员函数来替换
  std::cout << &c[0] << std::endl;  // hello,world

  strcpy(c.data(), "xxxx");
  std::cout << c.data() << std::endl;  // xxxx
  // 容器的迭代器和指针是不能混用的，
  // 即上面代码中不能用a.begin()来代替&a[0]或者a.data[]，这可能会引发错误：
  strcpy(c.begin(), "yyyy");            // 没报错......
  std::cout << c.begin() << std::endl;  // yyyy

  std::array<char, 50> addr1{"hello1"};
  std::array<char, 50> addr2{"hello"};
  addr1.swap(addr2);
  std::cout << "addr1:" << addr1.data() << std::endl;  // addr1:hello
  std::cout << "addr2:" << addr2.data() << std::endl;  // addr2:hello1

  // 当两个array容器满足大小相同并且保存元素的类型相同时，
  // 两个array容器可以直接直接做赋值操作，即将一个容器中的元素赋值给另一个容器：
  addr1 = addr2;
  std::cout << addr1.data() << std::endl;  // hello1

  // 在满足以上2个条件的基础上，如果其保存的元素也支持比较运算符，
  // 就可以用任何比较运算符直接比较两个array容器：
  if (addr1 == addr2) {
    std::cout << "addr1 == addr2" << std::endl;
  }
  if (addr1 < addr2) {
    std::cout << "addr1 < addr2" << std::endl;
  }
  if (addr1 > addr2) {
    std::cout << "addr1 > addr2" << std::endl;
  }
  // addr1 == addr2
  // 两个容器比较大小的原理，和两个字符串比较大小是一样的，即从头开始，
  // 逐个取两容器中的元素进行大小比较（根据ASCII码表），
  // 直到遇到两个不相同的元素，那个元素的值大，则该容器就大。
}

int main() {
  func1();
  func2();
  func3();
  func4();

  return 0;
}