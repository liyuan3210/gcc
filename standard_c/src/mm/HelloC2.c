#include <stdio.h>
#include <stdlib.h>
/**
 c语言内存介绍
  https://www.bilibili.com/video/BV1Dt411R77A
  
HelloC2(4101,0x104397e00) malloc: can't allocate region
:*** mach_vm_map(size=1073741824, flags: 100) failed (error code=3)
 */
void mm(){
   char *p = malloc(sizeof(char)*1024);
}
int main()
{
   /* 堆溢出 */
   printf("-------------堆溢出\n");
   for(;;){
      mm();
   }
    printf("-------------end\n");
   return 0;
}

