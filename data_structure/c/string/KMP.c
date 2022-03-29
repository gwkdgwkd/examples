#include <stdio.h>
#include <string.h>

// 快速模式匹配算法，简称KMP算法，是在BF算法基础上改进得到的算法。
// BF该算法的实现过程就是"傻瓜式"地用模式串（假定为子串的串）与主串中的字符一一匹配，算法执行效率不高。
// KMP算法不同，它的实现过程接近人为进行模式匹配的过程。
// 例如，对主串A（"ABCABCE"）和模式串B（"ABCE"）进行模式匹配，如果人为去判断，仅需匹配两次。
// 若使用BF算法，则此模式匹配过程需要进行4次。
// 每次匹配失败后模式串移动的距离不一定是1，某些情况下一次可移动多个位置，这就是KMP模式匹配算法。

// 模式串移动距离的判断
// 每次模式匹配失败后，计算模式串向后移动的距离是KMP算法中的核心部分。
// 其实，匹配失败后模式串移动的距离和主串没有关系，只与模式串本身有关系。
// 换句话说，不论主串如何变换，只要给定模式串，则匹配失败后移动的距离就已经确定了。
// 不仅如此，模式串中任何一个字符都可能导致匹配失败，因此串中每个字符都应该对应一个数字，用来表示匹配失败后模式串移动的距离。
// 可以给每个模式串配备一个数组（例如next[]），用于存储模式串中每个字符对应指针j重定向的位置（也就是存储模式串的数组下标）。
// 模式串中各字符对应next值的计算方式是，取该字符前面的字符串（不包含自己），
// 其前缀字符串和后缀字符串相同字符的最大个数再+1就是该字符对应的next值。
// 1.前缀字符串指的是位于模式串起始位置的字符串，例如模式串"ABCD"，则 "A"、"AB"、"ABC"以及"ABCD"都属于前缀字符串；
// 2.后缀字符串指的是位于串结尾处的字符串，还拿模式串"ABCD"来说，"D"、"CD"、"BCD"和"ABCD"为后缀字符串。
// 3.注意，模式串中第一个字符对应的值为0，第二个字符对应1，这是固定不变的。
//   因此，模式串"ABCAE"中，各字符对应的next＝[0,1,1,1,2];
// 每个字符对应的next值都可以表示该字符前"同后缀字符串相同的前缀字符串最后一个字符所在的位置"，
// 因此在每次匹配失败后，都可以轻松找到次长前缀字符串的最后一个字符与该字符进行比较。
void Next1(char *T, int *next) {
  next[1] = 0;
  int i = 1;
  int j = 0;
  // next[2]=1可以通过第一次循环直接得出
  while (i < strlen(T)) {
    if (j == 0 || T[i - 1] == T[j - 1]) {
      i++;
      j++;
      next[i] = j;
    } else {
      j = next[j];
    }
  }
}
// 上面Next函数的缺陷：
// A: ABACBCE   A: ABACBCE
// B: ABAB      B:   ABAB
// 当匹配失败时，Next函数会由图AC的Ａ开始继续进行模式匹配，但是从图中可以看到，这样做是没有必要的，纯属浪费时间。
// 出现这种多余的操作，问题在当T[i-1]==T[j-1]成立时，没有继续对i++和j++后的T[i-1]和T[j-1]的值做判断。
// 改进后的Next函数如下所示：
void Next2(char *T, int *next) {
  next[1] = 0;
  int i = 1;
  int j = 0;
  while (i < strlen(T)) {
    if (j == 0 || T[i - 1] == T[j - 1]) {
      i++;
      j++;
      if (T[i - 1] != T[j - 1]) {
        next[i] = j;
      } else {
        next[i] = next[j];
      }
    } else {
      j = next[j];
    }
  }
}
// 这里只设定了next[1]的值为0，而next[1]的值，需要经过判断之后，才能最终得出，所以它的值不一定是1。

void Next3(char *p, int *next) {
  next[0] = -1;
  int i = 0, j = -1;

  while (i < (int)strlen(p)) {
    if (j == -1 || p[i] == p[j]) {
      ++i;
      ++j;
      next[i] = j;
    } else
      j = next[j];
  }
}

int KMP(char *S, char *T) {
  int next[10] = {0};
  Next2(T, next);  // 根据模式串T,初始化next数组

  for (int i = 0; i < 10; ++i) {
    printf("%d ", next[i]);
  }
  printf("\n");

  int i = 1;
  int j = 1;
  while (i <= strlen(S) && j <= strlen(T)) {
    // j==0:代表模式串的第一个字符就和当前测试的字符不相等；
    // S[i-1]==T[j-1],如果对应位置字符相等，两种情况下，指向当前测试的两个指针下标i和j都向后移
    if (j == 0 || S[i - 1] == T[j - 1]) {
      i++;
      j++;
    } else {
      // 如果测试的两个字符不相等，i不动，j变为当前测试字符串的next值
      j = next[j];
    }
  }
  if (j > strlen(T)) {  // 如果条件为真，说明匹配成功
    return i - (int)strlen(T);
  }
  return -1;
}

void GetNextval(char *p, int next[]) {
  int pLen = strlen(p);
  next[0] = -1;
  int k = -1;
  int j = 0;
  while (j < pLen - 1) {
    // p[k]表示前缀，p[j]表示后缀
    if (k == -1 || p[j] == p[k]) {
      ++j;
      ++k;
      // 较之前next数组求法，改动在下面4行
      if (p[j] != p[k])
        next[j] = k;  // 之前只有这一行
      else
        // 因为不能出现p[j] = p[next[j]]，所以当出现时需要继续递归，k = next[k] = next[next[k]]
        next[j] = next[k];
    } else {
      k = next[k];
    }
  }
}

int KmpSearch(char *s, char *p) {
  int i = 0;
  int j = 0;
  int sLen = strlen(s);
  int pLen = strlen(p);

  int next[pLen];
  memset(next, 0, sizeof(next));
  GetNextval(p, next);
  for (int i = 0; i < pLen; ++i) {
    printf("%d ", next[i]);
  }
  printf("\n");

  while (i < sLen && j < pLen) {
    // 如果j = -1，或者当前字符匹配成功（即S[i] == P[j]），都令i++，j++
    if (j == -1 || s[i] == p[j]) {
      i++;
      j++;
    } else {
      // 如果j != -1，且当前字符匹配失败（即S[i] != P[j]），则令i不变，j = next[j]
      // next[j]即为j所对应的next值
      j = next[j];
    }
  }
  if (j == pLen)
    return i - j;
  else
    return -1;
}

int main() {
  int i = KMP("ababcabcacbab", "abcac");
  printf("%d\n", i);  // 6

  i = KmpSearch("ababcabcacbab", "abcac");
  printf("%d\n", i);  // 6

  return 0;
}