#ifndef OPEN_STATE_H
#define OPEN_STATE_H

#include "network_state.h"

class OpenState : public NetworkState {
  static NetworkState* m_instance;

 public:
  static NetworkState* getInstance();
  void Open();
  void Connect();
  void Close();
};
#endif  // OPEN_STATE_H