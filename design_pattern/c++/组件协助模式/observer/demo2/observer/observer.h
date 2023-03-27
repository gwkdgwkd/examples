#ifndef OBSERVER_H
#define OBSERVER_H

// 观察者为那些在目标发生改变时需获得通知的对象定义一个更新接口。
class Observer {
 public:
  virtual void Update(int) = 0;
};
#endif