//111111111111111111111111111111111111111111111111111111111111111111111111
//格式测试:
//邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵
//起始日期:
//完成日期:
//************************************************************************
//修改日志:

//, , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , ,, , ,


//多进程模型 + 互斥锁 demo 
//[肯定失败!! 堆变量都不一样, fork() 之后就会崩溃, 
// windows 平台可能有这种烂操作, 教坏人]

//编译:
//g++ -o x ./test_fork_mutex_lock.cpp -lpthread

#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>//posix fork() getpid()

#define ipc_fork_pool_max 3


pthread_cond_t cond;//定义事件
pthread_mutex_t mutex;//定义锁


//全局变量
int pid_main = 0;//getpid() 无错误返回
int pid_son_himself = 0;//子进程自身的pid 记录, 父进程为0
int pid_pool[ipc_fork_pool_max];//子进程pid 记录池[只有父进程有效]


//子进程执行函数switch 选择器
void son_proc_switch_box(int switch_box_tmp){
	switch(switch_box_tmp){
		case 0:
			printf("doing case 0\n\n");
			break;
		case 1:
			printf("doing case 1\n\n");
			break;
		case 2:
			printf("doing case 2\n\n");
			break;
		case 3:
			printf("doing case 3\n\n");
			break;
		case 4:
			break;
		default:
			printf("doing default, a mess!\n");
			break;
	}
}



//主函数
int main(int argc, char** argv){
	pid_main = getpid();//getpid() 无错误返回
	memset(pid_pool,0,sizeof(pid_pool));

	//父进程和子进程都--初始化互斥锁和条件变量
	pthread_mutex_init(&mutex,NULL);
	pthread_cond_init(&cond,NULL);
	
	//创建子进程
	int tmp = 0;
	int switch_box_tmp = -1;
	for(;tmp < ipc_fork_pool_max;tmp++){
		if(getpid() != pid_main)
			break;//不是父进程自动退出, 这样防止子进程重复运行下去

		int pid_tmp = fork();//成功之后, 父进程返回子进程的pid, 子进程返回0
		assert(pid_tmp != -1);
		if(pid_tmp == 0){
			//当前新建的子进程
			switch_box_tmp = tmp;
			printf("pid_son_himself before:%d\n",pid_son_himself);
			pid_son_himself = getpid();
			printf("当前新建的子进程, i am %d\n",pid_son_himself);
			sleep(1);
			pthread_mutex_lock(&mutex);
			pthread_cond_signal(&cond);//出发事件, 启动等待
			pthread_mutex_unlock(&mutex);
		}
		else{
			//父进程需要做的事
			pthread_mutex_lock(&mutex);
			pthread_cond_wait(&cond,&mutex);
			pthread_mutex_unlock(&mutex);
			pid_pool[tmp] = pid_tmp;
		}
	}//for end

	//分两步走: 第一步创建好子进程, 并区分, 第二部再执行对应的函数
	if(getpid() == pid_main){
		sleep(1);
		printf("父进程打印所有子进程编号\n");
		for(tmp = 0;tmp < ipc_fork_pool_max;tmp++)
			printf("pid_%d = %d\n", tmp, pid_pool[tmp]);
	}
	else{
		//子进程全部进入switch box !!
		son_proc_switch_box(switch_box_tmp);
	}
	
	//父进程和子进程都--摧毁互斥锁
	pthread_cond_destroy(&cond);//销毁事件
	pthread_mutex_destroy(&mutex);//销毁锁
	return 0;
}

