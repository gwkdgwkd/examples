#ifndef CONCRETE_SUBJECT_H
#define CONCRETE_SUBJECT_H

#include <iostream>

#include "subject.h"

class ConcreteSubject : public Subject {
 public:
  void Request() { std::cout << "ConcreteSubject::Request" << std::endl; }
};
#endif  // CONCRETE_SUBJECT_H