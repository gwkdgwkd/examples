#ifndef AMERICA_SHOES_H
#define AMERICA_SHOES_H

#include "shoes.h"

class AmericaShoes : public Shoes {
 public:
  void showShoes() { std::cout << "America Shoes" << std::endl; }
};
#endif