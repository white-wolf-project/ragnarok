#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

#include <include/server.h>
#include <include/common.h>

void INThandler(int sig)
{
	signal(sig, SIG_IGN);
	debug("ctrl + c\n");
	close(sock);
	exit(0);
}