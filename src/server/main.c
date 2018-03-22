/**
 * @file main.c
 * @author Mathieu Hautebas
 * @date 22 March 2018
 * @brief file containing the main function and usage function.
 *
 */
#include <stdio.h>
#include <signal.h>
#include <getopt.h>
#include <string.h>
#include <signal.h>
/* local headers */
#include <include/server.h>
#include <include/server_tool.h>
#include <include/common.h>
#include <include/xml.h>
#include <include/network.h>

static struct option longopts[] = {
	{ "network",	no_argument,	NULL, 'n'},
	{ "port", 		required_argument, 	NULL, 'p'},
	{ "interface",	required_argument,	NULL, 'f'},
	{ "xml",	required_argument, NULL, 'x'},
	{ "stop",		no_argument,		NULL, 's'},
	{ "version", 	no_argument, 	NULL, 'v'},
	{ "help", 		no_argument, 	NULL, 'h'},
	{ NULL, 0, NULL, 0 }
};

void usage(int argc, char  *argv[]){
	char *name = NULL;
	name = strrchr(argv[0], '/');
	fprintf(stdout, "Usage : %s [OPTIONS]\n", (name ? name + 1: argv[0]));
	fprintf(stdout, " -p, --port\t\t\t specify port to bind\n");
	fprintf(stdout, " -f, --interface\t\t specify interface to grab info\n");
	fprintf(stdout, " -x, --xml <xmlfile> \t\t XML file to parse\n");
	fprintf(stdout, " -s, --stop\t\t\t stop daemon\n");
	fprintf(stdout, " -v, --version\t\t\t print version\n");
	fprintf(stdout, " -h, --help\t\t\t print this help\n");
	debug("DEBUG : ON\n");
}

int main(int argc, char *argv[]){
	int opt, optindex = 0;
	int xconfig = 0;
	int is_port = 0, is_iface = 0;
	int stop_srv = 0;
	int srv_pid = 0;
	char *newport, *newiface;
	const char *xmlfile;
	while((opt = getopt_long(argc, (char**)argv, "pfvhxs", longopts, &optindex)) != -1){
		switch(opt){
			case 'h':
				usage(argc, argv);
				return 0;
			case 'v' :
				version();
				return 0;
			case 'x' :
				xmlfile = argv[optind];
				xconfig = 1;
				break;
			case 'p' :
				newport = argv[optind];
				is_port = 1;
				break;
			case 'f' :
				newiface = argv[optind];
				is_iface = 1;
				break;
			case 's' :
				stop_srv = 1;
				break;
		}
	}

	if (stop_srv){
		srv_pid = get_instance_pid("ragnarok-srv.pid");
		/* No need to kill something that does exist*/
		if (srv_pid == -1)
			return 0;
		remove("ragnarok-srv.pid");
		kill(srv_pid, SIGINT);
		debug("[i] server stopped\n");
		return 0;
	}

	/*
		if you don't specify a config file
		then set default XML for RELEASE or DEV/DEBUG
	*/
	if (!xconfig){
		#ifdef RELEASE
		xmlfile = "/etc/ragnarok/server.xml";
		#else
		xmlfile = "config/server.xml";
		#endif
	}


	/* parse XML file to get iface (for sysnet) and port to run server */
	if (parse_config_file(xmlfile) != 0)
		return -1;

	if (is_port)
	{
		port = newport;
	}
	if (is_iface)
	{
		iface = newiface;
	}

	signal(SIGINT, INThandler);
	debug("[+] starting server...\n");

	/* call network_info from sysnet to get IP of the server */
	network_info(iface, 4);
	fprintf(stdout, "[i] server port : %s\n", port);

	/* init damrun TCP server */
	init_srv_daemon();
	tcp_server(port);
	return 0;
}
