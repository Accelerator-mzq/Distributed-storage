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

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#ifdef _WIN32
#include <process.h>
#else
extern char **environ;
#endif

#include <hiredis.h>
#include "fcgi_stdio.h"
#include "data_use.h"
#include "usefull.h"

#define DATA_LOG_MODULE     "data_cgi"
#define DATA_LOG_PROC       "data_test"


int main ()
{
    char cmd[20] = {0};
    char fromId[64] = {0};
    char cnt[5] = {0};
    char fileId[FILE_ID_LEN] = {0};

    while (FCGI_Accept() >= 0) {
#if 1
        //char *query = getenv("QUERY_STRING");
        memset(cmd, 0, 20);
        memset(fromId, 0, 64);
        memset(cnt, 0, 5);
        memset(fileId, 0, FILE_ID_LEN);
        
        get_cmd(cmd);
        
        if (strcmp(cmd, "newFile") == 0)
        {
            get_fromId(fromId);
            get_cnt(cnt);
            LOG(DATA_LOG_MODULE, DATA_LOG_PROC, "=== fromId:%s, count:%s, cmd:%s", fromId, cnt, cmd);

	        printf("Content-type: text/html\r\n");
            printf("\r\n");
            
            read_redis_to_json(atoi(fromId), atoi(cnt), cmd);

        }
        else if (strcmp(cmd, "increase") == 0)
        {
            //char *query = getenv("QUERY_STRING");
            //文件被点击

            //得到被点击的fileId
            get_file_id(fileId);
            LOG(DATA_LOG_MODULE, DATA_LOG_PROC, "=== fileId:%s, cmd:%s", fileId, cmd);

            if (str_replace(fileId, "%2F", "/") != 0)
            {
                LOG(DATA_LOG_MODULE, DATA_LOG_PROC, "str_replace error");
                return -1;
            }

            increase_file_pv(fileId);

	        printf("Content-type: text/html\r\n");
            printf("\r\n");
        }
#endif

    } /* while */

    return 0;
}
