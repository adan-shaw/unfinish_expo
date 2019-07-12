//111111111111111111111111111111111111111111111111111111111111111111111111
//格式测试:
//邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵
//起始日期:
//完成日期:
//************************************************************************
//修改日志:

//, , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , ,, , ,

//编译:
//g++ -o x ./fifo.cpp -ggdb3

//1.'有名管道'简介
/*
1.1 无名管道fifo与'有名管道'fifo的区别:
	无名管道应用的一个重大限制是它没有名字,
	因此, 只能用于具有亲缘关系的进程间通信,
	在'有名管道'(named fifo或FIFO)提出后, 该限制得到了克服.


1.2 '有名管道'的实现细节:
	FIFO不同于无名管道之处在于它提供一个路径名与之关联,
	以FIFO的文件形式存在于文件系统中.
	这样, 即使与FIFO的创建进程不存在亲缘关系的进程,
	只要可以访问该路径, 就能够彼此通过FIFO相互通信.
	因此, 通过FIFO不相关的进程也能交换数据.


1.3 '有名管道'的使用规则:
	FIFO严格遵循先进先出(first in first out),
	对管道及FIFO的读总是从开始处返回数据,
	对它们的写则把数据添加到末尾.
	它们不支持诸如lseek()等文件定位操作.

	'有名管道'的缓冲区是有限的('有名管道'创建时,为缓冲区分配一个页面大小)
	'有名管道'所传送的是无格式字节流,
	这就要求'有名管道'的读出方和写入方必须事先约定好数据的格式,
	比如多少字节算作一个消息(或命令、或记录)等等.

	FIFO往往都是多个写进程, 一个读进程.


1.4 FIFO的打开规则：
	如果当前打开操作是为读而打开FIFO时,
	若已经有相应进程为写而打开该FIFO,
	则当前打开操作将成功返回;
	否则,可能阻塞直到有相应进程为写而打开该FIFO(当前打开操作设置了阻塞标志);
	或者, 成功返回(当前打开操作没有设置阻塞标志).

	如果当前打开操作是为写而打开FIFO时,已经有相应进程为读而打开该FIFO,
	则当前打开操作将成功返回;
	否则,可能阻塞直到有相应进程为读而打开该FIFO(当前打开操作设置了阻塞标志);
	或者, 返回ENXIO错误(当前打开操作没有设置阻塞标志).

	总之就是一句话, 一旦设置了阻塞标志, 调用mkfifo建立好之后,
	那么'有名管道'的两端读写必须分别打开,
	有任何一方未打开, 则在调用open的时候就阻塞.


1.5 从FIFO中读取数据：
	约定：
		如果一个进程为了从FIFO中读取数据而阻塞打开FIFO,
		那么称该进程内的读操作为设置了阻塞标志的读操作.
		(意思就是我现在要打开一个'有名管道'来读数据！)

		如果有进程写打开FIFO, 且当前FIFO内没有数据
		(可以理解为管道的两端都建立好了, 但是写端还没开始写数据！)
		则对于设置了阻塞标志的读操作来说, 将一直阻塞
		(就是block住了, 等待数据. 它并不消耗CPU资源,
		 这种进程的同步方式对CPU而言是非常有效率的.)
		对于没有设置阻塞标志读操作来说则返回-1,
		当前errno值为EAGAIN, 提醒以后再试.

	对于设置了阻塞标志的读操作说(见上面的约定)
	造成阻塞的原因有两种:
		FIFO内有数据, 但有其它进程在读这些数据
		FIFO内没有数据. 解阻塞的原因:
			则是FIFO中有新的数据写入,
			不论信写入数据量的大小, 也不论读操作请求多少数据量.

	读打开的阻塞标志只对本进程第一个读操作施加作用,
	如果本进程内有多个读操作序列,
	则在第一个读操作被唤醒并完成读操作后,
	其它将要执行的读操作将不再阻塞,
	即使在执行读操作时, FIFO中没有数据也一样,此时, 读操作返回0.

	注：如果FIFO中有数据,
	则设置了阻塞标志的读操作不会因为FIFO中的字节数小于请求读的字节数而阻塞,
	此时, 读操作会返回FIFO中现有的数据量.


1.6 向FIFO中写入数据：
	约定：
		如果一个进程为了向FIFO中写入数据而阻塞打开FIFO,
		那么称该进程内的写操作为设置了阻塞标志的写操作.
		(意思就是我现在要打开一个'有名管道'来写数据！)

	对于设置了阻塞标志的写操作：
		当要写入的数据量不大于PIPE_BUF时, linux将保证写入的原子性.
		如果此时管道空闲缓冲区不足以容纳要写入的字节数,
		则进入睡眠, 直到当缓冲区中能够容纳要写入的字节数时,
		才开始进行一次性写操作.
		(PIPE_BUF ==>> /usr/include/linux/limits.h)

		当要写入的数据量大于PIPE_BUF时, linux将不再保证写入的原子性.
		FIFO缓冲区一有空闲区域, 写进程就会试图向管道写入数据,
		写操作在写完所有请求写的数据后返回.

	对于没有设置阻塞标志的写操作：
		当要写入的数据量大于PIPE_BUF时, linux将不再保证写入的原子性.
		在写满所有FIFO空闲缓冲区后, 写操作返回.
		当要写入的数据量不大于PIPE_BUF时, linux将保证写入的原子性.
		如果当前FIFO空闲缓冲区能够容纳请求写入的字节数, 写完后成功返回;
		如果当前FIFO空闲缓冲区不能够容纳请求写入的字节数,
		则返回EAGAIN错误, 提醒以后再写;



//2.其他所需函数介绍.
//2.1: access() 函数介绍
int access(const char* pathname, int mode);
返回值：成功0, 失败-1
参数介绍：
	pathname 是文件的路径名+文件名
	mode：指定access的作用, 取值如下
		F_OK 值为0, 判断文件是否存在
		X_OK 值为1, 判断对文件是可执行权限
		W_OK 值为2, 判断对文件是否有写权限
		R_OK 值为4, 判断对文件是否有读权限
	注：后三种可以使用或“|”的方式, 一起使用, 如W_OK|R_OK


//2.2:mkfifo()
int mkfifo(const char * pathname,mode_t mode);
返回值:
	若成功则返回0, 否则返回-1, 错误原因存于errno中.

函数说明:
	mkfifo()会依参数pathname建立特殊的FIFO文件,
	该文件必须不存在, 而参数mode为该文件的权限(mode%~umask),
	因此umask值也会影响到FIFO文件的权限.
	Mkfifo()建立的FIFO文件其他进程都可以用读写一般文件的方式存取.
	当使用open()来打开 FIFO文件时, O_NONBLOCK旗标会有影响:
		1、当使用O_NONBLOCK旗标时,打开FIFO文件来读取的操作会立刻返回,
				但是若还没有其他进程打开FIFO 文件来读取,
				则写入的操作会返回ENXIO 错误代码.
		2、没有使用O_NONBLOCK旗标时,
				打开FIFO来读取的操作会等到其他进程打开FIFO文件来写入才正常返回.
				同样地, 打开FIFO文件来写入的操作,
				会等到其他进程打开FIFO文件来读取后才正常返回.

错误代码:
	EACCESS	参数pathname所指定的目录路径无可执行的权限
	EEXIST	参数pathname所指定的文件已存在.
	ENAMETOOLONG	参数pathname的路径名称太长.
	ENOENT	参数pathname包含的目录不存在
	ENOSPC	文件系统的剩余空间不足
	ENOTDIR	参数pathname路径中的目录存在但却非真正的目录.
	EROFS		参数pathname指定的文件存在于只读文件系统内.

*/



#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdbool.h>



#define FIFO_NAME "./fifo_test"
//#define TEN_MEG (1024 * 4)
//#define BUFFER_SIZE PIPE_BUF
#define TEN_MEG (1024 * 63)//极限是1024 * 64, 超出这个数假死(阻塞).
#define BUFFER_SIZE TEN_MEG//单次写入也是极限:1024 * 64,超出(阻塞)
#define FIFO_LIMITS 0666 //FIFO 使用权限(重要,没有这个,FIFO不能正常工作)
//#define FIFO_LIMITS 0777



//一次性递归所需的判别变量
//bool is_recursion = false;
int G_fifo_fd=-1;//记住打开的'有名管道',使用完之后再close()关闭
int G_fifo_write_size=0;//'有名管道'写入的数据大小,决定了读出时的数据大小



//1.'有名管道'-write 测试
void fifo_write(void);

//2.'有名管道'-read 测试
void fifo_read(void);



int main(void){
	//1.'有名管道'-write 测试
	fifo_write();

	//2.'有名管道'-read 测试
	fifo_read();

	//3.释放fifo '有名管道'
	close(G_fifo_fd);
	return 0;
}



//1.'有名管道'-write 测试
void fifo_write(void){
	int fifo_fd;
	int tmp;
	int bytes = 0;
	char buffer[BUFFER_SIZE + 1] = "hello world: fifo";



	//1.询问FIFO_NAME 是否已经被占用
	//	(本段1, 对fifo 测试无效!!)
	tmp = access(FIFO_NAME, F_OK);
	if(tmp == -1){
		perror("access() failed");
		printf("cant find file:%s, good for mkfifo!!\n",FIFO_NAME);
		//return;
	}
	else if(tmp != 0){
		printf("access(%s) != 0, cant access file\n",FIFO_NAME);
		//这个递归实用意义不大, 但是表明如何做一个一次性递归.(只递归一次)
		/*
		if(!is_recursion){
			is_recursion = true;//递归过一次了, 就不能再递归了, 防止死循环
			unlink(FIFO_NAME);//清除'有名管道'占用
			fifo_write();//递归调用
		}
		*/
		return;
	}
	else{
		unlink(FIFO_NAME);//如果存在'同名同路径的文件',
											//则删除它, 再创建'有名管道'.
		printf("unlink(%s) okay\n",FIFO_NAME);
	}


	//2.创建'有名管道'O_RDWR = 可读可写
	//unlink(FIFO_NAME);//尝试先移除其它应用'占用管道'.(会删除'目标文件'的)
										//(unlink()必须要有,防止重复调用程序时,出错)

	//FIFO_LIMITS=0666是'有名管道'权限,不是'有名管道'读写模式;
	//后面的open()读写模式应该与FIFO_LIMITS一致.
	tmp = mkfifo(FIFO_NAME, FIFO_LIMITS);
	//tmp = mkfifo(FIFO_NAME, O_RDWR);//O_WRONLY | O_RDONLY
	if(tmp == -1){
		perror("mkfifo() failed, Could not create fifo(没权限??)\n");
		return;
	}
	G_fifo_fd = fifo_fd;//记住fifo'有名管道', 方便释放资源


	//3.打开FIFO '有名管道'
	printf("Process %d opening FIFO %d\n",\
			getpid(),FIFO_LIMITS);
	fifo_fd = open(FIFO_NAME, FIFO_LIMITS);
	if(fifo_fd == -1){
		perror("open() failed");
		unlink(FIFO_NAME);
		return;
	}
	printf("Process %d result %d\n", getpid(), fifo_fd);


	usleep(2000);


	//4.向FIFO '有名管道'写入数据,
	//	数据总量为:TEN_MEG, 单次数据量为:strlen(buffer)+1
	//4.1: 先填充buffer 缓冲区
	for(tmp=0;tmp<sizeof(buffer);tmp++)
		buffer[tmp] = 'A';
	buffer[tmp] = '\0';
	//4.2: 再按照FIFO写入任务量, 写入数据
	while(bytes < TEN_MEG){
		tmp = write(fifo_fd, buffer, strlen(buffer));//阻塞!!同步写,会阻塞
		if(tmp == -1){
			perror("write() failed");
			close(fifo_fd);
			unlink(FIFO_NAME);
			return;
		}
		bytes += tmp;
		printf("written data size: %d\n",bytes);
	}
	//close(fifo_fd);//不能随便关闭fifo, 否则找不到了.
	//unlink(FIFO_NAME);//会删除'目标文件'的
	G_fifo_write_size = bytes;

	printf("Process %d finish\n\n\n", getpid());
	return;
}



//2.'有名管道'-read 测试
void fifo_read(void){
	int fifo_fd;
	int tmp;
	char buffer[BUFFER_SIZE + 1];
	int bytes = 0;



	//1.打开FIFO '有名管道'(此时已经创建, 且没有unlink()释放)
	printf("Process %d opening FIFO='%s' %d\n",\
			getpid(),FIFO_NAME,FIFO_LIMITS);
	fifo_fd = open(FIFO_NAME, FIFO_LIMITS);
	if(fifo_fd == -1){
		perror("open() failed");
		return;
	}
	printf("Process %d result %d\n", getpid(), fifo_fd);


	//2.从FIFO '有名管道'中读取数据
	memset(buffer, '\0', sizeof(buffer));
	do{
		tmp = read(fifo_fd, buffer, BUFFER_SIZE);//阻塞!!同步读,会阻塞
		if(tmp == -1){
			perror("read() failed");
			close(fifo_fd);
			return;
		}
		bytes += tmp;
		printf("read data size: %d\n",bytes);
		printf("data:\n%s\n\n",buffer);//显示读到的数据
	}while(bytes < G_fifo_write_size);

	close(fifo_fd);

	printf("Process %d finished, %d bytes read\n", getpid(), bytes);
	return;
}
