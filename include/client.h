extern int sock;
char *ipaddr;
char *port;
int parse_config_file(const char *xmlfile);
int send_data(int sock2server, const char* data2send, ...);
char *get_mac_addr(char *interface);
