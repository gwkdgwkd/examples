#include <iostream>

using namespace std;

// 将非尾端类设计为抽象类

class Animal {
 public:
  Animal() { a = 0; }
  Animal(int i) : a(i) {}
  virtual ~Animal() {}
  Animal& operator=(const Animal& rhs) {
    cout << "Animal::operator=" << endl;
    a = rhs.a;
  }
  virtual void display() { cout << a << endl; }

 private:
  int a;
};
class Lizard : public Animal {
 public:
  Lizard() { b = 0; }
  Lizard(int i) : b(i), Animal(i * 10) {}
  Lizard& operator=(const Lizard& rhs) {
    cout << "Lizard::operator=" << endl;
    b = rhs.b;
    Animal::operator=(rhs);
  }
  virtual void display() {
    Animal::display();
    cout << b << endl;
  }

 private:
  int b;
};
class Chicken : public Animal {
 public:
  Chicken() { c = 0; }
  Chicken(int i) : c(i), Animal(i * 20) {}
  Chicken& operator=(const Chicken& rhs) {
    cout << "Chicken::operator=" << endl;
    c = rhs.c;
    Animal::operator=(rhs);
  }
  virtual void display() {
    Animal::display();
    cout << c << endl;
  }

 private:
  int c;
};

void func1() {
  Lizard liz1(3);
  liz1.display();
  // 30
  // 3

  Lizard liz2(4);
  liz2.display();
  // 40
  // 4
  Animal* pAnimal1 = &liz1;
  Animal* pAnimal2 = &liz2;
  *pAnimal1 = *pAnimal2;
  liz1.display();
  // Animal::operator=
  // 40
  // 3

  // 这里有两个问题:
  // 1.最后一行的赋值运算调用的是Animal类的，
  //   虽然相关对象的类型是Lizard。
  //   结果，只有liz1的Animal部分被修改，这是部分赋值。
  //   在赋值后，liz1的Animal成员有了来自于liz2的值，
  //   但其Lizard成员部分没被改变。
  // 2.真的有程序员把代码写成这样，用指针来给对象赋值并不少见，
  //   特别是那些对C有丰富经验而转移到C++的程序员。
}

// 一个解决方法是将赋值运算申明为虚函数：
class Animal1 {
 public:
  Animal1() { a = 0; }
  Animal1(int i) : a(i) {}
  virtual ~Animal1() {}
  virtual Animal1& operator=(const Animal1& rhs) {
    cout << "Animal1::operator=" << endl;
    a = rhs.a;
  }
  virtual void display() { cout << a << endl; }

 private:
  int a;
};
class Lizard1 : public Animal1 {
 public:
  Lizard1() { b = 0; }
  Lizard1(int i) : b(i), Animal1(i * 10) {}
  // 基于C++语言最近作出的修改，我们可以修改返回值的类型，
  // 于是每个都返回正确的类的引用，但C++的规则强迫声明相同的参数类型。
  // 这意味着Lizard类和Chicken类的赋值操作，
  // 必须准备接受任意类型的Animal对象。
  virtual Lizard1& operator=(const Animal1& rhs) {
    cout << "Lizard1::operator=" << endl;
    // 这个函数只在rhs确实是Lizard类型时将它赋给*this。
    // 如果rhs不是Lizard类型，
    // 函数传递出dynamic_cast转换失败时抛的bad_cast类型的异常。
    // 即使不在乎有异常，这个函数看起来也是没必要的复杂和昂贵，
    // dynamic_cast必要引用一个type_info结构；
    // 因为通常情况下都是一个Lizard对象赋给另一个。
    b = dynamic_cast<const Lizard1&>(rhs).b;
    Animal1::operator=(rhs);
  }
  virtual void display() {
    Animal1::display();
    cout << b << endl;
  }

 private:
  int b;
};

void func2() {
  Lizard1 liz1(3);
  Lizard1 liz2(4);
  liz1.display();
  // 30
  // 3
  liz2.display();
  // 40
  // 4
  Animal1* pAnimal1 = &liz1;
  Animal1* pAnimal2 = &liz2;
  *pAnimal1 = *pAnimal2;
  liz1.display();
  // Lizard1::operator=
  // Animal1::operator=
  // 40
  // 4
}

class Lizard2 : public Animal1 {
 public:
  Lizard2() { b = 0; }
  Lizard2(int i) : b(i), Animal1(i * 10) {}

  virtual Lizard2& operator=(const Animal1& rhs) {
    // 现在这个函数试图将rhs转换为一个Lizard。
    // 如果转换成功，通常的赋值操作被调用；
    // 否则，一个bad_cast异常被抛出。
    cout << "Lizard2::operator=　Animal1" << endl;
    return operator=(dynamic_cast<const Lizard2&>(rhs));
  }
  // 可以处理这种情况而无需增加复杂度或花费dynamic_cast，
  // 只要在Lizard中增加一个通常形式的赋值操作：
  Lizard2& operator=(const Lizard2& rhs) {
    cout << "Lizard2::operator=　Lizard2" << endl;
    b = rhs.b;
    Animal1::operator=(rhs);
  }
  virtual void display() {
    Animal1::display();
    cout << b << endl;
  }

 private:
  int b;
};

void func3() {
  Lizard2 liz1;
  Lizard2 liz2;
  Animal1* pAnimal1 = &liz1;
  Animal1* pAnimal2 = &liz2;

  *pAnimal1 = *pAnimal2;
  // Lizard2::operator=　Animal1
  // Lizard2::operator=　Lizard2
  // Animal1::operator=
}

class Chicken1 : public Animal1 {
 public:
  Chicken1() { c = 0; }
  Chicken1(int i) : c(i), Animal1(i * 20) {}
  virtual Chicken1& operator=(const Chicken1& rhs) {
    cout << "Chicken1::operator=" << endl;
    c = rhs.c;
    Animal1::operator=(rhs);
  }
  virtual void display() {
    Animal1::display();
    cout << c << endl;
  }

 private:
  int c;
};

// 在运行期使用dynamic_cast进行类型检测，这令人很紧张。
// 有一件事要注意，一些编译器仍然没有支持dynamic_cast，
// 所以使用它的代码虽然理论上具有可移植性，实际上不一定。
// 更重要的是，它要求使用Lizard和Chicken的用户，
// 必须在每次赋值操作时都准备好捕获bad_cast异常的准备。
// 如果他们没有这么做的话，那么不清楚我们得到的好处是否超过最初的方案。
// 指出了这个关于虚赋值操作的令人非常不满意的状态后，
// 在最开始的地方重新整理，
// 以试图找到一个方法来阻止用户写出有问题的赋值语句是有必要的。
// 如果这样的赋值语句在编译期被拒绝，我们就不用担心它们做错事了。
// 也就是说，这意味着我们必须面对这样的事实，下面的代码是合法的：
void func4() {
  Lizard1 liz1;
  Chicken1 chick1;
  Animal1* pAnimal1 = &liz1;
  Animal1* pAnimal2 = &chick1;
  try {
    *pAnimal1 = *pAnimal2;  // 抛出异常std::bad_cast
  } catch (const std::bad_cast& e) {
    std::cerr << e.what() << '\n';
  }

  Chicken1 chick2;
  Animal1* pAnimal3 = &chick2;
  try {
    *pAnimal1 = *pAnimal3;  // 抛出异常std::bad_cast
  } catch (const std::bad_cast& e) {
    std::cerr << e.what() << '\n';
  }

  // Lizard1::operator=
  // std::bad_cast
  // Lizard1::operator=
  // std::bad_cast

  // 这是一个混合类型赋值：左边是一个Lizard，右边是一个Chicken。
  // 混合类型赋值在C++中通常不是问题，因为C++的强类型原则将评定它们非法。
  // 然而，通过将Animal的赋值操作设为虚函数，我们打开了混合类型操作的门。
  // 这使得我们处境艰难，我们应该允许通过指针进行同类型赋值，
  // 而禁止通过同样的指针进行混合类型赋值。
  // 只能在运行期区分它们，
  // 因为将*pAnimal2赋给*pAnimal1有时是正确的，有时不是。
  // 我们于是陷入了基类型运行期错误的黑暗世界中。
  // 尤其是，我们需要在混合类型赋值时指出在operator=内部发生了错误，
  // 而类型相同时，我们期望按通常的方式完成赋值。
}

// 最容易的方法是在Animal中将operator=置为private。
// 于是，Lizard对象可以赋值给Lizard对象，
// Chicken对象可以赋值给Chicken对象，但部分或混合类型赋值被禁止：
class Animal2 {
 public:
  Animal2() { a = 0; }
  Animal2(int i) : a(i) {}
  virtual ~Animal2() {}
  virtual void display() { cout << a << endl; }

 protected:
  // 下面的问题可以通过将Animal::operator=声明为protected来解决：
  virtual Animal2& operator=(const Animal2& rhs) {
    cout << "Animal2::operator=" << endl;
    a = rhs.a;
  }

 private:
  // 它也使得不可能正确实现Lizard和Chicken类的赋值操作，
  // 因为派生类的赋值操作函数有责任调用其基类的赋值操作函数：
  // virtual Animal2& operator=(const Animal2& rhs) {
  //   cout << "Animal2::operator=" << endl;
  //   a = rhs.a;
  // }
  int a;
};
class Lizard3 : public Animal2 {
 public:
  Lizard3() { b = 0; }
  Lizard3(int i) : b(i), Animal2(i * 10) {}

  virtual Lizard3& operator=(const Animal2& rhs) {
    cout << "Lizard3::operator=Animal2" << endl;
    return operator=(dynamic_cast<const Lizard3&>(rhs));
  }
  Lizard3& operator=(const Lizard3& rhs) {
    cout << "Lizard3::operator=Lizard3" << endl;
    b = rhs.b;
    Animal2::operator=(rhs);
  }
  virtual void display() {
    Animal2::display();
    cout << b << endl;
  }

 private:
  int b;
};
class Chicken3 : public Animal2 {
 public:
  Chicken3() { c = 0; }
  Chicken3(int i) : c(i), Animal2(i * 20) {}
  virtual Chicken3& operator=(const Chicken3& rhs) {
    cout << "Chicken3::operator=" << endl;
    c = rhs.c;
    Animal2::operator=(rhs);
  }
  virtual void display() {
    Animal2::display();
    cout << c << endl;
  }

 private:
  int c;
};

void func5() {
  Lizard3 liz1, liz2;
  liz1 = liz2;  // fine
  Chicken3 chick1, chick2;
  chick1 = chick2;  // also fine
  Animal2* pAnimal1 = &liz1;
  Animal2* pAnimal2 = &chick1;
  // *pAnimal51 = *pAnimal52;
  // error! attempt to call private Animal::operator=
  // 不幸的是，Animal也是实体类，
  // 这个方法同时将Animal对象间的赋值评定为非法了：
  Animal2 animal1, animal2;
  // animal1 = animal2;
  // error! attempt to call private Animal::operator=

  // Lizard3::operator=Lizard3
  // Animal2::operator=
  // Chicken3::operator=
  // Animal2::operator=
}

// 允许Animal对象间的赋值，
// 而阻止Lizard和Chicken对象通过Animal的指针进行部分赋值，
// 两难问题仍然存在。
// 程序该怎么办？
// 最容易的事情是排除Animal对象间赋值的需求，
// 其最容易的实现方法是将Animal设计为抽象类。
// 作为抽象类，Animal不能被实例化，
// 所以也就没有了Animal对象间赋值的需求了。
// 当然，这导致了一个新问题，因为我们最初的设计表明Animal对象是必须的。
// 有一个很容易的解决方法：
// 不用将Animal设为抽象类，我们创建一个新类叫AbstractAnimal，
// 来包含Animal、Lizard、Chikcen的共有属性，并把它设为抽象类。
// 然后将每个实体类从AbstractAnimal继承。
class AbstractAnimal {
 protected:
  AbstractAnimal& operator=(const AbstractAnimal& rhs) {
    cout << "AbstractAnimal::operator=" << endl;
    a = rhs.a;
  }

 public:
  AbstractAnimal() { a = 0; }
  AbstractAnimal(int i) : a(i) {}
  virtual ~AbstractAnimal() = 0;
  virtual void display() { cout << a << endl; }

 private:
  int a;
};
AbstractAnimal::~AbstractAnimal() {}
class Animal4 : AbstractAnimal {
 public:
  Animal4() {}
  virtual ~Animal4() {}

  Animal4& operator=(const Animal4& rhs) {}
};
class Lizard4 : public AbstractAnimal {
 public:
  Lizard4() { b = 0; }
  Lizard4(int i) : b(i), AbstractAnimal(i * 10) {}

  Lizard4& operator=(const Lizard4& rhs) {
    cout << "Lizard4::operator=" << endl;
    b = rhs.b;
    AbstractAnimal::operator=(rhs);
  }

  virtual void display() {
    Lizard4::display();
    cout << b << endl;
  }

 private:
  int b;
};
class Chicken4 : public AbstractAnimal {
 public:
  Chicken4() { c = 0; }
  Chicken4(int i) : c(i), AbstractAnimal(i * 20) {}
  Chicken4& operator=(const Chicken4& rhs) {
    cout << "Chicken4::operator=" << endl;
    c = rhs.c;
    AbstractAnimal::operator=(rhs);
  }
  virtual void display() {
    AbstractAnimal::display();
    cout << c << endl;
  }

 private:
  int c;
};

void func6() {
  Lizard4 liz1, liz2;
  liz1 = liz2;
  Chicken4 chick1, chick2;
  chick1 = chick2;
  AbstractAnimal* pAnimal1 = &liz1;
  AbstractAnimal* pAnimal2 = &liz2;
  AbstractAnimal* pAnimal3 = &chick1;
  // *pAnimal1 = *pAnimal2;
  // *pAnimal1 = *pAnimal3;
  Animal4 animal1, animal2;
  animal1 = animal2;

  // Lizard4::operator=
  // AbstractAnimal::operator=
  // Chicken4::operator=
  // AbstractAnimal::operator=
}

// 这个设计给你所以你需要的东西：
// 1.同类型间的赋值被允许，部分赋值或不同类型间的赋值被禁止；
// 2.派生类的赋值操作函数可以调用基类的赋值操作函数。
// 此外，所有涉及Aniaml、Lizard或Chicken类的代码都不需要修改，
// 因为这些类仍然操作，其行为与引入AbstractAnimal前保持了一致。
// 肯定，这些代码需要重新编译，
// 但这是为获得确保下面的条件付出很小的代价：
// 编译通过的赋值语句的行为是正确的而行为可能不正确的赋值语句不能编译通过。
// 要使得这一切工作，AbstractAnimal类必须是抽象类，
// 它必须至少有一个纯虚函数。
// 大部分情况下，带一个这样的函数是没问题的，但在极少见的情况下，
// 你会发现需要创一个如AbstractAnimal这样的类，
// 没有哪个成员函数是自然的纯虚函数。
// 此时，传统方法是将析构函数申明为纯虚函数；这也是上面所采用的。
// 为了支持多态，基类总需要虚析构函数，
// 将它再多设为纯虚的唯一麻烦就是必须在类的定义之外实现它。
// 申明一个函数为虚并不意味着它没有实现，它意味着：
// 1.当前类是抽象类。
// 2.任何从此类派生的实体类必须将此函数申明为一个普通的虚函数，
//   也就是说，不能带=0。
// 是的，绝大部分纯虚函数都没有实现，但纯虚析构函数是个特例。
// 它们必须被实现，因为它们在派生类析构函数被调用时也将被调用。
// 而且，它们经常执行有用的任务，诸如释放资源或纪录消息。
// 实现纯虚函数一般不常见，但对纯虚析构函数，它不只是常见，它是必须。

// 你可能已经注意到这里讨论的通过基类指针进行赋值的问题，
// 是基于假设实体类（如Animal）有数据成员。
// 如果它们没有数据成员，你可能指出，那么就不会有问题，
// 从一个无数据的实体类派生新的实体类是安全的。
// 无数据而可以成为实体类的基类会两种可能：
// 在将来，或者它可能有数据成员，或者它仍然没有。
// 如果它将来可能有数据成员，
// 你现在做的只是推迟问题的发生直到数据成员被加入，你在用短利换长痛。
// 如果这个基类真的不会有数据成员，
// 那么它现在就该是抽象类，没有数据的实体类有什么用处？

// 用如AbstractAnimal这样的抽象基类替换如Animal这样的实体基类，
// 其好处远比简单地使得operator=的行为易于了解。
// 它也减少了你试图对数组使用多态的可能，这种行为的令人不愉快的后果。
// 然而，这个技巧最大的好处发生在设计的层次上，
// 因为这种替换强迫你明确地认可有用处的抽象行为的实体。
// 也就是说，它使得你为有用的原型（concept）创造了新的抽象类，
// 即使你并不知道这个有用的原型的存在。
// 如果你有两个实体类C1和C2并且你喜欢C2公有继承自C1，
// 你应该将两个类的继承层次改为三个类的继承层次，
// 通过创造一个新的抽象类A并将C1和C2都从它继承。
// 这种修改的重要价值是强迫你确定抽象类A。

// 这一切导致了一些令人不安的思考。
// 毕竟，每个类都完成了某些类型的抽象，
// 我们不应该在此继承体系中创造两个类来针对每个原型吗？
// 一个是抽象类来表示其抽象部分，
// to embody the abstract part of the abstraction，
// 一个是实体类来表示对象生成部分，
// to embody the object-generation part of the abstraction？
// 不应该，如果你这么做了，将使得继承体系中有太多的类。
// 这样的继承体系是难以理解的，难以维护的，
// 编译的代价很昂贵，这不是面向对象设计的目的。
// 其目的是：确认有用的抽象，
// 并强迫它们（并且只有它们）放入如抽象类这样的实体。
// 但怎么确认有用的抽象？
// 谁知道什么抽象在将来被证明有用？
// 谁能预知他将来要从什么进行继承？
// 在一个原型第一次被需要时，
// 我们无法证明同时创造一个抽象类（为了这个原型），
// 和一个实体类（为了原型对应的对象）是正确的，
// 但第二次需要时，我们就能够这么做是正确的。
// 我描述过的修改简单地实现了这个过程，
// 并且在这么做的过程中强迫设计着和程序员明确表达那些有用的抽象，
// 即使他们不知道那些有用的原型，这也碰巧使得构建正确的赋值行为很容易。

// 假设你处理的只有一种传输协议，也只有一种包。
// 也许你听说了其它协议和数据包类型的存在，
// 但还从未支持它们，也没有任何计划以在未来支持它们。
// 你会为数据包既设计一个抽象类吗，
// 又设计一个你实际使用的实体类？
// 如果你这么做了，你可以在以后增加新的数据包而不用改变基类。
// 这使得你增加新的数据包类型时程序不用重新编译。
// 但这种设计需要两个类，而你现在只需要一个，
// 针对于你现在使用的特殊数据包类型。
// 这值得吗，增加设计的复杂度以允许扩充特性，而这种扩充可能从不发生？
// 这儿没有肯定正确的选择，但经验显示：
// 为我们还不完全了解的原型设计优秀的类几乎是不可能的。
// 如果你为数据包设计了抽象类，你怎么保证它正确，
// 尤其是在你的经验只局限于这唯一的数据包类型时？
// 记住，只有在设计出的类能被将来的类从它继承而不需要它作任何修改时，
// 你才能从数据包的抽象类中获得好处。
// 如果它需要被修改，你不得不重新编译所有使用数据包类的代码，你没得到任何好处。
// 看起来不太能够设计出一个领人满意的抽象设计包类，
// 除非你精通各种数据包的区别以及它们相应的使用环境。
// 鉴于你有限的经验，我建议不要定义抽象类，等到以后需要从实体类继承时再加。

// 我所说的转换方法是一个判断是否需要抽象类的方法，但不是唯有的方法。
// 还有很多其它的好方法；讲述面向对象分析的书籍上满是这类方法。
// 当发现需求从一个实体类派生出另外一个实体类时，
// 这也不是唯一需要引入抽象类的地方。
// 不管怎么说啦，需要通过公有继承将两个实体类联系起来，
// 通常表示需要一个新的抽象类。
// 这种情况是如此常见，所以引起了我们的深思。
// 第三方的C++类库越来越多，
// 当发现你需要从类库中的实体类派生出一个新的实体类，
// 而这个库你只有只读权时，你要怎么做？
// 你不能修改类库以加入一个新的抽象类，所以你的选择将很有限、很无趣：
// 1.从已存在的实体类派生出你的实体类，
//   并容忍我们在本Item开始时说到的赋值问题。
//   你还要注意在Item M3中说过的数组问题。
// 2.试图在类库的继承树的更高处找到一个完成了你所需的大部分功能的抽象类，
//   从它进行继承。当然，可能没有合适的类；
//   即使有，你可能不得不重复很多已经在实体类中实现了的东西。
// 3.用包容你试图继承的类的方法来实现你的新类。
//   例如，你将一个类库中的类的对象为数据成员，并在你的类中重实现它的接口，
//   这种方法需要你在类库每次升级时也要更新你自己的类。
//   它还需要你放弃重定义类库中的类的虚函数的能力，因为你用的不是继承。
class Window {
 public:
  virtual void resize(int newWidth, int newHeight) {}
  virtual void repaint() const {}
  int width() const {}
  int height() const {}
};
class SpecialWindow {
 public:
  int width() const { return w.width(); }
  int height() const { return w.height(); }
  virtual void resize(int newWidth, int newHeight);
  virtual void repaint() const;

 private:
  Window w;
};
// 4.使用你得到，使用类库中的类，而将你自己的程序修改得那个类适用。
//   用非成员函数来提供扩展功能（那些你想加入那个类而没有做到的）。
//   结果，程序将不如你所期望中的清晰、高效、可维护、可扩展，
//   但至少它完成了你所需要的功能。

// 这些选择都不怎么吸引人，所以你不得不作出判断并选择最轻的毒药。
// 这不怎么有趣，但生活有时就是这样。
// 想让事情在以后对你自己（和我们其它人）容易些，将问题反馈给类库生产商。
// 靠运气（以及大量的用户反馈），随时间的流逝，那些设计可能被改进。
// 最后，一般的规则是：非尾端类应该是抽象类。
// 在处理外来的类库时，你可能需要违背这个规则；
// 但对于你能控制的代码，
// 遵守它可以提高程序的可靠性、健壮性、可读性、可扩展性。

int main() {
  func1();
  func2();
  func3();
  func4();
  func5();
  func6();

  return 0;
}