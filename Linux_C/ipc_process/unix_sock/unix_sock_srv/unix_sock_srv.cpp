//111111111111111111111111111111111111111111111111111111111111111111111111
//格式测试:
//邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵
//起始日期:
//完成日期:
//************************************************************************
//修改日志:

//, , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , ,, , ,


//编译:
//g++ -ggdb3 -o x_cli ./unix_sock_cli.cpp;\
	g++ -ggdb3 -o x_srv ./unix_sock_srv.cpp


#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>



#define UNIX_DOMAIN "/tmp/UNIX.domain"\
	//'客户端-服务端'约定的unix socket文件名
#define LISTEN_LIST_MAX 8\
	//由于unix socket几乎不用等待,这个值可以很小,8/16 都可以



//启动unix socket srv端(SOCK_STREAM 流式socket)
void start_srv(void);



int main(void){
	//启动unix socket srv端(SOCK_STREAM 流式socket)
	start_srv();

	return 0;
}



//启动unix socket srv端(SOCK_STREAM 流式socket)
void start_srv(void){
	int usfd_li,usfd_acc;
	struct sockaddr_un srv_addr_info;
	struct sockaddr_un cli_addr_info;
	socklen_t cli_addr_info_len;
	int tmp;
	char recv_buf[1024];



	//1.创建unix socket(SOCK_STREAM 流式)
	usfd_li = socket(PF_UNIX,SOCK_STREAM,0);
	if(usfd_li<0){
		perror("socket() failed");
		return;
	}


	//2.set unix socket srv端的addr 信息
	srv_addr_info.sun_family=AF_UNIX;
	strncpy(srv_addr_info.sun_path,UNIX_DOMAIN,\
			sizeof(srv_addr_info.sun_path)-1);


	//3.如果unix socket key-name 已经注册, unlink 取消它
	unlink(UNIX_DOMAIN);


	//4.bind() 绑定unix socket 与unix 地址信息
	tmp = bind(usfd_li,(struct sockaddr*)&srv_addr_info,\
				sizeof(srv_addr_info));
	if(tmp == -1){
		perror("bind() failed");
		shutdown(usfd_li,2);
		close(usfd_li);
		unlink(UNIX_DOMAIN);
		return;
	}


	//5.listen() 开始监听
	tmp = listen(usfd_li,LISTEN_LIST_MAX);
	if(tmp==-1){
		perror("listen() failed");
		shutdown(usfd_li,2);
		close(usfd_li);
		unlink(UNIX_DOMAIN);
		return;
	}


	//6.接受unix socket 链接
	usfd_acc = accept(usfd_li,(struct sockaddr*)&cli_addr_info,\
			&cli_addr_info_len);
	if(usfd_acc<0){
		perror("accept() failed");
		shutdown(usfd_li,2);
		close(usfd_li);
		unlink(UNIX_DOMAIN);
		return ;
	}


	//7.读取client 发送过来的一条信息(阻塞读取)
	printf("\n=====mess from client=====\n");
	memset(recv_buf,'\0',sizeof(recv_buf));
	tmp = read(usfd_acc,recv_buf,sizeof(recv_buf));
	if(tmp == -1)
		perror("read() failed(read() from client)");
	else
		printf("Message from client(%d):\n	%s\n",tmp,recv_buf);


	//8.结束回收资源
	close(usfd_acc);
	shutdown(usfd_li,2);
	close(usfd_li);
	unlink(UNIX_DOMAIN);
	return ;
}
