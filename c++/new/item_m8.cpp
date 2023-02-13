#include <iostream>
#include <new>

// 理解各种不同含义的new和delete

// 人们有时好像喜欢故意使C++语言的术语难以理解，
// 比如说new操作符(new operator)和new操作(operator new)的区别。

struct A {
  A() { std::cout << "A" << std::endl; }
  ~A() { std::cout << "~A" << std::endl; }
};

namespace n1 {
// new操作符就象sizeof一样是语言内置的，不能改变它的含义，
// 它的功能总是一样的，分成两部分：
// 1.分配足够的内存以便容纳所需类型的对象；
// 2.它调用构造函数初始化内存中的对象。
// new操作符总是做这两件事情，不能以任何方式改变它的行为。
void func() {
  A *pa = new A;  // new操作符
  delete pa;      // 使用delete操作符

  // A
  // ~A
}
}  // namespace n1

namespace n2 {
// 能改变的是如何为对象分配内存，new操作符调用一个函数来完成内存分配，
// 这个函数就是operator new，可以重写或重载这个函数来改变它的行为。
// 函数operator new通常这样声明：
// void *operator new(size_t size);
// 返回值类型是void*，因为这个函数返回一个未经处理的指针，未初始化的内存。
// 参数size_t确定分配多少内存。
// 能增加额外的参数重载函数operator new，但是第一个参数类型必须是size_t。
// 就象malloc一样，operator new的职责只是分配内存，它对构造函数一无所知。
void func() {
  // 一般不会直接调用operator new，但可以象调用其它函数一样调用它：
  void *rawMemory = operator new(50 * sizeof(A));  // 分配内存，没有调用构造函数
  // 操作符operator new将返回一个指针，指向一块足够大的内存。

  operator delete(rawMemory);  // 释放内存,没有调用析构函数

  // 这些操作与在C中调用malloc和free等同。
}
}  // namespace n2

namespace n3 {
// 在一个已存在的对象上调用构造函数是没有意义的，
// 因为构造函数用来初始化对象，而一个对象仅仅能在给它初值时被初始化一次。
// 但有时有一些已经被分配但是尚未处理的内存，需要在这些内存中构造一个对象。

// 被调用的operator new函数除了带有强制的参数size_t外，
// 还必须接受void*指针参数，指向构造对象占用的内存空间，
// 这个operator new就是placement new，看上去象这样：
// void *operator new(size_t, void *location) {
//   // 没有用的（但是强制的）参数size_t没有名字，以防止编译器警告。
//   return location;
// }

// operator new的目的是为对象分配内存然后返回指向该内存的指针。
// 在使用placement new的情况下，调用者已经获得了指向内存的指针，
// 因为调用者知道对象应该放在哪里。
// placement new必须做的就是返回转递给它的指针。
// 为了使用placement new，必须使用语句#include <new>

void func1() {
  void *buffer = malloc(sizeof(int) * 5);
  A *pa = new (buffer) A;  // 对象在参数buffer里分配

  // 如果用placement new在内存中建立对象，应该避免调用delete操作符。
  // 因为delete操作符调用operator delete来释放内存，
  // 但是包含对象的内存最初不是被operator new分配的，
  // placement new只是返回转递给它的指针。
  delete pa;  // 结果不确定

  // A
  // ~A

  free(buffer);  // free(): double free detected in tcache 2
}
void func2() {
  void *buffer = malloc(sizeof(int) * 5);  // 分配内存
  A *pa = new (buffer) A;                  // 使用placement new创建对象
  pa->~A();      // 析构pa指向的A，但是没有释放包含A的内存
  free(buffer);  // 释放pa指向的共享内存但是没有调用析构函数

  // A
  // ~A
}
}  // namespace n3

// 看看new操作符（new operator）与operator new的关系:
// 1.想在堆上创建对象，应该用new操作符，它既分配内存又为对象调用构造函数；
// 2.如果仅仅想分配内存，就应该调用operator new函数，它不会调用构造函数；
// 3.如果想定制在堆对象被建立时的内存分配过程，应该重写operator new函数，
//   然后使用new操作符，new操作符会调用重写的operator new；
// 4.如果想在一块已经获得指针的内存里建立一个对象，应该用placement new。

// 函数operator delete与delete操作符的关系，
// 与operator new与new操作符的关系一样。
// operator delete用来释放内存，它被这样声明：
// void operator delete(void *memoryToBeDeallocated);

// 数组分配函数，叫做operator new[]（经常被称为array new），
// 它与operator new一样能被重载。
// 这就允许控制数组的内存分配，就象能控制单个对象内存分配一样。
// 对于数组，在数组里的每一个对象的构造函数都必须被调用。
// 同样当delete操作符用于数组时，它为每个数组元素调用析构函数，
// 然后调用operator delete来释放内存。
// 和operator delete一样，也替换或重载operator delete[]。

// new和delete操作符是内置的，其行为不受控制，
// 只有它们调用的内存分配和释放函数可以控制。
// 当想定制new和delete操作符的行为时，不能真的做到这一点。
// 只能改变它们为完成它们的功能所采取的方法，而它们完成的功能则不能改变。

int main(int argc, char *argv[]) {
  if (argc < 2) {
    std::cout << argv[0] << " i [0 - 3]" << std::endl;
    return 0;
  }
  int type = argv[1][0] - '0';
  switch (type) {
    case 0:
      n1::func();
      break;
    case 1:
      n2::func();
      break;
    case 2:
      n3::func1();
      break;
    case 3:
      n3::func2();
      break;
    default:
      std::cout << "invalid type" << std::endl;
      break;
  }

  return 0;
}