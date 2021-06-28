#ifndef __COLLISION_H_
#define __COLLISION_H_
#include "Asteroid.h"
#include "SpaceShip.h"
#include "SpaceStation.h"

namespace {
// 用了无名的命名空间来包含实现碰撞处理函数所需要的函数。无名命名空间中的东西是当前编译单元（其实就是当前文件）私有的，很象被申明为文件范围内static的函数一样。
// 有了命名空间后，文件范围内的static已经不赞成使用了，你应该尽快让自己习惯使用无名的命名空间（只要编译器支持）。
void shipAsteroid(GameObject3& spaceShip, GameObject3& asteroid) {
  cout << "spaceShip and asteroid" << endl;
}
void shipStation(GameObject3& spaceShip, GameObject3& spaceStation) {
  cout << "spaceShip and spaceStation" << endl;
}
void asteroidStation(GameObject3& asteroid, GameObject3& spaceStation) {
  cout << "asteroid and spaceStation" << endl;
}

void asteroidShip(GameObject3& asteroid, GameObject3& spaceShip) {
  shipAsteroid(spaceShip, asteroid);
}
void stationShip(GameObject3& spaceStation, GameObject3& spaceShip) {
  shipStation(spaceShip, spaceStation);
}
void stationAsteroid(GameObject3& spaceStation, GameObject3& asteroid) {
  asteroidStation(asteroid, spaceStation);
}

void shipShip(GameObject3& spaceShip1, GameObject3& shipStation1) {
  cout << "spaceShip and spaceShip" << endl;
}
void stationStation(GameObject3& spaceStation1, GameObject3& spaceStation2) {
  cout << "shipStation and shipStation" << endl;
}
void asteroidAsteroid(GameObject3& asteroid1, GameObject3& asteroid2) {
  cout << "asteroid and asteroid" << endl;
}

typedef void (*HitFunctionPtr)(GameObject3&, GameObject3&);
typedef map<pair<string, string>, HitFunctionPtr> HitMap;
pair<string, string> makeStringPair(const char* s1, const char* s2);
HitMap* initializeCollisionMap();
HitFunctionPtr lookup(const string& class1, const string& class2);
}  // end namespace
void processCollision(GameObject3& object1, GameObject3& object2);

// 这个类允许我们在映射表中进行增加和删除操作，以及根据类型名对查找相应的碰撞处理函数。
// 它也使用了Item E26中讲的技巧来限制CollisionMap对象的个数为1，因为我们的系统中只有一个映射表。（更复杂的游戏需要多张映射表是可以想象到的。）
// 最后，它允许我们简化在映射表中增加对称性的碰撞（也就是说，类型T1的对象撞击T2的对象和T2的对象撞击T1的对象，其效果是相同的）的过程，
// 它自动增加对称的映射关系，只要addEntry被调用时可选参数symmetric被设为true。
class CollisionMap {
 public:
  typedef void (*HitFunctionPtr)(GameObject3&, GameObject3&);
  void addEntry(const string& type1, const string& type2,
                HitFunctionPtr collisionFunction, bool symmetric = true) {}
  void removeEntry(const string& type1, const string& type2) {}
  HitFunctionPtr lookup(const string& type1, const string& type2) {}
  static CollisionMap& theCollisionMap() {}

 private:
  CollisionMap();
  CollisionMap(const CollisionMap&);
};
// 借助于CollisionMap类，每个想增加映射关系的用户可以直接这么做：
// CollisionMap::theCollisionMap().addEntry("SpaceShip", "Asteroid", &shipAsteroid);
// CollisionMap::theCollisionMap().addEntry("SpaceShip", "SpaceStation", &shipStation);
// CollisionMap::theCollisionMap().addEntry("Asteroid", "SpaceStation", &asteroidStation);
// 必须确保在发生碰撞前就将映射关系加入了映射表。一个方法是让GameObject的子类在构造函数中进行确认。
// 这将导致在运行期的一个小小的性能开销。另外一个方法是创建一个RegisterCollisionFunction类：
class RegisterCollisionFunction {
 public:
  RegisterCollisionFunction(const string& type1, const string& type2,
                            CollisionMap::HitFunctionPtr collisionFunction,
                            bool symmetric = true) {
    CollisionMap::theCollisionMap().addEntry(type1, type2, collisionFunction,
                                             symmetric);
  }
};

#endif  // __COLLISION_H_