//111111111111111111111111111111111111111111111111111111111111111111111111
//格式测试:
//邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵
//起始日期:
//完成日期:
//************************************************************************
//修改日志:

//, , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , ,, , ,

//编译:
//g++ -o x ./stat.cpp -ggdb3


#include <sys/types.h>//for stat(), man 2 stat
#include <sys/stat.h>
#include <unistd.h>

#include <sys/types.h>//for ftok(), man 3 ftok
#include <sys/ipc.h>

#include <stdio.h>
#include <errno.h>
#include <time.h>
#include <stdlib.h>

//创建ftok() key 之前, 用stat()检查一下路径是否被占用
//相当与: stat <file_path> 查看文件信息一样
void stat_check(char* pfile_name);



int main(void){
	char file_name[260] = "./stat.cpp";
	key_t kfd;



	//1.stat() 检查路径
	stat_check(file_name);


	//2.ftok() 测试
	kfd = ftok(file_name, 0x57);
	if(kfd == -1){
		perror("ftok() failed");
		exit(EXIT_FAILURE);//1
	}

	return EXIT_SUCCESS;//0
}



//创建ftok() key 之前, 用stat()检查一下路径是否被占用
//相当与: stat <file_path> 查看文件信息一样
void stat_check(char* pfile_name){
	struct stat stat_buf;
	int ret;
	char ptimebuf[26];



	ret = stat(pfile_name, &stat_buf);
	if(ret == -1){
		//如果stat() 出错, 分析出错原因
		perror("Problem getting information, stat() failed");
		switch(errno){
			case ENOENT:
				printf("File %s not found.\n", pfile_name);
				break;
			case EINVAL:
				printf("Invalid parameter to stat().\n");
				break;
		 default:
			 printf("Unexpected error in stat().\n");
		}
		exit(EXIT_FAILURE);
	}
	else{
		//如果stat() 没有出错, 打印stat() 结果
		//Output some of the statistics(输出一些统计数据):
		printf("File size 文件大小: %ld\n", stat_buf.st_size);
		printf("Drive 驱动标记: %c:\n", stat_buf.st_dev + 'A');
		printf("st_dev: %lx, st_ino: %lx\n", \
					 (u_long) stat_buf.st_dev, (u_long) stat_buf.st_ino);

		//打印-最近修改时间
		//ctime_s() 是c11 特有, 线程安全 + 缓冲区安全
		//ctime_s(timebuf, 26, &stat_buf.st_mtime);
		ctime_r(&stat_buf.st_mtime, ptimebuf);//自己保障缓冲区不会溢出
		printf("Time modified 修改时间: %s\n", ptimebuf);
	}
	return;
}
