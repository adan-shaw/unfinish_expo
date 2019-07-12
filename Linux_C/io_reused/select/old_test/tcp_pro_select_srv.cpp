//同步-select-单次io <--> tcp 回显服务器

//g++ -o x ./tcp_pro_select_srv.cpp -ggdb3 -lpthread

#include <sys/time.h> // for select
#include <sys/types.h>
#include <unistd.h>

#include <sys/types.h> // for socket
#include <sys/socket.h>
#include <sys/socket.h> // for shutdown

#include <netinet/in.h> // for inet_addr
#include <arpa/inet.h>

#include <stdio.h>
#include <strings.h> // for bzero
#include <string.h> // for strlen
#include <assert.h>
#include <errno.h>
#include <unistd.h> // for fork
#include <fcntl.h> // for fcntl
#include <stdlib.h> // for exit

#include <sys/types.h>//for ftok(), man 3 ftok
#include <sys/ipc.h>

#include <semaphore.h> // for sem

#include <sys/shm.h> // for shm

#include <sys/ipc.h> // ftop

#include <sys/wait.h> // for waitpid

#include <signal.h> // for singal

#include <time.h> // for time

#ifdef _printf
#else
	#define _printf(args...) printf(args)
#endif

#define sfd_max 1023
#define pro_max 1023

#define io_buf_max 2048
#define io_sock_close_timeout 2

#define srv_ip "127.0.0.1"
#define srv_port 6666
#define BACKLOG 64 //监听队列最大长度 64

#define shm_max 4096//1024 int *4
#define posix_sem_name "tcp_pro_select_srv_sem"

//全局变量声明列表:
//int pro_count = 0;//子进程的数量
int err_count = 0;
int test_count = 0;
bool is_working = false;
int ppid = 0;
key_t shm_key = 14422;
int shm_id = 0;
int* p_shm = 0;//子进程的数量
sem_t *sem = NULL;

unsigned int sfd_li = 0;   //监听sfd
struct timeval timeout = {0,50000};//timeout 变量
fd_set so_set;


//前置函数声明列表
bool fork_mission(void);//fork 任务函数
bool do_mission(int sfd_acc);//执行一次任务


//信号函数--回收资源
void stop_sig(int sig){
  printf("program suspend by signal: %d\n", sig);
	
	if(!sem){
		int sem_tmp = -1;
		sem_tmp = sem_wait(sem);
		assert(sem_tmp != -1);
		if(p_shm != 0){
			if(shmdt(p_shm) == -1)//释放共享内存块(只是删除某个块)
				printf("shmdt() fail!! errno = %d\n", errno);
		}
		if(shm_id != 0){
			if(shmctl(shm_id, IPC_RMID, 0) == -1)//整个共享内存释放
				printf("shmctl fail !! errno = %d\n", errno);
		}
		sem_tmp = sem_post(sem);
		assert(sem_tmp != -1);
		sem_close(sem);//父进程关闭信号量fd 句柄
		sem_unlink(posix_sem_name);//从内核中将信号量除名
	}
	printf("cleanup finish\n");
	exit(0);
}


//屏蔽31 个信号(9 和19 除外)
void ignore_all_signal(void){
  int i = 1;
	/*
  for(;i <= 31;i++){//屏蔽31 个信号
    if(i == 9)
      continue;
    if(i == 19)
      continue;
    //printf("%d\n",i);
    if(signal(i, SIG_IGN) == SIG_ERR)
			_printf("signal() fail, errno = %d\n", errno);
  }
	//恢复Ctrl + C--> stop_sig()
   if(signal(SIGINT, SIG_DFL) == SIG_ERR)
			_printf("signal() fail, errno = %d\n", errno);
	*/
  if(signal(SIGINT, stop_sig) == SIG_ERR)
		_printf("signal() fail, errno = %d\n", errno);
}

//主函数
int main(void){
	//主进程初始化
  _printf("\n program start\n\n");
	ppid = getpid();
	//初始化进程信号
	ignore_all_signal();
	
	//posix的有名信号量是kernel persistent的, 调用sem_unlink删除以前的信号量 
  if(sem_unlink(posix_sem_name) == -1){
    printf("sem_unlink() fail !!, errno = %d\n", errno);
		//return -1;//这里只是防止有重名sem, 所以错误返回是正常的.
	}

  //创建新的有名信号量, 初值为1, sem_open会创建共享内存, 所以信号量是内核持续的
  sem = sem_open(posix_sem_name, O_CREAT, 0600, 1);
  if(sem == SEM_FAILED){
    printf("sem_open() fail !!, errno = %d\n", errno);
    return -1;
  }

	//shm_key = ftok("handsome_adan2", 0x39);

	//创建共享内存--存在则打开, 不存在则创建
	shm_key = time(NULL);//这样可能会有内存泄漏!!
  shm_id = shmget(shm_key,shm_max,IPC_CREAT|IPC_EXCL|SHM_R|SHM_W);
  if(shm_id == -1){
    printf("shmget() fail !! errno = %d\n", errno);
    return -1;
  }

	//将共享内存映射到进程
  p_shm = (int *)shmat(shm_id, NULL, 0);
  if(p_shm == (int *)-1){
    printf("shmat() fail !!, errno = %d\n", errno);
    return -1;
  }
	*(p_shm+0) = 0;
	//*(p_shm+0) = 8999;
	//*(p_shm+1) = 18999;
	//*(p_shm+2) = 28999;
	//*(p_shm+3) = 38999;
	//*(p_shm+4) = 48999;
	//*(p_shm+5) = 58999;
	//*(p_shm+6) = 68999;
	//*(p_shm+7) = 78999;
	//printf("%d,%d,%d,%d\n",p_shm[0],p_shm[1],*(p_shm+2),p_shm[3]);
	//printf("%d,%d,%d,%d\n",p_shm[4],p_shm[5],*(p_shm+6),*(p_shm+7));

  //创建监听socket
  sfd_li = socket(AF_INET, SOCK_STREAM, 0);
	if(sfd_li == -1){
		_printf("socket() fail, errno = %d\n", errno);
		return -1;
	}

  //*设置监听socket 选项*
  //设置地址重用
  int opt_val = true;
  opt_val = setsockopt(sfd_li, SOL_SOCKET, SO_REUSEADDR, \
							&opt_val, sizeof(int));
	if(opt_val == -1){
		_printf("set_sockopt_reuseaddr() fail, errno = %d\n", errno);
		close(sfd_li);
		return -1;
	}

  //执行bind
	struct sockaddr_in addr;
	bzero(&addr, sizeof(struct sockaddr_in));
	addr.sin_addr.s_addr = inet_addr(srv_ip);
	addr.sin_family = AF_INET;
	addr.sin_port = htons(srv_port);

  opt_val = bind(sfd_li, (struct sockaddr*)&addr, \
							sizeof(struct sockaddr));
  if(opt_val == -1){
    _printf("socket %d bind fail, errno: %d\n", sfd_li, errno);
    close(sfd_li);
    return -1;
  }

  //执行listen
  opt_val = listen(sfd_li, BACKLOG);
  if(opt_val == -1){
    _printf("socket %d listen fail, errno: %d\n", sfd_li, errno);
    close(sfd_li);
    return -1;
  }
	is_working = true;

  //工作循环
  while(is_working){
    //查看监听sfd_li 有没有读事件
		FD_ZERO(&so_set);
    FD_SET(sfd_li, &so_set);  
		opt_val = select(sfd_li + 1, &so_set, NULL, NULL, &timeout);
		if(opt_val == -1){//select 错误, 终止程序
      _printf("select() fail, errno: %d\n", errno);
			is_working = false;
			break;
		}
		if(opt_val == 0){//暂时没有数据操作, pass
			;
		}
		if(opt_val > 0){//select 有opt_val 个sfd 响应<读事件>
			//_printf("select return %d sfd has read event\n", opt_val);
			opt_val = FD_ISSET(sfd_li, &so_set);
      if(opt_val == 0){//未知错误
				_printf("triggering select is not listen sfd %d, \
					unknow error !! \n", sfd_li);
				is_working = false;
				break;
			}
			else{//有新客户端链接到服务器
				bool btmp = fork_mission();
				if(!btmp && getpid() == ppid){
					_printf("fork_mission() fail, accept client fail!!\n");
					_printf("fork_mission() err_count = %d\n",err_count);
					err_count++;
					break;//for test
					//continue;
				}
				if(getpid() != ppid){
					if(!btmp)
						_printf("son pid %d fork_mission() fail\n",getpid());
					int sem_tmp = -1;
					sem_tmp = sem_wait(sem);
					assert(sem_tmp != -1);
      		*(p_shm+0) = *(p_shm+0) - 1;
					sem_tmp = sem_post(sem);
      		assert(sem_tmp != -1);
					sem_close(sem);//子进程关闭自己的信号量
					//_printf("pid %d quit,pro_count = %d\n",getpid(),p_shm[0]);
					exit(0);//子进程强制退出, 但等待io 完成了才退出(可能有僵尸进程)
					//_exit(0);//直接清空io 并强制退出
				}
			}
		}


    //查看监听sfd_li 有没有网络错误
		FD_ZERO(&so_set);
    FD_SET(sfd_li, &so_set);
    opt_val = select(sfd_li + 1, NULL, NULL, &so_set, &timeout);
		if(opt_val == -1){//select 错误, 终止程序
      _printf("select() fail, errno: %d\n", errno);
			is_working = false;
			break;
		}
		if(opt_val == 0){//暂时没有数据操作, pass
			;
		}
		if(opt_val > 0){//select 有opt_val 个sfd 响应<错误事件>
      //_printf("select return %d sfd has error event\n", opt_val);
			opt_val = FD_ISSET(sfd_li, &so_set);
      if(opt_val == 0){//未知错误
				_printf("triggering select is not listen sfd %d, \
					unknow error !! \n", sfd_li);
				is_working = false;
				break;
			}
			else{
				_printf("listen sfd %d just occurred An error, \
					program going to quit\n", sfd_li);
				is_working = false;
				break;
			}
		}

  }//while(is_working) end


  //清空资源
	if(getpid() == ppid){
  	shutdown(sfd_li,2);
		close(sfd_li);
		
		//等待所有子进程退出(等下做)
				
		
		int sem_tmp = -1;
		sem_tmp = sem_wait(sem);
		assert(sem_tmp != -1);
		if(shmdt(p_shm) == -1)//释放共享内存块(只是删除某个块)
	  	printf("shmdt() fail!! errno = %d\n", errno);
  	if(shmctl(shm_id, IPC_RMID, 0) == -1)//整个共享内存释放
	  	printf("shmctl fail !! errno = %d\n", errno);
		sem_tmp = sem_post(sem);
		assert(sem_tmp != -1);
		sem_close(sem);//父进程关闭信号量fd 句柄
    sem_unlink(posix_sem_name);//从内核中将信号量除名
  	_printf("\nprogram going to quit\nbye!!\n\n");
	}
  return 0;
}

//fork 任务函数
bool fork_mission(void){
  struct sockaddr addr;
	socklen_t addr_len = sizeof(struct sockaddr);
	bzero(&addr, sizeof(struct sockaddr_in));
	//接受客户端
	int sfd_acc = accept(sfd_li, &addr, &addr_len);
	if(sfd_acc == -1){
		_printf("accept() fail, errno: %d\n", errno);
		return false;
	}
	else{//fork 子进程, 去执行任务
			_printf("check pro_count = %d\n",p_shm[0]);//for test
		if(p_shm[0] >= pro_max - 1){//进程池已经满了
			_printf("process pool has full, fork_mission fail\n");
			shutdown(sfd_acc,2);
			close(sfd_acc);
			//return false;
			return true;//进程池已经满, 拒绝客户端请求.
		}
		//设置close-on-exec标志
		//fcntl(sfd_acc, FD_CLOEXEC, true);

		int tmp = fork();
    if(tmp == -1){
			_printf("fork() fail, errno = %d\n", errno);
			shutdown(sfd_acc,2);
			close(sfd_acc);
			//return false;
			return true;//出现僵尸进程, 就等待, 但不退出
		}
		if(tmp == 0){
    	//当前新建的子进程
			//_printf("i am son process, pid = %d\n", getpid());//for test
			tmp = fork();
			if(tmp != 0 && tmp != -1)
				exit(0);//子进程马上结束
			//孙进程继续执行任务			
			tmp = do_mission(sfd_acc);
			if(!tmp)
				_printf("son process %d do_mission() fail !!\n", getpid());
			shutdown(sfd_acc,2);
			close(sfd_acc);
			//sleep(1);//for test
			if(!tmp)
				return false;
			else
				return true; 
			//exit(0);
    }
    else{
    	//父进程需要做的事
			close(sfd_acc);//自己关闭sfd_acc
			
			if(waitpid(tmp, NULL, 0) == -1){//等待子进程结束
				_printf("waitpid()4 [%d] fail!, errno = %d\n",tmp,errno);
				return false;
			}
			
			int sem_tmp = -1;
			sem_tmp = sem_wait(sem);
			assert(sem_tmp != -1);
  		*(p_shm+0) = *(p_shm+0) + 1;
			sem_tmp = sem_post(sem);
  		assert(sem_tmp != -1);
			test_count++;

			return true;
    }
	}

  _printf("unknow error in fork_mission()\n");
  return false;//未知错误
}


//执行一次任务
bool do_mission(int sfd_acc){
	//设置接收缓冲区
	int opt_val = io_buf_max;
	opt_val = setsockopt(sfd_acc, SOL_SOCKET, SO_RCVBUF, \
							&opt_val, sizeof(int));
	if(opt_val == -1){
		_printf("set_sockopt_revbuf() fail, errno = %d\n", errno);
		return false;
	}
	//设置发送缓冲区
	opt_val = io_buf_max;
  opt_val = setsockopt(sfd_acc, SOL_SOCKET, SO_SNDBUF, \
							&opt_val, sizeof(int));
	if(opt_val == -1){
		_printf("set_sockopt_sndbuf() fail, errno = %d\n", errno);
		return false;
	}

	//设置关闭时如果有数据未发送完, 等待n 秒再关闭socket
	struct linger plinger;
	plinger.l_linger = io_sock_close_timeout;
	plinger.l_onoff = true;
	opt_val = setsockopt(sfd_acc, SOL_SOCKET, SO_LINGER, \
							&plinger, sizeof(struct linger));
	if(opt_val == -1){
		_printf("set_sockopt_linger() fail, errno = %d\n", errno);
		return false;
	}

	//设置地址重用
	opt_val = true;
	opt_val = setsockopt(sfd_acc, SOL_SOCKET, SO_REUSEADDR, \
							&opt_val, sizeof(int));
	if(opt_val == -1){
		_printf("set_sockopt_reuseaddr() fail, errno = %d\n", errno);
		return false;
	}

	char xbuf[io_buf_max];
	bzero(&xbuf, io_buf_max);
	//接收一次客户端数据
	opt_val = recv(sfd_acc, &xbuf, io_buf_max, 0);
	if(opt_val == 0){//对端已经关闭
		_printf("client has close when srv recving data\n");
		return true;
	}
	if(opt_val == -1){//recv 错误
		_printf("recv() fail, errno = %d\n", errno);
		return false;
	}
	if(opt_val > 0){//收到数据
		;
	}

  //显示一次客户端数据
	_printf("recv data from client:%s\n", xbuf);

	//组装回送数据
	bzero(&xbuf, io_buf_max);
	sprintf(xbuf, "hello client, you are the %d\n", test_count);

  //回送一次客户端数据
	opt_val = send(sfd_acc, &xbuf, strlen(xbuf), 0);
	if(opt_val == 0){//对端已经关闭
		_printf("client has close when srv sending data\n");
		return true;
	}
	if(opt_val == -1){//recv 错误
		_printf("send() fail, errno = %d\n", errno);
		return false;
	}
	if(opt_val > 0){//回送数据成功
		;
	}

	_printf("return %d finish\n", test_count);
	return true;
}
