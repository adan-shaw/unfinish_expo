//111111111111111111111111111111111111111111111111111111111111111111111111
//格式测试:
//邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵
//起始日期:
//完成日期:
//************************************************************************
//修改日志:

//, , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , ,, , ,


//编译:
//g++ -o x ./posix_sem_pthread.cpp -lpthread -ggdb3


/*
说明:
	由于posix_sem_fork()已经对'有名信号量'(信号灯),作了详细的使用解析,
	这里就不再重复了, 你可以吧这个看成是一个test.
	test for show you how to using sem in pthread.
	sem_init(sem,0,0);的时候, 表明sem 信号灯, 只能由pthread间共享.
	不支持跨进程共享.

	这个可以fork() 一个子进程进行测试.
*/



#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <semaphore.h>
#include <fcntl.h>
#include <pthread.h>



#define TEST_COUNT 9//9999999



/*
	两个线程对'全局变量'进行有序'加减操作'(子加父减)
	结果预测:
		仅当不出现乱序的情况下, 最终G_test_val = 0.
		这说明: 两个线程有序地操作-抢占操作G_test_val变量.
*/
int G_test_val = 0;
int G_count = 0;//只加不减,只是累加sem_wait()&&sem_post()的次数
sem_t sem;



//1.线程函数
void *__pth_func(void* arg);

//2.posix sem 多线程信号灯测试
void posix_sem_pthread_test(void);



int main(void){
	//posix sem 多线程信号灯测试
	posix_sem_pthread_test();

	return 0;
}



//1.子线程函数
void *pth_func_son(void* arg){
	int i = 0;



	for(; i < TEST_COUNT; i++){
		if(sem_wait(&sem) == -1){//简单死等
			perror("sem_wait() failed");
			break;
		}

		G_test_val++;
		G_count++;

		if(sem_post(&sem) == -1){//死等,必然归还使用权
			perror("sem_post() failed");
			break;
		}

		printf("son-%d: G_test_val=%d\n", G_count,G_test_val);
	}

	return NULL;
}



//2.posix sem 多线程信号灯测试
void posix_sem_pthread_test(void){
	int tmp;
	unsigned long int pth_id_son;
	void* son_pth_ret;



	//1.创建无名信号量
	tmp = sem_init(&sem, 0, 1);
	if(tmp == -1){
		perror("sem_init() failed");
		return ;
	}

	//2.创建子线程
	tmp = pthread_create(&pth_id_son,NULL,pth_func_son,NULL);
	if(tmp != 0){
		perror("pthread_create() failed");
		goto posix_sem_pthread_test__failed;
	}

	//3.对无名sem进行'抢夺使用权'操作
	for(; tmp < TEST_COUNT; tmp++){//父进程对<共享内存>中的变量tmp 减1
		if(sem_wait(&sem) == -1){//简单死等
			perror("sem_wait() failed");
			break;
		}

		G_test_val--;
		G_count++;
		sleep(1);//for test, 看是否会阻塞, sem 强占1s 是否有效

		if(sem_post(&sem) == -1){//死等,必然归还使用权
			perror("sem_post() failed");
			break;
		}

		printf("main-pth-%d: G_test_val=%d\n", G_count,G_test_val);
	}

	//4.查看子线程的返回值
	tmp = pthread_join(pth_id_son, &son_pth_ret);
	if(tmp != 0){
		perror("pthread_join() failed");
		goto posix_sem_pthread_test__failed;
	}

posix_sem_pthread_test__failed:
	//5.摧毁无名sem
	tmp = sem_destroy(&sem);
	if(tmp == -1){
		perror("sem_destroy() failed");
		return ;
	}

	//如果同步成功, G_test_val的值应为0
	printf("main pthread finally, G_test_val: %d\n", G_test_val);
	return ;
}


