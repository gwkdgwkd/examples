#include <iostream>
#include <string>
#include <vector>

// decltype是C++11新增的一个关键字，它和auto的功能一样，都用来在编译时期进行自动类型推导。
// decltype是“declare type”的缩写，译为“声明类型”。
// 既然已经有了auto关键字，为什么还需要decltype关键字呢？因为auto并不适用于所有的自动类型推导场景，在某些特殊情况下auto用起来非常不方便，甚至压根无法使用，
// 所以decltype关键字也被引入到C++11中。

class Student {
public:
  static int total;
  std::string name;
  int age;
  float scores;
};

class Base {
public:
  Base(){};
  int x;
};

int Student::total = 0;

void test() {}
int &func_int_r(int, char);           //返回值为 int&
int &&func_int_rr(void);              //返回值为 int&&
int func_int(double);                 //返回值为 int
const int &fun_cint_r(int, int, int); //返回值为 const int&
const int &&func_cint_rr(void);       //返回值为 const int&&

template <typename T> class Base1 {
public:
  void func(T &container) { m_it = container.begin(); }

private:
  // 要想解决这个问题，在之前的C++98/03版本下只能想办法把const类型的容器用模板特化单独处理，增加了不少工作量，看起来也非常晦涩。
  typename T::iterator m_it; // 注意这里
};
template <typename T> class Base2 {
public:
  void func(T &container) { m_it = container.begin(); }

private:
  // 有了C++11的decltype 关键字，就可以直接这样写：
  decltype(T().begin()) m_it; // 注意这里
};

int main() {
  // auto和decltype关键字都可以自动推导出变量的类型，但它们的用法是有区别的:
  // auto varname = value;
  // decltype(exp) varname = value;
  // 其中，varname表示变量名，value表示赋给变量的值，exp表示一个表达式。
  // auto根据=右边的初始值value推导出变量的类型，而decltype根据exp表达式推导出变量的类型，跟=右边的value没有关系。
  // 另外，auto要求变量必须初始化，而decltype不要求。这很容易理解，auto是根据变量的初始值来推导出变量类型的，如果不初始化，变量的类型也就无法推导了。decltype可以写成下面的形式：
  // decltype(exp) varname;
  // 原则上讲，exp就是一个普通的表达式，它可以是任意复杂的形式，但是我们必须要保证exp的结果是有类型的，不能是void；
  // 当exp调用一个返回值类型为void的函数时，exp的结果也是void类型，此时就会导致编译错误。
  // decltype(test()); // declaration does not declare anything [-fpermissive]

  // decltype能够根据变量、字面量、带有运算符的表达式推导出变量的类型:
  int a = 0;
  decltype(a) b = 1;      // b被推导成了int
  decltype(10.8) x = 5.5; // x被推导成了double
  decltype(x + 100) y;    // y被推导成了double,没有初始化

  // decltype推导规则，当程序员使用decltype(exp)获取类型时，编译器将根据以下三条规则得出结果:
  // 如果exp是一个不被括号()包围的表达式，或者是一个类成员访问表达式，或者是一个单独的变量，那么decltype(exp)的类型就和exp一致，这是最普遍最常见的情况。
  // 如果exp是函数调用，那么decltype(exp)的类型就和函数返回值的类型一致。
  // 如果exp是一个左值，或者被括号()包围，那么decltype(exp)的类型就是exp的引用；假设exp的类型为T，那么decltype(exp)的类型就是T&。

  // exp是一个普通表达式:
  int n = 0;
  const int &r = n;
  Student stu;
  decltype(n) a3 = n; // n为int类型，a3被推导为int类型
  decltype(r) b3 = n; // r为const int&类型, b3被推导为const int&类型
  decltype(Student::total) c = 0; // c被推导为int类型
  decltype(stu.name) url = "http://c.biancheng.net/cplus/"; // url被推导为string

  // exp为函数调用:
  int n2 = 100;
  decltype(func_int_r(100, 'A')) a2 = n2; // a2的类型为int&
  decltype(func_int_rr()) b2 = 0;         // b2的类型为int&&
  decltype(func_int(10.5)) c2 = 0;        // c2的类型为int
  decltype(fun_cint_r(1, 2, 3)) x2 = n;   // x2的类型为const int&
  decltype(func_cint_rr()) y2 = 0;        // y2的类型为const int&&

  // exp是左值，或者被()包围:
  const Base obj;
  //带有括号的表达式
  decltype(obj.x) a1 = 0; // obj.x为类的成员访问表达式，符合推导规则一，a1为int
  decltype((obj.x)) b1 = a1; // obj.x带有括号，符合推导规则三，b的类型为int&
  //加法表达式
  int n1 = 0, m1 = 0;
  decltype(n1 + m1) c1 = 0; // n+m得到一个右值，符合规则一，所以推导结果为int
  decltype(n1 = n1 + m1) d1 = c1; // n=n+m得到一个左值，符号规则三，结果为int&

  // decltype的实际应用:
  // auto的语法格式比decltype简单，所以在一般的类型推导中，使用auto比使用decltype更加方便。
  // auto只能用于类的静态成员，不能用于类的非静态成员（普通成员），如果我们想推导非静态成员的类型，这个时候就必须使用decltype了:
  const std::vector<int> v;
  // 使用Base1类的时候，如果传入一个const类型的容器，编译器马上就会弹出一大堆错误信息。
  // 原因就在于，T::iterator并不能包括所有的迭代器类型，当T是一个const容器时，应当使用const_iterator。
  // Base1<const std::vector<int>> obj1;
  // obj1.func(v);
  Base2<const std::vector<int>> obj2;
  obj2.func(v);
}