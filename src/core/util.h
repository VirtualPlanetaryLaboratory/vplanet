/**
  @file util.h
  @brief Header file for functions in util.c
  @author Rory Barnes ([RoryBarnes](https://github.com/RoryBarnes/))
  @date 6 Mar 2024
*/

void fvCopyPointerToArray(double *, double *, int);
void fvNormalizeVector(double *, int);
double fdDotProduct(const double *, const double *, int);
void fvTwoEulerRotations(double *, double, int, double, int);
void fvCrossProduct(double *, double *, double *);
double *fdaCrossProduct(double *, double *);
void RotateVector(double*, double*, double, int);