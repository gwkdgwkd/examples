#include "Collision.h"

namespace {
// 因为makeStringPair、initializeCollisionMap和lookup，
// 都是申明在无名的命名空间中的，它们的实现也必须在同一命名空间中。
// 这就是为什么这些函数的实现在上面被写在了一个无名命名空间中的原因，
// 必须和它们的申明在同一编译单元中：
// 这样链接器才能正确地将它们的定义与它们的前置申明关联起来。
pair<string, string> makeStringPair(const char* s1, const char* s2) {
  return pair<string, string>(s1, s2);
}
HitMap* initializeCollisionMap() {
  HitMap* phm = new HitMap;
  (*phm)[makeStringPair("10SpaceShip3", "9Asteroid3")] = &shipAsteroid;
  (*phm)[makeStringPair("10SpaceShip3", "13SpaceStation3")] = &shipStation;
  (*phm)[makeStringPair("10SpaceShip3", "10SpaceShip3")] = &shipShip;
  (*phm)[makeStringPair("9Asteroid3", "13SpaceStation3")] = &asteroidStation;
  (*phm)[makeStringPair("9Asteroid3", "10SpaceShip3")] = &asteroidShip;
  (*phm)[makeStringPair("9Asteroid3", "9Asteroid3")] = &asteroidAsteroid;
  (*phm)[makeStringPair("13SpaceStation3", "10SpaceShip3")] = &stationShip;
  (*phm)[makeStringPair("13SpaceStation3", "9Asteroid3")] = &stationAsteroid;
  (*phm)[makeStringPair("13SpaceStation3", "13SpaceStation3")] =
      &stationStation;
  return phm;
}
HitFunctionPtr lookup(const string& class1, const string& class2) {
  static shared_ptr<HitMap> collisionMap(initializeCollisionMap());
  HitMap::iterator mapEntry = collisionMap->find(make_pair(class1, class2));
  if (mapEntry == collisionMap->end()) return 0;
  return (*mapEntry).second;
}
}  // namespace
void processCollision(GameObject3& object1, GameObject3& object2) {
  // cout << typeid(object1).name() << "," << typeid(object2).name() << endl;
  HitFunctionPtr phf = lookup(typeid(object1).name(), typeid(object2).name());
  if (phf)
    phf(object1, object2);
  else
    throw UnknownCollision(object1, object2);
}