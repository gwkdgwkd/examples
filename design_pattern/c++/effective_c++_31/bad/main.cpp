#include <iostream>

#include "person.h"

// C++类定义式不只叙述了class接口，还包括十足的实现细节。
// 将导致编译依存关系（compilation dependency），
// 更严重的将导致连串编译依存关系（cascading compilation dependencies），
// 会对许多项目造成难以形容的灾难。

// Effective C++中的31条讲到想将文件的依赖遗存关系降低至最低，
// 可以通过handle class和interface class实现。

int main() {
  std::string name = "lwl";
  std::string birthdate = "1025";
  std::string address = "shbl";
  Person p(name, birthdate, address);

  std::cout << "hello," << p.Name() << std::endl;
  std::cout << p.BirthDate() << std::endl;
  std::cout << p.Address() << std::endl;

  return 0;
}

// 第一次make，所有文件都编译：
// g++  -DUSE_DEBUG -c -Wall person.cpp -o person.o
// g++  -DUSE_DEBUG -c -Wall main.cpp -o main.o
// g++  ./person.o  ./main.o   -o person

// 修改了person.h，person.o和person都要重新编译：
// g++  -DUSE_DEBUG -c -Wall person.cpp -o person.o
// g++  ./person.o  ./main.o   -o person