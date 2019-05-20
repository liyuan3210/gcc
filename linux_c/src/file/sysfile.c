#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>

#define BUFFER_SIZE 1024
			/*文件编程_系统调用*/
/*创建一个文件*/
void creatfile(char * filename){
	/*creat(filename,int mode)*/
	//filename文件名(可以是文件路径)
	//mode文件权限(无任何权限：0，执行权限：1，可写权限：2，只读权限：4)
	//mode文件权限宏表示（S_IRUSR:可读,S_IWUSR:可写,S_IXUSR:可执行,S_IRWXU:可读，写，执行）
	if(creat(filename,700)<0){
	//0765权限表示（0：占位符，(文件所有者)7：1+2+4，(本组的)6：2+4+0,(其他组)1+4+0）
	printf("creat file %s failure!\n",filename);
	exit(EXIT_FAILURE);//失败
	}else{
	printf("creat file %s success!\n",filename);
	exit(EXIT_SUCCESS);//成功
	}
}
/*打开一个文件*/
void openofile(char * filename){
	//int open(filename,flag) 方式一
	//int open(filename,flag,mode) 方式二（当mode是O_CREAT时就需要使用此open）
	//常见flage，O_RDONLY(只读),O_WRONLY(只写),O_RDWR(读写)
	//O_APPEND(追加),O_CREAT(创建),O_NOBLOCK(非阻塞方式)
	int fd;//open返回值是int型“文件描述符”
	if((fd=open(filename,O_CREAT|O_RDWR,0765))<0){
	printf("open file %s failure!\n",filename);
	exit(1);
	}else{
	printf("open file %s success!\n",filename);
	}
	close(fd);//关闭文件描述符int close(int fp)
	exit(0);
}
/*读，写文件*/
void rwfile(char * filename){
	int bytes_read,bytes_write;
	int size,i;
	char buffer_r[BUFFER_SIZE];
	char buffer_w[]="liyuan_李渊adfadfasdf";
	/*写*/
	bytes_write=open(filename,O_WRONLY|O_CREAT,0444);
	size=write(bytes_write,buffer_w,sizeof(buffer_w)-1);
	close(bytes_write);
	/*读*/
	bytes_read=open(filename,O_RDONLY);
	size=read(bytes_read,buffer_r,sizeof(buffer_r)-1);
	printf("\n读取的text是：%s\n",buffer_r);
	close(bytes_read);
}
/*文件访问判断，创建目录，获取路径，文件定位*/
void otherFile(){
	int filebyte,size;
	char buffer_w[]="liyuan_李渊adfadfasdf";
	filebyte=open("file/otherFile.txt",O_WRONLY|O_CREAT,0777);
	size=write(filebyte,buffer_w,sizeof(buffer_w)-1);

		/*文件定位int lseek(文件标识符，移动的字节数（前<负>，后<正>），相对标识)*/
	//"相对标识"可以表示成:SEEK_SET(相对文件开头)，SEEK_CUR(相对文件指针当前)，SEEK_END(相对文件末尾)
	int file_len=lseek(filebyte,0,SEEK_END);
	printf("\nfile bytes is %d .\n",file_len);

		/*获取文件路径*/
	char filepath[100];
	getcwd(filepath,sizeof(filepath));
	printf("\nfile path is %s .\n",filepath);

	close(filebyte);


		/*访问判断int access(filename,int mode),条件相符返回0，否测-1*/
	//mode值：R_OK(可读),W_OK(可写),X_OK(可执行),F_OK(文件存在)
	int isNo=access("file/wfile.txt",F_OK);
	if(isNo==0)
	printf("\n文件存在!\n");
	else
	printf("\n文件不存在!\n");

		/*创建目录*/
	int diNo=mkdir("testDir",0777);
	if(diNo!=0)
	printf("creat dir failure!");
	else
	printf("creat dir success!");
	
}
void main(){
	
	//creatfile("file/creatfile.txt");
	//openofile("file/openfile.txt");
	//rwfile("file/rwfile.txt");
	otherFile();

}
