/**
 * @file common.c
 * @author Mathieu Hautebas
 * @date 22 March 2018
 * @brief File containing common code used client and server.
 *
 */

#include <stdio.h>
#include <stdarg.h>
#include <Python.h>
#include <stdbool.h>
#include <ifaddrs.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <libxml/parser.h>
#include <libxml/tree.h>
/* local headers */
#include <include/client.h>
#include <include/common.h>
#include <include/xml.h>
#include "sysnet/include/network.h"

#ifdef DEBUG
static FILE* dbg = NULL; // file used only in DEBUG mode.
#endif

/**
 * @brief
 * prints version informations :
 * TOOLNAME, TOOL_VERSION, DATE and TIME
 * @return void, no return
 */
void version(){
	#ifdef DEBUG
		fprintf(stdout, "%s, version %s-DEBUG\nCompiled on %s at %s\nCopyright 2018 - White Wolf Team\n", TOOLNAME, TOOL_VERSION, __DATE__, __TIME__);
	#else
		fprintf(stdout, "%s, version %s\nCompiled on %s at %s\nCopyright 2018 - White Wolf Team\n", TOOLNAME, TOOL_VERSION, __DATE__, __TIME__);
	#endif
}

/**
 * @brief
 * We call this function almost everywhere.
 * But it prints to stdout/stderr only
 * when you build the project in DEBUG mode (we use #ifdef DEBUG ... #endif)
 * @param format : Path or name of python script to run
 * @param ... : like printf, which are params
 * @return void, no return
 */
void debug(const char* format, ...) {
	#ifdef DEBUG
	va_list vargs;
	va_start(vargs, format);
	vfprintf((dbg) ? dbg : stderr,format, vargs);
	va_end(vargs);
	#endif
}

/**
 * @brief
 * booleen function to check if a file exists
 * @param file : Path or name of file
 * @return true if file exists, or false if file is not found
 */
bool file_exists(const char* file){
	if(access(file, F_OK ) != -1) {
		return true;
	} else {
		return false;
	}
}

/**
 * @brief
 * Function based on libpyton, use to run Python 3 scripts
 * run like this :
 * @code
 * run_python("file.py", NULL);
 * @endcode
 * or
 * @code
 * run_python("file.py", args);
 * @endcode
 * @param pyscript : Path or name of python script to run
 * @param param : argument to set; put NULL if there is no arg needed. Array needs only three args
 * @return 0 is everything's gone well, or -2 if Python file is not found
 */
int run_python(const char *pyscript, const char *param[]) {
	FILE* file;
	wchar_t *argvw[4];

	if (!file_exists(pyscript))
	{
		fprintf(stderr, "%s: %s\n", pyscript, strerror(errno));
		return -2;
	}

	argvw[0] = Py_DecodeLocale(pyscript, NULL);

	if (param != NULL)
	{
		for (int i = 1; i <= 3; i++)
		{
			debug("%d : %s\n", i, param[i]);
			if (file_exists(param[i])){
				argvw[i] = Py_DecodeLocale(param[i], NULL);
			} else {
				debug("%s : %s\n", param[i], strerror(errno));
				return -1;
			}
		}
	}

	Py_SetProgramName(argvw[0]);
	Py_Initialize();

	if (param != NULL)
	{
		PySys_SetArgv(4, argvw);
	}

	file = fopen(pyscript,"r");
	PyRun_SimpleFile(file, pyscript);
	Py_Finalize();
	return 0;
}

/**
 * @brief
 * get date and time (thank you captain obvious)
 * This function is used in iwlist to get the time
 * of the scan
 * You can use it like this
 * @code
 * printf("%s\n", get_date_and_time());
 * @endcode
 * @return curr_time; with print to stdout, output is like this : "02/03/18-15:12:32"
 */
char *get_date_and_time(void)
{
	time_t local_time;
	struct tm * tm;
	char *curr_time;
	curr_time = malloc(sizeof(char) *30);
	time(& local_time);
	tm = localtime(& local_time);
	sprintf(curr_time, "%02d/%02d/%02d-%02d:%02d:%02d", tm->tm_mday, tm->tm_mon + 1, tm->tm_year % 100, tm->tm_hour, tm->tm_min, tm->tm_sec);
	return (char *)curr_time;
}

/**
 * @brief
 * Find text between two patterns
 * This function is use to check when XML start and ends
 * Then we split log and XML
 * Simple usage : cut useless stuff
 * @code
 * char new_buf[12];
 * sprintf(new_buf, "%sa", buffer);
 * channel_out = get_txt(new_buf, "Channel:", "a");
 * send_data(sock, "channel : %s\n", channel_out);
 * @endcode
 * @param str : string to find
 * @param p1 : first pattern
 * @param p2 : last pattern
 * @return target which is the output we want or NULL if we did not find anything
 * @note Used in the server part
 */
char *get_txt(const char *str, const char *p1, const char *p2){
	char *target = NULL;
	char *start, *end;

	if ((start = strstr(str, p1))) {
		start += strlen(p1);
		if ((end = strstr(start, p2)))
		{
			target = (char *)malloc(end - start + 1);
			memcpy(target, start, end - start);
			target[end - start] = '\0';
		}
	}

	if (target)
		return (char *)target;
	else 
		return NULL;
}

xmlDocPtr doc = NULL;
xmlNodePtr root = NULL, device_AP_name = NULL, info_ap = NULL;
xmlNodePtr device_AP_name_child = NULL, info_ap_child = NULL;

/**
 * @brief
 * Start xml process.
 * Call this function once to start XML related stuff
 * Make sure these vars are initialized
 * @code
 * xmlDocPtr doc = NULL;
 * xmlNodePtr root = NULL, device_AP_name = NULL, info_ap = NULL;
 * xmlNodePtr device_AP_name_child = NULL, info_ap_child = NULL;
 * @endcode
 * @param docname : name or path to file we will write XML data
 * @return void, no return.
 */
void init_xml(char *docname){
	doc = xmlNewDoc(BAD_CAST "1.0");
	root = xmlNewNode(NULL, BAD_CAST docname);
	xmlDocSetRootElement(doc, root);
	/* create two childs we will deal with */
	device_AP_name = xmlNewChild(root, NULL, BAD_CAST "rpi_info", NULL);

	device_AP_name_child = xmlNewChild(device_AP_name, NULL, BAD_CAST "mac", NULL);
	xmlNodeAddContent(device_AP_name_child, BAD_CAST get_mac_addr(get_wireless()));

	device_AP_name_child = xmlNewChild(device_AP_name, NULL, BAD_CAST "ip", NULL);
	xmlNodeAddContent(device_AP_name_child, BAD_CAST get_ip(iface));

	device_AP_name_child = xmlNewChild(device_AP_name, NULL, BAD_CAST "time", NULL);
	xmlNodeAddContent(device_AP_name_child, BAD_CAST get_date_and_time());
}

/**
 * @brief
 * End xml process.
 * Call this function once we are done with XML related stuff
 * @param docname : name or path to file we will write XML data
 * @return void, no return.
 * @note use init_xml() before this function
 */

void end_xml(char *docname){
	/* 	this commented line can output the XML file to stdout
	* 	I keep it for debugging purposes
	*/
	// xmlSaveFormatFileEnc("-", doc, "UTF-8", 1);
	xmlSaveFormatFileEnc(docname, doc, "UTF-8", 1);
	xmlFreeDoc(doc);
}

/**
 * @brief
 * Get PID of client or server process located in a file.
 * We call this function find the PID then we kill client or server process
 * @param file name or path to the file containing the PID.
 * @return pid_val which is a char but we use atoi() function to convert it to an integer.
 * @note only works with one-line files
 */

int get_instance_pid(const char *file){
	FILE *fp = NULL;
	char *pid_val = NULL;
	size_t len = 0;

	fp = fopen(file, "r");
	/* check if we can access file */
	if (fp == NULL){
		return -1;
	}
	/* get the first line of the file which is PID */
	getline(&pid_val, &len, fp);
	fclose(fp);

	/* return PID of server */
	return atoi(pid_val);
}

/**
 * @brief
 * Function to get wireless interface
 * example usage :
 * Create a pointer to char
 * @verbatim char *my_wireless_interface; @endverbatim
 * Here is how you call it
 * @code
 * my_wireless_interface = get_wireless();
 * printf("wireless interface found : %s\n", my_wireless_interface);
 * @endcode
 * @return returns the first wireless interface found or NULL, if there is no interface
 * @see https://gist.github.com/edufelipe/6108057
 * @warning Only tested on Linux Operating System.
 */
char *get_wireless(void) {
	struct ifaddrs *ifaddr, *ifa;
	char *wireless_iface;
	if (getifaddrs(&ifaddr) == -1) {
		perror("getifaddrs");
		return NULL;
	}

	/* walk through linked list, maintaining head pointer so we can free list later */
	for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) {
		char protocol[IFNAMSIZ]  = {0};

		if (ifa->ifa_addr == NULL || ifa->ifa_addr->sa_family != AF_PACKET)
			continue;

		if (check_wireless(ifa->ifa_name, protocol)) {
			wireless_iface = ifa->ifa_name;
			freeifaddrs(ifaddr);
			return (char *)wireless_iface;
		}
	}
	freeifaddrs(ifaddr);
	return NULL;
}

/**
 * @brief
 * Function to get IP address of an Interface
 * example usage :
 * Create a pointer to char
 * @verbatim char *my_interface; @endverbatim
 * Here is how you call it
 * @code
 * my_interface = get_ip();
 * printf("interface IP is : %s\n", my_interface);
 * @endcode
 * You will have to know the network interface.
 * @param net_interface Network interface. Type : const char *
 * @return IP address of the network interface
 * @see https://stackoverflow.com/questions/2283494/get-ip-address-of-an-interface-on-linux
 */
char *get_ip(const char *net_interface){
	int fd;
	struct ifreq ifr;

	fd = socket(AF_INET, SOCK_DGRAM, 0);

	/* type of addr is IPv4 */
	ifr.ifr_addr.sa_family = AF_INET;

	/* copy the interface name to ifreq structure */
	strncpy(ifr.ifr_name, net_interface, IFNAMSIZ-1);

	ioctl(fd, SIOCGIFADDR, &ifr);

	close(fd);
	return (char *)inet_ntoa(( (struct sockaddr_in *)&ifr.ifr_addr )->sin_addr);
}
