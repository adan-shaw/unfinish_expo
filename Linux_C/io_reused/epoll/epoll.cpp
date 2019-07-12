//111111111111111111111111111111111111111111111111111111111111111111111111
//格式测试:
//邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵
//起始日期:
//完成日期:
//************************************************************************
//修改日志:
//	2019-05-13: 新增'tty 文本标准格式'风格
//, , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , ,, , ,


//编译
//g++ -o x ./epoll.cpp -ggdb3


/*

1.从粘包问题看epoll:
	epoll 有两种模式, ET模式
	ET模式-边缘模式:
		仅当状态发生变化的时候才获得通知,
		这里所谓的状态的变化并不包括缓冲区中还有未处理的数据,
		也就是说,如果要采用ET模式,需要一直read/write直到出错为止,
		很多人反映为什么采用ET模式只接收了一部分数据就再也得不到通知了;
	LT模式-水平模式:
		是只要有数据没有处理就会一直通知下去的.

	但无论那种模式, 都可能出发'粘包问题';
	例如:
		用epoll监听read事件, 第一次收到epoll read通知, 再去读数据,
		数据未读完, 第二波数据又来了.
		通知仍然不断拥入, 但此时, 你还未退出读取循环.
		就会连第二波的数据, 也读进了装载第一次read 的缓冲区中.
	粘包解决办法1:
		固定每一次数据缓冲区的长度, 补足则填充.
	粘包解决办法2:(这个方法可行)
		分两次发送数据,
		第一次是固定的数据信息元, 格式, 长度固定.
		第二次是数据体, 长度由第一次固定信息元提供.
		传输保证, by来自tcp 协议对数据安全完整传输的可靠性!



2.ET 边缘模式和LT 水平模式的区别:
	select() 是LT 水平模式的io 复用机制,
	LT 水平模式与ET 边缘模式的区别是:
		LT 是一个IO 通知一次, ET 是一个/多个IO 都只通知一次;
	从'粘包'案例分析ET/LT 模式的区别:
		多个IO 涌入的时候, 就会发生粘包的情况, 这时候,
		如果是LT 模式, 则: 无论多少个包, 逐个包通知一次.
		如果是ET 模式, 则: 无论多少个包, 只通知一次.
	如过你调用select()/epoll_wait(),
	无论是什么模式, 都视作从内核取走fd 通知事件,
	内核会自动消除一次fd 通知事件.

	如果是LT 模式, 通知是有序的, 你根本不需要理会内核如何消除fd 通知事件;
	如果是ET 模式, 会很乱!!
		有可能,断断续续,多次积累成一次,也有可能是一次分成2 次fd 通知事件(重发);
		这时候就应该, 按照接受数据的长度, 逐节点接受, 按缓冲区大小接收数据.
		你还最好, 设置缓冲区限额长度:
			80 字节(64+16), 只有一个TCP 独立包, 80+20+20=120 字节(小数据)
			2048 字节, 一个mbuf + 一个簇(中型数据)
			4096 字节, 刚好一个4K 页(中型数据)
			4K*n 字节, n 个4k 页的数据(大数据)

	由此可见, LT 水平模式的内核消耗, 比ET 边缘模式大很多!!
	select/poll 的LT 水平模式, 又要比epoll 的LT 水平模式, 消耗更多.
	因此epoll 即使是LT 水平模式, 但都确保'事件通知'fd 集合, 只拷贝一次;
	select/poll 的LT水平模式,则需要从用户态往内核态拷贝一次,实际拷贝两次了.



3.epoll apt 解析:
(1).epoll 的数据结构实验方式:
	1.兴趣列表
	2.就绪列表-负责通知事件(信号就绪, 并非数据就绪)
要理解其实很简单:
	你将fd 推入兴趣列表后,如果该fd发生信号异动,
	内核就会调用callback 函数, 将该fd添加到信号就绪列表中,
	供epoll_wait()提取fd 事件.
	(epoll_wait() 取出事件后, 内核会自动删除该fd事件)

	你将fd踢出epoll, 就需要从兴趣列表中删除fd, 实现fd 数量守恒.
	如果你贸然close(fd), 那么内核会自动从epoll兴趣列表中剔除,
	但是此时, 你的epoll fd 数量不守恒了, 这可能会带来混乱.
	所以,即使对端socket自动关闭了,你服务器,也要按序清退坏死socket服务者

(2).max_user_watches 上限:
	查看方式(需要root 权限):
		cat /proc/sys/fs/inotify/max_user_watches
	临时修改方式(需要root 权限):
		echo 8192000 > /proc/sys/fs/inotify/max_user_watches

	linux 进程打开文件fd 数量上限:
	查看方式:
		ulimit -n
	临时修改方式:
		ulimit -HSn 2048 # 临时修改方式, 重启失效
	永久修改方式:
		略
	查看整个linux 可打开的fd 数量:
		cat /proc/sys/fs/file-max

(3).3 个操作:
>1.epoll_create(n) 创建epoll, 并初始化n 个元素的兴趣列表.
		(一般情况下空值参数也可以, 直接创建即可)

>2.epoll_ctl(), 三个操作:
	1.add, 2.mod, 3.del (无非就是增, 改, 删. )
	实际语义是: 操作fd->兴趣列表

>3.epoll_wait()[异步版本]:
返回值:
	-1(出错), 应该查看errno
	0(超时)--(阻塞-epoll_wait 第四参数控制阻塞多少毫秒超时)
	n>0(返回发生信号异动的fd 数量count)

如果返回了n>0, 则有fd 产生通知事件, 则需要判别fd 的信号类型:
*1.epollin			读信号
*2.epollrdhup		对端关闭信号
*3.epollet			边缘触发
*4.epollloneshot	一次性epoll信号通知,通知一次后,就会禁用epoll通知,
									(相当于epoll_ctl-del)

*5.epollhup		挂断(挂断和错误并不需要理会, 两者通常是or 组合在一起)
*6.epollerror	错误

>4.epoll_pwait()[阻塞版本]:
	类似pselect(), 如果你需要调用阻塞epoll_wait(),
	又不想让信号中断你的epoll_wait(),那么epoll_pwait()是你的最佳选择.



4.epoll_wait() 应该创建多大的epoll_event[] ??
	实际,你想一次处理多少个epoll事件,那么就设置多大的epoll_event[],
	因为一次epoll_wait() 之后, 未被处理的fd 仍然不会有任何变动.
	未被处理,就是fd事件未被取出,无论是ET/LT模式,都不会有任何变动的.
	未被处理, 下次epoll_wait() 还能继续取出, 不受影响.

	所以这epoll_event[]的数量大小, 还是根据程序逻辑来取比较好.
	越小越好, 这样内核CP负担会比较小.

	一个epoll_event 的实际大小是12 字节.
	4096=1024*4, 12=3*4; 除掉4, 得1024:3.
		(小)12*256=3072 字节(3/4 页, 3K).
		(中)12*512=3/4 页*2
		(大)12*2048=3 页 = 4KB*3.

参考网文:
http://bbs.chinaunix.net/thread-1690835-1-1.html

	另外:
		epoll_create() 创建的时候, 决定了epoll内核选择的数据结构体.
		但现已被废弃,从Linux2.6.8开始,大小参数被忽略,但必须大于零.
		(一般也给一个恰当的值吧, 8192 最好)
*/



#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>//for signal()
#include <sys/time.h>//for setitimer()
#include <sys/epoll.h>



#define EPOLL_CREATE_DEFAULT_VAL 8192
#define EPOLL_EVENTS_MAX 256



//1.打印信号类型(signal() 异步-循环式)
void print_sig_type(int sig);

//2.打印信号类型(sigaddset() 实时信号-延时式定时器-一次性)
void print_sig_type2(int sig);

//3.测试epoll_wait()
void epoll_wait_test(void);

//4.测试epoll_pwait()
void epoll_pwait_test(void);



int main(void){
	//一个struct epoll_event = 12 字节.
	printf("sizeof(struct epoll_event)=%d\n",sizeof(epoll_event));

	//1.测试epoll_wait()
	printf("\n\n1.测试epoll_wait()\n");
	epoll_wait_test();

	printf("\n\n\n\n\n");

	//2.测试epoll_pwait()
	printf("2.测试epoll_pwait()\n");
	epoll_pwait_test();

	return 0;
}





//1.打印信号类型(signal() 异步-循环式)
void print_sig_type(int sig){
	if(sig == SIGALRM)
		printf("signal() SIGALRM=%d\n",sig);
	else if(sig == SIGVTALRM)
		printf("signal() SIGVTALRM=%d\n",sig);
	else
		printf("signal() unknow signal=%d\n",sig);
	return;
}

//2.打印信号类型(sigaddset() 实时信号-延时式定时器-一次性)
void print_sig_type2(int sig){
	if(sig == SIGALRM)
		printf("sigaction() SIGALRM=%d\n",sig);
	else if(sig == SIGVTALRM)
		printf("sigaction() SIGVTALRM=%d\n",sig);
	else
		printf("sigaction() unknow signal=%d\n",sig);
	return;
}



//3.测试epoll_wait()
void epoll_wait_test(void){
	int epfd,fd,tmp;
	int timeout=0;//异步
	int epfd_alive_count;//epoll当前'兴趣列表'中fd的数量count
	epoll_event ev,evs[EPOLL_EVENTS_MAX];
	char buf[256];



	//0.初始化准备
	fd=fileno(stdin);//获取stdin FILE* 的fd
	epfd_alive_count=0;//初始化epoll当前‘兴趣列表’中fd的数量count


	//1.创建epoll 句柄
	epfd = epoll_create(EPOLL_CREATE_DEFAULT_VAL);
	if(epfd == -1){
		perror("epoll_create() failed");
		return;
	}


	//2.将fd 加入到epoll 中
	ev.data.fd=fd;
	ev.events=EPOLLIN|EPOLLET;//‘感兴趣事件’
	epoll_ctl(epfd,EPOLL_CTL_ADD,fd,&ev);//新增
	epfd_alive_count++;
	//epoll_ctl(epfd,EPOLL_CTL_DEL,fd,&ev);//删除
	//epfd_alive_count--;

	//修改-一般用作修改‘感兴趣事件’, 或者LE/LT 模式
	//epoll_ctl(epfd,EPOLL_CTL_MOD,fd,&ev);


	//3.epoll_wait() 测试：循环等待stdin 输入n 串字符串
	while(1){
		tmp = epoll_wait(epfd,evs,epfd_alive_count,0);//异步模式0=非阻塞
		if(tmp == -1){
			perror("epoll_wait() failed");
			break;
		}
		else if(tmp == 0)
			continue;//只是超时
		else{
			//收到stdin io事件, 从stdin获取一串字符串
			memset(buf,'\0',256);
			fgets(buf,256,stdin);
			printf("fgets() a string from stdin:%s\n",buf);
			break;
		}
	}//while end


	ev.data.fd=fd;
	ev.events=0;//随意值
	epoll_ctl(epfd,EPOLL_CTL_DEL,fd,&ev);//将fd删除从epoll中删除
	epfd_alive_count--;

	close(epfd);
	return;
}



//4.测试epoll_pwait()
void epoll_pwait_test(void){
	int epfd,fd,tmp;
	int timeout=0;//异步
	int epfd_alive_count;//epoll当前‘兴趣列表’中fd的数量count
	epoll_event ev,evs[EPOLL_EVENTS_MAX];
	char buf[256];

	sigset_t sigmask;//实时信号集合
	struct itimerval value;//实时信号-定时器触发周期



	//0.初始化准备
	fd=fileno(stdin);//获取stdin FILE* 的fd
	epfd_alive_count=0;//初始化epoll当前‘兴趣列表’中fd的数量count



	//*****************************************************
	//选择1: 异步 or 实时 定时器, 只能选其一
	//			选择两个都可以, 但是没啥使用意义,
	//			一般程序不会同时有'异步 or 实时 定时器',
	//			但pselect() 是根据信号类型来屏蔽的,所以即使两个开启都可以.

	//设置'异步'定时器, 尝试中断pselect()
	signal(SIGALRM,print_sig_type);

	//设置'同步实时'定时器的延时, 尝试中断pselect()
	value.it_interval.tv_sec = 2;
	value.it_interval.tv_usec = 0;
	value.it_value.tv_sec = 2;
	value.it_value.tv_usec = 0;
	if(setitimer(ITIMER_REAL,&value,NULL) == -1){
		perror("setitimer() failed");
		return ;
	}
	//初始化实时信号集合
	if(sigemptyset(&sigmask) == -1){
		perror("sigemptyset() failed");
		return ;
	}
	//激活SIGALRM 定时器实时信号-单次
	if(sigaddset(&sigmask,SIGALRM) == -1){
		perror("sigaddset() failed");
		return ;
	}
	//*****************************************************



	//1.创建epoll 句柄
	epfd = epoll_create(EPOLL_CREATE_DEFAULT_VAL);
	if(epfd == -1){
		perror("epoll_create() failed");
		return;
	}


	//2.将fd 加入到epoll 中
	ev.data.fd=fd;
	ev.events=EPOLLIN|EPOLLET;//‘感兴趣事件’
	epoll_ctl(epfd,EPOLL_CTL_ADD,fd,&ev);//新增
	epfd_alive_count++;
	//epoll_ctl(epfd,EPOLL_CTL_DEL,fd,&ev);//删除
	//epfd_alive_count--;

	//修改-一般用作修改‘感兴趣事件’, 或者LE/LT 模式
	//epoll_ctl(epfd,EPOLL_CTL_MOD,fd,&ev);


	//3.epoll_wait() 测试：循环等待stdin 输入n 串字符串
	while(1){
		timeout = -1;//阻塞死等
		tmp = epoll_pwait(epfd,evs,epfd_alive_count,timeout,&sigmask);
		if(tmp == -1){
			perror("epoll_pwait() failed");
			break;
		}
		else if(tmp == 0)
			continue;//只是超时
		else{
			//收到stdin io事件, 从stdin获取一串字符串
			memset(buf,'\0',256);
			fgets(buf,256,stdin);
			printf("fgets() a string from stdin:%s\n",buf);
			break;
		}

		sleep(2);	//反向测试, 等待时钟中断程序(有效)
							//但是, 这里只是证明epoll_pwait() 期间,
							//epoll_pwait()阻塞不受SIGALRM 信号影响,
							//但epoll_pwait()在受到stdin read事件后还是会返回
							//所以程序还是会结束的
	}//while end


	ev.data.fd=fd;
	ev.events=0;//随意值
	epoll_ctl(epfd,EPOLL_CTL_DEL,fd,&ev);//将fd删除从epoll中删除
	epfd_alive_count--;

	close(epfd);
	return;
}
