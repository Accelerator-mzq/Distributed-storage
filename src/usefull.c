#include <stdio.h>
#include <stdlib.h>

#include "usefull.h"
#include "redis_op.h"
#include "cJSON.h"
#include <hiredis.h>


#define FDFS_CLIENT_MODULE "fdfs_client"
#define FDFS_CLIENT_PROC   "fdfs_test"

#define REDIS_WRITE_MODULE  "Redis-write"
#define REDIS_WRITE_PROC   "Re-wt"


int trim_space(char *src)
{
    char *str = NULL;
    char *begin = NULL;
    char *end = NULL;

    int len = 0;

    if ((str = NULL))
    {
        printf("src = NULL\n");
        return -1;
    }

    str = src;
    begin = str;

    end = strchr(begin, '\0');
    end--;

    while (isspace(*begin) && *begin != '\0')
    {
        begin++;
    }

    while (isspace(*end) && (end - begin) > 0)
    {
        end--;
    }

    len = end - begin + 1;
    strncpy(str, begin, len);
    str[len] = '\0';

    return 0;

}

int get_buf(char **src, int len)
{
    char *buf = NULL;

    LOG(UPLOAD_MODULE, UPLOAD_PROC, "len = %d", len);
    buf = (char *)malloc(sizeof(char) * (len + 1));
    if (buf == NULL)
    {
        //printf("malloc error\n");
        LOG(UPLOAD_MODULE, UPLOAD_PROC, "malloc error");
        return -1;
    }
    memset(buf, 0, (len + 1));
    *src = buf;

    return 0;
}

int upload_file(char *src, int len, char *s_filename)
{
    int i;
    int len1 = 0;
    int len2 = 0;
    int nameLen = 0;
    int retn = 0;

    char *p = NULL;
    char *q = NULL;
    char *s = NULL;
    char key[] = "\r\n";
    char key1[] = "---------";
    char key2[] = "filename=";
    char *filename = NULL;
    FILE *fp;

    q = src;
    filename = s_filename;
    //提取出文件的名字
    p = strstr(q, key2);
    p += 10;

    for (i = 0; i < 2; i++)
    {
        s = strstr(q, key);
        q = s + 1;
    }
    q -= 2;

    nameLen = q - p;
    LOG(UPLOAD_MODULE, UPLOAD_PROC, "nameLen:%d", nameLen);
#if 0
    filename = (char *)malloc(sizeof(nameLen + 1));
    if (filename == NULL)
    {
        LOG(UPLOAD_MODULE, UPLOAD_PROC, "filename malloc error");
        return -1;
    }
#endif
    LOG(UPLOAD_MODULE, UPLOAD_PROC, "xxxxxxxxxx");
    memset(filename, 0, nameLen+1);
    strncpy(filename, p, nameLen);
    LOG(UPLOAD_MODULE, UPLOAD_PROC, "cccccccccc");
    filename[nameLen] = '\0';

    LOG(UPLOAD_MODULE, UPLOAD_PROC, "filename:%s", filename);

    //提取出图片的二进制数据
    q = src;
    for (i = 0; i < 4; i++)
    {
        p = strstr(q, key);
        q = p+1;
        //LOG(UPLOAD_MODULE, UPLOAD_PROC, "p, p_address:%p",p);
    }
    LOG(UPLOAD_MODULE, UPLOAD_PROC, "bbbbbbbbbbbbbbbbbbbbbb");
    p += 2;
    q = src;

    len1 = len - (p - q);
    s = memstr(p, len1, key1);

    s -= 2;
    len2 = s - p;
#if 1
    LOG(UPLOAD_MODULE, UPLOAD_PROC, "ccccccccccccccc");
    LOG(UPLOAD_MODULE, UPLOAD_PROC, "len2:%d", len2);
    fp = fopen(filename, "wb+");
    if (fp == NULL)
    {
        LOG(UPLOAD_MODULE, UPLOAD_PROC, "fp == NULL");
        retn = -1;
        goto END;
    }
    LOG(UPLOAD_MODULE, UPLOAD_PROC, "eeeeeeeeeeeeee");
    fwrite(p, 1, len2, fp);
    LOG(UPLOAD_MODULE, UPLOAD_PROC, "ddddddddddddddd");
#endif
#if 0
    int fd = 0;
    LOG(UPLOAD_MODULE, UPLOAD_PROC, "+++++++++++");
    fd = open(filename, O_CREAT|O_WRONLY, 0644);
    if (fd < 0)
    {
        LOG(UPLOAD_MODULE, UPLOAD_PROC, "open %s error", filename);
        retn = -1;
        goto END;
    }
    ftruncate(fd, len2);
    write(fd, p, len2);
#endif

    //*s_filename = filename;

    //close(fd);
    fclose(fp);

END:

    return retn;

}


char* memstr(char* full_data, int full_data_len, char* substr) 
{ 
    if (full_data == NULL || full_data_len <= 0 || substr == NULL) { 
        return NULL; 
    } 

    if (*substr == '\0') { 
        return NULL; 
    } 

    int sublen = strlen(substr); 

    int i; 
    char* cur = full_data; 
    int last_possible = full_data_len - sublen + 1; 
    for (i = 0; i < last_possible; i++) { 
        if (*cur == *substr) { 
            //assert(full_data_len - i >= sublen); 
            if (memcmp(cur, substr, sublen) == 0) { 
                //found
                return cur;
            }
        }
        cur++;
    }
    return NULL;
}

int fdfs_client(char *s_filename, char *s_file_id)
{
    char *file_name = s_filename;
    char *file_id = s_file_id;
    pid_t pid;


    int pfd[2] = {0};

    if (pipe(pfd) < 0) {
        LOG(FDFS_CLIENT_MODULE, FDFS_CLIENT_PROC, "[errror], pipe error");
        exit(1);
    }

    pid = fork();
    if (pid < 0)
    {
        LOG(FDFS_CLIENT_MODULE, FDFS_CLIENT_PROC, "[errror], fork error");
    }

    if (pid == 0) {
        //chlid
        //关闭读端
        close(pfd[0]);

        //将标准输出 重定向到管道中
        dup2(pfd[1], STDOUT_FILENO);

        //exec
        if (execlp("fdfs_upload_file", "fdfs_upload_file", FDFS_CLIENT_CONF, file_name, NULL) == -1)
        {
            LOG(FDFS_CLIENT_MODULE, FDFS_CLIENT_PROC, "exec fdfs_upload_file error");
            return -1;
        }
    }
    else {
        //parent
        //关闭写端
        close(pfd[1]);

        wait(NULL);

        //从管道中去读数据
        read(pfd[0], file_id, FILE_ID_LEN);
        trim_space(file_id);


        LOG(FDFS_CLIENT_MODULE, FDFS_CLIENT_PROC, "file_id = [%s]", file_id);
    }

    return 0;
}

int write_redis(char *file_id, char *fdfs_file_url, char *filename, char *usr)
{
    redisContext *conn = NULL;
    char *ip = "127.0.0.1";
    char *port = "6379";
    char *redis_value_buf= NULL;
    char create_time[TIME_STRING_LEN] = {0};
    char suffix[SUFFIX_LEN] = {0};

    int retn = 0;

    LOG(REDIS_WRITE_MODULE, REDIS_WRITE_PROC, "222222222222222");
    conn = rop_connectdb_nopwd(ip, port);
    if (conn == NULL)
    {
        LOG(REDIS_WRITE_MODULE, REDIS_WRITE_PROC, "conn redis server error");
        exit(1);
    }
    LOG(REDIS_WRITE_MODULE, REDIS_WRITE_PROC, "11111111111");

    printf("connect server succ!\n");

    redis_value_buf = (char *)malloc(VALUES_ID_SIZE);
    if (redis_value_buf == NULL)
    {
        LOG(REDIS_WRITE_MODULE, REDIS_WRITE_PROC, "redis_value_buf malloc error");
        retn = -1;
        goto END;
    }
    memset(redis_value_buf, 0, VALUES_ID_SIZE);


    //make redis_value_buf
    //file_id
    strcat(redis_value_buf, file_id);
    strcat(redis_value_buf, REDIS_DILIMIT);
    LOG(REDIS_WRITE_MODULE, REDIS_WRITE_PROC, "redis_value_buf--id--:%s", redis_value_buf);

    //url
    strcat(redis_value_buf, fdfs_file_url);
    strcat(redis_value_buf, REDIS_DILIMIT);
    LOG(REDIS_WRITE_MODULE, REDIS_WRITE_PROC, "redis_value_buf--url--:%s", redis_value_buf);

    //file_name
    strcat(redis_value_buf, filename);
    strcat(redis_value_buf, REDIS_DILIMIT);

    //create_time
    time_t now;
    now = time(NULL);
    strftime(create_time, TIME_STRING_LEN, "%Y-%m-%d %H-%M:%S", localtime(&now));
    trim_space(create_time);

    strcat(redis_value_buf, create_time);
    strcat(redis_value_buf, REDIS_DILIMIT);

    //user
    strcat(redis_value_buf, usr);
    strcat(redis_value_buf, REDIS_DILIMIT);

    //type
    //得到上传文件的后缀名
    get_file_suffix(filename, suffix);
    strcat(redis_value_buf, suffix);

    //将文件信息插入到FILE_INFO_LIST中
    rop_list_push(conn, FILE_INFO_LIST, redis_value_buf);

    //将文件信息插入到FILE_HOT_ZSET中，默认的权值为1
    rop_zset_increment(conn, FILE_HOT_ZSET, file_id);

    free(redis_value_buf);

END:
    rop_disconnect(conn);

    return retn;
}

//得到cmd
int get_cmd(char *cmd)
{
    char *query_string = getenv("QUERY_STRING");

    //得到cmd
    get_query_string(query_string, "cmd", cmd, NULL);
    if (strlen(cmd) == 0)
    {
        LOG(GET_CMD_MODULE, GET_CMD_PROC, "get cmd has no value!");
    }
    else
    {
        LOG(GET_CMD_MODULE, GET_CMD_PROC, "get cmd = [%s]", cmd);
    }

    return 0;
}

//得到fromId--从哪个Id开始
int get_fromId(char *fromId)
{
    char *query_string = getenv("QUERY_STRING");

    //得到fromId
    get_query_string(query_string, "fromId", fromId, NULL);
    if (strlen(fromId) == 0)
    {
        LOG(GET_FROMID_MODULE, GET_FROMID_PROC, "get fromId has no value!");
    }
    else
    {
        LOG(GET_FROMID_MODULE, GET_FROMID_PROC, "get fromId = [%s]", fromId);
    }

    return 0;
}

//得到count--一共显示几个文件
int get_cnt(char *cnt)
{
    char *query_string = getenv("QUERY_STRING");

    //得到count
    get_query_string(query_string, "count", cnt, NULL);
    if (strlen(cnt) == 0)
    {
        LOG(GET_COUNT_MODULE, GET_COUNT_PROC, "get count has no value!");
    }
    else
    {
        LOG(GET_COUNT_MODULE, GET_COUNT_PROC, "get count = [%s]", cnt);
    }

    return 0;
}

//得到usr--用户名
int get_usr(char *usr)
{
    char *query_string = getenv("QUERY_STRING");

    //得到count
    get_query_string(query_string, "user", usr, NULL);
    if (strlen(usr) == 0)
    {
        LOG(GET_USR_MODULE, GET_USR_PROC, "get user has no value!");
    }
    else
    {
        LOG(GET_USR_MODULE, GET_USR_PROC, "get user = [%s]", usr);
    }

    return 0;
}

//得到file_id
int get_file_id(char *file_id)
{
    char *query_string = getenv("QUERY_STRING");

    //得到file_id
    get_query_string(query_string, "fileId", file_id, NULL);
    if (strlen(file_id) == 0)
    {
        LOG(GET_FROMID_MODULE, GET_FROMID_PROC, "get file_id has no value!");
    }
    else
    {
        LOG(GET_FROMID_MODULE, GET_FROMID_PROC, "get file_id = [%s]", file_id);
    }

    return 0;
}

//从query_string字符串取得参数的值
int get_query_string(char *query, char *key, char *value, int *value_len_p)
{
    char *tmp = NULL;
    char *end = NULL;
    int ret = 0;
    int keyLen = 0;
    int valueLen = 0;


    tmp = strstr(query, key);
    if (tmp == NULL)
    {
        LOG(QUERY_STRING_MODULE, QUERY_STRING_PROC, "NOT FOUND %s key", key);
        ret = -1;
        goto END;
    }

    keyLen = strlen(key);
    tmp += keyLen;
    tmp++;

    end = tmp;

    while ('\0' != *end && '#' != *end && '&' != *end)
    {
        end++;
    }

    valueLen = end - tmp;

    strncpy(value, tmp, valueLen);
    value[valueLen] = '\0';

    if (value_len_p != NULL)
    {
        *value_len_p = valueLen;
    }


END:
    return ret;
}


//得到上传文件的url
int get_url(char *s_file_id, char *s_url)
{
    char *file_id = s_file_id;
    char *url = s_url;
    char *begin = NULL;
    char *end = NULL;

    char fdfs_buf[TEMP_BUF_MAX_LEN] = {0};
    char key[] = "source ip address:";
    char ip_buf[16] = {0};

    pid_t pid;

    int key_len = 0;
    int ip_len = 0;
    int pfd[2] = {0};
    int retn = 0;

    if (pipe(pfd) < 0) {
        LOG(GET_URL_MODULE, GET_URL_PROC, "[errror], pipe error");
        retn = -1;
        goto END;
    }

    key_len = strlen(key);
    pid = fork();
    if (pid < 0)
    {
        LOG(GET_URL_MODULE, GET_URL_PROC, "[errror], fork error");
        retn = -1;
        goto END;
    }

    if (pid == 0) {
        //chlid
        //关闭读端
        close(pfd[0]);

        //将标准输出 重定向到管道中
        dup2(pfd[1], STDOUT_FILENO);

        //exec
        if (execlp("fdfs_file_info", "fdfs_file_info", FDFS_CLIENT_CONF, file_id, NULL) == -1)
        {
            LOG(GET_URL_MODULE, GET_URL_PROC, "[errror], execlp error");
            retn = -1;
            goto END;
        }
    }
    else 
    {
        //parent
        //关闭写端
        close(pfd[1]);

        wait(NULL);

        //从管道中去读数据
        read(pfd[0], fdfs_buf, FILE_ID_LEN);
        LOG(GET_URL_MODULE, GET_URL_PROC, "fdfs_buf = %s\n", fdfs_buf);

        begin = strstr(fdfs_buf, key);
        begin = begin + key_len;
        end = strchr(begin, '\n');
        ip_len = end - begin;
        strncpy(ip_buf, begin, ip_len);
        ip_buf[ip_len] = '\0';
        trim_space(ip_buf);
        LOG(GET_URL_MODULE, GET_URL_PROC, "ip_buf = %s\n", ip_buf);

        strcat(url, "http://");
        strcat(url, ip_buf);
        strcat(url, "/");
        strcat(url, file_id);
        LOG(GET_URL_MODULE, GET_URL_PROC, "url = %s\n", url);


    }
END:

    return retn;

}

//得到上传文件的后缀名
int get_file_suffix(char *s_filename, char *s_suffix)
{
    char *src = s_filename;
    char *begin = NULL;
    char *end = NULL;
    char *suffix = s_suffix;

    char key[] = ".";

    int suffix_len = 0;

    if (s_filename == NULL || s_suffix == NULL)
    {
        LOG(GET_SUFFIX_MODULE, GET_SUFFIX_PROC, "s_filename == NULL || s_suffix == NULL");
        return -1;
    }

    end = src;
    while (*end != '\0')
    {
        end++;
    }

    if ((begin = strstr(src, key)) != NULL)
    {
        begin++;

        suffix_len = end - begin;
        if (suffix_len > 0)
        {
            strncpy(suffix, begin, suffix_len);
            suffix[suffix_len] = '\0';
            LOG(GET_SUFFIX_MODULE, GET_SUFFIX_PROC, "suffix = %s\n", suffix);
        }
        else
        {
            strncpy(suffix, "null", 5);
        }
    }
    else
    {
        strncpy(suffix, "null", 5);
    }

    return 0;

}
