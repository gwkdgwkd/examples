#include <stdio.h>
#include <stdlib.h>

// fscanf()和fprintf()函数与scanf()和printf()功能相似，
// 都是格式化读写函数，两者的区别在于：
// fscanf()和fprintf()的读写对象不是键盘和显示器，而是磁盘文件。
// 这两个函数的原型为：
// int fscanf(FILE *fp, char * format, ...);
// int fprintf(FILE *fp, char * format, ...);
// fp为文件指针，format为格式控制字符串，...表示参数列表。
// 与scanf()和printf()相比，它们仅仅多了一个fp参数。
// fprintf()返回成功写入的字符的个数，失败则返回负数。
// fscanf()返回参数列表中被成功赋值的参数个数。
// 如果将fp设置为stdin，那么fscanf()会从键盘读数据，与scanf相同；
// 设置为stdout，那么fprintf()会向显示器输出，与printf的相同。

#define N 2
struct stu {
  char name[10];
  int num;
  int age;
  float score;
} boya[N], boyb[N], *pa, *pb;

int main() {
  int flag = 1;
  if (0 == flag) {
    FILE *fp;
    int i;
    pa = boya;
    pb = boyb;
    if ((fp = fopen("demo", "wt+")) == NULL) {
      puts("Fail to open file!");
      exit(0);
    }
    // 从键盘读入数据，保存到boya：
    printf("Input data:\n");
    for (i = 0; i < N; i++, pa++) {
      scanf("%s %d %d %f", pa->name, &pa->num, &pa->age, &pa->score);
    }
    pa = boya;
    // 将boya中的数据写入到文件：
    for (i = 0; i < N; i++, pa++) {
      fprintf(fp, "%s %d %d %f\n", pa->name, pa->num, pa->age, pa->score);
    }
    // 重置文件指针：
    rewind(fp);
    // 从文件中读取数据，保存到boyb：
    for (i = 0; i < N; i++, pb++) {
      fscanf(fp, "%s %d %d %f\n", pb->name, &pb->num, &pb->age, &pb->score);
    }
    pb = boyb;
    // 将boyb中的数据输出到显示器：
    for (i = 0; i < N; i++, pb++) {
      printf("%s  %d  %d  %f\n", pb->name, pb->num, pb->age, pb->score);
    }
    fclose(fp);
  } else if (1 == flag) {
    int a, b, sum;
    fprintf(stdout, "Input two numbers: ");
    fscanf(stdin, "%d %d", &a, &b);
    sum = a + b;
    fprintf(stdout, "sum=%d\n", sum);
  }

  return 0;
}