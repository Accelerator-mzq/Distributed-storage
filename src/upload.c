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


static void PrintEnv(char *label, char **envp)
{
    printf("%s:<br>\n<pre>\n", label);
    for ( ; *envp != NULL; envp++) {
        printf("%s\n", *envp);
    }
    printf("</pre><p>\n");
}


int main ()
{
    char **initialEnv = environ;
    int count = 0;
    int ret = 0;
    char *buf = NULL;
    

    while (FCGI_Accept() >= 0) 
    {
        char *contentLength = getenv("CONTENT_LENGTH");
        int len;

        printf("Content-type: text/html\r\n"
                "\r\n"
                "<title>FastCGI echo</title>"
                "<h1>FastCGI echo</h1>\n"
                "Request number %d,  Process ID: %d<p>\n", ++count, getpid());

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
                goto END;
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

                putchar(ch);
            }
            *p++ = '\0';

            //完成上传文件操作，提取数据
            if (upload_file(buf, len) != 0)
            {
                LOG(UPLOAD_MODULE, UPLOAD_PROC, "upload_file error");
                ret = -1;
                goto END;
            }


            printf("\n</pre><p>\n");
        }


        PrintEnv("Request environment", environ);
        PrintEnv("Initial environment", initialEnv);
    } /* while */

END:
    free(buf);
    if (ret == -1)
    {
        return -1;
    }
    return 0;
}

