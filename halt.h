/********************* HALT.H **********************/ 
/*
 * Rory Barnes, Wed May  7 13:50:14 PDT 2014
 *
 * This header file contains all the subroutines in
 * file halt.c.
 *
*/

int fbCheckHalt(BODY*,CONTROL*,UPDATE*);
void InitializeHalts(CONTROL*,MODULE*);
void VerifyHalts(BODY*,CONTROL*,MODULE*,OPTIONS*);
int HaltMaxEcc(BODY*,EVOLVE*,HALT*,IO*,UPDATE*,int);  
