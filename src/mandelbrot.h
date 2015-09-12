#ifndef _MANDELBROT_
#define _MANDELBROT_

#include <stdint.h>
#include <gmp.h>
#include "mp.h"

struct fractal_params
{
	mp_t min_re;
	mp_t min_im;
	mp_t max_re;
	mp_t max_im;
};

struct render_params
{
	uint32_t iteration_count;
	
	int32_t width, height;
	
	int32_t xOff, xSkip, xMax;
	int32_t yOff, ySkip, yMax;
	
	uint32_t orbit_start; 
	uint32_t orbit_length;
	uint32_t skip_primary_bulbs;
};

int calc_mandelbrot (struct fractal_params * fractal, struct render_params * render);

int 		ProcessRow	(struct fractal_params * fractal, struct render_params * render, int32_t y);
uint32_t 	ProcessPoint(struct fractal_params * fractal, struct render_params * render, mp_t im0, int32_t x, int32_t y);

int InBulb(mp_t z_re, mp_t z_im);

#endif
