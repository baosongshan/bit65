#include"ftpproto.h"

//ftp �������
void handle_child(session_t *sess)
{
	send(sess->ctrl_fd,  "220 (bit miniftp 1.0)\n\r", strlen("220 (bit miniftp 1.0)\n\r"), 0);

	while(1)
	{
		//��ͣ�ĵȴ��ͻ��˵�������д���
	}
}
