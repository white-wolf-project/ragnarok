#include <stdbool.h>
int check4xml(const char *data);
void write_to_xml(FILE *fp_xml, const char *xml_data, int xml_data_len, int xml_start, int xml_end);
int handle_xml_data(int counter);

bool check4db(const char *db_name);
int remove_db(const char *db_name);