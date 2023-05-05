#include <cstring>
#include <iostream>
#include <map>
#include <string>
#include <vector>

char arc[] = "http://c.biancheng.net/cplus/11/";
std::vector<char> v(arc, arc + 23);

namespace n1 {
// C++11标准之前，使用for循环语句遍历一个数组或者容器，只能：
// for(表达式 1; 表达式 2; 表达式 3){}

void func() {
  // for循环遍历普通数组：
  for (int i = 0; i < strlen(arc); i++) {
    std::cout << arc[i];
  }
  std::cout << std::endl;

  std::vector<char>::iterator iter;
  // for循环遍历vector容器：
  for (iter = v.begin(); iter != v.end(); ++iter) {
    std::cout << *iter;
  }
  std::cout << std::endl;

  // http://c.biancheng.net/cplus/11/
  // http://c.biancheng.net/
}
}  // namespace n1

namespace n2 {
// 而C++11标准中，为for循环添加了一种全新的语法格式，如下所示：
// for (declaration : expression){}
// 其中，两个参数各自的含义如下，
// 1.declaration：
//   表示此处要定义一个变量，该变量的类型为要遍历序列中存储元素的类型。
//   C++11中，declaration参数处定义的变量类型可以用auto关键字表示，
//   该关键字可以使编译器自行推导该变量的数据类型。
// 2.expression：
//   表示要遍历的序列，可以为事先定义好的普通数组或者容器，
//   还可以是用{}大括号初始化的序列。

// 同C++98/03中for相比，此格式并没有明确限定for循环的遍历范围，
// 这是它们最大的区别，即旧格式的for循环可以指定循环的范围，
// 新的for只会逐个遍历expression参数处指定序列中的每个元素。

void func1() {
  // for循环遍历普通数组：
  for (char ch : arc) {
    std::cout << '[' << ch << ']';
  }
  std::cout << std::endl;
  // 最后还有一个空，新格式的for循环在遍历字符串序列时，
  // 不只是遍历到最后一个字符，还会遍历位于该字符串末尾的'\0'：
  // [h][t][t][p][:][/][/][c][.][b][i][a][n][c][h][e]
  // [n][g][.][n][e][t][/][c][p][l][u][s][/][1][1][/][]
}

void func2() {
  // for循环遍历vector容器：
  for (auto ch : v) {
    std::cout << '[' << ch << ']';
  }
  std::cout << std::endl;
  // 没有空，是因为v容器中没有存储'\0'：
  // [h][t][t][p][:][/][/][c][.][b][i][a]
  // [n][c][h][e][n][g][.][n][e][t][/]

  // 程序中在遍历v容器时，定义了auto类型的ch变量，当编译器编译程序时，
  // 会通过v容器中存储的元素类型自动推导出ch为char类型。
  // 这里的ch不是迭代器类型，而表示的是v容器中存储的每个元素。
}

void func3() {
  // 新语法格式的for循环还支持遍历用{}大括号初始化的列表：
  for (int num : {1, 2, 3, 4, 5}) {
    std::cout << num << " ";
  }
  std::cout << std::endl;  // 1 2 3 4 5
}

void func4() {
  // 在使用新语法格式的for循环遍历某个序列时，
  // 如果需要遍历的同时修改序列中元素的值，
  // 实现方案是在declaration参数处定义引用形式的变量:
  char a[] = "abcde";
  std::vector<char> m(a, a + 5);
  // for循环遍历并修改容器中各个字符的值：
  for (auto ch : m) {
    ch++;
  }
  for (auto ch : m) {
    std::cout << ch;
  }
  std::cout << std::endl;  // abcde，没有修改成功

  // for循环遍历并修改容器中各个字符的值：
  for (auto &ch : m) {
    ch++;
  }
  for (auto ch : m) {
    std::cout << ch;
  }
  std::cout << std::endl;  // bcdef，修改成功

  // 参数既可以定义成普通变量，也可以定义成引用，应该如何选择呢？
  // 其实很简单：
  // 1.如果需要在遍历序列的过程中修改元素值，就必须定义引用形式的变量；
  // 2.反之，建议用const&，避免了复制，效率更高，也可以定义普通变量。
}

void func() {
  func1();
  func2();
  func3();
  func4();
}
}  // namespace n2

namespace n3 {
// C++11 for循环使用注意事项：
// 1.首先需要明确的一点是，当使用for循环遍历某个序列时，
//   无论该序列是普通数组、容器还是用{}大括号包裹的初始化列表，
//   遍历序列的变量都表示的是当前序列中的各个元素，不是迭代器；
// 2.基于范围的for循环可以遍历普通数组、string、容器以及初始化列表。
//   除此之外，循环冒号后还可以放置返回string字符串以及容器对象的函数；
std::string retStr() { return arc; }
char *retStr1() { return arc; }
std::vector<int> retVector() { return std::vector<int>{1, 2, 3}; }
void func1() {
  // 遍历函数返回的string字符串：
  for (char ch : retStr()) {
    std::cout << ch;
  }
  std::cout << std::endl;
  // http://c.biancheng.net/cplus/11/

  // 遍历函数返回的vector容器：
  for (int num : retVector()) {
    std::cout << num << " ";
  }
  std::cout << std::endl;  // 1 2 3

  // 注意，基于范围的for不支持遍历函数返回的以指针形式表示的数组，比如：
  // for (char ch : retStr1()) {
  //   std::cout << ch;
  // }
  // ‘begin’ was not declared in this scope
  // 原因很简单，此格式的for循环只能遍历有明确范围的一组数据，
  // retStr1()函数返回的是指针变量，遍历范围并未明确指明，所以编译失败。
}

// 3.当基于范围的for循环遍历的是某函数返回的string对象或者容器时，
//   整个遍历过程中，函数只会执行一次；
std::string retStr2() {
  std::cout << "retStr2:";
  return arc;
}
void func2() {
  // 遍历函数返回的string字符串：
  for (char ch : retStr2()) {
    std::cout << ch;
  }
  std::cout << std::endl;
  // retStr2:http://c.biancheng.net/cplus/11/
}

// 4.基于关联式容器（包括哈希容器）底层存储机制的限制。
void func3() {
  // 不允许修改map、unordered_map、multimap以及unordered_multimap存储的键的值；
  // 不允许修改set、unordered_set、multiset以及unordered_multiset存储的元素的值。
  // 如果修改了键值会导致程序的执行出现各种Bug。
  // 另外，基于范围的for循环完成对容器的遍历，其底层也是借助容器的迭代器实现的：
  std::vector<int> arr = {1, 2, 3, 4, 5};
  for (auto val : arr) {
    std::cout << val << ' ';
    arr.push_back(10);  // 向容器中添加元素
  }
  std::cout << std::endl;  // 1 0 -704102384 21992 5
  // 执行结果并不是我们想要的，因为在遍历的同时向尾部添加了新的元素（容器扩容），
  // 致使遍历容器所使用的迭代器失效，整个遍历过程出现错误。
  // 因此，在使用基于范围的for循环遍历时，应避免在循环体中修改容器元素的个数。
}

void func() {
  func1();
  func2();
  func3();
}
}  // namespace n3

int main(int argc, char *argv[]) {
  if (argc < 2) {
    std::cout << argv[0] << " i [0 - 2]" << std::endl;
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
    default:
      std::cout << "invalid type" << std::endl;
      break;
  }

  return 0;
}