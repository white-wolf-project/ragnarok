extern int sock;
char *ipaddr;
char *port;

int parse_config_file(const char *xmlfile);
int send_data(int sock2server, const char* data2send, ...);
int read_and_send_data(const char *xmlfile);
int init_client_daemon(void);