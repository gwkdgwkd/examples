#include <iostream>
#include <map>
#include <set>

// 对于如何修改容器中某个键值对的键，所有关联式容器可以采用同一种解决思路，
// 即先删除该键值对，然后再向容器中添加修改之后的新键值对。

// 是否可以不删除目标键值对，而直接修改它的键呢？
// map和multimap容器只能采用先删除再添加的方式修改某个键值对的键。
// C++ STL标准中明确规定，map和multimap存储类型为pair<const K,V>。
// 显然，只要目标键值对存储在当前容器中，键的值就无法被修改。
// 直接修改map或multimap容器中某个键值对的键是行不通的。
// 但对于set或者multiset容器来说，却是可行的，和map、multimap不同，
// C++ STL标准中并没有用const限定set和multiset容器中存储元素的类型。
// 换句话说，对于set<T>或multiset<T>，其存储元素的类型是T而不是const T。
// 总之，set和multiset容器的元素类型没有用const修饰。
// 所以从语法的角度分析，可以直接修改容器中元素的值，但一定不要修改元素的键。

void func1() {
  // 下面都编译报错：
  // error: assignment of read-only member ‘std::pair<const int, int>::first’
  // map<int, int> m{{1, 10}, {2, 20}};
  // m.begin()->first = 100;
  // multimap<int, int> mm{{10, 100}, {20, 200}};
  // mm.begin()->first = 100;

  auto print = [](std::map<std::string, int> &m) -> void {
    for (auto i = m.begin(); i != m.end(); ++i) {
      std::cout << i->first << " " << i->second << std::endl;
    }
  };

  // map和multimap容器中元素的键是无法直接修改的，
  // 但借助const_cast，可以直接修改map和multimap容器中元素的键。
  std::map<std::string, int> m{{"one", 1}, {"two", 2}, {"three", 3}};
  print(m);
  // one 1
  // three 3
  // two 2

  std::map<std::string, int>::iterator it = m.begin();
  const_cast<std::string &>(it->first) = "zero";
  it = --m.end();
  const_cast<std::string &>(it->first) = "five";
  print(m);
  // zero 1
  // three 3
  // five 2
  // 顺序乱了

  // 不要试图修改map或者multimap中键值对的键，这违反了C++ STL标准的规定。
}

// 对set和multiset容器中的元素类型作const修饰，是违背常理的。
class student {
 public:
  student(std::string name, int id, int age) : name(name), id(id), age(age) {}
  const int &getid() const { return id; }
  void setid(int id) { this->id = id; }
  void setname(const std::string name) { this->name = name; }
  std::string getname() const { return name; }
  void setage(int age) { this->age = age; }
  int getage() const { return age; }
  void display() const {
    std::cout << "[ " << id << ", " << name << ", " << age << " ]" << std::endl;
  }

 private:
  std::string name;
  int id;
  int age;
};
class cmp {
 public:
  bool operator()(const student &stua, const student &stub) {
    return stua.getid() < stub.getid();
  }
};
void func2() {
  auto print = [](std::set<student, cmp> &s) -> void {
    for (auto i = s.begin(); i != s.end(); ++i) {
      std::cout << "[ " << i->getid() << ", " << i->getname() << ", "
                << i->getage() << " ]" << std::endl;
    }
  };

  // set容器中每个元素也可以看做是键和值相等的键值对，但对于这里的s来说，
  // 其实每个student的id才是真正的键，name和age只不过是和id绑定在一起而已。
  // 因此，在不破坏s容器中元素的有序性的前提下（即不修改每个学生的id），
  // 学生的其它信息是应该允许修改的，但有一个前提，
  // 即s容器中存储的各个student对象不能被const修饰，
  // 这也是set容器中的元素类型不能被const修饰的原因。
  std::set<student, cmp> s{
      {"zhangsan", 10, 20}, {"lisi", 20, 21}, {"wangwu", 15, 19}};
  print(s);
  // [ 10, zhangsan, 20 ]
  // [ 15, wangwu, 19 ]
  // [ 20, lisi, 21 ]

  // 在已创建好的s容器的基础上，尝试修改s容器中学生的name，也是无法通过编译：
  // set<student>::iterator iter = s.begin();
  // (*iter).setname("xiaoming");
  // 虽然C++ STL标准没有用const修饰set或者multiset容器中元素的类型，
  // 但也做了其它工作来限制用户修改容器的元素。
  // *iter会调用operator*，其返回的是一个const T&类型元素。
  // 这意味着C++ STL标准不允许借助迭代器来直接修改set或者multiset中的元素。

  // 那么，如何才能正确修改set或multiset容器中的元素呢？
  // 最直接的方式就是借助const_cast运算符，
  // 该运算符可以去掉指针或者引用的const限定符。
  std::set<student>::iterator iter = s.begin();
  const_cast<student &>(*iter).setname("xiaoming");
  const_cast<student &>(*++iter).setage(99);
  const_cast<student &>(*++iter).setid(5);
  print(s);
  // [ 10, xiaoming, 20 ]
  // [ 15, wangwu, 99 ]
  // [ 5, lisi, 21 ]

  // 虽然const_cast能直接修改set或multiset的元素，但一定不要修改元素的键！
  // 如果要修改，只能采用先删除再添加的方式。
}

int main() {
  func1();
  func2();

  return 0;
}