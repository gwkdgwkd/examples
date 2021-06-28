#ifndef __ASTEROID_H_
#define __ASTEROID_H_
#include "GameObject.h"

class Asteroid2 : public GameObject2 {
 public:
  virtual void collide(GameObject2& otherObject);
  virtual void hitSpaceShip(GameObject2& spaceShip);
  virtual void hitSpaceStation(GameObject2& spaceStation);
  virtual void hitAsteroid(GameObject2& asteroid);

 private:
  typedef void (Asteroid2::*HitFunctionPtr)(GameObject2&);
  static HitFunctionPtr lookup(const GameObject2& whatWeHit);
  typedef map<string, HitFunctionPtr> HitMap;
  static HitMap* initializeCollisionMap();
};

class Asteroid3 : public GameObject3 {
 public:
  virtual void collide(GameObject3& otherObject) {}
};
#endif  // __ASTEROID_H_