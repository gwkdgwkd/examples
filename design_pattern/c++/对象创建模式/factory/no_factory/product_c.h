#ifndef PRODUCT_C_H
#define PRODUCT_C_H
#include "product.h"

// 新增的产品C：添加产品类
class ProductC : public Product {
 public:
  virtual void show() { std::cout << "Product C" << std::endl; }
};
#endif