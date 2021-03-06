#include <iostream>
#include <vector>

using namespace std;

// 当处理输入输出时，我们必须预计到其中可能发生的错误并给出相应的处理措施。
//  当我们输入时，可能会由于人的失误（错误理解了指令、打字错误、允许自家的小猫在键盘上散步等）、文件格式不符、错误估计了情况等原因造成读取失败。
//  当我们输出时，如果输出设备不可用、队列满或者发生了故障等，都会导致写入失败。
// 发生输入输出错误的可能情况是无限的！但C++将所有可能的情况归结为四类，称为流状态（stream state）。每种流状态都用一个iostate类型的标志位来表示。
// 流状态对应的标志位
//  badbit 	发生了（或许是物理上的）致命性错误，流将不能继续使用。
//  eofbit 	输入结束（文件流的物理结束或用户结束了控制台流输入，例如用户按下了Ctrl+Z或Ctrl+D组合键。
//  failbit I/O操作失败，主要原因是非法数据（例如，试图读取数字时遇到字母）。流可以继续使用，但会设置failbit标志。
//  goodbit 一切止常，没有错误发生，也没有输入结束。
// ios_base类定义了以上四个标志位以及iostate类型，但是ios类又派生自ios_base类，所以可以使用ios::failbit代替ios_base::failbit以节省输入。
// 一旦流发生错误，对应的标志位就会被设置，我们可以通过下表列出的函数检测流状态。C++流状态检测函数及其说明：
//  good() 	goodbit 操作成功，没有发生任何错误。
//  eof() 	eofbit 	到达输入末尾或文件尾。
//  fail() 	failbit 发生某些意外情况（例如，我们要读入一个数字，却读入了字符'x'）。
//  bad() 	badbit 	发生严重的意外（如磁盘读故障）。
// 不幸的是，fail()和bad()之间的区别并未被准确定义，程序员对此的观点各种各样。但是，基本的思想很简单：
//  如果输入操作遇到一个简单的格式错误，则使流进入fail() 状态，也就是假定我们（输入操作的用户）可以从错误中恢复。
//  如果错误真的非常严重，例如发生了磁盘故障，输入操作会使得流进入bad()状态。也就是假定面对这种情况你所能做的很有限，只能退出输入。
// 与istream—样，ostream也有四个状态：good()、fail()、eof()和bad()。不过，输出错误要比输入错误少得多，因此通常不对ostream进行状态检测。
// 如果程序运行环境中输出设备不可用、队列满或者发生故障的概率很高，就可以像处理输入操作那样，在每次输出操作之后都检测其状态。

// 从ist中读入整数到v中，直到遇到eof()或终结符
void fill_vector(istream& ist, vector<int>& v, char terminator) {
  for (int i; ist >> i;) v.push_back(i);
  //正常情况
  if (ist.eof()) return;  // 发现到了文件尾，正确，返回
  // 发生严重错误，只能退出函数
  if (ist.bad()) {
    // error("cin is bad!");  // error是自定义函数，它抛出异常，并给出提示信息
  }
  // 发生意外情况
  if (ist.fail()) {  // 最好清除混乱，然后汇报问题
    ist.clear();     // 清除流状态
    // 检测下一个字符是否是终结符
    char c;
    ist >> c;                        // 读入一个符号，希望是终结符
    if (c != terminator) {           // 非终结符
      ist.unget();                   // 放回该符号
      ist.clear(ios_base::failbit);  // 将流状态设置为 fail()
    }
  }
}

// 如果fill_vector()的调用者想知道是什么原因终止了输入，那么可以检测流是处于fail()还是eof()状态。当然也可以捕获error()抛出的runtime_error异常，但当istream
// 处于bad()状态时，继续获取数据是不可能的。大多数的调用者无须为此烦恼。因为这意味着，几乎在所有情况下，对于bad()状态，我们所能做的只是抛出一个异常。
// 从ist中读入整数到v中，直到遇到eof()或终结符
void fill_vector1(istream& ist, vector<int>& v, char terminator) {
  ist.exceptions(ist.exceptions() | ios_base::badbit);
  for (int i; ist >> i;) v.push_back(i);
  if (ist.eof()) return;  // 发现到了文件尾
  // 不是good()，不是bad()，不是eof()，ist的状态一定是fail()
  ist.clear();  // 清除流状态
  char c;
  ist >> c;               // 读入一个符号，希望是终结符
  if (c != terminator) {  // 不是终结符号，一定是失败了
    ist.unget();          // 也许程序调用者可以使用这个符号
    ist.clear(ios_base::failbit);  // 将流状态设置为 fail()
  }
}

int main() {
  // int i = 0;
  // cin >> i;
  // // !cin可以理解为“cin 不成功”或者“cin发生了某些错误”或者“cin的状态不是good()”，这与“操作成功”正好相反
  // if (!cin) {         // 只有输入操作失败，才会跳转到这里
  //   if (cin.bad()) {  // 流发生严重故障，只能退出函数
  //     error("cin is bad!");  // error是自定义函数，它抛出异常，并给出提示信息
  //   }
  //   if (cin.eof()) {  // 检测是否读取结束
  //     // todo
  //   }
  //   if (cin.fail()) {  // 流遇到了一些意外情况
  //     // 处理fail()时所使用的cin.clear()。当流发生错误时，我们可以进行错误恢复。为了恢复错误，我们显式地将流从fail()状态转移到其他状态，从而可以继续从
  //     // 中读取字符。clear()就起到这样的作用——执行cin.clear()后，cin的状态就变为 good()。
  //     cin.clear();  // 清除/恢复流状态
  //     // todo
  //   }
  // }
  return 0;
}