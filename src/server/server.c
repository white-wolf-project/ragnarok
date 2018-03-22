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
#include <sys/types.h>
#include <sys/stat.h>
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