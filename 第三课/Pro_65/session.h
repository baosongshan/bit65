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

	/* ftp Э��״̬ */
	int is_ascii;
}session_t;

void begin_session(session_t *sess);

#endif /* _SESSION_H_ */