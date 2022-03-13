#include <cstring>
#include <iomanip>
#include <iostream>

namespace n1 {
// 读入单个字符：
// get()是istream类的成员函数，它有多种重载形式，最简单常用的：int get();
// 此函数从输入流中读入一个字符，返回值就是该字符的ASCII码。
// 如果碰到输入的末尾，则返回值为EOF。
// istream类中从输入流（包括文件）中读取数据的成员函数，在把输入数据都读取完后再进行读取，就会返回EOF。
// EOF是在iostream类中定义的一个整型常量，值为-2。
// get()函数不会跳过空格、制表符、回车等特殊字符，所有的字符都能被读入。

// 程序中的变量c应为int类型，而不能是char类型。
// 在输入流中碰到ASCII码等于0xfe的字符时，cin.get()返回0xFF，0xFF赋值给c，此时如果c是char类型的，
// 那么其值就是-2（因为符号位为1代表负数），即等于EOF，于是程序就错误地认为输入已经结束。
// 而在c为int类型的情况下，将0xfe赋值给c，c的值是255（因为符号位为0，是正数），
// 而非-2，即除非读到输入末尾，c的值都不可能是-1。
void func1() {
  int c;
  while ((c = std::cin.get()) != EOF) std::cout.put(c);
}

void func2() {
  int c;
  freopen("in.txt", "r", stdin);  // 将标准输入重定向为in.txt
  while ((c = std::cin.get()) != EOF) std::cout.put(c);
}
}  // namespace n1

namespace n2 {
// 读入一行字符串（整行数据）：
// getline()是istream类的成员函数，它有如下两个重载版本：
//  istream & getline(char* buf, int bufSize);
//  istream & getline(char* buf, int bufSize, char delim);
// 第一个版本从输入流中读取bufSize-1个字符到缓冲区buf，或遇到\n为止（哪个条件先满足就按哪个执行）。
// 函数会自动在buf中读入数据的结尾添加\0。
// 第二个版本和第一个版本的区别在于，第一个版本是读到\n为止，第二个版本是读到delim字符为止。
// \n或delim都不会被读入buf，但会被从输入流中取走。
// 这两个函数的返回值就是函数所作用的对象的引用。
// 如果输入流中\n或delim之前的字符个数达到或超过bufSize，就会导致读入出错，虽然本次读入已经完成，但是之后的读入都会失败。
// 从输入流中读入一行，可以用第一个版本。
// 用cin>>str这种写法是不行的，因为此种读法在碰到行中的空格或制表符时就会停止，因此就不能保证str中读入的是整行。
// 可以用getline()函数的返回值（为false则输入结束）来判断输入是否结束。

void func1() {
  char szBuf[20];
  int n = 120;
  if (!std::cin.getline(szBuf, 6))  // 如果输入流中一行字符超过5个，就会出错
    std::cout << "error" << std::endl;
  std::cout << szBuf << std::endl;
  // 读入时因字符串超长导致出错，于是并没有从输入流读入n，n维持了原来的值120。

  std::cin >> n;
  std::cout << n << std::endl;
  std::cin.clear();  // clear能够清除cin内部的错误标记，使之恢复正常
  std::cin >> n;  // 把之前剩下的又读出来了
  std::cout << n << std::endl;
  // 第一次大于5个
  // adbde12345
  // error
  // adbde
  // 120
  // 12345
}

void func2() {
  char szBuf[20];
  memset(szBuf, 0, sizeof(szBuf));
  if (!std::cin.getline(szBuf, 6, ' ')) {
    std::cout << "error" << std::endl;
  }
  std::cout << szBuf << std::endl;
  // ni hao
  // ni
}

const int MAX_LINE_LEN = 10000;  // 假设文件中一行最长10000个字符
void func3() {
  char szBuf[MAX_LINE_LEN + 10];
  freopen("test.txt", "r", stdin);  // 将标准输入重定向为test.txt
  while (std::cin.getline(szBuf, MAX_LINE_LEN + 5)) {
    std::cout << szBuf << std::endl;
  }
}
}  // namespace n2

namespace n3 {
// 如何跳过（忽略）指定字符？
// ignore()是istream类的成员函数，它的原型是：
//  istream & ignore(int n =1, int delim = EOF);
// 此函数的作用是跳过输入流中的n个字符，或跳过delim及其之前的所有字符，哪个条件先满足就按哪个执行。
// 两个参数都有默认值，因此cin.ignore()就等效于cin.ignore(1, EOF)，即跳过一个字符。
// 该函数常用于跳过输入中的无用部分，以便提取有用的部分。
void testN3() {
  // 输入的电话号码形式是Tel:63652823，Tel:就是无用的内容。
  int n;
  std::cin.ignore(4, 'A');
  std::cin >> n;
  std::cout << n << std::endl;
  // Tel:123456
  // 123456
  // Tel:123A11234556
  // 123
}
}  // namespace n3

namespace n4 {
// 怎样查看输入流中的下一个字符？
// peek()是istream类的成员函数，它的原型是：int peek();
// 此函数返回输入流中的下一个字符，但是并不将该字符从输入流中取走——相当于只是看了一眼下一个字符，因此叫peek。
// cin.peek()不会跳过输入流中的空格、回车符。
// 在输入流已经结束的情况下，cin.peek()返回EOF。
// 在输入数据的格式不同，需要预先判断格式再决定如何输入时，peek()就能起到作用。

// istream还有一个成员函数istsdream &putback(char c)，可以将一个字符插入输入流的最前面。
// 对于下面的例题，也可以在用get()函数读取一个字符并判断是中式格式还是西式格式时，
// 将刚刚读取的字符再用putback()成员函数放回流中，然后再根据判断结果进行不同方式的读入。
std::string Months[12] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun",
                          "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
int get_time() {
  int c;
  while ((c = std::cin.peek()) != EOF) {  // 取输入流中的第一个字符进行查看
    int year, month, day;
    if (c >= 'A' && c <= 'Z') {  // 美国日期格式
      std::string sMonth;
      std::cin >> sMonth >> day >> year;
      for (int i = 0; i < 12; ++i)  // 查找月份
        if (sMonth == Months[i]) {
          month = i + 1;
          break;
        }
    } else {  // 中国日期格式
      std::cin >> year;
      std::cin.ignore() >> month;  // 用ignore跳过 "2011.12.3"中的'.'
      std::cin.ignore() >> day;
    }
    std::cin.ignore();  // 跳过行末 '\n'
    std::cout << std::setfill('0') << std::setw(2)
              << month;  // 设置填充字符'\0'，输出宽度2
    std::cout << "-" << std::setw(2) << day << "-" << std::setw(4) << year
              << std::endl;
  }
  return 0;
}

void testN4() {
  get_time();

  // 输入：
  // Dec 3 1990
  // 2011.2.3
  // 458.12.1
  // Nov 4 1998
  // Feb 12 2011
  // 输出：
  // 12-03-1990
  // 02-03-2011
  // 12-01-0458
  // 11-04-1998
  // 02-12-2011
}
}  // namespace n4

namespace n5 {
// cin是如何判断输入结束（读取结束）的？
// cin可以用来从键盘输入数据；将标准输入重定向为文件后，cin也可以用来从文件中读入数据。
// 在输入数据的多少不确定，且没有结束标志的情况下，该如何判断输入数据已经读完了呢？
// 从文件中读取数据很好办，到达文件末尾就读取结束了。
// 从控制台读取数据怎么办呢？总不能把控制台关闭吧？这样程序也运行结束了！
// 其实，在控制台中输入特殊的控制字符就表示输入结束了：
//  在Windows系统中，通过键盘输入时，按Ctrl+Z组合键后再按回车键，就代表输入结束。
//  在UNIX/Linux/Mac OS系统中，Ctrl+D代表输入结束。
// 不管是文件末尾，还是Ctrl+Z或者Ctrl+D，它们都是结束标志；
// cin在正常读取时返回true，遇到结束标志时返回false，我们可以根据cin的返回值来判断是否读取结束。
// 在这种情况下，文件中并不需要包含Ctrl+Z或者Ctrl+D，只要有用空格或回车隔开的若干个正整数即可。
// cin读到文件末尾时，cin>>n就会返回false，从而导致程序结束。

void testN5() {
  int n;
  int maxN = 0;
  // cin>>n的返回值的确是istream &类型的，而while语句中的条件表达式的返回值应该是bool类型、
  // 整数类型或其他和整数类型兼容的类型，istream &显然和整数类型不兼容，为什么while(cin>>n)还能成立呢？
  // 这是因为，istream类对强制类型转换运算符bool进行了重载，这使得cin对象可以被自动转换成bool类型。
  // 所谓自动转换的过程，就是调用cin的operator bool()这个成员函数，而该成员函数可以返回某个标志值，
  // 该标志值在cin没有读到输入结尾时为true，读到输入结尾后变为false。
  // 对该标志值的设置，在operator <<()成员函数中进行。
  // 程序本该输入整数，如果输入了一个字母，则程序就会结束。
  // 因为，应该读入整数时却读入了字母也算读入出错。
  while (std::cin >> n) {  // 输入没有结束，cin就返回true，条件就为真；
    if (maxN < n) maxN = n;
  }
  // 1
  // 2
  // 3
  // 4 56
  // ctrl+D
  std::cout << maxN << std::endl;  // 56
}
}  // namespace n5

int main(int argc, char* argv[]) {
  if (argc < 2) {
    std::cout << argv[0] << " i [0 - 7]" << std::endl;
    return 0;
  }
  int type = argv[1][0] - '0';
  switch (type) {
    case 0:
      n1::func1();
      break;
    case 1:
      n1::func2();
      break;
    case 2:
      n2::func1();
      break;
    case 3:
      n2::func2();
      break;
    case 4:
      n2::func3();
      break;
    case 5:
      n3::testN3();
      break;
    case 6:
      n4::testN4();
      break;
    case 7:
      n5::testN5();
      break;
    default:
      std::cout << "invalid type" << std::endl;
      break;
  }

  return 0;
}
