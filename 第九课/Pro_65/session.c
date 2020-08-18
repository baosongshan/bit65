#include"session.h"

void begin_session(session_t *sess)
{
	//初始化父子进程通讯通道
	priv_sock_init(sess);

	pid_t pid = fork();
	if(pid == -1)
		ERR_EXIT("fork");
	if(pid == 0)
	{
		//ftp 服务进程
		priv_sock_set_child_context(sess);
		handle_child(sess);
	}
	else
	{
		//nobody 进程
		priv_sock_set_parent_context(sess);
		handle_parent(sess);
	}
}