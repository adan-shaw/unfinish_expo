//111111111111111111111111111111111111111111111111111111111111111111111111
//格式测试:
//邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵
//起始日期:
//完成日期:
//************************************************************************
//修改日志:

//, , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , ,, , ,


//编译: (编译选项: -lpthread)
//g++ -o x ./posix_sem_fork.cpp -ggdb3 -lpthread


//1.posix sem API 总览:
/*

sem_t *sem_open(const char *name, int oflag,\
	mode_t mode, unsigned int value);//打开'有名信号量'
	当打开一个一定存在的'有名信号量'时, ofalg设置为: 0
	如果想创建一个'有名信号量'时, ofalg设置为: O_CREAT

	mode = 权限控制, 一般是0600, 属主独占-读写.
				 这样就只能父子进程之间共享？其它没有继承属主使用权的进程不得使用？

	value = 初始化sem_getvalue()值, 与sem_init()第三参数类似.

	ps: 如果你不需要控制sem 访问权限, 你不需要sem_open(),
			直接sem_init() 也可以直接使用.
			例如: 在pthread之间共享sem, 你可以直接:
						sem_init()+sem_destroy() 即可??
	有sem_open()的, 称之为: 有名信号量;--进程间共享
	无sem_open()的, 称之为: 无名信号量;--线程间共享


int sem_close(sem_t *sem);//关闭'有名信号量'
	一个进程终止时, 内核对其上仍打开的所有'有名信号量'自动执行关闭操作.
	不论进程是自愿还是非自愿终止.
	* 这个是手动关闭当前进程的'有名信号量'使用权.


int sem_unlink(const char *name);//从系统中删除'有名信号量'
	关闭一个'有名信号量'并没有将他从系统中删除.
	POSIX'有名信号量'是随内核持续的,
	即使当前没有进程打开着某个'有名信号量', 他的值仍保持.
	* 这个是直接命令内核删除'有名信号量'.





int sem_init(sem_t *sem,int pshared,unsigned int value);
	sem 是要初始化的'有名信号量'.
	pshared 表示此'有名信号量'是在进程间共享还是线程间共享,
					pshared=0=线程间共享,pshared!=0=多进程共享.
	value 是'有名信号量'的初始值.(一般可以为0 or 1)


int sem_destroy(sem_t *sem);
	sem是要销毁的'有名信号量'.
	只有用sem_init初始化的'有名信号量'才能用sem_destroy销毁.





int sem_post(sem_t *sem);
	post sem'有名信号量', 成功则sem_getvalue() +1;


int sem_getvalue(sem_t *sem, int *sval);
	可以用来测试'有名信号量'的值,
	当sem_getvalue() == 0 时, sem_wait()系列函数会陷入阻塞.
	当sem_getvalue() != 0 时, sem_wait()系列函数会马上成功返回.

	如果你的sem_getvalue() = n > 1,
	那么就是允许同一时间内, 有n 个资源使用者, 但是这样很容易发生混乱.
	一般情况下,n=1.
	除非你的资源, 本身也有繁忙标记, 使用前检查一下繁忙标记, 再决定是否使用.

	有些时候, 比如多路网卡的情况下, 你可能会有2 个以上的网卡,
	也就有2个输出端口, 这时候你的n 可以为2.

	sem_post()一次, sem_getvalue() +1;
	sem_wait()一次, sem_getvalue() -1;

	sem_wait()是死等, 一次sem_wait()必然需要一次sem_post();
	sem_trywait(), sem_timedwait() 都不一定会成功,
	也就是不一定需要sem_post();

	千万不可在sem_wait() 之前：
	(这样就等于自动解锁, 资源任意使用, 有sem和没有sem根本没有区别).
	if(sem_getvalue(sem) == 0){
		sem_post(sem);//sem_post() 不是为sem_wait()服务的.
		sem_wait();		//sem_post() 决定, sem 有多少个使用者!
	}								//所以: sem_getvalue() 的数量, 一般控制在=1 !!





int sem_wait(sem_t *sem);
	等待sem'有名信号量', 成功则sem_getvalue() -1;
	如果'有名信号量'的值大于0,将'有名信号量'的值减1,立即返回.
	如果'有名信号量'的值为0,则线程阻塞.


int sem_trywait(sem_t *sem);
	sem_trywait()与sem_wait()类似, 成功则sem_getvalue() -1;
	只是如果递减不能立即执行,
	调用将返回错误(errno 设置为 EAGAIN)而不是阻塞.


int sem_timedwait(sem_t *sem,const struct timespec *abs_timeout);
	sem_timedwait()与sem_wait()类似, 成功则sem_getvalue() -1;
	只不过abs_timeout指定一个阻塞的时间上限,
	如果调用因不能立即执行递减而要阻塞.
	abs_timeout 参数指向一个指定绝对超时时刻的结构,

*/



//2.posix '有名信号量'简述
/*
	'有名信号量' + 共享内存的性能还算可以吧.
	<共享内存怎么说也是内存,性能不会太差,虽然做成映射,有一定消耗.
		而且多进程机制,会损耗掉cpu cache 机制,但是这是最快的多进程同步方式.>


	附: 由于涉及进程间通信, 也就涉及file fd 属性权限,
			你必须像打开文件操作一样操作<通信key>

*/



#include <stdbool.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <semaphore.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/shm.h>



#define SHM_KEY 0x34//共享内存key
#define SEM_NAME "/sem_test"
#define TEST_COUNT 9//9999999



int G_count = 0;//只加不减



//posix sem 多进程信号灯测试
void posix_sem_fork_test(void);



int main(void){
	//posix sem 多进程信号灯测试
	posix_sem_fork_test();

	return 0;
}


//posix sem 多进程信号灯测试
void posix_sem_fork_test(void){
	int main_pid = getpid();
	int son_pid;
	bool son_working = false;
	int shm_id,*shm_val;
	int tmp,sem_val;
	sem_t *sem;
	struct timespec wait_t;



	//1.创建一块共享内存,分配一个sizeof(int)的大小,用来存一个int变量
	//	(SHM_R|SHM_W = 0600,有点区别,但差不多)
	shm_id = shmget(SHM_KEY, sizeof(int), IPC_CREAT | 0600);
	if(shm_id == -1){
		perror("shmget() failed");
		return ;
	}

	//2.将共享内存映射到进程(即从'共享内存'中分配出变量缓冲区,供进程使用)
	//	[也可以看作: 从系统中取出'共享内存']
	//	fork()后,子进程可以继承映射,
	//	这时父子进程就会对shm_val变量的使用, 形成竞争关系.
	shm_val = (int *)shmat(shm_id, NULL, 0);
	if(*shm_val == -1){
		perror("shmat() failed");
		goto posix_sem_fork_test__failed_shm;
	}





	//3.posix的'有名信号量'是kernel persistent的,
	//	调用sem_unlink()删除以前的信号量
	/*
	if(sem_unlink(SEM_NAME) == -1){
		perror("sem_unlink() failed");
		goto posix_sem_fork_test__failed_shm;
	}
	*/


	//4.创建新的'有名信号量',初值为1,sem_open会创建共享内存,
	//	所以信号量是内核持续的.
	//	第四参数初始化: sem_getvalue() = 0, 是一般做法.
	//	第四参数初始化: 与sem_init() 第三参数, 功能类似？ 应该也有可能.
	sem = sem_open(SEM_NAME, O_CREAT, 0600, 0);
	if(sem == SEM_FAILED){//SEM_FAILED=NULL=0
		perror("sem_open() failed");
		goto posix_sem_fork_test__failed_shm;
	}


	//5.初始化sem 为进程间共享的'有名信号量'
	tmp = sem_init(sem,true,0);
	if(tmp == -1){
		perror("sem_init() failed");
		if(sem_unlink(SEM_NAME) == -1)	//如果初始化sem失败,
			perror("sem_unlink() failed");//不能直接sem_destroy()
		goto posix_sem_fork_test__failed_shm;						//这里直接sem_unlink()就算了
	}


	//6.判断sem() 当前sem_getvalue()是否 == 0,
	//	如果是,sem_getvalue()==0, 则sem_wait()系列函数会陷入阻塞.
	//	如果是,sem_getvalue()>0,则sem_getvalue()减1,
	//	sem_wait()系列函数会马上返回.
	sem_val = -1;
	if(sem_getvalue(sem,&sem_val) == -1){
		perror("fork() failed");
		goto posix_sem_fork_test__failed_all;
	}
	if(sem_val == 0){//sem_getvalue()==0,sem_wait()会阻塞.
		//只向sem_getvalue() 投入1个使用权.
		//'无论哪个进程'使用sem_wait(), 抢到这'唯一一个'使用权,
		//'抢到的进程'调用的sem_wait(), 都会对sem_getvalue()减1,
		//这样'其它进程'的sem_wait() 都必须等待.
		//等到, '使用者进程'结束, 重新sem_post()入一个使用权的时候,
		//'其它进程'才能抢到使用权.
		if(sem_post(sem) == -1)
			perror("sem_post() failed");
	}


	printf("posix_sem_fork_test is all ready !! let's go\n\n");
	//7.sem同步测试,利用sem信号灯,对'共享内存中的变量'进行同步操作
	son_pid = fork();
	if(son_pid == -1){
		perror("fork() failed");
		goto posix_sem_fork_test__failed_all;
	}
	if(son_pid != 0){//父进程对<共享内存>中的变量tmp 减1
		printf("i'm father process-%d\n\n",getpid());
		son_working=true;	//标记son进程已经工作,只有父进程有资格改动这个变量.
											//也只有这里需要改动son_working变量
		for(tmp = 0; tmp < TEST_COUNT; tmp++){

			//7.1.1: sem_wait()同步阻塞,等待sem使用权
			//***********************************
			//P操作,信号量-1,死等sem_wait()抢'sem使用权';
			//死等sem_wait()必须sem_post()返还'sem使用权'.
			if(sem_wait(sem) == -1)
				perror("sem_wait() failed");

			(*shm_val)--;//共享内存int 指针 -1
			G_count++;//测试次数+1

			if(sem_post(sem) == -1)//N操作, 信号量+1(返还'sem使用权')
				perror("sem_post() failed");

			printf("father-G_count=%d,sem_wait().shm_val=%d\n",\
					G_count, *shm_val);
			//***********************************


			//7.1.2: sem_trywait()异步非阻塞,等待sem使用权
			//###################################
			//非阻塞sem_trywait()不一定抢到'sem使用权',
			//也就不一定需要sem_post()返还'sem使用权'.
			if(sem_trywait(sem)==-1){
				perror("sem_trywait() failed");
				if(errno == EAGAIN)
					printf("sem_trywait()[ errno == EAGAIN ].\
							正常,只是忙碌,没有抢到使用权.\n");
				else
					printf("unknow error,errno = %d\n",errno);
			}
			else{
				//异步等待'sem使用权'-成功拿到'sem使用权'.
				(*shm_val)--;
				G_count++;

				//只有当sem_trywait()成功抢到'sem使用权'时,
				//才需要sem_post()返还'sem使用权'
				if(sem_post(sem) == -1)
					perror("sem_post() failed");

				printf("father-G_count=%d,sem_trywait().shm_val=%d\n",\
						G_count, *shm_val);
			}
			//###################################


			//7.1.3: sem_timedwait()’限时阻塞‘;
			//				同步阻塞-限时退出阻塞,等待sem使用权
			wait_t.tv_nsec = 1000;//初始化阻塞时间
			wait_t.tv_sec = 0;
			//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
			//’限时阻塞‘等待sem_timedwait(), 不一定抢到使用权,
			//也就不一定需要sem_post()返还使用权.
			if(sem_timedwait(sem,&wait_t)==-1)
				perror("sem_timedwait() failed");
			else{
				//’限时阻塞‘等待'sem使用权'-成功拿到'sem使用权'.
				(*shm_val)--;
				G_count++;

				if(sem_post(sem) == -1)
					perror("sem_post() failed");

				printf("father-G_count=%d,sem_timedwait().shm_val=%d\n",\
						G_count, *shm_val);
			}
			//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

		}//for() end
	}//if{} end, 父进程结束
	else{//子进程对<共享内存>中的变量tmp 加1
		printf("i'm son process-%d\n\n",getpid());
		//7.2：
		//父进程已经做了'详细的示范',
		//子进程只需简单地sem_wait(),产生sem 使用竞争即可.
		//不需要再重复'详细的示范'.
		for(tmp = 0; tmp < TEST_COUNT*3; tmp++){
			if(sem_wait(sem) == -1)//死等,一定可以拿到使用权
				perror("sem_wait() failed");

			(*shm_val)++;
			G_count++;

			if(sem_post(sem) == -1)//死等归还,一定需要归还
				perror("sem_post() failed");

			printf("son-G_count=%d,sem_wait().shm_val=%d\n",G_count,*shm_val);
		}//for() end

		sleep(3);//制造事端,让子进程总是最迟结束,这样父进程waitpid()不会落空
	}//if{} end, 子进程结束



	//8.释放资源!!
posix_sem_fork_test__failed_all:
	//关闭本进程对sem'有名信号量'的使用权
	if(sem_close(sem) == -1)//无论主进程/son 进程,都需要sem_close()
		perror("sem_close() failed");

	if(getpid()==main_pid && son_working==true)
		if(waitpid(main_pid,&son_pid,0) == -1)//父进程等待子进程退出(死等)
			perror("waitpid() failed");

	//摧毁已经初始化的sem'有名信号量'(等待子进程结束,再摧毁sem)
	if(getpid() == main_pid)//只有主进程能够sem_destroy()
		if(sem_destroy(sem) == -1)
			perror("sem_destroy() failed");


posix_sem_fork_test__failed_shm:
	//本进程, 放弃'共享内存'使用权
	if(shmdt(shm_val) == -1)
		perror("shmdt() failed");

	//释放'共享内存'
	if(getpid() == main_pid)//只有主进程能够shmctl()
		if(shmctl(shm_id, IPC_RMID, 0) == -1)
			perror("shmctl()-IPC_RMID failed");

	return ;
}
