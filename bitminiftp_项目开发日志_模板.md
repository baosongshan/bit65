<h1 align=center> BitMiniFtp 项目 开发日志 </h1>
<h2 align=center> 第一天 </h2>
### 一、项目介绍
>1、【见BitMiniftp_开发文档.pdf】

### 二、开发环境搭建
>1、安装vsftpd
>
>```
>yum install vsftpd -y
>```
>
>2、启动vsftpd
>
>```
>systemctl status vsftpd
>systemctl start  vsftpd
>systemctl stop   vsftpd
>systemctl restart vsftpd
>修改配置文件
>修改 /etc/vsftpd/vsftpd.conf
>```
>
>3、安装客户端工具lftp
>
>```
>yum install lftp -y
>```
>
>4、lftp的使用
>
>5、Windows下Leapftp的简单使用
>
>6、Windows下Editplus开发环境的配置

### 三、系统框架搭建
> #### 1、创建common.h模块
> ```C++
> #ifndef _COMMON_H_
> #define _COMMON_H_
> 
> #include<unistd.h>
> #include<stdio.h>
> #include<stdlib.h>
> #include<string.h>
> 
> #include<sys/socket.h>
> #include<netinet/in.h>
> #include<arpa/inet.h>
> 
> #define ERR_EXIT(m) \
> 	do{\
> 	perror(m);\
> 	exit(EXIT_FAILURE);\
> 	}while(0)
> 
> #endif /* _COMMON_H_ */
> ```
> #### 2、创建sysutil模块
> ```C++
> #include"sysutil.h"
> 
> int tcp_server(const char *host, unsigned short port)
> {
> 	int listenfd;
> 	if((listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
> 		ERR_EXIT("tcp_server");
> 
> 	struct sockaddr_in addrSer;
> 	addrSer.sin_family = AF_INET;
> 	addrSer.sin_port = htons(port);
> 	addrSer.sin_addr.s_addr = inet_addr(host);
> 
> 	int on = 1;
> 	if(setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) < 0)
> 		ERR_EXIT("setsockopt");
> 
> 	if(bind(listenfd, (struct sockaddr*)&addrSer, sizeof(addrSer)) < 0)
> 		ERR_EXIT("bind");
> 
> 	if(listen(listenfd, SOMAXCONN) < 0)
> 		ERR_EXIT("listen");
> 
> 	return listenfd;
> }
> ```
> #### 3、创建会话模块 session
> ```c++
> #include"session.h"
> 
> void begin_session(session_t *sess)
> {
> 	pid_t pid = fork();
> 	if(pid == -1)
> 		ERR_EXIT("fork");
> 	if(pid == 0)
> 	{
> 		//ftp 服务进程
> 		handle_child(sess);
> 	}
> 	else
> 	{
> 		//nobody 进程
> 		handle_parent(sess);
> 	}
> }
> ```
> #### 4、创建ftp服务进程模块 ftpproto
> ```c++
> #include"ftpproto.h"
> 
> //ftp 服务进程
> void handle_child(session_t *sess)
> {
> 	send(sess->ctrl_fd,  "220 (bit miniftp 1.0)\n\r", strlen("220 (bit miniftp 1.0)\n\r"), 0);
> 
> 	while(1)
> 	{
> 		//不停的等待客户端的命令并进行处理
> 	}
> }
> 
> ```
> #### 5、创建nobody进程模块 privparent
> ```c++
> #include"privparent.h"
> 
> //nobody 进程
> void handle_parent(session_t *sess)
> {
> 	while(1)
> 	{
> 		//不停的等待ftp进程的消息
> 	}
> }
> ```
> #### 6、主进程模块 bitminiftp
> ```c++
> #include"common.h"
> #include"sysutil.h"
> #include"session.h"
> 
> int main(int argc, char *argv[])
> {
> 	session_t sess = 
> 	{
> 		/* 控制连接 */
> 		-1
> 	};
> 
> 	int listenfd = tcp_server("192.168.232.10", 9188); 
> 
> 	int sockConn;
> 	struct sockaddr_in addrCli;
> 	socklen_t addrlen;
> 	while(1)
> 	{
> 		if((sockConn = accept(listenfd, (struct sockaddr*)&addrCli, &addrlen)) < 0)
> 			ERR_EXIT("accept");
> 
> 		pid_t pid = fork();
> 		if(pid == -1)
> 			ERR_EXIT("fork");
> 
> 		if(pid == 0)
> 		{
> 			//Child Process
> 			close(listenfd);
> 			
> 			//会话
> 			sess.ctrl_fd = sockConn;
> 			begin_session(&sess);
> 			exit(EXIT_SUCCESS);
> 		}
> 		else 
> 		{
> 			//Parent Process
> 			close(sockConn);
> 		}
> 	}
> 
> 	close(listenfd);
> 	return 0;
> }
> ```
> #### 6、创建Makefile文件
> ```makefile
> CC = gcc
> CFLAGS = -g
> OBJS = sysutil.o session.o ftpproto.o privparent.o bitminiftp.o
> LIBS = 
> BIN  = bitminiftp
> 
> $(BIN):$(OBJS)
> 	$(CC) $(CFLAGS) $^ -o $@ $(LIBS)
> %.o:%.c
> 	$(CC) $(CFLAGS) -c $< -o $@
> 
> .PHONY:clean
> clean:
> 	rm -fr *.o $(BIN)
> ```

<h2 align=center> 第二天 </h2>
```

```
<h2 align=center> 第三天 </h2>
```

```
<h2 align=center> 第四天 </h2>
```

```
<h2 align=center> 第五天 </h2>
```

```



