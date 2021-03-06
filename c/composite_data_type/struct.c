#include <stdio.h>

// 在C语言中，可以使用结构体（Struct）来存放一组不同类型的数据。结构体的定义形式为：
// struct 结构体名{
//     结构体所包含的变量或数组
// };
// 结构体是一种集合，它里面包含了多个变量或数组，它们的类型可以相同，也可以不同，每个这样的变量或数组都称为结构体的成员（Member）。
// 结构体成员的定义方式与变量和数组的定义方式相同，只是不能初始化。
// 注意大括号后面的分号;不能少，这是一条完整的语句。
// 结构体也是一种数据类型，它由程序员自己定义，可以包含多个其他类型的数据。
// 像int、float、char等是由C语言本身提供的数据类型，不能再进行分拆，我们称之为基本数据类型；而结构体可以包含多个基本类型的数据，也可以包含其他的结构体，
// 将它称为复杂数据类型或构造数据类型。
// 理论上讲结构体的各个成员在内存中是连续存储的，和数组非常类似。但是在编译器的具体实现中，各个成员之间可能会存在缝隙，需要内存对齐。
// 需要注意的是，结构体是一种自定义的数据类型，是创建变量的模板，不占用内存空间；结构体变量才包含了实实在在的数据，需要内存空间来存储。

// 定义结构体变量
// 1 既然结构体是一种数据类型，那么就可以用它来定义变量：
struct stu1 {
  char *name;   // 姓名
  int num;      // 学号
  int age;      // 年龄
  char group;   // 所在学习小组
  float score;  // 成绩
};
struct stu1 stu11, stu12;  // 关键字struct不能少。
// 2 也可以在定义结构体的同时定义结构体变量：
struct stu2 {
  char *name;   // 姓名
  int num;      // 学号
  int age;      // 年龄
  char group;   // 所在学习小组
  float score;  // 成绩
} stu21, stu22;
// 3 如果只需要两个变量，后面不需要再使用结构体名定义其他变量，那么在定义时也可以不给出结构体名：
struct {        // 没有写stu3
  char *name;   // 姓名
  int num;      // 学号
  int age;      // 年龄
  char group;   // 所在学习小组
  float score;  // 成绩
} stu31, stu32;

// 成员的获取和赋值
// 结构体和数组类似，也是一组数据的集合，整体使用没有太大的意义。数组使用下标[]获取单个元素，结构体使用点号.获取单个成员。
// 通过这种方式可以获取成员的值，也可以给成员赋值。

// 所谓结构体数组，是指数组中的每个元素都是一个结构体。定义结构体数组和定义结构体变量的方式类似:
struct stu4 {
  char *name;  // 姓名
  int num;     // 学号
} class1[2] = {{"Li ping", 5}, {"Zhang ping", 4}};
// 当对数组中全部元素赋值时，也可不给出数组长度
struct stu5 {
  char *name;  // 姓名
  int num;     // 学号
} class2[] = {{"Li ping", 5}, {"Zhang ping", 4}};

// 当一个指针变量指向结构体时，我们就称它为结构体指针。
struct stu6 {
  char *name;  // 姓名
  int num;     // 学号
} stu6 = {"Tom", 12};
struct stu6 *pstu6 = &stu6;
// 也可以在定义结构体的同时定义结构体指针：
struct stu7 {
  char *name;  // 姓名
  int num;     // 学号
} stu7 = {"Tom", 12}, *pstu7 = &stu7;
// 注意，结构体变量名和数组名不同，数组名在表达式中会被转换为数组指针，而结构体变量名不会，无论在任何表达式中它表示的都是整个集合本身，要想取得结构体变量
// 的地址，必须在前面加&，所以给pstu赋值只能写作：
//  struct stu *pstu = &stu1;
// 而不能写作：
//  struct stu *pstu = stu1;
// 结构体是一种数据类型，是创建变量的模板，不会分配内存空间，就像int、float、char这些关键字本身不占用内存一样；结构体变量才包含数据，才需要内存来存储。
// 下面的写法是错误的，不可能去取一个结构体名的地址，也不能将它赋值给其他变量：
// struct stu *pstu = &stu;
// struct stu *pstu = stu;
// 通过结构体指针可以获取结构体成员，一般形式为：
//  (*pointer).memberName
// 或者：
//  pointer->memberName
// 第一种写法中，.的优先级高于*，(*pointer)两边的括号不能少。如果去掉括号写作*pointer.memberName，那么就等效于*(pointer.memberName)，这样意义就完全不对了。
// 第二种写法中，->是一个新的运算符，习惯称它为“箭头”，有了它，可以通过结构体指针直接取得结构体成员；这也是->在C语言中的唯一用途。
// 上面的两种写法是等效的，通常采用后面的写法，这样更加直观。

// 结构体指针作为函数参数
// 结构体变量名代表的是整个集合本身，作为函数参数时传递的整个集合，也就是所有成员，而不是像数组一样被编译器转换成一个指针。如果结构体成员较多，尤其是成员为数组时，
// 传送的时间和空间开销会很大，影响程序的运行效率。所以最好的办法就是使用结构体指针，这时由实参传向形参的只是一个地址，非常快速。

int main() { return 0; }