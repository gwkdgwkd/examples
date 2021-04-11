#include <stdio.h>
#include <stdlib.h>

// 移动文件内部位置指针的函数主要有两个，即rewind()和fseek()。
// rewind()用来将位置指针移动到文件开头，它的原型为：void rewind (FILE *fp);
// fseek()用来将位置指针移动到任意位置，它的原型为：int fseek (FILE *fp, long offset, int origin);
// 参数说明：
//  1) fp为文件指针，也就是被移动的文件。
//  2) offset为偏移量，也就是要移动的字节数。之所以为long类型，是希望移动的范围更大，能处理的文件更大。offset为正时，向后移动；offset为负时，向前移动。
//  3) origin为起始位置，也就是从何处开始计算偏移量。C语言规定的起始位置有三种，分别为文件开头、当前位置和文件末尾，每个位置都用对应的常量来表示：
//     文件开头 	SEEK_SET 	0
//     当前位置 	SEEK_CUR 	1
//     文件末尾 	SEEK_END 	2
// 值得说明的是，fseek()一般用于二进制文件，在文本文件中由于要进行转换，计算的位置有时会出错。
// 在移动位置指针之后，就可以用任何一种读写函数进行读写了。由于是二进制文件，因此常用fread()和fwrite()读写。

#define N 3
struct stu {
  char name[10];  // 姓名
  int num;        // 学号
  int age;        // 年龄
  float score;    // 成绩
} boys[N], boy, *pboys;

int main() {
  FILE *fp;
  int i;
  pboys = boys;
  if ((fp = fopen("demo", "wb+")) == NULL) {
    printf("Cannot open file, press any key to exit!\n");
    exit(1);
  }
  printf("Input data:\n");
  for (i = 0; i < N; i++, pboys++) {
    scanf("%s %d %d %f", pboys->name, &pboys->num, &pboys->age, &pboys->score);
  }
  fwrite(boys, sizeof(struct stu), N, fp);  // 写入三条学生信息
  fseek(fp, sizeof(struct stu), SEEK_SET);  // 移动位置指针
  fread(&boy, sizeof(struct stu), 1, fp);   // 读取一条学生信息
  printf("%s  %d  %d %f\n", boy.name, boy.num, boy.age, boy.score);
  fclose(fp);

  return 0;
}