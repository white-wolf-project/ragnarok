#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <include/server.h>
#include <include/common.h>

int check4xml(const char *data){
	if (strstr(data , "-xml-") != NULL){
		return 0;
	} else if (strstr(data, "-end_xml-") != NULL){
		return 1;
	} else {
		return 2;
	}
}

// xml_val = end or begin 1 = end; 0 = begin
void write_to_xml(FILE *fp_xml, const char *xml_data, int xml_data_len, int xml_start, int xml_end){ 
	char *end_of_xml = strstr(xml_data, "-end_xml-");

	if (xml_start == 1) {
		fwrite(xml_data + strlen("-xml-"), sizeof(char), xml_data_len - strlen("-xml-"), fp_xml);
	} else if (xml_end == 1) {
		end_of_xml[0] = '\0';
		fwrite(xml_data, sizeof(char), xml_data_len - strlen("-end_xml-"), fp_xml);
	} else if (xml_start == 0 && xml_end == 0) {
		fwrite(xml_data, sizeof(char), xml_data_len, fp_xml);
	}
}

int handle_xml_data(int counter){
	const char *arg_tab[4] = {0};

#ifdef RELEASE
	arg_tab[0] = NULL;
	arg_tab[1] = "/var/opt/ragnarok1.xml";
	arg_tab[2] = "/var/opt/ragnarok2.xml";
	arg_tab[3] = "/var/opt/ragnarok3.xml";
#else
	arg_tab[0] = NULL;
	arg_tab[1] = "server_0.xml";
	arg_tab[2] = "server_1.xml";
	arg_tab[3] = "server_2.xml";
#endif
	if (counter == 2) {
		log_it("check if ragnarok_bdd exists");

		/* this statement is not really useful, we don't need it for the moment */
		if (check4db("ragnarok_bdd") != true) {
			log_it("ragnarok_bdd does not exist, creating it");
			run_python("src/python/ragnarok_bdd.py", NULL);
			sleep(1);
			// system("python3 src/python/xmlparser.py test/server_0.xml test/server_1.xml test/server_2.xml");
			run_python("src/python/xmlparser.py", arg_tab);
		} else {
			log_it("ragnarok_bdd found");
			// system("python3 src/python/xmlparser.py test/server_0.xml test/server_1.xml test/server_2.xml");
			run_python("src/python/xmlparser.py", arg_tab);
		}
	}
	return 0;
}