//111111111111111111111111111111111111111111111111111111111111111111111111
//格式测试:
//邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵
//起始日期:
//完成日期:
//************************************************************************
//修改日志:

//, , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , ,, , ,



//创建共享内存和信号量, 输入变量值, 修改共享内存中的值

//编译:
//g++ -o x ./demo_sysv_sem.cpp

#include <stdio.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/sem.h>
#include <unistd.h> // for getpagesize

#define SEM_KEY 6666

union semun {
	int setval;
	struct semid_ds *buf;
	unsigned short *array;
};

int shmid;//共享内存id
int semid;//信号量id
float *addr;//共享内存变量指针

//float h,w;

int main(int argc ,char *argv[]){
	//创建一个共享内存, 权限为0666, getpagesize() 获取当前系统页内存大小
	shmid= shmget(ftok(".",1000),getpagesize(),IPC_CREAT | 0666);
	if(shmid==-1){
		perror("shmget error:");
		exit(EXIT_FAILURE);
	}
	printf("shmid=%d\n",shmid);


	//获取共享内存的起始地址, 且为可读可写
	addr=(float*)shmat(shmid,0,0);
	if(-1==*addr){
		perror("shmat error:");
		exit(EXIT_FAILURE);
	}


	//创建2个信号量
	semid = semget(SEM_KEY, 2, IPC_CREAT | 0600);
	if(-1 == semid){
		perror("semget");
		exit(EXIT_FAILURE);
	}
	printf("semid = %d\n", semid);


	//初始化2个信号量
	union semun sem_args;
	unsigned short array[2]={1,1};
	sem_args.array = array;
	//SETALL代表设置信号集中所有的信号量的值.
	//1,代表2个, sem_args是具体初始化的值放在共用体中.
	int ret = semctl(semid, 1, SETALL, sem_args);
	if(-1 == ret){
		perror("semctl");
		exit(EXIT_FAILURE);
	}


	//对资源的使用处理操作
	struct sembuf sem_opt_wait1[1] = {0, -1, SEM_UNDO};
	struct sembuf sem_opt_wakeup1[1] = {0, 1, SEM_UNDO};
	struct sembuf sem_opt_wait2[1] = {1, -1, SEM_UNDO};
	struct sembuf sem_opt_wakeup2[1] = {1, 1, SEM_UNDO};
	while(1){
		//获取进程2的资源, 让进程2等待
		semop(semid, sem_opt_wait2, 1);

		printf(" enter you height(CM) and height(KG):\n");
		scanf("%f%f",addr,addr+1);

		//唤醒进程1, 即释放资源的使用权
		semop(semid, sem_opt_wakeup1, 1);

		//如果输入身高是1或体重是1就退出
		if(*addr==1||*(addr+1)==1 )
			break;
	}

	return 0;
}
