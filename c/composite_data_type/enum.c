#include <stdio.h>

// 在实际编程中，有些数据的取值往往是有限的，只能是非常少量的整数，
// 并且最好为每个值都取一个名字，以方便在后续代码中使用。
// #define命令虽然能解决问题，但也带来了不小的副作用，
// 导致宏名过多，代码松散，看起来总不舒服。
// C语言提供了枚举（Enum）类型，能列出所有可能值，并给它们取一个名字。
// 枚举类型的定义形式为：
// enum typeName{ valueName1, valueName2, valueName3, ...... };
// enum是一个新的关键字，专门用来定义枚举类型，这也是它在C语言中的唯一用途；
// typeName是枚举类型的名字；
// valueName1, valueName2, valueName3, ......是每个值对应的名字的列表。
// 注意最后的;不能少。

// 定义枚举和枚举变量
// 枚举值默认从0开始，往后逐个加1（递增）；
// enum week1 {Mon, Tues, Wed, Thurs, Fri, Sat, Sun };
// 可以给每个名字都指定一个值：
// enum week2 {Mon = 1, Tues = 2, Wed = 3, Thurs = 4,
//             Fri = 5, Sat = 6, Sun = 7 };
// 更为简单的方法（跟上面的写法是等效的）是只给第一个名字指定值，后面递增：
// enum week3 {Mon = 1, Tues, Wed, Thurs, Fri, Sat, Sun };
// 枚举是一种类型，通过它可以定义枚举变量：
// enum week3 a1, b1, c1;
// 也可以在定义枚举类型的同时定义变量：
// enum week4 {Mon = 1,Tues,Wed,Thurs,Fri,Sat,Sun} a2, b2, c2;
// 有了枚举变量，就可以把列表中的值赋给它：
// enum week3 a3 = Mon, b3 = Wed, c3 = Sat;
// enum week5{Mon=1,Tues,Wed,Thurs,Fri,Sat,Sun }a=Mon,b=Wed,c=Sat;

// 需要注意的两点是：
// 1.枚举列表中的Mon、Tues、Wed这些标识符的作用范围是全局的，
//   严格来说是main()函数内部，不能再定义与它们名字相同的变量。
// 2.Mon、Tues、Wed等都是常量，不能对它们赋值，
//   只能将它们的值赋给其他的变量。
// 枚举和宏其实非常类似：
// 宏在预处理阶段将名字替换成对应的值，枚举在编译阶段将名字替换成对应的值。
// 可以将枚举理解为编译阶段的宏。
// case关键字后面必须是一个整数，或者是结果为整数的表达式，
// 但不能包含任何变量，正是由于Mon、Tues这些名字最终会被替换成一个整数，
// 所以它们才能放在case后面。

int main() {
  // 枚举类型变量需要存放的是一个整数，它的长度和int应该相同：
  enum week { Mon = 1, Tues, Wed, Thurs, Fri, Sat, Sun } day = Mon;
  printf("%d, %d, %d, %d, %d\n", sizeof(enum week), sizeof(day), sizeof(Mon),
         sizeof(Wed), sizeof(int));
  // 4, 4, 4, 4, 4

  return 0;
}