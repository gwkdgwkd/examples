#ifndef CHINA_FACTORY_H
#define CHINA_FACTORY_H

#include "../body/china_body.h"
#include "../clothes/china_clothes.h"
#include "../shoes/china_shoes.h"
#include "factory.h"

class ChinaFactory : public Factory {
 public:
  Body* createBody() { return new ChinaBody(); }
  Clothes* createClothes() { return new ChinaClothes(); }
  Shoes* createShoes() { return new ChinaShoes(); }
};
#endif