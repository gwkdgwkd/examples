#include <iostream>

using namespace std;

// 编写new和delete时需固守常规

// 实现operator new必须返回正确的值，
// 内存不足时必须调用new-handing函数，
// 必须有对付零内存需求的准备，还需避免不慎掩盖正常形式的new。

// operator new的返回值十分单纯。
// 如果有能力提供内存，就返回指针指向那块内存。
// 如果没有能力，就遵循条款49的规则，并抛出bad_alloc异常。
// 然后其实也不是非常单纯，
// 因为operator new实际上不只一次尝试分配内存，
// 并在每次失败后调用new-handling函数。
// 假设new-handling函数也许能够做某些动作将某些内存释放出来。
// 只有当指向new-handling函数的指针是null，
// operator new才会抛出异常。

// C++规定，即使客户要求0 byte，operator new也得返回一个合法指针。
// void *operator new(size_t size) throw(bad_alloc) {
//   using namespace std;
//   if (size == 0) {
//     size = 1;
//   }
//   while(true) {
//     // 尝试分配
//     if(分配成功)
//       return 一个指针，指向分配的内存
//     else {// 分配失败，没有办法直接取得new-handing函数指针：
//       new_handler globalHandler = set_new_handler(0);
//       set_new_handler(globalHandler);
//       if(globalHandler) 
//         (*globalHandler)();
//       else 
//         throw bad_alloc();
//     }
//   }
// }

// 针对class设计的operator new，
// 行为很典型地只为大小刚好sizeof(X)的对象设计。
// 一旦被继承下去，
// 有可能base class的operator new被调用用以分配derived class对象。
class Base {
 public:
  static void* operator new(size_t size) throw(bad_alloc) {
    // 如果大小错误，令标准的operator new处理这个size，
    // 包括size是0的情况：
    if (size != sizeof(Base)) return ::operator new(size);
  }
  static void operator delete(void* rawMemory, size_t size) throw();
};
class Derived : public Base {};
// 不能在Base::operator new[]内假设，
// array的每个元素对象的大小是sizeof(Base)，
// 这意味着不能假设array元素对象个数为：byte申请数/sizeof(Base)。
// 此外，传递给operator new[]的size_t参数，
// 其值可能比将被填充以对象的内存数量更多。
// 因为动态分配的array可能包含额外空间来存放元素个数。

// operator delete情况更简单，
// 需要记住的唯一事情就是C++保证删除null指针永远安全，
// 所以必须实现这项保证：
void operator delete(void* rawMemory) throw() {
  if (rawMemory == 0) return;
  // ...
}
// 这个函数的member版本也很简单，只需要多加一个动作检查删除数量：
void Base::operator delete(void* rawMemory, size_t size) throw() {
  if (rawMemory == 0) return;
  if (size != sizeof(Base)) {
    ::operator delete(rawMemory);
    return;
  }
  // 归还rawMemory所指的内存
  return;
}

// 如果即将被删除的对象派生自某个base class，
// 而后者欠缺virtual析构函数，
// 那么C++传给operator delete的size数值可能不正确。
// 这是让你的base class拥有virtual析构函数的一个够好的理由。
// 换句话说，如果base class遗漏virtual析构函数，
// operator delete可能无法正确运作。

// 请记住：
// 1.operator new应该内含一个无穷循环，并在其中尝试分配内存，
//   如果它无法满足内存需求，就应该调用new_handler。
//   它也应该有能力处理0 byte申请。
// 2.class专属版本则还应处理比正确大小更大的（错误）申请。
// 3.operator delete应该在收到null指针时不做任何事情。

int main() {
  // 调用的是Base::operator new：
  Derived* p = new Derived;

  return 0;
}