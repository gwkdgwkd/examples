class MainForm : public Form {
  TextBox* txtFilePath;
  TextBox* txtFileNumber;
  ProgressBar* progressBar;

 public:
  void Button1_Click() {
    // 不能光考虑左边，右边有要考虑
    ISplitter* splitter = new BinarySplitter();  // 依赖具体类，应该依赖接口

    splitter->split();
  }
};
