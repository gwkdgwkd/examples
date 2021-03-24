#include <cmath>
#include <iomanip>
#include <iostream>

using namespace std;

// 四则运算符（+、-、*、/、+=、-=、*=、/=）和关系运算符（>、<、<=、>=、==、!=）都是数学运算符，它们在实际开发中非常常见，被重载的几率也很高，并且有着相似的重载格式。
// 复数能够进行完整的四则运算，但不能进行完整的关系运算：只能判断两个复数是否相等，但不能比较它们的大小，所以不能对>、<、<=、>=进行重载。

class Complex {
 public:
  Complex(double real = 0.0, double imag = 0.0) : m_real(real), m_imag(imag) {}

 public:
  friend Complex operator+(const Complex &c1, const Complex &c2);
  friend Complex operator-(const Complex &c1, const Complex &c2);
  friend Complex operator*(const Complex &c1, const Complex &c2);
  friend Complex operator/(const Complex &c1, const Complex &c2);
  friend bool operator==(const Complex &c1, const Complex &c2);
  friend bool operator!=(const Complex &c1, const Complex &c2);
  Complex &operator+=(const Complex &c);
  Complex &operator-=(const Complex &c);
  Complex &operator*=(const Complex &c);
  Complex &operator/=(const Complex &c);
  friend istream &operator>>(istream &in, Complex &A);
  friend ostream &operator<<(ostream &out, Complex &A);

 public:  //成员函数
  double real() const { return m_real; }
  double imag() const { return m_imag; }

 private:
  double m_real;  //实部
  double m_imag;  //虚部
};

Complex operator+(const Complex &c1, const Complex &c2) {
  Complex c;
  c.m_real = c1.m_real + c2.m_real;
  c.m_imag = c1.m_imag + c2.m_imag;
  return c;
}
//重载-运算符
Complex operator-(const Complex &c1, const Complex &c2) {
  Complex c;
  c.m_real = c1.m_real - c2.m_real;
  c.m_imag = c1.m_imag - c2.m_imag;
  return c;
}
// (a+bi) * (c+di) = (ac-bd) + (bc+ad)i
Complex operator*(const Complex &c1, const Complex &c2) {
  Complex c;
  c.m_real = c1.m_real * c2.m_real - c1.m_imag * c2.m_imag;
  c.m_imag = c1.m_imag * c2.m_real + c1.m_real * c2.m_imag;
  return c;
}
// (a+bi) / (c+di) = [(ac+bd) / (c²+d²)] + [(bc-ad) / (c²+d²)]i
Complex operator/(const Complex &c1, const Complex &c2) {
  Complex c;
  c.m_real = (c1.m_real * c2.m_real + c1.m_imag * c2.m_imag) /
             (pow(c2.m_real, 2) + pow(c2.m_imag, 2));
  c.m_imag = (c1.m_imag * c2.m_real - c1.m_real * c2.m_imag) /
             (pow(c2.m_real, 2) + pow(c2.m_imag, 2));
  return c;
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
Complex &Complex::operator+=(const Complex &c) {
  this->m_real += c.m_real;
  this->m_imag += c.m_imag;
  return *this;
}
Complex &Complex::operator-=(const Complex &c) {
  this->m_real -= c.m_real;
  this->m_imag -= c.m_imag;
  return *this;
}
Complex &Complex::operator*=(const Complex &c) {
  this->m_real = this->m_real * c.m_real - this->m_imag * c.m_imag;
  this->m_imag = this->m_imag * c.m_real + this->m_real * c.m_imag;
  return *this;
}
Complex &Complex::operator/=(const Complex &c) {
  this->m_real = (this->m_real * c.m_real + this->m_imag * c.m_imag) /
                 (pow(c.m_real, 2) + pow(c.m_imag, 2));
  this->m_imag = (this->m_imag * c.m_real - this->m_real * c.m_imag) /
                 (pow(c.m_real, 2) + pow(c.m_imag, 2));
  return *this;
}
istream &operator>>(istream &in, Complex &A) {
  in >> A.m_real >> A.m_imag;
  return in;
}
ostream &operator<<(ostream &out, Complex &A) {
  out << A.m_real << " + " << A.m_imag << " i ";
  return out;
}

// 以全局函数的形式重载了 +、-、*、/、==、!=，以成员函数的形式重载了 +=、-=、*=、/=，而且应该坚持这样做，不能一股脑都写作成员函数或者全局函数。
// Complex(double real);在作为普通构造函数的同时，还能将double类型转换为Complex类型，集合了“构造函数”和“类型转换”的功能，所以被称为「转换构造函数」。
// 换句话说，转换构造函数用来将其它类型（可以是bool、int、double等基本类型，也可以是数组、指针、结构体、类等构造类型）转换为当前类类型。
class Complex1 {
 public:
  Complex1() : m_real(0.0), m_imag(0.0) {}
  Complex1(double real, double imag) : m_real(real), m_imag(imag) {}
  Complex1(double real) : m_real(real), m_imag(0.0) {}  // 转换构造函数

 public:
  friend Complex1 operator+(const Complex1 &c1, const Complex1 &c2);
  Complex1 operator-(const Complex1 &c);

 public:
  double real() const { return m_real; }
  double imag() const { return m_imag; }

 private:
  double m_real;
  double m_imag;
};
Complex1 Complex1::operator-(const Complex1 &c) {
  return Complex1(this->m_real + c.m_real, this->m_imag + c.m_imag);
}

Complex1 operator+(const Complex1 &c1, const Complex1 &c2) {
  Complex1 c;
  c.m_real = c1.m_real + c2.m_real;
  c.m_imag = c1.m_imag + c2.m_imag;
  return c;
}

// 为什么要以全局函数的形式重载+
// operator+是一个全局函数（一个友元函数），而不是成员函数，这样做是为了保证+运算符的操作数能够被对称的处理；换句话说，小数（double类型）在+左边和右边都是正确的。
// 如果将operator+定义为成员函数，根据“+运算符具有左结合性”这条原则，Complex c2 = c1 + 15.6;会被转换为下面的形式：
// Complex c2 = c1.operator+(Complex(15.6));
// 这就是通过对象调用成员函数，是正确的。而对于Complex c3 = 28.23 + c1;，编译器会尝试转换为不同的形式：
// Complex c3 = (28.23).operator+(c1);
// 很显然这是错误的，因为double类型并没有以成员函数的形式重载+。
// 也就是说，以成员函数的形式重载+，只能计算c1 + 15.6，不能计算28.23 + c1，这是不对称的。
// 有人问，编译器明明可以把 28.23 先转换成 Complex 类型再相加呀，也就是下面的形式：
// Complex c3 = Complex(28.23).operator+(c1);
// 为什么就是不转换呢？没错，编译器不会转换，原因在于，C++只会对成员函数的参数进行类型转换，而不会对调用成员函数的对象进行类型转换。以下面的语句为例：
// obj.func(params);
// 编译器不会尝试对obj进行任何类型转换，它有func()成员函数就调用，没有就报错。而对于实参params，编译器会“拼命地”将它转换为形参的类型。

// 为什么要以成员函数的形式重载+=
// 首先要明白，运算符重载的初衷是给类添加新的功能，方便类的运算，它作为类的成员函数是理所应当的，是首选的。
// 不过，类的成员函数不能对称地处理数据，必须在（参与运算的）所有类型的内部都重载当前的运算符。所以C++进行了折中，允许以全局函数（友元函数）的形式重载运算符。
// C++创始人Bjarne Stroustrup也曾考虑过为内部类型（bool、int、double 等）定义额外运算符的问题，但后来还是放弃了这种想法，因为Bjarne Stroustrup不希
// 望改变现有规则：任何类型（无论是内部类型还是用户自定义类型）都不能在其定义完成以后再增加额外的操作。这里还有另外的一个原因，C内部类型之间的转换已经够肮脏
// 了，决不能再向里面添乱。而通过成员函数为已存在的类型提供混合运算的方式，从本质上看，比我们所采用的全局函数（友元函数）加转换构造函数的方式还要肮脏许多。
// 采用全局函数能使我们定义这样的运算符，它们的参数具有逻辑的对称性。与此相对应的，把运算符定义为成员函数能够保证在调用时对第一个（最左的）运算对象不出现类型
// 转换，也就是上面提到的「C++不会对调用成员函数的对象进行类型转换」。
// 总起来说，有一部分运算符重载既可以是成员函数也可以是全局函数，虽然没有一个必然的、不可抗拒的理由选择成员函数，但我们应该优先考虑成员函数，这样更符合运算符重载的初衷；
// 另外有一部分运算符重载必须是全局函数，这样能保证参数的对称性；除了 C++ 规定的几个特定的运算符外，暂时还没有发现必须以成员函数的形式重载的运算符。
// C++规定，箭头运算符->、下标运算符[]、函数调用运算符()、赋值运算符=只能以成员函数的形式重载。

// 在C++中，标准库本身已经对左移运算符<<和右移运算符>>分别进行了重载，使其能够用于不同数据的输入输出，但是输入输出的对象只能是C++内置的数据类型（例如bool、int、
// double等）和标准库所包含的类类型（例如string、complex、ofstream、ifstream等）。
// 如果我们自己定义了一种新的数据类型，需要用输入输出运算符去处理，那么就必须对它们进行重载。
// cout是ostream类的对象，cin是istream类的对象，要想达到这个目标，就必须以全局函数（友元函数）的形式重载<<和>>，否则就要修改标准库中的类，这显然不是我们所期望的。
// istream表示输入流，cin是istream类的对象，只不过这个对象是在标准库中定义的。之所以返回istream类对象的引用，是为了能够连续读取复数。
// ostream表示输出流，cout是ostream类的对象。由于采用了引用的方式进行参数传递，并且也返回了对象的引用，所以重载后的运算符可以实现连续输出。

// C++ 规定，下标运算符[]必须以成员函数的形式进行重载。该重载函数在类中的声明格式如下：
// 返回值类型 & operator[] (参数);
// const 返回值类型 & operator[] (参数) const;
// 使用第一种声明方式，[]不仅可以访问元素，还可以修改元素。使用第二种声明方式，[]只能访问而不能修改元素。在实际开发中，应该同时提供以上两种形式，
// 这样做是为了适应const对象，因为通过const对象只能调用const成员函数，如果不提供第二种形式，那么将无法访问const对象的任何元素。
class Array {
 public:
  Array(int length = 0);
  ~Array();

 public:
  int &operator[](int i);
  const int &operator[](int i) const;

 public:
  int length() const { return m_length; }
  void display() const;

 private:
  int m_length;
  int *m_p;
};
Array::Array(int length) : m_length(length) {
  if (length == 0) {
    m_p = NULL;
  } else {
    m_p = new int[length];
  }
}
Array::~Array() { delete[] m_p; }
int &Array::operator[](int i) { return m_p[i]; }
const int &Array::operator[](int i) const { return m_p[i]; }
void Array::display() const {
  for (int i = 0; i < m_length; i++) {
    if (i == m_length - 1) {
      cout << m_p[i] << endl;
    } else {
      cout << m_p[i] << ", ";
    }
  }
}

// 自增++和自减--都是一元运算符，它的前置形式和后置形式都可以被重载。
class stopwatch {
 public:
  stopwatch() : m_min(0), m_sec(0) {}

 public:
  void setzero() {
    m_min = 0;
    m_sec = 0;
  }
  stopwatch run();
  stopwatch operator++();     // ++i，前置形式
  stopwatch operator++(int);  // i++，后置形式
  friend ostream &operator<<(ostream &, const stopwatch &);

 private:
  int m_min;  //分钟
  int m_sec;  //秒钟
};
stopwatch stopwatch::run() {
  ++m_sec;
  if (m_sec == 60) {
    m_min++;
    m_sec = 0;
  }
  return *this;
}
// operator++()函数实现自增的前置形式，直接返回run()函数运行结果即可。
stopwatch stopwatch::operator++() { return run(); }
// operator++ (int n)函数实现自增的后置形式，返回值是对象本身，但是之后再次使用该对象时，对象自增了，
// 所以在该函数的函数体中，先将对象保存，然后调用一次run()函数，之后再将先前保存的对象返回。
// 在这个函数中参数n是没有任何意义的，它的存在只是为了区分是前置形式还是后置形式。
stopwatch stopwatch::operator++(int n) {
  stopwatch s = *this;
  run();
  return s;
}
ostream &operator<<(ostream &out, const stopwatch &s) {
  out << setfill('0') << setw(2) << s.m_min << ":" << setw(2) << s.m_sec;
  return out;
}

// 内存管理运算符new、new[]、delete和delete[]也可以进行重载，其重载形式既可以是类的成员函数，也可以是全局函数。
// 一般情况下，内建的内存管理运算符就够用了，只有在需要自己管理内存时才会重载。
//  void *className::operator new(size_t size){}  // 以成员函数的形式重载new运算符
//  void *operator new(size_t size){}  // 以全局函数的形式重载new运算符
// 两种重载形式的返回值相同，都是void *类型，并且都有一个参数，为size_t类型。在重载new或new[]时，无论是作为成员函数还是作为全局函数，它的第一个参数必须是size_t类型。
// size_t表示的是要分配空间的大小，对于new[]的重载函数而言，size_t则表示所需要分配的所有空间的总和。size_t在头文件<cstdio>中被定义为typedef unsigned int size_t;
// 重载函数也可以有其他参数，但都必须有默认值，并且第一个参数的类型必须是size_t。
// delete运算符也有两种重载形式:
//  void className::operator delete( void *ptr){}  // 以类的成员函数的形式进行重载
//  void operator delete( void *ptr){}  // 以全局函数的形式进行重载
// 两种重载形式的返回值都是void类型，并且都必须有一个void类型的指针作为参数，该指针指向需要释放的内存空间。
// 如果类中没有定义new和delete的重载函数，那么会自动调用内建的new和delete运算符。

// 在C++中，类型的名字（包括类的名字）本身也是一种运算符，即类型强制转换运算符。
// 类型强制转换运算符是单目运算符，也可以被重载，但只能重载为成员函数，不能重载为全局函数。
// 经过适当重载后，(类型名)对象这个对对象进行强制类型转换的表达式就等价于对象.operator类型名()，即变成对运算符函数的调用。
class Complex2 {
  double real, imag;

 public:
  Complex2(double r = 0, double i = 0) : real(r), imag(i){};
  // 对double运算符重载，在本该出现double类型的变量或常量的地方，如果出现了一个Complex类型的对象，那么该对象的operator double成员函数就会被调用，然后取其返回值使用。
  operator double() { return real; }  // 重载强制类型转换运算符double
};

int main() {
  Complex c1(25, 35);
  Complex c2(10, 20);
  Complex c3(1, 2);
  Complex c4(4, 9);
  Complex c5(34, 6);
  Complex c6(80, 90);

  Complex c7 = c1 + c2;
  Complex c8 = c1 - c2;
  Complex c9 = c1 * c2;
  Complex c10 = c1 / c2;
  cout << "c7 = " << c7.real() << " + " << c7.imag() << "i" << endl;
  cout << "c8 = " << c8.real() << " + " << c8.imag() << "i" << endl;
  cout << "c9 = " << c9.real() << " + " << c9.imag() << "i" << endl;
  cout << "c10 = " << c10.real() << " + " << c10.imag() << "i" << endl;

  c3 += c1;
  c4 -= c2;
  c5 *= c2;
  c6 /= c2;
  cout << "c3 = " << c3.real() << " + " << c3.imag() << "i" << endl;
  cout << "c4 = " << c4.real() << " + " << c4.imag() << "i" << endl;
  cout << "c5 = " << c5.real() << " + " << c5.imag() << "i" << endl;
  cout << "c6 = " << c6.real() << " + " << c6.imag() << "i" << endl;

  if (c1 == c2) {
    cout << "c1 == c2" << endl;
  }
  if (c1 != c2) {
    cout << "c1 != c2" << endl;
  }
  // c7 = 35 + 55i
  // c8 = 15 + 15i
  // c9 = -450 + 850i
  // c10 = 1.9 + -0.3i
  // c3 = 26 + 37i
  // c4 = -6 + -11i
  // c5 = 220 + 4460i
  // c6 = 5.2 + 1.592i
  // c1 != c2

  Complex1 c11(25, 35);
  // 小数被转换成了匿名的Complex对象。
  // 在这个转换过程中，构造函数Complex(double real);起到了至关重要的作用，如果没有它，转换就会失败，Complex也不能和double相加。
  Complex1 c12 = c11 + 15.6;
  Complex1 c13 = 28.23 + c11;
  cout << c12.real() << " + " << c12.imag() << "i" << endl;  // 40.6 + 35i
  cout << c13.real() << " + " << c13.imag() << "i" << endl;  // 53.23 + 35i
  // Complex1 c14 = 28.23 - c11;  // 成员函数，不对称的。
  Complex1 c15 = c11 - 28.23;
  cout << c15.real() << " + " << c15.imag() << "i" << endl;  // 53.23 + 35i

  Complex c21, c22, c23;
  cin >> c21 >> c22;
  c23 = c21 + c22;
  cout << "c1 + c2 = " << c23 << endl;
  c23 = c21 - c22;
  cout << "c1 - c2 = " << c23 << endl;
  c23 = c1 * c22;
  cout << "c1 * c2 = " << c23 << endl;
  c23 = c21 / c22;
  cout << "c1 / c2 = " << c23 << endl;
  // c1 + c2 = 244 + 66i
  // c1 - c2 = -222 + -22i
  // c1 * c2 = 4285 + 9255i
  // c1 / c2 = 0.0628012 + 0.0825611i

  Array A(5);
  for (int i = 0, len = A.length(); i < len; i++) {
    A[i] = i * 5;
  }
  A.display();
  const Array B(5);
  // 需要说明的是，B是const对象，如果Array类没有提供const版本的operator[]，那么代码将报错。 虽然只是读取对象的数据，并没有试图修改对象，
  // 但是它调用了非const版本的operator[]，编译器不管实际上有没有修改对象，只要是调用了非const的成员函数，编译器就认为会修改对象（至少有这种风险）。
  cout << B[5 - 1] << endl;
  // 重载[]运算符以后，表达式arr[i]会被转换为：arr.operator[](i);

  stopwatch s1, s2;
  s1 = s2++;
  cout << "s1: " << s1 << endl;  // 00:00
  cout << "s2: " << s2 << endl;  // 00:01
  s1.setzero();
  s2.setzero();
  s1 = ++s2;
  cout << "s1: " << s1 << endl;  // 00:01
  cout << "s2: " << s2 << endl;  // 00:01

  Complex2 cc(1.2, 3.4);
  cout << (double)cc << endl;  // 1.2  (double)c等价于c.operator double()
  double nn = 2 + cc;          // 等价于double n = 2 + c.operator double()
  cout << nn << endl;          // 3.2

  return 0;
}