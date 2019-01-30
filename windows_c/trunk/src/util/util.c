#include <stdio.h>
#include <stdlib.h>
#include <time.h>
//指针拼接字符串方法一，不改变字符串a,b, 通过malloc，生成第三个字符串c, 返回局部指针变量
char *join1(char *a, char *b) {
    char *c = (char *) malloc(strlen(a) + strlen(b) + 1); //局部变量，用malloc申请内存
    if (c == NULL) exit (1);
    char *tempc = c; //把首地址存下来
    while (*a != '\0') {
        *c++ = *a++;
    }
    while ((*c++ = *b++) != '\0') {
        ;
    }
    //注意，此时指针c已经指向拼接之后的字符串的结尾'\0' !
    return tempc;//返回值是局部malloc申请的指针变量，需在函数调用结束后free之
}
//指针拼接字符串方法二，调用C库函数
char* join3(char *s1, char *s2){
    char *result = malloc(strlen(s1)+strlen(s2)+1);//+1 for the zero-terminator
    //in real code you would check for errors in malloc here
    if (result == NULL) exit (1);

    strcpy(result, s1);
    strcat(result, s2);

    return result;
}
/*获取规则:进程号id+yyyymmddhhmmss+3次时间5位随机数*/
char* getDateTimeRandUid(){
	char *dateTimeString="";
	char mid[6];
	char *m;
	struct tm * local;
	time_t t;
	//获取日历当前时间
	t=time(NULL);
	//转换成“本地时间”
	local=localtime(&t);
	printf("Local hour is:%d长度:%d\n,进程号%d\n",local->tm_year,strlen("12345"),_getpid());
	//进程号
	itoa(_getpid(),mid,10);
	m=join3(dateTimeString,mid);
	dateTimeString=m;
	//年
	itoa((local->tm_year+1900),mid,10);
	m=join3(dateTimeString,mid);
	dateTimeString=m;
	//月
	itoa((local->tm_mon+1),mid,10);
	m=join3(dateTimeString,mid);
	dateTimeString=m;
	//日
	itoa(local->tm_mday,mid,10);
	m=join3(dateTimeString,mid);
	dateTimeString=m;
	//小时
	itoa(local->tm_hour,mid,10);
	m=join3(dateTimeString,mid);
	dateTimeString=m;
	//小时
	itoa(local->tm_min,mid,10);
	m=join3(dateTimeString,mid);
	dateTimeString=m;
	//秒
	itoa(local->tm_sec,mid,10);
	m=join3(dateTimeString,mid);
	dateTimeString=m;
	//初始化随机数
	srand((unsigned)time(NULL));
	//第一次随机数
	itoa(rand(),mid,10);
	m=join3(dateTimeString,mid);
	dateTimeString=m;
	//第二次随机数
	itoa(rand(),mid,10);
	m=join3(dateTimeString,mid);
	dateTimeString=m;
	//第三次随机数
	itoa(rand(),mid,10);
	m=join3(dateTimeString,mid);
	dateTimeString=m;
	printf("长度:%d,字符串:%s\n",strlen(dateTimeString),dateTimeString);
	//mid=join3(dateTimeString,mid);
//	printf("合并%s:",dateTimeString);
	//转换成“格林威治(世界标准)”时间
	//local=gmtime(&t);
	//printf("UTC hour is:%d\n",local->tm_hour);
	/*常用tm元素（所有元素都是int型）*/
	//tm_sec:秒，tm_min:分钟，tm_hour:小时
	//tm_mday:本月第几日,tm_mon:本年第几月,tm_year:哪一年(tm_year+1900=哪一年)
	return dateTimeString;
}

/**
 *字符串替换
 *src:源字符串
 *sub:在src要替换的字符串
 *dst:把sub替换成的字符串，如果要删除直接写""空字符串
 */
char *replace(char *src, char *sub, char *dst)
{
    int pos = 0;
    int offset = 0;
    int srcLen, subLen, dstLen;
    char *pRet = NULL;

    srcLen = strlen(src);
    subLen = strlen(sub);
    dstLen = strlen(dst);
    pRet = (char *)malloc(srcLen + dstLen - subLen + 1);//(外部是否该空间)
    if (NULL != pRet)
    {
        pos = strstr(src, sub) - src;
        memcpy(pRet, src, pos);
        offset += pos;
        memcpy(pRet + offset, dst, dstLen);
        offset += dstLen;
        memcpy(pRet + offset, src + pos + subLen, srcLen - pos - subLen);
        offset += srcLen - pos - subLen;
        *(pRet + offset) = '\0';
    }
    return pRet;
}
//读取文件内容为 字符串 类型
char* getBuff(){
	FILE * fp2;
	fp2=fopen("d:/GetWebPage.xml","r");//读取一个文件
	char *ret= "";
	char *m;
	char b[1024]={};
	char c=fgetc(fp2);
	int i=0;
	while(c!=EOF){//EOF 等于-1，比较
		if(i==0){
			b[i]=c;
			c=fgetc(fp2);
			i++;
		}else if(i==1024){
			b[i]=c;
			c=fgetc(fp2);
			i=0;
			m=join3(ret,b);
			ret=m;
			memset(b,0,1024);
		}else{
			b[i]=c;
			c=fgetc(fp2);
			i++;
		}
		if(c==EOF){
			m=join3(ret,b);
			ret=m;
		}
	}
	printf("file text:%s",ret);
	fclose(fp2);//关闭文件
	return ret;
}
