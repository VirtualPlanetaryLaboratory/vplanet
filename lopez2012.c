/***************** LOPEZ2012.C *********************** 
 *
 * Rodrigo Luger, Summer 2016
 *
 * This module contains all the routines used to interpolate
 * over the Lopez and Fortney (2012) planet radius evolution grid.
 *
 *
*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "vplanet.h"

double fdLopezRadius(double dMass, double dComp, double dFlux, double dAge, int iMetal){
	int m, c, f, t, z;
	double dm, dc, df, dt;
	double R000,R001,R010,R011,R100,R101,R110,R111;
	double R00,R10,R01,R11;
	double R0,R1;	
	double dMassEarth = dMass/MEARTH;
	double dAgeYears = dAge/YEARSEC;
	
	/* We're not going to bother interpolating between metallicities. */
	z = iMetal;
	
	// Add a small tolerance
	if ((dMassEarth/daLopezMass[0] < 1) && (dMassEarth/daLopezMass[0]) > 0.99) dMassEarth = daLopezMass[0];
	
	if ((dMassEarth/daLopezMass[0] < 1) || (dMassEarth >= daLopezMass[MASSLEN-1])){
		/* Out of bounds */
		return 0;
	} else {
		/* Get index just below desired mass */
		for (m = 0; m < MASSLEN-1; m++)
			if (dMassEarth < daLopezMass[m+1]) break;
	}
	if (dComp < daLopezComp[0]){
		/* Out of bounds, assuming it's OK to use min val */
		dComp = daLopezComp[0];
		c = 0;	
	} else if (dComp >= daLopezComp[COMPLEN-1]){
		/* Out of bounds */
		return 0;
	} else {
		/* Get index just below desired composition */
		for (c = 0; c < COMPLEN-1; c++)
			if (dComp < daLopezComp[c+1]) break;
	}
	if (dFlux < daLopezFlux[0]){
		/* Out of bounds, assuming it's OK to use min val */
		dFlux = daLopezFlux[0];
		f = 0;	
	} else if (dFlux >= daLopezFlux[FLUXLEN-1]){
		/* Out of bounds, assuming it's OK to use max val */
		dFlux = daLopezFlux[FLUXLEN-1];
		f = FLUXLEN - 1;
	} else {
		/* Get index just below desired composition */
		for (f = 0; f < FLUXLEN-1; f++)
			if (dFlux < daLopezFlux[f+1]) break;
	}
	if (dAgeYears < daLopezAge[0]){
		/* Out of bounds, assuming it's OK to use min val */
		dAgeYears = daLopezAge[0];
		t = 0;		
	} else if (dAgeYears >= daLopezAge[TIMELEN-1]){
		/* Out of bounds, assuming it's OK to use max val */
		dAgeYears = daLopezAge[TIMELEN-1];
		t = TIMELEN - 1;
	} else {
		/* Get index just below desired time */
		for (t = 0; t < TIMELEN-1; t++)
			if (dAgeYears < daLopezAge[t+1]) break;
	}
	/* We now have the coordinates below our desired point.
	 * Let's use them to do a simple tetralinear interpolation.
	 * Adapted from the method described in
	 * http://en.wikipedia.org/wiki/Trilinear_interpolation */
	dm = (dMassEarth - daLopezMass[m])/(daLopezMass[m+1] - daLopezMass[m]);
	dc = (dComp - daLopezComp[c])/(daLopezComp[c+1] - daLopezComp[c]);
	df = (dFlux - daLopezFlux[f])/(daLopezFlux[f+1] - daLopezFlux[f]);
	dt = (dAgeYears - daLopezAge[t])/(daLopezAge[t+1] - daLopezAge[t]);
	R000 = daLopezRadius[m][c][f][z][t]*(1-dm) + daLopezRadius[m+1][c][f][z][t]*dm;
	R001 = daLopezRadius[m][c][f][z][t+1]*(1-dm) + daLopezRadius[m+1][c][f][z][t+1]*dm;
	R010 = daLopezRadius[m][c][f+1][z][t]*(1-dm) + daLopezRadius[m+1][c][f+1][z][t]*dm;
	R011 = daLopezRadius[m][c][f+1][z][t+1]*(1-dm) + daLopezRadius[m+1][c][f+1][z][t+1]*dm;
	R100 = daLopezRadius[m][c+1][f][z][t]*(1-dm) + daLopezRadius[m+1][c+1][f][z][t]*dm;
	R101 = daLopezRadius[m][c+1][f][z][t+1]*(1-dm) + daLopezRadius[m+1][c+1][f][z][t+1]*dm;
	R110 = daLopezRadius[m][c+1][f+1][z][t]*(1-dm) + daLopezRadius[m+1][c+1][f+1][z][t]*dm;
	R111 = daLopezRadius[m][c+1][f+1][z][t+1]*(1-dm) + daLopezRadius[m+1][c+1][f+1][z][t+1]*dm;
	R00 = R000*(1-dc) + R100*dc;
	R10 = R010*(1-dc) + R110*dc;
	R01 = R001*(1-dc) + R101*dc;
	R11 = R011*(1-dc) + R111*dc;
	R0 = R00*(1-df) + R10*df;
	R1 = R01*(1-df) + R11*df;
	return (R0*(1-dt) + R1*dt)*REARTH;
}