#include <stdio.h>
#include <include/common.h>
#include <include/server.h>

int main(void){
	char *wireless_out;
	wireless_out = get_wireless();
	if (wireless_out == NULL)
	{
		return 1;
	}
	fprintf(stdout, "%s\n", wireless_out);
	return 0;
}
