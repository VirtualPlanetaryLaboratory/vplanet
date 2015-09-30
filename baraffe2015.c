/***************** BARAFFE2015.C *********************** 
 *
 * Rodrigo Luger, Summer 2015
 *
 * This module contains all the routines used to interpolate
 * over the Baraffe+ (2015) stellar evolution grid using either a
 * bilinear or a bicubic interpolator.
 *
 *
*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "vplanet.h"

double fdDotProduct(const int *x, const double *y){
	double res = 0.0;
	int i;
	for (i = 0; i < 16; i++){
		res += x[i] * y[i];
	}
	return res;
}

void fvMatrixVectorMult(const int mat[16][16], const double *vec, double *result){ 
	// in matrix form: result = mat * vec;
	int i;
	for (i = 0; i < 16; i++){
		result[i] = fdDotProduct(mat[i], vec);
	}
}

int fiGetLowerBound(double val, const double *arr, int dim){
	int i;
	if ((val < arr[0]) || (val > arr[dim-1])){
			return STELLAR_ERR_WAY_OUTOFBOUNDS;
	} else {
			for (i=0;i<dim-2;i++){
				if (val < arr[i+1]) break;
			}
	}
	// Check to see if i-1, i, i+1 and i+2 are all valid indices
	if (i == 0)
		return STELLAR_ERR_OUTOFBOUNDS_LO;
	else if (i>=dim-2)
		return STELLAR_ERR_OUTOFBOUNDS_HI;
	else
		return i;
}

double fdBaraffeBiLinear(int iMLEN, int iALEN, double const data[iMLEN][iALEN], int xi, int yi, double dx, double dy) {
	// Linearly interpolate over data, given indices of lower bounds on grid xi, yi
	// and normalized distances to the interpolation point dx, dy.
	double C0, C1, C;	
	C0 = data[xi][yi]*(1-dx) + data[xi+1][yi]*dx;
	C1 = data[xi][yi+1]*(1-dx) + data[xi+1][yi+1]*dx;
	C = C0*(1-dy) + C1*dy;
	return C;
}

double fdBaraffeBiCubic(int iMLEN, int iALEN, double const data[iMLEN][iALEN], int xi, int yi, double dx, double dy) {
	double dvCoeff[16];
	int j,k;
	int ijkn = 0;
  double dypow = 1;
  double result = 0;
  
	// Linear algebra time!
	// Adapted from http://en.wikipedia.org/wiki/Bicubic_interpolation
	double dvDeriv[16] = {
										// values of the function at each corner
										data[xi][yi],
										data[xi+1][yi],
										data[xi][yi+1],
										data[xi+1][yi+1],
										
										// values of df/dx at each corner.
										0.5*(data[xi+1][yi]-data[xi-1][yi]),
										0.5*(data[xi+2][yi]-data[xi][yi]),
										0.5*(data[xi+1][yi+1]-data[xi-1][yi+1]),
										0.5*(data[xi+2][yi+1]-data[xi][yi+1]),
										
										// values of df/dy at each corner.
										0.5*(data[xi][yi+1]-data[xi][yi-1]),
										0.5*(data[xi+1][yi+1]-data[xi+1][yi-1]),
										0.5*(data[xi][yi+2]-data[xi][yi]),
										0.5*(data[xi+1][yi+2]-data[xi+1][yi]),
										
										// values of d2f/dxdy at each corner.
										0.25*(data[xi+1][yi+1]-data[xi-1][yi+1]-data[xi+1][yi-1]+data[xi-1][yi-1]),
										0.25*(data[xi+2][yi+1]-data[xi][yi+1]-data[xi+2][yi-1]+data[xi][yi-1]),
										0.25*(data[xi+1][yi+2]-data[xi-1][yi+2]-data[xi+1][yi]+data[xi-1][yi]),
										0.25*(data[xi+2][yi+2]-data[xi][yi+2]-data[xi+2][yi]+data[xi][yi])
										};
										
	fvMatrixVectorMult(STELLAR_BICUBIC_MATRIX,dvDeriv,dvCoeff);
	dypow = 1;
	for(j = 0; j < 4; ++j) {
		result += dypow*(dvCoeff[ijkn] + dx*(dvCoeff[ijkn+1] + dx*(dvCoeff[ijkn+2] + dx*dvCoeff[ijkn+3])));
		ijkn += 4;
		dypow *= dy;
  }
  return result;
}

double fdBaraffeInterpolate(int iMLEN, int iALEN, double const xarr[iMLEN], double const yarr[iALEN], double const data[iMLEN][iALEN], double M, double A, int iOrder, int *iError){
	double dx, dy;
	int xi,yi;
	int dxi, dyi;
  double result = 0;

	// Let's enforce a minimum age of 0.001 GYR
	// NOTE: This results in a constant luminosity at times earlier than this, which
	// is not realistic. Shouldn't be an issue for most planet evolution calculations,
	// since planets typically form after this time, but this issue needs to be
	// revisited eventually.
	if (A < 0.001) A = 0.001;
		
	// Get bounds on grid
	*iError = 0;
	xi = fiGetLowerBound(M,xarr,iMLEN);
	yi = fiGetLowerBound(A,yarr,iALEN);

	if (xi < 0) {
		*iError = xi;
		return 0;
	} else if (yi < 0) {
		*iError = yi;
		return 0;
	}

	// Normalized distance to grid points
	dx = (M-xarr[xi])/(xarr[xi+1]-xarr[xi]);
	dy = (A-yarr[yi])/(yarr[yi+1]-yarr[yi]);

	if (iOrder == 1) {
		result = fdBaraffeBiLinear(iMLEN,iALEN,data,xi,yi,dx,dy);
		if isnan(result) {
			*iError = STELLAR_ERR_ISNAN;
			return 0;
		}
		return result;
	} else if (iOrder == 3) {
		result = fdBaraffeBiCubic(iMLEN,iALEN,data,xi,yi,dx,dy);
		if isnan(result) {
			// Maybe we can still linearly interpolate. Let's check:
			for (dxi = 0; dxi<2; dxi++){
				for (dyi = 0; dyi<2; dyi++){
					if isnan(data[xi+dxi][yi+dyi]){
						// Hopeless; you're bounded by
						// a NaN on at least one side
						*iError = STELLAR_ERR_ISNAN;
						return 0;
					}
				}
			}
			// We're good! A linear interpolation will save the day.
			*iError = STELLAR_ERR_LINEAR;
			return fdBaraffeBiLinear(iMLEN,iALEN,data,xi,yi,dx,dy);
  	}
  	return result;
	} else {
		*iError = STELLAR_ERR_BADORDER;
		return 0;
	}
}

double fdBaraffe(int iParam, double A, double M, int iOrder, int *iError) {
	// Returns T, L, or R
	// by interpolating over the Baraffe grid
	// using either a bilinear (iOrder = 1) or
	// a bicubic (iOrder = 3) interpolation.
	double res;

  if (iParam == STELLAR_T) {			
      res = fdBaraffeInterpolate(STELLAR_BAR_MLEN, STELLAR_BAR_ALEN, STELLAR_BAR_MARR, STELLAR_BAR_AARR, DATA_LOGT, M / MSUN, A / (1.e9 * YEARSEC), iOrder, iError);
      return pow(10., res);
  } else if (iParam == STELLAR_L) {
      res = fdBaraffeInterpolate(STELLAR_BAR_MLEN, STELLAR_BAR_ALEN, STELLAR_BAR_MARR, STELLAR_BAR_AARR, DATA_LOGL, M / MSUN, A / (1.e9 * YEARSEC), iOrder, iError);
      return LSUN * pow(10., res);
  } else if (iParam == STELLAR_R) {	
      res = fdBaraffeInterpolate(STELLAR_BAR_MLEN, STELLAR_BAR_ALEN, STELLAR_BAR_MARR, STELLAR_BAR_AARR, DATA_RADIUS, M / MSUN, A / (1.e9 * YEARSEC), iOrder, iError);
      return RSUN * res;
  } else {
      *iError = STELLAR_ERR_FILE;
      return 0;
  }
}