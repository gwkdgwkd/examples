#include <array>
#include <cstring>
#include <iostream>

using namespace std;

// array容器是C++11标准中新增的序列容器，简单地理解，它就是在C++普通数组的基础上，添加了一些成员函数和全局函数。
// 在使用上，它比普通数组更安全，且效率并没有因此变差。
// 和其它容器不同，array容器的大小是固定的，无法动态的扩展或收缩，这也就意味着，在使用该容器的过程无法借由增加或移除元素而改变其大小，它只允许访问或者替换存储的元素。
// array容器以类模板的形式定义在<array>头文件，并位于命名空间std中
// namespace std {
// template <typename T, size_t N>
// class array;
// }
// 在array<T,N>类模板中，T用于指明容器中的存储的具体数据类型，N用于指明容器的大小，需要注意的是，这里的N必须是常量，不能用变量表示。

// array容器还提供有很多功能实用的成员函数:
// begin() 	返回指向容器中第一个元素的随机访问迭代器。
// end() 	返回指向容器最后一个元素之后一个位置的随机访问迭代器，通常和begin()结合使用。
// rbegin() 返回指向最后一个元素的随机访问迭代器。
// rend() 	返回指向第一个元素之前一个位置的随机访问迭代器。
// cbegin() 和begin()功能相同，只不过在其基础上增加了const属性，不能用于修改元素。
// cend() 	和end()功能相同，只不过在其基础上，增加了const属性，不能用于修改元素。
// crbegin() 和rbegin()功能相同，只不过在其基础上，增加了const属性，不能用于修改元素。
// crend() 	和rend()功能相同，只不过在其基础上，增加了const属性，不能用于修改元素。
// size() 	返回容器中当前元素的数量，其值始终等于初始化array类的第二个模板参数N。
// max_size() 返回容器可容纳元素的最大数量，其值始终等于初始化array类的第二个模板参数N。
// empty() 	判断容器是否为空，和通过 size()==0 的判断条件功能相同，但其效率可能更快。
// at(n) 	返回容器中 n 位置处元素的引用，该函数自动检查n是否在有效的范围内，如果不是则抛出out_of_range异常。
// front() 	返回容器中第一个元素的直接引用，该函数不适用于空的array容器。
// back() 	返回容器中最后一个元素的直接应用，该函数同样不适用于空的array容器。
// data() 	返回一个指向容器首个元素的指针。利用该指针，可实现复制容器中所有元素等类似功能。
// fill(val) 将val这个值赋值给容器中的每个元素。
// array1.swap(array2) 	交换array1和array2容器中的所有元素，但前提是它们具有相同的长度和类型。

// 除此之外，C++11标准库还新增加了begin()和end()这2个函数，和array容器包含的begin()和end()成员函数不同的是，标准库提供的这2个函数的操作对象，既可以是容器，
// 还可以是普通数组。当操作对象是容器时，它和容器包含的begin()和end()成员函数的功能完全相同；如果操作对象是普通数组，则begin()函数返回的是指向数组第一个元素的指
// 针，同样end()返回指向数组中最后一个元素之后一个位置的指针（注意不是最后一个元素）。
// 另外，在<array>头文件中还重载了get()全局函数，该重载函数的功能是访问容器中指定的元素，并返回该元素的引用。
// 正是由于array容器中包含了at()这样的成员函数，使得操作元素时比普通数组更安全。

// 当迭代器指向容器中的一个特定元素时，它们不会保留任何关于容器本身的信息，所以我们无法从迭代器中判断，它是指向array容器还是指向vector容器

int main() {
  // array容器有多种初始化方式:
  std::array<double, 10> values1;  // 各个值是不确定的,不会做默认初始化操作
  std::array<double, 10> values2{};  // 所有的元素都会被初始化为0.0
  // 像创建常规数组那样对元素进行初始化。这里只初始化了前4个元素，剩余的元素都会被初始化为0.0
  std::array<double, 10> values3{0.5, 1.0, 1.5, 2.0};

  std::array<int, 4> values4{};
  // 初始化values容器为{0,1,2,3}
  for (int i = 0; i < values4.size(); i++) {
    values4.at(i) = i;
  }
  // 使用get()重载函数输出指定位置元素
  cout << get<3>(values4) << endl;
  // 如果容器不为空，则输出容器中所有的元素
  if (!values4.empty()) {
    for (auto val = values4.begin(); val < values4.end(); val++) {
      cout << *val << " ";
    }
  }
  cout << endl;
  // 3
  // 0 1 2 3

  // array容器模板类中的begin()和end()成员函数返回的都是正向迭代器，它们分别指向「首元素」和「尾元素+1」的位置。
  // 在实际使用时，我们可以利用它们实现初始化容器或者遍历容器中元素的操作。
  array<int, 5> values;
  int h = 1;
  auto first = values.begin();
  auto last = values.end();
  // 还可以使用全局的begin()和end()函数来从容器中获取迭代器，因为当操作对象为array容器时，它们和begin()/end()成员函数是通用的。
  // auto first = std::begin(values4);
  // auto last = std::end(values4);
  while (first != last) {
    *first = h;  // 初始化values容器为{1,2,3,4,5}
    ++first;
    h++;
  }
  first = values.begin();
  while (first != last) {
    cout << *first << " ";
    ++first;
  }
  cout << endl;
  // 1 2 3 4 5

  // array 模板类还提供了cbegin()和cend()成员函数，它们和begin()/end()唯一不同的是，前者返回的是const类型的正向迭代器，这就意味着，
  // 有cbegin()和cend()成员函数返回的迭代器，可以用来遍历容器内的元素，也可以访问元素，但是不能对所存储的元素进行修改。
  auto first1 = values.cbegin();
  auto last1 = values.cend();
  // 由于*first为const类型，不能用来修改元素
  // *first1 = 10;  // assignment of read-only location ‘* first1’
  // 遍历容器并输出容器中所有元素
  while (first1 != last1) {
    // 可以使用const类型迭代器访问元素
    cout << *first1 << " ";
    ++first1;
  }
  cout << endl;
  // 1 2 3 4 5

  // array模板类中还提供了rbegin()/rend()和crbegin()/crend()成员函数，每对都可以分别得到指向最一个元素和第一个元素前一个位置的随机访问迭代器，又称它们为反向迭代器
  // 在使用反向迭代器进行++或--运算时，++指的是迭代器向左移动一位，--指的是迭代器向右移动一位，即这两个运算符的功能也“互换”了。
  // 在反向迭代器上使用++递增运算符，会让迭代器用一种和普通正向迭代器移动方向相反的方式移动。
  h = 1;
  auto first2 = values.rbegin();
  auto last2 = values.rend();
  // 初始化values容器为{5,4,3,2,1}
  while (first != last) {
    *first2 = h;
    ++first2;
    h++;
  }
  // 重新遍历容器，并输入各个元素
  first2 = values.rbegin();
  // while (first2 != last2) {
  //   cout << *first2 << " ";
  //   ++first2;
  // }
  for (auto first2 = values.rbegin(); first2 != values.rend(); ++first2) {
    cout << *first2 << " ";
  }
  cout << endl;
  // 5 4 3 2 1

  // crbegin()/crend()组合和rbegin()/crend()组合的功能唯一的区别在于，前者返回的迭代器为const类型，即不能用来修改容器中的元素，除此之外在使用上和后者完全相同。

  // array容器访问元素的几种方式:
  // 一、访问array容器中单个元素:
  // 1.可以通过容器名[]的方式直接访问和使用容器中的元素，这和C++标准数组访问元素的方式相同
  values[4] = values[3] + 2 * values[1];
  // 2.为了能够有效地避免越界访问的情况，可以使用array容器提供的at()成员函数
  values.at(4) = values.at(3) + 2 * values.at(1);
  // 当传给at()的索引是一个越界值时，程序会抛出std::out_of_range异常。因此当需要访问容器中某个指定元素时，建议大家使用at()，除非确定索引没有越界。
  // 为什么array容器在重载[]运算符时，没有实现边界检查的功能呢？
  // 答案很简单，因为性能。如果每次访问元素，都去检查索引值，无疑会产生很多开销。当不存在越界访问的可能时，就能避免这种开销。
  // 3.array容器还提供了get<n>模板函数，它是一个辅助函数，能够获取到容器的第n个元素。需要注意的是，该模板函数中，参数的实参必须是一个在编译时可以确定的常量表达式，
  // 所以它不能是一个循环变量。也就是说，它只能访问模板参数指定的元素，编译器在编译时会对它进行检查。
  cout << get<4>(values) << endl;  // 8
  // cout << get<6>(values) << endl;  // error: static assertion failed: index is out of bounds
  // 4.array容器提供了data()成员函数，通过调用该函数可以得到指向容器首个元素的指针。通过该指针，我们可以获得容器中的各个元素
  cout << *(values.data() + 4) << endl;  // 8
  cout << *(values.data() + 6) << endl;  // 665313008 随机值

  // 二、访问array容器中多个元素
  // 1.array容器提供的size()函数能够返回容器中元素的个数（函数返回值为size_t类型）,size()函数的存在，为array容器提供了标准数组所没有的优势，即能够知道它包含多少元素。
  int total = 0;
  for (size_t i = 0; i < values.size(); ++i) {
    total += values[i];
  }
  cout << total << endl;  // 18
  // array容器的empty()成员函数，即可知道容器中有没有元素（如果容器中没有元素，此函数返回true）
  if (values.empty()) {
    std::cout << "The container has no elements.\n";
  } else {
    std::cout << "The container has " << values.size() << " elements.\n";
  }
  // 很少会创建空的array容器，因为当生成一个array容器时，它的元素个数就固定了，而且无法改变，所以生成空array容器的唯一方法是将模板的第二个参数指定为0，但这种情况基本不可能发生。
  // array容器之所以提供empty()，对于其他元素可变或者元素可删除的容器（例如vector、deque等）来说，它们使用empty()时的机制是一样的，因此为它们提供了一个一致性的操作。
  // 2.除了借助size()外，对于任何可以使用迭代器的容器，都可以使用基于范围的循环，因此能够更加简便地计算容器中所有元素的和
  total = 0;
  for (auto&& value : values) {
    total += value;
  }
  cout << total << endl;  // 18
  total = 0;
  for (auto i = values.begin(); i < values.end(); i++) {
    total += *i;
  }
  cout << total << endl;  // 18

  // 和C++普通数组存储数据的方式一样，C++标准库保证使用array容器存储的所有元素一定会位于连续且相邻的内存中
  array<int, 5> a{1, 2, 3};
  cout << &a[2] << " " << &a[0] + 2 << endl;  // 0x7ffc931fc898 0x7ffc931fc898
  // 在实际编程过程中，完全有理由去尝试，在原本使用普通数组的位置，改由array容器去实现。
  // 用array容器替换普通数组的好处是，array模板类中已经封装好了大量实用的方法，在提高开发效率的同时，代码的运行效率也会大幅提高。

  // 完全可以使用array容器去存储char*或const char*类型的字符串：
  // array容器的大小必须保证能够容纳复制进来的数据，而且如果是存储字符串的话，还要保证在存储整个字符串的同时，在其最后放置一个\0作为字符串的结束符。
  array<char, 50> aa{1, 2, 3};
  strcpy(&aa[0], "http://c.biancheng.net/stl\n");
  printf("%s", &aa[0]);  // http://c.biancheng.net/stl
  // &a[0]还可以用array模板类提供的data()成员函数来替换
  strcpy(aa.data(), "xxxx\n");
  printf("%s", aa.data());  // xxxx
  // 容器的迭代器和指针是不能混用的，即上面代码中不能用a.begin()来代替&a[0] 或者a.data[]，这可能会引发错误。
  strcpy(aa.begin(), "yyyy\n");  // 没报错......
  printf("%s", aa.begin());      // yyyy

  // 使用array容器代替普通数组，最直接的好处就是array模板类中已经为我们写好了很多实用的方法，可以大大提高我们编码效率
  // array容器提供的at()成员函数，可以有效防止越界操纵数组的情况；
  // fill()函数可以实现数组的快速初始化；
  // swap()函数可以轻松实现两个相同数组（类型相同，大小相同）中元素的互换。
  array<char, 50> addr1{"http://c.biancheng.net"};
  array<char, 50> addr2{"http://c.biancheng.net/stl"};
  addr1.swap(addr2);
  printf("addr1 is：%s\n", addr1.data());
  printf("addr2 is：%s\n", addr2.data());
  // addr1 is：http://c.biancheng.net/stl
  // addr2 is：http://c.biancheng.net

  // 当两个array容器满足大小相同并且保存元素的类型相同时，两个array容器可以直接直接做赋值操作，即将一个容器中的元素赋值给另一个容器
  addr1 = addr2;
  printf("%s\n", addr1.data());  // http://c.biancheng.net
  // 在满足以上2个条件的基础上，如果其保存的元素也支持比较运算符，就可以用任何比较运算符直接比较两个array容器
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
  // 两个容器比较大小的原理，和两个字符串比较大小是一样的，即从头开始，逐个取两容器中的元素进行大小比较（根据ASCII码表），直到遇到两个不相同的元素，那个元素的值大，则该容器就大。

  // 总之，array 容器就是普通数组的“升级版”，使用普通数组能实现的，使用array容器都可以实现，而且无论是代码功能的实现效率，还是程序执行效率，都比普通数组更高。

  return 0;
}