#include <iostream>

using namespace std;

// 两个基类有同名的虚函数要实现, 怎么办?

class ChineseName1 {
 public:
  virtual string getname() { return string(); }
};
class EnglishName1 {
 public:
  virtual string getname() { return string(); }
};
class Name1 : public ChineseName1, public EnglishName1 {
 public:
  virtual string getname() { return string("c or e name? I donot know"); }
};

class ChineseName2 {
 public:
  virtual string getname() { return string(); }
};
class EnglishName2 {
 public:
  virtual string getname() { return string(); }
};
class ICName2 : public ChineseName2 {
 public:
  virtual string getname() { return getChineseName(); }

 private:
  virtual string getChineseName() = 0;
};
class IEName2 : public EnglishName2 {
 public:
  virtual string getname() { return getEnglishName(); }

 private:
  virtual string getEnglishName() = 0;
};
class Name2 : public ICName2, public IEName2 {
 private:
  virtual string getChineseName() { return string("二斗墨汁"); }
  virtual string getEnglishName() { return string("Adward Ink"); }
};

int main() {
  Name1 n1;
  ChineseName1& c1 = n1;
  EnglishName1& e1 = n1;
  cout << n1.getname() << endl;  // c or e name? I donot know
  cout << c1.getname() << endl;  // c or e name? I donot know
  cout << e1.getname() << endl;  // c or e name? I donot know

  Name2 n2;
  ChineseName2& c2 = n2;
  EnglishName2& e2 = n2;
  // cout << n2.getname() << endl;  // complie error
  cout << c2.getname() << endl;  // 二斗墨汁
  cout << e2.getname() << endl;  // Adward Ink

  return 0;
}