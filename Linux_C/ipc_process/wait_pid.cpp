//111111111111111111111111111111111111111111111111111111111111111111111111
//格式测试:
//邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵
//起始日期:
//完成日期:
//************************************************************************
//修改日志:

//, , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , ,, , ,


//编译:
//g++ -o x ./wait_pid.cpp -ggdb3


//demo： 父进程wait() waitpid() 获取子进程<结束状态>
//前提是: 子进程没有调用exit(), _exit() 等函数清除<结束状态>
//			 中断仍然可以获取结束状态


//__1.wait() 函数介绍:
//pid_t wait(int *wstatus);
//	wait() 返回子进程pid;
//	传入的参数, 是一个引用int 参数. int 参数表明: 退出状态.
//	ps:
//		这也是为什么int main(); 必须是int 返回类型.
//		因为内核在对'孤儿进程'回收的时候,
//		也需要用wait() 结束'孤儿进程',
//		所以int main() 也是int 返回类型

//注意: 如果没有子进程, 却调用wait() 会失败返回-1



//__2.waitpid() 函数介绍:
//pid_t waitpid(pid_t pid, int *wstatus, int options);
//options = 0, 等同wait(), 阻塞当前进程, 等待waitpid() 结果.

//options = 1 = WNOHANG, 若由pid 指定的子进程还没有结束,
//					则waitpid() 不阻塞而立即返回, 此时返回0.

//options = 2 = WUNTRACED,(未知操作, 很少使用.)
//					为了实现某种操作, 由pid 指定的任一进程已被暂停,
//					且其状态自暂停以来, 还未报告过, 则返回其状态报告.

//注意: waitpid() 不一定是等待子进程, 也可以是其它进程.
//			如果目标<进程/子进程>不存在, 则失败返回-1

//补	: 如果子进程结束, 返回子进程pid.



#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/wait.h>



#define fork_pool_max 4



//全局变量
int pid_main = 0;//getpid() 无错误返回
int pid_son_himself = 0;//子进程自身的pid 记录, 父进程为0
int pid_pool[fork_pool_max];//子进程pid 记录池[只有父进程有效]
bool isInitPool = false;


//1.wait() 测试(有两种情况)
void test_wait(void);
//2.waitpid() 测试
void test_waitpid(int _pid);
//3.子进程执行函数switch 选择器(主要用来显示区分各个子进程)
void son_switch_box(int sw_box_tmp);





int main(void){
	int tmp = 0;
	int sw_box_tmp = -1;//自身的线程池位置



	for(;tmp < fork_pool_max;tmp++)
		pid_pool[tmp] = -1;//初始化池
	tmp = 0;
	pid_main = getpid();
	memset(pid_pool,0,sizeof(pid_pool));



	//1.测试wait() for 一个空进程, 返回报错-1
	test_wait();



	//2.创建子进程
	for(;tmp < fork_pool_max;tmp++){
		pid_pool[tmp] = fork();
		if(pid_pool[tmp] == -1){
			perror("fork() failed");
			break;
		}
		if(pid_pool[tmp] == 0){
			//当前新建的子进程
			sw_box_tmp = tmp;
			pid_son_himself = getpid();
			printf("当前新建的子进程, i am %d\n",pid_son_himself);
		}
		else{
			//父进程需要做的事
			;
		}

		if(getpid() != pid_main)
			break;//不是父进程自动退出, 这样防止子进程重复运行下去
	}//for end
	isInitPool = true;//初始化池okay



	//3.创建子, 父进程准备完毕, 开始测试wait,waitpid
	if(getpid() == pid_main){
		sleep(3);//等待子进程结束
		printf("\n\n\n打印所有子进程pid 编号和退出状态\n");
		//执行测试
		//子进程pid_pool[0][1] 用来测试waitpid()
		test_waitpid(pid_pool[0]);
		test_waitpid(pid_pool[1]);
		//子进程pid_pool[2][3] 用来测试wait()
		test_wait();
		test_wait();//wait() for 真实有效的子进程
	}
	else{
		//子进程全部进入switch box !!
		son_switch_box(sw_box_tmp);
		//*************************
		assert(0);//子进程全部异常中断,以此来证明:子进程中断不影响父进程的运行
		//*************************
	}
	return 0;
}



//1.wait() 测试(有两种情况)
void test_wait(void){
	int son_ret = -6666;
	int tmp = wait(&son_ret);



	//当isInitPool = true, 表示父进程wait() 子进程有效, 0;
	//当isInitPool = false, 表示父进程wait() 子进程无效, -1;
	if(isInitPool)
		assert(tmp != -1);
	else{
		printf("wait() for 一个空进程测试:\n");
		assert(tmp == -1);
	}
	printf("<1>son pid=%d, exit status=%d\n",tmp, son_ret);
	return;
}



//2.waitpid() 测试
void test_waitpid(int _pid){
	int son_ret = -6666;
	int tmp;



	tmp = waitpid(_pid, &son_ret, 1);
	assert(tmp != -1);//断言waitpid() 没有出错,
										//如果出错errno=10 表明指定的进程不存在

	assert(tmp != 0); //断言waitpid() 的目标子进程<已经结束>
										//注意: tmp!=0, 表示waitpid()==0 表示子进程未结束
										//子进程结束, 返回son pid = tmp !!

	printf("<2.1>son pid=%d, exit status=%d\n",_pid, son_ret);
	printf("<2.2>son pid=%d, exit status=%d\n",tmp, son_ret);
	return;
}





//3.子进程执行函数switch 选择器(主要用来显示区分各个子进程)
void son_switch_box(int sw_box_tmp){
	switch(sw_box_tmp){
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
			printf("doing case 4\n\n");
			break;
		default:
			printf("doing default, a mess!\n");
			break;
	}
	return;
}
