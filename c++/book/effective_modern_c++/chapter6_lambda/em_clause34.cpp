#include <chrono>
#include <functional>
#include <iostream>

// 条款34：优先选用lambda表达式，而非std::bind

// std::bind是C++98中std::bind1st和std::bind2nd的后继特性，但是，
// 作为一种非标准特性而言，std::bind在2005年就已经是标准库的组成部分了。
// 正是在那时，标准委员会接受了名称TR1的文档，里面就包含了std::bind的规格，
// 在TR1中，bind位于不同的名字空间，所以是std::tr1::bind而非std::bind，
// 还有一些接口细节与现在有所不同。
// 这样的历史意味着，有些开发者已经有了十多年std::bind的开发经验，
// 那么可能不太愿意放弃这么一个运作良好的工具，这可以理解，但是对于这个特定的情况，
// 改变是有收益的，因为在C++11中，相对于std::bind，lambda几乎总是更好的选择，
// 到了C++14，lambda不仅是优势变强，简直已成为不二之选。

using namespace std::chrono;
using namespace std::placeholders;
#if __cplusplus == 201402L
using namespace std::literals;
#endif

using Time = std::chrono::steady_clock::time_point;
using Duration = std::chrono::steady_clock::duration;
enum class Sound { Beep, Siren, Whistle };
void setAlarm(Time t, Sound s, Duration d) {}

namespace n1 {
// 之所以说优先选用lambda式，而非std::bind，最主要原因是lambda式具备更高的可读性。

void func1() {
  // 在程序的某处想要设置在一小时之后发出警报并持续30秒，警报的具体声音，却尚未决定。
  // 可以撰写lambda表达式，修改setAlarm的接口，这个新的接口只需指定声音即可：
  auto setSoundL = [](Sound s) {
    setAlarm(steady_clock::now() + hours(1), s, seconds(30));
  };
  // 将lambda里对setAlarm的调用突显了出来，这是个观感无奇的函数调用，
  // 就算没有lambda经验的人都能看的出来，传递给lambda的形参会作为实参传递给setAlarm。
}

void func2() {
#if __cplusplus == 201402L
  // 可以利用C++14所提供的秒s，毫秒ms和小时h等标准后缀来简化上述代码，
  // 这一点建立在C++11用户定义字面量这项能力的支持之上。
  // 这些后缀在std::literals名字空间里实现，所以上面的代码可以重写如下：
  auto setSoundL = [](Sound s) { setAlarm(steady_clock::now() + 1h, s, 30s); };
#endif
}

void func3() {
  // std::bind调用语句的首次尝试：
  auto setSoundB =
      std::bind(setAlarm, steady_clock::now() + hours(1), _1, seconds(30));

  // 不能像lambda一样把setAlarm的调用在这里突显出来，只需了解，在调用setSoundB时，
  // 会使用在调用std::bind时指定的时刻和时长来唤起setAlarm。
  // 对于初学者而言，占位符_1简直好比天书，
  // 但即使是行家也需要脑补出从占位符中数字到它在std::bind形参列表位置的映射关系，
  // 才能理解在调用setSoundB时传入的第一个实参，会作为第二个实参传递给setAlarm。
  // 该实参的类型在std::bind的调用过程中未加识别，
  // 所以还需要去查询setAlarm的声明方能决定应该传递何种类型的实参给到setSoundB。

  // 这段代码不正确，在lambda中，表达式steady_clock::now()+1h是setAlarm的实参，
  // 这一点清清楚楚，该表达式会在setAlarm被调用的时刻评估求值。
  // 这样做合情合理：就是想要在setAlarm被调用的时刻之后的一个小时启动警报。
  // 但在bind中，steady_clock::now()+1h作为实参被传递给了bind，而非setAlarm。
  // 意味着表达式评估求值的时刻是在调用bind的时刻，而且结果会被存储在结果绑定对象中。
  // 最终导致的结果是，警报被设定的启动时刻是在调用std::bind的时刻之后的一个小时，
  // 而非调用setAlarm的时刻之后的一个小时。
}

void func4() {
  // 欲解决func3的问题，就要求知会bind以延迟表达式的评估求值到调用setAlarm的时刻，
  // 而是先这一点的途径，就是在原来的std::bind里嵌套第二层std::bind的调用：

#if __cplusplus == 201402L
  auto setSoundB = std::bind(
      setAlarm, std::bind(std::plus<>(), steady_clock::now, 1h), _1, 30s);
  // 如果是从C++98的年代开始了解std::plus模板的，可能会感到一丝惊诧，
  // 因为代码中出现了一对尖括号之间没有指定类型的写法，
  // 即代码中有一处std::plus<>,而非std::plus<type>。
  // 在C++14中，标准运算符模板类型实参大多数情况下可以省略不写。
#elif __cplusplus == 201103L
  // 而C++11中则没有这样的特性，所以在C++11中，
  // 欲使用std::bind撰写与lambda式等价的代码，就只能像下面这样：
  auto setSoundB = std::bind(setAlarm,
                             std::bind(std::plus<steady_clock::time_point>(),
                                       steady_clock::now(), hours(1)),
                             -1, seconds(30));
#endif
}

// 如果到了这个份上，还是看不出lambda的实现版本更有吸引力的话，那真的要去检查视力了。
}  // namespace n1

namespace n2 {
// 一旦对setAlarm实施了重载，新的问题就会马上浮现，
// 加入有个重载版本会接受第四个形参，用以指定警报的音量：
enum class Volume { Normal, Loud, LoudPlusPlus };
void setAlarm(Time t, Sound s, Duration d, Volume v) {}

void func1() {
  // 之前那个Lambda会一如既往地工作，因为重载决议会选择三形参版本的setAlarm：
  auto setSoundL = [](Sound s) {
    using namespace std::chrono;
    setAlarm(steady_clock::now() + hours(1), s, seconds(30));
  };

  // 不过，对std::bind的调用，现在可就无法通过编译了：
  // auto setSoundB = std::bind(
  //     setAlarm, std::bind(std::plus<>(), steady_clock::now, 1h), _1, 30s);
  // auto setSoundB = std::bind(setAlarm,
  //                            std::bind(std::plus<steady_clock::time_point>(),
  //                                      steady_clock::now(), hours(1)),
  //                            -1, seconds(30));

  // 问题在于，编译器无法确定应该将哪个setAlarm版本传递给std::bind，
  // 它拿到的所有信息就只有一个函数名，而仅函数名本身是多义的。
}

void func2() {
  // 为使得std::bind的调用能够通过编译，setAlarm必须强制转型到适当的函数指针类型：
  using SetAlarm3ParamType = void (*)(Time t, Sound s, Duration d);
  auto setSoundB = std::bind(static_cast<SetAlarm3ParamType>(::setAlarm),
                             std::bind(std::plus<steady_clock::time_point>(),
                                       steady_clock::now(), hours(1)),
                             _1, seconds(30));

  // 但这么做，又带出来了lambda式和std::bind的另一个不同之处。
  // 在setSoundL的函数调用运算符中调用setAlarm是常规的函数唤起方式，
  // 这么一来，编译器就可以用惯常的手法将其内联：
  // setSoundL(Sound::Siren);  // 在这里，setAlarm的函数体大可以被内联

  // 可是，std::bind的调用传递了一个指涉到setAlarm的函数指针，
  // 而那就意味着在setSoundB的函数调用运算符中，setAlarm通过函数指针调用的。
  // 由于编译器不太会内联掉通过函数指针发起的函数调用，
  // 那也就意味着通过setSoundB调用setAlarm而被完全内联的几率，
  // 比起通过setSoundL调用setAlarm要低：
  // setSoundB(Sound::Siren);  // 在这里，setAlarm的函数体被内联的可能性不大
  // 综上所述，使用lambda式就有可能会生成比使用std::bind运行更快的代码。
}
}  // namespace n2

namespace n3 {
void func() {
  int lowVal = 0;
  int highVal = 10;

#if __cplusplus == 201402L
  auto betweenL = [lowVal, highVal](const auto& val) {
    return lowVal <= val && val <= highVal;
  };

  auto betweenB = std::bind(std::logical_and<>(),
                            std::bind(std::less_equal<>(), lowVal, _1),
                            std::bind(std::less_equal<>(), _1, highVal));
#elif __cplusplus == 201103L
  auto betweenL = [lowVal, highVal](int val) {
    return lowVal <= val && val <= highVal;
  };

  auto betweenB = std::bind(std::logical_and<bool>(),
                            std::bind(std::less_equal<int>(), lowVal, _1),
                            std::bind(std::less_equal<int>(), _1, highVal));
#endif
}

// 不管是用C++11还是C++14，lambda的版本不仅更加短小，还更易于理解和维护。
}  // namespace n3

namespace n4 {
struct Widget {};
enum class CompLevel { Low, Normal, High };         // 压缩等级
Widget compress(const Widget& w, CompLevel lev) {}  // 制作w的压缩副本

void func1() {
  Widget w;
  auto compressRateB = std::bind(compress, w, _1);
  // 当把w传递给std::bind时，然后加以存储，以供未来让compress调用时使用，
  // 它存储的位置是在对象compressRateB内，但它以哪一种方式存储的：按值还是按引用呢？
  // 如果w在对std::bind的调用动作与对compressRateB调用动作之间被修改了，
  // 如果采用按引用方式存储，那么存储起来的w的值也会随之修改，
  // 而如果采用按值存储，则存储起来的w值不会改变。
  // w是按值存储的，可是，了解答案的唯一途径，就是牢记std::bind的工作原理。
  // 在std::bind调用中，不能从表面分辨出来是按值存储的。

  compressRateB(CompLevel::High);  // 实参是采用什么方式传递的？
  // 还是那句话，欲知答案，唯一的途径是牢记std::bind的工作原理，
  // 绑定对象的所有实参都是按引用传递的，因为此种对象的函数调用运算符利用了完美转发。

  // 对比之下，采用lambda的方法，w无论是按值或按引用捕获，在代码中都是显明的：
  auto compressRateL = [w](CompLevel lev) { return compress(w, lev); };
  compressRateL(CompLevel::High);
  // w和形参lev都是按值传递，一目了然。
}

void func2() {
  // bind总是复制其实参，但调用方却可以通过ref达成按引用存储的效果，下述语句：
  Widget w;
  auto compressRateB = std::bind(compress, std::ref(w), _1);
  // 结果就是compressRateB的行为如同持有的是个指涉到w的引用，而非其副本。
}
}  // namespace n4

namespace n5 {
// 总而言之，比起lambda，使用bind可读性更差、表达力更低，运行效率也可能更糟。
// 在C++14中，根本没有使用std::bind的适当用例。
// 而在C++11中，std::bind仅在两个受限的场合还算有着使用的理由：
// 1.移动捕获，C++11的lambda没有提供移动捕获特性，
//   但可以通过结合std::bind和lambda式来模拟移动捕获，见条款32；
// 2.多态函数对象，因为绑定对象的函数调用运算符利用了完美转发，
//   它就可以接受任何类型的实参，除了在条款30讲过的那些完美转发的限制情况，
//   这个特点在想要绑定的对象具有一个函数调用运算符模板时，是有利用价值的。

class PolyWidget {
 public:
  template <typename T>
  void operator()(const T& param) {}
};

void func() {
  // std::bind可以采用如下方式绑定polyWidget类型的对象：
  PolyWidget pw;
  auto boundPW = std::bind(pw, _1);

  // 这么依赖，boundPw就可以通过任意类型的实参加以调用：
  boundPW(1930);       // 传递int给PolyWidget::operator()
  boundPW(nullptr);    // 传递nullptr给PolyWidget::operator()
  boundPW("Rosebud");  // 传递字符串字面量给PolyWidget::operator()

#if __cplusplus == 201402L
  // 在C++11中的lambda式，是无法达成上面的效果的。
  // 但是在C++14中，使用带有auto形参的lambda就可以轻而易举地达成同样的效果：
  auto boundPWL = [pw](const auto& param) { pw(param); };
#endif
}

// 这些都是边缘用例，而且也会转瞬即逝，因为支持C++14的lambda的编译器已经日渐普及。
// 当2005年bind被非正式地加入C++时，比其它在1998年的前身已经有了长足的进度，
// 而C++11的lambda则使得bind相形见绌，到了C++14，bind已经彻底失去了用武之地。
}  // namespace n5

// 要记住的东西：
// 1.lambda比起使用std::bind而言，可读性更好，表达力更强，可能运行效率也更高；
// 2.仅在C++11中，std::bind在实现移动捕获，
//   或是绑定到具备模板化的函数调用运算符的对象的场合中，可能尚有余热可以发挥。

int main() { return 0; }