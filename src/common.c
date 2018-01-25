#include <stdio.h>
#include <stdarg.h>
#include <Python.h>

#include <include/common.h>

#ifdef DEBUG
static FILE* dbg = NULL;
#endif

void debug(const char* format, ...)
{
	#ifdef DEBUG
	va_list vargs;
	va_start(vargs, format);
	vfprintf((dbg) ? dbg : stderr,format, vargs);
	va_end(vargs);
	#endif
}

int run_python(const char *pyscript){
	Py_Initialize();
	FILE* file = fopen(pyscript, "r");
	if (file == NULL)
	{
		fprintf(stderr, "%s: %s\n", pyscript, strerror(errno));
		return -2;
	}
	PyRun_SimpleFile(file, pyscript);
	Py_Finalize();
	return 0;
}
