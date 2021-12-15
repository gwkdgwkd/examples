class Memento {
  string state;  // 与Originator中数据一样，或者为一部分
                 // ...
 public:
  Memento(const string& s) : state(s) {}
  string getState() const { return state; }
  void setState(const string& s) { state = s; }
};

class Originator {
  string state;
  // ...
 public:
  Originator() {}
  Memento createMomento() {
    Memento m(state);
    return m;
  }
  void setMomento(const Memento& m) { state = m.getState(); }
};

int main() {
  Originator orginator;

  // 捕获对象状态，存储到备忘录
  Memento memento = orginator.createMomento();

  // ... 改变orginator状态

  // 从备忘录中恢复
  orginator.setMomento(memento);
}