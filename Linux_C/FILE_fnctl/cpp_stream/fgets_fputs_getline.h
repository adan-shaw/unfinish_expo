//111111111111111111111111111111111111111111111111111111111111111111111111
//格式测试:
//邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵
//起始日期:
//完成日期:
//************************************************************************
//修改日志:
//	2019-05-13: 新增'tty 文本标准格式'风格
//, , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , ,, , ,


/*
一、gets()
	gets是个不安全的函数, 应该坚决不用.
	它每遇到换行符就会返回, 即使是一开始就遇到一个换行符.
	返回后会将换行符变为'\0'.
	由于gets()无法知道字符串的大小,
	遇到'\n 换行符'或'EOF文件结束符'才会结束,
	因此容易造成缓存溢出的安全性问题.

	C11标准已经不让用gets()了, 可以用fgets()来代替.
	比如对于同一个字符串str[50]：
	fgets()的用法是:
		fgets(str, 50, stdin);
	遇到'\n 换行符'/'EOF文件结束符'/50缓冲区溢出,都会截断结束fgets()
	但fgets() 没法判断'\0' 字符串结束符号.





二、fgets()
	char *fgets(char *buf, int bufsize, FILE *stream);
	遇到'\n 换行符'/'EOF文件结束符'/50缓冲区溢出,
	都会截断结束fgets(), 返回读到的字符串.

	如果是'\n换行符'终结fgets(),
	则‘\n’会被保存在缓冲区中, 只是后面再加个'\0'.
	对于fgets()来说,'\n'是一个特别的字符,而'\0'并无任何特别之处,
	读到'\0'就当作普通字符读入.

	如果文件中存在'\0'字符(或者0x00字节),
	fgets()就无法判断缓冲区中的'\0'究竟是‘从文件读上来的字符’,
	还是由fgets() 自动添加的结束符.

	所以fgets()只适合读'文本文件', 而不适合读'二进制文件',
	因为'文本文件'中的所有字符都应该是可见字符, 不能有'\0'.
	但是'二进制文件'并不理会'\0', '二进制文件'中没有特殊字符.


补充：glibc 提供的getline()函数：
<< getline 是依赖c++ ifstream 工作的函数,不用c++就不用getline >>
参考http://man7.org/linux/man-pages/man3/getline.3.html

可以读取不定长的“行”, 能正确处理'\n'/'\0'/EOF 的问题,
遇到'\n换行'/'\0字符串结束'/EOF文件结束, 都会结束返回.

但是它返回的是malloc()分配的内存, 需要用户自己free()掉缓冲区内存.
详情请看: fin_getline.cpp





三、scanf()
char str[100];
scanf("%s", str);
	如果scanf()在开头遇到前次函数留下的空格或换行符,
	scanf()一律跳过,继续运行;

	如果在中间遇到空格或遇到换行符, scanf()将会停止.
	在读取的字符串后加上'\0'.





四、puts与fputs
int puts(const char *str);
int fputs(const char *str, FILE *fp);//注意区别于fputc(c, fin).

fputs 向指定的文件写入一个字符串, puts 向标准输出写入一个字符串. 
两者都不会在结尾自动插入'\0', 所以首先, 你写入的字符串必须有结束符号.
否则程序可能会溢出.

fputs并不关心的字符串中的'\n'字符, 字符串中可以有'\n'也可以没有'\n'. 

puts将字符串str 写到标准输出, 然后自动写一个'\n'到标准输出. 





五、read是一个系统调用, 其他函数会调用该函数(最底层的读操作)
ssize_t read(int filedes, void *buf, size_t nbytes);
char data[100];
n = read(STDIN_FILENO, data, 100);

read 函数从 filedes 指定的已打开文件中读取 nbytes 字节到 buf 中. 
以下几种情况会导致读取到的字节数小于 nbytes ：
  A.读取普通文件时, 读到文件末尾还不够 nbytes 字节. 
    例如：如果文件只有 30 字节, 而我们想读取 100 字节, 
    那么实际读到的只有 30 字节, read 函数返回 30 . 
    此时再使用 read 函数作用于这个文件会导致 read 返回 0 . 
    (表明: read 读的时候, 文件光标也是在移动的.)
  B.从终端设备(terminal device)读取时, 一般情况下每次只能读取一行. 
  C.从网络读取时, 网络缓存可能导致读取的字节数小于 nbytes 字节. 
  D.读取 pipe 或者 FIFO 时, 
    pipe 或 FIFO 里的字节数可能小于 nbytes . 
  E.从面向记录(record-oriented)的设备读取时, 
    某些面向记录的设备(如磁带)每次最多只能返回一个记录. 
  F.在读取了部分数据时被信号中断. 

注意：read不会自动加上'\0'; 
     对于'\n', 它也是原封不动的读入, 当做普通字符. 





六、readline库
#include <readline/readline.h>
#include <readline/history.h>
char *line_read = (char *)NULL;
line_read = readline("Please Enter:");

readline 是一个强大的库, 只要使用了它的程序, 
都可以用同一个配置文件配置, 而且用同样的方法操作命令行, 
让你可以方便的编辑命令行. 





七、strlen 返回字符串的长度. 
该字符串可能是自己定义的, 也可能是内存中随机的, 
该函数实际完成的功能是从代表该字符串的第一个地址开始遍历, 
直到遇到结束符NULL. 返回的长度大小不包括NULL. 





八、C的关于打开、读取、关闭文件的api
std::string s;
FILE *fin=fopen("./test_data","r");
FILE *fout=fopen("./test_data","w");
while(fscanf(fin,"%c",&c)!=EOF)
{
	s+=c;
	fprintf(fout,"%s",s.c_str());
	fputc(c,fout);
}
fclose(fin);
fclose(fout);





九、C++部分

http://www.cnblogs.com/JCSU/articles/1190685.html

1、逐词读取, 词之间用空格区分

ifstream fin("data.txt");  //#include <fstream>
string s;                  //#include <string>
while( fin >> s )
{
  cout << "Read from file: " << s << endl;
}
fin.close(); //最后别忘了关闭


2、行读取, 将行读入字符数组, 行之间用回车换行区分

ifstream fin("data.txt");
const int LINE_LENGTH = 100;
char str[LINE_LENGTH];
//如果是终端, 就是cin.getline(str, LINE_LENGTH)
while( fin.getline(str,LINE_LENGTH) )
{ //默认遇到\n即结束读取, 最多读取99个字符, 并在结尾自动加\0.
  cout << "Read from file: " << str << endl;
}


3、逐行读取, 将行读入字符串, 行之间用回车换行区分

ifstream fin("data.txt");
string s;
//如果是终端, 就是getline(cin, s)
while( getline(fin,s) )
{ //注意这里s必须是string, 而不能是字符数组. s的末尾会自动有\0.
  cout << "Read from file: " << s << endl;
}

*/
