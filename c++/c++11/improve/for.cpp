#include <cstring>
#include <iostream>
#include <map>
#include <string>
#include <vector>

using namespace std;

string str = "http://c.biancheng.net/cplus/11/";
vector<int> myvector = {1, 2, 3, 4, 5};
string retStr() {
  cout << "retStr:";
  return str;
}
vector<int> retVector() { return myvector; }

char str1[] = "http://c.biancheng.net/cplus/11/";
char *retStr1() { return str1; }

int main() {
  // C++11标准之前（C++98/03标准），如果要用for循环语句遍历一个数组或者容器，只能套用如下结构：
  // for(表达式 1; 表达式 2; 表达式 3){
  //   //循环体
  // }

  char arc[] = "http://c.biancheng.net/cplus/11/";
  int i;
  // for循环遍历普通数组
  for (i = 0; i < strlen(arc); i++) {
    cout << arc[i];
  }
  cout << endl;
  vector<char> myvector(arc, arc + 23);
  vector<char>::iterator iter;
  // for循环遍历vector容器
  for (iter = myvector.begin(); iter != myvector.end(); ++iter) {
    cout << *iter;
  }
  cout << endl;

  // 而C++11标准中，除了可以沿用前面介绍的用法外，还为for循环添加了一种全新的语法格式，如下所示：
  // for (declaration : expression){
  //   //循环体
  // }
  // 其中，两个参数各自的含义如下：
  // declaration：表示此处要定义一个变量，该变量的类型为要遍历序列中存储元素的类型。
  // 需要注意的是，C++11标准中，declaration参数处定义的变量类型可以用auto关键字表示，该关键字可以使编译器自行推导该变量的数据类型。
  // expression：表示要遍历的序列，常见的可以为事先定义好的普通数组或者容器，还可以是用{}大括号初始化的序列。

  // 同C++98/03中for循环的语法格式相比较，此格式并没有明确限定for循环的遍历范围，这是它们最大的区别，即旧格式的for循环可以指定循环的范围，
  // 而C++11标准增加的for循环，只会逐个遍历expression参数处指定序列中的每个元素。

  // for循环遍历普通数组
  for (char ch : arc) {
    cout << '[' << ch << ']';
  }
  // [h][t][t][p][:][/][/][c][.][b][i][a][n][c][h][e][n][g][.][n][e][t][/][c][p][l][u][s][/][1][1][/][]
  // 最后还有一个空，新格式的for循环在遍历字符串序列时，不只是遍历到最后一个字符，还会遍历位于该字符串末尾的'\0'（字符串的结束标志）。
  cout << endl;
  // for循环遍历vector容器
  for (auto ch : myvector) {
    cout << '[' << ch << ']';
  }
  // [h][t][t][p][:][/][/][c][.][b][i][a][n][c][h][e][n][g][.][n][e][t][/]
  // 没有空，是因为myvector容器中没有存储'\0'。
  cout << endl;

  // 程序中在遍历myvector容器时，定义了auto类型的ch变量，当编译器编译程序时，会通过myvector容器中存储的元素类型自动推导出ch为char类型。
  // 注意，这里的ch不是迭代器类型，而表示的是myvector容器中存储的每个元素。

  // 新语法格式的for循环还支持遍历用{}大括号初始化的列表:
  for (int num : {1, 2, 3, 4, 5}) {
    cout << num << " "; // 1 2 3 4 5
  }
  cout << endl;

  // 在使用新语法格式的for循环遍历某个序列时，如果需要遍历的同时修改序列中元素的值，实现方案是在declaration参数处定义引用形式的变量:
  char a[] = "abcde";
  vector<char> m(a, a + 5);
  for (auto ch : m) { // for循环遍历并修改容器中各个字符的值
    ch++;
  }
  for (auto ch : m) { // for循环遍历输出容器中各个字符
    cout << ch;       // abcde,没有修改成功
  }
  cout << endl;
  for (auto &ch : m) { // for循环遍历并修改容器中各个字符的值
    ch++;
  }
  for (auto ch : m) { // for循环遍历输出容器中各个字符
    cout << ch;       // bcdef,修改成功
  }
  cout << endl;

  // declaration参数既可以定义普通形式的变量，也可以定义引用形式的变量，应该如何选择呢？其实很简单:
  // 如果需要在遍历序列的过程中修改器内部元素的值，就必须定义引用形式的变量；
  // 反之，建议定义const&（常引用）形式的变量（避免了底层复制变量的过程，效率更高），也可以定义普通变量。

  // C++11 for循环使用注意事项:
  // 1)首先需要明确的一点是，当使用for循环遍历某个序列时，无论该序列是普通数组、容器还是用{}大括号包裹的初始化列表，遍历序列的变量都表示的是当前序列中的各个元素，不是迭代器。
  map<string, string> mymap{{"C++11", "http://c.biancheng.net/cplus/11/"},
                            {"Python", "http://c.biancheng.net/python/"},
                            {"Java", "http://c.biancheng.net/java/"}};
  for (pair<string, string> ch : mymap) {
    cout << ch.first << " " << ch.second << endl;
  }
  // 基于范围的for循环也可以直接遍历某个字符串，比如：
  for (char ch : "http://c.biancheng.net/cplus/11/") {
    cout << ch;
  }
  cout << endl;
  // 当然，基于范围的for循环也可以遍历string类型的字符串，这种情况下冒号前定义char类型的变量即可。
  string str = "abcdefg";
  for (char ch : str) {
    cout << ch;
  }
  cout << endl;

  // 2)总的来说，基于范围的for循环可以遍历普通数组、string字符串、容器以及初始化列表。除此之外，for循环冒号后还可以放置返回string字符串以及容器对象的函数，比如：
  for (char ch : retStr()) { //遍历函数返回的string字符串
    cout << ch;
  }
  cout << endl;
  for (int num : retVector()) { //遍历函数返回的vector容器
    cout << num << " ";
  }
  cout << endl;
  // 注意，基于范围的for循环不支持遍历函数返回的以指针形式表示的数组，比如：
  // for (char ch : retStr1()) { // ‘begin’ was not declared in this scope
  //   cout << ch;
  // }
  // 原因很简单，此格式的for循环只能遍历有明确范围的一组数据，上面程序中retStr1()函数返回的是指针变量，遍历范围并未明确指明，所以编译失败。

  // 3)值得一提的是，当基于范围的for循环遍历的是某函数返回的string对象或者容器时，整个遍历过程中，函数只会执行一次。
  for (char ch : retStr()) { // 遍历函数返回的string字符串
    cout << ch;              // retStr:http://c.biancheng.net/cplus/11/
  }
  cout << endl;

  // 4) 基于关联式容器（包括哈希容器）底层存储机制的限制：
  // 不允许修改map、unordered_map、multimap以及unordered_multimap容器存储的键的值；
  // 不允许修改set、unordered_set、multiset以及unordered_multiset容器中存储的元素的值。
  // 因此，当使用基于范围的for循环遍历此类型容器时，切勿修改容器中不允许被修改的数据部分，否则会导致程序的执行出现各种Bug。
  // 另外，基于范围的for循环完成对容器的遍历，其底层也是借助容器的迭代器实现的。举个例子：
  std::vector<int> arr = {1, 2, 3, 4, 5};
  for (auto val : arr) {
    std::cout << val << ' '; // 结果不唯一：1 0 3 4 5
    arr.push_back(10);       // 向容器中添加元素
  }
  cout << endl;
  // 执行结果并不是我们想要的。因为在for循环遍历arr容器的同时向该容器尾部添加了新的元素（对arr容器进行了扩增），致使遍历容器所使用的迭代器失效，整个遍历过程出现错误。
  // 因此，在使用基于范围的for循环遍历容器时，应避免在循环体中修改容器存储元素的个数。

  return 0;
}