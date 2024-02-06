#ifndef PROXY_H
#define PROXY_H

#include <iostream>

#include "subject.h"

class Proxy {
 public:
  Proxy() = default;
  Proxy(Subject* sub) : sub_(sub) {}
  ~Proxy() { delete sub_; }
  void Request() {
    std::cout << "Proxy::Request" << std::endl;
    sub_->Request();
  }

 private:
  Subject* sub_;
};
#endif  // PROXY_H