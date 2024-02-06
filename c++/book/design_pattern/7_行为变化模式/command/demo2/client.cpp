#include <iostream>

#include "concrete_command.h"
#include "invoker.h"

// Command模式实现和思想都很简单，其关键就是将一个请求封装到一个类Command中，
// 再提供处理对象（Receiver），最后Command命令由Invoker激活。
// 另外，可以将请求接收者的处理抽象出来作为参数传给Command对象，
// 实际也就是回调的机制（Callback）来实现这一点，
// 也就是说将处理操作方法地址（在对象内部）通过参数传递给Command对象，
// Command对象在适当的时候（Invoke激活的时候）再调用该函数。

// Command模式的思想非常简单，但是Command模式也十分常见，并且威力不小，实际上，
// Command模式关键就是提供一个抽象的Command类，并将执行操作封装到Command类接口中，
// Command类中一般就是只是一些接口的集合，并不包含任何的数据属性，
// 当然在示例代码中，Command类有一个处理操作的Receiver类的引用，
// 但是其作用也仅仅就是为了实现这个Command的Excute接口。
// 这种方式在是纯正的面向对象设计者最为鄙视的设计方式，就像OO设计新手做系统设计的时候，
// 仅仅将Class作为一个关键字，将C种的全局函数找一个类封装起来就以为是完成了面向对象的设计。
// 但是世界上的事情不是绝对的，上面提到的方式在OO设计种绝大部分的时候可能是一个不成熟的体现，
// 但是在Command模式中却是起到了很好的效果，主要体现在：
// 1.Command模式将调用操作的对象和知道如何实现该操作的对象解耦，
//   在Command的结构图中，Invoker对象根本就不知道具体的是那个对象在处理Excute操作，
//   当然要知道是Command类别的对象，也仅此而已；
// 2.在Command要增加新的处理操作对象很容易，可以通过创建新的继承自Command的子类来实现；
// 3.Command模式可以和Memento模式结合起来，支持取消的操作。

int main() {
  Reciever* rev = new Reciever();
  Command* cmd = new ConcreteCommand(rev);
  Invoker* inv = new Invoker(cmd);

  inv->Invoke();
  // Reciever action.......
  // ConcreteCommand...

  return 0;
}