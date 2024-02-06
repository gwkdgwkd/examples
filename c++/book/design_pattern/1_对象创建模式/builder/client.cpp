#include <iostream>

#include "audi_builder.h"
#include "benz_builder.h"
#include "director.h"

// Builder 构建器

// 生活中有着很多的Builder的例子，大学生活就是一个Builder模式的最好体验：
// 要完成大学教育，一般将大学教育过程分成4个学期进行，
// 因此没有学习可以看作是构建完整大学教育的一个部分构建过程，
// 每个人经过这4年的（4个阶段）构建过程得到的最后的结果不一样，
// 因为可能在四个阶段的构建中引入了很多的参数，每个人的机会和际遇不完全相同。

// Builder模式要解决的也正是这样的问题：
// 当要创建的对象很复杂的时候，通常是由很多其他的对象组合而成，
// 要把复杂对象的创建过程和这个对象的表示（展示）分离开来，
// 好处就是通过一步步的进行复杂对象的构建，由于在每一步的构造过程中可以引入参数，
// 使得经过相同的步骤创建最后得到的对象的展示不一样。

// 将复杂对象的构建与表示相分离，使得同样的构建过程（稳定）可以创建不同的表示（变化）。
// 它将变与不变相分离，即产品的组成部分是不变的，但每一部分是可以灵活选择的。

// 组成：
// 1.产品Product，它是包含多个组成部件的复杂对象，由具体建造者来创建其各个零部件；
// 2.抽象建造者Builder，它是一个包含创建产品各个子部件的抽象方法的接口，
//   通常还包含一个返回复杂产品的方法getResult()；
// 3.具体建造者Concrete Builder，实现Builder接口，
//   完成复杂产品的各个部件的具体创建方法；
// 4.指挥者Director，它调用建造者对象中的部件构造与装配方法完成复杂对象的创建，
//   在指挥者中不涉及具体产品的信息。

// 优点：
// 1.封装性好，构建和表示分离；
// 2.扩展性好，各个具体的建造者相互独立，有利于系统的解耦；
// 3.客户端不必知道产品内部组成的细节，建造者可以对创建过程逐步细化，
//   而不对其它模块产生任何影响，便于控制细节风险。

// 缺点：
// 1.产品的组成部分必须相同，这限制了其使用范围；
// 2.如果产品的内部变化复杂，如果产品内部发生变化，
//   则建造者也要同步修改，后期维护成本较大。

// 适用于以下情况：
// 1.当创建复杂对象的算法应该独立于该对象的组成部分以及它们的装配方式时；
// 2.当构造过程必须允许被构造的对象有不同的表示时。

// Builder和AbstractFactory很相似，都是用来创建大的复杂的对象，它们的区别是：
// 1.Builder强调的是一步步创建对象，并通过相同的创建过程可以获得不同的结果对象，
//   一般来说Builder模式中对象不是直接返回的，是最后一步返回的；
// 2.AbstractFactory模式中对象是直接返回的，
//   AbstractFactory模式强调的是为创建多个相互依赖的对象提供一个统一的接口。

int main(int argc, char* argv[]) {
  if (argc < 2) {
    std::cout << argv[0] << " i [0 - 1]" << std::endl;
    return 0;
  }
  int type = argv[1][0] - '0';

  CarBuilder* builder;
  Director* director = new Director();
  Car car;

  switch (type) {
    case 0:
      builder = new BenzBuilder();
      // set tire: benz_tire
      // set steering wheel: benz_steering_wheel
      // set engine: benz_engine
      // set engine: benz_window_1
      // set engine: benz_window_2
      // set engine: benz_window_3
      // set engine: benz_window_4
      break;
    case 1:
      builder = new AudiBuilder();
      // set tire: audi_tire
      // set steering wheel: audi_steering_wheel
      // set engine: audi_engine
      // set engine: audi_window_1
      // set engine: audi_window_2
      break;
    default:
      return -1;
  }

  director->set_builder(builder);
  car = director->ConstructCar();
  delete builder;

  return 0;
}
