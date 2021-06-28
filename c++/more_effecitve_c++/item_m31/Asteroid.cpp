#include "Asteroid.h"
#include "SpaceShip.h"
#include "SpaceStation.h"

void Asteroid2::collide(GameObject2& otherObject) {
  HitFunctionPtr hfp = lookup(otherObject);
  if (hfp) {
    (this->*hfp)(otherObject);
  } else {
    throw CollisionWithUnknownObject2(otherObject);
  }
}
void Asteroid2::hitSpaceShip(GameObject2& spaceShip) {
  SpaceShip2& otherShip = dynamic_cast<SpaceShip2&>(spaceShip);
  cout << "Asteroid2 SpaceShip2" << endl;
}
void Asteroid2::hitSpaceStation(GameObject2& spaceStation) {
  SpaceStation2& station = dynamic_cast<SpaceStation2&>(spaceStation);
  cout << "Asteroid2 SpaceStation2" << endl;
}
void Asteroid2::hitAsteroid(GameObject2& asteroid) {
  Asteroid2& theAsteroid = dynamic_cast<Asteroid2&>(asteroid);
  cout << "Asteroid2 Asteroid2" << endl;
}
Asteroid2::HitFunctionPtr Asteroid2::lookup(const GameObject2& whatWeHit) {
  static shared_ptr<HitMap> collisionMap(initializeCollisionMap());
  // cout << typeid(whatWeHit).name() << endl;
  HitMap::iterator mapEntry = collisionMap->find(typeid(whatWeHit).name());
  if (mapEntry == collisionMap->end()) return 0;
  return (*mapEntry).second;
}
Asteroid2::HitMap* Asteroid2::initializeCollisionMap() {
  HitMap* phm = new HitMap;
  (*phm)["10SpaceShip2"] = &Asteroid2::hitSpaceShip;
  (*phm)["13SpaceStation2"] = &Asteroid2::hitSpaceStation;
  (*phm)["9Asteroid2"] = &Asteroid2::hitAsteroid;
  return phm;
}