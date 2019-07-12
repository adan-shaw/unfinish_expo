//同步-select-单次io <--> tcp 回显服务器

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

//#include <sys/types.h>
#include <sys/wait.h> // for waitpid

#include <deque>

#ifdef _printf
#else
	#define _printf(args...) printf(args)
#endif

#define sfd_max 1024
#define pro_max 1024

#define io_buf_max 2048
#define io_sock_close_timeout 2

#define srv_ip "127.0.0.1"
#define srv_port 6666
#define BACKLOG 64 //监听队列最大长度 64

//全局变量声明列表:
std::deque<int> pid_pool;
int err_count = 0;
int test_count = 0;
bool is_working = false;
int ppid = 0;

unsigned int sfd_li = 0;	 //监听sfd
struct timeval timeout = {0,50000};//timeout 变量
fd_set so_set;


//前置函数声明列表
bool fork_mission(void);//fork 任务函数
bool do_mission(int sfd_acc);//执行一次任务


//主函数
int main(void){
	_printf("\n program start\n\n");
	ppid = getpid();

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
					_printf("fork_mission() fail, \
						err_count = %d\n", err_count);
					err_count++;
					break;//for test
					//continue;
				}
				//if(fork_mission() && getpid() == ppid)
					//continue;
				if(getpid() != ppid){
					if(!btmp)
						_printf("son pid %d fork_mission() fail\n",getpid());
					break;//子进程退出
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

		
		//检查线程池哪个子进程已经结束
		int dqsize = pid_pool.size();
		int tmp = 0;
		int son_ret_stat = -1;
		bool cleanup_before = false;
		if(dqsize > 0){
			for(tmp = 0;tmp < dqsize;tmp++){
				opt_val = waitpid(pid_pool[tmp], &son_ret_stat, WNOHANG);
				//opt_val = waitpid(-1, &son_ret_stat, WNOHANG);
				//if(opt_val == -1 && errno != 10){
				if(opt_val == -1){//waitpid 失败(errno 10-NoChildProcesses)
					_printf("waitpid() fail,pid_pool[tmp]=%d,errno=%d!!\n",\
						pid_pool[tmp], errno);
					is_working = false;
					break;
				}
				if(is_working == true)//主进程要退出?
					break;
				if(opt_val == 0){//子进程还没结束
					_printf("son process %d has not end\n",pid_pool[tmp]);
					continue;
				}
				_printf("son process %d had quit, return value = %d\n",\
					opt_val, son_ret_stat);
				pid_pool[tmp] = -1;//还原线程池
				cleanup_before = true;
			}

			for(tmp = dqsize - 1;tmp >= 0;tmp--){
				if(pid_pool[tmp] == -1)//统一删除刚才标记已经清除的子进程
					pid_pool.erase(pid_pool.begin()+tmp);//只能用迭代器工作
			}
	
			if(cleanup_before)
				_printf("son process count now is: %d\n", pid_pool.size());
		}

	}//while(is_working) end


	//清空资源
	if(getpid() == ppid){
		shutdown(sfd_li,2);
		close(sfd_li);
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
		if(pid_pool.size() >= pro_max - 1){//进程池已经满了
			_printf("process pool has full, fork_mission fail\n");
			shutdown(sfd_acc,2);
			close(sfd_acc);
			return false;
		}
		//设置close-on-exec标志
		fcntl(sfd_acc, FD_CLOEXEC, true);

		int tmp = fork();
		if(tmp == -1){
			_printf("fork fail, errno = %d\n", errno);
			shutdown(sfd_acc,2);
			close(sfd_acc);
			return false;
		}
		if(tmp == 0){
			//当前新建的子进程
			_printf("i am son process, pid = %d\n", getpid());
			tmp = do_mission(sfd_acc);
			if(!tmp)
				_printf("son process %d do_mission() fail !!\n", getpid());
			shutdown(sfd_acc,2);
			close(sfd_acc);
			sleep(2);
			if(!tmp)
				return false;
			else
				return true; 
			//exit(0);//子进程exit(), 系统会自动回收所有东西, waitpid 会失败!
								//因为进程里面的记录已经被清除
		}
		else{
			//父进程需要做的事
			pid_pool.insert(pid_pool.end(),tmp);
			test_count++;
			_printf("son pid %d,deque val:%d\n",tmp,pid_pool[0]);
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
	sprintf(xbuf, \
		"hello client, you are the %d\n", test_count);

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

