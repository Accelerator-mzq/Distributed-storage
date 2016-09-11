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

#define GET_CMD_MODULE      "get_cmd"
#define GET_CMD_PROC        "cmd"

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




//从redis读取key:FILE_INFO_LIST的value值，并且封装成json形式
int read_redis_to_json(int fromId, int cnt, char *cmd);

//从redis的value值中提取出各个字段的数据
//  with_quote 该字段是否有“”包括 1 是 其他否   //
//  word_num   字段号                             //
//  redis_value    redis的value值的首地址         //
//  max_len        数据最大长度                   //
int get_value_by_redis(char *redis_value, int word_num, char *value, int max_len);
