//编译:
//g++ -o x ./test_fork_socket.cpp -ggdb3


//demo：fork()子进程,子进程fork()孙进程,孙进程退出,init进程回收
//			不产生僵尸进程,
//			记录孙进程的工作状态(通过waitpid()失败且errno=10来判断)
//			子进程返回孙进程的pid 给父进程
//			由此可限定孙进程的数量(也是服务器最大并发量)

//附: 为了迎合设计, 这里引用最简单的udp socket 来展示设计

#include <stdio.h>
#include <string.h>
#include <strings.h> // for bzero
#include <assert.h>
#include <errno.h>
#include <unistd.h> // for fork() getpid()

#include <sys/types.h>
#include <sys/wait.h> // for waitpid

#include <sys/socket.h> // for socket

#include <netinet/in.h> // for inet_addr
#include <arpa/inet.h>

#include <deque>

#include <fcntl.h> // for fcntl


//宏定义列表
#ifdef _printf
#else
	#define _printf(args...) printf(args)
#endif

#define fork_count_max 8192
#define srv_ip "127.0.0.1"
#define srv_port 6666
#define BACKLOG 64 //监听队列最大长度 64

#define deque_del_sig -1

#define io_buf_max 512
#define io_sock_close_timeout 2

//全局变量列表
int pid_main = 0;//主线程pid
int sfd_li = 0;//监听sfd
std::deque<int> pid_pool;
int err_count = 0;//错误count
int test_count = 0;//接待count
bool is_working = false;


//函数声明
//孙进程的任务函数(子进程只负责创建孙进程并返回孙进程pid)
//孙进程的任务函数 必须返回int !!, 输入参数可以变
int grandson_mission(int sfd_acc);

//waitpid() 测试
void test_waitpid(int _pid);
//wait() 测试
void test_wait(void);



//主函数
int main(int argc, char** argv){
	//全局变量初始化
	pid_main = getpid();

	//创建监听socket
	sfd_li = socket(AF_INET, SOCK_STREAM, 0);
	if(sfd_li == -1){
		_printf("socket() fail, errno = %d\n", errno);
		return -1;
	}

	//设置地址重用
	int opt_val = true;
	opt_val = setsockopt(sfd_li, SOL_SOCKET, SO_REUSEADDR, \
							&opt_val, sizeof(int));
	if(opt_val == -1){
		_printf("set_sockopt_reuseaddr() fail, errno = %d\n", errno);
		close(sfd_li);
		return -1;
	}

	//设置监听socket 不阻塞
	opt_val = fcntl(STDIN_FILENO, F_GETFL, 0);
	if(opt_val == -1){
		_printf("fcntl() fail, errno = %d\n", errno);
		close(sfd_li);
		return -1;
	}
	opt_val |= O_NONBLOCK;
	opt_val = fcntl(STDIN_FILENO, F_SETFL, opt_val);
	if(opt_val == -1){
		_printf("fcntl() fail, errno = %d\n", errno);
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

	//工作循环
	_printf("program ready, working hard, good start.\n");
	is_working = true;
	struct sockaddr xaddr;
	socklen_t xaddrlen;
	int tmp;
	int sfd_acc;
	int gson_pid;
	while(is_working){
		bzero(&xaddr, sizeof(struct sockaddr));
		xaddrlen = 0;
		//_printf("xx1\n\n");//阻塞??what the hell?
		sfd_acc = accept4(sfd_li,&xaddr,&xaddrlen,\
								SOCK_NONBLOCK|SOCK_CLOEXEC);
		//_printf("xx2\n\n");//不是说不阻塞么?
		if(sfd_acc == -1){
			_printf("accept4() fail, errno=%d\n",errno);
			is_working = false;
			break;
		}
		else{
			tmp = pid_pool.size();//获取线程池的max
			if(tmp > fork_count_max - 2){//超出进程max管限范围?
				shutdown(sfd_acc,2);
				close(sfd_acc);
				_printf("overranging max of process, %d\n", tmp);
				err_count++;
				_printf("err_count = %d\n",err_count);
				continue;
			}
			tmp = fork();
			if(tmp == -1){
				_printf("fork() fail, errno = %d\n", errno);
				shutdown(sfd_acc,2);
				close(sfd_acc);
				is_working = false;
				break;
			}
			if(tmp == 0){
				//son go on, 子进程
				tmp = fork();
				if(tmp != 0 && tmp != -1){
					return tmp;//子进程马上结束, 返回孙进程pid
				}
				else{
					//grandson go on, 孙进程
					return grandson_mission(sfd_acc);//孙进程执行任务函数后结束
																					 //参数的数量可以改!!

				}
			}
			else{
				//father go on, 父进程
				gson_pid = -1;
				if(waitpid(tmp,&gson_pid,0) == -1){//等待子进程结束,不允许失败
					_printf("waitpid()4 [%d] fail!, errno = %d\n",tmp,errno);
					is_working = false;
					break;
				}
				//创建孙进程成功, 记录孙进程pid
				pid_pool.insert(pid_pool.end(),gson_pid);
				test_count++;
			}
		}

		//检查孙进程的结束状态(未知孙进程是否已经结束, 但这里是随机检查)
		int deq_size = pid_pool.size();//获取线程池的max

		if(deq_size > 0){//cleanup if start

		int gson_ret;
		int que_del_count = 0;
		for(tmp = 0;tmp < deq_size;tmp++){
			int x = waitpid(pid_pool[tmp], &gson_ret, WNOHANG);
			if(x == -1 && errno != 10){//未知错误
				_printf("waitpid()4 %d fail, errno = %d\n",\
					pid_pool[tmp], errno);
				is_working = false;
				break;
			}
			if(x == -1 && errno == 10){//孙进程已经结束且结束记录不见了
				que_del_count++;				 //记录不见了就会被Init 进程消灭
				pid_pool[tmp] = deque_del_sig;
				continue;
			}
			if(x == 0){//孙进程还没结束
				continue;
			}
			if(x == pid_pool[tmp]){//孙进程正常结束
				que_del_count++;
				pid_pool[tmp] = deque_del_sig;//修改值为deque_del_sig,统一删除
				continue;
			}
			//未知错误
			_printf("waitpid()4 %d unknow error, errno = %d\n",\
				pid_pool[tmp], errno);
			err_count++;
			que_del_count++;
			pid_pool[tmp] = deque_del_sig;
		}

		//统一清除deque 里面的gpid 标记
		for(tmp = deq_size - 1;tmp >= 0;tmp--){
			if(pid_pool[tmp] == -1)
			pid_pool.erase(pid_pool.begin()+tmp);
			que_del_count--;
		}
		if(que_del_count != 0){
			_printf("pid_pool.erase() unknow error, que_del_count=%d\n",\
				que_del_count);
		}

		}//cleanup if end

	}//while(is_working) end

	if(getpid() == pid_main){//资源回收
		//清空deq, 就算还有grandson 也不管了, 反正退出后init 会自动清理
		pid_pool.clear();
		//关闭socket
		shutdown(sfd_li,2);
		close(2);
		_printf("father pid = %d quit\n",pid_main);
	}
	else{
		_printf("unknow error,son/grandson shouldnt run to here\n");
		_printf("father pid = %d, now pid = %d\n",pid_main,getpid());
	}

	return 0;
}



//孙进程的任务函数(子进程只负责创建孙进程并返回孙进程pid)
//孙进程的任务函数 必须返回int !!, 输入参数可以变
int grandson_mission(int sfd_acc){
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
