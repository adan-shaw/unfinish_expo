//111111111111111111111111111111111111111111111111111111111111111111111111
//格式测试:
//邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵
//起始日期:
//完成日期:
//************************************************************************
//修改日志:

//, , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , ,, , ,


/*

使用socket实现进程间通信：(UNIX domain中面向连接通信)
	使用套接字除了可以实现网络间不同主机间的通信外,
	还可以实现同一主机的不同进程间的通信, 且建立的通信是双向的通信.

man unix内容如下：
NAME(名称)
	PF_UNIX,AF_UNIX,PF_LOCAL,AF_LOCAL(用于本地内部进程通讯的套接字)


SYNOPSIS( 总览 )
	#include <sys/socket.h>
	#include <sys/un.h>

	unix_socket = socket(PF_UNIX, type, 0);
	error = socketpair(PF_UNIX, type, 0, int *sv);


DESCRIPTION( 描述 )
	PF_UNIX(也称作PF_LOCAL)套接字族用来在同一机器上的提供有效的进程间通讯.
	Unix 套接字可以是匿名的(由socketpair(2)创建),
	也可以与套接字类型文件相关联.
	Linux 还支持一种抽象名字空间, 它是独立于文件系统的.
	有效的类型有:
		SOCK_STREAM 用于面向流的套接字,
		SOCK_DGRAM 用于面向数据报的套接字,其可以保存消息界限.
		Unix	套接字总是可靠的, 而且不会重组数据报.
		Unix	套接字支持把文件描述符, 或'进程的信用证明'
					作为'数据报的辅助数据'传递给其它进程．


ADDRESS FORMAT( 地址格式 )
	*unix 地址定义为文件系统中的一个文件名,
		或者抽象名字空间中的一个单独的字符串.
	*由socketpair(2)创建的套接字是匿名的.
	*对于非匿名的套接字,目标地址可使用connect(2)设置.
	*本地地址可使用bind(2)设置.
	*当套接字连接上而且它没有一个本地地址时,
		会自动在抽象名字空间中生成一个唯一的地址.
*/
//#define UNIX_PATH_MAX	 108

//struct sockaddr_un {
//	sa_family_t	sun_family;				/* AF_UNIX */
//	char	sun_path[UNIX_PATH_MAX];/* 路径名 */
//};
/*
	sun_family 总是包含 AF_UNIX.
	sun_path 包含空零结尾的套接字在文件系统中的路径名.
	如果sun_path以空零字节开头,
	它指向由 Unix 协议模块维护的抽象名字空间.
	该套接字在此名字空间中的地址由sun_path中的剩余字节给定.
	注意抽象名字空间的名字都不是空零终止的.


SOCKET OPTIONS( 套接字选项 )
	由于历史原因,这些套接字选项通过SOL_SOCKET类型确定,
	即使它们是PF_UNIX 指定的.
	它们可以由 setsockopt(2) 设置.
	通过指定SOL_SOCKET作为套接字族用 getsockopt(2) 来读取.
	SO_PASSCRED 允许接收进程辅助信息发送的信用证明.
	当设置了该选项且套接字尚未连接时,
	则会自动生成一个抽象名字空间的唯一名字, 值为一个整数布尔标识.


ANCILLARY MESSAGES( 辅助信息 )
	由于历史原因, 这些辅助信息类型通过SOL_SOCKET类型确定,
	即使它们是PF_UNIX指定的.
	要发送它们, 可设置结构cmsghdr的cmsg_level字段为SOL_SOCKET,
	并设置cmsg_type 字段为其类型.
	要获得更多信息, 请参看cmsg(3).
	SCM_RIGHTS
		为其他进程发送或接收一套打开文件描述符.
		其数据部分包含一个文件 描述符的整型数组.
		已传文件描述符的效果就如它们已由dup(2)创建过一样.
	SCM_CREDENTIALS
		发送或者接收 unix 信用证明.可用作认证.
		信用证明传送 以 struct ucred 辅助信息的形式传送．
		struct ucred {
			pid_t	 pid;		// 发送进程的进程标识
			uid_t	 uid;		// 发送进程的用户标识
			gid_t	 gid;		// 发送进程的组标识
		};
		发送者确定的信用证明由内核检查.
		一个带有有效用户标识0的进程允许指定不与其自身值相匹配的值.
		发送者必须确定其自身的进程标识(除非它带有CAP_SYS_ADMIN),
		其用户标识,有效用户标识或者设置用户标识(除非它带有CAP_SETUID),
		以及其组标识,有效组标识或者设置组标识(除非它带有CAP_SETGID).
		为了接收一条struct ucred消息,必须在套接字上激活SO_PASSCRED选项.


ERRORS( 错误 )
	ENOMEM
		内存溢出.
	ECONNREFUSED
		connect(2) 调用了一个未在监听的套接字对象.
		这可能发生在远程套 接字不存在或者文件名不是套接字的时候.
	EINVAL
		传递了无效参数.
		通常的产生原因是已传地址的sun_type字段的AF_UNIX设置丢失,
		或者套接字对应用的操作处于无效状态.
	EOPNOTSUPP
		在非面向流的套接字上调用了流操作,或者试图使用出界的数据选项.
	EPROTONOSUPPORT
		传递的协议是非PF_UNIX的.
	ESOCKTNOSUPPORT
		未知的套接字类型.
	EPROTOTYPE
		远程套接字与本地套接字类型不匹配(SOCK_DGRAM对SOCK_STREAM).
	EADDRINUSE
		选择的本地地址已经占用,或者文件系统套接字对象已经存在.
	EISCONN
		在一个已经连接的套接字上调用connect(2),
		或者指定的目标地址在一个已连接的套接字上.
	ENOTCONN
		套接字操作需要一个目的地址,但是套接字尚未连接.
	ECONNRESET
		远程套接字意外关闭.
	EPIPE
		远程套接字在一个流套接字上关闭了.
		如果激活,会同时发送一个SIGPIPE标识.
		这可以通过传递MSG_NOSIGNAL标识给sendmsg(2)或者recvmsg(2)来避免.
	EFAULT
		用户内存地址无效.
	EPERM
		发送者在 struct ucred 中传递无效的信用证明.
		当生成一个文件系统套接字对象时,
		可能会由通用套接层或者文件系统产生其它错误.
		要获得更多信息,可参见合适的手册页.



实践：
	使用套接字在UNIX域内实现进程间通信的服务端程序.
	首先, 程序通过调用socket函数, 建立了监听连接的套接字,
	然后调用bind函数, 将套接字与地址信息关联起来.
	调用listen函数实现对该端口的监听,
	当有连接请求时, 通过调用accept函数建立与客户机的连接,
	最后, 调用read函数来读取客户机发送过来的消息,
	当然也可以使用recv函数实现相同的功能.
