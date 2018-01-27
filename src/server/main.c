#include <stdio.h>
#include <signal.h>

/* local headers */
#include <include/server.h>
#include <include/server_tool.h>
#include <include/common.h>
#include <include/xml.h>

int main(void){
	signal(SIGINT, INThandler);
	debug("starting server...\n");
	// parse file to get iface (for sysnet) and port to run server
	parse_config_file("config/server.xml");
	fprintf(stdout, "interface : %s\nport : %s\n", iface, port);
	tcp_server(port);
	return 0;
}
