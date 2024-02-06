#ifndef OBSERVER_H
#define OBSERVER_H

class Observer {
 public:
  // 观察者为那些在目标发生改变时需获得通知的对象定义一个更新接口：
  virtual void Update(int) = 0;
};
#endif