#include"session.h"

void begin_session(session_t *sess)
{
	pid_t pid = fork();
	if(pid == -1)
		ERR_EXIT("fork");
	if(pid == 0)
	{
		//ftp 服务进程
		handle_child(sess);
	}
	else
	{
		//nobody 进程
		handle_parent(sess);
	}
}