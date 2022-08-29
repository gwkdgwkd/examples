#include <stdio.h>
#include <stdlib.h>

// fgets()有局限性，每次最多只能从文件中读取一行内容，
// 因为fgets()遇到换行符就结束读取。
// 如果希望读取多行内容，需要使用fread()函数；
// 相应地写入函数为fwrite()。

// fread()函数用来从指定文件中读取块数据。
// 所谓块数据，也就是若干个字节的数据，可以是一个字符，
// 可以是一个字符串，可以是多行数据，并没有什么限制。
// fread()的原型为：
// size_t fread(void *ptr, size_t size, size_t count, FILE *fp);
// fwrite()函数用来向文件中写入块数据，它的原型为：
// size_t fwrite(void *ptr, size_t size, size_t count, FILE *fp);
// 对参数的说明：
// 1.ptr为内存区块的指针，它可以是数组、变量、结构体等。
//   fread()中的ptr用来存放读取到的数据，
//   fwrite()中的ptr用来存放要写入的数据。
// 2.size：表示每个数据块的字节数。
// 3.count：表示要读写的数据块的块数。
// 4.fp：表示文件指针。
// 理论上，每次读写size*count个字节的数据。
// size_t是在stdio.h和stdlib.h头文件中使用typedef定义的数据类型，
// 表示无符号整数，也即非负数，常用来表示数量。
// 返回值：返回成功读写的块数，也即count。
// 如果返回值小于count：
// 1.对于fwrite()来说，肯定发生了写入错误，可以用ferror()函数检测。
// 2.对于fread()来说，可能读到了文件末尾，
//   可能发生了错误，可以用ferror()或feof()检测。

#define N 5

#define M 2
struct stu {
  char name[10];  // 姓名
  int num;        // 学号
  int age;        // 年龄
  float score;    // 成绩
} boya[M], boyb[M], *pa, *pb;

int main() {
  int flag = 1;
  if (0 == flag) {
    // 从键盘输入的数据放入a，从文件读取的数据放入b：
    int a[N], b[N];
    int i, size = sizeof(int);
    FILE *fp;
    // 以二进制方式打开：
    if ((fp = fopen("demo", "rb+")) == NULL) {
      puts("Fail to open file!");
      exit(0);
    }

    // 从键盘输入数据 并保存到数组a：
    for (i = 0; i < N; i++) {
      scanf("%d", &a[i]);
    }
    // 将数组a的内容写入到文件：
    fwrite(a, size, N, fp);
    // 将文件中的位置指针重新定位到文件开头：
    rewind(fp);
    // 从文件读取内容并保存到数组b：
    fread(b, size, N, fp);
    // 在屏幕上显示数组b的内容：
    for (i = 0; i < N; i++) {
      printf("%d ", b[i]);
    }
    printf("\n");
    fclose(fp);
  } else if (1 == flag) {
    FILE *fp;
    int i;
    pa = boya;
    pb = boyb;
    if ((fp = fopen("demo", "wb+")) == NULL) {
      puts("Fail to open file!");
      exit(0);
    }
    // 从键盘输入数据：
    printf("Input data:\n");
    for (i = 0; i < M; i++, pa++) {
      scanf("%s %d %d %f", pa->name, &pa->num, &pa->age, &pa->score);
    }
    // 将数组boya的数据写入文件：
    fwrite(boya, sizeof(struct stu), M, fp);
    // 将文件指针重置到文件开头：
    rewind(fp);
    // 从文件读取数据并保存到数据boyb：
    fread(boyb, sizeof(struct stu), M, fp);
    // 输出数组boyb中的数据：
    for (i = 0; i < M; i++, pb++) {
      printf("%s  %d  %d  %f\n", pb->name, pb->num, pb->age, pb->score);
    }
    fclose(fp);
  }
  return 0;
}