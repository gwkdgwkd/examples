#include <iostream>
#include <iterator>
#include <list>
#include <string>
#include <vector>

using namespace std;

// C++ 11还为STL标准库增添了move_iterator 迭代器适配器，又可简称为移动迭代器，其可以实现以移动而非复制的方式，将某个区域空间中的元素移动至另一个指定的空间。
// 实现移动迭代器的模板类定义在<iterator>头文件，并位于std命名空间中。
// 实现move_iterator移动迭代器的模板类定义如下：
// template <class Iterator> class move_iterator; 在使用此迭代器时，需要传入一个基础迭代器Iterator。
// 此基础迭代器的类型虽然没有明确要求，但该模板类中某些成员方法的底层实现，需要此基础迭代器为双向迭代器或者随机访问迭代器。也就是说，如果指定的Iterator类型仅仅
// 是输入迭代器，则某些成员方法将无法使用。
// 在move_iterator模板类中就包含有指定Iterator类型的基础迭代器，整个模板类也是借助此基础迭代器实现的。

// C++ STL标准库中，make_move_iterator() 是以函数模板的形式提供的，其语法格式如下：
// template <class Iterator> move_iterator<Iterator> make_move_iterator (const Iterator& it);
// 其中，参数it为基础迭代器，用于初始化新建迭代器。同时，该函数会返回一个创建好的移动迭代器。

int main() {
  // move_iterator模板类中，提供了4种创建move_iterator迭代器的方法
  // 1 通过调用该模板类的默认构造函数，可以创建一个不指向任何对象的移动迭代器
  typedef std::vector<std::string>::iterator Iter1;
  std::move_iterator<Iter1> mIter1;
  // 创建好了一个mIter移动迭代器，该迭代器底层使用的是vector容器的随机访问迭代器，但这里没有为此基础迭代器明确指向，所以mIter迭代器也不知向任何对象。
  // 2 在创建move_iterator迭代器的同时，也可以为其初始化
  std::vector<std::string> myvec{"one", "two", "three"};
  typedef std::vector<std::string>::iterator Iter2;
  std::move_iterator<Iter2> mIter2(myvec.begin());
  // 创建了一个mIter移动迭代器，同时还为底层使用的随机访问迭代器做了初始化，即令其指向myvec容器的第一个元素。
  // 3 move_iterator模板类还支持用已有的移动迭代器初始化新建的同类型迭代器
  std::move_iterator<Iter1> mIter31(mIter1);
  std::move_iterator<Iter1> mIter32 = mIter1;
  // 4 以上3种创建move_iterator迭代器的方式，其本质都是直接调用move_iterator模板类中的构造方法实现的。
  // 除此之外，C++ STL标准库还提供了一个make_move_iterator()函数，通过调用此函数可以快速创建一个move_iterator迭代器。
  typedef std::vector<std::string>::iterator Iter3;
  std::move_iterator<Iter3> mIter4 = make_move_iterator(myvec.begin());

  vector<string> myvec1{"STL", "Python", "Java"};
  vector<string> othvec1(myvec1.begin(), myvec1.end());
  cout << "myvec:" << endl;
  for (auto ch : myvec1) {
    cout << ch << " ";
  }
  cout << endl << "othvec:" << endl;
  for (auto ch : othvec1) {
    cout << ch << " ";
  }
  cout << endl;
  //othvec容器从myvec容器中复制了一份"STL"、"Python"、"Java"并存储起来，此过程不会影响myvec容器。
  // 那么，如果不想采用复制的方式，而就是想myvec容器中存储的元素全部移动到othvec容器中，该怎么办呢？没错，就是采用移动迭代器。

  vector<string> myvec2{"STL", "Python", "Java"};
  vector<string> othvec2(make_move_iterator(myvec2.begin()),
                         make_move_iterator(myvec2.end()));

  cout << "myvec:" << endl;
  for (auto ch : myvec2) {
    cout << ch << " ";
  }
  cout << endl << "othvec:" << endl;
  for (auto ch : othvec2) {
    cout << ch << " ";
  }
  cout << endl;
  // myvec:
  //
  // othvec:
  // STL Python Java
  // 即便通过移动迭代器将容器中某区域的元素移动到了其他容器中，该区域内仍可能残留有之前存储的元素，但这些元素是不能再被使用的，否则极有可能使程序产生各种其他错误。

  // 和其他迭代器适配器一样，move_iterator模板类中也提供有base()成员方法，通过该方法，我们可以获取到当前移动迭代器底层所使用的基础迭代器。

  typedef std::vector<std::string>::iterator Iter5;
  vector<std::string> myvec3{"STL", "Java", "Python"};
  std::move_iterator<Iter5> begin = make_move_iterator(myvec3.begin());
  std::move_iterator<Iter5> end = make_move_iterator(myvec3.end());
  vector<std::string> othvec3(begin.base(), end.base());
  cout << "myvec:" << endl;
  for (auto ch : myvec3) {
    cout << ch << " ";
  }
  cout << endl << "othvec:" << endl;
  for (auto ch : othvec3) {
    cout << ch << " ";
  }
  cout << endl;
  // myvec:
  // STL Java Python
  // othvec:
  // STL Java Python
  // 通过调用base()成员方法，初始化othvec容器的方式转变为以复制而非移动的方式，因此myvec容器不会受到影响。

  return 0;
}