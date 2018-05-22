#define LG_BUFFER	1024
#define LOG_TYPE 0

struct addrinfo *results;

extern int sock;

int send_data(int sock2server, const char* data2send, ...);
int read_and_send_data(const char *xmlfile);
int init_client (int server, char *host, char *port, struct addrinfo **result);
int init_client_daemon(const char *pidfile);