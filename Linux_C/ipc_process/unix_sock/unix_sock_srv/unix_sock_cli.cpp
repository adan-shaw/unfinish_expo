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



//启动unix socket cli端, 链接到srv端(SOCK_STREAM 流式socket)
void start_cli(void);



int main(void){
	//启动unix socket cli端, 链接到srv端(SOCK_STREAM 流式socket)
	start_cli();

	return 0;
}



//启动unix socket cli端, 链接到srv端(SOCK_STREAM 流式socket)
void start_cli(void){
	int usfd_conn;
	struct sockaddr_un srv_addr_info;
	int tmp;
	char snd_buf[1024];



	//1.创建unix socket(SOCK_STREAM 流式)
	usfd_conn = socket(PF_UNIX,SOCK_STREAM,0);
	if(usfd_conn == -1){
		perror("socket() failed");
		return;
	}


	//2.填充unix socket srv 端地址信息
	srv_addr_info.sun_family = AF_UNIX;
	strncpy(srv_addr_info.sun_path,UNIX_DOMAIN,\
			sizeof(srv_addr_info.sun_path)-1);


	//3.connect 链接到srv 端
	tmp = connect(usfd_conn,(struct sockaddr*)&srv_addr_info,\
			sizeof(srv_addr_info));
	if(tmp == -1){
		perror("connect() failed");
		close(usfd_conn);
		return ;
	}


	//4.send mes to server 发送一次数据到srv端(阻塞发送)
	memset(snd_buf,'\0',sizeof(snd_buf));
	strncpy(snd_buf,"hello, the unix socket server",sizeof(snd_buf));
	tmp = write(usfd_conn,snd_buf,sizeof(snd_buf));
	if(tmp == -1)
		perror("write() failed(write to server)");
	else
		printf("Message send to server(%d):\n	%s\n",tmp,snd_buf);


	//5.结束回收资源
	close(usfd_conn);
	return ;
}
