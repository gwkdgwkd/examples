#include <stdio.h>

// 动态内存管理机制，主要包含两方面内容，用户申请内存空间时，系统如何分配；用户使用内存空间完成后，系统如何及时回收。
// 另外，内存不断分配与回收的过程，会产生诸多内存碎片，但通过利用数据结构，内存碎片化的问题能够得到有效的解决。
// 其实，无论是那种操作系统，其内存管理机制都使用了大量数据结构的知识，大大提高内存管理的效率。

// 数据结构之动态内存管理机制
// 对于计算机中的内存来说，称已经分配给用户的的内存区统称为“占用块”；还未分配出去的内存区统称为“空闲块”或者“可利用空间块”。
// 对于初始状态下的内存来说，整个空间都是一个空闲块（在编译程序中称为“堆”）。但是随着不同的用户不断地提出存储请求，系统依次分配。
// 整个内存区就会分割成两个大部分：低地址区域会产生很多占用块；高地址区域还是空闲块。
// 但是当某些用户运行结束，所占用的内存区域就变成了空闲块。此时，就形成了占用块和空闲块犬牙交错的状态。当后续用户请求分配内存时，系统有两种分配方式：
//  1 系统继续利用高地址区域的连续空闲块分配给用户，不去理会之前分配给用户的内存区域的状态。
//    直到分配无法进行，也就是高地址的空闲块不能满足用户的需求时，系统才会去回收之前的空闲块，重新组织继续分配；
//  2 当用户运行一结束，系统马上将其所占空间进行回收。当有新的用户请求分配内存时，系统遍历所有的空闲块，从中找出一个合适的空闲块分配给用户。
// 当采用第2种方式时，系统需要建立一张记录所有空闲块信息的表。表的形式有两种：目录表和链表。
//  目录表：表中每一行代表一个空闲块，由三部分组成：
//   初始地址：记录每个空闲块的起始地址。
//   空闲块大小：记录每个空闲块的内存大小。
//   使用情况：记录每个空闲块是否存储被占用的状态。
//  链表：表中每个结点代表一个空闲块，每个结点中需要记录空闲块的使用情况、大小和连接下一个空闲块的指针域。
// 由于链表中有指针的存在，所以结点中不需要记录各内存块的起始地址。
// 系统在不同的环境中运行，根据用户申请空间的不同，存储空闲块的可利用空间表有以下不同的结构：
//  如果每次用户请求的存储空间大小相同，对于此类系统中的内存来说，在用户运行初期就将整个内存存储块按照所需大小进行分割，然后通过链表链接。
//   当用户申请空间时，从链表中摘除一个结点归其使用；用完后再链接到可利用空间表上。
//  每次如果用户申请的都是若干种大小规格的存储空间，针对这种情况可以建立若干个可利用空间表，每一个链表中的结点大小相同。
//   当用户申请某一规格大小的存储空间时，就从对应的链表中摘除一个结点供其使用；用完后链接到相同规格大小的链表中。
//  用户申请的内存的大小不固定，所以造成系统分配的内存块的大小也不确定，回收时，链接到可利用空间表中每个结点的大小也各不一样。
// 第2种情况下容易面临的问题是：如果同用户申请空间大小相同的链表中没有结点时，就需要找结点更大的链表，从中取出一个结点，一部分给用户使用，剩余部分插入到相应大小的链表中；
// 回收时，将释放的空闲块插入到大小相同的链表中去。如果没有比用户申请的内存空间相等甚至更大的结点时，就需要系统重新组织一些小的连续空间，然后给用户使用。
// 分配存储空间的方式。由于链表中各结点的大小不一，在用户申请内存空间时，就需要从可利用空间表中找出一个合适的结点，有三种查找的方法：
//  首次拟合法：在可利用空间表中从头开始依次遍历，将找到的第一个内存不小于用户申请空间的结点分配给用户，剩余空间仍留在链表中；回收时只要将释放的空闲块插入在链表的表头即可。
//  最佳拟合法：和首次拟合法不同，最佳拟合法是选择一块内存空间不小于用户申请空间，但是却最接近的一个结点分配给用户。为了实现这个方法，首先要将链表中的各个结点按照存储空间
//            的大小进行从小到大排序，由此，在遍历的过程中只需要找到第一块大于用户申请空间的结点即可进行分配；用户运行完成后，需要将空闲块根据其自身的大小插入到链表的相应位置。
//  最差拟合法：和最佳拟合法正好相反，该方法是在不小于用户申请空间的所有结点中，筛选出存储空间最大的结点，从该结点的内存空间中提取出相应的空间给用户使用。为了实现这一方法，
//            可以在开始前先将可利用空间表中的结点按照存储空间大小从大到小进行排序，第一个结点自然就是最大的结点。回收空间时，同样将释放的空闲块插入到相应的位置上。
// 以上三种方法各有所长：
//  最佳拟合法由于每次分配相差不大的结点给用户使用，所以会生成很多存储空间特别小的结点，以至于根本无法使用，使用过程中，链表中的结点存储大小发生两极分化，大的很大，小的很小。
//   该方法适用于申请内存大小范围较广的系统
//  最差拟合法，由于每次都是从存储空间最大的结点中分配给用户空间，所以链表中的结点大小不会起伏太大。依次适用于申请分配内存空间较窄的系统。
//  首次拟合法每次都是随机分配。在不清楚用户申请空间大小的情况下，使用该方法分配空间。
// 同时，三种方法中，最佳拟合法相比于其它两种方式，无论是分配过程还是回收过程，都需要遍历链表，所有最费时间。
// 无论使用以上三种分配方式中的哪一种，最终内存空间都会成为一个一个特别小的内存空间，对于用户申请的空间的需求，单独拿出任何一个结点都不能够满足。
// 但是并不是说整个内存空间就不够用户使用。在这种情况下，就需要系统在回收的过程考虑将地址相邻的空闲块合并。

// 一种解决系统中内存碎片过多而无法使用的方法——边界标识法。
// 在使用边界标识法的系统管理内存时，可利用空间表中的结点的构成，每个结点中包含 3 个区域，head 域、foot 域 和 space 域：
//  space 域表示为该内存块的大小，它的大小通过head域中的size值表示。
//  head 域中包含有4部分：llink和rlink分别表示指向当前内存块结点的直接前驱和直接后继。tag值用于标记当前内存块的状态，是占用块（用1表示）还是空闲块（用0表示）。
//                     size用于记录该内存块的存储大小。
//  foot 域中包含有2部分：uplink是指针域，用于指向内存块本身，通过 uplink 就可以获取该内存块所在内存的首地址。tag同head域中的tag相同，都是记录内存块状态的。
// 注意：head域和foot域在本节中都假设只占用当前存储块的1个存储单位的空间，对于该结点整个存储空间来说，可以忽略不计。
//      也就是说，在可利用空间表中，知道下一个结点的首地址，该值减1就可以找到当前结点的foot域。
// 使用边界标识法的可利用空间表本身是双向循环链表，每个内存块结点都有指向前驱和后继结点的指针域。
// typedef struct WORD {
//   union {
//     struct WORD *llink;   // 指向直接前驱
//     struct WORD *uplink;  // 指向结点本身
//   };
//   int tag;             // 标记域,0表示为空闲块；1表示为占用块
//   int size;            // 记录内存块的存储大小
//   struct WORD *rlink;  // 指向直接后继
//   OtherType other;     // 内存块可能包含的其它的部分
// } WORD, head, foot, *Space;
// 分配算法
// 当用户申请空间时，系统可以采用 3 种分配方法中的任何一种。但在不断地分配的过程中，会产生一些容量极小以至无法利用的空闲块，这些不断生成的小内存块就会减慢遍历分配的速度。
//  3种分配方法分别为：首部拟合法、最佳拟合法和最差拟合法。
// 针对这种情况，解决的措施是：
//  选定一个常量e，每次分配空间时，判断当前内存块向用户分配空间后，如果剩余部分的容量比e小，则将整个内存块全部分配给用户。
//  采用头部拟合法进行分配时，如果每次都从pav指向的结点开始遍历，在若干次后，会出现存储量小的结点密集地分布在pav结点附近的情况，严重影响遍历的时间。
//    解决办法就是：在每次分配空间后，让pav指针指向该分配空间结点的后继结点，然后从新的pav指向的结点开始下一次的分配。
// 回收算法
// 在用户活动完成，系统需要立即回收被用户占用的存储空间，以备新的用户使用。回收算法中需要解决的问题是：在若干次分配操作后，可利用空间块中会产生很多存储空间很小以致无法使用的空闲块。但是经过回收用户释放的空间后，可利用空间表中可能含有地址相邻的空闲块，回收算法需要将这些地址相邻的空闲块合并为大的空闲块供新的用户使用。
// 合并空闲块有3种情况：
//  该空闲块的左边有相邻的空闲块可以进行合并；
//  该空闲块的右边用相邻的空闲块可以进行合并；
//  该空闲块的左右两侧都有相邻的空闲块可以进行合并；
// 判断当前空闲块左右两侧是否为空闲块的方法是：对于当前空闲块p，p-1就是相邻的低地址处的空闲块的foot域，如果foot域中的tag值为0，表明其为空闲块；
//  p+p->size表示的是高地址处的块的head域，如果head域中的tag值为0，表明其为空闲块。

// 伙伴系统本身是一种动态管理内存的方法，和边界标识法的区别是：使用伙伴系统管理的存储空间，无论是空闲块还是占用块，大小都是2的n次幂（n为正整数）。
// 伙伴系统中可利用空间表中的结点构成:
//  llink和rlink为结点类型的指针域，分别用于指向直接前驱和直接后继结点。
//  tag值：用于标记内存块的状态，是占用块（用1表示）还是空闲块（用0表示）
//  kval：记录该存储块的容量。由于系统中各存储块都是2的m幂次方，所以kval记录m的值。
// typedef struct WORD_b {
//   struct WORD_b *llink;  // 指向直接前驱
//   int tag;               // 记录该块是占用块还是空闲块
//   int kval;              // 记录该存储块容量大小为2的多少次幂
//   struct WORD_b *rlink;  // 指向直接后继
//   OtherType other;       // 记录结点的其它信息
// } WORD_b, head;
// 在伙伴系统中，由于系统会不断地接受用户的内存申请的请求，所以会产生很多大小不同但是都是容量为2m的内存块，所以为了在分配的时候查找方便，系统采用将大小相同的各自建立一个链表。
// 对于初始容量为2m的一整块存储空间来说，形成的链表就有可能有m+1个，为了更好的对这些链表进行管理，系统将这m+1个链表的表头存储在数组中，就类似于邻接表的结构。
// #define m 16  // 设定m的初始值
// typedef struct HeadNode {
//   int nodesize;     // 记录该链表中存储的空闲块的大小
//   WORD_b* first;    // 相当于链表中的next指针的作用
// } FreeList[m + 1];  // 一维数组
// 伙伴系统的分配算法很简单。假设用户向系统申请大小为n的存储空间，若2^(k-1) < n <= 2^k，此时就需要查看可利用空间表中大小为2k的链表中有没有可利用的空间结点：
//  如果该链表不为NULL，可以直接采用头插法从头部取出一个结点，提供给用户使用；
//  如果大小为2^k的链表为NULL，就需要依次查看比2^k大的链表，找到后从链表中删除，截取相应大小的空间给用户使用，剩余的空间，根据大小插入到相应的链表中。
// 无论使用什么内存管理机制，在内存回收的问题上都会面临一个共同的问题：如何把回收的内存进行有效地整合，伙伴系统也不例外。
// 当用户申请的内存块不再使用时，系统需要将这部分存储块回收，回收时需要判断是否可以和其它的空闲块进行合并。
// 在寻找合并对象时，伙伴系统和边界标识法不同，在伙伴系统中每一个存储块都有各自的“伙伴”，当用户释放存储块时只需要判断该内存块的伙伴是否为空闲块，如果是则将其合并，
//  然后合并的新的空闲块还需要同其伙伴进行判断整合。反之直接将存储块根据大小插入到可利用空间表中即可。
// 使用伙伴系统进行存储空间的管理过程中，在用户申请空间时，由于大小不同的空闲块处于不同的链表中，所以分配完成的速度会更快，算法相对简单。
// 回收存储空间时，对于空闲块的合并，不是取决于该空闲块的相邻位置的块的状态；而是完全取决于其伙伴块。所以即使其相邻位置的存储块时空闲块，但是由于两者不是伙伴的关系，
// 所以也不会合并。这也就是该系统的缺点之一：由于在合并时只考虑伙伴，所以容易产生存储的碎片。

// 无用单元收集（垃圾回收机制）
// 在实际使用过程中，有时会因为用户申请了空间，但是在使用完成后没有向系统发出释放的指令，导致存储空间既没有被使用也没有被回收，变为了无用单元或者会产生悬挂访问的问题。
// 什么是无用单元？简单来讲，无用单元是一块用户不再使用，但是系统无法回收的存储空间。例如在C语言中，用户可以通过malloc和free两个功能函数来动态申请和释放存储空间。
// 当用户使用malloc申请的空间使用完成后，没有使用free函数进行释放，那么该空间就会成为无用单元。
// 悬挂访问也很好理解：假设使用malloc申请了一块存储空间，有多个指针同时指向这块空间，当其中一个指针完成使命后，私自将该存储空间使用free释放掉，导致其他指针处于悬空状态，
// 如果释放掉的空间被再分配后，再通过之前的指针访问，就会造成错误。数据结构中称这种访问为悬挂访问。
// 解决存储空间可能成为无用单元或者产生悬挂访问的方法有两个：
//  每个申请的存储空间设置一个计数域，这个计数域记录的是指向该存储空间的指针数目，只有当计数域的值为0时，该存储空间才会被释放。
//  在程序运行时，所有的存储空间无论是处于使用还是空闲的状态，一律不回收，当系统中的可利用空间表为空时，将程序中断，对当前不在使用状态的存储空间一律回收，全部链接成一个新
//   的可利用空间表后，程序继续执行。
// 第二种方法中，在程序运行过程中很难找出此时哪些存储空间是空闲的。解决这个问题的办法是：找当前正在被占用的存储空间，只需要从当前正在工作的指针变量出发依次遍历，就可以找到
//  当前正在被占用的存储空间，剩余的自然就是此时处于空闲状态的存储空间。

// 无论是边界标识法还是伙伴系统，都是以将空闲的存储空间链接成一个链表，即可利用空间表，对存储空间进行分配和回收。
// 另外一种动态内存管理的方法，使用这种方式在整个内存管理过程中，不管哪个时间段，所有未被占用的空间都是地址连续的存储区。
// 这些地址连续的未被占用的存储区在编译程序中称为堆。
