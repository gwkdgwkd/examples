#include <iostream>

// 考虑使用lazy evaluation（懒惰计算法）

// 在计算机科学中，称这样的延迟为lazy evaluation（懒惰计算法）。
// 当使用了lazy
// evaluation后，采用此种方法的类将推迟计算工作直到系统需要这些计算的结果。 lazy
// evaluation广泛适用于各种应用领域，所以将分四个部分讲述。

namespace n1 {
// 引用计数
// class String {};
// String s1 = "Hello";
// String s2 = s1;
// 通常string拷贝构造函数让s2被s1初始化后，s1和s2都有自己的Hello拷贝。
// 这种拷贝构造函数会引起较大的开销，因为要制作s1值的拷贝，并把值赋给s2，
// 这通常需要用new操作符分配堆内存，需要调用strcpy函数拷贝s1内的数据到s2。
// 这是一个eager evaluation（热情计算）：
// 只因为到string拷贝构造函数，就要制作s1值的拷贝并把它赋给s2。
// 然而这时的s2并不需要这个值的拷贝，因为s2没有被使用。
// 懒惰能就是少工作，不应该赋给s2一个s1的拷贝，而是让s2与s1共享一个值。
// 只须做一些记录以便知道谁在共享什么，就能够省掉调用new和拷贝字符的开销。
// 事实上s1和s2共享一个数据结构，这对于client来说是透明的。
// 仅仅当这个或那个string的值被修改时，共享同一个值的方法才会造成差异。
// 仅仅修改一个string的值，而不是两个都被修改，这一点是极为重要的。
// 除非确实需要，不去为任何东西制作拷贝，应该是懒惰的，只要可能就共享使用其它值。
// 在一些应用领域，经常可以这么做。

// 区别对待读取和写入
// String s = "Homer's Iliad";
// cout << s[3]; // 调用operator[]读取s[3]
// s[3] = 'x';
// 首先调用operator[]用来读取string的部分值，但是第二次调用该函数是为了完成写操作。
// 应能够区别对待读调用和写调用，因为读取reference-counted string是很容易的，
// 而写入这个string则需要在写入前对该string值制作一个新拷贝。
// 如何判断调用operator[]的context是读取操作还是写入操作呢？
// 残酷的事实是不可能判断出来，通过使用lazy evaluation和条款M30中讲述的proxy
// class， 可以推迟做出是读操作还是写操作的决定，直到能判断出正确的答案。
}  // namespace n1

namespace n2 {
// Lazy Fetching（懒惰提取）

namespace test1 {
// 假设程序使用了一些包含许多字段的大型对象。
// 这些对象的生存期超越了程序运行期，所以它们必须被存储在数据库里。
// 每一个对都有一个唯一的对象标识符，用来从数据库中重新获得对象：
class LargeObject {
 public:
  LargeObject(int id) {}                // 从磁盘中恢复对象
  const std::string& field1() const {}  // field1的值
  int field2() const {}                 // field2的值
  double field3() const {}              // ...
  const std::string& field4() const {}
  const std::string& field5() const {}
};
// 从磁盘中恢复LargeObject的开销：
void restoreAndProcessObject(int id) {
  LargeObject object(id);  // 恢复对象
}
}  // namespace test1

namespace test2 {
// 这里仅仅需要filed2的值，所以为获取其它字段而付出的努力都是浪费。
// 当LargeObject对象被建立时，不从磁盘上读取所有的数据，这样懒惰法解决了这个问题。
// 不过这时建立的仅是一个对象壳，当需要某个数据时，这个数据才被从数据库中取回。
// 这种demand-paged对象初始化的实现方法是：
class LargeObject {
 public:
  LargeObject(int id);
  const std::string& field1() const;
  int field2() const {};
  double field3() const;
  const std::string& field4() const;

 private:
  int oid;
  // 实现Lazy Fetching时，面临着一个问题：
  // 在任何成员函数里都有可能需要初始化空指针使其指向真实的数据，
  // 包括在const成员函数里，例如field1。
  // 然而当你试图在const成员函数里修改数据时，编译器会出现问题。
  // 最好的方法是声明字段指针为mutable，
  // 这表示在任何函数里它们都能被修改，甚至在const成员函数里。
  mutable std::string* field1Value;
  mutable int* field2Value;
  mutable double* field3Value;
  mutable std::string* field4Value;
};

// 对象中每个字段都用一个指向数据的指针来表示，LargeObject构造函数把每个指针初始化为空，
// 这些空指针表示字段还没有从数据库中读取数值。
LargeObject::LargeObject(int id)
    : oid(id), field1Value(0), field2Value(0), field3Value(0) {}
const std::string& LargeObject::field1() const {
  // 每个LargeObject成员函数，在访问字段指针所指向的数据之前必须字段指针检查的状态。
  // 如果指针为空，在对数据进行操作之前必须从数据库中读取对应的数据。
  if (field1Value == 0) {
    // 从数据库中为filed1读取数据，使field1Value指向这个值;
  }
  return *field1Value;
}

// 因为LargeObject对象实例很大，为这样的对象获取所有的数据，
// 数据库的操作的开销将非常大，特别是如果从远程数据库中获取数据和通过网络发送数据时。
// 而在这种情况下，不需要读去所有数据。
void restoreAndProcessObject(int id) {
  LargeObject object(id);
  if (object.field2() == 0) {
    std::cout << "Object " << id << ": null field2.\n";
  }
}
}  // namespace test2

namespace test3 {
// 如果编译器不支持mutable，需要另一种方法让编译器允许你在const成员函数里修改数据成员。
// 一种方法叫做fake this，建立一个指向non-const指针，指向的对象与this指针一样。
// 当想修改数据成员时，你通过fake this访问它：
class LargeObject {
 public:
  const std::string& field1() const;  // 没有变化
  int field2() const;

 private:
  std::string* field1Value;  // 不声明为mutable，因为老的编译器不支持
};
const std::string& LargeObject::field1() const {
  // 声明指针fakeThis，其与this指向同样的对象但是已经去掉了对象的常量属性
  LargeObject* const fakeThis = const_cast<LargeObject* const>(this);
  // 这个函数使用了const_cast，去除了*this的const属性。
  // 如果你的编译器不支持cosnt_cast，可以使用老式C风格的cast：
  // LargeObject* const fakeThis = (LargeObject * const)this;
  if (field1Value == 0) {
    // 正确的，因为fakeThis指向的对象不是const：
    fakeThis->field1Value = nullptr;
  }
  return *field1Value;
}
// LargeObject里的指针，必须把这些指针都初始化为空，
// 然后每次使用它们时都必须进行测试，这是令人厌烦的而且容易导致错误发生。
// 幸运的是使用smart指针可以自动地完成这种苦差使。
// 如果在LargeObject里使用smart指针，也将发现不再需要用mutalbe声明指针。
// 这只是暂时的，因为当实现smart指针类时最终会碰到mutalbe。
}  // namespace test3
}  // namespace n2

namespace n3 {
// Lazy Expression Evaluation(懒惰表达式计算)
template <class T>
class Matrix {};
// Matrix<int> m1(1000, 1000); // 一个 1000 ＊ 1000 的矩阵
// Matrix<int> m2(1000, 1000); // 同上
// Matrix<int> m3 = m1 + m2;
// 通常operator的实现使用eagar evaluation：
// 在这种情况下，它会计算和返回m1与m2的和。
// 这个计算量相当大（1000000次加法运算），当然系统也会分配内存来存储这些值。
// lazy evaluation方法说这样做工作太多，所以还是不要去做。
// 而是应该建立一个数据结构来表示m3的值是m1与m2的和，在用一个enum表示它们间是加法操作。
// 很明显，建立这个数据结构比m1与m2相加要快许多，也能够节省大量的内存。
// 公正地讲，懒惰有时也会失败，如果这样使用m3：
// cout << m3;
// 打印m3所有的值，一切都完了，必须计算m3的全部数值。
// 因为需要存储两个值之间的依赖关系，维护存储值、依赖关系或上述两者，
// 重载操作符例如赋值符、拷贝操作和加法操作，所以lazy evaluation在数字领域应用得很多。
// 另一方面运行程序时它经常节省大量的时间和空间。
}  // namespace n3

// 以上这四个例子展示了lazy evaluation在各个领域都是有用的：
// 能避免不需要的对象拷贝，通过使用operator[]区分出读操作，
// 避免不需要的数据库读取操作，避免不需要的数字操作，但是它并不总是有用。
// 实际上，如果计算都是重要的，lazy evaluation可能会减慢速度并增加内存的使用，
// 因为除了进行所有的计算以外，还必须维护数据结构让lazy evaluation尽可能地在第一时间运行。
// 在某些情况下要求软件进行原来可以避免的计算，这时lazy evaluation才是有用的。
// lazy evaluation对于C++来说没有什么特殊的东西，这个技术能被运用于各种语言里，
// 所有的数据流语言例如著名的APL、dialects of Lisp，都把这种思想做为语言的一个基本部分。
// 然而主流程序设计语言采用的是eager evaluation，C++是主流语言。
// 不过C++特别适合用户实现lazy evaluation，
// 因为它对封装的支持使得能在类里加入lazy evaluation，而根本不用让类的使用者知道。

int main() { return 0; }