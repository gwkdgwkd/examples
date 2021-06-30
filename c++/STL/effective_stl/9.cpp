#include <iostream>

using namespace std;

// 慎重选择删除元素的方法

// 删除容器中所有指定值的元素，完成这一任务的方式随容器类型而异，没有对所有容器类型都适用的方式。
// 连续内存的容器（vector、deque或string），那么最好的办法是使用erase-remove:
// c.erase(remove(c.begin(), c.end(), 1963), c.end());
// 对list，上面的办法一样适用，但list的成员函数remove更加有效：c.remove(1963);
// 对于标准关联容器（set、multiset、map或multimap）时，使用任何名为remove的操作都是完全错误的。
// 这样的容器没有名为remove的成员函数，使用remove算法可能会覆盖容器的值，同时可能会破坏容器。
// 对于关联容器，解决问题的正确方法是调用erase：cerase(1963);
// 这样做不仅正确，而且高效，只需要对数时间的开销。关联容器的erase成员函数还有另外一个优点，它是基于等价而不是相等的。

// 使用下面的判断式：
bool badValue(int);
// 对于序列容器（vector、string、deque和list），把每个remove的调用换成remove_if就可以了：
// c.erase(remove_if(c.begin(), c.end(), badValue), c.end());
// c.remove_if(badValue);  // 这是删除使badValue返回true的对象的最好办法

// 对于标准关联容器，有两种办法，一种易于编码，另一种则效率更高。
// 简单但效率稍低的办法是，利用remove_copy_if把需要的值拷贝到一个新容器中，然后把原来容器的内容和新容器的内容互换：
// AssocContainer<int> c;
// AssocContainer<int> goodValues;
// remove_copy_if(c.begin(),c.end(),inserter(goodValues,goodValues.end()),badValue);
// c.swap(goodValues);
// 这种办法的缺点是需要拷贝所有不被删除的元素。

// 也可以直接从原始的容器中删除元素，从而降低代价。
// 关联容器没有提供类似remove_if的成员函数，所以必须写一个循环来遍历元素，并在过程中删除元素。
// 立刻想到的代码很少恰好是能工作的代码。比如，下面是很多成员首先能想到的：
// AssocContainer<int> c;
// for (AssocContainer<int>::iterator i = c.begin(); i != c.end(); ++i) {
//   if (badValue(*i)) {
//     c.erase(i);  // 一旦c.erase(i)返回，i就成为无效值。
//   }
// }
// 这会导致不确定的行为。当容器中的一个元素被删除时，指向该元素的所有迭代器都将变得无效。
// 为了避免这个问题，要确保在调用erase之前，有一个迭代器指向容器中的下一个元素。最简单的办法是，调用时对i使用后缀递增：
// for (AssocContainer<int>::iterator i = c.begin(); i != c.end();) {
//   if (badValue(*i)) {
//     c.erase(i++);
//   } else {
//     ++i;
//   }
// }

// 不仅要删除，还要在每次元素被删除时，都向一个日志文件中写一条信息。对于关联容器，这非常简单：
// ofstream logFile;
// AssocContainer<int> c;
// for (AssocContainer<int>::iterator i = c.begin(); i != c.end();) {
//   if (badValue(*i)) {
//     logFile << "Erasing " << *i << '\n';
//     c.erase(i++);
//   } else {
//     ++i;
//   }
// }
// 但对于vector、string和deque，不能再使用erase-remove惯用方法了，因为无法向日志中写信息。
// 而且也不能使用关联容器的方法，因为会导致不确定的行为。
// 这类容器调用erase不仅会使指向被删除元素的迭代器失效，也会使被删除元素之后的所有迭代器都无效。
// 对于vector、string和deque，必须采取不同的策略，尤其要利用erase的返回值。
// 一旦erase完成，它是指向紧随被删除元素的下一个元素的有效迭代器。可以这样写：
// for (SeqContainer<int>::iterator i = c.begin(); i != c.end();) {
//   if (badValue(*i)) {
//     logFile << "Erasing " << *i << '\n';
//     i = c.erase(i);  // 把erase的返回值赋给i，使i的值保持有效
//   } else {
//     ++i;
//   }
// }

// 对于标准关联容器，erase的返回类型是void。对于这类容器，得使用将传给erase的迭代器进行后缀递增的技术。
// 对于list，遍历和删除时，可以把list当作vector/string/deque来对对待，也可以把它当作关联容器来对待。
// 两种方式对list都适用，一般的惯例是对list采取和vector、string和deque相同的方式。

// 总结：
// 要删除容器中有特定值的所有对象：
//  如果容器是vector、string或deque，则使用erase-remove习惯用法。
//  如果容器是list，则使用list::remove。
//  如果容器是一个标准关联容器，则使用它的erase成员函数。
// 要删除容器中满足特定判别式（条件）的所有对象：
//  如果容器是vector、string或deque，则使用erase-remove_if习惯用法。
//  如果容器是list，则使用list::remove_if。
//  如果容器是一个标准关联容器，则使用remove_copy_if和swap，或者写一个循环来遍历容器中的元素，记住当把迭代器传给erase时，进行后缀递增。
// 要在循环内部做某些（除了删除对象之外的）操作：
//  如果容器是一个标准序列容器，则写一个循环来遍历容器中的元素，记住每次调用erase时，要用它的返回值更新迭代器。
//  如果容器是一个标准关联容器，则写一个循环来遍历容器中的元素，记住当把迭代器传给erase时，要对迭代器做后缀递增。

int main() { return 0; }