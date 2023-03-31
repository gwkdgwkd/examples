#include "product_a.h"
#include "product_b.h"
#include "product_c.h"

// Prototype 原型模式

// 动机：
// 1.在软件系统中，经常面临着某些结构复杂的对象的创建工作，由于需求的变化，
//   这些对象经常面临着剧烈的变化，但是它们却拥有比较稳定一致的接口；
// 2.如何应对这种变化？如何向客户程序（使用这些对象的程序）隔离出这些易变对象，
//    从而使得依赖这些易变对象的客户程序不随需求改变而变化？

// 定义：使用原型实例指定创建对象的种类，然后通过拷贝这些原型来创建新的对象。

// 要点总结：
// 1.Prototype模式同样用于隔离类对象的使用者和具体类型（易变类）之间的耦合关系，
//   它同样要求这些易变类拥有稳定的接口；
// 2.Prototype模式对于如何创建易变类的实体对象采用原型克隆的方法来做，
//   它使得可以非常灵活地动态创建拥有某些稳定接口的新对象，
//   所需工作仅仅是注册一个新类的对象（即原型），然后在任何需要的地方Clone；
// 3.Prototype模式中的Clone方法可以利用某些框架中的序列化来实现深拷贝。

// Prototype、Builder和AbstractFactory都是通过一个类来专门负责对象的创建工作，区别是：
// 1.Builder模式重在复杂对象的一步步创建（并不直接返回对象）；
// 2.AbstractFactory模式重在产生多个相互依赖类的对象；
// 3.Prototype模式重在从自身复制自己创建新类。

int main(int argc, char* argv[]) {
  if (argc < 2) {
    std::cout << argv[0] << " i [0 - 2]" << std::endl;
    return 0;
  }
  int type = argv[1][0] - '0';

  auto func = [](Product* p) {
    Product* product = p->clone();
    product->show();
    delete product;
  };

  Product* p;
  switch (type) {
    case 0:
      p = new ProductA();
      func(p);  // Product A
      break;
    case 1:
      p = new ProductB();
      func(p);  // Product B
      break;
    case 2:
      p = new ProductC();
      func(p);  // Product C
      break;
    default:
      std::cout << "invalid type" << std::endl;
      return -1;
  }

  delete p;

  return 0;
}