#ifndef _FTP_PROTO_H_
#define _FTP_PROTO_H_

#include"common.h"
#include"session.h"

void handle_child(session_t *sess);
void ftp_reply(session_t *sess, int code, const char *text);
#endif /* _FTP_PROTO_H_ */