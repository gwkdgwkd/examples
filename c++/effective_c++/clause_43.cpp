#include <iostream>
#include <string>

using namespace std;

// 学习处理模板化基类内的名称

class CompanyA {
 public:
  void sendCleartext(const string& msg) {
    cout << "CompanyA::sendCleartext" << endl;
  };
  void sendEncrypted(const string& msg) {
    cout << "CompanyA::sendEncrypted" << endl;
  };
};
class CompanyB {
 public:
  void sendCleartext(const string& msg) {
    cout << "CompanyB::sendCleartext" << endl;
  };
  void sendEncrypted(const string& msg) {
    cout << "CompanyB::sendEncrypted" << endl;
  };
};
class MsgInfo {};
template <typename Company>
class MsgSender {
 public:
  void sendClear(const MsgInfo& info) {
    string msg;
    Company c;
    c.sendCleartext(msg);
  }
  void sendSecret(const MsgInfo& info) {
    string msg;
    Company c;
    c.sendEncrypted(msg);
  }
};

// 上面的做法行得通，但有时想要在每次发送消息时打印日志。
// derived class可以轻易加上这样的功能：
template <typename Company>
class LoggingMsgSender : public MsgSender<Company> {
 public:
  // 与base class内的名称不同，是个好设计：
  // 1.条款33（避免遮掩继承而得的名称）
  // 3.条款36（避免重新定义继承的non-virtual函数）
  void sendClearMsg(const MsgInfo& info) {
    cout << "befor sendClear:" << endl;
    // 调用base class函数，这段代码无法通过编译，
    // 编译器抱怨sendClear不存在：
    // sendClear(info);
    // 我们的眼睛可以看到sendClear在base class内，
    // 编译器却看不到它们，为什么?
    // 问题在于当编译器遭遇LoggingMsgSender定义时，
    // 并不知道它继承什么样的class。
    // 继承的是MsgSender<Company>，但其中Company是参数，
    // 不到具现化时无法确切知道它是什么。
    // 无法知道Company是什么，
    // 就无法知道class MsgSender<Company>中是否有sendClear函数。
    cout << "after sendClear:" << endl;
  }
};

class CompanyZ {
 public:
  void sendEncrypted(const string& msg) {
    cout << "CompanyZ::sendEncrypted" << endl;
  };
};
// 一般性的MsgSender对CompanyZ并不合适，
// 因为那个template提供了一个sendClear函数，
// CompanyZ没有sendCleartext函数。
// 针对CompanyZ产生一个MsgSender特化版：
template <>  // 不是template也不是标准class，而是特化版的template
class MsgSender<CompanyZ> {  // 全特化的MsgSender，删掉了sendClear
 public:
  void sendSecret(const MsgInfo& info) {
    string msg;
    CompanyZ z;
    z.sendEncrypted(msg);
  }
};
// 再看LoggingMsgSender，
// 如果base class被指定为MsgSender<CompanyZ>时，
// 代码不合法，因为那个class并未提供sendClear函数。
// 这就是C++为什么拒绝这个调用的原因：
// 它指定base class template有可能被特化，
// 而特化版本可能不提供和一般性template相同的接口。
// 因此编译器往往拒绝：
// 在template base class（模板化基类）内寻找继承而来的名称。
// 从某种意义而言，当从Object Oriented C++跨进Template C++时，
// 继承就不像以前那般畅行无阻了。

// 必须有某种办法令C++不进入template base class观察的行为失效，
// 有三个办法：
// 1.在base class函数调用动作之前加上this->。
template <typename Company>
class LoggingMsgSender1 : public MsgSender<Company> {
 public:
  void sendClearMsg(const MsgInfo& info) {
    cout << "befor sendClear:" << endl;
    this->sendClear(info);
    cout << "after sendClear:" << endl;
  }
};
// 2.使用using声明，
//   将被掩盖的base class名称带入一个derived class作用域内。
template <typename Company>
class LoggingMsgSender2 : public MsgSender<Company> {
 public:
  // 使用using告诉编译器，进入base class作用域内查找：
  using MsgSender<Company>::sendClear;
  void sendClearMsg(const MsgInfo& info) {
    cout << "befor sendClear:" << endl;
    sendClear(info);
    cout << "after sendClear:" << endl;
  }
};
// 3.明白指出被调用的函数位于base class内。
template <typename Company>
class LoggingMsgSender3 : public MsgSender<Company> {
 public:
  void sendClearMsg(const MsgInfo& info) {
    cout << "befor sendClear:" << endl;
    MsgSender<Company>::sendClear(info);
    cout << "after sendClear:" << endl;
  }
};
// 但这往往是最不让人满意的一个解法，因为如果被调用的是virtual函数，
// 上述的明确资格修饰会关闭virtual绑定行为。

// 面对指向base class members的无线引用，
// 编译器的诊断时间可能发生在早期，
// 解析deriverd class template的定义式时，
// 也可能发生在晚期，当那些template被特定的template实参具现化时。
// C++的政策是宁愿较早诊断。
// 这就是为什么当base class从template中被具现化时，
// 它假设对那些base class的内容毫无所悉的缘故。

// 请记住：
// 可在derived class template内，
// 通过this->指向base class template内的成员名称，
// 或由一个明白写出的base class资格修饰符完成。

int main() {
  MsgInfo m;
  MsgSender<CompanyA> a;
  a.sendSecret(m);  // CompanyA::sendEncrypted
  MsgSender<CompanyB> b;
  b.sendClear(m);  // CompanyB::sendCleartext

  MsgSender<CompanyZ> z;
  z.sendSecret(m);  // CompanyZ::sendEncrypted

  LoggingMsgSender1<CompanyA> lms1;
  lms1.sendClearMsg(m);
  // befor sendClear:
  // CompanyA::sendCleartext
  // after sendClear:

  LoggingMsgSender2<CompanyB> lms2;
  lms2.sendClearMsg(m);
  // befor sendClear:
  // CompanyB::sendCleartext
  // after sendClear:

  LoggingMsgSender3<CompanyA> lms3;
  lms3.sendClearMsg(m);
  // befor sendClear:
  // CompanyA::sendCleartext
  // after sendClear:

  return 0;
}