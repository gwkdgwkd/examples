#include "student_li.h"
#include <iostream>

void Li::Student::display() { std::cout << "Li::display" << std::endl; }
void demo::display() { std::cout << "demo1::display" << std::endl; }
int demo::num = 20;