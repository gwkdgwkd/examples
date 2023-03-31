#include "product_a.h"
#include "product_b.h"
#include "product_c.h"

int main(int argc, char* argv[]) {
  if (argc < 2) {
    std::cout << argv[0] << " i [0 - 2]" << std::endl;
    return 0;
  }
  int type = argv[1][0] - '0';

  Product* p;
  switch (type) {
    case 0:
      p = new ProductA();  // 使用new+具体类名来创建对象是一种依赖具体类型的紧耦合关系
      break;
    case 1:
      p = new ProductB();  // 要依赖具体产品的类名，不知道类名就创建不出产品
      break;
    case 2:
      p = new ProductC();  // 后续产品种类增加，继续采用这种方式实在是太低效且难以维护了
      break;
    default:
      std::cout << "invalid type" << std::endl;
      return -1;
  }

  p->show();
  delete p;

  return 0;
}