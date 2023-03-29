#include "product_factory_a.h"
#include "product_factory_b.h"
#include "product_factory_c.h"
#include "product_factory_d.h"

// 工厂方法模式是使用频率最高的工厂模式，该模式又被简称为工厂模式或者多态工厂模式。
// 简单工厂违背了开闭原则，工厂方法通过增加新的工厂类来创建新产品，不会修改原来的函数。

// 工厂方法模式抽象出了工厂类，提供创建具体产品的接口，交由子类去实现，
// 并不只是为了封装具体产品对象的创建，而是要把具体产品对象的创建放到具体工厂类实现。

// 工厂方法模式的结构组成：
// 1.抽象工厂类，工厂方法模式的核心类，提供创建具体产品的接口，由具体工厂类实现；
// 2.具体工厂类，继承于抽象工厂，实现创建对应具体产品对象的方式；
// 3.抽象产品类，它是具体产品继承的基类；
// 4.具体产品类，具体工厂所创建的对象，就是此类。

// 优点：
// 1.抽象出了工厂类，并把具体产品对象的创建放到具体工厂类实现，
//   用户只需要关心所需产品对应的工厂，无需关心创建细节；
// 2.加入新产品时不需要修改工厂类，只需要增加新的具体工厂类即可，符合开闭原则，提高可扩展性。

// 缺点：
// 1.类的个数容易过多，增加系统复杂度；
// 2.在添加新产品时，除了编写新的产品类之外，还要编写该产品类对应的工厂类；
// 3.增加了系统抽象性和理解难度；
// 4.工厂方法本身利用了抽象，该模式中会引入抽象层，如果要动态创建产品类，还要引入反射技术；
// 5.一个工厂只能创建一个产品。

// 工厂方法模式的适用场景：
// 1.创建对象需要使用大量重复的代码；
// 2.客户端不依赖产品类，不关心实例如何被创建、实现等细节；
// 3.一个类通过其子类来指定创建哪个对象。

int main(int argc, char* argv[]) {
  if (argc < 2) {
    std::cout << argv[0] << " i [0 - 3]" << std::endl;
    return 0;
  }
  int type = argv[1][0] - '0';

  ProductFactory* pf;
  switch (type) {
    case 0:
      pf = new ProductFactoryA();  // 多态new
      break;
    case 1:
      pf = new ProductFactoryB();  // 不在依赖具体类（实现），都依赖接口
      break;
    case 2:
      pf = new ProductFactoryC();
      break;
    case 3:
      pf = new ProductFactoryD();  // 新增加的产品D
      break;
    default:
      std::cout << "invalid type" << std::endl;
      return -1;
  }

  createProduct(pf);
  delete pf;

  return 0;
}