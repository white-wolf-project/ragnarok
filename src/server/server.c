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

/* local headers */
#include <include/common.h>
#include <include/server.h>
#include <include/server_util.h>
int sock;

/**
 * @brief
 * Catch ctrl+c to close sock and exit correctly
 * Since it's a deamon this function is useless. I keep it for the moment
 * @param int sig : signal
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
 * @param const char *pidfile
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
int init_srv_daemon(const char *pidfile){
	pid_t process_id = 0;
	pid_t sid = 0;
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
 * @param const char* service_port
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
		count++;
		if (count == 3){
			count = 0;
			log_it("remove database");
			remove_db("ragnarok_bdd");
		}
	}
	return 0;
}

/**
 * @brief
 * Here we deal with the data we received. Data is splited in two file : client.log and server.xml.
 * In server.log, "normal" data from the scan it's for debugging purpose.
 * We also call Python3 scripts to handle XML files, still in early stage for the moment.
 * XML data is stored in server.xml
 * @param int sock : socket of the server
 * @param int counter : count number of opened sessions, once counter hits 3, run python script will all 3 xml files
 */
void manage_co(int sock, int counter)
{
	int  buf_len;
	int xml_start = 0;
	int xml_end = 0;
	char hostname [NI_MAXHOST];
	char port [NI_MAXSERV];
	char buffer[257];
	char xml_filename[32];
	struct sockaddr * sockaddr;
	bool isXML = false;
	socklen_t length = 0;
	getpeername(sock, NULL, &length);

	if (length == 0)
		return;

	sockaddr = malloc(length);

	if (getpeername(sock, sockaddr, & length) < 0) {
		perror ("getpeername");
		free(sockaddr);
		return;
	}

	/* we send non-XML data to server.log */
	FILE *fp = fopen("server.log" ,"a+"); // or w+ idk yet

	if (getnameinfo(sockaddr, length, hostname, NI_MAXHOST, port, NI_MAXSERV, NI_NUMERICHOST | NI_NUMERICSERV) == 0) {
		sprintf (buffer, "IP:%s\tPort: %s\n", hostname, port);
		log_it("new client : %s", buffer);
		fprintf(stdout, "%s\n", buffer);
		fprintf(fp, "%s\n", buffer);
	}
	free(sockaddr);
	int xml_checker;
	FILE *fp_xml;
	while (1) {
		memset(buffer, 0, 257);
		buf_len = read(sock, buffer, 256);

		if (buf_len < 0) {
			perror("read");
			exit(EXIT_SUCCESS);
		}
		if (buf_len == 0){
			break;
		}

		/* check the begining and the end of the XML data */
		xml_checker = check4xml(buffer);
		if (xml_checker == 0)
		{	
			isXML = true;
			xml_start = 1;
			sleep(1);
			sprintf(xml_filename, "server_%d.xml", counter);
			fp_xml = fopen(xml_filename ,"w+");
			write_to_xml(fp_xml, buffer, buf_len, xml_start, xml_end);
			xml_start = 0;
		}
		else if (xml_checker == 1) {
			xml_end = 1; xml_start = 0;

			write_to_xml(fp_xml, buffer, buf_len, xml_start, xml_end);
			fclose(fp_xml);
			if (counter == 2)
				handle_xml_data();
		}
		else {
			/* check if we are in the "middle" of the XML file */
			if (isXML)
			{
				write_to_xml(fp_xml, buffer, buf_len, xml_start, xml_end);

				if (xml_end == 1)
					isXML = false;
			}
			else {
				// debug("%s\r\n",buffer);
				fprintf(fp, "%s", buffer);
				fclose(fp);
			}
		}
	}
}
