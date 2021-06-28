#include "SpaceStation.h"
#include "Asteroid.h"
#include "SpaceShip.h"

void SpaceStation2::collide(GameObject2& otherObject) {
  HitFunctionPtr hfp = lookup(otherObject);
  if (hfp) {
    (this->*hfp)(otherObject);
  } else {
    throw CollisionWithUnknownObject2(otherObject);
  }
}
void SpaceStation2::hitSpaceShip(GameObject2& spaceShip) {
  SpaceShip2& otherShip = dynamic_cast<SpaceShip2&>(spaceShip);
  cout << "SpaceStation2 SpaceShip2" << endl;
}
void SpaceStation2::hitSpaceStation(GameObject2& spaceStation) {
  SpaceStation2& station = dynamic_cast<SpaceStation2&>(spaceStation);
  cout << "SpaceStation2 SpaceStation2" << endl;
}
void SpaceStation2::hitAsteroid(GameObject2& asteroid) {
  Asteroid2& theAsteroid = dynamic_cast<Asteroid2&>(asteroid);
  cout << "SpaceStation2 Asteroid2" << endl;
}
SpaceStation2::HitFunctionPtr SpaceStation2::lookup(
    const GameObject2& whatWeHit) {
  static shared_ptr<HitMap> collisionMap(initializeCollisionMap());
  // cout << typeid(whatWeHit).name() << endl;
  HitMap::iterator mapEntry = collisionMap->find(typeid(whatWeHit).name());
  if (mapEntry == collisionMap->end()) return 0;
  return (*mapEntry).second;
}
SpaceStation2::HitMap* SpaceStation2::initializeCollisionMap() {
  HitMap* phm = new HitMap;
  (*phm)["10SpaceShip2"] = &SpaceStation2::hitSpaceShip;
  (*phm)["13SpaceStation2"] = &SpaceStation2::hitSpaceStation;
  (*phm)["9Asteroid2"] = &SpaceStation2::hitAsteroid;
  return phm;
}
