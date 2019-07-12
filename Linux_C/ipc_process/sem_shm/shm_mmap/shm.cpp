//111111111111111111111111111111111111111111111111111111111111111111111111
//格式测试:
//邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵
//起始日期:
//完成日期:
//************************************************************************
//修改日志:

//, , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , ,, , ,


//编译:
//g++ -ggdb3 -o x ./shm.cpp


//https://www.cnblogs.com/52php/p/5861372.html
//https://blog.csdn.net/ydyang1126/article/details/52804268

//1.mmap && shm 介绍:
/*
	mmap 映射到磁盘, shm 映射到内存.
	mmap 已经成为概念产品, 不实用的,
	并没有自动内存换页, 这么智能化的技术.
	如果内存不足, 需要换页保存数据, 还是需要自己操作file.

	shm 却很实用, shm 主要是‘内存块地址'映射到'进程堆地址',
	供多进程之间访问(但一般需要加有名sem 信号灯, 来做同步控制.)
	shm 主要做'数据共享', 不做'同步控制'.
*/



//2.shm 的使用:
/*
2.1 共享内存shm 使用原则:
	共享内存的实现原理(类似于file, 将内存块变成内存file, 和进程共享).

	但实际使用方式, 比较类似于: malloc()和free();
	不同点是:
		shm 分配到的变量内存地址, 是在内核手上, 你还需要多一个步骤,
		从内核手上取出'共享内存地址'.

	多进程共享时, 就需要利用file特性了.
	根据int shm_id;调用shmat()找到shm变量地址,从内核手上,取出shm变量.

	因此:
		共享内存的变量怎样排列存放, 不用担心, 内核会自己帮你安排.
		你只需要关注: 申请的共享内存, 是否足够大容纳该变量.

	附录:
		一块共享内存, 只能存放一个变量.
		多个变量需要共享, 就需要有多个共享内存.


2.2 共享内存shm与sem 协同使用:
	shm 只能共享数据, 不能做多进程同步协作操作.
	因此一般需要配有sem 有名信号灯, 来控制shm 的使用同步.

*/



//3.共享内存shell 操作
/*
3.1 查看与释放:
	shm 释放不了, 用ipcs and ipcrm 来操作即可.

	ipcs 看到nattch 项 为 0的, 表示没有人使用了, 你可以随便释放.

	ipcrm -m <shm_id> # shell 释放操作


3.2 shm 系统限制:
	共享内存有限制:
		SHMMAX, SHMMAX 的默认值是 32MB

	# shell 查看SHMMAX 的命令
	# cat /proc/sys/kernel/shmmax
*/



//4.shm API和结构体 总览:
/*
1、int shmget(key_t key, size_t size, int shmflg);
第一个参数,
	参数key(非0整数),它有效地为共享内存段命名,
	shmget()函数成功时, 返回一个与key相关的shm_id(非负整数),
	用于后续访问, 共享的凭证.
第二个参数,
	size 字节为单位, 指定'共享的内存容量'.

第三个参数,
	shmflg是权限标志.
	如果要想'不存在时创建', 可以添加: IPC_CREAT|0644.
	‘共享内存’的权限标志, 与文件的读写权限一样, 没权限的用户是不能访问的.
	举例来说, 0644, 它表示:
		允许'进程创建者用户'的进程, 对共享内存'享有读取和写入'数据的权限,
		'其他用户'创建的进程, 只能读取共享内存.



2、void *shmat(int shm_id, const void *shm_addr, int shmflg);
第一个参数,
	shm_id由shmget()函数返回的(非负整数)

第二个参数,
	shm_addr 通常为空, 表示让内核'自动选择'共享内存的地址.

第三个参数,
	shm_flg是一组标志位, 通常为0.



3、int shmdt(const void *shmaddr);
	shmdt() 用于将'共享内存'从当前进程中分离.
	(即当前进程, 放弃'共享内存'使用权)
	注意：
		将’共享内存‘分离并不是删除它,
		只是使'该共享内存'对当前进程不再可用.
第一个参数,
	shmaddr是shmat()函数返回的'共享内存地址指针';



4、int shmctl(int shm_id, int command, struct shmid_ds *buf);
	shmctl()用来控制共享内存.

第一个参数,
	shm_id由shmget()函数返回的(非负整数)

第二个参数,
	command 是要采取的操作, 它可以取下面的三个值：
		IPC_STAT：
			引用返回'shmid_ds{}结构体'中的数据.(读取)
		IPC_SET：
			如果进程有足够的权限,
			即覆盖'原来的shmid_ds{}结构体'的值.(修改)
		IPC_RMID：
			删除共享内存段,不需要'shmid_ds{}结构体'.(删除)

第三个参数,
	buf是一个shmid_ds{}结构指针, 它指向共享内存模式和访问权限的结构.
struct shmid_ds{
	uid_t shm_perm.uid;
	uid_t shm_perm.gid;
	mode_t shm_perm.mode;
};
 */



#include <sys/ipc.h>
#include <sys/shm.h>//共享内存 needs
#include <errno.h>
#include <stdio.h>
#include <unistd.h> // for getpagesize
#include <stdlib.h>
#include <string.h>



//共享内存测试
void shm_test(void);



int main(void){
	//共享内存测试
	shm_test();

	return 0;
}



//共享内存测试
void shm_test(void){
	key_t shm_key;//<文件名+索引编号> key
	int shm_id1,shm_id2;		//'共享内存'标识id
	char* pshm1;	//'共享内存'地址指针1
	int pshm1_len;
	int* pshm2;		//'共享内存'地址指针2
	int tmp;



	//1.初始化'共享内存shm'key_t信息
	//	同一个key_t只需要释放一次即可.
	//	同一个ket_t下,可以有多个shm 变量,多个shm_id,多个共享内存!!
	shm_key = ftok("./x", 0x57);//以自身索引来做ftok() key_t
	if(shm_key == -1){
		perror("ftok() failed");
		return;
	}
	else
		printf("ftok() = %d\n",shm_key);
	//ipcs -s # 查看是否存在与ftok() 返回值一样的key_t, 存在可删除
	//ipcs -m <key_t> # 删除该ftok() key_t



	//2.创建共享内存--存在则打开, 不存在则创建
	pshm1_len = getpagesize();//获取系统页内存大小
	shm_id1=shmget(shm_key,pshm1_len,IPC_CREAT|SHM_R|SHM_W);
	if(shm_id1 == -1){
		perror("shmget() failed");
		return ;
	}

	//同一个key_t 用两次
	tmp = sizeof(int);//取int 的大小
	shm_id2=shmget(shm_key,tmp,IPC_CREAT|SHM_R|SHM_W);
	if(shm_id2 == -1){
		perror("shmget() failed");
		//尝试释放shm_id1
		if(shmctl(shm_id1, IPC_RMID, 0) == -1)
			perror("shmctl() failed");
		return ;
	}



	//3.从内核手上, 提出'共享内存'到进程堆上面.
	pshm1 = (char*)shmat(shm_id1, NULL, 0);
	if(*pshm1 == -1){
		perror("shmat() failed");
		goto shm_test__failed;
	}
	else{
		//操作共享内存-写
		snprintf(pshm1,pshm1_len,"%s", "hello world");
		//读取'共享内存'数据, 并打印
		printf("pshm1: %s\n", pshm1);
	}

	pshm2 = (int*)shmat(shm_id2, NULL, 0);
	if(*pshm2 == -1){
		perror("shmat() failed");
		goto shm_test__failed;
	}
	else{
		//操作共享内存-写
		*pshm2 = 9999999;
		//读取'共享内存'数据, 并打印
		printf("pshm2: %d\n", *pshm2);
	}



	//4.当前进程, 释放shm 使用权
	if(shmdt(pshm1) == -1){
		perror("shmdt() failed");
		goto shm_test__failed;
	}
	if(shmdt(pshm2) == -1){
		perror("shmdt() failed");
		goto shm_test__failed;
	}
	//再次打印字符串, 此时已经消失了, 内存溢出
	//printf("print ’pshm1‘ again: %s\n", pshm1);
	//*pshm2 = 9999999;



shm_test__failed:
	//5.释放整个共享内存shm
	if(shmctl(shm_id1, IPC_RMID, NULL) == -1)
		perror("shmctl() failed");

	//同一个key_t 只需要释放一次即可.
	//同一个ket_t下,可以有多个shm 变量,多个shm_id,多个共享内存!!
	//if(shmctl(shm_id2, IPC_RMID, NULL) == -1)
		//perror("shmctl() failed");


	printf("shm_test() finish\n");
	return ;
}
