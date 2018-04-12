#include <libxml/parser.h>
extern char  *ipaddr, *port, *iface;

int parse_config_file(const char *xmlfile);
int is_leaf(xmlNode * node);
void print_xml(xmlNode * node, int indent_len);