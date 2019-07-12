//111111111111111111111111111111111111111111111111111111111111111111111111
//格式测试:
//邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵
//起始日期:
//完成日期:
//************************************************************************
//修改日志:

//, , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , ,, , ,



//编译:
//g++ -o x ./timer.cpp -ggdb3



//1.定时器-实体与定时器信号
//	(启动定时器, 需要将'定时器与信号'绑定并激活'信号')
/*
	linux系统给每个进程提供了3个定时器,
	每个定时器实体,也绑定了一个固定信号类型,
	每个定时器在各自不同的域里面计数.

	当任何一个timer计数到结束,
	系统就'发一个信号'给该进程,进程收到信号后,就会重置定时器的计数.


	(信号类型=ITIMER_REAL, 表示'绑定并激活'该'定时器信号')
	一共支持以下3中计数器形式：
	ITIMER_REAL
		数值为0, 计时器的值实时递减, 发送的信号是SIGALRM.
		实时, 就是真实时间减少出发的.
	ITIMER_VIRTUAL
		数值为1, 进程执行时递减计时器的值, 发送的信号是SIGVTALRM.
		进程计时器, 就是进程运行命令所消耗的时间做统计
	ITIMER_PROF
		数值为2, 进程和系统执行时都递减计时器的值,
		发送的信号是SIGPROF, 进程和系统联合消耗时间, 做统计
*/


//2.旧时异步, '非实时'的定时信号'绑定激活方式'
//	(SUSv4 已经删除, 不建议用, 建议还是直接用posix 定时器)
//	就算你激活实时定时器, ITIMER_REAL,
//	但是信号传递的方式, 仍然可能不是实时的, 信号可能会丢失.
//	虽然机会比较低, 所以posix 才会删除这种旧式定时器.
/*
	2.1 '循环-间隔'定时器, 定期向进程发送SIGALRM 信号.
	int getitimer(int which, struct itimerval *curr_value);
	int setitimer(int which, const struct itimerval *new_value,\
			struct itimerval *old_value);


	2.2 alarm 一次性定时器, 类似c 语言的sleep().
			时间一到, 会向进程发送SIGALRM 信号.
	unsigned int alarm(unsigned int seconds);
*/


//3.posix同步, '实时'的定时信号'绑定激活方式'
/*
	ps: 这里没有什么特殊的用法, 其实定时器, 就是定时器信号:SIGALRM
			所以, 实时定时器=sigaction(SIGALRM),
			用实时的方式, 激活SIGALRM信号, 你就获得一个实时定时器.
			这个的定时器有3 个(默认有3 个)
*/


//4.定时器, 信号的本质!!
/*
	实时信号-union sigval 的本质, 是一个int.
	但是内核可以将这个int 解析为void*;
	所以, 内核愿意用一个union 联用体, 来解析信号int.
	信号可以是int, 也可以传递一个void* 指针.
	但, 如果你要传递void* 指针, 必须是统一进程内!
	或者是子父进程之间, 或者是有'共同的共享内存数据'的数据指针.
	总之: 你传递的指针, 使用者必须可以找得到这个指针,
				否则, 使用者一旦使用了空指针, 就会发生崩溃.
	本进程内的.heap, .code, .data 区, 都可以找到
*/



#include <signal.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/time.h>
#include <sys/types.h>
#include <errno.h>
#include <assert.h>



//统一的'定时器-任务'count
static int printMes_count = 0;



//统一的'定时器-任务'for test
void printMes(int signo);

//用旧时信号的方式, 绑定定时器信号, 并激活该定时信号.
//输入: sig = 定时器类型; ptick = 定时器间隔时钟
//	ITIMER_REAL			= SIGALRM 信号
//	ITIMER_VIRTUAL	= SIGVTALRM 信号
//	ITIMER_PROF			= SIGPROF 信号
void old_setup_timer(int timer_type,struct itimerval *ptick);

//测试'旧式signal() 设置方式'的定时器
//(不安全, 实时定时器, 但是信号传递方式不是实时的)
void old_timer_test(void);



//参数信号处理函数(指针)的实现
//这是一个很不好的实现方式, 无论如何都不应该这么做!!
//
//就算在进程内, 为了让传递void* 指针后,
//可以根据'信号指针'找到对应的数据,
//被传递的数据, 必须是全局变量or static 变量
static void handler_point(int signum,siginfo_t *info,void *myact);

//参数信号处理函数(整形)的实现
void handler_int(int signum,siginfo_t *info,void *myact);

//实时信号定时器测试
void setup_timer_test(void);

//设置一个实时信号为: 实时定时器信号.
//参数1-sig_num必须是: [32,64] 之间的数字,
//系统一共有64 个信号[1,64], 前[1,31]是异步信号, 不可覆盖!
//实时信号区间: [32,64]
//参数2-preal_sig_info必须指明: 定时器类型, 和信号响应函数
void setup_timer(int sig_num,struct sigaction *preal_sig_info,\
		struct itimerval *ptick);





//主测试函数
int main(void){
	//1.测试'旧式signal() 设置方式'的定时器
	//old_timer_test();

	//2.实时信号定时器测试
	setup_timer_test();
	return 0;
}



//统一的'定时器-任务'for test
void printMes(int signo){
	printf("Get a SIGALRM, %d counts!\n", ++printMes_count);
}



//用旧时信号的方式, 绑定定时器信号, 并激活该定时信号.
//输入: sig = 定时器类型; ptick = 定时器间隔时钟
//	ITIMER_REAL			= SIGALRM 信号
//	ITIMER_VIRTUAL	= SIGVTALRM 信号
//	ITIMER_PROF			= SIGPROF 信号
void old_setup_timer(int timer_type,struct itimerval *ptick){
	struct itimerval tick;
	int sig_type;



	//1.定时间隔赋值
	if(ptick){
		tick.it_value.tv_sec = ptick->it_value.tv_sec;
		tick.it_value.tv_usec = ptick->it_value.tv_sec;
		tick.it_interval.tv_sec = ptick->it_value.tv_sec;
		tick.it_interval.tv_usec = ptick->it_value.tv_sec;
	}
	else{
		//启动延时(也可以设置为马上启动. )
		tick.it_value.tv_sec = 1;//秒
		tick.it_value.tv_usec = 0;//1000 000微秒=1s
		//触发间隔
		tick.it_interval.tv_sec = 1;
		tick.it_interval.tv_usec = 0;
	}

	//2.判断定时器类型, 并选择对应的信号
	switch(timer_type){
	case ITIMER_REAL:
		sig_type = SIGALRM;//(实时定时器)
		break;
	case ITIMER_VIRTUAL:
		sig_type = SIGVTALRM;//VT ALRM VT警报, 执行时递减计时器
		break;
	case ITIMER_PROF:
		sig_type = SIGPROF;//'PROF 故障'计时器(执行时递减)
		break;
	default:
		printf("unknow 'timer_type'!!\n");
		return;
	}

	//3.绑定'信号和响应函数', 并激活信号
	if(signal(sig_type, printMes) == SIG_ERR)
		perror("signal() failed");

	//4.'设置'并'激活'定时器<实时定时器>
	if(setitimer(timer_type, &tick, NULL) == -1)
		perror("setitimer() failed");

	return;
}



//测试'旧时signal() 设置方式'的定时器
//(不安全, 实时定时器, 但是信号传递方式不是实时的)
void old_timer_test(void){
	struct itimerval tick;
	int test_count = 0;



	//启动延时(也可以设置为马上启动. )
	tick.it_value.tv_sec = 1;
	tick.it_value.tv_usec = 0;
	//触发间隔
	tick.it_interval.tv_sec = 1;
	tick.it_interval.tv_usec = 0;

	//1.实时定时器test
	old_setup_timer(ITIMER_REAL,&tick);
	//2.警告计时器test
	old_setup_timer(ITIMER_VIRTUAL,NULL);//默认时间间隔,NULL就是默认
	//3.故障计时器test
	old_setup_timer(ITIMER_PROF,&tick);


	while(1){
		pause();//使调用进程(或线程)睡眠状态, 直到接收到'一个信号'.
						//证明pause() 状态下, 仍然能收到一个信号.
		if(test_count >= 6)
			break;
		else
			test_count++;
	}
	return;
}









//参数信号处理函数(指针)的实现
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

//参数信号处理函数(整形)的实现
void handler_int(int signum,siginfo_t *info,void *myact){
	printf("signum=%d, siginfo_t* info->si_int=%d, void *myact=%d\n",\
	signum, info->si_int, myact);
}



//实时信号定时器测试
void setup_timer_test(void){
	struct sigaction act;//实时信号描述体
	sigset_t sig_set;//阻塞信号集
	struct itimerval tick;//定时器间隔
	int test_count = 0;
	union sigval sig_snd_val;//发送信号时, 需要的信号参数
	//激活的实时信号, 必须与ITIMER_REAL 定时器配对.
	//ITIMER_REAL = SIGALRM
	int real_sig_num = SIGALRM;



	//1.初始化'延时信息'
	//启动延时(也可以设置为马上启动. )
	tick.it_value.tv_sec = 1;
	tick.it_value.tv_usec = 0;
	//触发间隔
	tick.it_interval.tv_sec = 1;
	tick.it_interval.tv_usec = 0;


	//2.初始化'实时信号描述体信息'
	sigemptyset(&sig_set);
	//act.sa_sigaction = handler_point;//参数信号处理函数(指针)
	act.sa_sigaction = handler_int;//参数信号处理函数(整形)
	act.sa_flags = ITIMER_REAL;//实时定时器
	act.sa_mask = sig_set;


	//3.实时定时器test
	setup_timer(real_sig_num,&act,&tick);


	//4.等待定时信号到来(原子操作)
	while(1){
		sigsuspend(&act.sa_mask);//直接挂起等待6 个信号即可

		if(test_count >= 6)
			break;
		else
			test_count++;
	}

	return ;
}



//设置一个实时信号为: 实时定时器信号.
//参数1-sig_num必须是: [32,64] 之间的数字,
//系统一共有64 个信号[1,64], 前[1,31]是异步信号, 不可覆盖!
//实时信号区间: [32,64]
//参数2-preal_sig_info必须指明: 定时器类型, 和信号响应函数
void setup_timer(int sig_num,struct sigaction *preal_sig_info,\
		struct itimerval *ptick){
	struct sigaction real_sig_info;
	struct itimerval tick;



	//1.初始化值
	//参数信号处理函数(整形)
	real_sig_info.sa_sigaction = preal_sig_info->sa_sigaction;
	//指定定时器类型
	real_sig_info.sa_flags = preal_sig_info->sa_flags;
	//实时信号集, 可以passed
	real_sig_info.sa_mask = preal_sig_info->sa_mask;


	//定时间隔赋值
	if(ptick){
		tick.it_value.tv_sec = ptick->it_value.tv_sec;
		tick.it_value.tv_usec = ptick->it_value.tv_sec;
		tick.it_interval.tv_sec = ptick->it_value.tv_sec;
		tick.it_interval.tv_usec = ptick->it_value.tv_sec;
	}
	else{
		//启动延时(也可以设置为马上启动. )
		tick.it_value.tv_sec = 1;//秒
		tick.it_value.tv_usec = 0;//1000 000微秒=1s
		//触发间隔
		tick.it_interval.tv_sec = 1;
		tick.it_interval.tv_usec = 0;
	}


	//2.设置并激活实时信号
	if(sigaction(sig_num, &real_sig_info, NULL) == -1)//激活信号
		perror("sigaction() failed");


	//3.'设置'并'激活'定时器<实时定时器>
	if(setitimer(real_sig_info.sa_flags, &tick, NULL) == -1)
		perror("setitimer() failed");

	return;
}
