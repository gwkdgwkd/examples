#ifndef PRODUCT_FACTORY_B_H
#define PRODUCT_FACTORY_B_H

#include "../product/product_b.h"
#include "product_factory.h"

class ProductFactoryB : public ProductFactory {
 public:
  Product* createProduct() { return new ProductB(); }
};
#endif