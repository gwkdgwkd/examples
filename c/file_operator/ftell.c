#include <stdio.h>
#include <stdlib.h>

// 实际开发中，有时候需要先获取文件大小再进行下一步操作。
// C语言没有提供获取文件大小的函数，要想实现该功能，必须自己编写函数。

// ftell()函数用来获取文件内部指针距离文件开头的字节数，它的原型为：
// long int ftell(FILE *fp);
// 注意：fp要以二进制方式打开，如果以文本方式打开，返回值可能没有意义。
// 先使用fseek()将文件内部指针定位到文件末尾，
// 再使用ftell()返回内部指针距离文件开头的字节数，
// 这个返回值就等于文件的大小。
// fpos_t是在stdio.h中定义的结构体，用来保存文件的内部指针。
// fgetpos()用来获取文件内部指针，fsetpos()用来设置文件内部指针。
long fsize(FILE *fp) {
  long n;
  fpos_t fpos;         // 当前位置
  fgetpos(fp, &fpos);  // 获取当前位置
  fseek(fp, 0, SEEK_END);
  n = ftell(fp);
  fsetpos(fp, &fpos);  // 恢复之前的位置
  return n;
}

int main() {
  long size = 0;
  FILE *fp = NULL;
  char filename[30] = "demo";
  // 以二进制方式打开文件：
  if ((fp = fopen(filename, "rb")) == NULL) {
    printf("Failed to open %s...", filename);
    exit(EXIT_SUCCESS);
  }

  printf("%ld\n", fsize(fp));

  return 0;
}