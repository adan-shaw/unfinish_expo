//111111111111111111111111111111111111111111111111111111111111111111111111
//格式测试:
//邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵
//起始日期:
//完成日期:
//************************************************************************
//修改日志:

//, , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , ,, , ,


//编译:
//g++ -ggdb3 -o x ./ftok.cpp -std=c++0x


//1.为什么'进程间通讯'需要有一个key_t 的键？
/*
	建立'进程间通讯'(如消息队列、共享内存时)必须指定一个ID值.
	(消息队列, 共享内存等, 对系统来说, 类似于file, 一切皆file)

	通常情况下, 该id值通过ftok() 函数得到.
	key_t 实际是文件名 + 自定义索引编号得到的.

	因为一个文件可以有很多<用户/进程>索引,
	(文件特性在'进程间通讯'里面同样适合)
	系统还需要用编号来区分<用户/进程>彼此的不同.
	文件是所有<用户/进程>共享的, 这就是<文件名+索引编号>存在的理由

	而根据文件路径, 根据stat(), 你可以索引出文件属性,
	只有fd 描述符只是一个整形, 不能区分这是什么文件.
	socket? pid? file?

	至于文件描述符, fd, 则是进程内部的访问资源<令牌>,
	也是<一连串访问方法函数>, io buffer, io stat 等的综合体指针
*/



//ftok原型如下：
//key_t ftok(char* fname, int id);
//fname就时你指定的文件名(该文件必须是存在而且可以访问的)
//id是子序号, 虽然为int, 但是只有8个比特被使用(0-255)

//当成功执行的时候, 一个key_t值将会被返回, 否则 -1 被返回


//在一般的UNIX实现中, 是将文件的<索引节点号>取出, 
//前面加上子序号得到key_t的返回值. 
//
//如指定文件的索引节点号为65538, 换算成16进制为 0x010002, 
//而你指定的ID值为38, 换算成16进制为0x26, 
//则最后的key_t返回值为0x26010002. 
//查询文件索引节点号的方法是： ls -i

#include<sys/types.h>//for stat(), man 2 stat
#include<sys/stat.h>
#include<unistd.h>

#include<sys/types.h>//for ftok(), man 3 ftok
#include<sys/ipc.h>

#include<stdio.h>
#include<errno.h>
#include<time.h>
#include<stdlib.h>

/*
struct stat {
	dev_t			 st_dev;		// ID of device containing file -文件所在设备的ID
	ino_t			 st_ino;		// inode number -inode节点号
	mode_t			st_mode;	// protection -保护模式?
	nlink_t		 st_nlink;	// number of hard links -链向此文件的连接数(硬连接)
	uid_t			 st_uid;		// user ID of owner -user id
	gid_t			 st_gid;		// group ID of owner - group id
	dev_t			 st_rdev;		// device ID (if special file) -设备号, 针对设备文件
	off_t			 st_size;		// total size, in bytes -文件大小, 字节为单位
	blksize_t	 st_blksize;// blocksize for filesystem I/O -系统块的大小
	blkcnt_t		st_blocks;// number of blocks allocated -文件所占块数
	time_t			st_atime;	// time of last access -最近存取时间
	time_t			st_mtime;	// time of last modification -最近修改时间
	time_t			st_ctime;	// time of last status change -
};
*/


//创建ftok() key 之前, 用stat()检查一下路径是否被占用
//相当与: stat <file_path> 查看文件信息一样
void stat_check(char* pfile_name);



int main(void){
	char file_name[260] = "./x";//以自身索引来做ftok() key_t
	key_t kfd;



	//1.stat() 检查路径
	stat_check(file_name);


	//2.ftok() 测试
	//ipcs -s # 查看是否存在与ftok() 返回值一样的key_t, 存在可删除
	//ipcs -m <key_t> # 删除该ftok() key_t
	kfd = ftok(file_name, 0x57);
	if(kfd == -1)
		perror("ftok() failed");
	else
		printf("ftok() = %d\n",kfd);

	return 0;
}



//创建ftok() key 之前, 用stat()检查一下路径是否被占用
//相当与: stat <file_path> 查看文件信息一样
void stat_check(char* pfile_name){
	struct stat stat_buf;
	int ret;
	char ptimebuf[26];
	char* pret;



	ret = stat(pfile_name, &stat_buf);
	if(ret == -1){
		//如果stat() 出错, 分析出错原因
		perror("stat() failed");
		switch(errno){
			case ENOENT:
				printf("File %s not found.\n", pfile_name);
				break;
			case EINVAL:
				printf("Invalid parameter to _stat.\n");
				break;
			default:
				printf("Unexpected error in _stat.\n");
		}
		return;
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
