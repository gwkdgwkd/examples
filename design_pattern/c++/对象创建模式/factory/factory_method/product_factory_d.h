#ifndef PRODUCT_FACTORY_D_H
#define PRODUCT_FACTORY_D_H

#include "product_d.h"
#include "product_factory.h"

// 新增的产品D：添加产品工厂类
class ProductFactoryD : public ProductFactory {
 public:
  Product* createProduct() { return new ProductD(); }
};
#endif