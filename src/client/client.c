#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <errno.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <time.h>
/* local headers */
#include <include/client.h>
#include <include/xml.h>
#include <include/common.h>
#define STRING_LEN 2048

int sock;

int send_data(int sock2server, const char* data2send, ...){

	// TODO : fix length
	char string2send[STRING_LEN];

	/*
	* When you use va_arg() "function" on the va_list obtained
	* from va_start(),
	* the va_list is modified, so when passed to another function
	* it is NOT the same as you got at the beginning
	* https://stackoverflow.com/questions/27126387/vfprintf-throws-segfault-but-input-is-well-defined#comment42752821_27126509
	* This is why I'm creating 2 va_lists first one is the fprintf equivalent and the other one is sprintf.
	*/

	va_list vargs;
	va_list vargs_out;
	va_list vargs_log;
	va_start(vargs, data2send);

	/* copy vargs -> vargs_out (stdout) & vargs_log (client.log)*/
	va_copy(vargs_out, vargs);
	va_copy(vargs_log, vargs);

	FILE *fp = fopen("client.log" ,"a+");
	if (fp != NULL)
	{
		vfprintf(fp, data2send, vargs_log);
		fclose(fp);
	} else {
		#ifdef DEBUG_ADV
		perror("fopen");
		#endif
	}
	
	#ifdef DEBUG
	vfprintf(stdout, data2send, vargs_out);
	#endif
	
	vsprintf(string2send, data2send, vargs);

	va_end(vargs);
	va_end(vargs_out);
	if (strcmp(ipaddr, "0") != 0){
		if (write(sock2server, string2send, strlen(string2send)) < 0) {
			perror("write");
			return -1;
		}
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


void get_time(void)
{
	time_t local_time;
	struct tm * tm;
	time(& local_time);
	tm = localtime(& local_time);
	send_data(sock, "local time = %02d/%02d/%02d - %02d:%02d:%02d \n", tm->tm_mday, tm->tm_mon + 1, tm->tm_year % 100, tm->tm_hour, tm->tm_min, tm->tm_sec);
}
