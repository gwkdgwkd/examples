class MainForm : public Form {
  SplitterFactory* factory;  // 工厂

 public:
  // factory从构造函数传进来
  MainForm(SplitterFactory* factory) { this->factory = factory; }

  void Button1_Click() {
    // 不在依赖具体类（实现），都是依赖接口
    ISplitter* splitter = factory->CreateSplitter();  // 多态new

    splitter->split();
  }
};
