#include <iostream>
#include <string>

// 注意string实现的多样性

// 几乎每个string实现都包含如下信息：
// 1.字符串的大小（size），即它所包含的字符的个数。
// 2.用于存储该字符串中字符的内存的容量（capacity）。
// 3.字符串的值（value），即构造该字符串的字符。
// 除此之外，一个string还可能包含：它的分配子的一份拷贝，这个字段是可选的。
// 建立在引用计数基础上的string实现可能还包括：对值的引用计数。

// 实现A：
// 每个string对象包含其分配子的一份拷贝、该字符串的大小、容量，
// 已经一个指针，指向一块动态分配的内容，其中包含了引用计数和字符串的值。
// 使用默认分配子的string对象其大小是一个指针的4倍。
// 使用自定义的分配子，string对象会更大一些。

// 实现B：
// string对象（使用默认分配子，自定义会更大）与指针大小相同，
// 因为它只包含一个指向结构的指针。
// 指向的对象包含了该字符串的大小、容量和引用计数，
// 以及一个指向一块动态分配的内存的指针，该内存中存放了字符串的值。
// 该对象还包含了一些与多线程环境下的同步控制相关的额外数据。

// 实现C：
// 对象的大小总是与指针相同，而该指针指向一块动态分配的内存，
// 其中包含了与该字符串相关的一切数据（容量、大小、引用计数和值）。
// 没有对单个对象的分配子支持。该内存中也包含了一些与值的可共享性有关的数据。

// 实现D：
// string对象是指针大小的七倍（假设使用默认分配子）。
// 不使用引用计数，但是每个string内部包含一块内存，
// 最大可容纳15个字符串的字符串。
// 因此小的字符串可以完整地存放在该string对象中，
// 这一特性通常被称为小字符串优化特性。
// 当一个string的容量超过15时，
// 该内存的起始部分被当作一个指向一块动态分配的内存的指针，
// 而该string的值就放在这块内存中。

// 总结如下：
// 1.string的值可能会被引用计数，也可能不会。
//   很多实现在默认的情况下使用引用计数，但通常提供了关闭默认选择的方法，
//   往往是通过预处理宏来做到这一点。
// 2.string对象的大小的范围可以使一个char*指针的大小的1倍到7倍。
// 3.创建一个新的字符串值可能需要零次、一次或两次动态分配内存。
// 4.string对象可能共享，也可能不共享其大小和容量信息。
// 5.string可能支持，也可能不支持针对单个对象的分配子。
// 6.不同的实现对字符内存的最小分配单位有不同的策略。

int main() { return 0; }