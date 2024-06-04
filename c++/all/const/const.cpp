#include <cstdio>

// 在C语言中，const用来限制一个变量，表示这个变量不能被修改，
// 通常称这样的变量为常量（Constant）。
// 在C++中，const含义没变，只对细节作了一些调整，以下是最主要的两点：
// 1.C++中的const更像编译阶段的#define；
// 2.C++中全局const变量的可见范围是当前文件。

const int m = 10;
int n = m;
// 变量是要占用内存的，即使被const修饰也不例外。
// m、n两个变量占用不同的内存，int n = m;
// 表示将m的值赋给n，这个赋值的过程在C和C++中是有区别的。
// 在C语言中，编译器会先到m所在的内存取出一份数据，再将这份数据赋给n；
// 而在C++中，会直接将10赋给n，没有读内存的过程，和int n=10;的效果一样。
// C++中的常量更类似于#define命令，是一个值替换的过程，
// 只不过#define是在预处理阶段替换，而常量是在编译阶段替换。
// C++对const的处理少了读取内存的过程，优点是提高了程序执行效率，
// 缺点是不能反映内存的变化，一旦const变量被修改，C++就不能取得最新的值。
// const变量不是禁止被修改吗？
// 对，这种说法没错，不过这只是语法层面上的限制，通过指针仍然可以修改。
// C语言对const的处理和普通变量一样，会到内存中读取数据；
// C++对const的处理更像是编译时期的#define，是一个值替换的过程。

// 普通全局变量的作用域是当前文件，但在其他文件也可见，加extern后就可以使用。
// 在C语言中，const变量和普通变量一样，在其他源文件中也是可见的。
// C语言中的const变量在多文件编程时和普通变量一样，除了不能修改，没其他区别。
// C++对const的特性做了调整，C++规定，全局const变量的作用域仍然是当前文件，
// 但是它在其他文件中是不可见的，这和添加了static关键字的效果类似。
// C和C++中全局const变量的作用域相同，都是当前文件，不同的是它们的可见范围：
// 1.C语言中const全局变量的可见范围是整个程序，其他文件加extern后就可用；
// 2.而C++中const全局变量的可见范围仅限于当前文件，在其他文件中不可见，
//   所以可以定义在头文件中，多次引入后也不会出错。
// 如果是GCC，那么可以添加extern关键字来增大C++全局const变量的可见范围。
// extern const int n = 10;
// 这样n的可见范围就变成了整个程序，在其他文件中使用extern声明后就可以使用了，
// 不过这种方式只适用于GCC，不适用于VS/VC。

// C++中的const变量虽然也会占用内存，也能使用&获取得它的地址，
// 但使用却更像编译期的#define，#define也是值替换，可见范围也仅限于当前文件。
// 很多C++教程在对比const和#define的优缺点时提到，
// #define定义的常量仅仅是字符串的替换，不会进行类型检查，
// 而const定义的常量是有类型的，编译器会进行类型检查，
// 相对来说比#define更安全，所以鼓励大家使用const代替#define。
// 这种说法虽然没错，但是没有发现不进行类型检查有什么隐患，
// 在使用#define的过程中也没有发现什么缺点。

int main() {
  // 将代码放到.c文件中，以C语言的方式编译，运行结果为99；
  // 再将代码放到.cpp文件中，以C++的方式编译，运行结果就变成了10。
  // 这种差异正是由于C和C++对const的处理方式不同造成的。
  const int n = 10;
  // &n是const int*，必须强制转换为int*后才能赋给p，否则类型是不兼容的。
  int *p = (int *)&n;
  *p = 99;
  printf("%d\n", n);
  // c++ : 10
  // c : 99

  // 在C语言中，使用printf输出n时会到内存中获取n的值，
  // 这个时候n所在内存中的数据已经被修改成了99，所以输出结果也是99。
  // 而在C++中，printf("%d\n",n);效果和printf("%d\n",10);一样，
  // 在编译时就将n的值替换成了10，不管n的内存如何变化，都不会影响输出结果。

  return 0;
}