//111111111111111111111111111111111111111111111111111111111111111111111111
//格式测试:
//邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵
//起始日期:
//完成日期:
//************************************************************************
//修改日志:

//, , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , ,, , ,



//用ipcs -s					来查看是否创建成功
//用ipcrm -s semid号	来删除指定的信号量

//编译:
//g++ -o x ./ipc_sem_sysv_old.cpp -lpthread -ggdb3

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/sem.h>
#include <errno.h>

/*
int semctl(int _semid ,int _semnum, int _cmd	……);
功能：控制信号量的信息. 
返回值：成功返回0, 失败返回-1；
参数：
_semid	 信号量的标志码(ID), 也就是semget（）函数的返回值;
_semnum	操作信号在信号集中的编号. 从0开始.
_cmd		 命令, 表示要进行的操作.


参数cmd中可以使用的命令如下：
IPC_STAT 读取一个信号量集的数据结构semid_ds, 并将其存储在semun中的buf参数中. 
IPC_SET	设置信号量集的数据结构semid_ds中的元素ipc_perm,
				 其值取自semun中的buf参数.
IPC_RMID 将信号量集从内存中删除. 
GETALL	 用于读取信号量集中的所有信号量的值.
GETNCNT	返回正在等待资源的进程数目.
GETPID	 返回最后一个执行semop操作的进程的PID.
GETVAL	 返回信号量集中的一个单个的信号量的值.
GETZCNT	返回这在等待完全空闲的资源的进程数目.
SETALL	 设置信号量集中的所有的信号量的值.
SETVAL	 设置信号量集中的一个单独的信号量的值.

第4个参数Semunion 是可选的：
semunion :是union semun的实例. 
*/



































//shit demo!!



//对信号量做减1操作, 即等待P(sv) = 占用
bool semaphore_p(int sem_id){
	struct sembuf sem_b;
	sem_b.sem_num = 0;
	sem_b.sem_op = -1;//P()
	sem_b.sem_flg = SEM_UNDO;
	if(semop(sem_id, &sem_b, 1) == -1){
		printf("semaphore_p() >> semop() fail! errno = %d\n", errno);
		return false;
	}
	else
		return true;
}


//这是一个释放操作, 它使信号量变为可用, 即发送信号V（sv）
bool semaphore_v(int sem_id){
	struct sembuf sem_b;
	sem_b.sem_num = 0;
	sem_b.sem_op = 1;//V()
	sem_b.sem_flg = SEM_UNDO;
	if(semop(sem_id, &sem_b, 1) == -1){
		printf("semaphore_v() >> semop() fail! errno = %d\n", errno);
		return false;
	}
	else
		return true;
}


//测试函数(测试方法：)
//第一个进程: 
//解封//sleep(rand() % 100 + 50);,重新编译运行... 
//创建信号量, 占用信号量50s

//第二个进程: 
//注释//sleep(rand() % 100 + 50);,重新编译运行... 你会看到:
//IPC_EXCL = 1024, errno = 0, sem_id = 1605634 (join 没有错)
//semaphore_p() >> semop() fail! errno = 13 
//(操作信号量错了, 程序错误返回...
//这是没有root 权限的情况, 信号量操作fd 可能需要权限)
//正常情况下, 第二个进程是假死的

union semun{//<自定义>system v 信号量semctl 第四参数结构体
	int val;
	struct semid_ds *buf;
	unsigned short *arry;
};

int sem_id = -1;
key_t sysv_sem_key = 1244444;
union semun sem_union;

int main(void){
	//尝试加入一个已经存在的信号量
	/*
	sem_id = semget((key_t)sysv_sem_key, 1, IPC_EXCL);
	if(sem_id == -1 && errno != 2){
		printf("semget() fail! errno = %d\n", errno);
	}
	else{
		printf("sysv sem is exist already\n");
		printf("IPC_EXCL = %d, errno = %d, sem_id = %d\n", \
			IPC_EXCL, errno, sem_id);
	}
	*/
	//创建新的信号量, 权限为0666(可能需要root 权限)
	sem_id = semget((key_t)sysv_sem_key, 1, IPC_CREAT|0666);
	if(sem_id == -1){
		printf("semget() fail! errno = %d\n", errno);
		return -1;
	}

	//初始化信号量--出错返回22
	//SETVAL 初始化
	//IPC_RMID 删除
	sem_union.val = 1;
	int tmp = semctl(sem_id, 0, SETVAL, &sem_union);
	if(tmp < 0){
		printf("semctl()4 init fail! errno = %d\n", errno);
		sem_union.val = 0;
		sem_union.arry = NULL;
		sem_union.buf = NULL;
		tmp = semctl(sem_id, 0, IPC_RMID, &sem_union);
		if(tmp == -1)
			printf("semctl()4 del fail! errno = %d\n", errno);
		return -1;
	}

	int i;
	for(i = 0; i < 10; ++i){
		if(!semaphore_p(sem_id)){//进入临界区
			printf("semaphore_p() fail !!\n");
			break;
		}

		printf("%s", "kidding me ?\n");//向屏幕中输出数据
		sleep(3);

		if(!semaphore_v(sem_id)){//离开临界区, 休眠随机时间后继续循环
			printf("semaphore_v() fail !!\n");
			break;
		}
	}

	printf("\npid = %d - finished\n", getpid());
	//退出前删除信号量
	sem_union.val = 0;
	sem_union.arry = NULL;
	sem_union.buf = NULL;
	tmp = semctl(sem_id, 0, IPC_RMID, &sem_union);
	if(tmp == -1){
		printf("semctl()4 del fail! errno = %d\n", errno);
		return -1;
	}

	return 0;
}

