/* gcc run_python.c  -I/usr/include/python3.5 -Wall -g -o run_python -lpython3.5m */
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <Python.h>

int run_python(const char *pyscript, const char *param[]);

int main(int argc, char const *argv[])
{	
	const char *arg_tab[12];
	if (argc != 4)
	{	
		arg_tab[0] = NULL;
		arg_tab[1] = "../src/python/test/ragnarok1.xml"; 
		arg_tab[2] = "../src/python/test/ragnarok2.xml"; 
		arg_tab[3] = "../src/python/test/ragnarok3.xml";
	} 
	else {
		for (int i = 1; i < argc; ++i)
		{
			arg_tab[i] = argv[i];
		}
	}
	printf("create database\n");
	run_python("../src/python/ragnarok_bdd.py", NULL);
	sleep(1);
	printf("run parser\n");
	run_python("../src/python/xmlparser.py", arg_tab);
	return 0;
}

int run_python(const char *pyscript, const char *param[]) {
	FILE* file;
	wchar_t *argvw[5];
	
	/*if (!file_exists(pyscript))
	{
		fprintf(stderr, "%s: %s\n", pyscript, strerror(errno));
		return -2;
	}*/
	file = fopen(pyscript,"r");
	if (file == NULL){
		fprintf(stderr, "%s: %s\n", pyscript, strerror(errno));
	}
	argvw[0] = Py_DecodeLocale(pyscript, NULL);

	if (param != NULL)
	{	
		for (int i = 1; i <= 3; i++)
		{	printf("%d : %s\n", i, param[i]);

			argvw[i] = Py_DecodeLocale(param[i], NULL);
		}
	}

	Py_SetProgramName(argvw[0]);
	Py_Initialize();

	if (param != NULL)
	{
		PySys_SetArgv(4, argvw);
	}

	PyRun_SimpleFile(file, pyscript);
	Py_Finalize();
	return 0;
}