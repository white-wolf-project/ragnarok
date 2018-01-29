#include <stdio.h>
#include <signal.h>

/* local headers */
#include <include/server.h>
#include <include/server_tool.h>
#include <include/common.h>
#include <include/xml.h>
#include <include/network.h>

int main(void){
	signal(SIGINT, INThandler);
	debug("starting server...\n");

	// parse file to get iface (for sysnet) and port to run server
	parse_config_file("config/server.xml");
	printf("\t");

	// call network_info from sysnet to get IP of the server
	network_info(iface, 4);
	fprintf(stdout, "\n\tserver port : %s\n", port);

	// run TCP server
	tcp_server(port);
	return 0;
}
