#ifndef CONNECT_STATE_H
#define CONNECT_STATE_H

#include "network_state.h"

class ConnectState : public NetworkState {
  static NetworkState* m_instance;

 public:
  static NetworkState* getInstance();
  void Open();
  void Connect();
  void Close();
};
#endif  // CONNECT_STATE_H