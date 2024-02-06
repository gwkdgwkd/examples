#ifndef HANDLER3_H
#define HANDLER3_H

#include "chain_handler.h"

class Handler3 : public ChainHandler {
 protected:
  bool canHandleRequest(const Reqest &req) override {
    return req.getReqType() == RequestType::REQ_HANDLER3;
  }
  void processRequest(const Reqest &req) override {
    std::cout << "Handler3 is handle reqest: " << req.getDescription()
              << std::endl;
  }
};
#endif  // HANDLER3_H