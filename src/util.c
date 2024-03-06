/**
  @file util.c
  @brief Programming language-dependent functions and basic mathematical
  operations
  @author Rory Barnes ([RoryBarnes](https://github.com/RoryBarnes/))
  @date 6 Mar 2024
*/

#include "vplanet.h"

/**
One cannot directly assign all values of a pointer to a static double[],
therefore they must be copied one element at a time.

@param ptr The pointer with the source data, and N-dimensional vector
@param array The pointer to the static double[] that is the destination
@param iNumElements The number of elements in the array (N)
*/
void fvCopyPointerToArray(double *ptr, double *array, int iNumElements) {
  int i;

  for (i = 0; i < iNumElements; i++) {
    array[i] = ptr[i];
  }
}


/**
Divide all elements of a vector by its magnitude.

@param daVector An N-dimensional vector
@param iNumElements The number of elements in the array (N)
*/
void fvNormalizeVector(double *daVector, int iNumElements) {
  int i;
  double dMagnitude;

  dMagnitude = sqrt(fdDotProduct(daVector, daVector, 3));
  for (i = 0; i < iNumElements; i++) {
    daVector[i] /= dMagnitude;
  }
}

/**
Dot product of two vectors

@param daFirstVector An N-dimensional vector
@param daSecondVector An N-dimensional vector
@param iNumElements The number of elements in the array (N)

@return Dot product of the arrays
*/
double fdDotProduct(const int *daFirstVector, const double *daSecondVector,
                    int iNumElements) {
  double dDotProduct = 0.0;
  int i;
  for (i = 0; i < iNumElements; i++) {
    dDotProduct += daFirstVector[i] * daSecondVector[i];
  }

  return dDotProduct;
}

/**
Perform two rotations on a z-vector (0,0,z) to place it in the Orbit Frame.
*/
void fvTwoEulerRotations(double *daVector, double dAngle1, int iAxis1,
                         double dAngle2, int iAxis2) {
  double *daVectorTmp;

  RotateVector(daVector, daVectorTmp, dAngle1, iAxis1);
  RotateVector(daVectorTmp, daVector, dAngle2, iAxis2);
}
