#include <iostream>
#include <new>
#include <string>

using namespace std;

// 理解各种不同含义的new和delete

// 人们有时好像喜欢故意使C++语言的术语难以理解。
// 比如说new操作符（new operator）和new操作（operator new）的区别。

// new操作符就象sizeof一样是语言内置的，你不能改变它的含义，它的功能总是一样的。
// 它要完成的功能分成两部分:
// 1.分配足够的内存以便容纳所需类型的对象;
// 2.它调用构造函数初始化内存中的对象。
// new操作符总是做这两件事情，你不能以任何方式改变它的行为。

// 你所能改变的是如何为对象分配内存。
// new操作符调用一个函数来完成必需的内存分配，你能够重写或重载这个函数来改变它的行为。
// new操作符为分配内存所调用函数的名字是operator new。
// 函数operator new通常这样声明：
// void *operator new(size_t size);
// 返回值类型是void*，因为这个函数返回一个未经处理（raw）的指针，未初始化的内存。
// 参数size_t确定分配多少内存。
// 你能增加额外的参数重载函数operator new，但是第一个参数类型必须是size_t。
// 就象malloc一样，operator new的职责只是分配内存，它对构造函数一无所知。

// 在一个已存在的对象上调用构造函数是没有意义的，因为构造函数用来初始化对象，
// 而一个对象仅仅能在给它初值时被初始化一次。
// 但是有时你有一些已经被分配但是尚未处理的(raw)内存，你需要在这些内存中构造一个对象。
// 你可以使用一个特殊的operator new，它被称为placement new。
class Widget {
 public:
  Widget(int widgetSize) { cout << "Widget" << endl; }
};
Widget *constructWidgetInBuffer(void *buffer, int widgetSize) {
  // 这个函数返回一个指针，指向一个Widget对象，对象在转递给函数的buffer里分配。
  return new (buffer) Widget(widgetSize);
  // 它是new操作符的一个用法，需要使用一个额外的变量（buffer），
  // 当new操作符隐含调用operator new函数时，把这个变量传递给它。
  // 被调用的operator new函数除了待有强制的参数size_t外，
  // 还必须接受void*指针参数，指向构造对象占用的内存空间。
  // 这个operator new就是placement new。
}
// placement new看上去象这样：
// void *operator new(size_t, void *location) {
//   // 没有用的（但是强制的）参数size_t没有名字，以防止编译器发出警告说它没有被使用
//   return location;
// }

// operator new的目的是为对象分配内存然后返回指向该内存的指针。
// 在使用placement new的情况下，调用者已经获得了指向内存的指针，因为调用者知道对象应该放在哪里。
// placement new必须做的就是返回转递给它的指针。
// 为了使用placement new，你必须使用语句#include <new>

// 看看new操作符（new operator）与operator new的关系:
// 1.你想在堆上建立一个对象，应该用new操作符。
//   它既分配内存又为对象调用构造函数。
// 2.如果你仅仅想分配内存，就应该调用operator new函数；
//   它不会调用构造函数。
// 3.如果你想定制自己的在堆对象被建立时的内存分配过程，你应该写你自己的operator new函数，
//   然后使用new操作符，new操作符会调用你定制的operator new。
// 4.如果你想在一块已经获得指针的内存里建立一个对象，应该用placement new。

// 函数operator delete与delete操作符的关系与operator new与new操作符的关系一样。
// operator delete用来释放内存，它被这样声明：
void operator delete(void *memoryToBeDeallocated);

// 如果你用placement new在内存中建立对象，你应该避免在该内存中用delete操作符。
// 因为delete操作符调用operator delete来释放内存，
// 但是包含对象的内存最初不是被operator new分配的，placement new只是返回转递给它的指针。
void *mallocShared(size_t size) {}
void freeShared(void *memory) {}

// 数组分配函数，叫做operator new[]（经常被称为array new）。
// 它与operator new一样能被重载。这就允许你控制数组的内存分配，就象你能控制单个对象内存分配一样。
// 对于数组，在数组里的每一个对象的构造函数都必须被调用。
// 同样当delete操作符用于数组时，它为每个数组元素调用析构函数，然后调用operator delete来释放内存。
// 就象你能替换或重载operator delete一样，你也替换或重载operator delete[]。

// new和delete操作符是内置的，其行为不受你的控制，凡是它们调用的内存分配和释放函数则可以控制。
// 当你想定制new和delete操作符的行为时，请记住你不能真的做到这一点。
// 你只能改变它们为完成它们的功能所采取的方法，而它们所完成的功能则被语言固定下来，不能改变。

struct A {
  A() { std::cout << "A" << std::endl; }
  ~A() { std::cout << "~A" << std::endl; }
};

int main() {
  string *ps = new string("Memory Management");  // new操作符

  // 一般不会直接调用operator new，但是一旦这么做，就可以象调用其它函数一样调用它：
  void *rawMemory = operator new(sizeof(string));
  // 操作符operator new将返回一个指针，指向一块足够容纳一个string类型对象的内存。

  // 你的编译器会生成代码来析构对象并释放对象占有的内存。
  delete ps;  // 使用delete操作符

  // 如果你只想处理未被初始化的内存，你应该绕过new和delete操作符，
  // 而调用operator new获得内存和operator delete释放内存给系统：
  A *buffer = (A *)operator new(50 * sizeof(A));  // 分配内存，没有调用构造函数
  operator delete(buffer);  // 释放内存,没有调用析构函数
  // 这与在C中调用malloc和free等同

  void *sharedMemory = mallocShared(sizeof(Widget));
  Widget *pw = constructWidgetInBuffer(sharedMemory, 10);  // 使用placement new
  delete pw;  // 结果不确定! 共享内存来自mallocShared, 而不是operator new
  pw->~Widget();  // 正确。析构pw指向的Widget，但是没有释放包含Widget的内存
  freeShared(pw);  // 正确。释放pw指向的共享内存但是没有调用析构函数

  return 0;
}