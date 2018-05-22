/**
 * @file client.c
 * @author Mathieu Hautebas
 * @date 22 March 2018
 * @brief File containing Functions for the client.
 *
 */
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <errno.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <netdb.h>
#include <sys/socket.h>
#include <libxml/parser.h>
/* local headers */
#include <include/client.h>
#include <include/xml.h>
#include <include/common.h>

#define STRING_LEN 2048
#define BUFSIZE 1024 /* read 1024 bytes at a time */

int sock;

/**
 * @brief
 * Function to send data to server.
 * example usage :
 * init socket :
 * @verbatim int sock; @endverbatim
 * call this function like this :
 * @code
 * int number = 12;
 * send_data(sock, "number is : %d", number);
 * @endcode
 * When you use va_arg() "function" on the va_list obtained
 * from va_start(),
 * the va_list is modified, when passed to another function
 * it is NOT the same as you got at the beginning
 * That's why I'm creating two va_lists first one is the fprintf equivalent and the other one is sprintf.
 * Then I copy vargs to vargs_out and vargs_log like this :
 * @verbatim copy vargs -> vargs_out (stdout) & vargs_log (client.log) @endverbatim
 * @code
 * va_copy(vargs_out, vargs);
 * va_copy(vargs_log, vargs);
 * @endcode
 * @param sock2server the socket we will use to deal with the server.
 * @param data2send : the date we will send.
 * @param ... : other params like printf()
 * @return returns 0 if everything's done well or a number != 0 if any issue.
 * @see https://stackoverflow.com/questions/27126387/vfprintf-throws-segfault-but-input-is-well-defined#comment42752821_27126509
 */
int send_data(int sock2server, const char* data2send, ...){

	// TODO : fix length
	char string2send[STRING_LEN];

	va_list vargs;
	va_list vargs_out;
	va_list vargs_log;
	va_start(vargs, data2send);

	/* copy vargs -> vargs_out (stdout) & vargs_log (client.log)*/
	va_copy(vargs_out, vargs);
	va_copy(vargs_log, vargs);

	FILE *fp = fopen("client.log" ,"a+");
	if (fp != NULL)
	{
		vfprintf(fp, data2send, vargs_log);
		fclose(fp);
	} else {
		#ifdef DEBUG_ADV
		perror("fopen");
		#endif
	}

	#ifdef DEBUG
	vfprintf(stdout, data2send, vargs_out);
	#endif

	vsprintf(string2send, data2send, vargs);

	va_end(vargs);
	va_end(vargs_out);
	if (strcmp(ipaddr, "0") != 0){
		if (write(sock2server, string2send, strlen(string2send)) < 0) {
			perror("write");
			return -1;
		}
	}
	return 0;
}

/**
 * @brief
 * Read data in XML file then send it to the server using send_data() function.
 * This function is used in main()
 * @param xmlfile : path or name of file.
 * @return returns 0 if everything's done well or a number != 0 if any issue.
 */
int read_and_send_data(const char *xmlfile){

	FILE* fp;
	char buf[BUFSIZE];

	if ((fp = fopen(xmlfile, "r")) == NULL)
	{ /* Open source file. */
		perror("fopen");
		return 1;
	}

	send_data(sock, "-xml-\r");
	while (fgets(buf, sizeof(buf), fp) != NULL)
	{
		/* eat the newline fgets() stores */
		buf[strlen(buf) - 1] = '\0';
		send_data(sock, "%s\n", buf);
	}
	send_data(sock, "-end_xml-\r");
	fclose(fp);
	return 0;
}

/**
 * @brief
 * Function to init network stuff for the client
 * @param server
 * @param host : pointer to char, it's the ip address of the server
 * @param port : port of the server to deal with
 * @param results : pointer in struct addrinfo
 * @return returns 0 if everything's done well or a number != 0 if any issue or just exit().
 */
int init_client(int server, char *host, char *port, struct addrinfo **results)
{
	int err;
	struct addrinfo  hints;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	if (server)
		hints.ai_flags = AI_PASSIVE;
	if ((err = getaddrinfo(host, port, &hints, results)) != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(err));
		return -1;
	}
	return 0;
}

/**
 * @brief
 * init deamon for the client
 * First we check if the pidfile exists. If it exists, it means client is already running.
 * Print pid of process the quit.
 * @param pidfile
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
int init_client_daemon(const char *pidfile){
	pid_t process_id = 0;
	pid_t sid = 0;
	char *pid_val = NULL;
	size_t len = 0;

	FILE *fp = NULL;

	if (file_exists(pidfile))
	{
		fp = fopen(pidfile, "r");
		fprintf(stdout, "[-] ragnarok is already running\n");
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