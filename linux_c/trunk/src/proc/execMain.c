#include <stdio.h>
	/*execlp测试调用程序*/
void main(int argc,char *argv[]){
	int i;
	if(argc==1)
	printf("peleas input selve arg!\n");

	for(i=1;i<argc;i++){
	printf("use exec run arg is: %s\n",argv[i]);
	}
}
