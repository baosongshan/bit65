#ifndef _SESSION_H_
#define _SESSION_H_

#include"common.h"

typedef struct session
{
	/* �������� */
	uid_t uid;
	int ctrl_fd;
	char cmdline[MAX_COMMAND_LINE];
	char cmd[MAX_COMMAND];
	char arg[MAX_ARG];

	/* �������� */
	struct sockaddr_in *port_addr;
	int  data_fd;
	int  pasv_listen_fd;
	int  data_process;

	/* ftp Э��״̬ */
	int is_ascii;
	char *rnfr_name;
	long long restart_pos;
	unsigned int num_clients;
	unsigned int num_per_ip;

	/* ���ӽ���ͨ�� */
	int parent_fd;
	int child_fd;

	/* ���� */
	unsigned long upload_max_rate;
	unsigned long download_max_rate;
	long transfer_start_sec;
	long transfer_start_usec;
}session_t;

void begin_session(session_t *sess);

#endif /* _SESSION_H_ */