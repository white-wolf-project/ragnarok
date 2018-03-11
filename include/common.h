#include <stdbool.h>
#include <libxml/parser.h>
#include <libxml/tree.h>

#define VERSION "0.0.1"
#define TOOLNAME "ragnarok"
#define SRV_TOOLNAME "ragnarok-srv"

extern xmlDocPtr doc;
extern xmlNodePtr root, device_AP_name, info_ap, info_ap_nb;
extern xmlNodePtr device_AP_name_child, info_ap_child;

void version();
void debug(const char* format, ...);
bool file_exists(const char* file);
int run_python(const char *pyscript);
char *get_date_and_time(void);
char *get_txt(const char *str, const char *p1, const char *p2);

void init_xml(char *docname);
void end_xml(char *docname);