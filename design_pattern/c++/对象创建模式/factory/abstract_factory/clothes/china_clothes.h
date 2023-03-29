#ifndef CHINA_CLOTHES_H
#define CHINA_CLOTHES_H

#include "clothes.h"

class ChinaClothes : public Clothes {
 public:
  void showClothes() { std::cout << "China Clothes" << std::endl; }
};
#endif