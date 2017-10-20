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

double fdProximaCenBiLinear(int iALEN, double const data_lo[iALEN], double const data_hi[iALEN], int xi, int yi, double dx, double dy) {
	// Linearly interpolate over data, given indices of lower bounds on grid xi, yi
	// and normalized distances to the interpolation point dx, dy.
	double C0, C1, C;	
	if (dx == 0) {
	  C0 = data_lo[xi];
	  C1 = data_hi[xi];
	} else {
	  C0 = data_lo[xi]*(1-dx) + data_lo[xi+1]*dx;
	  C1 = data_hi[xi]*(1-dx) + data_hi[xi+1]*dx;	
	}
	if (dy == 0)
	  C = C0;
	else
	  C = C0*(1-dy) + C1*dy;
	return C;
}

double fdProximaCenInterpolate(int iALEN, int iMLEN, double const xarr[iALEN], double const yarr[iMLEN], double const data_lo[iALEN], double const data_hi[iALEN], double A, double M, int *iError){
  double dx,dy;
  int xi,yi;
  int dxi,dyi;
  double result = 0;

  // Let's enforce a minimum age of 0.001 GYR and a maximum age of 10.0 GYR
  // NOTE: This results in a constant luminosity at times beyond this range.
  if (A < 0.001) A = 0.001;
  if (A > 10.00) A = 10.00;
  
  // Bounds on mass
  if (M < 0.1) {
		*iError = STELLAR_ERR_OUTOFBOUNDS_LO;
		return 0;
	} else if (M > 0.15) {
		*iError = STELLAR_ERR_OUTOFBOUNDS_HI;
		return 0;
	}
  
  // Get the lower bound
  xi = fiGetLowerBoundProximaCen(log10(A),xarr,iALEN);
  yi = fiGetLowerBoundProximaCen(M,yarr,iMLEN);
  
  // Paranoia check (REMOVE)
  if (yi < 0) {
    *iError = STELLAR_ERR_OUTOFBOUNDS_LO;
    return 0;
  } else if (yi > 1) {
    *iError = STELLAR_ERR_OUTOFBOUNDS_HI;
    return 0;
  }
  
  // Normalized distance to grid points
  dx = (log10(A)-xarr[xi]) / (xarr[xi + 1] - xarr[xi]);
  dy = (M-yarr[yi]) / (yarr[yi + 1] - yarr[yi]);
  
  // Calculate
  result = fdProximaCenBiLinear(iALEN,data_lo,data_hi,xi,yi,dx,dy);
  if (isnan(result)) {
    *iError = PROXIMACEN_ERROR;
    return 0;
  }
  *iError = 0;
  return result;
}

double fdProximaCenStellar(int iParam, double A, double M, int *iError) {
	// Returns T, L, or R by interpolating over the grid
	double res;
	double dLum, dRad;

  if (iParam == PROXIMACEN_T) {			
      // Get fudged luminosity
      dLum = fdProximaCenInterpolate(PROXIMACEN_ALEN, PROXIMACEN_MLEN, PROXIMACEN_AARR, PROXIMACEN_MARR, PROXIMACEN_LOGL_LO, PROXIMACEN_LOGL_HI, A / (1.e9 * YEARSEC), M / MSUN, iError);
      dLum = LSUN * pow(10., dLum * PROXIMACEN_FUDGE);
      // Get radius
      dRad = fdProximaCenInterpolate(PROXIMACEN_ALEN, PROXIMACEN_MLEN, PROXIMACEN_AARR, PROXIMACEN_MARR, PROXIMACEN_LOGR_LO, PROXIMACEN_LOGR_HI, A / (1.e9 * YEARSEC), M / MSUN, iError);
      dRad = RSUN * pow(10., dRad);
      // Compute self-consistent temperature
      res = pow(dLum / (4 * PI * dRad * dRad * SIGMA), 0.25);
      return res;
  } else if (iParam == PROXIMACEN_L) {
      res = fdProximaCenInterpolate(PROXIMACEN_ALEN, PROXIMACEN_MLEN, PROXIMACEN_AARR, PROXIMACEN_MARR, PROXIMACEN_LOGL_LO, PROXIMACEN_LOGL_HI, A / (1.e9 * YEARSEC), M / MSUN, iError);
      return LSUN * pow(10., res * PROXIMACEN_FUDGE);
  } else if (iParam == PROXIMACEN_R) {	
      res = fdProximaCenInterpolate(PROXIMACEN_ALEN, PROXIMACEN_MLEN, PROXIMACEN_AARR, PROXIMACEN_MARR, PROXIMACEN_LOGR_LO, PROXIMACEN_LOGR_HI, A / (1.e9 * YEARSEC), M / MSUN, iError);
      return RSUN * pow(10., res);
  } else {
      *iError = PROXIMACEN_ERROR;
      return 0;
  }
}