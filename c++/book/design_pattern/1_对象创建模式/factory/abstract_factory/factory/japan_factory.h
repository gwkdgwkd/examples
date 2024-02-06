#ifndef JAPAN_FACTORY_H
#define JAPAN_FACTORY_H

#include "../body/japan_body.h"
#include "../clothes/japan_clothes.h"
#include "../shoes/japan_shoes.h"
#include "factory.h"

class JapanFactory : public Factory {
 public:
  Body* createBody() { return new JapanBody(); }
  Clothes* createClothes() { return new JapanClothes(); }
  Shoes* createShoes() { return new JapanShoes(); }
};
#endif