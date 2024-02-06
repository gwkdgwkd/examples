#include <iostream>

// Decorator 装饰模式

// 动机（Motivation）：
// 1.在某些情况下可能会过度地使用继承来扩展对象的功能，由于继承为类型引入的静态特质，
//   使得这种扩展方式缺乏灵活性，并且随着子类的增多（扩展功能的增多），
//   各种子类的组合（扩展功能的组合）会导致更多子类的膨胀；
// 2.如何使对象功能的扩展能够根据需要来动态地实现？
//   同时避免扩展功能的增多带来的子类膨胀问题？
//   从而使得任何功能扩展变化所导致的影响将为最低？

// 定义：
// 动态（组合）地给一个对象增加一些额外的职责，就增加功能而言，
// Decorator模式比生成子类（继承）更为灵活（消除重复代码&减少子类个数）。

// 要点总结：
// 1.通过采用组合而非继承的手法，Decorator模式实现了在运行时动态扩展对象功能的能力，
//   而且可以根据需要扩展多个功能，避免了使用继承带来的灵活性差和多子类衍生问题；
// 2.Decorator类在接口上表现为is-a Component的继承关系，
//   即Decorator类继承了Component类所具有的接口，
//   但在实现上又表现为has-a Component的组合关系，
//   即Decorator类又使用了另外一个Component类；
// 3.Decorator模式的目的并非解决多子类衍生的多继承问题，
//   Decorator模式应用的要点在于解决主体类在多个方向上的扩展功能，是为装饰的含义。

// 业务操作
class Stream {
 public:
  virtual char Read(int number) = 0;
  virtual void Seek(int position) = 0;
  virtual void Write(char data) = 0;

  virtual ~Stream() {}
};

// 主体类
class FileStream : public Stream {
 public:
  char Read(int number) {  // 读文件流
    std::cout << "FileStream::Read" << std::endl;
  }
  void Seek(int position) {  // 定位文件流
    std::cout << "FileStream::Seek" << std::endl;
  }
  void Write(char data) {  // 写文件流
    std::cout << "FileStream::Write" << std::endl;
  }
};

class NetworkStream : public Stream {
 public:
  char Read(int number) {  // 读网络流
    std::cout << "NetworkStream::Read" << std::endl;
  }
  void Seek(int position) {  // 定位网络流
    std::cout << "NetworkStream::Seek" << std::endl;
  }
  void Write(char data) {  // 写网络流
    std::cout << "NetworkStream::Write" << std::endl;
  }
};

class MemoryStream : public Stream {
 public:
  char Read(int number) {  // 读内存流
    std::cout << "MemoryStream::Read" << std::endl;
  }
  void Seek(int position) {  // 定位内存流
    std::cout << "MemoryStream::Seek" << std::endl;
  }
  void Write(char data) {  // 写内存流
    std::cout << "MemoryStream::Write" << std::endl;
  }
};

namespace n1 {  // 使用继承的方式扩展
// 扩展操作1：加密
class CryptoFileStream : public FileStream {
 public:
  virtual char Read(int number) {
    std::cout << "Crypto ";    // 额外的加密操作
    FileStream::Read(number);  // 读文件流
  }
  virtual void Seek(int position) {
    std::cout << "Crypto ";      // 额外的加密操作
    FileStream::Seek(position);  // 定位文件流
  }
  virtual void Write(char data) {
    std::cout << "Crypto ";   // 额外的加密操作
    FileStream::Write(data);  // 写文件流
  }
};

class CryptoNetworkStream : public NetworkStream {
 public:
  virtual char Read(int number) {
    std::cout << "Crypto ";       // 额外的加密操作
    NetworkStream::Read(number);  // 读网络流
  }
  virtual void Seek(int position) {
    std::cout << "Crypto ";         // 额外的加密操作
    NetworkStream::Seek(position);  // 定位网络流
  }
  virtual void Write(char data) {
    std::cout << "Crypto ";      // 额外的加密操作
    NetworkStream::Write(data);  // 写网络流
  }
};

class CryptoMemoryStream : public MemoryStream {
 public:
  virtual char Read(int number) {
    std::cout << "Crypto ";      // 额外的加密操作
    MemoryStream::Read(number);  // 读内存流
  }
  virtual void Seek(int position) {
    std::cout << "Crypto ";        // 额外的加密操作
    MemoryStream::Seek(position);  // 定位内存流
  }
  virtual void Write(char data) {
    std::cout << "Crypto ";     // 额外的加密操作
    MemoryStream::Write(data);  // 写内存流
  }
};

// 扩展操作2：缓存
class BufferedFileStream : public FileStream {
 public:
  virtual char Read(int number) {
    std::cout << "Buffered ";  // 额外的缓存操作
    FileStream::Read(number);  // 读文件流
  }
  virtual void Seek(int position) {
    std::cout << "Buffered ";    // 额外的缓存操作
    FileStream::Seek(position);  // 定位文件流
  }
  virtual void Write(char data) {
    std::cout << "Buffered ";  // 额外的缓存操作
    FileStream::Write(data);   // 写文件流
  }
};

class BufferedNetworkStream : public NetworkStream {
 public:
  virtual char Read(int number) {
    std::cout << "Buffered ";     // 额外的缓存操作
    NetworkStream::Read(number);  // 读文件流
  }
  virtual void Seek(int position) {
    std::cout << "Buffered ";       // 额外的缓存操作
    NetworkStream::Seek(position);  // 定位文件流
  }
  virtual void Write(char data) {
    std::cout << "Buffered ";    // 额外的缓存操作
    NetworkStream::Write(data);  // 写文件流
  }
};

class BufferedMemoryStream : public MemoryStream {
 public:
  virtual char Read(int number) {
    std::cout << "Buffered ";    // 额外的缓存操作
    MemoryStream::Read(number);  // 读文件流
  }
  virtual void Seek(int position) {
    std::cout << "Buffered ";      // 额外的缓存操作
    MemoryStream::Seek(position);  // 定位文件流
  }
  virtual void Write(char data) {
    std::cout << "Buffered ";   // 额外的缓存操作
    MemoryStream::Write(data);  // 写文件流
  }
};

class CryptoBufferedFileStream : public FileStream {
 public:
  virtual char Read(int number) {
    std::cout << "Buffered ";  // 额外的缓存操作
    std::cout << "Crypto ";    // 额外的加密操作
    FileStream::Read(number);  // 读文件流
  }
  virtual void Seek(int position) {
    std::cout << "Buffered ";    // 额外的缓存操作
    std::cout << "Crypto ";      // 额外的加密操作
    FileStream::Seek(position);  // 定位文件流
  }
  virtual void Write(char data) {
    std::cout << "Buffered ";  // 额外的缓存操作
    std::cout << "Crypto ";    // 额外的加密操作
    FileStream::Write(data);   // 写文件流
  }
};

void func1() {
  // 编译时装配：
  CryptoFileStream *fs = new CryptoFileStream();
  fs->Read(5);  // Crypto FileStream::Read
}

void func2() {
  BufferedFileStream *fs = new BufferedFileStream();
  fs->Seek(6);  // Buffered FileStream::Seek
}

void func3() {
  CryptoBufferedFileStream *fs = new CryptoBufferedFileStream();
  fs->Write('c');  // Buffered Crypto FileStream::Write
}
}  // namespace n1

namespace n2 {  // 使用组合的方式扩展
// 扩展操作1：加密
class CryptoStream : public Stream {
  Stream *stream;

 public:
  CryptoStream(Stream *stm) : stream(stm) {}

  virtual char Read(int number) {
    std::cout << "Crypto ";  // 额外的加密操作
    stream->Read(number);    // 读文件流
  }
  virtual void Seek(int position) {
    std::cout << "Crypto ";  // 额外的加密操作
    stream->Seek(position);  // 定位文件流
  }
  virtual void Write(char data) {
    std::cout << "Crypto ";  // 额外的加密操作
    stream->Write(data);     // 写文件流
  }
};

// 扩展操作2：缓存
class BufferedStream : public Stream {
  Stream *stream;

 public:
  BufferedStream(Stream *stm) : stream(stm) {}
  char Read(int number) {
    std::cout << "Buffered ";  // 额外的缓存操作
    stream->Read(number);      // 读文件流
  }
  void Seek(int position) {
    std::cout << "Buffered ";  // 额外的缓存操作
    stream->Seek(position);    // 定位文件流
  }
  void Write(char data) {
    std::cout << "Buffered ";  // 额外的缓存操作
    stream->Write(data);       // 写文件流
  }
};

void func1() {
  // 运行时装配：
  FileStream *s = new FileStream();
  CryptoStream *s1 = new CryptoStream(s);
  BufferedStream *s2 = new BufferedStream(s);
  s->Read(5);      // FileStream::Read
  s1->Seek(6);     // Crypto FileStream::Seek
  s2->Write('c');  // Buffered FileStream::Write
}

void func2() {
  NetworkStream *s = new NetworkStream();
  CryptoStream *s1 = new CryptoStream(s);
  BufferedStream *s2 = new BufferedStream(s);
  s->Read(5);      // NetworkStream::Read
  s1->Seek(6);     // Crypto NetworkStream::Seek
  s2->Write('c');  // Buffered NetworkStream::Write
}

void func3() {
  MemoryStream *s = new MemoryStream();
  CryptoStream *s1 = new CryptoStream(s);
  BufferedStream *s2 = new BufferedStream(s);
  s->Read(5);      // MemoryStream::Read
  s1->Seek(6);     // Crypto MemoryStream::Seek
  s2->Write('c');  // Buffered MemoryStream::Write
}
}  // namespace n2

namespace n3 {  // 使用装饰器模式
// 扩展操作的基类
class DecoratorStream : public Stream {
 protected:
  Stream *stream;
  DecoratorStream(Stream *stm) : stream(stm) {}
};

// 扩展操作1：加密
class CryptoStream : public DecoratorStream {
 public:
  CryptoStream(Stream *stm) : DecoratorStream(stm) {}

  char Read(int number) {
    std::cout << "Crypto ";  // 额外的加密操作
    stream->Read(number);    // 读文件流
  }
  void Seek(int position) {
    std::cout << "Crypto ";  // 额外的加密操作
    stream->Seek(position);  // 定位文件流
  }
  void Write(char data) {
    std::cout << "Crypto ";  // 额外的加密操作
    stream->Write(data);     // 写文件流
  }
};

// 扩展操作2：缓存
class BufferedStream : public DecoratorStream {
 public:
  BufferedStream(Stream *stm) : DecoratorStream(stm) {}

  char Read(int number) {
    std::cout << "Buffered ";  // 额外的缓存操作
    stream->Read(number);      // 读文件流
  }
  void Seek(int position) {
    std::cout << "Buffered ";  // 额外的缓存操作
    stream->Seek(position);    // 定位文件流
  }
  void Write(char data) {
    std::cout << "Buffered ";  // 额外的缓存操作
    stream->Write(data);       // 写文件流
  }
};

void func1() {
  // 运行时装配：
  FileStream *s = new FileStream();
  CryptoStream *s1 = new CryptoStream(s);
  BufferedStream *s2 = new BufferedStream(s);
  s->Read(5);      // FileStream::Read
  s1->Seek(6);     // Crypto FileStream::Seek
  s2->Write('c');  // Buffered FileStream::Write
}

void func2() {
  NetworkStream *s = new NetworkStream();
  CryptoStream *s1 = new CryptoStream(s);
  BufferedStream *s2 = new BufferedStream(s);
  s->Read(5);      // NetworkStream::Read
  s1->Seek(6);     // Crypto NetworkStream::Seek
  s2->Write('c');  // Buffered NetworkStream::Write
}

void func3() {
  MemoryStream *s = new MemoryStream();
  CryptoStream *s1 = new CryptoStream(s);
  BufferedStream *s2 = new BufferedStream(s);
  s->Read(5);      // MemoryStream::Read
  s1->Seek(6);     // Crypto MemoryStream::Seek
  s2->Write('c');  // Buffered MemoryStream::Write
}
}  // namespace n3

int main(int argc, char *argv[]) {
  if (argc < 2) {
    std::cout << argv[0] << " i [0 - 5]" << std::endl;
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
      n1::func3();
      break;
    case 3:
      n2::func1();
      break;
    case 4:
      n2::func2();
      break;
    case 5:
      n2::func3();
      break;
    case 6:
      n3::func1();
      break;
    case 7:
      n3::func2();
      break;
    case 8:
      n3::func3();
      break;
    default:
      std::cout << "invalid type" << std::endl;
      break;
  }

  return 0;
}