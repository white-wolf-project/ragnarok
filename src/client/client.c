#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdarg.h>
/* local headers */
#include <include/client.h>

#define STRING_LEN 2048

static FILE* info = NULL;

int send_data(int sock2server, const char* data2send, ...){

	// TODO : fix length
	char string2send[STRING_LEN];

	/* When you use va_arg() "function" on the va_list obtained
	 * from va_start(),
	 * the va_list is modified, so when passed to another function
	 * it is NOT the same as you got at the beginning
	 * https://stackoverflow.com/questions/27126387/vfprintf-throws-segfault-but-input-is-well-defined#comment42752821_27126509 */
	/* This is why I'm creating 2 va_lists one the fprintf equivalent and the other one is the sprintf. */
	va_list vargs;
	va_list vargs_dbg;

	va_start(vargs, data2send);
	va_copy(vargs_dbg, vargs);

	vfprintf((info) ? info : stderr, data2send, vargs_dbg);
	vsprintf(string2send, data2send, vargs);
	va_end(vargs);

	if (write(sock2server, string2send, strlen(string2send)) < 0) {
		perror("write");
		return -1;
	}

return 0;
}