#include "stdio.h" //两种引入方式
#include <stdbool.h>//《》方式通常加载标准到库文件
#include "../c2/files.c"//""方式通常加载自定义的文件
#define GET_ARRAY_LEN(array,len){len = (sizeof(array) / sizeof(array[0]));}//获取数组长度方法
#define PRICE 30 //常量
#define S(a,b)PRICE+a+b//带参的宏定义

#define COMPUT_A 2//是否编译
/*hello word!*/
void helloWord(){
	printf("hello word！\n");
}
/*整型*/
void plastic(){
	//整形常量表示方法
	int ax=012;//0开头到数是”八进制“
	int bx=0x12a;//0x开头的数是“十六进制”
	//整形声明
	signed int a=1;//默认有符号
	unsigned int b=2;//无符号的，比有符号的大一倍 
	double c=321;
	long d=456;
	printf("\n整型:\na:%d\nb:%u\nPRICE:%d\n",a,b,PRICE*3);
	//变量存储类别
        extern ea;//声明 外部变量
	printf("extern value:%d\n",ea);
	auto int ty=8;//auto 自动释放存储空间（默认加上）
	register int ty2=8;//寄存器变量
	//自定义新的类型名
	typedef float ft;
	ft f=1;//等同float f=1;
	//枚举类型
	enum weekday{sun,mon,tue,wed,the,fri,sat};//直接赋值
	//用此枚举类型定义变量
	enum weekday ae,be;
	//赋值
	ae=sun;
	be=mon;
}
int ea=123;//定义 外部变量
/*字符型,强制转换,大小字母到转换*/
void charStr(){
	//字符型,强制转换
	char a1='a';
	int b1=2;
	double c1=(double)b1;
	printf("\n字符型强制转换：\n%c\nc1:%f\n",a1,c1);
	//大小字母到转换
	char a2='a';
	char a3=a2-32;
	int a4='a';//字符数据与整型可以相互赋值
	printf("\n大小字母转换：\na3:%c\na4:%c\n",a3,a4);
}

/*array数组*/
void array(){
	int a[3][6]={};
	int i=0; 
	int j=0;
    int count;//获取数组长度方法一
	GET_ARRAY_LEN(a,count);
	int b[]={12,14,16};//获取数组长度方法二
	int count2=sizeof(b)/sizeof(int);
	int i2=0;
	printf("\n方法二获取数组长度是%d：",count2);
	for(i2=0;i2<count2;i2++){
		printf("\n方法二获取数组值是%d：",b[i2]);
	}
	//多维数组赋值
	for(i=0;i<3;i++){//c低版本编译器不支持for里面声明
		for(j=0;j<6;j++){
	        a[i][j]=i*j;
		}
	}
	//多维数组打印
	for(i=0;i<3;i++){
	printf("\n第%d行:",(i+1));
		for(j=0;j<6;j++){
	        printf("列%d值是%d|",(j+1),a[i][j]);
		}
	}
}
/*定义字符串,switch,goto语句,逗号表达式*/
void defindStr(){
	//定义字符串
	bool bl=true;
	if(bl){//表达式中若if里面值为“0”就按”假“处理，非0就按“真”处理
	char * a="字符串";//指针方式
	char b[]="字符串-字符串";//字符数组方式类似 char b[100]
	//b="affasdfasdfasdf";
	printf("\nString:%s\nString2:%s\n\n",a,b);
	}
	//switch 里面“表达式”允许ANSI标准任何类型
	int tj=1;
	switch(tj<4){//可以是表达式
	case 1:
	printf("1!\n\n");
	break;
	case 2:
	printf("2!\n");
	break;
	case 3:
	printf("3!\n");
	break;
	default:
	printf("default!\n");
	break;
	}
	//goto语句
	int j=0;
	ly: if(j<10){
	printf("\ngoto value：%d\n",j);
	j++;
	goto ly;
	}
	//逗号表达式
	int dx;
	int cx=((dx=3*5,dx*4),dx+5);//表达式1，表达式2，....
	printf("\n逗号表达式：%d",cx);//逗号表达式到值是:表达式2
}
/*按条件编译(使用#define COMPUT_A)*/
void requirementCompile(){
//ifdef(定义过就编译，否就不编译)
	#ifdef COMPUT_A
	printf("\nifdef_COMPUT_A定义过！\n");
	#else
	printf("\nifdef_COMPUT_A没定义！\n");
	#endif
//ifndef(与上面相反)
	#ifndef COMPUT_A
	printf("\nifndef_COMPUT_A没定义！\n");
	#else
	printf("\nifndef_COMPUT_A定义过！\n");
	#endif
//if(非0就编译，否则就不编译（里面为表达式）)
	#if COMPUT_A>2
	printf("\nif_COMPUT_A定义过！\n");
	#elif COMPUT_A==2
	printf("\nif_COMPUT_A等于2！\n");
	#else
	printf("\nif_COMPUT_A没定义！\n");
	#endif
//带参的宏定义
	int chd=S(1,2);
	printf("\n带参的宏定义value:%d\n",chd);
}
/*main主函数*/
void main()
{
	outPut();//调用外部文件函数
	helloWord();
	plastic();
	charStr();
	array();
	defindStr();
	requirementCompile();
}

