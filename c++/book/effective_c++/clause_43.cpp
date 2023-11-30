#include <iostream>
#include <string>

// 学习处理模板化基类内的名称

class CompanyA {
 public:
  void sendCleartext(const std::string& msg) {
    std::cout << "CompanyA::sendCleartext" << std::endl;
  };
  void sendEncrypted(const std::string& msg) {
    std::cout << "CompanyA::sendEncrypted" << std::endl;
  };
};
class CompanyB {
 public:
  void sendCleartext(const std::string& msg) {
    std::cout << "CompanyB::sendCleartext" << std::endl;
  };
  void sendEncrypted(const std::string& msg) {
    std::cout << "CompanyB::sendEncrypted" << std::endl;
  };
};
class MsgInfo {};
template <typename Company>
class MsgSender {
 public:
  void sendClear(const MsgInfo& info) {
    std::string msg;
    Company c;
    c.sendCleartext(msg);
  }
  void sendSecret(const MsgInfo& info) {
    std::string msg;
    Company c;
    c.sendEncrypted(msg);
  }
};

class CompanyZ {
 public:
  void sendEncrypted(const std::string& msg) {
    std::cout << "CompanyZ::sendEncrypted" << std::endl;
  };
};
// 一般的MsgSender对CompanyZ并不合适，因为template提供了sendClear函数，
// 但CompanyZ没有sendCleartext函数，针对CompanyZ产生一个MsgSender特化版：
template <>  // 不是template也不是标准class，而是特化版的template
class MsgSender<CompanyZ> {  // 全特化的MsgSender，删掉了sendClear
 public:
  void sendSecret(const MsgInfo& info) {
    std::string msg;
    CompanyZ z;
    z.sendEncrypted(msg);
  }
};
namespace n1 {
void func1() {
  MsgInfo m;
  MsgSender<CompanyA> a;
  a.sendSecret(m);  // CompanyA::sendEncrypted
  MsgSender<CompanyB> b;
  b.sendClear(m);  // CompanyB::sendCleartext
  MsgSender<CompanyZ> z;
  z.sendSecret(m);  // CompanyZ::sendEncrypted
}

// 上面的做法行得通，但有时想要在每次发送消息时打印日志，
// derived class可以轻易加上这样的功能：
template <typename Company>
class LoggingMsgSender : public MsgSender<Company> {
 public:
  // 与base class内的名称不同，是个好设计：
  // 1.条款33（避免遮掩继承而得的名称）；
  // 2.条款36（避免重新定义继承的non-virtual函数）。
  void sendClearMsg(const MsgInfo& info) {
    std::cout << "befor sendClear:" << std::endl;
    // 调用base class函数，这段代码无法通过编译，编译器抱怨sendClear不存在：
    // sendClear(info);
    // 眼睛可以看到sendClear在base class内，编译器却看不到它们，为什么?
    // 问题在于LoggingMsgSender定义时，编译器并不知道它继承什么样的class。
    // 继承的是MsgSender<Company>，但其中Company不到具现化时无法知道是什么。
    // 不知道Company是什么，就不知道知道MsgSender<Company>中是否有sendClear。
    std::cout << "after sendClear:" << std::endl;
  }
};
void func2() {
  MsgInfo m;
  LoggingMsgSender<CompanyA> lms;
  lms.sendClearMsg(m);
}
}  // namespace n1

// 有三个办法令C++不进入template base class观察的行为失效：
namespace n2 {
// 1.在base class函数调用动作之前加上this->：
template <typename Company>
class LoggingMsgSender : public MsgSender<Company> {
 public:
  void sendClearMsg(const MsgInfo& info) {
    std::cout << "befor sendClear:" << std::endl;
    this->sendClear(info);
    std::cout << "after sendClear:" << std::endl;
  }
};

void func() {
  MsgInfo m;
  LoggingMsgSender<CompanyA> lmsa;
  lmsa.sendClearMsg(m);
  // befor sendClear:
  // CompanyA::sendCleartext
  // after sendClear:

  LoggingMsgSender<CompanyZ> lmsz;
  // lmsz.sendClearMsg(m);
  // 代码不合法，因为那个class并未提供sendClear函数。
  // 这就是C++为什么拒绝这个调用的原因：
  // 它指定base class template有可能被特化，
  // 而特化版本可能不提供和一般性template相同的接口。
  // 因此编译器往往拒绝：
  // 在template base class（模板化基类）内寻找继承而来的名称。
  // 从某种意义而言，当从Object Oriented C++跨进Template C++时，
  // 继承就不像以前那般畅行无阻了。
}
}  // namespace n2

namespace n3 {
// 2.使用using声明将被掩盖的base名称带到derived作用域内：
template <typename Company>
class LoggingMsgSender : public MsgSender<Company> {
 public:
  // 使用using告诉编译器，进入base class作用域内查找：
  using MsgSender<Company>::sendClear;
  void sendClearMsg(const MsgInfo& info) {
    std::cout << "befor sendClear:" << std::endl;
    sendClear(info);
    std::cout << "after sendClear:" << std::endl;
  }
};
void func() {
  MsgInfo m;
  LoggingMsgSender<CompanyA> lmsa;
  lmsa.sendClearMsg(m);
  // befor sendClear:
  // CompanyA::sendCleartext
  // after sendClear:

  // LoggingMsgSender<CompanyZ> lmsz;
  // lmsz.sendClearMsg(m);
}
}  // namespace n3

namespace n4 {
// 3.明白指出被调用的函数位于base class内：
template <typename Company>
class LoggingMsgSender : public MsgSender<Company> {
 public:
  void sendClearMsg(const MsgInfo& info) {
    std::cout << "befor sendClear:" << std::endl;
    MsgSender<Company>::sendClear(info);
    std::cout << "after sendClear:" << std::endl;
  }
};
// 但这往往是最不让人满意的一个解法，因为如果被调用的是virtual函数，
// 上述的明确资格修饰会关闭virtual绑定行为。
void func() {
  MsgInfo m;
  LoggingMsgSender<CompanyB> lmsb;
  lmsb.sendClearMsg(m);
  // befor sendClear:
  // CompanyB::sendCleartext
  // after sendClear:

  LoggingMsgSender<CompanyZ> lmsz;
  // lmsz.sendClearMsg(m);
}
}  // namespace n4

// 面对指向base class members的引用，编译器的诊断时间可能发生在早期，
// 解析deriverd class template的定义式时，也可能发生在晚期，
// 当那些template被特定的template实参具现化时。
// C++的政策是宁愿较早诊断，这就是为什么当base从template中被具现化时，
// 它假设对那些base的内容毫无所悉的缘故。

// 请记住：
// 1.可在derived class template内，
//   通过this->指向base class template内的成员名称，
//   或由一个明白写出的base class资格修饰符完成。

int main(int argc, char* argv[]) {
  if (argc < 2) {
    std::cout << argv[0] << " i [0 - 2]" << std::endl;
    return 0;
  }
  int type = argv[1][0] - '0';
  switch (type) {
    case 0:
      n1::func1();
      break;
    case 1:
      n1::func2();
      break;
    case 2:
      n2::func();
      break;
    case 3:
      n3::func();
      break;
    case 4:
      n4::func();
      break;
    default:
      std::cout << "invalid type" << std::endl;
      break;
  }

  return 0;
}