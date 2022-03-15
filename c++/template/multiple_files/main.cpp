#include "multiple_files.h"

// 在将函数应用于多文件编程时，通常是将函数定义放在源文件中，将函数声明放在头文件中，使用函数时引入头文件即可。
// 编译是针对单个源文件的，只要有函数声明，编译器就能知道函数调用是否正确；
// 而将函数调用和函数定义对应起来的过程，可以延迟到链接时期。
// 正是有了链接器的存在，函数声明和函数定义的分离才得以实现。
// 将类应用于多文件编程也是类似的道理，可以将类的声明和类的实现分别放到头文件和源文件中。
// 类的声明已经包含了所有成员变量的定义和所有成员函数的声明（也可以是inline形式的定义），
// 这样就知道如何创建对象了，也知道如何调用成员函数了，只是还不能将函数调用与函数实现对应起来，
// 但是这又有什么关系呢，反正链接器可以帮助我们完成这项工作。
// 总起来说，不管是函数还是类，声明和定义（实现）的分离其实是一回事，都是将函数定义放到其他文件中，
// 最终要解决的问题也只有一个，就是把函数调用和函数定义对应起来，而保证完成这项工作的就是链接器。

// 基于传统的编程思维，往往也会将模板（函数模板和类模板）的声明和定义分散到不同的文件中，以期达到模块化编程的目的。
// 但事实证明这种做法是不对的，惯用的做法是将模板的声明和定义都放到头文件中。
// 模板并不是真正的函数或类，它仅仅是用来生成函数或类的一张“图纸”，在这个生成过程中有三点需要明确：
// 1.模板的实例化是按需进行的，用到哪个类型就生成针对哪个类型的函数或类，不会提前生成过多的代码；
// 2.模板的实例化是由编译器完成的，而不是由链接器完成的；
// 3.在实例化过程中需要知道模板的所有细节，包含声明和定义。

// 不能将模板的声明和定义分散到多个文件中的根本原因是：
// 模板的实例化是由编译器完成的，而不是由链接器完成的，这可能会导致在链接期间找不到对应的实例。

#define EXPLICITINSTANTIATE
#ifdef EXPLICITINSTANTIATE
// 模板的实例化是在调用函数或者创建对象时由编译器自动完成的，不需要程序员引导，因此称为隐式实例化。
// 相对应的，我们也可以通过代码明确地告诉编译器需要针对哪个类型进行实例化，这称为显式实例化。
// 编译器在实例化的过程中需要知道模板的所有细节：
// 对于函数模板，也就是函数定义；对于类模板，需要同时知道类声明和类定义。
// 必须将显式实例化的代码放在包含了模板定义的源文件中，而不是仅仅包含了模板声明的头文件中。
// 显式实例化的一个好处是，可以将模板的声明和定义分散到不同的文件中了。
// 显式实例化也包括声明和定义，定义要放在模板定义所在的源文件，声明要放在模板声明所在的头文件（也可以不写）。

// 函数模板和类模板的实例化语法是类似的，做一下总结：
// extern template declaration;  // 实例化声明
// template declaration;  // 实例化定义
// 对于函数模板来说，declaration就是一个函数原型；对于类模板来说，declaration就是一个类声明。
// 另外需要注意的是，显式实例化一个类模板时，会一次性实例化该类的所有成员，包括成员变量和成员函数。
// 有了类模板的显式实例化，就可以将类模板的声明和定义分散到不同的文件中了。

// 显式实例化的缺陷:
// C++支持显式实例化的目的是为模块化编程提供一种解决方案，这种方案虽然有效，但是也有明显的缺陷：
// 1.程序员必须要在模板的定义文件中对所有使用到的类型进行实例化。
//   这就意味着，每次更改了模板使用文件（调用函数模板的文件，或者通过类模板创建对象的文件），
//   也要相应地更改模板定义文件，以增加对新类型的实例化，或者删除无用类型的实例化。
// 2.一个模板可能会在多个文件中使用到，要保持这些文件的同步更新是非常困难的。
//   而对于库的开发者来说，他不能提前假设用户会使用哪些类型，所以根本就无法使用显式实例化，
//   只能将模板的声明和定义全部放到头文件中；
//   C++标准库几乎都是用模板来实现的，这些模板的代码也都位于头文件中。
// 总起来说，如果开发的模板只有我们自己使用，那也可以勉强使用显式实例化；
// 如果希望让其他人使用（例如库、组件等），那只能将模板的声明和定义都放到头文件中了。

// 函数模板显示实例化声明（也可以不写）:
extern template void Swap(double &a, double &b);
extern template void Swap(int &a, int &b);

// 类模板显式实例化声明（也可以不写）:
extern template class Point<char *, char *>;
extern template class Point<int, int>;
#else
// multiple_files_main.cpp:(.text+0x61)：对‘void Swap<double>(double&, double&)’未定义的引用
// 主函数main()中共调用了int和double两个版本的Swap()函数，
// 为什么针对int的版本能够找到定义，而针对double的版本就找不到呢？
// 对double的版本为什么找不到定义:
// 1.当编译器编译main.cpp时，发现使用到了double版本的Swap()函数，于是尝试生成一个double版本的实例，
//   但是由于只有声明没有定义，所以生成失败。
// 2.不过这个时候编译器不会报错，而是对该函数的调用做一个记录，希望等到链接程序时在其他目标文件中找到该函数的定义。
//   但是遗憾的是，multiple_files.cpp中没有调用double版本的Swap()函数，编译器不会生成double版本的实例，
//   所以链接器最终也找不到double版本的函数定义，只能抛出一个链接错误，让程序员修改代码。
// 针对int的版本为什么能够找到定义:
// 1.bubble_sort()函数用来对数组元素进行排序，在排序过程中需要频繁的交换两个元素的值，所以调用了Swap()函数，
//   这样做的结果是：编译生成的multipe_files.obj中会有一个int版本的Swap()函数定义。
// 2.编译器在编译main.cpp时虽然找不到int版本的实例，
//   但是等到链接程序时，链接器在func.obj中找到了，所以针对int版本的调用就不会出错。

// main.cpp:(.text+0x119)：对‘Point<char*, char*>::display() const’未定义的引用
// 类模板声明位于point.h中，它包含了所有成员变量的定义以及构造函数、get函数、set函数的定义，
// 这些信息足够创建出一个完整的对象了，并且可以通过对象调用get函数和set函数，
// 所以调用display()函数前不会报错，调用该函数会报错。
// 该函数的定义位于point.cpp文件中，并且multipe_files.cpp中也没有生成对应的实例，所以会在链接期间抛出错误。

// 修复上面两个项目的方法也很简单，就是将multipe_files.cpp中的模板定义部分分别合并到头文件中。
#endif

int main() {
  int n1 = 10, n2 = 20;
  Swap(n1, n2);
  std::cout << n1 << "," << n2 << std::endl;  // 20,10
  double f1 = 23.8, f2 = 92.6;
  Swap(f1, f2);
  std::cout << f1 << "," << f2 << std::endl;  // 92.6,23.8

  Point<int, int> p1(10, 20);
  p1.setX(40);
  p1.setY(50);
  std::cout << p1.getX() << "," << p1.getY() << std::endl;  // 40,50

  Point<char *, char *> p2("hello", "world");  // x=hello,y=world
  p2.display();

  return 0;
}

// g++ -std=c++11 multiple_files.cpp main.cpp
