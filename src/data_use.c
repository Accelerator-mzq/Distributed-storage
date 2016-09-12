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
                end = strstr(begin, key);
                begin = end + key_len;

                if ((i == 0))
                {
                    first = begin;
                }
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
    int score = 0;

    cJSON *root = NULL;
    cJSON *array = NULL;

    char *out = NULL;
    char filename[256] = {0};
    char file_id[256] = {0};
    char create_time[64] = {0};
    char suffix[SUFFIX_LEN] = {0};
    char url[256] = {0};
    char usr[128] = {0};
    char picurl[PIC_URL_LEN] = {0};
    char pic_name[PIC_NAME_LEN] = {0};
    char get_ip_buf[PIC_URL_LEN] = {0};

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
        memset(file_id, 0, 256);
        get_value_by_redis(file_list_values[i], 1, file_id, VALUES_ID_SIZE-1);
        cJSON_AddStringToObject(item, "id", file_id);

        //kind
        cJSON_AddNumberToObject(item, "kind", 2);

        //title_m(filename)
        memset(filename, 0, 256);
        get_value_by_redis(file_list_values[i], 3, filename, VALUES_ID_SIZE-1);
        LOG(READ_REDIS_MODULE, READ_REDIS_PROC, "filename = %s", filename);

        cJSON_AddStringToObject(item, "title_m", filename);

        //title_s
        memset(usr, 0, 128);
        get_value_by_redis(file_list_values[i], 5, usr, VALUES_ID_SIZE-1);
        LOG(READ_REDIS_MODULE, READ_REDIS_PROC, "usr = %s", usr);
        cJSON_AddStringToObject(item, "title_s", usr);

        //time
        memset(create_time, 0, 64);
        get_value_by_redis(file_list_values[i], 4, create_time, VALUES_ID_SIZE-1);
        cJSON_AddStringToObject(item, "descrip", create_time);
        LOG(READ_REDIS_MODULE, READ_REDIS_PROC, "create_time = %s", create_time);

        //url
        memset(url, 0, 256);
        get_value_by_redis(file_list_values[i], 2, url, VALUES_ID_SIZE-1);
        cJSON_AddStringToObject(item, "url", url);
        LOG(READ_REDIS_MODULE, READ_REDIS_PROC, "url = %s", url);

        //picurl_m
        //从url里提取出到ip为止的字符串
        memset(picurl, 0, PIC_URL_LEN);
        if (get_ip_url(url, get_ip_buf) < 0)
        {
            LOG(READ_REDIS_MODULE, READ_REDIS_PROC, "get ip_url error");
        }
        strcat(picurl, get_ip_buf);
        strcat(picurl, "/static/file_png/");

        get_file_suffix(filename, suffix);
        sprintf(pic_name, "%s.png", suffix);
        strcat(picurl, pic_name);

        cJSON_AddStringToObject(item, "picurl_m", picurl);

        //pv
        score = rop_zset_get_score(redis_conn, FILE_HOT_ZSET, file_id);
        cJSON_AddNumberToObject(item, "pv", score-1);

        //hot
        cJSON_AddNumberToObject(item, "hot", 0);


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

//从url里提取出到ip为止的字符串
int get_ip_url(char *s_url, char *s_get_ip_buf)
{
    char *url = s_url;
    char *begin = NULL;
    char *end = NULL;
    
    char key[] = "/group";

    int len = 0;

    if (s_url == NULL || s_get_ip_buf == NULL)
    {
        LOG(GET_IP_URL_MODULE, GET_IP_URL_PROC, "s_url == NULL || s_get_ip_buf == NULL");
        return -1;
    }
    
    begin = url;
    if ((end = strstr(url, key)) != NULL)
    {
        len = end - begin;
        strncpy(s_get_ip_buf, begin, len);
        s_get_ip_buf[len] = '\0';
        LOG(GET_IP_URL_MODULE, GET_IP_URL_PROC, "s_get_ip_buf =%s", s_get_ip_buf);
    }
    else
    {
        strncpy(s_get_ip_buf, "null", 5);
    }

    return 0;

}

//将FILE_HOT_ZSET中的对应的file_id的值加1
int increase_file_pv(char *file_id)
{
    redisContext *redis_conn = NULL;

    redis_conn = rop_connectdb_nopwd("127.0.0.1", "6379");
    if (redis_conn == NULL)
    {
        LOG(ADD_PV_MODULE, ADD_PV_PROC, "redis_conn == NULL");
        return -1;
    }

    rop_zset_increment(redis_conn, FILE_HOT_ZSET, file_id);

    rop_disconnect(redis_conn);

    return 0;
}

//对字符串中的子串进行替换
int str_replace(char *src, char *key, char *replace)
{
    int keyLen = 0;
    int len = 0;
    int srcLen = 0;
    int count = 0;
    
    char *p = NULL;
    char *q = NULL;
    char *str_buf = NULL;;

    if (src == NULL || key == NULL || replace == NULL)
    {
        LOG(STR_REPLACE_MODULE, STR_REPLACE_PROC, "src == NULL || key == NULL || replace == NULL");
        return -1;
    }

    srcLen = strlen(src);
    keyLen = strlen(key);
    
    str_buf = (char *)malloc(sizeof(char) * srcLen);
    if (str_buf == NULL)
    {
        LOG(STR_REPLACE_MODULE, STR_REPLACE_PROC, "str_buf malloc error");
        return -1;
    }


    q = src;

    while ((p = strstr(q, key)) != NULL)
    {
        count++;
        len = p - q;
        LOG(STR_REPLACE_MODULE, STR_REPLACE_PROC, "len = %d", len);
        if (count == 1)
        {
            strncpy(str_buf, q, len);
        }
        else
        {
            strncat(str_buf, q, len);
        }
        LOG(STR_REPLACE_MODULE, STR_REPLACE_PROC, "str_buf1 = %s", str_buf);
        strcat(str_buf, replace);
        LOG(STR_REPLACE_MODULE, STR_REPLACE_PROC, "str_buf2 = %s", str_buf);
        q = p + keyLen; 
        LOG(STR_REPLACE_MODULE, STR_REPLACE_PROC, "q = %s", q);
    }

    if (*q != '\0')
    {
        strcat(str_buf, q);
    }
    LOG(STR_REPLACE_MODULE, STR_REPLACE_PROC, "str_buf:%s", str_buf);

    memset(src, 0, srcLen+1);
    strncpy(src, str_buf, srcLen);
    LOG(STR_REPLACE_MODULE, STR_REPLACE_PROC, "src:%s", src);

    free(str_buf);

    return 0;
}
