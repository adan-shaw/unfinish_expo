#1.<信号shell 操作>(tty前台进程)

#1.测试进程是否存在:
kill -0 6666

#2.强制杀死某个进程
kill -9 6666

#3.根据进程描述名, 杀死所有相关进程
killall top

#4.显示系统信号列表
kill -l



#2.<'作业进程'(后台进程)信号shell 操作>
#1.测试进程是否存在:
kill -0 %1 

#2.强制杀死某个进程
kill -9 %1 

