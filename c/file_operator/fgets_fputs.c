#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// fgetc()和fputc()函数每次只能读写一个字符，速度较慢；
// 实际开发中往往是每次读写一个字符串或者一个数据块，这样能明显提高效率。

// fgets()函数用来从指定的文件中读取一个字符串，
// 并保存到字符数组中，它的用法为：
// char *fgets (char *str, int n, FILE *fp);
// str为字符数组，n为要读取的字符数目，fp为文件指针。
// 返回值：
// 读取成功时返回字符数组首地址，也即str；
// 读取失败时返回NULL；
// 如果开始读取时文件内部指针已经指向了文件末尾，
// 那么将读取不到任何字符，也返回NULL。
// 注意，读取到的字符串会在末尾自动添加'\0'，n个字符也包括'\0'。
// 需要重点说明的是，在读取到n-1个字符之前如果出现了换行，
// 或者读到了文件末尾，则读取结束。
// 这就意味着，不管n的值多大，fgets()最多只能读取一行数据，不能跨行。
// 在C语言中，没有按行读取文件的函数，可以借助fgets()，
// 将n的值设置地足够大，每次就可以读取到一行数据。
// fgets()遇到换行时，会将换行符一并读取到当前字符串。
// 而gets()不一样，它会忽略换行符。

// fputs()函数用来向指定的文件写入一个字符串，它的用法为：
// int fputs(char *str, FILE *fp);
// str为要写入的字符串，fp为文件指针。
// 写入成功返回非负数，失败返回EOF。

#define N 101

int main() {
  int flag = 1;
  if (0 == flag) {
    FILE *fp;
    char str[N + 1];
    if ((fp = fopen("demo", "rt")) == NULL) {
      puts("Fail to open file!");
      exit(0);
    }

    while (fgets(str, N, fp) != NULL) {
      printf("%s", str);
    }
    fclose(fp);
  } else if (1 == flag) {
    FILE *fp;
    char str[102] = {0}, strTemp[100];
    if ((fp = fopen("demo", "at+")) == NULL) {
      puts("Fail to open file!");
      exit(0);
    }
    printf("Input a string:");
    gets(strTemp);
    strcat(str, "\n");
    strcat(str, strTemp);
    fputs(str, fp);
    fclose(fp);
  }
  return 0;
}