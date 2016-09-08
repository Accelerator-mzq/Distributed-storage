#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

#include "make_log.h"
#include "redis_op.h"

#define LOG_MAIN_TEST   "test"
#define LOG_MAIN_PROC   "main-test"

int main(int argc, char *argv[])
{
    redisContext *conn = NULL;
    char *ip = "127.0.0.1";
    char *port = "6379";
    char tmp[256] = {0};
    conn = rop_connectdb_nopwd(ip, port);
    if (conn == NULL)
    {
        LOG(LOG_MAIN_TEST, LOG_MAIN_PROC, "conn redis server error");
        exit(1);
    }

    printf("connect server succ!\n");

    rop_set_string(conn, "itcast", "0512");
    if (rop_get_string(conn, "itcast", tmp) != -1)
    {
        printf("tmp = %s\n", tmp);
    }
    else
    {
        printf("not found\n");
    }


    rop_disconnect(conn);

	return 0;
}
