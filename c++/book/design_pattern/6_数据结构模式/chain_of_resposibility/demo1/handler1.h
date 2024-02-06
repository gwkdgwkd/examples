#ifndef HANDLER1_H
#define HANDLER1_H

#include "chain_handler.h"

class Handler1 : public ChainHandler {
 protected:
  bool canHandleRequest(const Reqest &req) override {
    return req.getReqType() == RequestType::REQ_HANDLER1;
  }
  void processRequest(const Reqest &req) override {
    std::cout << "Handler1 is handle reqest: " << req.getDescription()
              << std::endl;
  }
};
#endif  // HANDLER1_H