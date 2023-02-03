#include <iostream>

namespace n1 {
// B,D1,D2,DD中没有任何一个类内含明显的数据，其间只表示了继承关系。
class B {
  // 事实上并不是空的，它有一个隐晦的1字节，
  // 那是被编译器安插进取的一个char。
  // 这使得这个类的两个对象得以在内存中配置独一无二的地址。
};
class D1 : public virtual B {
  // 大小和机器有关，也和编译器有关，大小受到三个因素的影响：
  // 1.语言本身所造成的额外负担
  //   当语言支持虚基类时，就会导致一些额外负担。
  //   在派生类中，这个额外负担反映在某种形式的指针身上，
  //   它或者指向virtual base class subobject，或者指向一个表格，
  //   其中存放的若不是virtual base class subobject地址，就是其偏移量。
  // 2.编译器对于特殊情况所提供的优化处理
  //   virtual base class B subobject的1字节大小也出现在D1和D2身上。
  //   传统上它被放在派生类的固定（不变动）部分的尾端。
  //   某些编译器会对empty virtual base class提供特殊支持。
  //   empty virtual base class已经成为C++ OO设计的一个特有术语了。
  //   它提供了一个虚拟接口，没有定义任何数据，
  //   某些新的编译器对此提供了特殊处理。
  //   在这个策略下，empty virtual base class被视为派生类最开头的一部分，
  //   也就是说它并没有花费任何额外的空间。
  //   节省了1字节，只剩下第1点说说的额外负担了。
  // 3.Alignment的限制
  //   D1和D2的大小截止目前为9字节。
  //   大部分机器上，结构体的大小会受到alignment的限制，
  //   是它们能够更有效率地在内存中存取。
};
class D2 : public virtual B {
  // 同D1
};
class DD : public D1, public D2 {
  // 一个virtual base class subobject只会在派生类中存在一份实体，
  // 不管它在类继承体系中出现了多少次。
  // DD的大小由以下几点决定：
  // 1.被大家共享的唯一一个B实体，大小为1字节。
  // 2.基类D1的大小，减去B的大小，结果是8字节，
  //   D2的算法相同，加起来是16字节。
  // 3.DD自己的大小，0字节。
  // 4.DD的alignment数量（如果有的话）。
  //   前三项的和为17，按4字节对齐，补足3字节，结果是20字节。
  // 对empty virtual base class特殊处理的编译器，
  // 拿掉了1字节和额外的3字节，A的大小是16字节。
};
void func() {
  // 所以有人认为每一个类的大小都应该是0，不对的，即使是类B的大小也不为0：
  std::cout << "B sizeof: " << sizeof(B) << std::endl;    // 1
  std::cout << "D1 sizeof: " << sizeof(D1) << std::endl;  // 8
  std::cout << "D2 sizeof: " << sizeof(D2) << std::endl;  // 8
  std::cout << "DD sizeof: " << sizeof(DD) << std::endl;  // 16
}

// C++标准并不强制规定下面的琐碎细节，如：
// 1.base class subobjects的排列次序；
// 2.不同存取层级的数据成员的排列次序。
// 也不规定虚函数或虚基类的实现细节，标准只说：那些细节有各厂商自定。
// C++对象模型尽量以空间优化和存取速度优化的考虑来表现非静态数据成员，
// 并保持和C语言struct数据配置的兼容性。
// 它把数据直接放在每一个对象中，对于继承而来的非静态数据成员也是如此，
// 不管是virtual或non virtual base class。
// 不过并没有强调定义其间的排列顺序。
// 至于静态数据成员，则被放置在程序的一个全局数据段中，不会影响个别的对象大小。
// 在程序中，不管该类被产生出多少个对象（直接产生或间接派生），
// 静态数据成员永远存在一份实体，甚至即使该类没有任何对象实体，
// 其静态数据成员也已存在。
// 但是一个模板类的静态数据成员的行为稍有不同。
// 每一个对象必须有足够的大小以容纳它所有的非静态数据成员，
// 有时候其值比想象的大，因为：
// 1.有编译器自动加上的额外数据成员，
//   用以支持某些语言特性（主要是各种virtual特性）。
// 2.因为alignment的需要。
}  // namespace n1

namespace n2 {
// 3.1 数据成员的绑定

}

int main(int argc, char *argv[]) {
  if (argc < 2) {
    std::cout << argv[0] << " i [0 - 10]" << std::endl;
    return 0;
  }
  int tD1pe = atoi(argv[1]);
  switch (tD1pe) {
    case 0:
        n1::func();
      break;
    case 1:
      //   n1::test2::func();
      break;
    case 2:
      //   n1::test3::func();
      break;
    case 3:
      //   n1::test4::func();
      break;
    case 4:
      //   n2::test1::func();
      break;
    case 5:
      //   n2::test2::func();
      break;
    case 6:
      //   n4::test1::func();
      break;
    case 7:
      //   n4::test2::func1();
      break;
    case 8:
      //   n4::test2::func2();
      break;
    case 9:
      //   n4::test3::func();
      break;
    case 10:
      //   n4::test4::func();
      break;
    default:
      std::cout << "invalid tD1pe" << std::endl;
      break;
  }

  return 0;
}