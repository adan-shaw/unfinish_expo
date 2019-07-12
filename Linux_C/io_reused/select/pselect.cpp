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
//g++ -o x ./pselect.cpp -ggdb3

/*

1.select 操作宏:
void FD_CLR(int fd, fd_set *set);
	调用FD_ISSET()查找fd是否存在,
	存在则删除, 不存在则什么都不做.

void FD_SET(int fd, fd_set *set);
	调用FD_ISSET()查找fd是否存在,
	存在则什么都不做, 不存在则在最靠近fd_set[0]处添加fd.

void FD_ZERO(fd_set *set);
	重置整个fd_set数组

int FD_ISSET(int fd, fd_set *set);
询问fd_set集合中, 指定的fd是否存在, 存在=1, 不存在=0.



2.select 查找算法推断
[查看源码得知, FD_ISSET() 的查找方式是: 顺序if, 简单的数组遍历]
[那么FD_CLR()必然会调用FD_ISSET()找到fd位置, 然后再删除]
[那么FD_SET()必然会调用FD_ISSET()查找是否有重复元素,然后再插入/重置]
[推论: select 所有的查找算法, 都是顺序if 数组元素]



3.select()分析:
	3.1:
		select() 第一参数nfds 必须为最大的值的fd_max + 1,
		因为fd_set 数组游标, 是从[0, fd_max]顺序遍历询问.
	3.2:
		参数timeout设置为NULL空指针，就表示永远等待;
		否则就有等待超时, 0 秒超时就是异步询问.
	3.3:
		由于select()一般都可以被信号中断阻塞, 那么select()一般是异步使用的.
		如果需要阻塞等待, 请用pselect() 确保阻塞等待有效.



4.pselect()分析:
	4.1:
		可以屏蔽'信号中断'对'阻塞等待select()事件'的影响,
		理论上, select()阻塞超时和无限等待, 都应该用pselect()函数.
		异步select() 可以不用这个函数.
		客户端必须可以用这个函数, 来防止抵赖.
		服务器下发数据, 客户端即使退出, 也要将数据接受完毕再说.
	4.2:
		pselect() 所使用的一切, 与select() 无异,
		包括fd_set FD_ISSET() 等一切
	4.3:
		pselect() 主要是屏蔽信号类型, 比如'定时器信号=SIGALRM',
		无论实时信号, 还是异步信号, pselect() 一概不管.



5.ET 边缘模式和LT 水平模式的区别:
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
		有可能, 断断续续, 多次积累成一次, 也有可能是一次分成2 次fd 通知事件(重发);
		这时候就应该, 按照接受数据的长度, 逐节点接受, 按缓冲区大小接收数据.
		你还最好, 设置缓冲区限额长度:
			80 字节(64+16), 只有一个TCP 独立包, 80+20+20=120 字节(小数据)
			2048 字节, 一个mbuf + 一个簇(中型数据)
			4096 字节, 刚好一个4K 页(中型数据)
			4K*n 字节, n 个4k 页的数据(大数据)

	由此可见, LT 水平模式的内核消耗, 比ET 边缘模式大很多!!
	select/poll 的LT 水平模式, 又要比epoll 的LT 水平模式, 消耗更多.
	因此epoll 即使是LT 水平模式, 但都确保'事件通知'fd 集合, 只拷贝一次;
	select/poll 的LT 水平模式, 则需要从用户态往内核态拷贝一次, 实际拷贝两次了.


参考网文:
https://blog.csdn.net/feng19870412/article/details/8995951
https://blog.csdn.net/sukhoi27smk/article/details/44084173

*/



#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <sys/select.h>//for select()
#include <signal.h>//for signal()
#include <sys/time.h>//for setitimer()



//1.打印信号类型(signal() 异步-循环式)
void print_sig_type(int sig);

//2.打印信号类型(sigaddset() 实时信号-延时式定时器-一次性)
void print_sig_type2(int sig);

//3.测试fd_set数组的容量，探索FD_SET 的原理.
void test_fd_set(void);

//4.pselect() 最简单的demo[阻塞型]
void pselect_easy_demo(void);



int main(void){
	//1.fd_set 容量测试
	//test_fd_set();

	//2.pselect() 测试
	pselect_easy_demo();

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



//3.测试fd_set数组的容量，探索FD_SET 的原理.
void test_fd_set(void){
	//fd 在select 中只是一个数字, 但fd 的数字不能重复
	int fd = 1;//fd 数字重复test-结果: 失败!!
	int tmp;
	fd_set xset;



	//测试单个fd_set 可容纳的fd 数量,
	//标准posix = 1024 个fd; debian9.0=2024
	for(tmp=0;tmp<2024;tmp++){
		printf("%d  ",tmp);
		FD_SET(tmp,&xset);//fd_set 中不能有重复fd 值, 重复即重置！！
		//FD_SET(1,&xset);//错误用法, fd 永远为1
	}

	return ;
}



//4.pselect() 最简单的demo[阻塞型]
void pselect_easy_demo(void){
	sigset_t sigmask;//实时信号集合
	fd_set rset;//pselect()的fd 集合
	struct itimerval value;//实时信号-定时器触发周期
	int maxfd;//pselect() fd max 统计器
	int tmp;
	//pselect() 阻塞超时-这里用到阻塞死等, 废弃
	//struct timeval timeout;


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



	while(1){
		FD_ZERO(&rset);//每次都清空fd_set集合
		FD_SET(fileno(stdin),&rset);//以stdin 为pselect() 检索对象
		maxfd = fileno(stdin) + 1;//计算pselect() 第一参数fd_max

		//选择2: test1 or test2 只能选其一

		//test1:
		//检验pselect() 的有效性, 使用test1 时, 肯定会被SIGALRM 中断程序
		//可以是signal() 异步信号, 也可以是实时信号, 他们都能中断程序.
		sleep(5);

		//test2:
		//阻塞死等, 看pselect() 会不会受'实时信号'影响,
		//事实上, 只要你启动test2, 程序就是一个陷入阻塞的假死程序.
		//因为此时stdin 完全没有read 事件, pselect() 陷入假死, 不能终止.
		//tmp = pselect(maxfd,&rset,NULL,NULL,NULL,&sigmask);
		if(tmp == -1)
			perror("pselect() failed");
		else
			printf("pselect() action fd count = %d\n",tmp);

		break;//由于pselect() 阻塞死等, 测试一次即可
	}
	return ;
}
