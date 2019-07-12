//111111111111111111111111111111111111111111111111111111111111111111111111
//格式测试:
//邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵
//起始日期:
//完成日期:
//************************************************************************
//修改日志:

//, , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , ,, , ,

//编译:
//g++ -o x ./cjson_struct.cpp -ggdb3 -lcjson



/*
(这里的错误就是: 字符串取出的方式, 是不能用回调的!! 只能用strncpy!!
	字符串不是整形!! 缓冲区一般都需要memset() or strncpy()
	只有int 整形, 才可以回调. 这就是为什么会迷之错误！！)
*/



//cjson api 解析:
/*
1.cJSON  结构体如下：
typedef struct cJSON{
	struct cJSON *next,*prev;	//'前向或后向'的链表指针
	struct cJSON *child;			//子节点
	int type;									//'本节点'的类型
	char *valuestring;				//类型-字符串-值
	int valueint;							//类型-整数-值
	double valuedouble;				//类型-浮点数-值
	char *string;							//'本节点'的名字
}cJSON;
说明：
	1、cJSON{}是使用链表来存储数据的,其访问方式很像一颗树.
		每个节点, 可以通过next/prev指针来查找'同级节点', 它类似双向链表;
		每个节点, 也可以有孩子节点, 通过child指针来访问, 进入下一层.
		只有节点是'Object对象'或'Array数组'时, 才可以有孩子节点.

	2、type是键(key)的类型, 一共有7种取值, 分别是：
		False, Ture, NULL, Number, String, Array, Object.
		若是Number类型, 则valueint或valuedouble中存储着值.
			如果期望Number的是int, 则访问valueint,
			如果期望Number的是double, 则访问valuedouble.
		若是String类型的, 则valuestring中存储着值.
		若是False/Ture/NULL类型的, 则valuestring中存储着值.

	3、string中存放的是这个’节点的名字‘, 可理解为key的名称.





2.cjson 解析类API：
	1.cJSON *cJSON_Parse(const char *value);
	参数:
		解析参数'json字符串'.
	返回值:
		返回cJSON结构体(整个cJSON Object树--对json字符串的第一步解析).
		(返回的cJSON结构体, 用完之后需要用cJSON_Delete()释放)



	2.cJSON *cJSON_GetObjectItem(cJSON *object,const char *string);
		提取一个节点, 返回这个节点的cJSON结构体.
		(这个节点的类型, 可能是:
			False, Ture, NULL, Number, String, Array, Object.
			因此, 节点并不一定代表结束节点, 也有可能是中间节点)
	参数：
		*objec：需要提取节点的集合体(Object).
						[ps:你也不知道这个集合体中, 有多少个同级节点,
								也不知道具体每个节点下面, 还有字节点(即节点深度)]
		string：’节点的名字‘, 可理解为key的名称.
	返回值：'本节点'的cJSON结构体, 但不需要释放! 因为:
					这里只是指向上一步cJSON_Parse()解析出来的实体的区间位置而已.



	3.cJSON *cJSON_GetArrayItem(cJSON *array,int item);
		当你确定, 这个节点是一个Array数组时,
		你需要遍历这个数组的时候, 就可以用cJSON_GetArrayItem().
		cJSON_GetArrayItem()类似于cJSON_GetObjectItem(),
		只不过cJSON_GetObjectItem()是通用的,cJSON_GetArrayItem()是特殊的.
	参数:
		*array: Array数组cJSON结构体
		item: 想要获取的数组下标
	返回值:数组下标指向的元素, 的'cJSON结构体'节点.
				(然后你就可以通过这个数组元素的'cJSON结构体',访问到数组中的元素值,
					例如: 'cJSON结构体'->valueint )



	4.int cJSON_GetArraySize(const cJSON *array);
		获取数组的长度, 返回一个int 值.



	5.cJSON_Delete()
		用来释放cJSON_Parse()产生的实体, 释放整个实体内存.
		cJSON_Parse()生成实体, 是自动生成的.



	6.cJSON 节点类型判断类API:
		(cJSON_bool) cJSON_IsInvalid(const cJSON*const item);
		(cJSON_bool) cJSON_IsFalse(const cJSON*const item);
		(cJSON_bool) cJSON_IsTrue(const cJSON*const item);
		(cJSON_bool) cJSON_IsBool(const cJSON*const item);
		(cJSON_bool) cJSON_IsNull(const cJSON*const item);
		(cJSON_bool) cJSON_IsNumber(const cJSON*const item);
		(cJSON_bool) cJSON_IsString(const cJSON*const item);
		(cJSON_bool) cJSON_IsArray(const cJSON*const item);
		(cJSON_bool) cJSON_IsObject(const cJSON*const item);
		(cJSON_bool) cJSON_IsRaw(const cJSON*const item);





3.cjson 组装类API:
3.1 注意:
	组装好整个root cjson 树之后, 得到的cJSON 结构, 只需释放root 树根即可.
	其它'子节点cJSON 结构', 会被自动释放.



3.2 创建类:
	//非赋值创建cJSON 节点结构体(空节点, 需要自己填充值!)
	cJSON* cJSON_CreateNull(void);
	cJSON* cJSON_CreateArray(void);//创建空数组
	cJSON* cJSON_CreateObject(void);//创建空Object
	cJSON* cJSON_CreateTrue(void);//创建空bool ?
	cJSON* cJSON_CreateFalse(void);//创建空bool ?

	//常量赋值创建cJSON 节点结构体
	cJSON* cJSON_CreateBool(cJSON_bool boolean);
	cJSON* cJSON_CreateNumber(double num);
	cJSON* cJSON_CreateString(const char *string);
	cJSON* cJSON_CreateRaw(const char *raw);

	//’数组赋值‘, 创建cJSON 节点结构体(第二参数是指明'数组长度')
	cJSON* cJSON_CreateIntArray(const int *numbers, int count);
	cJSON* cJSON_CreateFloatArray(const float *numbers, int count);
	cJSON* cJSON_CreateDoubleArray(const double *numbers, int count);
	cJSON* cJSON_CreateStringArray(const char **strings, int count);



3.3 插入类:
	//将(已经初始化完毕的)’cJSON节点/数组‘插入到’root cJSON‘中
	void cJSON_AddItemToArray(cJSON *array,cJSON *item);
	void cJSON_AddItemToObject(cJSON*object,const char*string,cJSON*item);


	//将常量插入到’root cJSON‘中, 并自动填充’cJSON结构体‘
	cJSON* cJSON_AddBoolToObject(\
		cJSON *const object,const char *const name,const cJSON_bool boolean);
	cJSON* cJSON_AddNumberToObject(\
		cJSON *const object,const char *const name,const double number);
	cJSON* cJSON_AddStringToObject(\
		cJSON *const object,const char *const name,const char *const string);
	cJSON* cJSON_AddRawToObject(\
		cJSON *const object,const char *const name,const char *const raw);


	//插入’cJSON结构体‘空节点, 并命名节点名, 但未赋值(未初始化):
	cJSON* cJSON_AddNullToObject(const cJSON* object,const char *name);
	cJSON* cJSON_AddTrueToObject(const cJSON* object,const char *name);
	cJSON* cJSON_AddFalseToObject(const cJSON* object,const char *name);
	cJSON* cJSON_AddObjectToObject(const cJSON* object,const char *name);
	cJSON* cJSON_AddArrayToObject(const cJSON* object,const char *name);



3.4 分离/删除类:(分离不等于删除, 删除即释放内存, 分离不会释放内存)
	//根据数组元素'cJSON结构体'指针,将数组元素'cJSON结构体'从Array节点中分离
	cJSON* cJSON_DetachItemViaPointer(cJSON *parent, cJSON*const item);

	//根据数组元素下标,将数组元素'cJSON 结构体'从Array节点中分离
	cJSON* cJSON_DetachItemFromArray(cJSON *array, int which);

	//根据数组元素下标,将数组元素'cJSON 结构体'从Array节点中删除
	void cJSON_DeleteItemFromArray(cJSON *array, int which);


	//根据节点名称,将'Object节点''cJSON 结构体'从Object root节点中分离
	cJSON* cJSON_DetachItemFromObject(cJSON *object, const char *string);
	cJSON* cJSON_DetachItemFromObjectCaseSensitive(\
		cJSON *object, const char *string);

	//根据节点名称,将'Object节点的cJSON 结构体'从Object root节点中删除
	void cJSON_DeleteItemFromObject(cJSON *object, const char *string);
	void cJSON_DeleteItemFromObjectCaseSensitive(\
		cJSON *object, const char *string);



3.5 unknow 大小的数据缓冲区创建类:
	void* cJSON_malloc(size_t size);//创建unknow 大小的数据缓冲区节点.
	void cJSON_free(void *object);//释放unknow 大小的数据缓冲区节点.





4.cjson 杂项API
	const char *cJSON_GetErrorPtr(void);//打印cjson 错误
	char* cJSON_Print(const cJSON *item);//打印cJSON节点
	char* cJSON_PrintUnformatted(const cJSON *pRoot);//cJSON节点->json字符串
*/



#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cjson/cJSON.h>



#define TESTHOST_LEN 2
#define TESTGROUP_LEN 2
#define ARR_LEN 16
#define IP_BUF_LEN 32
#define GNAME_LEN 128



//结构体定义
typedef struct TestHost_s{
	char ip[IP_BUF_LEN];
	unsigned short port;
}TestHost_t;

typedef struct TestGroup_s{
	bool isWork;
	int iArrNo[ARR_LEN];//常态数组
	int groupNo;
	char groupName[GNAME_LEN];
	TestHost_t host[TESTHOST_LEN];//内嵌结构体-复杂数组
}TestGroup_t;



//测试数据, 全局变量.
TestGroup_t g_TestGroups[TESTGROUP_LEN]={
{1,{0,1,2},10,"TestGroup1",{{"10.10.10.101",5060},{"10.10.10.102",5080}}},
{0,{9,8,7},20,"TestGroup2",{{"20.20.20.201",6060},{"20.20.20.202",6080}}}
};



//1.打印内嵌结构体-复杂数组,
//	普通数组或者普通cjson树, 直接cJSON_Print()打印即可.
//	如果是'内嵌结构体-复杂数组', 必须自己造打印函数.
void printSructArr(cJSON* root);

//2.将数据转换成'cJSON结构体',再将'cJSON结构体'转换成json字符串导出
void Get_TestGroup_t_Str(TestGroup_t *ptg,char **pret);

//3.解析json字符串,得到'cJSON结构体',
//	再利用'cJSON结构体'填充实体化结构体
bool Set_TestGroup_t(TestGroup_t *ptg,char *pdata);

//4.测试函数
void cjson_struct_test(void);



//主测试函数
int main(void){
	//测试函数
	cjson_struct_test();
	return 0;
}



//1.打印内嵌结构体-复杂数组,
//	普通数组或者普通cjson树, 直接cJSON_Print()打印即可.
//	如果是'内嵌结构体-复杂数组', 必须自己造打印函数.
void printSructArr(cJSON* root){
	//cjson 类型大全
	/*
		#define cJSON_Invalid (0)//0
		#define cJSON_False  (1 << 0)//1 向左位移0 位
		#define cJSON_True   (1 << 1)//1 向左位移1 位
		#define cJSON_NULL   (1 << 2)//1 向左位移2 位
		#define cJSON_Number (1 << 3)//1 向左位移3 位
		#define cJSON_String (1 << 4)//1 向左位移4 位
		#define cJSON_Array  (1 << 5)//1 向左位移5 位
		#define cJSON_Object (1 << 6)//1 向左位移6 位
		#define cJSON_Raw    (1 << 7)//1 向左位移7 位
	*/
	int i,arrSize;
	cJSON* item;



	//明知其是: 内嵌结构体-复杂数组, 那么
	arrSize = cJSON_GetArraySize(root);

	for(i=0; i<arrSize; i++){
		item = cJSON_GetArrayItem(root, i);//获取cjson节点

		//如果节点是cJSON_Object类型, 则递归本函数
		if(item->type == cJSON_Object)
				printSructArr(item);
		else{
			printf("	%s", item->string);//打印key
			printf("%s\n", cJSON_Print(item));//打印值
		}
	}
}



//2.将数据转换成'cJSON结构体',再将'cJSON结构体'转换成json字符串导出
void Get_TestGroup_t_Str(TestGroup_t *ptg,char **pret){
	cJSON *pRoot=NULL,*pnode=NULL,*pSon=NULL,*pSon_tmp,*ptmp;
	int tmp,pos,pos2;
	char* pJson;



	pRoot = cJSON_CreateObject();
	pnode = cJSON_CreateArray();
	for(pos = 0;pos < TESTGROUP_LEN;pos++){

		//组装TestHost_s
		pSon_tmp = cJSON_CreateArray();
		for(pos2 = 0;pos2 < TESTHOST_LEN;pos2++){
			ptmp = cJSON_CreateObject();
			cJSON_AddStringToObject(ptmp, "ip", ptg[pos].host[pos2].ip);
			cJSON_AddNumberToObject(ptmp, "port", ptg[pos].host[pos2].port);
			cJSON_AddItemToArray(pSon_tmp, ptmp);
		}

		//组装TestGroup_s
		pSon = cJSON_CreateObject();
		cJSON_AddNumberToObject(pSon, "groupNo", ptg[pos].groupNo);//bumber
		cJSON_AddStringToObject(pSon, "groupName", ptg[pos].groupName);//str
		cJSON_AddBoolToObject(pSon,"isWork",ptg[pos].isWork);//bool
		//常态数组
		ptmp = cJSON_CreateArray();
		cJSON_AddItemToArray(ptmp,\
				cJSON_CreateIntArray(ptg[pos].iArrNo,ARR_LEN));
		cJSON_AddItemToObject(pSon,"iArrNo",ptmp);

		cJSON_AddItemToObject(pSon, "host", pSon_tmp);//将数组打入json 里面

		cJSON_AddItemToArray(pnode,pSon);

	}

	cJSON_AddItemToObject(pRoot, "root_struct_array", pnode);//将数组推入根中

	//只用作测试, 不归入struct 结构体中, 免得结构体出现3 层.
	cJSON_AddNumberToObject(pRoot, "rootNum", 999);
	cJSON_AddStringToObject(pRoot, "rootStr", "rootStrAAA");
	cJSON_AddBoolToObject(pRoot,"rootBool",true);//存放在int里面,int=1
	cJSON_AddBoolToObject(pRoot,"rootBool2",false);//存放在int里面,int=0


	//得到json 字符串
	pJson = cJSON_PrintUnformatted(pRoot);
	tmp = strlen(pJson);
	*pret = (char*)malloc(tmp+1);
	strncpy(*pret,pJson,tmp+1);


	//释放root 根
	//printSructArr(pRoot);
	//printf("\n\n\n");
	cJSON_Delete(pRoot);

	return;
}



//3.解析json字符串,得到'cJSON结构体',
//	再利用'cJSON结构体'填充实体化结构体
bool Set_TestGroup_t(TestGroup_t *ptg,char *pdata){
	cJSON *pRoot,*pRootArr,*pNode,*pSon,*ptmp;
	int arrLen,arrLen2,pos,pos2;



	//解析json 字符串, 得到一个单个root Object cJSON 结构体
	pRoot = cJSON_Parse(pdata);
	if(pRoot == NULL) {
		printf("cJSON_Parse Err:%s\n",cJSON_GetErrorPtr());
		return false;
	}

	//从单个root Object cJSON 结构体中, 根据数组名, 获取数组root
	pRootArr = cJSON_GetObjectItem(pRoot, "root_struct_array");
	arrLen = cJSON_GetArraySize(pRootArr);
	//打印节点名和节点本身的数组长度(本节点是一个数组root)
	printf("%s arrLen = %d\n",pRootArr->string,arrLen);


	//只用作测试, 不归入struct 结构体中, 免得结构体出现3 层.
	printf("rootNum = %d\n",\
			cJSON_GetObjectItem(pRoot, "rootNum")->valueint);//number
	printf("rootStr = %s\n",\
			cJSON_GetObjectItem(pRoot, "rootStr")->valuestring);//str
	printf("rootBool = %d\n",\
			cJSON_GetObjectItem(pRoot, "rootBool")->valueint);//bool
	printf("rootBool2 = %d\n",\
			cJSON_GetObjectItem(pRoot, "rootBool2")->valueint);//bool


	//开始提取赋值
	for(pos = 0;pos < arrLen;pos++){

		//获取TestGroup_s[pos]
		pNode = cJSON_GetArrayItem(pRootArr, pos);

		//重置填充TestGroup_s[pos]
		ptg[pos].groupNo = cJSON_GetObjectItem(pNode, "groupNo")->valueint;
		strncpy(ptg[pos].groupName, \
				cJSON_GetObjectItem(pNode, "groupName")->valuestring,GNAME_LEN);
		ptg[pos].isWork = cJSON_GetObjectItem(pNode, "isWork")->valueint;


		pSon = cJSON_GetObjectItem(pNode, "iArrNo");//获取数组
		arrLen2 = cJSON_GetArraySize(pSon);//获取数组长度
		for(pos2 = 0;pos2 < arrLen2;pos2++)
			ptg[pos].iArrNo[pos2]=cJSON_GetArrayItem(pSon,pos2)->valueint;


		//重置填充TestHost_s
		pSon = cJSON_GetObjectItem(pNode,"host");
		arrLen2 = cJSON_GetArraySize(pSon);
		for(pos2 = 0;pos2 < arrLen2;pos2++){
			ptmp = cJSON_GetArrayItem(pSon, pos2);
			ptg[pos].host[pos2].port=cJSON_GetObjectItem(ptmp,"port")->valueint;
			strncpy(ptg[pos].host[pos2].ip,\
					cJSON_GetObjectItem(ptmp,"ip")->valuestring,IP_BUF_LEN);
		}

	}


	cJSON_Delete(pRoot);

	return true;
}



//4.测试函数
void cjson_struct_test(void){
	char* jsonStr = NULL;
	char* jsonStr2 = NULL;
	int jsonStr_len,jsonStr_len2;



	//1.测试-将数据转换成'cJSON结构体',再将'cJSON结构体'转换成json字符串导出
	//	(生成json 字符串)
	Get_TestGroup_t_Str(&g_TestGroups[0],&jsonStr);
	Get_TestGroup_t_Str(&g_TestGroups[1],&jsonStr2);
	jsonStr_len = strlen(jsonStr);
	jsonStr_len2 = strlen(jsonStr2);
	printf("\narray[0] string len=%d,array[0] string len=%d\n\n",\
			jsonStr_len,jsonStr_len2);
	printf("\n\nreturn json string array[0]:\n%s\n\n",jsonStr);
	printf("\n\nreturn json string array[1]:\n%s\n\n",jsonStr2);


	//2.修改全局变量数据
	g_TestGroups[0].groupNo = 0;
	g_TestGroups[1].groupNo = 0;
	strncpy(g_TestGroups[1].groupName,"ffffffffff1111111111",GNAME_LEN);
	strncpy(g_TestGroups[0].groupName,"ffffffffff0000000000",GNAME_LEN);
	printf("\n\n值已经被篡改, 等待json 字符串恢复结构体的数据完整性!!\n\n");


	//3.测试是否可以根据json 字符串, 还原来的值, 如果可以, 证明还原也成功
	//	(根据'json字符串'还原全局变量数据)
	Set_TestGroup_t(&g_TestGroups[0],jsonStr);//执行还原
	Set_TestGroup_t(&g_TestGroups[1],jsonStr2);//执行还原
	//还原之后, 释放jsonStr 缓冲区
	free(jsonStr);
	free(jsonStr2);
	jsonStr = NULL;
	jsonStr2 = NULL;


	printf("\n\n\n<<还原之后>>\n\n");
	//5.重新生成一次json 字符串, 看前后的字符串是否一致, 一致则成功.
	Get_TestGroup_t_Str(&g_TestGroups[0],&jsonStr);
	Get_TestGroup_t_Str(&g_TestGroups[1],&jsonStr2);
	jsonStr_len = strlen(jsonStr);
	jsonStr_len2 = strlen(jsonStr2);
	printf("\n还原之后:array[0] string len=%d,array[0] string len=%d\n\n",\
			jsonStr_len,jsonStr_len2);
	printf("\n\n还原之后:return json string array[0]:\n%s\n\n",jsonStr);
	printf("\n\n还原之后:return json string array[1]:\n%s\n\n",jsonStr2);
	//用完之后, 释放jsonStr 缓冲区
	free(jsonStr);
	free(jsonStr2);
	jsonStr = NULL;
	jsonStr2 = NULL;

	return ;
}


