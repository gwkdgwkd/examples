// c++支持多继承，但是不建议。除了继承一个基类和多个接口
class MainForm : public Form, public IProgress {
  TextBox* txtFilePath;
  TextBox* txtFileNumber;

  ProgressBar* progressBar;

 public:
  void Button1_Click() {
    string filePath = txtFilePath->getText();
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

class ConsoleNotifier : public IProgress {
 public:
  virtual void DoProgress(float value) { cout << "."; }
};
