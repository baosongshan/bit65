#ifndef _COMMON_H_
#define _COMMON_H_

#include<unistd.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<assert.h>
#include<malloc.h>
#include<fcntl.h>
#include<errno.h>

#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<netdb.h>
#include<signal.h>

#include<pwd.h>
#include<shadow.h>
#include<crypt.h>

#include<dirent.h>

#include<sys/stat.h>
#include<time.h>

#include<linux/capability.h>
#include<sys/syscall.h>

#define ERR_EXIT(m) \
	do{\
	perror(m);\
	exit(EXIT_FAILURE);\
	}while(0)

#define MAX_BUFFER_SIZE 1024
#define MAX_COMMAND_LINE 1024
#define MAX_COMMAND         32
#define MAX_ARG          1024
#define MAX_SETTING_LINE 1024
#define MAX_KEY_VALUE_SIZE 128

#define MAX_HOST_NAME_SIZE 128

#define MAX_BUCKET_SIZE    256

#endif /* _COMMON_H_ */