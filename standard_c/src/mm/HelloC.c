#include <stdio.h>
 
 /**
  c语言内存介绍
  https://www.bilibili.com/video/BV1Dt411R77A

  问题：
 Segmentation fault: 11
  原因：
  主要检查数组越界的bug，
  死循环爆栈bug。
  */
int main()
{
   /* 栈溢出，一般栈比较小 */
   printf("-------------栈溢出\n");
   char array[1024*1024*1024]={0};
   array[0]='a';
   printf("%s\n",array);
   return 0;
}
