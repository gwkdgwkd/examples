class FileSplitter {
  string m_filePath;
  int m_fileNumber;
  // ProgressBar是实现细节，实现很容易改变。有可能更改进度显示方式，所以不要依赖实现，要依赖接口
  ProgressBar* m_progressBar;  // 违背了依赖（编译时依赖）倒置原则。

 public:
  FileSplitter(const string& filePath, int fileNumber, ProgressBar* progressBar)
      : m_filePath(filePath),
        m_fileNumber(fileNumber),
        m_progressBar(progressBar) {}

  void split() {
    // 1.读取大文件

    // 2.分批次向小文件中写入
    for (int i = 0; i < m_fileNumber; i++) {
      // ...
      float progressValue = m_fileNumber;
      progressValue = (i + 1) / progressValue;
      m_progressBar->setValue(progressValue);
    }
  }
};