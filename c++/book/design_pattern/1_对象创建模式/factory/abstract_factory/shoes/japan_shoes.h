#ifndef JAPAN_SHOES_H
#define JAPAN_SHOES_H

#include "shoes.h"

class JapanShoes : public Shoes {
 public:
  void showShoes() { std::cout << "Japan Shoes" << std::endl; }
};
#endif