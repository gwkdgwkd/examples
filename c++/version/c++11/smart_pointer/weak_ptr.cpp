#include <iostream>
#include <memory>

// 和shared_ptr、unique_ptr一样，weak_ptr也是以模板类的方式实现的。
// weak_ptr<T>定义在<memory>头文件，并位于std命名空间中。

// C++11标准虽然将weak_ptr定位为智能指针的一种，但通常不单独使用，
// 只能和shared_ptr类型指针搭配使用，weak_ptr是shared_ptr指针的辅助工具，
// 借助weak_ptr类型指针，可以获取shared_ptr指针的一些状态信息，
// 比如有多少指向相同的shared_ptr指针、shared_ptr指针指向的堆内存是否已经被释放等等。
// 当weak_ptr类型指针的指向和某一shared_ptr指针相同时，
// weak_ptr指针并不会使所指堆内存的引用计数加1；
// 同样，当weak_ptr指针被释放时，之前所指堆内存的引用计数也不会因此而减1。
// 也就是说，weak_ptr类型指针并不会影响所指堆内存空间的引用计数。

namespace n1 {
void func() {
  // 1.可以创建一个空weak_ptr指针：
  std::weak_ptr<int> wp1;
  // std::weak_ptr<int> wp2(nullptr);  // 不行
  // std::weak_ptr<int> wp3 = nullptr;  // 不行

  // 2.weak_ptr指针更常用于指向某一shared_ptr指针拥有的堆内存，
  //   因为在构建weak_ptr指针对象时，可以用已有的shared_ptr为其初始化：
  std::shared_ptr<int> sp(new int(5));
  std::cout << sp.use_count() << std::endl;  // 1
  std::weak_ptr<int> wp4(sp);
  std::cout << wp4.use_count() << std::endl;  // 1

  // 3.凭借已有的weak_ptr指针，可以创建一个新的weak_ptr指针：
  std::weak_ptr<int> wp5(wp1);  // 若wp1为空指针，则wp5也为空指针
  std::cout << wp5.use_count() << std::endl;  // 0
  // 反之，如果wp4指向某一shared_ptr指针拥有的堆内存，
  // 则wp6也指向该块存储空间（可以访问，但无所有权）：
  std::weak_ptr<int> wp6(wp4);
  std::cout << wp6.use_count() << std::endl;  // 1
  std::weak_ptr<int> wp7(sp);
  std::cout << wp7.use_count() << std::endl;  // 1

  std::weak_ptr<int> wp8 = wp4;
  std::weak_ptr<int> wp9 = sp;
  std::weak_ptr<int> wp10 = std::move(wp4);
  std::weak_ptr<int> wp11(std::move(wp4));

  std::cout << sp.use_count() << std::endl;  // 1
  wp6.reset();
  std::cout << sp.use_count() << std::endl;  // 1
  sp.reset();
  std::cout << sp.use_count() << std::endl;   // 0
  std::cout << wp5.use_count() << std::endl;  // 0
  std::cout << wp6.use_count() << std::endl;  // 0
  std::cout << wp7.use_count() << std::endl;  // 0
}
}  // namespace n1

namespace n2 {
// weak_ptr模板类提供的成员方法：
// operator=()  重载=赋值运算符，
//              使得weak_ptr指针可以用weak_ptr或shared_ptr指针赋值。
// swap(x) 	    其中x表示一个同类型的weak_ptr类型指针，
//              该函数可以互换2个同类型weak_ptr指针的内容。
// reset() 	    将当前weak_ptr指针置为空指针。
// use_count()  查看指向和当前weak_ptr指针相同的shared_ptr指针的数量。
// expired() 	  判断当前weak_ptr为否过期，指针为空，或者堆内存已经被释放。
// lock() 	    如果当前weak_ptr已经过期，则该函数会返回一个空的shared_ptr；
//              反之，该函数返回一个和当前weak_ptr指向相同的shared_ptr指针。
// weak_ptr<T>模板类中没有重载*和->运算符，这也就意味着，
// weak_ptr类型指针只能访问所指的堆内存，而无法修改它。

void func1() {
  std::shared_ptr<int> p1(new int(5));
  std::shared_ptr<int> p2(p1);
  std::weak_ptr<int> wp1(p1);
  std::cout << wp1.use_count() << std::endl;  // 2

  std::shared_ptr<int> p3(new int(6));
  std::weak_ptr<int> wp2(p3);
  std::cout << wp2.use_count() << std::endl;  // 1

  wp1.swap(wp2);
  std::cout << wp1.use_count() << std::endl;  // 1
  std::cout << wp2.use_count() << std::endl;  // 2
}

void func2() {
  std::shared_ptr<int> p1(new int(5));
  std::weak_ptr<int> wp1(p1);
  std::cout.setf(std::ios_base::boolalpha);
  std::cout << wp1.expired() << " " << wp1.use_count() << " " << p1.use_count()
            << std::endl;  // false 1 1

  wp1.reset();
  std::cout << wp1.expired() << " " << wp1.use_count() << " " << p1.use_count()
            << std::endl;  // true 0 1

  std::weak_ptr<int> wp2(p1);
  std::cout << wp2.expired() << " " << wp2.use_count() << " " << p1.use_count()
            << std::endl;  // false 1 1

  p1.reset();
  std::cout << wp2.expired() << " " << wp2.use_count() << " " << p1.use_count()
            << std::endl;  // true 0 0
}

void func3() {
  std::shared_ptr<int> p1(new int(6));
  std::weak_ptr<int> wp1(p1);
  std::cout.setf(std::ios_base::boolalpha);
  std::cout << wp1.expired() << " " << *(wp1.lock()) << std::endl;  // false 6

  wp1.reset();
  std::cout << wp1.expired() << std::endl;  // true

  std::weak_ptr<int> wp2(p1);
  *(wp2.lock()) = 7;
  std::cout << wp2.expired() << " " << *(wp2.lock().get()) << std::endl;
  // false 7

  p1.reset();
  std::cout << wp2.expired() << std::endl;  // true
}
}  // namespace n2

int main(int argc, char *argv[]) {
  if (argc < 2) {
    std::cout << argv[0] << " i [0 - 2]" << std::endl;
    return 0;
  }
  int type = argv[1][0] - '0';
  switch (type) {
    case 0:
      n1::func();
      break;
    case 1:
      n2::func1();
      break;
    case 2:
      n2::func2();
      break;
    case 3:
      n2::func3();
      break;
    default:
      std::cout << "invalid type" << std::endl;
      break;
  }

  return 0;
}