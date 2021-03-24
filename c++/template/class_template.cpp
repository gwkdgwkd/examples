#include <cstring>
#include <iostream>

using namespace std;

// C++除了支持函数模板，还支持类模板（Class Template）。
// 函数模板中定义的类型参数可以用在函数声明和函数定义中，类模板中定义的类型参数可以用在类声明和类实现中。类模板的目的同样是将数据的类型参数化。
// 声明类模板的语法为：
// template<typename 类型参数1 , typename 类型参数2 , …> class 类名{
// };
// 类模板和函数模板都是以template开头（当然也可以使用class，目前来讲它们没有任何区别），后跟类型参数；类型参数不能为空，多个类型参数用逗号隔开。
// 一但声明了类模板，就可以将类型参数用于类的成员函数和成员变量了。换句话说，原来使用int、float、char等内置类型的地方，都可以用类型参数来代替。
// 模板头和类头是一个整体，可以换行，但是中间不能有分号。

template <typename T1, typename T2>  // 这里不能有分号
class Point {
 public:
  Point(T1 x, T2 y) : m_x(x), m_y(y) {}

 public:
  T1 getX() const;
  void setX(T1 x);
  T2 getY() const;
  void setY(T2 y);

 private:
  T1 m_x;
  T2 m_y;
};
// 还需要在类外定义成员函数。在类外定义成员函数时仍然需要带上模板头，格式为：
// template <typename 类型参数1, typename 类型参数2, …>
// 返回值类型 类名<类型参数1, 类型参数2, ...>::函数名(形参列表) {
// }
// 第一行是模板头，第二行是函数头，它们可以合并到一行，不过为了让代码格式更加清晰，一般是将它们分成两行。
template <typename T1, typename T2>  // 模板头
T1 Point<T1, T2>::getX() const {     // 函数头
  return m_x;
}
template <typename T1, typename T2>
void Point<T1, T2>::setX(T1 x) {
  m_x = x;
}
// 在类外定义成员函数时，template后面的类型参数要和类声明时的一致。
template <typename T1, typename T2>
T2 Point<T1, T2>::getY() const {
  return m_y;
}
// 除了template关键字后面要指明类型参数，类名Point后面也要带上类型参数，只是不加typename关键字了。
template <typename T1, typename T2>
void Point<T1, T2>::setY(T2 y) {
  m_y = y;
}

// 用类模板实现可变长数组
template <class T>
class CArray {
  int size;  // 数组元素的个数
  T *ptr;    // 指向动态分配的数组
 public:
  CArray(int s = 0);  // s代表数组元素的个数
  CArray(CArray &a);
  ~CArray();
  void push_back(const T &v);  // 用于在数组尾部添加一个元素v
  CArray &operator=(const CArray &a);  // 用于数组对象间的赋值
  T length() { return size; }
  T &operator[](
      int i) {  // 用以支持根据下标访问数组元素，如a[i] = 4;和n = a[i]这样的语句
    return ptr[i];
  }
};
template <class T>
CArray<T>::CArray(int s) : size(s) {
  if (s == 0)
    ptr = NULL;
  else
    ptr = new T[s];
}
template <class T>
CArray<T>::CArray(CArray &a) {
  if (!a.ptr) {
    ptr = NULL;
    size = 0;
    return;
  }
  ptr = new T[a.size];
  memcpy(ptr, a.ptr, sizeof(T) * a.size);
  size = a.size;
}
template <class T>
CArray<T>::~CArray() {
  if (ptr) delete[] ptr;
}
template <class T>
CArray<T> &CArray<T>::operator=(
    const CArray &
        a) {       // 赋值号的作用是使"="左边对象里存放的数组，大小和内容都和右边的对象一样
  if (this == &a)  // 防止a=a这样的赋值导致出错
    return *this;
  if (a.ptr == NULL) {  // 如果a里面的数组是空的
    if (ptr) delete[] ptr;
    ptr = NULL;
    size = 0;
    return *this;
  }
  if (size < a.size) {  // 如果原有空间够大，就不用分配新的空间
    if (ptr) delete[] ptr;
    ptr = new T[a.size];
  }
  memcpy(ptr, a.ptr, sizeof(T) * a.size);
  size = a.size;
  return *this;
}
template <class T>
void CArray<T>::push_back(const T &v) {  // 在数组尾部添加一个元素
  if (ptr) {
    T *tmpPtr = new T[size + 1];            // 重新分配空间
    memcpy(tmpPtr, ptr, sizeof(T) * size);  // 拷贝原数组内容
    delete[] ptr;
    ptr = tmpPtr;
  } else  // 数组本来是空的
    ptr = new T[1];
  ptr[size++] = v;  // 加入新的数组元素
}

int main() {
  // 使用类模板创建对象时，需要指明具体的数据类型
  // 与函数模板不同的是，类模板在实例化时必须显式地指明数据类型，编译器不能根据给定的数据推演出数据类型。
  Point<int, int> p1(10, 20);
  cout << "x=" << p1.getX() << ", y=" << p1.getY() << endl;
  Point<int, float> p2(10, 15.5);
  cout << "x=" << p2.getX() << ", y=" << p2.getY() << endl;
  Point<float, char *> p3(12.4, "东经180度");
  cout << "x=" << p3.getX() << ", y=" << p3.getY() << endl;

  // 除了对象变量，也可以使用对象指针的方式来实例化：
  Point<float, float> *p4 = new Point<float, float>(10.6, 109.3);
  Point<char *, char *> *p5 =
      new Point<char *, char *>("东经180度", "北纬210度");
  cout << "x=" << p5->getX() << ", y=" << p5->getY() << endl;
  // 赋值号两边都要指明具体的数据类型，且要保持一致。下面的写法是错误的：
  // Point<float, float> *p = new Point<float, int>(10.6, 109);
  // Point<float, float> *p = new Point(10.6, 109);

  CArray<int> a;
  for (int i = 0; i < 5; ++i) a.push_back(i);
  for (int i = 0; i < a.length(); ++i) cout << a[i] << " ";
  cout << endl;  // 0 1 2 3 4

  return 0;
}