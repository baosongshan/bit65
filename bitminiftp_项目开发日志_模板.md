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
### 一、命令映射
>#### 1、封装回应函数
>```C++
>void ftp_reply(session_t *sess, int code, const char *text)
>{
>	char buf[MAX_BUFFER_SIZE] = {0};
>	sprintf(buf, "%d %s\r\n", code, text);
>	send(sess->ctrl_fd, buf, strlen(buf), 0);
>}
>```
>#### 2、解析命令
>```C++
>//在session_t结构中增加成员
>typedef struct session
>{
>	/* 控制连接 */
>	int ctrl_fd;
>	char cmdline[MAX_COMMAND_LINE];
>	char cmd[MAX_COMMAND];
>	char arg[MAX_ARG];
>}session_t;
>
>
>//////////////////////////////////////////////////////////////////////////
>static void do_user(session_t *sess);
>static void do_pass(session_t *sess);
>
>typedef struct ftpcmd
>{
>	const char *cmd;
>	void(*cmd_handler)(session_t *sess);
>}ftpcmd_t;
>
>//命令映射表
>static ftpcmd_t ctrl_cmds[] = 
>{
>	{"USER",  do_user},
>	{"PASS",  do_pass}
>};
>
>//////////////////////////////////////////////////////////////////////////////
>void handle_child(session_t *sess)
>{
>	ftp_reply(sess, FTP_GREET, "(bit65 miniftp 1.0 )");
>
>	int ret;
>	while(1)
>	{
>		//不停的等待客户端的命令并进行处理
>		memset(sess->cmdline, 0, MAX_COMMAND_LINE);
>		memset(sess->cmd, 0, MAX_COMMAND);
>		memset(sess->arg, 0, MAX_ARG);
>		ret = recv(sess->ctrl_fd, sess->cmdline, MAX_COMMAND_LINE, 0);
>		if(ret == -1)
>			ERR_EXIT("readline");
>		else if(ret == 0)
>			exit(EXIT_SUCCESS);
>
>		str_trim_crlf(sess->cmdline);
>		printf("cmdline = %s\n", sess->cmdline);
>		str_split(sess->cmdline, sess->cmd, sess->arg, ' ');
>		printf("cmd = %s\n", sess->cmd);
>		printf("arg = %s\n", sess->arg);
>
>		int table_size = sizeof(ctrl_cmds) / sizeof(ftpcmd_t);
>		int i;
>		for(i=0; i<table_size; ++i)
>		{
>			if(strcmp(sess->cmd, ctrl_cmds[i].cmd) == 0)
>			{
>				if(ctrl_cmds[i].cmd_handler != NULL)
>					ctrl_cmds[i].cmd_handler(sess);
>				else
>					ftp_reply(sess, FTP_COMMANDNOTIMPL, "Unimplement command.");
>				break;
>			}
>		}
>
>		if(i >= table_size)
>			ftp_reply(sess, FTP_BADCMD, "Unknown command.");
>	}
>}
>```
>#### 3、登录、鉴权、用户名更改
>```C++
>//USER abc
>static void do_user(session_t *sess)
>{
>	struct passwd *pwd = getpwnam(sess->arg);
>	if(pwd != NULL)
>		sess->uid = pwd->pw_uid;
>
>	ftp_reply(sess, FTP_GIVEPWORD, "Please specify the password.");
>}
>
>//PASS 123abc
>static void do_pass(session_t *sess)
>{
>	//鉴权
>	struct passwd *pwd = getpwuid(sess->uid);
>	if(pwd == NULL)
>	{
>		ftp_reply(sess, FTP_LOGINERR, "Login incorrect.");
>		return;
>	}
>	
>	struct spwd *spd = getspnam(pwd->pw_name);
>	if(spd == NULL)
>	{
>		ftp_reply(sess, FTP_LOGINERR, "Login incorrect.");
>		return;
>	}
>
>	char *encry_pwd = crypt(sess->arg, spd->sp_pwdp);
>	if(strcmp(encry_pwd, spd->sp_pwdp) != 0)
>	{
>		ftp_reply(sess, FTP_LOGINERR, "Login incorrect.");
>		return;
>	}
>	
>	setegid(pwd->pw_gid);
>	seteuid(pwd->pw_uid);
>	chdir(pwd->pw_dir);
>
>	ftp_reply(sess, FTP_LOGINOK, "Login successful.");
>}
>
>```
>
<h2 align=center> 第三天 </h2>
####  一、实现命令 syst 
```C++
static void do_syst(session_t *sess)
{
	ftp_reply(sess, FTP_SYSTOK, "UNIX Type: L8");
}
```
#### 一、实现命令 feat 
```C++
static void do_feat(session_t *sess)
{
	send(sess->ctrl_fd, "211-Features:\r\n" ,strlen("211-Features:\r\n"), 0);
	send(sess->ctrl_fd, " EPRT\r\n", strlen(" EPRT\r\n"), 0);
	send(sess->ctrl_fd, " EPSV\r\n", strlen(" EPSV\r\n"), 0);
	send(sess->ctrl_fd, " MDTM\r\n", strlen(" MDTM\r\n"), 0);
	send(sess->ctrl_fd, " PASV\r\n", strlen(" PASV\r\n"), 0);
	send(sess->ctrl_fd, " REST STREAM\r\n", strlen(" REST STREAM\r\n"), 0);
	send(sess->ctrl_fd, " SIZE\r\n", strlen(" SIZE\r\n"), 0);
	send(sess->ctrl_fd, " TVFS\r\n", strlen(" TVFS\r\n"), 0);
	send(sess->ctrl_fd, " UTF8\r\n", strlen(" UTF8\r\n"), 0);
	send(sess->ctrl_fd, "211 End\r\n", strlen("211 End\r\n"), 0);
}
```
#### 一、实现命令 pwd 
```C++
static void do_pwd (session_t *sess)
{
	char buffer[MAX_BUFFER_SIZE] = {0};
	getcwd(buffer, MAX_BUFFER_SIZE);   //  /home/bss
	char msg[MAX_BUFFER_SIZE] = {0};
	sprintf(msg, "\"%s\"", buffer);    //  "/home/bss"
	ftp_reply(sess, FTP_PWDOK, msg);
}

```
#### 一、实现命令 type 
```C++
static void do_type(session_t *sess)
{
	//TYPE A  or  TYPE I
	if(strcmp(sess->arg, "A") == 0)
	{
		sess->is_ascii = 1;
		ftp_reply(sess, FTP_TYPEOK, "Switching to ASCII mode.");
	}
	else if(strcmp(sess->arg, "I") == 0)
	{
		sess->is_ascii = 0;
		ftp_reply(sess, FTP_TYPEOK, "Switching to Binary mode.");
	}
	else
		ftp_reply(sess, FTP_BADCMD, "Unrecognised TYPE command.");
}
```
#### 一、实现命令 port
```C++
static void do_port(session_t *sess)
{
	//PORT 192,168,232,1,7,34
	unsigned int v[6] = {0};
	sscanf(sess->arg, "%u,%u,%u,%u,%u,%u", &v[0],&v[1],&v[2],&v[3],&v[4],&v[5]);

	sess->port_addr = (struct sockaddr_in*)malloc(sizeof(struct sockaddr_in));
	unsigned char *p = (unsigned char *)&sess->port_addr->sin_port;
	p[0] = v[4];
	p[1] = v[5];

	p = (unsigned char *)&sess->port_addr->sin_addr;
	p[0] = v[0];
	p[1] = v[1];
	p[2] = v[2];
	p[3] = v[3];

	sess->port_addr->sin_family = AF_INET;
	ftp_reply(sess, FTP_PORTOK, "command successful. Consider using PASV.");
}
```
#### 一、实现命令 list
```C++
//首先建立数据连接
int port_active(session_t *sess)
{
	if(sess->port_addr)
		return 1;
	return 0;
}

int pasv_active(session_t *sess)
{
	return 0; //没有被激活
}

int get_transfer_fd(session_t *sess)
{
	if(!port_active(sess) && !pasv_active(sess))
	{
		ftp_reply(sess, FTP_BADSENDCONN,"Use PORT or PASV first.");
		return 0;
	}

	int ret = 1;
	//port
	if(port_active(sess))
	{
		int sock = tcp_client();
		if(connect(sock, (struct sockaddr*)sess->port_addr, sizeof(struct sockaddr)) < 0)
			ret = 0;
		else
			sess->data_fd = sock;
	}
	//pasv
	if(pasv_active(sess))
	{

	}

	if(sess->port_addr)
	{
		free(sess->port_addr);
		sess->port_addr = NULL;
	}

	return ret;
}

static void list_common(session_t *sess)
{
	DIR *dir = opendir(".");//打开当前目录
	if(dir == NULL)
		return;

	//drwxr-xr-x    3 1000     1000           30 Sep 09  2019 Desktop
	char buf[MAX_BUFFER_SIZE] = {0};

	struct stat sbuf; //用于保存文件的属性
	struct dirent *dt;
	while((dt = readdir(dir)) != NULL)
	{
		if(stat(dt->d_name, &sbuf) < 0)
			continue;
		if(dt->d_name[0] == '.')  //过滤掉隐藏文件
			continue;

		memset(buf, MAX_BUFFER_SIZE, 0);
		//先组合权限
		const char *perms = statbuf_get_perms(&sbuf);  //drwxr-xr-x
		int offset = 0;
		offset += sprintf(buf, "%s", perms);
		offset += sprintf(buf+offset, "%3d %-8d %-8d %8u", sbuf.st_nlink, sbuf.st_uid, sbuf.st_gid, sbuf.st_size);
		
		//后组合时间日期
		const char *pdate = statbuf_get_date(&sbuf);   //Sep 09  2019 
		offset += sprintf(buf+offset, "%s", pdate);
		sprintf(buf+offset, "%s\r\n", dt->d_name);

		//发送数据
		send(sess->data_fd, buf, strlen(buf), 0);
	}
}

static void do_list(session_t *sess)
{
	//1 建立数据连接
	if(get_transfer_fd(sess) == 0)
		return;

	//2 回复 150
	ftp_reply(sess, FTP_DATACONN ,"Here comes the directory listing.");

	//3 显示列表
	list_common(sess);


	//4 关闭连接
	close(sess->data_fd);
	sess->data_fd = -1;

	//5 回复 226
	ftp_reply(sess, FTP_TRANSFEROK, "Directory send OK.");
}
```
<h2 align=center> 第四天 </h2>
```

```
<h2 align=center> 第五天 </h2>
```

```



