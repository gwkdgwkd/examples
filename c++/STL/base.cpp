#include <iostream>
#include <vector>

// STL，英文全称standard template library，
// 中文可译为标准模板库或者泛型库，
// 其包含有大量的模板类和模板函数，是C++提供的一个基础模板的集合，
// 用于完成诸如输入/输出、数学计算等功能。
// STL最初由惠普实验室开发，于1998年被定为国际标准，
// 正式成为C++程序库的重要组成部分。
// 值得一提的是，如今STL已完全被内置到支持C++的编译器中，
// 无需额外安装，这可能也是STL被广泛使用的原因之一。
// STL就位于各个C++的头文件中，即它并非以二进制代码的形式提供，
// 而是以源代码的形式提供。
// 从根本上说，STL是一些容器、算法和其他一些组件的集合，
// 所有容器和算法都是总结了几十年来算法和数据结构的研究成果，
// 汇集了许多计算机专家学者经验的基础上实现的，因此可以说，
// STL基本上达到了各种存储方法和相关算法的高度优化。

// Alexander Stepanov（后被誉为STL标准模板库之父，后简称Stepanov），
// 1950年出生与前苏联的莫斯科，他曾在莫斯科大学研究数学，
// 此后一直致力于计算机语言和泛型库研究。
// 1987年，在贝尔实验室工作的Stepanov开始首次采用C++语言进行泛型软件库的研究。
// 由于当时的C++语言还没有引入模板的编程技术，
// 泛型库只能是通过C++的继承机制来开发，代码表达起来非常笨拙。
// 1992年，由于参加并主持了实验室主任Bill Worley所建立的一个有关算法的研究项目，
// 才使他重新回到了泛型化算法的研究工作上来。
// 项目自建立之后，参与者从最初的8人逐渐减少，
// 最后只剩下Stepanov和Meng Lee两个人。
// 经过长时间的努力，
// 最终完成了一个包含有大量数据结构和算法部件的庞大运行库（HP版本的C++STL），
// 这便是现在STL的雏形。
// 1993年，当时在贝尔实验室的Andrew Koenig看到了Stepanov的研究成果，
// 在他的鼓励与帮助下，Stepanov于1993年9月在圣何塞为ANSI/ISO C++标准委员会，
// 做了一个题为The Science of C++ Programming的演讲，向委员们讲述了其观念。
// 然后又于1994年3月，在圣迭戈会议上向委员会提交了一份建议书，
// 以期将STL通用库纳入C++标准。
// 最终在1994年的滑铁卢会议上，委员们通过了提案，
// 决定将STL正式纳入C++标准化进程之中，随后STL便被放进了会议的工作文件中。
// 自此，STL终于成为C++家族中的重要一员。
// 此后，随者C++标准的不断改进，STL也在不断地做着相应的演化。
// 直至1998年，ANSI/ISO C++标准正式定案，
// STL始终是C++标准库不可或缺的重要组成部分。

// 自1998年ANSI/ISO C++标准正式定案，C++ STL规范版本正式通过以后，
// 由于其实开源的，各个C++编译器厂商在此标准的基础上，
// 实现了满足自己需求的C++ STL泛型库，
// 主要包括HP STL、SGI STL、STLport、PJ STL、Rouge Wave STL等。

// C++标准程序库发展至今，几乎所有内容都被设计为了模板的形式，
// STL已经成为C++程序库的重要组成部分。
// 可以这么说，如果C++不支持STL标准模板库，就无法使用程序库。
// STL是一个具有高度可用性、高效的模板库，
// 该库包含了诸多在计算机科学领域中常用的基础数据结构和算法，
// 掌握了STL标准，很多功能不用重复的造轮子，直接拿来用即可。
// 总的来说，STL模板库是C++标准程序库的重要组成部分，
// 为C++程序员提供了大量的可扩展的程序框架，
// 高度实现了代码的可重用性，并且它是内置的，不需要额外安装，使用非常方便。

// 在计算机程序设计领域，为了避免因数据类型的不同，
// 而被迫重复编写大量相同业务逻辑的代码，人们发展的泛型及泛型编程技术。
// 所以泛型，实质上就是不使用具体数据类型（int、double、float等），
// 而是使用一种通用类型来进行程序设计的方法，
// 该方法可以大规模的减少程序代码的编写量，
// 让程序员可以集中精力用于业务逻辑的实现。
// 值得一提的是，既然泛型并不是真实的数据类型，
// 那么使用泛型编写的代码也就不是真正的程序实体，
// 只能算是一个程序实体的样板。
// 故此，通常形象的将这种使用了泛型的代码称为模板，
// 由模板生成实际代码的过程称为模板的具体实现。
// 类型占位符的替换工作，不需要人为操控，可以完全交由计算机来完成，
// 更准确的说，是交由编译器在编译阶段来完成模板的具体实现。
// 总之一句话，泛型也是一种数据类型，
// 只不过它是一种用来代替所有类型的通用类型。
// 在C++中，用以支持泛型应用的就是标准模板库STL，
// 它提供了C++泛型设计常用的类模板和函数模板。

// 通常认为，STL是由容器、算法、迭代器、函数对象、
// 适配器、内存分配器这6部分构成，其中后面4部分是为前2部分服务的:
// 1.容器：一些封装数据结构的模板类，
//   例如vector向量容器、list列表容器等。
// 2.算法：STL提供了非常多（约100个）的数据结构算法，
//   都被设计成一个个的模板函数，在std命名空间中定义，
//   其中大部分算法都包含在头文<algorithm>中，
//   少部分位于头文件<numeric>中。
// 3.迭代器：在C++ STL中，对容器中数据的读和写，
//   是通过迭代器完成的，扮演着容器和算法之间的胶合剂。
// 4.函数对象：如果一个类将()运算符重载为成员函数，
//   这个类就称为函数对象类，这个类的对象就是函数对象，又称仿函数。
// 5.适配器：可以使一个类的接口（模板的参数）适配成用户指定的形式，
//   从而让原本不能在一起工作的两个类工作在一起。
//   值得一提的是，容器、迭代器和函数都有适配器。
// 6.内存分配器：为容器类模板提供自定义的内存申请和释放功能，
//   由于往往只有高级用户才有改变内存分配策略的需求，
//   因此内存分配器对于一般用户来说，并不常用。
// 在惠普实验室最初发行的版本中，STL被组织成48个头文件；
// 但在C++标准中，它们被重新组织为13个头文件：
//  <iterator> <functional> <vector> <deque> <list> <queue>
//  <stack> <set> <map> <algorithm> <numeric> <memory> <utility>
// 按照C++标准库的规定，所有标准头文件都不再有扩展名。
// 以<vector>为例，此为无扩展名的形式，而<vector.h>为有扩展名的形式。
// 但是，或许是为了向下兼容，或许是为了内部组织规划，
// 某些STL版本同时存储具备扩展名和无扩展名的两份文件；
// 甚至有些STL版本同时拥有3种形式的头文件；
// 但也有个别的STL版本只存在包含扩展名的头文件，
// 建议养成良好的习惯，遵照C++规范，使用无扩展名的头文件。

// 学习C++标准库，特别是STL，
// 经常需要考量算法和成员函数的效能（也就是运行效率，又称复杂度），
// 因此每个学习STL的都需要掌握一种衡量算法（或成员函数）复杂度的方法，
// 目前最常用的方法称为大O表示法（注意，不是数字0，而是字母O）。
// 使用大O表示法衡量某个算法的复杂度，
// 其实就是将该算法的运行时间用输入量为n的函数表示出来。
// 这里的输入量n在STL中通常指的是算法操作的元素个数。
// 常见的算法复杂度种类，以及使用大O表示法表示的复杂度:
//  常数阶  算法运行时间和所操作的元素个数无关 	       O(1)
//  对数阶  算法运行时间随所操作的元素个数呈对数增长 	  O(log(n))
//  线性阶  算法运行时间随所操作的元素个数呈线性增长 	  O(n)
//  指数阶  算法运行时间随所操作的元素个数呈m次方增长   O(n^m)常见的有O(n^2)、O(n^3)等
// 值得注意的是，大O表示法并不关心算法运行所消耗的具体时间，
// 换句话说，对于那些影响算法运行效率较小的因素，
// 使用大O表示法表示时会直接将其忽略。
// 例如，某个算法运行的复杂度为O(n)，呈线性增长，
// 但至于线性增长的具体程度（是100n还是2n），
// 在大O表示法看来，它们是一样的。
// 也就是说，采用这种测量法则，任何两个线性算法都将被视为具有相同的复杂度。
// 采用大O表示法甚至会出现这种一种情况，即带有巨大常量的线性算法，
// 很有可能会比小常量的指数算法更受欢迎，因为该方法无法显示出真实的运行时间。
// 所以请记住，大O表示法只是某种度量方法，它所显示的算法的最佳复杂度，
// 并不一定就是真正的最佳（最快）算法。
//  种类 	大O表示 	   1 	 2 	 5 	 10   50   100     1000     10000  // 元素个数
//  常量阶 	O(1) 	    1 	1 	1 	1 	 1 	  1 	    1 	     1
//  对数阶 	O(log(n)) 1 	2 	3 	4    6 	  7 	    10 	     13
//  线性阶 	O(n) 	    1 	2 	5 	10 	 50   100     1000     10000
//  2次方   O(n^2) 	  1 	4   25 	100  2500 10000   1000000  100000000
// 在考量算法的复杂度时，输入量n（操作元素的个数）的值必须足够大才有意义。

// 通过前面对STL各种民间版本的介绍我们知道，
// 全世界所有的STL实现版本都源于Stepanov和Meng Lee完成的原始版本，
// 也就是HP STL，该版本的每一个头文件中都有一份声明，
// 即允许任何人任意运用、拷贝、修改、传播、贩卖这些代码，
// 无需付费，但唯一的条件是必须将该份声明置于使用者新开发的文件内。
// 开放源代码的观念源自美国人Richard Stallman（理察·史托曼），
// 他认为私藏源代码是一种违反人性的罪恶行为，
// 而如果能与他人分享源代码，便可以让其他人从中学习，并回馈给原始创作者。
// 封锁源代码虽然可以程度不一地保障智慧可能衍生的财富，
// 却阻碍了使用者从中学习和修正错误的机会。
// 1984年，Stallman离开麻省理工学院，创立自由软件基金会（简称FSF），
// 并写下著名的GNU宣言，开始进行名为GNU的开放改革计划。
// GNU这个名称所要表达的含义可以理解为GNU is Not Unix，
// 还可以译为革奴，意思是从此革去被奴役的命运。
// GNU计划中，最著名的软件包括Emacs，Stallman开发的一个非常灵活的文本编辑器，
// 允许使用者自行增加新功能、GCC（C/C++编译器）以及后来的Linux操作系统。
// 后来就衍生出各种不同的授权，包括Library GPL、BSD License等，
// 这些授权的共同原则就是开放源代码，
// 但由于各种授权的用户群众所掺杂的本位主义，再加上精英分子难以妥协的个性，
// 使开放源代码阵营中的各个分支发生意见分歧甚至相互对立，
// 其中属GNU CPL和BSD License的拥护者表现最突出。
// 1998年，自由软件社群企图创造出开源这个新名词来整合各方，
// 他们组成了一个非财团法人的组织，注册了一个标记，并设立网站。
// 开源的定义共有9条，即任何软件只要符合这9条，就可称呼自己为开源软件。

int main() {
  int a1[10];
  // 这种定义数组的方法需要事先确定好数组的长度，即n必须为常量，
  // 这意味着，如果在实际应用中无法确定数组长度，
  // 则一般会将数组长度设为可能的最大值，但这极有可能导致存储空间的浪费。
  int *p = new int[20];

  // 这种定义方式可根据变量n动态申请内存，不会出现存储空间浪费的问题。
  // 但是，如果程序执行过程中出现空间不足的情况时，
  // 则需要加大存储空间，此时需要进行如下操作：
  // 1.新申请一个较大的内存空间，即执行：
  //   int *temp = new int[m];
  // 2.将原内存空间的数据全部复制到新申请的内存空间中，即执行：
  //   memecpy(temp, p, sizeof(int)*n);
  // 3.将原来的堆空间释放，即执行：
  //   delete []p; p = temp;
  // 而完成相同的操作，如果采用STL标准库，则会简单很多，
  // 因为大多数操作细节将不需要程序员关心。
  std::vector<int> a;  // 长度为0，不同的是，a可以根据数据的数量自动变长
  for (int i = 0; i < 10; i++) a.push_back(i);  // 向数组a中添加10个元素
  a.resize(100);  // 还可以手动调整数组a的大小
  a[90] = 100;
  a.clear();  // 还可以直接删除数组a中所有的元素，此时a的长度变为0
  a.resize(20, -1);  // 重新调整a的大小为20，并存储20个-1元素

  return 0;
}