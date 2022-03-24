#include <iostream>

using namespace std;

// 在资源管理类中小心copying行为

typedef int Mutex;
void lock(Mutex* pm){};
void unlock(Mutex* pm){};
class Lock {
 public:
  explicit Lock(Mutex* pm) : mutexPtr(pm) { lock(mutexPtr); }
  ~Lock() { unlock(mutexPtr); }

 private:
  Mutex* mutexPtr;
};

Mutex m;

void f() {
  Lock m1(&m);  // 加锁
  // 执行临界区的内的操作
}  // 自动解锁

// RAII class作者一定要面对的：当一个RAII对象被复制，会发生什么事？大多数会选择以下两种：
// 1.禁止复制：许多时候允许RAII对象被复制并不合理。
//   class Lock: private Uncopyable {};
// 2.对底层资源使用“引用计数法”：保有资源直到最后一个使用者被销毁。
// 3.复制底部资源：对资源进行复制，也就是进行“深度拷贝”。
// 4.转移底部资源的拥有权：某些罕见场合下希望确保永远只有一个RAII对象指向一个未加工资源，即使复制也依然如此。

// copying函数（包括copy构造函数和copy assignment操作符）有可能被编译器自动创建出来，
// 除非编译做了你想要的事，否则就需要自己编写。

// 请记住：
// 复制RAII对象必须一并复制它所管理的资源，所以资源的copying行为决定RAII对象的copy行为。
// 普遍而常见的RAII class copying行为是：抑制copying、实施引用计数法。不过其他行为也都可能被实现。

int main() {
  Lock m11(&m);
  Lock m12(m11);  // 将m11复制到m12，这会发生什么事？

  return 0;
}