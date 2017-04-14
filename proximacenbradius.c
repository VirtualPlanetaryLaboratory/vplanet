/***************** PROXIMACENBRADIUS.C *********************** 
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

int fiGetLowerBoundProximaCenB(double val, const double *arr, int dim){
	int i;
	for (i=0;i<dim-2;i++){
	  if (val < arr[i+1]) break;
  }
	return i;
}

double fdProximaCenBLinear(int xi, int yi, double dx, double dy) {
	// Linearly interpolate over data, given indices of lower bounds on grid xi, yi
	// and normalized distances to the interpolation point dx, dy.
	double C0, C1, C;	
	if (dx == 0) {
	  C0 = daProxCenBRadius[xi][yi];
	  C1 = daProxCenBRadius[xi][yi+1];
	} else {
	  C0 = daProxCenBRadius[xi][yi]*(1-dx) + daProxCenBRadius[xi+1][yi]*dx;
	  C1 = daProxCenBRadius[xi][yi+1]*(1-dx) + daProxCenBRadius[xi+1][yi+1]*dx;	
	}
	if (dy == 0)
	  C = C0;
	else
	  C = C0*(1-dy) + C1*dy;
	
	return C;
}

double fdProximaCenBRadius(double C, double A){
	double dx, dy;
	int xi, yi;

	// Let's enforce a minimum age of 0.001 GYR and a maximum age of 10.0 GYR
	A /= YEARSEC;
	if (A < 1e7) A = 1e7;
	if (A > 1e10) A = 1e10;
	
	// Let's enforce the bounds for the composition as well
	if (C < 0.00001) C = 0.00001;
	if (C > 0.01) C = 0.01;
	
	// Get the lower bounds
	xi = fiGetLowerBoundProximaCenB(C, daProxCenBComp, PROXCENBCOMPLEN);
	yi = fiGetLowerBoundProximaCenB(A, daProxCenBAge, PROXCENBTIMELEN);

	// Normalized distance to grid points
	dx = (C - daProxCenBComp[xi]) / (daProxCenBComp[xi + 1] - daProxCenBComp[xi]);
	dy = (A - daProxCenBAge[yi]) / (daProxCenBAge[yi + 1] - daProxCenBAge[yi]);
    
  // Calculate
  return fdProximaCenBLinear(xi,yi,dx,dy) * REARTH;
}