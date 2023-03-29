#ifndef AMERICA_CLOTHES_H
#define AMERICA_CLOTHES_H

#include "clothes.h"

class AmericaClothes : public Clothes {
 public:
  void showClothes() { std::cout << "America Clothes" << std::endl; }
};
#endif