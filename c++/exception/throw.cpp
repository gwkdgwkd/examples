#include <iostream>

namespace n1 {
// 异常必须显式地抛出，才能被检测和捕获到；如果没有显式的抛出，即使有异常也检测不到。
// 在C++中，我们使用throw关键字来显式地抛出异常，它的用法为：throw exceptionData;
// exceptionData可以包含任意的信息，完全有程序员决定。
// exceptionData可以是int、float、bool等基本类型，也可以是指针、数组、字符串、结构体、类等聚合类型。
class OutOfRange {
 public:
  OutOfRange() : m_flag(1){};
  OutOfRange(int len, int index) : m_len(len), m_index(index), m_flag(2) {}

 public:
  void what() const;  // 获取具体的错误信息
 private:
  int m_flag;   // 不同的flag表示不同的错误
  int m_len;    // 当前数组的长度
  int m_index;  // 当前使用的数组下标
};
void OutOfRange::what() const {
  if (m_flag == 1) {
    std::cout << "Error: empty array, no elements to pop." << std::endl;
  } else if (m_flag == 2) {
    std::cout << "Error: out of range( array length " << m_len
              << ", access index " << m_index << " )" << std::endl;
  } else {
    std::cout << "Unknown exception." << std::endl;
  }
}
class Array {
 public:
  Array();
  ~Array() { free(m_p); };

 public:
  int operator[](int i) const;           // 获取数组元素
  int push(int ele);                     // 在末尾插入数组元素
  int pop();                             // 在末尾删除数组元素
  int length() const { return m_len; };  // 获取数组长度

 private:
  int m_len;       // 数组长度
  int m_capacity;  // 当前的内存能容纳多少个元素
  int *m_p;        // 内存指针
 private:
  static const int m_stepSize = 50;  // 每次扩容的步长
};
Array::Array() {
  m_p = (int *)malloc(sizeof(int) * m_stepSize);
  m_capacity = m_stepSize;
  m_len = 0;
}
int Array::operator[](int index) const {
  if (index < 0 || index >= m_len) {  // 判断是否越界
    throw OutOfRange(m_len, index);  // 抛出异常（创建一个匿名对象）
  }
  return *(m_p + index);
}
int Array::push(int ele) {
  if (m_len >= m_capacity) {  // 如果容量不足就扩容
    m_capacity += m_stepSize;
    m_p = (int *)realloc(m_p, sizeof(int) * m_capacity);  // 扩容
  }
  *(m_p + m_len) = ele;
  m_len++;
  return m_len - 1;
}
int Array::pop() {
  if (m_len == 0) {
    throw OutOfRange();  // 抛出异常（创建一个匿名对象）
  }
  m_len--;
  return *(m_p + m_len);
}
void printArray(Array &arr) {
  int len = arr.length();
  // 判断数组是否为空
  if (len == 0) {
    std::cout << "Empty array! No elements to print." << std::endl;
    return;
  }
  for (int i = 0; i < len; i++) {
    if (i == len - 1) {
      std::cout << arr[i] << std::endl;
    } else {
      std::cout << arr[i] << ", ";
    }
  }
}

void testN1() {
  Array nums;
  for (int i = 0; i < 10; i++) {
    nums.push(i);
  }
  printArray(nums);  // 0, 1, 2, 3, 4, 5, 6, 7, 8, 9
  try {
    std::cout << nums[20] << std::endl;
  } catch (OutOfRange &e) {
    e.what();
    // Error: out of range( array length 10, access index 20 )
  }
  try {
    for (int i = 0; i < 20; i++) {
      nums.pop();
    }
  } catch (OutOfRange &e) {
    e.what();
    // Error: empty array, no elements to pop.
  }
  printArray(nums);  // Empty array! No elements to print.
}
}  // namespace n1

namespace n2 {
// throw用作异常规范:
// throw关键字除了可以用在函数体中抛出异常，
// 还可以用在函数头和函数体之间，指明当前函数能够抛出的异常类型，
// 这称为异常规范（Exception specification），有些教程也称为异常指示符或异常列表，
// 如果抛出异常列表中没有的异常，try将无法捕获，只能终止程序。

void func1(bool b) throw(int) {  // 只能抛出int类型的异常
  if (b) {
    throw 8;
  } else {
    throw std::exception();
  }
}
void func2(int i) throw(int, char, std::exception) {  // 抛出多种异常用逗号隔开
  if (0 == i) {
    throw 7;
  } else if (1 == i) {
    throw 'a';
  } else if (2 == i) {
    throw std::exception();
  } else {
    throw 2.34;
  }
}
void func3(bool b) throw() {  // 不会抛出任何异常，即使抛出了，try也检测不到
  if (b) {
    throw 2;
  }
}

// C++规定，派生类虚函数的异常规范必须与基类虚函数的异常规范一样严格，或者更严格。
// 只有这样，当通过基类指针（或者引用）调用派生类虚函数时，才能保证不违背基类成员函数的异常规范。
class Base {
 public:
  virtual int f1(int) throw();
  virtual int f2(int) throw(int);
  virtual std::string f3() throw(int, std::string);
};
class Derived : public Base {
 public:
  // int f1(int) throw(int);  // 错！异常规范不如throw()严格
  int f2(int) throw(int);               // 对！有相同的异常规范
  std::string f3() throw(std::string);  // 对！比throw(int,string)更严格
};

// C++规定，异常规范在函数声明和函数定义中必须同时指明，并且要严格保持一致，不能更加严格或者更加宽松。
// 错！定义中有异常规范，声明中没有:
// void func1();
// void func1() throw(int) {}
// 错！定义和声明中的异常规范不一致:
// void func2() throw(int);
// void func2() throw(int, bool) {}
// 对！定义和声明中的异常规范严格一致:
void func4() throw(float, char *);
void func4() throw(float, char *) {}

// 异常规范的初衷是好的，它希望让程序员看到函数的定义或声明后，立马就知道该函数会抛出什么类型的异常，
// 这样程序员就可以使用try-catch来捕获了。
// 如果没有异常规范，程序员必须阅读函数源码才能知道函数会抛出什么异常。
// 不过这有时候也不容易做到，例如：
// 1.函数可能不会引发异常，但它调用了另外一个函数()，另外的函数可能会引发异常。
// 2.编写的函数调用了老式的库函数，此时不会引发异常，但是库更新以后这个函数却引发了异常。
// 总之，异常规范的初衷实现起来有点困难，所以大家达成的一致意见是，最好不要使用异常规范。
// 异常规范是C++98新增的一项功能，但是后来的C++11已经将它抛弃了，不再建议使用。
// 另外，各个编译器对异常规范的支持也不一样，比如，抛出异常规范中没有的异常：
// 1.在GCC下，try-catch将捕获不到异常，只能交给系统处理，终止程序。
// 2.在Visual C++中使用异常规范虽然没有语法错误，但是也没有任何效果，
//   会直接忽略异常规范的限制，函数可以抛出任何类型的异常。

void testN2() {
  try {
    func1(true);

    // func1(false);
    // terminate called after throwing an instance of 'std::exception'
  } catch (int &i) {
    std::cout << "Exception type: int " << i << std::endl;
  }
  // Exception type: int 8

  try {
    // func1(false);
  } catch (int &i) {
    std::cout << "Exception type: int " << i << std::endl;
  }
  // terminate called after throwing an instance of 'std::exception'

  try {
    func2(0);
  } catch (int &i) {
    std::cout << "Exception type: int " << i << std::endl;
  }
  // Exception type: int 7

  try {
    func2(1);
  } catch (char &c) {
    std::cout << "Exception type: char " << c << std::endl;
  }
  // Exception type: char a

  try {
    func2(2);
  } catch (std::exception &e) {
    std::cout << "Exception type: exception " << e.what() << std::endl;
  }
  // Exception type: exception std::exception

  try {
    // func2(3);
  } catch (std::exception &e) {
    std::cout << "Exception type: exception " << e.what() << std::endl;
  }
  // terminate called after throwing an instance of 'double'

  try {
    func3(false);
  } catch (std::exception &e) {
    std::cout << "Exception type: exception " << e.what() << std::endl;
  }

  try {
    // func3(true);
  } catch (std::exception &e) {
    std::cout << "Exception type: exception " << e.what() << std::endl;
  }
  // terminate called after throwing an instance of 'int'
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
      n1::testN1();
      break;
    case 1:
      n2::testN2();
      break;
    default:
      std::cout << "invalid type" << std::endl;
      break;
  }

  return 0;
}