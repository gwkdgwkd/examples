#include <iostream>
#include <string>

using namespace std;

// C++大大增强了对字符串的支持，除了可以使用C风格的字符串，还可以使用内置的string类。
// string类处理起字符串来会方便很多，完全可以代替C语言中的字符数组或字符串指针。
// 使用string类需要包含头文件<string>。
// string变量可以直接通过赋值操作符=进行赋值。
// string变量也可以用C风格的字符串进行赋值。

// string的内部究竟是什么样的？
// 在C语言中，有两种方式表示字符串(两种形式总是以\0作为结束标志)：
//  一种是用字符数组来容纳字符串，例如char str[10] = "abc"，这样的字符串是可读写的；
//  一种是使用字符串常量，例如char *str = "abc"，这样的字符串只能读，不能写。
// C++ string与它们在C语言中的前身截然不同。
// 首先，也是最重要的不同点，C++ string隐藏了它所包含的字符序列的物理表示。
// 程序设计人员不必关心数组的维数或\0方面的问题。
// string在内部封装了与内存和容量有关的信息。
// 具体地说，C++ string对象知道自己在内存中的开始位置、包含的字符序列以及字符序列长度；
// 当内存空间不足时，string还会自动调整，让内存空间增长到足以容纳下所有字符序列的大小。
// C++ string的这种做法，极大地减少了C语言编程中三种最常见且最具破坏性的错误：
//  数组越界；
//  通过未被初始化或者被赋以错误值的指针来访问数组元紊；
//  释放了数组所占内存，但是仍然保留了“悬空”指针。
// C++标准没有定义string类的内存布局，各个编译器厂商可以提供不同的实现，但必须保证string的行为一致。
// 采用这种做法是为了获得足够的灵活性。
// C++标准没有定义在哪种确切的情况下应该为string对象分配内存空间来存储字符序列。
// string内存分配规则明确规定：允许但不要求以引用计数（reference counting）的方式实现。
// 但无论是否采用引用计数，其语义都必须一致。
// C++的这种做法和C语言不同，在C语言中，每个字符型数组都占据各自的物理存储区。
// 在C++中，独立的几个string对象可以占据也可以不占据各自特定的物理存储区，但是，如果采用引用计数避免
// 了保存同一数据的拷贝副本，那么各个独立的对象必须看起来并表现得就像独占地拥有各自的存储区一样。
// 只有当字符串被修改的时候才创建各自的拷贝，这种实现方式称为写时复制（copy-on-write）策略。
// 当字符串只是作为值参数或在其他只读情形下使用，这种方法能够节省时间和空间。
// 不论一个库的实现是不是采用引用计数，它对string类的使用者来说都应该是透明的。
// 遗憾的是，情况并不总是这样，在多线程程序中，几乎不可能安全地使用引用计数来实现。

int main() {
  // 定义string变量（对象）的方法：
  string s1;
  string s2 = "c plus plus";
  string s3 = s2;
  string s4(5, 's');

  // 与C风格的字符串不同，当我们需要知道字符串长度时，可以调用string类提供的length()函数:
  cout << s2.length() << endl;  // 11
  // 由于string的末尾没有'\0'字符，所以length()返回的是字符串的真实长度，而不是长度+1。

  // 虽然C++提供了string类来替代C语言中的字符串，但是在实际编程中，有时候必须要使用C风格的字符串，
  // 为此，string类为我们提供了一个转换函数c_str()，该函数能够将
  // string字符串转换为C风格的字符串，并返回该字符串的const指针（const char*）。
  // 为了使用C语言中的fopen()函数打开文件，必须将string字符串转换为C风格的字符串。
  string path = "D:\\demo.txt";
  FILE *fp = fopen(path.c_str(), "rt");

  // string类重载了输入输出运算符，可以像对待普通变量那样对待string变量，也就是用>>进行输入，用<<进行输出。
  cout << s2 << endl;  // c plus plus

  // string字符串也可以像C风格的字符串一样按照下标来访问其中的每一个字符。string字符串的起始下标仍是从0开始。
  cout << s2[2] << endl;  // p

  // 有了string类，可以使用+或+=运算符来直接拼接字符串，非常方便，
  // 再也不需要使用C语言中的strcat()、strcpy()、malloc()来拼接字符串了，再也不用担心空间不够会溢出了。
  string ss1 = "first ";
  string ss2 = "second ";
  char *ss3 = (char *)"third ";
  char ss4[] = "fourth ";
  char ch = '@';
  string ss5 = ss1 + ss2;
  string ss6 = ss1 + ss3;
  string ss7 = ss1 + ss4;
  string ss8 = ss1 + ch;
  cout << ss5 << endl << ss6 << endl << ss7 << endl << ss8 << endl;
  // first second
  // first third
  // first fourth
  // first @

  // C++提供的string类包含了若干实用的成员函数，大大方便了字符串的增加、删除、更改、查询等操作。
  // insert()函数可以在string字符串中指定的位置插入另一个字符串，
  // insert()函数的第一个参数有越界的可能，如果越界，则会产生运行时异常
  // string& insert (size_t pos, const string& str);
  string t1("123456789");
  t1.insert(3, "nihao");
  cout << t1 << endl;  // 123nihao456789
  // erase()函数可以删除string中的一个子字符串。
  // string& erase (size_t pos = 0, size_t len = npos);
  // pos表示要删除的子字符串的起始下标，len表示要删除子字符串的长度。
  // 如果不指明len的话，那么直接删除从pos到字符串结束处的所有字符（len = str.length - pos）。
  // 在pos参数没有越界的情况下， len参数也可能会导致要删除的子字符串越界。
  // 但实际上这种情况不会发生，erase()函数会从以下两个值中取出最小的一个作为待删除子字符串的长度：
  //  len的值；
  //  字符串长度减去pos的值。
  t1.erase(3, 5);
  cout << t1 << endl;  // 123456789
  t1.erase(3);
  cout << t1 << endl;  // 123
  // substr()函数用于从string字符串中提取子字符串，它的原型为：
  // string substr (size_t pos = 0, size_t len = npos) const;
  // pos为要提取的子字符串的起始下标，len为要提取的子字符串的长度。
  // 对substr()参数的处理和erase()类似：
  //  如果pos越界，会抛出异常；
  //  如果len越界，会提取从pos到字符串结尾处的所有字符。
  string t2 = "first second third";
  string t3;
  t3 = t2.substr(6, 6);
  cout << t2 << endl;  // first second third
  cout << t3 << endl;  // second
  // string类提供了几个与字符串查找有关的函数
  // find()函数用于在string字符串中查找子字符串出现的位置，它其中的两种原型为：
  // size_t find (const string& str, size_t pos = 0) const;
  // size_t find (const char* s, size_t pos = 0) const;
  // 第一个参数为待查找的子字符串，它可以是string字符串，也可以是C风格的字符串。第二个参数为开始查找的位置（下标）；如果不指明，则从第0个字符开始查找。
  // find()函数最终返回的是子字符串第一次出现在字符串中的起始下标。
  int index = t2.find(t3, 2);
  // int index = t2.find(t3);
  if (index < t2.length())
    cout << "Found at index : " << index << endl;  // Found at index : 6
  else
    cout << "Not found" << endl;
  // rfind()和find() 很类似，同样是在字符串中查找子字符串，不同的是find()函数从第二个参数开始往后查找，而rfind()函数则最多查找到第二个参数处，如果到
  // 了第二个参数所指定的下标还没有找到子字符串，则返回一个无穷大值4294967295。
  index = t2.rfind(t3, 6);
  if (index < t2.length())
    cout << "Found at index : " << index << endl;  // Found at index : 6
  else
    cout << "Not found" << endl;
  // find_first_of()函数用于查找子字符串和字符串共同具有的字符在字符串中首次出现的位置。
  string b1 = "first second second third";
  string b2 = "asecond";
  index = b1.find_first_of(b2);
  if (index < b1.length())
    cout << "Found at index : " << index << endl;  // Found at index : 3
  else
    cout << "Not found" << endl;

  string y1("12345");
  string y2 = y1;
  cout << (y1 == y2) << endl;  // 1
  y1[0] = '6';
  cout << "s1 = " << y1 << endl;  // 62345
  cout << "s2 = " << y2 << endl;  // 12345
  cout << (y1 == y2) << endl;     // 0

  return 0;
}