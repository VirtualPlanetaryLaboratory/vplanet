/**
  @file verify.h
  @brief Validate the options. This will become a mess!
  @author Rory Barnes ([RoryBarnes](https://github.com/RoryBarnes/))
  @date May 7 2014
*/

/* @cond DOXYGEN_OVERRIDE */

void VerifyBodyExit(char[],char[],char[],int,int,int);
void DoubleLineExit(char[],char[],int,int);
void VerifyTripleExit(char[],char[],char[],int,int,int,char[],int);
void VerifyOptions(BODY*,CONTROL*,FILES*,MODULE*,OPTIONS*,OUTPUT*,SYSTEM*,UPDATE*,fnIntegrate*,fnUpdateVariable****);
void VerifyDynEllip(BODY*,CONTROL*,OPTIONS*,char[],int,int);
int bFloatComparison(double,double);
void fnNullDerivatives(BODY*,EVOLVE*,MODULE*,UPDATE*,fnUpdateVariable***);

/* @endcond */
