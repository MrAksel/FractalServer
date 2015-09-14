#include <stdlib.h>
#include "log.h"
#include "network.h"
#include "mandelbrot.h"
#include "interlacing.h"

#define MIN(x, y) ((x < y) ? x : y)

int calc_mandelbrot(struct fractal_params * fractal, struct render_params * render)
{
	// Loop through every row supposed to be processed in this pass
	LOG(PRIO_VERBOSE, "Rendering y=%d to y=%d\n", render->yOff, render->yMax);
	for (int32_t y = render->yOff; y < render->yMax; y += render->ySkip)
	{
		if (!ProcessRow(fractal, render, y))
		{
			LOG(PRIO_ERROR, "Canceled on row %d\n", y);
			return 0;
		}
	}
	
	int32_t finito = -1; // Send a negative number to signify no more orbits for client
	if (!network_write(&finito, 4, 1))
		return 0;
		
	return 1;
}

int ProcessRow(struct fractal_params * fractal, struct render_params * render, int32_t y)
{
	mp_t im0; mp_init(im0); //Mapped y to imaginary value
	mp_map(im0, y, 0, render->height, fractal->min_im, fractal->max_im);

	LOG(PRIO_VVERBOSE, "Rendering x=%d to x=%d\n", render->xOff, render->xMax);
	for (int32_t x = render->xOff; x < render->xMax; x += render->xSkip)
	{
		if (!ProcessPoint(fractal, render, im0, x, y))
		{
			LOG(PRIO_ERROR, "Failed to calculate x=%d y=%d\n", x, y);
			mp_clear(im0);
			return 0;
		}
	}

	LOG(PRIO_VVERBOSE, "Finished row %d\n", y);
	return 1;
}

uint32_t ProcessPoint(struct fractal_params * fractal, struct render_params * render, mp_t im0, int32_t x, int32_t y)
{
	int retcode = 0; // Only set to 1 (success) if not 'goto cleanup'

	mp_t re0; mp_init(re0);
	mp_map(re0, x, 0, render->width, fractal->min_re, fractal->max_re);

	// Cardioid / bulb checking if enabled in render params
	if (render->skip_primary_bulbs && InBulb(re0, im0))
	{
		LOG(PRIO_VVVERBOSE, "Yeah not any work to do!\n");

		int32_t zero = 0;
		
		// Write data
		if (!network_write(&x, 4, 1) || 	// x
			!network_write(&y, 4, 1)) ||	// y
			!network_write_q(&re0) ||		// re
			!network_write_q(&im0) ||		// im
		    !network_write(&zero, 4, 1) ||  // Iteration count
		    !network_write(&zero, 4, 1))    // Orbit length
		{
			LOG(PRIO_ERROR, "Failed to write to client\n");
			mp_clear(re0);			
			
			return 0;
		}
		
		mp_clear(re0);
		return 1;
	}

	mp_t *orbit = NULL;
	if (render->orbit_length)
	{
		orbit = (mp_t *)malloc(2 * render->orbit_length * sizeof(mp_t));
		if (!orbit)
		{
			LOG(PRIO_ERROR, "Failed to allocate orbit buffer\n");
			mp_clear(re0);
			return 0; 
		}
	}

	mp_t z_re;	mp_init_set(z_re, re0);
	mp_t z_im;	mp_init_set(z_im, im0);

	mp_t z_re_sq;	mp_init(z_re_sq);
	mp_t z_im_sq;	mp_init(z_im_sq);
	mp_t euclidd;	mp_init(euclidd);

	mp_t z_re_tmp;	mp_init(z_re_tmp);

	uint32_t iter = 0;

	while (iter < render->iteration_count) // Loop while under max count limit
	{
		int orbitIndex = iter - render->orbit_start;
		if (orbit && orbitIndex >= 0 && orbitIndex < render->orbit_length)
		{
			// Store position in orbit array
			mp_init_set(orbit[2 * orbitIndex], z_re);
			mp_init_set(orbit[2 * orbitIndex + 1], z_im);
		}

		// Calculate new distance from center
		mp_mul(z_re_sq, z_re, z_re);
		mp_mul(z_im_sq, z_im, z_im);
		mp_add(euclidd, z_re_sq, z_im_sq);

		if (mp_cmp_si(euclidd, 4) >= 0)	// Only loop while inside radius 2
			break;

		//Perform one iteration 

		// xtemp = x*x - y*y + x0
		// Already calculated squared values
		mp_sub(z_re_tmp, z_re_sq, z_im_sq);
		mp_add(z_re_tmp, z_re_tmp, re0);

		// y = 2*x*y + y0
		mp_mul(z_im, z_re, z_im);	//x*y
		mp_mul_si(z_im, z_im, 2);	//x*y*2
		mp_add(z_im, z_im, im0);		//x*y*2 + y0

		// x = xtemp
		mp_swap(z_re, z_re_tmp);

		// Increment iteration counter
		iter++;
	}

	uint32_t ob_count = MIN(render->orbit_length, iter);

	// TODO Implement off-grid orbits (algorithms to choose good long orbits for buddhabrot)
	if (!network_write(&x, 4, 1) ||
		!network_write(&y, 4, 1))
	{
		LOG(PRIO_ERROR, "Failed to write to client");
		goto cleanup;	
	}
	
	if (!network_write_q(&re0) ||
		!network_write_q(&im0))
	{
		LOG(PRIO_ERROR, "Failed to write to client\n");
		goto cleanup;
	}

	// Send iteration count & orbit length
	if (!network_write(&iter, 4, 1) ||
		!network_write(&render->orbit_start) ||
	    !network_write(&ob_count, 4, 1)) 
	{
		LOG(PRIO_ERROR, "Failed to write to client\n");
		goto cleanup;
	}


	int tryWrite = 1; 
	for (uint32_t i = 0; i < ob_count; i++)
	{
		if (tryWrite)
		{
			// Write rationals to client
#ifdef MP_FLOATS
			mpq_t treal, timag;
			mpq_inits(treal, timag, NULL);

			mpq_set_f(treal, orbit[2 * i]); // Convert to rationals for exporting
			mpq_set_f(timag, orbit[2 * i + 1]); 

			int res = network_write_q(&treal);
			if (res)
			{
				res = network_write_q(&timag);
			}
			if (!res)
			{
				LOG(PRIO_ERROR, "Failed to write rationals to client\n");
				tryWrite = 0;
			}

			mpq_clear(treal);
			mpq_clear(timag);
#else
			int res = network_write_q(&orbit[2 * i]);
			if (res)
			{
				res = network_write_q(&orbit[2 * i + 1]);
			}
			if (!res)
			{
				LOG(PRIO_ERROR, "Failed to write rationals to client\n");
				tryWrite = 0;
			}
#endif
		}
	}

	retcode = tryWrite;
cleanup:

	// Clear list of visited points
	for (uint32_t i = 0; i < ob_count; i++)
	{
		mp_clear(orbit[2 * i]);
		mp_clear(orbit[2 * i + 1]);		
	}
	free(orbit);

	mp_clear(re0); // TODO mp_clears
	mp_clear(z_re);
	mp_clear(z_im);
	mp_clear(z_re_sq);
	mp_clear(z_im_sq);
	mp_clear(euclidd);
	mp_clear(z_re_tmp);

	return retcode;
}

int InBulb(mp_t z_re, mp_t z_im)
{
	mp_t z_re_sq;
	mp_t z_im_sq;
	mp_t q, xmq, ls, rs;

	mp_init(z_re_sq);
	mp_init(z_im_sq);
	mp_init(q); 		//TODO mp_inits
	mp_init(ls);
	mp_init(rs);

	mp_mul(z_re_sq, z_re, z_re); //Square input
	mp_mul(z_im_sq, z_im, z_im);


	mp_init_set(xmq, z_re);		// xmq = z_re
	mp_sub_r(xmq, xmq, 1, 4); 	// xmq = z_re - 1/4

	mp_mul(q, xmq, xmq);		// q = (x - 1/4)²
	mp_add(q, q, z_im_sq);		// q = (x - 1/4)² + z_im²

	mp_add(ls, q, xmq);			// left side =   q + (x - 1/4)
	mp_mul(ls, ls, q);			// left side = q(q + (x - 1/4))

	mp_mul_r(rs, z_im_sq, 1, 4);// right side = z_im² / 4

	if (mp_cmp(ls, rs) < 0)		// Check if inside main bulb..
	{
		mp_clear(z_re_sq);
		mp_clear(z_im_sq);
		mp_clear(q);
		mp_clear(ls);
		mp_clear(rs);
		mp_clear(xmq);
		return 1; // In primary bulb
	}

	mp_add_r(ls, z_re, 1, 1);	// x+1
	mp_mul(ls, ls, ls);			// (x+1)²
	mp_add(ls, ls, z_im_sq);	// (x+1)² + y²

	if (mp_cmp_r(ls, 1, 16) < 0)
	{
		return 2; // In secondary bulb
	}

	mp_clear(z_re_sq); // TODO mp_clears
	mp_clear(z_im_sq);
	mp_clear(q);
	mp_clear(ls);
	mp_clear(rs);
	mp_clear(xmq);

	return 0; // Outside bulbs, iterate as normal
}
