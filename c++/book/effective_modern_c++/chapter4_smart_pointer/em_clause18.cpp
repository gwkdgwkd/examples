#include <iostream>
#include <memory>
#include <string>

// 条款18：对于独占资源使用std::unique_ptr

// 当需要一个智能指针时，std::unique_ptr通常是最合适的，默认情况下，
// std::unique_ptr大小等同于原始指针，而且对于大多数操作，执行的指令完全相同。
// 这意味着甚至可以在内存和时间都比较紧张的情况下使用它。
// 如果原始指针够小够快，那么std::unique_ptr一样可以。

// std::unique_ptr体现了专有所有权语义。
// 一个non-null std::unique_ptr始终拥有其指向的内容。
// 移动一个std::unique_ptr将所有权从源指针转移到目的指针，源指针被设为null。
// 拷贝一个std::unique_ptr是不允许的，因为如果能拷贝一个std::unique_ptr，
// 会得到指向相同内容的两个std::unique_ptr，每个都认为自己拥有资源并且应当最后销毁，
// 销毁时就会出现重复销毁，因此，std::unique_ptr是一种只可移动类型。
// 当析构时，一个non-null std::unique_ptr销毁它指向的资源。
// 默认情况下，资源析构通过对std::unique_ptr里原始指针调用delete来实现。

// std::unique_ptr的常见用法是作为继承层次结构中对象的工厂函数返回类型。

// 有一个投资类型（比如股票、债券、房地产等）的继承结构，使用基类Investment。
// 这种继承关系的工厂函数在堆上分配对象然后返回指针，调用方在不需要的时候有责任销毁对象。
// 这使用场景完美匹配std::unique_ptr，因为调用者对工厂返回的资源负责，
// 并且std::unique_ptr在自己被销毁时会自动销毁指向的内容。
class Investment {
 public:
  Investment(int type) : t(type) {}
  ~Investment() {}
  int getType() { return t; }

 private:
  int t;
};
class Stock : public Investment {
 public:
  Stock(int type) : Investment(type) {
    std::cout << "Stock(int) " << type << std::endl;
  }
};
class Bond : public Investment {
 public:
  Bond(int type) : Investment(type) {
    std::cout << "Bond(int) " << type << std::endl;
  }
};
class RealEstate : public Investment {
 public:
  RealEstate(int type) : Investment(type) {
    std::cout << "RealEstate(int) " << type << std::endl;
  }
};

namespace n1 {
// 对象使用给定实参创建，返回指向对象的std::unique_ptr：
template <typename... Ts>
std::unique_ptr<Investment> makeInvestment(int type, Ts&&... params) {
  std::unique_ptr<Investment> pInv(nullptr);
  if (type == 1) {
    pInv.reset(new Stock(std::forward<Ts>(params)...));
  } else if (type == 2) {
    pInv.reset(new Bond(std::forward<Ts>(params)...));
  } else if (type == 3) {
    pInv.reset(new RealEstate(std::forward<Ts>(params)...));
  }
  return pInv;
}

void func() {
  // 调用者应该在单独的作用域中使用返回的std::unique_ptr智能指针：
  auto pInvestment1 = makeInvestment(1, 1);  // Stock(int) 1
  auto pInvestment2 = makeInvestment(2, 2);  // Bond(int) 2
  auto pInvestment3 = makeInvestment(3, 3);  // RealEstate(int) 3

  // 但是也可以在所有权转移的场景中使用它，比如将工厂返回的std::unique_ptr移入容器中，
  // 然后将容器元素移入一个对象的数据成员中，然后对象过后被销毁。
  // 发生这种情况时，这个对象的std::unique_ptr数据成员也被销毁，
  // 并且智能指针数据成员的析构将导致从工厂返回的资源被销毁。
  // 如果所有权链由于异常或者其他非典型控制流出现中断（比如提前return或者循环中的break），
  // 则拥有托管资源的std::unique_ptr将保证指向内容的析构函数被调用，销毁对应资源。
  // 这个规则也有些例外，大多数情况发生于不正常的程序终止：
  // 1.如果一个异常传播到线程的基本函数（比如程序初始线程的main函数）外，
  //   或者违反noexcept说明，局部变量可能不会被销毁；
  // 2.如果std::abort或者退出函数被调用，
  //   如std::_Exit，std::exit，或std::quick_exit，局部变量一定没被销毁。

  // 销毁*pInvestment1、*pInvestment2、*pInvestment3
}
}  // namespace n1

namespace n2 {
// 默认情况下，销毁将通过delete进行，但是在构造过程中，
// std::unique_ptr对象可以被设置为使用（对资源的）自定义删除器：
// 当资源需要销毁时可调用的任意函数（或者函数对象，包括lambda表达式）。
// 如果通过makeInvestment创建的对象不应仅仅被delete，而应该先写一条日志。

#if __cplusplus == 201103L                     // C++11
auto delInvmt = [](Investment* pInvestment) {  // 自定义删除器，lambda表达式
  std::cout << "C++11 delete " << pInvestment->getType() << std::endl;
  delete pInvestment;
};
template <typename... Ts>
std::unique_ptr<Investment, decltype(delInvmt)> makeInvestment(int type,
                                                               Ts&&... params) {
#elif __cplusplus == 201402L
// 在C++14中，由于函数的返回类型推导存在，意味着可以以更简单，更封装的方式实现：
template <typename... Ts>
auto makeInvestment(int type, Ts&&... params) {
  auto delInvmt = [](Investment* pInvestment) {  // 自定义删除器，lambda表达式
    std::cout << "C++14 delete " << pInvestment->getType() << std::endl;
    delete pInvestment;
  };
#endif
  std::unique_ptr<Investment, decltype(delInvmt)> pInv(nullptr, delInvmt);
  if (type == 1) {
    pInv.reset(new Stock(std::forward<Ts>(params)...));
  } else if (type == 2) {
    pInv.reset(new Bond(std::forward<Ts>(params)...));
  } else if (type == 3) {
    pInv.reset(new RealEstate(std::forward<Ts>(params)...));
  }
  return pInv;
}

void func() {
  auto pInvestment1 = makeInvestment(1, 1);  // Stock(int) 1
  auto pInvestment2 = makeInvestment(2, 2);  // Bond(int) 2
  auto pInvestment3 = makeInvestment(3, 3);  // RealEstate(int) 3

  // delete 3
  // delete 2
  // delete 1

  // 存储makeInvestment结果到auto变量中，那么将忽略在删除过程中需要特殊处理的事实。
  // 当然，确实幸福，因为使用了unique_ptr意味着不需要关心什么时候资源应被释放，
  // 不需要考虑在资源释放时的逻辑，以及确保只释放一次，unique_ptr自动解决了这些问题。
  // 从使用者角度，makeInvestment接口很棒。
}

// 需要理解的：
// 1.delInvmt是从makeInvestment返回的对象的自定义的删除器。
//   所有的自定义的删除行为接受要销毁对象的原始指针，然后执行所有必要行为实现销毁操作，
//   在上面情况中，操作包括调用makeLogEntry然后应用delete。
//   使用lambda创建delInvmt是方便的，而且，比编写常规的函数更有效。
// 2.当使用自定义删除器时，删除器类型必须作为第二个类型实参传给std::unique_ptr。
//   在上面情况中，就是delInvmt的类型，这就是为什么：
//   makeInvestment返回类型是std::unique_ptr<Investment,decltype(delInvmt)>。
// 3.makeInvestment的基本策略是创建空的unique_ptr，然后指向合适类型的对象，然后返回。
//   为了将自定义删除器delInvmt与pInv关联，把delInvmt作为pInv构造函数的第二个实参。
// 4.尝试将原始指针（比如new创建）赋值给std::unique_ptr通不过编译，
//   因为是一种从原始指针到智能指针的隐式转换。
//   这种隐式转换会出问题，所以C++11的智能指针禁止这个行为。
//   这就是通过reset来让pInv接管通过new创建的对象的所有权的原因。
// 5.使用new时，使用std::forward把传给makeInvestment的实参完美转发出去。
//   这使调用者提供的所有信息可用于正在创建的对象的构造函数。
// 6.自定义删除器的形参类型是Investment*，不管在makeInvestment返回的类型是什么，
//   它最终在lambda表达式中，作为Investment*对象被删除。
//   这意味着通过基类指针删除派生类实例，为此，基类Investment必须有虚析构函数。
}  // namespace n2

namespace n3 {
// 当使用默认删除器时（如delete），可以合理假设std::unique_ptr对象和原始指针大小相同。
// 当自定义删除器时，情况可能不再如此：
// 1.函数指针形式的删除器，通常会使std::unique_ptr的从一个字（word）大小增加到两个；
// 2.对于函数对象形式的删除器来说，变化的大小取决于函数对象中存储的状态多少，
//   无状态函数对象（比如不捕获变量的lambda表达式）对大小没有影响，
//   这意味当自定义删除器可以实现为函数或者lambda时，尽量使用lambda。

namespace test1 {
auto delInvmt = [](Investment* pInvestment) {  // 无状态的lambda自定义删除器
  std::cout << "delete " << pInvestment->getType() << std::endl;
  delete pInvestment;
};

// 返回类型大小是Investment*的大小：
template <typename... Ts>
std::unique_ptr<Investment, decltype(delInvmt)> makeInvestment(Ts&&... args);
}  // namespace test1

namespace test2 {
void delInvmt(Investment* pInvestment) {  // 函数形式的自定义删除器
  std::cout << "delete " << pInvestment->getType() << std::endl;
  delete pInvestment;
}

// 返回类型大小是Investment*的指针加至少一个函数指针的大小：
template <typename... Ts>
std::unique_ptr<Investment, void (*)(Investment*)> makeInvestment(
    Ts&&... params);
}  // namespace test2

// 具有很多状态的自定义删除器会产生大尺寸std::unique_ptr对象。
// 如果发现自定义删除器使得的std::unique_ptr变得过大，需要审视修改设计。
}  // namespace n3

namespace n4 {
// 工厂函数不是std::unique_ptr的唯一常见用法，作为实现Pimpl Idiom的一种机制，
// 它更为流行，代码并不复杂，但有时并不直观，参考条款22。
// Pimpl Idiom是一种隐藏实现而减弱编译依赖性的思想，参考Effective C++条款31。

// std::unique_ptr有两种形式，一种用于单个对象std::unique_ptr<T>，
// 一种用于数组std::unique_ptr<T[]>，结果就是，指向哪种形式没有歧义。
// std::unique_ptr的API设计会自动匹配用法，比如operator[]就是数组对象，
// 解引用操作符（operator*和operator->）就是单个对象专有。

// 应该对数组的std::unique_ptr的存在兴趣泛泛，
// 因为std::array，std::vector，std::string这些更好用的数据容器应该取代原始数组。
// std::unique_ptr<T[]>有用的唯一情况是使用类似C的API返回一个指向堆数组的原始指针，
// 而你想接管这个数组的所有权。

void func() {
  // std::unique_ptr是C++11中表示专有所有权的方法，
  // 但是其最吸引人的功能之一是它可以轻松高效的转换为std::shared_ptr：
  std::shared_ptr<Investment> sp = n2::makeInvestment(1, 1);

  // Stock(int) 1
  // C++14 delete 1
}
}  // namespace n4

// 要记住的东西：
// 1.std::unique_ptr是轻量级、快速的、只可移动的管理专有所有权语义资源的智能指针；
// 2.默认情况，资源销毁通过delete实现，但是支持自定义删除器，
//   有状态的删除器和函数指针会增加std::unique_ptr对象的大小；
// 3.将std::unique_ptr转化为std::shared_ptr非常简单。

int main(int argc, char* argv[]) {
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
      n4::func();
      break;
    default:
      std::cout << "invalid type" << std::endl;
      break;
  }

  return 0;
}