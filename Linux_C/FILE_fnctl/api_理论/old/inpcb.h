//111111111111111111111111111111111111111111111111111111111111111111111111
//格式测试:
//邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵
//起始日期:
//完成日期:
//************************************************************************
//修改日志:
//	2019-05-13: 新增'tty 文本标准格式'风格
//, , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , ,, , ,



//网文参考：http://blog.csdn.net/houjixin/article/details/51923919

//本文件主要讲述文件fd 的操作流程, 同样我们只关注数据存储的流程, 其它忽略。
//
//引言：
//由fd.c 我们得知文件fd 的总控制结构file{} 下面有有两个指针, 一个类型识别：
//  f_ops  ：通用fd 操作函数, 有5种：read,write,ioctl,select,close
//  f_data  ：文件数据实体
//  f_type  ：文件类型标识
//当f_type=DTYPE_SOCKET时, 则有f_data 指向下一层socket{}


/*
  --No.1
  socket{} 有一个类型识别, 一个指针：
  so_type: socket 类型的判别--TCP/UDP？SOCK_STREAM / SOCK_DGRAM
  so_pcb : 套接字的协议控制块-internet pcb, 指向下一层inpcb{}

  --No.2
  inpcb{} 是当前socket fd 支持的所有协议块的信息, 它是一个双向环形链表,
  支持多少种网络协议就有多少个inpcb{},
  so_pcb 指向inpcb{} 双向环形链表的首地址, 可更改协议, 支持增删查改, 同时减少内存消耗, 但增加了CPU 负担
  协议块并不是用数组固定的, inpcb{} 元素如下：
  inp_next    --下一个协议块
  inp_prev    --上一个协议块
  inp_faddr   --f 地址
  inp_fport   --f 端口
  inp_laddr   --l 地址
  inp_lport   --l 端口
  inp_socket  --回访指针, 返回上一层socket{} 实体

  --No.3
//当f_type=DTYPE_VNODE时, 则有f_data 指向下一层vnode{}
  此时的vnode{} 是实际的文件缓冲区, 类似哈希算法中的哈希桶




*/
