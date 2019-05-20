#include <stdio.h>
#include "domain.c"
/*fputc,fgetc字符型*/
void putCharfile(){
	//创建一个文件
	FILE * fp;
	if((fp=fopen("c2/createFile.txt","wb+"))==NULL){//NULL等于0比较
	printf("can not open file\n");
	return;
	}
	//写入文件
	char cstr[]="liyuan is shengren!李渊";
	int i;
	for(i=0;cstr[i]!='\0';i++){
	fputc(cstr[i],fp);
	}
	fclose(fp);//关闭文件
	
	//读出文件内容
	FILE * fp2;
	fp2=fopen("c2/createFile.txt","r");//读取一个文件
	char cm;
	cm=fgetc(fp2);
	printf("\n文件读取value:");

	while(cm!=EOF){//EOF 等于-1，比较
	putchar(cm);
	cm=fgetc(fp2);
	}
	printf("\n");
	fclose(fp2);//关闭文件
	//ex;it(0);
	
}
/*fread,fwrite读入与写出,一般用来处理一组数据*/
void frwfile(){
	//写入
	FILE * fp;
	struct studentFile sf1={12,"liyuan"};
	struct studentFile sf2={21,"litianyuan"};
	struct studentFile stf[2]={sf1,sf2};
	fp=fopen("c2/frwFile.txt","wb");
	int i;
	for(i=0;i<2;i++){
	if(fwrite(&stf[i],sizeof(struct studentFile),1,fp)!=1)
	printf("file write error!\n");
	}
	fclose(fp);
	//读入
	FILE * fp2;
	struct studentFile str[2];
	fp2=fopen("c2/frwFile.txt","rb");
	printf("\n读入的数据是：");
	//读取方式一
	fread(&str,sizeof(struct studentFile),2,fp2);
	printf("\nage:%dname:%s\n",str[0].age,str[0].name);
	printf("\nage:%dname:%s\n",str[1].age,str[1].name);
	//读取方式二
	//for(i=0;i<2;i++){
	//fread(&str,sizeof(struct studentFile),1,fp2);
	//printf("\nage:%dname:%s\n",str.age,str.name);
	//}
}
/*fprintf,fscanf格式化函数 常用来格式化数据写入*/
void fps(){
	//写入
	int i = 10;
	double fpd = 1.5;
	int i2 = 100;
	double f2 = 10.5;
	char s[] = "this is a string";
	FILE * fp = fopen( "c2/fprintf.txt", "w" );
	//fprintf( fp, "%s\n", s);
	fprintf( fp, "%d\n", i );
	fprintf( fp, "%f\n", fpd );
	fprintf( fp, "%d\n", i2 );
	fprintf( fp, "%f\n", f2 );
	fclose(fp);
	//读写
	FILE * fp2 = fopen( "c2/fprintf.txt", "r" );
	int i3=0;
	double fp3=0;
	char s2[1000];
	fscanf(fp2,"%d",&i3);//读取时是根据写入到顺序读写的
	printf("\nout str:%d\n",i3);
	//fscanf(stream2,"%s",s2);
	//printf("\nout str:%s\n",s2);
	//fscanf(stream2,"%s",s2);
	//printf("\nout str:%s\n",s2);
	fclose(fp2);
	
}
//putw
int putw(int i,FILE *fp){
	char * s;
	s=(char*)&i;
	putc(s[0],fp);
	putc(s[1],fp);
	return(i);
}
//getw
int getw(FILE * fp){
	char * s;
	int i;
	s=(char*)&i;//使S指向i的起始地址
	s[0]=getc(fp);
	s[1]=getc(fp);
	return(i);
}
/*putw,getw整数读出，写出.(如果有的C编译器没有这两个函数，可以自己编写)*/
void pgtw(){
//写入
	FILE * fp=fopen("pgtw.txt","wb");
	//int su=10;
	int fg1=putw(11,fp);
	close(fp);
//读入
	FILE * fp2=fopen("pgtw.txt","wb");
	int val=getw(fp2);
	printf("\ngetw value:%d\n",val);
	close(fp2);
}
//fgets,fputs字符串写入与读取
void strInputOut(){
	//写入
	FILE * fp = fopen( "c2/strInputOut.txt", "w" );
	fputs("li yuan",fp);
	fclose(fp);
	//读取
	char cstr[100];
	FILE * fp2 = fopen( "c2/strInputOut.txt", "r" );
	fgets(cstr,100,fp2);
	printf("\nfgets读取的字符是：%s\n",cstr);
	fclose(fp2);
}
void main(){
	//putCharfile();
	//frwfile();
	//fps();
	//strInputOut();
	pgtw();
}
	/*文件定位 函数*/
//rewind(文件指针):使指针重新返回文件头的指针
//fseek(文件指针，位移量，起始点):改变文件位子指针
//ftell(文件指针):用于得到流式文件的当前位置，如果函数返回-1L表示出错
