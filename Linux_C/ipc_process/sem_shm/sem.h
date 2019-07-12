//111111111111111111111111111111111111111111111111111111111111111111111111
//格式测试:
//邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵
//起始日期:
//完成日期:
//************************************************************************
//修改日志:

//, , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , ,, , ,


/*
信号量有两种实现: 传统的System V信号量和新的POSIX信号量. 
(你可以不用system v, 已经过时了)

<1.API 对比>
<sys/sem.h>			<semaphore.h>
Systm V					POSIX
semctl()				sem_getvalue()	*
semget()				sem_post()			*
semop()					sem_timedwait()	*
								sem_trywait()		*
								sem_wait()			*

								sem_destroy()
								sem_init()

								sem_close()
								sem_open()
								sem_unlink()

总结:
System V的信号量一般用于进程同步, 且是内核持续的, api为:
semget()
semctl()
semop()



<进程>
Posix的有名信号量一般用于进程同步,有名信号量是内核持续的.有名信号量的api为:
sem_open()
sem_close()
sem_unlink()
+ 5 个* 号api


<线程>
Posix的无名信号量一般用于线程同步,无名信号量是进程持续的.无名信号量的api为:
sem_init()
sem_destroy()
+ 5 个* 号api


*/



