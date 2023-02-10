#include <iostream>
#include <list>

// 将构造函数和非成员函数虚拟化

// 从字面来看，谈论虚拟构造函数没有意义。
// 当有一个指针或引用，但是不知道其指向对象的真实类型是什么时，
// 可以调用虚拟函数来完成特定类型（type-specific）对象的行为。
// 仅当还没拥有一个对象但是你又确切地知道想要的对象的类型时，
// 才会调用构造函数，那么虚拟构造函数又从何谈起呢？
// 很简单，尽管虚拟构造函数看起来好像没有意义，其实它们有非常大的用处。
class NLComponent {
 public:
  // 至少包含一个纯虚函数
  virtual NLComponent* clone() const = 0;  // 虚拟拷贝构造函数
  virtual std::ostream& operator<<(
      std::ostream& str) const = 0;  // 不寻常的使用方法
  virtual std::ostream& print(std::ostream& s) const = 0;
};
class TextBlock : public NLComponent {
 public:
  // 不包含纯虚函数
  virtual TextBlock* clone() const { return new TextBlock(*this); }
  virtual std::ostream& operator<<(std::ostream& str) const {
  }  // 不寻常的使用方法
  virtual std::ostream& print(std::ostream& s) const {}
};
class Graphic : public NLComponent {
 public:
  // 不包含纯虚函数
  virtual Graphic* clone() const { return new Graphic(*this); }
  virtual std::ostream& operator<<(std::ostream& str) const {
  }  // 不寻常的使用方法
  virtual std::ostream& print(std::ostream& s) const {}
};
class NewsLetter {
 public:
  NewsLetter(std::istream& str);
  NewsLetter(const NewsLetter& rhs);

 private:
  std::list<NLComponent*> components;
  static NLComponent* readComponent(std::istream& str);
};
// 对象NewLetter不运行时就会存储在磁盘上。
// 为了能够通过位于磁盘的替代物来建立Newsletter对象，
// 让NewLetter的构造函数带有istream参数是一种很方便的方法。
// 当构造函数需要一些核心的数据结构时，它就从流中读取信息：
NewsLetter::NewsLetter(std::istream& str) {
  while (str) {
    // 从str读取下一个component对象；
    // 把对象加入到newsletter的components对象的链表中去；
  }
}
// 或者，把这种技巧用于另一个独立出来的函数叫做readComponent：
// NewsLetter::NewsLetter(istream& str) {
//   while (str) {
//     // 把readComponent返回的指针添加到components链表的最后，
//     // push_back一个链表的成员函数，用来在链表最后进行插入操作。
//     components.push_back(readComponent(str));
//   }
// }
// 考虑一下readComponent所做的工作，
// 它根据所读取的数据建立了一个新对象，或是TextBlock或是Graphic。
// 因为它能建立新对象，它的行为与构造函数相似，
// 而且因为它能建立不同类型的对象，我们称它为虚拟构造函数。
// 虚拟构造函数是指能够根据输入给它的数据的不同而建立不同类型的对象。
// 虚拟构造函数在很多场合下都有用处，
// 从磁盘（或者通过网络连接，或者从磁带机上）读取对象信息只是其中的一个应用。

// 还有一种特殊种类的虚拟构造函数――虚拟拷贝构造函数，也有着广泛的用途。
// 虚拟拷贝构造函数能返回一个指针，指向调用该函数的对象的新拷贝。
// 因为这种行为特性，虚拟拷贝构造函数的名字一般都是copySelf，
// cloneSelf或者是象下面这样就叫做clone，
// 很少会有函数能以这么直接的方式实现它：
// 正如看到的，类的虚拟拷贝构造函数只是调用它们真正的拷贝构造函数。
// 因此拷贝的含义与真正的拷贝构造函数相同。
// 如果真正的拷贝构造函数只做了简单的拷贝，那么虚拟拷贝构造函数也做简单的拷贝。
// 如果真正的拷贝构造函数做了全面的拷贝，那么虚拟拷贝构造函数也做全面的拷贝。
// 如果真正的拷贝构造函数做一些奇特的事情，象引用计数或copy-on-write，
// 那么虚拟构造函数也这么做，完全一致，太棒了。

// clone代码的实现利用了最近才被采纳的较宽松的虚拟函数返回值类型规则。
// 被派生类重定义的虚拟函数不用必须与基类的虚拟函数具有一样的返回类型。
// 如果函数的返回类型是一个指向基类的指针（或一个引用），
// 那么派生类的函数可以返回一个指向基类的派生类的指针（或引用）。
// 这不是C++的类型检查上的漏洞，它使得有可能声明象虚拟构造函数这样的函数。
// 这就是为什么TextBlock的clone函数能够返回TextBlock*，
// 和Graphic的clone能够返回Graphic*的原因，
// 即使NLComponent的clone返回值类型为NLComponent*。
// 在NLComponent中的虚拟拷贝构造函数，
// 能让实现NewLetter的(正常的)拷贝构造函数变得很容易：
NewsLetter::NewsLetter(const NewsLetter& rhs) {
  // 遍历整个rhs链表，使用每个元素的虚拟拷贝构造函数，
  // 把元素拷贝进这个对象的component链表：
  for (std::list<NLComponent*>::const_iterator it = rhs.components.begin();
       it != rhs.components.end(); ++it) {
    // it指向rhs.components的当前元素，调用元素的clone函数，
    // 得到该元素的一个拷贝，并把该拷贝放到这个对象的component链表的尾端。
    components.push_back((*it)->clone());
  }
}
// 遍历被拷贝的NewsLetter对象中的整个component链表，
// 调用链表内每个元素对象的虚拟构造函数。
// 在这里需要一个虚拟构造函数，因为链表中包含指向NLComponent对象的指针，
// 但是其实每一个指针不是指向TextBlock对象就是指向Graphic对象。
// 无论它指向谁，都想进行正确的拷贝操作，虚拟构造函数能够做到这点。

// 虚拟化非成员函数
// 就象构造函数不能真的成为虚拟函数一样，非成员函数也不能成为真正的虚拟函数。
// 然而，既然一个函数能够构造出不同类型的新对象是可以理解的，
// 那么同样也存在非成员函数，可以根据参数的不同动态类型而其行为特性也不同。
// 例如，假设想为TextBlock和Graphic对象实现一个输出操作符。
// 显而易见的方法是虚拟化这个输出操作符。
// 但是输出操作符是operator<<，
// 函数把ostream&做为它的左参数（left-hand argument），
// 即把它放在函数参数列表的左边，这就不可能使该函数成为成员函数。
// 为了能够回到正常的语法上来，必须把operator<<移出TextBlock和Graphic类，
// 但是如果这样做，就不能再把它声明为虚拟了。

// 另一种方法是为打印操作声明一个虚拟函数，例如print，
// 把它定义在TextBlock和Graphic类里。
// 但是如果这样，打印TextBlock和Graphic对象的语法，
// 就与使用operator<<做为输出操作符的其它类型的对象不一致了，
// 这些解决方法都不很令人满意。
// 想要的是一个operator<<的非成员函数，其具有类似print虚拟函数的行为特性。
// 有关想要什么的描述实际上已经很接近如何得到它的描述。
// 定义operator<<和print函数，让前者调用后者：
inline std::ostream& operator<<(std::ostream& s, const NLComponent& c) {
  return c.print(s);
}
// 具有虚拟行为的非成员函数很简单。
// 编写一个虚拟函数来完成工作，然后再写一个非虚拟函数，
// 它什么也不做只是调用这个虚拟函数。
// 为了避免这个句法花招引起函数调用开销，当然可以内联这个非虚拟函数。

// 知道如何根据它们的一个参数让非成员函数虚拟化，
// 可能想知道是否可能让它们根据一个以上的参数虚拟化呢？
// 可以，但是不是很容易，参见条款M31。

int main() {
  TextBlock t;
  Graphic g;
  // 通过virtual operator<<把t打印到cout中，不寻常的语法：
  t << std::cout;  
  g << std::cout;
  // 类的使用者得把stream对象放到<<符号的右边，这与输出操作符一般的用法相反。

  // 虚拟非成员函数：
  std::cout << t;
  std::cout << g;

  return 0;
}