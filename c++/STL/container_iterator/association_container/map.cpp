#include <iostream>
#include <iterator>
#include <map>
#include <utility>

// map容器存储的都是pair对象，也就是用pair类模板创建的键值对。
// 其中，各个键值对的键和值可以是任意数据类型，包括C++基本数据类型，使用结构体或类自定义的类型。
// 通常情况下，map容器中存储的各个键值对都选用string字符串作为键的类型。
// 在使用map容器存储多个键值对时，该容器会自动根据各键值对的键的大小，按照既定的规则进行排序。
// 默认情况下，map容器选用std::less<T>排序规则（其中T表示键的数据类型），其会根据键的大小对所有键值对做升序排序。
// 当然，也可以手动指定map容器的排序规则，既可以选用STL中提供的其它排序规则（std::greater<T>），也可以自定义排序规则。
// 另外需要注意的是，使用map容器存储的各个键值对，键的值既不能重复也不能被修改。
// 换句话说，map容器中存储的各个键值对不仅键的值独一无二，键的类型也会用const修饰，
// 这意味着只要键值对被存储到map容器中，其键的值将不能再做任何修改。
// map容器存储的都是pair类型的键值对元素，更确切的说，
// 该容器存储的都是pair<const K,T>类型（其中K和T分别表示键和值的数据类型）的键值对元素。

// template <class Key,                  // 指定键（key）的类型
//           class T,                    // 指定值（value）的类型
//           class Compare = less<Key>,  // 指定排序规则
//           class Alloc = allocator<pair<const Key, T>>  // 指定分配器对象的类型
//           > class map;
// map容器模板有4个参数，其中后2个参数都设有默认值。
// 大多数场景中，我们只需要设定前2个参数的值，有些场景可能会用到第3个参数，但最后一个参数几乎不会用到。

// map容器包含的成员方法:
// begin() 	        返回指向容器中第一个（是已排好序的第一个）键值对的双向迭代器。
//                  如果map容器用const限定，则该方法返回的是const类型的双向迭代器。
// end() 	          返回指向容器最后一个元素（排好序的最后一个）所在位置后一个位置的双向迭代器，通常和begin()结合使用。
//                  如果map容器用const限定，则该方法返回的是const类型的双向迭代器。
// rbegin()         返回指向最后一个（是已排好序的最后一个）元素的反向双向迭代器。
//                  如果map容器用const限定，则该方法返回的是const类型的反向双向迭代器。
// rend() 	        返回指向第一个（是已排好序的第一个）元素所在位置前一个位置的反向双向迭代器。
//                  如果map容器用const限定，则该方法返回的是const类型的反向双向迭代器。
// cbegin()         和begin()功能相同，只不过在其基础上，增加了const属性，不能用于修改容器内存储的键值对。
// cend() 	        和end()功能相同，只不过在其基础上，增加了const属性，不能用于修改容器内存储的键值对。
// crbegin()        和rbegin()功能相同，只不过在其基础上，增加了 const 属性，不能用于修改容器内存储的键值对。
// crend() 	        和end()功能相同，只不过在其基础上，增加了 const 属性，不能用于修改容器内存储的键值对。
// find(key)        在map容器中查找键为key的键值对，如果成功找到，则返回指向该键值对的双向迭代器；
//                  反之，则返回和end()方法一样的迭代器。
//                  另外，如果map容器用const限定，则该方法返回的是const类型的双向迭代器。
// lower_bound(key) 返回一个指向当前map容器中第一个大于或等于key的键值对的双向迭代器。
//                  如果map容器用const限定，则该方法返回的是const类型的双向迭代器。
// upper_bound(key) 返回一个指向当前map容器中第一个大于key的键值对的迭代器。
//                  如果map容器用const限定，则该方法返回的是const类型的双向迭代器。
// equal_range(key) 该方法返回一个pair对象（包含2个双向迭代器），其中pair.first和lower_bound()方法的返回值等价，
//                  pair.second和upper_bound()方法的返回值等价。
//                  也就是说，该方法将返回一个范围，该范围中包含的键为key的键值对。
// empty()          若容器为空，则返回true；否则false。
// size() 	        返回当前map容器中存有键值对的个数。
// max_size()       返回map容器所能容纳键值对的最大个数，不同的操作系统，其返回值亦不相同。
// operator[]       map容器重载了[]运算符，只要知道map容器中某个键值对的键的值，就可以向获取数组中元素那样，通过键直接获取对应的值。
// at(key) 	        找到map容器中key键对应的值，如果找不到，该函数会引发out_of_range异常。
// insert()         向map容器中插入键值对。
// erase() 	        删除map容器指定位置、指定键（key）值或者指定区域内的键值对。
// swap() 	        交换2个map容器中存储的键值对，这意味着，操作的2个键值对的类型必须相同。
// clear() 	        清空map容器中所有的键值对，即使map容器的size()为0。
// emplace() 	      在当前map容器中的指定位置处构造新键值对。其效果和插入键值对一样，但效率更高。
// emplace_hint()   在本质上和emplace()在map容器中构造新键值对的方式是一样的，不同之处在于，
//                  使用者必须为该方法提供一个指示键值对生成位置的迭代器，并作为该方法的第一个参数。
// count(key)       在当前map容器中，查找键为key的键值对的个数并返回。
//                  注意，由于map容器中各键值对的键的值是唯一的，因此该函数的返回值最大为1。

template <typename K, typename V>
void print(std::map<K, V>& m) {
  std::cout << "size:" << m.size() << std::endl;
  for (auto it = m.begin(); it != m.end(); ++it) {
    std::cout << it->first << ":" << it->second << std::endl;
  }
}

struct map_cmp {
  bool operator()(const std::string& k1, const std::string& k2) {
    return k1 > k2;
  }
};

void func1() {  // hello 10
  // 1.通过调用map容器类的默认构造函数，可以创建出一个空的map容器
  std::map<std::string, int> m1;
  print(m1);  // size:0

  // 2.当然在创建map容器的同时，也可以进行初始化
  std::map<std::string, int> m2{{"hello", 10}, {"world", 20}};
  print(m2);
  // size:2
  // hello:10
  // world:20

  std::map<std::string, int> m3 = {std::make_pair("hello", 10),
                                   std::make_pair("world", 20)};
  print(m3);
  // size:2
  // hello:10
  // world:20

  // 3.除此之外，在某些场景中，可以利用先前已创建好的map容器，再创建一个新的map容器：
  std::map<std::string, int> m4(m1);   // 拷贝构造函数
  std::map<std::string, int> m5 = m2;  // 拷贝构造函数

  auto disMap = []() -> std::map<std::string, int> {
    std::map<std::string, int> tempMap{{"hello", 5}, {"world", 7}};
    return tempMap;
  };
  std::map<std::string, int> m6(disMap());  // 移动构造函数
  print(m6);
  // size:2
  // hello:5
  // world:7
  // 注意，无论是调用复制构造函数还是调用拷贝构造函数，都必须保证这2个容器的类型完全一致。

  // 4.map类模板还支持取已建map容器中指定区域内的键值对，创建并初始化新的map容器：
  std::map<std::string, int> m7(++m2.begin(), m2.end());
  print(m7);
  // size:1
  // world:20
  // 通过调用map容器的双向迭代器，实现了在创建newMap容器的同时，将其初始化为包含一个{"world",20}键值对的容器。

  // 5.在以上几种创建map容器的基础上，我们都可以手动修改map容器的排序规则。
  // 默认情况下，map容器调用 std::less<T>规则，根据容器内各键值对的键的大小，对所有键值对做升序排序。
  // 如下2行创建map容器的方式，其实是等价的：
  std::map<std::string, int> m8{{"a", 10}, {"b", 20}};
  print(m8);
  // size:2
  // hello:10
  // world:20
  std::map<std::string, int, std::less<std::string>> m9{{"a", 10}, {"b", 20}};
  print(m9);
  // size:2
  // hello:10
  // world:20

  std::map<std::string, int, std::greater<std::string>> m10{{"a", 10},
                                                            {"b", 20}};
  for (auto it = m10.begin(); it != m10.end(); ++it) {
    std::cout << it->first << ":" << it->second << std::endl;
  }
  // b:20
  // a:10

  std::map<std::string, int, map_cmp> m11{{"a", 10}, {"b", 20}};
  for (auto it = m11.begin(); it != m11.end(); ++it) {
    std::cout << it->first << ":" << it->second << std::endl;
  }
  // b:20
  // a:10
}

void func2() {
  // map容器中存储的都是pair类型的键值对，但几乎在所有使用map容器的场景中，
  // 经常要做的不是找到指定的pair对象（键值对），而是从该容器中找到某个键对应的值。
  // 使用map容器存储的各个键值对，其键的值都是唯一的，因此指定键对应的值最多有1个。

  std::map<char, int> m{{'a', 1}, {'b', 2}, {'c', 3}};

  // map容器的类模板中提供了以下2种方法，可直接获取map容器指定键对应的值，还有两个其他的方法：
  // 1.map类模板中对[]运算符进行了重载，这意味着，类似于借助数组下标可以直接访问数组中元素：
  std::cout << m['b'] << std::endl;

  m['d'];  // key不存在，新增key，value为对应类型的默认值
  print(m);
  // size:4
  // a:1
  // b:2
  // c:3
  // d:0
  m['d'] = 4;  // key已经存在，更新key对应的value
  print(m);
  // size:4
  // a:1
  // b:2
  // c:3
  // d:4

  // 2.除了借助[]运算符获取map容器中指定键对应的值，还可以使用at()成员方法。
  //   和前一种方法相比，at()成员方法也需要根据指定的键，才能从容器中找到该键对应的值；
  //   不同之处在于，如果在当前容器中查找失败，该方法不会向容器中添加新的键值对，而是直接抛出out_of_range异常。
  std::cout << m.at('b') << std::endl;  // 2
  try {
    m.at('e');
  } catch (std::out_of_range e) {
    std::cout << e.what() << std::endl;  // map::at
  }

  // 3.除了可以直接获取指定键对应的值之外，还可以借助find()成员方法间接实现此目的：
  //   和以上2种方式不同的是，该方法返回的是一个迭代器，即如果查找成功，该迭代器指向查找到的键值对；
  //   反之，则指向map容器最后一个键值对之后的位置（和end()成功方法返回的迭代器一样）。
  std::map<char, int>::iterator it = m.find('b');
  std::cout << it->first << ":" << it->second << std::endl;  // b:2
  it = m.find('e');
  if (it == m.end()) {  // 当find()失败时，其返回指向最后一个键值对之后的位置
    std::cout << "not found" << std::endl;
  }

  // 4.如果以上方法都不适用，我们还可以遍历整个map容器，找到包含指定键的键值对，进而获取该键对应的值：
  for (auto iter = m.begin(); iter != m.end(); ++iter) {
    if (iter->first == 'b') {
      std::cout << iter->first << ":" << iter->second << std::endl;
    }
  }
  // b:2

  // 从“在map容器存储的键值对中，获取指定键对应的值”的角度出发，更推荐使用at()成员方法，因为该方法既简单又安全。
}

void func3() {
  // 除了使用[]运算符实现向map容器中添加新键值对外，map类模板中还提供有insert()成员方法，
  // 该方法专门用来向map容器中插入新的键值对。
  // 这里所谓的“插入”，指的是insert()方法可以将新的键值对插入到map容器中的指定位置，
  // 但这与map容器会自动对存储的键值对进行排序并不冲突。
  // 当使用insert()方法向map容器的指定位置插入新键值对时，其底层会先将新键值对插入到容器的指定位置，
  // 如果其破坏了map容器的有序性，该容器会对新键值对的位置进行调整。
  // 自C++11标准后，insert()成员方法的用法大致有以下4种
  // 1.无需指定插入位置，直接将键值对添加到map容器中。insert()方法的语法格式有以下2种：
  //  引用传递一个键值对pair<iterator,bool> insert (const value_type& val);
  //  以右值引用的方式传递键值对template <class P> pair<iterator,bool> insert (P&& val);
  // 其中，val参数表示键值对变量，同时该方法会返回一个pair对象，其中pair.first表示一个迭代器，pair.second为一个bool类型变量：
  //  如果成功插入val，则该迭代器指向新插入的val，bool值为true；
  //  如果插入val失败，则表明当前map容器中存有和val的键相同的键值对（用p表示），此时返回的迭代器指向p，bool值为false。
  // 以上2种语法格式的区别在于传递参数的方式不同，即无论是局部定义的键值对变量还是全局定义的键值对变量，都采用普通引用传递的方式；
  // 而对于临时的键值对变量，则以右值引用的方式传参。

  std::map<char, int> m{{'a', 1}, {'b', 2}, {'c', 3}};
  std::pair<std::map<char, int>::iterator, bool> ret;
  // ret = m.insert(std::pair<char, int>{'d', 4});
  // ret = m.insert(std::make_pair('d', 4));
  ret = m.insert({'d', 4});  // 以右值引用的方式传递临时的键值对变量
  std::cout << ret.first->first << ":" << ret.first->second << ", "
            << std::boolalpha << ret.second << std::endl;
  // d:4, true
  std::pair<char, int> p = {'c', 4};
  ret = m.insert(p);  // 以左值引用的方式传递变量
  std::cout << ret.first->first << ":" << ret.first->second << ", "
            << std::boolalpha << ret.second << std::endl;
  // c:3, false

  // 2 除此之外，insert()方法还支持向map容器的指定位置插入新键值对，该方法的语法格式如下：
  //  以普通引用的方式传递val参数iterator insert (const_iterator position, const value_type& val);
  //  以右值引用的方式传递val键值对参数template <class P> iterator insert (const_iterator position, P&& val);
  // 其中val为要插入的键值对变量。注意，和第1种方式的语法格式不同，这里insert()方法返回的是迭代器，而不再是pair对象：
  //  如果插入成功，insert()方法会返回一个指向map容器中已插入键值对的迭代器；
  //  如果插入失败，insert()方法同样会返回一个迭代器，该迭代器指向map容器中和val具有相同键的那个键值对。
  // std::map<string, string> mymap1;
  // std::map<string, string>::iterator it = mymap1.begin();
  // // 向it位置以普通引用的方式插入STL
  // auto iter11 = mymap1.insert(it, STL);
  // cout << iter11->first << " " << iter11->second << endl;  // STL教程 stl
  // // 向it位置以右值引用的方式插入临时键值对
  // auto iter2 = mymap1.insert(it, std::pair<string, string>("C语言教程", "c"));
  // cout << iter2->first << " " << iter2->second << endl;  // C语言教程 c
  // 插入失败样例
  // auto iter3 = mymap1.insert(
  //     it, std::pair<string, string>("STL教程", "http://c.biancheng.net/java/"));
  // cout << iter3->first << " " << iter3->second << endl;  // STL教程 stl
  // 3 insert()方法还支持向当前map容器中插入其它map容器指定区域内的所有键值对，该方法的语法格式如下：
  // template <class InputIterator> void insert (InputIterator first, InputIterator last);
  // 其中first和last都是迭代器，它们的组合<first,last>可以表示某map容器中的指定区域。
  // std::map<std::string, std::string> copymap;
  // std::map<string, string>::iterator first = ++myMap.begin();
  // std::map<string, string>::iterator last = myMap.end();
  // copymap.insert(first, last);
  // for (auto iter = copymap.begin(); iter != copymap.end(); ++iter) {
  //   cout << iter->first << " " << iter->second << endl;
  // }
  // Cxxx
  // C语言教程 http://c.biancheng.net/c/
  // Python教程 http://c.biancheng.net/python/
  // 4 除了以上一种格式外，insert()方法还允许一次向map容器中插入多个键值对，其语法格式为：
  // void insert ({val1, val2, ...}); 其中，vali都表示的是键值对变量。
  // std::map<std::string, std::string> mymap2;
  //向 mymap 容器中添加 3 个键值对
  // mymap2.insert({{"STL教程", "http://c.biancheng.net/stl/"},
  //                {"C语言教程", "http://c.biancheng.net/c/"},
  //                {"Java教程", "http://c.biancheng.net/java/"}});
  // for (auto iter = mymap2.begin(); iter != mymap2.end(); ++iter) {
  //   cout << iter->first << " " << iter->second << endl;
  // }
  // C语言教程 http://c.biancheng.net/c/
  // Java教程 http://c.biancheng.net/java/
  // STL教程 http://c.biancheng.net/stl/

  // map容器模板类中提供有operator[]和insert()这2个成员方法具有相同的功能，它们既可以实现向map容器中添加新的键值对元素，也可以实现更新（修改）map容器已存储键值对的值。
  // std::map<string, string> mymap3;
  // 借用operator[]添加新键值对
  // mymap3["STL教程"] = "http://c.biancheng.net/java/";
  // cout << "old mymap：" << mymap3["STL教程"] << endl;
  // 借用operator[]更新某个键对应的值
  // mymap3["STL教程"] = "http://c.biancheng.net/stl/";
  // cout << "new mymap：" << mymap3["STL教程"] << endl;
  // 借用insert()添加新键值对
  // std::pair<string, string> STL1 = {"Java教程", "java"};
  // std::pair<std::map<string, string>::iterator, bool> ret1;
  // ret1 = mymap3.insert(STL1);
  // cout << "old ret.iter = <{" << ret1.first->first << ", " << ret1.first->second
  //      << "}, " << ret1.second << ">" << endl;
  // 借用insert()更新键值对
  // mymap3.insert(STL1).first->second = "http://c.biancheng.net/java/";
  // cout << "new ret.iter = <" << ret1.first->first << ", " << ret1.first->second
  //      << ">" << endl;
  // old mymap：http://c.biancheng.net/java/
  // new mymap：http://c.biancheng.net/stl/
  // old ret.iter = <{Java教程, java}, 1>
  // new ret.iter = <Java教程, http://c.biancheng.net/java/>
  // map模板类中operator[]和insert()的功能发生了重叠，这就产生了一个问题，谁的执行效率更高呢？
  // 当实现“向map容器中添加新键值对元素”的操作时，insert()成员方法的执行效率更高；而在实现“更新map容器指定键值对的值”的操作时，operator[]的效率更高。
  // 1 向map容器中增添元素，insert()效率更高：
  // mymap["STL教程"] = "http://c.biancheng.net/java/";
  // mymap["STL教程"]实际上是 mymap.operator[](“STL教程”)的缩写（底层调用的operator[]方法），该方法会返回一个指向“STL教程”对应的value值的引用。
  // 由于此时mymap容器是空的，并没有"STL教程"对应的value值。这种情况下，operator[]方法会默认构造一个string对象，并将其作为"STL教程"对应的value值，然后返回一个指向
  // 此string对象的引用。在此基础上，代码还会将"http://c.biancheng.net.java/"赋值给这个string对象。相当于以下流程：
  // typedef map<string, string> mstr;
  // pair<mstr::iterator, bool> res = mymap.insert(mstr::value_type("STL教程", string()));
  // res.first->second = "http://c.biancheng.net/java/";
  // 使用operator[]添加新键值对元素的流程是，先构造一个有默认值的键值对，然后再为其value赋值。那么，为什么不直接构造一个要添加的键值对元素呢？
  // mymap.insert(mstr::value_type("STL教程", "http://c.biancheng.net/java/"));
  // 和上面程序的执行效果完全相同，但它省略了创建临时string对象的过程以及析构该对象的过程，同时还省略了调用string类重载的赋值运算符。由于可见，同样是完成向map容器添加新键
  // 值对，insert()方法比operator[]的执行效率更高。
  // 2 更新map容器中的键值对，operator[]效率更高：
  // mymap["STL教程"] = "http://c.biancheng.net/stl/";
  // std::pair<string, string> STL = { "Java教程","http://c.biancheng.net/python/" };
  // mymap.insert(STL).first->second = "http://c.biancheng.net/java/";
  // 仅仅从语法形式本身来考虑，或许已经促使很多读者选择operator[]了。
  // insert()方法在进行更新操作之前，需要有一个pair类型（也就是map::value_type类型）元素做参数。这意味着，该方法要多构造一个pair对象（附带要构造2个string对象），并且事
  // 后还要析构此pair对象（附带2个string对象的析构）。
  // 而和insert()方法相比，operator[]就不需要使用pair对象，自然不需要构造（并析构）任何pair对象或者string对象。
  // 因此，对于更新已经存储在map容器中键值对的值，应优先使用operator[]方法。
}

void func4() {}

void func5() {}

void func6() {}

void func7() {}

void func8() {}

void func9() {}

int main(int argc, char* argv[]) {
  if (argc < 2) {
    std::cout << argv[0] << " i [0 - 8]" << std::endl;
    return 0;
  }
  int type = argv[1][0] - '0';
  switch (type) {
    case 0:
      func1();
      break;
    case 1:
      func2();
      break;
    case 2:
      func3();
      break;
    case 3:
      func4();
      break;
    case 4:
      func5();
      break;
    case 5:
      func6();
      break;
    case 6:
      func7();
      break;
    case 7:
      func8();
      break;
    case 8:
      func9();
      break;
    default:
      std::cout << "invalid type" << std::endl;
      break;
  }

  return 0;
}