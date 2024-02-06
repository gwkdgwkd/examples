#ifndef SHOES_H
#define SHOES_H

#include <iostream>

class Shoes {
 public:
  virtual void showShoes() = 0;
  virtual ~Shoes() {}
};
#endif