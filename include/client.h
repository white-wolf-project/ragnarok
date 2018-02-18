extern int sock;
char *ipaddr;
char *port;
int parse_config_file(const char *xmlfile);
int send_data(int sock2server, const char* data2send, ...);
int get_mac(char *interface);
void get_time(void);