#include <cstring>
#include <iostream>

using namespace std;

// 对于计算机来说，拷贝是指用一份原有的、已经存在的数据创建出一份新的数据，最终的结果是多了一份相同的数据。
// 在C++中，拷贝并没有脱离它本来的含义，只是将这个含义进行了“特化”，是指用已经存在的对象创建出一个新的对象。从本质上讲，对象也是一份数据，因为它会占用内存。
// 严格来说，对象的创建包括两个阶段，首先要分配内存空间，然后再进行初始化：
//  分配内存很好理解，就是在堆区、栈区或者全局数据区留出足够多的字节。这个时候的内存还比较“原始”，没有被“教化”，它所包含的数据一般是零值或者随机值，没有实际的意义。
//  初始化就是首次对内存赋值，让它的数据有意义。注意是首次赋值，再次赋值不叫初始化。初始化的时候还可以为对象分配其他的资源（打开文件、连接网络、动态分配内存等），
//  或者提前进行一些计算（根据价格和数量计算出总价、根据长度和宽度计算出矩形的面积等）等。说白了，初始化就是调用构造函数。
// 很明显，这里所说的拷贝是在初始化阶段进行的，也就是用其它对象的数据来初始化新对象的内存。
// 当以拷贝的方式初始化一个对象时，会调用一个特殊的构造函数，就是拷贝构造函数（Copy Constructor）。
// 拷贝构造函数只有一个参数，它的类型是当前类的引用，而且一般都是const引用。
//  1) 为什么必须是当前类的引用呢？
//  如果拷贝构造函数的参数不是当前类的引用，而是当前类的对象，那么在调用拷贝构造函数时，会将另外一个对象直接传递给形参，这本身就是一次拷贝，会再次调用拷贝构造函数，
//  然后又将一个对象直接传递给了形参，将继续调用拷贝构造函数……这个过程会一直持续下去，没有尽头，陷入死循环。
//  只有当参数是当前类的引用时，才不会导致再次调用拷贝构造函数，这不仅是逻辑上的要求，也是C++语法的要求。
//  2) 为什么是const引用呢？
//  拷贝构造函数的目的是用其它对象的数据来初始化当前对象，并没有期望更改其它对象的数据，添加const限制后，这个含义更加明确了。
//  另外一个原因是，添加const限制后，可以将const对象和非const对象传递给形参了，因为非const类型可以转换为const类型。如果没有const限制，就不能将const对象传递给
//  形参，因为const类型不能转换为非const类型，这就意味着，不能使用const对象来初始化当前对象了。
// 一个类可以同时存在两个拷贝构造函数，一个函数的参数为const引用，另一个函数的参数为非const引用。
// 如果没有显式地定义拷贝构造函数，那么编译器会自动生成一个默认的拷贝构造函数。这个默认的拷贝构造函数很简单，使用“老对象”的成员变量对“新对象”的成员变量进行一一赋值。
// 对于简单的类，默认拷贝构造函数一般是够用的，我们也没有必要再显式地定义一个功能类似的拷贝构造函数。但是当类持有其它资源时，如动态分配的内存、打开的文件、指向其他数
// 据的指针、网络连接等，默认拷贝构造函数就不能拷贝这些资源，我们必须显式地定义拷贝构造函数，以完整地拷贝对象的所有数据。

// 对于func()的形参str，其实在定义时就为它分配了内存，但是此时并没有初始化，只有等到调用func()时，才会将其它对象的数据拷贝给str以完成初始化。
void func(string str) { cout << str << endl; }

class Student {
 public:
  Student(string name = "", int age = 0, float score = 0.0f);  // 普通构造函数
  Student(const Student &stu);  // 拷贝构造函数（声明）
 public:
  Student &operator=(const Student &stu);  // 重载=运算符
  void display();

 private:
  string m_name;
  int m_age;
  float m_score;
};
Student::Student(string name, int age, float score)
    : m_name(name), m_age(age), m_score(score) {}
// 拷贝构造函数（定义）
Student::Student(const Student &stu) {
  this->m_name = stu.m_name;
  this->m_age = stu.m_age;
  this->m_score = stu.m_score;

  cout << "Copy constructor was called." << endl;
}
// 重载=运算符
Student &Student::operator=(const Student &stu) {
  this->m_name = stu.m_name;
  this->m_age = stu.m_age;
  this->m_score = stu.m_score;
  cout << "operator=() was called." << endl;

  return *this;
}
void Student::display() {
  cout << m_name << "的年龄是" << m_age << "，成绩是" << m_score << endl;
}

// 当以拷贝的方式初始化对象时会调用拷贝构造函数。这里有两个关键点，分别是「以拷贝的方式」和「初始化对象」。
// 初始化对象是指，为对象分配内存后第一次向内存中填充数据，这个过程会调用构造函数。对象被创建后必须立即被初始化，换句话说，只要创建对象，就会调用构造函数。
// 初始化和赋值都是将数据写入内存中，并且从表面上看起来，初始化在很多时候都是以赋值的方式来实现的，所以很容易混淆。
// 在定义的同时进行赋值叫做初始化（Initialization），定义完成以后再赋值（不管在定义的时候有没有赋值）就叫做赋值（Assignment）。初始化只能有一次，赋值可以有多次。
// 对于基本类型的数据，我们很少会区分「初始化」和「赋值」这两个概念，即使将它们混淆，也不会出现什么错误。
// 但是对于类，它们的区别就非常重要了，因为初始化时会调用构造函数（以拷贝的方式初始化时会调用拷贝构造函数），而赋值时会调用重载过的赋值运算符。
// 初始化对象时会调用构造函数，不同的初始化方式会调用不同的构造函数：
//  如果用传递进来的实参初始化对象，那么会调用普通的构造函数，将此称为普通初始化；
//  如果用其它对象（现有对象）的数据来初始化对象，那么会调用拷贝构造函数，这就是以拷贝的方式初始化。
// 具体有哪些情况是以拷贝的方式来初始化对象呢？
//  1) 将其它对象作为实参。即使我们不在类中显式地定义拷贝构造函数，这种初始化方式也是有效的，因为编译器会生成默认的拷贝构造函数。
//     Student stu1("小明", 16, 90.5);  // 普通初始化
//     Student stu2(stu1);  // 以拷贝的方式初始化
//  2) 在创建对象的同时赋值。这是最常见的一种以拷贝的方式初始化对象的情况，非常容易理解。
//     Student stu2 = stu1;  // 以拷贝的方式初始化
//  3) 函数的形参为类类型。如果函数的形参为类类型（也就是一个对象），那么调用函数时要将另外一个对象作为实参传递进来赋值给形参，这也是以拷贝的方式初始化形参对象。
void func3(Student s) {}
//     func3(stu);  // 以拷贝的方式初始化
//     函数是一段可以重复使用的代码，只有等到真正调用函数时才会为局部数据（形参和局部变量）在栈上分配内存。对于上面的func()，虽然它的形参s是一个对象，但在定义函数时s
//     对象并没有被创建，只有等到调用函数时才会真正地创建s对象，并在栈上为它分配内存。而创建s对象，就是以拷贝的方式进行的。
//  4) 函数返回值为类类型
Student func4() {
  Student s("小明", 16, 90.5);
  return s;
}
//     Student stu = func4();  // 理论上讲，运行代码后会调用两次拷贝构造函数，一次是返回s对象时，另外一次是创建stu对象时。
//     当函数的返回值为类类型时，return语句会返回一个对象，不过为了防止局部对象被销毁，也为了防止通过返回值修改原来的局部对象，编译器并不会直接返回这个对象，而是根据这
//     个对象先创建出一个临时对象（匿名对象），再将这个临时对象返回。而创建临时对象的过程，就是以拷贝的方式进行的，会调用拷贝构造函数。
//     在较老的编译器上，你或许真的能够看到调用两次拷贝构造函数，例如iPhone上的C/C++编译器。但是在现代编译器上，只会调用一次拷贝构造函数，或者一次也不调用，例如在VS2010
//     下会调用一次拷贝构造函数，在GCC、Xcode下一次也不会调用。这是因为，现代编译器都支持返回值优化技术，会尽量避免拷贝对象，以提高程序运行效率。

// C++深拷贝和浅拷贝
// 对于基本类型的数据以及简单的对象，它们之间的拷贝非常简单，就是按位复制内存。这种默认的拷贝行为就是浅拷贝，这和调用memcpy()函数的效果非常类似。
// 对于简单的类，默认的拷贝构造函数一般就够用了，我们也没有必要再显式地定义一个功能类似的拷贝构造函数。但是当类持有其它资源时，例如动态分配的内存、指向其他数据的指针等，
// 默认的拷贝构造函数就不能拷贝这些资源了，我们必须显式地定义拷贝构造函数，以完整地拷贝对象的所有数据。
// 这种将对象所持有的其它资源一并拷贝的行为叫做深拷贝，我们必须显式地定义拷贝构造函数才能达到深拷贝的目的。
// 标准模板库（STL）中的string、vector、stack、set、map等也都必须使用深拷贝。
// 到底是浅拷贝还是深拷贝？
// 如果一个类拥有指针类型的成员变量，那么绝大部分情况下就需要深拷贝，因为只有这样，才能将指针指向的内容再复制出一份来，让原有对象和新生对象相互独立，彼此之间不受影响。
// 如果类的成员变量没有指针，一般浅拷贝足以。
// 另外一种需要深拷贝的情况就是在创建对象时进行一些预处理工作，比如统计创建过的对象的数目、记录对象创建的时间等
class Array {
 public:
  Array(int len);
  Array(const Array &arr);  // 拷贝构造函数
  ~Array();

 public:
  int operator[](int i) const { return m_p[i]; }  // 获取元素（读取）
  int &operator[](int i) { return m_p[i]; }       // 获取元素（写入）
  int length() const { return m_len; }

 private:
  int m_len;
  int *m_p;
};
Array::Array(int len) : m_len(len) { m_p = (int *)calloc(len, sizeof(int)); }
Array::Array(const Array &arr) {  // 拷贝构造函数
  this->m_len = arr.m_len;
  this->m_p = (int *)calloc(this->m_len, sizeof(int));
  memcpy(this->m_p, arr.m_p, m_len * sizeof(int));
}
Array::~Array() { free(m_p); }
// 另外需要注意的是，printArray()函数的形参为引用类型，这样做能够避免在传参时调用拷贝构造函数；
// 又因为printArray()函数不会修改任何数组元素，所以我们添加了const限制，以使得语义更加明确。
void printArray(const Array &arr) {
  int len = arr.length();
  for (int i = 0; i < len; i++) {
    if (i == len - 1) {
      cout << arr[i] << endl;
    } else {
      cout << arr[i] << ", ";
    }
  }
}

int main() {
  // 表面上看起来是将一个字符串直接赋值给了s1，实际上在内部进行了类型转换，将const char *类型转换为string类型后才赋值的
  string s1 = "http://c.biancheng.net";
  string s2(s1);
  string s3 = s1;
  string s4 = s1 + " " + s2;
  func(s1);
  cout << s1 << endl << s2 << endl << s3 << endl << s4 << endl;
  // s1、s2、s3、s4 以及func()的形参str，都是使用拷贝的方式来初始化的。
  // 对于s1、s2、s3、s4，都是将其它对象的数据拷贝给当前对象，以完成当前对象的初始化。

  Student stu11("小明", 16, 90.5);
  Student stu12 = stu11;  // 调用拷贝构造函数
  Student stu13(stu11);   // 调用拷贝构造函数
  stu11.display();
  stu12.display();
  stu13.display();

  int a = 100;  // 以赋值的方式初始化
  a = 200;      // 赋值
  a = 300;      // 赋值
  int b;        // 默认初始化
  b = 29;       // 赋值
  b = 39;       // 赋值

  // stu1、stu2、stu3都会调用普通构造函数Student(string name, int age, float score)
  Student stu1("小明", 16, 90.5);
  Student stu2("王城", 17, 89.0);
  Student stu3("陈晗", 18, 98.0);
  Student stu4 = stu1;  // 调用拷贝构造函数Student(const Student &stu)
  stu4 = stu2;          // 调用operator=()
  stu4 = stu3;          // 调用operator=()
  Student stu5;         // 调用普通构造函数Student()
  stu5 = stu1;          // 调用operator=()
  stu5 = stu2;          // 调用operator=()
  // Copy constructor was called.
  // operator=() was called.
  // operator=() was called.
  // operator=() was called.
  // operator=() was called.

  Student ss1("小明", 16, 90.5);
  Student ss2(ss1);       // 第一种,调用一次
  Student ss3 = ss1;      // 第二种,调用一次
  func3(ss1);             // 第三种,调用一次
  Student ss4 = func4();  // 第四种，一次也没调用
  // Copy constructor was called.
  // Copy constructor was called.
  // Copy constructor was called.

  Array arr1(10);
  for (int i = 0; i < 10; i++) {
    arr1[i] = i;
  }
  Array arr2 = arr1;
  arr2[5] = 100;
  arr2[3] = 29;
  printArray(arr1);  // 0, 1, 2, 3, 4, 5, 6, 7, 8, 9
  printArray(arr2);  // 0, 1, 2, 29, 4, 100, 6, 7, 8, 9
  // 除了会将原有对象的所有成员变量拷贝给新对象，还会为新对象再分配一块内存，并将原有对象所持有的内存也拷贝过来。这样做的结果是，
  // 原有对象和新对象所持有的动态内存是相互独立的，更改一个对象的数据不会影响另外一个对象，更改了arr2的数据，就没有影响arr1。
  // 拷贝构造函数删除后的打印：
  // 0, 1, 2, 29, 4, 100, 6, 7, 8, 9
  // 0, 1, 2, 29, 4, 100, 6, 7, 8, 9

  return 0;
}