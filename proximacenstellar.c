/***************** PROXIMACENSTELLAR.C *********************** 
 *
 * Rodrigo Luger, Summer 2016
 *
 *
 *
*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "vplanet.h"

int fiGetLowerBoundProximaCen(double val, const double *arr, int dim){
	int i;
	for (i=0;i<dim-2;i++){
	  if (val < arr[i+1]) break;
  }
	return i;
}

double fdProximaCenLinear(int iALEN, double const data[iALEN], int xi, double dx) {
	// Linearly interpolate over data, given index of lower bound on grid xi
	// and normalized distance to the interpolation point dx.
	if (dx == 0)
	  return data[xi];
  else
	  return data[xi] * (1 - dx) + data[xi + 1] * dx;
}

double fdProximaCenInterpolate(int iALEN, double const xarr[iALEN], double const data[iALEN], double A, int *iError){
	double dx;
	int xi;
	int dxi;
  double result = 0;

	// Let's enforce a minimum age of 0.001 GYR and a maximum age of 10.0 GYR
	// NOTE: This results in a constant luminosity at times beyond this range.
	if (A < 0.001) A = 0.001;
	if (A > 10.00) A = 10.00;
	
	// Get the lower bound
	xi = fiGetLowerBoundProximaCen(log10(A),xarr,iALEN);

	// Normalized distance to grid points
	dx = (log10(A)-xarr[xi]) / (xarr[xi + 1] - xarr[xi]);
    
  // Calculate
  result = fdProximaCenLinear(iALEN,data,xi,dx);
  if (isnan(result)) {
    *iError = PROXIMACEN_ERROR;
    return 0;
  }
  return result;
}

double fdProximaCenStellar(int iParam, double A, int *iError) {
	// Returns T, L, or R by interpolating over the grid
	double res;

  if (iParam == PROXIMACEN_T) {			
      res = fdProximaCenInterpolate(PROXIMACEN_ALEN, PROXIMACEN_AARR, PROXIMACEN_LOGT, A / (1.e9 * YEARSEC), iError);
      return pow(10., res);
  } else if (iParam == PROXIMACEN_L) {
      res = fdProximaCenInterpolate(PROXIMACEN_ALEN, PROXIMACEN_AARR, PROXIMACEN_LOGL, A / (1.e9 * YEARSEC), iError);
      return LSUN * pow(10., res);
  } else if (iParam == PROXIMACEN_R) {	
      res = fdProximaCenInterpolate(PROXIMACEN_ALEN, PROXIMACEN_AARR, PROXIMACEN_LOGR, A / (1.e9 * YEARSEC), iError);
      return RSUN * pow(10., res);
  } else {
      *iError = PROXIMACEN_ERROR;
      return 0;
  }
}