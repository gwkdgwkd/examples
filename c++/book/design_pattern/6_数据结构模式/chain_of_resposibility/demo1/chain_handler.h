#ifndef CHAIN_HANDLER_H
#define CHAIN_HANDLER_H

#include <iostream>

#include "request.h"

class ChainHandler {
 public:
  ChainHandler() { nextChain = nullptr; }
  void setNextChain(ChainHandler *next) { nextChain = next; }
  void handle(const Reqest &req) {
    if (canHandleRequest(req))
      processRequest(req);
    else
      sendReqestToNextHandler(req);
  }

 protected:
  virtual bool canHandleRequest(const Reqest &req) = 0;
  virtual void processRequest(const Reqest &req) = 0;

 private:
  void sendReqestToNextHandler(const Reqest &req) {
    if (nextChain != nullptr) {
      nextChain->handle(req);
    }
  }

  ChainHandler *nextChain;
};
#endif  // CHAIN_HANDLER_H