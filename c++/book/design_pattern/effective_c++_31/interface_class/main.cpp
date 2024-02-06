#include <iostream>

#include "person.h"

int main() {
  std::string name = "lwl";
  std::string birthdate = "1025";
  std::string address = "shbl";
  std::shared_ptr<Person> p(Person::create(name, birthdate, address));

  std::cout << "hello," << p->Name() << std::endl;
  std::cout << p->BirthDate() << std::endl;
  std::cout << p->Address() << std::endl;

  return 0;
}

// 第一次make，所有文件都编译：
// Remove linked and compiled files......
// rm -rf  ./person.o  ./real_person.o  ./main.o person 
// g++  -DUSE_DEBUG -c -Wall person.cpp -o person.o
// g++  -DUSE_DEBUG -c -Wall real_person.cpp -o real_person.o
// g++    -c -o main.o main.cpp
// g++  ./person.o  ./real_person.o  ./main.o   -o person

// 修改了real_person.h，只有real_person.o和person重新编译，
// person.o和main.o不用重新编译：
// g++  -DUSE_DEBUG -c -Wall real_person.cpp -o real_person.o
// g++  ./person.o  ./real_person.o  ./main.o   -o person
