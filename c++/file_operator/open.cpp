#include <fstream>
#include <iostream>

// 在对文件进行读写操作之前，先要打开文件。打开文件有以下两个目的：
// 1.通过指定文件名，建立起文件和文件流对象的关联，以后要对文件进行操作时，
//   就可以通过与之关联的流对象来进行。
// 2.指明文件的使用方式。使用方式有只读、只写、既读又写、在文件末尾添加数据、
//   以文本方式使用、以二进制方式使用等多种。
// 打开文件可以通过以下两种方式进行：
// 1.调用流对象的open成员函数打开文件。
// 2.定义文件流对象时，通过构造函数打开文件。

// ifstream类有一个open成员函数，其他两个文件流类也有同样的open成员函数：
// void open(const char* szFileName, int mode)
// 第一个参数是指向文件名的指针，第二个参数是文件的打开模式标记。
// 文件的打开模式标记代表了文件的使用方式，这些标记可以单独使用，也可以组合使用:
//  ios::in 	                ifstream fstream 	打开文件用于读取数据。如果文件不存在，则打开出错。
//  ios::out 	                ofstream fstream 	打开文件用于写入数据。如果文件不存在，则新建该文件；
//                                              如果文件原来就存在，则打开时清除原来的内容。
//  ios::app 	                ofstream fstream 	打开文件，用于在其尾部添加数据。如果文件不存在，则新建该文件。
//  ios::ate 	                ifstream 	        打开一个已有的文件，并将文件读指针指向文件末尾。
//                                              如果文件不存在，则打开出错。
//  ios::trunc                ofstream 	        打开文件时会清空内部存储的所有数据，单独使用时与ios::out 相同。
//  ios::binary      ifstream ofstream fstream 	以二进制方式打开文件。若不指定此模式，则以文本模式打开。
//  ios::in|ios::out 	        fstream 	        打开已存在的文件，既可读取其内容，也可向其写入数据。
//                                              文件刚打开时，原有内容保持不变。如果文件不存在，则打开出错。
//  ios::in|ios::out 	        ofstream 	        打开已存在的文件，可以向其写入数据。文件刚打开时，原有内容保持不变。
//                                              如果文件不存在，则打开出错。
//  ios::in|ios::out|ios::trunc fstream         打开文件，既可读取其内容，也可向其写入数据。
//                                              如果文件存在，则清除原来的内容；如果文件不存在，则新建该文件。
// ios::binary可以和其他模式标记组合使用，例如：
//  ios::in|ios::binary表示用二进制模式，以读取的方式打开文件。
//  ios::out|ios::binary表示用二进制模式，以写入的方式打开文件。
// 文本方式与二进制方式打开文件的区别其实非常微小。
// 一般来说，如果处理的是文本文件，那么用文本方式打开会方便一些。
// 但其实任何文件都可以以二进制方式打开来读写。
// 在流对象上执行open成员函数，给出文件名和打开模式，就可以打开文件。
// 判断文件打开是否成功，可以看“对象名”这个表达式的值是否为true，如果为true，则表示文件打开成功。

// 定义流对象时，在构造函数中给出文件名和打开模式也可以打开文件。以ifstream类为例，它有如下构造函数：
// ifstream::ifstream (const char* szFileName, int mode = ios::in, int);
// 第一个参数是指向文件名的指针；
// 第二个参数是打开文件的模式标记，默认值为ios::in;第三个参数是整型的，也有默认值，一般极少使用。
// 当不再对打开的文件进行任何操作时，应及时调用close()成员方法关闭文件。

// 第二个参数是一个字符串，用来表示文件打开方式，即如果使用ios::binary，则表示以二进制方式打开文件；
// 反之，则以文本文件的方式打开文件。
// 文本文件和二进制文件的区别:
//  根据以往的经验，文本文件通常用来保存肉眼可见的字符，比如.txt文件、.c文件、.dat文件等，
//  用文本编辑器打开这些文件，能够顺利看懂文件的内容。
//  二进制文件通常用来保存视频、图片、程序等不可阅读的内容，用文本编辑器打开这些文件，会看到一堆乱码，根本看不懂。
//  但是从物理上讲，二进制文件和字符文件并没有什么区别，它们都是以二进制的形式保存在磁盘上的数据。
//  之所以能看懂文本文件的内容，是因为文本文件中采用的是ASCII、UTF-8、GBK等字符编码，
//  文本编辑器可以识别出这些编码格式，并将编码值转换成字符展示出来。
//  而二进制文件使用的是mp4、gif、exe等特殊编码格式，文本编辑器并不认识这些编码格式，
//  只能按照字符编码格式胡乱解析，所以就成了一堆乱七八糟的字符，有的甚至都没见过。
//  如果我们新建一个mp4文件，给它写入一串字符，然后再用文本编辑器打开，你一样可以读得懂。
// 总的来说，不同类型的文件有不同的编码格式，必须使用对应的程序（软件）才能正确解析，否则就是一堆乱码，或者无法使用。
// 两种打开方式的区别：
//  文本方式和二进制方式并没有本质上的区别，只是对于换行符的处理不同。
//  在UNIX/Linux平台中，用文本方式或二进制方式打开文件没有任何区别，因为文本文件以\n（ASCII码为0x0a）作为换行符号。
//  但在Windows平台上，文本文件以连在一起的\r\n作为换行符号。
//  如果以文本方式打开文件，当读取文件时，程序会将文件中所有的\r\n转换成一个字符\n。
//  也就是说，如果文本文件中有连续的两个字符是\r\n，则程序会丢弃前面的\r，只读入\n。
//  同样当写入文件时，程序会将\n转换成\r\n写入。
//  也就是说，如果要写入的内容中有字符\n，则在写入该字符前，程序会自动先写入一个\r。
//  因此在Windows平台上，如果用文本方式打开二进制文件进行读写，读写的内容就可能和文件的内容有出入。
// 总的来说，Linux平台使用哪种打开方式都行；
// Windows平台上最好用"ios::in|ios::out"等打开文本文件，用"ios::binary"打开二进制文件。
// 但无论哪种平台，用二进制方式打开文件总是最保险的。

int main() {
  std::ifstream inFile;
  inFile.open("test.txt", std::ios::in);
  if (inFile)  // 条件成立，则说明文件打开成功
    inFile.close();
  else
    std::cout << "test.txt doesn't exist" << std::endl;
  std::ofstream oFile;
  oFile.open("test1.txt", std::ios::out);
  if (!oFile)  // 条件成立，则说明文件打开出错
    std::cout << "error 1" << std::endl;
  else
    oFile.close();
  oFile.open("test2.txt", std::ios::out | std::ios::in);
  if (oFile)  // 条件成立，则说明文件打开成功
    oFile.close();
  else
    std::cout << "error 2" << std::endl;
  std::fstream ioFile;
  ioFile.open("test3.txt", std::ios::out | std::ios::in | std::ios::trunc);
  if (!ioFile)
    std::cout << "error 3" << std::endl;
  else
    ioFile.close();
  // error 2

  std::ifstream inFile1("test.txt", std::ios::in);
  if (inFile)
    inFile1.close();
  else
    std::cout << "test.txt doesn't exist" << std::endl;
  std::ofstream oFile1("test1.txt", std::ios::out);
  if (!oFile1)
    std::cout << "error 1" << std::endl;
  else
    oFile1.close();
  std::fstream oFile2("test2.txt", std::ios::out | std::ios::in);
  if (!oFile2)
    std::cout << "error 2" << std::endl;
  else
    oFile2.close();
  // error 2

  return 0;
}