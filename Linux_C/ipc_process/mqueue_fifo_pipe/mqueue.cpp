//111111111111111111111111111111111111111111111111111111111111111111111111
//格式测试:
//邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵
//起始日期:
//完成日期:
//************************************************************************
//修改日志:

//, , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , ,, , ,


//编译:
//g++ -o x ./mqueue.cpp -lrt -ggdb3

//<1>.posix mqueue 简介
/*
*1. 消息队列的劣势:
	消息队列也是依赖信号来做通知,
	利用<信号>来通知<对方进程>取消息的,(所以<消息队列>是异步的)
	收到通知之后, 再从消息队列里面提取出通知,
	真心还不如管道简便.


*2. 消息队列较管道的优势:
	不需要另外<某个进程>在该队列上<等待消息的达到>,这一点与管道和FIFO相反.
	某个进程往一个消息队列中写入消息之后,
	由内核通过信号通知对方来去消息, 但不保证消息一定会送达.


*3: 'Posix消息队列'与'System V消息队列'的区别如下：
(1):对Posix消息队列的读总是返回最高优先级的最早消息,
		对System V消息队列的读则可以返回任意指定优先级的消息.

(2):当往一个空队列放置一个消息时,
		Posix消息队列允许产生一个信号或启动一个线程,
		System V消息队列则不提供类似的机制.


*4：mqueue 消息队列极限:
	一个消息队列最多能有10条消息, 每条消息的最大长度为8192字节,
	一个系统最多能有256个消息队列.

*/



//<2>.posix mqueue(消息队列结构体和api接口):
//编译选项 -l rt
/*

-2.2-api>>:
mqd_t	mq_open();//打开/创建一个消息队列
									(存在该mqueue则打开,不存在则创建mqueue)
int	mq_close();	//关闭一个消息队列
									(本进程退出mqueue,不再接收mqueue的消息通知)
int	mq_unlink();//删除一个消息队列
									(释放操作-彻底清除该mqueue)
	(mq_unlink()删除指定的消息队列, 并不一定会立即执行.
	如果当前仍有其它进程正在使用消息队列, 则标记删除操作, 并立即返回,
	直到其它进程通过调用mq_close()关闭之后, 再进行删除操作. 回收资源)


int	mq_getattr();//获取消息队列属性
int	mq_setattr();//设置消息队列属性
int	mq_notify();//绑定<消息队列>与<信号>
	注意: 异步<响应>方式有两种:
	>*1.产生一个信号, 执行信号函数;
	>*2.创建一个线程来执行一个指定的函数;
			自建线程执行<消息队列>任务, 实际只是执行任务的方式,
			并不是<消息队列>传递数据的方式.
			<消息队列> 有<传递消息>和<同步>两种作用.


int	mq_send();//发送消息
ssize_t	mq_receive();//接收消息


int	mq_timedsend();//限时发送
ssize_t	mq_timedreceive();//限时接收.


 */



#include <stdbool.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <signal.h>
#include <fcntl.h>//mq_open() 函数需要的依赖
#include <sys/stat.h>
#include <mqueue.h>



#define MQUEUE_NAME "/mq_test"//注意命名,应该/开头
#define MQ_AUTH 0666//权限



//1.信号执行函数
void sig_usr1(int sig){
	printf("sig_usr1 = %d\n", sig);
}

//2.消息队列测试函数
void mqueue_test(void);





int main(void){
	//1.消息队列测试函数
	mqueue_test();

	return 0;
}



//2.消息队列测试函数(需要整体整改.)
void mqueue_test(void){
	int mode,auth;//模式/标志
	mqd_t mqid;//消息队列id
	struct mq_attr attr;//消息队列属性
	struct sigevent sigev;//信号事件改写体(mq_notify() 改写为mq事件)
	void *recv_buf,*send_buf;//消息的缓冲区指针
	unsigned long int recv_len,send_len;//消息的缓冲区长度
	struct timespec send_time,recv_time;//mq 限时消息-限时容器
	int tmp;
	int main_pid=getpid();



	//1.非阻塞式<创建>mqueue
	//	mqueue 需要依赖信号和fd特性.
	//	mqueue 依赖信号做消息通知
	//	mqueue 依赖fd 特性来绑定进程-消息队列, 方便内核维护'消息队列'
	//mode = O_CREAT | O_RDWR | O_RDONLY | O_WRONLY;//原子操作选项
	mode = O_CREAT | O_RDWR;//原子操作选项
	auth = MQ_AUTH;//权限
	mqid = mq_open(MQUEUE_NAME, mode, MQ_AUTH, NULL);
	if(mqid == -1){
		perror("mq_open() failed");
		return ;
	}


	//2.获取消息队列属性
	if(mq_getattr(mqid, &attr) == -1){
		perror("mq_getattr() failed");
		goto failed;
	}



	//3.设置信号, 作为'消息队列'的接收通知(没有实用意义, 这里屏蔽)
	//	但是, 你可以通过控制信号, 来监管mq 消息的接收.
	//	但是, 必须是实时信号, 才能控制, 监管.
	//	如果是异步信号, 直接被丢失！！
	//	这里用的是signal() 异步信号, 可能会丢失, 不能被监管!
	//	异步消息是即时的!!
	//sigset_t mask;//'屏蔽信号集'掩码
	/*
	sigemptyset(&mask);//先清空'屏蔽信号集'
	//占用SIGUSR1用户信号,作为消息队列通知信号.
	//信号SIGUSR1,SIGUSR2是为数不多的用户自定义信号,你可以随便使用
	if(sigaddset(&mask, SIGUSR1) == -1){
		perror("sigaddset() failed");
		goto failed;
	}
	*/



	//4.设置<信号处理函数>(异步信号即可, 你也可以切换为实时信号模式)
	//	这里证明:实时'屏蔽信号集',可以屏蔽1-64个信号,无论是'实时还是异步'信号,都可屏蔽
	if(signal(SIGUSR1, sig_usr1) == SIG_ERR){
		perror("signal() failed");
		goto failed;
	}


	//5.将<消息队列>与<信号>绑定在一起, 并激活信号.
	//	以后该进程收到该'消息队列'的消息, 内核就会发出SIGUSR1信号通知.
	sigev.sigev_notify = SIGEV_SIGNAL;
	sigev.sigev_signo = SIGUSR1;
	if(mq_notify(mqid, &sigev) == -1){
		perror("mq_notify() failed");
		goto failed;
	}


	printf("mqueue test is ready !!let's go on\n");
	//6.开始测试发送'消息'到消息队列中;
	//	开始测试从消息队列中接收'消息'.
	tmp = fork();
	if(tmp == -1){
		perror("fork() failed");
		goto failed;
	}
	if(tmp == 0){
		//子进程-发送消息
		send_buf = malloc(attr.mq_msgsize);
		send_len = attr.mq_msgsize;

		//6.1.1: mq_send();//发送即时消息
		strncpy((char*)send_buf,"message from mq_send()",send_len);
		tmp = mq_send(mqid, (char*)send_buf, send_len, NULL);
		if(tmp == -1){
			perror("mq_send() failed");
			if(errno == EAGAIN)
				printf("attention: errno == EAGAIN !!");
			goto failed;
		}

		//6.1.2: mq_timedsend();//限时发送消息
		send_time.tv_sec = 0;
		send_time.tv_nsec = 1000;
		memset(send_buf,'\0',send_len);
		strncpy((char*)send_buf,"message from mq_timedsend()",send_len);
		tmp = mq_timedsend(mqid, (char*)send_buf, send_len, NULL, &send_time);
		if(tmp == -1){
			perror("mq_timedsend() failed");
			if(errno == EAGAIN)
				printf("attention: errno == EAGAIN !!");
			goto failed;
		}

		////6.1.3: 释放资源
		free(send_buf);
	}
	else{
		//父进程-读取消息
		recv_buf = malloc(attr.mq_msgsize);//分配消息缓冲区
		recv_len = attr.mq_msgsize;


		//6.2.1: mq_receive(); 接收即时消息
		memset(recv_buf,'\0',recv_len);
		tmp=mq_receive(mqid,(char*)recv_buf,attr.mq_msgsize,0);
		if(tmp >= 0)
			printf("SIGUSR1 received, read %ld bytes. data = %s\n",\
					(long)tmp, (char*)recv_buf);
		else if(errno == EAGAIN)
			printf("mq_receive errno == EAGAIN\n");
		else
			perror("mq_receive() failed");


		//6.2.2: mq_timedreceive(); 限时接收消息
		recv_time.tv_sec = 0;
		recv_time.tv_nsec = 1000;
		memset(recv_buf,'\0',recv_len);
		tmp=mq_timedreceive(mqid,(char*)recv_buf,recv_len,0,&recv_time);
		if(tmp >= 0)
			printf("SIGUSR1 received, read %ld bytes. data = %s\n",\
					(long)tmp, (char*)recv_buf);
		else if(errno == EAGAIN)
			printf("mq_receive errno == EAGAIN\n");
		else
			perror("mq_timedreceive() failed");


		//6.2.3: 释放资源
		free(recv_buf);
	}


failed:
	mq_close(mqid);//本进程退出mqueue, 不再接收mqueue的消息通知
	if(getpid() == main_pid)//只有主进程可以清除mqueue
		mq_unlink(MQUEUE_NAME);//彻底清除该mqueue
	return ;
}

