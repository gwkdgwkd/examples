#include "messager/messager_lite.h"
#include "messager/messager_perfect.h"
#include "messager_imp/messager_imp_mobile.h"
#include "messager_imp/messager_imp_pc.h"

// Bridge 桥模式

// 总结面向对象实际上就两句话：一是松耦合（Coupling），二是高内聚（Cohesion）。
// 面向对象系统追求的目标就是尽可能地提高系统模块内部的内聚、尽可能降低模块间的耦合。
// 然而这也是面向对象设计过程中最为难把握的部分，在OO系统的开发过程中遇到这样的问题：
// 1.客户给了一个需求，于是使用一个类来实现A；
// 2.客户需求变化，有两个算法实现功能，于是改变设计，通过一个抽象的基类，
//   再定义两个具体类实现两个不同的算法A1和A2；
// 3.客户又说要对应不同的操作系统，于是再抽象一个层次，作为一个抽象基类A0，
//   在分别为每个操作系统派生具体类A00和A01，其中A00表示原来的类A，
//   实现不同操作系统上的客户需求，这样就有了一共4个类；
// 4.可能用户的需求又有变化，比如说又有了一种新的算法；
// 5.陷入了一个需求变化的郁闷当中，也因此带来了类的迅速膨胀。

// Bridge模式则正是解决了这类问题。

// 动机：
// 1.由于某些类型的固有的实现逻辑，使得它们具有两个变化的维度，乃至多个纬度的变化；
// 2.如何应对这种多维度的变化？
//   如何利用面向对象技术来使得可以轻松地沿两个或多个方向变化，而不增加复杂度？

// 定义：将抽象部分（业务功能）与实现部分（平台实现）分离，使它们都可以独立地变化。

// 要点总结：
// 1.Bridge模式使用对象间的组合关系解耦了抽象和实现之间固有的绑定关系，
//   使得抽象和实现可以沿着各自的维度来变化，
//   所谓抽象和实现沿着各自纬度的变化，即子类化它们；
// 2.Bridge模式有时候类似于多继承方案，但是多继承方案往往违背单一职责原则，
//   即一个类只有一个变化的原因，复用性比较差，
//   Bridge模式是比多继承方案更好的解决方法；
// 3.Bridge模式的应用一般在两个非常强的变化维度，
//   有时一个类也有多于两个的变化维度，这时可以使用Bridge的扩展模式。

// Bridge模式解决了demo3中的问题：
// 假设m表示要支持的操作系统数量，n表示要支持的算法的数量，
// 那么有m+n个具体类，也就是4个具体类。
// 可能用户的需求又有变化，比如说增加了一个新的算法，
// 有要支持平板，那么需要3+3，6个具体类。

void func(Messager *p) {
  p->login();
  p->sendMessage();
  p->sendPicture();
}

int main(int argc, char *argv[]) {
  if (argc < 2) {
    std::cout << argv[0] << " i [0 - 3]" << std::endl;
    return 0;
  }
  int type = argv[1][0] - '0';

  MessagerImp *imp;
  Messager *p;
  switch (type) {
    case 0:
      // 运行时装配：
      imp = new MessagerImpPC();
      p = new MessagerLite(imp);
      func(p);
      // login: pc connect
      // sendMessage: pc writeText
      // sendPicture: pc drawShape
      break;
    case 1:
      imp = new MessagerImpPC();
      p = new MessagerPerfect(imp);
      func(p);
      // pc playSound -> login: pc connect
      // pc playSound -> sendMessage: pc writeText
      // pc playSound -> sendPicture: pc drawShape
      break;
    case 2:
      imp = new MessagerImpMobile();
      p = new MessagerLite(imp);
      func(p);
      // login: mobile connect
      // sendMessage: mobile writeText
      // sendPicture: mobile drawShape
      break;
    case 3:
      imp = new MessagerImpMobile();
      p = new MessagerPerfect(imp);
      func(p);
      // mobile playSound -> login: mobile connect
      // mobile playSound -> sendMessage: mobile writeText
      // mobile playSound -> sendPicture: mobile drawShape
      break;
    default:
      return -1;
  }
  delete imp;
  delete p;

  return 0;
}