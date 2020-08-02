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

		//把root进程更改进程名为nobody
		struct passwd *pw = getpwnam("nobody");
		if(pw == NULL)
			ERR_EXIT("getpwname");
		if(setegid(pw->pw_gid) < 0)
			ERR_EXIT("setegid");
		if(seteuid(pw->pw_uid) < 0)
			ERR_EXIT("seteuid");

		handle_parent(sess);
	}
}