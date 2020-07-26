#include"ftpproto.h"

//ftp 服务进程
void handle_child(session_t *sess)
{
	send(sess->ctrl_fd,  "220 (bit miniftp 1.0)\n\r", strlen("220 (bit miniftp 1.0)\n\r"), 0);

	while(1)
	{
		//不停的等待客户端的命令并进行处理
	}
}
