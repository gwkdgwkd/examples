#include <iostream>

#include "close_state.h"
#include "connect_state.h"
#include "open_state.h"

NetworkState* OpenState::m_instance = nullptr;
NetworkState* OpenState::getInstance() {
  if (m_instance == nullptr) {
    m_instance = new OpenState();
  }
  return m_instance;
}

void OpenState::Open() {
  std::cout << "it has been opened" << std::endl;
  pNext = OpenState::getInstance();
}
void OpenState::Connect() {
  pNext = ConnectState::getInstance();
  std::cout << "open -> connect" << std::endl;
}
void OpenState::Close() {
  pNext = CloseState::getInstance();
  std::cout << "open -> close" << std::endl;
}