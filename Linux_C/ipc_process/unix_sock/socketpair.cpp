//111111111111111111111111111111111111111111111111111111111111111111111111
//格式测试:
//邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵
//起始日期:
//完成日期:
//************************************************************************
//修改日志:

//, , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , ,, , ,


//编译:
//g++ -ggdb3 -o x ./socketpair.cpp


//socketpair()创建了一对无名的套接字描述符(只能在AF_UNIX域中使用)
/*
	Linux实现了一个源自BSD的socketpair调用,
	在同一个文件描述符中进行读写的功能.
	该系统调用能创建一对已连接的UNIX族socket.
	在Linux中,完全可以把这一对socket当成pipe返回的文件描述符一样使用,
	唯一的区别就是这一对文件描述符中的任何一个都可读和可写.

函数介绍：
	socketpair()函数建立一对匿名的已经连接的套接字,
							其特性由协议族d、类型type、协议protocol决定,
							建立的两个套接字描述符会放在sv[0]和sv[1]中.
	成功返回0, 失败返回-1.

参数介绍：
	第1个参数d, 表示协议族, 只能为AF_LOCAL或者AF_UNIX;
	第2个参数type, 表示类型, 只能为0.
	第3个参数protocol, 表示协议, 可以是SOCKsizeTREAM或者SOCK_DGRAM.
	用SOCKsizeTREAM建立的套接字对是管道流, 与一般的管道相区别的是:
		套接字对建立的通道是双向的, 即每一端都可以进行读写.
		参数sv, 是一个引用返回参数, 用于保存建立的套接字对.
		而管道是单相工, 单向的.

fd描述符存储于一个二元一维数组,例如sv[2].
这对套接字可以进行双工通信, 每一个描述符既可以读也可以写.
这个在同一个进程中也可以进行通信,
向sv[0]中写入, 就可以从sv[1]中读取(只能从sv[1]中读取),
也可以在sv[1]中写入, 然后从sv[0]中读取;

但是, 若没有在0端写入, 而从1端读取,
则1端的读取操作会阻塞, 即使在1端写入, 也不能从1读取, 仍然阻塞;


socketpair 只用做传输数据, 并不能进行数据同步,
socketpair 数据同步也一般不用信号量, 信号量一般面向共享内存.
socketpair 数据同步一般使用信号,因为不是构建临界区,
						而是通知对方去取数据, 用信号函数比较好.
socketpair 一般只用作fork() 之间通信



*/



#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>



//socketpair() 简单双向通信测试
void socketpair_test(void);



int main(void){
	//socketpair() 简单双向通信测试
	socketpair_test();

	return 0;
}



//socketpair() 简单双向通信测试
void socketpair_test(void){
	int sv[2];//一对无名的套接字描述符
	int tmp;
	pid_t id;
	const char* msg2son = "我是父亲\n";
	const char* msg2father = "我是孩子\n";
	char buf[1024];
	ssize_t size;//接收/发送的数据大小tmp 值



	//1.创建socketpair() socket 对, sv[0],sv[1] socket 关联sfd组
	tmp = socketpair(PF_LOCAL,SOCK_STREAM,0,sv);
	if(tmp < 0){
		perror("socketpair()");
		return ;
	}

	//2.执行fork() 创建子进程
	id = fork();
	if(id == -1){
		perror("fork() failed");
		return;
	}
	if(id == 0){//子进程
		//close(sv[0]);//关闭一个端, 是sv[0],sv[1]都是可写可读的
		close(sv[1]);//子进程-关闭sv[1]

		while(1){
			//write(sv[1],msg,strlen(msg));
			write(sv[0],msg2father,strlen(msg2father));//阻塞write()
			sleep(1);

			//size = read(sv[1],buf,sizeof(buf)-1);
			size = read(sv[0],buf,sizeof(buf)-1);//阻塞read()
			if(size > 0){
				buf[size] = '\0';
				printf("read() from 父亲 : %s\n",buf);

				//只接收一个回覆便退出
				close(sv[0]);
				//close(sv[1]);//子进程-关闭sv[1]
				break;
			}
		}
	}
	else{//父进程
		//close(sv[1]);
		close(sv[0]);//父进程-关闭sv[0]

		while(1){
			//size = read(sv[0],buf,sizeof(buf)-1);
			size = read(sv[1],buf,sizeof(buf)-1);//阻塞read()
			if(size > 0){
				buf[size] = '\0';
				printf("read() from 孩子 : %s\n",buf);
				sleep(2);
			}

			//write(sv[0],msg,strlen(msg));
			write(sv[1],msg2son,strlen(msg2son));//阻塞write()

			//只接收一个回覆便退出
			close(sv[1]);
			//close(sv[0]);//父进程-关闭sv[0]
			break;
		}
	}

	return;
}
