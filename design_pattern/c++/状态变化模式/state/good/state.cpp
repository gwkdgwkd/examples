class NetworkState {
 public:
  NetworkState* pNext;
  virtual void Operation1() = 0;
  virtual void Operation2() = 0;
  virtual void Operation3() = 0;

  virtual ~NetworkState() {}
};

class OpenState : public NetworkState {
  static NetworkState* m_instance;

 public:
  static NetworkState* getInstance() {
    if (m_instance == nullptr) {
      m_instance = new OpenState();
    }
    return m_instance;
  }

  void OpenState::Operation1() {
    // **********
    pNext = CloseState::getInstance();
  }

  void OpenState::Operation2() {
    // ..........
    pNext = ConnectState::getInstance();
  }

  void OpenState::Operation3() {
    // $$$$$$$$$$
    pNext = OpenState::getInstance();
  }
};

class CloseState : public NetworkState {};

// 如果添加一个状态，需要增加一个状态类，不用修改NetworkProcessor，符合开发封闭原则
class NetworkProcessor {
  NetworkState* pState;

 public:
  NetworkProcessor(NetworkState* pState) { this->pState = pState; }

  void Operation1() {
    // ...
    pState->Operation1();
    pState = pState->pNext;
    // ...
  }
  void Operation2() {
    // ...
    pState->Operation2();
    pState = pState->pNext;
    // ...
  }
  void Operation3() {
    // ...
    pState->Operation3();
    pState = pState->pNext;
    // ...
  }
};
