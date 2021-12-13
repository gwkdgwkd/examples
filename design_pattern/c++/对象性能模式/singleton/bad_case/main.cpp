#include <dlfcn.h>
#include <iostream>
#include "singleton.h"
int main() {
  using namespace std;

  // call singleton firstly
  singleton::instance().num = 100;
  cout << "singleton.num in main : " << singleton::instance().num << endl;

  // open the library
  void* handle = dlopen("./hello.so", RTLD_LAZY);
  if (!handle) {
    cerr << "Cannot open library: " << dlerror() << '\n';
    return 1;
  }

  // load the symbol
  typedef void (*hello_t)();

  // reset errors
  dlerror();
  hello_t hello = (hello_t)dlsym(handle, "hello");
  const char* dlsym_error = dlerror();
  if (dlsym_error) {
    cerr << "Cannot load symbol 'hello': " << dlsym_error << '\n';
    dlclose(handle);
    return 1;
  }

  hello();  // call method in the plugin
            // call singleton secondly
  cout << "singleton.num in main : " << singleton::instance().num << endl;
  dlclose(handle);
}

// 动态库之间单例模式出现多个实例

// 可能，认为会输出这些内容：
// singleton.num in main : 100
// singleton.num in hello.so : 100
// singleton.num in hello.so after ++ : 101
// singleton.num in main : 101
// 其实：
// singleton.num in main : 100
// singleton.num in hello.so : -1
// singleton.num in hello.so after ++ : 0
// singleton.num in main : 100
// 从输出内容中，可以看出singleton出现了两个实例，一个实例在main函数中，另一个实例在插件hello.so中。

// 原因分析:
// 究其原因，是由于插件的动态链接引起的。hello.so在动态链接过程中，没有发现example1中有singleton::instance这个实例（但是事实上，该实例已经存在了），
// 那么就会自己创建一个实例（正如单例的逻辑实现），这样就导致了两个实例。
// 通过工具nm查看example1的符号表（symboltable），看看其中是否包含singleton::instance符号（dynamic）
// nm -C example1 | grep singleton
// 0000000000400c6e t _GLOBAL__sub_I__ZN9singleton9pInstanceE
// 0000000000400c99 W singleton::instance()
// 00000000006022d8 B singleton::pInstance
// 0000000000400c84 W singleton::singleton()
// 0000000000400c84 W singleton::singleton()
// nm -C -D example1 | grep singleton
// D选项用于查看动态符号（dynamicsymbol），你会发现singleton::pInstance在静态表中存在，在动态表中不存在，这也就是说，动态连接器（dynamiclinker）在加
// 载hello.so的时候，无法找到singleton::pInstance的唯一实例，只能构造一个新的实例，该实例在hello.so中是唯一的，但是不能保证在example1和hello.so中唯一。

// 解决方法:
// 解决问题的关键在于如何暴露example1中的singleton::pInstance。好在，GNUmake有一个链接选项-rdynamic，可以帮我们解决这个问题。
// nm -C example1 | grep singleton
// 0000000000400f3e t _GLOBAL__sub_I__ZN9singleton9pInstanceE
// 0000000000400f69 W singleton::instance()
// 00000000006022d8 B singleton::pInstance
// 0000000000400f54 W singleton::singleton()
// 0000000000400f54 W singleton::singleton()
// nm -C -D example1 | grep singleton
// 0000000000400f69 W singleton::instance()
// 00000000006022d8 B singleton::pInstance
// 0000000000400f54 W singleton::singleton()
// 0000000000400f54 W singleton::singleton()
// 静态符号没有什么变化，但是动态符号却显示了更多的内容，其中包括我们想要的singleton::pInstance，也就是singleton的唯一实例。
// singleton.num in main : 100
// singleton.num in hello.so : 100
// singleton.num in hello.so after ++ : 101
// singleton.num in main : 101
// 此结果表明singleton在example1和hello.so之间只产生了一个实例。