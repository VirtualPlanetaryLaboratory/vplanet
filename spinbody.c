/******************************** SpiNBody.c ***********************************
 *
 * Hayden Smotherman, Tue Feb 21 2017
 *
 * Subroutines that control the integration of the N Body simulation
 *
 ******************************************************************************/

#include <stdio.h>
#include <math.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "vplanet.h"
#include "options.h"
#include "output.h"


void BodyCopySpiNBody(BODY *dest,BODY *src,int iFoo,int iNumBodies,int iBody) {
  int iPert;
  dest[iBody].dVelX = src[iBody].dVelX;
  dest[iBody].dVelY = src[iBody].dVelY;
  dest[iBody].dVelZ = src[iBody].dVelZ;
  dest[iBody].dPositionX = src[iBody].dPositionX;
  dest[iBody].dPositionY = src[iBody].dPositionY;
  dest[iBody].dPositionZ = src[iBody].dPositionZ;

  dest[iBody].iGravPerts = src[iBody].iGravPerts;
  //dest[iBody].iaGravPerts = malloc(dest[iBody].iGravPerts*sizeof(int)); //Will create a memory leak. Replaced by InitializeUpdateTmpBodySpiNBody

  for (iPert=0;iPert<src[iBody].iGravPerts;iPert++){
    dest[iBody].iaGravPerts[iPert] = src[iBody].iaGravPerts[iPert];
    dest[iBody].dDistance3[iPert]  = src[iBody].dDistance3[iPert];
  }

}

void InitializeUpdateTmpBodySpiNBody(BODY *body,CONTROL *control,UPDATE *update,int iBody) {
  //This replaces malloc'ing the destination body in BodyCopySpiNBody
  control->Evolve.tmpBody[iBody].iaGravPerts = malloc(body[iBody].iGravPerts*sizeof(int));
  control->Evolve.tmpBody[iBody].dDistance3  = malloc(body[iBody].iGravPerts*sizeof(double));
}

//================================== Read Inputs ===============================
void ReadPositionX(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  /* This parameter cannot exist in primary file */
  int lTmp=-1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn,options->cName,&dTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile,options->cName,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);
    //if (dTmp < 0) //No Negative option right now
    //  body[iFile-1].dPositionX = dTmp*dNegativeDouble(*options,files->Infile[iFile].cIn,control->Io.iVerbose);
    //else
    body[iFile-1].dPositionX = dTmp;
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else
    if (iFile > 0)
      body[iFile-1].dPositionX = options->dDefault;
}

void ReadPositionY(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  /* This parameter cannot exist in primary file */
  int lTmp=-1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn,options->cName,&dTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile,options->cName,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);
    body[iFile-1].dPositionY = dTmp;
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else
    if (iFile > 0)
      body[iFile-1].dPositionY = options->dDefault;
}

void ReadPositionZ(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  /* This parameter cannot exist in primary file */
  int lTmp=-1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn,options->cName,&dTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile,options->cName,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);
    body[iFile-1].dPositionZ = dTmp;
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else
    if (iFile > 0)
      body[iFile-1].dPositionZ = options->dDefault;
}

void ReadVelX(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  /* This parameter cannot exist in primary file */
  int lTmp=-1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn,options->cName,&dTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile,options->cName,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);
    body[iFile-1].dVelX = dTmp;
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else
    if (iFile > 0)
      body[iFile-1].dVelX = options->dDefault;
}

void ReadVelY(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  /* This parameter cannot exist in primary file */
  int lTmp=-1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn,options->cName,&dTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile,options->cName,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);
    body[iFile-1].dVelY = dTmp;
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else
    if (iFile > 0)
      body[iFile-1].dVelY = options->dDefault;
}

void ReadVelZ(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  /* This parameter cannot exist in primary file */
  int lTmp=-1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn,options->cName,&dTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile,options->cName,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);
    body[iFile-1].dVelZ = dTmp;
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else
    if (iFile > 0)
      body[iFile-1].dVelZ = options->dDefault;
}

void ReadMeanA(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  /* This parameter cannot exist in the primary file */
  int lTmp=-1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn,options->cName,&dTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile,options->cName,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);
    if (control->Units[iFile].iAngle == 0) {
      if (dTmp < 0 || dTmp > 2*PI) {
        if (control->Io.iVerbose >= VERBERR)
            fprintf(stderr,"ERROR: %s must be in the range [0,2*PI].\n",options->cName);
        LineExit(files->Infile[iFile].cIn,lTmp);
      }
    } else {
      if (dTmp < 0 || dTmp > 360) {
        if (control->Io.iVerbose >= VERBERR)
            fprintf(stderr,"ERROR: %s must be in the range [0,360].\n",options->cName);
        LineExit(files->Infile[iFile].cIn,lTmp);
      }
      /* Change to radians */
      dTmp *= DEGRAD;
    }

    body[iFile-1].dMeanA = dTmp;
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else
    if (iFile > 0)
      body[iFile-1].dMeanA = options->dDefault;
}

void ReadUseOrbParams(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  /* This parameter cannot exist in primary file */
  int lTmp=-1;
  int bTmp;

  AddOptionBool(files->Infile[iFile].cIn,options->cName,&bTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile,options->cName,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);
    body[iFile-1].bUseOrbParams = bTmp;
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  }
  else {
    body[iFile-1].bUseOrbParams = options->dDefault;
  }
}

void InitializeOptionsSpiNBody(OPTIONS *options,fnReadOption fnRead[]) {
  int iOpt,iFile;

  sprintf(options[OPT_POSITIONXSPINBODY].cName,"dPositionXSpiNBody");
  sprintf(options[OPT_POSITIONXSPINBODY].cDescr,"X position of the body");
  sprintf(options[OPT_POSITIONXSPINBODY].cDefault,"0");
  options[OPT_POSITIONXSPINBODY].dDefault = 0.0;
  options[OPT_POSITIONXSPINBODY].iType = 2;
  options[OPT_POSITIONXSPINBODY].iMultiFile = 1;
  fnRead[OPT_POSITIONXSPINBODY] = &ReadPositionX;

  sprintf(options[OPT_POSITIONYSPINBODY].cName,"dPositionYSpiNBody");
  sprintf(options[OPT_POSITIONYSPINBODY].cDescr,"Y position of the body");
  sprintf(options[OPT_POSITIONYSPINBODY].cDefault,"0");
  options[OPT_POSITIONYSPINBODY].dDefault = 0.0;
  options[OPT_POSITIONYSPINBODY].iType = 2;
  options[OPT_POSITIONYSPINBODY].iMultiFile = 1;
  fnRead[OPT_POSITIONYSPINBODY] = &ReadPositionY;

  sprintf(options[OPT_POSITIONZSPINBODY].cName,"dPositionZSpiNBody");
  sprintf(options[OPT_POSITIONZSPINBODY].cDescr,"Z position of the body");
  sprintf(options[OPT_POSITIONZSPINBODY].cDefault,"0");
  options[OPT_POSITIONZSPINBODY].dDefault = 0.0;
  options[OPT_POSITIONZSPINBODY].iType = 2;
  options[OPT_POSITIONZSPINBODY].iMultiFile = 1;
  fnRead[OPT_POSITIONZSPINBODY] = &ReadPositionZ;

  sprintf(options[OPT_VELXSPINBODY].cName,"dVelXSpiNBody");
  sprintf(options[OPT_VELXSPINBODY].cDescr,"X velocity of the body");
  sprintf(options[OPT_VELXSPINBODY].cDefault,"0");
  options[OPT_VELXSPINBODY].dDefault = 0.0;
  options[OPT_VELXSPINBODY].iType = 2;
  options[OPT_VELXSPINBODY].iMultiFile = 1;
  fnRead[OPT_VELXSPINBODY] = &ReadVelX;

  sprintf(options[OPT_VELYSPINBODY].cName,"dVelYSpiNBody");
  sprintf(options[OPT_VELYSPINBODY].cDescr,"Y velocity of the body");
  sprintf(options[OPT_VELYSPINBODY].cDefault,"0");
  options[OPT_VELYSPINBODY].dDefault = 0.0;
  options[OPT_VELYSPINBODY].iType = 2;
  options[OPT_VELYSPINBODY].iMultiFile = 1;
  fnRead[OPT_VELYSPINBODY] = &ReadVelY;

  sprintf(options[OPT_VELZSPINBODY].cName,"dVelZSpiNBody");
  sprintf(options[OPT_VELZSPINBODY].cDescr,"Z velocity of the body");
  sprintf(options[OPT_VELZSPINBODY].cDefault,"0");
  options[OPT_VELZSPINBODY].dDefault = 0.0;
  options[OPT_VELZSPINBODY].iType = 2;
  options[OPT_VELZSPINBODY].iMultiFile = 1;
  fnRead[OPT_VELZSPINBODY] = &ReadVelZ;

  sprintf(options[OPT_MEANA].cName,"dMeanA");
  sprintf(options[OPT_MEANA].cDescr,"Mean anomaly");
  sprintf(options[OPT_MEANA].cDefault,"0.0");
  options[OPT_MEANA].dDefault = 0.0;
  options[OPT_MEANA].iType = 2;
  options[OPT_MEANA].iMultiFile = 1;
  fnRead[OPT_MEANA] = &ReadMeanA;

  sprintf(options[OPT_USEORBPARAMS].cName,"bUseOrbParams");
  sprintf(options[OPT_USEORBPARAMS].cDescr,"Flag to use orbital parameters as inputs");
  sprintf(options[OPT_USEORBPARAMS].cDefault,"0");
  options[OPT_USEORBPARAMS].dDefault = 0;
  options[OPT_USEORBPARAMS].iType = 0;
  options[OPT_USEORBPARAMS].iMultiFile = 1;
  fnRead[OPT_USEORBPARAMS] = &ReadUseOrbParams;
}

void ReadOptionsSpiNBody(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,fnReadOption fnRead[],int iBody) {
  int iOpt;

  for (iOpt=OPTSTARTSPINBODY;iOpt<OPTENDSPINBODY;iOpt++) {
    if (options[iOpt].iType != -1)
      fnRead[iOpt](body,control,files,&options[iOpt],system,iBody+1);
  }
}

//============================ End Read Inputs =================================

void InitializeBodySpiNBody(BODY *body,CONTROL *control,UPDATE *update,int iBody,int iModule) {
  if (body[iBody].bSpiNBody){
    body[iBody].iGravPerts = control->Evolve.iNumBodies-1; //All bodies except the body itself are perturbers
    body[iBody].iaGravPerts = malloc(body[iBody].iGravPerts*sizeof(int));
    body[iBody].dDistance3 = malloc(body[iBody].iGravPerts*sizeof(double));


    //If orbital parameters are defined, then we want to set position and velocity based on those
    if (body[iBody].bUseOrbParams){
       if (iBody == 0){ //Only want to do this once
         for (int iTmpBody = 0; iTmpBody<control->Evolve.iNumBodies; iTmpBody++){
           body[iTmpBody].dCartPos = malloc(3*sizeof(double));
           body[iTmpBody].dCartVel = malloc(3*sizeof(double));
           //Convert all bodies w/ orbital elements to Heliocentric
           if (body[iTmpBody].bUseOrbParams){
             body[iTmpBody].dSinc = sin(0.5*body[iTmpBody].dInc);
             OrbElems2Helio(body, iTmpBody);
           }
         }
       }

       Helio2Bary(body, control->Evolve.iNumBodies, iBody);
       body[iBody].dPositionX = body[iBody].dCartPos[0]*AUCM;
       body[iBody].dPositionY = body[iBody].dCartPos[1]*AUCM;
       body[iBody].dPositionZ = body[iBody].dCartPos[2]*AUCM;
       body[iBody].dVelX      = body[iBody].dCartVel[0]*AUCM/DAYSEC;
       body[iBody].dVelY      = body[iBody].dCartVel[1]*AUCM/DAYSEC;
       body[iBody].dVelZ      = body[iBody].dCartVel[2]*AUCM/DAYSEC;
    }

  } //elseif (body[iBody].bDistOrb == 0){
  //  body[iBody].iGravPerts = 0; //Is this already set to zero somewhere?
  //}
}

void InitializeUpdateSpiNBody(BODY *body,UPDATE *update,int iBody) {
  if (iBody >= 0) {
    if (update[iBody].iNumPositionX == 0)
      update[iBody].iNumVars++;
    update[iBody].iNumPositionX++;

    if (update[iBody].iNumPositionY == 0)
      update[iBody].iNumVars++;
    update[iBody].iNumPositionY++;

    if (update[iBody].iNumPositionZ == 0)
      update[iBody].iNumVars++;
    update[iBody].iNumPositionZ++;

    if (update[iBody].iNumVelX == 0)
      update[iBody].iNumVars++;
    update[iBody].iNumVelX++;

    if (update[iBody].iNumVelY == 0)
      update[iBody].iNumVars++;
    update[iBody].iNumVelY++;

    if (update[iBody].iNumVelZ == 0)
      update[iBody].iNumVars++;
    update[iBody].iNumVelZ++;
  }
}

//======================== Verify Variable Functions ===========================
void VerifyPositionX(BODY *body,OPTIONS *options, UPDATE *update, double dAge, fnUpdateVariable ***fnUpdate, int iBody) {

  update[iBody].iaType[update[iBody].iPositionX][0] = 7;
  update[iBody].iNumBodies[update[iBody].iPositionX][0] = 1;
  update[iBody].iaBody[update[iBody].iPositionX][0] = malloc(update[iBody].iNumBodies[update[iBody].iPositionX][0]*sizeof(int));
  update[iBody].iaBody[update[iBody].iPositionX][0][0] = iBody;

  update[iBody].pdDPositionX = &update[iBody].daDerivProc[update[iBody].iPositionX][0];
  fnUpdate[iBody][update[iBody].iPositionX][0] = &fdDPositionXDt;
}

void VerifyPositionY(BODY *body,OPTIONS *options, UPDATE *update, double dAge, fnUpdateVariable ***fnUpdate, int iBody) {

  update[iBody].iaType[update[iBody].iPositionY][0] = 7;
  update[iBody].iNumBodies[update[iBody].iPositionY][0] = 1;
  update[iBody].iaBody[update[iBody].iPositionY][0] = malloc(update[iBody].iNumBodies[update[iBody].iPositionY][0]*sizeof(int));
  update[iBody].iaBody[update[iBody].iPositionY][0][0] = iBody;

  update[iBody].pdDPositionY = &update[iBody].daDerivProc[update[iBody].iPositionY][0];
  fnUpdate[iBody][update[iBody].iPositionY][0] = &fdDPositionYDt;
}

void VerifyPositionZ(BODY *body,OPTIONS *options, UPDATE *update, double dAge, fnUpdateVariable ***fnUpdate, int iBody) {

  update[iBody].iaType[update[iBody].iPositionZ][0] = 7;
  update[iBody].iNumBodies[update[iBody].iPositionZ][0] = 1;
  update[iBody].iaBody[update[iBody].iPositionZ][0] = malloc(update[iBody].iNumBodies[update[iBody].iPositionZ][0]*sizeof(int));
  update[iBody].iaBody[update[iBody].iPositionZ][0][0] = iBody;

  update[iBody].pdDPositionZ = &update[iBody].daDerivProc[update[iBody].iPositionZ][0];
  fnUpdate[iBody][update[iBody].iPositionZ][0] = &fdDPositionZDt;
}

void VerifyVelX(BODY *body,OPTIONS *options, UPDATE *update, double dAge, fnUpdateVariable ***fnUpdate, int iBody) {

  update[iBody].iaType[update[iBody].iVelX][0] = 7;
  update[iBody].iNumBodies[update[iBody].iVelX][0] = 1;
  update[iBody].iaBody[update[iBody].iVelX][0] = malloc(update[iBody].iNumBodies[update[iBody].iVelX][0]*sizeof(int));
  update[iBody].iaBody[update[iBody].iVelX][0][0] = iBody;

  update[iBody].pdDVelX = &update[iBody].daDerivProc[update[iBody].iVelX][0];
  fnUpdate[iBody][update[iBody].iVelX][0] = &fdDVelXDt;
}

void VerifyVelY(BODY *body,OPTIONS *options, UPDATE *update, double dAge, fnUpdateVariable ***fnUpdate, int iBody) {

  update[iBody].iaType[update[iBody].iVelY][0] = 7;
  update[iBody].iNumBodies[update[iBody].iVelY][0] = 1;
  update[iBody].iaBody[update[iBody].iVelY][0] = malloc(update[iBody].iNumBodies[update[iBody].iVelY][0]*sizeof(int));
  update[iBody].iaBody[update[iBody].iVelY][0][0] = iBody;

  update[iBody].pdDVelY = &update[iBody].daDerivProc[update[iBody].iVelY][0];
  fnUpdate[iBody][update[iBody].iVelY][0] = &fdDVelYDt;
}

void VerifyVelZ(BODY *body,OPTIONS *options, UPDATE *update, double dAge, fnUpdateVariable ***fnUpdate, int iBody) {

  update[iBody].iaType[update[iBody].iVelZ][0] = 7;
  update[iBody].iNumBodies[update[iBody].iVelZ][0] = 1;
  update[iBody].iaBody[update[iBody].iVelZ][0] = malloc(update[iBody].iNumBodies[update[iBody].iVelZ][0]*sizeof(int));
  update[iBody].iaBody[update[iBody].iVelZ][0][0] = iBody;

  update[iBody].pdDVelZ = &update[iBody].daDerivProc[update[iBody].iVelZ][0];
  fnUpdate[iBody][update[iBody].iVelZ][0] = &fdDVelZDt;
}

void VerifyPerturbersSpiNBody(BODY *body,int iNumBodies,int iBody) {
  int iPert=0, j;
//   body[iBody].iaGravPerts = malloc(body[iBody].iGravPerts*sizeof(int));
  for (j=0;j<iNumBodies;j++) {
    if (j != iBody) {
      if (body[j].bSpiNBody == 0) {
        fprintf(stderr,"ERROR: SpiNBody must be the called for all planets\n");
        exit(EXIT_INPUT);
      }
      body[iBody].iaGravPerts[iPert] = j;
      iPert++;
    }
  }
}

void VerifySpiNBody(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,OUTPUT *output,SYSTEM *system,UPDATE *update,fnUpdateVariable ***fnUpdate,int iBody,int iModule) {

  VerifyPerturbersSpiNBody(body,control->Evolve.iNumBodies,iBody);

  //VerifyVelX MUST run first, or else distance won't be calculated
  //Should add a check to see if it's been calculated?
  VerifyVelX(body,options,update,body[iBody].dAge,fnUpdate,iBody);
  VerifyVelY(body,options,update,body[iBody].dAge,fnUpdate,iBody);
  VerifyVelZ(body,options,update,body[iBody].dAge,fnUpdate,iBody);
  VerifyPositionX(body,options,update,body[iBody].dAge,fnUpdate,iBody);
  VerifyPositionY(body,options,update,body[iBody].dAge,fnUpdate,iBody);
  VerifyPositionZ(body,options,update,body[iBody].dAge,fnUpdate,iBody);

  control->fnForceBehavior[iBody][iModule] = &ForceBehaviorSpiNBody;
  control->fnPropsAux[iBody][iModule] = &PropertiesSpiNBody;
  control->Evolve.fnBodyCopy[iBody][iModule] = &BodyCopySpiNBody;
}

//========================== End Verify Functions ==============================

//========================== Coordinate Changes ================================
void OrbElems2Helio(BODY *body, int iBody) {
  double xtmp, ytmp, vxtmp, vytmp;

  if (iBody == 0) {
    body[iBody].dCartPos[0] = 0;
    body[iBody].dCartPos[1] = 0;
    body[iBody].dCartPos[2] = 0;

    body[iBody].dCartVel[0] = 0;
    body[iBody].dCartVel[1] = 0;
    body[iBody].dCartVel[2] = 0;
  } else {
    kepler_eqn(body, iBody);
    xtmp = xinit(body, iBody);
    ytmp = yinit(body, iBody);
    vxtmp = vxi(body, iBody);
    vytmp = vyi(body, iBody);

    body[iBody].dCartPos[0] = xtmp*(xangle1(body,iBody))+ytmp*(xangle2(body,iBody));
    body[iBody].dCartPos[1] = xtmp*(yangle1(body,iBody))+ytmp*(yangle2(body,iBody));
    body[iBody].dCartPos[2] = xtmp*(zangle1(body,iBody))+ytmp*(zangle2(body,iBody));

    body[iBody].dCartVel[0] = vxtmp*(xangle1(body,iBody))+vytmp*(xangle2(body,iBody));
    body[iBody].dCartVel[1] = vxtmp*(yangle1(body,iBody))+vytmp*(yangle2(body,iBody));
    body[iBody].dCartVel[2] = vxtmp*(zangle1(body,iBody))+vytmp*(zangle2(body,iBody));
  }
}

void Helio2Bary(BODY *body, int iNumBodies, int iBody) {
  int i, iTmpBody;
  double *xcom, *vcom, mtotal;
  xcom = malloc(3*sizeof(double));
  vcom = malloc(3*sizeof(double));
  mtotal = 0;
  for (iTmpBody=0;iTmpBody<iNumBodies;iTmpBody++) mtotal += body[iTmpBody].dMass;

  for (i=0;i<3;i++) {
    xcom[i] = 0;
    vcom[i] = 0;
    for (iTmpBody=1;iTmpBody<iNumBodies;iTmpBody++) {
      xcom[i] += (body[iTmpBody].dMass*body[iTmpBody].dCartPos[i]/mtotal);
      vcom[i] += (body[iTmpBody].dMass*body[iTmpBody].dCartVel[i]/mtotal);
    }
  }

  for (i=0;i<3;i++) {
    body[iBody].dCartPos[i] -= xcom[i];
    body[iBody].dCartVel[i] -= vcom[i];
  }

  free(xcom);
  free(vcom);
}

void Bary2Helio(BODY *body, int iBody){
  //Calculate heliocentric cartesian position
  body[iBody].dCartPos[0] = body[iBody].dPositionX - body[0].dPositionX;
  body[iBody].dCartPos[1] = body[iBody].dPositionY - body[0].dPositionY;
  body[iBody].dCartPos[2] = body[iBody].dPositionZ - body[0].dPositionZ;

  //Calculate heliocentric cartesian velocity
  body[iBody].dCartVel[0] = body[iBody].dVelX - body[0].dVelX;
  body[iBody].dCartVel[1] = body[iBody].dVelY - body[0].dVelY;
  body[iBody].dCartVel[2] = body[iBody].dVelZ - body[0].dVelZ;
}

void Bary2OrbElems(BODY *body, int iBody){
  double rsq, normr, vsq, mu, *h, hsq, normh, sinwf, coswf, sinfAngle,
         cosfAngle, rdot, sinw, cosw, f, cosE;

  h = malloc(3*sizeof(double));
  //First convert from Barycentric to heliocentric
  //Helio values are stored in body[iBody].dCartPos and body[iBody].dCartVel
  Bary2Helio(body, iBody);

  if (iBody==0) {
    body[iBody].dSemi = 0;
    body[iBody].dEcc  = 0;
    body[iBody].dInc  = 0;
    body[iBody].dLongA = 0;
    body[iBody].dLongP = 0;
    body[iBody].dMeanA = 0;
  } else {

    //Solve for various values that are used repeatedly
    //Solve for h = r X v
    cross(body[iBody].dCartPos, body[iBody].dCartVel, h);
    hsq = h[0]*h[0]+h[1]*h[1]+h[2]*h[2];                  // ||h||^2
    normh = sqrt(hsq);                                    // ||h||
    vsq = body[iBody].dCartVel[0]*body[iBody].dCartVel[0]   // ||v||^2
         +body[iBody].dCartVel[1]*body[iBody].dCartVel[1]
         +body[iBody].dCartVel[2]*body[iBody].dCartVel[2];
    rsq = body[iBody].dCartPos[0]*body[iBody].dCartPos[0]   // ||r||^2
         +body[iBody].dCartPos[1]*body[iBody].dCartPos[1]
         +body[iBody].dCartPos[2]*body[iBody].dCartPos[2];
    normr = sqrt(rsq);                                    // ||r||
    rdot = (body[iBody].dCartPos[0]*body[iBody].dCartVel[0]
           +body[iBody].dCartPos[1]*body[iBody].dCartVel[1]
           +body[iBody].dCartPos[2]*body[iBody].dCartVel[2])/normr;
    mu  = BIGG * (body[iBody].dMass + body[0].dMass);     // G(M+m)

    // Solve for semi-major axis
    body[iBody].dSemi = 1/(2/normr - vsq/mu);

    // Solve for eccentricity
    body[iBody].dEccSq = 1.0 - hsq/(mu*body[iBody].dSemi);
    body[iBody].dEcc   = sqrt(body[iBody].dEccSq);

    //Solve for inclination
    body[iBody].dInc = acos(h[2]/normh);
    body[iBody].dSinc = 0.5*sin(body[iBody].dInc); //For DistOrb usage

    //Solve for longitude of ascending node
    body[iBody].dLongA = atan2(h[0],-h[1]);
    if (body[iBody].dLongA<0){ //Make sure the signs are all right
      body[iBody].dLongA += 2.0*PI;
    }

    //Solve for w and f
    sinwf = body[iBody].dCartPos[2]/(normr*sin(body[iBody].dInc));
    coswf = (body[iBody].dCartPos[0]/normr+sin(body[iBody].dLongA)*sinwf
           *cos(body[iBody].dInc))/cos(body[iBody].dLongA);
    if (body[iBody].dEcc != 0) { //No true anomaly for circular orbits
      sinfAngle = body[iBody].dSemi*(1-body[iBody].dEccSq)*rdot
                 /(normh*body[iBody].dEcc);
      cosfAngle = (body[iBody].dSemi*(1-body[iBody].dEccSq)/normr-1)/body[iBody].dEcc;
      sinw = sinwf*cosfAngle - coswf*sinfAngle;
      cosw = sinwf*sinfAngle + coswf*cosfAngle;

      body[iBody].dArgP = atan2(sinw,cosw);
      body[iBody].dLongP = atan2(sinw,cosw) + body[iBody].dLongA;

      //Ensure all angles are in [0,2PI)
      if (body[iBody].dLongP >= 2.*PI) {
        body[iBody].dLongP -= 2.*PI;
      } else if (body[iBody].dLongP < 0.0) {
        body[iBody].dLongP += 2.*PI;
      }
      if (body[iBody].dArgP >= 2.*PI) {
        body[iBody].dArgP -= 2.*PI;
      } else if (body[iBody].dArgP < 0.0) {
        body[iBody].dArgP += 2.*PI;
      }

      f = atan2(sinfAngle, cosfAngle);
      if ( f>= 2.*PI) {
        f -= 2.*PI;
      } else if (f < 0.0) {
        f += 2.*PI;
      }

      //Calculate Mean anomaly
      cosE = (cosfAngle+body[iBody].dEcc / (1.0+body[iBody].dEcc*cosfAngle));
      if (f <= PI){
        body[iBody].dEccA = acos(cosE);
      } else {
        body[iBody].dEccA = 2.*PI - acos(cosE);
      }

      body[iBody].dMeanA = body[iBody].dEccA
                          - body[iBody].dEcc * sin(body[iBody].dEccA);
      if (body[iBody].dMeanA < 0) {
        body[iBody].dMeanA += 2.*PI;
      } else if (body[iBody].dMeanA >= 2.*PI){
        body[iBody].dMeanA -= 2.*PI;
      }
    }
  }
  free(h);
}
//Functions below are EXACTLY the same as in distorb.c, but needed in SpiNBody
//Should be relocated to system.c?

//
// void kepler_eqn(BODY *body, int iBody) { //Same as in distorb.c
//   double di1, di2, di3, fi, fi1, fi2, fi3;
//   if (body[iBody].dMeanA == 0) {
//     body[iBody].dEccA = 0;
//   } else {
//     body[iBody].dEccA = body[iBody].dMeanA + signf(sin(body[iBody].dMeanA))*0.85*body[iBody].dEcc;
//     di3 = 1.0;
//
//     while (di3 > 1e-15) {
//       fi = body[iBody].dEccA - body[iBody].dEcc*sin(body[iBody].dEccA) - body[iBody].dMeanA;
//       fi1 = 1.0 - body[iBody].dEcc*cos(body[iBody].dEccA);
//       fi2 = body[iBody].dEcc*sin(body[iBody].dEccA);
//       fi3 = body[iBody].dEcc*cos(body[iBody].dEccA);
//       di1 = -fi/fi1;
//       di2 = -fi/(fi1+0.5*di1*fi2);
//       di3 = -fi/(fi1+0.5*di2*fi2+1./6.*pow(di2,2)*fi3);
//       body[iBody].dEccA += di3;
//     }
//   }
// }
//
// double signf(double value) { //Same as in distorb.c
//   if (value > 0) return 1.;
//   if (value < 0) return -1.;
//   return 0;
// }
//
// double xinit(BODY *body, int iBody) {
//   return body[iBody].dSemi/AUCM * (cos(body[iBody].dEccA) - body[iBody].dEcc);
// }
//
// double yinit(BODY *body, int iBody) {
//   return body[iBody].dSemi/AUCM * sqrt(1.0-pow(body[iBody].dEcc,2)) * sin(body[iBody].dEccA);
// }
//
// double vxi(BODY *body, int iBody) {
//   double x, y, mu, n;
//   x = xinit(body, iBody);
//   y = yinit(body, iBody);
//   mu = pow(KGAUSS,2)*(body[0].dMass+body[iBody].dMass)/MSUN;
//   n = sqrt(mu/pow(body[iBody].dSemi/AUCM,3));
//   return -pow(body[iBody].dSemi/AUCM,2)*n*sin(body[iBody].dEccA)/sqrt(pow(x,2)+pow(y,2));
// }
//
// double vyi(BODY *body, int iBody) {
//   double x, y, mu, n, v;
//   x = xinit(body, iBody);
//   y = yinit(body, iBody);
//   mu = pow(KGAUSS,2)*(body[0].dMass+body[iBody].dMass)/MSUN;
//   n = sqrt(mu/pow(body[iBody].dSemi/AUCM,3));
//   v = pow(body[iBody].dSemi/AUCM,2)*n*sqrt((1.0-pow(body[iBody].dEcc,2))/(pow(x,2)+pow(y,2)))*cos(body[iBody].dEccA);
//   return v;
// }
//
// double xangle1(BODY *body, int iBody) {
//   return cos(body[iBody].dLongA)*cos(body[iBody].dLongP-body[iBody].dLongA) - sin(body[iBody].dLongA)*sin(body[iBody].dLongP-body[iBody].dLongA)*(1.0-2.*pow(body[iBody].dSinc,2));
// }
//
// double xangle2(BODY *body, int iBody) {
//   return -cos(body[iBody].dLongA)*sin(body[iBody].dLongP-body[iBody].dLongA) - sin(body[iBody].dLongA)*cos(body[iBody].dLongP-body[iBody].dLongA)*(1.0-2.*pow(body[iBody].dSinc,2));
// }
//
// double yangle1(BODY *body, int iBody) {
//   return sin(body[iBody].dLongA)*cos(body[iBody].dLongP-body[iBody].dLongA) + cos(body[iBody].dLongA)*sin(body[iBody].dLongP-body[iBody].dLongA)*(1.0-2.*pow(body[iBody].dSinc,2));
// }
//
// double yangle2(BODY *body, int iBody) {
//   return -sin(body[iBody].dLongA)*sin(body[iBody].dLongP-body[iBody].dLongA) + cos(body[iBody].dLongA)*cos(body[iBody].dLongP-body[iBody].dLongA)*(1.0-2.*pow(body[iBody].dSinc,2));
// }
//
// double zangle1(BODY *body, int iBody) {
//   return sin(body[iBody].dLongP-body[iBody].dLongA)*(2.*body[iBody].dSinc*sqrt(1.0-pow(body[iBody].dSinc,2)));
// }
//
// double zangle2(BODY *body, int iBody) {
//   return cos(body[iBody].dLongP-body[iBody].dLongA)*(2.*body[iBody].dSinc*sqrt(1.0-pow(body[iBody].dSinc,2)));
// }

//========================== End Coordinate Changes ============================

void CountHaltsSpiNBody(HALT *halt,int *iHalt) {
  //What halts do we want? Collision?
}

void VerifyHaltSpiNBody(BODY *body,CONTROL *control,OPTIONS *options,int iBody,int *iHalt) {

}

//============================ Writing Functions ===============================

void WritePositionX(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  *dTmp = body[iBody].dPositionX;
  strcpy(cUnit,"");
}

void WritePositionY(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  *dTmp = body[iBody].dPositionY;
  strcpy(cUnit,"");
}

void WritePositionZ(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  *dTmp = body[iBody].dPositionZ;
  strcpy(cUnit,"");
}

void WriteVelX(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  *dTmp = body[iBody].dVelX;
  strcpy(cUnit,"");
}

void WriteVelY(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  *dTmp = body[iBody].dVelY;
  strcpy(cUnit,"");
}

void WriteVelZ(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  *dTmp = body[iBody].dVelZ;
  strcpy(cUnit,"");
}

void InitializeOutputSpiNBody(OUTPUT *output,fnWriteOutput fnWrite[]) {
  //Output example for dPositionX variable
  sprintf(output[OUT_POSITIONXSPINBODY].cName,"PositionXSpiNBody");
  sprintf(output[OUT_POSITIONXSPINBODY].cDescr,"Body's x position in SpiNBody");
  sprintf(output[OUT_POSITIONXSPINBODY].cNeg," ");
  output[OUT_POSITIONXSPINBODY].bNeg = 0;
  //output[OUT_POSITIONXSPINBODY].dNeg = 1;
  output[OUT_POSITIONXSPINBODY].iNum = 1;
  output[OUT_POSITIONXSPINBODY].iModuleBit = SPINBODY;
  fnWrite[OUT_POSITIONXSPINBODY] = &WritePositionX;

  sprintf(output[OUT_POSITIONYSPINBODY].cName,"PositionYSpiNBody");
  sprintf(output[OUT_POSITIONYSPINBODY].cDescr,"Body's y position in SpiNBody");
  sprintf(output[OUT_POSITIONYSPINBODY].cNeg," ");
  output[OUT_POSITIONYSPINBODY].bNeg = 0;
  //output[OUT_POSITIONYSPINBODY].dNeg = 1;
  output[OUT_POSITIONYSPINBODY].iNum = 1;
  output[OUT_POSITIONYSPINBODY].iModuleBit = SPINBODY;
  fnWrite[OUT_POSITIONYSPINBODY] = &WritePositionY;

  sprintf(output[OUT_POSITIONZSPINBODY].cName,"PositionZSpiNBody");
  sprintf(output[OUT_POSITIONZSPINBODY].cDescr,"Body's z position in SpiNBody");
  sprintf(output[OUT_POSITIONZSPINBODY].cNeg," ");
  output[OUT_POSITIONZSPINBODY].bNeg = 0;
  //output[OUT_POSITIONZSPINBODY].dNeg = 1;
  output[OUT_POSITIONZSPINBODY].iNum = 1;
  output[OUT_POSITIONZSPINBODY].iModuleBit = SPINBODY;
  fnWrite[OUT_POSITIONZSPINBODY] = &WritePositionZ;

  sprintf(output[OUT_VELXSPINBODY].cName,"VelXSpiNBody");
  sprintf(output[OUT_VELXSPINBODY].cDescr,"Body's x velocity in SpiNBody");
  sprintf(output[OUT_VELXSPINBODY].cNeg," ");
  output[OUT_VELXSPINBODY].bNeg = 0;
  //output[OUT_VELXSPINBODY].dNeg = 1;
  output[OUT_VELXSPINBODY].iNum = 1;
  output[OUT_VELXSPINBODY].iModuleBit = SPINBODY;
  fnWrite[OUT_VELXSPINBODY] = &WriteVelX;

  sprintf(output[OUT_VELYSPINBODY].cName,"VelYSpiNBody");
  sprintf(output[OUT_VELYSPINBODY].cDescr,"Body's y velocity in SpiNBody");
  sprintf(output[OUT_VELYSPINBODY].cNeg," ");
  output[OUT_VELYSPINBODY].bNeg = 0;
  //output[OUT_VELYSPINBODY].dNeg = 1;
  output[OUT_VELYSPINBODY].iNum = 1;
  output[OUT_VELYSPINBODY].iModuleBit = SPINBODY;
  fnWrite[OUT_VELYSPINBODY] = &WriteVelY;

  sprintf(output[OUT_VELZSPINBODY].cName,"VelZSpiNBody");
  sprintf(output[OUT_VELZSPINBODY].cDescr,"Body's z velocity in SpiNBody");
  sprintf(output[OUT_VELZSPINBODY].cNeg," ");
  output[OUT_VELZSPINBODY].bNeg = 0;
  //output[OUT_VELZSPINBODY].dNeg = 1;
  output[OUT_VELZSPINBODY].iNum = 1;
  output[OUT_VELZSPINBODY].iModuleBit = SPINBODY;
  fnWrite[OUT_VELZSPINBODY] = &WriteVelZ;
}

//============================ End Writing Functions ===========================

void ForceBehaviorSpiNBody(BODY *body,EVOLVE *evolve,IO *io,SYSTEM *system,UPDATE *update,fnUpdateVariable ***fnUpdate,int iBody,int iModule) {

}

void PropertiesSpiNBody(BODY *body, EVOLVE *evolve, UPDATE *update, int iBody) {

}

//========================= Finalize Variable Functions ========================
void FinalizeUpdateVelXSpiNBody(BODY *body,UPDATE*update,int *iEqn,int iVar,int iBody,int iFoo) {
  update[iBody].iaModule[iVar][*iEqn] = SPINBODY;
  update[iBody].iNumVelX = (*iEqn)++;
}

void FinalizeUpdateVelYSpiNBody(BODY *body,UPDATE*update,int *iEqn,int iVar,int iBody,int iFoo) {
  update[iBody].iaModule[iVar][*iEqn] = SPINBODY;
  update[iBody].iNumVelY = (*iEqn)++;
}

void FinalizeUpdateVelZSpiNBody(BODY *body,UPDATE*update,int *iEqn,int iVar,int iBody,int iFoo) {
  update[iBody].iaModule[iVar][*iEqn] = SPINBODY;
  update[iBody].iNumVelZ = (*iEqn)++;
}

void FinalizeUpdatePositionXSpiNBody(BODY *body,UPDATE*update,int *iEqn,int iVar,int iBody,int iFoo) {
  update[iBody].iaModule[iVar][*iEqn] = SPINBODY;
  update[iBody].iNumPositionX = (*iEqn)++;
}

void FinalizeUpdatePositionYSpiNBody(BODY *body,UPDATE*update,int *iEqn,int iVar,int iBody,int iFoo) {
  update[iBody].iaModule[iVar][*iEqn] = SPINBODY;
  update[iBody].iNumPositionY = (*iEqn)++;
}

void FinalizeUpdatePositionZSpiNBody(BODY *body,UPDATE*update,int *iEqn,int iVar,int iBody,int iFoo) {
  update[iBody].iaModule[iVar][*iEqn] = SPINBODY;
  update[iBody].iNumPositionZ = (*iEqn)++;
}
//======================= End Finalize Variable Functions ======================

//================================ Log Functions ===============================

void LogBodySpiNBody(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UPDATE *update,fnWriteOutput fnWrite[],FILE *fp,int iBody) {
  int iOut;
  fprintf(fp,"----- SPINBODY PARAMETERS (%s)------\n",body[iBody].cName);

  for (iOut=OUTSTARTSPINBODY;iOut<OUTENDSPINBODY;iOut++) {
    if (output[iOut].iNum > 0)
      WriteLogEntry(body,control,&output[iOut],system,update,fnWrite[iOut],fp,iBody);
  }
}

void LogSpiNBody(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UPDATE *update,fnWriteOutput fnWrite[],FILE *fp) {
  int iOut;
  //I don't think this is called anywhere
  fprintf(fp,"\n----- SPINBODY PARAMETERS ------\n");
  for (iOut=OUTSTARTSPINBODY;iOut<OUTBODYSTARTSPINBODY;iOut++) {
    if (output[iOut].iNum > 0)
      WriteLogEntry(body,control,&output[iOut],system,update,fnWrite[iOut],fp,0);
  }
}

//============================= End Log Functions ==============================

// Add Module Function
void AddModuleSpiNBody(MODULE *module,int iBody,int iModule) {

  module->iaModule[iBody][iModule] = SPINBODY; //Name

  //Halts
  module->fnCountHalts[iBody][iModule] = &CountHaltsSpiNBody;
  module->fnVerifyHalt[iBody][iModule] = &VerifyHaltSpiNBody;

  module->fnLogBody[iBody][iModule] = &LogBodySpiNBody;
  module->fnReadOptions[iBody][iModule] = &ReadOptionsSpiNBody;
  module->fnVerify[iBody][iModule] = &VerifySpiNBody;

  module->fnInitializeBody[iBody][iModule] = &InitializeBodySpiNBody;
  module->fnInitializeUpdate[iBody][iModule] = &InitializeUpdateSpiNBody;
  module->fnInitializeOutput[iBody][iModule] = &InitializeOutputSpiNBody;
  module->fnInitializeUpdateTmpBody[iBody][iModule] = &InitializeUpdateTmpBodySpiNBody;

  //Primary Variable Finalizations
  module->fnFinalizeUpdateVelX[iBody][iModule] = &FinalizeUpdateVelXSpiNBody;
  module->fnFinalizeUpdateVelY[iBody][iModule] = &FinalizeUpdateVelYSpiNBody;
  module->fnFinalizeUpdateVelZ[iBody][iModule] = &FinalizeUpdateVelZSpiNBody;
  module->fnFinalizeUpdatePositionX[iBody][iModule] = &FinalizeUpdatePositionXSpiNBody;
  module->fnFinalizeUpdatePositionY[iBody][iModule] = &FinalizeUpdatePositionYSpiNBody;
  module->fnFinalizeUpdatePositionZ[iBody][iModule] = &FinalizeUpdatePositionZSpiNBody;
}

//============================= SpiNBody Functions =============================

double fdDPositionXDt(BODY *body, SYSTEM *system, int *iaBody) {
  return body[iaBody[0]].dVelX;
}

double fdDPositionYDt(BODY *body, SYSTEM *system, int *iaBody) {
  return body[iaBody[0]].dVelY;
}

double fdDPositionZDt(BODY *body, SYSTEM *system, int *iaBody) {
  return body[iaBody[0]].dVelZ;
}

double fdDVelXDt(BODY *body, SYSTEM *system, int *iaBody) {
  double dSumX = 0;        //Double used to calculate the net perturbation
  int iPertBody = -1;      //Int used to clean up code.
  int j;

  for(j=0; j<body[iaBody[0]].iGravPerts; j++){

    iPertBody = body[iaBody[0]].iaGravPerts[j]; //Which body is perturbing? Is this fast enough?

    //This is faster than using pow() but not very clean
    // Calculate the cube of the distance to each perturbing body. Used in Vy and Vz calculations as well.
    body[iaBody[0]].dDistance3[j] = sqrt((body[iPertBody].dPositionX-body[iaBody[0]].dPositionX)*(body[iPertBody].dPositionX-body[iaBody[0]].dPositionX)
          + (body[iPertBody].dPositionY-body[iaBody[0]].dPositionY)*(body[iPertBody].dPositionY-body[iaBody[0]].dPositionY)
          + (body[iPertBody].dPositionZ-body[iaBody[0]].dPositionZ)*(body[iPertBody].dPositionZ-body[iaBody[0]].dPositionZ));
    body[iaBody[0]].dDistance3[j] = body[iaBody[0]].dDistance3[j]*body[iaBody[0]].dDistance3[j]*body[iaBody[0]].dDistance3[j];

    dSumX = dSumX + BIGG*body[body[iaBody[0]].iaGravPerts[j]].dMass*(body[body[iaBody[0]].iaGravPerts[j]].dPositionX-body[iaBody[0]].dPositionX)/body[iaBody[0]].dDistance3[j];
  }

  return dSumX;
}

double fdDVelYDt(BODY *body, SYSTEM *system, int *iaBody) {
  double dSumY = 0;        //Double used to calculate the net perturbation
  int j;

  for(j=0; j<body[iaBody[0]].iGravPerts; j++){
    dSumY = dSumY + BIGG*body[body[iaBody[0]].iaGravPerts[j]].dMass*(body[body[iaBody[0]].iaGravPerts[j]].dPositionY-body[iaBody[0]].dPositionY)/body[iaBody[0]].dDistance3[j];
  }

  return dSumY;
}

double fdDVelZDt(BODY *body, SYSTEM *system, int *iaBody) {
  double dSumZ = 0;        //Double used to calculate the net perturbation
  int j;

  for(j=0; j<body[iaBody[0]].iGravPerts; j++){
    dSumZ = dSumZ + BIGG*body[body[iaBody[0]].iaGravPerts[j]].dMass*(body[body[iaBody[0]].iaGravPerts[j]].dPositionZ-body[iaBody[0]].dPositionZ)/body[iaBody[0]].dDistance3[j];
  }

  return dSumZ;
}
//========================== End SpiNBody Functions ============================
