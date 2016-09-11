#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#include <unistd.h>
#include <pthread.h>
#include <sys/wait.h>

#include <hiredis.h>

#include "fcgi_stdio.h"
#include "make_log.h"
#include "redis_op.h"
#include "cJSON.h"

#define UPLOAD_MODULE "upload"
#define UPLOAD_PROC   "upload_test"

#define FDFS_CLIENT_MODULE "fdfs_client"
#define FDFS_CLIENT_PROC   "fdfs_test"

#define QUERY_STRING_MODULE "query_string"
#define QUERY_STRING_PROC   "query"

#define GET_CMD_MODULE      "get_cmd"
#define GET_CMD_PROC        "cmd"

#define GET_USR_MODULE      "get_usr"
#define GET_USR_PROC        "usr"

#define GET_FROMID_MODULE      "get_fromId"
#define GET_FROMID_PROC        "fromId"

#define GET_COUNT_MODULE      "get_count"
#define GET_COUNT_PROC        "count"

#define READ_REDIS_MODULE   "read_redis_json"
#define READ_REDIS_PROC     "read_redis_json_test"

#define GET_REDIS_VALUE_MODULE "get_redis_value" 
#define GET_REDIS_VALUE_PROC  "get_redis_v_test"

#define FILE_ID_LEN     (256)
#define TIME_STRING_LEN (64)
#define SUFFIX_LEN      (8)

#define REDIS_DILIMIT   "||"

//开辟内存空间，保存上传文件以及htpp协议里的全部数据
int get_buf(char **src, int len);

//提取出数据中文件名，二进制数据，完成上传
int upload_file(char *src, int len, char **s_filename, int *filenameBufLen);

//从二进制字符串中查找子串
char* memstr(char* full_data, int full_data_len, char* substr);

//使用fdfs将文件传入storage
int fdfs_client(char *s_filename, char *s_fild_id);

//将得到的数据写入到Redis数据库中
int write_redis(char *file_id, char *url, char *filename, char *usr);

//从query_string字符串取得参数的值
int get_query_string(char *query, char *key, char *value, int *value_len_p);

//得到usr--用户名
int get_usr(char *usr);

//得到cmd
int get_cmd(char *cmd);

//得到fromId--从哪个Id开始
int get_fromId(char *fromId);

//得到count--一共显示几个文件
int get_cnt(char *count);


