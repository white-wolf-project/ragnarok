#include <openssl/ssl.h>
extern int sock;
extern SSL *ssl;
char *ipaddr;
char *port;
int parse_config_file(const char *xmlfile);
int send_data(SSL *ssl, const char* data2send, ...);
int get_mac(char *interface);
