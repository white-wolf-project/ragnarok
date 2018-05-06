#include <stdbool.h>
#include <string.h>
#include <libxml/parser.h>
#include <libxml/tree.h>

#define TOOL_VERSION "0.0.1"
#define TOOLNAME "ragnarok"
#define SRV_TOOLNAME "ragnarok-srv"

#define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)
#define log_it(...) logger(LOG_TYPE, __FILENAME__, __LINE__, __VA_ARGS__)

extern xmlDocPtr doc;
extern xmlNodePtr root, device_AP_name, info_ap, info_ap_nb;
extern xmlNodePtr device_AP_name_child, info_ap_child;

void version();
void debug(const char* format, ...);

int logger(int type, const char *file, int line, const char *format, ...);

bool file_exists(const char* file);
int run_python(const char *pyscript, const char *param[]);
char *get_date_and_time(void);
char *get_txt(const char *str, const char *p1, const char *p2);

void init_xml(char *docname);
void end_xml(char *docname);

int get_instance_pid(const char *file);
char *get_wireless(void);
char *get_ip(const char *net_interface);