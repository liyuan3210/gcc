/*
 ============================================================================
 Name        : vsDb.c
 Author      : liyuan
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <stdio.h>
#include <mysql.h>
/**
 * 使用mysql驱动访问操作Mysql数据库
 * c/c+配置:
 * includes 指向"D:\soft\mysql-connector-c-6.1.6-win32\include"目录
 * minGw配置:
 * libraries 要添加配置libmysql
 * libraries 指向驱动lib目录"D:\soft\mysql-connector-c-6.1.6-win32\lib"
 * 动态链接库文件libmysql.dll需要放在exe文件下面
 */
void main5(void){
	dbInit();
	//sreach();
//	while(1){}
	//insert();
}
//数据库初始化
void dbInit(){
	MYSQL mysql;
	/*连接之前，先用mysql_init初始化MYSQL连接句柄*/
	mysql_init(&mysql);
	/*使用mysql_real_connect连接服务器,其参数依次为MYSQL句柄，服务器IP地址，
	登录mysql的用户名，密码，要连接的数据库等*/
	if(!mysql_real_connect(&mysql, "liyuan.imblog.in", "ams", "123", "dep", 28386, NULL, 0)) {
		printf("Error connecting to Mysql!\n");
	}else {
		printf("Connected Mysql successful!\n");
	}

	/*关闭连接*/
	mysql_close(&mysql);

	puts("over!"); /* prints !!!Hello World!!! */
}
//查询
void sreach()
{
 printf("please wait...");
 printf("\n");
 MYSQL *pConn;
 pConn = mysql_init(NULL);
 //第2、3、4、5参数的意思分别是：服务器地址、用户名、密码、数据库名，第6个为mysql端口号（0为默认值3306）
 if(!mysql_real_connect(pConn,"liyuan.imblog.in", "liyuan", "123", "test", 28386, NULL, 0))
 {
  printf("无法连接数据库:%s",mysql_error(pConn));
  return;
 }
 //防止乱码。设置和数据库的编码一致就不会乱码
 mysql_query(pConn,"set character_set_results=utf8");


 if(mysql_query(pConn,"select addr,cont,create_dt from s_log"))
 {
  printf("查询失败:%s",mysql_error(pConn));
  return;
 }

 MYSQL_RES *result = mysql_store_result(pConn);
 MYSQL_ROW row;
 int t;
 while(row = mysql_fetch_row(result))
 {
	 for(t=0;t<mysql_num_fields(result);t++){
		 //printf("%s,", t);
		 printf("%s,", row[t]);
	 }
	 printf("\n");
  //printf("%s %s\n",row[0],row[1],row[2]);
 }
 //释放资源
 mysql_free_result(result);
 //结束
 mysql_close(pConn);
}
//新增
void insert()
{
 MYSQL *pConn;
 pConn = mysql_init(NULL);
 if(!mysql_real_connect(pConn,"liyuan.imblog.in", "ams", "123", "dep", 28386, NULL, 0))
 {
  printf("无法连接数据库:%s",mysql_error(pConn));
  return;
 }

// mysql_query(pConn,"set character_set_results=utf8");
 mysql_query(pConn,"SET NAMES utf8");

 if(mysql_query(pConn,"insert into tb_user(id,nage,age) values('11111aaaa','安迪',100)"))
  {
   printf("执行insert失败%s",mysql_error(pConn));
   return;
  }

 printf("执行insert成功，新id=%d",mysql_insert_id(pConn));
 mysql_close(pConn);
}
//更新
void update(){
	 MYSQL *pConn;
	 pConn = mysql_init(NULL);
	 if(!mysql_real_connect(pConn,"liyuan.imblog.in", "ams", "123", "dep", 28386, NULL, 0))
	 {
	  printf("无法连接数据库:%s",mysql_error(pConn));
	  return;
	 }

	// mysql_query(pConn,"set character_set_results=utf8");
	 mysql_query(pConn,"SET NAMES utf8");

	 if(mysql_query(pConn,"UPDATE tb_user t SET t.nage='名字修改成功' WHERE t.id='11111'"))
	  {
	   printf("执行update失败%s",mysql_error(pConn));
	   return;
	  }

	 printf("执行update成功，新id=%d",mysql_insert_id(pConn));
	 mysql_close(pConn);
}
