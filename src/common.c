#include <stdio.h>
#include <stdarg.h>
#include <Python.h>
#include <stdbool.h>

#include <include/common.h>

#ifdef DEBUG
static FILE* dbg = NULL;
#endif

/*
*	We call this function almost everywhere
*	But it prints to stdout/stderr only
*	when you build the project in DEBUG mode
*	for the moment
*/
void debug(const char* format, ...) {
	#ifdef DEBUG
	va_list vargs;
	va_start(vargs, format);
	vfprintf((dbg) ? dbg : stderr,format, vargs);
	va_end(vargs);
	#endif
}

/*
*	This functions checks if a file exists
*	returns TRUE if exists
*	in the other case it returns FALSE.
*	param is the name of the file
*/
bool file_exists(const char* file){
	if(access(file, F_OK ) != -1) {
		return true;
	} else {
		return false;
	}
}

/*
*	Call a Python script with this function
*	param is the python script you want to run
*	We use Python3
*/
int run_python(const char *pyscript){
	Py_Initialize();
	FILE* file = fopen(pyscript, "r");
	if (!file_exists(pyscript))
	{
		fprintf(stderr, "%s: %s\n", pyscript, strerror(errno));
		return -2;
	}
	PyRun_SimpleFile(file, pyscript);
	Py_Finalize();
	return 0;
}