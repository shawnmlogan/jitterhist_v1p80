#include "jitterhist.h"

/* SPFFTC     02/20/87 */
/* FAST FOURIER TRANSFORM OF N=2**K COMPLEX DATA POINTS USING TIME */
/* DECOMPOSITION WITH INPUT BIT REVERSAL.  N MUST BE A POWER OF 2. */
/* X MUST BE SPECIFIED COMPLEX X(0:N-1)OR LARGER. */
/* INPUT IS N COMPLEX SAMPLES, X(0),X(1),...,X(N-1). */
/* COMPUTATION IS IN PLACE, OUTPUT REPLACES INPUT. */
/* ISIGN = -1 FOR FORWARD TRANSFORM, +1 FOR INVERSE. */
/* X(0) BECOMES THE ZERO TRANSFORM COMPONENT, X(1) THE FIRST, */
/* AND SO FORTH.  X(N-1) BECOMES THE LAST COMPONENT. */


void spfftc(complex *x, long *n, long *isign)

{
    /* Builtin functions */
    void complex_exp();

    /* Local variables */
    long i, l, m, mr,tmp_int;
    complex t, tmp_complex, tmp;
    double pisign;

    pisign = (double) ((double) *isign * M_PI);

    mr = 0;

    for (m = 1 ; m < *n ; ++m)
    {
	l = *n;
	l /= 2;

	while (mr + l >= *n)
	{
	    l /= 2;
	}

	mr = mr % l + l;

	if (mr > m)
	{
	    t.r = x[m].r;
	    t.i = x[m].i;
	    x[m].r = x[mr].r;
	    x[m].i = x[mr].i;
	    x[mr].r = t.r;
	    x[mr].i = t.i;
	}
    }

    l = 1;

    while (l < *n)
    {
	for (m = 0 ; m < l ; ++m)
	{

	    tmp_int = l * 2;

	    for (i = m ; tmp_int < 0 ? i >= (*n - 1) : i < *n ;
		 i += tmp_int)
	    {
		tmp.r = 0.0;
		tmp.i = (double) m * pisign / (double) l;

		complex_exp(&tmp_complex, &tmp);

		t.r = x[i + l].r * tmp_complex.r - x[i + l].i * tmp_complex.i;
		t.i = x[i + l].r * tmp_complex.i + x[i + l].i * tmp_complex.r;

		x[i + l].r = x[i].r - t.r;
		x[i + l].i = x[i].i - t.i;

		x[i].r = x[i].r + t.r;
		x[i].i = x[i].i + t.i;
	    }
        }
        l *= 2;
    }

    return;
} /* spfftc */