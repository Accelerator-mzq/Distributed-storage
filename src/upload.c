/*
 * echo.c --
 *
 *	Produce a page containing all FastCGI inputs
 *
 *
 * Copyright (c) 1996 Open Market, Inc.
 *
 * See the file "LICENSE.TERMS" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 *
 */
#ifndef lint
static const char rcsid[] = "$Id: echo.c,v 1.5 1999/07/28 00:29:37 roberts Exp $";
#endif /* not lint */

#include "fcgi_config.h"

#include <stdlib.h>
#include <string.h>

#include <hiredis.h>

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#ifdef _WIN32
#include <process.h>
#else
extern char **environ;
#endif

#include "fcgi_stdio.h"
#include "make_log.h"
#include "usefull.h"

#define UPLOAD_MODULE "upload"
#define UPLOAD_PROC   "upload_test"


int main ()
{
    int ret = 0;
    char *buf = NULL;
    char filename[256] = {0};
    //char *filename = NULL;
    char file_id[256] = {0};
    char fdfs_file_url[256] = {0};
    char usr[64] = {0};
    

    while (FCGI_Accept() >= 0) 
    {
        char *contentLength = getenv("CONTENT_LENGTH");
        int len;

        printf("Content-type: text/html\r\n"
                "\r\n");

        if (get_usr(usr) < 0)
        {
            break;
        }

        LOG(UPLOAD_MODULE, UPLOAD_PROC, "get_usr:%s", usr);

        if (contentLength != NULL) {
            len = strtol(contentLength, NULL, 10);
        }
        else {
            len = 0;
        }

        if (len <= 0) {
            printf("No data from standard input.<p>\n");
        }
        else {
            int i, ch;
            char *p = NULL;

            //开辟空间
            if (get_buf(&buf, len) != 0)
            {
                LOG(UPLOAD_MODULE, UPLOAD_PROC, "get_buf error");
                ret = -1;
                goto FAIL;
            }
            p = buf;

            printf("Standard input:<br>\n<pre>\n");
            for (i = 0; i < len; i++) {
                if ((ch = getchar()) < 0) {
                    printf("Error: Not enough bytes received on standard input<p>\n");
                    break;
                }
                *p = ch;
                p++;

                //putchar(ch);
            }
            *p++ = '\0';


#if 1
            //完成上传文件操作，提取数据
            if (upload_file(buf, len, filename) != 0)
            {
                LOG(UPLOAD_MODULE, UPLOAD_PROC, "upload_file error");
                goto END;
            }
#endif

            //使用fdfs传入storage
            if (fdfs_client(filename, file_id) != 0)
            {
                LOG(UPLOAD_MODULE, UPLOAD_PROC, "fdfs_client error");
                goto END;
            }
            LOG(UPLOAD_MODULE, UPLOAD_PROC, "file_id:%s", file_id);

            //删除临时文件
            unlink(filename);

            //得到上传文件的url
            if (get_url(file_id, fdfs_file_url) != 0)
            {
                LOG(UPLOAD_MODULE, UPLOAD_PROC, "get_url error");
            }


            LOG(UPLOAD_MODULE, UPLOAD_PROC, "gogoggogogogo");
            //将得到数据写入到Redis数据库中
            strcpy(usr, "user");
            if (write_redis(file_id, fdfs_file_url, filename, usr) != 0)
            {
                LOG(UPLOAD_MODULE, UPLOAD_PROC, "write_redis error");
            }
            LOG(UPLOAD_MODULE, UPLOAD_PROC, "fdfs_file_url:%s", fdfs_file_url);


        }
        

END:
        memset(filename, 0, 256);
        memset(file_id, 0, 256);
        memset(fdfs_file_url, 0, 256);
        memset(usr, 0, 64);
    } /* while */

FAIL:

    return ret;
}

