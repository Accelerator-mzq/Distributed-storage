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

#define UPLOAD_MODULE "upload"
#define UPLOAD_PROC   "upload_test"

#define FDFS_CLIENT_MODULE "fdfs_client"
#define FDFS_CLIENT_PROC   "fdfs_test"

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


