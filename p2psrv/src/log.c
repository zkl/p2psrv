#include <stdarg.h>
#include <stdio.h>
#include <time.h>
#include "log.h"

void log_ptime()
{
	char num_str[][3] = {
		"00", "01", "02", "03", "04", "05", "06", "07", "08", "09",
		"10", "11", "12", "13", "14", "15", "16", "17", "18", "19",
		"20", "21", "22", "23", "24", "25", "26", "27", "28", "29",
		"30", "31", "32", "33", "34", "35", "36", "37", "38", "39",
		"40", "41", "42", "43", "44", "45", "46", "47", "48", "49",
		"50", "51", "52", "53", "54", "55", "56", "57", "58", "59",
	};

	char mon_str[][5] = {
		"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", 
		"Otc", "Nov", "Dec"
	};

	time_t utc_time = time(0);
	struct tm lc_time;
	localtime_r(&utc_time, &lc_time);

	printf("<%d %s %s %s:%s:%s> ", lc_time.tm_year + 1900, 
		mon_str[lc_time.tm_mon%12], num_str[lc_time.tm_mday%31], 
		num_str[lc_time.tm_hour%24], num_str[lc_time.tm_min%60],
		num_str[lc_time.tm_sec%60]);
}

void log_alert(const char * format, ...)
{
	va_list ap;
	va_start(ap, format);
	char str[1024];

	log_ptime();
	printf("[alert] - ");
	if(vsnprintf(str, 1024, format, ap))
		printf("%s\n", str);
	
	va_end(ap);
}
void log_error(const char * format, ...)
{
	va_list ap;
	va_start(ap, format);
	char str[1024];

	log_ptime();
	printf("[error] - ");
	if(vsnprintf(str, 1024, format, ap))
		printf("%s\n", str);
	
	va_end(ap);
}
void log_debug(const char * format, ...)
{
	va_list ap;
	va_start(ap, format);
	char str[1024];

	log_ptime();
	printf("[debug] - ");
	if(vsnprintf(str, 1024, format, ap))
		printf("%s\n", str);
	
	va_end(ap);
}
void log_print(const char * format, ...)
{
	va_list ap;
	va_start(ap, format);
	char str[1024];

	log_ptime();
	printf("[print] - ");
	if(vsnprintf(str, 1024, format, ap))
		printf("%s\n", str);
	
	va_end(ap);
}
