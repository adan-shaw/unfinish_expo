//111111111111111111111111111111111111111111111111111111111111111111111111
//格式测试:
//邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵
//起始日期:
//完成日期:
//************************************************************************
//修改日志:

//, , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , ,, , ,



//创建/获取共享内存, 获取已经创建的信号量, 
//等待输入进程输入(注意: 上一个进程, 输入时是阻塞的)
//打印共享内存中的值
//删除共享内存和信号量

//编译:
//g++ -o x1 ./demo1_sysv_sem.cpp

#include <stdio.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/sem.h>
#include <unistd.h>

#define SEM_KEY 6666

union semun {
	int setval;
	struct semid_ds *buf;
	unsigned short *array;
};

int shmid;//共享内存id
int semid;//信号量id
float *addr;//共享内存变量指针

float h,w;

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


	//取得信号量
	semid = semget(SEM_KEY, 0, IPC_CREAT | 0600);
	if (-1 == semid){
		perror("semget");
		exit(EXIT_FAILURE);
	}
	printf("semid = %d\n", semid);


	//对资源的使用处理操作
	struct sembuf sem_opt_wait1[1] = {0, -1, SEM_UNDO};
	struct sembuf sem_opt_wakeup1[1] = {0, 1, SEM_UNDO};
	struct sembuf sem_opt_wait2[1] = {1, -1, SEM_UNDO};
	struct sembuf sem_opt_wakeup2[1] = {1, 1, SEM_UNDO};
	while(1){
		semop(semid, sem_opt_wait1, 1);//获取进程1的资源好让进程1等待.

		h=*addr;
		w=*(addr+1);
		printf("h=%f\tw=%f\n",h,w);
		if(h==1.0 || w==1.0)
			break;;//如果输入身高是1或体重是1就退出

		int ret=w/(h*h/10000);
		if(ret>=20 &&ret <=25){
			printf("ok!\n");
		}
		else if(ret <20){
			printf("thin!\n");
		}
		else{
			printf("fat!\n");
		}

		semop(semid, sem_opt_wakeup2, 1);//释放进程2的资源, 即唤醒进程2
	}

	//删除信号量
	if(-1==semctl(semid,1,IPC_RMID,0)){
		perror("semctl error:");
		exit(EXIT_FAILURE);
	}

	//释放共享内存, 使其不再有任何指向它的指针
	if(-1==shmdt(addr)){
		perror("shmdt error:");
		exit(EXIT_FAILURE);
	}

	//删除共享内存
	if(shmctl(shmid,IPC_RMID,0)==-1){
		perror("shctl error:");
		exit(EXIT_FAILURE);
	}

	return 0;
}
