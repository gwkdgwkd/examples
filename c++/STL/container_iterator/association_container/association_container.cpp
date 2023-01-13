#include <iostream>
#include <map>

// 关联式容器包括map、multimap、set以及multiset这4种容器。
// 关联式容器则大不一样，此类容器在存储元素值的同时，
// 还会为各元素额外再配备一个值，又称为键，
// 其本质也是一个C++基础数据类型或自定义类型的元素，
// 它的功能是在使用关联式容器的过程中，如果已知目标元素的键的值，
// 则直接通过该键就可以找到目标元素，而无需再通过遍历整个容器的方式。
// 弃用序列式容器，转而选用关联式容器存储元素，
// 往往就是看中了关联式容器可以快速查找、读取或者删除所存储的元素，
// 同时该类型容器插入元素的效率也比序列式容器高。
// 也就是说，使用关联式容器存储的元素，
// 都是一个一个的键值对<key,value>，这是和序列式容器最大的不同。
// 除此之外，序列式容器中存储的元素默认都是未经过排序的，
// 而使用关联式容器存储的元素，默认会根据各元素的键值的大小做升序排序。
// 关联式容器所具备的这些特性，归咎于STL标准库在实现该类型容器时，
// 底层选用了红黑树这种数据结构来组织和存储各个键值对。

// map定义在<map>头文件中，使用该容器存储的数据，
// 其各个元素的键必须是唯一的，即不能重复，
// 该容器会根据各元素键的大小，默认进行升序排序，调用std::less<T>。
// set定义在<set>头文件中，使用该容器存储的数据，各个元素键和值完全相同，
// 且各个元素的值不能重复，保证了各元素键的唯一性。
// 该容器会自动根据各个元素的键的大小进行升序排序，调用std::less<T>。
// multimap定义在<map>头文件中，和map容器唯一的不同在于，
// multimap容器中存储元素的键可以重复。
// multiset定义在<set>头文件中，和set容器唯一的不同在于，
// multiset容器中存储元素的值可以重复，键也是重复的。

// C++11还新增了4种哈希容器：
// 1.unordered_map；
// 2.unordered_multimap；
// 3.unordered_set；
// 4.unordered_multiset。
// 严格来说，它们也属于关联式容器，
// 但由于哈希容器底层采用的是哈希表，而不是红黑树。

int main() {
  std::map<std::string, std::string> mymap;
  mymap["http://c.biancheng.net/c/"] = "C语言教程";
  mymap["http://c.biancheng.net/python/"] = "Python教程";
  mymap["http://c.biancheng.net/java/"] = "Java教程";
  for (std::map<std::string, std::string>::iterator it = mymap.begin();
       it != mymap.end(); ++it) {
    std::cout << it->first << " => " << it->second << '\n';
  }
  // map存储元素时默认按照升序排序，因此最终存储的元素顺序为：
  // http://c.biancheng.net/c/ => C语言教程
  // http://c.biancheng.net/java/ => Java教程
  // http://c.biancheng.net/python/ => Python教程

  return 0;
}