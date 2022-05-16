#include <iostream>

using namespace std;

// 不要对数组使用多态

// 类继承的最重要的特性是你可以通过基类指针或引用来操作派生类。
// 这样的指针或引用具有行为的多态性，就好像它们同时具有多种形态。
// C++允许你通过基类指针和引用来操作派生类数组。
// 不过这根本就不是一个特性，因为这样的代码几乎从不如你所愿地那样运行。
class BST {
 public:
  BST() { a = 1; }
  friend ostream &operator<<(ostream &out, const BST &obj);
  int &operator[](const size_t i) { return a; }

 private:
  int a;
};
ostream &operator<<(ostream &out, const BST &obj) {
  out << obj.a << " ";
  return out;
}
class BalancedBST : public BST {
 public:
  BalancedBST() { b = 2; }

 private:
  int b;
};
// 编译器将会毫无警告地编译这个函数
void printBSTArray(ostream &s, const BST array[], int numElements) {
  for (int i = 0; i < numElements; ++i) {
    // 这里的array[i]只是一个指针算法的缩写：它所代表的是*(array + i)
    // array是一个指向数组起始地址的指针，但是array中各元素内存地址与数组的起始地址的间隔究竟有多大呢？
    // 它们的间隔是i*sizeof(一个在数组里的对象)，因为在array数组[0]到[i]间有i个对象。
    // 编译器为了建立正确遍历数组的执行代码，它必须能够确定数组中对象的大小，这对编译器来说是很容易做到的。
    // 参数array被声明为BST类型，所以array数组中每一个元素都是BST类型，
    // 因此每个元素与数组起始地址的间隔是i*sizeof(BST)。
    s << array[i];
  }
  s << endl;
}

// 如果你试图删除一个含有派生类对象的数组，将会发生各种各样的问题。
// 以下是一种你可能采用的但不正确的做法。
// 删除一个数组, 但是首先记录一个删除信息。
class BST1 {
 public:
  ~BST1() { cout << "~BST1" << endl; }
};
class BalancedBST1 : public BST1 {
 public:
  ~BalancedBST1() { cout << "~BalancedBST1" << endl; }
};
void deleteArray(ostream &logStream, BST1 array[]) {
  logStream << "Deleting array at address " << static_cast<void *>(array)
            << '\n';
  delete[] array;
}
// 语言规范中说通过一个基类指针来删除一个含有派生类对象的数组，结果将是不确定的。
// 这实际意味着执行这样的代码肯定不会有什么好结果。
// 多态和指针算法不能混合在一起来用，所以数组与多态也不能用在一起。

// 如果不从一个具体类（例如 BST）派生出另一个具体类（例如BalancedBST），
// 那么你就不太可能犯这种使用多态性数组的错误。

int main() {
  BST BSTArray[10];
  printBSTArray(cout, BSTArray, 10);  // 1 1 1 1 1 1 1 1 1 1
  BalancedBST bBSTArray[10];
  printBSTArray(cout, bBSTArray, 10);  // 1 2 1 2 1 2 1 2 1 2
  // 把一个含有BalancedBST对象的数组变量传递给printBSTArray函数，你的编译器就会犯错误。
  // 编译器原先已经假设数组中元素与BST对象的大小一致，但是现在数组中每一个对象大小却与BalancedBST一致。
  // 派生类的长度通常都比基类要长。

  BalancedBST1 *balTreeArray = new BalancedBST1[4];
  deleteArray(cout, balTreeArray);
  // Deleting array at address 0x12e8038
  // ~BST1
  // ~BST1
  // ~BST1
  // ~BST1

  return 0;
}