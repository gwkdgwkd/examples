#ifndef PRODUCT_FACTORY_H
#define PRODUCT_FACTORY_H

#include "../product/product_a.h"
#include "../product/product_b.h"
#include "../product/product_c.h"

class ProductFactory {
 public:
  Product* createProduct(int product) {
    Product* p = nullptr;
    if (product == 0) {
      p = new ProductA();
    } else if (product == 1) {
      p = new ProductB();
    } else if (product == 2) {  // 新增的产品C：修改工厂类，添加新的分支
      p = new ProductC();
    } else {
      std::cout << "no product for " << product << std::endl;
    }

    return p;
  }
};
#endif