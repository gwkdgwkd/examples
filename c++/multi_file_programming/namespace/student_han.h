#ifndef _STUDENT_HAN_H
#define _STUDENT_HAN_H
#include <iostream>

namespace Han {
class Student {
 public:
  void display();
};
}

namespace demo {
void display(int a);
// int num; 因为demo1.h中已经声明有同名的变量，取消注释会造成重定义错误
}
#endif