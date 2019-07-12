//111111111111111111111111111111111111111111111111111111111111111111111111
//格式测试:
//邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵
//起始日期:
//完成日期:
//************************************************************************
//修改日志:

//, , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , ,, , ,

//*****************
//glibc 中的休眠函数
//*****************
//头文件：

#include <unistd.h>


//*1. <休眠>
//1.1: 低分辨率休眠(挂起)<单位:秒>
//	sleep()内部实现也是用nanosleep()来实现的,
//	sleep()完全抛弃了旧式系统的方式.
unsigned int sleep(unsigned int second);


//1.2: 高分辨率休眠(挂起)1,000 微秒 = 1毫秒;10^6 微妙 = 1秒
int usleep(useconds_t usec);



//1.3: '广域'休眠
//休眠, 并不是用信号来实现的,
//因为SUSv3 规定： 休眠的本意就是, 拒绝接受系统信号.
//程序休眠的时候, cpu 使用权出让, 程序被挂起.

#include <time.h>
struct timespec {
	time_t tv_sec;	// seconds			秒
	long	 tv_nsec; // nanoseconds	10^9纳秒 = 1秒
};

int nanosleep(const struct timespec *req, struct timespec *rem);

//使用nanosleep应注意判断返回值和错误代码, 否则容易造成cpu占用率100%. 

//调用nanosleep(), 进程会进入TASK_INTERRUPTIBLE状态(脱离就绪队列),
//这种状态会等待'激活/唤醒'信号, 而返回TASK_RUNNING状态的.
//这就意味着:
//	有可能会没有等到你规定的时间就因为其它信号而唤醒, 此时函数返回-1,
//	切还剩余的时间会被记录在rem中.

















//内核开发中的休眠函数, 跟glibc 中的休眠函数不一样!!
/*
************************************************************
在linux 内核驱动开发中, 经常要用到延迟函数：msleep, mdelay／udelay
虽然msleep 和mdelay 都有延迟的作用, 但他们是有区别的．
(上面的函数,不是linux api里面的函数,需要有头文件, 只有sleep-秒 是c 语言函数)

而在服务器开发中, 一般不会用到忙等待, 做完任务就马上返回,
等待也是同步数据时的死等, 很少故意睡眠线程的...
************************************************************

1.)对于模块本身
mdelay是忙等待函数, 在延迟过程中无法运行其他任务．
这个延迟的时间是准确的．是需要等待多少时间就会真正等待多少时间．

msleep是休眠函数, 它不涉及忙等待．你如果是msleep(10),
那实际上延迟的时间, 大部分时候是要多于１０ms的, 是个不定的时间值．


2.)对于系统：
delay函数是忙则等待, 占用CPU时间；
mdelay() 会占用cpu资源, 导致其他功能此时也无法使用cpu资源.

而sleep函数使调用的进程进行休眠,
msleep() 则不会占住cpu资源, 其他模块此时也可以使用cpu资源.


3.)udelay() mdelay() ndelay() 区别：
实现的原理本质上都是忙等待, ndelay和mdelay都是通过udelay衍生出来的.

我们使用这些函数的实现往往会碰到编译器的警告:
implicit declaration of function 'udelay',
这往往是由于头文件的使用不当造成的. 

在include/asm-xxx/delay.h中定义了udelay(),
而在include/Linux/delay.h中定义了mdelay和ndelay.

udelay一般适用于一个比较小的delay, 如果你填的数大于2000,
系统会认为你这个是一个错误的delay函数, 
因此如果需要2ms以上的delay需要使用mdelay函数.


4.)msleep, ssleep区别：休眠单位不同 ms and ss


5.)秒的单位
ms是毫秒=0.001秒
us是微秒=0.000001秒
ns是纳秒=0.000000001秒


*/




