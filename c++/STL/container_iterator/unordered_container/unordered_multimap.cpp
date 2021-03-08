#include <iostream>
#include <unordered_map>

using namespace std;

// 和unordered_map容器一样，unordered_multimap容器也以键值对的形式存储数据，且底层也采用哈希表结构存储各个键值对。
// 两者唯一的不同之处在于，unordered_multimap容器可以存储多个键相等的键值对，而unordered_map容器不行。

// STL标准库中实现unordered_multimap容器的模板类并没有定义在以自己名称命名的头文件中，而是和unordered_map容器一样，定义在<unordered_map>头文件，且位于std命名空间中。
// unordered_multimap 容器模板的定义如下所示：
//     template < class Key,      //键（key）的类型
//                class T,        //值（value）的类型
//                class Hash = hash<Key>,  //底层存储键值对时采用的哈希函数
//                class Pred = equal_to<Key>,  //判断各个键值对的键相等的规则
//                class Alloc = allocator< pair<const Key,T> > // 指定分配器对象的类型
//                > class unordered_multimap;
// 以上5个参数中，必须显式给前2个参数传值，且除极个别的情况外，最多只使用前4个参数
// <key,T> 前2个参数分别用于确定键值对中键和值的类型，也就是存储键值对的类型。
// Hash = hash<Key> 用于指明容器在存储各个键值对时要使用的哈希函数，默认使用STL标准库提供的hash<key>哈希函数。
//                  注意，默认哈希函数只适用于基本数据类型（包括string类型），而不适用于自定义的结构体或者类。
// Pred = equal_to<Key> unordered_multimap 容器可以存储多个键相等的键值对，而判断是否相等的规则，由此参数指定。
//                      默认情况下，使用STL标准库中提供的equal_to<key>规则，该规则仅支持可直接用==运算符做比较的数据类型。
// 当unordered_multimap容器中存储键值对的键为自定义类型时，默认的哈希函数hash<key>以及比较函数equal_to<key>将不再适用，
// 这种情况下，需要我们自定义适用的哈希函数和比较函数，并分别显式传递给Hash参数和Pred参数。

int main() { return 0; }