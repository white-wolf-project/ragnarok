#include <stdio.h>
#include <signal.h>
#include <getopt.h>
#include <string.h>
/* local headers */
#include <include/server.h>
#include <include/server_tool.h>
#include <include/common.h>
#include <include/xml.h>
#include <include/network.h>

// Not sure it is the best way to do what I want to do.
// TODO : find another way if the user wants to modify the path
// to the XML file (eg : with args)
#ifdef RELEASE
	#define CONFIG_PATH "/etc/ragnarok/server.xml"
#elif DEVELOPMENT
	#define CONFIG_PATH "config/server.xml"
#else // default is DEVELOPMENT
	#define CONFIG_PATH "config/server.xml"
#endif


static struct option longopts[] = {
	{ "network",	no_argument,	NULL, 'n'},
	{ "xml",	required_argument, NULL, 'x'},
	{ "version", 	no_argument, 	NULL, 'v'},
	{ "help", 		no_argument, 	NULL, 'h'},
	{ NULL, 0, NULL, 0 }
};

void usage(int argc, char const *argv[]){
	char *name = NULL;
	name = strrchr(argv[0], '/');
	fprintf(stdout, "Usage : %s [OPTIONS]\n", (name ? name + 1: argv[0]));
	fprintf(stdout, "-x, --xml <xmlfile> \t\tXML file to parse\n");
	fprintf(stdout, "-h, --help\t\t\tprint this help\n");
	debug("DEBUG : ON\n");
}
int main(int argc, char const *argv[]){
	int opt, optindex = 0;
	int xconfig = 0;
	const char *xmlfile;
	while((opt = getopt_long(argc, (char* const *)argv, "vhx", longopts, &optindex)) != -1){
		switch(opt){
			case 'h':
				usage(argc, argv);
				return 0;
			case 'v' :
				version();
				return 0;
			case 'x' :
				xmlfile = argv[optind];
				debug("%s\n", xmlfile);
				xconfig = 1;
				break;
		}
	}

	if (!xconfig){
		#ifdef RELEASE
		xmlfile = "/etc/ragnarok/server.xml";
		#else
		xmlfile = "config/server.xml";
		#endif
	}
	signal(SIGINT, INThandler);
	debug("starting server...\n");

	// parse file to get iface (for sysnet) and port to run server
	if (parse_config_file(CONFIG_PATH) != 0)
		return -1;

	printf("\t");

	// call network_info from sysnet to get IP of the server
	network_info(iface, 4);
	fprintf(stdout, "\n\tserver port : %s\n", port);

	// run TCP server
	tcp_server(port);
	return 0;
}
