#include <iostream>
#include <memory>

using namespace std;

// 以对象管理资源

// 所谓资源就是，一旦用了它，将来必须还给系统。C++程序中最常使用的资源就是动态分配内存，但内存只是必须管理的众多资源之一。
// 其他常见的资源还包括文件描述符、互斥锁、图形界面中的字符和笔刷、数据库连接、以及网络socket。不论哪种，不使用时，必须还给系统。

class Investment {};
Investment *createInvestment() { return new (Investment); };
void f() {
  Investment *pInv = createInvestment();
  // ...
  // 若干情况下，f可能无法删除它得自createInvestment的对象：
  //  这个区域有一个过早return语句；
  //  createInvestment和delete位于循环内时，continue和goto语句过早退出；
  //  这个区域的语句抛出异常。
  delete pInv;  // 被忽略时，就泄露了对象对应的那块内存。
}
// 可以谨慎的编写程序防止上面的错误，但是代码会被修改，一切都有可能。

// 一种想法是：把资源放进对象内，便可依赖C++的“析构函数自动调用机制”来确保资源被释放。

// 许多资源被动态分配于heap内而后被用于单一区块或函数内。应该在控制流离开那个区块或函数时被释放。
// auto_ptr正是针对这种形势而设计的特质产品。是个“类指针对象”，也就是所谓的智能指针。其析构函数自动对其所指对象调用delete。
// void f1() {
//   std::auto_ptr<Investment> pInv(createInvestment());
//   // ...
// }
// 以对象管理资源的两个关键想法：
//  1 获得资源后立刻放进管理对象内
//    “以对象管理资源”的观念常被称为“资源取得时便是初始化的时机”（RAII），几乎总是在获得一笔资源后于统一语句内以它初始化某个管理对象。
//    有时候获得的资源被拿来赋值（而非初始化）某个对象，不论哪种做法，每个资源都在获得的同时立刻被放进管理对象中。
//  2 管理对象运用析构函数确保资源释放
//    不论控制流如何离开区块，一旦对象被销毁，其析构函数自然会被自动调用，于是资源被释放。
//    如果资源资源释放可能导致抛出异常，参考条款8。

// auto_ptr被销毁时会自动删除它所指之物，所以一定要注意别让多个auto_ptr同时指向同一对象。否则对象被被删除一次以上，未定义行为。
// 为预防这个问题，auto_ptr有一个不寻常的性质：若通过copy构造函数或copy assignment操作符复制它们，它们会变成null，复制所得的指针取得资源唯一拥有权。
// 上面的限制导致auto_ptr并非管理动态分配资源的神兵利器。

// auto_ptr的替代方案是“引用计数型智能指针”（RCSP）。记录资源的引用数量，只有无人指向时才自动删除。
// 但是无法打破环状引用（其实已经没有被使用的对象彼此互指）。
void f1() {
  shared_ptr<Investment> pInv(createInvestment());
  // ...
}
// shared_ptr的复制行为一如预期，所以可被用于STL容器以及其他“auto_ptr之非正统复制行为并不适用”的语境上。

// shared_ptr和auto_ptr两者都在其析构函数内delete，而不是delete[]。意味着在动态分配而得到的array身上使用是个坏主意，但可悲的是，可以通过编译。
// 并没有针对“C++动态分配数组”而设计的智能指针，TR1中也没有。因为vector和string几乎总是可以取代动态分配的数组。

// 请记住：
// 为防止资源泄露，请使用RAII对象，它们在构造函数中获得资源并在析构函数中释放资源。
// 两个常被使用的RAII class分别是shared_ptr和auto_ptr。前者比较好，copy行为比较直观。后者复制动作会使它（被复制物）指向null。

int main() {
  shared_ptr<int> spi(new int[1024]);  // shared_ptr使用delete而不是delete[]

  return 0;
}