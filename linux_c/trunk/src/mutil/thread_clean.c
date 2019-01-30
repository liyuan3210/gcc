/**********************************************************
*实验要求：   在程序中创建一个线程，使用线程API对该线程进行清理工作。
*功能描述：   创建线程，并在其中使用函数pthread_cleanup_push和函数
*           pthread_cleanup_pop，验证这两个清理函数的效果。
**********************************************************/
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
/*
 * 线程清理函数
 * */
void *clean(void *arg)
{
    printf("cleanup :%s\n",(char *)arg);
    return (void *)0;
}
/*
 * 线程1的执行函数
 * */
void *thr_fn1(void *arg)
{
    printf("thread 1 start  \n");
 /*将线程清理函数压入清除栈两次*/
    pthread_cleanup_push( (void*)clean,"thread 1 first handler");
    pthread_cleanup_push( (void*)clean,"thread 1 second hadler");
    printf("thread 1 push complete  \n");
    if(arg)
    {
        return((void *)1); //线程运行到这里会结束，后面的代码不会被运行。由于是用return退出，所以不会执行线程清理函数。
    }
    pthread_cleanup_pop(0);
    pthread_cleanup_pop(0);
    return (void *)1;
}
/*
 * 线程2的执行函数
 * */
void *thr_fn2(void *arg)
{
    printf("thread 2 start  \n");
 /*将线程清理函数压入清除栈两次*/
    pthread_cleanup_push( (void*)clean,"thread 2 first handler");
    pthread_cleanup_push( (void*)clean,"thread 2 second handler");
    printf("thread 2 push complete  \n");
   
 if(arg)
    {
        pthread_exit((void *)2);//线程运行到这里会结束，后面的代码不会被运行。由于是用pthread_exit退出，所以会执行线程清理函数。执行的顺序是先压进栈的后执行，即后进先出。
    }
    pthread_cleanup_pop(0);
    pthread_cleanup_pop(0);
    pthread_exit((void *)2);
}
/*
 * 程序入口
 * */
int main(void)
{
    int err;
    pthread_t tid1,tid2;
    void *tret;
 /*创建线程1并执行线程执行函数*/
    err=pthread_create(&tid1,NULL,thr_fn1,(void *)1);
    if(err!=0)
    {
        printf("error .... \n");
        return -1;
    }
 /*创建线程2并执行线程执行函数*/
    err=pthread_create(&tid2,NULL,thr_fn2,(void *)1);
 if(err!=0)
    {
        printf("error .... \n");
        return -1;
    }
 /*阻塞等待线程1退出，并获取线程1的返回值*/
    err=pthread_join(tid1,&tret);
    if(err!=0)
    {
        printf("error .... \n");
        return -1;
    }
    printf("thread 1 exit code %d  \n",(int)tret);
 /*阻塞等待线程2退出，并获取线程2的返回值*/
    err=pthread_join(tid2,&tret);
    if(err!=0)
    {
        printf("error .... ");
        return -1;
    }
 printf("thread 2 exit code %d  \n",(int)tret); 
    return 1;
}
 
