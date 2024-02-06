#ifndef AMERICA_FACTORY_H
#define AMERICA_FACTORY_H

#include "../body/america_body.h"
#include "../clothes/america_clothes.h"
#include "../shoes/america_shoes.h"
#include "factory.h"

class AmericaFactory : public Factory {
 public:
  Body* createBody() { return new AmericaBody(); }
  Clothes* createClothes() { return new AmericaClothes(); }
  Shoes* createShoes() { return new AmericaShoes(); }
};
#endif