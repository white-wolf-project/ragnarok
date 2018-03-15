extern int sock;
char *iface, *port;
void INThandler(int sig);
int init_daemon(void);
int get_srv_pid(const char *file);