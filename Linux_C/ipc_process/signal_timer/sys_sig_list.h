//111111111111111111111111111111111111111111111111111111111111111111111111
//格式测试:
//邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵
//起始日期:
//完成日期:
//************************************************************************
//修改日志:

//, , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , ,, , ,



//1.查看系统信号列表: kill -l 
/*
编号为1~31的信号为传统UNIX支持的信号, 是不可靠信号(非实时的),
	编号为32~63的信号是后来扩充的, 称做可靠信号(实时信号). 
	
	信号可靠与否, 在于信号是否有等待队列?? 
	这种说法是错的!!linux 编程手册P343 页有记录: 
	给同一个进程发送100W 次同一个信号, 进程也只会收到一次6同一类型的信号.
	准确的说法是: 
	进程只会有一个等待信号, 一个正在执行的信号, 
	如果正在处理信号的时候, 再进来一个信号就等待, 进来第二个就passed 丢失
	可靠不可靠, 见仁见智, 肯定会覆盖的.
*/





//2.linux 系统信号简析:
/*
系统信号产生后<默认的动作>主要有:
	*忽略信号: ignore (内核将信号丢弃, 不将信号发送给进程)
	*杀死进程: term
	*产生核心转存: core
	*停止进程: stop (也叫暂停进程)
	*恢复进程: cont

2.1 <默认行为列表>:
num  信号名		行为	 描述
 0)  无			无		空信号(可用来检索进程是否存在)
 1) SIGHUP：	term	 挂起
 2) SIGINT：	term	 终端中断
 3) SIGQUIT：	core	 终端退出
 4) SIGILL：	core	 非法指令
 5) SIGTRAP：	core	 跟踪/<断点陷阱>
 6) SIGABRT：	core	 中止进程
 7) SIGBUS：	core	 内存访问错误
 8) SIGFPE：	core	 算术异常
 9* SIGKILL：	term	 必杀信号
10) SIGUSR1：	term	 用户自定义信号1
11) SIGSEGV：	core	 无效内存引用(指针溢出)
12) SIGUSR2：	term	 用户自定义信号2
13) SIGPIPE：	term	 管道断开
14) SIGALRM：	term	 实时[定时器]过期
15) SIGTERM：	term	 终止进程
16) SIGSTKFLT:  term	 协处理器栈错误<协处理器很少有, passed>
17) SIGCHLD：	ignore	终止/停止<子进程>
18) SIGCONT：	cont	 恢复进程运行
19* SIGSTOP：	stop	 确保停止
20) SIGTSTP：	stop	 终端停止
21) SIGTTIN：	stop	 BG 从终端读取(BG = 后台进程组)
22) SIGTTOU：	stop	 BG 向终端写
23) SIGURG：	ignore	套接字上的紧急数据
24) SIGXCPU：	core	 突破对CPU 时间的限制
25) SIGXFSZ：	core	 突破对文件大小的限制
26) SIGVTALRM： term	 虚拟[定时器]过期
27) SIGPROF：	term	 性能分析[定时器]过期
28) SIGWINCH：  ignore	终端窗口尺寸发生变化
29) SIGIO：	 term	 IO 时可能产生的中断信号
30) SIGPWR：	term	 电量行将耗尽
31) SIGSYS：	core	 非法的系统调用


2.2 <信号规则约定如下>:
程序不可捕获、阻塞或忽略的信号有：
	SIGKILL,SIGSTOP. 
不能恢复至默认动作的信号有：
	SIGILL,SIGTRAP
默认会导致进程流产的信号有：
	SIGABRT,SIGBUS,SIGFPE,SIGILL,SIGIOT,SIGQUIT,
	SIGSEGV,SIGTRAP,SIGXCPU,SIGXFSZ
默认会导致进程退出的信号有:
	SIGALRM,SIGHUP,SIGINT,SIGKILL,SIGPIPE,
	SIGPOLL,SIGPROF,SIGSYS,SIGTERM,SIGUSR1,SIGUSR2,SIGVTALRM
默认会导致进程停止的信号有：
	SIGSTOP,SIGTSTP,SIGTTIN,SIGTTOU
默认进程忽略的信号有：
	SIGCHLD,SIGPWR,SIGURG,SIGWINCH
*/





//3.各个系统信号详解(所有系统信号都是SIG_ 打头的)
/*
1)SIGHUP：
	本信号在用户终端连接(<正常/非正常>结束)时发出, 
	当用户退出Linux登录时, 前台&后台进程将会收到SIGHUP信号, 
	这个信号的默认操作为终止前台&后台进程. 
	这个信号篡改和忽略, 比如wget能捕获SIGHUP信号, 并忽略它, 
	这样就算退出了Linux登录, wget也能继续下载. 
	此外, 对于与终端脱离关系的守护进程, 
	这个信号用于通知它重新读取配置文件. 

2) SIGINT： 
	程序终止(interrupt)信号, 
	在用户键入INTR字符(通常是Ctrl-C)时发出, 
	用于通知前台进程组终止进程. 

3) SIGQUIT：
	和SIGINT类似, 但由QUIT字符(通常是Ctrl-/)来控制. 
	进程在因收到SIGQUIT退出时会产生core文件,
	在这个意义上类似于一个程序错误信号. 

4) SIGILL： 
	执行了非法指令. 通常是因为可执行文件本身出现错误, 
	或者试图执行数据段. 堆栈溢出时也有可能产生这个信号. 

5) SIGTRAP：
	由断点指令或其它trap指令产生. 由debugger使用. 

6) SIGABRT：
	调用abort函数生成的信号. 

7) SIGBUS：
	非法地址, 包括内存地址对齐(alignment)出错. 
	比如访问一个四个字长的整数, 但其地址不是4的倍数. 
	它与SIGSEGV的区别在于:
	后者是由于对<合法存储地址>的非法访问触发的
	(如访问不属于自己存储空间或只读存储空间). 

8) SIGFPE：
	在发生致命的算术运算错误时发出. 
	不仅包括浮点运算错误, 还包括溢出及除数为0等其它所有的算术的错误. 

9) SIGKILL：
	用来立即结束程序的运行. <本信号不能被阻塞、处理和忽略>. 
	如果管理员发现某个进程终止不了, 可尝试发送这个信号. 
	@@@不能修改的信号@@@

10) SIGUSR1：
	留给用户使用

11) SIGSEGV：
	试图访问未分配给自己的内存, 
	或试图往没有写权限的内存地址写数据.(指针越界)

12) SIGUSR2：
	留给用户使用

13) SIGPIPE：
	管道破裂. 这个信号通常在进程间通信产生. 
	比如采用FIFO(管道)通信的两个进程, 
	读管道没打开或者意外终止就往管道写, 写进程会收到SIGPIPE信号. 
	此外用Socket通信的两个进程, 
	写进程在写Socket的时候, 读进程已经终止. 

14) SIGALRM：
	时钟定时信号, 计算的是实际的时间或时钟时间. alarm函数使用该信号.

15) SIGTERM：
	程序结束(terminate)信号, 
	与SIGKILL不同的是该信号可以被阻塞和处理. 
	通常用来要求程序自己正常退出, shell命令kill缺省产生这个信号. 
	如果进程终止不了, 我们才会尝试SIGKILL. 

17) SIGCHLD：
	子进程结束时, 父进程会收到这个信号. 
	如果父进程没有处理这个信号, 也没有等待(wait)子进程, 
	子进程虽然终止, 但是还会在内核进程表中占有表项, 
	这时的子进程称为僵尸进程. 
	这种情况我们应该避免
	(父进程或者忽略SIGCHILD信号, 或者捕捉它, 
	或者wait它派生的子进程, 或者父进程先终止, 
	这时子进程的终止自动由init进程来接管). 

18) SIGCONT：
	让一个停止(stopped)的进程继续执行. 本信号不能被阻塞. 
	可以用一个handler 来让程序在由stopped状态变为继续执行时,
	完成特定的工作. 例如: 重新显示提示符

19) SIGSTOP：
	停止(stopped)进程的执行. 
	注意它和terminate以及interrupt的区别:
	该进程还未结束,只是暂停执行. 
	<本信号不能被阻塞, 处理或忽略>. @@@不能修改的信号@@@

20) SIGTSTP：
	停止进程的运行, 但该信号可以被处理和忽略. 
	用户键入SUSP字符时(通常是Ctrl-Z)发出这个信号

21) SIGTTIN：
	当后台作业要从用户终端读数据时, 
	该作业中的所有进程会收到SIGTTIN信号. 缺省时这些进程会停止执行.

22) SIGTTOU：
	类似于SIGTTIN, 但在写终端(或修改终端模式)时收到.

23) SIGURG：
	有"紧急"数据或out-of-band数据到达socket时产生.

24) SIGXCPU：
	超过CPU时间资源限制. 
	这个限制可以由getrlimit/setrlimit来读取/改变. 

25) SIGXFSZ：
	当进程企图扩大文件以至于超过文件大小资源限制. 

26) SIGVTALRM：
	虚拟时钟信号. 类似于SIGALRM, 但是计算的是该进程占用的CPU时间.

27) SIGPROF：
	类似于SIGALRM/SIGVTALRM, 
	但包括该进程用的CPU时间以及系统调用的时间.

28) SIGWINCH：
	窗口大小改变时发出.

29) SIGIO：
	文件描述符准备就绪, 可以开始进行输入/输出操作.

30) SIGPWR：
	Powerfailure

31) SIGSYS：
	非法的系统调用. 

*/



