//结构体
struct date
{
int year;
char * str;
};
struct student
{
int age;
char * name;
struct date dt;
};
/*简单一个链表*/
struct StructDomain
{
int age;
char * name;
struct StructDomain * next; 
};
struct studentFile
{
int age;
char name[30];
};
//共用体特点（在同一个时间只有一个变量起效并且所有元素都占同样的内存地址）
union data{
int i;
char ch;
float f;
}a,b,c;
union data d,e;

