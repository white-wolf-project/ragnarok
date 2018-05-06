/**
 * @file server.c
 * @author Mathieu Hautebas
 * @date 22 March 2018
 * @brief file containing functions for the server.
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <errno.h>
#include <netdb.h>
#include <time.h>
#include <stdbool.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <my_global.h>
#include <mysql.h>
/* local headers */
#include <include/server.h>
#include <include/common.h>

int sock;

/**
 * @brief
 * Catch ctrl+c to close sock and exit correctly
 * Since it's a deamon this function is useless. I keep it for the moment
 * @return void, no return just exit() the program
 */
void INThandler(int sig)
{
	signal(sig, SIG_IGN);
	debug("ctrl + c\n");
	close(sock);
	exit(0);
}

/**
 * @brief
 * init deamon for the server
 * First we check if the pidfile exists. If it exists, it means client is already running.
 * Print pid of process the quit.
 * @code
 * if (file_exists(pidfile))
 * {
 *		fp = fopen(pidfile, "r");
 *		fprintf(stdout, "[-] ragnarok is already running\n");
 *		getline(&pid_val, &len, fp);
 *		fprintf(stdout, "[i] PID = %s\r", pid_val);
 *		fclose(fp);
 *		exit(-1);
 * }
 * @endcode
 * We create a normal process and a child process. We kill parent process and set a new session.
 * Close stdin, stdout and stderr for ragnarok
 * @code
 * close(STDIN_FILENO);
 * close(STDOUT_FILENO);
 * close(STDERR_FILENO);
 * @endcode
 * @return returns 0 if everything's done well or a number != 0 if any issue or just exit().
 * @see https://www.thegeekstuff.com/2012/02/c-daemon-process/
 */
int init_srv_daemon(void){
	pid_t process_id = 0;
	pid_t sid = 0;
	char *pidfile = "ragnarok-srv.pid";
	char *pid_val = NULL;
	size_t len = 0;

	FILE *fp = NULL;

	if (file_exists(pidfile))
	{
		fp = fopen(pidfile, "r");
		fprintf(stdout, "[-] server is already running\n");
		getline(&pid_val, &len, fp);
		fprintf(stdout, "[i] PID = %s\r", pid_val);
		fclose(fp);
		exit(-1);
	}

	/* create child process */
	process_id = fork();

	/* check if fork() failed */
	if (process_id < 0)
	{
		perror("fork");
		exit(1);
	}

	/* PARENT PROCESS. Let's kill it. */
	if (process_id > 0)
	{
		fp = fopen(pidfile, "a+");
		debug("[i] PID %d\r", process_id);
		fprintf(fp, "%d\n",process_id);
		fclose(fp);
		/* return success in exit status */
		exit(0);
	}
	/* unmask the file mode */
	umask(0);

	/* set new session */
	sid = setsid();
	if(sid < 0)
	{
		exit(1);
	}

	/* Close stdin. stdout and stderr */
	close(STDIN_FILENO);
	close(STDOUT_FILENO);
	close(STDERR_FILENO);

	return 0;
}

/**
 * @brief
 * TCP server function to handle multiple clients
 * @return returns 0 if everything's done well or a number != 0 if any issue or just exit().
 * @see https://www.thegeekstuff.com/2012/02/c-daemon-process/
 */

int tcp_server(const char* service_port)
{
	int err;
	int sock_server;
	int sock;
	struct addrinfo  hints;
	struct addrinfo *results;
	int count = 0;
	struct sockaddr_in* addr_in;
	socklen_t length = 0;
	char hostname [NI_MAXHOST];
	char servname [NI_MAXSERV];

	// Create srv socket & attribute service number
	if ((sock_server = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
		perror("socket");
		return -1;
	}

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;
	if ((err = getaddrinfo(NULL, service_port,  &hints, &results)) != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(err));
		close(sock_server);
		return -1;
	}

	err = bind(sock_server, results->ai_addr, results->ai_addrlen);
	freeaddrinfo(results);
	if (err < 0) {
		perror("bind");
		close(sock_server);
		return -1;
	}

	length = sizeof(struct sockaddr_in);
	if (getsockname(sock_server, (struct sockaddr *) &addr_in, &length) < 0) {
		perror ("getsockname");
		return -1;
	}
	if (getnameinfo((struct sockaddr *) &addr_in, length,
	                hostname, NI_MAXHOST,
	                servname, NI_MAXSERV,
	                NI_NUMERICHOST | NI_NUMERICSERV) == 0)
		#ifdef DEBUG
		fprintf (stdout, "IP = %s, Port = %s \n", hostname, servname);
		#endif
	listen(sock_server, 5);
	while (1) {
		sock = accept(sock_server, NULL, NULL);
		if (sock < 0) {
			perror("accept");
			return -1;
		}
		switch (fork()) {
			case 0 : // son
				close(sock_server);
				manage_co(sock, count);
				exit(EXIT_SUCCESS);
			case -1 :
				perror("fork");
				return -1;
			default : // father
				close(sock);
		}
		if (count++ == 3)
			count = 0;
	}
	return 0;
}

/**
 * @brief
 * Here we deal with the data we received. Data is splited in two file : client.log and server.xml.
 * In server.log, "normal" data from the scan it's for debugging purpose.
 * We also call Python3 scripts to handle XML files, still in early stage for the moment.
 * XML data is stored in server.xml
 * @param sock : socket of the server
 * @param counter : count number of opened sessions, once counter hits 3, run python script will all 3 xml files
 */
void manage_co(int sock, int counter)
{
	struct sockaddr * sockaddr;
	socklen_t length = 0;
	char hostname [NI_MAXHOST];
	char port [NI_MAXSERV];
	char buffer[256];
	int  buf_len;
	char xml_filename[32];
	bool isXML = false;
	const char *arg_tab[4] = {0};

	getpeername(sock, NULL, &length);

	if (length == 0)
		return;

	sockaddr = malloc(length);

	if (getpeername(sock, sockaddr, & length) < 0) {
		perror ("getpeername");
		free(sockaddr);
		return;
	}

	FILE *fp = fopen("server.log" ,"a+"); // or w+ idk yet
	FILE *fp_xml = NULL;

	if (getnameinfo(sockaddr, length, hostname, NI_MAXHOST, port, NI_MAXSERV, NI_NUMERICHOST | NI_NUMERICSERV) == 0) {
		sprintf (buffer, "IP:%s\tPort: %s\n", hostname, port);
		log_it("new client : %s", buffer);
		fprintf(stdout, "%s\n", buffer);
		fprintf(fp, "%s\n", buffer);
	}
	free(sockaddr);
	while (1) {
		buf_len = read(sock, buffer, 256);

		if (buf_len < 0) {
			perror("read");
			exit(EXIT_SUCCESS);
		}
		if (buf_len == 0){
			break;
		}
		if (strstr(buffer, "-xml-") != NULL){
			isXML = true;
			sprintf(xml_filename, "server_%d.xml", counter);
			fp_xml = fopen(xml_filename ,"w+");
		}
		else {
			if (strstr(buffer, "-end_xml-") != NULL)
			{
				fclose(fp_xml);
				isXML = false;
				#ifdef RELEASE
				arg_tab[0] = NULL;
				arg_tab[1] = "/var/opt/ragnarok1.xml";
				arg_tab[2] = "/var/opt/ragnarok2.xml";
				arg_tab[3] = "/var/opt/ragnarok3.xml";
				#else
				arg_tab[0] = NULL;
				arg_tab[1] = "src/python/test/ragnarok1.xml";
				arg_tab[2] = "src/python/test/ragnarok2.xml";
				arg_tab[3] = "src/python/test/ragnarok3.xml";
				#endif

				/* 0, 1, 2 */
				if (counter == 2)
				{
					if (check4db("ragnarok_bdd") != true){
						run_python("src/python/ragnarok_bdd.py", NULL);
						run_python("src/python/xmlparser.py", arg_tab);
					} else {
						run_python("src/python/xmlparser.py", arg_tab);
					}
				}
			}
		}
		if (isXML)
		{
			if (strstr(buffer, "-xml-") == NULL && strstr(buffer, "-end_xml-") == NULL)
			{
				debug("%s\r\n", buffer);
				fprintf(fp_xml, "%s\n", buffer);
			}
		} else {
			debug("%s\r\n",buffer);
			fprintf(fp, "%s\n", buffer);
			fclose(fp);
		}
		memset(buffer, 0, 256);
		buffer[buf_len] = '\0';
	}
}

/**
 * @brief
 * TODO : change db name
 * Check if the ragnarok_bdd datase exists
 * We open a connection to the SQL server. Then we check if you can create a new DB.
 * If DB is created we just delete it and return false to let Python scripts do their job.
 * @param db_name : name of database to check for
 * @return true if database exists
 * @see https://dev.mysql.com/
 */
bool check4db(const char *db_name)
{
	char err[128];
	char db_query[64];
	MYSQL *con = mysql_init(NULL);

	if (con == NULL)
	{
		fprintf(stderr, "%s\n", mysql_error(con));
		exit(1);
	}

	/* connection to mysql server */
	if (mysql_real_connect(con, "localhost", "root", "root", NULL, 0, NULL, 0) == NULL)
	{
		fprintf(stderr, "%s\n", mysql_error(con));
		mysql_close(con);
		return false;
	}
	/* try to create DB, if it already exists then return true else we return false */
	sprintf(db_query, "CREATE DATABASE %s", db_name);
	if (mysql_query(con, db_query))
	{
		sprintf(err, "Can't create database '%s'; database exists", db_name);
		if (!strcmp(mysql_error(con), err))
		{
			fprintf(stdout, "%s\n", err);
			mysql_close(con);
			return true;
		}
	} else {
		/* we delete DB after testing to not break python scripts */
		sprintf(db_query, "DROP DATABASE %s", db_name);
		mysql_query(con, db_query);
		mysql_close(con);
		return false;
	}
	return false;
}
