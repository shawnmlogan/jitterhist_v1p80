#include "jitterhist.h"

/* SPPOWR     O2/20/87 */
/* COMPUTES RAW PERIODOGRAM, AVERAGED OVER SEGMENTS OF X(0:LX). */
/* X(0),X(1),---,X(LX)=INPUT DATA SEQUENCE. */
/* Y(0),Y(1),---,Y(LY)=OUTPUT PERIODOGRAM.  LY MUST BE A POWER OF 2. */
/* WORK=WORK ARRAY DIMENSIONED AT LEAST WORK(0:2*LY+1). */
/* LX=LAST INDEX IN DATA SEQUENCE AS ABOVE. */
/* LY=FREQUENCY INDEX CORRESPONDING TO HALF SAMPLING RATE.  POWER OF 2. 
*/
/* SEGMENT LENGTH IS 2*LY.  DATA LENGTH (LX+1) MUST BE AT LEAST THIS BIG. */
/* IWINDO=DATA WINDOW TYPE, 1(RECTANGULAR), 2(TAPERED RECTANGULAR), */
/*   3(TRIANGULAR), 4(HANNING), 5(HAMMING), OR 6(BLACKMAN). SEE CH. 14. 
*/
/* OVRLAP=FRACTION THAT EACH DATA SEGMENT OF SIZE 2*LY OVERLAPS ITS */
/*   PREDECESSOR.  MUST BE GREATER THAN OR EQUAL 0 AND LESS THAN 1. */
/* NSGMTS=NO. OVERLAPPING SEGMENTS OF X AVERAGED TOGETHER.  OUTPUT. */
/* IERROR=0  NO ERROR DETECTED. */
/*        1  IWINDO OUT OF RANGE (1-6). */
/*        2  LX TOO SMALL, I.E., LESS THAN 2*LY-1. */
/*        3  LY NOT A POWER OF 2  */

void sppowr(double *x, double *y, double *work, long int *lx, long int *ly, long int *iwindo, double *ovrlap, long int *nsgmts, long int *error)
{

    long int m, nsamp, tmp_int, isegmt, nshift;
    double base, tsv, *temp_pointer;
    complex *pwork;

    if ((*lx + 1) < (*ly * 2))
    {
        *error = 2;
	return;
    }

    base = (double) (*ly);
    base /= 2.0;

    while ((base - 2.0) > 0.0)
    {
	base /= 2.0;
    }

    if ((base - 2.0) == 0)
    {
	for (m = 0 ; m <= *ly ; ++m)
		{
	   y[m] = 0.0;
		}
	nshift = MIN(MAX((long) ((*ly * 2) * (1.0 - *ovrlap) + 0.5), 1),
	*ly * 2);

	*nsgmts = 1 + (*lx + 1 - (*ly * 2)) / nshift;

	for (isegmt = 0 ; isegmt < *nsgmts ; ++isegmt)
	{
	    for (nsamp = 0 ; nsamp < (*ly * 2) ; ++nsamp)
	    {
		work[nsamp] = x[nshift * isegmt + nsamp];
	    }

	    tmp_int = (*ly * 2) - 1;
	    spmask(work, &tmp_int, iwindo, &tsv, error);

	    if (*error != 0)
	    {
		return;
	    }
	    
		/* Convert doubles array work to complex array pwork for compatiblity */
		/* with argument of spfftc */
		
	    tmp_int = *ly * 2;
	    if ((pwork = calloc(tmp_int,sizeof(complex))) == NULL)
	    	{
	    	printf("Error allocating memory to pwork in sppowr(), exiting...\n");
	    	exit(0);
	    	}
	    temp_pointer = work; /*Save pointer*/
	    for (m = 0 ; m < tmp_int; m++)
	    	{
	    	pwork[m].r = *work++;
	    	pwork[m].i = 0.0;
	    	}
	    work = temp_pointer; /*Restore pointer*/
	    tmp_int = *ly * 2;
	    spfftc(pwork, &tmp_int, &neg_i1);

	    for (m = 0 ; m < *ly ; ++m)
			y[m] += ((pow(pwork[m].r,2.0) + pow(pwork[m].i,2.0))) / (tsv * *nsgmts);
	free(pwork);
	}
    }
    else if ((base - 2.0) < 0.0)
    {
	*error = 3;
    }
    return;
} /* sppowr */

/* SPMASK     05/19/86 */
/* THIS ROUTINE APPLIES A DATA WINDOW TO THE DATA VECTOR X(0:LX). */
/* ITYPE=1(RECTANGULAR), 2(TAPERED RECTANGULAR), 3(TRIANGULAR), */
/*       4(HANNING), 5(HAMMING), OR 6(BLACKMAN). */
/*       (NOTE:  TAPERED RECTANGULAR HAS COSINE-TAPERED 10% ENDS.) */
/* TSV=SUM OF SQUARED WINDOW VALUES. */
/* IERROR=0 IF NO ERROR, 1 IF ITYPE OUT OF RANGE. */

void spmask(double *x, long int *lx, long int *type, double *tsv, long int *error)
{

    long int k, tmp_int;
    double w;

    if (*type < 1 || *type > 7)
    {
	*error = 1;
	return;
    }

    *tsv = 0.0;
    for (k = 0 ; k <= *lx ; ++k)
    {
	tmp_int = 1 + *lx;
	w = spwndo(type, &tmp_int, &k);
	x[k] *= (double) w;
	*tsv += (double) (w * w);
    }

    *error = 0;
    return;
} /* spmask */

/* SPWNDO     11/13/85 */
/* THIS FUNCTION GENERATES A SINGLE SAMPLE OF A DATA WINDOW. */
/* ITYPE=1(RECTANGULAR), 2(TAPERED RECTANGULAR), 3(TRIANGULAR), */
/*       4(HANNING), 5(HAMMING), OR 6(BLACKMAN). */
/*       (NOTE:  TAPERED RECTANGULAR HAS COSINE-TAPERED 10% ENDS.) */
/* N=SIZE (TOTAL NO. SAMPLES) OF WINDOW. */
/* K=SAMPLE NUMBER WITHIN WINDOW, FROM 0 THROUGH N-1. */
/*   (IF K IS OUTSIDE THIS RANGE, WINDOW IS SET TO 0.) */

double spwndo(long int *type, long int *n, long int *k)
{
    /* Local variables */
    long int l;
    double ret_val, factor1;

    if ((*type < 1 || *type > 7) || (*k < 0 || *k >= *n))
    {
	ret_val = 0.0;
	return(ret_val);
    }

    ret_val = 1.0;

    switch (*type)
    {

	case 1:

	    break;

	case 2:

	    l = (*n - 2) / 10;

	    if (*k <= l)
	    {
		ret_val = 0.5 * (1.0 - cos((double) *k * M_PI
					   / ((double) l + 1.0)));
	    }

	    if (*k > (*n - l - 2))
	    {
		ret_val = 0.5 * (1.0 - cos((double) (*n - *k - 1) * M_PI
					   / ((double) l + 1.0)));
	    }

	    break;

	case 3:

	    ret_val = 1.0 - ABS(1.0 - (double) (*k * 2) / ((double) *n - 1.0));

	    break;

	case 4:

	    ret_val = 0.5 * (1.0 - cos((double) (*k * 2) * M_PI
				       / ((double) *n - 1.0)));

	    break;

	case 5:

	    ret_val = 0.54 - 0.46 * cos((double) (*k * 2) * M_PI
					/ ((double) *n - 1.0));

	    break;

	case 6:

	    ret_val = 0.42 - 0.5 * cos((double) (*k * 2) * M_PI
				       / ((double) *n - 1.0))
		      + 0.08 * cos((double) (*k * 4) * M_PI
				   / ((double) *n - 1.0));

	    break;
	    
/*Add Blackman/Harris*/

	case 7:
	
        factor1 =  M_PI/((double) *n);
        factor1 = factor1 * ((double) *k - (((double) *n - 1.0)/2.0));
 
	    ret_val = 0.35875 + 0.48829 * cos(2.0 * factor1) +
	    0.14128 * cos(4.0 * factor1) + 0.01168 * cos(6.0 * factor1);
	    
	    break;

    }

    return(ret_val);
} /* spwndo */