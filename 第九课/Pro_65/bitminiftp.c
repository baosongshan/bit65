#include"common.h"
#include"sysutil.h"
#include"session.h"
#include"tunable.h"
#include"parseconf.h"
#include"ftpcodes.h"
#include"ftpproto.h"
#include"hash.h"

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

extern session_t *p_sess;

//最大连接数限制
static unsigned int s_children; 
void handle_sigchld(int sig);
//每ip连接数限制
unsigned int hash_func(unsigned int buckets, void *key);
unsigned int handle_ip_count(void *ip);
void drop_ip_count(void *ip);
static hash_t *s_ip_count_hash;
static hash_t *s_pid_ip_hash;


static void check_limit(session_t *sess);

int main(int argc, char *argv[])
{
	ParseConf_Test();
	//parseconf_load_file("miniftp.conf");

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
		NULL, -1, -1, 0,

		/* ftp 协议状态 */
		0, NULL, 0, 0, 0,

		/* 父子进程通道 */
		-1, -1,
		/* 限速 */
		0, 0, 0, 0
	};

	p_sess = &sess;

	sess.upload_max_rate = tunable_upload_max_rate;
	sess.download_max_rate = tunable_download_max_rate;

	//申请ip和count对应的哈希表
	s_ip_count_hash = hash_alloc(MAX_BUCKET_SIZE, hash_func);
	//申请pid和ip对应的哈希表
	s_pid_ip_hash = hash_alloc(MAX_BUCKET_SIZE, hash_func);

	int listenfd = tcp_server(tunable_listen_address, tunable_listen_port); 


	//signal(SIGCHLD, SIG_IGN);
	signal(SIGCHLD, handle_sigchld);


	int sockConn;
	struct sockaddr_in addrCli;
	socklen_t addrlen;
	while(1)
	{
		if((sockConn = accept(listenfd, (struct sockaddr*)&addrCli, &addrlen)) < 0)
			ERR_EXIT("accept");

		
		//最大连接数限制
		++s_children;
		sess.num_clients = s_children;

		//每ip连接数限制
		unsigned int ip = addrCli.sin_addr.s_addr;
		sess.num_per_ip = handle_ip_count(&ip);

		pid_t pid = fork();
		if(pid == -1)
		{
			--s_children;
			ERR_EXIT("fork");
		}

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
			//登记每个进程所对应的ip
			hash_add_entry(s_pid_ip_hash, &pid, sizeof(pid), &ip, sizeof(ip));
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
		ftp_reply(sess, FTP_TOO_MANY_USERS, "There are too many connected users, please try later");
		exit(EXIT_FAILURE);
	}
	if(tunable_max_per_ip!=0 && sess->num_per_ip>tunable_max_per_ip)
	{
		// 421 There are too many connections from your internet address
		ftp_reply(sess, FTP_IP_LIMIT, "There are too many connections from your internet address");
		exit(EXIT_FAILURE);
	}
}

void handle_sigchld(int sig)
{
	pid_t pid;
	while((pid = waitpid(-1, NULL, WNOHANG)) > 0)
	{
		--s_children;
		unsigned int *ip = hash_lookup_entry(s_pid_ip_hash, &pid, sizeof(pid));
		if(ip == NULL)
			continue;
		drop_ip_count(ip);
		hash_free_entry(s_pid_ip_hash, &pid, sizeof(pid));
	}
}

unsigned int hash_func(unsigned int buckets, void *key)
{
	return (*(unsigned int*)key % buckets);
}

unsigned int handle_ip_count(void *ip)
{
	int count = 0;
	unsigned int *p_count = hash_lookup_entry(s_ip_count_hash, ip, sizeof(unsigned int));
	if(p_count == NULL)
	{
		count = 1;
		hash_add_entry(s_ip_count_hash, ip, sizeof(unsigned int), &count, sizeof(unsigned int));
	}
	else
	{
		count = *p_count;
		++count;
		*p_count = count;
	}
	return count;
}

void drop_ip_count(void *ip)
{
	unsigned int *p_count = hash_lookup_entry(s_ip_count_hash, ip, sizeof(unsigned int));
	if(p_count == NULL)
		return;
	int count = *p_count;
	--count;
	*p_count = count;
	if(count == 0)
		hash_free_entry(s_ip_count_hash, ip, sizeof(unsigned int));
}