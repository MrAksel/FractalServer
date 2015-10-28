#include <gmp.h>

extern int 			port;
extern int 			log_priority_mask;
extern mp_bitcnt_t	float_default_precision;

int parse_args(int argc, char** argv);
void show_usage();