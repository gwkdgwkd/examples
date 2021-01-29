#include <iostream>
#include <vector>

int main() {
  // 语法格式的区别:
  // auto和decltype都是C++11新增的关键字，都用于自动类型推导，但是它们的语法格式是有区别的，如下所示：
  // auto varname = value;  //auto的语法格式
  // decltype(exp) varname [= value];  //decltype的语法格式
  // 其中，varname表示变量名，value表示赋给变量的值，exp表示一个表达式，方括号[]表示可有可无。
  // auto和decltype都会自动推导出变量varname的类型：
  // auto根据=右边的初始值value推导出变量的类型；
  // decltype根据exp表达式推导出变量的类型，跟=右边的value没有关系。
  // 另外，auto要求变量必须初始化，也就是在定义变量的同时必须给它赋值；而decltype不要求，初始化与否都不影响变量的类型。
  // 这很容易理解，因为auto是根据变量的初始值来推导出变量类型的，如果不初始化，变量的类型也就无法推导了。
  // auto将变量的类型和初始值绑定在一起，而decltype将变量的类型和初始值分开；虽然auto的书写更加简洁，但decltype的使用更加灵活。

  // 对cv限定符的处理:
  // 「cv限定符」是const和volatile关键字的统称：
  // const关键字用来表示数据是只读的，也就是不能被修改；
  // volatile和const是相反的，它用来表示数据是可变的、易变的，目的是不让CPU将数据缓存到寄存器，而是从原始的内存中读取。
  // 在推导变量类型时，auto和decltype对cv限制符的处理是不一样的。decltype会保留cv限定符，而auto有可能会去掉cv限定符。
  // 以下是auto关键字对cv限定符的推导规则：
  // 如果表达式的类型不是指针或者引用，auto会把cv限定符直接抛弃，推导成non-const或者non-volatile类型。
  // 如果表达式的类型是指针或者引用，auto将保留cv限定符。
  //非指针非引用类型
  const int n1 = 0;
  auto n2 = 10;
  // n2赋值成功，说明不带const，也就是const被auto抛弃了，这验证了auto的第一条推导规则
  n2 = 99; //赋值不报错
  decltype(n1) n3 = 20;
  // 说明decltype不会去掉表达式的const属性
  // n3 = 5; //赋值报错
  //指针类型
  const int *p1 = &n1;
  auto p2 = p1;
  // p2赋值失败，说明是带const的，也就是const没有被auto抛弃，这验证了auto的第二条推导规则。
  // *p2 = 66; //赋值报错
  decltype(p1) p3 = p1;
  // 说明decltype不会去掉表达式的const属性
  // *p3 = 19; //赋值报错

  // 对引用的处理:
  // 当表达式的类型为引用时，auto和decltype的推导规则也不一样；decltype会保留引用类型，而auto会抛弃引用类型，直接推导出它的原始类型。
  int n = 10;
  int &r1 = n;
  // auto推导
  auto r2 = r1;
  r2 = 20;
  // 给r2赋值并没有改变n的值，这说明r2没有指向n，而是自立门户，单独拥有了一块内存，这就证明r不再是引用类型，它的引用类型被auto抛弃了。
  std::cout << n << ", " << r1 << ", " << r2 << std::endl; // 10, 10, 20
  // decltype推导
  decltype(r1) r3 = n;
  r3 = 99;
  // 给r3赋值，n的值也跟着改变了，这说明r3仍然指向n，它的引用类型被decltype保留了。
  std::cout << n << ", " << r1 << ", " << r3 << std::endl; // 99, 99, 99

  // 总结:
  // auto虽然在书写格式上比decltype简单，但是它的推导规则复杂，有时候会改变表达式的原始类型；
  // 而decltype比较纯粹，它一般会坚持保留原始表达式的任何类型，让推导的结果更加原汁原味。

  // 从代码是否健壮的角度考虑，推荐使用decltype，它没有那么多是非；但是decltype总是显得比较麻烦，尤其是当表达式比较复杂时，例如：
  std::vector<int> nums;
  decltype(nums.begin()) it1 = nums.begin();
  // 而如果使用auto就会清爽很多：
  auto it2 = nums.begin();

  // 在实际开发中人们仍然喜欢使用auto关键字，因为它用起来简单直观，更符合人们的审美。
  // 如果你的表达式类型不复杂，还是推荐使用auto关键字，优雅的代码总是叫人赏心悦目，沉浸其中。

  return 0;
}