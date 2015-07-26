#include <stdarg.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include "args.h"
#include "log.h"

int last_errno = 0;

void LOG_h(int priority, char *file, int line, char *format, ...)
{
	if (priority & log_priority_mask)
	{
		if (priority & PRIO_ERROR)
		{
			if (last_errno != errno)
				printf("%-16s: line %03d: %s\n", file, line, strerror(errno));
				
			printf("%-16s: line %03d: ", file, line);			
			last_errno = errno;
		}
		va_list args;
		va_start(args, format);
	
		vprintf(format, args);
	
		va_end(args);
	}
}
