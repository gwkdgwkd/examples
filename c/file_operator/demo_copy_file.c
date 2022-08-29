#include <stdio.h>
#include <stdlib.h>

// 文件的复制是常用的功能，要求写一段代码，
// 让用户输入要复制的文件以及新建的文件，然后对文件进行复制。
// 能够复制的文件包括文本文件和二进制文件，
// 你可以复制1G的电影，也可以复制1Byte的txt文档。
// 实现文件复制的主要思路是：
// 开辟一个缓冲区，不断从原文件中读取内容到缓冲区，
// 每读取完一次就将缓冲区中的内容写入到新建的文件，直到把原文件的内容读取完。
// 这里有两个关键的问题需要解决：
// 1.开辟多大的缓冲区合适？
//   缓冲区过小会造成读写次数的增加，过大也不能明显提高效率。
//   目前大部分磁盘的扇区都是4K对齐的，如果读写的数据不是4K的整数倍，
//   就会跨扇区读取，降低效率，所以开辟4K的缓冲区。
// 2.缓冲区中的数据是没有结束标志的，如果缓冲区填充不满，
//   如何确定写入的字节数？
//   最好的办法就是每次读取都能返回读取到的字节数。

int copyFile(char *fileRead, char *fileWrite);
int main() {
  char fileRead[100];   // 要复制的文件名
  char fileWrite[100];  // 复制后的文件名

  // 获取用户输入：
  printf("要复制的文件：");
  scanf("%s", fileRead);
  printf("将文件复制到：");
  scanf("%s", fileWrite);
  // 进行复制操作：
  if (copyFile(fileRead, fileWrite)) {
    printf("恭喜你，文件复制成功！\n");
  } else {
    printf("文件复制失败！\n");
  }
  return 0;
}
/**
 * 文件复制函数
 * @param    fileRead    要复制的文件
 * @param    fileWrite   复制后文件的保存路径
 * @return   int         1:复制成功；2:复制失败
**/
int copyFile(char *fileRead, char *fileWrite) {
  FILE *fpRead;                              // 指向要复制的文件
  FILE *fpWrite;                             // 指向复制后的文件
  int bufferLen = 1024 * 4;                  // 缓冲区长度
  char *buffer = (char *)malloc(bufferLen);  // 开辟缓存
  int readCount;                             // 实际读取的字节数
  if ((fpRead = fopen(fileRead, "rb")) == NULL ||
      (fpWrite = fopen(fileWrite, "wb")) == NULL) {
    printf("Cannot open file, press any key to exit!\n");
    exit(1);
  }
  // 不断从fileRead读取内容，放在缓冲区，再将缓冲区的内容写入fileWrite：
  while ((readCount = fread(buffer, 1, bufferLen, fpRead)) > 0) {
    fwrite(buffer, readCount, 1, fpWrite);
  }
  free(buffer);
  fclose(fpRead);
  fclose(fpWrite);

  return 1;
}