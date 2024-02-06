#include <iostream>

#include "concrete_subject.h"
#include "proxy.h"

// Proxy 代理模式

// 动机（Motivation）：
// 1.在面向对象系统中，由于某种原因，对象间访问会给使用者、或系统结构带来很多麻烦，
//   比如对象创建的开销很大，或者某些操作需要安全控制，或者需要进程外的访问等；
// 2.如何在不失去透明操作对象的同时来管理/控制这些对象特有的复杂性？
//   增加一层间接层是软件开发中常见的解决方式。

// 定义：
// 为其他对象提供一种代理以控制（隔离、使用接口）对这个对象的访问。

// 要点总结：
// 1.增加一层间接层是软件系统中对许多复杂对象的一种常见解决方法，
//   在面向对象系统中，直接使用某些对象会带来很多问题，
//   作为间接层的proxy对象便是解决这一问题的常用手段；
// 2.具体proxy设计模式的实现方法、实现粒度都相差很大，
//   有些可能对单个对象做细粒度的控制，如copy-on-write技术，
//   有些可能对组件模块提供抽象代理层，在架构层次对对象做proxy；
// 3.Proxy并不一定要求保持接口完整的一致性，只要能够实现间接控制，
//   有时候损失一些透明性是可以接受的。

// 至少在以下集中情况下可以用Proxy模式解决问题：
// 1.创建开销大的对象时候，比如显示一幅大的图片，将这个创建的过程交给代理去完成，
//   GoF称之为虚代理（Virtual Proxy）；
// 2.为网络上的对象创建一个局部的本地代理，比如要操作一个网络上的一个对象，
//   网络性能不好的时候，问题尤其突出，将这个操纵的过程交给一个代理去完成，
//   GoF称之为远程代理（Remote Proxy）；
// 3.对对象进行控制访问的时候，比如在论坛中不同权限的用户将获得不同层次的操作权限，
//   将这个工作交给一个代理去完成，GoF称之为保护代理（Protection Proxy）；
// 4.智能指针（Smart Pointer）。

// Proxy模式最大的好处就是实现了逻辑和实现的彻底解耦。

int main() {
  Subject* sub = new ConcreteSubject();
  Proxy* p = new Proxy(sub);
  p->Request();
  // Proxy::Request
  // ConcreteSubject::Request

  return 0;
}