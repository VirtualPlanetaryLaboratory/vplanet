/***************** CUBIC.H *********************** 
 *
 * Rodrigo Luger, Summer 2014
 *
 * Taken from Numerical Recipes in C, routines
 * spline and splint.
 * 
 * fdInterpSpline() does a cubic spline interpolation
 * on arrays xa[] and ya[]. fvInitSpline() must be
 * called first, but only once for a given grid of xa[]
 * and ya[]. The array y2[] returned by fvInitSpline()
 * is the array of second derivatives passed as an
 * argument to fdInterpSpline. N is the length of the
 * arrays, x is the interpolation point. Yp1 and ypn
 * are the derivatives at the first and last grid
 * points. Set to >= 1e30 to compute these from the 
 * data.
 *
 * TO COMPILE (MAC):
 *
 * >>> gcc -shared -Wl,-install_name,cubic -o cubic.so -fPIC cubic.c
 *
*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include <string.h>

#define NR_END 				1
#define FREE_ARG			char*

double *vector(long nl, long nh) {
	// Allocate a double vector with subscript range v[nl...nh]
	double *v;
	v = (double *)malloc((size_t) ((nh-nl+1+NR_END)*sizeof(double)));
	if (!v) {
		printf("ERROR in vector() allocation.\n");
		exit(0);
	}
	return v-nl+NR_END;
}

void free_vector(double *v, long nl, long nh) {
	free((FREE_ARG) (v+nl-NR_END));
}

void fvInitSpline(double x[], double y[], int n, double yp1, double ypn, double y2[]) {
	int i,k;
	double p,qn,sig,un,*u;

	u=vector(0,n-2);
	if (yp1 > 0.99e30)
					y2[0]=u[0]=0.0;
	else {
					y2[0] = -0.5;
					u[0]=(3.0/(x[1]-x[0]))*((y[1]-y[0])/(x[1]-x[0])-yp1);
	}
	for (i=1;i<=n-2;i++) {
					sig=(x[i]-x[i-1])/(x[i+1]-x[i-1]);
					p=sig*y2[i-1]+2.0;
					y2[i]=(sig-1.0)/p;
					u[i]=(y[i+1]-y[i])/(x[i+1]-x[i]) - (y[i]-y[i-1])/(x[i]-x[i-1]);
					u[i]=(6.0*u[i]/(x[i+1]-x[i-1])-sig*u[i-1])/p;
	}
	if (ypn > 0.99e30)
					qn=un=0.0;
	else {
					qn=0.5;
					un=(3.0/(x[n-1]-x[n-2]))*(ypn-(y[n-1]-y[n-2])/(x[n-1]-x[n-2]));
	}
	y2[n-1]=(un-qn*u[n-2])/(qn*y2[n-2]+1.0);
	for (k=n-2;k>=0;k--)
		y2[k]=y2[k]*y2[k+1]+u[k];
	free_vector(u,0,n-2);
}

double fdInterpSpline(double xa[], double ya[], double y2a[], int n, double x, int *iError) {
		int klo,khi,k;
		double h,b,a;
		double result;

		klo=0;
		khi=n-1;
		while (khi-klo > 1) {
			k=(khi+klo) >> 1;
			if (xa[k] > x) khi=k;
			else klo=k;
		}
		h=xa[khi]-xa[klo];
		if (h == 0.0) {
			*iError = 1; // Bad xa input
			return 0;
		}
		a=(xa[khi]-x)/h;
		b=(x-xa[klo])/h;
		result=a*ya[klo]+b*ya[khi]+((a*a*a-a)*y2a[klo]+(b*b*b-b)*y2a[khi])*(h*h)/6.0;
		
		*iError = 0;
		return result;
}