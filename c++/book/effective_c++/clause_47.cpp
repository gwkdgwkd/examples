#include <iostream>

// 请使用traits class表现类型信息

// STL主要由用以表现容器、迭代器和算法的template构成，但也覆盖若干工具性template。
// 其中一个名为advance，用来将某个迭代器移动某个给定距离。
// 只有随机访问迭代器才支持+=操作，其他迭代器种类，advance必须反复++或--。

// STL共有5种迭代器：
// 1.输入迭代器只能向前移动，一次一步，客户只能读取（不能写）所指的东西，
//   而且只能读取一次，例如istream_iterator；
// 2.输出迭代器智能向前移动，一次一步，客户只能写，不能读，
//   而且只能写一次，例如ostream_iterator。
// 3.forward迭代器，可以做前面两种迭代器所做的每一件事，而且可以读或写一次以上。
// 4.bidirectional迭代器比上一个威力更大，除了可以向前，还可以向后移动。
// 5.最有威力的是随机迭代器，比上一个威力更大的地方在于，
//   可以执行迭代器算术运算，以内置指针为榜样的。

// advance的实现处理两种情况，
// 通过traits（允许在编译期间取得某些类型信息）可以知道迭代器的类型。
// traits并不是C++的关键字或一个预先定义好的构件。
// 它们是一种技术，也是一个C++程序员共同遵守的协议。
// 这个技术的要求之一是，对内置类型和用户自定义类型的表现必须一样好。
// traits技术必须也能够实施于内置类型如指针身上，traits信息必须位于类型自身之外。
// 标准技术是把它放进一个template及其一或多个特化版本中。
// 这样的template在标准库中有很多，针对迭代器的叫iterator_traits，是个struct。
// 习惯上总被实现为struct，但它们却往往被称为traits class。

// iterator_traits的实现方式是，针对每一个类型IterT，
// 在struct iterator_traits<IterT>内：
// 一定声明某个typedef名为iterator_category，这个typedef用来确认IterT的迭代器分类。
// iterator_traits需要做两部分：
// 1.首先要求每一个用户自定义的迭代器类型必须嵌套一个typedef，
//   名为iterator_category，用来确认适当的卷标结构tag stuct。
//   iterator_traits只是鹦鹉学舌般地响应iterator class的嵌套式typedef。
//   这对用户自定义类型行的通，但对指针性不通，因为指针不能嵌套typedef。
// 2.为了支持指针迭代器，iterator_traits特别针对指针类型提供一个偏特化版本。
//   由于指针的行为与随机迭代器类似，所以iterator_traits为指针指定的迭代器类型是：
//   typedef random_access_iterator_tag iterator_category

// 设计并实现一个traits class：
// 1.确认若干希望将来可取得的类型相关信息，例如对迭代器，希望将来可以取得其分类。
// 2.为该信息选择一个名称，例如iterator_category；
// 3.提供一个template和一组特化版本iterator_traits，内含希望支持的类型相关信息。

// advance类似如下：
// tepmlate<typename IterT, typename DistT>
// void advance(IterT &iter, DistT d) {
//   if (typeid(typename std::iterator_traits<IterT>::iterator_category) ==
//       typeid(std::random_access_iterator_tag))
//   // ...
// }
// 首先编译有问题，看条款48，if需要在运行期间才能确定。
// 可以在编译期间确定的事，不应该拖延到运行期才做。
// 需要编译期判断，可以使用重载，编译期条件语句。
// tepmlate<typename IterT, typename DistT>
// void doAdvance(IterT &iter, DistT d, std::random_access_iterator_tag) {
//   iter += d;
// }
// tepmlate<typename IterT, typename DistT>
// void doAdvance(IterT &iter, DistT d, std::bidirectional_iterator_tag) {
//   if (d >= 0) {
//     while (d--) ++iter;
//   } else {
//     while (d++) --iter;
//   }
// }
// tepmlate<typename IterT, typename DistT>
// void doAdvance(IterT &iter, DistT d, std::input_iterator_tag) {
//   if(d<0) {
//     throw std::out_of_range("negative distance");
//   }
//   while (d--) ++iter;
// }
// 有了重载版本，advance需要就是调用并额外传递一个对象，后者必须带有适当的迭代器分类：
// void advance(IterT &iter, DistT d) {
//   doAdvance(iter,d,
//     typename std::iterator_traits<IterT>::iterator_category());
// }

// 如何使用一个traits class：
// 1.建立一组重载函数或函数模板，彼此间的差异只在于各自的traits参数，
//   令每个函数实现码与其接受的traits信息相对应；
// 2.建立一个控制函数或函数模板，调用上述那些函数并传递traits class所提供的信息。

// Traits广泛用于标准程序库，iterator_traits除了iterator_categor外，
// 还提供另外四份迭代器相关信息：
// 1.最有用的value_type；
// 2.char_traits用来保存字符类型的相关信息；
// 3.numeric_limits用来保存数值类型的相关信息，
//   例如某数值类型的最小值和最大值，没有以traits结束，有点奇怪。

// TR1导入许多新的traits class用以提供类型信息，包括：
// 1.is_fundamental<T>判断T是否为内置类型；
// 2.is_array<T>判断T是否为数组类型；
// 3.is_base_of<T1,T2>T1和T2相同，异或T1是T2的base class；
// TR1一共为标准C++添加来50个以上的traits class。

// 请记住：
// 1.traits class使得类型相关信息在编译期可用；
// 2.它们以template和template特化完成实现；
// 3.整合重载技术后，traits class有可能在编译期对类型执行if...else测试。

int main() { return 0; }