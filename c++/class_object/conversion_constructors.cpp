#include <iostream>

// 在C/C++中，不同的数据类型之间可以相互转换。
// 无需用户指明如何转换的称为自动类型转换（隐式类型转换），
// 需要用户显式地指明如何转换的称为强制类型转换。
// 不管是自动类型转换还是强制类型转换，前提必须是编译器知道如何转换，
// 这些规则都是编译器内置的，并没有告诉编译器。
// 例如，将小数转换为整数会抹掉小数点后面的数字，
// 将int*转换为float*只是简单地复制指针的值，
// 换句话说，如果编译器不知道转换规则就不能转换，使用强制类型也无用。
// C++允许自定义类型转换规则，用户可以将其它类型转换为当前类类型，
// 也可以将当前类类型转换为其它类型。
// 这种自定义的类型转换规则只能以类的成员函数的形式出现，这种转换规则只适用于类。
// 将其它类型转换为当前类类型需要借助转换构造函数（Conversion constructor）。
// 转换构造函数也是一种构造函数，它遵循构造函数的一般规则，但只有一个参数。
// 转换构造函数也是构造函数，它除了可以用来将其它类型转换为当前类类型，
// 还可以用来初始化对象，这是构造函数本来的意义。

namespace n1 {
class Complex {
 public:
  Complex() : m_real(0.0), m_imag(0.0) {
    std::cout << "Complex()" << std::endl;
  }
  Complex(double real, double imag) : m_real(real), m_imag(imag) {
    std::cout << "Complex(double real, double imag)" << std::endl;
  }
  // 将double类型的参数real转换成Complex类的对象，
  // 并将real作为复数的实部，将0作为复数的虚部：
  Complex(double real) : m_real(real), m_imag(0.0) {  // 转换构造函数
    std::cout << "Complex(double real)" << std::endl;
  }
  // 在进行数学运算、赋值、拷贝等操作时，如果遇到类型不兼容、
  // 需要将double类型转换为Complex类型时，
  // 编译器会检索当前的类是否定义了转换构造函数，
  // 如果没有定义的话就转换失败，如果定义了的话就调用转换构造函数。

  // 借助函数的默认参数，可以简化为：
  // Complex(double real=0.0, double imag=0.0):m_real(real),m_imag(imag){}
  // 转换构造函数只包含了一个参数，能够和其他两个普通的构造函数融合在一起。

  Complex operator+(const Complex &c);

 public:
  friend std::ostream &operator<<(std::ostream &out, Complex &c);  // 友元函数
 private:
  double m_real;
  double m_imag;
};
std::ostream &operator<<(std::ostream &out, Complex &c) {
  out << c.m_real << " + " << c.m_imag << "i";
  return out;
}
Complex Complex::operator+(const Complex &c) {
  return Complex(this->m_real + c.m_real, this->m_imag + c.m_imag);
}

void func1() {
  // 编译器对7.5是作为double类型处理的，在求解表达式时，
  // 先将a转换为double类型，然后与7.5相加，得到和为13.5。
  // 在向整型变量a赋值时，将13.5转换为整数13，然后赋给a。
  // 并没有告诉编译器如何去做，编译器使用内置的规则完成数据类型的转换。
  int a = 6;
  a = 7.5 + a;
  std::cout << a << std::endl;  // 13

  // p1是int*类型，它指向的内存里面保存的是整数，p2是float*类型，
  // 将p1赋值给p2后，p2也指向了这块内存，并把这块内存中的数据作为小数处理。
  // 整数和小数的存储格式大相径庭，将整数作为小数处理非常荒诞，
  // 可能会引发莫名其妙的错误，所以编译器默认不允许将p1赋值给p2。
  // 但是，使用强制类型转换后，编译器就认为知道这种风险的存在，
  // 并进行了适当的权衡，所以最终还是允许了这种行为。
  int n = 100;
  int *p1 = &n;
  float *p2 = (float *)p1;
  std::cout << *p2 << std::endl;  // 1.4013e-43，可能会崩溃
}

void func2() {
  Complex a1;                    // Complex()
  std::cout << a1 << std::endl;  // 0 + 0i

  Complex a2(10.0, 20.0);        // Complex(double real, double imag)
  std::cout << a2 << std::endl;  // 10 + 20i

  // 以拷贝的方式初始化对象，先调用转换构造函数，
  // 将25.5转换为Complex类型（匿名对象），然后再拷贝给a3：
  Complex a3 = 25.5;             // Complex(double real)
  std::cout << a3 << std::endl;  // 25.5 + 0i

  // 转换构造函数创建具体对象：
  Complex a4(18.9);              // Complex(double real)
  std::cout << a4 << std::endl;  // 18.9 + 0i

  // 创建匿名对象：
  Complex(77.7);  // Complex(double real)

  // 创建一个匿名对象并将它赋值给a2：
  a2 = Complex(88.2);            // Complex(double real)
  std::cout << a2 << std::endl;  // 88.2 + 0i

  // 调用转换构造函数，相当于a1.Complex(34.4):
  a1 = 34.4;                     // Complex(double real)
  std::cout << a1 << std::endl;  // 34.4 + 0i

  a1 = (Complex)45.6;            // Complex(double real)
  std::cout << a1 << std::endl;  // 45.6 + 0i

  a1 = Complex(57.8);            // Complex(double real)
  std::cout << a1 << std::endl;  // 57.8 + 0i

  a1 = static_cast<Complex>(78.4);  // Complex(double real)
  std::cout << a1 << std::endl;     // 78.4 + 0i
}

void func3() {
  Complex c1(15.6, 89.9);  // Complex(double real, double imag)
  Complex c2;              // Complex()
  // 在进行加法运算符时，编译器先将29.6转换为Complex类型（匿名对象）再相加：
  c2 = c1 + 29.6;
  // Complex(double real)
  // Complex(double real, double imag)
  std::cout << c2 << std::endl;  // 45.2 + 89.9i
}

void func4() {
  // 需要注意的是，为了获得目标类型，编译器会不择手段，
  // 会综合使用内置的转换规则和用户自定义的转换规则，并且会进行多级类型转换：

  // 先将int转换为double，再根据用户自定义规则将double转换为Complex：
  Complex c1 = 100;              // int --> double --> Complex
  std::cout << c1 << std::endl;  // 100 + 0i

  // 先将char转换为int，再将int转换为double，
  // 最后根据用户自定义规则将double转换为Complex：
  c1 = 'A';                      // char --> int --> double --> Complex
  std::cout << c1 << std::endl;  // 65 + 0i

  c1 = true;                     // bool --> int --> double --> Complex
  std::cout << c1 << std::endl;  // 1 + 0i

  // 将char、bool、int都转换为Complex类型再运算：
  Complex c2(25.8, 0.7);
  c1 = c2 + 'H' + true + 15;
  std::cout << c1 << std::endl;  // 113.8 + 0.7i
}

void func() {
  func1();
  func2();
  func3();
  func4();
}
}  // namespace n1

namespace n2 {
// 转换构造函数能够将其它类型转换为当前类类型，
// 但是不能反过来将当前类类型转换为其它类型。
// 类型转换函数（Type conversion function）解决了这个问题。
// 作用就是将当前类类型转换为其它类型，
// 它只能以成员函数的形式出现，也就是只能出现在类中。

// 类型转换函数的语法格式为：
// operator type(){ return data;}
// operator是C++关键字；
// type是要转换的目标类型；
// data是要返回的type类型的数据。

// 因为要转换的目标类型是type，所以返回值data也必须是type类型。
// 既然已经知道了要返回type类型的数据，所以没有必要明确地给出返回值类型。
// 类型转换函数看起来没有返回值类型，其实是隐式地指明了返回值类型。
// 类型转换函数也没有参数，因为要将当前类的对象转换为其它类型，所以参数已知。
// 实际上编译器会把当前对象的地址赋值给this指针，
// 这样在函数体内就可以操作当前对象了。
// 类型转换函数和运算符的重载非常相似，都使用operator关键字，
// 因此也把类型转换函数称为类型转换运算符。
// 关于类型转换函数的说明：
// 1.type可以是内置类型、类类型以及由typedef定义的类型别名，
//   任何可作为函数返回类型的类型（void除外）都能够被支持。
//   一般而言，不允许转换为数组或函数类型，转换为指针类型或引用类型是可以的；
// 2.类型转换函数一般不会更改被转换的对象，所以通常被定义为const成员；
// 3.类型转换函数可以被继承，可以是虚函数；
// 4.一个类虽然可以有多个类型转换函数（类似于函数重载），
//   但是如果多个类型转换函数要转换的目标类型本身又可以相互转换（类型相近），
//   那么有时候就会产生二义性。
class A {
 public:
  operator double() const { return 1.2; }  // 转换为double类型
  operator int() const { return 1; }       // 转换为int类型
};
void func1() {
  A a;
  // float f = 12.5 + a;  // 引发二义性
  // 编译器可以调用operator double()将a转换为double类型，
  // 也可以调用operator int()将a转换为int类型，
  // 这两种类型都可以跟12.5进行加法运算，
  // 并且从a转换为double与从a转化为int是平级的，没有谁的优先级更高，
  // 所以这个时候编译器就不知道该调用哪个函数了，
  // 干脆抛出一个二义性错误，让用户解决。
}

class Complex {
 public:
  Complex() : m_real(0.0), m_imag(0.0) {}
  Complex(double real, double imag) : m_real(real), m_imag(imag) {}

 public:
  friend std::ostream &operator<<(std::ostream &out, Complex &c);
  friend Complex operator+(const Complex &c1, const Complex &c2);
  // 类型转换函数非常简单，就是返回m_real的值，所以建议写成inline的形式：
  operator double() const { return m_real; }  // 类型转换函数
 private:
  double m_real;
  double m_imag;
};
std::ostream &operator<<(std::ostream &out, Complex &c) {
  out << c.m_real << " + " << c.m_imag << "i";
  return out;
}
Complex operator+(const Complex &c1, const Complex &c2) {
  Complex c;
  c.m_real = c1.m_real + c2.m_real;
  c.m_imag = c1.m_imag + c2.m_imag;
  return c;
}

void func() {
  Complex c1(24.6, 100);
  double f = c1;  // 相当于double f = Complex::operator double(&c1);
  std::cout << "f = " << f << std::endl;  // f = 24.6
  f = 12.5 + c1 + 6;  // 相当于f = 12.5 + Complex::operator double(&c1) + 6;
  std::cout << "f = " << f << std::endl;  // f = 43.1
  int n = Complex(43.2, 9.3);  // 先转换为double，再转换为int
  std::cout << "n = " << n << std::endl;  // n = 43
}
}  // namespace n2

namespace n3 {
// 转换构造函数和类型转换函数的作用是相反的：
// 1.转换构造函数会将其它类型转换为当前类类型；
// 2.类型转换函数会将当前类类型转换为其它类型。
// 如果没有这两个函数，Complex类和int、double、bool等基本类型的四则运算、
// 逻辑运算都将变得非常复杂，要编写大量的运算符重载函数。
// 但是，如果一个类同时存在这两个函数，就有可能产生二义性。
// 解决二义性问题的办法也很简单粗暴，
// 要么只使用转换构造函数，要么只使用类型转换函数。
// 实践证明对转换构造函数的需求往往更加强烈，这样能增加编码的灵活性，例如：
// 1.可以将一个字符串字面量或者一个字符数组直接赋值给string类的对象；
// 2.可以将一个int、double、bool等基本类型的数据直接赋值给Complex类。
// 那么，如果想把当前类类型转换为其它类型怎么办呢？
// 很简单，增加一个普通的成员函数即可，
// 例如，string类使用c_str()函数转换为C风格的字符串，
// complex类使用real()和imag()函数来获取复数的实部和虚部。
class Complex {
 public:
  // 包含了转换构造函数
  Complex(double real = 0.0, double imag = 0.0) : m_real(real), m_imag(imag) {
    std::cout << "Complex" << std::endl;
  }

 public:
  friend std::ostream &operator<<(std::ostream &out, Complex &c);
  friend Complex operator+(const Complex &c1, const Complex &c2);
  operator double() const {  // 类型转换函数
    std::cout << "double" << std::endl;
    return m_real;
  }

 private:
  double m_real;
  double m_imag;
};
std::ostream &operator<<(std::ostream &out, Complex &c) {
  out << c.m_real << " + " << c.m_imag << "i";
  return out;
}
Complex operator+(const Complex &c1, const Complex &c2) {
  Complex c;
  c.m_real = c1.m_real + c2.m_real;
  c.m_imag = c1.m_imag + c2.m_imag;
  return c;
}

void func() {
  Complex c1(24.6, 100);  // Complex
  double d = c1;          // double，调用类型转换函数
  c1 = 78.4;              // Complex，调用转换构造函数

  // d = 12.5 + c1;  // 产生二义性
  // 进行加法运算时，有两种转换方案：
  // 1.先将12.5转换为Complex类型再运算，这样得到的结果也是Complex类型，
  //   再调用类型转换函数就可以赋值给f了；
  // 2.先将c1转换为double类型再运算，这样得到的结果也是double类型，
  //   可以直接赋值给f。
  // 很多人认为，既然=左边是double类型，很显然应该选择第二种方案才合理。
  // 其实不然，编译器不会根据=左边的数据类型来选择转换方案，
  // 编译器只关注12.5+c1这个表达式本身，站在这个角度考虑，
  // 上面的两种转换方案都可以，编译器不知道选择哪一种，
  // 所以会抛出二义性错误，让用户自己去解决。

  // 即使假设编译器会根据=左边的数据类型来选择解决方案，那仍然会存在二义性问题：
  // Complex c2 = c1 + 46.7;  // 产生二义性
}
}  // namespace n3

namespace n4 {
// 一个实用的Complex类能够进行四则运算和关系运算，
// 需要重载+、-、*、/、+=、-=、*=、/=、==、!=这些运算符，
// 不过这里仅仅重载了+、+=、==、!=运算符：
class Complex {
 public:
  Complex(double real = 0.0, double imag = 0.0) : m_real(real), m_imag(imag) {}

 public:
  friend std::ostream &operator<<(std::ostream &out, Complex &c);
  friend std::istream &operator>>(std::istream &in, Complex &c);
  friend Complex operator+(const Complex &c1, const Complex &c2);
  friend bool operator==(const Complex &c1, const Complex &c2);
  friend bool operator!=(const Complex &c1, const Complex &c2);
  Complex &operator+=(const Complex &c);

 public:
  double real() const { return m_real; }
  double imag() const { return m_imag; }

 private:
  double m_real;
  double m_imag;
};
std::ostream &operator<<(std::ostream &out, Complex &c) {
  out << c.m_real << " + " << c.m_imag << "i";
  return out;
}
std::istream &operator>>(std::istream &in, Complex &c) {
  in >> c.m_real >> c.m_imag;
  return in;
}
Complex operator+(const Complex &c1, const Complex &c2) {
  Complex c;
  c.m_real = c1.m_real + c2.m_real;
  c.m_imag = c1.m_imag + c2.m_imag;
  return c;
}
Complex &Complex::operator+=(const Complex &c) {
  this->m_real += c.m_real;
  this->m_imag += c.m_imag;
  return *this;
}
bool operator==(const Complex &c1, const Complex &c2) {
  if (c1.m_real == c2.m_real && c1.m_imag == c2.m_imag) {
    return true;
  } else {
    return false;
  }
}
bool operator!=(const Complex &c1, const Complex &c2) {
  if (c1.m_real != c2.m_real || c1.m_imag != c2.m_imag) {
    return true;
  } else {
    return false;
  }
}
void func() {
  Complex c1(12, 60);
  std::cout << "cz1 = " << c1 << std::endl;  // c1 = 12 + 60i

  // 先调用转换构造函数将22.8转换为Complex类型，再调用重载过的+运算符：
  Complex c2 = c1 + 22.8;
  std::cout << "c2 = " << c2 << std::endl;  // c2 = 34.8 + 60i
  // 同上
  Complex c3 = 8.3 + c1;
  std::cout << "c3 = " << c3 << std::endl;  // c3 = 20.3 + 60i

  // 先调用转换构造函数将73转换为Complex类型，再调用重载过的+=运算符：
  Complex c4(4, 19);
  c4 += 73;
  std::cout << "c4 = " << c4 << std::endl;  // c4 = 77 + 19i

  // 调用重载过的+=运算符：
  Complex c5(14.6, 26.2);
  c5 += c1;
  std::cout << "c5 = " << c5 << std::endl;  // c5 = 26.6 + 86.2i

  // 调用重载过的==运算符
  if (c1 == c2) {
    std::cout << "c1 == c2" << std::endl;
  } else {
    std::cout << "c1 != c2" << std::endl;
  }
  // c1 != c2

  // 先调用转换构造函数将77转换为Complex类型，再调用重载过的!=运算符：
  if (c4 != 77) {
    std::cout << "c4 != 77" << std::endl;
  } else {
    std::cout << "c4 == 77" << std::endl;
  }
  // c4 != 77

  // 将Complex转换为double，没有调用类型转换函数，
  // 而是调用了real()这个普通的成员函数：
  double ff = c5.real();
  std::cout << "f = " << ff << std::endl;  // f = 26.6
}
}  // namespace n4

int main(int argc, char *argv[]) {
  if (argc < 2) {
    std::cout << argv[0] << " i [0 - 3]" << std::endl;
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
      n3::func();
      break;
    case 3:
      n4::func();
      break;
    default:
      std::cout << "invalid type" << std::endl;
      break;
  }

  return 0;
}