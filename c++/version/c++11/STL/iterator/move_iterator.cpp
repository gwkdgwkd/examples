#include <algorithm>
#include <iostream>
#include <iterator>
#include <list>
#include <string>
#include <vector>

// C++11还为STL标准库增添了move_iterator迭代器适配器，又可简称为移动迭代器，
// 其可以实现以移动而非复制的方式，将某个区域空间中的元素移动至另一个指定的空间。
// 实现移动迭代器的模板类定义在<iterator>头文件，并位于std命名空间中。
// 实现move_iterator移动迭代器的模板类定义如下：
// template <class Iterator> class move_iterator;
// 在使用此迭代器时，需要传入一个基础迭代器Iterator。
// 此基础迭代器的类型虽然没有明确要求，但该模板类中某些成员方法的底层实现，
// 需要此基础迭代器为双向迭代器或者随机访问迭代器。
// 也就是说，如果指定的Iterator类型仅仅是输入迭代器，则某些成员方法将无法使用。
// 在move_iterator模板类中就包含有指定Iterator类型的基础迭代器，
// 整个模板类也是借助此基础迭代器实现的。

// C++ STL标准库中，make_move_iterator()是以函数模板的形式提供的：
// template <class Iterator>
// move_iterator<Iterator> make_move_iterator(const Iterator& it);
// 其中，参数it为基础迭代器，用于初始化新建迭代器。
// 同时，该函数会返回一个创建好的移动迭代器。

template <typename T>
void print(T t) {
  std::copy(t.begin(), t.end(),
            std::ostream_iterator<std::string>(std::cout, ","));
  std::cout << std::endl;
}

typedef std::vector<std::string>::iterator Iter;
void func1() {
  // move_iterator模板类中，提供了4种创建move_iterator迭代器的方法：

  // 1.通过调用该模板类的默认构造函数，可以创建一个不指向任何对象的移动迭代器
  std::move_iterator<Iter> it1;
  // 创建好了一个移动迭代器，该迭代器底层使用的是vector容器的随机访问迭代器，
  // 但这里没有为此基础迭代器明确指向，所以迭代器也不知向任何对象。

  // 2.在创建move_iterator迭代器的同时，也可以为其初始化
  std::vector<std::string> v{"one", "two", "three"};
  std::move_iterator<Iter> it2(v.begin());
  // 创建的同时还为底层使用的随机访问迭代器做了初始化，即令其指向容器的第一个元素。

  // 3.move_iterator模板类还支持用已有的移动迭代器初始化新建的同类型迭代器
  std::move_iterator<Iter> it3(it1);
  std::move_iterator<Iter> it4 = it1;

  // 4.以上3种创建move_iterator迭代器的方式，其本质都是直接调用构造方法实现的，
  //   除此之外，C++ STL标准库还提供了一个make_move_iterator()函数：
  std::move_iterator<Iter> it5 = std::make_move_iterator(v.begin());
}

void func2() {
  std::vector<std::string> v1{"nihao", "wohao", "tahao"};
  std::vector<std::string> v2(v1.begin(), v1.end());
  std::cout << "v1:";
  print(v1);  // v1:nihao,wohao,tahao,
  std::cout << "v2:";
  print(v2);  // v2:nihao,wohao,tahao,
  // v2容器从v1容器中复制了一份数据并存储起来，此过程不会影响v1容器。

  std::vector<std::string> v3{"nihao", "wohao", "tahao"};
  std::vector<std::string> v4(make_move_iterator(v3.begin()),
                              make_move_iterator(v3.end()));
  std::cout << "v3:";
  print(v3);  // v3:,,,
  std::cout << "v4:";
  print(v4);  // v4:nihao,wohao,tahao,

  // 即便通过移动迭代器将容器中某区域的元素移动到了其他容器中，
  // 该区域内仍可能残留有之前存储的元素，但是不能再被使用，否则很可能产生错误。

  std::cout << v3.size() << std::endl;  // 3
}

void func3() {
  // 和其他迭代器适配器一样，move_iterator模板类中也提供有base()成员方法，
  // 通过该方法，可以获取到当前移动迭代器底层所使用的基础迭代器
  std::vector<std::string> v1{"nihao", "wohao", "tahao"};
  std::move_iterator<Iter> begin = std::make_move_iterator(v1.begin());
  std::move_iterator<Iter> end = std::make_move_iterator(v1.end());
  std::vector<std::string> v2(begin.base(), end.base());
  std::cout << "v1:";
  print(v1);  // v1:nihao,wohao,tahao,
  std::cout << "v2:";
  print(v2);  // v2:nihao,wohao,tahao,
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