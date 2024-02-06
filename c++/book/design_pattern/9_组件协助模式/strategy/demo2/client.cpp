#include <iostream>

#include "concrete_strategy_a.h"
#include "concrete_strategy_b.h"
#include "context.h"

// Strategy模式和Template模式要解决的问题是相同（类似）的，
// 都是为了给业务逻辑（算法）具体实现和抽象接口之间的解耦。
// Strategy模式将逻辑（算法）封装到一个类（Context）里面，
// 通过组合的方式将具体算法的实现在组合对象中实现，
// 再通过委托的方式将抽象接口的实现委托给组合对象实现。

// 可以看到Strategy和Template解决了类似的问题，也正如在Template模式中分析的，
// Strategy模式和Template模式实际是实现抽象接口的两种方式，继承和组合：
// 1.要实现一个抽象接口，继承是一种方式：
//   将抽象接口声明在基类中，将具体的实现放在具体子类中。
// 2.组合（委托）是另外一种方式：
//   将接口的实现放在被组合对象中，将抽象接口放在组合类中。

// 这两种方式各有优缺点，先列出来：
// 1.继承
//   优点：易于修改和扩展那些被复用的实现。
//   缺点：
//   a.破坏了封装性，继承中父类的实现细节暴露给子类了；
//   b.白盒复用，原因在a中；
//   c.当父类的实现更改时，其所有子类将不得不随之改变；
//   d.从父类继承而来的实现在运行期间不能改变，编译期间就已经确定了。
// 2.组合
//   优点：
//   a.黑盒复用，因为被包含对象的内部细节对外是不可见的；
//   b.封装性好，原因为a；
//   c.实现和抽象的依赖性很小，组合对象和被组合对象之间的依赖性小；
//   d.可以在运行期间动态定义实现，通过指向抽象基类的指针。
//   缺点：系统中对象过多。

// 从上面对比中可以看出，组合相比继承可以取得更好的效果，
// 因此在面向对象的设计中的有一条很重要的原则就是：优先使用（对象）组合，而非（类）继承。

// 实际上，继承是一种强制性很强的方式，因此也使得基类和具体子类之间的耦合性很强。
// 例如在Template模式中在ConcreteClass1中定义的原语操作别的类是不能够直接复用，
// 除非继承自AbstractClass，而组合（委托）的方式则有很小的耦合性，
// 实现（具体实现）和接口（抽象接口）之间的依赖性很小，例如在本实现中，
// ConcreteStrategyA的具体实现操作很容易复用，例如另一个Context类AnotherContext，
// 只要组合一个指向Strategy的指针就可以很容易地复用ConcreteStrategyA的实现了。
// 在Bridge模式的问题和Bridge模式的分析中，正是说明了继承和组合之间的区别。

// 另外Strategy很State也有相似之处，但是State模式注重的对象在不同的状态下不同的操作。
// 两者之间的区别就是State中具体实现类中有一个指向Context的引用，而Strategy则没有。

void func(Strategy* ps) {
  Context* pc = new Context(ps);
  pc->DoAction();
  if (NULL != pc) delete pc;
}

int main(int argc, char* argv[]) {
  if (argc < 2) {
    std::cout << argv[0] << " i [0 - 1]" << std::endl;
    return 0;
  }
  int type = argv[1][0] - '0';

  switch (type) {
    case 0:
      func(new ConcreteStrategyA);
      // ConcreteStrategyA::AlgrithmInterface
      break;
    case 1:
      func(new ConcreteStrategyB);
      // ConcreteStrategyB::AlgrithmInterface
      break;
    default:
      std::cout << "invalid type" << std::endl;
      break;
  }

  return 0;
}