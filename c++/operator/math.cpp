#include <cmath>
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

  return 0;
}