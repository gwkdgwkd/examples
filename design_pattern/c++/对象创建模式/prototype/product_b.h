#ifndef PRODUCT_B_H
#define PRODUCT_B_H
#include "product.h"

class ProductB : public Product {
 public:
  virtual void show() { std::cout << "Product B" << std::endl; }
  Product *clone() { return new ProductB(*this); }
};
#endif