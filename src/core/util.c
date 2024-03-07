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
double fdDotProduct(const double *daFirstVector, const double *daSecondVector,
                    int iNumElements) {
  double dDotProduct = 0.0;
  int i;
  for (i = 0; i < iNumElements; i++) {
    dDotProduct += daFirstVector[i] * daSecondVector[i];
  }

  return dDotProduct;
}

void RotateVector(double *v1, double *v2, double theta, int axis) {
  if (axis == XAXIS) {
    v2[0] = v1[0];
    v2[1] = cos(theta) * v1[1] - sin(theta) * v1[2];
    v2[2] = sin(theta) * v1[1] + cos(theta) * v1[2];
  } else if (axis == YAXIS) {
    v2[0] = cos(theta) * v1[0] + sin(theta) * v1[2];
    v2[1] = v1[1];
    v2[2] = -sin(theta) * v1[0] + cos(theta) * v1[2];
  } else if (axis == ZAXIS) {
    v2[0] = cos(theta) * v1[0] - sin(theta) * v1[1];
    v2[1] = sin(theta) * v1[0] + cos(theta) * v1[1];
    v2[2] = v1[2];
  } else {
    fprintf(stderr, "ERROR: Invaliad roational axis in RotateVector.\n");
    exit(EXIT_INT);
  }
}

/**
Perform two rotations on a z-vector (0,0,z) to place it in the Orbit Frame.
*/
void fvTwoEulerRotations(double *daVector, double dAngle1, int iAxis1,
                         double dAngle2, int iAxis2) {
  static double daVectorTmp[3];

  RotateVector(daVector, daVectorTmp, dAngle1, iAxis1);
  RotateVector(daVectorTmp, daVector, dAngle2, iAxis2);
}

/**
Calculates cross product of vectors

@param a First vector of cross prodect
@param b Second vector of cross product
@param c Resulting product containing cross product
*/
void fvCrossProduct(double *daVector1, double *daVector2,
                    double *daCrossProduct) {
  daCrossProduct[0] = daVector1[1] * daVector2[2] - daVector2[1] * daVector1[2];
  daCrossProduct[1] = daVector1[2] * daVector2[0] - daVector2[2] * daVector1[0];
  daCrossProduct[2] = daVector1[0] * daVector2[1] - daVector2[0] * daVector1[1];
}

/**
Return the cross product of two vectors
*/
double *fdaCrossProduct(double *daVector1, double *daVector2) {
  static double daCrossProduct[3];
  daCrossProduct[0] = daVector1[1] * daVector2[2] - daVector2[1] * daVector1[2];
  daCrossProduct[1] = daVector1[2] * daVector2[0] - daVector2[2] * daVector1[0];
  daCrossProduct[2] = daVector1[0] * daVector2[1] - daVector2[0] * daVector1[1];

  return daCrossProduct;
}
