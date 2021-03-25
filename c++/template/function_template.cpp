#include <iostream>

using namespace std;

// 为了交换不同类型的变量的值，我们通过函数重载定义了四个名字相同、参数列表不同的函数。这些函数虽然在调用时方便了一些，但从本质上说还是定义了三个功能相同、函数体相
// 同的函数，只是数据的类型不同而已，这看起来有点浪费代码，能不能把它们压缩成一个函数呢？能，使用模板。
// 数据的值可以通过函数参数传递，在函数定义时数据的值是未知的，只有等到函数调用时接收了实参才能确定其值。这就是值的参数化。
// 在C++中，数据的类型也可以通过参数来传递，在函数定义时可以不指明具体的数据类型，当发生函数调用时，编译器可以根据传入的实参自动推断数据类型。这就是类型的参数化。
// 值（Value）和类型（Type）是数据的两个主要特征，它们在C++中都可以被参数化。
// 所谓函数模板，实际上是建立一个通用函数，它所用到的数据的类型（包括返回值类型、形参类型、局部变量类型）可以不具体指定，而是用一个虚拟的类型来代替（实际上是用一个标
// 识符来占位），等发生函数调用时再根据传入的实参来逆推出真正的类型。这个通用函数就称为函数模板（Function Template）。
// 在函数模板中，数据的值和类型都被参数化了，发生函数调用时编译器会根据传入的实参来推演形参的值和类型。换个角度说，函数模板除了支持值的参数化，还支持类型的参数化。
// 一但定义了函数模板，就可以将类型参数用于函数定义和函数声明了。说得直白一点，原来使用int、float、char等内置类型的地方，都可以用类型参数来代替。
// 定义了函数模板后，就可以像调用普通函数一样来调用它们了。
void Swap(int *a, int *b) {
  int temp = *a;
  *a = *b;
  *b = temp;
}
void Swap(float *a, float *b) {
  float temp = *a;
  *a = *b;
  *b = temp;
}
void Swap(char *a, char *b) {
  char temp = *a;
  *a = *b;
  *b = temp;
}
void Swap(bool *a, bool *b) {
  char temp = *a;
  *a = *b;
  *b = temp;
}

// template <typename 类型参数1, typename 类型参数2 , ...> 返回值类型  函数名(形参列表){
//   在函数体中可以使用类型参数
// }
// 类型参数可以有多个，它们之间以逗号,分隔。类型参数列表以<>包围，形式参数列表以()包围。
// 类型参数的命名规则跟其他标识符的命名规则一样，不过使用T、T1、T2、Type等已经成为了一种惯例。
// template是定义函数模板的关键字，它后面紧跟尖括号<>，尖括号包围的是类型参数（也可以说是虚拟的类型，或者说是类型占位符）。
// typename是另外一个关键字，用来声明具体的类型参数，这里的类型参数就是T。从整体上看，template<typename T>被称为模板头。
// 模板头中包含的类型参数可以用在函数定义的各个位置，包括返回值、形参列表和函数体；
// typename关键字也可以使用class关键字替代，它们没有任何区别。C++早期对模板的支持并不严谨，没有引入新的关键字，而是用class来指明类型参数，
// 但是class关键字本来已经用在类的定义中了，这样做显得不太友好，所以后来C++又引入了一个新的关键字typename，专门用来定义类型参数。
// 不过至今仍然有很多代码在使用class关键字，包括C++标准库、一些开源程序等。
template <typename T>
void SwapT(T *a, T *b) {
  T temp = *a;
  *a = *b;
  *b = temp;
}
template <typename T>
void SwapR(T &a, T &b) {
  T temp = a;
  a = b;
  b = temp;
}

// 函数模板也可以提前声明，不过声明时需要带上模板头，并且模板头和函数定义（声明）是一个不可分割的整体，它们可以换行，但中间不能有分号。
template <typename T>
T max(T a, T b, T c);

// 当需要对不同的类型使用同一种算法（同一个函数体）时，为了避免定义多个功能重复的函数，可以使用模板。
// 然而，并非所有的类型都使用同一种算法，有些特定的类型需要单独处理，为了满足这种需求，C++允许对函数模板进行重载，程序员可以像重载常规函数那样重载模板定义。
template <class T>
void Swap(T &a, T &b);  // 模板①：交换基本类型的值
// 在该函数模板中，最后一个参数的类型为具体类型（int），而不是泛型。并不是所有的模板参数都必须被泛型化。
template <typename T>
void Swap(T a[], T b[], int len);     // 模板②：交换两个数组
void printArray(int arr[], int len);  // 打印数组元素

// 在使用类模板创建对象时，程序员需要显式的指明实参（也就是具体的类型）:template<typename T1, typename T2> class Point;
// 可以在栈上创建对象，也可以在堆上创建对象：
//  Point<int, int> p1(10, 20);  // 在栈上创建对象
//  Point<char*, char*> *p = new Point<char*, char*>("东京180度", "北纬210度");  // 在堆上创建对象
// 因为已经显式地指明了T1、T2的具体类型，所以编译器就不用再自己推断了，直接拿来使用即可。
// 而对于函数模板，调用函数时可以不显式地指明实参（也就是具体的类型）。这种通过函数实参来确定模板实参（也就是类型参数的具体类型）的过程称为模板实参推断。
// 在模板实参推断过程中，编译器使用函数调用中的实参类型来寻找类型参数的具体类型。
// 对于普通函数（非模板函数），发生函数调用时会对实参的类型进行适当的转换，以适应形参的类型。这些转换包括：
//  算数转换：例如int转换为float，char转换为int，double转换为 int等。
//  派生类向基类的转换：也就是向上转型
//  const转换：也即将非const类型转换为const类型，例如将char *转换为const char *。
//  数组或函数指针转换：如果函数形参不是引用类型，那么数组名会转换为数组指针，函数名也会转换为函数指针。
//  用户自定的类型转换。
// void func1(int n, float f);
// void func2(int *arr, const char *str);
// int nums[5];
// char *url = "http://c.biancheng.net";
// func1(12.5, 45);  // 12.5会从double转换为int，45会从int转换为float
// func2(nums, url);  // nums会从int [5]转换为int *，url会从char *转换为const char *
// 而对于函数模板，类型转换则受到了更多的限制，仅能进行「const 转换」和「数组或函数指针转换」，其他的都不能应用于函数模板。
template <typename T>
void func1(T a, T b) {}
template <typename T>
void func2(T *buffer) {}
template <typename T>
void func3(const T &stu) {}
template <typename T>
void func4(T a) {}
// 当函数形参是引用类型时，数组不会转换为指针。
template <typename T>
void func5(T &a) {}
struct S {};
template <typename T>
void func6(T &a, T &b);

// 函数模板的实参推断是指「在函数调用过程中根据实参的类型来寻找类型参数的具体类型」的过程，这在大部分情况下是奏效的，但是当类型参数的个数较多时，
// 就会有个别的类型无法推断出来，这个时候就必须显式地指明实参。
// 「为函数模板显式地指明实参」和「为类模板显式地指明实参」的形式是类似的，就是在函数名后面添加尖括号<>，里面包含具体的类型。
// 显式指明的模板实参会按照从左到右的顺序与对应的模板参数匹配：第一个实参与第一个模板参数匹配，第二个实参与第二个模板参数匹配，以此类推。
// 只有尾部（最右）的类型参数的实参可以省略，而且前提是它们可以从传递给函数的实参中推断出来。
template <typename T1, typename T2>
void func7(T1 a) {
  T2 b;
}
// 对于上面的func7()，虽然只有T2的类型不能自动推断出来，但是由于它位于类型参数列表的尾部（最右），所以必须同时指明T1和T2的类型。
template <typename T1, typename T2>
void func8(T2 a) {
  T1 b;
}
// 由于T2的类型能够自动推断出来，并且它位于参数列表的尾部（最右），所以可以省略。

// 显式地指明实参时可以应用正常的类型转换
// 函数模板仅能进行「const 转换」和「数组或函数指针转换」两种形式的类型转换，但是当我们显式地指明类型参数的实参（具体类型）时，就可以使用正常的类型转
// 换（非模板函数可以使用的类型转换）了。
template <typename T>
void func9(T a, T b) {}

int main() {
  int n1 = 100, n2 = 200;
  SwapT(&n1, &n2);
  cout << n1 << ", " << n2 << endl;  // 200, 100
  SwapR(n1, n2);
  cout << n1 << ", " << n2 << endl;  // 100, 200
  float f1 = 12.5, f2 = 56.93;
  SwapT(&f1, &f2);
  cout << f1 << ", " << f2 << endl;  // 56.93, 12.5
  SwapR(f1, f2);
  cout << f1 << ", " << f2 << endl;  // 12.5, 56.93
  char c1 = 'A', c2 = 'B';
  SwapT(&c1, &c2);
  cout << c1 << ", " << c2 << endl;  // B, A
  SwapR(c1, c2);
  cout << c1 << ", " << c2 << endl;  // A, B
  bool b1 = false, b2 = true;
  SwapT(&b1, &b2);
  cout << b1 << ", " << b2 << endl;  // 1, 0
  SwapR(b1, b2);
  cout << b1 << ", " << b2 << endl;  // 0, 1

  cout << "i_max=" << max(1, 3, 4) << endl;        // 4
  cout << "d_max=" << max(1.5, 3.5, 4.5) << endl;  // 4.5
  cout << "g_max=" << max(1, 3, 4) << endl;        // 4

  int m = 10, n = 99;
  Swap(m, n);
  cout << m << ", " << n << endl;  // 99, 10
  int a[5] = {1, 2, 3, 4, 5};
  int b[5] = {10, 20, 30, 40, 50};
  int len = sizeof(a) / sizeof(int);
  Swap(a, b, len);
  printArray(a, len);  // 10, 20, 30, 40, 50
  printArray(b, len);  // 1, 2, 3, 4, 5

  int name[20];
  S stu1;
  // 12.5的类型为double，30的类型为int，编译器不知道该将T实例化为double还是int，也不会尝试将int转换为double，或者将double转换为int，所以调用失败。
  // func1(12.5, 30);
  func2(name);  // name的类型从int[20]换转换为int *，所以T的真实类型为int
  func3(stu1);  // 非const转换为const，T的真实类型为S
  func4(name);  // name的类型从int[20]换转换为int *，所以T的真实类型为int *
  func5(name);  // name依然为int[20]，不会转换为int *，所以T的真实类型为int[20]
  // name它本来的类型是int[20]：
  // 对于func2(name)和func4(name)，name的类型会从int[20]转换为int *，也即将数组转换为指针，所以T的类型分别为int *和int。
  // 对于func5(name)，name的类型依然为int[20]，不会转换为int *，所以T的类型为int[20]。

  int str1[20];
  int str2[10];
  // 由于str1、str2的类型分别为int[20]和int[10]，在函数调用过程中又不会转换为指针，所以编译器不知道应该将T实例化为int[20]还是int[10]，导致调用失败。
  // func6(str1, str2);

  // func(10);
  // func()有两个类型参数，分别是T1和T2，但是编译器只能从函数调用中推断出T1的类型来，不能推断出T2的类型来，所以这种调用是失败的，必须显式地指明T1、T2的具体类型。
  func7<int, int>(10);

  func8<int>(10);       // 省略T2的类型
  func8<int, int>(20);  // 指明T1、T2的类型

  // func9(10, 23.5);  // Error
  // 已经显式地指明了T的类型为float，编译器不会再为「T的类型到底是int还是double」而纠结了，所以可以从容地使用正常的类型转换了。
  func9<float>(20, 93.7);  // Correct

  return 0;
}

// 定义函数模板
template <typename T>   // 模板头，这里不能有分号
T max(T a, T b, T c) {  // 函数头
  T max_num = a;
  if (b > max_num) max_num = b;
  if (c > max_num) max_num = c;
  return max_num;
}

template <class T>
void Swap(T &a, T &b) {
  T temp = a;
  a = b;
  b = temp;
}
template <typename T>
void Swap(T a[], T b[], int len) {
  T temp;
  for (int i = 0; i < len; i++) {
    temp = a[i];
    a[i] = b[i];
    b[i] = temp;
  }
}
void printArray(int arr[], int len) {
  for (int i = 0; i < len; i++) {
    if (i == len - 1) {
      cout << arr[i] << endl;
    } else {
      cout << arr[i] << ", ";
    }
  }
}