//111111111111111111111111111111111111111111111111111111111111111111111111
//格式测试:
//邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵
//起始日期:
//完成日期:
//************************************************************************
//修改日志:

//, , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , ,, , ,



//进程优先级调度

/*

1.进程优先级--nice 值(非实时调度-倾向调度)
	进程nice 值允许进程间接地影响内核的调度算法,
	每个进程都有一个nice 值, 其范围[-20,+19],
	-20 为最高优先级, +19 为最低优先级, 默认值为0.

	进程的调度并不是严格按照nice 值的层次进行划分的,
	相反, nice 值只是一个权衡因素

	api 也很简单:
	int getpriority(int which, id_t who);//失败返回-1
	int setpriority(int which, id_t who, int prio);//失败返回-1
	which 参数:
	which = PRIO_PROCESS, who = 0, 则作用对象是当前调用进程.
	which = PRIO_PGRP, who = 0, 则作用对象是当前调用进程组.
	which = PRIO_USER, who = 0, 则作用对象是当前调用进程的用户.


2.CPU 亲和力(非实时调度-倾向调度)
	减少进程/线程切换时的消耗. (很少用)
	#include <sched.h>
	sched_setaffinity(pid_t pid,size_t len,cpu_set_t *set);//失败返回-1
	sched_getaffinity(pid_t pid,size_t len,cpu_set_t *set);
	void CPU_ZERO(cpu_set_t *set);
	void CPU_SET(int cpu, cpu_set_t *set);
	void CPU_CLR(int cpu, cpu_set_t *set);

	int CPU_ISSET(int cpu, cpu_set_t *set);//成功返回1, 失败返回0


3.实时调度机制
	一般系统并不需要使用到<实时调度>, 更不需要用到上面两种调度.
	但是也有例外:
	3.1: 当外部输入的互交进程数据显得很重要的情况下,
			 外部一个情况就可以影响整个主进程的工作状态的系统,
			 如交通系统, 抢占性能就显得很重要.
			 操作台抢占系统控制权, 以应对突发事件, 优先分配等需求.
			 这时候就需要做实时优先级抢占了.
	3.2: .
	api:
	int sched_get_priority_min(int policy);//获取系统优先级范围
	int sched_get_priority_max(int policy);

	int sched_setscheduler(pid_t pid, int policy,\
		const struct *sched_param);//修改进程调度策略和优先级
	int sched_getscheduler(pid_t pid);//获取优先级

	策略					描述
	SCHED_FIFO	 实时先进先出
	SCHED_RR		 实时循环
	SCHED_OTHER	标准的循环时间分享
	SCHED_BATCH
	SCHED_IDLE

	//修改进程调度策略, 但不修改优先级
	int sched_setparam(pid_t pid, const struct *sched_param);
	//获取进程调度策略
	int sched_getparam(pid_t pid, const struct *sched_param);
	//挂起进程, 失败返回-1
	int sched_yield(void);
	//获取系统cpu 的时间片长度
	int sched_rr_get_interval(pid_t pid, struct timespec *tp);



