#include <stdio.h>
#include <stdlib.h>

#include "usefull.h"



int get_buf(char **src, int len)
{
    char *buf = NULL;

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

int upload_file(char *src, int len)
{
    int i;
    int len1 = 0;
    int len2 = 0;
    int nameLen = 0;

    char *p = NULL;
    char *q = NULL;
    char *s = NULL;
    char key[] = "\r\n";
    char key1[] = "---------";
    char key2[] = "filename=";
    char *filename = NULL;
    FILE *fp;

    q = src;

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
    filename = (char *)malloc(sizeof(nameLen + 1));
    if (filename == NULL)
    {
        LOG(UPLOAD_MODULE, UPLOAD_PROC, "filename malloc error");
        return -1;
    }
    memset(filename, 0, nameLen+1);
    strncpy(filename, p, nameLen);
    filename[nameLen] = '\0';


    //提取出图片的二进制数据
    q = src;
    for (i = 0; i < 4; i++)
    {
        p = strstr(q, key);
        q = p+1;
        //LOG(UPLOAD_MODULE, UPLOAD_PROC, "p, p_address:%p",p);
    }
    p += 2;
    q = src;

    len1 = len - (p - q);
    s = memstr(p, len1, key1);

    s -= 2;
    len2 = s - p;

    fp = fopen(filename, "wb+");
    fwrite(p, 1, len2, fp);

    free(filename);
    fclose(fp);


    return 0;

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
