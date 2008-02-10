#include "debug.h"

void report_error_function(char *p_string, char *p_filename, int p_line)
{
	printf("Error occured in file %s on line %d: ", p_filename, p_line);
	printf("%s\n", p_string);
}

void log_this(char *p_string)
{
	printf("this should be put in a file instead: ");
	printf("%s\n", p_string);
}
