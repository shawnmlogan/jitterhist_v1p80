#include "jitterhist.h"


double complex_abs(complex *z)

{
    return( sqrt(z->r * z->r + z->i * z->i) );
}


void complex_div(complex *quotient, complex *numerator, complex *denominator)
{
    double ratio, divisor;
    double real, imaginary;

    if( (real = denominator->r) < 0.0)
	real = -real;

    if( (imaginary = denominator->i) < 0.0)
	imaginary = -imaginary;

    if( real <= imaginary )
    {
	if(imaginary == 0)
	{
	    ratio = (double) (1.0 / denominator->r);

	    quotient->r = (double) (numerator->r * ratio);
	    quotient->i = (double) (numerator->i * ratio);
	}
	else
	{
	    ratio = (double) (denominator->r / denominator->i);

	    divisor = (double) denominator->i * (1.0 + ratio * ratio);

	    quotient->r = (double) ((numerator->r * ratio
				   + numerator->i) / divisor);
	    quotient->i = (double) ((numerator->i * ratio
				   - numerator->r) / divisor);
	}
    }
    else
    {
	if(real == 0)
	{
	    ratio = (double) (1.0 / denominator->i);

	    quotient->r = (double) (numerator->i * ratio);
	    quotient->i = (double) (-numerator->r * ratio);
	}
	else
	{
	    ratio = (double) (denominator->i / denominator->r);

	    divisor = (double) denominator->r * (1.0 + ratio * ratio);

	    quotient->r = (double) ((numerator->r
				   + numerator->i * ratio) / divisor);
	    quotient->i = (double) ((numerator->i
				   - numerator->r * ratio) / divisor);
	}
    }
}


void complex_exp(complex *r, complex *z)
{
    double expx;

    expx = exp((double) z->r);

    r->r = (double) expx * cos((double) z->i);
    r->i = (double) expx * sin((double) z->i);
}


void pow_ci(complex *result, complex *a, long *b)
{
    doublecomplex p1, a1;

    a1.r = (double) a->r;
    a1.i = (double) a->i;

    pow_zi(&p1, &a1, b);

    result->r = (double) p1.r;
    result->i = (double) p1.i;
}


long pow_ii(long *ap, long *bp)
{
    long pow, x, n;

    pow = 1;
    x = *ap;
    n = *bp;

    if(n > 0)
    {
	for( ; ; )
	{
	    if(n & 01)
		pow *= x;

	    if(n >>= 1)
		x *= x;
	    else
		break;
	}
    }

    return(pow);
}


double pow_ri(double *ap, long *bp)

{
    double pow, x;
    long n;

    pow = 1;
    x = *ap;
    n = *bp;

    if(n != 0)
    {
	if(n < 0)
	{
	    if(x == 0)
	    {
		return(pow);
	    }
	    n = -n;
	    x = 1/x;
	}

	for( ; ; )
	{
	    if(n & 01)
		pow *= x;

	    if(n >>= 1)
		x *= x;
	    else
		break;
	}
    }

    return(pow);
}


void pow_zi(doublecomplex *p, doublecomplex *a, long *b) 	/* p = a**b  */

{
    long n;
    double t;
    doublecomplex x;
    static doublecomplex one = {1.0, 0.0};

    n = *b;
    p->r = 1;
    p->i = 0;

    if(n == 0)
	return;

    if(n < 0)
    {
	n = -n;
	z_div(&x, &one, a);
    }
    else
    {
	x.r = a->r;
	x.i = a->i;
    }

    for( ; ; )
    {
	if(n & 01)
	{
	    t = p->r * x.r - p->i * x.i;
	    p->i = p->r * x.i + p->i * x.r;
	    p->r = t;
	}

	if(n >>= 1)
	{
	    t = x.r * x.r - x.i * x.i;
	    x.i = 2 * x.r * x.i;
	    x.r = t;
	}
	else
	{
	    break;
	}
    }
}

void r_cnjg(complex *r, complex *z)

{
    r->r = z->r;
    r->i = -z->i;
}


double r_mod(double *x, double *y)
{
    double quotient;

    if( (quotient = (double) (*x / *y)) >= 0)
    {
	quotient = floor(quotient);
    }
    else
    {
	quotient = floor(-quotient);
    }

    return((double) *x - (double) *y * quotient );
}


double r_sign(double *a, double *b)

{
    double x;

    x = (double) (*a >= 0 ? *a : - *a);

    return((double) ( *b >= 0 ? x : -x) );
}


void z_div(doublecomplex *quotient, doublecomplex *numerator, doublecomplex *denominator)

{
    double ratio, divisor;
    double real, imaginary;

    if( (real = denominator->r) < 0.0)
	real = -real;

    if( (imaginary = denominator->i) < 0.0)
	imaginary = -imaginary;

    if( real <= imaginary )
    {
	if(imaginary == 0)
	{
	    ratio = (double) (1.0 / denominator->r);

	    quotient->r = (double) (numerator->r * ratio);
	    quotient->i = (double) (numerator->i * ratio);
	}
	else
	{
	    ratio = (double) (denominator->r / denominator->i);

	    divisor = (double) denominator->i * (1.0 + ratio * ratio);

	    quotient->r = (double) ((numerator->r * ratio
				   + numerator->i) / divisor);
	    quotient->i = (double) ((numerator->i * ratio
				   - numerator->r) / divisor);
	}
    }
    else
    {
	if(real == 0)
	{
	    ratio = (double) (1.0 / denominator->i);

	    quotient->r = (double) (numerator->i * ratio);
	    quotient->i = (double) (-numerator->r * ratio);
	}
	else
	{
	    ratio = (double) (denominator->i / denominator->r);

	    divisor = (double) denominator->r * (1.0 + ratio * ratio);

	    quotient->r = (double) ((numerator->r
				   + numerator->i * ratio) / divisor);
	    quotient->i = (double) ((numerator->i
				   - numerator->r * ratio) / divisor);
	}
    }
}
