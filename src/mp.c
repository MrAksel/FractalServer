#include "mp.h"

#ifdef MP_RATIONALS
void mpq_init_set(mpq_t rop, mpq_t op)
{
	mpq_init(rop);
	mpz_set(mpq_numref(rop), mpq_numref(op)); //Set numerator
	mpz_set(mpq_denref(rop), mpq_denref(op)); //Set denominator
}

void mpq_init_set_r(mpq_t rop, signed long int num2, signed long int den2)
{
	mpq_init(rop);
	mpz_set_si(mpq_numref(rop), num2); //Set numerator
	mpz_set_si(mpq_denref(rop), den2); //Set denominator
}

void mpq_add_si(mpq_t rop, mpf_t op1, signed long int op2)
{
	mpq_t tmp;
	mpq_init_set_si(tmp, op2);
	mpq_add(rop, op1, tmp);
	mpq_clear(tmp);
}

void mpq_add_r(mpq_t rop, mpq_t op1, signed long int num2, signed long int den2)
{
	mpq_mul_si(rop, op1, den2);
	mpq_add_si(rop, rop, num2);
	mpq_div_si(rop, rop, den2);
}

void mpq_sub_si(mpq_t rop, mpf_t op1, signed long int op2)
{
	mpq_t tmp;
	mpq_init_set_si(tmp, op2);
	mpq_sub(rop, op1, tmp);
	mpq_clear(tmp);
}

void mpq_sub_r(mpq_t rop, mpq_t op1, signed long int num2, signed long int den2)
{
	mpq_mul_si(rop, op1, den2);
	mpq_mul_si(rop, rop, num2);
	mpq_div_si(rop, rop, den2);
}

void mpq_mul_si(mpq_t rop, mpf_t op1, signed long int op2)
{
	mpq_t tmp;
	mpq_init_set_si(tmp, op2);
	mpq_mul(rop, op1, tmp);
	mpq_clear(tmp);
}

void mpq_mul_r(mpq_t rop, mpq_t op1, signed long int num2, signed long int den2)
{
	mpq_mul_si(rop, op1, num2);
	mpq_div_si(rop, rop, den2);
}

void mpq_div_si(mpq_t rop, mpf_t op1, signed long int op2)
{
	mpq_t tmp;
	mpq_init_set_si(tmp, op2);
	mpq_div(rop, op1, tmp);
	mpq_clear(tmp);
}


#else

void mpf_init_set_r(mpf_t rop, signed long int num2, signed long int den2)
{
	mpf_init_set_si(rop, num2);
	mpf_div_si(rop, rop, den2);
}

void mpf_add_si(mpf_t rop, mpf_t op1, signed long int op2)
{
	mpf_t tmp;
	mpf_init_set_si(tmp, op2);
	mpf_add(rop, op1, tmp);
	mpf_clear(tmp);
}

void mpf_add_r(mpf_t rop, mpf_t op1, signed long int num2, signed long int den2)
{
	mpf_mul_si(rop, op1, den2);
	mpf_add_si(rop, rop, num2);
	mpf_div_si(rop, rop, den2);
}

void mpf_sub_si(mpf_t rop, mpf_t op1, signed long int op2)
{
	mpf_t tmp;
	mpf_init_set_si(tmp, op2);
	mpf_sub(rop, op1, tmp);
	mpf_clear(tmp);
}

void mpf_sub_r(mpf_t rop, mpf_t op1, signed long int num2, signed long int den2)
{
	mpf_mul_si(rop, op1, den2);
	mpf_add_si(rop, rop, num2);
	mpf_div_si(rop, rop, den2);
}

void mpf_mul_si(mpf_t rop, mpf_t op1, signed long int op2)
{
	mpf_t tmp;
	mpf_init_set_si(tmp, op2);
	mpf_mul(rop, op1, tmp);
	mpf_clear(tmp);
}

void mpf_mul_r(mpf_t rop, mpf_t op1, signed long int num2, signed long int den2)
{
	mpf_mul_si(rop, op1, num2);
	mpf_div_si(rop, rop, den2);
}

void mpf_div_si(mpf_t rop, mpf_t op1, signed long int op2)
{
	mpf_t tmp;
	mpf_init_set_si(tmp, op2);
	mpf_div(rop, op1, tmp);
	mpf_clear(tmp);
}

#endif

void mp_map(mp_t rop, int v, int s_min, int s_max, mp_t t_min, mp_t t_max)
{
	//rop = (v - s_min) * (t_max - t_min) / (s_max - s_min) + t_min
	
	mp_t tmp;
	mp_init(tmp);
	
	mp_sub(tmp, t_max, t_min);	
	
	mp_set_si(rop, (signed long int)v - s_min);
	mp_mul(rop, rop, tmp); // (v - s_min) * (t_max - t_min)
	
	mp_set_si(tmp, (signed long int)s_max - s_min);
	mp_div(rop, rop, tmp); // (v - s_min) * (t_max - t_min) / (s_max - s_min)
	
	mp_add(rop, rop, t_min); // + t_min
	
	mp_clear(tmp);
}
