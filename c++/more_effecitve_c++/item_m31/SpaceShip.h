#ifndef __SPACESHIP_H_
#define __SPACESHIP_H_
#include "GameObject.h"

class SpaceShip2 : public GameObject2 {
 public:
  virtual void collide(GameObject2& otherObject);
  // 和后来的基于虚函数的方法相似，每种碰撞都由一个独立的函数处理，不过不同的是，
  // 这次，这些函数有着不同的名字，而不是都叫collide。
  virtual void hitSpaceShip(GameObject2& spaceShip);
  virtual void hitSpaceStation(GameObject2& spaceStation);
  virtual void hitAsteroid(GameObject2& asteroid);

 private:
  typedef void (SpaceShip2::*HitFunctionPtr)(GameObject2&);
  // 在SpaceShip::collide中，
  // 我们需要一个方法来映射参数otherObject的动态类型到一个成员函数指针（指向一个适当的碰撞处理函数）。
  // 一个简单的方法是创建一个映射表，给定的类名对应恰当的成员函数指针。
  // 直接使用一个这样的映射表来实现collide是可行的，但如果增加一个中间函数lookup时，将更好理解。
  // lookup函数接受一个GameObject参数，返回相应的成员函数指针。
  static HitFunctionPtr lookup(const GameObject2& whatWeHit);
  typedef map<string, HitFunctionPtr> HitMap;
  // 不过这意味着我们要付出拷贝赋值的代价。我们不想这么做。
  // 如果initializeCollisionMap()返回指针的话，我们就不需要付出这个代价，
  // 但这样就需要担心指针指向的map对象是否能在恰当的时候被析构了。
  // static HitMap initializeCollisionMap();
  // 幸好，有个两全的方法。我们可以将collisionMap改为一个灵巧指针。
  // 它将在自己被析构时delete所指向的对象。
  static HitMap* initializeCollisionMap();
};

class SpaceShip3 : public GameObject3 {
 public:
  virtual void collide(GameObject3& otherObject) {}
};

class CommercialShip : public SpaceShip3 {};
#endif  // __SPACESHIP_H_