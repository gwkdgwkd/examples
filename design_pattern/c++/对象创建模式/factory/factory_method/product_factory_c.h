#ifndef PRODUCT_FACTORY_C_H
#define PRODUCT_FACTORY_C_H

#include "../no_factory/product_c.h"
#include "product_factory.h"

class ProductFactoryC : public ProductFactory {
 public:
  Product* createProduct() { return new ProductC(); }
};
#endif