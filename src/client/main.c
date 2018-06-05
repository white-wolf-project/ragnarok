/**
 * @file main.c
 * @author Mathieu Hautebas
 * @date 22 March 2018
 * @brief file containing the main function, usage function and the logo.
 *
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h> 
#include <netdb.h>
#include <getopt.h>
#include <signal.h>
#include <time.h>
/* local headers */
#include <include/client.h>
#include <include/iwlist.h>
#include <include/xml.h>
#include <include/common.h>
#include "sysnet/include/network.h"

static char logo[] = {
	0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
	0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
	0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
	0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x5f,
	0x20, 0x20, 0x20, 0x20, 0x0a, 0x20, 0x5f, 0x20, 0x5f, 0x5f,
	0x20, 0x5f, 0x5f, 0x20, 0x5f, 0x20, 0x20, 0x5f, 0x5f, 0x20,
	0x5f, 0x20, 0x5f, 0x20, 0x5f, 0x5f, 0x20, 0x20, 0x20, 0x5f,
	0x5f, 0x20, 0x5f, 0x20, 0x5f, 0x20, 0x5f, 0x5f, 0x20, 0x5f,
	0x5f, 0x5f, 0x20, 0x7c, 0x20, 0x7c, 0x20, 0x5f, 0x5f, 0x0a,
	0x7c, 0x20, 0x27, 0x5f, 0x5f, 0x2f, 0x20, 0x5f, 0x60, 0x20,
	0x7c, 0x2f, 0x20, 0x5f, 0x60, 0x20, 0x7c, 0x20, 0x27, 0x5f,
	0x20, 0x5c, 0x20, 0x2f, 0x20, 0x5f, 0x60, 0x20, 0x7c, 0x20,
	0x27, 0x5f, 0x5f, 0x2f, 0x20, 0x5f, 0x20, 0x5c, 0x7c, 0x20,
	0x7c, 0x2f, 0x20, 0x2f, 0x0a, 0x7c, 0x20, 0x7c, 0x20, 0x7c,
	0x20, 0x28, 0x5f, 0x7c, 0x20, 0x7c, 0x20, 0x28, 0x5f, 0x7c,
	0x20, 0x7c, 0x20, 0x7c, 0x20, 0x7c, 0x20, 0x7c, 0x20, 0x28,
	0x5f, 0x7c, 0x20, 0x7c, 0x20, 0x7c, 0x20, 0x7c, 0x20, 0x28,
	0x5f, 0x29, 0x20, 0x7c, 0x20, 0x20, 0x20, 0x3c, 0x20, 0x0a,
	0x7c, 0x5f, 0x7c, 0x20, 0x20, 0x5c, 0x5f, 0x5f, 0x2c, 0x5f,
	0x7c, 0x5c, 0x5f, 0x5f, 0x2c, 0x20, 0x7c, 0x5f, 0x7c, 0x20,
	0x7c, 0x5f, 0x7c, 0x5c, 0x5f, 0x5f, 0x2c, 0x5f, 0x7c, 0x5f,
	0x7c, 0x20, 0x20, 0x5c, 0x5f, 0x5f, 0x5f, 0x2f, 0x7c, 0x5f,
	0x7c, 0x5c, 0x5f, 0x5c, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x20,
	0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x7c, 0x5f, 0x5f, 0x5f,
	0x2f, 0x20, 0x62, 0x79, 0x20, 0x57, 0x68, 0x69, 0x74, 0x65,
	0x2d, 0x57, 0x6f, 0x6c, 0x66, 0x20, 0x54, 0x65, 0x61, 0x6d,
	0x20, 0x20, 0x0a
};

static struct option longopts[] = {
	{ "network",	no_argument,		NULL, 'n'},
	{ "ip", 		required_argument,	NULL, 'i'},
	{ "port", 		required_argument,	NULL, 'p'},
	{ "interface",	required_argument,	NULL, 'f'},
	{ "wireless",	required_argument,	NULL, 'w'},
	{ "xml",		required_argument,	NULL, 'x'},
	{ "no-deamon", 	no_argument, 		NULL, 'n'},
	{ "stop",		no_argument,		NULL, 's'},
	{ "version", 	no_argument,		NULL, 'v'},
	{ "help", 		no_argument,		NULL, 'h'},
	{ NULL, 0, NULL, 0 }
};

/**
 * @brief
 * We call this function almost everywhere.
 * But it prints to stdout/stderr only
 * when you build the project in DEBUG mode (we use #ifdef DEBUG ... #endif)
 * @param argc : number of args
 * @param argv : args
 * @return void, no return
 */
void usage(int argc, char  *argv[]){
	char *name = NULL;
	name = strrchr(argv[0], '/');
	fprintf(stdout, "Usage : %s [OPTIONS]\n", (name ? name + 1: argv[0]));
	fprintf(stdout, " -i, --ip\t\t\t server IP\n");
	fprintf(stdout, " -p, --port\t\t\t server port\n");
	fprintf(stdout, " -f, --interface <interface>\t ethernet interface\n");
	fprintf(stdout, " -w, --wireless <interface> \t wireless interface to scan with\n");
	fprintf(stdout, " -x, --xml <xmlfile> \t\t XML config file to parse\n");
	fprintf(stdout, " -n, --no-deamon\t\t do not run as deamon\n");
	fprintf(stdout, " -s, --stop\t\t\t stop client\n");
	fprintf(stdout, " -v, --version\t\t\t print version\n");
	fprintf(stdout, " -h, --help\t\t\t print this help\n");
	debug("DEBUG : ON\n");
}

/**
 * @brief
 * main function where everything starts
 * @param argc : number of args
 * @param argv : args
 * @return 0 if everything's ok, or a number != 0
 */
int main(int argc, char  *argv[]){

	int opt, optindex = 0;
	int xconfig = 0;
	int is_ip = 0, is_port = 0, is_wired_iface = 0, is_wireless_iface = 0;
	int stop_client = 0, client_pid = 0;
	bool is_deamon = true;
	char *newip, *newport, *wired_iface, *newwireless;
	char *mac_addr, *wireless;
	const char *xmlfile, *pidfile;

	while((opt = getopt_long(argc, (char**)argv, "ipfwvhxns", longopts, &optindex)) != -1){
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
			case 'i':
				newip = argv[optind];
				is_ip = 1;
				break;
			case 'p':
				newport = argv[optind];
				is_port = 1;
				break;
			case 'f':
				wired_iface = argv[optind];
				is_wired_iface = 1;
				break;
			case 'w' :
				newwireless = argv[optind];
				is_wireless_iface = 1;
				break;
			case 'n' :
				is_deamon = false;
				break;
			case 's' :
				stop_client = 1;
				break;
			default :
				break;
		}
	}
	#ifdef RELEASE
		pidfile = "/etc/ragnarok/ragnarok.pid";
	#else
		pidfile = "ragnarok.pid";
	#endif

	if (stop_client){
		client_pid = get_instance_pid(pidfile);

		/* No need to kill something that does not exist*/
		if (client_pid == -1)
			return 0;

		remove(pidfile);
		close(sock);
		kill(client_pid, SIGINT);
		debug("[i] ragnarok stopped\n");
		return 0;
	}


	debug("%s\n", logo);
	if (!xconfig){
		#ifdef RELEASE
		xmlfile = "/etc/ragnarok/client.xml";
		#else
		xmlfile = "config/client.xml";
		#endif
	}

	// parse XML file to get default server IP/port and
	// interface to use for AP scan
	if (parse_config_file(xmlfile) != 0)
		return -1;

	if (is_ip)
	{
		ipaddr = newip;
	}
	if (is_port)
	{
		port = newport;
	}

	if (is_wired_iface)
	{
		iface = wired_iface;
	}

	if (is_wireless_iface)
	{
		wireless = newwireless;
	} else {
		wireless = get_wireless();
	}

	if (wireless == NULL)
	{
		fprintf(stderr, "[-] no wireless interface found\n");
		return -1;
	} else {
		if (is_iface_up(wireless) != true){
			if (getuid() != 0){
				fprintf(stderr, "[-] you need higher privileges\n");
				return 1;
			}
			up_iface(wireless);
		}
	}

	debug("ip : %s\n", ipaddr);
	debug("port : %s\n", port);
	debug("iface : %s\n", iface);
	debug("wireless : %s\n", wireless);

	if (strcmp(ipaddr, "0") != 0)
	{
		/* Check for IP adddr and port */
		if (init_client(0, ipaddr, port, &results) < 0){
			exit(EXIT_FAILURE);
		}
		if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
			perror("socket");
			exit(EXIT_FAILURE);
		}
		if (connect(sock, results->ai_addr, results->ai_addrlen) < 0) {
			perror("connect");
			exit(EXIT_FAILURE);
		}
		freeaddrinfo(results);
	}

	// I grab iface value in config.xml.
	// Idea is to use config.xml instead of hardcoded values in code
	mac_addr = get_mac_addr(wireless);
	send_data(sock, "mac : %s\n", mac_addr);

	log_it("starting client");
	if (is_deamon == true){
		init_client_daemon(pidfile);
		log_it("PID is %d", get_instance_pid(pidfile));
		while(1){
			init_xml("ragnarok.xml");
			run_iwlist(get_wireless());
			end_xml("ragnarok.xml");
			read_and_send_data("ragnarok.xml");
			sleep(20);
		}
	} else {
		init_xml("ragnarok.xml");
		run_iwlist(get_wireless());
		end_xml("ragnarok.xml");
		read_and_send_data("ragnarok.xml");
	}
	close(sock);
	return 0;
}
