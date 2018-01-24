#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdarg.h>
#include <errno.h>
#include <net/if.h>
#include <sys/ioctl.h>
/* local headers */
#include <include/client.h>
#include <include/client_tool.h>
#include <include/common.h>

#define STRING_LEN 2048

static FILE* info = NULL;

int parse_config_file(const char *xmlfile){

	if (xmlfile == NULL || access(xmlfile, F_OK) == -1)
	{
		fprintf(stderr, "%s : %s\n", xmlfile, strerror(errno));
		return -1;
	} else {
		xmlDoc *doc = NULL;
		xmlNode *root_element = NULL;
		doc = xmlReadFile(xmlfile, NULL, 0);

		if (doc == NULL) {
			printf("Could not parse the XML file");
			return -2;
		}

		root_element = xmlDocGetRootElement(doc);
		print_xml(root_element, 1);
		xmlFreeDoc(doc);
		xmlCleanupParser();
		debug("%s\n", ipaddr);
		debug("%s\n", port);
	}

	return 0;
}

int send_data(int sock2server, const char* data2send, ...){

	// TODO : fix length
	char string2send[STRING_LEN];

	/* When you use va_arg() "function" on the va_list obtained
	 * from va_start(),
	 * the va_list is modified, so when passed to another function
	 * it is NOT the same as you got at the beginning
	 * https://stackoverflow.com/questions/27126387/vfprintf-throws-segfault-but-input-is-well-defined#comment42752821_27126509 */
	/* This is why I'm creating 2 va_lists one the fprintf equivalent and the other one is the sprintf. */
	va_list vargs;
	va_list vargs_dbg;

	va_start(vargs, data2send);

	/* copy vargs -> vargs_dbg */
	va_copy(vargs_dbg, vargs);

	vfprintf((info) ? info : stderr, data2send, vargs_dbg);
	vsprintf(string2send, data2send, vargs);

	va_end(vargs);
	va_end(vargs_dbg);

	if (write(sock2server, string2send, strlen(string2send)) < 0) {
		perror("write");
		return -1;
	}

	return 0;
}

int get_mac(char *interface){
	int fd;
	struct ifreq ifr;
	unsigned char *mac = NULL;

	memset(&ifr, 0, sizeof(ifr));

	fd = socket(AF_INET, SOCK_DGRAM, 0);

	ifr.ifr_addr.sa_family = AF_INET;
	strncpy(ifr.ifr_name , interface , IFNAMSIZ-1);

	if (0 == ioctl(fd, SIOCGIFHWADDR, &ifr)) {
		mac = (unsigned char *)ifr.ifr_hwaddr.sa_data;
		// if interface == "lo"; it prints -> mac : 00:00:00:00:00:00
		if (strcmp(interface, "lo") != 0)
			send_data(sock, "mac : %.2X:%.2X:%.2X:%.2X:%.2X:%.2X\n" , mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
	}
	close(fd);
	return 0;
}
