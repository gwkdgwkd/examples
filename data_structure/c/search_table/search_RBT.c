#include <stdio.h>
#include <stdlib.h>

// 红黑树（R-B TREE，全称：Red-Black Tree），本身是一棵二叉查找树，在其基础上附加了两个要求：
// 1.树中的每个结点增加了一个用于存储颜色的标志域；
// 2.树中没有一条路径比其他任何路径长出两倍，整棵树要接近于“平衡”的状态。
// 这里所指的路径，指的是从任何一个结点开始，一直到其子孙的叶子结点的长度；
// 接近于平衡：红黑树并不是平衡二叉树，只是由于对各路径的长度之差有限制，所以近似于平衡的状态。
// 红黑树对于结点的颜色设置不是任意的，需满足以下性质的二叉查找树才是红黑树：
// 1.树中的每个结点颜色不是红的，就是黑的；
// 2.根结点的颜色是黑的；
// 3.所有为nil的叶子结点的颜色是黑的；（注意：叶子结点说的只是为空（nil或NULL）的叶子结点！）
// 4.如果此结点是红的，那么它的两个孩子结点全部都是黑的；
// 5.对于每个结点，从该结点到到该结点的所有子孙结点的所有路径上包含有相同数目的黑结点；
// 红黑树中每个结点都有各自的黑高度，整棵树也有自己的黑高度，即为根结点的黑高度。

// 对于一棵具有n个结点的红黑树，树的高度至多为：2lg(n+1)。
// 由此可推出红黑树进行查找操作时的时间复杂度为O(lgn)，因为对于高度为h的二叉查找树的运行时间为O(h)，
// 而包含有n个结点的红黑树本身就是最高为lgn（简化之后）的查找树（h=lgn），所以红黑树的时间复杂度为O(lgn)。
// 红黑树本身作为一棵二叉查找树，所以其任务就是用于动态表中数据的插入和删除的操作。
// 在进行该操作时，避免不了会破坏红黑树的结构，此时就需要进行适当的调整，使其重新成为一棵红黑树，
// 可以从两个方面着手对树进行调整：
// 1.调整树中某些结点的指针结构；
// 2.调整树中某些结点的颜色；

// 当使用红黑树进行插入或者删除结点的操作时，可能会破坏红黑树的5条性质，从而变成了一棵普通树，
// 此时就可以通过对树中的某些子树进行旋转，从而使整棵树重新变为一棵红黑树。
// 旋转操作分为左旋和右旋，同二叉排序树转平衡二叉树的旋转原理完全相同。

// 当创建一个红黑树或者向已有红黑树中插入新的数据时，只需要按部就班地执行以下3步：
// 1.由于红黑树本身是一棵二叉查找树，所以在插入新的结点时，完全按照二叉查找树插入结点的方法，找到新结点插入的位置；
// 2.将新插入的结点结点初始化，颜色设置为红色后插入到指定位置；（将新结点初始化为红色插入后，不会破坏红黑树第5条的性质）
// 3.由于插入新的结点，可能会破坏红黑树第4条的性质（若其父结点颜色为红色，就破坏了红黑树的性质），
//   此时需要调整二叉查找树，想办法通过旋转以及修改树中结点的颜色，使其重新成为红黑树！

// 在红黑树中删除结点，思路更简单，只需要完成2步操作：
// 1.将红黑树按照二叉查找树删除结点的方法删除指定结点；
// 2.重新调整删除结点后的树，使之重新成为红黑树；（还是通过旋转和重新着色的方式进行调整）

typedef enum { RED, BLACK } ColorType;
typedef struct RB_TREE {
  int key;
  struct RB_TREE* left;
  struct RB_TREE* right;
  struct RB_TREE* p;
  ColorType color;
} RB_TREE;
typedef struct RBT_Root {
  RB_TREE* root;
  RB_TREE* nil;
} RBT_Root;
RBT_Root* rbTree_init(void);
void rbTree_insert(RBT_Root** T, int k);
void rbTree_delete(RBT_Root** T, int k);
void rbTree_transplant(RBT_Root* T, RB_TREE* u, RB_TREE* v);
void rbTree_left_rotate(RBT_Root* T, RB_TREE* x);
void rbTree_right_rotate(RBT_Root* T, RB_TREE* x);
void rbTree_inPrint(RBT_Root* T, RB_TREE* t);
void rbTree_prePrint(RBT_Root* T, RB_TREE* t);
void rbTree_print(RBT_Root* T);
RB_TREE* rbt_findMin(RBT_Root* T, RB_TREE* t);
RB_TREE* rbt_findMin(RBT_Root* T, RB_TREE* t) {
  if (t == T->nil) {
    return T->nil;
  }
  while (t->left != T->nil) {
    t = t->left;
  }
  return t;
}
RBT_Root* rbTree_init(void) {
  RBT_Root* T;
  T = (RBT_Root*)malloc(sizeof(RBT_Root));
  T->nil = (RB_TREE*)malloc(sizeof(RB_TREE));
  T->nil->color = BLACK;
  T->nil->left = T->nil->right = NULL;
  T->nil->p = NULL;
  T->root = T->nil;
  return T;
}
void RB_Insert_Fixup(RBT_Root* T, RB_TREE* x) {
  // 首先判断其父结点颜色为红色时才需要调整；为黑色时直接插入即可，不需要调整
  while (x->p->color == RED) {
    // 由于还涉及到其叔叔结点，所以此处需分开讨论，确定父结点是祖父结点的左孩子还是右孩子
    if (x->p == x->p->p->left) {
      RB_TREE* y = x->p->p->right;  // 找到其叔叔结点
      // 如果叔叔结点颜色为红色，此为第1种情况，处理方法为：父结点颜色改为黑色；
      // 叔叔结点颜色改为黑色；
      // 祖父结点颜色改为红色，将祖父结点赋值为当前结点，继续判断；
      if (y->color == RED) {
        x->p->color = BLACK;
        y->color = BLACK;
        x->p->p->color = RED;
        x = x->p->p;
      } else {
        // 反之，如果叔叔结点颜色为黑色，此处需分为两种情况：
        // 1.当前结点时父结点的右孩子；
        // 2.当前结点是父结点的左孩子
        if (x == x->p->right) {
          // 第2种情况：当前结点时父结点的右孩子。
          // 解决方案：将父结点作为当前结点做左旋操作。
          x = x->p;
          rbTree_left_rotate(T, x);
        } else {
          // 第3种情况：当前结点是父结点的左孩子。
          // 解决方案：将父结点颜色改为黑色，祖父结点颜色改为红色，从祖父结点处进行右旋处理。
          x->p->color = BLACK;
          x->p->p->color = RED;
          rbTree_right_rotate(T, x->p->p);
        }
      }
    } else {  // 如果父结点时祖父结点的右孩子，换汤不换药，只需将以上代码部分中的left改为right即可，道理是一样的。
      RB_TREE* y = x->p->p->left;
      if (y->color == RED) {
        x->p->color = BLACK;
        y->color = BLACK;
        x->p->p->color = RED;
        x = x->p->p;
      } else {
        if (x == x->p->left) {
          x = x->p;
          rbTree_right_rotate(T, x);
        } else {
          x->p->color = BLACK;
          x->p->p->color = RED;
          rbTree_left_rotate(T, x->p->p);
        }
      }
    }
  }
  T->root->color = BLACK;
}
// 插入操作分为3步：
// 1.将红黑树当二叉查找树，找到其插入位置；
// 2.初始化插入结点，将新结点的颜色设为红色；
// 3.通过调用调整函数，将二叉查找树重新改为红黑树
void rbTree_insert(RBT_Root** T, int k) {
  // 1.找到其要插入的位置。
  // 解决思路为：从树的根结点开始，通过不断的同新结点的值进行比较，最终找到插入位置
  RB_TREE *x, *p;
  x = (*T)->root;
  p = x;

  while (x != (*T)->nil) {
    p = x;
    if (k < x->key) {
      x = x->left;
    } else if (k > x->key) {
      x = x->right;
    } else {
      printf("\n%d已存在\n", k);
      return;
    }
  }
  // 初始化结点，将新结点的颜色设为红色
  x = (RB_TREE*)malloc(sizeof(RB_TREE));
  x->key = k;
  x->color = RED;
  x->left = x->right = (*T)->nil;
  x->p = p;
  // 对新插入的结点，建立与其父结点之间的联系
  if ((*T)->root == (*T)->nil) {
    (*T)->root = x;
  } else if (k < p->key) {
    p->left = x;
  } else {
    p->right = x;
  }
  // 3.对二叉查找树进行调整
  RB_Insert_Fixup((*T), x);
}
void rbTree_transplant(RBT_Root* T, RB_TREE* u, RB_TREE* v) {
  if (u->p == T->nil) {
    T->root = v;
  } else if (u == u->p->left) {
    u->p->left = v;
  } else {
    u->p->right = v;
  }
  v->p = u->p;
}
void RB_Delete_Fixup(RBT_Root** T, RB_TREE* x) {
  while (x != (*T)->root && x->color == BLACK) {
    if (x == x->p->left) {
      RB_TREE* w = x->p->right;
      // 第1种情况：兄弟结点是红色的
      if (RED == w->color) {
        w->color = BLACK;
        w->p->color = RED;
        rbTree_left_rotate((*T), x->p);
        w = x->p->right;
      }
      // 第2种情况：兄弟是黑色的，并且兄弟的两个儿子都是黑色的。
      if (w->left->color == BLACK && w->right->color == BLACK) {
        w->color = RED;
        x = x->p;
      }
      // 第3种情况
      if (w->left->color == RED && w->right->color == BLACK) {
        w->left->color = BLACK;
        w->color = RED;
        rbTree_right_rotate((*T), w);
        w = x->p->right;
      }
      // 第4种情况
      if (w->right->color == RED) {
        w->color = x->p->color;
        x->p->color = BLACK;
        w->right->color = BLACK;
        rbTree_left_rotate((*T), x->p);
        x = (*T)->root;
      }
    } else {
      RB_TREE* w = x->p->left;
      // 第1种情况
      if (w->color == RED) {
        w->color = BLACK;
        x->p->color = RED;
        rbTree_right_rotate((*T), x->p);
        w = x->p->left;
      }
      // 第2种情况
      if (w->left->color == BLACK && w->right->color == BLACK) {
        w->color = RED;
        x = x->p;
      }
      // 第3种情况
      if (w->left->color == BLACK && w->right->color == RED) {
        w->color = RED;
        w->right->color = BLACK;
        w = x->p->left;
      }
      // 第4种情况
      if (w->right->color == BLACK) {
        w->color = w->p->color;
        x->p->color = BLACK;
        w->left->color = BLACK;
        rbTree_right_rotate((*T), x->p);
        x = (*T)->root;
      }
    }
  }
  x->color = BLACK;  // 最终将根结点的颜色设为黑色
}
void rbTree_delete(RBT_Root** T, int k) {
  if (NULL == (*T)->root) {
    return;
  }
  // 找到要被删除的结点
  RB_TREE* toDelete = (*T)->root;
  RB_TREE* x = NULL;
  // 找到值为k的结点
  while (toDelete != (*T)->nil && toDelete->key != k) {
    if (k < toDelete->key) {
      toDelete = toDelete->left;
    } else if (k > toDelete->key) {
      toDelete = toDelete->right;
    }
  }
  if (toDelete == (*T)->nil) {
    printf("\n%d 不存在\n", k);
    return;
  }
  // 如果两个孩子，就找到右子树中最小的结点，将之代替，然后直接删除该结点即可
  if (toDelete->left != (*T)->nil && toDelete->right != (*T)->nil) {
    RB_TREE* alternative = rbt_findMin((*T), toDelete->right);
    // 这里只对值进行复制，并不复制颜色，以免破坏红黑树的性质
    k = toDelete->key = alternative->key;
    toDelete = alternative;
  }
  // 如果只有一个孩子结点（只有左孩子或只有右孩子），
  // 直接用孩子结点顶替该结点位置即可（没有孩子结点的也走此判断语句）。
  if (toDelete->left == (*T)->nil) {
    x = toDelete->right;
    rbTree_transplant((*T), toDelete, toDelete->right);
  } else if (toDelete->right == (*T)->nil) {
    x = toDelete->left;
    rbTree_transplant((*T), toDelete, toDelete->left);
  }
  // 在删除该结点之前，需判断此结点的颜色：如果是红色，直接删除，不会破坏红黑树；
  // 若是黑色，删除后会破坏红黑树的第5条性质，需要对树做调整。
  if (toDelete->color == BLACK) {
    RB_Delete_Fixup(T, x);
  }
  // 最终可以彻底删除要删除的结点，释放其占用的空间
  free(toDelete);
}
// T表示为树根，x表示需要进行左旋的子树的根结点
void rbTree_left_rotate(RBT_Root* T, RB_TREE* x) {
  RB_TREE* y = x->right;  // 找到根结点的右子树

  x->right = y->left;  // 将右子树的左孩子移动至结点x的右孩子处
  if (x->right !=
      T->nil) {  // 如果x的右子树不是nil，需重新连接 右子树的双亲结点为x
    x->right->p = x;
  }
  y->p = x->p;  // 设置y的双亲结点为x的双亲结点
  // 重新设置y的双亲结点同y的连接，分为2种情况：
  // 1.原x结点本身就是整棵树的数根结点，此时只需要将T指针指向 y；
  // 2.根据y中关键字同其父结点关键字的值的大小，判断y是父结点的左孩子还是右孩子
  if (y->p == T->nil) {
    T->root = y;
  } else if (y->key < y->p->key) {
    y->p->left = y;
  } else {
    y->p->right = y;
  }
  y->left = x;  // 将x连接给y结点的左孩子处
  x->p = y;     // 设置x的双亲结点为y。
}
void rbTree_right_rotate(RBT_Root* T, RB_TREE* x) {
  RB_TREE* y = x->left;
  x->left = y->right;
  if (T->nil != x->left) {
    x->left->p = x;
  }
  y->p = x->p;
  if (y->p == T->nil) {
    T->root = y;
  } else if (y->key < y->p->key) {
    y->p->left = y;
  } else {
    y->p->right = y;
  }
  y->right = x;
  x->p = y;
}
void rbTree_prePrint(RBT_Root* T, RB_TREE* t) {
  if (T->nil == t) {
    return;
  }
  if (t->color == RED) {
    printf("%dR ", t->key);
  } else {
    printf("%dB ", t->key);
  }
  rbTree_prePrint(T, t->left);
  rbTree_prePrint(T, t->right);
}
void rbTree_inPrint(RBT_Root* T, RB_TREE* t) {
  if (T->nil == t) {
    return;
  }
  rbTree_inPrint(T, t->left);
  if (t->color == RED) {
    printf("%dR ", t->key);
  } else {
    printf("%dB ", t->key);
  }
  rbTree_inPrint(T, t->right);
}
// 输出红黑树的前序遍历和中序遍历的结果
void rbTree_print(RBT_Root* T) {
  printf("前序遍历 ：");
  rbTree_prePrint(T, T->root);
  printf("\n");
  printf("中序遍历 ：");
  rbTree_inPrint(T, T->root);
  printf("\n");
}

// 红黑树，虽隶属于二叉查找树，但是二叉查找树的时间复杂度会受到其树深度的影响，
// 而红黑树可以保证在最坏情况下的时间复杂度仍为O(logn)。
// 当数据量多到一定程度时，使用红黑树比二叉查找树的效率要高。
// 同平衡二叉树相比较，红黑树没有像平衡二叉树对平衡性要求的那么苛刻，
// 虽然两者的时间复杂度相同，但是红黑树在实际测算中的速度要更胜一筹！
// 提示：平衡二叉树的时间复杂度是O(logn)，红黑树的时间复杂度为O(lgn)，
// 两者都表示的都是时间复杂度为对数关系（lg函数为底是10的对数，用于表示时间复杂度时可以忽略）。

int main() {
  RBT_Root* T = rbTree_init();

  rbTree_insert(&T, 3);
  rbTree_insert(&T, 5);
  rbTree_insert(&T, 1);
  rbTree_insert(&T, 2);
  rbTree_insert(&T, 4);
  rbTree_print(T);
  printf("\n");
  rbTree_delete(&T, 3);
  rbTree_print(T);

  // 前序遍历 ：3B 1B 2R 5B 4R
  // 中序遍历 ：1B 2R 3B 4R 5B

  // 前序遍历 ：4B 1B 2R 5B
  // 中序遍历 ：1B 2R 4B 5B

  return 0;
}
