#include"common.h"
#include"sysutil.h"
#include"session.h"

int main(int argc, char *argv[])
{
	if(getuid() != 0)
	{
		printf("bit miniftp : must be started as root.\n");
		exit(EXIT_FAILURE);
	}

	session_t sess = 
	{
		/* 控制连接 */
		-1,-1,"", "", "",

		/* 数据连接 */
		NULL, -1, -1,

		/* ftp 协议状态 */
		0,
		/* 父子进程通道 */
		-1, -1
	};

	int listenfd = tcp_server("192.168.232.10", 9188); 

	int sockConn;
	struct sockaddr_in addrCli;
	socklen_t addrlen;
	while(1)
	{
		if((sockConn = accept(listenfd, (struct sockaddr*)&addrCli, &addrlen)) < 0)
			ERR_EXIT("accept");

		pid_t pid = fork();
		if(pid == -1)
			ERR_EXIT("fork");

		if(pid == 0)
		{
			//Child Process
			close(listenfd);
			
			//会话
			sess.ctrl_fd = sockConn;
			begin_session(&sess);
			exit(EXIT_SUCCESS);
		}
		else 
		{
			//Parent Process
			close(sockConn);
		}
	}

	close(listenfd);
	return 0;
}