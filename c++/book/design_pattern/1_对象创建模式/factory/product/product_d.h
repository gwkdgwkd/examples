#ifndef PRODUCT_D_H
#define PRODUCT_D_H
#include "product.h"

// 新增的产品D：添加产品类
class ProductD : public Product {
 public:
  virtual void show() { std::cout << "Product D" << std::endl; }
};
#endif