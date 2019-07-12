//111111111111111111111111111111111111111111111111111111111111111111111111
//格式测试:
//邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵
//起始日期:
//完成日期:
//************************************************************************
//修改日志:

//, , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , ,, , ,


//编译:
//g++ -o x ./signal.cpp -lpthread -ggdb3

/*
	本demo 测试进程屏蔽64 个信号之后,
	断言失败, 会不会崩溃退出
	指针越界, 会不会崩溃退出
	陷入死循环后, 能不能再收到信号

	实验结论(实现实录-勿删):
	1.屏蔽信号的预想和实际操作差别很大,
		断言 = 6 + 9, 内核应该会发送两个信号


	2.空指针没有信号产生, 只是所在的线程突然崩溃退出, 没有信号产生.
		无论什么线程, 访问到空指针都会崩溃退出的.

	 7) SIGBUS：		core		 内存访问错误
	11) SIGSEGV：	core		 无效内存引用(指针溢出)

		产生核心转存: core, 并不是: 杀死进程: term.
		但是所在的线程会被终止.


	3.死循环对信号机制的影响:
		程序进入死循环之后, 只有9/19 信号不受影响, 其它都收不到.
		9/19 实际也是内核管理的, 才会收到, 一般信号全部收到不能处理.


	结论: 屏蔽信号并不能阻止进程崩溃, 你可以在退出前做信号处理.
*/



/*
1.发送信号操作
	int kill(pid_t pid, int sig);	成功返回0, 失败返回-1
	第一参数使用说明:
	pid > 0, 则发送信号sig 到指定的进程中
	pid = 0, 发送信号到<同组的每个进程>, 包括自己.
	         ps: 进程组=所有的前台, 后台程序, shell里面的 & 运行
	pid <-1, 先将pid 取绝对值, 取绝对值之后的pid = 进程组ID,
	         对进程组进行信号广播
	pid =-1, 广播发送信号到所有进程中, 除了自身和init 进程之外.


2.给自身发送信号
	int raise(int sig);	成功返回0, 失败返回-1
	相当于: kill(getpid(), sig);	没啥意思


3.给进程组发送信号
	int killpg(pid_t pgrp, int sig);
	相当于: kill(-pgrp, sig);	kill() 发生负数同样能实现这个功能.


4.显示信号描述字符串
	char* strsignal(int sig);\
		肯定不会失败的, 只有当信号不存在的情况下返回NULL


5.发送异步信号, signal()
	sighandler_t signal(int signum, sighandler_t handler);
	参数1: 异步信号类型[1,31].
	参数2: [sighandler_t handler]宏定义:
		SIG_IGN = 屏蔽信号
		SIG_DFL = 重置为默认值
	失败返回: SIG_ERR.
*/



#include <signal.h>
#include <string.h>
#include <stdio.h>
#include <pthread.h>
#include <errno.h>
#include <time.h>
#include <unistd.h>
#include <stdlib.h>



//线程函数1所需的结构体
struct _test{
	int a;
};



//1.屏蔽31 个信号(9 和19 除外)
void ignore_all_signal(void);
//2.恢复32 个信号的默认设置
void reset_all_signal(void);
//3.SIGINT 信号处理函数
void sigint_handler(int sig);
//4.启动信号: SIGINT, 并执行信号处理函数=sigint_handler()
void active_sig(int sig);

//5.线程函数1
void *thread1(void *arg);





int main(void){
	int tmp;



	//1.屏蔽所有信号
	ignore_all_signal();


	//2.测试sig=1 是否被屏蔽, 证明屏蔽有效
	tmp = kill(getpid(), 1);//测试发送'挂起信号'=1
	//int tmp = kill(getpid(), 9);//测试发送'必杀信号'=9
	printf("kill(getpid(%d), 1)=%d\n",getpid(),tmp);
	printf("看到这句话说明:信号屏蔽成功\nkill()发送信号的信号被屏蔽\n");


	//3.启动信号: SIGINT=Ctrl + C
	active_sig(SIGINT);


	//4.测试恢复后的信号, 是否有效
	tmp = kill(getpid(),SIGINT);
	printf("kill(getpid(%d),SIGINT)=%d\n",getpid(),tmp);


	//5.恢复所有信号
	reset_all_signal();


	//6.死循环对信号机制的影响:
	//	程序进入死循环之后, 只有9/19 信号不受影响, 其它都收不到.
	//	9/19 实际也是内核管理的, 才会收到, 一般信号全部收到不能处理.
	/*
	//死循环阻塞, 发生kill -s 6 <pid> 失败, 不响应
	//kill -s 9 <pid> 成功, 响应杀死进程
	//说明: 信号6 已经被屏蔽
	while(1)
		;
	*/


	//7.测试断言是否会发送6 + 9两个信号
	//assert(0);
	//printf("看不到这句话, 说明assert() 6+9 是成立的,\
			断言中断, 会发送两个信号 6+9 ");
	//证明: 屏蔽信号并不能阻止进程崩溃.



	//8.创建线程,分离线程和主进程.让崩溃线程结束不影响主进程.
	pthread_t thid1;
	pthread_create(&thid1,NULL,thread1,NULL);
	printf("你看到这句话说明, 子线程指针越界并不影响父进程继续工作\n");


	//9.测试访问'空指针'崩溃, 会不会收到信号
	//	结论: 无任何信号, 程序直接中断退出.
	//struct _test *p = NULL;//父进程也指针崩溃退出
	//p->a = 1;



	return 0;
}





//1.屏蔽31 个信号(9 和19 除外)
void ignore_all_signal(void){
	int i = 1;



	for(;i <= 31;i++){//屏蔽31 个信号
		if(i == 9)
			continue;
		if(i == 19)
			continue;
		//printf("%d\n",i); for test
		if(signal(i, SIG_IGN) == SIG_ERR){
			printf("signal(%d)-SIG_IGN failed, err:%s\n",i,strerror(errno));
			return;
		}
	}
	//printf("ignore_all_signal() success\n");
}



//2.恢复32 个信号的默认设置
void reset_all_signal(void){
	int i = 1;



	for(;i <= 31;i++){
		if(i == 9)
			continue;
		if(i == 19)
			continue;
		if(signal(i, SIG_DFL) == SIG_ERR){
			printf("signal(%d)-SIG_DFL failed, err:%s\n",i,strerror(errno));
			return;
		}
	}
	//printf("reset_all_signal() success\n");
}



//3.SIGINT 信号处理函数
void sigint_handler(int sig){
	printf("i am the sigint_handler() with SIGINT\n");

	exit(0);
}

//4.启动信号: SIGINT, 并执行信号处理函数=sigint_handler()
void active_sig(int sig){
	if(signal(sig, sigint_handler) == SIG_ERR){
		perror("signal() failed");
		return;
	}
}



//5.线程函数1
void *thread1(void *arg){
	//断言测试
	//assert(0);
	//printf("可以看到这句话说明: 屏蔽信号后, 断言失效\n\n");

	//空指针访问越界测试
	struct _test *p = NULL;
	p->a = 1;
	printf("可以看到这句话说明: 屏蔽信号后, 指针越界失效\n");
}
