#ifndef PRODUCT_FACTORY_H
#define PRODUCT_FACTORY_H

#include "../no_factory/product.h"

class ProductFactory {
 public:
  virtual Product* createProduct() = 0;
  virtual ~ProductFactory() {}
};

void createProduct(ProductFactory* pf) {
  // 这个函数应该在单独的文件中，也可以是类，只依赖产品接口和工厂接口，
  // 不依赖具体的产品和具体的工厂，新增产品D时，该函数不用修改，
  // 通过增加扩展而不是修改已有代码的方式来实现创建新产品对象，符合开闭原则。
  Product* p = pf->createProduct();
  p->show();
  delete p;
}
#endif