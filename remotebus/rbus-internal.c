#include <stdio.h>
#include <stdarg.h>
#include <include/remotebus-internal.h>


void _rbus_verbose_real (const char *file, int line, const char *function_name, const char *format,...) {
	va_list args;
	fprintf (stderr, "File: %s, Line: %d, Function: %s\n", file, line, function_name);
	va_start (args, format);
	vfprintf (stderr, format, args);
	va_end (args);
	fflush(stderr);
}
