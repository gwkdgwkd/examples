#include "explicit_instantiate.h"

// 模板的实例化是在调用函数或者创建对象时由编译器自动完成的，不需要程序员引导，因此称为隐式实例化。
// 相对应的，我们也可以通过代码明确地告诉编译器需要针对哪个类型进行实例化，这称为显式实例化。
// 编译器在实例化的过程中需要知道模板的所有细节：对于函数模板，也就是函数定义；对于类模板，需要同时知道类声明和类定义。
// 必须将显式实例化的代码放在包含了模板定义的源文件中，而不是仅仅包含了模板声明的头文件中。
// 显式实例化的一个好处是，可以将模板的声明和定义（实现）分散到不同的文件中了。
// 显式实例化也包括声明和定义，定义要放在模板定义（实现）所在的源文件，声明要放在模板声明所在的头文件（当然也可以不写）。
// 函数模板和类模板的实例化语法是类似的，做一下总结：
//  extern template declaration;  // 实例化声明
//  template declaration;  // 实例化定义
//  对于函数模板来说，declaration就是一个函数原型；对于类模板来说，declaration就是一个类声明。
// 显式实例化的缺陷:
//  C++支持显式实例化的目的是为「模块化编程」提供一种解决方案，这种方案虽然有效，但是也有明显的缺陷：程序员必须要在模板的定义文件（实现文件）中对所有使用到的
//  类型进行实例化。这就意味着，每次更改了模板使用文件（调用函数模板的文件，或者通过类模板创建对象的文件），也要相应地更改模板定义文件，以增加对新类型的实例化，
//  或者删除无用类型的实例化。
//  一个模板可能会在多个文件中使用到，要保持这些文件的同步更新是非常困难的。而对于库的开发者来说，他不能提前假设用户会使用哪些类型，所以根本就无法使用显式实例
//  化，只能将模板的声明和定义（实现）全部放到头文件中；C++标准库几乎都是用模板来实现的，这些模板的代码也都位于头文件中。
//  总起来说，如果我们开发的模板只有我们自己使用，那也可以勉强使用显式实例化；如果希望让其他人使用（例如库、组件等），那只能将模板的声明和定义都放到头文件中了。

// 函数模板显示实例化声明（也可以不写）
extern template void Swap(double &a, double &b);
extern template void Swap(int &a, int &b);

// 类模板的显式实例化和函数模板类似。
// 另外需要注意的是，显式实例化一个类模板时，会一次性实例化该类的所有成员，包括成员变量和成员函数。
// 有了类模板的显式实例化，就可以将类模板的声明和定义分散到不同的文件中了

// 类模板显式实例化声明（也可以不写）
extern template class Point<char *, char *>;
extern template class Point<int, int>;

int main() {
  int n1 = 10, n2 = 20;
  Swap(n1, n2);
  cout << n1 << " " << n2 << endl;  // 20 10
  double f1 = 23.8, f2 = 92.6;
  Swap(f1, f2);
  cout << f1 << " " << f2 << endl;  // 92.6 23.8

  Point<int, int> p1(10, 20);
  p1.setX(40);
  p1.setY(50);
  cout << "x=" << p1.getX() << ", y=" << p1.getY() << endl;  // x=40, y=50
  Point<char *, char *> p2("180度", "210度");  // x=180度, y=210度
  p2.display();

  return 0;
}