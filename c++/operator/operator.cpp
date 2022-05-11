#include <cmath>
#include <iomanip>
#include <iostream>

// 所谓重载，就是赋予新的含义。
// 函数重载（Function Overloading）可以让一个函数名有多种功能，在不同情况下进行不同的操作。
// 运算符重载（Operator Overloading）也是一个道理，同一个运算符可以有不同的功能。
// 实际上，已经在不知不觉中使用了运算符重载。
// 例如，+号可以对不同类型（int、float 等）的数据进行加法操作；
// <<既是位移运算符，又可以配合cout向控制台输出数据，C++本身已经对这些运算符进行了重载。
// C++也允许程序员自己重载运算符，这带来了很大的便利。
// 运算符重载其实就是定义一个函数，在函数体内实现想要的功能，
// 当用到该运算符时，编译器会自动调用这个函数。
// 也就是说，运算符重载是通过函数实现的，它本质上是函数重载。
// 运算符重载的格式为：返回值类型 operator 运算符名称 (形参表列){}
// operator是关键字，专门用于定义重载运算符的函数，可以将operator运算符名称这一部分看做函数名。
// 运算符重载函数除了函数名有特定的格式，其它地方和普通函数并没有区别。
// 虽然运算符重载所实现的功能完全可以用函数替代，但运算符重载使得程序的书写更加人性化，易于阅读。
// 运算符被重载后，原有的功能仍然保留，没有丧失或改变。
// 通过运算符重载，扩大了C++已有运算符的功能，使之能用于对象。

// 运算符重载的注意事项:
// 1.并不是所有的运算符都可以重载。能够重载的运算符包括：
//   + - * / % ^ & | ~ ! = < > += -= *= /= %= ^= &= |= << >> <<= >>=
//   == != <= >= && || ++ -- , ->* -> () [] new new[] delete delete[]
//   上述运算符中，[]是下标运算符，()是函数调用运算符，自增自减运算符的前置和后置形式都可以重载。
//   长度运算符sizeof、条件运算符: ?、成员选择符.和域解析运算符::不能被重载。
// 2.重载不能改变运算符的优先级和结合性。
// 3.重载不会改变运算符的用法，原有有几个操作数、操作数在左边还是在右边，这些都不会改变。
//   例如~号右边只有一个操作数，+号总是出现在两个操作数之间，重载后也必须如此。
// 4.运算符重载函数不能有默认的参数，否则就改变了运算符操作数的个数，这显然是错误的。
// 5.运算符重载函数既可以作为类的成员函数，也可以作为全局函数。
//   将运算符重载函数作为类的成员函数时，二元运算符的参数只有一个，一元运算符不需要参数。
//   之所以少一个参数，是因为这个参数是隐含的。
//   将运算符重载函数作为全局函数时，二元操作符就需要两个参数，
//   一元操作符需要一个参数，而且其中必须有一个参数是对象，
//   好让编译器区分这是程序员自定义的运算符，防止程序员修改用于内置类型的运算符的性质。
//   如果有两个参数，这两个参数可以都是对象，也可以一个是对象，一个是C++内置类型的数据。
//   另外，将运算符重载函数作为全局函数时，一般都需要在类中将该函数声明为友元函数。
//   原因很简单，该函数大部分情况下都需要使用类的private成员。
// 6.箭头运算符->、下标运算符[]、函数调用运算符()、赋值运算符=只能以成员函数的形式重载。

// 全局的new和delete不能定义到命名空间中，对吧
void *operator new(size_t size) {
  std::cout << "g new" << std::endl;
  return malloc(sizeof(int));
}
void *operator new[](size_t size) {
  std::cout << "g new[]" << std::endl;
  return malloc(sizeof(int) * size);
}
void operator delete(void *ptr) { std::cout << "g delete" << std::endl; }
void operator delete[](void *ptr) { std::cout << "g delete[]" << std::endl; }

namespace n1 {  // 所有运算符的重载
class Complex {
 public:
  Complex(double real = 0.0, double imag = 0.0) : m_real(real), m_imag(imag) {
    std::cout << "Complex" << std::endl;
  }

 public:
  // C++规定，下标运算符[]必须以成员函数的形式进行重载，该重载函数在类中的声明格式如下：
  double &operator[](int i) {  // 不仅可以访问元素，还可以修改元素
    if (i % 2) {
      return m_imag;
    } else {
      return m_real;
    }
  }
  const double &operator[](int i) const {  // 只能访问而不能修改元素
    if (i % 2) {
      return m_imag;
    } else {
      return m_real;
    }
  }
  // 在实际开发中，应该同时提供以上两种形式，这样做是为了适应const对象，
  // 因为const对象只能调用const成员函数，如果不提供第二种形式，那么将无法访问const对象的任何元素。

  // 在C++中，类型的名字（包括类的名字）本身也是一种运算符，即类型强制转换运算符。
  // 类型强制转换运算符是单目运算符，也可以被重载，但只能重载为成员函数，不能重载为全局函数。
  operator double() {
    std::cout << "operator double" << std::endl;
    return m_real;
  }
  operator int() {
    std::cout << "operator string" << std::endl;
    return static_cast<int>(m_real);
  }

  // ==========================上面的只能用成员函数==========================

  // 为什么要以成员函数的形式重载+=等：
  // 首先要明白，运算符重载的初衷是给类添加新的功能，
  // 方便类的运算，它作为类的成员函数是理所应当的，是首选的。
  // 不过，类的成员函数不能对称地处理数据，必须在（参与运算的）所有类型的内部都重载当前的运算符。
  // 所以C++进行了折中方案，允许以全局函数（友元函数）的形式重载运算符。
  // C++创始人Bjarne Stroustrup也曾考虑过为内部类型（bool、int、double等）定义额外运算符的问题，
  // 但后来还是放弃了这种想法，因为Bjarne Stroustrup不希望改变现有规则：
  // 任何类型（无论是内部类型还是用户自定义类型）都不能在其定义完成以后再增加额外的操作。
  // 这里还有另外的一个原因，C内部类型之间的转换已经够肮脏了，决不能再向里面添乱。
  // 而通过成员函数为已存在的类型提供混合运算的方式，
  // 从本质上看，比所采用的全局函数（友元函数）加转换构造函数的方式还要肮脏许多。
  // 采用全局函数能使我们定义这样的运算符，它们的参数具有逻辑的对称性。
  // 与此相对应的，把运算符定义为成员函数能够保证在调用时对第一个（最左的）运算对象不出现类型转换，
  // 也就是C++不会对调用成员函数的对象进行类型转换。
  // 总起来说，有一部分运算符重载既可以是成员函数也可以是全局函数，
  // 虽然没有一个必然的、不可抗拒的理由选择成员函数，
  // 但我们应该优先考虑成员函数，这样更符合运算符重载的初衷；
  Complex &operator+=(const Complex &c) {
    this->m_real += c.m_real;
    this->m_imag += c.m_imag;
    return *this;
  }
  Complex &operator-=(const Complex &c) {
    this->m_real -= c.m_real;
    this->m_imag -= c.m_imag;
    return *this;
  }
  Complex &operator*=(const Complex &c) {
    this->m_real = this->m_real * c.m_real - this->m_imag * c.m_imag;
    this->m_imag = this->m_imag * c.m_real + this->m_real * c.m_imag;
    return *this;
  }
  Complex &operator/=(const Complex &c) {
    this->m_real = (this->m_real * c.m_real + this->m_imag * c.m_imag) /
                   (pow(c.m_real, 2) + pow(c.m_imag, 2));
    this->m_imag = (this->m_imag * c.m_real - this->m_real * c.m_imag) /
                   (pow(c.m_real, 2) + pow(c.m_imag, 2));
    return *this;
  }

  // 自增++和自减--都是一元运算符，它的前置形式和后置形式都可以被重载：
  Complex operator++() {  // ++i，前置形式
    m_real += 1;
    m_imag += 1;
    return *this;  // 返回当前对象
  }
  Complex operator++(int n) {  // i++，后置形式
    // 参数n是没有任何意义的，它的存在只是为了区分是前置形式还是后置形式
    Complex c = *this;
    m_real += 1;
    m_imag += 1;
    return c;
  }
  Complex operator--() {  // --i，前置形式
    m_real -= 1;
    m_imag -= 1;
    return *this;  // 返回当前对象
  }
  Complex operator--(int n) {  // i--，后置形式
    // 参数n是没有任何意义的，它的存在只是为了区分是前置形式还是后置形式
    Complex c = *this;
    m_real -= 1;
    m_imag -= 1;
    return c;
  }

  // 内存管理运算符new、new[]、delete和delete[]也可以进行重载，既可以是类的成员函数，也可以是全局函数。
  // 无论是作为成员函数还是作为全局函数，new的第一个参数必须是size_t类型。
  // size_t表示的是要分配空间的大小，对于new[]的重载函数而言，size_t则表示所需要分配的所有空间的总和。
  void *operator new(size_t size) {
    std::cout << "m new" << std::endl;
    return malloc(sizeof(double) * 2);
  }
  void *operator new[](size_t size) {
    std::cout << "m new[]" << std::endl;
    return malloc(sizeof(double) * 2 * size);
  }
  void operator delete(void *ptr) {
    std::cout << "m delete" << std::endl;
    free(ptr);
  }
  void operator delete[](void *ptr) {
    std::cout << "m delete[]" << std::endl;
    free(ptr);
  }

  // ==========================上面的可以用成员函数也可以用全局函数==========================

  // 为了保持对称性，应该以全局函数的形式重载+、-、*、/、==、!=
  friend Complex operator+(const Complex &c1, const Complex &c2);
  friend Complex operator-(const Complex &c1, const Complex &c2);
  friend Complex operator*(const Complex &c1, const Complex &c2);
  friend Complex operator/(const Complex &c1, const Complex &c2);
  friend bool operator==(const Complex &c1, const Complex &c2);
  friend bool operator!=(const Complex &c1, const Complex &c2);

  // cout是ostream类的对象，cin是istream类的对象，如果要使用cout和cin处理自定类型，
  // 就必须以全局函数的形式重载<<和>>，否则就要修改标准库中的类，这显然不是所期望的。
  // 由于采用了引用的方式进行参数传递，并且也返回了对象的引用，所以重载后的运算符可以实现连续输出。
  friend std::istream &operator>>(std::istream &in, Complex &A);
  friend std::ostream &operator<<(std::ostream &out, Complex &A);

  // ==========================上面的应该用全局函数==========================

 public:
  double real() const { return m_real; }
  double imag() const { return m_imag; }

 private:
  double m_real;  // 实部
  double m_imag;  // 虚部
};
Complex operator+(const Complex &c1, const Complex &c2) {
  Complex c;
  c.m_real = c1.m_real + c2.m_real;
  c.m_imag = c1.m_imag + c2.m_imag;
  return c;
}
Complex operator-(const Complex &c1, const Complex &c2) {
  Complex c;
  c.m_real = c1.m_real - c2.m_real;
  c.m_imag = c1.m_imag - c2.m_imag;
  return c;
}
Complex operator*(const Complex &c1, const Complex &c2) {
  // (a+bi) * (c+di) = (ac-bd) + (bc+ad)i
  Complex c;
  c.m_real = c1.m_real * c2.m_real - c1.m_imag * c2.m_imag;
  c.m_imag = c1.m_imag * c2.m_real + c1.m_real * c2.m_imag;
  return c;
}
Complex operator/(const Complex &c1, const Complex &c2) {
  // (a+bi) / (c+di) = [(ac+bd) / (c²+d²)] + [(bc-ad) / (c²+d²)]i
  Complex c;
  c.m_real = (c1.m_real * c2.m_real + c1.m_imag * c2.m_imag) /
             (pow(c2.m_real, 2) + pow(c2.m_imag, 2));
  c.m_imag = (c1.m_imag * c2.m_real - c1.m_real * c2.m_imag) /
             (pow(c2.m_real, 2) + pow(c2.m_imag, 2));
  return c;
}
bool operator==(const Complex &c1, const Complex &c2) {
  return (c1.m_real == c2.m_real && c1.m_imag == c2.m_imag);
}
bool operator!=(const Complex &c1, const Complex &c2) {
  return (c1.m_real != c2.m_real || c1.m_imag != c2.m_imag);
}
std::istream &operator>>(std::istream &in, Complex &A) {
  in >> A.m_real >> A.m_imag;
  return in;
}
std::ostream &operator<<(std::ostream &out, Complex &A) {
  out << A.m_real << "+" << A.m_imag << "i";
  return out;
}

void func1() {
  Complex c = 15.0;             // Complex
  std::cout << c << std::endl;  // 15+0i

  double d = c;                         // operator double
  std::cout << d << std::endl;          // 15
  std::cout << (double)c << std::endl;  // 15

  int i = c;                    // operator int
  std::cout << d << std::endl;  // 15
}
void func2() {
  Complex c1(2, 2);
  std::cout << c1 << std::endl;  // 2+2i
  ++c1;
  std::cout << c1 << std::endl;  // 3+3i
  // std::cout << ++c << std::endl;  // 报错
  Complex c2 = c1++;
  std::cout << c2 << std::endl;  // 3+3i
  std::cout << c1 << std::endl;  // 4+4i

  --c2;
  std::cout << c2 << std::endl;  // 2+2i
  Complex c3 = c2--;
  std::cout << c3 << std::endl;  // 2+2i
  std::cout << c2 << std::endl;  // 1+1i
}
void func3() {
  Complex c(2, 3);
  std::cout << c[0] << "," << c[1] << "," << c[8] << "," << c[13] << std::endl;
  // 2,3,2,3
  c[7 + 2] = 6;
  c[100] = 4;
  std::cout << c[0] << "," << c[1] << "," << c[8] << "," << c[13] << std::endl;
  // 4,6,4,6

  const Complex cc(8, 10);
  std::cout << cc[0] << "," << cc[1] << "," << cc[8] << "," << cc[13]
            << std::endl;
  // 8,10,8,10
  // cc[9] = 0;
}
void func4() {
  // 编译器看到使用new创建自己定义的类的对象时，它选择成员版本的operator new()而不是全局版本的new()：
  Complex *c1 = new Complex();   // m new
  Complex *c2 = new Complex[2];  // m new[]
  delete c1;                     // m delete
  delete[] c2;                   // m delete[]

  // 但是全局版本的new和delete仍为所有其他类型对象使用（除非它们也有自己的new和delete）：
  int *i = new int;     // g new
  int *p = new int[3];  // g new[]
  delete i;             // g delete
  delete[] p;           // g delete[]
}
void func5() {
  Complex c1(10, 20);
  Complex c2(5, 10);

  Complex c3 = c1 + c2;
  Complex c4 = c1 - c2;
  Complex c5 = c1 * c2;
  Complex c6 = c1 / c2;
  std::cout << c3 << std::endl;  // 15+30i
  std::cout << c4 << std::endl;  // 5+10i
  std::cout << c5 << std::endl;  // -150+200i
  std::cout << c6 << std::endl;  // 2+0i

  c3 -= c1;
  c4 += c2;
  c5 *= c4;
  c6 /= c2;
  std::cout << c3 << std::endl;  // 5+10i
  std::cout << c4 << std::endl;  // 10+20i
  std::cout << c5 << std::endl;  // -5500+-108000i
  std::cout << c6 << std::endl;  // 0.08+-0.0064i

  if (c4 == c1) {
    std::cout << "c1 == c2" << std::endl;
  }
  if (c1 != c2) {
    std::cout << "c1 != c2" << std::endl;
  }
  // c1 == c2
  // c1 != c2
}

void testN1() {
  func1();
  func2();
  func3();
  func4();
  func5();
}
}  // namespace n1

namespace n2 {
class Complex {
 public:
  Complex() : m_real(0.0), m_imag(0.0) {}
  Complex(double real, double imag) : m_real(real), m_imag(imag) {}
  // 在作为普通构造函数的同时，还能将double类型转换为Complex类型，
  // 集合了“构造函数”和“类型转换”的功能，所以被称为转换构造函数。
  Complex(double real) : m_real(real), m_imag(0.0) {}  // 转换构造函数

 public:
  friend Complex operator+(const Complex &c1, const Complex &c2);  // 全局函数
  friend std::ostream &operator<<(std::ostream &out, Complex &A);
  Complex operator-(const Complex &c) {  // 成员函数
    return Complex(this->m_real - c.m_real, this->m_imag - c.m_imag);
  }

 public:
  double real() const { return m_real; }
  double imag() const { return m_imag; }

 private:
  double m_real;
  double m_imag;
};
Complex operator+(const Complex &c1, const Complex &c2) {
  Complex c;
  c.m_real = c1.m_real + c2.m_real;
  c.m_imag = c1.m_imag + c2.m_imag;
  return c;
}
std::ostream &operator<<(std::ostream &out, Complex &A) {
  out << A.m_real << "+" << A.m_imag << "i";
  return out;
}

void testN2() {
  Complex c1(12, 34);
  Complex c2(43, 21);
  std::cout << c1 << "," << c2 << std::endl;  // 12+34i,43+21i

  Complex c3 = c1 + c2;
  Complex c4 = c2 + c1;
  std::cout << c3 << std::endl;  // 55+55i
  std::cout << c4 << std::endl;  // 55+55i

  Complex c5 = c1 + 32.0;
  Complex c6 = 32.0 + c1;
  std::cout << c5 << std::endl;  // 44+34i
  std::cout << c6 << std::endl;  // 44+34i
  // 为什么要以全局函数的形式重载+，而不是成员函数
  // 这样做是为了保证+运算符的操作数能够被对称的处理，
  // 换句话说，小数（double类型）在+左边和右边都是正确的。

  Complex c7 = c1 - c2;
  Complex c8 = c2 - c1;
  std::cout << c7 << std::endl;  // -31+13i
  std::cout << c8 << std::endl;  // 31+-13i

  Complex c9 = c2 - 11.0;  // 转换为：Complex c9 = c2.operator-(Complex(11.0));
  std::cout << c9 << std::endl;  // 32+21i
  // Complex c10 = 11.0 - c2;
  // 转换为：Complex c10 = (11.0).operator+(c2);
  // 显然是错误的，因为double类型并没有以成员函数的形式重载-
  // 也就是说，以成员函数的形式重载-，只能计算c2-11.0，不能计算11.0-c2，这是不对称的。

  // 有人问，编译器明明可以把11.0先转换成Complex类型再相加呀，也就是下面的形式：
  // Complex c10 = Complex(11.0).operator-(c2);
  // 为什么就是不转换呢？
  // 没错，编译器不会转换，原因在于，C++只会对成员函数的参数进行类型转换，
  // 而不会对调用成员函数的对象进行类型转换。
  // 以下面的语句为例：obj.func(params);
  // 编译器不会尝试对obj进行任何类型转换，它有func()成员函数就调用，没有就报错。
  // 而对于实参params，编译器会“拼命地”将它转换为形参的类型。
}
}  // namespace n2

namespace n3 {
// 把n1中的成员函数重载，定义成全局函数
class Complex {
 public:
  Complex(double real = 0.0, double imag = 0.0) : m_real(real), m_imag(imag) {}

  friend std::istream &operator>>(std::istream &in, Complex &A);
  friend std::ostream &operator<<(std::ostream &out, Complex &A);

 public:
  double real() const { return m_real; }
  double imag() const { return m_imag; }

 private:
  double m_real;  // 实部
  double m_imag;  // 虚部
};
std::istream &operator>>(std::istream &in, Complex &A) {
  in >> A.m_real >> A.m_imag;
  return in;
}
std::ostream &operator<<(std::ostream &out, Complex &A) {
  out << A.m_real << "+" << A.m_imag << "i";
  return out;
}

void testN3() {}
}  // namespace n3

int main(int argc, char *argv[]) {
  if (argc < 2) {
    std::cout << argv[0] << " i [0 - 2]" << std::endl;
    return 0;
  }
  int type = argv[1][0] - '0';
  switch (type) {
    case 0:
      n1::testN1();
      break;
    case 1:
      n2::testN2();
      break;
    case 2:
      n3::testN3();
      break;
    default:
      std::cout << "invalid type" << std::endl;
      break;
  }

  return 0;
}