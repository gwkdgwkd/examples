#include <iostream>
#include <typeinfo>

// 一般情况下，在编译期间就能确定一个表达式的类型，
// 但是当存在多态时，有些表达式的类型在编译期间就无法确定了，
// 必须等到程序运行后根据实际的环境来确定。
// C++的对象内存模型主要包含了以下几个方面的内容：
// 1.如果没有虚函数也没有虚继承，那么对象内存模型中只有成员变量；
// 2.如果类包含了虚函数，那么会额外添加一个虚函数表，
//   并在对象内存中插入一个指针，指向这个虚函数表；
// 3.如果类包含了虚继承，那么会额外添加一个虚基类表，
//   并在对象内存中插入一个指针，指向这个虚基类表；
// 4.如果类包含了虚函数，那么内存中还会额外增加类型信息，即type_info对象。

// 编译器会在虚函数表vftable的开头插入一个指针，指向当前类对应的type_info对象。
// 当程序在运行阶段获取类型信息时，可以通过对象指针p找到虚函数表指针vfptr，
// 再通过vfptr找到type_info对象的指针，进而取得类型信息。
// 编译器在编译阶段无法确定p指向哪个对象，也就无法获取*p的类型信息，
// 但编译器可以在编译阶段做好各种准备，在运行后可以借助准备好的数据来获取类型信息。
// 包括：
// 1.创建type_info对象，并在vftable的开头插入一个指针，指向type_info对象；
// 2.将获取类型信息的操作转换成类似**(p->vfptr-1)这样的语句。
// 这样做虽然会占用更多的内存，效率也降低了，但这没办法，编译器实在是无能为力了。
// 这种在程序运行后确定对象的类型信息的机制称为运行时类型识别，
// Run-Time Type Identification，RTTI。
// C++中，只有类包含了虚函数才启用RTTI机制，其他情况都可以在编译阶段确定类型信息。
// 多态（Polymorphism）是面向对象编程的一个重要特征，它极大地增加了程序的灵活性，
// C++、C#、Java等正统的面向对象编程语言都支持多态。
// 但是支持多态的代价也很大，有些信息在编译阶段无法确定下来，必须提前做好充足准备，
// 让程序运行后再执行一段代码获取，这会消耗更多的内存和CPU资源。

// C/C++用变量来存储数据，用函数来定义一段可以重复使用的代码，
// 它们最终都要放到内存中才能供CPU使用。
// CPU通过地址来取得内存中的代码和数据，
// 程序在执行过程中会告知CPU要执行的代码以及要读写的数据的地址。
// CPU访问内存需要的是地址，而不是变量名和函数名，它们只是地址的一种助记符，
// 当源文件被编译和链接成可执行程序后，它们都会被替换成地址。
// 编译和链接过程的一项重要任务就是找到这些名称所对应的地址。
// 变量名和函数名提供了方便，让在编写代码的过程中可以使用易于阅读和理解的英文字符串，
// 不用直接面对二进制地址，那场景简直让人崩溃。
// 不妨将变量名和函数名统称为符号（Symbol），找到符号对应的地址的过程叫做符号绑定。
// 函数调用是执行函数体中的代码，函数体是内存中的一个代码段，
// 函数名是该代码段的首地址，函数执行时就从这里开始。
// 简单说就是必须要知道函数的入口地址，才能成功调用函数。
// 找到函数名对应的地址，然后将函数调用处用该地址替换，这称为函数绑定。
// 一般情况下，在编译期间（包括链接期间）就能找到函数名对应的地址，
// 完成函数的绑定，程序运行后直接使用这个地址即可，这称为静态绑定。
// 但是有时候在编译期间想尽所有办法都不能确定使用哪个函数，
// 必须要等到程序运行后根据具体的环境或者用户操作才能决定，这称为动态绑定。
// C++是一门静态性的语言，会尽力在编译期间找到函数的地址，以提高程序的运行效率，
// 但是有时候实在没办法，只能等到程序运行后再执行一段很少的代码才能找到函数的地址。
// 动态绑定的本质：
// 编译器在编译期间不能确定指针指向哪个对象，只能等到程序运行后根据具体的情况再决定。

// 在C++中，typeid运算符，dynamic_cast运算符和异常处理都依赖RTTI机制，
// 并且要能够通过派生类获取基类的信息，或者说要能够判断一个类是否是另一个类的基类，
// 必须要在基类和派生类之间再增加一条绳索，把它们连接起来，形成一条通路，
// 让程序在各个对象之间游走，在面向对象语言中，称此为继承链（Inheritance Chain）。
// 将基类和派生类连接起来很容易，只需要在基类对象中增加一个指向派生类对象的指针，
// 然而考虑到多继承、降低内存使用等诸多方面的因素，
// 真正的对象内存模型很复杂，C++标准并没有对对象内存模型的细节做出规定。
// 对于有虚函数的类，内存模型中除了有虚函数表，还会额外增加好几个表，
// 以维护当前类和基类的信息，空间上的开销不小。

// typeid(type).name()方法返回的类名就来自当前类的信息表。
// typeid经过固定次数的间接转换返回type_info对象，
// 间接次数不会随着继承层次的增加而增加，对效率的影响很小，可以放心使用。
// 而dynamic_cast运算符和异常处理不仅要经过数次间接转换，
// 还要遍历继承链，如果继承层次较深，那么它们的性能堪忧，应当谨慎使用。
// 类型是表达式的一个属性，不同的类型支持不同的操作，
// 类型对于编程语言来说非常重要，编译器内部有一个类型系统来维护表达式的各种信息。
// 在C/C++中，变量、函数参数、函数返回值等在定义时都必须显式地指明类型，
// 并且一旦指明类型后就不能再更改了，所以大部分表达式的类型都能够精确的推测出来，
// 编译器在编译期间就能够搞定这些事情，这样的编程语言称为静态语言。
// 除了C/C++，典型的静态语言还有Java、C#、Haskell、Scala等。
// 静态语言在定义变量时通常需要显式地指明类型，
// 并且在编译期间会拼尽全力来确定表达式的类型信息，
// 只有在万不得已时才让程序等到运行后动态地获取类型信息（例如多态），
// 这样做可以提高程序运行效率，降低内存消耗。
// 与静态语言（Static Language）相对的是动态语言（Dynamic Language）。
// 动态语言在定义变量时往往不需要指明类型，并且变量的类型可以根据赋值随时改变，
// 编译器在编译期间也不容易确定表达式的类型信息，只能等到程序运行后再动态地获取。
// 典型的动态语言有JavaScript、Python、PHP、Perl、Ruby等。
// 动态语言为了能够使用灵活，部署简单，往往是边编译边执行，模糊了编译和运行的过程。
// 总起来说，静态语言由于类型的限制会降低编码的速度，
// 但是它的执行效率高，适合开发大型的、系统级的程序；
// 动态语言则比较灵活，编码简单，部署容易，在Web开发中大显身手。

// typeid运算符用来获取一个表达式的类型信息。
// 类型信息对于编程语言非常重要，它描述了数据的各种属性：
// 1.对于基本类型（int、float 等C++内置类型）的数据，
//   类型信息所包含的内容比较简单，主要是指数据的类型。
// 2.对于类类型的数据（也就是对象），
//   类型信息是指对象所属的类、所包含的成员、所在的继承关系等。
// 类型信息是创建数据的模板，数据占用多大内存、能进行什么样的操作、
// 该如何操作等，这些都由它的类型信息决定。

namespace n1 {
class B {
 public:
  virtual void test() {}
};
class D : public B {};

void func() {
  B *p;
  int n;

  std::cin >> n;
  if (n == 1) {
    p = new B();
  } else {
    p = new D();
  }
  if (typeid(*p) == typeid(B)) {
    std::cout << "B" << std::endl;
  } else {
    std::cout << "D" << std::endl;
  }

  // >> 1
  // B

  // >> 2
  // D
}
}  // namespace n1

class Base {};
struct STU {};
class Derived : public Base {};

namespace n2 {
// typeid的操作对象既可以是表达式，也可以是数据类型，这和sizeof运算符非常类似，
// 只不过sizeof有时候可以省略括号()，而typeid必须带上括号。
// typeid把获得的类型信息保存到一个type_info类型对象里，并返回该对象的常引用；
// 当需要具体的类型信息时，可以通过成员函数来提取。
// typeid操作数可以是普通变量、对象、内置类型、自定义类型，还可以是一个表达式。

// 成员函数：
// 1.name()用来返回类型的名称；
// 2.raw_name()用来返回名字编码（Name Mangling）算法产生的新名称；
// 3.hash_code()用来返回当前类型对应的hash值；
//   hash值是一个可以用来标志当前类型的整数，
//   有点类似学生的学号、公民的身份证号、银行卡号等。
//   不过hash值有赖于编译器的实现，在不同的编译器下可能会有不同的整数，
//   但它们都能唯一地标识某个类型。
//   遗憾的是，C++标准只对type_info类做了很有限的规定，
//   不仅成员函数少，功能弱，而且各个平台的实现不一致。
//   hash_code()在VC/VS和较新的GCC下有效。
// 4.raw_name()是VC/VS独有的一个成员函数，

// C++标准规定，type_info类至少要有4个public属性的成员函数，其他扩展不限制：
// 1.const char* name() const;
//   返回一个能表示类型名称的字符串，但是C++标准并没有规定这个字符串是的格式。
// 2.bool before(const type_info& rhs) const;
//   判断一个类型是否位于另一个类型的前面，rhs参数是一个type_info对象的引用。
//   但是C++标准并没有规定类型的排列顺序，
//   不同的编译器有不同的排列规则，程序员也可以自定义。
//   这个排列顺序和继承顺序没有关系，基类并不一定位于派生类的前面。
// 3.bool operator==(const type_info& rhs) const;
//   重载运算符==，判断两个类型是否相同，rhs参数是一个type_info对象的引用。
// 4.bool operator!= (const type_info& rhs) const;
//   重载运算符!=，判断两个类型是否不同，rhs参数是一个type_info对象的引用。
// 可以发现，不像Java、C#等动态性较强的语言，C++能获取到的类型信息非常有限，
// 也没有统一的标准，大部分情况下只是使用重载过的==运算符来判断两个类型是否相同。

void func1() {
  // 获取一个普通变量的类型信息：
  int n = 100;
  const std::type_info &nInfo = typeid(n);
  std::cout << nInfo.name() << " | " << nInfo.hash_code() << std::endl;
  // i | 6253375586064260614

  // 获取一个字面量的类型信息：
  const std::type_info &dInfo = typeid(25.65);
  std::cout << dInfo.name() << " | " << dInfo.hash_code() << std::endl;
  // d | 14494284460613645429

  // 获取一个对象的类型信息：
  Base obj;
  const std::type_info &objInfo = typeid(obj);
  std::cout << objInfo.name() << " | " << objInfo.hash_code() << std::endl;
  // 4Base | 1646098473708828226

  // 获取一个类的类型信息：
  const std::type_info &baseInfo = typeid(Base);
  std::cout << baseInfo.name() << " | " << baseInfo.hash_code() << std::endl;
  // 4Base | 1646098473708828226

  // 获取一个结构体的类型信息：
  const std::type_info &stuInfo = typeid(struct STU);
  std::cout << stuInfo.name() << " | " << stuInfo.hash_code() << std::endl;
  // 3STU | 12536571244334020757

  // 获取一个普通类型的类型信息：
  const std::type_info &charInfo = typeid(char);
  std::cout << charInfo.name() << " | " << charInfo.hash_code() << std::endl;
  // c | 10959529184379665549

  // 获取一个表达式的类型信息：
  const std::type_info &expInfo = typeid(20 * 45 / 4.5);
  std::cout << expInfo.name() << " | " << expInfo.hash_code() << std::endl;
  // d | 14494284460613645429
}

void func2() {
  float j;
  int i;

  // before是指类型名字排列顺序，不是定义的顺序：
  std::cout << std::boolalpha << typeid(i).before(typeid(j)) << std::endl;
  std::cout << std::boolalpha << typeid(j).before(typeid(i)) << std::endl;

  // false
  // true
}

void func() {
  func1();
  func2();
}
}  // namespace n2

namespace n3 {
// typeid返回type_info对象的引用，而typeid(a)==typeid(b)的结果为true，
// 可以说明，一个类型不管使用了多少次，编译器都只创建一个type_info对象对象，
// 所有typeid都返回这个对象的引用，为了减小编译后文件的体积，
// 编译器不会为所有的类型创建type_info对象，只会为使用了typeid的类型创建。
// 不过有一种特殊情况，就是带虚函数的类（包括继承来的），
// 不管有没有使用typeid运算符，编译器都会为带虚函数的类创建type_info对象。

// type_info的构造函数是private属性的，所以不能在代码中直接实例化，
// 只能由编译器在内部实例化（借助友元）。
// 而且还重载了=运算符，也是private属性的，所以也不能赋值。

void func1() {
  char *str;
  int a = 2;
  int b = 10;
  float f;
  std::cout.setf(std::ios::boolalpha);
  std::cout << (typeid(int) == typeid(int)) << std::endl;      // true
  std::cout << (typeid(int) == typeid(char)) << std::endl;     // false
  std::cout << (typeid(char *) == typeid(char)) << std::endl;  // false
  std::cout << (typeid(str) == typeid(char *)) << std::endl;   // true
  std::cout << (typeid(a) == typeid(int)) << std::endl;        // true
  std::cout << (typeid(b) == typeid(int)) << std::endl;        // true
  std::cout << (typeid(a) == typeid(a)) << std::endl;          // true
  std::cout << (typeid(a) == typeid(b)) << std::endl;          // true
  std::cout << (typeid(a) == typeid(f)) << std::endl;          // false
  std::cout << (typeid(a / b) == typeid(int)) << std::endl;    // true
}

void func2() {
  Base obj1;
  Base *p1;
  Derived obj2;
  Derived *p2 = new Derived;
  p1 = p2;
  std::cout << (typeid(obj1) == typeid(p1)) << std::endl;       // false
  std::cout << (typeid(obj1) == typeid(*p1)) << std::endl;      // true
  std::cout << (typeid(&obj1) == typeid(p1)) << std::endl;      // true
  std::cout << (typeid(obj1) == typeid(obj2)) << std::endl;     // false
  std::cout << (typeid(obj1) == typeid(Base)) << std::endl;     // true
  std::cout << (typeid(*p1) == typeid(Base)) << std::endl;      // true
  std::cout << (typeid(p1) == typeid(Base *)) << std::endl;     // true
  std::cout << (typeid(p1) == typeid(Derived *)) << std::endl;  // false

  // typeid(*p1)==typeid(Base)和typeid(p1)==typeid(Base*)的结果为true，
  // 可以说明：即使将派生类指针p2赋值给基类指针p1，p1的类型仍然为Base*。
}

void func() {
  func1();
  func2();
}
}  // namespace n3

int main(int argc, char *argv[]) {
  if (argc < 2) {
    std::cout << argv[0] << " i [0 - 2]" << std::endl;
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
    default:
      std::cout << "invalid type" << std::endl;
      break;
  }

  return 0;
}