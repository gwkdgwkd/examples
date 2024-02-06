#ifndef JAPAN_CLOTHES_H
#define JAPAN_CLOTHES_H

#include "clothes.h"

class JapanClothes : public Clothes {
 public:
  void showClothes() { std::cout << "Japan Clothes" << std::endl; }
};
#endif