/**
 * @file send_xml.c
 * @author Mathieu Hautebas
 * @date 22 March 2018
 * @brief file to test ragnarok-srv
 * we send an XML file to check if server can handle it.
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#define BUFSIZE 1024 /* read 1024 bytes at a time */

int main(int argc, char const *argv[]){
	FILE* fp;
	int sock;
	struct sockaddr_in server;
	char buf[BUFSIZE];

	if (argc != 2)
	{
		printf("[-] xml file needed\n");
		return -1;
	}

	//Create socket
	sock = socket(AF_INET , SOCK_STREAM , 0);
	if (sock == -1)
	{
		printf("Could not create socket");
	}
	puts("Socket created");

	server.sin_addr.s_addr = inet_addr("127.0.0.1");
	server.sin_family = AF_INET;
	server.sin_port = htons(12345);

	//Connect to remote server
	if (connect(sock , (struct sockaddr *)&server , sizeof(server)) < 0)
	{
		perror("connect failed. Error");
		return 1;
	}

	puts("Connected\n");

	if ((fp = fopen(argv[1], "r")) == NULL)
	{ /* Open source file. */
		perror("fopen");
		return 1;
	}
	printf("%ld\n", strlen("-xml-"));
	if(write(sock , "-xml-\0" , strlen("-xml-\0")) < 0)
	{
		puts("Send failed");
		return 1;
	}

	while (fgets(buf, sizeof(buf), fp) != NULL)
	{
		/* eat the newline fgets() stores */
		buf[strlen(buf) - 1] = '\0';
		if(write(sock , buf , strlen(buf)) < 0)
		{
			puts("Send failed");
			return 1;
		}
		printf("%s\n", buf);
		sleep(0.5);
	}
	if(write(sock , "-end_xml-" , strlen("-end_xml-")) < 0)
	{
		puts("Send failed");
		return 1;
	}

	fclose(fp);
	return 0;
}
