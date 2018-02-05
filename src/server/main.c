#include <stdio.h>
#include <signal.h>

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

int main(void){
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
