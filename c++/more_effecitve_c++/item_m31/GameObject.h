#ifndef __GAMEOBJECT_H_
#define __GAMEOBJECT_H_
#include <iostream>
#include <map>
#include <memory>
#include <string>

using namespace std;

class GameObject2 {
 public:
  // 和开始时使用的基于RTTI的方法相似，GameObjcet类只有一个处理碰撞的函数，它实现必须的二重调度的第一重。
  virtual void collide(GameObject2& otherObject) = 0;
};
class CollisionWithUnknownObject2 {
 public:
  CollisionWithUnknownObject2(GameObject2& whatWeHit) {}
};

class GameObject3 {
 public:
  virtual void collide(GameObject3& otherObject) = 0;
};
class UnknownCollision {
 public:
  UnknownCollision(GameObject3& whatWeHit, GameObject3& w) {}
};
#endif  // __GAMEOBJECT_H_