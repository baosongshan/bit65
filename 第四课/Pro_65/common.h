#ifndef _COMMON_H_
#define _COMMON_H_

#include<unistd.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<assert.h>
#include<malloc.h>

#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>

#include<pwd.h>
#include<shadow.h>
#include<crypt.h>

#include<dirent.h>

#include<sys/stat.h>
#include<time.h>

#define ERR_EXIT(m) \
	do{\
	perror(m);\
	exit(EXIT_FAILURE);\
	}while(0)

#define MAX_BUFFER_SIZE 1024
#define MAX_COMMAND_LINE 1024
#define MAX_COMMAND         32
#define MAX_ARG          1024

#endif /* _COMMON_H_ */