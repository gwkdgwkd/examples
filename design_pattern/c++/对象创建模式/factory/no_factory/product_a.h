#ifndef PRODUCT_A_H
#define PRODUCT_A_H
#include "product.h"

class ProductA : public Product {
 public:
  virtual void show() { std::cout << "Product A" << std::endl; }
};
#endif