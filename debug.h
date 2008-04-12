#ifndef ERROR_H_
#define ERROR_H_

#include <stdio.h>
#include <iostream>

#define report_error(msg) report_error_function(msg, __FILE__, __LINE__);
#define got_here() printf("got here: %s:%d\n", __FILE__, __LINE__); fflush(stdout);
#define print_me(msg) std::cout << __FILE__ << ":" << __LINE__ << " \"" << msg << "\"" << std::endl;

//this could later on log to a file
void report_error_function(char *p_string, char *p_filename, int p_line);	//dont call this directly
void log_this(char *p_string);

#endif //ERROR_H_

