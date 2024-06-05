#include <functional>
#include <iostream>
#include <memory>
#include <vector>

// 条款32：使用初始化捕获将对象移入闭包

// 有时，按值的捕获和按引用的捕获皆非你所欲。
// 如果想要把一个只移动对象放入闭包，例如std::unique_ptr或std::future对象，
// C++11未提供任何办法做到此事，如果有个对象，其复制操作开销昂贵，
// 而移动操作成本低廉，例如大部分标准容器库，而又需要把该对象放入闭包。
// 那么肯定更愿意移动该对象，而非复制它，但是C++11中也还是没有实现这一点的途径。
// 但那只是C++11，C++14则有云泥之别，为对象移动入闭包提供了直接支持。

// 移动捕获的缺失即使在C++11标准被刚接受时，也被视为一种缺陷。
// 最直接的补救措施本是在C++14中添加这一特性，但标准委员会却另辟蹊径，
// 委员们提出了一种全新的捕获机制，它是如此灵活，
// 按移动的捕获只不过属于该机制能够实现的多种效果之一罢了，这种新能力称为初始化捕获。
// 实际上，它可以做到C++11的捕获形式能够做到的所有事情，而且还不止如此。
// 初始化捕获不能表示默认捕获模式，但条款31解释过，无论如何都能应该远离这种模式，
// 对于将C++11捕获可以实现的情况，若用初始化捕获语法则会稍显啰嗦，
// 所以若使用C++11捕获已能解决问题，则大可以使用之。

// 使用初始化捕获，则你会得到机会指定：
// 1.由lambda生成的闭包类中的成员变量的名字；
// 2.一个表达式，用以初始化该成员变量。

namespace n1 {
class Widget {
 public:
  bool show() const { std::cout << "Widget::show" << std::endl; }
};

namespace test1 {
void func1() {
#if __cplusplus == 201402L
  auto p = std::make_unique<Widget>();
  auto f1 = [pw = std::move(p)] { return pw->show(); };
  f1();  // Widget::show
  // [pw = std::move(pw)]这段代码就是初始化捕获，位于=左侧的，
  // 是所指定的闭包类成员变量的名字，而位于其右侧的则是其初始化表达式。
  // =的左右两侧位于不同的作用域，左侧作用域就是闭包类的作用域，
  // 而右侧的作用域则与lambda定义之处的作用域相同。
  // 在上述例子中，=左侧的名字pw指涉的闭包类的成员变量。
  // 而右侧的名字pw指涉的则是在lambda式上面一行声明的对象，
  // 即经由调用make_unique所初始化的对象，所以，
  // pw=std::move(pw)表达了在闭包中创建一个成员变量pw，
  // 然后使用针对局部变量pw实施std::move的结果来初始化该成员变量。
  // 和平时一样，lambda代码的作用域位于闭包类中，所以pw指向也是闭包类的成员变量。

  // 闭包类成员变量可以经由std::make_unique实施初始化：
  auto f2 = [pw = std::make_unique<Widget>()] { return pw->show(); };
  f2();  // Widget::show
  // C++11的捕获概念在C++14得到了显著的泛化，因为C++11中不可能捕获表达式的结果。
  // 因此，初始化捕获还有另一美名，称为广义lambda捕获。

#elif __cplusplus == 201103L  // C++11
  auto pw = std::unique_ptr<Widget>(new Widget);

  // 传值捕获报错：
  // auto func = [pw] { return pw->show(); };

  // 使用引用捕获可以，但是没有真正移动到闭包内：
  auto f1 = [&pw] { return pw->show(); };
  f1();  // Widget::show

  auto f2 =
      std::bind([](const std::unique_ptr<Widget>& pw) { return pw->show(); },
                std::unique_ptr<Widget>(new Widget));
  f2();  // Widget::show

#endif
}
}  // namespace test1

namespace test2 {
// 但如果使用的编译器缺少对C++14初始化捕获的支持，又将如何是好？
// 在不支持按移动捕获的语言中，又该如何实现按移动捕获呢？
// 一个Lambda表达式不过是生成一个类并且创建一个该类的对象的手法罢了。
// 并不存在lambda能做，而手工做不到的事情。
// 以上面所见的C++14示例代码为例，就可以如下用C++11写作：
class Show {
 public:
  using DataType = std::unique_ptr<Widget>;
  explicit Show(DataType&& ptr) : pw(std::move(ptr)) {}
  bool operator()() const { return pw->show(); }

 private:
  DataType pw;
};
void func2() {
  auto f = Show(std::unique_ptr<Widget>(new Widget));
  f();  // Widget::show
}

// 这比起撰写一个lambda式长了不少，但是它并未改变一个事实，即在C++11中，
// 如果想要支持对成员变量实施移动初始化的类，那么也只需多花时间敲键盘，就能达到目的。
}  // namespace test2

void func() {
  test1::func1();
  test2::func2();
}
}  // namespace n1

namespace n2 {
// 考虑到便利性，如果非想要用lambda式，按移动捕获在C++11中也可以模拟，只需要：
// 1.把需要捕获的对象移动到std::bind产生的函数对象中；
// 2.给lambda一个指向欲捕获的对象的引用。

void func() {
  std::vector<double> data;

#if __cplusplus == 201402L
  auto f1 = [data = std::move(data)] {};
  f1();
#endif

  auto f2 = std::bind([](const std::vector<double>& data) {}, std::move(data));
  f2();

  // 和lambda表达式类似地，bind也生成函数对象，称bind返回的对象为绑定对象，
  // 绑定对象含有传递给std::bind所有实参的副本，对于每个左值实参，
  // 在绑定对象内的对应的对象内对其实施的是复制构造；
  // 对于每个右值实参，实施的则是移动构造。
  // 在这个例子中，第二个实参是个右值，即std::move的结果，参见条款23，
  // 所以data在绑定对象中实施的是移动构造，这是实现模拟移动捕获的核心所在，
  // 因为把右值移入绑定对象，正是绕过C++11无法将右值移到闭包的手法。

  // 当一个绑定的对象被调用时，即其函数调用运算符被唤起，
  // 它所存储的实参会传递给原先传递给std::bind的那个可调用对象。
  // 在本例中，也就是当绑定对象f被调用时，f内经由移动构造出所得到的data的副本，
  // 就会作为实参传递给那个原先传递给bind的lambda表达式。

  // 这个lambda和C++14版本的lambda长得一样，但是多加了一个形参data，
  // 它对应于伪移动捕获对象，该形参是个指向绑定对象内的data副本的左值引用，
  // 而不是右值引用，因为虽然初始化data副本的表达式是std::move(data)，
  // 但data的副本本身是一个左值，这么一来，在lambda内对data做的操作，
  // 都会实施在绑定对象内移动构造而得的data的副本之上。

  // 默认情况下，lambda生成的闭包类中的operator()成员函数会带有const饰词。
  // 结果，闭包里的所有成员变量在lambda的函数体内都会带有const饰词。
  // 但是，绑定对象里移动构造得到的data副本却并不带有const饰词，所以，
  // 为了防止该data的副本在lambda式内被意外修改，lambda的形参就声明为常量引用。
  // 但如果lambda式的声明带有mutable饰词，
  // 闭包里的operator()函数就不会在声明时带有const饰词，
  // 相应的适当做法，就是在lambda声明中略去const：
  auto f3 =
      std::bind([](std::vector<double>& data) mutable {}, std::move(data));
  f3();
  // 因为绑定对象存储着传递给std::bind所有实参的副本，
  // 在本例中的绑定对象就包含一份由作为bind的第一个实参的lambda产生的闭包的副本。
  // 这么一来，该闭包的生命期和绑定对象就是相同的。
  // 这一点很重要，因为这意味着只要闭包还存在，则绑定对象内的伪移动捕获对象也存在。
}

// 在这里展示的是如何使用std::bind来绕开C++11中lambda语法的限制，
// 这不免有些讽刺，因为在条款34中，又提倡优先选用lambda式，而非std::bind。
// 不过那个条款也有解释说，C++11中有一些情况下，std::bind可能会更有用，
// 而以上就是这些情况之一，在C++14中，像初始化捕获和auto形参这些特征可以消除那些情况。
}  // namespace n2

// 总结：
// 1.移动构造一个对象入C++11闭包是不可能实现的，
//   但移动构造一个对象入绑定对象则是可能实现的；
// 2.欲在C++11中模拟移动捕获包括一下步骤：
//   a.先构造一个对象入绑定对象；
//   b.然后按引用把该移动构造所得的对象传递给lambda式。
// 3.因为绑定对象的生命期和闭包相同，
//   所以针对绑定对象中的对象和闭包里的对象可以采用同样手法加以处置。

// 要记住的东西：
// 1.使用C++14的初始化捕获将对象移入闭包；
// 2.在C++11中，经由手工实现的类或std::bind去模拟初始化捕获。

int main(int argc, char* argv[]) {
  if (argc < 2) {
    std::cout << argv[0] << " i [0 - 1]" << std::endl;
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
    default:
      std::cout << "invalid type" << std::endl;
      break;
  }

  return 0;
}