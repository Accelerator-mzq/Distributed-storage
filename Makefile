
CC=gcc
CPPFLAGS= -I./include -I/usr/local/include/hiredis/
CFLAGS=-Wall 
LIBS=-lhiredis -lpthread -lfcgi -lm

#找到当前目录下所有的.c文件
src = $(wildcard *.c ./test/*.c ./src/*.c)

#将当前目录下所有的.c  转换成.o给obj
obj = $(patsubst %.c, %.o, $(src))

my_test_cJSON=test/my_test_cJSON
cJSON_test=test/cJSON_test
upload=src/upload
fcgi_test=test/fcgi_test
redis_test=test/redis_test
fdfs_test=test/fdfs_test
test_main=test/test_main

target=$(test_main) $(fdfs_test) $(redis_test) $(fcgi_test) $(upload) $(cJSON_test) $(my_test_cJSON)


ALL:$(target)


#生成所有的.o文件
$(obj):%.o:%.c
	$(CC) -c $< -o $@ $(CPPFLAGS) $(CFLAGS) 


#test_main程序
$(test_main): test/test_main.o src/make_log.o src/redis_op.o 
	$(CC) $^ -o $@ $(LIBS)

#fdfs_est程序
$(fdfs_test): test/fdfs_client_test.o src/make_log.o
	$(CC) $^ -o $@ $(LIBS)

#redis_test程序
$(redis_test): test/test_redis_api.o src/make_log.o
	$(CC) $^ -o $@ $(LIBS)

#fcgi_test程序
$(fcgi_test): test/fcgi_test.o src/make_log.o
	$(CC) $^ -o $@ $(LIBS)

#upload程序
$(upload): src/upload.o src/usefull.o src/redis_op.o src/make_log.o
	$(CC) $^ -o $@ $(LIBS)

#cJSON_test程序
$(cJSON_test): test/cJSON_test.o src/cJSON.o src/make_log.o 
	$(CC) $^ -o $@ $(LIBS)

#my_test_cJSON程序
$(my_test_cJSON): test/my_test_cJSON.o src/cJSON.o src/make_log.o 
	$(CC) $^ -o $@ $(LIBS)

#clean指令

clean:
	-rm -rf $(obj) 

distclean:
	-rm -rf $(obj) $(target)

#将clean目标 改成一个虚拟符号
.PHONY: clean ALL distclean
