#include <stdio.h>

/* local headers */
#include <include/client.h>
#include <include/iwlist.h>
#include <include/common.h>

int main(int argc, char const *argv[]){
	if (argc == 2)
	{
		run_iwlist(argv[1]); // wlo1 for Ubuntu, not sure about other distros
	} else {
		run_iwlist("wlan0");
	}
	return 0;
}
