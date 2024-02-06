#include <iostream>

#include "close_state.h"
#include "connect_state.h"

NetworkState* ConnectState::m_instance = nullptr;
NetworkState* ConnectState::getInstance() {
  if (m_instance == nullptr) {
    m_instance = new ConnectState();
  }
  return m_instance;
}

void ConnectState::Open() {
  pNext = ConnectState::getInstance();
  std::cout << "close first,please" << std::endl;
}
void ConnectState::Connect() {
  std::cout << "it has been connected" << std::endl;
}
void ConnectState::Close() {
  pNext = CloseState::getInstance();
  std::cout << "connect -> close" << std::endl;
}