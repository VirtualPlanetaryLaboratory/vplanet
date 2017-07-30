#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "vplanet.h"

double fdLehmerRadius(double Menv, double lil_g, double RadSurf, double Pxuv, double H){
/**********************************************************
* Input:
* ------
* - Mcore : mass of core [kg]
* - Menv : mass of envelope [kg]
* - dM : mass difference to be subtracted from 
		 current Menv [kg]--> not needed?????????

* ++++Required Params++++
* - Rg : gas constant for atmosphere [4124 -> H2]
* - T : temperature of atmosphere [K]
* - Pxuv : pressure at base of thermosphere [Pa]
*
* Output:
* -------
* - Rxuv : Radius separating hydrostatic equilibrium and 
*  		   hydrodynamic escape in atmospher
*************************************************************/

	double P;		// pressure at surface due to envelope
	double Rxuv;	// see above

	P = lil_g * Menv / (4 * PI * RadSurf * RadSurf); // [kg/ms2]
	Rxuv = RadSurf * RadSurf / (H * log(Pxuv/P) + RadSurf);
	return Rxuv;
}
