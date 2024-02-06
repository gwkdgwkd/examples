#ifndef PRODUCT_FACTORY_C_H
#define PRODUCT_FACTORY_C_H

#include "../product/product_c.h"
#include "product_factory.h"

class ProductFactoryC : public ProductFactory {
 public:
  Product* createProduct() { return new ProductC(); }
};
#endif