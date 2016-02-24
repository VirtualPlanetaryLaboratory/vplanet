/************************ OUTPUT.C **********************/
/*
 * Rory Barnes, Wed May  7 16:38:28 PDT 2014
 *
 * Subroutines for output and logging.
*/


#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include <ctype.h>
#include <string.h>
#include "vplanet.h"
#include "output.h"

/* Individual WriteOutput functions */

/*
 * A
 */

void WriteAge(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  *dTmp = body[iBody].dAge;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else {
    *dTmp /= fdUnitsTime(units->iTime);
    fsUnitsTime(units->iTime,cUnit);
  }
}

/*
 * D
 */

void WriteDeltaTime(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {

  if (control->Evolve.dTime > 0) 
    *dTmp = control->Io.dOutputTime/control->Evolve.nSteps;
  else
    *dTmp = 0;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else {
    *dTmp /= fdUnitsTime(units->iTime);
    fsUnitsTime(units->iTime,cUnit);
  }
}

/*
 * H
 */

void WriteHecc(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {

  *dTmp = body[iBody].dHecc;
  strcpy(cUnit,"");
}

/*
 * K
 */

void WriteKecc(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {

  *dTmp = body[iBody].dKecc;
  strcpy(cUnit,"");
}

/*
 * L
 */

void WriteLongP(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  *dTmp = atan2(body[iBody].dHecc, body[iBody].dKecc);

  while (*dTmp < 0.0) {
    *dTmp += 2*PI;
  }
  while (*dTmp > 2*PI) {
    *dTmp -= 2*PI;
  }
  
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else {
    *dTmp /= fdUnitsAngle(units->iAngle);
    fsUnitsAngle(units->iAngle,cUnit);
  }
}  

/*
 * M
 */

void WriteMass(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {

  *dTmp = body[iBody].dMass;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else {
    *dTmp /= fdUnitsMass(units->iMass);
    fsUnitsMass(units->iMass,cUnit);
  }
}

/*
 * O
 */

void WriteObliquity(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {

  *dTmp = atan2(sqrt(pow(body[iBody].dXobl,2)+pow(body[iBody].dYobl,2)),body[iBody].dZobl);
  
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else {
    *dTmp /= fdUnitsAngle(units->iAngle);
    fsUnitsAngle(units->iAngle,cUnit);
  }
}


void WriteOrbAngMom(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  char cTmp;

  *dTmp = fdOrbAngMom(body);
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else {
    *dTmp *= fdUnitsTime(units->iTime)/(fdUnitsMass(units->iMass)*fdUnitsLength(units->iLength)*fdUnitsLength(units->iLength));
    fsUnitsAngMom(units,cUnit);
  }
}

void WriteOrbEcc(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  if (iBody > 0)
    *dTmp = sqrt(pow(body[iBody].dHecc,2)+pow(body[iBody].dKecc,2));
  else
    *dTmp = -1;
  sprintf(cUnit,"");
}

void WriteOrbEnergy(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {

  *dTmp = fdOrbEnergy(body,iBody);
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else {
    *dTmp /= fdUnitsEnergy(units->iTime,units->iMass,units->iLength);
    fsUnitsEnergy(units,cUnit);
  }
}

void WriteOrbMeanMotion(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {

  if (iBody > 0)
    *dTmp = body[iBody].dMeanMotion;
  else
    *dTmp = -1;

  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else {
    *dTmp /= fdUnitsTime(units->iTime);
    fsUnitsRate(units->iTime,cUnit);
  }
}

void WriteOrbPeriod(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {

  if (iBody > 0)
    *dTmp = fdSemiToPeriod(body[iBody].dSemi,(body[0].dMass+body[iBody].dMass));
  else
    *dTmp=-1;

  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else {
    *dTmp *= fdUnitsTime(units->iTime);
    fsUnitsTime(units->iTime,cUnit);
  }
}

void WriteOrbSemi(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  if (iBody > 0)
    *dTmp = body[iBody].dSemi;
  else
    *dTmp = -1;

  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else {
    *dTmp /= fdUnitsLength(units->iLength);
    fsUnitsLength(units->iLength,cUnit);
  }
}

/*
 * R
 */

void WriteRadius(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {

  *dTmp = body[iBody].dRadius;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else {
    *dTmp /= fdUnitsLength(units->iLength);
    fsUnitsLength(units->iLength,cUnit);
  }
}

void WriteRotAngMom(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {

  *dTmp = fdRotAngMom(body[iBody].dRadGyra,body[iBody].dMass,body[iBody].dRadius,body[iBody].dRotRate);

  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else {
    *dTmp *= fdUnitsTime(units->iTime)/(fdUnitsMass(units->iMass)*fdUnitsLength(units->iLength)*fdUnitsLength(units->iLength));
    fsUnitsAngMom(units,cUnit);
  }
}

void WriteRotKinEnergy(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {

  *dTmp = fdRotKinEnergy(body[iBody].dMass,body[iBody].dRadius,body[iBody].dRadGyra,body[iBody].dRotRate);
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else {
    *dTmp /= fdUnitsEnergy(units->iTime,units->iMass,units->iLength);
    fsUnitsEnergy(units,cUnit);
  }
}

void WriteRotRate(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {

  *dTmp = body[iBody].dRotRate;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else {
    *dTmp *= fdUnitsTime(units->iTime);
    fsUnitsRate(units->iTime,cUnit);
  }
}

void WriteRotPer(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {

  *dTmp = fdFreqToPer(body[iBody].dRotRate);
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else {
    *dTmp /= fdUnitsTime(units->iTime);
    fsUnitsTime(units->iTime,cUnit);
  }
}

void WriteRotVel(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {

  *dTmp = fdRotVel(body[iBody].dRadius,body[iBody].dRotRate);
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else {
    *dTmp *= fdUnitsTime(units->iTime)/fdUnitsLength(units->iLength);
    fsUnitsVel(units,cUnit);
  }
}

void WriteSurfaceEnergyFlux(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  /* Multiple modules can contribute to this output */
  int iModule;

  *dTmp=0;
  for (iModule=0;iModule<control->Evolve.iNumModules[iBody];iModule++)
    // Only module reference in file, can this be changed? XXX
    *dTmp += output->fnOutput[iBody][iModule](body,system,update,iBody,control->Evolve.iEqtideModel);

  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else {
    *dTmp /= fdUnitsEnergyFlux(units->iTime,units->iMass,units->iLength);
    fsUnitsEnergyFlux(units,cUnit);
  }
}

void WriteTime(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {

  *dTmp = control->Evolve.dTime;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else {
    *dTmp /= fdUnitsTime(units->iTime);
    fsUnitsTime(units->iTime,cUnit);
  }
}

void WriteTotAngMom(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {

  *dTmp = fdTotAngMom(body);

  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else {
    *dTmp *= fdUnitsTime(units->iTime)/(fdUnitsMass(units->iMass)*fdUnitsLength(units->iLength)*fdUnitsLength(units->iLength));
    fsUnitsAngMom(units,cUnit);
  }
}

void WriteTotEnergy(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {

  *dTmp = fdTotEnergy(body);

  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else {
    *dTmp /= fdUnitsEnergy(units->iTime,units->iMass,units->iLength);
    fsUnitsEnergy(units,cUnit);
  }
}

void WritePotEnergy(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {

  *dTmp = fdPotEnergy(body);

  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else {
    *dTmp /= fdUnitsEnergy(units->iTime,units->iMass,units->iLength);
    fsUnitsEnergy(units,cUnit);
  }
}

void WriteKinEnergy(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {

  *dTmp = fdKinEnergy(body);

  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else {
    *dTmp /= fdUnitsEnergy(units->iTime,units->iMass,units->iLength);
    fsUnitsEnergy(units,cUnit);
  }
}

void WriteOrbKinEnergy(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {

  if (iBody > 0)
    *dTmp = fdOrbKinEnergy(body[0].dMass,body[iBody].dMass,body[iBody].dSemi);
  else
    *dTmp = -1;

  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else {
    *dTmp /= fdUnitsEnergy(units->iTime,units->iMass,units->iLength);
    fsUnitsEnergy(units,cUnit);
  }
}

void WriteOrbPotEnergy(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {

  if (iBody > 0)
    *dTmp = fdOrbPotEnergy(body[0].dMass,body[iBody].dMass,body[iBody].dSemi);
  else
    *dTmp = -1;

  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else {
    *dTmp /= fdUnitsEnergy(units->iTime,units->iMass,units->iLength);
    fsUnitsEnergy(units,cUnit);
  }
}

void WriteTidalQ(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {

  // XXX I don't think this will work with just eqtide
  //*dTmp = body[iBody].dK2Man/body[iBody].dImk2Man;
  *dTmp = body[iBody].dViscUMan*body[iBody].dMeanMotion/body[iBody].dShmodUMan;
  strcpy(cUnit,"");
}

void WriteXobl(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {

  *dTmp = body[iBody].dXobl;
  strcpy(cUnit,"");
}

void WriteYobl(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {

  *dTmp = body[iBody].dYobl;
  strcpy(cUnit,"");
}

void WriteZobl(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {

  *dTmp = body[iBody].dZobl;
  strcpy(cUnit,"");
}

/*
 * End individual write functions
 */

void InitializeOutputGeneral(OUTPUT *output,fnWriteOutput fnWrite[]) {
  /*
   * Age
   */
  
  sprintf(output[OUT_AGE].cName,"Age");
  sprintf(output[OUT_AGE].cDescr,"System Age");
  sprintf(output[OUT_AGE].cNeg,"Gyr");
  output[OUT_AGE].bNeg = 1;
  output[OUT_AGE].dNeg = 1./(YEARSEC*1e9);
  output[OUT_AGE].iNum = 1;
  fnWrite[OUT_AGE] = &WriteAge;

  /*
   * D
   */
  
  sprintf(output[OUT_DT].cName,"DeltaTime");
  sprintf(output[OUT_DT].cDescr,"Average Timestep Over Last Output Interval");
  sprintf(output[OUT_DT].cNeg,"years");
  output[OUT_DT].bNeg = 1;
  output[OUT_DT].dNeg = 1./YEARSEC;
  output[OUT_DT].iNum = 1;
  fnWrite[OUT_DT] = &WriteDeltaTime;
  
  /*
   * H
   */

  sprintf(output[OUT_HECC].cName,"HEcc");
  sprintf(output[OUT_HECC].cDescr,"Poincare's h (=e*sin(varpi)");
  output[OUT_HECC].bNeg = 0;
  output[OUT_HECC].iNum = 1;
  fnWrite[OUT_HECC] = &WriteHecc;
  
  /*
   * K
   */

  sprintf(output[OUT_KECC].cName,"KEcc");
  sprintf(output[OUT_KECC].cDescr,"Poincare's k (=e*cos(varpi)");
  output[OUT_KECC].bNeg = 0;
  output[OUT_KECC].iNum = 1;
  fnWrite[OUT_KECC] = &WriteKecc;

  /*
   * L
   */

  sprintf(output[OUT_LONGP].cName,"LongP");
  sprintf(output[OUT_LONGP].cDescr,"Body's Longitude of pericenter in Lagrange");
  sprintf(output[OUT_LONGP].cNeg,"Deg");
  output[OUT_LONGP].bNeg = 1;
  output[OUT_LONGP].dNeg = 1./DEGRAD;
  output[OUT_LONGP].iNum = 1;
  fnWrite[OUT_LONGP] = &WriteLongP; 
  
  /*
   * M
   */
  
  sprintf(output[OUT_MASS].cName,"Mass");
  sprintf(output[OUT_MASS].cDescr,"Mass");
  sprintf(output[OUT_MASS].cNeg,"Earth");
  output[OUT_MASS].bNeg = 1;
  output[OUT_MASS].dNeg = 1./MEARTH;
  output[OUT_MASS].iNum = 1;
  fnWrite[OUT_MASS] = &WriteMass;
  
  
  /*
   *   O
   */
  
  sprintf(output[OUT_OBL].cName,"Obliquity");
  sprintf(output[OUT_OBL].cDescr,"Obliquity");
  sprintf(output[OUT_OBL].cNeg,"deg");
  output[OUT_OBL].bNeg = 1;
  output[OUT_OBL].dNeg = DEGRAD;
  output[OUT_OBL].iNum = 1;
  fnWrite[OUT_OBL] = &WriteObliquity;
  
  sprintf(output[OUT_ORBANGMOM].cName,"OrbAngMom");
  sprintf(output[OUT_ORBANGMOM].cDescr,"Orbital Angular Momentum");
  output[OUT_ORBANGMOM].iNum = 1;
  fnWrite[OUT_ORBANGMOM] = &WriteOrbAngMom;
  
  sprintf(output[OUT_ORBECC].cName,"Eccentricity");
  sprintf(output[OUT_ORBECC].cDescr,"Orbital Eccentricity");
  output[OUT_ORBECC].iNum = 1;
  output[OUT_ORBECC].bNeg = 0;
  fnWrite[OUT_ORBECC] = &WriteOrbEcc;
  
  sprintf(output[OUT_ORBEN].cName,"OrbEnergy");
  sprintf(output[OUT_ORBEN].cDescr,"Orbital Energy");
  sprintf(output[OUT_ORBEN].cNeg,"ergs");
  output[OUT_ORBEN].bNeg = 1;
  output[OUT_ORBEN].iNum = 1;
  fnWrite[OUT_ORBEN] = &WriteOrbEnergy;
  
  sprintf(output[OUT_ORBMEANMOTION].cName,"MeanMotion");
  sprintf(output[OUT_ORBMEANMOTION].cDescr,"Orbital Mean Motion");
  sprintf(output[OUT_ORBMEANMOTION].cNeg,"/day");
  output[OUT_ORBMEANMOTION].bNeg = 1;
  output[OUT_ORBMEANMOTION].dNeg = DAYSEC;
  output[OUT_ORBMEANMOTION].iNum = 1;
  fnWrite[OUT_ORBMEANMOTION] = &WriteOrbMeanMotion;
  
  sprintf(output[OUT_ORBPER].cName,"OrbPeriod");
  sprintf(output[OUT_ORBPER].cDescr,"Orbital Period");
  sprintf(output[OUT_ORBPER].cNeg,"days");
  output[OUT_ORBPER].bNeg = 1;
  output[OUT_ORBPER].dNeg = 1./DAYSEC;
  output[OUT_ORBPER].iNum = 1;
  fnWrite[OUT_ORBPER] = &WriteOrbPeriod;
  
  sprintf(output[OUT_ORBSEMI].cName,"SemiMajorAxis");
  sprintf(output[OUT_ORBSEMI].cDescr,"Semi-major Axis");
  sprintf(output[OUT_ORBSEMI].cNeg,"AU"); 
  output[OUT_ORBSEMI].bNeg = 1;
  output[OUT_ORBSEMI].dNeg = 1./AUCM;
  output[OUT_ORBSEMI].iNum = 1;
  fnWrite[OUT_ORBSEMI] = &WriteOrbSemi;
  
  /*
   * R
   */
  
  sprintf(output[OUT_RADIUS].cName,"Radius");
  sprintf(output[OUT_RADIUS].cDescr,"Radius");
  output[OUT_RADIUS].bNeg = 1;
  sprintf(output[OUT_RADIUS].cNeg,"Solar");
  output[OUT_RADIUS].dNeg = 1./RSUN;
  output[OUT_RADIUS].iNum = 1;
  fnWrite[OUT_RADIUS] = &WriteRadius;
  
  sprintf(output[OUT_ROTANGMOM].cName,"RotAngMom");
  sprintf(output[OUT_ROTANGMOM].cDescr,"Rotational Angular Momentum");
  output[OUT_ROTANGMOM].bNeg = 0;
  output[OUT_ROTANGMOM].iNum = 1;
  fnWrite[OUT_ROTANGMOM] = &WriteRotAngMom;
  
  sprintf(output[OUT_ROTKINENERGY].cName,"RotKinEnergy");
  sprintf(output[OUT_ROTKINENERGY].cDescr,"Rotational Energy");
  sprintf(output[OUT_ROTKINENERGY].cNeg,"ergs");
  output[OUT_ROTKINENERGY].iNum = 1;
  fnWrite[OUT_ROTKINENERGY] = &WriteRotKinEnergy;
  
  sprintf(output[OUT_ROTPER].cName,"RotPer");
  sprintf(output[OUT_ROTPER].cDescr,"Rotational Period");
  sprintf(output[OUT_ROTPER].cNeg,"days");
  output[OUT_ROTPER].bNeg = 1;
  output[OUT_ROTPER].dNeg = 1./DAYSEC;
  output[OUT_ROTPER].iNum = 1;
  fnWrite[OUT_ROTPER] = &WriteRotPer;
  
  sprintf(output[OUT_ROTRATE].cName,"RotRate");
  sprintf(output[OUT_ROTRATE].cDescr,"Rotational Frequency");
  sprintf(output[OUT_ROTRATE].cNeg,"/day");
  output[OUT_ROTRATE].bNeg = 1;
  output[OUT_ROTRATE].dNeg = DAYSEC;
  output[OUT_ROTRATE].iNum = 1;
  fnWrite[OUT_ROTRATE] = &WriteRotRate;
  
  sprintf(output[OUT_ROTVEL].cName,"RotVel");
  sprintf(output[OUT_ROTVEL].cDescr,"Rotational Velocity");
  sprintf(output[OUT_ROTVEL].cNeg,"km/s");
  output[OUT_ROTVEL].bNeg = 1;
  output[OUT_ROTVEL].dNeg = 1e-5;
  output[OUT_ROTVEL].iNum = 1;
  fnWrite[OUT_ROTVEL] = &WriteRotVel;
  
  sprintf(output[OUT_SURFENFLUX].cName,"SurfEnFluxTotal");
  sprintf(output[OUT_SURFENFLUX].cDescr,"Total Surface Energy Flux");
  sprintf(output[OUT_SURFENFLUX].cNeg,"W/m^2");
  output[OUT_SURFENFLUX].bNeg = 1;
  output[OUT_SURFENFLUX].dNeg = 1e-3;
  output[OUT_SURFENFLUX].iNum = 1;
  fnWrite[OUT_SURFENFLUX] = &WriteSurfaceEnergyFlux;
  
  sprintf(output[OUT_TIME].cName,"Time");
  sprintf(output[OUT_TIME].cDescr,"Simulation Time");
  sprintf(output[OUT_TIME].cNeg,"Gyr");
  output[OUT_TIME].bNeg = 1;
  output[OUT_TIME].dNeg = 1./(YEARSEC*1e9);
  output[OUT_TIME].iNum = 1;
  output[OUT_TIME].bGrid = 2;
  fnWrite[OUT_TIME] = &WriteTime;
  
  sprintf(output[OUT_TOTANGMOM].cName,"TotAngMom");
  sprintf(output[OUT_TOTANGMOM].cDescr,"Total Angular Momentum");
  output[OUT_TOTANGMOM].iNum = 1;
  fnWrite[OUT_TOTANGMOM] = &WriteTotAngMom;
  
  sprintf(output[OUT_TOTENERGY].cName,"TotEnergy");
  sprintf(output[OUT_TOTENERGY].cDescr,"Total Energy");
  sprintf(output[OUT_TOTENERGY].cNeg,"ergs");
  output[OUT_TOTENERGY].bNeg = 1;
  output[OUT_TOTENERGY].dNeg = 1;
  output[OUT_TOTENERGY].iNum = 1;
  fnWrite[OUT_TOTENERGY] = &WriteTotEnergy;
  
  sprintf(output[OUT_POTENERGY].cName,"PotEnergy");
  sprintf(output[OUT_POTENERGY].cDescr,"Total Potential Energy");
  sprintf(output[OUT_POTENERGY].cNeg,"ergs");
  output[OUT_POTENERGY].bNeg = 1;
  output[OUT_POTENERGY].dNeg = 1;
  output[OUT_POTENERGY].iNum = 1;
  fnWrite[OUT_POTENERGY] = &WritePotEnergy;
  
  sprintf(output[OUT_KINENERGY].cName,"KinEnergy");
  sprintf(output[OUT_KINENERGY].cDescr,"Total Kinetic Energy");
  sprintf(output[OUT_KINENERGY].cNeg,"ergs");
  output[OUT_KINENERGY].bNeg = 1;
  output[OUT_KINENERGY].dNeg = 1;
  output[OUT_KINENERGY].iNum = 1;
  fnWrite[OUT_KINENERGY] = &WriteKinEnergy;
  
  sprintf(output[OUT_ORBKINENERGY].cName,"OrbKinEnergy");
  sprintf(output[OUT_ORBKINENERGY].cDescr,"Orbital Kinetic Energy");
  sprintf(output[OUT_ORBKINENERGY].cNeg,"ergs");
  output[OUT_ORBKINENERGY].bNeg = 1;
  output[OUT_ORBKINENERGY].dNeg = 1;
  output[OUT_ORBKINENERGY].iNum = 1;
  fnWrite[OUT_ORBKINENERGY] = &WriteOrbKinEnergy;
  
  sprintf(output[OUT_ORBPOTENERGY].cName,"OrbPotEnergy");
  sprintf(output[OUT_ORBPOTENERGY].cDescr,"Orbital Potential Energy");
  sprintf(output[OUT_ORBPOTENERGY].cNeg,"ergs");
  output[OUT_ORBPOTENERGY].bNeg = 1;
  output[OUT_ORBPOTENERGY].dNeg = 1;
  output[OUT_ORBPOTENERGY].iNum = 1;
  fnWrite[OUT_ORBPOTENERGY] = &WriteOrbPotEnergy;
  
  sprintf(output[OUT_ORBENERGY].cName,"OrbEnergy");
  sprintf(output[OUT_ORBENERGY].cDescr,"Orbital Energy");
  sprintf(output[OUT_ORBENERGY].cNeg,"ergs");
  output[OUT_ORBENERGY].bNeg = 1;
  output[OUT_ORBENERGY].dNeg = 1;
  output[OUT_ORBENERGY].iNum = 1;
  fnWrite[OUT_ORBENERGY] = &WriteOrbEnergy;

  sprintf(output[OUT_TIDALQ].cName,"TidalQ");
  sprintf(output[OUT_TIDALQ].cDescr,"Tidal Q");
  output[OUT_TIDALQ].bNeg = 0;
  output[OUT_TIDALQ].iNum = 1;
  fnWrite[OUT_TIDALQ] = WriteTidalQ;

  sprintf(output[OUT_XOBL].cName,"Xobl");
  sprintf(output[OUT_XOBL].cDescr,"Body's sin(obl)*cos(pA)");
  output[OUT_XOBL].iNum = 1;
  fnWrite[OUT_XOBL] = &WriteXobl;
  
  sprintf(output[OUT_YOBL].cName,"Yobl");
  sprintf(output[OUT_YOBL].cDescr,"Body's sin(obl)*sin(pA)");
  output[OUT_YOBL].iNum = 1;
  fnWrite[OUT_YOBL] = &WriteYobl;
  
  sprintf(output[OUT_ZOBL].cName,"Zobl");
  sprintf(output[OUT_ZOBL].cDescr,"Body's cos(obl)");
  output[OUT_ZOBL].iNum = 1;
  fnWrite[OUT_ZOBL] = &WriteZobl;  

}

void InitializeOutputFunctions(MODULE *module,OUTPUT *output,int iNumBodies) {
  int iBody,iModule;

  // Add new mult-module outputs here
  output[OUT_SURFENFLUX].fnOutput = malloc(iNumBodies*sizeof(fnOutputModule*));

  for (iBody=0;iBody<iNumBodies;iBody++) {
    // Malloc number of modules for each multi-module output
    output[OUT_SURFENFLUX].fnOutput[iBody] = malloc(module->iNumModules[iBody]*sizeof(fnOutputModule));
    for (iModule=0;iModule<module->iNumModules[iBody];iModule++) {
      /* Initialize them all to return nothing, then they get changed 
      from AddModule subroutines */
      output[OUT_SURFENFLUX].fnOutput[iBody][iModule] = &fdReturnOutputZero;
    }
  }
}

/*
void FinalizeOutputFunctions(MODULE *module,OUTPUT *output,int iBody) {
  First initialize functions for the number of bodies 
  module->fnFinalizeOutput[iBody] = malloc(module->iNumModules[iBody]*sizeof(fnOutputModule));
  for (iModule=0;iModule<module->iNumModules[iBody];iModule++) 
    module->fnFinalizeOutput[iBody][iModule];
}
*/

void CGSUnits(UNITS *units) {
  units->iTime = 0;
  units->iLength = 0;
  units->iMass = 0;
  units->iAngle = 0;
}

void WriteLogEntry(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UPDATE *update,fnWriteOutput fnWrite,FILE *fp,int iBody) {
  double *dTmp;
  char cUnit[48];
  UNITS units;
  int i,j,iNumIndices;


  cUnit[0]='\0';
  dTmp=malloc(output->iNum*sizeof(double));
  CGSUnits(&units);
  fnWrite(body,control,output,system,&units,update,iBody,dTmp,cUnit);

  fprintf(fp,"(%s) %s [%s]: ",output->cName,output->cDescr,cUnit);
  for (j=0;j<output->iNum;j++) {
    fprintd(fp,dTmp[j],control->Io.iSciNot,control->Io.iDigits);
    fprintf(fp," ");
  }
  free(dTmp);
  fprintf(fp,"\n");
}

void LogUnits(FILE *fp) {
  /* Mass Units */
  UNITS units;

  CGSUnits(&units);

  fprintf(fp,"Mass Units: ");
  if (units.iMass == 0) {
    fprintf(fp,"Kilograms\n");
  } else if (units.iMass == 1) {
    fprintf(fp,"Grams\n");
  } else if (units.iMass == 2) {
    fprintf(fp,"Solar\n");
  } else if (units.iMass == 3) {
    fprintf(fp,"Earth\n");
  } else if (units.iMass == 4) {
    fprintf(fp,"Jupiter\n");
  } else if (units.iMass == 5) {
    fprintf(fp,"Saturn\n");
  }
  
  /* Length Units */
  
  fprintf(fp,"Length Units: ");
  if (units.iLength == 0) {
    fprintf(fp,"Meters\n");
  } else if (units.iLength == 1) {
    fprintf(fp,"Centimeters\n");
  } else if (units.iLength == 2) {
    fprintf(fp,"Kilometers\n");
  } else if (units.iLength == 3) {
    fprintf(fp,"Solar\n");
  } else if (units.iLength == 4) {
    fprintf(fp,"Earth\n");
  } else if (units.iLength == 5) {
    fprintf(fp,"Jupiter\n");
  } else if (units.iLength == 6) {
    fprintf(fp,"AU\n");
  }
  
  /* Time Units */
  
  fprintf(fp,"Time Units: ");
  if (units.iTime == 0) {
    fprintf(fp,"Seconds\n");
  } else if (units.iTime == 1) {
    fprintf(fp,"Days\n");
  } else if (units.iTime == 2) {
    fprintf(fp,"Years\n");
  } else if (units.iTime == 3) {
    fprintf(fp,"Megayears\n");
  } else if (units.iTime == 4) {
    fprintf(fp,"Gigayears\n");
  }
  
  /* Angle Units */
  
  fprintf(fp,"Angle Units: ");
  if (units.iAngle == 0) {
    fprintf(fp,"Radians\n");
  } else if (units.iAngle == 1) {
    fprintf(fp,"Degrees\n");
  }
  
}

void LogIntegration(CONTROL *control,FILE *fp) {

  fprintf(fp,"Integration Method: ");
  if (control->Evolve.iOneStep == EULER)
    fprintf(fp,"Euler");
  else if (control->Evolve.iOneStep == RUNGEKUTTA)
    fprintf(fp,"Runge-Kutta4");
  fprintf(fp,"\n");

  fprintf(fp,"Direction: ");
  if (control->Evolve.bDoBackward) 
    fprintf(fp,"Backward\n");
  else
    fprintf(fp,"Forward\n");

  fprintf(fp,"Time Step: ");
  fprintd(fp,control->Evolve.dTimeStep,control->Io.iSciNot,control->Io.iDigits);
  fprintf(fp,"\n");
  
  fprintf(fp,"Stop Time: ");
  fprintd(fp,control->Evolve.dStopTime,control->Io.iSciNot,control->Io.iDigits);
  fprintf(fp,"\n");
  
  fprintf(fp,"Output Interval: ");
  fprintd(fp,control->Io.dOutputTime,control->Io.iSciNot,control->Io.iDigits);
  fprintf(fp,"\n");
  
  fprintf(fp,"Use Variable Timestep: ");
  if (control->Evolve.bVarDt == 0) {
    fprintf(fp,"No\n");
  } else {
    fprintf(fp,"Yes\n");
    fprintf(fp,"dEta: ");
    fprintd(fp,control->Evolve.dEta,control->Io.iSciNot,control->Io.iDigits);
    fprintf(fp,"\n");
  }
}

void LogHalt(CONTROL *control,FILE *fp) {

  fprintf(fp,"Minimum Value of ecc and obl: ");
  fprintd(fp,control->Evolve.dMinValue,control->Io.iSciNot,control->Io.iDigits);
  fprintf(fp,"\n");
  fprintf(fp,"\n");

}

void LogBodyRelations(CONTROL *control,FILE *fp,int iBody) {

  if (control->iMassRad[iBody] > 0) {
    fprintf(fp,"Mass-Radius Relationship: ");
    if (control->iMassRad[iBody] == 1) {
      fprintf(fp,"New Light On Dark Stars\n");
    } else if (control->iMassRad[iBody] == 2) {
      fprintf(fp,"Gorda, S.Y. & Svechnikov, M.A. 1999, Astronomy Reports, 43, 521-525.\n");
    } else if (control->iMassRad[iBody] == 3) {
      fprintf(fp,"Bayless, A.J. & Orosz, J.A. 2006, ApJ, 651, 1155-1165.\n");
    } else if (control->iMassRad[iBody] == 4) {
      fprintf(fp,"Sotin, C. et al. 2007, Icarus, 191, 337-351.\n");
    } else {
      fprintf(fp,"Unknown!\n");
      fprintf(stderr,"Unknown Mass-Radius Relationship!\n");
      exit(EXIT_INPUT);
    }
  }
}

void LogOutputOrder(BODY *body,CONTROL *control,FILES *files,OUTPUT *output,SYSTEM *system,UPDATE *update,fnWriteOutput fnWrite[],FILE *fp,int iBody) {
  int iCol,iOut,iSubOut,iExtra=0;
  char cCol[NUMOUT][OPTLEN];
  double *dTmp;
  char cUnit[48],cTmp[48];
  
  for (iCol=0;iCol<files->Outfile[iBody].iNumCols;iCol++) {
    for (iOut=0;iOut<MODULEOUTEND;iOut++) {
      if (memcmp(files->Outfile[iBody].caCol[iCol],output[iOut].cName,strlen(output[iOut].cName)) == 0) {
        /* Match! */
        dTmp=malloc(output[iOut].iNum*sizeof(double));
        fnWrite[iOut](body,control,&output[iOut],system,&control->Units[iBody],update,iBody,dTmp,cUnit);
        for (iSubOut=0;iSubOut<output[iOut].iNum;iSubOut++) {
          strcpy(cCol[iCol+iSubOut+iExtra],files->Outfile[iBody].caCol[iCol]);
          sprintf(cTmp,"[%s]",cUnit);
          strcat(cCol[iCol+iSubOut+iExtra],cTmp);
        }
        iExtra += (output[iOut].iNum-1);
	free(dTmp);
      }
      
    }
  }

  fprintf(fp,"Output Order:");
  for (iCol=0;iCol<(files->Outfile[iBody].iNumCols + iExtra);iCol++)
    fprintf(fp," %s",cCol[iCol]);
  fprintf(fp, "\n");
}

void LogGridOutput(BODY *body,CONTROL *control,FILES *files,OUTPUT *output,SYSTEM *system,UPDATE *update,fnWriteOutput fnWrite[],FILE *fp,int iBody) {
  int iCol,iOut,iSubOut,iExtra=0;
  char cCol[NUMOUT][OPTLEN];
  double *dTmp;
  char cUnit[48],cTmp[48];
  

  for (iCol=0;iCol<files->Outfile[iBody].iNumGrid;iCol++) {
    for (iOut=0;iOut<MODULEOUTEND;iOut++) {
      if (memcmp(files->Outfile[iBody].caGrid[iCol],output[iOut].cName,strlen(output[iOut].cName)) == 0) {
        /* Match! */
        dTmp=malloc(output[iOut].iNum*sizeof(double));
        fnWrite[iOut](body,control,&output[iOut],system,&control->Units[iBody],update,iBody,dTmp,cUnit);
        for (iSubOut=0;iSubOut<output[iOut].iNum;iSubOut++) {
          strcpy(cCol[iCol+iSubOut+iExtra],files->Outfile[iBody].caGrid[iCol]);
          sprintf(cTmp,"[%s]",cUnit);
          strcat(cCol[iCol+iSubOut+iExtra],cTmp);
        }
        iExtra += (output[iOut].iNum-1);
  free(dTmp);
      }
    }
  }
  
  
  fprintf(fp,"Grid Output Order:");
  for (iCol=0;iCol<(files->Outfile[iBody].iNumGrid + iExtra);iCol++)
    fprintf(fp," %s",cCol[iCol]);
  fprintf(fp, "\n");
}

void LogOptions(CONTROL *control,FILES *files,MODULE *module,SYSTEM *system,FILE *fp) {
  int iFile,iModule;

  fprintf(fp,"-------- Log file %s -------\n\n",files->cLog);
  fprintf(fp,"Executable: %s\n",files->cExe);
  
  fprintf(fp,"System Name: %s\n",system->cName);
  fprintf(fp,"Primary Input File: %s\n",files->Infile[0].cIn);
  for (iFile=1;iFile<files->iNumInputs;iFile++) 
    fprintf(fp,"Body File #%d: %s\n",iFile,files->Infile[iFile].cIn);
  fprintf(fp,"Allow files to be overwitten: ");
  if (control->Io.bOverwrite)
    fprintf(fp,"Yes");
  else
    fprintf(fp,"No");
  fprintf(fp,"\n");
  
  /* Log modules XXX?
  for (iModule=0;iModule<module->iNumModules;iModule++)
    module->fnLogOptions[iModule](control,fp);
  */  

  LogUnits(fp);

  fprintf(fp,"\n------- FORMATTING -----\n");
  fprintf(fp,"Verbosity Level: %d\n",control->Io.iVerbose);
  fprintf(fp,"Crossover Decade for Scientific Notation: %d\n",control->Io.iSciNot);
  fprintf(fp,"Number of Digits After Decimal: %d\n",control->Io.iDigits);
  
  if (control->Evolve.bDoForward || control->Evolve.bDoBackward) {
    LogIntegration(control,fp);
    LogHalt(control,fp);
  }
} 

void LogSystem(BODY *body,CONTROL *control,MODULE *module,OUTPUT *output,SYSTEM *system,UPDATE *update,fnWriteOutput fnWrite[],FILE *fp) {
  int iOut,iModule;

  fprintf(fp,"SYSTEM PROPERTIES ----\n");

  for (iOut=OUTSTART;iOut<OUTBODYSTART;iOut++) {
    if (output[iOut].iNum > 0) 
      WriteLogEntry(body,control,&output[iOut],system,update,fnWrite[iOut],fp,0);
  }

  /* Log modules? XXX
  for (iModule=0;iModule<module->iNumModules;iModule++) 
    module->fnLog[iModule](control,output,body,system,update,fnWrite,fp);
 */
}

void LogBody(BODY *body,CONTROL *control,FILES *files,MODULE *module,OUTPUT *output,SYSTEM *system,fnWriteOutput fnWrite[],FILE *fp,UPDATE *update) {
  int iBody,iOut,iModule;

  for (iBody=0;iBody<control->Evolve.iNumBodies;iBody++) {
    fprintf(fp,"\n----- BODY: %s ----\n",body[iBody].cName);
    for (iOut=OUTBODYSTART;iOut<OUTEND;iOut++) {
      LogBodyRelations(control,fp,iBody);
      if (output[iOut].iNum > 0) 
        WriteLogEntry(body,control,&output[iOut],system,update,fnWrite[iOut],fp,iBody);
    }
    /* Log modules */
    for (iModule=0;iModule<module->iNumModules[iBody];iModule++)
      module->fnLogBody[iBody][iModule](body,control,output,system,update,fnWrite,fp,iBody);
    
    LogOutputOrder(body,control,files,output,system,update,fnWrite,fp,iBody);
    LogGridOutput(body,control,files,output,system,update,fnWrite,fp,iBody);
  }
}

void WriteLog(BODY *body,CONTROL *control,FILES *files,MODULE *module,OPTIONS *options,OUTPUT *output,SYSTEM *system,UPDATE *update,fnUpdateVariable ***fnUpdate,fnWriteOutput fnWrite[],int iEnd) {
  char cTime[OPTLEN];
  FILE *fp;
  double dDt;

  /* Get derivatives */
  PropertiesAuxiliary(body,control,update);
  dDt=fdGetUpdateInfo(body,control,system,update,fnUpdate);

  if (iEnd == 0) {
    sprintf(cTime,"Input");
    fp=fopen(files->cLog,"w");
  } else if (iEnd == 1) {
    sprintf(cTime,"Final");
    fp=fopen(files->cLog,"a");
  } else if (iEnd == -1) {
    sprintf(cTime,"Initial");
    fp=fopen(files->cLog,"w");
  }
  
  if (!iEnd) {
    LogOptions(control,files,module,system,fp);

    fprintf(fp,"\n---- INITIAL ");
  } else 
    fprintf(fp,"\n\n\n---- FINAL ");

  /* System Properties */
  LogSystem(body,control,module,output,system,update,fnWrite,fp);
  
  /* Bodies' Properties */
  LogBody(body,control,files,module,output,system,fnWrite,fp,update);

  fclose(fp);
}

void WriteOutput(BODY *body,CONTROL *control,FILES *files,OUTPUT *output,SYSTEM *system,UPDATE *update,fnWriteOutput *fnWrite,double dTime,double dDt){
  int iBody,iCol,iOut,iSubOut,iExtra=0,iGrid,iLat,jBody,j;
  double dCol[NUMOPT],*dTmp,dGrid[NUMOPT];
  FILE *fp;
  char cUnit[OPTLEN], cPoiseGrid[NAMELEN], cLaplaceFunc[NAMELEN];

  /* Write out all data columns for each body. As some data may span more than
     1 column, we search the input list sequentially, adding iExtra to the
     total number of columns as we go. The calls to fnWrite return the column
     value in the correct units, and output.iNum already contains the
     number of columns. */

  for (iBody=0;iBody<control->Evolve.iNumBodies;iBody++) {
    for (iCol=0;iCol<files->Outfile[iBody].iNumCols;iCol++) {
      for (iOut=0;iOut<MODULEOUTEND;iOut++) {
        if (output[iOut].bGrid == 0 || output[iOut].bGrid == 2) {
          if (memcmp(files->Outfile[iBody].caCol[iCol],output[iOut].cName,strlen(output[iOut].cName)) == 0) {
            /* Match! */
            dTmp=malloc(output[iOut].iNum*sizeof(double));
            fnWrite[iOut](body,control,&output[iOut],system,&control->Units[iBody],update,iBody,dTmp,cUnit);
            for (iSubOut=0;iSubOut<output[iOut].iNum;iSubOut++)
              dCol[iCol+iSubOut+iExtra]=dTmp[iSubOut];
            iExtra += (output[iOut].iNum-1);
	    free(dTmp);
	    dTmp = NULL;
          }
        }
      }
    }

    /* Now write the columns */
    fp = fopen(files->Outfile[iBody].cOut,"a");
    for (iCol=0;iCol<files->Outfile[iBody].iNumCols+iExtra;iCol++) {
      fprintd(fp,dCol[iCol],control->Io.iSciNot,control->Io.iDigits);
      fprintf(fp," ");
    }
    fprintf(fp,"\n");
    fclose(fp);
   
    /* Grid outputs, currently only set up for POISE */
    if (body[iBody].bPoise) {
      dTmp = malloc(1*sizeof(double));
      for (iLat=0;iLat<body[iBody].iNumLats;iLat++) {
        for (iGrid=0;iGrid<files->Outfile[iBody].iNumGrid;iGrid++) {
          for (iOut=0;iOut<MODULEOUTEND;iOut++) {
            if (output[iOut].bGrid == 1 || output[iOut].bGrid == 2) {
              if (memcmp(files->Outfile[iBody].caGrid[iGrid],output[iOut].cName,strlen(output[iOut].cName)) == 0) {
                body[iBody].iWriteLat = iLat;
                fnWrite[iOut](body,control,&output[iOut],system,&control->Units[iBody],update,iBody,dTmp,cUnit);
                dGrid[iGrid]=*dTmp;
              }
            }
          }
        }
        /* Now write the columns */
 
        sprintf(cPoiseGrid,"%s.%s.Climate",system->cName,body[iBody].cName);
   
        if (control->Evolve.dTime == 0 && iLat == 0) {
          WriteDailyInsol(body,control,&output[iOut],system,&control->Units[iBody],update,iBody,dTmp,cUnit);
          WriteSeasonalTemp(body,control,&output[iOut],system,&control->Units[iBody],update,iBody,dTmp,cUnit);
                    WriteSeasonalIceBalance(body,control,&output[iOut],system,&control->Units[iBody],update,iBody,dTmp,cUnit);

          fp = fopen(cPoiseGrid,"w");     
        } else {
          fp = fopen(cPoiseGrid,"a");
        }
     
        for (iGrid=0;iGrid<files->Outfile[iBody].iNumGrid+iExtra;iGrid++) {
          fprintd(fp,dGrid[iGrid],control->Io.iSciNot,control->Io.iDigits);
          fprintf(fp," ");
        }
        fprintf(fp,"\n");
        fclose(fp);
      }
      free(dTmp);
    }
  } 
  
  if (control->bOutputLapl) {
      for (iBody=1;iBody<(control->Evolve.iNumBodies-1);iBody++) {
        if (body[iBody].bDistOrb && body[iBody].bEqtide) {
          if (control->Evolve.iDistOrbModel == RD4) {
            /* open body file to write laplace functions and related */
            for (jBody=iBody+1;jBody<control->Evolve.iNumBodies;jBody++) {
              sprintf(cLaplaceFunc,"%s.%s.Laplace",body[iBody].cName,body[jBody].cName);
              if (control->Evolve.dTime == 0) {
                fp = fopen(cLaplaceFunc,"w");
              } else {
                fp = fopen(cLaplaceFunc,"a");
              }
              if (body[iBody].dSemi < body[jBody].dSemi) {
                for (j=0;j<LAPLNUM;j++) {       
                  /* output alpha, laplace func, derivatives for each internal/external pair. 
                  external/internal pairs are duplicates and so not output. this can create a 
                  large amount of data for systems with lots of planets (78 columns/planet pair) */
                  fprintd(fp,system->dmAlpha0[system->imLaplaceN[iBody][jBody]][j], control->Io.iSciNot,control->Io.iDigits); //output alpha
                  fprintf(fp," ");
                 
                  fprintd(fp,system->dmLaplaceC[system->imLaplaceN[iBody][jBody]][j], control->Io.iSciNot,control->Io.iDigits); //output LaplaceC
                  fprintf(fp," ");
              
                  fprintd(fp,system->dmLaplaceD[system->imLaplaceN[iBody][jBody]][j], control->Io.iSciNot,control->Io.iDigits); //output LaplaceD
                  fprintf(fp," ");
                }
              }
              fprintf(fp,"\n");
              fclose(fp);
            }
          }   
        } else if (body[iBody].bDistOrb) {
          if (control->Evolve.dTime == 0) {
            if (control->Evolve.iDistOrbModel == RD4) {
              /* open body file to write laplace functions and related */
              for (jBody=iBody+1;jBody<control->Evolve.iNumBodies;jBody++) {
                sprintf(cLaplaceFunc,"%s.%s.Laplace",body[iBody].cName,body[jBody].cName);
                fp = fopen(cLaplaceFunc,"w");
                if (body[iBody].dSemi < body[jBody].dSemi) {
                  for (j=0;j<LAPLNUM;j++) {       
                    /* output alpha, laplace func, derivatives for each internal/external pair. 
                    external/internal pairs are duplicates and so not output. this can create a 
                    large amount of data for systems with lots of planets (78 columns/planet pair) */
                    fprintd(fp,system->dmAlpha0[system->imLaplaceN[iBody][jBody]][j], control->Io.iSciNot,control->Io.iDigits); //output alpha
                    fprintf(fp," ");
                 
                    fprintd(fp,system->dmLaplaceC[system->imLaplaceN[iBody][jBody]][j], control->Io.iSciNot,control->Io.iDigits); //output LaplaceC
                    fprintf(fp," ");
              
                    fprintd(fp,system->dmLaplaceD[system->imLaplaceN[iBody][jBody]][j], control->Io.iSciNot,control->Io.iDigits); //output LaplaceD
                    fprintf(fp," ");
                  }
                }
                fprintf(fp,"\n");
                fclose(fp);
              }
            }
          }
        }
      }
  }
}

void InitializeOutput(OUTPUT *output,fnWriteOutput fnWrite[]) {
  int iOut,iBody,iModule;

  for (iOut=0;iOut<MODULEOUTEND;iOut++) {
    sprintf(output[iOut].cName,"null");
    output[iOut].bGrid = 0;
    output[iOut].bNeg = 0; /* Is a negative option allowed */
    output[iOut].dNeg = 1; /* Conversion factor for negative options */
    output[iOut].iNum = 0; /* Number of parameters associated with option */
    output[iOut].bDoNeg = malloc(MAXBODIES*sizeof(int));
    for (iBody=0;iBody<MAXBODIES;iBody++)
        output[iOut].bDoNeg[iBody] = 0;
  }

  
  InitializeOutputGeneral(output,fnWrite);

  /* How can I do this without the developer needing to modify output.c??? I want the code to know about all possible output options so it can flag ouput parameters that do not apply to any uploaded module. XXXX*/

  /*
  for (iBody=0;iBody<control->Evolve.iNumBodies;iBody++) {
    output[iOut].bDoNeg[iBody] = 0;
    Initialize modules 
    for (iModule=0;iModule<module->iNumModules[iBody];iModule++)
      module->fnInitializeOutput[iBody][iModule](output,fnWrite);
  }
  */

  /************************
   * ADD NEW MODULES HERE *
   ************************/

  InitializeOutputEqtide(output,fnWrite);
  InitializeOutputRadheat(output,fnWrite);
  InitializeOutputAtmEsc(output,fnWrite);
  InitializeOutputStellar(output,fnWrite);
  InitializeOutputDistOrb(output,fnWrite);
  InitializeOutputDistRot(output,fnWrite);
  InitializeOutputThermint(output,fnWrite);
  InitializeOutputPoise(output,fnWrite);

}



