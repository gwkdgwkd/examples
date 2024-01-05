#include <algorithm>
#include <functional>
#include <iostream>
#include <iterator>
#include <memory>
#include <vector>

// 条款31：避免默认捕获模式

// C++11中有两种默认捕获模式：按引用或按值。
// 按引用的默认捕获模式可能导致空悬引用，按值的默认捕获模式会忽悠你，
// 好像可以对空悬引用免疫，其实并没有，让人认为闭包是独立的，事实上他们可能不是独立的。

using FilterContainer = std::vector<std::function<bool(int)>>;
FilterContainer filters;
int computeSomeValue1() { return 1; }
int computeSomeValue2() { return 4; }
int computeDivisor(int x, int y) { return x + y; }

namespace n1 {
// 按引用捕获会导致闭包包含指向局部变量的引用，或者指向定义lambda的作用域内的形参的引用。
// 一旦由lambda式所创建的闭包越过了该局部变量或形参的生命期，那么闭包内的引用就会空悬。

void func() {
  // 可以像下面这样添加一个筛选5的倍数的函数：
  filters.emplace_back([](int value) { return value % 5 == 0; });

  // 但是，可能需要在运行期计算出除数，而不是把硬编码的5写入lambda表达式中：
  auto calc1 = computeSomeValue1();
  auto calc2 = computeSomeValue2();
  auto divisor = computeDivisor(calc1, calc2);

  filters.emplace_back([&](int value) { return value % divisor == 0; });
  // 这段代码随时会出错，lambda表达式包含指向局部变量divisor的引用，
  // 但该变量的在func返回时即不再存在，该变量紧接着filters.emplace返回后就销毁了。
  // 所以这就等于说，添加到筛选器聚集的那个函数刚刚被添加完就消亡了。
  // 使用这个筛选器，从它刚被创建的那一刻起，就会产生未定义的行为。

  // 就算不这样做，换做以显示方式按引用捕获divisor，问题依旧：
  filters.emplace_back([&divisor](int value) { return value % divisor == 0; });
  // 不过，通过显式捕获，确实较容易看出lambda式的生存依赖于divisor的生命期。
  // 而且，明白地写出divisor还提醒再次确认了divisor至少该和lambda闭包具有一样的生命周期。
  // 比起[&]所传达的这种不痛不痒的要保证没有空悬式的劝告，显式指明更让人印象深刻。

  // 解决这个问题的一种办法是对divisor采用按值的默认捕获模式：
  filters.emplace_back([=](int value) { return value % divisor == 0; });
}

// 如果知道闭包会被立即使用并且不会被复制，例如，传递给STL算法，
// 那么引用比它持有的局部变量或形参生命期更长，就不存在风险，可能会争论说，
// 这样的情况下，既然没有引用空悬风险，也就没有理由要避免使用默认引用捕获模式。
// 例如，lambda仅用作C++11的std::all_of的实参，
// 后者的作用是返回某作用域内的元素是否都满足某条件的判断：
template <typename C>
void workWithContainer(const C& container) {
  auto calc1 = computeSomeValue1();
  auto calc2 = computeSomeValue2();
  auto divisor = computeDivisor(calc1, calc2);

#if __cplusplus == 201103L  // C++11
  using ContElemt = typename C::value_type;
  if (std::all_of(
          std::begin(container), std::end(container),
          [&](const ContElemt& value) { return value % divisor == 0; })) {
#elif __cplusplus == 201402L
  if (std::all_of(std::begin(container), std::end(container),
                  [&](const auto& value) { return value % divisor == 0; })) {
#endif
    // 若全是，执行这里
  } else {
    // 若至少有一个不是，执行这里
  }

  // 不错，这样使用的确安全，但是这样的安全可谓朝不保，如果发现该lambda在其他语境中有用，
  // 然后被复制并粘贴到其他闭包比divisor生命期更长的语境中的话，就又被拖回悬空的困境了。
  // 这一回，在捕获语句中，可没有任何让你对divisor进行生命期分析的提示之物了。
}
}  // namespace n1

namespace n2 {
// 按值的默认捕获并非你想象中能够避免空悬的灵丹妙药。
// 问题在于，按值捕获了一个指针以后，在lambda创建的闭包中持有的是这个指针的副本，
// 但并无法阻止lambda之外的代码去针对该指针实施delete操作所导致的指针副本空悬。

namespace test1 {
// 你抗议道：“这种事根本不会发生！我已经看完了第4章，智能指针是我的崇拜！
// 只有没前途的C++98程序员才会使用裸指针和delete。”
// 可能的确如此，但你很难脱离干系，因为事实上，有时真的会使用裸指针，还有的时候，
// 它们会在眼皮底下实施delete操作，只不过现代C++编程风格中，在源代码中经常难觅其迹。
class Widget {
 public:
  void addFilter() const {
    filters.emplace_back([=](int value) { return value % divisor == 0; });
    // 在满心欢喜的外行看来，这像是安全的代码，虽然lambda式对divisor有依赖，
    // 但按值的默认捕获模式会确保divisor被复制到该lambda式创建的任何闭包里，对吗？
    // 错，错的彻底，错的离谱，错的无药可救。
    // 捕获只能针对于在创建lambda式的作用域内可见的非静态局部变量（包括形参）。
    // 在Widget::addFilter的函数体内，divisor并非局部变量，
    // 而是Widget类的成员变量，它压根无法被捕获。

    // 如果默认捕获模式被消除，代码就不会通过编译：
    // filters.emplace_back([](int value) { return value % divisor == 0; });

    // 而且，如果试图显式捕获divisor，无论按值还是引用，这无关紧要，
    // 这个捕获语句都不能通过编译，因为divisor既不是局部变量，也不是形参：
    // filters.emplace_back([divisor](int value) {return value%divisor ==0;});
    // filters.emplace_back([&divisor](int value) {return value%divisor==0;});

    // 为什么按值默认捕获能编译，关键在于this，每一个非静态成员函数都持有一个this指针，
    // 然后每当提及该类的成员变量时都会用到这个指针，会把divisor替换成this->divisor。
    // 从编译器视角来看，上述代码相当于：
    auto currentObjectPtr = this;
    filters.emplace_back([currentObjectPtr](int value) {
      return value % currentObjectPtr->divisor == 0;
    });
  }

 private:
  int divisor;
};

void func() {
  // lambda闭包存活与它含有其this指针副本的Widget对象的生命期是绑在一起的。
  // 特别地，考虑下面的代码，它掌握了第4章精髓，仅使用了智能指针：
  auto pw = std::make_shared<Widget>();  // 或者make_unique
  pw->addFilter();

  // addFilter中，该函数被添加到filters中，不过当doSomeWork执行结束之后，
  // Widget对象即被管理着它的生命期的std::shared_ptr销毁。
  // 从那一刻起，filters中就含有了一个带有空悬指针的元素。
}
}  // namespace test1

namespace test2 {
class Widget {
 public:
  void addFilter() const {
    // test1的问题可以通过将想捕获的成员变量复制到局部变量中，尔后捕获局部副本加以解决：
    auto divisorCopy = divisor;
    filters.emplace_back(
        [divisorCopy](int value) { return value % divisorCopy == 0; });

    // 如果采用这种方法，那么按值的默认捕获也能够运作：
    filters.emplace_back([=](int value) { return value % divisorCopy == 0; });
    // 但是为何要冒此不必要的风险？
    // 按值的默认捕获才是最开始造成意外地捕获了this指针，而不是期望中的divisor的始作俑者。

#if __cplusplus == 201402L
    // 在C++14中，捕获成员变量的一种更好的方法是使用广义Lambda捕获（条款32）：
    filters.emplace_back([divisorCopy = divisor](int value) {
      return value % divisorCopy == 0;
    });
    // 对广义lambda捕获而言，没有默认捕获模式一说。
    // 但是，就算在C++14中，本条款的建议（避免使用默认捕获模式）依然成立。
#endif
  }

 private:
  int divisor;
};
}  // namespace test2

namespace test3 {
// 使用默认值捕获模式的另一个缺点是，在于它似乎表明闭包是自洽的，
// 与闭包外的数据变化绝缘，作为一般的结论，这是不正确的。
// 因为lambda可能不仅依赖于局部变量和形参（可以被捕获），它们还会依赖于静态存储器对象。
// 这样的对象定义在全局或名字空间作用域中，又或在类、函数、文件中以static饰词声明。
// 这样的对象可以在lambda内使用，但是它们不能被捕获。
// 但如果使用了默认值捕获模式，这些对象就会给人以错觉，认为它们可以加以捕获，
// 思考下面这个前面见过的addDivisorFilter函数的修改版：

void func() {
  static auto calc1 = computeSomeValue1();
  static auto calc2 = computeSomeValue2();
  static auto divisor = computeDivisor(calc1, calc2);

  // 未捕获任何东西，但使用了前面以static饰词声明的对象：
  filters.emplace_back([=](int value) { return value % divisor == 0; });

  ++divisor;  // 意外修改了divisor

  // 一目十行的读者在看到代码中有着[=]后，就会想当然地认为：
  // lambda复制了它内部使用的对象，因此lambda式是自洽的。
  // 这无可厚非，因为它没有使用任何的非静态局部变量和形参，所以它没能捕获任何东西。
  // 但该lambda并不独立，更糟糕的是lambda的代码中向了静态变量divisor。
  // 因而，每次调用func的最后divisor都会被递增，从而在把好多个lambda式添加到filters时，
  // 每个lambda式的行为都不一样对应于divisor的新值，从实际效果来说，
  // 这个lambda式实现的效果是按引用捕获divisor，和按值默认捕获所暗示的含义有着直接的矛盾，
  // 如果从一开始就远离按值的默认捕获模式，也就能消除代码被如此勿读的风险了。
}
}  // namespace test3
}  // namespace n2

// 要记住的东西：
// 1.按引用的默认捕获会导致空悬指针问题；
// 2.按值的默认捕获极易受空悬指针影响，尤其是this，并会误导人们认为lambda是自洽的。

int main() { return 0; }