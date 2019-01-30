#include <stdio.h>
#include <time.h>
#include <stdlib.h>
			/*时间编程*/
/*获取日历时间，并转换成，格林威治(世界标准)，本地时间*/
void getTime(){
	struct tm * local;
	time_t t;
	//获取日历当前时间
	t=time(NULL);
	//转换成“本地时间”
	local=localtime(&t);
	printf("Local hour is:%d\n",local->tm_hour);
	//转换成“格林威治(世界标准)”时间
	local=gmtime(&t);
	printf("UTC hour is:%d\n",local->tm_hour);
	/*常用tm元素（所有元素都是int型）*/
	//tm_sec:秒，tm_min:分钟，tm_hour:小时
	//tm_mday:本月第几日,tm_mon:本年第几月,tm_year:哪一年(tm_year+1900=哪一年)
}
/*时间显示*/
void timeShow(){
	//将tm格式值转换成字符串(sat jui 30 08:43:03 2005)
	struct tm * local;
	time_t t;
	//获取tm时间
	t=time(NULL);
	local=localtime(&t);
	//将tm转换成为字符串
	char * pt=asctime(local);
	printf("\ntm转换成为字符串value:%s\n",pt);
	//将日历时间转换成本地的字符串形式
	char * lpt=ctime(&t);
	printf("\n日历转换成为字符串value:%s\n",lpt);
}
//function功能1
void function(){
	unsigned int i,j;
	double y;
	for(i=0;i<10000;i++)
		for(j=0;j<10000;j++)
			y++;
}
/*gettimeofday获取今天零时至现在的时间差*/
//计算程序的运行时间
void timeApplicationGap(){
	//声明所需变量
	struct timeval tpstart,tpend;
	float timeuse;
	//记录开始运行的时间
	gettimeofday(&tpstart,NULL);
	function();//运行功能
	sleep(1);//程序睡眠多少秒
	usleep(1000000);//程序睡眠多少微妙
	//记录结束运行的时间
	gettimeofday(&tpend,NULL);
	//计算执行时间
	timeuse=1000000*(tpend.tv_sec-tpstart.tv_sec)+(tpend.tv_usec-tpstart.tv_usec);
	timeuse/=1000000;
	printf("\nused time:%f\n",timeuse);
	exit(0);
}

void main(){
	getTime();
	timeShow();
	timeApplicationGap();
}
