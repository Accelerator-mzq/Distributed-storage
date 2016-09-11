#include <stdio.h>
#include <stdlib.h>

#include "data_use.h"


//从redis的value值中提取出各个字段的数据
//  word_num   字段号                             //
//  redis_value    redis的value值的首地址         //
//  max_len        数据最大长度                   //
int get_value_by_redis(char *redis_value, int word_num, char *value, int max_len)
{
    int ret = 0;
    int value_len = 0;
    int i = 0;
    int key_len = 0;
    int key_error_len = 0;

    char *begin = NULL;
    char *end = NULL;
    char *first = NULL;
    char *p = NULL;
    char key[] = "||";
    char key_error[] = "||||";

    if (word_num < 1 && word_num > 6)
    {
        fprintf(stderr, "word_num must > 1 && < 7\n");
        ret = 1;
        goto END;
    }

    begin = redis_value;
    key_len = strlen(key);
    key_error_len = strlen(key_error);

    switch (word_num)
    {
        case 1:
            end = strstr(begin, key);
            value_len = ((end - begin) > max_len) ? max_len : (end - begin);
            strncpy(value, begin, value_len);            
            LOG(GET_REDIS_VALUE_MODULE, GET_REDIS_VALUE_PROC, "file_id:%s", value);
            break;
        case 2:
            for (i = 0; i < 2; i++)
            {
                LOG(GET_REDIS_VALUE_MODULE, GET_REDIS_VALUE_PROC, "ooooooooooooo");
                end = strstr(begin, key);
                begin = end + key_len;

                if ((i == 0))
                {
                    first = begin;
                }
                LOG(GET_REDIS_VALUE_MODULE, GET_REDIS_VALUE_PROC, "begin:%s, i = %d", begin, i);
                LOG(GET_REDIS_VALUE_MODULE, GET_REDIS_VALUE_PROC, "first:%s, i = %d", first, i);
            }
            value_len = ((end - first) > max_len) ? max_len : (end - first);
            strncpy(value, first, value_len);
            LOG(GET_REDIS_VALUE_MODULE, GET_REDIS_VALUE_PROC, "url:%s", value);
            break;
        case 3:
            for (i = 0; i < 3; i++)
            {
                end = strstr(begin, key);
                begin = end + key_len;
                
                if ((i == 1))
                {
                    first = begin;
                }
            }

            value_len = ((end - first) > max_len) ? max_len : (end - first);
            strncpy(value, first, value_len);
            LOG(GET_REDIS_VALUE_MODULE, GET_REDIS_VALUE_PROC, "filename:%s", value);
            break;
        case 4:
            p = strstr(begin, key_error);
            if (p != NULL)
            {
                for (i = 0; i < 3; i++)
                {
                    end = strstr(begin, key);
                    begin = end + key_len;
                }
                end = p;
                value_len = ((end - begin) > max_len) ? max_len : (end - begin);
                strncpy(value, begin, value_len);
                LOG(GET_REDIS_VALUE_MODULE, GET_REDIS_VALUE_PROC, "create_time:%s", value);
            }
            else
            {
                for (i = 0; i < 4; i++)
                {
                    end = strstr(begin, key);
                    begin = end + key_len;
                    if ((i == 2))
                    {
                        first = begin;
                    }
                }
                value_len = ((end - first) > max_len) ? max_len : (end - first);
                strncpy(value, first, value_len);
                LOG(GET_REDIS_VALUE_MODULE, GET_REDIS_VALUE_PROC, "create_time:%s", value);
            }
            break;
        case 5:
            p = strstr(begin, key_error);
            if (p != NULL)
            {
                memset(value, 0, 10);
                LOG(GET_REDIS_VALUE_MODULE, GET_REDIS_VALUE_PROC, "user:%s", value);
            }
            else
            {
                for (i = 0; i < 5; i++)
                {
                    end = strstr(begin, key);
                    begin = end + key_len;
                    if ((i == 3))
                    {
                        first = begin;
                    }
                }
                value_len = ((end - first) > max_len) ? max_len : (end - first);
                strncpy(value, first, value_len);
                LOG(GET_REDIS_VALUE_MODULE, GET_REDIS_VALUE_PROC, "user:%s", value);
            }
            break;
        case 6:
            p = strstr(begin, key_error);
            if (p != NULL)
            {
                p = p + key_error_len;
                strcpy(value, p);
                LOG(GET_REDIS_VALUE_MODULE, GET_REDIS_VALUE_PROC, "kind:%d", value);
            }
            else
            {
                for (i = 0; i < 5; i++)
                {
                    end = strstr(begin, key);
                    begin = end + key_len;
                }
                value_len = (strlen(begin) > max_len) ? max_len : strlen(begin);
                strncpy(value, begin, value_len);
                LOG(GET_REDIS_VALUE_MODULE, GET_REDIS_VALUE_PROC, "kind:%d", value);
            }
            break;
        default:
            LOG(GET_REDIS_VALUE_MODULE, GET_REDIS_VALUE_PROC, "read redis error");
            break;

    }

END:
    return ret;
}

//从redis读取key:FILE_INFO_LIST的value值，并且封装成json形式
int read_redis_to_json(int fromId, int cnt, char *cmd)
{
    int i = 0;
    int retn = 0;
    int value_num = 0;
    int endId = fromId + cnt - 1;

    cJSON *root = NULL;
    cJSON *array = NULL;

    char *out = NULL;
    char filename[256] = {0};
    char file_id[256] = {0};
    char create_time[64] = {0};
    //char suffix[10] = {0};
    char url[256] = {0};
    char usr[128] = {0};

    RVALUES file_list_values = NULL;


    redisContext *redis_conn = NULL;
    char *ip = "127.0.0.1";
    char *port = "6379";
    redis_conn = rop_connectdb_nopwd(ip, port);
    if (redis_conn == NULL)
    {
        LOG(READ_REDIS_MODULE, READ_REDIS_PROC, "redis_conn error");
        exit(1);
    }

    LOG(READ_REDIS_MODULE, READ_REDIS_PROC, "fromId = %d, count = %d", fromId, cnt);
    file_list_values = (RVALUES)malloc(cnt * VALUES_ID_SIZE);

    retn = rop_range_list(redis_conn, FILE_INFO_LIST, fromId, endId, file_list_values, &value_num);
    if (retn < 0)
    {
        LOG(READ_REDIS_MODULE, READ_REDIS_PROC, "redis range list error");
        rop_disconnect(redis_conn);
        return -1;
    }
    LOG(READ_REDIS_MODULE, READ_REDIS_PROC, "value_num = %d", value_num);

    root = cJSON_CreateObject();
    array = cJSON_CreateArray();
    for (i = 0; i < value_num; i++)
    {
        cJSON *item = cJSON_CreateObject();

        //id
        get_value_by_redis(file_list_values[i], 1, file_id, VALUES_ID_SIZE-1);
        cJSON_AddStringToObject(item, "id", file_id);

        //kind
        cJSON_AddNumberToObject(item, "kind", 2);

        //title_m(filename)
        get_value_by_redis(file_list_values[i], 3, filename, VALUES_ID_SIZE-1);
        LOG(READ_REDIS_MODULE, READ_REDIS_PROC, "filename = %s", filename);

        cJSON_AddStringToObject(item, "title_m", filename);

        //title_s
        get_value_by_redis(file_list_values[i], 5, usr, VALUES_ID_SIZE-1);
        LOG(READ_REDIS_MODULE, READ_REDIS_PROC, "usr = %s", usr);
        cJSON_AddStringToObject(item, "title_s", usr);

        //time
        get_value_by_redis(file_list_values[i], 4, create_time, VALUES_ID_SIZE-1);
        cJSON_AddStringToObject(item, "descrip", create_time);
        LOG(READ_REDIS_MODULE, READ_REDIS_PROC, "create_time = %s", create_time);

        //url
        LOG(READ_REDIS_MODULE, READ_REDIS_PROC, "uuuuuuuuuuuuuuuuuuuuuuuuuu-");
        get_value_by_redis(file_list_values[i], 2, url, VALUES_ID_SIZE-1);
        cJSON_AddStringToObject(item, "url", url);
        LOG(READ_REDIS_MODULE, READ_REDIS_PROC, "--------------------");

        cJSON_AddItemToArray(array, item);

    }

    cJSON_AddItemToObject(root, "games", array);

    out = cJSON_Print(root);

    LOG(READ_REDIS_MODULE, READ_REDIS_PROC, "out = %s", out);
    printf("%s\n", out);

    free(file_list_values);
    free(out);

    rop_disconnect(redis_conn);

    return 0;
}
