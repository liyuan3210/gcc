#include <stdio.h>
#include <regex.h>
#include<string.h>
/**
 *下载地址：
 *https://sourceforge.net/projects/gnuwin32/files/regex/2.7/(regex-2.7-bin.zip)
 *下载完包解压后：
 *将lib中的libregex.dll.a和libregex.la放进MinGW的lib目录，将regex2.dll放进程序所在目录。
 *
 *libraries 要添加配置regex
 *动态链接库文件regex2.dll需要放在exe文件下面
 */
int main9()
{
//	regex();
	regex2();


//	char *str="aaabbbccc";
//	char *dst;
//	printf("err:%s\n",replace(str,"bbb",""));
	printf("------------over");
    return 0;
}
/* 取子串的函数 */
//char* substr (const char*str, unsigned start, unsigned end)
//{
//unsigned n = end - start;
//char *stbuf;
//strncpy(stbuf, str + start, n);
//return stbuf;
//}

void regex2(){
	char *bematch = "aaaaahttp://baiducomz bhttp://34123qwerqezcom http://liyuancomzadfzhttp://1liyuancomz";
		char *pattern = "(http://[a-zA-Z0-9]+[com])";
		char errbuf[1024];
		char match[1024];
		regex_t reg;
		int err,nm = 10;
		regmatch_t pmatch[nm];

	//	if(regcomp(&reg,pattern,REG_EXTENDED) < 0){
		if(regcomp(&reg,pattern,REG_EXTENDED) < 0){
			regerror(err,&reg,errbuf,sizeof(errbuf));
			printf("err:%s\n",errbuf);
		}

		err = regexec(&reg,bematch,nm,pmatch,0);

		if(err == REG_NOMATCH){
			printf("no match\n");
			exit(-1);
		}else if(err){
			regerror(err,&reg,errbuf,sizeof(errbuf));
			printf("err:%s\n",errbuf);
			exit(-1);
		}

//		while(err != REG_NOMATCH){
//			char *m=substr(bematch,pmatch[0].rm_so,pmatch[0].rm_eo);
//			printf("找到的字符串:%s",m);
//			//执行替换
////			replace(bematch,ret,"");
////			err = regexec(&reg,bematch,nm,pmatch,0);
//
//
//		}

//		for(int i=0;i<10 && pmatch[i].rm_so!=-1;i++){

			while(err != REG_NOMATCH){
				int len = pmatch[0].rm_eo-pmatch[0].rm_so;
//				printf("数字%d",i);
				memset(match,'\0',sizeof(match));
				memcpy(match,bematch+pmatch[0].rm_so,len);
				printf("打印%s\n",match);
//				char *m2=substr(bematch,pmatch[0].rm_so,pmatch[0].rm_eo);
//				printf("打印m2%s\n",m2);
				char *m=replace(bematch,match,"");
				bematch=m;
//				match=substr(bematch,pmatch[0].rm_so,pmatch[0].rm_eo);
//				printf("打印2%s\n",m);
				err = regexec(&reg,bematch,nm,pmatch,0);
//				printf("xxxx:%s\n",substr(bematch,pmatch[0].rm_so,pmatch[0].rm_eo));
			}
//		}
}
void regex(){
	char *bematch = "hhhericchd@gmail.com";
	char *pattern = "^h{3,10}(.*)@.{5}.(.*)";
	char errbuf[1024];
	char match[100];
	regex_t reg;
	int err,nm = 10;
	regmatch_t pmatch[nm];

//	if(regcomp(&reg,pattern,REG_EXTENDED) < 0){
	if(regcomp(&reg,pattern,NULL) < 0){
		regerror(err,&reg,errbuf,sizeof(errbuf));
		printf("err:%s\n",errbuf);
	}

	err = regexec(&reg,bematch,nm,pmatch,0);

	if(err == REG_NOMATCH){
		printf("no match\n");
		exit(-1);
	}else if(err){
		regerror(err,&reg,errbuf,sizeof(errbuf));
		printf("err:%s\n",errbuf);
		exit(-1);
	}

	for(int i=0;i<10 && pmatch[i].rm_so!=-1;i++){
		int len = pmatch[i].rm_eo-pmatch[i].rm_so;
		if(len){
			memset(match,'\0',sizeof(match));
			memcpy(match,bematch+pmatch[i].rm_so,len);
			printf("%s\n",match);
		}
	}
}
