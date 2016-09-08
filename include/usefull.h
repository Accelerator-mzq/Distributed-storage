#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <unistd.h>
#include <pthread.h>
#include <sys/wait.h>

#include "fcgi_stdio.h"
#include "make_log.h"

#define UPLOAD_MODULE "upload"
#define UPLOAD_PROC   "upload_test"

#define FDFS_CLIENT_MODULE "fdfs_client"
#define FDFS_CLIENT_PROC   "fdfs_test"

#define FILE_ID_LEN     (256)

//开辟内存空间，保存上传文件以及htpp协议里的全部数据
int get_buf(char **src, int len);

//提取出数据中文件名，二进制数据，完成上传
int upload_file(char *src, int len, char **s_filename);

//从二进制字符串中查找子串
char* memstr(char* full_data, int full_data_len, char* substr);

//使用fdfs将文件传入storage
int fdfs_client(char *s_filename, char *s_fild_id);


