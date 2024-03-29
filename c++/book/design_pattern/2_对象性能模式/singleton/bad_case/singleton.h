#ifndef SINGLETON_H_
#define SINGLETON_H_
class singleton {
 private:
  singleton() { num = -1; }
  static singleton* pInstance;

 public:
  static singleton& instance() {
    if (nullptr == pInstance) {
      pInstance = new singleton();
    }
    return *pInstance;
  }

 public:
  int num;
};
singleton* singleton::pInstance = nullptr;
#endif