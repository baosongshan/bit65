#include"common.h"
#include"sysutil.h"
#include"session.h"
#include"tunable.h"
#include"parseconf.h"
#include"ftpcodes.h"
#include"ftpproto.h"

void ParseConf_Test()
{
	parseconf_load_file("miniftp.conf");

	printf("tunable_pasv_enable = %d\n", tunable_pasv_enable);
	printf("tunable_port_enable = %d\n", tunable_port_enable);
	printf("tunable_listen_port = %d\n", tunable_listen_port);
	printf("tunable_max_clients = %d\n", tunable_max_clients);
	printf("tunable_max_per_ip = %d\n", tunable_max_per_ip);
	printf("tunable_accept_timeout = %d\n", tunable_accept_timeout);
	printf("tunable_connect_timeout = %d\n", tunable_connect_timeout);
	printf("tunable_idle_session_timeout = %d\n", tunable_idle_session_timeout);
	printf("tunable_data_connection_timeout = %d\n", tunable_data_connection_timeout);
	printf("tunable_loacl_umask = %d\n", tunable_local_umask);
	printf("tunable_upload_max_rate = %d\n", tunable_upload_max_rate);
	printf("tunable_download_mas_rate = %d\n", tunable_download_max_rate);
	printf("tunable_listen_address = %s\n", tunable_listen_address);
}

static unsigned int s_children; 
static void check_limit(session_t *sess);

int main(int argc, char *argv[])
{
	//ParseConf_Test();
	parseconf_load_file("miniftp.conf");

	if(getuid() != 0)
	{
		printf("bit miniftp : must be started as root.\n");
		exit(EXIT_FAILURE);
	}

	session_t sess = 
	{
		/* 控制连接 */
		-1,-1,"", "", "",

		/* 数据连接 */
		NULL, -1, -1,

		/* ftp 协议状态 */
		0, NULL, 0, 0, 0,

		/* 父子进程通道 */
		-1, -1,
		/* 限速 */
		0, 0, 0, 0
	};

	sess.upload_max_rate = tunable_upload_max_rate;
	sess.download_max_rate = tunable_download_max_rate;

	int listenfd = tcp_server(tunable_listen_address, tunable_listen_port); 


	signal(SIGCHLD, SIG_IGN);

	int sockConn;
	struct sockaddr_in addrCli;
	socklen_t addrlen;
	while(1)
	{
		if((sockConn = accept(listenfd, (struct sockaddr*)&addrCli, &addrlen)) < 0)
			ERR_EXIT("accept");

		++s_children;
		sess.num_clients = s_children;

		pid_t pid = fork();
		if(pid == -1)
			ERR_EXIT("fork");

		if(pid == 0)
		{
			close(listenfd);
			sess.ctrl_fd = sockConn;

			//进行连接数检查限制
			check_limit(&sess);

			begin_session(&sess);
			exit(EXIT_SUCCESS);
		}
		else 
		{
			//Parent Process
			close(sockConn);
		}
	}

	close(listenfd);
	return 0;
}

static void check_limit(session_t *sess)
{
	if(tunable_max_clients!=0 && sess->num_clients>tunable_max_clients)
	{
		// 421 There are too many connected users, please try later
		ftp_reply(&sess, FTP_TOO_MANY_USERS, "There are too many connected users, please try later");
		exit(EXIT_FAILURE);
	}
	if(tunable_max_per_ip!=0 && sess->num_per_ip>tunable_max_per_ip)
	{
		// 421 There are too many connections from your internet address
	}
}