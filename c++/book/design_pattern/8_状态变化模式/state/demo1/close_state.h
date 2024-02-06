#ifndef CLOSE_STATE_H
#define CLOSE_STATE_H

#include "network_state.h"

class CloseState : public NetworkState {
  static NetworkState* m_instance;

 public:
  static NetworkState* getInstance();
  void Open();
  void Connect();
  void Close();
};
#endif  // CLOSE_STATE_H