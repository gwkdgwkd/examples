#ifndef PRODUCT_H
#define PRODUCT_H
#include <iostream>

class Product {
 public:
  virtual void show() = 0;
  virtual ~Product() {}
};
#endif