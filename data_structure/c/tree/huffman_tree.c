#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// 赫夫曼树，别名哈夫曼树、最优树以及最优二叉树。

//            o
//           / \
//         a7   o
//             / \
//           b5   o
//               / \
//             c2   d4

// 哈夫曼树相关的几个名词
// 1.路径：在一棵树中，一个结点到另一个结点之间的通路，称为路径。
//   从根结点到结点a之间的通路就是一条路径。
// 2.路径长度：在一条路径中，每经过一个结点，路径长度都要加1。
//   在一棵树中，规定根结点所在层数为1层，那么从根结点到第i层结点的路径长度为i-1。
//   从根结点到结点c的路径长度为3。
// 结点的权：给每一个结点赋予一个新的数值，被称为这个结点的权。结点a的权为7，结点b的权为5。
// 结点的带权路径长度：指的是从根结点到该结点之间的路径长度与该结点的权的乘积。
// 结点b的带权路径长度为2*5=10。
// 树的带权路径长度为树中所有叶子结点的带权路径长度之和，通常记作“WPL”。
// WPL=7*1+5*2+2*3+4*3

// 什么是哈夫曼树
// 当用n个结点（都做叶子结点且都有各自的权值）试图构建一棵树时，如果构建的这棵树的带权路径长度最小，
// 称这棵树为最优二叉树，有时也叫赫夫曼树或者哈夫曼树。
// 在构建哈弗曼树时，要使树的带权路径长度最小，只需要遵循一个原则，那就是：权重越大的结点离树根越近。

// 构建哈夫曼树的过程
// 对于给定的有各自权值的n个结点，构建哈夫曼树有一个行之有效的办法：
// 1.在n个权值中选出两个最小的权值，对应的两个结点组成一个新的二叉树，
//   且新二叉树的根结点的权值为左右孩子权值的和；
// 2.在原有的n个权值中删除那两个最小的权值，同时将新的权值加入到n–2个权值的行列中，以此类推；
// 3.重复1和2，直到所以的结点构建成了一棵二叉树为止，这棵树就是哈夫曼树。

// 构建哈夫曼树时，首先需要确定树中结点的构成。
// 由于哈夫曼树的构建是从叶子结点开始，不断地构建新的父结点，直至树根，所以结点中应包含指向父结点的指针。
// 但是在使用哈夫曼树时是从树根开始，根据需求遍历树中的结点，因此每个结点需要有指向其左孩子和右孩子的指针。
typedef struct {
  int weight;               // 结点权重
  int parent, left, right;  // 父结点、左孩子、右孩子在数组中的位置下标
} HTNode, *HuffmanTree;

// 构建哈弗曼树的算法实现
// 构建哈夫曼树时，需要每次根据各个结点的权重值，筛选出其中值最小的两个结点，然后构建二叉树。
// 查找权重值最小的两个结点的思路是：
// 从树组起始位置开始，首先找到两个无父结点的结点（说明还未使用其构建成树），
// 然后和后续无父结点的结点依次做比较，有两种情况需要考虑：
// 1.如果比两个结点中较小的那个还小，就保留这个结点，删除原来较大的结点；
// 2.如果介于两个结点权重值之间，替换原来较大的结点；

// HT数组中存放的哈夫曼树，end表示HT数组中存放结点的最终位置，
// s1和s2传递的是HT数组中权重值最小的两个结点在数组中的位置
void Select(HuffmanTree HT, int end, int *s1, int *s2) {
  int min1, min2;
  // 遍历数组初始下标为 1
  int i = 1;
  // 找到还没构建树的结点
  while (HT[i].parent != 0 && i <= end) {
    i++;
  }
  min1 = HT[i].weight;
  *s1 = i;

  i++;
  while (HT[i].parent != 0 && i <= end) {
    i++;
  }
  // 对找到的两个结点比较大小，min2为大的，min1为小的
  if (HT[i].weight < min1) {
    min2 = min1;
    *s2 = *s1;
    min1 = HT[i].weight;
    *s1 = i;
  } else {
    min2 = HT[i].weight;
    *s2 = i;
  }
  // 两个结点和后续的所有未构建成树的结点做比较
  for (int j = i + 1; j <= end; j++) {
    // 如果有父结点，直接跳过，进行下一个
    if (HT[j].parent != 0) {
      continue;
    }
    // 如果比最小的还小，将min2=min1，min1赋值新的结点的下标
    if (HT[j].weight < min1) {
      min2 = min1;
      min1 = HT[j].weight;
      *s2 = *s1;
      *s1 = j;
    }
    // 如果介于两者之间，min2赋值为新的结点的位置下标
    else if (HT[j].weight >= min1 && HT[j].weight < min2) {
      min2 = HT[j].weight;
      *s2 = j;
    }
  }
}

// HT为地址传递的存储哈夫曼树的数组，w为存储结点权重值的数组，n为结点个数
void CreateHuffmanTree(HuffmanTree *HT, int *w, int n) {
  if (n <= 1) return;  // 如果只有一个编码就相当于0
  int m = 2 * n - 1;   // 哈夫曼树总节点数，n就是叶子结点
  *HT = (HuffmanTree)malloc((m + 1) * sizeof(HTNode));  // 0号位置不用
  HuffmanTree p = *HT;
  // 初始化哈夫曼树中的所有结点
  for (int i = 1; i <= n; i++) {
    (p + i)->weight = *(w + i - 1);
    (p + i)->parent = 0;
    (p + i)->left = 0;
    (p + i)->right = 0;
  }
  // 从树组的下标n+1开始初始化哈夫曼树中除叶子结点外的结点
  for (int i = n + 1; i <= m; i++) {
    (p + i)->weight = 0;
    (p + i)->parent = 0;
    (p + i)->left = 0;
    (p + i)->right = 0;
  }
  // 构建哈夫曼树
  for (int i = n + 1; i <= m; i++) {
    int s1, s2;
    Select(*HT, i - 1, &s1, &s2);
    (*HT)[s1].parent = (*HT)[s2].parent = i;
    (*HT)[i].left = s1;
    (*HT)[i].right = s2;
    (*HT)[i].weight = (*HT)[s1].weight + (*HT)[s2].weight;
  }
}

// 哈夫曼编码就是在哈夫曼树的基础上构建的，这种编码方式最大的优点就是用最少的字符包含最多的信息内容。
// 根据发送信息的内容，通过统计文本中相同字符的个数作为每个字符的权值，建立哈夫曼树。
// 对于树中的每一个子树，统一规定其左孩子标记为0，右孩子标记为1。
// 这样，用到哪个字符时，从哈夫曼树的根结点开始，依次写出经过结点的标记，最终得到的就是该结点的哈夫曼编码。
// 文本中字符出现的次数越多，在哈夫曼树中的体现就是越接近树根。编码的长度越短。
// 权值越大，表示此字符在文件中出现的次数越多，那么，为了实现用最少的字符包含最多的内容，
// 就应该给出现次数越多的字符，分配的哈弗曼编码越短。
// 使用程序求哈夫曼编码有两种方法：
// 1.从叶子结点一直找到根结点，逆向记录途中经过的标记。
// 2.从根结点出发，一直到叶子结点，记录途中经过的标记。

typedef char **HuffmanCode;
// HT为哈夫曼树，HC为存储结点哈夫曼编码的二维动态数组，n为结点的个数
void HuffmanCoding1(HuffmanTree HT, HuffmanCode *HC, int n) {
  *HC = (HuffmanCode)malloc((n + 1) * sizeof(char *));
  // 存放结点哈夫曼编码的字符串数组
  char *cd = (char *)malloc(n * sizeof(char));
  cd[n - 1] = '\0';  // 字符串结束符

  for (int i = 1; i <= n; i++) {
    // 从叶子结点出发，得到的哈夫曼编码是逆序的，需要在字符串数组中逆序存放
    int start = n - 1;
    // 当前结点在数组中的位置
    int c = i;
    // 当前结点的父结点在数组中的位置
    int j = HT[i].parent;
    // 一直寻找到根结点
    while (j != 0) {
      // 如果该结点是父结点的左孩子则对应路径编码为0，否则为右孩子编码为1
      if (HT[j].left == c)
        cd[--start] = '0';
      else
        cd[--start] = '1';
      // 以父结点为孩子结点，继续朝树根的方向遍历
      c = j;
      j = HT[j].parent;
    }
    // 跳出循环后，cd数组中从下标start开始，存放的就是该结点的哈夫曼编码
    (*HC)[i] = (char *)malloc((n - start) * sizeof(char));
    strcpy((*HC)[i], &cd[start]);
  }
  // 使用malloc申请的cd动态数组需要手动释放
  free(cd);
}

// HT为哈夫曼树，HC为存储结点哈夫曼编码的二维动态数组，n为结点的个数
void HuffmanCoding2(HuffmanTree HT, HuffmanCode *HC, int n) {
  *HC = (HuffmanCode)malloc((n + 1) * sizeof(char *));
  int m = 2 * n - 1;
  int p = m;
  int cdlen = 0;
  char *cd = (char *)malloc(n * sizeof(char));
  // 将各个结点的权重用于记录访问结点的次数，首先初始化为0
  for (int i = 1; i <= m; i++) {
    HT[i].weight = 0;
  }
  // 一开始p初始化为m，也就是从树根开始。一直到p为0
  while (p) {
    // 如果当前结点一次没有访问，进入这个if语句
    if (HT[p].weight == 0) {
      HT[p].weight = 1;  // 重置访问次数为1
      // 如果有左孩子，则访问左孩子，并且存储走过的标记为0
      if (HT[p].left != 0) {
        p = HT[p].left;
        cd[cdlen++] = '0';
      } else if (HT[p].right == 0) {
        // 当前结点没有左孩子，也没有右孩子，说明为叶子结点，直接记录哈夫曼编码
        (*HC)[p] = (char *)malloc((cdlen + 1) * sizeof(char));
        cd[cdlen] = '\0';
        strcpy((*HC)[p], cd);
      }
    } else if (HT[p].weight == 1) {
      // 如果weight为1，说明访问过一次，即是从其左孩子返回的
      HT[p].weight = 2;  // 设置访问次数为2
      // 如果有右孩子，遍历右孩子，记录标记值1
      if (HT[p].right != 0) {
        p = HT[p].right;
        cd[cdlen++] = '1';
      }
    } else {  // 如果访问次数为2，说明左右孩子都遍历完了，返回父结点
      HT[p].weight = 0;
      p = HT[p].parent;
      --cdlen;
    }
  }
}

// 打印哈夫曼编码的函数
void PrintHuffmanCode(HuffmanCode htable, int *w, int n) {
  printf("Huffman code : \n");
  for (int i = 1; i <= n; i++) printf("%d code = %s\n", w[i - 1], htable[i]);
}

//            o
//           /  \
//         o     o
//        / \   / \
//       6   7 o   8
//            / \ 
//           2   5

int main(void) {
  int w[5] = {2, 8, 7, 6, 5};
  int n = 5;
  HuffmanTree htree;
  HuffmanCode htable;
  CreateHuffmanTree(&htree, w, n);
  // HuffmanCoding1(htree, &htable, n);
  HuffmanCoding2(htree, &htable, n);
  PrintHuffmanCode(htable, w, n);
  // Huffman code :
  // 2 code = 100
  // 8 code = 11
  // 7 code = 01
  // 6 code = 00
  // 5 code = 101

  return 0;
}