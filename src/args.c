#include <gmp.h>
#include <argp.h>
#include "args.h"
#include "log.h"

int log_priority_mask = 0xFFFFFFFF;
mp_bitcnt_t float_default_precision;

const char *argp_program_version = "FractalServer 0.1";
const char *argp_program_bug_address = "<mraksel@hotmail.no>";

static char doc[] = "FractalServer - a backend for computing fractals with support for interlacing and parallelizing";

int parse_args(int argc, char** argv)
{
	log_priority_mask = PRIO_ERROR | PRIO_CRITICAL | PRIO_HIGH | PRIO_INFO | PRIO_VERBOSE	;
		
	float_default_precision = 64 * 8; // 64 bytes precision
	mpf_set_default_prec(float_default_precision); //Initialized precision for floating-point numbers
	
	return 1;
}
