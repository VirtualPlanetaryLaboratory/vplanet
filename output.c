
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


void WriteAge(CONTROL *control,UNITS *units,OUTPUT output,BODY *body,SYSTEM *system,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  *dTmp = system->dAge;
  if (output.bDoNeg[iBody]) {
    *dTmp *= output.dNeg;
    strcpy(cUnit,output.cNeg);
  } else {
    *dTmp /= fdTimeUnit(units->iTime);
    fvTimeUnit(units->iTime,cUnit);
  }
}



/*
 * D
 */

void WriteDeltaTime(CONTROL *control,UNITS *units,OUTPUT output,BODY *body,SYSTEM *system,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {

  if (control->Integr.dTime > 0) 
    *dTmp = control->dOutputTime/control->Integr.nSteps;
  else
    *dTmp = 0;
  if (output.bDoNeg[iBody]) {
    *dTmp *= output.dNeg;
    strcpy(cUnit,output.cNeg);
  } else {
    *dTmp /= fdTimeUnit(units->iTime);
    fvTimeUnit(units->iTime,cUnit);
  }
}




/*
 * M
 */

void WriteMass(CONTROL *control,UNITS *units,OUTPUT output,BODY *body,SYSTEM *system,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {

  *dTmp = body[iBody].dMass;
  if (output.bDoNeg[iBody]) {
    *dTmp *= output.dNeg;
    strcpy(cUnit,output.cNeg);
  } else {
    *dTmp /= fdMassUnit(units->iMass);
    fvMassUnit(units->iMass,cUnit);
  }
}

/*
 * O
 */

void WriteObliquity(CONTROL *control,UNITS *units,OUTPUT output,BODY *body,SYSTEM *system,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {

  *dTmp = body[iBody].dObliquity;
  if (output.bDoNeg[iBody]) {
    *dTmp *= output.dNeg;
    strcpy(cUnit,output.cNeg);
  } else {
    *dTmp /= fdAngleUnit(units->iAngle);
    fvAngleUnit(units->iAngle,cUnit);
  }
}


void WriteOrbAngMom(CONTROL *control,UNITS *units,OUTPUT output,BODY *body,SYSTEM *system,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  char cTmp;

  *dTmp = fdOrbAngMom(body);
  if (output.bDoNeg[iBody]) {
    *dTmp *= output.dNeg;
    strcpy(cUnit,output.cNeg);
  } else {
    *dTmp *= fdTimeUnit(units->iTime)/(fdMassUnit(units->iMass)*fdLengthUnit(units->iLength)*fdLengthUnit(units->iLength));
    fvAngMomUnit(units,cUnit);
  }
}

void WriteOrbEcc(CONTROL *control,UNITS *units,OUTPUT output,BODY *body,SYSTEM *system,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  *dTmp = body[1].dEcc;
  sprintf(cUnit,"");
}

void WriteOrbEnergy(CONTROL *control,UNITS *units,OUTPUT output,BODY *body,SYSTEM *system,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {

  *dTmp = fdOrbEnergy(body,iBody);
  if (output.bDoNeg[iBody]) {
    *dTmp *= output.dNeg;
    strcpy(cUnit,output.cNeg);
  } else {
    *dTmp /= fdEnergyUnit(units->iTime,units->iMass,units->iLength);
    fvEnergyUnit(units,cUnit);
  }
}

void WriteOrbMeanMotion(CONTROL *control,UNITS *units,OUTPUT output,BODY *body,SYSTEM *system,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  *dTmp = body[1].dMeanMotion;
  if (output.bDoNeg[iBody]) {
    *dTmp *= output.dNeg;
    strcpy(cUnit,output.cNeg);
  } else {
    *dTmp /= fdTimeUnit(units->iTime);
    fvRateUnit(units->iTime,cUnit);
  }
}

void WriteOrbPeriod(CONTROL *control,UNITS *units,OUTPUT output,BODY *body,SYSTEM *system,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {

  *dTmp = fdSemiToPeriod(body[1].dSemi,(body[0].dMass+body[1].dMass));
  if (output.bDoNeg[iBody]) {
    *dTmp *= output.dNeg;
    strcpy(cUnit,output.cNeg);
  } else {
    *dTmp *= fdTimeUnit(units->iTime);
    fvTimeUnit(units->iTime,cUnit);
  }
}

void WriteOrbSemi(CONTROL *control,UNITS *units,OUTPUT output,BODY *body,SYSTEM *system,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  *dTmp = body[1].dSemi;
  if (output.bDoNeg[iBody]) {
    *dTmp *= output.dNeg;
    strcpy(cUnit,output.cNeg);
  } else {
    *dTmp /= fdLengthUnit(units->iLength);
    fvLengthUnit(units->iLength,cUnit);
  }
}

/*
 * R
 */

void WriteRadius(CONTROL *control,UNITS *units,OUTPUT output,BODY *body,SYSTEM *system,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {

  *dTmp = body[iBody].dRadius;
  if (output.bDoNeg[iBody]) {
    *dTmp *= output.dNeg;
    strcpy(cUnit,output.cNeg);
  } else {
    *dTmp /= fdLengthUnit(units->iLength);
    fvLengthUnit(units->iLength,cUnit);
  }
}

void WriteRotAngMom(CONTROL *control,UNITS *units,OUTPUT output,BODY *body,SYSTEM *system,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {

  *dTmp = fdRotAngMom(body[iBody].dRadGyra,body[iBody].dMass,body[iBody].dRadius,body[iBody].dRotRate);

  if (output.bDoNeg[iBody]) {
    *dTmp *= output.dNeg;
    strcpy(cUnit,output.cNeg);
  } else {
    *dTmp *= fdTimeUnit(units->iTime)/(fdMassUnit(units->iMass)*fdLengthUnit(units->iLength)*fdLengthUnit(units->iLength));
    fvAngMomUnit(units,cUnit);
  }
}

void WriteRotKinEnergy(CONTROL *control,UNITS *units,OUTPUT output,BODY *body,SYSTEM *system,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {

  *dTmp = fdRotKinEnergy(body[iBody].dMass,body[iBody].dRadius,body[iBody].dRadGyra,body[iBody].dRotRate);
  if (output.bDoNeg[iBody]) {
    *dTmp *= output.dNeg;
    strcpy(cUnit,output.cNeg);
  } else {
    *dTmp /= fdEnergyUnit(units->iTime,units->iMass,units->iLength);
    fvEnergyUnit(units,cUnit);
  }
}

void WriteRotRate(CONTROL *control,UNITS *units,OUTPUT output,BODY *body,SYSTEM *system,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {

  *dTmp = body[iBody].dRotRate;
  if (output.bDoNeg[iBody]) {
    *dTmp *= output.dNeg;
    strcpy(cUnit,output.cNeg);
  } else {
    *dTmp *= fdTimeUnit(units->iTime);
    fvRateUnit(units->iTime,cUnit);
  }
}

void WriteRotPer(CONTROL *control,UNITS *units,OUTPUT output,BODY *body,SYSTEM *system,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {

  *dTmp = fdFreqToPer(body[iBody].dRotRate);
  if (output.bDoNeg[iBody]) {
    *dTmp *= output.dNeg;
    strcpy(cUnit,output.cNeg);
  } else {
    *dTmp /= fdTimeUnit(units->iTime);
    fvTimeUnit(units->iTime,cUnit);
  }
}


void WriteRotVel(CONTROL *control,UNITS *units,OUTPUT output,BODY *body,SYSTEM *system,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {

  *dTmp = fdRotVel(body[iBody].dRadius,body[iBody].dRotRate);
  if (output.bDoNeg[iBody]) {
    *dTmp *= output.dNeg;
    strcpy(cUnit,output.cNeg);
  } else {
    *dTmp *= fdTimeUnit(units->iTime)/fdLengthUnit(units->iLength);
    fvVelUnit(units,cUnit);
  }
}



void WriteTime(CONTROL *control,UNITS *units,OUTPUT output,BODY *body,SYSTEM *system,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {

  *dTmp = control->Integr.dTime;
  if (output.bDoNeg[iBody]) {
    *dTmp *= output.dNeg;
    strcpy(cUnit,output.cNeg);
  } else {
    *dTmp /= fdTimeUnit(units->iTime);
    fvTimeUnit(units->iTime,cUnit);
  }
}

void WriteTotAngMom(CONTROL *control,UNITS *units,OUTPUT output,BODY *body,SYSTEM *system,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {

  *dTmp = fdTotAngMom(body);

  if (output.bDoNeg[iBody]) {
    *dTmp *= output.dNeg;
    strcpy(cUnit,output.cNeg);
  } else {
    *dTmp *= fdTimeUnit(units->iTime)/(fdMassUnit(units->iMass)*fdLengthUnit(units->iLength)*fdLengthUnit(units->iLength));
    fvAngMomUnit(units,cUnit);
  }
}

void WriteTotEnergy(CONTROL *control,UNITS *units,OUTPUT output,BODY *body,SYSTEM *system,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {

  *dTmp = fdTotEnergy(body);

  if (output.bDoNeg[iBody]) {
    *dTmp *= output.dNeg;
    strcpy(cUnit,output.cNeg);
  } else {
    *dTmp /= fdEnergyUnit(units->iTime,units->iMass,units->iLength);
    fvEnergyUnit(units,cUnit);
  }
}

void WritePotEnergy(CONTROL *control,UNITS *units,OUTPUT output,BODY *body,SYSTEM *system,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {

  *dTmp = fdPotEnergy(body);

  if (output.bDoNeg[iBody]) {
    *dTmp *= output.dNeg;
    strcpy(cUnit,output.cNeg);
  } else {
    *dTmp /= fdEnergyUnit(units->iTime,units->iMass,units->iLength);
    fvEnergyUnit(units,cUnit);
  }
}

void WriteKinEnergy(CONTROL *control,UNITS *units,OUTPUT output,BODY *body,SYSTEM *system,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {

  *dTmp = fdKinEnergy(body);

  if (output.bDoNeg[iBody]) {
    *dTmp *= output.dNeg;
    strcpy(cUnit,output.cNeg);
  } else {
    *dTmp /= fdEnergyUnit(units->iTime,units->iMass,units->iLength);
    fvEnergyUnit(units,cUnit);
  }
}

void WriteOrbKinEnergy(CONTROL *control,UNITS *units,OUTPUT output,BODY *body,SYSTEM *system,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {

  *dTmp = fdOrbKinEnergy(body[0].dMass,body[1].dMass,body[1].dSemi);

  if (output.bDoNeg[iBody]) {
    *dTmp *= output.dNeg;
    strcpy(cUnit,output.cNeg);
  } else {
    *dTmp /= fdEnergyUnit(units->iTime,units->iMass,units->iLength);
    fvEnergyUnit(units,cUnit);
  }
}

void WriteOrbPotEnergy(CONTROL *control,UNITS *units,OUTPUT output,BODY *body,SYSTEM *system,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {

  *dTmp = fdOrbPotEnergy(body[0].dMass,body[1].dMass,body[1].dSemi);

  if (output.bDoNeg[iBody]) {
    *dTmp *= output.dNeg;
    strcpy(cUnit,output.cNeg);
  } else {
    *dTmp /= fdEnergyUnit(units->iTime,units->iMass,units->iLength);
    fvEnergyUnit(units,cUnit);
  }
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
     * M
     */
    
    sprintf(output[OUT_MASS].cName,"Mass");
    sprintf(output[OUT_MASS].cDescr,"Mass");
    output[OUT_MASS].bNeg = 0;
    output[OUT_MASS].iNum = 1;
    fnWrite[OUT_MASS] = &WriteMass;


    /*
     *   O
     */

    sprintf(output[OUT_OBL].cName,"Obliquity");
    sprintf(output[OUT_OBL].cDescr,"Obliquity");
    output[OUT_OBL].bNeg = 0;
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

    sprintf(output[OUT_TIME].cName,"Time");
    sprintf(output[OUT_TIME].cDescr,"Simulation Time");
    output[OUT_TIME].bNeg = 1;
    output[OUT_TIME].dNeg = 1./(YEARSEC*1e9);
    output[OUT_TIME].iNum = 1;
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

}

void CGSUnits(UNITS *units) {
  units->iTime = 0;
  units->iLength = 0;
  units->iMass = 0;
  units->iAngle = 0;
}

void WriteLogEntry(CONTROL *control,OUTPUT output,BODY *body,SYSTEM *system,fnWriteOutput fnWrite,FILE *fp,UPDATE *update,int iBody) {
  double *dTmp;
  char cUnit[48];
  UNITS units;
  int i,j,iNumIndices;


  cUnit[0]='\0';
  dTmp=malloc(output.iNum*sizeof(double));
  CGSUnits(&units);
  fnWrite(control,&units,output,body,system,update,iBody,dTmp,cUnit);

  fprintf(fp,"(%s) %s [%s]: ",output.cName,output.cDescr,cUnit);
  for (j=0;j<output.iNum;j++) {
    fprintd(fp,dTmp[j],control->iSciNot,control->iDigits);
    fprintf(fp," ");
  }
  fprintf(fp,"\n");
}

void LogUnits(FILE *fp) {
  /* Mass Units */
  UNITS units;

  CGSUnits(&units);

  fprintf(fp,"Mass Units: ");
  if (units.iMass == 0) {
    fprintf(fp,"Grams\n");
  } else if (units.iMass == 1) {
    fprintf(fp,"Kilograms\n");
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
    fprintf(fp,"Centimeters\n");
  } else if (units.iLength == 1) {
    fprintf(fp,"Meters\n");
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
  if (control->iOneStep == EULER)
    fprintf(fp,"Euler");
  else if (control->iOneStep == RUNGEKUTTA)
    fprintf(fp,"Runge-Kutta4");
  fprintf(fp,"\n");

  fprintf(fp,"Direction: ");
  if (control->Integr.bDoBackward) 
    fprintf(fp,"Backward\n");
  else
    fprintf(fp,"Forward\n");

  fprintf(fp,"Time Step: ");
  fprintd(fp,control->Integr.dTimeStep,control->iSciNot,control->iDigits);
  fprintf(fp,"\n");
  
  fprintf(fp,"Stop Time: ");
  fprintd(fp,control->Integr.dStopTime,control->iSciNot,control->iDigits);
  fprintf(fp,"\n");
  
  fprintf(fp,"Output Interval: ");
  fprintd(fp,control->dOutputTime,control->iSciNot,control->iDigits);
  fprintf(fp,"\n");
  
  fprintf(fp,"Use Variable Timestep: ");
  if (control->Integr.bVarDt == 0) {
    fprintf(fp,"No\n");
  } else {
    fprintf(fp,"Yes\n");
    fprintf(fp,"dEta: ");
    fprintd(fp,control->Integr.dEta,control->iSciNot,control->iDigits);
    fprintf(fp,"\n");
  }
}

void LogHalt(CONTROL *control,FILE *fp) {

  fprintf(fp,"Minimum Value of ecc and obl: ");
  fprintd(fp,control->Integr.dMinValue,control->iSciNot,control->iDigits);
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

void LogOutputOrder(CONTROL *control,OUTPUT *output,BODY *body,SYSTEM *system,FILES *files,fnWriteOutput fnWrite[],FILE *fp,UPDATE *update,int iBody) {
  int i,j,k,iExtra=0;
  char cCol[NUMOUT][OPTLEN];
  double *dTmp;
  char cUnit[48],cTmp[48];
  
  for (i=0;i<files->Outfile[iBody].iNumCols;i++) {
    for (j=0;j<NUMOUT;j++) {
      if (memcmp(files->Outfile[iBody].caCol[i],output[j].cName,strlen(output[j].cName)) == 0) {
 	/* Match! */
	dTmp=malloc(output[j].iNum*sizeof(double));
	fnWrite[j](control,&control->Units[iBody],output[j],body,system,update,iBody,dTmp,cUnit);
	for (k=0;k<output[j].iNum;k++) {
	  strcpy(cCol[i+k+iExtra],files->Outfile[iBody].caCol[i]);
	  sprintf(cTmp,"[%s]",cUnit);
	  strcat(cCol[i+k+iExtra],cTmp);
	}
	iExtra += (output[j].iNum-1);
      }
      
    }
  }

  fprintf(fp,"Output Order:");
  for (i=0;i<(files->Outfile[iBody].iNumCols + iExtra);i++)
    fprintf(fp," %s",cCol[i]);
  fprintf(fp, "\n");
}

void LogOptions(CONTROL *control,FILES *files,SYSTEM *system,FILE *fp) {
  int iFile;

  fprintf(fp,"-------- Log file %s -------\n\n",files->cLog);
  fprintf(fp,"Executable: %s\n",files->cExe);
  
  fprintf(fp,"System Name: %s\n",system->cName);
  fprintf(fp,"Primary Input File: %s\n",files->Infile[0].cIn);
  for (iFile=1;iFile<files->iNumInputs;iFile++) 
    fprintf(fp,"Body File #%d: %s\n",iFile,files->Infile[iFile].cIn);
  fprintf(fp,"Allow files to be overwitten: ");
  if (control->bOverwrite)
    fprintf(fp,"Yes");
  else
    fprintf(fp,"No");
  fprintf(fp,"\n");
  
  /* Needs to be vectorized VPL */
  LogOptionsEqtide(control,fp);
  
  LogUnits(fp);

  fprintf(fp,"\n------- FORMATTING -----\n");
  fprintf(fp,"Verbosity Level: %d\n",control->iVerbose);
  fprintf(fp,"Crossover Decade for Scientific Notation: %d\n",control->iSciNot);
  fprintf(fp,"Number of Digits After Decimal: %d\n",control->iDigits);
  
  if (control->Integr.bDoForward || control->Integr.bDoBackward) {
    LogIntegration(control,fp);
    LogHalt(control,fp);
  }
} 

void LogSystem(CONTROL *control,OUTPUT *output,BODY *body,SYSTEM *system,UPDATE *update,fnWriteOutput fnWrite[],FILE *fp) {
  int iOpt;

  fprintf(fp,"SYSTEM PROPERTIES ----\n");

  for (iOpt=OUTSTART;iOpt<OUTBODYSTART;iOpt++) {
    if (output[iOpt].iNum > 0) 
      WriteLogEntry(control,output[iOpt],body,system,fnWrite[iOpt],fp,update,0);
  }

  /* Needs to be vectorized VPL */
  LogEqtide(control,output,body,system,update,fnWrite,fp);

}

void LogBody(CONTROL *control,OUTPUT *output,BODY *body,SYSTEM *system,FILES *files,fnWriteOutput fnWrite[],FILE *fp,UPDATE *update) {
  int iBody,iOut;

  for (iBody=0;iBody<control->iNumBodies;iBody++) {
    fprintf(fp,"\n----- BODY: %s ----\n",body[iBody].cName);
    for (iOut=OUTBODYSTART;iOut<OUTEND;iOut++) {
      LogBodyRelations(control,fp,iBody);
      if (output[iOut].iNum > 0) 
	WriteLogEntry(control,output[iOut],body,system,fnWrite[iOut],fp,update,iBody);

    }
    /* Needs to be vectorized VPL */
    LogBodyEqtide(control,output,body,system,update,fnWrite,fp,iBody);
    
    LogOutputOrder(control,output,body,system,files,fnWrite,fp,update,iBody);
  }
}

void WriteLog(CONTROL *control,BODY *body,SYSTEM *system,OUTPUT *output,OPTIONS *options,FILES *files,fnWriteOutput fnWrite[],UPDATE *update,fnUpdateVariable ***fnUpdate,int iEnd) {
  char cTime[OPTLEN];
  FILE *fp;
  int i,j;
  double dDt;

  /* Get derivatives */
  /* Needs to be vectorized VPL */
  fvTidalProperties(control,body);

  dDt=fdGetUpdateInfo(control,body,update,fnUpdate);

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
    LogOptions(control,files,system,fp);

    fprintf(fp,"\n---- INITIAL ");
  } else 
    fprintf(fp,"\n\n\n---- FINAL ");

  /* System Properties */
  LogSystem(control,output,body,system,update,fnWrite,fp);
  
  /* Bodies' Properties */
  LogBody(control,output,body,system,files,fnWrite,fp,update);

  fclose(fp);
}

void WriteOutput(CONTROL *control,OUTPUT *output,BODY *body,SYSTEM *system,FILES *files,fnWriteOutput *fnWrite,double dTime,double dDt,UPDATE *update){
  int i,j,k,l,iExtra=0;
  double dCol[NUMOPT],*dTmp;
  FILE *fp;
  char cUnit[OPTLEN];

  /* Write out all data columns for each body. As some data may span more than
     1 column, we search the input list sequentially, adding iExtra to the 
     total number of columns as we go. The calls to fnWrite return the column
     value in the correct units, and output.iNum already contains the 
     number of columns. */

  /* First ensure that semi-major axis and eccentricity are propogated to both bodies, just in case. 
  body[0].dEcc=body[1].dEcc;
  body[0].dSemi=body[1].dSemi;
  */

  for (i=0;i<control->iNumBodies;i++) {
    for (j=0;j<files->Outfile[i].iNumCols;j++) {
      for (k=0;k<NUMOUT;k++) {
	if (memcmp(files->Outfile[i].caCol[j],output[k].cName,strlen(output[k].cName)-1) == 0) {
	  /* Match! */
	  dTmp=malloc(output[k].iNum*sizeof(double));
	  fnWrite[k](control,&control->Units[i],output[k],body,system,update,i,dTmp,cUnit);
	  for (l=0;l<output[k].iNum;l++)
	    dCol[j+l+iExtra]=dTmp[l];
	  iExtra += (output[k].iNum-1);
	}
      }
    }

    /* Now write the columns */
    fp = fopen(files->Outfile[i].cOut,"a");
    for (j=0;j<files->Outfile[i].iNumCols+iExtra;j++) {
      fprintd(fp,dCol[j],control->iSciNot,control->iDigits);
      fprintf(fp," ");
    }
    fprintf(fp,"\n");
    fclose(fp);
  }
}

void InitializeOutput(OUTPUT *output,fnWriteOutput fnWrite[]) {
  int iOut,iBody;

  for (iOut=0;iOut<NUMOUT;iOut++) {
    sprintf(output[iOut].cName,"null");
    output[iOut].bNeg = 0; /* Is a negative option allowed */
    output[iOut].dNeg = 1; /* Conversion factor for negative options */
    output[iOut].iNum = 0; /* Number of parameters associated with option */
    output[iOut].bDoNeg = malloc(2*sizeof(int));
    for (iBody=0;iBody<2;iBody++)
      output[iOut].bDoNeg[iBody] = 0;
  }

  InitializeOutputGeneral(output,fnWrite);

  /* Needs to be vectorized VPL */
  InitializeOutputEqtide(output,fnWrite);
}



