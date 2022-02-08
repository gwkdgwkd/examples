# 分治算法
分治算法中，“分治”即“分而治之”的意思。分治算法解决问题的思路是：
先将整个问题拆分成多个相互独立且数据量更少的小问题，通过逐一解决这些简单的小问题，最终找到解决整个问题的方案。
所谓问题间相互独立，简单理解就是每个问题都可以单独处理，不存在“谁先处理，谁后处理”的次序问题。

分治算法解决问题的过程需要经历3个阶段，分别是：
1. 分：将整个问题划分成多个相对独立、涉及数据量更少的小问题，有些小问题还可以划分成很多更小的问题，直至每个问题都不可再分；
2. 治：逐个解决所有的小问题；
3. 合并：将所有小问题的解决方案合并到一起，找到解决整个问题的方案。

# 分治算法的利弊
分治算法中，“分而治之”的小问题之间是相互独立的，处理次序不分先后，因此可以采用“并行”的方式让计算机同时处理多个小问题，提高问题的解决效率。
分治算法的弊端也很明显，该算法经常和递归算法搭配使用，整个解决问题的过程会耗费较多的时间和内存空间，严重时还可能导致程序运行崩溃。

# 分治算法的应用场景
分治算法解决的经典问题有很多，包括汉诺塔问题、寻找数列中最大值和最小值的问题等等。
分治算法还和其它算法搭配使用，比如二分查找算法、归并排序算法、快速排序算法等。
