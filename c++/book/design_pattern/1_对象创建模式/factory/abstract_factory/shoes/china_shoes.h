#ifndef CHINA_SHOES_H
#define CHINA_SHOES_H

#include "shoes.h"

class ChinaShoes : public Shoes {
 public:
  void showShoes() { std::cout << "China Shoes" << std::endl; }
};
#endif