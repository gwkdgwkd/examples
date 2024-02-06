#include <iostream>

#include "close_state.h"
#include "open_state.h"

NetworkState* CloseState::m_instance = nullptr;
NetworkState* CloseState::getInstance() {
  if (m_instance == nullptr) {
    m_instance = new CloseState();
  }
  return m_instance;
}

void CloseState::Open() {
  pNext = OpenState::getInstance();
  std::cout << "close -> open" << std::endl;
}
void CloseState::Connect() {
  pNext = CloseState::getInstance();
  std::cout << "open first,please" << std::endl;
}
void CloseState::Close() {
  pNext = CloseState::getInstance();
  std::cout << "it has been closed" << std::endl;
}
