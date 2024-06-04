#include <future>
#include <iostream>
#include <thread>
#include <vector>

// 条款38：对变化多端的线程句柄析构函数行为保持关注

// 条款37解释过，可联结的线程对应着一个底层系统执行线程，
// 未推迟任务（参见条款36）的期望和系统线程有类似关系。
// 这么一来，std::thread类型对象和期望对象都可以视作系统线程的句柄。

// 从这个视角来看，std::thread对象和期望对象的析构函数表现出如此不同的行为值得深思。
// 正如条款37所提及的，针对可联结的std::thread类型对象实施析构会导致程序终止，
// 因为隐式join和隐式detach两个显而易见的选择都被认为是更糟糕的选择。
// 而期望的析构函数从不会导致程序终止，有时候行为像是执行了一次隐式join，
// 有时候行为像是执行了一次隐式detash，有时候行为像是二者都没有执行。
// 这套线程句柄行为的大杂烩，值得仔细品鉴一番。

namespace n1 {
// 期望位于信道的一端，被调方把结果通过该信道传输给调用方。
// 通常以异步方式运行的被调方把其计算所得的结果，通常经由promise类型对象写入信道，
// 而调用方则使用一个期望来读取该结果。

// 但被调方的结果要存储在哪里呢？
// 在调用方唤起对应期望的get之前，被调方可能已经执行完毕，
// 因此结果不会存储在被调用方的std::promise类型对象里。
// 那个对象，对于被调方来说就是个局部量，在被调方结束后会实施析构。

// 该结果也不能存储在调用方的期望中，出于其他种种原因，
// 可能会从std::future类型对象出发创建std::shared_future类型对象，
// 因此把被调方结果的所有权从future类型对象转移至shared_future类型对象，
// 而后者可能会在原始的std::future析构之后复制多次。
// 如果被调方的结果类型不都是可复制的（即只移类型），
// 而该结果至少生存期要延至和最后一个指涉到它的期望一样长。
// 这么多个对应同一结果的期望中的哪一个，应该包含该结果呢？

// 既然与被调方相关联的对象和与调用方相关联的对象都不适合作为被调方结果的存储之所，
// 那就只能将该结果存储在位于两者外部的某个位置，这个位置称为共享状态。
// 共享状态通常使用堆上的对象来表示，但是其类型、接口和实现标准皆未指定。
// 标准库作者可以自由地用他们喜好的方法去实现共享状态。

// 共享状态的存在很重要，因为期望析构函数的行为是由与其关联的共享状态决定的：
// 1.指向经由async启动的未推迟任务的共享状态的最后一个期望会保持阻塞，直至该任务结束，
//   本质上，这样一个期望的析构函数是对底层异步执行任务的线程实施了一次隐式join；
// 2.其他所有期望对象的析构函数只仅仅将期望对象析构就结束了，对于底层异步运行的任务，
//   这样做类似于对线程实施了一次隐式detach，对于那些被推迟任务而言，
//   如果这一期望是最后一个，也就意味着被推迟的任务将不会有机会运行了。

// 这些规则听上去复杂，其实不然，真正需要关心的，
// 是一个平凡的常规行为外加一个不甚常见的例外而已。
// 常规行为是指期望的析构函数仅会析构期望对象，就这样，
// 它不会针对任何东西实施join,也不会从任何东西实施detach，也不会运行任何东西。
// 它仅会析构期望的成员变量，实际上，它还针对共享状态里的引用计数实施了一次自减。
// 该共享状态由指涉到它的期望和被调方的std::promise共同操作。
// 该引用计数使得库能知道何时可以析构共享状态。

// 而相对于正常行为的那个例外，只有在期望满足以下全部条件时才会发挥作用：
// 1.期望所指向的共享状态是由于调用了std::async才创建的；
// 2.该任务的启动策略是std::launch::async，
//   这既可能是运行时系统的选择，也可能是在调用std::async时指定的；
// 3.该期望是指涉到该共享状态的最后一个期望，对于std::future类型对象而言，
//   这一点总是成立，而对于std::shared_future类型对象而言，
//   在析构时如果不是最后一个指向共享状态的期望，则它会遵循常规行为仅析构其成员变量。
// 只有当所有条件都满足，期望的析构函数才会表现出特别行为。
// 而行为的具体表现为阻塞直到异步运行的任务结束。
// 从效果来看，这相当于针对正在运行的std::async所创建的任务的线程实施了一次隐式join。

// 经常会有人把这个例外和常规期望析构函数行为的差异说成是，
// 来自std::async的期望会在其析构函数里被阻塞。
// 如果只是最粗略的近似，这种说法也不为错，但有时候你需要比最粗略的近似了解得更深一步。

// 为什么要为从std::async出发启动的非推迟任务相关联的共享状态专门制定一条规则？
// 标准委员会想要避免隐式detach相关的问题（参见条款37），
// 但是他们又不想简单粗暴地让程序终止了事，他们针对可联结线程就是这样做的，参见条款37，
// 所以妥协结果就是实施一次隐式join，这个决定并非没有争议，
// 委员会也曾认真讨论过要在C++14中舍弃这样的行为，但是最后没有做出改变，
// 所以期望析构函数的行为在C++11和C++14中是保持了一致的。
}  // namespace n1

namespace n2 {
// 期望的API没有提供任何方法判断其指涉的共享状态是否诞生于std::async的调用，
// 所以对于任意期望对象，它不可能知道自己是否会在析构函数中阻塞到异步任务执行结束。
// 这个事实暗示着一些意味深长的推论：

// 该容器的析构函数可能会在其析构函数中阻塞，因为它所持有的期望中可能会有一个或多个，
// 指向经由std::async启动未推迟任务所产生的共享状态
std::vector<std::future<void>> futs;
class Widget {
 public:  // Widget类型对象可能会在其析构函数中阻塞
 private:
  std::shared_future<double> fut;
};

}  // namespace n2

namespace n3 {
// 当然，如果有办法判定给定的期望不满足触发特殊析构行为的条件，
// 例如，通过分析程序逻辑，即可断定该期望不会阻塞在其析构函数中。
// 例如，只有因std::async调用而出现的共享状态才够格去展示特别行为，
// 但是还有其它方法可以创建出共享状态，其中一个方法就是运用std::packaged_task，
// std::packaged_task类型对象会准备一个函数（或其他可调用的对象）以供异步执行，
// 手法是将它加上一层包装，把其结果置入共享状态。
// 而指涉到该共享状态的期望则可以经由std::packaged_task的get_future函数得到：

int calcValue() {}
void func() {
  std::packaged_task<int()> pt(calcValue);
  auto fut = pt.get_future();
  // 期望fut没有指向std::async调用产生的共享状态，所以它的析构函数将表现出常规行为。

  // std::packaged_task类型对象pt一经创建，就会运行在线程之上，
  // 它也可以经由std::async的调用而运行，但是如果你要用std::async运行任务，
  // 就没有很好的理由再去创建什么std::packaged_task类型对象，
  // 因为std::async能够在调度任务执行之前就做到std::packaged_task能够做到的任何事情。

  // std::packaged_task不能复制，所以欲将pt传递给std::thread的构造函数，
  // 就一定要将它强制转型有右值：
  std::thread t(std::move(pt));

  // ...
}

// 这里最值得探讨的代码是...部分，它位于t创建之后、代码块结束之前。
// 值得探讨的是，在...中t的命运如何，基本存在三种可能：
// 1.未对t实施任何操作，此时t在作用域结束点是可联结的，而这将导致程序终止；
// 2.针对t实施了join，此时fut无须在析构函数中阻塞，因为在调用的代码已经有过join；
// 3.针对t实施了detach，此时fut无须在任何析构函数中实施detach，
// 因为在调用的代码已经做过了这件事了

// 换句话说，当的期望所对应的共享状态是由std::packaged_task产生的，
// 则通常无需采用特别的析构策略，因为，关于是终止、联结还是分离的决定，
// 会由操纵std::thread的代码作出，而std::packaged_task通常就运行在该线程之上。
}  // namespace n3

// 要记住的东西：
// 1.期望的析构函数在常规情况下，仅会析构期望的成员变量；
// 2.指向经async启动的未推迟任务的共享状态的最后一个期望会保持阻塞，直至该任务结束。

int main() { return 0; }