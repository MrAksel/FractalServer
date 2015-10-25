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
		LOG(PRIO_ERROR | PRIO_CRITICAL, "Failed parsing arguments\n");
		show_usage();
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

	int errors_c = 0;
	while (1)	// TODO implement signal handler for ^C to exit loop
	{
		LOG(PRIO_INFO, "Waiting for inbound connection\n");
		if (!network_accept())
		{
			LOG(PRIO_ERROR, "Failed to accept client\n");
			errors_c++;
			if (errors_c == 10) break;	// Give up after ten errors
			else				continue;
		}

		pid_t darkside = fork();
		if (darkside != 0)		// the dark side of the fork (parent side or error)
		{
			network_close();	// Close client socket (still open in forked process if success)
		}
		else if (darkside == 0)	// On child side
		{
			network_stop();		// Close listening socket (still open in parent process)

			LOG(PRIO_INFO, "Forked\n");

			uint32_t magic = 0xDEAF;
			if (!network_write(&magic, 4, 1)) 	// Write magic number to client (lets client infer endianness)
				network_quit();

			if (!network_read(&magic, 4, 1) ||	// Read magic number to confirm client
				magic != 0xFEED)
				network_quit();

			struct fractal_params fractal;
			if (!network_read_mp(&fractal))		// Read parameters 
				network_quit();

			LOG(PRIO_VERBOSE, "Received fractal params\n");

			struct render_params param;
			if (!network_read(&param, sizeof(param), 1))	// Read some more parameters
				network_quit();

			LOG(PRIO_VERBOSE, "Received render params\n");
			
			int ok = 200;
			if (!network_write(&ok, 4, 1))	// All is good - send 200 OK
				network_quit();

			if (!calc_mandelbrot(&fractal, &param))	// Perform the task and send results to client (all processing is in calc_mandelbrot)
				LOG(PRIO_ERROR, "Task failed\n");	// ... sucks

			LOG(PRIO_INFO, "Done\n");	// Clean up
			network_finish();	// Shutdown socket properly
			network_close();	// Close client socket (fully closed connection now)
			exit(0);
		}
	}

	network_stop();	// Stop listening

	LOG(PRIO_INFO, "Parent exiting\n");

	return 0;
}
