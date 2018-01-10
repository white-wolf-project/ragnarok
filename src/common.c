#include <stdio.h>
#include <stdarg.h>

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
