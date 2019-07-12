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

#include "ini_config.h"



//****************************************
//*1.ini 文件/ini 字符串的’导入读取‘/’导出保存‘
//****************************************
// 装载一个ini file, 从file中读取ini 数据[load from a data file]
bool eopen_inifile(CSimpleIniA* psini, const char *file_path){
	SI_Error rc = psini->LoadFile(file_path);
	return rc<0 ? false : true;
}

// 从string字符串中, 装载ini 数据[load from a string]
// 如果你已经读取了整个ini配置file的数据,
// 你可以将数据全部导入string字符串, 然后让CSimpleIniA类装载
bool eopen_inistring(CSimpleIniA* psini, string *str_data){
	SI_Error rc = psini->LoadData(str_data->c_str(), str_data->size());
	return rc<0 ? false : true;
}

// 将ini数据保存到string缓冲区中[save the ini data to a string]
bool save_inidata2string(CSimpleIniA* psini, string* strData){
	SI_Error rc = psini->Save(*strData);
	return rc<0 ? false : true;
}

// 将ini数据保存到file中[save the ini data to a file]
bool save_inidata2file(CSimpleIniA* psini, const char* file_path){
	SI_Error rc = psini->SaveFile(file_path);
	return rc<0 ? false : true;
}



//*****************************************
//*2.从CSimpleIniA 类实体中, 获取节点数据类API
//*****************************************
// 获取CSimpleIniA中‘所有的节点’[get all sections]
void get_all_sections(CSimpleIniA* psini,\
		CSimpleIniA::TNamesDepend* psections){
	psini->GetAllSections(*psections);
}

// 获取一个节点下‘所有的键’[get all keys in a section]
bool get_section_all_keys(CSimpleIniA* psini,\
		const char* section_name, CSimpleIniA::TNamesDepend* pkeys){
	return psini->GetAllKeys(section_name, *pkeys);
}

// 获取‘一个键下面的一个值’[get the value of a key]
bool get_a_key_val(CSimpleIniA* psini,const char* section_name,\
		const char* key, char* val_buf, int buf_len){
	const char* tmp = psini->GetValue(section_name, key, NULL);

	if(tmp == NULL)
		return false;

	strncpy(val_buf, tmp, buf_len);
	return true;
}

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
		const char* key, char* val_buf, int buf_len){
	bool bHasMultipleValues = false;//尝试获取多个值
	const char* tmp = psini->GetValue(section_name,\
			key, NULL, &bHasMultipleValues);

	if(tmp == NULL)
		return false;

	if(bHasMultipleValues != true)//一个键下面,有多个值返回
		return false;//当成失败,不允许一个键多个值
								 //如果你需要返回多个值,则需要CSimpleIniA::TNamesDepend
								 //这样的列表接住返回结果,然后再逐个处理.麻烦

	strncpy(val_buf, tmp, buf_len);
	return true;
}

// 获取节点下的一个key的所有value,
// 用CSimpleIniA::TNamesDepend* pvalues值表返回数据.
// [get all values of a key with multiple values]
bool get_key_all_val(CSimpleIniA* psini, const char* section_name,\
		const char* key, CSimpleIniA::TNamesDepend* pvalues){
	if(psini->GetAllValues(section_name, key, *pvalues)){
		// sort the values into the original load order
		#if defined(_MSC_VER) && _MSC_VER <= 1200
			pvalues->sort();//STL of VC6 doesn't allow me to \
												specify my own comparator for list::sort()
		#else
			pvalues->sort(CSimpleIniA::Entry::LoadOrder());
		#endif
		return true;
	}
	else
		return false;
}



//********************************
//*3.向CSimpleIniA 类中添加数据类API
//********************************
// 添加一个空节点到CSimpleIniA 类中[adding a new section]
// 没有指定'key键'和'value值', 少用.
bool add_section(CSimpleIniA* psini, const char* section_name){
	SI_Error rc = psini->SetValue(section_name, NULL, NULL);
	bool tmp = rc<0 ? false : true;
	printf("section: %s\n",rc==SI_INSERTED ? "inserted" : "updated");
	return tmp;
}

// 添加一个新节点到CSimpleIniA 类中,并填充正确的值[adding a new section]
// 添加key, 每个key 必然带一个值, 尽量不要为空值!!
// adding a new key：
// new-section" will be added automatically if it doesn't already exist.
bool add_section_key_val(CSimpleIniA* psini, const char* section_name,\
		const char* key_name, const char* val_name){
	SI_Error rc = psini->SetValue(section_name, key_name, val_name);
	bool tmp = rc<0 ? false : true;
	printf("section: %s\n",rc==SI_INSERTED ? "inserted" : "updated");
	return tmp;
}



//********************************
//*4.从CSimpleIniA 类中删除数据类API
//********************************
// 删除一个节点下面的一个key 数据, 的一个value 的数据!
// deleting a key with a value from a section.
// Optionally the entire section may be deleted if it is now empty.
bool del_a_val(CSimpleIniA* psini, const char* section_name,\
		const char* key_name, const char* val_name){
	//第四参数=true, 表示删除后, 如果节点一个值都不剩了, 自动删除节点.
	//delete the section if empty = true
	return psini->DeleteValue(section_name, key_name, val_name, true);
}

// 删除一个节点下面的一个key 数据(无论有多少个value,直接删除)
// deleting a key with any value from a section.
bool del_a_key(CSimpleIniA* psini, const char* section_name,\
		const char* key_name){
	//第四参数=true, 表示删除后, 如果节点一个值都不剩了, 自动删除节点.
	//delete the section if empty = true
	return psini->Delete(section_name, key_name, true);
}

// [删除一个section]deleting an entire section and all keys in it.
bool del_a_section(CSimpleIniA* psini, const char* section_name){
	return psini->Delete(section_name, NULL);
}



//********************************
//*5.CSimpleIniA 类杂项API
//********************************
//清空CSimpleIniA中'所有的装载数据'
void reset_all(CSimpleIniA* psini){
	//~CSimpleIniTempl() 释构函数自动调用, 只管删除类即可
	psini->Reset();
}

//查看CSimpleIniA 实体容器是否为空
bool check_empty(CSimpleIniA* psini){
	return psini->IsEmpty();//可以检查一下类是否初始化成功
}

// 打印输出一个'pvalues 值集'下的所有值[output all of the items]
void printf_val_set(CSimpleIniA::TNamesDepend* pvalues){
	CSimpleIniA::TNamesDepend::const_iterator i;
	int count = 1;



	printf("printing a 'pvalues set' all value:\n");
	for(i = pvalues->begin(); i != pvalues->end(); ++i){
		printf("p%d = '%s'\n", count, i->pItem);
		count++;
	}
}









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
bool ini_config_control_func(G_val_t* pg_val){
	//创建simple_ini 类, 初始化为(UTF-8, 允许MultiKey, 允许MultiLine)
	CSimpleIniA sini(true, true, true);
	CSimpleIniA::TNamesDepend sections;	//节点容器
	CSimpleIniA::TNamesDepend keys;			//键容器
	CSimpleIniA::TNamesDepend values;		//值容器
	CSimpleIniA::TNamesDepend::const_iterator i;//节点迭代器
	CSimpleIniA::TNamesDepend::const_iterator k;
	CSimpleIniA::TNamesDepend::const_iterator v;
	int section_count = 0;
	int key_count = 0;
	int value_count = 0;



	//********************************************
	//simple_ini 遍历方案: 遍历所有节点, key, value
	//********************************************

	//1.打开ini file, 装载数据到CSimpleIniA中
	if(!eopen_inifile(&sini,"./target_data_demo.ini"))
		return false;

	//2.获取CSimpleIniA中所有的section到'节点容器'中
	get_all_sections(&sini,&sections);


	//3.遍历所有的section
	for(i = sections.begin(); i != sections.end(); ++i){

		//3.1: 匹配节点是否是'server_info key'节点
		//			是则为'server_info key'节点提取数据, 不是则passed
		if(strstr(i->pItem,"server_info")!= NULL){
			if(get_section_all_keys(&sini,i->pItem,&keys)){//获取该节点下所有的key
				for(k = keys.begin(); k != keys.end(); ++k){//遍历所有的key
					if(strstr(k->pItem,"srv_ip")!= NULL){
						//获取key下所有的value
						if(get_key_all_val(&sini,i->pItem,k->pItem,&values)){
							v = values.begin();//只取一个值
							pg_val->srv_ip = v->pItem;
						}
					}
					if(strstr(k->pItem,"srv_port")!= NULL){
						if(get_key_all_val(&sini,i->pItem,k->pItem,&values)){
							v = values.begin();
							pg_val->srv_port = atoi(v->pItem);
						}
					}
					if(strstr(k->pItem,"test_count")!= NULL){
						if(get_key_all_val(&sini,i->pItem,k->pItem,&values)){
							v = values.begin();
							pg_val->test_count = atoi(v->pItem);
						}
					}
					if(strstr(k->pItem,"test_err_max")!= NULL){
						if(get_key_all_val(&sini,i->pItem,k->pItem,&values)){
							v = values.begin();
							pg_val->test_err_max = atoi(v->pItem);
						}
					}
					if(strstr(k->pItem,"thread_count")!= NULL){
						if(get_key_all_val(&sini,i->pItem,k->pItem,&values)){
							v = values.begin();
							pg_val->pth_io_count = atoi(v->pItem);
						}
					}
					if(strstr(k->pItem,"logger_path")!= NULL){
						if(get_key_all_val(&sini,i->pItem,k->pItem,&values)){
							v = values.begin();
							pg_val->log_path = v->pItem;
						}
					}
				}
			}
		}


		//3.2: 匹配节点是否是'redis_info key'节点
		//			是则为'redis_info key'节点提取数据, 不是则passed
		if(strstr(i->pItem,"redis_info")!= NULL){
			if(get_section_all_keys(&sini,i->pItem,&keys)){//获取该节点下所有的key
				for(k = keys.begin(); k != keys.end(); ++k){//遍历所有的key
					if(strstr(k->pItem,"redis_ip")!= NULL){
						if(get_key_all_val(&sini,i->pItem,k->pItem,&values)){
							v = values.begin();//只取一个值
							pg_val->redis_ip = v->pItem;
						}
					}
					if(strstr(k->pItem,"redis_port")!= NULL){
						if(get_key_all_val(&sini,i->pItem,k->pItem,&values)){
							v = values.begin();
							pg_val->redis_port = atoi(v->pItem);
						}
					}
					if(strstr(k->pItem,"redis_dbkey")!= NULL){
						if(get_key_all_val(&sini,i->pItem,k->pItem,&values)){
							v = values.begin();
							pg_val->redis_dbkey = atoi(v->pItem);
						}
					}
				}
			}
		}


		//3.3：匹配节点是否是'mysql_info key'节点
		//			是则为'mysql_info key'节点提取数据, 不是则passed
		if(strstr(i->pItem,"mysql_info")!= NULL){
			if(get_section_all_keys(&sini,i->pItem,&keys)){//获取该节点下所有的key
				for(k = keys.begin(); k != keys.end(); ++k){//遍历所有的key
					if(strstr(k->pItem,"mysql_ip")!= NULL){
						//获取key下所有的value
						if(get_key_all_val(&sini,i->pItem,k->pItem,&values)){
							v = values.begin();//只取一个值
							pg_val->mysql_ip = v->pItem;
						}
					}
					if(strstr(k->pItem,"mysql_port")!= NULL){
						if(get_key_all_val(&sini,i->pItem,k->pItem,&values)){
							v = values.begin();
							pg_val->mysql_port = atoi(v->pItem);
						}
					}
					if(strstr(k->pItem,"mysql_user")!= NULL){
						if(get_key_all_val(&sini,i->pItem,k->pItem,&values)){
							v = values.begin();
							pg_val->mysql_user = v->pItem;
						}
					}
					if(strstr(k->pItem,"mysql_pw")!= NULL){
						if(get_key_all_val(&sini,i->pItem,k->pItem,&values)){
							v = values.begin();
							pg_val->mysql_pw = v->pItem;
						}
					}
					if(strstr(k->pItem,"mysql_dbname")!= NULL){
						if(get_key_all_val(&sini,i->pItem,k->pItem,&values)){
							v = values.begin();
							pg_val->mysql_dbname = v->pItem;
						}
					}
				}
			}
		}

		//3.4: 无论该节点是否与上面的目标节点名匹配,
		//			统一遍历打印'已经存在的节点'
		if(get_section_all_keys(&sini,i->pItem,&keys)){

			//遍历所有的key
			for(k = keys.begin(); k != keys.end(); ++k){

				printf("--键%d = %s\n", key_count, k->pItem);//打印key名

				//获取key下所有的value
				if(get_key_all_val(&sini,i->pItem,k->pItem,&values)){

					//遍历所有的value
					for(v = values.begin(); v != values.end(); ++v){
						printf(">>>>值%d = %s\n", value_count, v->pItem);//打印value
						value_count++;//统计value个数
					}

				}
				key_count++;//统计key个数

			}

		}


		section_count++;//统计section个数
		printf("\n\n");//每个节点产生一个换行
	}



	//打印统计数据
	printf("\n\nsection_count = %d,key_count = %d,value_count = %d.\n",\
			section_count, key_count, value_count);
	return true;
}




int main(void){
	G_val_t g_val;



	if(!ini_config_control_func(&g_val))
		printf("ini_config_control_func() failed !!\n");

	return 0;
}
