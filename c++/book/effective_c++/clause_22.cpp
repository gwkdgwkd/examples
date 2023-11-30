#include <iostream>
#include <string>

// 将成员变量声明为private

// 为什么不采用public成员变量？
// 1.从语法一致性角度看，如果成员变量不是public，客户唯一能访问的办法就是通过成员函数。
//   如果public内都是函数，使用者不用迷惑地记住是否使用小括号，都是函数，直接使用就行。
// 2.使用函数可以对成员变量的处理有更精确的控制，如果令成员变量为public，谁都可以读写它；
//   但如果使用函数，可以实现出不准访问、只读访问以及读写访问，甚至可以实现唯写访问。
//   如此细微地划分访问控制颇有必要，因为许多成员变量应该被隐藏起来。
//   每个成员变量都提供一个getter和setter函数比较罕见。
class AccessLevels {
 public:
  int getReadOnly() const { return readOnly; }
  void setReadWrite(int value) { readWrite = value; }
  int getReadWrite() const { return readWrite; }
  void setWriteOnly(int value) { writeOnly = value; }

 private:
  int noAccess;
  int readOnly;
  int readWrite;
  int writeOnly;
};
// 3.封装，如果通过函数访问成员变量，以后可以用某个计算替换这个成员变量，
//   而class客户一点也不会知道class内部实现已经起了变化。
// 4.将成员变量隐藏在函数接口的背后，可以为所有可能的实现提供弹性。
//   可使得成成员变量被读或被写时轻松通知其他对象；
//   可以验证class的约束条件以及函数的前提和事后状态；
//   可以在多线程环境中执行同步控制等等。

// public意味不封装，不封装几乎可以说不可改变，
// 被广泛使用的class是最需要封装的，因为可以从改用一个较好实现版本中获益。
// protected成员变量和public成员变量的论点相同。
// 语法一致性和细微划分访问控制对protected一样适用，封装性并不比public高。

// 一旦将变量声明为public或protected而客户开始适用它，就很难改变成员变量所涉及的一切。
// 太多代码需要重写、重新测试、重写编写文档、重新编译。
// 从封装的角度看，其实只有两种访问权限：private（提供封装）和其他（不提供封装）。

// 请记住：
// 1.切记将成员变量声明为private；
// 2.这可赋予客户访问数据的一致性、可细微划分访问控制、允诺约束条件获得保证，
//   并提供class作者充分的弹性；
// 3.protected并不比public更具封装性。

int main() { return 0; }