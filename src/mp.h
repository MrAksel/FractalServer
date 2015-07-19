#include <gmp.h>

#ifndef MP_RATIONALS
#ifndef MP_FLOATS
	#define MP_FLOATS
#endif
#endif

#ifdef MP_RATIONALS
	#define mp_t mpq_t
	
	#define mp_init(x)				mpq_init(x)
	#define mp_init_set(x, y)		mpq_init_set(x, y)
	#define mp_init_set_si(x, y)	mpq_init_set_r(x, y, 1)
	#define mp_init_set_r(x, y, z)	mpq_init_set_r(x, y, z)
	
	#define mp_set_si(x, y)			mpq_set_si(x, y, 1)
	
	#define mp_add(x, y, z)			mpq_add(x, y, z)
	#define mp_add_si(x, y, z)		mpq_add_si(x, y, z)
	#define mp_add_r(x, y, z, w)	mpq_add_r(x, y, z, w)
	#define mp_sub(x, y, z)			mpq_sub(x, y, z)
	#define mp_sub_si(x, y, z)		mpq_sub_si(x, y, z)
	#define mp_sub_r(x, y, z, w)	mpq_sub_r(x, y, z, w)
	#define mp_mul(x, y, z)			mpq_mul(x, y, z)
	#define mp_mul_si(x, y, z)		mpq_mul_si(x, y, z)
	#define mp_mul_r(x, y, z, w)	mpq_mul_r(x, y, z, w)
	#define mp_div(x, y, z)			mpq_div(x, y, z)
	#define mp_div_si(x, y, z)		mpq_div_si(x, y, z)
	#define mp_div_r(x, y, z, w)	mpq_mul_r(x, y, w, z)
	
	#define mp_cmp(x, y)			mpq_cmp(x, y)
	#define mp_cmp_si(x, y)			mpq_cmp_si(x, y, 1)
	#define mp_cmp_r(x, y, z)		mpq_cmp_si(x, y, z)
	#define mp_swap(x, y)			mpq_swap(x, y)
	
	#define mp_clear(x)				mpq_clear(x);
#else
	#define mp_t mpf_t
	
	#define mp_init(x)				mpf_init(x)
	#define mp_init_set(x, y)		mpf_init_set(x, y)
	#define mp_init_set_si(x, y)	mpf_init_set_si(x, y)
	#define mp_init_set_r(x, y, z)	mpf_init_set_r(x, y, z)
	
	#define mp_set_si(x, y)			mpf_set_si(x, y)
	
	#define mp_add(x, y, z)			mpf_add(x, y, z)
	#define mp_add_r(x, y, z, w)	mpf_add_r(x, y, z, w)
	#define mp_sub(x, y, z)			mpf_sub(x, y, z)
	#define mp_sub_r(x, y, z, w)	mpf_sub_r(x, y, z, w)
	#define mp_mul(x, y, z)			mpf_mul(x, y, z)
	#define mp_mul_r(x, y, z, w)	mpf_mul_r(x, y, z, w)
	#define mp_div(x, y, z)			mpf_div(x, y, z)
	#define mp_div_r(x, y, z, w)	mpf_mul_r(x, y, w, z)
	
	#define mp_cmp(x, y)			mpf_cmp(x, y)
	#define mp_cmp_si(x, y)			mpf_cmp_si(x, y)
	#define mp_cmp_r(x, y, z)		mpf_cmp_d(x, (double)y / z)
	#define mp_swap(x, y)			mpf_swap(x, y)
	
	#define mp_clear(x)				mpf_clear(x)
#endif

void mpq_init_set(mpq_t rop, mpq_t op);
void mpq_init_set_r(mpq_t rop, signed long int num2, signed long int den2);
void mpq_add_si(mpq_t rop, mpq_t op1, signed long int op2);
void mpq_add_r(mpq_t rop, mpq_t op1, signed long int num2, signed long int den2);
void mpq_sub_si(mpq_t rop, mpq_t op1, signed long int op2);
void mpq_sub_r(mpq_t rop, mpq_t op1, signed long int num2, signed long int den2);
void mpq_mul_si(mpq_t rop, mpq_t op1, signed long int op2);
void mpq_mul_r(mpq_t rop, mpq_t op1, signed long int num2, signed long int den2);
void mpq_div_si(mpq_t rop, mpq_t op1, signed long int op2);
void mpq_div_r(mpq_t rop, mpq_t op1, signed long int num2, signed long int den2);

void mpf_init_set_r(mpf_t rop, signed long int num2, signed long int den2);
void mpf_add_si(mpf_t rop, mpf_t op1, signed long int op2);
void mpf_add_r(mpf_t rop, mpf_t op1, signed long int num2, signed long int den2);
void mpf_sub_si(mpf_t rop, mpf_t op1, signed long int op2);
void mpf_sub_r(mpf_t rop, mpf_t op1, signed long int num2, signed long int den2);
void mpf_mul_si(mpf_t rop, mpf_t op1, signed long int op2);
void mpf_mul_r(mpf_t rop, mpf_t op1, signed long int num2, signed long int den2);
void mpf_div_si(mpf_t rop, mpf_t op1, signed long int op2);
void mpf_div_r(mpf_t rop, mpf_t op1, signed long int num2, signed long int den2);

void mp_map(mp_t rop, int v, int s_min, int s_max, mp_t t_min, mp_t t_max);


