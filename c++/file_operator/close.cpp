#include <fstream>
#include <iostream>

// 文件流对象还可以主动关闭先前打开的文件，即调用close()成员方法。
// 调用open()方法打开文件，是文件流对象和文件之间建立关联的过程。
// 那么，调用close()方法关闭已打开的文件，就可以理解为是切断文件流对象和文件之间的关联。
// 注意，close()方法的功能仅是切断文件流与文件之间的关联，
// 该文件流并会被销毁，其后续还可用于关联其它的文件。
// close()方法的用法很简单，其语法格式如下：
// void close();
// 该方法既不需要传递任何参数，也没有返回值。
// 即便不调用close()方法，也能成功向文件中写入字符串。
// 这是因为，当文件流对象的生命周期结束时，会自行调用其析构函数，该函数内部在销毁对象之前，
// 会先调用close()方法切断它与任何文件的关联，最后才销毁它。
// 强烈建议使用open()方法打开的文件，一定要手动调用close()方法关闭，这样可以避免程序发生一些奇葩的错误！

// 4种流状态也同样适用于文件流。
// 当文件流对象未关联任何文件时，调用close()方法会失败，
// 其会为文件流设置failbit状态标志，该标志可以被fail()成员方法捕获。

// 既然文件流对象自行销毁时会隐式调用close()方法，是不是就不用显式调用close()方法了呢？
// 当然不是。在实际进行文件操作的过程中，对于打开的文件，
// 要及时调用close()方法将其关闭，否则很可能会导致读写文件失败。
// 对于已经打开的文件，如果不及时关闭，一旦程序出现异常，则很可能会导致之前读写文件的所有操作失效。

// 在很多实际场景中，即便已经对文件执行了写操作，但后续还可能会执行其他的写操作。
// 对于这种情况，我们可能并不想频繁地打开/关闭文件，
// 可以使用flush()方法及时刷新输出流缓冲区，也能起到防止写入文件失败的作用。
// 总之，C++中使用open()打开的文件，在读写操作执行完毕后，应及时调用close()方法关闭文件，
// 或者对文件执行写操作后及时调用flush()方法刷新输出流缓冲区。

int main() {
  const char *url = "http://c.biancheng.net/cplus/";
  std::ofstream outFile("url.txt", std::ios::out);
  outFile.write(url, 30);
  outFile.close();

  std::ofstream outFile1;
  outFile1.close();
  if (outFile1.fail()) {
    std::cout << "文件操作过程发生了错误！" << std::endl;
  }

  std::ofstream destFile("out.txt", std::ios::out);
  if (!destFile) {
    std::cout << "文件打开失败" << std::endl;
    return 0;
  }
  destFile << url;
  destFile.flush();  // 在异常前加入flush，那么可以内容可以写入到文件中
  // 之所以写入文件失败，是因为<<写入运算符会先将url字符串写入到输出流缓冲区中，
  // 待缓冲区满或者关闭文件时，数据才会由缓冲区写入到文件中。
  // 但直到程序崩溃，close()方法也没有得到执行，且destFile对象也没有正常销毁，
  // 所以url字符串一直存储在缓冲区中，没有写入到文件中。
  throw "Exception";  // 导致文件写入操作失败
  destFile.close();

  return 0;
}