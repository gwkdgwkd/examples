#ifndef CLOTHES_H
#define CLOTHES_H

#include <iostream>

class Clothes {
 public:
  virtual void showClothes() = 0;
  virtual ~Clothes() {}
};
#endif