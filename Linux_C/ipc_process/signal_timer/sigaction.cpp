//111111111111111111111111111111111111111111111111111111111111111111111111
//格式测试:
//邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵
//起始日期:
//完成日期:
//************************************************************************
//修改日志:

//, , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , ,, , ,


//编译:
//g++ -o x ./sigaction.cpp -ggdb3

//注意: 测试时, 必须根据<pid>, 用kill 命令发送实时信号进行测试.
//			异步信号1-31 默认被屏蔽, 实时信号模拟, 需要用到kill命令.
//			还需要获取pid.

//测试shell 命令：kill -s 2 <pid> (SIGINT)	#无效,异步信号忽略
//					 or kill -s 10 <pid> (SIGUSR1)#无效,异步信号忽略
//							kill -s 64 <pid> (SIGUSR1)#有效,实时信号,且64信号是目标
//							kill -s 35 <pid> (SIGUSR1)#无效,实时信号,35信号不是目标

//kill 命令默认 = kill -s 9 <pid>



//1.实时信号描述结构体
/*
struct sigaction{
	__sighandler_t sa_handler;	//信号响应函数api指针
	__sigset_t sa_mask;					//实时'阻塞信号集'
	int sa_flags;								//实时信号类型
	void (*sa_restorer) (void);	//unknow, 可以不设置.
};
sa_restorer:
	已过时, POSIX不支持它, 不应再使用.
sa_handler:
	此参数和signal()的参数handler相同, 代表新的信号处理函数.
sa_mask:
	标记正在启用的信号/正在屏蔽的信号,是信号描述标准,1024/8*sizeof(long)
sa_flags:
	用来设置信号处理的其他相关操作, 下列的数值可用:
SA_NODEFER: 使对信号的屏蔽无效,即在信号处理函数执行期间仍能发出这个信号.
						一般情况下,当信号处理函数运行时,内核将阻塞该给定信号.
						(sigaction 是实时信号, 所以会阻塞)
						但是如果设置了SA_NODEFER标记,
						那么在该信号处理函数运行时,内核将不会阻塞该信号.
SA_SIGINFO: 当设定了该标志位时,表示信号附带的参数可以传递到信号处理函数中.
						即使sa_sigaction指定信号处理函数,
						如果不设置SA_SIGINFO,信号处理函数不能得到信号传递过来的数据.
						在'信号处理函数中'对这些信息的'访问'都将导致段错误.
						(但实际上, 这个可传递的数据, 只是一个int/void* 指针)
◆ SA_RESTART：使'被信号打断的系统调用'自动重新发起.
◆ SA_NOCLDSTOP：使父进程在它的子进程暂停或继续运行时不会收到SIGCHLD信号.
◆ SA_NOCLDWAIT：使父进程在它的子进程退出时不会收到SIGCHLD 信号,
								这时子进程如果退出也不会成为僵尸进程.
◆ SA_RESETHAND：信号处理之后重新设置为默认的处理方式.
								(默认处理方式会终结进程, 这个选项, 一般不能用!!)

< 这里还可能是3种实时定时器:
	ITIMER_REAL, ITIMER_VIRTUAL,ITIMER_PROF >

*/


//2.<<'阻塞信号集'操作api>>:
/*
//2.1: 清空'阻塞信号集'的act.mask == sigset_t
int sigemptyset(sigset_t *set);
//2.2: 在set指向的'阻塞信号集'中加入signum信号;
int sigaddset(sigset_t *set, int signum);
//2.3: 在set指向的'阻塞信号集'中删除signum信号;
int sigdelset(sigset_t *set, int signum);
//2.4: 判定信号signum是否在set指向的'阻塞信号集'中;
int sigismember(const sigset_t *set, int signum);
*/


//3.实时信号-操作函数
/*
3.1：激活信号
int sigaction(int signum, const struct sigaction *act, \
		struct sigaction *oldact);

3.2：发送信号(kill() 函数实际也是差不多的)
int sigqueue(pid_t pid, int sig, const union sigval value);
	sigqueue() 的第三参数, 是一个整形, 不能修改长度.
	同一进程可以传递指针, 不同进程只能传递Int;
	同时, 需要开启:sa_flags=SA_SIGINFO

	sigqueue() 发送实时信号的时候,需要提供pid,
	’阻塞信号集合‘和进程绑定,
	每个进程当前工作的’阻塞信号集合‘只有一个

	//实时信号-union sigval 的本质, 是一个int.
	//但是内核可以将这个int 解析为void*;
	//所以, 内核愿意用一个union 联用体, 来解析信号int.
	//信号可以是int, 也可以传递一个void* 指针.
	//但, 如果你要传递void* 指针, 必须是统一进程内!
	//或者是子父进程之间, 或者是有'共同的共享内存数据'的数据指针.
	//总之: 你传递的指针, 使用者必须可以找得到这个指针,
	//			否则, 使用者一旦使用了空指针, 就会发生崩溃.
	//本进程内的.heap, .code, .data 区, 都可以找到


3.3：'阻塞信号集'的批量复制/拷贝操作, 阻塞 != 屏蔽
int sigprocmask(int how, const sigset_t *set, sigset_t *oldset);
	how：
		用于指定信号修改的方式, 可能选择有三种:
		SIG_BLOCK
			将*oldset所指向的'阻塞信号集'中的信号'拷贝到‘*set信号集’.
			即'*oldset信号集'和'*set信号集'进行'或操作'.

		SIG_UNBLOCK
			将*oldset所指向的'阻塞信号集'中的信号从'*set信号集'中'删除'.
			即'*oldset信号集'和'*set信号集'进行'与操作'.

		SIG_SETMASK
			将*set'阻塞信号集'设定为新的进程'阻塞信号集'.
			即'*set信号集'对'*oldset信号集'进行'赋值操作'.

	set:
		当前'阻塞信号集'的指针,如果想读取现在的屏蔽值,可将其置为NULL.
	oldset：
		输入模参, '阻塞信号集'指针.

	<<信号掩码集用途>>:
	1.sigprocmask()
		用于改变进程的当前'阻塞信号集',也可以用来检测当前进程的信号掩码

	2.有时候‘不希望’在接到信号时就‘立即停止’当前任务,去处理信号,
		同时'也不希望'忽略该信号,而是延时一段时间去调用信号处理函数.
		这种情况是通过'阻塞信号掩码集'实现的.

	3.信号阻塞和忽略信号的区别:
	>阻塞的概念和忽略信号是不同的.
		内核在信号阻塞期间, 不会讲信号传递出去,
		被阻塞的信号也不会被丢弃, 信号只是暂时被阻止传递.

	>当进程忽略一个信号时, 信号会被传递出去但进程会将信号丢弃.

	>执行信号的处理动作称为信号递达(Delivery),
		信号从产生到递达之间的状态, 称为信号未决(Pending).
		进程可以选择阻塞(Block)某个信号.


3.4：挂起进程, 等待信号到来, 一旦有信号到来, 即唤醒进程.
		[必须先等待实时信号, 才能发送实时信号]
		[实时信号, 需要牺牲一个进程, 等待信号到来??]
		[阻塞就是实时信号的性能代价, 性能消耗成本?]
int sigsuspend(const sigset_t *mask);
	[sigsuspend() always returns -1]--总是返回失败,但执行okay.
	 因此, sigsuspend() 不需要判断返回值!!

	进程执行到sigsuspend()时, sigsuspend()并不会立刻返回.
	此时, 进程进入TASK_INTERRUPTIBLE 状态并立刻放弃CPU,
	等待没有被阻塞的'有效信号'来唤醒.

	进程在接收到'UNBLOCK有效信号'后, 调用处理函数,
	然后把现在的信号集还原为原来的, sigsuspend()返回, 进程恢复执行.


	sigsuspend()原子操作问题：
	实时信号不存在原子操作问题, 只有signal() 异步信号才存在'原子操作问题'

*/



#include <unistd.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>



//注册新的信号值, 不能是已知的1-31, 否则会注册失败
#define sig_num_test 64// 目标信号num



//1.参数信号处理函数(指针)的实现
//这是一个很不好的实现方式, 无论如何都不应该这么做!!
//
//就算在进程内, 为了让传递void* 指针后,
//可以根据'信号指针'找到对应的数据,
//被传递的数据, 必须是全局变量or static 变量
static void handler_point(int signum,siginfo_t *info,void *myact);

//2.参数信号处理函数(整形)的实现
void handler_int(int signum,siginfo_t *info,void *myact);

//3.实时信号测试
void sigaction_test(void);

//4.屏蔽31 个信号(9 和19 除外)
void ignore_all_signal(void);



int main(void){
	printf("main pthread pid = %d\n",getpid());

	//1.测试之前的准备,防止1-31的信号,干扰测试
	ignore_all_signal();

	//2.实时信号测试
	sigaction_test();

	return 0;
}



//1.参数信号处理函数(指针)的实现
//这是一个很不好的实现方式, 无论如何都不应该这么做!!
//
//就算在进程内, 为了让传递void* 指针后,
//可以根据'信号指针'找到对应的数据,
//被传递的数据, 必须是全局变量or static 变量
static void handler_point(int signum,siginfo_t *info,void *myact){
	int i;
	for(i=0;i<10;i++){
		printf("%c-",(*( (char*)((*info).si_ptr)+i)));
	}
	printf("handle signal %d over;\n",signum);
}



//2.参数信号处理函数(整形)的实现
void handler_int(int signum,siginfo_t *info,void *myact){
	printf("signum=%d, siginfo_t* info->si_int=%d, void *myact=%d\n",\
	signum, info->si_int, myact);
}



//3.实时信号测试
void sigaction_test(void){
	struct sigaction sig_act;
	int sig = sig_num_test;//64 - 目标信号num
	union sigval sig_snd_val;//信号-传递参数int(唯一)
	sigset_t sig_set;//阻塞信号集
	int tmp;



	//1.初始化填充struct sigaction
	//sig_act.sa_sigaction = handler_point;//参数信号处理函数(指针)
	sig_act.sa_sigaction = handler_int;//参数信号处理函数(整形)
	sig_act.sa_flags=SA_SIGINFO|SA_RESTART;
	//sig_act.sa_flags=SA_SIGINFO|SA_RESTART|SA_NODEFER;
	sigemptyset(&sig_set);
	sig_act.sa_mask = sig_set;//初始化'阻塞信号集'sa_mask


	//2.屏蔽所有非目标信号, 9/19 除外(不能屏蔽)
	//1-31 都不屏蔽, 31-64, 非目标信号, 全部屏蔽.
	for(tmp=32;tmp<=64;tmp++){
		if(tmp != sig_num_test)//剔除目标信号
			sigaddset(&sig_set,tmp);
	}
	if(sigismember(&sig_set,sig_num_test) == 1)//判断是否操作正确
		printf("sigismember() return a wrong value,\n\
			sig_num_test=%d should not being 'sig_set'\n",sig_num_test);


	//3.激活信号
	if(sigaction(sig, &sig_act, NULL) == -1)
		perror("sigaction() failed");


	//4.常规发送和接收信号操作.
	tmp=fork();
	if(tmp==-1){
		perror("fork() failed");
		return;
	}
	if(tmp==0){
		//子进程
		usleep(2000);//让父进程先跑, 方便完成测试

		//信号附加值赋值
		//sig_snd_val.sival_ptr = data;//指针--只能在本进程内有效
		sig_snd_val.sival_int = 12345678;//整形

		//发送一个有效信号64=sig_num_test给父进程
		if(sigqueue(getppid(), sig_num_test, sig_snd_val) == -1)
			perror("sigqueue() failed");

		//发送一个无效信号35给父进程(不会被丢弃, 但没有起到作用)
		//无效信号35 中断了进程??
		//if(sigqueue(getppid(), 35, sig_snd_val) == -1)
		if(sigqueue(getppid(), 64, sig_snd_val) == -1)
			perror("sigqueue() failed");

		return;//结束子进程
	}
	else{
		//父进程
		usleep(1000);//父进程故意慢一点, 看看父进程不等待子进程的话,
								//有效信号是否会被丢弃.
								//事实证明: 信号不会被丢弃, 但是信号集已经激活.
								//所以, 这是实际上变成了自己给自己发送信号.
		//等待一个有效信号到来
		tmp = sigsuspend(&sig_set);
		if(tmp == -1 && errno == EFAULT)
			printf("sig_set 屏蔽信号集合, 实体地址是个空指针/无效指针\n");
		if(tmp == -1 && errno == EINTR)
			printf("api调用被信号中断;信号(7)\n");
		printf("tmp = sigsuspend() = %d\n",tmp);

		//解除无效信号的阻塞(反转阻塞关系, 有效信号->无效信号)
		sigprocmask(SIG_BLOCK, &sig_set, NULL);

		//等待一个有效信号到来(再尝试一次)
		sigsuspend(&sig_set);

		//等待一个有效信号到来(第三次)
		sigsuspend(&sig_set);
	}

	return;
}



//4.屏蔽31 个信号(9 和19 除外)
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
	printf("ignore_all_signal() success\n");
}



