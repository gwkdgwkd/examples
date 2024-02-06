#ifndef RECIEVER_H
#define RECIEVER_H

#include <iostream>

class Reciever {
 public:
  Reciever() = default;
  ~Reciever() = default;
  void Action() { std::cout << "Reciever action......." << std::endl; }
};
#endif  // RECIEVER_H