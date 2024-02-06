#ifndef REQUEST_H
#define REQUEST_H
#include <string>

enum class RequestType { REQ_HANDLER1, REQ_HANDLER2, REQ_HANDLER3 };

class Reqest {
  std::string description;
  RequestType reqType;

 public:
  Reqest(const std::string &desc, RequestType type)
      : description(desc), reqType(type) {}
  RequestType getReqType() const { return reqType; }
  const std::string &getDescription() const { return description; }
};
#endif  // REQUEST_H