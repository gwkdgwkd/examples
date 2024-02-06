#ifndef PRODUCT_H
#define PRODUCT_H
#include <iostream>

class Product {
 public:
  virtual void show() = 0;
  virtual Product* clone() = 0;  // 通过克隆自己来创建对象
  virtual ~Product() {}
};
#endif