#include <iostream>
#include <map>

// 在面向对象系统的设计何实现中，创建对象是最为常见的操作。
// 这里面就有一个问题：如果一个应用程序使用了太多的对象，就会造成很大的存储开销。
// 特别是对于大量轻量级（细粒度）的对象，比如在文档编辑器的设计过程中，
// 如果为没有字母创建一个对象的话，系统可能会因为大量的对象而造成存储开销的浪费。
// 例如一个字母a在文档中出现了100000次，而实际上可以让这一万个字母a共享一个对象，
// 当然因为在不同的位置可能字母a有不同的显示效果（例如字体和大小等设置不同），
// 在这种情况可以为将对象的状态分为外部状态和内部状态，
// 将可以被共享（不会变化）的状态作为内部状态存储在对象中，
// 而外部对象（如字体、大小等）可以在适当的时候将外部对象最为参数传递给对象，
// 例如在显示的时候，将字体、大小等信息传递给对象。

// 定义：
// 享元模式是一种结构型设计模式，它旨在减少对象的数量，从而降低内存和性能消耗。
// 享元模式的核心思想是共享相同状态的多个对象，而不是为每个对象都分配独立的内存。

// 享元模式包括以下几个关键角色：
// 1.享元工厂Flyweight Factory，享元工厂是一个类，它负责创建和管理享元对象，
//   它通常包括一个池（或缓存），用于存储已创建的享元对象，以便在需要时共享它们；
// 2.享元Flyweight，享元是一个接口或抽象类，它定义了享元对象的接口，
//   享元对象通常包括可以共享的内部状态和不可共享的外部状态；
// 3.具体享元Concrete Flyweight，具体享元是实现享元接口的具体类，
//   它包含了内部状态，并根据外部状态进行相应的操作；
// 4.客户端Client，客户端是使用享元模式的应用程序的一部分，
//   客户端通常负责维护外部状态，并在需要时从享元工厂获取共享的享元对象。

// Flyweight模式在实现过程中主要是要为共享对象提供一个存放的仓库（对象池），
// 可以通过C++ STL中Vector容器实现，当然就牵涉到STL编程的一些问题。
// 另外应该注意的就是对对象仓库（对象池）的管理策略（查找、插入等），
// 可以通过直接的顺序遍历实现的，当然可以使用其他更加有效的索引策略，
// 例如Hash表的管理策略，当时这些细节已经不是Flyweight模式本身要处理的了。

class Shape {  // 享元接口，图形
 public:
  virtual void draw(const std::string& color) = 0;
};

class Circle : public Shape {  // 具体享元，圆形
 public:
  Circle() : radius_(0) {}
  void setRadius(int radius) { radius_ = radius; }
  void draw(const std::string& color) override {
    std::cout << "Drawing a " << color << " circle with radius " << radius_
              << "." << std::endl;
  }

 private:
  int radius_;
};

class ShapeFactory {  // 享元工厂，管理并共享享元对象
 public:
  Shape* getCircle(const std::string& color) {
    // 当客户端请求创建圆形对象时，首先检查是否已经存在具有相同颜色的圆形对象。
    // 如果是，它将返回共享的对象，否则将创建一个新的对象并存储在池中。
    if (circles_.find(color) == circles_.end()) {
      Circle* circle = new Circle();
      circle->draw(color);
      circles_[color] = circle;
    }
    return circles_[color];
  }

 private:
  std::map<std::string, Shape*> circles_;
};

// 通过享元模式，可以共享相同颜色的圆形对象，从而减少内存消耗。
// 这在需要大量相似对象的场景中非常有用，例如图形绘制和游戏开发中的粒子系统。

// 享元、单例、缓存区别：
// 1.单例只能创建一个类对象，而享元一个类可以创建多个对象，
//   每个对象被多处代码引用共享，而享元有点类似单例的变体，多例；
// 2.享元是为了复用减少对象的创建，而缓存为了提高访问效率。

int main() {
  ShapeFactory factory;

  // 获取并绘制不同颜色的圆形：
  Shape* redCircle1 = factory.getCircle("Red");
  Shape* greenCircle1 = factory.getCircle("Green");
  Shape* blueCircle1 = factory.getCircle("Blue");

  // 再次获取相同颜色的圆形，实际上是共享了已存在的对象：
  Shape* redCircle2 = factory.getCircle("Red");
  Shape* greenCircle2 = factory.getCircle("Green");
  Shape* blueCircle2 = factory.getCircle("Blue");

  // 手动设置圆形的半径：
  dynamic_cast<Circle*>(redCircle1)->setRadius(5);
  dynamic_cast<Circle*>(greenCircle1)->setRadius(10);
  dynamic_cast<Circle*>(blueCircle1)->setRadius(8);

  redCircle2->draw("Red");      // 共享的圆形，半径为5
  greenCircle2->draw("Green");  // 共享的圆形，半径为10
  blueCircle2->draw("Blue");    // 共享的圆形，半径为8

  return 0;
}