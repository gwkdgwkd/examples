#include <iostream>

// 不要对数组使用多态

namespace n1 {
// 类继承的最重要的特性是可以通过基类指针或引用来操作派生类。
// 这样的指针或引用具有行为的多态性，就好像它们同时具有多种形态。
// C++允许你通过基类指针和引用来操作派生类数组。
// 不过这根本就不是一个特性，因为这样的代码几乎从不如愿地运行。
class BST {
 public:
  BST() { a = 1; }
  friend std::ostream &operator<<(std::ostream &out, const BST &obj);
  int &operator[](const size_t i) { return a; }

 private:
  int a;
};
std::ostream &operator<<(std::ostream &out, const BST &obj) {
  out << obj.a << " ";
  return out;
}
class BalancedBST : public BST {
 public:
  BalancedBST() { b = 2; }

 private:
  int b;
};
// 编译器将会毫无警告地编译这个函数：
void printBSTArray(std::ostream &s, const BST array[], int numElements) {
  for (int i = 0; i < numElements; ++i) {
    // 这里的array[i]只是一个指针算法的缩写：它所代表的是*(array + i)
    // array是一个指向数组起始地址的指针，
    // 但是array中各元素内存地址与数组的起始地址的间隔究竟有多大呢？
    // 它们的间隔是i*sizeof(一个在数组里的对象)，
    // 因为在array数组[0]到[i]间有i个对象。
    // 编译器为了建立正确遍历数组的执行代码，
    // 它必须能够确定数组中对象的大小，这对编译器来说是很容易做到的。
    // 参数array被声明为BST类型，所以array数组中每一个元素都是BST类型，
    // 因此每个元素与数组起始地址的间隔是i*sizeof(BST)。
    s << array[i];
  }
  s << std::endl;
}

void func() {
  BST BSTArray[10];
  printBSTArray(std::cout, BSTArray, 10);  // 1 1 1 1 1 1 1 1 1 1
  BalancedBST bBSTArray[10];
  printBSTArray(std::cout, bBSTArray, 10);  // 1 2 1 2 1 2 1 2 1 2
  // 把一个BalancedBST对象数组传给printBSTArray，编译器就会犯错误。
  // 编译器原先已经假设数组中元素与BST对象的大小一致，
  // 但是现在数组中每一个对象大小却与BalancedBST一致，派生类通常都比基类要长。
}
}  // namespace n1

namespace n2 {
// 如果试图删除一个含有派生类对象的数组，将会发生各种各样的问题。
// 以下是一种可能采用的但不正确的做法。
// 删除一个数组, 但是首先记录一个删除信息。
class BST {
 public:
  ~BST() { std::cout << "~BST" << std::endl; }
};
class BalancedBST : public BST {
 public:
  ~BalancedBST() { std::cout << "~BalancedBST" << std::endl; }
};
void deleteArray(BST array[]) { delete[] array; }
// 语言规范中说通过一个基类指针来删除一个含有派生类对象的数组，结果将是不确定的。
// 这实际意味着执行这样的代码肯定不会有什么好结果。
// 多态和指针算法不能混合在一起来用，所以数组与多态也不能用在一起。

void func() {
  BalancedBST *a1 = new BalancedBST[4];
  deleteArray(a1);
  // ~BST
  // ~BST
  // ~BST
  // ~BST

  BalancedBST *a2 = new BalancedBST[4];
  delete[] a2;
  // ~BalancedBST
  // ~BST
  // ~BalancedBST
  // ~BST
  // ~BalancedBST
  // ~BST
  // ~BalancedBST
  // ~BST
}

// 如果不从一个具体类（例如BST）派生出另一个具体类（例如BalancedBST），
// 那么就不太可能犯这种使用多态性数组的错误。
}  // namespace n2

int main(int argc, char *argv[]) {
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