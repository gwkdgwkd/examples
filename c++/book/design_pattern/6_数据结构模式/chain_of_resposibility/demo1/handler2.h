#ifndef HANDLER2_H
#define HANDLER2_H

#include "chain_handler.h"

class Handler2 : public ChainHandler {
 protected:
  bool canHandleRequest(const Reqest &req) override {
    return req.getReqType() == RequestType::REQ_HANDLER2;
  }
  void processRequest(const Reqest &req) override {
    std::cout << "Handler2 is handle reqest: " << req.getDescription()
              << std::endl;
  }
};
#endif  // HANDLER2_H