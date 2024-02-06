#include <iostream>

#include "close_state.h"
#include "connect_state.h"
#include "open_state.h"

// State 状态

// 动机（Motivation）：
// 1.在软件构建过程中，某些对象的状态如果改变，其行为也会随之而发生变化，
//   比如文档处于只读状态，其支持的行为和读写状态支持的行为可能完全不同；
// 2.如何在运行时根据对象的状态来透明地更改对象的行为？
//   而不会为对象操作和状态转化之间引入紧耦合？

// 定义：
// 允许一个对象在其内部状态改变时改变它的行为。从而使对象看起来似乎修改了其行为。

// 要点总结：
// 1.State模式将所有与一个特定状态相关的行为都放入一个State的子类对象中，
//   这样在切换状态时，切换相应的对象，但同时维持State的接口，
//   这样实现了具体操作与状态转换之间的解耦；
// 2.为不同的状态引入不同的对象使得状态转换变得更加明确，
//   而且可以保证不会出现状态不一致的情况，因为转换是原子性的，
//   即要么彻底转换过来，要么不转换；
// 3.如果State对象没有实例，那么各个上下文可以共享同一个State对象，从而节省对象开销。

namespace n1 {
enum NetworkState {
  Network_Open,
  Network_Close,
  Network_Connect,
  Network_Wait,  // 新增状态
};

// 如果增加一个状态，需要修改NetworkProcessor，违反了开发封闭原则
class NetworkProcessor {
 public:
  void Open() {
    if (state == Network_Open) {
      std::cout << "it has been opened" << std::endl;
    } else if (state == Network_Close) {
      state = Network_Open;
      std::cout << "open" << std::endl;
    } else if (state == Network_Connect) {
      std::cout << "close first,please" << std::endl;
    }
  }
  void Connect() {
    if (state == Network_Open) {
      state = Network_Connect;
      std::cout << "connect" << std::endl;
    } else if (state == Network_Close) {
      std::cout << "open first,please" << std::endl;
    } else if (state == Network_Connect) {
      std::cout << "it has been connected" << std::endl;
    }
  }
  void Close() {
    if (state == Network_Open) {
      state = Network_Close;
      std::cout << "close" << std::endl;
    } else if (state == Network_Close) {
      std::cout << "it has been closed" << std::endl;
    } else if (state == Network_Connect) {
      state = Network_Close;
      std::cout << "close" << std::endl;
    }
  }

 private:
  NetworkState state = Network_Close;
};

void func() {
  NetworkProcessor np;

  np.Connect();  // open first,please
  np.Close();    // it has been closed
  np.Open();     // open

  np.Open();     // it has been opened
  np.Connect();  // connect
  np.Close();    // close

  np.Open();   // open
  np.Close();  // close
}
}  // namespace n1

namespace n2 {
// 如果添加一个状态，需要增加一个状态类，不用修改NetworkProcessor，符合开发封闭原则
class NetworkProcessor {
  NetworkState* pState;

 public:
  NetworkProcessor(NetworkState* pState) { this->pState = pState; }

  void Open() {
    pState->Open();
    pState = pState->pNext;
  }
  void Connect() {
    pState->Connect();
    pState = pState->pNext;
  }
  void Close() {
    pState->Close();
    pState = pState->pNext;
  }
};

void func1() {
  NetworkProcessor np(OpenState::getInstance());
  np.Open();     // it has been opened
  np.Connect();  // open -> connect
  np.Close();    // connect -> close

  np.Open();   // close -> open
  np.Close();  // open -> close
}

void func2() {
  NetworkProcessor np(ConnectState::getInstance());
  np.Open();     // close first,please
  np.Connect();  // it has been connected
  np.Close();    // connect -> close
}

void func3() {
  NetworkProcessor np(CloseState::getInstance());
  np.Connect();  // open first,please
  np.Close();    // it has been closed
  np.Open();     // close -> open
}
}  // namespace n2

int main(int argc, char* argv[]) {
  if (argc < 2) {
    std::cout << argv[0] << " i [0 - 3]" << std::endl;
    return 0;
  }
  int type = argv[1][0] - '0';

  NetworkState* ns;
  switch (type) {
    case 0:
      n1::func();
      break;
    case 1:
      n2::func1();
      break;
    case 2:
      n2::func2();
      break;
    case 3:
      n2::func3();
      break;
    default:
      std::cout << "invalid type" << std::endl;
      break;
  }
}

// g++ client.cpp close_state.cpp open_state.cpp connect_state.cpp