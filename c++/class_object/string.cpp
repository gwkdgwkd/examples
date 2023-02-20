#include <string.h>  // C风格字符串相关函数的头文件，比如strlen
#include <iostream>
#include <string>  // string类库的头文件

// C++大大增强了对字符串的支持，除了可以使用C风格的字符串，还可以使用string类。
// string类处理起字符串来会方便很多，完全可以代替C语言中的字符数组或字符串指针。
// string变量可以直接通过赋值操作符=进行赋值。
// string变量也可以用C风格的字符串进行赋值。

// string的内部究竟是什么样的？
// 在C语言中，有两种方式表示字符串（两种形式总是以\0作为结束标志）：
// 1.一种是用字符数组来容纳字符串，这样的字符串是可读写的：
//   char str[10] = "abc";
// 2.一种是使用字符串常量，这样的字符串只能读，不能写：
//   char *str = "abc";

// C++ string与它们在C语言中的前身截然不同。
// 首先，也是最重要的不同点，C++ string隐藏了它所包含的字符序列的物理表示。
// 程序设计人员不必关心数组的维数或\0方面的问题。
// string在内部封装了与内存和容量有关的信息。
// C++ string对象知道自己在内存中的开始位置、包含的字符序列以及字符序列长度；
// 当内存空间不足时，string还会自动调整，增加内存以足以容纳下所有字符。
// C++ string的这种做法，极大地减少了C语言编程中三种最常见且最具破坏性的错误：
// 1.数组越界；
// 2.通过未被初始化或者被赋以错误值的指针来访问数组元素；
// 3.释放了数组所占内存，但是仍然保留了悬空指针。
// C++标准没有定义string类的内存布局，各个编译器厂商可以提供不同的实现，
// 但必须保证string的行为一致，采用这种做法是为了获得足够的灵活性。
// C++标准没有定义在哪种确切的情况下应该为string对象分配内存空间来存储字符序列。
// string内存分配规则明确规定：
// 允许但不要求以引用计数（reference counting）的方式实现。
// 但无论是否采用引用计数，其语义都必须一致。
// C++的这种做法和C语言不同，在C语言中，每个字符型数组都占据各自的物理存储区。
// 在C++中，独立的几个string对象可以占据也可以不占据各自特定的物理存储区，
// 但是，如果采用引用计数避免了保存同一数据的拷贝副本，
// 那么各个独立的对象必须看起来并表现得就像独占地拥有各自的存储区一样。
// 只有当字符串被修改的时候才创建各自的拷贝，
// 这种实现方式称为写时复制（copy-on-write）策略。
// 当字符串只是作为值参数或在其他只读情形下使用，这种方法能够节省时间和空间。
// 不论一个库的实现是不是采用引用计数，它对string类的使用者来说都应该是透明的。
// 遗憾的是，情况并不总是这样，在多线程程序中，几乎不可能安全地使用引用计数来实现。

// 字符串是存储在内存的连续字节中的一系列字符。
// C++处理字符串的方式有两种：
// 1.来自C语言，常被称为C风格字符串；
// 2.基于string类库的字符串处理方式。

namespace n1 {
void func1() {
  std::string s1;            // 创建一个空的string对象
  std::string s2("Hello!");  // 将string对象初始化为参数指向的字符串
  std::string s3(10, 'a');    // 创建string对象，包含n个字符c
  std::string s4 = "hello!";  // 使用C语言风格来初始化string对象
  std::string s5(s2);         // 拷贝构造函数
  std::string s6 = s2;        // 拷贝构造函数
  std::string s7;
  s7 = s2;  // 赋值操作符

  // string类重载了输入输出运算符，可以像对待普通变量那样对待string变量，
  // 也就是用>>进行输入，用<<进行输出：
  std::cout << std::boolalpha << s1.empty() << std::endl;  // true
  std::cout << s2 << std::endl;                            // Hello!
  std::cout << s3 << std::endl;                            // aaaaaaaaaa
  std::cout << s4 << std::endl;                            // hello!
  std::cout << s5 << std::endl;                            // Hello!
  std::cout << s6 << std::endl;                            // Hello!
  std::cout << s7 << std::endl;                            // Hello!
}

void func2() {
  std::string s = "hello!";

  // 转换函数c_str()，能够将string字符串转换为C风格的字符串，
  // 并返回该字符串的const指针（const char*）。
  // 在C语言中，使用strlen函数获取字符串的长度：
  std::cout << strlen(s.c_str()) << std::endl;  // 6

  // 在C++中，可以使用size函数或length函数来获得string对象的长度：
  std::cout << s.length() << std::endl;  // 6
  std::cout << s.size() << std::endl;    // 6

  // lenght()和size()的实现并没有区别：
  // 1.length()方法是C语言习惯保留的；
  // 2.size()方法则是为了兼容STL容器而引入的。

  // 由于string的末尾没有'\0'字符，
  // 所以length()返回的是字符串的真实长度，而不是长度+1。
}

void func3() {
  // 在C语言中，使用strcpy、strncpy函数来实现字符串的复制。
  // 在C++中则方便很多，可以直接将一个string对象赋值给另一个string对象。
  // string类会自动调整对象的大小，因此不需要担心目标数组不够大的问题。

  std::string s1 = "hello!";
  std::cout << s1.size() << " " << s1.capacity() << std::endl;  // 6 15
  s1 = "123456789123456789";
  std::cout << s1.size() << " " << s1.capacity() << std::endl;  // 18 30
}

void func4() {
  // 在C语言中，使用strcat、strncat函数来进行字符串拼接操作。
  // 在C++中也有多种方法来实现字符串拼接和附加操作。
  // 再也不需要使用C语言中的strcat()、strcpy()、malloc()来拼接字符串了，
  // 再也不用担心空间不够会溢出了。

  std::string s1 = "hello";
  std::string s2 = "world";

  // 使用+操作符拼接两个字符串：
  std::string s3 = s1 + "," + s2;
  // 使用+=操作符在字符串后面附加内容：
  std::string s4 = "!";
  s3 += s4;
  s3 += "i ";
  // 使用string.append()函数：
  std::string s5 = "am ";
  s3.append(s5);
  s3.append("lwl");
  // 使用string.push_back()函数：
  s3.push_back('!');

  std::cout << s3 << std::endl;  // hello,world!i am lwl!
}

void func5() {
  // 可以像C语言中一样，将string对象当做一个数组，
  // 然后使用数组下标的方式来访问字符串中的元素；
  // 也可以使用string.at(index)的方式来访问元素（索引号0开始）。
  std::string s = "hello";
  std::cout << s[1] << std::endl;     // e
  std::cout << s.at(0) << std::endl;  // h

  std::cout << s[18] << std::endl;  // 越界了，可能会崩溃
  try {
    std::cout << s.at(10) << std::endl;  // 越界抛出异常out_of_range
  } catch (std::out_of_range e) {
    std::cout << e.what() << std::endl;
    // basic_string::at: __n (which is 10) >= this->size() (which is 5)
  }
}

void func6() {
  // 可以使用string.substr()函数来获取子串，定义如下：
  // string substr(size_t pos = 0，size_t len = npos) const;
  // 其中，pos是子字符串的起始位置，len是子串的长度。
  // 复制一个string中从pos处开始的len个字符到substr中去，并返回substr。
  std::string s = "hello,world";
  std::cout << s.substr(1, 4) << std::endl;  // ello

  // 对substr()参数的处理和erase()类似：
  // 1.如果pos越界，会抛出异常；
  // 2.如果len越界，会提取从pos到字符串结尾处的所有字符。
  std::cout << s.substr(2, 70) << std::endl;  // llo,world

  try {
    s.substr(20, 44);  // pos越界会抛出out_of_range
  } catch (std::out_of_range e) {
    std::cout << e.what() << std::endl;
    // basic_string::substr : __pos(which is 20) > this->size()(which is 11)
  }
}

void func() {
  func1();
  func2();
  func3();
  func4();
  func5();
  func6();
}
}  // namespace n1

namespace n2 {
void func1() {
  // 在C语言中，使用strcmp、strncmp函数来进行字符串的比较。
  // 在C++中，由于将string对象声明为了简单变量，比较操作十分简单了，
  // 直接使用关系运算符（==、!=、<、<=、>、>=）即可。
  // 也可以使用类似strcmp的函数来进行string对象的比较，
  // string类提供的是string.compare()方法。

  auto compare = [](const std::string &str1, const std::string &str2) {
    if (str1 == str2) {
      std::cout << "s1 = s2" << std::endl;
    } else if (str1 < str2) {
      std::cout << "s1 < s2" << std::endl;
    } else {
      std::cout << "s1 > s2" << std::endl;
    }
  };

  std::string s1("hello");
  std::string s2("hello");
  compare(s1, s2);                           // s1 = s2
  std::cout << s1.compare(s2) << std::endl;  // 0

  s1 += ',';
  compare(s1, s2);                           // s1 > s2
  std::cout << s1.compare(s2) << std::endl;  // 1

  s1[1] = 'c';
  compare(s1, s2);                           // s1 < s2
  std::cout << s1.compare(s2) << std::endl;  // -1
}

void func2() {
  std::string s1 = "hello";
  std::string s2 = "world";

  std::cout << s1 << std::endl;  // hello
  std::cout << s2 << std::endl;  // world
  s1.swap(s2);
  std::cout << s1 << std::endl;  // world
  std::cout << s2 << std::endl;  // hello
}

void func3() {
  std::string s;
  getline(std::cin, s);  // 从输入流中读取一行数据到str
  std::cout << s << std::endl;
}

void func() {
  func1();
  func2();
  func3();
}
}  // namespace n2

namespace n3 {
void func() {
  // 使用string.find()方法查找字符串，
  // 如果找到，则返回该子字符串首次出现时其首字符的索引；
  // 否则，返回string::npos。
  auto check = [](const int &i) {
    if (i == std::string::npos) {  // npos：一个无穷大值4294967295
      std::cout << "not found" << std::endl;
    } else {
      std::cout << "find : " << i << std::endl;
    }
  };

  std::string s("cat,dog,cat,pig,little cat,hotdog,little pig,angry dog");
  std::string cat = "cat";
  check(s.find(cat));                // find : 0
  check(s.find(cat, 0));             // find : 0
  check(s.find(cat, 1));             // find : 8
  check(s.find("hotdog"));           // find : 27
  check(s.find("dog", 1));           // find : 4
  check(s.find("dog", 15));          // find : 30
  check(s.find("dogsdfsd", 15));     // not found
  check(s.find("dogsdfsd", 15, 3));  // find : 30
  check(s.find('a', 0));             // find : 1

  // string.rfind()与string.find()方法类似，只是查找顺序不一样，
  // string.rfind()是从指定位置pos（默认为字符串末尾）开始向前查找，
  // 直到字符串的首部，并返回第一次查找到匹配项时匹配项首字符的索引。
  // 换句话说，就是查找子字符串或字符最后一次出现的位置。
  check(s.rfind(cat));                          // find : 23
  check(s.rfind(cat, s.size() - 1));            // find : 23
  check(s.rfind("hotdog"));                     // find : 27
  check(s.rfind("hotdog", s.length() - 1));     // find : 27
  check(s.rfind("dog", s.length() - 5));        // find : 30
  check(s.rfind("dog", 0));                     // not found
  check(s.rfind("dogsdf"));                     // not found
  check(s.rfind("dogsdf", s.length() - 5, 3));  // find : 30
  check(s.rfind("dog"));                        // find : 51
  check(s.rfind('o', s.length() - 1));          // find : 52

  // string.find_first_of()方法在字符串中从指定位置开始，
  // 向后查找参数中任何一个字符首次出现的位置：
  check(s.find_first_of(",wc"));     // find : 0
  check(s.find_first_of(",wc", 1));  // find : 3

  // string.find_last_of()方法在字符串中查找参数中，
  // 任何一个字符最后一次出现的位置，从指定位置向前找：
  check(s.find_last_of("ao"));      // find : 52
  check(s.find_last_of("ao", 0));   // not found
  check(s.find_last_of("ao", 50));  // find : 45

  // string.find_first_not_of()方法，
  // 在字符串中查找第一个不包含在参数中的字符：
  check(s.find_first_not_of("cat,"));      // find : 4
  check(s.find_first_not_of("cat,", 10));  // find : 12

  // string.find_last_not_of()方法，
  // 在字符串中查找最后一个不包含在参数中的字符：
  check(s.find_last_not_of("dog"));      // find : 50
  check(s.find_last_not_of("cat", 10));  // find : 6
}
}  // namespace n3

namespace n4 {
void func() {
  std::string s1 = "h!";
  std::string s2 = "o,";
  std::string s3 = "xoryyy";

  s1.insert(1, s2);              // 在位置pos处插入字符串s
  std::cout << s1 << std::endl;  // ho,!
  s1.insert(3, s3, 1, 2);  // 在pos处插入字符串s的从pos处开始的len个字符
  std::cout << s1 << std::endl;  // ho,or!
  s1.insert(3, "w");             // 在位置pos处插入字符串s
  std::cout << s1 << std::endl;  // ho,wor!
  s1.insert(6, "ldzzz", 2);  // 在位置pos处插入字符串s的前n个字符
  std::cout << s1 << std::endl;  // ho,world!
  s1.insert(1, 2, 'l');          // 在位置pos处插入n个字符c
  std::cout << s1 << std::endl;  // hllo,world!

  // 在p处插入字符c，并返回插入后迭代器的位置：
  auto pos = s1.insert(s1.begin() + 1, 'e');
  std::cout << s1 << std::endl;    // hello,world!
  std::cout << *pos << std::endl;  // e

  pos = s1.insert(s1.end() - 1, 2, '!');
  std::cout << s1 << std::endl;          // hello,world!!!
  std::cout << *(pos - 1) << std::endl;  // d
}
}  // namespace n4

namespace n5 {
void func() {
  std::string s = "hello,,,world!";

  // 删除从pos处开始的n个字符：
  std::cout << s.erase(5, 2) << std::endl;  // hello,world!

  // 删除p处的一个字符，并返回删除后迭代器的位置：
  auto pos = s.erase(s.end() - 1);
  std::cout << s << std::endl;           // hello,world
  std::cout << *(pos - 1) << std::endl;  // d

  // 删除从first到last之间的字符，并返回删除后迭代器的位置：
  pos = s.erase(s.begin() + 2, s.begin() + 4);
  std::cout << s << std::endl;           // heo,world
  std::cout << *(pos - 1) << std::endl;  // e

  // 如果不指明len的话，那么直接删除从pos到字符串结束处的所有字符：
  std::cout << s.erase(3) << std::endl;  // heo

  // 在pos参数没有越界的情况下，len参数也可能会导致要删除的子字符串越界。
  // 但实际上这种情况不会发生，
  // erase()函数会从以下两个值中取出最小的一个作为待删除子字符串的长度：
  // len的值和字符串长度减去pos的值。
  std::cout << s.erase(2, 10) << std::endl;  // he

  try {
    s.erase(10);  // pos越界会抛出out_of_range
  } catch (std::out_of_range e) {
    std::cout << e.what() << std::endl;
    // basic_string::erase: __pos (which is 10) > this->size() (which is 2)
  }
}
}  // namespace n5

int main(int argc, char *argv[]) {
  if (argc < 2) {
    std::cout << argv[0] << " i [0 - 4]" << std::endl;
    return 0;
  }
  int type = argv[1][0] - '0';
  switch (type) {
    case 0:
      n1::func();
      break;
    case 1:
      n2::func();
      break;
    case 2:
      n3::func();
      break;
    case 3:
      n4::func();
      break;
    case 4:
      n5::func();
      break;
    default:
      std::cout << "invalid type" << std::endl;
      break;
  }

  return 0;
}