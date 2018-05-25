#include <stdbool.h>

#define LOG_TYPE 1

extern int sock;
char *iface, *port;

void INThandler(int sig);
void manage_co(int sock, int counter);

int init_srv_daemon(const char *pidfile);
int tcp_server(const char* service_port);

bool check4db(const char *db_name);
int remove_db(const char *db_name);