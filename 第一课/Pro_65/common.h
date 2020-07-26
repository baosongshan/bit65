#ifndef _COMMON_H_
#define _COMMON_H_

#include<unistd.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>

#define ERR_EXIT(m) \
	do{\
	perror(m);\
	exit(EXIT_FAILURE);\
	}while(0)

#endif /* _COMMON_H_ */