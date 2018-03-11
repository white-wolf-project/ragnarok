#include <stdbool.h>
#define VERSION "0.0.1"
#define TOOLNAME "ragnarok"
#define SRV_TOOLNAME "ragnarok-srv"

void version();
void debug(const char* format, ...);
bool file_exists(const char* file);
int run_python(const char *pyscript);
char *get_date_and_time(void);
char *get_txt(const char *str, const char *p1, const char *p2);