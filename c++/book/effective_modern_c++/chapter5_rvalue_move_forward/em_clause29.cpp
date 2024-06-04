#include <array>
#include <iostream>
#include <vector>

// 条款29：假定移动操作不存在、成本高、未使用

// 移动语义可以说在C++11的所有语言特性中占据着首要中的首要地位。
// 可能听说过下面的说法：
// 移动容器现在和复制指针一样成本低廉了！
// 复制临时对象现在已经如此高效，如果刻意在撰写代码总避免它，就无异于犯了过早优化的禁忌！

// 移动语义是个重要的语言特性，语言不只是允许编译器使用成本相对低廉的移动来代替昂贵的复制，
// 实际上语言会要求编译器这样做，只要满足适当条件就必须这样做。
// C++98的代码一字不改地使用符合C++11标准的编译器和标准库重新编译一遍，软件便可增速。

// 从为何许多类型不能支持移动语义的观察开始，整个C++98标准库都已为C++11彻底翻修过，
// 目的就是为那些的类型移动的可以实现成比复制更快的类型增添移动操作，
// 而且库组织的实现也已完成修订以充分利用这些移动操作，
// 不过问题在于有可能手上的代码存根并未完成修订以充分利用C++11的良好特性。
// 若应用中的（或采用的库中的）类型没有为C++11做过专门修改，
// 那么仅仅在编译器中有着对移动操作的支持也并不会给你带来什么明显的好处。
// 诚然，C++11愿意为这些缺少移动操作的类生成移动操作，
// 但这仅适用于那些未声明复制操作、移动操作以及析构函数的类，见条款17。
// 类型若是有数据成员或是基类禁用了移动（例如，通过将移动操作删除的方式参见条款11），
// 也将导致编译器生成的移动操作被抑制掉。
// 如果类型并不提供对移动的显式支持，也不符合编译器生成的移动操作的条件，
// 当然也就没有理由期望它在C++11下比在C++98下有任何性能的提升了。

struct Widget {};

namespace n1 {
// 即使对于那些显式支持移动操作的类型，也可能不会像人们希望的那样带来那么大的利好。
// 举个例子，在标准的c++11库中，所有的容器都支持移动操作，
// 但如果因此就断言所有的容器的移动都是成本低廉的，那就贻笑大方了。
// 对于有些容器而言，它们虽然有着确实成本低廉的移动操作，
// 但却又有一些附加条件造成其容器元素不能满足。

// 考虑一下std::array这个C++11中引入的新容器类型，它实质上就是带有STL接口的内建数组。
// 这一点和其他标准库容器存在根本差异，因为其他标准容器都是将其内容存放在堆上的，
// 从而在概念上，只需有个数据成员指向到存放容器内容的堆内存的地址。
// 由于该指针的存在，把整个容器的内容在常数时间内日加以移动就成为了可能：
// 仅仅把那个指针到容器内容的指针从源容器复制到目标容器，尔后吧源容器包含的指针置空即可。

void func() {
  // 移动vw1入vw2，完成执行仅需常数时间，仅仅是包含在vw1和vw2中的指针被修改了：
  std::vector<Widget> vw1;
  auto vw2 = std::move(vw1);

  // 移动aw1入aw2，完成执行需要线性时间，需要把aw1中的所有元素移动入aw2：
  std::array<Widget, 10000> aw1;
  auto aw2 = std::move(aw1);

  // aw1中的元素是被移动入aw2中的，假定Widget类型的移动比复制更快，
  // 则移动一个元素类型为Widget的std::array自然也会比复制一个同样的std::array更快。
  // 毫无疑问，std::array当然提供移动操作。
  // 然而无论是移动还是复制std::array类型的对象都还是需要线性时间的计算复杂度，
  // 因为容器中的每个元素都必须逐一复制或移动。
  // 这可比人们有时会听说的移动容器现在和赋值一堆指针一样成本低廉了这类说法，要超出太多了。
}
}  // namespace n1

namespace n2 {
// 作为对比，std::string类型提供的是常数时间的移动和线性时间的复制，这听起来像是在说，
// 它的移动比复制更快，但可能并非如此，许多string的实现都采用了小型字符串优化SSO，
// 采用了SSO以后，小型字符串，例如容量不超过15个字符的字符串，
// 会存储在std::string对象内的某个缓冲区内，而不去使用在堆上分配的存储。
// 在使用了基于SSO的实现的前提下，对小型字符串实施移动并不比复制更快，
// 因为通常在移动的性能优于复制背后仅复制一个指针的把戏会失灵了。

// 有着发明SSO的动机，就充分表明短字符串才是许多应用程序的标配，
// 使用一个内部缓冲区来存储这样的字符串，就消弭了为它们动态分配内存的需要，
// 而这往往在效率上是一步胜手，而这样做是一步胜手，就隐含着移动并不比复制更快的意思，
// 当然正好比半杯水既可以说是半空也可以说半满，前面这句话同样可以说成复制并不比移动更慢。

}  // namespace n2

// 即使是那些支持快速移动操作的类型，一些看似万无一失的移动场景还是以复制副本告终，
// 条款14解释过，标准库中一些容器操作提供了强异常安全保证，并且为了确保依赖于这样的保证，
// 并且为了确保依赖于这样的保证的那些C++98的遗留代码在升级到C++11时不会破坏这样的保证，
// 底层的复制操作只有在已知移动操作不会抛出异常的前提下才会使用移动操作将其替换。
// 这么做导致一个后果是，即使某个类型的移动操作比对应的复制操作更高效，
// 甚至在代码的某个特定位置，移动操作一般肯定不会有问题（例如，源对象是个右值的情况下），
// 编译器仍会强制去调用一个复制操作，只要对应的移动操作未加上noexcept声明。

// 总而言之，在这样几个场景中，C++11的移动语义不会给你带来什么好处：
// 1.没有移动操作，待移动的对象未能提供移动操作，因此，移动请求变成了复制请求；
// 2.移动未能更快，待移动的对象虽然有移动操作，但并不比其复制操作更快；
// 3.移动不可用，移动本可以发生的语境下，要求移动操作不可发射异常，
//   但该操作未加上noexcept声明；
// 4.源对象是个左值，除了极少数例外，条款25的例子，只有右值可以作为移动操作的源。

// 但本条款的标题是假定移动操作不存在、成本高、未使用。这比较典型地适合于通用代码的情形，
// 例如，撰写模板的时候，因为还不知道将要与哪些类型配合。
// 在这种情况下，必须像在使用C++98一样保守地去复制对象，正如移动语义上不存在那样。
// 这同样也符合不稳定代码的情形，即代码中所涉及类型的特征会比较频繁地加以修改。

// 然而，通常已知代码中会使用的类型，也可以肯定它们的特征不会改变，
// 例如，它们是否支持成本低廉的移动操作。
// 如果是这样的情况，就不需要前面那些假定，可以直接查阅所使用的类型对移动的支持细节。
// 如果涉及的类型能提供成本低廉的移动操作，并且是在这些移动操作会被调用的语境中使用对象，
// 则可以放心大胆地依靠移动语义来将复制操作替换成相对不那么昂贵的对应移动操作。

// 要记住的东西：
// 1.假定移动操作不存在、成本高、未使用；
// 2.对于那些类型或对于移动语义的支持情况已知的代码，则无需作以上假定。

int main() { return 0; }