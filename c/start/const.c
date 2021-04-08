#include <stdio.h>
#include <string.h>

// 有时候我们希望定义这样一种变量，它的值不能被改变，在整个作用域中都保持固定。为了满足这一要求，可以使用const关键字对变量加以限定：
const int MaxNum = 100;
// MaxNum = 90;  // 错误，试图向const变量写入数据
// 经常将const变量称为常量（Constant）。创建常量的格式通常为：
//  const type name = value;
//  const和type都是用来修饰变量的，它们的位置可以互换，也就是将type放在const前面：
//  type const name = value;
//  但我们通常采用第一种方式，不采用第二种方式。另外建议将常量名的首字母大写，以提醒程序员这是个常量。
// 由于常量一旦被创建后其值就不能再改变，所以常量必须在定义的同时赋值（初始化），后面的任何赋值行为都将引发错误。
int getNum() { return 100; }

// const和指针
// const也可以和指针变量一起使用，这样可以限制指针变量本身，也可以限制指针指向的数据。const和指针一起使用会有几种不同的顺序，如下所示：
const int *p1;
int const *p2;
int *const p3;
// 前面两种指针所指向的数据是只读的，也就是p1、p2本身的值可以修改（指向不同的数据），但它们指向的数据不能被修改。
// 最后一种指针是只读的，也就是p3本身的值不能被修改。
// 当然，指针本身和它指向的数据都有可能是只读的，下面的两种写法能够做到这一点：
const int *const p4;
int const *const p5;
// const离变量名近就是用来修饰指针变量的，离变量名远就是用来修饰指针指向的数据，如果近的和远的都有，那么就同时修饰指针变量以及它指向的数据。

// const和函数形参
// 在C语言中，单独定义const变量没有明显的优势，完全可以使用#define命令代替。
// const通常用在函数形参中，如果形参是一个指针，为了防止在函数内部修改指针指向的数据，就可以用const来限制。
// 在C语言标准库中，有很多函数的形参都被 const 限制了，下面是部分函数的原型：
//  size_t strlen ( const char * str );
//  int strcmp ( const char * str1, const char * str2 );
//  char * strcat ( char * destination, const char * source );
//  char * strcpy ( char * destination, const char * source );
//  int system (const char* command);
//  int puts ( const char * str );
//  int printf ( const char * format, ... )
size_t strnchr(const char *str, char ch) {
  int i, n = 0, len = strlen(str);
  for (i = 0; i < len; i++) {
    if (str[i] == ch) {
      n++;
    }
  }

  return n;
}

// const和非const类型转换
// 当一个指针变量str1被const限制时，并且类似const char *str1这种形式，说明指针指向的数据不能被修改；如果将str1赋值给另外一个未被const修饰的指针变量str2，
// 就有可能发生危险。因为通过str1不能修改数据，而赋值后通过str2能够修改数据了，意义发生了转变，所以编译器不提倡这种行为，会给出错误或警告。
// 也就是说，const char *和char *是不同的类型，不能将const char *类型的数据赋值给char *类型的变量。
// 但反过来是可以的，编译器允许将char *类型的数据赋值给const char *类型的变量。
// 这种限制很容易理解，char *指向的数据有读取和写入权限，而const char *指向的数据只有读取权限，降低数据的权限不会带来任何问题，但提升数据的权限就有可能发生危险。
void func(char *str) {}

int main() {
  int n = 90;
  const int MaxNum1 = getNum();                       // 运行时初始化
  const int MaxNum2 = n;                              // 运行时初始化
  const int MaxNum3 = 80;                             // 编译时初始化
  printf("%d, %d, %d\n", MaxNum1, MaxNum2, MaxNum3);  // 100, 90, 80

  char *str = "http://c.biancheng.net";
  char ch = 't';
  int n1 = strnchr(str, ch);
  printf("%d\n", n1);  // 3

  const char *str1 = "c.biancheng.net";
  char *str2 = str1;
  func(str1);

  return 0;
}