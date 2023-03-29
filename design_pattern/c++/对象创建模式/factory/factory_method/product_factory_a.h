#ifndef PRODUCT_FACTORY_A_H
#define PRODUCT_FACTORY_A_H

#include "../no_factory/product_a.h"
#include "product_factory.h"

class ProductFactoryA :public ProductFactory {
 public:
  Product* createProduct() { return new ProductA(); }
};
#endif