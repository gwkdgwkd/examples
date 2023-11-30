#include <deque>
#include <iostream>
#include <list>
#include <set>
#include <string>
#include <vector>

// 了解如何把vector和string数据传给旧的API

// 自从C++在1998年被标准化以来，C++精英们就试图使程序员们从数组中解放出来，
// 转用vector并从char*指针转向string对象，旧的C API还存在，
// 使用char*指针和数组进行数据的交换，需要转换。

namespace n1 {
void print(const int* pInts, size_t numInts) {
  for (int i = 0; i < numInts; ++i) {
    std::cout << *(pInts + i) << ' ';
  }
  std::cout << std::endl;
}
void func1() {
  std::vector<int> v{1, 2, 3};

  // v可能为空，&v[0]则试图产生一个指向的东西并不存在的指针：
  if (!v.empty()) {
    // &v[0]指向第一个元素的指针，vector中的元素是连续存储的，就像数组一样：
    print(&v[0], v.size());  // 1 2 3

    // 不能使用begin，begin返回的是迭代器不是指针，&*v.begin()也可以：
    print(&*v.begin(), v.size());  // 1 2 3
  }
}

// &v[0]的方法对于vector是适用的，但对于string却是不可靠的，因为：
// 1.string中的数据不一定存储在连续的内存中；
// 2.string的内部表示不一定是以空字符结尾的。
// string中有c_str，该函数返回指向字符串的值的指针，而且该指针可用于C：
void print(const char* pString) { std::cout << pString << std::endl; }

// 上面两种情况，要传入的都是指向const的指针，被用来读取，而不是修改数据的API，
// 到此为止，这是最安全的方式，对于string，这是唯一能做的，
// 因为c_str所产生的指针并不一定指向字符串数据的内部表示；
// 它返回的指针可能是指向字符串数据的一个不可修改的拷贝，
// 该拷贝已经被做了适当的格式化，以满足C API的要求。

void func2() {
  std::string s = "Hello,World";
  print(s.c_str());
}

void func3() {
  std::vector<int> v{6, 5, 5};
  // 实际上，先让C API把数据写入到vector中，
  // 然后把数据拷贝到期望最终写入的STL容器中，这一思想总是可行的：
  std::deque<int> d(v.begin(), v.end());
  std::list<int> l(v.begin(), v.end());
  std::set<int> s(v.begin(), v.end());

  // 而且这意味着：
  // 除了vecor和string以外的其他STL容器也能把它们的数据传递给C API。
  // 只需把每个容器的元素拷贝到一个vector中，然后传给该API：
  std::set<int> intSet;
  std::vector<int> vi(intSet.begin(), intSet.end());
  if (!vi.empty()) {
    print(&vi[0], vi.size());
  }
  // 也可以把数据拷贝到一个数组中，然后把该数组传给C API，但为什么这样做呢？
  // 除非在编译时知道容器的大小，否则即要动态地分配数组。
}

void func() {
  func1();
  func2();
  func3();
}
}  // namespace n1

namespace n2 {
// vector更灵活，通常是可以修改元素的值，但不能试图改变元素的个数。
// 比如，不能试图在vector的未使用的容量中创建新元素，不然内部将会变得不一致，
// size就会产生不正确的结果，如果容量和大小相等时，真正可怕的事情就会发生。
// 有些vector对数据有额外的限制，C API改变数据时，
// 也要保证这些额外的限制还能被满足，比如要保持有序。

// 想用C API中的元素初始化vector，那么可以利用vector和数组的内容布局兼容性，
// 想API传入该矢量中元素的存储区域：
size_t fillArray(double* pArray, size_t arraySize) {
  for (int i = 0; i < arraySize; ++i) {
    pArray[i] = i + 0.1;
  }
  return arraySize;
}
void func1() {
  std::vector<double> vd(5);
  vd.resize(fillArray(&vd[0], vd.size()));
  for (int i = 0; i < vd.size(); ++i) {
    std::cout << vd[i] << ",";
  }
  std::cout << std::endl;  // 0.1,1.1,2.1,3.1,4.1,
}

// 上面的技术只对vector有效，因为只有vector才保证和数组有同样的内存布局，
// 如果想用来自C API中的数据初始化一个sting，也很容易就能做到。
size_t fillString(char* pArray, size_t arraySize) {
  for (int i = 0; i < arraySize; ++i) {
    pArray[i] = 'a' + i;
  }
  return arraySize;
}
void func2() {
  std::vector<char> vc(5);
  size_t charsWritten = fillString(&vc[0], vc.size());

  std::string s(vc.begin(), vc.begin() + charsWritten);
  std::cout << s << std::endl;  // abcde
}

void func() {
  func1();
  func2();
}
}  // namespace n2

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