//111111111111111111111111111111111111111111111111111111111111111111111111
//格式测试:
//邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵
//起始日期:
//完成日期:
//************************************************************************
//修改日志:

//, , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , ,, , ,


//编译:
//g++ -ggdb3 -o des_test des.cpp -lssl -lcrypto


//用法:
//对称性加密算法有: AES、DES、3DES
//非对称性算法有: RSA、DSA、ECC
//其中:
/*
1.对称加密算法介绍:
DES(Data Encryption Standard): 
	数据加密标准, 速度较快, 适用于加密大量数据的场合. 

3DES(Triple DES): 
	是基于DES, 对一块数据用三个不同的密钥进行三次加密, 强度更高. 

AES(Advanced Encryption Standard): 
	高级加密标准, 是下一代的加密算法标准, 速度快, 安全级别高; 
	AES是一个使用128为分组块的分组加密算法,
	分组块和128、192或256位的密钥一起作为输入, 对4×4的字节数组上进行操作.
	众所周之AES是种十分高效的算法, 尤其在8位架构中, 这源于它面向字节的设计. 
	AES 适用于8位小型单片机或32位微处理器, 并且适合用专门的硬件实现,
	硬件实现能够使其吞吐量(每秒可以到达的加密/解密bit数)达到十亿量级.
	同样, 其也适用于RFID系统. 



2.des 加密算法的加密模式(只举例ebc, cbc, 还有更多没有举例)
//4.1: 使用ECB模式进行加密
	关键api:
		void DES_ecb_encrypt(const_DES_cblock *input, \
			DES_cblock *output, DES_key_schedule *ks, int enc);
	参数说明:
		enc:DES_ENCRYPT=加密; DES_DECRYPT=解密
	填充方式:
		ECB 电子密码本模式, 即将数据'按照8个字节一段'进行DES加密或解密,
		得到一段段的8个字节的密文或者明文,
		最后一段不足8个字节(补0或补F), 按照需求补足8个字节进行计算(并行计算),
		之后按照顺序将计算所得的数据连在一起即可, 各段数据之间互不影响.


//*4.2*: 使用CBC模式进行加密
	关键api:
		void DES_ncbc_encrypt(const unsigned char *input, \
			unsigned char *output, long length, \
			DES_key_schedule, DES_cblock *ivec, int enc);
	参数说明:
		length: 指明文或者密文长度
		ivec: 是初始化向量IV
		enc:DES_ENCRYPT=加密; DES_DECRYPT=解密
	填充方式:
		如果length不是8的倍数, 会使用00填充

	CBC, 密文分组链接方式, 它的实现机制使加密的各段数据之间有了联系.
	其实现的机理如下:
	1. 首先将数据按照8个字节一组进行分组得到D1D2...Dn
			(若数据不是8的整数倍, 用指定的PADDING数据补位)
	2. 第一组数据D1与'初始化向量IV', 异或后的结果, 进行DES加密,
			得到第一组密文C1(初始化向量I为全零)
	3. 第二组数据D2与'第一组的加密结果C1', 异或以后的结果进行DES加密,
			得到第二组密文C2
	4. 之后的数据以此类推, 得到Cn
	5. 按顺序连为C1C2C3...Cn即为加密结果.



3.des 主要函数api 说明:
//3.1: 创建'DES key变量'
	typedef unsigned char DES_cblock[8];
	DES_cblock key;//变量key
	const_DES_cblock key;//常量key


//3.2: 随机生成'DES key变量'
	生成一个随机的key, 必须使用下面的set_key函数转换为schedule之后才能使用
	void DES_random_key(DES_cblock *ret);

//3.3: key-校验清算:
	这两个函数的主要区别在于: 是否检测key的奇偶校检位
		checked() 会对奇偶校检位进行检查,
			如果校检位错误, 返回-1, 如果key强度比较弱, 返回-2;
		unchecked() 则不会检查
	int DES_set_key_checked(const_DES_cblock *key, \
				DES_key_schedule *schedule);
	void DES_set_key_unchecked(const_DES_cblock *key, \
				DES_key_schedule *schedule);

	ps: 一般用DES_set_key_unchecked() 比较快捷.

//3.4:
	DES_ecb_encrypt()	 //ebc 模块的加密, 解密, 通用函数
	DES_ncbc_encrypt()	//cbc 模块的加密, 解密, 通用函数

	最后一个参数(控制'加密', 还是'解密'):
		加密：DES_ENCRYPT;
		解密：DES_DECRYPT;

*/





#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/des.h>



char *smart_point;//这个指针的存在,使得这个测试程序是单线程版本的



//1.打印密文
inline void print_encrypted(char* data);
//2.打印明文
inline void print_text(char* data);



//3.des ebc 加密
//(注意: strlen(key_str),意味着只能传入一串字符串,而且长度不能超出8字节)
//char* data, char* ret,这两个参数的缓冲区长度,应当一致!!
//int buf_len = sizeof(data_buf) = sizeof(ret_buf)
void des_ebc_encrypt(char* key_str, \
		char* data, char* ret, int buf_len);

//4.des ebc 解密
void des_ebc_decrypt(char* key_str, \
		char* data, char* ret, int buf_len);

//5.des cbc 加密(char* key_str 长度不限, char* ivec 必须<=8字节)
void des_cbc_encrypt(char* key_str, \
		char* ivec, char* data, char* ret, int buf_len);

//6.des cbc 解密
void des_cbc_decrypt(char* key_str, \
		char* ivec, char* data, char* ret, int buf_len);



//7.des ebc encrypt 测试
void des_ebc_encrypt_test(void);
//8.des ebc decrypt 测试
void des_ebc_decrypt_test(void);
//9.des ebc encrypt 测试
void des_cbc_encrypt_test(void);
//10.des ebc decrypt 测试
void des_cbc_decrypt_test(void);



int main(void){
	//1.des ebc 测试
	printf("1.des ebc 测试:\n");
	des_ebc_encrypt_test();
	des_ebc_decrypt_test();

	//2.des cbc 测试
	printf("2.des cbc 测试:\n");
	des_cbc_encrypt_test();
	des_cbc_decrypt_test();

	return 0;
}



//1.打印密文
inline void print_encrypted(char* data){
	int i;
	printf("print_encrypted:\n");
	for(i = 0; i < strlen(data); i++)
	 printf("%02x", data[i]);
	printf("\n\n");
	return;
}



//2.打印明文
inline void print_text(char* data){
	printf("print_text:\n%s\n\n", data);
	return;
}



//3.des ebc 加密
//(注意: strlen(key_str),意味着只能传入一串字符串,而且长度不能超出8字节)
//char* data, char* ret,这两个参数的缓冲区长度,应当一致!!
//int buf_len = sizeof(data_buf) = sizeof(ret_buf)
void des_ebc_encrypt(char* key_str, \
		char* data, char* ret, int buf_len){
	DES_cblock key, data_tmp_in, data_tmp_out;
	DES_key_schedule schedule;
	int i = strlen(key_str) + 1;
	int datalen = strlen(data);



	//key 长度检查
	if(i > 8){
		printf("des_ebc_encrypt() fail, 'char* key_str' length > 8\n");
		return;
	}
	//缓冲区长度检查(必须为8的倍数)
	if(buf_len%8 != 0){
		printf("des_ebc_encrypt() fail, 'int buf_len' length%8 != 0\n");
		return;
	}
	//数据填补后, 数据长度'是否大于'缓冲区长度??
	datalen = datalen + 1;//补齐'\0' 结束字符串
	datalen = (datalen%8 == 0) ? datalen : datalen + (8-datalen%8);
	if(datalen > buf_len){
		printf("des_ebc_encrypt() fail, datalen > buf_len\n");
		return;
	}


	memset(key,'\0',sizeof(key));
	memset(ret,'\0',buf_len);//如果这里崩溃了, 也容易发现

	strncpy((char*)&key,key_str,i);
	DES_set_key_unchecked(&key, &schedule);//将'des 密钥'转换成schedule

	//执行ebc 加密
	for(i = 0;i < datalen/8;i++){
		memcpy(data_tmp_in,data+i*8,8);
		DES_ecb_encrypt(&data_tmp_in, \
				&data_tmp_out, &schedule, DES_ENCRYPT);
		memcpy(ret+i*8,data_tmp_out,8);
	}

	return;
}



//4.des ebc 解密
void des_ebc_decrypt(char* key_str, \
		char* data, char* ret, int buf_len){
	DES_cblock key, data_tmp_in, data_tmp_out;
	DES_key_schedule schedule;
	int i = strlen(key_str) + 1;
	int datalen = strlen(data);



	//key 长度检查
	if(i > 8){
		printf("des_ebc_decrypt() fail, 'char* key_str' length > 8\n");
		return;
	}
	//缓冲区长度检查(必须为8的倍数)
	if(buf_len%8 != 0){
		printf("des_ebc_decrypt() fail, 'char* data' length%8 != 0\n");
		return;
	}
	//数据填补后, 数据长度'是否大于'缓冲区长度??
	datalen = datalen + 1;//补齐'\0' 结束字符串
	datalen = (datalen%8 == 0) ? datalen : datalen + (8-datalen%8);
	if(datalen > buf_len){
		printf("des_ebc_decrypt() fail, datalen > buf_len\n");
		return;
	}


	memset(key,'\0',sizeof(key));
	memset(ret,'\0',buf_len);//如果这里崩溃了, 也容易发现

	strncpy((char*)&key,key_str,i);
	DES_set_key_unchecked(&key, &schedule);//将'des 密钥'转换成schedule

	//执行ebc 解密
	for(i = 0;i < datalen/8;i++){
		memcpy(data_tmp_in,data+i*8,8);
		DES_ecb_encrypt(&data_tmp_in, \
				&data_tmp_out, &schedule, DES_DECRYPT);
		memcpy(ret+i*8,data_tmp_out,8);
	}

	return;
}



//5.des cbc 加密(char* key_str 长度不限, char* ivec 必须<=8字节)
void des_cbc_encrypt(char* key_str, \
		char* ivec, char* data, char* ret, int buf_len){
	DES_cblock key, key_ivec;
	DES_key_schedule schedule;
	int ivec_len = strlen(ivec) + 1;
	//int keylen = strlen(key_str);
	int datalen = strlen(data);



	//key 长度检查
	if(ivec_len > 8){
		printf("des_cbc_encrypt() fail, 'char* ivec' length > 8\n");
		return;
	}
	//缓冲区长度检查(必须为8的倍数)
	if(buf_len%8 != 0){
		printf("des_cbc_encrypt() fail, 'char* data' length%8 != 0\n");
		return;
	}
	//数据填补后, 数据长度'是否大于'缓冲区长度??
	datalen = datalen + 1;//补齐'\0' 结束字符串
	datalen = (datalen%8 == 0) ? datalen : datalen + (8-datalen%8);
	if(datalen > buf_len){
		printf("des_cbc_encrypt() fail, datalen > buf_len\n");
		return;
	}
	/*
	//钥匙长度不受限制
	if(keylen%8 != 0){
		printf("des_cbc_encrypt() fail,'char* key_str' length%8 != 0\n");
		return;
	}
	*/
	memset(key_ivec,'\0',sizeof(key_ivec));
	memset(ret,'\0',buf_len);//如果这里崩溃了, 也容易发现

	DES_string_to_key(key_str, &key);//生成钥匙
	strncpy((char*)&key_ivec, ivec, ivec_len);//生成ivec key
	DES_set_key_unchecked(&key, &schedule);//将'des 密钥'转换成schedule

	//执行cbc 加密
	DES_ncbc_encrypt((unsigned char*)data, (unsigned char*)ret, \
			(long int)datalen, &schedule, &key_ivec, DES_ENCRYPT);

	return;
}



//6.des cbc 解密
void des_cbc_decrypt(char* key_str, \
		char* ivec, char* data, char* ret, int buf_len){
	DES_cblock key, key_ivec;
	DES_key_schedule schedule;
	int ivec_len = strlen(ivec) + 1;
	//int keylen = strlen(key_str);
	int datalen = strlen(data);



	//key 长度检查
	if(ivec_len > 8){
		printf("des_cbc_decrypt() fail, 'char* ivec' length > 8\n");
		return;
	}
	//缓冲区长度检查(必须为8的倍数)
	if(buf_len%8 != 0){
		printf("des_cbc_decrypt() fail, 'char* data' length%8 != 0\n");
		return;
	}
	//数据填补后, 数据长度'是否大于'缓冲区长度??
	datalen = datalen + 1;//补齐'\0' 结束字符串
	datalen = (datalen%8 == 0) ? datalen : datalen + (8-datalen%8);
	if(datalen > buf_len){
		printf("des_cbc_decrypt() fail, datalen > buf_len\n");
		return;
	}
	/*
	//钥匙长度不受限制
	if(keylen%8 != 0){
		printf("des_cbc_encrypt() fail,'char* key_str' length%8 != 0\n");
		return;
	}
	*/
	memset(key_ivec,'\0',sizeof(key_ivec));
	memset(ret,'\0',buf_len);//如果这里崩溃了, 也容易发现

	DES_string_to_key(key_str, &key);//生成钥匙
	strncpy((char*)&key_ivec, ivec, ivec_len);//生成ivec key
	DES_set_key_unchecked(&key, &schedule);//将'des 密钥'转换成schedule

	//执行cbc 加密
	DES_ncbc_encrypt((unsigned char*)data, (unsigned char*)ret, \
			(long int)datalen, &schedule, &key_ivec, DES_DECRYPT);

	return;
}





//*************************************************






//7.des ebc encrypt 测试
void des_ebc_encrypt_test(void){
	char des_ebc_key[8] = "1234567";
	//char des_ebc_data[512+8] = "hi i'm adan shaw !!";
	char des_ebc_data[512+8] = "中文啊啊啊啊des ebc";//中文
	char* des_ebc_ret = (char*)malloc(512+8);
	int des_ebc_buf_len = 512+8;



	//执行加密
	des_ebc_encrypt(des_ebc_key,\
			des_ebc_data,des_ebc_ret,des_ebc_buf_len);

	print_text(des_ebc_data);//结束打印
	print_encrypted(des_ebc_ret);

	smart_point = des_ebc_ret;//传递给后续测试函数-解密
	return;
}



//8.des ebc decrypt 测试
void des_ebc_decrypt_test(void){
	char des_ebc_key[8] = "1234567";
	//char des_ebc_data[512+8] = "hi i'm adan shaw !!";
	char* des_ebc_ret = (char*)malloc(512+8);
	int des_ebc_buf_len = 512+8;



	//执行加密
	des_ebc_decrypt(des_ebc_key,\
			smart_point,des_ebc_ret,des_ebc_buf_len);

	print_text(des_ebc_ret);//结束打印

	free(smart_point);
	smart_point = NULL;
	return;
}



//9.des ebc encrypt 测试
void des_cbc_encrypt_test(void){
	char des_cbc_key[8] = "1234567";
	char des_cbc_ivce[8] = "9876543";
	//char des_cbc_data[512+8] = "hi i'm adan shaw !!";
	char des_cbc_data[512+8] = "中文啊啊啊啊des cbc";//中文
	char* des_cbc_ret = (char*)malloc(512+8);
	int des_cbc_buf_len = 512+8;



	//执行加密
	des_cbc_encrypt(des_cbc_key,\
			des_cbc_ivce,des_cbc_data,des_cbc_ret,des_cbc_buf_len);

	print_text(des_cbc_data);//结束打印
	print_encrypted(des_cbc_ret);

	smart_point = des_cbc_ret;//传递给后续测试函数-解密
	return;
}



//10.des ebc decrypt 测试
void des_cbc_decrypt_test(void){
	char des_cbc_key[8] = "1234567";
	char des_cbc_ivce[8] = "9876543";
	//char des_cbc_data[512+8] = "hi i'm adan shaw !!";
	char* des_cbc_ret = (char*)malloc(512+8);
	int des_cbc_buf_len = 512+8;



	//执行加密
	des_cbc_decrypt(des_cbc_key,\
			des_cbc_ivce,smart_point,des_cbc_ret,des_cbc_buf_len);

	print_text(des_cbc_ret);//结束打印

	free(smart_point);
	smart_point = NULL;
	return;
}


















/*

//***指针数组, 移动游标pos, CP 字符串的典范, 可惜用错地方而已.
//填充'数据输入缓冲区'
if(sizeof(data)%8 == 0){
	strncpy((char*)input,(char*)data,sizeof(data));
	memset((void*)&input[sizeof(data)],'8',8);//刚好够就补8个8
}
else{
	strncpy((char*)input,(char*)data,sizeof(data));
	i = 8 - sizeof(data)%8;

	//差多少就补多少个i
	memset((void*)&input[sizeof(data)],sizeof(data)%8,i);
}

*/




























//old 旧代码
/*

void des_ecb_test(void){
	DES_cblock key;//des 密钥结构体
	DES_key_schedule schedule;
	int i;

//注意: const_DES_cblock 的数组长度是8 个字节,
//ECB 模式下, 被加密的数据, 不能超过8 字节, 否则需要自己分段加密.
	//
//const_DES_cblock input = "hello world!! hahahaha";
const_DES_cblock input = "hello";//ECB 固定长度-源数据
DES_cblock output;//ECB 固定长度-密文输出



//人工指定des 密钥(默认发, 收双方, 都知道des 密钥 = "damn it")
	strncpy((char*)&key,"damn it",sizeof(key));
//DES_random_key(&key);//随机初始化des 密钥
	DES_set_key_checked(&key, &schedule);//将'des 密钥'转换成schedule


	printf("clear text: %s\n", input);//des 加密前显示
	//加密
	DES_ecb_encrypt(&input, &output, &schedule, DES_ENCRYPT);
	printf("Encrypted!\n\n");

	//打印密文
	printf("cipher text: ");

	for(i = 0; i < sizeof(input); i++)
	 printf("%02x", output[i]);
	printf("\n\n");



	//重置数据(重置之后, 只剩下output 密文是没有被重置的)
	memset(&key,'\0',sizeof(key));//重置密钥, 防止作弊
	memset(&input,'\0',sizeof(input));//重置input, 防止作弊
	memset(&schedule,'\0',sizeof(schedule));

//人工指定des 密钥(默认发, 收双方, 都知道des 密钥 = "damn it")
strncpy((char*)&key,"damn it",sizeof(key));
DES_set_key_checked(&key, &schedule);//将'des 密钥'转换成schedule

//解密
	DES_ecb_encrypt(&output, &input, &schedule, DES_DECRYPT);
	printf("Decrypted!\n");
	printf("clear text: %s\n\n\n", input);//显示解密后的数据

	return ;
}





//CBC模式(超长不需要分段)
void des_cbc_test(void){
	const char *keystring = "this is my key!!!";//自定义解密钥匙--共知
	DES_cblock key;//des 密钥结构体
	DES_key_schedule key_schedule;

	int i = 0;

	unsigned char data[] = "this is a text being encrypted by openssl";
	//unsigned char data[] = "1234567";
	//size_t len = (sizeof(input)+7)/8 * 8;//计算被加密的数据长度
	size_t len = (sizeof(data)%8 == 0) ? \
		sizeof(data) + 8 : sizeof(data) + (8 - sizeof(data)%8);

	//密文输出缓冲区
	unsigned char *output = (unsigned char *)malloc(len+1);
	//解密文输出缓冲区
	unsigned char *output2 = (unsigned char *)malloc(len+1);

	DES_cblock ivec;//iv随机数(或伪随机) 干扰数据; 主要作用于des 第一个block.
									//使得'des 密文'的复杂度增加. (但不足之处: 需要双方共知!!)
									//可随信附带iv 干扰数据, 反正你可以对iv 进行单节des 加密.
									//先解密出iv 干扰数据, 才能解密出真正的密文des(也是一种方法)



	DES_string_to_key(keystring, &key);//生成钥匙
	//将'des 密钥'转换成schedule
	if(DES_set_key_checked(&key, &key_schedule) != 0) {
		printf("convert to key_schedule failed.\n");
		return ;
	}

	//DES_random_key(&ivec);//初始化'扰乱因子'为随机字符串. ps: 错误方法
	memset((char*)&ivec, 0, sizeof(ivec));//初始化IV为0x0000000000000;


	//初始化数据缓冲区
	memset(output,'\0',len+1);
	memset(output2,'\0',len+1);


	printf("\n\nclear text: %s\n\n", data);//des 加密前显示
	//加密
	DES_ncbc_encrypt(data, output, \
		sizeof(data), &key_schedule, &ivec, DES_ENCRYPT);
	printf("Encrypted!\n\n");
	//输出加密以后的内容
	printf("cipher text: ");
	for(i = 0; i < len; ++i)
	 printf("%02x", output[i]);
	printf("\n\n");



	//重置数据, 防止作弊
	memset((char*)&ivec, 0, sizeof(ivec));//重置IV, 此时IV 为空字符串!!

	memset(&key,'\0',sizeof(key));//重置密钥, 防止作弊
	DES_string_to_key(keystring, &key);//重新根据'共知字符串'生成钥匙
	//将'des 密钥'转换成schedule
	if(DES_set_key_checked(&key, &key_schedule) != 0) {
		printf("convert to key_schedule failed.\n");
		return ;
	}

	//解密
	DES_ncbc_encrypt(output, output2, len, &key_schedule, &ivec, 0);
	printf("Decrypted!\n");
	printf("clear text: %s\n\n", output2);//打印解密内容

	free(output);
	free(output2);
	return;
}


 */
