#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <typeinfo>

#include "Asteroid.h"
#include "SpaceShip.h"
#include "SpaceStation.h"

#include "Collision.h"

using namespace std;

// 让函数根据一个以上的对象来决定怎么虚拟

class GameObject {
 public:
  virtual void collide(GameObject& otherObject) = 0;
};
class SpaceShip : public GameObject {
 public:
  virtual void collide(GameObject& otherObject);
};
class SpaceStation : public GameObject {
 public:
  virtual void collide(GameObject& otherObject) {}
};
class Asteroid : public GameObject {
 public:
  virtual void collide(GameObject& otherObject) {}
};
// void checkForCollision(GameObject& object1, GameObject& object2) {
//   if (theyJustCollided(object1, object2)) {
//     processCollision(object1, object2);
//   } else {
//   }
// }
// 当你调用processCollision()时，
// 你知道object1和object2正好相撞，
// 并且你知道发生的结果将取决于object1和object2的真实类型，
// 但你并不知道其真实类型；
// 你所知道的就只有它们是GameObject对象。
// 如果碰撞的处理过程只取决于object1的动态类型，
// 你可以将processCollision()设为虚函数，
// 并调用object1.processColliion(object2)。
// 如果只取决于object2的动态类型，也可以同样处理。
// 但现在，取决于两个对象的动态类型，
// 虚函数体系只能作用在一个对象身上，它不足以解决问题。
// 你需要的是一种作用在多个对象上的虚函数，C++没有提供这样的函数。
// 你必须用C++实现，所以必须找到一个方法，
// 来解决这个被称为二重调度（double dispatch）的问题。
// 这个名字来自于object-oriented programming community，
// 在那里虚函数调用的术语是message dispatch，
// 而基两个参数的虚调用是通过double dispatch实现的，
// 推而广之，在多个参数上的虚函数叫multiple dispatch。
// 有几个方法可以考虑：
// 但没有哪个是没有缺点的，这不该奇怪。
// C++没有直接提供double dispatch，
// 所以你必须自己完成编译器在实现虚函数时所做的工作。
// 如果容易的话，我们可能就什么都自己做了，并用C语言编程了。
// 我们没有，而且我们也不能够，所以系紧你的安全带，有一个坎途了。

// 用虚函数加RTTI
// 虚函数实现了一个单一调度，这只是我们所需要的一半；
// 编译器为我们实现虚函数，
// 所以我们在GameObject中申明一个虚函数collide。
// 实现二重调度的最常见方法：
// 就是和虚函数体系格格不入的if...then...else链。
// 在这种刺眼的体系下，首先是发现otherObject的真实类型，
// 然后测试所有的可能：
class CollisionWithUnknownObject {
 public:
  CollisionWithUnknownObject(GameObject& whatWeHit) {}
};
void SpaceShip::collide(GameObject& otherObject) {
  const type_info& objectType = typeid(otherObject);
  if (objectType == typeid(SpaceShip)) {
    SpaceShip& ss = static_cast<SpaceShip&>(otherObject);
    // process a SpaceShip - SpaceShip collision;
  } else if (objectType == typeid(SpaceStation)) {
    SpaceStation& ss = static_cast<SpaceStation&>(otherObject);
    // process a SpaceShip - SpaceStation collision;
  } else if (objectType == typeid(Asteroid)) {
    Asteroid& a = static_cast<Asteroid&>(otherObject);
    // process a SpaceShip - Asteroid collision;
  } else {
    throw CollisionWithUnknownObject(otherObject);
  }
}
// 这儿的代码一点都不复杂，它很容易实现，也很容易让它工作。
// RTTI只有一点令人不安：
// 它只是看起来无害，实际的危险来自于最后一个else语句，在这儿抛了一个异常。
// 我们的代价是几乎放弃了封装，
// 因为每个collide函数都必须知道所以其它同胞类中的版本。
// 尤其是，如果增加一个新的类时，
// 我们必须更新每一个基于RTTI的if...then...else链以处理这个新的类型。
// 即使只是忘了一处，程序都将有一个bug，而且它还不显眼。
// 编译器也没办法帮助检查这种疏忽，因为它们根本不知道我们在做什么。
// 这种类型相关的程序在C语言中已经很有一段历史了，
// 而我们也知道，这样的程序本质上是没有可维护性的。
// 扩充这样的程序最终是不可想象的，这是引入虚函数的主意原因：
// 将产生和维护类型相关的函数调用的担子由程序员转给编译器。
// 当我们用RTTI实现二重调度时，我们正退回到过去的苦日子中。
// 这种过时的技巧在C语言中导致了错误，它们C++语言也仍然导致错误。
// 认识到我们自己的脆弱，我们在collide函数中加上了最后的那个else语句，
// 以处理如果遇到一个未知类型。
// 这种情况原则上说是不可能发生的，但在我们决定使用RTTI时又怎么知道呢？
// 有很多种方法来处理这种未曾预料的相互作用，但没有一个令人非常满意。
// 在这个例子里，我们选择了抛出一个异常，
// 但无法想象调用者对这个错误的处理能够比我们好多少，
// 因为我们遇到了一个我们不知道其存在的东西。

// 只使用虚函数
// 其实有一个方法可以将用RTTI实现二重调度固有风险降到最低的，
// 不过在此之前让我们看一下怎么只用虚函数来解决二重调度问题。
// 这个方法和RTTI方法有这同样的基本构架。
// collide函数被申明为虚，并被所有派生类重定义，此外，
// 它还被每个类重载，每个重载处理一个派生类型：
class SpaceShip1;  // forward declarations
class SpaceStation1;
class Asteroid1;
class GameObject1 {
 public:
  virtual void collide(GameObject1& otherObject) = 0;
  virtual void collide(SpaceShip1& otherObject) = 0;
  virtual void collide(SpaceStation1& otherObject) = 0;
  virtual void collide(Asteroid1& otherobject) = 0;
};
class SpaceShip1 : public GameObject1 {
 public:
  virtual void collide(GameObject1& otherObject);
  virtual void collide(SpaceShip1& otherObject);
  virtual void collide(SpaceStation1& otherObject);
  virtual void collide(Asteroid1& otherobject);
};
void SpaceShip1::collide(GameObject1& otherObject) {
  otherObject.collide(*this);
}
void SpaceShip1::collide(SpaceShip1& otherObject) {
  cout << "SpaceShip1 SpaceShip1" << endl;
  //  process a SpaceShip-SpaceShip collision;
}
void SpaceShip1::collide(SpaceStation1& otherObject) {
  cout << "SpaceShip1 SpaceStation1" << endl;
  //  process a SpaceShip-SpaceStation collision;
}
void SpaceShip1::collide(Asteroid1& otherObject) {
  cout << "SpaceShip1 Asteroid1" << endl;
  //  process a SpaceShip-Asteroid collision;
}
class SpaceStation1 : public GameObject1 {
 public:
  virtual void collide(GameObject1& otherObject);
  virtual void collide(SpaceShip1& otherObject) {
    cout << "SpaceStation1 SpaceShip1" << endl;
  }
  virtual void collide(SpaceStation1& otherObject) {
    cout << "SpaceStation1 SpaceStation1" << endl;
  }
  virtual void collide(Asteroid1& otherobject) {
    cout << "SpaceStation1 Asteroid1" << endl;
  }
};
void SpaceStation1::collide(GameObject1& otherObject) {
  otherObject.collide(*this);
}
class Asteroid1 : public GameObject1 {
 public:
  virtual void collide(GameObject1& otherObject);
  virtual void collide(SpaceShip1& otherObject) {
    cout << "Asteroid1 SpaceShip1" << endl;
  }
  virtual void collide(SpaceStation1& otherObject) {
    cout << "Asteroid1 SpaceStation1" << endl;
  }
  virtual void collide(Asteroid1& otherobject) {
    cout << "Asteroid1 Asteroid1" << endl;
  }
};
void Asteroid1::collide(GameObject1& otherObject) {
  otherObject.collide(*this);
}
// 粗一看，它象依据参数的顺序进行循环调用，
// 也就是开始的otherObject变成了调用成员函数的对象，
// 而*this成了它的参数。
// 但再仔细看一下啦，它不是循环调用。
// 你知道的，编译器根据参数的静态类型决定调那一组函数中的哪一个。
// 在这儿，有四个不同的collide函数可以被调用，
// 但根据*this的静态类型来选中其中一个。
// 现在的静态类型是什么？
// 因为是在SpaceShip的成员函数中，
// 所以*this肯定是SpaceShip类型。
// 调用的将是接受SpaceShip参数的collide函数，
// 而不是带GameOjbect类型参数的collide函数。
// 一点都不混乱，也不麻烦，没有RTTI，也不需要为意料之外的对象类型抛异常。
// 不会有意料之外的类型的，这就是使用虚函数的好处。
// 实际上，如果没有那个致命缺陷的话，它就是实现二重调度问题的完美解决方案。
// 这个缺陷是，和前面看到的RTTI方法一样：每个类都必须知道它的同胞类。
// 当增加新类时，所有的代码都必须更新，不过，更新方法和前面不一样。
// 确实，没有if...then...else需要修改，但通常是更差：
// 每个类都需要增加一个新的虚函数。
// 就本例而言，如果你决定增加一个新类Satellite（继承于GameObjcet），
// 你必须为每个现存类增加一个collide函数。
// 修改现存类经常是你做不到的。
// 比如，你不是在写整个游戏，只是在完成程序框架下的一个支撑库，
// 你可能无权修改GameObject类或从其经常的框架类。
// 此时，增加一个新的成员函数（虚的或不虚的），都是不可能的。
// 也就说，你理论上有操作需要被修改的类的权限，但实际上没有。

// 总结一下就是：
// 如果你需要实现二重调度，最好的办法是修改设计以取消这个需要。
// 如果做不到的话，虚函数的方法比RTTI的方法安全，
// 但它限制了你的程序的可控制性（取决于你是否有权修改头文件）。
// 另一方面，RTTI的方法不需要重编译，
// 但通常会导致代码无法维护，自己做抉择啦！

void func1() {
  SpaceShip1 spaceShip;
  SpaceStation1 spaceStation;
  Asteroid1 asteroid;
  spaceShip.collide(spaceShip);
  spaceShip.collide(spaceStation);
  spaceShip.collide(asteroid);
  spaceStation.collide(spaceShip);
  spaceStation.collide(spaceStation);
  spaceStation.collide(asteroid);
  asteroid.collide(spaceShip);
  asteroid.collide(spaceStation);
  asteroid.collide(asteroid);
  // SpaceShip1 SpaceShip1
  // SpaceShip1 SpaceStation1
  // SpaceShip1 Asteroid1
  // SpaceStation1 SpaceShip1
  // SpaceStation1 SpaceStation1
  // SpaceStation1 Asteroid1
  // Asteroid1 SpaceShip1
  // Asteroid1 SpaceStation1
  // Asteroid1 Asteroid1
}

// 模拟虚函数表
// 有一个方法来增加选择，编译器通常创建一个函数指针数组（vtbl）来实现虚函数，
// 并在虚函数被调用时在这个数组中进行下标索引。
// 使用vtbl，编译器避免了使用if...then...else链，
// 并能在所有调用虚函数的地方生成同样的代码：
// 确定正确的vtbl下标，然后调用vtbl这个位置上存储的指针所指向的函数。
// 没理由说你不能这么做，如果这么做了，不但使得你基于RTTI的代码更具效率，
// 下标索引加函数指针的反引用通常比if...then...else高效，产生的代码也少。
// 同样也将RTTI的使用范围限定在一处：你初始化函数指针数组的地方。
void func2() {
  SpaceShip2 spaceShip2;
  SpaceStation2 spaceStation2;
  Asteroid2 asteroid2;
  spaceShip2.collide(spaceShip2);
  spaceShip2.collide(spaceStation2);
  spaceShip2.collide(asteroid2);
  spaceStation2.collide(spaceShip2);
  spaceStation2.collide(spaceStation2);
  spaceStation2.collide(asteroid2);
  asteroid2.collide(spaceShip2);
  asteroid2.collide(spaceStation2);
  asteroid2.collide(asteroid2);
  // SpaceShip2 SpaceShip2
  // SpaceShip2 SpaceStation2
  // SpaceShip2 Asteroid2
  // SpaceStation2 SpaceShip2
  // SpaceStation2 SpaceStation2
  // SpaceStation2 Asteroid2
  // Asteroid2 SpaceShip2
  // Asteroid2 SpaceStation2
  // Asteroid2 Asteroid2
}

// 使用非成员的碰撞处理函数
// 我们现在知道了怎么构造一个类似vtbl的映射表以实现二重调度的第二部分，
// 并且我们也知道了怎么将映射表的实现细节封装在lookup函数中。
// 因为这张表包含的是指向成员函数的指针，
// 所以在增加新的GameObject类型时仍然需要修改类的定义，
// 这还是意味着所有人都必须重新编译，即使他们根本不关心这个新的类型。
// 如果映射表中包含的指针指向非成员函数，那么就没有重编译问题了。
// 而且，转到非成员的碰撞处理函数将让我们发现一个一直被忽略的设计上的问题，
// 就是，应该在哪个类里处理不同类型的对象间的碰撞在前面的设计中，
// 如果对象1和对象2碰撞，而正巧对象1是processCollision的左边的参数，
// 碰撞将在对象1的类中处理；
// 如果对象2正巧是左边的参数，碰撞就在对象2的类中处理。
// 这个有特别的含义吗？是不是这样更好些：
// 类型A和类型B的对象间的碰撞应该既不在A中也不在B中处理，
// 而在两者之外的某个中立的地方处理？
// 如果将碰撞处理函数从类里移出来，
// 在给用户提供类定义的头文件时，不用带上任何碰撞处理函数。
// 理论上，这个实现和使用成员函数的版本是相同的，只有几个轻微区别：
// 1.HitFunctionPtr现在是一个指向非成员函数的指针类型的重定义;
// 2.意料之外的类CollisionWithUnknownObject 被改叫 UnknownCollision;
// 3.其构造函数需要两个对象作参数而不再是一个了。
//   这也意味着我们的映射需要三个消息了：两个类型名，一个HitFunctionPtr。
// 最终达到了我们的目的：
// 1.如果增加了新的GaemObject的子类，
//   现存类不需要重新编译（除非它们用到了新类）。
// 2.没有了RTTI的混乱和if...then...else的不可维护。
// 3.增加一个新类只需要做明确定义了的局部修改：
//   在initializeCollisionMap中增加一个或多个新的映射关系，
//   在processCollision所在的无名的命名空间中申明一个新的碰撞处理函数。

// 继承与模拟虚函数表
// 将实体类CommercialShip和MilitaryShip从抽象类SpaceShip继承。
// 假设CommercialShip和MilitaryShip在碰撞过程中的行为是一致的。
// 于是，我们期望可以使用相同的碰撞处理函数（在增加这两类以前就有的那个）。
// 尤其是，在一个MilitaryShip对象和一个Asteroid对象碰撞时，
// 我们期望调用
// void shipAsteroid(GameObject& spaceShip, GameObject& asteroid);
// 它不会被调用的，实际上，抛了一个UnknownCollision的异常。
// 因为lookup在根据类型名MilitaryShip和Asteroid，
// 在collisionMap中查找函数时没有找到。
// 虽然MilitaryShip可以被转换为一个SpaceShip，但lookup却不知道这一点。
// 而且，没有没有一个简单的办法来告诉它。
// 如果你需要实现二重调度，并且需要这儿的向上类型映射，
// 你只能采用我们前面讨论的二次虚函数调用的方法，
// 同时也意味着增加新类的时候，所有人都必须重新编译。

// 按目前情况来看，我们的设计完全是静态的。
// 每次我们注册一个碰撞处理函数，我们就不得不永远留着它。
// 如果我们想在游戏运行过程中增加、删除或修改碰撞处理函数，将怎么样？不提供。
// 但是是可以做到的，我们可以将映射表放入一个类，
// 并由它提供动态修改映射关系的成员函数。
// 用户于是可以使用此类型的一个全局对象来自动地注册他们所需要的函数：
RegisterCollisionFunction cf1("SpaceShip", "Asteroid", &shipAsteroid);
RegisterCollisionFunction cf2("SpaceShip", "SpaceStation", &shipStation);
RegisterCollisionFunction cf3("Asteroid", "SpaceStation", &asteroidStation);
// 因为这些全局对象在main被调用前就构造了，
// 它们在构造函数中注册的函数也在main被调用前就加入映射表了。
// 如果以后增加了一个派生类：
class Satellite : public GameObject3 {};
// 以及一个或多个碰撞处理函数
void satelliteShip(GameObject3& satellite, GameObject3& spaceShip) {}
void satelliteAsteroid(GameObject3& satellite, GameObject3& asteroid) {}
// 这些新函数可以用同样方法加入映射表而不需要修改现存代码：
RegisterCollisionFunction cf4("Satellite", "SpaceShip", &satelliteShip);
RegisterCollisionFunction cf5("Satellite", "Asteroid", &satelliteAsteroid);
// 这不会改变实现多重调度没有完美解决方法的事实。
// 但它使得容易提供数据给基于map的实现，
// 如果我们认为这种实现最接近我们的需要的话。
void func3() {
  SpaceShip3 spaceShip3;
  SpaceStation3 spaceStation3;
  Asteroid3 asteroid3;
  processCollision(spaceShip3, spaceShip3);
  processCollision(spaceShip3, spaceStation3);
  processCollision(spaceShip3, asteroid3);
  processCollision(spaceStation3, spaceStation3);
  processCollision(spaceStation3, spaceShip3);
  processCollision(spaceStation3, asteroid3);
  processCollision(asteroid3, spaceShip3);
  processCollision(asteroid3, spaceStation3);
  processCollision(asteroid3, asteroid3);
  // spaceShip3 and spaceShip3
  // spaceShip3 and spaceStation3
  // spaceShip3 and asteroid3
  // shipStation3 and shipStation3
  // spaceShip3 and spaceStation3
  // asteroid3 and spaceStation3
  // spaceShip3 and asteroid3
  // asteroid3 and spaceStation3
  // asteroid3 and asteroid3

  CommercialShip cs;
  // processCollision(cs, asteroid3);  // 抛出异常
}

// C++标准并没有规定type_info::name的返回值，
// 不同的实现，其行为会有区别。
// 例如，对于类Spaceship，type_info::name，
// 一个实现返回class SpaceShip。
// 更好的设计是通过它所关联的type_info对象的地址了鉴别一个类，
// 因为每个类关联的type_info对象肯定是不同的。
// HitMap于是应该被申明为 map<cont type_info*, HitFunctionPtr>。

int main() {
  func1();
  func2();
  func3();

  return 0;
}

// g++ -std=c++11 item_m31.cpp SpaceShip.cpp SpaceStation.cpp Asteroid.cpp Collision.cpp