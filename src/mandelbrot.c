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
			LOG(PRIO_ERROR | PRIO_CRITICAL, "Canceled on row %d\n", y);
			return 0;
		}
	}

	return 1;
}

int ProcessRow(struct fractal_params * fractal, struct render_params * render, int32_t y)
{
	mp_t im0; mp_init(im0); //Mapped y to imaginary value
	mp_map(im0, y, 0, render->height, fractal->min_im, fractal->max_im);

	uint32_t *buffer = NULL;
	int32_t xNum = (render->xMax - render->xOff) / render->xSkip;	// Number of points calculated in this row

	LOG(PRIO_VVERBOSE, "%d points to-be rendered in row %d\n", xNum, y);

	xNum++; // Make room for one int as header;

	if (render->orbit_length == 0)	// Means we're not storing the orbits, create a separate buffer for iterations
	{
		buffer = malloc(xNum * sizeof(uint32_t));

		if (!buffer) // Failed to allocate buffer 
		{
			LOG(PRIO_ERROR | PRIO_HIGH, "Failed to allocate buffer for iteration count at row %d\n", y);
			int32_t snd = -y - 1;	// -1 means error on row 0
			network_write(&snd, 4, 1); // Write negated y value, signifies an error at this row
			return 0;
		}

		buffer[0] = y;
	}

	LOG(PRIO_VVERBOSE, "Rendering x=%d to x=%d\n", render->xOff, render->xMax);
	int index = 1;
	for (int32_t x = render->xOff; x < render->xMax; x += render->xSkip)
	{
		uint32_t iter = ProcessPoint(fractal, render, im0, x, y);

		if (iter == 0)
		{
			if (buffer)
				free(buffer);

			LOG(PRIO_ERROR | PRIO_HIGH, "Failed to calculate x=%d y=%d\n", x, y);
			int32_t snd = -y - 1;				// -1 means error on row 0
			if (!network_write(&snd, 4, 1)) 	// Write negated y value, signifies an error at this row
			{
				LOG(PRIO_ERROR, "Failed to write to client\n");
				return 0;
			}
			return 0; 							// TODO should we just continue with next row? idk..
		}
		else if (buffer)
		{
			buffer[index++] = iter - 1;
		}
	}

	if (buffer)
	{
		LOG(PRIO_VERBOSE, "Writing buffer for row %d\n", y);
		int rcode = network_write(buffer, 4, xNum);
		free(buffer);
		if (!rcode)
		{
			LOG(PRIO_ERROR, "Failed to write buffer for row %d\n", y);
			return 0;
		}
	}

	LOG(PRIO_VVERBOSE, "Finished row %d\n", y);
	return 1;
}

uint32_t ProcessPoint(struct fractal_params * fractal, struct render_params * render, mp_t im0, int32_t x, int32_t y)
{
	mp_t re0; mp_init(re0);
	mp_map(re0, x, 0, render->width, fractal->min_re, fractal->max_re);

	// Cardioid / bulb checking if enabled in render params
	if (render->skip_primary_bulbs && InBulb(re0, im0))
	{
		LOG(PRIO_VVVERBOSE, "Yeah not any work to do!\n");
		mp_clear(re0);
		return 1;
	}

	mp_t *real = NULL;
	mp_t *imag = NULL;
	if (render->orbit_length)
	{
		real = (mp_t *)malloc(render->orbit_length * sizeof(mp_t));
		if (real)
			imag = (mp_t *)malloc(render->orbit_length * sizeof(mp_t));
		if (!imag)
		{
			if (real)
				free(real);

			LOG(PRIO_ERROR | PRIO_CRITICAL, "Failed to allocate orbit buffers\n");
			mp_clear(re0);
			return 0; // Failed to allocate either of buffers
		}
	}

	mp_t z_re;	mp_init_set(z_re, re0);
	mp_t z_im;	mp_init_set(z_im, im0);

	mp_t z_re_sq;	mp_init(z_re_sq);
	mp_t z_im_sq;	mp_init(z_im_sq);
	mp_t euclidd;	mp_init(euclidd);

	mp_t z_re_tmp;	mp_init(z_re_tmp);

	uint32_t iter = 1;

	while (iter < render->iteration_count) // Loop while under max count limit
	{
		int orbitIndex = iter - 1 - render->orbit_start;
		if (imag && orbitIndex >= 0 && orbitIndex < render->orbit_length)
		{
			// Store position in orbit array
			mp_init_set(real[orbitIndex], z_re);
			mp_init_set(imag[orbitIndex], z_im);
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
		mp_mul_r(z_im, z_im, 2, 1);	//x*y*2
		mp_add(z_im, z_im, im0);		//x*y*2 + y0

		// x = xtemp
		mp_swap(z_re, z_re_tmp);

		// Increment counter
		iter++;
	}

	uint32_t retcode = iter;
	if (imag) // Send the whole orbit
	{
		iter--;   // We started counting at 1

		// Send x coord
		int ierr = network_write(&x, 4, 1);

		// Send number of points
		uint32_t ob_count = MIN(render->orbit_length, iter);
		ierr += network_write(&ob_count, 4, 1);

		int tryWrite = (ierr == 2);
		for (int i = 0; i < ob_count; i++)
		{
			if (tryWrite)
			{
#ifdef MP_FLOATS
				mpq_t treal, timag;
				mpq_set_f(treal, real[i]); // Convert to rationals for exporting
				mpq_set_f(timag, imag[i]); 

				// Export treal timag
				int res = network_write_q(treal);
				if (res)
				{
					res = network_write_q(timag);
				}
				if (!res)
				{
					LOG(PRIO_HIGH | PRIO_ERROR, "Failed to write rationals to client\n");
					tryWrite = false;
				}

				mpq_clear(treal);
				mpq_clear(timag);
#else
				// Export real[i] imag[i]
				int res = network_write_q(real[i]);
				if (res)
				{
					res = network_write_q(imag[i]);
				}
				if (!res)
				{
					LOG(PRIO_HIGH | PRIO_ERROR, "Failed to write rationals to client\n");
					tryWrite = false;
				}
#endif
			}

			mp_clear(real[i]);
			mp_clear(imag[i]);
		}

		free(real);
		free(imag);

		if (!tryWrite)
		{
			LOG(PRIO_ERROR | PRIO_CRITICAL, "Failed to write orbit\n");
			retcode = 0; // Failed to write to connection, maybe broken?
		}
	}

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
		return 1;
	}

	mp_add_r(ls, z_re, 1, 1);	// x+1
	mp_mul(ls, ls, ls);			// (x+1)²
	mp_add(ls, ls, z_im_sq);	// (x+1)² + y²

	if (mp_cmp_r(ls, 1, 16) < 0)
	{
		return 2;
	}

	mp_clear(q); //TODO mp_clears
	mp_clear(xmq);
	mp_clear(ls);
	mp_clear(rs);

	return 0;
}
