#include <iostream>
#include <iterator>
#include <vector>

// 流迭代器也是一种迭代器适配器，不过和之前讲的迭代器适配器有所差别，它的操作对象不再是某个容器，而是流对象。
// 即通过流迭代器，我们可以读取指定流对象中的数据，也可以将数据写入到流对象中。
// 我们经常使用的cin、cout就属于流对象，其中cin可以获取键盘输入的数据，cout可以将指定数据输出到屏幕上。
// 除此之外，更常见的还有文件I/O流等。
// 介于流对象又可细分为输入流对象（istream）和输出流对象（ostream），C++ STL标准库中，也对应的提供了2类流迭代器：
// 1.将绑定到输入流对象的迭代器称为输入流迭代器（istream_iterator），其可以用来读取输入流中的数据；
// 2.将绑定到输出流对象的迭代器称为输出流迭代器（ostream_iterator），其用来将数据写入到输出流中。
// istream_iterator和ostream_iterator迭代器的模板类也定义在<iterator>头文件，并位于std命名空间中。

// 输入流迭代器用于直接从指定的输入流中读取元素，该类型迭代器本质上就是一个输入迭代器，
// 这意味着假设p是一个输入流迭代器，则其只能进行++p、p++、*p操作，同时输入迭代器之间也只能使用==和!=运算符。
// 输入流迭代器的底层是通过重载++运算符实现的，该运算符内部会调用operator >>读取数据。
// 也就是说，假设it为输入流迭代器，则只需要执行++it或者it++，即可读取一个指定类型的元素。

// 和输入流迭代器恰好相反，输出流迭代器用于将数据写到指定的输出流（如cout）中。
// 另外，该类型迭代器本质上属于输出迭代器，假设p为一个输出迭代器，则它能执行++p、p++、*p=t以及*p++=t等类似操作。
// 其次，输出迭代器底层是通过重载赋值（=）运算符实现的，即借助该运算符，
// 每个赋值给输出流迭代器的元素都会被写入到指定的输出流中。

void func1() {
  // 创建输入流迭代器的方式有3种

  // 1.调用istream_iterator模板类的默认构造函数，可以创建出一个具有结束标志的输入流迭代器：
  // 当我们从输入流中不断提取数据时，总有将流中数据全部提取完的那一时刻，这一时刻就可以用此方式构建的输入流迭代器表示。
  std::istream_iterator<int> eos;

  // 2.还可以创建一个可用来读取数据的输入流迭代器：
  std::istream_iterator<int> iit1(std::cin);
  // 创建了一个可从标准输入流cin读取数据的输入流迭代器。
  // 通过此方式创建的输入流迭代器，其调用的构造函数中，会自行尝试去指定流中读取一个指定类型的元素。

  // 3.istream_iterator模板类还支持用已创建好的istream_iterator迭代器为新建istream_iterator迭代器初始化：
  std::istream_iterator<int> iit2(iit1);
  std::istream_iterator<int> iit3 = iit1;

  int value1, value2;
  std::cout << "请输入2个数: ";
  std::istream_iterator<int> iit(std::cin);
  // 只有读取到EOF流结束符时，程序中的iit才会和eos相等。
  // 另外，Windows平台上使用Ctrl+Z组合键输入^Z表示EOF流结束符，此结束符需要单独输入，或者输入换行符之后再输入才有效。
  if (iit != eos) {
    value1 = *iit;
  }
  iit++;
  if (iit != eos) {
    value2 = *iit;
  }
  std::cout << "value1 = " << value1 << std::endl;
  std::cout << "value2 = " << value2 << std::endl;
}

void func2() {
  // ostream_iterator模板类中也提供了3种创建ostream_iterator迭代器的方法

  // 1.通过调用该模板类的默认构造函数，可以创建了一个指定输出流的迭代器：
  std::ostream_iterator<int> out_it1(std::cout);
  // 创建了一个可将int类型元素写入到输出流（屏幕）中的迭代器。
  *out_it1 = 1;
  *out_it1 = 2;
  *out_it1 = 3;
  std::cout << std::endl;  // 123

  // 2.在第一种方式的基础上，还可以为写入的元素之间指定一个分隔符：
  std::ostream_iterator<int> out_it2(std::cout, ",");
  // 和第一种写入方式不同之处在于，此方式在向输出流写入int类型元素的同时，还会附带写入一个逗号（,）。

  // 3.在创建输出流迭代器时，可以用已有的同类型的迭代器，为其初始化：
  std::ostream_iterator<int> out_it3(out_it1);
  std::ostream_iterator<std::string> out_it4(std::cout);

  *out_it4 = "hello world";
  std::cout << std::endl;  // hello world

  std::ostream_iterator<int> out_it5(std::cout, ",");
  *out_it5 = 1;
  *out_it5 = 2;
  *out_it5 = 3;
  std::cout << std::endl;  // 1,2,3,
}

void func3() {
  // 在实际场景中，输出流迭代器常和copy()函数连用，即作为该函数第3个参数:
  std::vector<int> v;
  for (int i = 1; i < 10; ++i) {
    v.push_back(i);
  }
  std::ostream_iterator<int> out_it(std::cout, ", ");
  std::copy(v.begin(), v.end(), out_it);
  std::cout << std::endl;  // 1, 2, 3, 4, 5, 6, 7, 8, 9,
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