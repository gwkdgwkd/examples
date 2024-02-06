#include <iostream>
#include <list>
#include <string>

// Observer 观察者模式

// 动机（Motivation）：
// 1.在软件构建过程中，需要为某些对象建立一种通知依赖关系，
//   一个对象（目标对象）的状态发生改变，所有的依赖对象（观察者对象）都将得到通知，
//   如果这样的依赖关系过于紧密，将使软件不能很好地抵御变化；
// 2.使用面向对象技术，可以将这种依赖关系弱化，
//   并形成一种稳定的依赖关系，从而实现软件体系结构的松耦合。

// 定义：
// 定义对象间的一种一对多（变化）的依赖关系，
// 以便当一个对象(Subject)的状态发生改变时，所有依赖于它的对象都得到通知并自动更新。

// 要点总结：
// 1.使用面向对象的抽象，Observer模式可以独立地改变目标与观察者，
//   从而使二者之间的依赖关系达致松耦合；
// 2.目标发送通知时，无需指定观察者，通知（可以携带通知信息作为参数）会自动传播；
// 3.观察者自己决定是否需要订阅通知，目标对象对此一无所知；
// 4.Observer模式是基于事件的UI框架中非常常用的设计模式，也是MVC模式的一个重要组成部分。

struct Form {};
struct ProgressBar {
  void setValue(float) {}
};
struct TextBox {
  std::string getText() { return "5"; }
};

namespace n1 {
// POP

class FileSplitter {
  std::string m_filePath;
  int m_fileNumber;

  // ProgressBar是实现细节，实现很容易改变，
  // 有可能更改进度显示方式，所以不要依赖实现，要依赖接口：
  ProgressBar* m_progressBar;  // 违背了依赖（编译时依赖）倒置原则

 public:
  FileSplitter(const std::string& filePath, int fileNumber,
               ProgressBar* progressBar)
      : m_filePath(filePath),
        m_fileNumber(fileNumber),
        m_progressBar(progressBar) {}

  void split() {
    // 1.读取大文件

    // 2.分批次向小文件中写入
    for (int i = 0; i < m_fileNumber; i++) {
      float progressValue = m_fileNumber;
      progressValue = (i + 1) / progressValue;
      m_progressBar->setValue(progressValue);
    }
  }
};

class MainForm : public Form {
  TextBox* txtFilePath;
  TextBox* txtFileNumber;
  ProgressBar* progressBar;

 public:
  MainForm() {
    txtFilePath = new TextBox();
    txtFileNumber = new TextBox();
    progressBar = new ProgressBar();
  }
  ~MainForm() {
    delete txtFilePath;
    delete txtFileNumber;
    delete progressBar;
  }
  void Button1_Click() {
    std::string filePath = txtFilePath->getText();
    int number = atoi(txtFileNumber->getText().c_str());
    FileSplitter splitter(filePath, number, progressBar);
    splitter.split();
  }
};

void func() {
  MainForm mf;
  mf.Button1_Click();
}
}  // namespace n1

namespace n2 {
// OOP

class IProgress {
 public:
  virtual void DoProgress(float value) = 0;
  virtual ~IProgress() {}
};

class ConsoleNotifier : public IProgress {
 public:
  virtual void DoProgress(float value) { std::cout << "."; }
};

class FileSplitter {
  std::string m_filePath;
  int m_fileNumber;

  // 符合依赖倒置原则，依赖通知接口，而不是某个具体的通知控件：
  std::list<IProgress*> m_iprogressList;

 public:
  FileSplitter(const std::string& filePath, int fileNumber)
      : m_filePath(filePath), m_fileNumber(fileNumber) {}

  void split() {
    // 1.读取大文件

    // 2.分批次向小文件中写入
    for (int i = 0; i < m_fileNumber; i++) {
      float progressValue = m_fileNumber;
      progressValue = (i + 1) / progressValue;
      onProgress(progressValue);
    }
  }

  void addIProgress(IProgress* iprogress) {
    m_iprogressList.push_back(iprogress);
  }

  void removeIProgress(IProgress* iprogress) {
    m_iprogressList.remove(iprogress);
  }

 protected:
  virtual void onProgress(float value) {  // 写成虚函数，基类可以修改计算方式
    std::list<IProgress*>::iterator itor = m_iprogressList.begin();
    while (itor != m_iprogressList.end()) {
      (*itor)->DoProgress(value);  // 更新进度条
      itor++;
    }
  }
};

// c++支持多继承，但是不建议，除非继承一个基类和多个接口：
class MainForm : public Form, public IProgress {
  TextBox* txtFilePath;
  TextBox* txtFileNumber;
  ProgressBar* progressBar;

 public:
  MainForm() {
    txtFilePath = new TextBox();
    txtFileNumber = new TextBox();
    progressBar = new ProgressBar();
  }
  ~MainForm() {
    delete txtFilePath;
    delete txtFileNumber;
    delete progressBar;
  }

  void Button1_Click() {
    std::string filePath = txtFilePath->getText();
    int number = atoi(txtFileNumber->getText().c_str());

    ConsoleNotifier cn;

    FileSplitter splitter(filePath, number);

    splitter.addIProgress(this);  // 第一个观察者
    splitter.addIProgress(&cn);   // 第二个观察者
    splitter.split();
    splitter.removeIProgress(this);
  }

  virtual void DoProgress(float value) { progressBar->setValue(value); }
};

void func() {
  MainForm mf;
  mf.Button1_Click();
}
}  // namespace n2

int main(int argc, char* argv[]) {
  if (argc < 2) {
    std::cout << argv[0] << " i [0 - 1]" << std::endl;
    return 0;
  }
  int type = argv[1][0] - '0';

  switch (type) {
    case 0:
      n1::func();
      break;
    case 1:
      n2::func();
      break;
    default:
      std::cout << "invalid type" << std::endl;
      break;
  }

  return 0;
}