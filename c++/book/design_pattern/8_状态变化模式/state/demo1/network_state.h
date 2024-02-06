#ifndef NETWORK_STATE_H
#define NETWORK_STATE_H

class NetworkState {
 public:
  virtual void Open() = 0;
  virtual void Connect() = 0;
  virtual void Close() = 0;
  virtual ~NetworkState() {}

  NetworkState* pNext;
};
#endif  // NETWORK_STATE_H