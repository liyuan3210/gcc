#include <stdio.h>
#include "domain.c"

//& 取地址运算符号，* 指针运算符号（取其指向到内容）
void  swap(int * p1,int * p2){
	int middle;
	middle=* p1;
	* p1=* p2;
	* p2=middle;
	//不能够改变指针形参的值来改便要交换的值。
	//int * mid;
	//mid=p1;
	//p1=p2;
	//p2=mid;
}
/*使用指针完成两个变量的交换(函数功能)*/
void pointer(){
	int i,j;
	i=1;j=2;
	int * pointer_i,*pointer_j;
	pointer_i=&i;//把变量i的地址赋给指针pointer_i
	pointer_j=&j;
	printf("\nPvalue:%d\n",*pointer_i);
	printf("\n交换前");
	printf("\ni:%d，j:%d",i,j);
	swap(pointer_i,pointer_j);
	printf("\n交换后");
	printf("\ni:%d，j:%d\n",i,j);
}
/*通过指针变量指向数组*/
void pointerArray(){
	int a[]={1,2,4,5};
	int * p,i;
	//循环出来
	printf("\n\n通过指针指向数组：\n");
	for(p=a;p<(a+4);p++){
	printf("p value：%d\n",*p);
	}
	//改变字符数组里面到值
	char astr[]="liyuat";
	printf("change value：%s\n",astr);
	for(i=0;i<6;i++){
		if(astr[i]=='t')
		astr[i]='n';
	}
	printf("change valueed：%s\n",astr);	
	//指针字符串通过下标来引用字符
	char * pstr="litianguan";
	printf("pstr value：%s\n",pstr);
	printf("\npstr：");
	for(i=0;pstr[i]!='\0';i++){
		printf("%c",pstr[i]);	
	}
	printf("\npstr valueed：%s\n",pstr);		
}
//比较函数
int max(int x,int y){
	int z;
	if(x>y)
	z=x;
	else
	z=y;

	return z;
}
/*指向函数的指针*/
void functionMax(){
	int max(int,int);//声明函数
	int (*p)(int,int);//定义一个为整型并指向max函数的指针变量
	p=max;//赋值语句
	int a=(*p)(2,3);//调用指针函数变量并把运算到值赋给a
	printf("\n指针函数调用的结果是：%d\n",a);

}
/*结构体*/
void structure(void){//void 可以不加
	struct date c1={2013,"描述二"};
	struct student s1={12,"andy",c1};
	struct student s2={13,"李渊",{2013,"描述三"}};
	s1.name="刘德华";
	s1.dt.str="描述...描述...描述...";
	printf("\n结构体:\n");
	printf("\nsd1:%s\n_%d\n_%s\n",s1.name,s1.age,s1.dt.str);
	printf("\nsd2:%s\n_%d\n_%s\n",s2.name,s2.age,s2.dt.str);
	//指向结构体的指针
	struct date d1=c1;
	struct date * pd;
	pd=&d1;
	int asd=sizeof(struct date),asa;//得到struct date长度
	asd=asa=2;
	printf("\n指向结构体指针：%d,%s,%d,%d",(*pd).year,pd->str,asd,asa);

}
/*链表*/
void linkList(){
	struct StructDomain * sd;
	//sd=0;
	//if(sd==0)printf("\n链表 value is:%d\n",sd);
}
//mian主函数
void main(){
	pointer();
	structure();
	pointerArray();
	functionMax();
	linkList();
}
	
