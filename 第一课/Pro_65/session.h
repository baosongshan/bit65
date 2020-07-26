#ifndef _SESSION_H_
#define _SESSION_H_

#include"common.h"

typedef struct session
{
	/* ¿ØÖÆÁ¬½Ó */
	int ctrl_fd;
}session_t;

void begin_session(session_t *sess);

#endif /* _SESSION_H_ */