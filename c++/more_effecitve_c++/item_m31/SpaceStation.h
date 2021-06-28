#ifndef __SPACESTATION_H_
#define __SPACESTATION_H_
#include "GameObject.h"

class SpaceStation2 : public GameObject2 {
 public:
  virtual void collide(GameObject2& otherObject);
  virtual void hitSpaceShip(GameObject2& spaceShip);
  virtual void hitSpaceStation(GameObject2& spaceStation);
  virtual void hitAsteroid(GameObject2& asteroid);

 private:
  typedef void (SpaceStation2::*HitFunctionPtr)(GameObject2&);
  static HitFunctionPtr lookup(const GameObject2& whatWeHit);
  typedef map<string, HitFunctionPtr> HitMap;
  static HitMap* initializeCollisionMap();
};

class SpaceStation3 : public GameObject3 {
 public:
  virtual void collide(GameObject3& otherObject) {}
};
#endif  // __SPACESTATION_H_