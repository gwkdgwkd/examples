#include <stdio.h>

// C语言允许为一个数据类型起一个新的别名，就像给人起“绰号”一样。起别名的目的不是为了提高程序运行效率，而是为了编码方便。
// typedef的用法一般为：typedef oldName newName;
// 需要强调的是，typedef是赋予现有类型一个新的名字，而不是创建新的类型。为了“见名知意”，请尽量使用含义明确的标识符，并且尽量大写。
// typedef还可以给数组、指针、结构体等类型定义别名。
//  typedef char ARRAY20[20];
//  char a1[20], a2[20], s1[20], s2[20];
//  typedef struct stu {
//    char name[20];
//    int age;
//    char sex;
//  } STU;
//  STU body1, body2;
//  struct stu body3, body4;
//  typedef int (*PTR_TO_ARR)[4];
//  PTR_TO_ARR p1, p2;
//  typedef int (*PTR_TO_FUNC)(int, int);
//  PTR_TO_FUNC pfunc;

// typedef和#define的区别
// typedef在表现上有时候类似于#define，但它和宏替换之间存在一个关键性的区别。正确思考这个问题的方法就是把typedef看成一种彻底的“封装”类型，声明之后不能再往里面增加别的东西。
// 1) 可以使用其他类型说明符对宏类型名进行扩展，但对typedef所定义的类型名却不能这样做。如下所示：
//  #define INTERGE int
//  unsigned INTERGE n;  // 没问题
//  typedef int INTERGE;
//  unsigned INTERGE n;  // 错误，不能在INTERGE前面添加unsigned
// 2) 在连续定义几个变量的时候，typedef能够保证定义的所有变量均为同一类型，而#define则无法保证。例如：
//  #define PTR_INT int *
//  PTR_INT p1, p2;
//  经过宏替换以后，第二行变为：
//  int *p1, p2;
//  这使得p1、p2成为不同的类型：p1是指向int类型的指针，p2是int类型。
//  相反，在下面的代码中：
//  typedef int * PTR_INT
//  PTR_INT p1, p2;
//  p1、p2类型相同，它们都是指向int类型的指针。

typedef char (*PTR_TO_ARR)[30];
typedef int (*PTR_TO_FUNC)(int, int);
int max(int a, int b) { return a > b ? a : b; }
char str[3][30] = {"http://c.biancheng.net", "C语言中文网", "C-Language"};

int main() {
  PTR_TO_ARR parr = str;
  PTR_TO_FUNC pfunc = max;
  int i;
  printf("max: %d\n", (*pfunc)(10, 20));  // max: 20
  for (i = 0; i < 3; i++) {
    printf("str[%d]: %s\n", i, *(parr + i));
  }
  // str[0]: http://c.biancheng.net
  // str[1]: C语言中文网
  // str[2]: C-Language

  return 0;
}