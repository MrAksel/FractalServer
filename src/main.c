#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include "mandelbrot.h"
#include "network.h"
#include "args.h"
#include "log.h"

FILE * dump;

int main(int argc, char** argv)
{
#ifdef MP_RATIONALS
	LOG(PRIO_INFO, "Compiled with MP_RATIONALS\n");
#else
	LOG(PRIO_INFO, "Compiled with MP_FLOATS\n");
#endif

	if (!parse_args(argc, argv))
	{
		LOG(PRIO_ERROR | PRIO_CRITICAL, "Failed initializing socket\n");
		exit(1);
	}
	if (!network_init())
	{
		LOG(PRIO_ERROR | PRIO_CRITICAL, "Failed initializing socket\n");
		exit(1);
	}

	struct sigaction sigchld_action = {
	  .sa_handler = SIG_DFL,
	  .sa_flags = SA_NOCLDWAIT
	};
	sigaction(SIGCHLD, &sigchld_action, NULL);	// Prevent spawning of child zombies

	while (1)
	{
		LOG(PRIO_INFO, "Waiting for inbound connection\n");
		if (!network_accept())
		{
			LOG(PRIO_ERROR, "Failed to accept client\n");
			continue;
		}

		pid_t darkside = fork();
		if (darkside != 0)		// On parent side or error
		{
			network_close();
		}
		else if (darkside == 0)	// On child side
		{
			network_stop();

			LOG(PRIO_INFO, "Forked\n");

			struct fractal_params fractal;
			if (!network_read_mp(&fractal))
				network_quit();

			LOG(PRIO_VERBOSE, "Initialized fractal params\n");

			struct render_params param;
			if (!network_read(&param, sizeof(param), 1))
				network_quit();

			/*
			param.iteration_count	= 1000;
			param.interlacing_pass	= 1;
			param.width		= 320;
			param.height	= 240;
			param.xOff		= 0;
			param.xSkip		= 1;
			param.xMax		= 320;
			param.yOff		= 0;
			param.ySkip		= 1;
			param.yMax		= 240;
			param.orbit_start	= 0;
			param.orbit_length	= 0;
			param.skip_primary_bulbs = 1;
			*/

			LOG(PRIO_VERBOSE, "Initialized render params\n");

			if (!calc_mandelbrot(&fractal, &param))
				LOG(PRIO_INFO, "Task failed\n");

			LOG(PRIO_INFO, "Done\n");
			network_finish();
			network_close();
		}
	}

	network_stop();

	LOG(PRIO_INFO, "Parent exiting\n");

	return 0;
}
