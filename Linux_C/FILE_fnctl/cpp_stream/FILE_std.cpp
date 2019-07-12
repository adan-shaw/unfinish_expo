//111111111111111111111111111111111111111111111111111111111111111111111111
//格式测试:
//邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵
//起始日期:
//完成日期:
//************************************************************************
//修改日志:
//	2019-05-13: 新增'tty 文本标准格式'风格
//, , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , ,, , ,

//编译:
//g++ -o x ./std_stream.cpp -ggdb3


//1.'标准FILE*文件流'简述:
/*
stdin,stdout,stderr是进程创建的时候,就会自动创建的FILE*.
stdin,stdout,stderr都是FILE*属性的文件流,其中:
(
1.0:
	'标准FILE*文件流'和普通文件流一样,可读可写,但是读写不一定有效:
		stdin 可读不可写,
		stdout 可写不可读,
		stderr 可写不可读,
		stdin,stdout,stderr都不可以seek.

	'标准FILE*文件流'设置异步模式, 不知道是否有效, 但是没有必要.
	'标准FILE*文件流'是内存FILE*, 速度这么快, 不需要异步.


1.1:
	stderr 已经被改造, 变为每个线程都有一个.
	stderr 不再是进程独占, 多线程共享的局面了. 而是线程各自独占.
	一般只用作: fwrite()写入; 从来不fread()读取,


1.2: 标准FILE*文件流的长度:
	无论你怎样读取stdin,stdout,stderr文件流的长度,
	都只会返回-1, 操作失败!.
	(亲测试: fscanf, fgets, fseek, ftell 全部都是返回1.)
	sprintf("xxxx","%s",stderr); 会内存溢出, 因为其长度为1啊.


1.3：标准FILE*文件流的读写方式:
	标准FILE*文件流, 只能通过cout / cin 操作？

	其实,fwrite()/fread()FILE*文件流'同步'操作,还是支持的.
	但是不需要'异步'操作!!
	因为内存的速度十分快, 并没有异步的需要.
	你不需要将'标准FILE*文件流'修改为异步模式, 这是一个愚蠢的做法!

	标准FILE*文件流, '冲刷读写法'/'截断读写法':
		用截断和冲刷, 效果都是一样的, example:
		*1.截断法(std out 输出截断)
			std::cout<<"test"<<std::endl;

		*2.冲刷法(std out 输出冲刷)
			std::cout<<"test";
			fflush(stdout);

			//默认冲刷'当前进程'的所有流:stdin,stdout,自己打开的流,等.
			fflush();


1.4:
	fread()是阻塞的,fwrite()也是阻塞的,都是'同步'操作.
	如果用fread()读取stderr/stdout,你的程序就会陷入阻塞.
	因为你没办法从stderr,stdout里面输入字符,所以就会一直阻塞等待字符串.

	用fwrite()写入stdin,你的程序就会陷入阻塞.


速度stdin 很难读到换行符号.
只会按照buf 缓冲区长度读取, 不会遇到'\n' 就停止读取.


1.5:
	由于'标准FILE*文件流'已经有FILE* 缓冲区,
	所以'标准FILE*文件流'并没有截断函数, 也不需要!
	流概念, 就是用缓冲区来先'接住'数据, 再统一io.
	本身这个缓冲区, 就承担了数据溢出的风险.
	FILE* 在发生缓冲区溢出的情况的时候, 会先自动flush() 数据,
	再继续写入.
*/


#include <iostream>
#include <stdio.h>
#include <string.h>



//2.'标准FILE*文件流'demo:
void std_stream_demo(void){
	char buf[256];
	int tmp;



	//1.1:stderr fwrite() 测试
	strncpy(buf,"stderr fwrite() 测试\n",256);
	fwrite(buf,sizeof(char),strlen(buf)+1,stderr);

	fprintf(stderr,"%s\n%s\n",\
			"fprintf() to stderr1","fprintf() to stderr2");

	fwrite("stderr test3\n\n",sizeof(char),\
			strlen("stderr test3\n\n")+1,stderr);


	//fread() 阻塞读取5字节测试
	memset(buf,'\0',256);
	fread(buf,sizeof(char),5,stdin);



	tmp = fwrite(buf,sizeof(char),strlen(buf)+1,stdin);


	//1.2: 标准FILE*文件流的长度:
	printf("ftell(stderr)=%d\n",ftell(stderr));
	perror("ftell(stderr)");

	printf("fseek(stdout,10,0)=%d\n",\
			fseek(stdout,10,0));//向后移动10字节
	perror("fseek(stdout,10,0)");
/*
	printf("ftell(stderr)=%d\n",ftell(stderr));
	printf("ftell(stderr)=%d\n",ftell(stderr));
		无论你怎样读取stdin,stdout,stderr文件流的长度, 都只会是1.
		(亲测试: fscanf, fgets, fseek, ftell 全部都是返回1.)
		sprintf("xxxx","%s",stderr); 会内存溢出, 因为其长度为1啊.

/*
	1.3：标准FILE*文件流的读写方式:
		标准FILE*文件流, 只能通过cout / cin 操作？

		其实,fwrite()/fread()FILE*文件流'同步'操作,还是支持的.
		但是不需要'异步'操作!!
		因为内存的速度十分快, 并没有异步的需要.
		你不需要将'标准FILE*文件流'修改为异步模式, 这是一个愚蠢的做法!

		标准FILE*文件流, '冲刷读写法'/'截断读写法':
			用截断和冲刷, 效果都是一样的, example:
			*1.截断法(std out 输出截断)
				std::cout<<"test"<<std::endl;

			*2.冲刷法(std out 输出冲刷)
				std::cout<<"test";
				fflush(stdout);

				//默认冲刷'当前进程'的所有流:stdin,stdout,自己打开的流,等.
				fflush();
*/
}



int main(void){
	//1.'标准FILE*文件流'demo测试
	std_stream_demo();

	return 0;
}
