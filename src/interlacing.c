#include "interlacing.h"

void get_y_values(int pass, int *offset, int *skip)
{
	*offset = 0;
	*skip 	= 1;
}

void get_x_values(int pass, int y, int *offset, int *skip)
{
	*offset = 0;
	*skip	= 1;
}

int get_row_count(int pass, int height)
{
	int yOff, ySkip;
	get_y_values(pass, &yOff, &ySkip);
	
	return (height - yOff) / ySkip;
}

int get_row_width(int pass, int y, int width)
{
	int xOff, xSkip;
	get_x_values(pass, y, &xOff, &xSkip);
	
	return (width - xOff) / xSkip;
}
