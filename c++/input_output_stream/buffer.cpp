#include <iostream>

// 每个输出流都管理一个缓冲区，用来保存程序读写的数据。
// 有了缓冲机制，操作系统就可以将程序的多个输出操作组合成单一的系统级写操作。
// 由于设备的写操作可能很耗时，允许操作系统将多个输出操作组合为单一的设备写操作可以带来很大的性能提升。
// 导致缓冲刷新（数据真正写到输出设备或文件）的原因有很多：
//  程序正常结束，作为main()函数的return操作的一部分，缓冲刷新被执行。
//  缓冲区满时，需要刷新缓冲，而后新的数据才能继续写入缓冲区。
//  可以使用操纵符如endl 来显式刷新缓冲区。
//  在每个输出操作之后，我们可以用操作符unitbuf设置流的内部状态，来清空缓冲区。
//  默认情况下，对cerr是设置unitbuf的，因此写到cerr的内容都是立即刷新的。
//  一个输出流可能被关联到另一个流。
//  当读写被关联的流时，关联到的流的缓冲区会被刷新。
//  默认情况下，cin和cerr都关联到cout。因此，读cin或写cerr都会导致cout的缓冲区被刷新。

// 操作符endl，它完成换行并刷新缓冲区的工作。IO库中还有两个类似的操纵符：flush和ends。
//  flush刷新缓冲区，但不输出任何额外的字符；
//  ends向缓冲区插入一个空字符，然后刷新缓冲区。
// 值得一提得是，cout所属ostream类中还提供有flush()成员方法，
// 它和flush操纵符的功能完全一样，仅在使用方法上（cout.flush()）有区别。
void func1() {
  std::cout << "hi!" << std::flush;  // 输出hi然后刷新缓冲区，不附加任何额外字符
  std::cout << "hi!" << std::ends;  // 输出hi和一个空字符，然后刷新缓冲区
  std::cout << "hi!" << std::endl;  // 输出hi和一个换行，然后刷新缓冲区
  // hi!hi!hi!
}

// 如果想在每次输出操作后都刷新缓冲区，我们可以使用unitbuf操作符，它告诉流在接下来的每次写操作之后都进行一次flush操作。
// 而nounitbuf操作符则重置流，使其恢复使用正常的系统管理的缓冲区刷新机制。
// 如果程序异常终止，输出缓冲区是不会被刷新的。
// 当一个程序崩溃后，它所输出的数据很可能停留在输出缓冲区中等待打印。
// 当调试一个已经崩溃的程序时，需要确认那些你认为已经输出的数据确实已经刷新了。
// 否则，可能将大量时间浪费在追踪代码为什么没有执行上，而实际上代码已经执行了，
// 只是程序崩溃后缓冲区没有被刷新，输出数据被挂起没有打印而已。
void func2() {
  std::cout << std::unitbuf;  // 所有输出操作后都会立即刷新缓冲区
  // 任何输出都立即刷新，无缓冲
  std::cout << std::nounitbuf;  // 回到正常的缓冲方式
}

// 当一个输入流被关联到一个输出流时，任何试图从输入流读取数据的操作都会先刷新关联的输出流。
// 标准库将cout和cin关联在一起，因此下面语句：
// cin >> ival;导致cout的缓冲区被刷新。
// 交互式系统通常应该关联输入流和输出流。
// 这意味着所有输出，包括用户提示信息，都会在读操作之前被打印出来。
// tie()函数可以用来绑定输出流，它有两个重载的版本：
// ostream* tie () const;  // 返回指向绑定的输出流的指针。
// ostream* tie (ostream* os);  // 将os指向的输出流绑定的该对象上，并返回上一个绑定的输出流指针。
// 第一个版本不带参数，返冋指向出流的指针。
// 如果本对象当前关联到一个输出流，则返回的就是指向这个流的指针，如果对象未关联到流，则返回空指针。
// tie()的第二个版本接受一个指向ostream的指针，将自己关联到此ostream，即x.tie(&o)将流x关联到输出流o。
// 每个流同时最多关联到一个流， 但多个流可以同时关联到同一个ostream。
void func3() {
  // 既可以将一个istream对象关联到另一个ostream，也可以将一个ostream关联到另一个ostream：
  std::cin.tie(&std::cout);  // 仅仅是用来展示，标准库已经将cin和cout关联在一起
  // old_tie指向当前关联到cin的流（如果有的话）
  std::ostream* old_tie = std::cin.tie(nullptr);  // cin不再与其他流关联
  // 将cin与cerr关联，这不是一个好主意，因为cin应该关联到cout
  std::cin.tie(&std::cerr);  // 读取cin会刷新cerr而不是cout
  std::cin.tie(old_tie);     // 重建cin和cout间的正常关联
}

int main(int argc, char* argv[]) {
  if (argc < 2) {
    std::cout << argv[0] << " i [0 - 2]" << std::endl;
    return 0;
  }
  int type = argv[1][0] - '0';
  switch (type) {
    case 0:
      func1();
      break;
    case 1:
      func2();
      break;
    case 2:
      func3();
      break;
    default:
      std::cout << "invalid type" << std::endl;
      break;
  }

  return 0;
}
