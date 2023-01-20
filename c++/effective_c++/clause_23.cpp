#include <iostream>

using namespace std;

// 以non-member、non-friend函数替换member函数

class WebBrowser {
 public:
  void clearCache() {}
  void clearHistory() {}
  void removeCookies() {}
  void clearEverything() {
    clearCache();
    clearHistory();
    removeCookies();
  };
};
// 在C++中，比较自然的做法是让clearEverything成为一个non-member函数，
// 并且位于WebBrowser同一个命名空间内（便于客户轻松扩展）：
void clearEverything(WebBrowser &wb) {
  wb.clearCache();
  wb.clearHistory();
  wb.removeCookies();
};

// non-member的clearEverything和member的clearEverything哪个好呢？
// 面向对象守则要求，数据以及数据的操作应该被捆绑在一起，
// 意味着建议member函数是好的。
// 不幸这个建议不正确，这是对面向对象真实意义的一个误解。
// 面向对象要求数据应该尽可能被封装，然而与直观相反，
// member的clearEverything带来的封装性比non-member函数低。
// 此外non-member的clearEverything函数，
// 可允许对WebBrowser相关机能有较大的包裹弹性，
// 而那最终导致较低的编译相依度，增加WebBrowser的可延伸性。

// 许多方面non-member做法比member做法好：
// 某东西被封装，就不在可见。
// 越多东西被封装，越少人可以看到它，越少人看到它，
// 就有越大的弹性去改变它，因为改变仅仅直接影响能看到的那些人。
// 因此，越多东西被封装，改变东西的能力也越大。
// 这就是推崇封装的原因，它能够改变事物而只影响有限客户。

// 作为一种粗超的测量方法，越多函数可以访问对象内部的数据，数据的封装性就越低。
// non-member和member提供相同功能时，
// 导致较大封装性的是non-member和non-friend函数，
// 因为它并不增加能够访问class内private成分的函数数量。

// 请记住：
// 1.宁可拿non-member、non-friend函数替换member函数，
//   这样做可以增加封装性、包裹性和机能扩充性。

int main() { return 0; }