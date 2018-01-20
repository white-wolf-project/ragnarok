#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdarg.h>
#include <errno.h>
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
	va_copy(vargs_dbg, vargs);

	vfprintf((info) ? info : stderr, data2send, vargs_dbg);
	vsprintf(string2send, data2send, vargs);
	va_end(vargs);

	if (write(sock2server, string2send, strlen(string2send)) < 0) {
		perror("write");
		return -1;
	}

return 0;
}