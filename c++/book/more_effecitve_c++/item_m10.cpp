#include <iostream>
#include <list>
#include <memory>
#include <string>

// 在构造函数中防止资源泄漏

class Image {  // 用于图像数据
 public:
  Image(const std::string& imageDataFileName) {}
};

class AudioClip {  // 用于声音数据
 public:
  AudioClip(const std::string& audioDataFileName) {}
};

class PhoneNumber {};  // 用于存储电话号码

namespace n1 {
class BookEntry {  // 通讯录中的条目
 public:
  BookEntry(const std::string& name, const std::string& address = "",
            const std::string& imageFileName = "",
            const std::string& audioClipFileName = "");
  ~BookEntry();
  // 通过这个函数加入电话号码
  void addPhoneNumber(const PhoneNumber& number);

 private:
  std::string theName;               // 人的姓名
  std::string theAddress;            // 他们的地址
  std::list<PhoneNumber> thePhones;  // 他的电话号码
  Image* theImage;                   // 他们的图像
  AudioClip* theAudioClip;           // 他们的一段声音片段
};
BookEntry::BookEntry(const std::string& name, const std::string& address,
                     const std::string& imageFileName,
                     const std::string& audioClipFileName)
    : theName(name), theAddress(address), theImage(0), theAudioClip(0) {
  // 构造函数把指针theImage和theAudioClip初始化为空，
  // 然后如果其对应的构造函数参数不是空，就让这些指针指向真实的对象。
  if (imageFileName != "") {
    theImage = new Image(imageFileName);
  }
  if (audioClipFileName != "") {
    theAudioClip = new AudioClip(audioClipFileName);
  }
}
BookEntry::~BookEntry() {
  // 析构函数负责删除这些指针，确保BookEntry对象不会发生资源泄漏。
  // 因为C++确保删除空指针是安全的，所以BookEntry的析构函数在删除指针前，
  // 不需要检测这些指针是否指向了某些对象。
  delete theImage;
  delete theAudioClip;
}

// 如果BookEntry的构造函数正在执行中，一个异常被抛出，会发生什么情况呢？
// 一个异常被抛出，可以是因为operator new不能给AudioClip分配足够的内存，
// 也可以因为AudioClip的构造函数自己抛出一个异常，不论什么原因，
// 如果在BookEntry构造函数内抛出异常，这个异常将传递到建立BookEntry对象的地方。
// 现在假设建立theAudioClip对象建立时，一个异常被抛出，
// 而且传递程序控制权到BookEntry构造函数的外面，那么谁来负责删除theImage指向的对象呢？
// 答案显然应该是由BookEntry来做，但是这个想当然的答案是错的。
// ~BookEntry()根本不会被调用，永远不会，C++仅仅能删除被完全构造的对象,
// 只有一个对象的构造函数完全运行完毕，这个对象才被完全地构造。

// 如果试图采取主动手段处理异常情况，即当异常发生时调用delete：
void testBookEntryClass() {
  BookEntry* pb = 0;
  try {
    pb = new BookEntry("Addison-Wesley Publishing Company",
                       "One Jacob Way, Reading, MA 01867");
  } catch (...) {  // 捕获所有异常
    delete pb;     // 删除pb,当抛出异常时
    throw;         // 传递异常给调用者
  }
  delete pb;  // 正常删除pb
}
// 会发现在BookEntry构造函数里为Image分配的内存仍旧被丢失了，
// 这是因为如果new操作没有成功完成，程序不会对pb进行赋值操作。
// 如果BookEntry的构造函数抛出一个异常，pb将是一个空值，
// 所以在catch块中删除它除了让你自己感觉良好以外没有任何作用。
// 用灵巧指针类auto_ptr<BookEntry>代替raw BookEntry*也不会也什么作用，
// 因为new操作成功完成前，也没有对pb进行赋值操作。
// C++拒绝为没有完成构造操作的对象调用析构函数是有一些原因的，而不是故意制造困难。
// 在很多情况下这么做是没有意义的，甚至是有害的。
// 当对象在构造中抛出异常后C++不负责清除对象，所以必须重新设计构造函数，自己清除。
// 经常用的方法是捕获所有的异常，然后执行清除代码，最后再重新抛出异常让它继续转递。
// BookEntry::BookEntry(const std::string& name, const std::string& address,
//                      const std::string& imageFileName,
//                      const std::string& audioClipFileName)
//     : theName(name), theAddress(address), theImage(0), theAudioClip(0) {
//   try {  // 这try block是新加入的
//     if (imageFileName != "") {
//       theImage = new Image(imageFileName);
//     }
//     if (audioClipFileName != "") {
//       theAudioClip = new AudioClip(audioClipFileName);
//     }
//   } catch (...) {     // 捕获所有异常
//     delete theImage;  // 完成必要的清除代码
//     delete theAudioClip;
//     throw;  // 继续传递异常
//   }
// }
// 不用为BookEntry中的非指针数据成员操心，在构造函数调用前数据成员就被自动地初始化。
// 所以如果BookEntry构造函数体开始执行，对象的theName，
// theAddress和thePhones数据成员已经被完全构造好了。
// 这些数据可以被看做是完全构造的对象，所以它们将被自动释放，不用介入操作。
// 当然如果这些对象的构造函数调用可能会抛出异常的函数，
// 那么构造函数必须自己去考虑捕获异常并在继续传递这些异常之前完成必需的清除操作。
}  // namespace n1

namespace n2 {
// BookEntry构造函数的catch块中的语句与在BookEntry的析构函数的语句几乎一样。
// 代码重复是不可容忍的，所以要把通用代码放到函数中，让构造函数与析构函数都调用它。
class BookEntry {
 public:
  BookEntry(const std::string& name, const std::string& address = "",
            const std::string& imageFileName = "",
            const std::string& audioClipFileName = "");
  ~BookEntry();

 private:
  void cleanup();                    // 通用清除代码
  std::string theName;               // 人的姓名
  std::string theAddress;            // 他们的地址
  std::list<PhoneNumber> thePhones;  // 他的电话号码
  Image* theImage;                   // 他们的图像
  AudioClip* theAudioClip;           // 他们的一段声音片段
};
void BookEntry::cleanup() {
  delete theImage;
  delete theAudioClip;
}
BookEntry::BookEntry(const std::string& name, const std::string& address,
                     const std::string& imageFileName,
                     const std::string& audioClipFileName)
    : theName(name), theAddress(address), theImage(0), theAudioClip(0) {
  try {
  } catch (...) {
    cleanup();  // 释放资源
    throw;      // 传递异常
  }
}
BookEntry::~BookEntry() { cleanup(); }
}  // namespace n2

namespace n3 {
// 略微改动一下设计，让theImage和theAudioClip是常量指针类型：
class BookEntry {
 public:
  BookEntry(const std::string& name, const std::string& address = "",
            const std::string& imageFileName = "",
            const std::string& audioClipFileName = "");
  ~BookEntry();

 private:
  void cleanup();                    // 通用清除代码
  std::string theName;               // 人的姓名
  std::string theAddress;            // 他们的地址
  std::list<PhoneNumber> thePhones;  // 他的电话号码
  const Image* theImage;             // 他们的图像
  const AudioClip* theAudioClip;     // 他们的一段声音片段
};
// 必须通过BookEntry构造函数的成员初始化表来初始化这样的指针，
// 因为再也没有其它地方可以给const指针赋值：
BookEntry::BookEntry(const std::string& name, const std::string& address,
                     const std::string& imageFileName,
                     const std::string& audioClipFileName)
    : theName(name),
      theAddress(address),
      theImage(imageFileName != "" ? new Image(imageFileName) : 0),
      theAudioClip(audioClipFileName != "" ? new AudioClip(audioClipFileName)
                                           : 0) {}
// 这样做导致原先一直想避免的问题重新出现：
// 如果theAudioClip初始化时一个异常被抛出，theImage所指的对象不会被释放。
// 而且不能通过在构造函数中增加try和catch语句来解决问题，
// 因为try和catch是语句，而成员初始化表仅允许有表达式。
// 这也是为什么必须在theImage和theAudioClip的初始化中，
// 使用?:以代替if-then-else的原因。
}  // namespace n3

namespace n4 {
// 无论如何，在异常传递之前完成清除工作的唯一的方法就是捕获这些异常，
// 所以如果不能在成员初始化表中放入try和catch语句，把它们移到其它地方。
// 一种可能是在私有成员函数中，
// 用这些函数返回指针指向初始化过的theImage和theAudioClip对象。
class BookEntry {
 public:
  BookEntry(const std::string& name, const std::string& address = "",
            const std::string& imageFileName = "",
            const std::string& audioClipFileName = "");
  ~BookEntry();

 private:
  Image* initImage(const std::string& imageFileName);
  AudioClip* initAudioClip(const std::string& audioClipFileName);
  std::string theName;               // 人的姓名
  std::string theAddress;            // 他们的地址
  std::list<PhoneNumber> thePhones;  // 他的电话号码
  const Image* theImage;             // 他们的图像
  const AudioClip* theAudioClip;     // 他们的一段声音片段
};
BookEntry::BookEntry(const std::string& name, const std::string& address,
                     const std::string& imageFileName,
                     const std::string& audioClipFileName)
    : theName(name),
      theAddress(address),
      theImage(initImage(imageFileName)),
      theAudioClip(initAudioClip(audioClipFileName)) {}
Image* BookEntry3::initImage(const std::string& imageFileName) {
  if (imageFileName != "") {
    return new Image(imageFileName);
  } else {
    return 0;
  }
}
AudioClip* BookEntry3::initAudioClip(const std::string& audioClipFileName) {
  try {
    if (audioClipFileName != "") {
      return new AudioClip(audioClipFileName);
    } else {
      return 0;
    }
  } catch (...) {
    delete theImage;
    throw;
  }
}
// 上面的程序的确不错，也解决了令我们头疼不已的问题。
// 不过也有缺点，在原则上应该属于构造函数的代码却分散在几个函数里，这很难维护。
}  // namespace n4

namespace n5 {
// 更好的解法决方法是采用条款M9的建议，
// 把theImage和theAudioClip指向的对象做为一个资源，被一些局部对象管理。
class BookEntry {
 public:
  BookEntry(const std::string& name, const std::string& address = "",
            const std::string& imageFileName = "",
            const std::string& audioClipFileName = "");
  ~BookEntry();

 private:
  const std::shared_ptr<Image> theImage;
  const std::shared_ptr<AudioClip> theAudioClip;
  std::string theName;               // 人的姓名
  std::string theAddress;            // 他们的地址
  std::list<PhoneNumber> thePhones;  // 他的电话号码
};
// 这样做使得BookEntry的构造函数即使在存在异常的情况下也能做到不泄漏资源，
// 而且能够使用成员初始化表来初始化theImage和theAudioClip：
BookEntry::BookEntry(const std::string& name, const std::string& address,
                     const std::string& imageFileName,
                     const std::string& audioClipFileName)
    : theName(name),
      theAddress(address),
      theImage(imageFileName != "" ? new Image(imageFileName) : 0),
      theAudioClip(audioClipFileName != "" ? new AudioClip(audioClipFileName)
                                           : 0) {}
// 如果在初始化theAudioClip时抛出异常，theImage已经是一个被完全构造的对象，
// 所以它能被自动删除掉，就象theName, theAddress和thePhones一样。
// 而且因为theImage和theAudioClip现在是包含在BookEntry中的对象，
// 当BookEntry被删除时它们能被自动地删除。
// 因此不需要手工删除它们所指向的对象，可以这样简化BookEntry的析构函数：
BookEntry::~BookEntry() {}

// 如果用智能指针对象替代指针成员变量，就可以防止构造函数在存在异常时发生资源泄漏，
// 也不用在析构函数中释放资源，还能象使用非const指针一样使用const指针，给其赋值。

// 在对象构造中，处理各种抛出异常的可能，是一个棘手的问题，
// 但是auto_ptr或者类似于auto_ptr的类能化繁为简。
// 它不仅把令人不好理解的代码隐藏起来，而且使得程序在面对异常时也能保持正常运行。
}  // namespace n5

int main() { return 0; }