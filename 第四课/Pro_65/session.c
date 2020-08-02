#include"session.h"

void begin_session(session_t *sess)
{
	//��ʼ�����ӽ���ͨѶͨ��
	priv_sock_init(sess);

	pid_t pid = fork();
	if(pid == -1)
		ERR_EXIT("fork");
	if(pid == 0)
	{
		//ftp �������
		priv_sock_set_child_context(sess);
		handle_child(sess);
	}
	else
	{
		//nobody ����
		priv_sock_set_parent_context(sess);
		
		//��root���̸��Ľ�����Ϊnobody
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