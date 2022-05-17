#include "Asteroid.h"
#include "SpaceShip.h"
#include "SpaceStation.h"

void SpaceShip2::collide(GameObject2& otherObject) {
  HitFunctionPtr hfp = lookup(otherObject);
  if (hfp) {
    (this->*hfp)(otherObject);
  } else {
    // 如果我们能保持映射表和GameObject的继承层次的同步，
    // lookup就总能找到传入对象对应的有效函数指针。
    // 人终究只是人，就算再仔细，错误也会钻入软件。
    // 这就是我们为什么检查lookup的返回值并在其失败时抛异常的原因。
    throw CollisionWithUnknownObject2(otherObject);
  }
}
// 和后来的基于虚函数的方法相似，每种碰撞都由一个独立的函数处理，不过不同的是，
// 这次，这些函数有着不同的名字，而不是都叫collide。
void SpaceShip2::hitSpaceShip(GameObject2& spaceShip) {
  // 如果转换失败，dynamic_cast会抛出一个bad_cast异常。
  // 当然，它们从不会失败，因为碰撞函数被调用时不会带一个错误的参数类型的，只是谨慎一些更好。
  SpaceShip2& otherShip = dynamic_cast<SpaceShip2&>(spaceShip);
  cout << "SpaceShip2 SpaceShip2" << endl;
}
void SpaceShip2::hitSpaceStation(GameObject2& spaceStation) {
  SpaceStation2& station = dynamic_cast<SpaceStation2&>(spaceStation);
  cout << "SpaceShip2 SpaceStation2" << endl;
}
void SpaceShip2::hitAsteroid(GameObject2& asteroid) {
  Asteroid2& theAsteroid = dynamic_cast<Asteroid2&>(asteroid);
  cout << "SpaceShip2 Asteroid2" << endl;
}

// 在SpaceShip::collide中，
// 我们需要一个方法来映射参数otherObject的动态类型到一个成员函数指针（指向一个适当的碰撞处理函数）。
// 一个简单的方法是创建一个映射表，给定的类名对应恰当的成员函数指针。
// 直接使用一个这样的映射表来实现collide是可行的，但如果增加一个中间函数lookup时，将更好理解。
// lookup函数接受一个GameObject参数，返回相应的成员函数指针。
SpaceShip2::HitFunctionPtr SpaceShip2::lookup(const GameObject2& whatWeHit) {
  // static HitMap collisionMap = initializeCollisionMap();
  static shared_ptr<HitMap> collisionMap(initializeCollisionMap());
  // cout << typeid(whatWeHit).name() << endl;
  HitMap::iterator mapEntry = collisionMap->find(typeid(whatWeHit).name());
  if (mapEntry == collisionMap->end()) return 0;
  // return (*mapEntry).second而不是习惯上的mapEntry->second以满足STL的奇怪行为。
  return (*mapEntry).second;
}

// 不过这意味着我们要付出拷贝赋值的代价。我们不想这么做。
// 如果initializeCollisionMap()返回指针的话，我们就不需要付出这个代价，
// 但这样就需要担心指针指向的map对象是否能在恰当的时候被析构了。
// static HitMap initializeCollisionMap();
// 幸好，有个两全的方法。我们可以将collisionMap改为一个灵巧指针。
// 它将在自己被析构时delete所指向的对象。
SpaceShip2::HitMap* SpaceShip2::initializeCollisionMap() {
  HitMap* phm = new HitMap;
  (*phm)["10SpaceShip2"] = &SpaceShip2::hitSpaceShip;
  (*phm)["13SpaceStation2"] = &SpaceShip2::hitSpaceStation;
  (*phm)["9Asteroid2"] = &SpaceShip2::hitAsteroid;
  return phm;
}
