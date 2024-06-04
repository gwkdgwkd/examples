#include <array>
#include <iostream>

// 将与参数无关的代码抽离template

// template是节省时间和避免代码重复的一个奇妙方法。
// 使用template可能会导致代码膨胀：
// 源码看起来很整洁，但目标码却不同，二进制码带着重复的代码、数据，或两者。

namespace n1 {
// 这是template引出代码膨胀的一个典型例子：
template <typename T, size_t n>
class SquareMatrix {
 public:
  void invert() {}
};
void func() {
  // 这会具现化两份invert，这些函数并非完完全全相同，只有常量5和10不同：
  SquareMatrix<double, 10> sm1;
  sm1.invert();
  SquareMatrix<double, 5> sm2;
  sm2.invert();
}
}  // namespace n1

namespace n2 {
// 第一次修改：
template <typename T>
class SquareMatrixBase {
  // SquareMatrixBase只对矩阵元素对象的类型参数化，不对矩阵的尺寸参数化。
  // 对于给定元素对象类型，所有矩阵共享唯一一个SquareMatrixBase class，
  // 也将因此共享这唯一一个class内的invert。
 protected:
  void invert(size_t matrixSize) {}
};
template <typename T, size_t n>
class SquareMatrix : private SquareMatrixBase<T> {
 private:
  using SquareMatrixBase<T>::invert;

 public:
  void invert() { this->invert(n); }
};
void func() {
  SquareMatrix<double, 10> sm1;
  sm1.invert();
  SquareMatrix<double, 5> sm2;
  sm2.invert();
}
}  // namespace n2

namespace n3 {
// SquareMatrixBase::invert如何知道该操作什么数据？
// 只知道尺寸，如何知道哪个特定矩阵的数据在哪儿？
// 只有derived class知道，Derived class如何联络base class做逆运算？
// 1.一个可能的做法是为SquareMatrixBase::invert添加另一个参数，
//   也许是个指针，指向一块用来放置矩阵数据的内存起起始点。
//   行的通，但invert一般不是唯一一个invert这样的函数，
//   每个函数都要加个参数，做相同的事，似乎不好。
// 2.另一个办法是令SquareMatrixBase存储一个指针，
//   指向矩阵数值所在的内存，其实也可能存储矩阵尺寸。
template <typename T>
class SquareMatrixBase {
 protected:
  SquareMatrixBase(size_t n, T* pMem) : size(n), pData(pMem) {}
  void setDataPtr(T* ptr) { pData = ptr; }

 private:
  size_t size;
  T* pData;
};
namespace test1 {
// 这允许derived class指定内存分配方式，
// 某些实现版本也许会决定将矩阵数据存储在SquareMatrix对象内部：
template <typename T, size_t n>
class SquareMatrix : private SquareMatrixBase<T> {
 public:
  SquareMatrix() : SquareMatrixBase<T>(n, data) {}

 private:
  T data[n * n];
};
}  // namespace test1

namespace test2 {
// 上面这种类型不需要动态分配内存，但对象自身可能非常大，
// 另一种做法是，把每一个矩阵的数据放进heap：
template <typename T, size_t n>
class SquareMatrix : private SquareMatrixBase<T> {
 public:
  SquareMatrix() : SquareMatrixBase<T>(n, 0), pData(new T[n * n]) {
    this->setDataPtr(pData.get());
  }

 private:
  std::array<T, n * n> pData;
};
}  // namespace test2
}  // namespace n3

// non-tpye template param能带来膨胀，其实type param也会导致膨胀。
// vector<int>和vector<long>的成员函数有可能完全相同。
// 有些连接器会合并完全相同的函数实现，但有些不会，
// 意味着template被具现化为int和long两个版本，造成代码膨胀。
// 大多数平台上，所有指针类型都有相同的二进制表述，
// 因此凡template持有指针者，list<int*>,list<const int*>等等，
// 往往应该对每一个成员函数使用唯一一份底层代码。
// 意味着，如果实现某些成员函数而它们操作强型指针（T*)，
// 应该令它们调用另一个操作void*的函数，由后者完成实际工作。

// 请记住：
// 1.template生成多个class和多个函数，所以任何template代码，
//   都不该与某个造成膨胀的template参数产生相依关系；
// 2.因非类型模板参数而造成的代码膨胀，往往可以消除，
//   做法是以函数参数或class成员变量替换template参数；
// 3.因类型参数而造成的代码膨胀，往往可降低，
//   做法是让带有完全相同二进制表述的具现类型共享实现码。

int main(int argc, char* argv[]) {
  if (argc < 2) {
    std::cout << argv[0] << " i [0 - 1]" << std::endl;
    return 0;
  }
  int type = argv[1][0] - '0';
  switch (type) {
    case 0:
      n1::func();
      break;
    case 1:
      n2::func();
      break;
    default:
      std::cout << "invalid type" << std::endl;
      break;
  }

  return 0;
}