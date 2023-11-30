#include <iostream>

namespace n1 {
// 为了交换不同类型的值，通过函数重载定义了四个名字相同、参数不同的函数：
void Swap(int &a, int &b) {
  int temp = a;
  a = b;
  b = temp;
}
void Swap(float &a, float &b) {
  float temp = a;
  a = b;
  b = temp;
}
void Swap(long &a, long &b) {
  long temp = a;
  a = b;
  b = temp;
}

void func() {
  int i1 = 1, i2 = 2;
  Swap(i1, i2);
  std::cout << i1 << "," << i2 << std::endl;  // 2,1

  float f1 = 1.0f, f2 = 2.0f;
  Swap(f1, f2);
  std::cout << f1 << "," << f2 << std::endl;  // 2,1

  long l1 = 1l, l2 = 2l;
  Swap(l1, l2);
  std::cout << l1 << "," << l2 << std::endl;  // 2,1
}
// 这些函数虽然在调用时方便，但从本质上是定义了三个功能相同、函数体相同的函数，
// 只是数据的类型不同而已，这看起来有点浪费代码，能不能把它们压缩成一个函数呢？
// 能，使用模板。
}  // namespace n1

namespace n2 {
// 数据的值可以通过函数参数传递，在函数定义时数据的值是未知的，
// 只有到函数调用时接收了实参才能确定值，这就是值的参数化。
// 在C++中，数据的类型也可以通过参数来传递，
// 在函数定义时可以不指明具体的数据类型，当发生函数调用时，
// 编译器可以根据传入的实参自动推断数据类型，这就是类型的参数化。
// 值和类型是数据的两个主要特征，它们在C++中都可以被参数化。

// 所谓函数模板，实际上是建立一个通用函数，它所用到的数据的类型，
// 包括返回值类型、形参类型、局部变量类型可以不具体指定，
// 而是用一个虚拟的类型来代替，实际上是用一个标识符来占位，
// 等发生函数调用时再根据传入的实参来逆推出真正的类型。
// 这个通用函数就称为函数模板Function Template。
// 在函数模板中，数据的值和类型都被参数化了，
// 发生函数调用时编译器会根据传入的实参来推演形参的值和类型。
// 换个角度说，函数模板除了支持值的参数化，还支持类型的参数化。
// 一但定义了函数模板，就可以将类型参数用于函数定义和函数声明了。
// 定义了函数模板后，就可以像调用普通函数一样来调用它们了。

// 类型参数可以有多个，它们之间以逗号,分隔。
// 类型参数列表以<>包围，形式参数列表以()包围。
// 类型参数的命名规则跟其他标识符的命名规则一样，
// 不过使用T、T1、T2、Type等已经成为了一种惯例。
// template是定义函数模板的关键字，它后面紧跟尖括号<>，
// 尖括号包围的是类型参数，也可以说是虚拟的类型，或者说是类型占位符。
// typename是另外一个关键字，用来声明具体的类型参数，这里的类型参数就是T。
// 模板头中包含的类型参数可以用在函数定义的各个位置，
// 包括返回值、形参列表和函数体。
template <typename T>    // 模板头
void Swap(T *a, T *b) {  // 函数头
  T temp = *a;
  *a = *b;
  *b = temp;
}

// 函数模板也可以提前声明，不过声明时需要带上模板头：
template <typename T>
void Swap(T &a, T &b);

// C++早期对模板的支持并不严谨，没有引入新的关键字，
// 而是用class来指明类型参数，但是class关键字本来已经用在类的定义中了，
// 这样不太友好，所以后来C++又引入了一个新的关键字typename，专门定义类型参数。
// 不过至今仍然有很多代码在使用class关键字，包括C++标准库、一些开源程序等。
template <class T>  // typename可以使用class替代，它们没有任何区别
void Swap(T &a, T &b) {
  T temp = a;
  a = b;
  b = temp;
}

// 当需要对不同的类型使用同一种算法（同一个函数体）时，
// 为了避免定义多个功能重复的函数，可以使用模板。
// 然而，并非所有的类型都使用同一种算法，有些特定的类型需要单独处理，
// 为了满足这种需求，C++允许对函数模板进行重载，
// 程序员可以像重载常规函数那样重载模板定义。
template <typename T>
// 模板头和函数定义（声明）是一个不可分割的整体，可以换行，但中间不能有分号。
// 最后一个参数为int，而不是泛型，并不是所有的模板参数都必须被泛型化。
// 模板头和函数头可以合并到一行，不过为了让代码格式更加清晰，一般是分成两行。
void Swap(T a[], T b[], int len) {
  T temp;
  for (int i = 0; i < len; i++) {
    temp = a[i];
    a[i] = b[i];
    b[i] = temp;
  }
}

void func1() {
  int a = 4, b = 5;
  std::cout << a << "," << b << std::endl;  // 4,5
  Swap(&a, &b);
  std::cout << a << "," << b << std::endl;  // 5,4
  Swap(a, b);
  std::cout << a << "," << b << std::endl;  // 4,5
}
void func2() {
  double a = 4.1, b = 5.2;
  std::cout << a << "," << b << std::endl;  // 4.1,5.2
  Swap(&a, &b);
  std::cout << a << "," << b << std::endl;  // 5.2,4.1
  Swap(a, b);
  std::cout << a << "," << b << std::endl;  // 4.1,5.2
}
void func3() {
  bool a = true, b = false;
  std::cout.setf(std::ios::boolalpha);
  std::cout << a << "," << b << std::endl;  // true,false
  Swap(&a, &b);
  std::cout << a << "," << b << std::endl;  // false,true
  Swap(a, b);
  std::cout << a << "," << b << std::endl;  // true,false
}
void func4() {
  char a = 'a', b = 'b';
  std::cout << a << "," << b << std::endl;  // a,b
  Swap(&a, &b);
  std::cout << a << "," << b << std::endl;  // b,a
  Swap(a, b);
  std::cout << a << "," << b << std::endl;  // a,b
}
void func5() {
  auto print = [](int *arr, int len) {
    for (int i = 0; i < len; ++i) {
      std::cout << arr[i];
      if (i < len - 1) std::cout << ",";
    }
    std::cout << std::endl;
  };
  int a1[4] = {1, 2, 3, 4};
  int a2[4] = {5, 6, 7, 8};
  print(a1, sizeof(a1) / sizeof(int));  // 1,2,3,4
  print(a2, sizeof(a2) / sizeof(int));  // 5,6,7,8
  Swap(a1, a2, 4);
  print(a1, sizeof(a1) / sizeof(int));  // 5,6,7,8
  print(a2, sizeof(a2) / sizeof(int));  // 1,2,3,4
}

void func() {
  func1();
  func2();
  func3();
  func4();
  func5();
}
}  // namespace n2

namespace n3 {
namespace test1 {
template <typename T>
void fun(T a, T b) {
  std::cout << "fun" << std::endl;
}
void func() {
  fun<int>(5, 6);  // fun

  // 在使用类模板创建对象时，需要显式的指明实参，可以在栈上和堆上创建对象。
  // 因为已经显式地指明了T1、T2的具体类型，所以编译器就不用再推断了，直接使用即可。
  // 而对于函数模板，调用函数时可以不显式地指明实参，也就是具体的类型。
  // 这种通过函数实参来确定模板实参（类型参数的具体类型）的过程称为模板实参推断。
  // 在模板实参推断过程中，编译器使用函数调用中的实参类型来寻找类型参数的具体类型。
  fun(5, 6);  // fun
}
}  // namespace test1

namespace test2 {
// 对于普通函数（非模板函数），发生函数调用时会对实参的类型进行适当的转换，
// 以适应形参的类型，这些转换包括：
// 1.算数转换：例如int转换为float，char转换为int，double转换为int等；
// 2.派生类向基类的转换：也就是向上转型；
// 3.const转换：将非const类型转换为const类型，例如char*转换为const char*。
// 4.数组或函数指针转换：如果函数形参不是引用类型，
//   那么数组名会转换为数组指针，函数名也会转换为函数指针；
// 5.用户自定的类型转换。
void func1(int n, float f) {
  std::cout << "void func1(int n, float f)" << std::endl;
}
void func2(int *arr, const char *str) {
  std::cout << "void func2(int *arr, const char *str)" << std::endl;
}
void func() {
  // 12.5从double转换为int，45从int转换为float：
  func1(12.5, 45);  // void func1(int n, float f)

  int nums[5];
  char *url = "nihao";
  // nums从int[5]转换为int*，url从char*转换为const char*：
  func2(nums, url);  // void func2(int *arr, const char *str)
}
}  // namespace test2

namespace test3 {
// 而对于函数模板，类型转换则受到了更多的限制，
// 仅能进行const转换和数组或函数指针转换，其他的都不能应用于函数模板：
template <typename T>
void fun1(T *buffer) {
  std::cout << "void fun1(T *buffer) : " << typeid(T).name() << std::endl;
}
template <typename T>
void fun2(const T &) {
  std::cout << "void fun2(const T &) : " << typeid(T).name() << std::endl;
}
template <typename T>
void fun3(T a) {
  std::cout << "void fun3(T a) : " << typeid(T).name() << std::endl;
}
template <typename T>
void fun4(T &a) {  // 当函数形参是引用类型时，数组不会转换为指针
  std::cout << "void fun4(T &a) : " << typeid(T).name() << std::endl;
}
template <typename T>
void fun5(T a, T b) {
  std::cout << "void fun5(T a, T b) : " << typeid(T).name() << std::endl;
}
template <typename T>
void fun6(T &a, T &b) {
  std::cout << "void fun6(T &a, T &b) : " << typeid(T).name() << std::endl;
}
template <typename T>
void fun7(T *a, T *b) {
  std::cout << "void fun7(T *a, T *b) : " << typeid(T).name() << std::endl;
}
template <typename T>
void fun8(T a, T b) {
  std::cout << "void fun8(T a, T b) : " << typeid(T).name() << std::endl;
}
void func() {
  // name的类型从int[20]换转换为int*，所以T的真实类型为int：
  int name[20];
  fun1(name);  // void fun1(T *buffer) : i

  // 非const转换为const，T的真实类型为int：
  int i;
  fun2(i);  // void fun2(const T &) : i

  // name的类型从int[20]换转换为int*，所以T的真实类型为int*：
  fun3(name);  // void fun3(T a) : Pi

  // name依然为int[20]，不会转换为int*，所以T的真实类型为int[20]：
  fun4(name);  // void fun4(T &a) : A20_i

  // 12.5为double，30为int，编译器不知道该将T实例化为double还是int，
  // 也不会尝试将int转换为double，或者将double转换为int，所以调用失败：
  // fun5(12.5, 30);

  int a1[20];
  int a2[10];
  // 由于引用类型在函数调用过程中不会转换为指针，
  // 所以编译器不知道应该将T实例化为int[20]还是int[10]，导致调用失败：
  // fun6(a1, a2);
  int a3[10];
  fun6(a2, a3);  // void fun6(T &a, T &b) : A10_i

  fun7(a1, a2);  // void fun7(T *a, T *b) : i
  fun8(a1, a2);  // void fun8(T a, T b) : Pi
}
}  // namespace test3
}  // namespace n3

namespace n4 {
// 函数模板的实参推断是指在调用时，根据实参类型寻找类型参数具体类型的过程，
// 这在大部分情况下是奏效的，但是当类型参数的个数较多时，
// 就会有个别的类型无法推断出来，这个时候就必须显式地指明实参。
// 为函数模板显式地指明实参和为类模板显式地指明实参的形式是类似的，
// 就是在函数名后面添加尖括号<>，里面包含具体的类型。
// 显式指明的模板实参会按照从左到右的顺序与对应的模板参数匹配。
// 只有尾部（最右）的类型参数的实参可以省略，
// 而且前提是它们可以从传递给函数的实参中推断出来。
template <typename T1, typename T2>
void fun1(T1 a) {
  std::cout << "void fun1(T1 a) : " << typeid(T1).name() << ","
            << typeid(T2).name() << std::endl;
  T2 b;
}
template <typename T1, typename T2>
void fun2(T2 a) {
  std::cout << "void fun2(T2 a) : " << typeid(T1).name() << ","
            << typeid(T2).name() << std::endl;
  T1 b;
}
template <typename T>
void fun3(T a, T b) {
  std::cout << "void fun3(T a, T b) : " << typeid(T).name() << std::endl;
}
void func() {
  // T2位于尾部，而且推断不出来，所以不能省略T2，否则报错：
  // fun1<int>(10);
  // 必须同时指明T1和T2的类型：
  fun1<int, int>(10);  // void fun1(T1 a) : i,i

  // 指明T1、T2的类型：
  fun2<int, int>(20);  // void fun2(T2 a) : i,i
  // 由于T2的类型能够自动推断出来，并且它位于参数列表的尾部，所以可以省略：
  fun2<int>(10);  // void fun2(T2 a) : i,i

  // 没有显示地指名类型参数，所以编译器推断不出T是int还是double：
  // fun3(10, 23.5);

  // 已经显式地指明了T的类型为float，可以从容地使用正常的类型转换，
  // 编译器不会再为T的类型到底是int还是double而纠结了：
  fun3<float>(20, 93.7);  // void fun3(T a, T b) : f

  // 显式地指明实参时可以应用正常的类型转换：
  // 函数模板仅能进行const转换和数组或函数指针转换两种形式的类型转换，
  // 但是当显式地指明类型参数的实参时，就可以使用正常的类型转换了，
  // 就是非模板函数可以使用的那些所有的类型转换。
}
}  // namespace n4

int main(int argc, char *argv[]) {
  if (argc < 2) {
    std::cout << argv[0] << " i [0 - 5]" << std::endl;
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
      n3::test1::func();
      break;
    case 3:
      n3::test2::func();
      break;
    case 4:
      n3::test3::func();
      break;
    case 5:
      n4::func();
      break;
    default:
      std::cout << "invalid type" << std::endl;
      break;
  }

  return 0;
}