//111111111111111111111111111111111111111111111111111111111111111111111111
//格式测试:
//邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵
//起始日期:
//完成日期:
//************************************************************************
//修改日志:

//, , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , ,, , ,

//编译:
//g++ -o x ./cjson_easy.cpp -ggdb3 -lcjson



#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cjson/cJSON.h>



#define nArr_ARR_LEN 16
#define cStr_BUF_LEN 256
#define JSON_STR_BUF_LEN 4096 //json 字符串缓冲区, 不够大,
															//容易造成cjson 解析出错.



//测试数据1-全部简单常量
typedef struct all_type{
	bool is_working;
	int iNum;
	double dSum;
	int nArr[nArr_ARR_LEN];
	char cStr[cStr_BUF_LEN];
}all_type_t;

//测试数据2-简单结构体内嵌, 非结构体数组式
typedef struct easy_struct{
	bool is_working;
	int iNum;
	double dSum;
	int nArr[nArr_ARR_LEN];
	char cStr[cStr_BUF_LEN];
	all_type_t easy;
}easy_struct_t;



//1.1 测试1-全部简单常量
void all_type_test(void);

//1.2 测试1-封装json 字符串, pickupStr
void all_type_pickupStr(all_type_t* pdata,char* pret);

//1.3 测试1-解析json 字符串, unpackStr
void all_type_unpackStr(all_type_t* pret,char* pdata);

//2.1 测试2-简单结构体内嵌, 非结构体数组式
void easy_struct_test(void);

//2.2 测试2-封装json 字符串, pickupStr
void easy_struct_pickupStr(easy_struct_t* pdata,char* pret);

//2.3 测试2-解析json 字符串, unpackStr
void easy_struct_unpackStr(easy_struct_t* pret,char* pdata);



int main(void){
	//1.测试1-全部简单常量
	printf("1.测试1-全部简单常量\n");
	all_type_test();

	printf("\n\n\n");
	//2.测试2-简单结构体内嵌, 非结构体数组式
	printf("测试2-简单结构体内嵌, 非结构体数组式\n");
	easy_struct_test();

	return 0;
}



//1.1 测试1-全部简单常量
void all_type_test(void){
	all_type_t x;
	int tmp;
	char jsonStr_buf[JSON_STR_BUF_LEN];



	//1.赋值
	x.iNum = 999;
	x.is_working = true;
	x.dSum = 399.33;
	strncpy(x.cStr,"all_type_test(): x.cStr",cStr_BUF_LEN);
	for(tmp=0;tmp<nArr_ARR_LEN;tmp++)
		x.nArr[tmp] = tmp+60;


	//2.打包出json 字符串
	memset(jsonStr_buf,'\0',JSON_STR_BUF_LEN);
	all_type_pickupStr(&x,jsonStr_buf);
	printf("all_type_pickupStr():\n%s\n\n",jsonStr_buf);


	//3.篡改all_type_t x; 原来的值, 看jsonStr能够将值还原.
	//	如果还原成功, 说明jsonStr解析成功.
	x.iNum = 111;
	x.is_working = false;
	x.dSum = 888.22;


	//4.解析json 字符串, 还原被篡改的值
	all_type_unpackStr(&x,jsonStr_buf);
	printf("all_type_unpackStr():\n");//打印还原后的结果
	printf("x.iNum = %d\n",x.iNum);
	printf("x.is_working = %d\n",x.is_working);
	printf("x.dSum = %lf\n",x.dSum);

	return;
}



//1.2 测试1-封装json 字符串, pickupStr
void all_type_pickupStr(all_type_t* pdata,char* pret){
	cJSON *pRoot,*ptmp;



	//1.生成cJSON 树
	pRoot = cJSON_CreateObject();
	cJSON_AddStringToObject(pRoot, "cStr", pdata->cStr);
	cJSON_AddNumberToObject(pRoot, "iNum", pdata->iNum);
	cJSON_AddNumberToObject(pRoot, "dSum", pdata->dSum);
	cJSON_AddBoolToObject(pRoot,"is_working",pdata->is_working);

	ptmp = cJSON_CreateIntArray(pdata->nArr,nArr_ARR_LEN);//数组常理
	cJSON_AddItemToObject(pRoot,"nArr",ptmp);


	//2.导出jsonStr
	//pret = cJSON_PrintUnformatted(pRoot);
	strncpy(pret, cJSON_PrintUnformatted(pRoot), JSON_STR_BUF_LEN);


	//3.删除cJSON 树
	cJSON_Delete(pRoot);

	return;
}



//1.3 测试1-解析json 字符串, unpackStr
void all_type_unpackStr(all_type_t* pret,char* pdata){
	cJSON *pRoot,*ptmp;
	int arrLen,pos;



	//1.解析json 字符串, 得到一个单个root Object cJSON 结构体
	pRoot = cJSON_Parse(pdata);
	if(pRoot == NULL) {
		printf("cJSON_Parse Err:%s\n",cJSON_GetErrorPtr());
		return;
	}


	//2.提取cJSON 中的值, 并进行赋值到结构体中返回
	ptmp = cJSON_GetObjectItem(pRoot, "cStr");
	strncpy(pret->cStr,ptmp->valuestring,cStr_BUF_LEN);

	ptmp = cJSON_GetObjectItem(pRoot, "iNum");
	pret->iNum = ptmp->valueint;

	ptmp = cJSON_GetObjectItem(pRoot, "dSum");
	pret->dSum = ptmp->valuedouble;

	ptmp = cJSON_GetObjectItem(pRoot, "is_working");
	pret->is_working = ptmp->valueint;


	ptmp = cJSON_GetObjectItem(pRoot, "nArr");//数组
	arrLen = cJSON_GetArraySize(ptmp);
	for(pos=0;pos<arrLen;pos++)
		pret->nArr[pos] = cJSON_GetArrayItem(ptmp,pos)->valueint;


	//3.删除cJSON 根
	cJSON_Delete(pRoot);

	return;
}



//2.1 测试2-简单结构体内嵌, 非结构体数组式
void easy_struct_test(void){
	easy_struct_t x;
	all_type_t* px;
	int tmp;
	char jsonStr_buf[JSON_STR_BUF_LEN];



	//1.赋值
	x.iNum = 999;
	x.is_working = true;
	x.dSum = 399.33;
	strncpy(x.cStr,"easy_struct_test(): x.cStr",cStr_BUF_LEN);
	for(tmp=0;tmp<nArr_ARR_LEN;tmp++)
		x.nArr[tmp] = tmp+60;

	px = &x.easy;//简单内嵌-结构体填充
	px->iNum = 999;
	px->is_working = true;
	px->dSum = 399.33;
	strncpy(px->cStr,"easy_struct_test(): x.cStr",cStr_BUF_LEN);
	for(tmp=0;tmp<nArr_ARR_LEN;tmp++)
		px->nArr[tmp] = tmp+60;


	//2.打包出json 字符串
	memset(jsonStr_buf,'\0',JSON_STR_BUF_LEN);
	easy_struct_pickupStr(&x,jsonStr_buf);
	printf("easy_struct_pickupStr():\n%s\n\n",jsonStr_buf);


	//3.篡改all_type_t x; 原来的值, 看jsonStr能够将值还原.
	//	如果还原成功, 说明jsonStr解析成功.
	x.iNum = 111;
	x.is_working = false;
	x.dSum = 888.22;
	px->iNum = 777;
	px->is_working = true;
	px->dSum = 5555.33;


	//4.解析json 字符串, 还原被篡改的值
	easy_struct_unpackStr(&x,jsonStr_buf);
	printf("easy_struct_unpackStr():\n");//打印还原后的结果
	printf("x.iNum = %d\n",x.iNum);
	printf("x.is_working = %d\n",x.is_working);
	printf("x.dSum = %lf\n",x.dSum);
	printf("px->iNum = %d\n",px->iNum);
	printf("px->is_working = %d\n",px->is_working);
	printf("px->dSum = %lf\n",px->dSum);

	return;
}



//2.2 测试2-封装json 字符串, pickupStr
void easy_struct_pickupStr(easy_struct_t* pdata,char* pret){
	cJSON *pRoot,*ptmp,*pSon;
	all_type_t* px = &pdata->easy;



	//1.生成cJSON 树
	pRoot = cJSON_CreateObject();
	cJSON_AddStringToObject(pRoot, "cStr", pdata->cStr);
	cJSON_AddNumberToObject(pRoot, "iNum", pdata->iNum);
	cJSON_AddNumberToObject(pRoot, "dSum", pdata->dSum);
	cJSON_AddBoolToObject(pRoot,"is_working",pdata->is_working);

	ptmp = cJSON_CreateIntArray(pdata->nArr,nArr_ARR_LEN);//数组常理
	cJSON_AddItemToObject(pRoot,"nArr",ptmp);

	pSon = cJSON_CreateObject();
	cJSON_AddStringToObject(pSon, "cStr", px->cStr);
	cJSON_AddNumberToObject(pSon, "iNum", px->iNum);
	cJSON_AddNumberToObject(pSon, "dSum", px->dSum);
	cJSON_AddBoolToObject(pSon,"is_working",px->is_working);

	ptmp = cJSON_CreateIntArray(px->nArr,nArr_ARR_LEN);
	cJSON_AddItemToObject(pSon,"nArr",ptmp);


	cJSON_AddItemToObject(pRoot,"easy",pSon);


	//2.导出jsonStr
	//pret = cJSON_PrintUnformatted(pRoot);
	strncpy(pret, cJSON_PrintUnformatted(pRoot), JSON_STR_BUF_LEN);


	//3.删除cJSON 树
	cJSON_Delete(pRoot);

	return;
}



//2.3 测试2-解析json 字符串, unpackStr
void easy_struct_unpackStr(easy_struct_t* pret,char* pdata){
	cJSON *pRoot,*ptmp,*pSon;
	int arrLen,pos;
	all_type_t* px = &pret->easy;



	//1.解析json 字符串, 得到一个单个root Object cJSON 结构体
	pRoot = cJSON_Parse(pdata);
	if(pRoot == NULL) {
		printf("cJSON_Parse Err:%s\n",cJSON_GetErrorPtr());
		return;
	}


	//2.提取cJSON 中的值, 并进行赋值到结构体中返回
	ptmp = cJSON_GetObjectItem(pRoot, "cStr");
	strncpy(pret->cStr,ptmp->valuestring,cStr_BUF_LEN);

	ptmp = cJSON_GetObjectItem(pRoot, "iNum");
	pret->iNum = ptmp->valueint;

	ptmp = cJSON_GetObjectItem(pRoot, "dSum");
	pret->dSum = ptmp->valuedouble;

	ptmp = cJSON_GetObjectItem(pRoot, "is_working");
	pret->is_working = ptmp->valueint;


	ptmp = cJSON_GetObjectItem(pRoot, "nArr");//数组
	arrLen = cJSON_GetArraySize(ptmp);
	for(pos=0;pos<arrLen;pos++)
		pret->nArr[pos] = cJSON_GetArrayItem(ptmp,pos)->valueint;


	pSon = cJSON_GetObjectItem(pRoot, "easy");

	ptmp = cJSON_GetObjectItem(pSon, "cStr");
	strncpy(pret->cStr,ptmp->valuestring,cStr_BUF_LEN);

	ptmp = cJSON_GetObjectItem(pSon, "iNum");
	pret->iNum = ptmp->valueint;

	ptmp = cJSON_GetObjectItem(pSon, "dSum");
	pret->dSum = ptmp->valuedouble;

	ptmp = cJSON_GetObjectItem(pSon, "is_working");
	pret->is_working = ptmp->valueint;


	ptmp = cJSON_GetObjectItem(pSon, "nArr");//数组
	arrLen = cJSON_GetArraySize(ptmp);
	for(pos=0;pos<arrLen;pos++)
		pret->nArr[pos] = cJSON_GetArrayItem(ptmp,pos)->valueint;


	//3.删除cJSON 根
	cJSON_Delete(pRoot);

	return;
}
