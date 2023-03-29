#ifndef FACTORY_H
#define FACTORY_H

#include "../body/body.h"
#include "../clothes/clothes.h"
#include "../shoes/shoes.h"

class Factory {
 public:
  virtual Body* createBody() = 0;
  virtual Clothes* createClothes() = 0;
  virtual Shoes* createShoes() = 0;
  virtual ~Factory() {}
};
#endif