#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h> 
#include <netdb.h>
/* local headers */
#include <include/client.h>
#include <include/iwlist.h>
#include <include/common.h>
#include <include/client_tool.h>


int main(int argc, char const *argv[]){

	parse_config_file("config.xml");
	printf("ip : %s\n", ipaddr);
	printf("port : %s\n", port);

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
	/*send_data(sock, data);*/

	if (argc == 2)
	{
		run_iwlist(argv[1]); // wlo1 for Ubuntu, not sure about other distros
	} else {
		run_iwlist("wlan0");
	}
	close(sock);
	return 0;
}