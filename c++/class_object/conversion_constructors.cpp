#include <iostream>

using namespace std;

// 在C/C++中，不同的数据类型之间可以相互转换。无需用户指明如何转换的称为自动类型转换（隐式类型转换），需要用户显式地指明如何转换的称为强制类型转换。
// 不管是自动类型转换还是强制类型转换，前提必须是编译器知道如何转换，例如，将小数转换为整数会抹掉小数点后面的数字，将int*转换为float*只是简单地复制指针的值，
// 这些规则都是编译器内置的，我们并没有告诉编译器。换句话说，如果编译器不知道转换规则就不能转换，使用强制类型也无用。
// C++允许我们自定义类型转换规则，用户可以将其它类型转换为当前类类型，也可以将当前类类型转换为其它类型。这种自定义的类型转换规则只能以类的成员函数的形式出现，
// 换句话说，这种转换规则只适用于类。
// 将其它类型转换为当前类类型需要借助转换构造函数（Conversion constructor）。转换构造函数也是一种构造函数，它遵循构造函数的一般规则。转换构造函数只有一个参数。
// 转换构造函数也是构造函数的一种，它除了可以用来将其它类型转换为当前类类型，还可以用来初始化对象，这是构造函数本来的意义。
class Complex {
 public:
  Complex() : m_real(0.0), m_imag(0.0) {}
  Complex(double real, double imag) : m_real(real), m_imag(imag) {}
  // 将double类型的参数real转换成Complex类的对象，并将real作为复数的实部，将0作为复数的虚部。
  Complex(double real) : m_real(real), m_imag(0.0) {}  // 转换构造函数
  // 在进行数学运算、赋值、拷贝等操作时，如果遇到类型不兼容、需要将double类型转换为Complex类型时，编译器会检索当前的类是否定义了转换构造函数，如果没有定义的话就
  // 转换失败，如果定义了的话就调用转换构造函数。

  // 借助函数的默认参数，可以将这三个(两个普通的构造函数和一个转换构造函数)构造函数简化为一个
  // Complex(double real = 0.0, double imag = 0.0) : m_real(real), m_imag(imag) {}
  // 精简后的构造函数包含了两个默认参数，在调用它时可以省略部分或者全部实参，也就是可以向它传递0个、1个、2个实参。
  // 转换构造函数就是包含了一个参数的构造函数，恰好能够和其他两个普通的构造函数“融合”在一起。

  Complex operator+(const Complex &c);

 public:
  friend ostream &operator<<(ostream &out, Complex &c);  // 友元函数
 private:
  double m_real;
  double m_imag;
};
ostream &operator<<(ostream &out, Complex &c) {
  out << c.m_real << " + " << c.m_imag << "i";
  return out;
}
Complex Complex::operator+(const Complex &c) {
  return Complex(this->m_real + c.m_real, this->m_imag + c.m_imag);
}

// 构造函数的本意是在创建对象的时候初始化对象，编译器会根据传递的实参来匹配不同的（重载的）构造函数。
// 1) 默认构造函数。就是编译器自动生成的构造函数。
// Complex();  // 没有参数
// 2) 普通构造函数。就是用户自定义的构造函数。
// Complex(double real, double imag);  // 两个参数
// 3) 拷贝构造函数。在以拷贝的方式初始化对象时调用。
// Complex(const Complex &c);
// 4) 转换构造函数。将其它类型转换为当前类类型时调用。
// Complex(double real);
// 不管哪一种构造函数，都能够用来初始化对象，这是构造函数的本意。假设Complex类定义了以上所有的构造函数，那么下面创建对象的方式都是正确的：
//     Complex c1();  // 调用Complex()
//     Complex c2(10, 20);  // 调用Complex(double real, double imag)
//     Complex c3(c2);  // 调用Complex(const Complex &c)
//     Complex c4(25.7);  // 调用Complex(double real)
// 这些代码都体现了构造函数的本意——在创建对象时初始化对象。
// 除了在创建对象时初始化对象，其他情况下也会调用构造函数，例如，以拷贝的的方式初始化对象时会调用拷贝构造函数，将其它类型转换为当前类类型时会调用转换构造函数。
// 这些在其他情况下调用的构造函数，就成了特殊的构造函数了。特殊的构造函数并不一定能体现出构造函数的本意。

// 转换构造函数能够将其它类型转换为当前类类型（将double类型转换为Complex类型），但是不能反过来将当前类类型转换为其它类型（将Complex类型转换为double类型）。
// 类型转换函数（Type conversion function）解决了这个问题。作用就是将当前类类型转换为其它类型，它只能以成员函数的形式出现，也就是只能出现在类中。
// 类型转换函数的语法格式为： operator type(){ return data;}
// operator是C++关键字，type是要转换的目标类型，data是要返回的type类型的数据。
// 因为要转换的目标类型是type，所以返回值data也必须是type类型。既然已经知道了要返回type类型的数据，所以没有必要再像普通函数一样明确地给出返回值类型。
// 这样做导致的结果是：类型转换函数看起来没有返回值类型，其实是隐式地指明了返回值类型。
// 类型转换函数也没有参数，因为要将当前类的对象转换为其它类型，所以参数不言而喻。实际上编译器会把当前对象的地址赋值给this指针，这样在函数体内就可以操作当前对象了。
// 类型转换函数和运算符的重载非常相似，都使用operator关键字，因此也把类型转换函数称为类型转换运算符。
// 关于类型转换函数的说明:
//  1) type可以是内置类型、类类型以及由typedef定义的类型别名，任何可作为函数返回类型的类型（void除外）都能够被支持。
//     一般而言，不允许转换为数组或函数类型，转换为指针类型或引用类型是可以的。
//  2) 类型转换函数一般不会更改被转换的对象，所以通常被定义为const成员。
//  3) 类型转换函数可以被继承，可以是虚函数。
//  4) 一个类虽然可以有多个类型转换函数（类似于函数重载），但是如果多个类型转换函数要转换的目标类型本身又可以相互转换（类型相近），那么有时候就会产生二义性。
// 假设它有两个类型转换函数：
//  operator double() const { return m_real; }  // 转换为double类型
//  operator int() const { return (int)m_real; }  // 转换为int类型
// 那么下面的写法就会引发二义性：
//   Complex c1(24.6, 100);
//   float f = 12.5 + c1;
// 编译器可以调用operator double()将c1转换为double类型，也可以调用operator int()将c1转换为int类型，这两种类型都可以跟12.5进行加法运算，并且从Complex
// 转换为double与从Complex转化为int是平级的，没有谁的优先级更高，所以这个时候编译器就不知道该调用哪个函数了，干脆抛出一个二义性错误，让用户解决。
class Complex1 {
 public:
  Complex1() : m_real(0.0), m_imag(0.0) {}
  Complex1(double real, double imag) : m_real(real), m_imag(imag) {}

 public:
  friend ostream &operator<<(ostream &out, Complex1 &c);
  friend Complex1 operator+(const Complex1 &c1, const Complex1 &c2);
  // 类型转换函数非常简单，就是返回成员变量m_real的值，所以建议写成inline的形式。
  operator double() const { return m_real; }  // 类型转换函数
 private:
  double m_real;
  double m_imag;
};
ostream &operator<<(ostream &out, Complex1 &c) {
  out << c.m_real << " + " << c.m_imag << "i";
  return out;
}
Complex1 operator+(const Complex1 &c1, const Complex1 &c2) {
  Complex1 c;
  c.m_real = c1.m_real + c2.m_real;
  c.m_imag = c1.m_imag + c2.m_imag;
  return c;
}

// 转换构造函数和类型转换函数的作用是相反的：转换构造函数会将其它类型转换为当前类类型，类型转换函数会将当前类类型转换为其它类型。
// 如果没有这两个函数，Complex类和int、double、bool等基本类型的四则运算、逻辑运算都将变得非常复杂，要编写大量的运算符重载函数。
// 但是，如果一个类同时存在这两个函数，就有可能产生二义性。
// 解决二义性问题的办法也很简单粗暴，要么只使用转换构造函数，要么只使用类型转换函数。实践证明，用户对转换构造函数的需求往往更加强烈，这样能增加编码的灵活性，
// 例如，可以将一个字符串字面量或者一个字符数组直接赋值给string类的对象，可以将一个int、double、bool等基本类型的数据直接赋值给Complex类的对象。
// 那么，如果我们想把当前类类型转换为其它类型怎么办呢？很简单，增加一个普通的成员函数即可，例如，string类使用c_str()函数转换为C风格的字符串，complex类使
// 用real()和imag()函数来获取复数的实部和虚部。
class Complex2 {
 public:
  Complex2(double real = 0.0, double imag = 0.0)
      : m_real(real), m_imag(imag) {}  // 包含了转换构造函数
 public:
  friend ostream &operator<<(ostream &out, Complex2 &c);
  friend Complex2 operator+(const Complex2 &c1, const Complex2 &c2);
  operator double() const { return m_real; }  // 类型转换函数
 private:
  double m_real;
  double m_imag;
};
ostream &operator<<(ostream &out, Complex2 &c) {
  out << c.m_real << " + " << c.m_imag << "i";
  return out;
}
Complex2 operator+(const Complex2 &c1, const Complex2 &c2) {
  Complex2 c;
  c.m_real = c1.m_real + c2.m_real;
  c.m_imag = c1.m_imag + c2.m_imag;
  return c;
}

// 一个实用的Complex类能够进行四则运算和关系运算，需要重载+、-、*、/、+=、-=、*=、/=、==、!=这些运算符，不过这里仅仅重载了+、+=、==、!=运算符
class Complex3 {
 public:
  Complex3(double real = 0.0, double imag = 0.0) : m_real(real), m_imag(imag) {}

 public:
  friend ostream &operator<<(ostream &out, Complex3 &c);
  friend istream &operator>>(istream &in, Complex3 &c);
  friend Complex3 operator+(const Complex3 &c1, const Complex3 &c2);
  friend bool operator==(const Complex3 &c1, const Complex3 &c2);
  friend bool operator!=(const Complex3 &c1, const Complex3 &c2);
  Complex3 &operator+=(const Complex3 &c);

 public:
  double real() const { return m_real; }
  double imag() const { return m_imag; }

 private:
  double m_real;
  double m_imag;
};
ostream &operator<<(ostream &out, Complex3 &c) {
  out << c.m_real << " + " << c.m_imag << "i";
  ;
  return out;
}
istream &operator>>(istream &in, Complex3 &c) {
  in >> c.m_real >> c.m_imag;
  return in;
}
Complex3 operator+(const Complex3 &c1, const Complex3 &c2) {
  Complex3 c;
  c.m_real = c1.m_real + c2.m_real;
  c.m_imag = c1.m_imag + c2.m_imag;
  return c;
}
Complex3 &Complex3::operator+=(const Complex3 &c) {
  this->m_real += c.m_real;
  this->m_imag += c.m_imag;
  return *this;
}
bool operator==(const Complex3 &c1, const Complex3 &c2) {
  if (c1.m_real == c2.m_real && c1.m_imag == c2.m_imag) {
    return true;
  } else {
    return false;
  }
}
bool operator!=(const Complex3 &c1, const Complex3 &c2) {
  if (c1.m_real != c2.m_real || c1.m_imag != c2.m_imag) {
    return true;
  } else {
    return false;
  }
}

int main() {
  // 编译器对7.5是作为double类型处理的，在求解表达式时，先将a转换为double类型，然后与7.5相加，得到和为13.5。
  // 在向整型变量a赋值时，将13.5转换为整数13，然后赋给a。整个过程中，我们并没有告诉编译器如何去做，编译器使用内置的规则完成数据类型的转换。
  int a = 6;
  a = 7.5 + a;

  // p1是int*类型，它指向的内存里面保存的是整数，p2是float*类型，将p1赋值给p2后，p2也指向了这块内存，并把这块内存中的数据作为小数处理。
  // 我们知道，整数和小数的存储格式大相径庭，将整数作为小数处理非常荒诞，可能会引发莫名其妙的错误，所以编译器默认不允许将p1赋值给p2。
  // 但是，使用强制类型转换后，编译器就认为我们知道这种风险的存在，并进行了适当的权衡，所以最终还是允许了这种行为。
  int n = 100;
  int *p1 = &n;
  float *p2 = (float *)p1;

  Complex a1(10.0, 20.0);
  // 25.5是实数，a是复数，将25.5赋值给a后，我们期望a的实部变为25.5，而虚部为0。
  // 但是，编译器并不知道这个转换规则，这超出了编译器的处理能力，所以转换失败，即使加上强制类型转换也无用。
  // a1 = (Complex)25.5;  // 错误，转换失败

  Complex a2(10.0, 20.0);
  cout << a2 << endl;  // 10 + 20i
  a2 = 25.5;           // 调用转换构造函数，相当于a2.Complex(25.5);
  cout << a2 << endl;  // 25.5 + 0i

  Complex c1(26.4);  // 创建具名对象
  // 在以拷贝的方式初始化对象时，编译器先调用转换构造函数，将240.3转换为Complex类型（创建一个Complex类的匿名对象），然后再拷贝给c2。
  Complex c2 = 240.3;  // 以拷贝的方式初始化对象
  Complex(15.9);       // 创建匿名对象
  c1 = Complex(46.9);  // 创建一个匿名对象并将它赋值给c1

  // 如果已经对+运算符进行了重载，使之能进行两个Complex类对象的相加，那么下面的语句也是正确的：
  Complex cc1(15.6, 89.9);
  Complex cc2;
  // 在进行加法运算符时，编译器先将29.6转换为Complex类型（创建一个Complex类的匿名对象）再相加。
  cc2 = cc1 + 29.6;
  cout << cc2 << endl;  // 45.2 + 89.9i

  // 需要注意的是，为了获得目标类型，编译器会“不择手段”，会综合使用内置的转换规则和用户自定义的转换规则，并且会进行多级类型转换，例如：
  //  编译器会根据内置规则先将int转换为double，再根据用户自定义规则将double转换为Complex（int-->double-->Complex）；
  //  编译器会根据内置规则先将char转换为int，再将int转换为double，最后根据用户自定义规则将double转换为Complex（char-->int-->double-->Complex）。
  // 只要一个类型能转换为double类型，就能转换为Complex类型
  Complex c11 = 100;    // int --> double --> Complex
  cout << c11 << endl;  // 100 + 0i
  c11 = 'A';            // char --> int --> double --> Complex
  cout << c11 << endl;  // 65 + 0i
  c11 = true;           // bool --> int --> double --> Complex
  cout << c11 << endl;  // 1 + 0i
  Complex c22(25.8, 0.7);
  c11 = c22 + 'H' + true + 15;  // 将char、bool、int都转换为Complex类型再运算
  cout << c11 << endl;  // 113.8 + 0.7i

  Complex1 cx1(24.6, 100);
  double f = cx1;  // 相当于double f = Complex::operator double(&c1);
  cout << "f = " << f << endl;  // f = 24.6
  f = 12.5 + cx1 + 6;  // 相当于f = 12.5 + Complex::operator double(&c1) + 6;
  cout << "f = " << f << endl;   // f = 43.1
  int nn = Complex1(43.2, 9.3);  // 先转换为double，再转换为int
  cout << "n = " << nn << endl;  // n = 43

  Complex2 cq1(24.6, 100);
  double fa = cq1;  // 调用类型转换函数
  cq1 = 78.4;       // 调用转换构造函数
  // 进行加法运算时，有两种转换方案：
  //  第一种方案是先将12.5转换为Complex类型再运算，这样得到的结果也是Complex类型，再调用类型转换函数就可以赋值给f了。
  //  第二种方案是先将c1转换为double类型再运算，这样得到的结果也是double类型，可以直接赋值给f。
  // f = 12.5 + cq1;  // 产生二义性
  // 很多读者会认为，既然=左边是double类型，很显然应该选择第二种方案，这样才符合“常理”。其实不然，编译器不会根据=左边的数据类型来选择转换方案，编译器只关
  // 注12.5+c1这个表达式本身，站在这个角度考虑，上面的两种转换方案都可以，编译器不知道选择哪一种，所以会抛出二义性错误，让用户自己去解决。
  // 即使我们假设编译器会根据=左边的数据类型来选择解决方案，那仍然会存在二义性问题，下面就是一个例子：cout<<c1 + 46.7<<endl;
  // Complex2 cq2 = cq1 + 46.7;  // 产生二义性

  Complex3 cz1(12, 60);
  cout << "cz1 = " << cz1 << endl;
  // 先调用转换构造函数将22.8 转换为Complex类型，再调用重载过的+运算符
  Complex3 cz2 = cz1 + 22.8;
  cout << "c2 = " << cz2 << endl;
  // 同上
  Complex3 cz3 = 8.3 + cz1;
  cout << "c3 = " << cz3 << endl;
  // 先调用转换构造函数将 73 转换为 Complex 类型，再调用重载过的 += 运算符
  Complex3 cz4(4, 19);
  cz4 += 73;
  cout << "c4 = " << cz4 << endl;
  // 调用重载过的+=运算符
  Complex3 cz5(14.6, 26.2);
  cz5 += cz1;
  cout << "c5 = " << cz5 << endl;
  // 调用重载过的==运算符
  if (cz1 == cz2) {
    cout << "c1 == c2" << endl;
  } else {
    cout << "c1 != c2" << endl;
  }
  // 先调用转换构造函数将77转换为Complex类型，再调用重载过的!=运算符
  if (cz4 != 77) {
    cout << "c4 != 77" << endl;
  } else {
    cout << "c4 == 77" << endl;
  }
  // 将Complex转换为double，没有调用类型转换函数，而是调用了real()这个普通的成员函数
  double ff = cz5.real();
  cout << "f = " << ff << endl;

  return 0;
}