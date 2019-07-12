//111111111111111111111111111111111111111111111111111111111111111111111111
//格式测试:
//邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵
//起始日期:
//完成日期:
//************************************************************************
//修改日志:

//, , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , ,, , ,



//编译:
//g++ -o x SimpleIni.h ini_config.h ini_config.cpp -ggdb3

#include "SimpleIni.h"//SimpleIni库是一个纯c++实现的库
#include <stdbool.h>
#include <string>
#include <iostream>
using namespace std;



//1.目标结构体定义
//	本例子的目标, 就是要打开类似如下结构体的ini 文件, 读取配置信息
typedef struct global_val{
	//[server_info]
	string srv_ip;
	unsigned int srv_port;
	unsigned int test_count;
	unsigned int test_err_max;
	unsigned int pth_io_count;
	string log_path;//日志文件保存路径

	//[redis_info] - redis 数据库信息
	string redis_ip;
	unsigned int redis_port;
	unsigned int redis_dbkey;

	//[mysql_info] - mysql 数据库信息
	string mysql_ip;
	unsigned int mysql_port;
	string mysql_user;
	string mysql_pw;
	string mysql_dbname;
}G_val_t;



//2.内部API 实现

//2.1 通用类ini API, 例如:
//		打开目标文件之类的, 简单锁定file打开等简单直接操作,
//		这类API 是通用的
//****************************************
//*1.ini 文件/ini 字符串的’导入读取‘/’导出保存‘
//****************************************
// 装载一个ini file, 从file中读取ini 数据[load from a data file]
bool eopen_inifile(CSimpleIniA* psini, const char *file_path);

// 从string字符串中, 装载ini 数据[load from a string]
// 如果你已经读取了整个ini配置file的数据,
// 你可以将数据全部导入string字符串, 然后让CSimpleIniA类装载
bool eopen_inistring(CSimpleIniA* psini, string *str_data);

// 将ini数据保存到string缓冲区中[save the ini data to a string]
bool save_inidata2string(CSimpleIniA* psini, string* strData);

// 将ini数据保存到file中[save the ini data to a file]
bool save_inidata2file(CSimpleIniA* psini, const char* file_path);



//*****************************************
//*2.从CSimpleIniA 类实体中, 获取节点数据类API
//*****************************************
// 获取CSimpleIniA中‘所有的节点’[get all sections]
void get_all_sections(CSimpleIniA* psini,\
		CSimpleIniA::TNamesDepend* psections);

// 获取一个节点下‘所有的键’[get all keys in a section]
bool get_section_all_keys(CSimpleIniA* psini,\
		const char* section_name, CSimpleIniA::TNamesDepend* pkeys);

// 获取‘一个键下面的一个值’[get the value of a key]
bool get_a_key_val(CSimpleIniA* psini,const char* section_name,\
		const char* key, char* val_buf, int buf_len);

// 获取‘一个键下面的n 个值’;
// 如果bHasMultipleValues=true,则只有一个值返回.
// 与get_a_key_val()一样
// 一般情况下, 不用这个函数, 你不需要获取一个键, 多个值.
// 一般情况下, ini 文件都是一个键一个值
// 如果你需要返回多个值,则需要CSimpleIniA::TNamesDepend
// 这样的列表接住返回结果,然后再逐个处理.麻烦
// get the value of a key which may have multiple values.
// If bHasMultipleValues is true, then just one value has been returned
bool get_key_all_val2(CSimpleIniA* psini,const char* section_name,\
		const char* key, char* val_buf, int buf_len);

// 获取节点下的一个key的所有value,
// 用CSimpleIniA::TNamesDepend* pvalues值表返回数据.
// [get all values of a key with multiple values]
bool get_key_all_val(CSimpleIniA* psini, const char* section_name,\
		const char* key, CSimpleIniA::TNamesDepend* pvalues);



//********************************
//*3.向CSimpleIniA 类中添加数据类API
//********************************
// 添加一个空节点到CSimpleIniA 类中[adding a new section]
// 没有指定'key键'和'value值', 少用.
bool add_section(CSimpleIniA* psini, const char* section_name);

// 添加一个新节点到CSimpleIniA 类中,并填充正确的值[adding a new section]
// 添加key, 每个key 必然带一个值, 尽量不要为空值!!
// adding a new key：
// new-section" will be added automatically if it doesn't already exist.
bool add_section_key_val(CSimpleIniA* psini, const char* section_name,\
		const char* key_name, const char* val_name);



//********************************
//*4.从CSimpleIniA 类中删除数据类API
//********************************
// 删除一个节点下面的一个key 数据, 的一个value 的数据!
// deleting a key with a value from a section.
// Optionally the entire section may be deleted if it is now empty.
bool del_a_val(CSimpleIniA* psini, const char* section_name,\
		const char* key_name, const char* val_name);

// 删除一个节点下面的一个key 数据(无论有多少个value,直接删除)
// deleting a key with any value from a section.
bool del_a_key(CSimpleIniA* psini, const char* section_name,\
		const char* key_name);

// [删除一个section]deleting an entire section and all keys in it.
bool del_a_section(CSimpleIniA* psini, const char* section_name);



//********************************
//*5.CSimpleIniA 类杂项API
//********************************
//清空CSimpleIniA中'所有的装载数据'
void reset_all(CSimpleIniA* psini);

//查看CSimpleIniA 实体容器是否为空
bool check_empty(CSimpleIniA* psini);

// 打印输出一个'pvalues 值集'下的所有值[output all of the items]
void printf_val_set(CSimpleIniA::TNamesDepend* pvalues);





//2.2 特异类ini API, 就像cjson 一样, 要根据具体目标结构体变化的API

//装载过程ini_config_control_func()的简要说明:
/*
	根据文件路径装载一个ini file
	(const char *a_pszFile, bool a_bIsUtf8,\
		bool a_bUseMultiKey,bool a_bUseMultiLine)
	注意参数:
		IsUtf8,UseMultiKey,UseMultiLine均为bool类型,上层是cpp写的.
		兼容false = 0; true = 1
		IsUtf8 表示ini file 是否是UTF-8 编码格式
		UseMultiKey 表示是否存在多键的情况??默认为true
		UseMultiLine 表示是否存在多行的情况??默认为true
*/
//装载过程控制函数--如果你修改了ini file, 则只需修改这个函数
bool ini_config_control_func(G_val_t* pg_val);





