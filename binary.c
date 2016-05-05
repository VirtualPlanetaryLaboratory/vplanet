/********************** BINARY.C **********************/
/*
 * David Fleming (dflemin3), Tue Jan 12 10:20am PDT 2016
 *
 * Subroutines that control the integration of the 
 * circumbinary planet dynamics module.
 * Note: body 0 = primary star, body 1 = secondary star, body 2+ = CBP (circumbinary planet)
 * Leung and Lee 2013 Theory ONLY applies to the restricted 3 body approximation.
*/

#include <stdio.h>
#include <math.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "vplanet.h"

void  InitializeControlBinary(CONTROL *control) {
  /* Nothing for now, but this subroutine is necessary for module loops. */
}
void BodyCopyBinary(BODY *dest,BODY *src,int foo,int iNumBodies,int iBody) {
  // Copy body properties from src to dest for cbp

  dest[iBody].iBodyType = src[iBody].iBodyType;
  dest[iBody].dCBPR = src[iBody].dCBPR;
  dest[iBody].dCBPZ = src[iBody].dCBPZ;
  dest[iBody].dCBPPhi = src[iBody].dCBPPhi;
  dest[iBody].dCBPRDot = src[iBody].dCBPRDot;
  dest[iBody].dCBPZDot = src[iBody].dCBPZDot;
  dest[iBody].dCBPPhiDot = src[iBody].dCBPPhiDot;

  dest[iBody].dFreeEcc = src[iBody].dFreeEcc;
  dest[iBody].dFreeInc = src[iBody].dFreeInc;
  dest[iBody].dLL13N0 = src[iBody].dLL13N0;
  dest[iBody].dLL13K0 = src[iBody].dLL13K0;
  dest[iBody].dLL13V0 = src[iBody].dLL13V0;
  dest[iBody].dR0 = src[iBody].dR0; 

  dest[iBody].dArgP = src[iBody].dArgP;
  dest[iBody].dInc = src[iBody].dInc;
  dest[iBody].dLongA = src[iBody].dLongA;
  dest[iBody].dLongP = src[iBody].dLongP;

}

void InitializeBodyBinary(BODY *body,CONTROL *control,UPDATE *update,int iBody,int iModule) {
// Only use this function for malloc'ing stuff
// Since nothing has to be malloc'ed for binary, do nothing
// Just keep for posterity
// RIP 2016 ~ 2016 #NeverForget

  /*
  // If ibody is the CBP (body 2, iBodyType 0), init
  if(body[iBody].iBodyType == 0) // If the body is a planet
  {

    // Init inital cylindrical positions, velocities
    // In verify, uses more intelligent starting points!
    body[iBody].dCBPR = body[iBody].dSemi;
    body[iBody].dCBPZ = 0.0;
    body[iBody].dCBPPhi = 0.0;
    body[iBody].dCBPRDot = 0.0;
    body[iBody].dCBPZDot = 0.0;
    body[iBody].dCBPPhiDot = 0.0;

    body[iBody].dR0 = body[iBody].dSemi; // CBPs Guiding Radius initial equal to dSemi, must be set before N0,K0,V0 !!!
    body[iBody].dMeanMotion = fdSemiToMeanMotion(body[iBody].dSemi,(body[0].dMass + body[1].dMass + body[iBody].dMass));
    
    body[iBody].dInc = body[iBody].dFreeInc; // CBP initial inc == free inclination
    body[iBody].dEcc = body[iBody].dFreeEcc; // CBP initial ecc == free ecc
    body[iBody].dLL13N0 = fdMeanMotionBinary(body,iBody);
    body[iBody].dLL13K0 = fdEpiFreqK(body,iBody);
    body[iBody].dLL13V0 = fdEpiFreqV(body,iBody);
   
    // Set up initial orbital elements
    body[iBody].dHecc = body[iBody].dEcc*sin(body[iBody].dLongP);
    body[iBody].dKecc = body[iBody].dEcc*cos(body[iBody].dLongP);
  }

  // Inits if the body is the secondary
  if(body[iBody].iBodyType == 1 && iBody == 1)
  {
    // Binary's inclination
    body[iBody].dInc = 0.0; //2.0*asin(body[iBody].dSinc);
    
    // Set Initial Poincare H, K using imputted dEcc
    body[iBody].dHecc = body[iBody].dEcc*sin(body[iBody].dLongP);
    body[iBody].dKecc = body[iBody].dEcc*cos(body[iBody].dLongP);
    body[iBody].dEccSq = body[iBody].dEcc*body[iBody].dEcc;
  }

  */
}

void InitializeUpdateTmpBodyBinary(BODY *body,CONTROL *control,UPDATE *update,int iBody) {
}

/**************** BINARY options ********************/

void ReadFreeEcc(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  // Free eccentricity, can't be in primary file
  // Note: Do error checking for negative values
  int lTmp=-1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn,options->cName,&dTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile,options->cName,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);
    if (dTmp < 0.0 || dTmp >= 1.0) {
      if (control->Io.iVerbose >= VERBERR)
        fprintf(stderr,"ERROR: %s must be in range [0,1).\n",options->cName);
      LineExit(files->Infile[iFile].cIn,lTmp);
    } else
      body[iFile-1].dFreeEcc = dTmp;
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else
    if (iFile > 0)
      body[iFile-1].dFreeEcc = options->dDefault;
}

void ReadLL13N0(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  // Lee + Leung 2013 Mean Motion N0
  /* This parameter cannot exist in primary file */
  int lTmp=-1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn,options->cName,&dTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile,options->cName,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);
    if (dTmp <= 0) {
      if (control->Io.iVerbose >= VERBERR)
        fprintf(stderr,"ERROR: %s must be greater than 0.\n",options->cName);
      LineExit(files->Infile[iFile].cIn,lTmp);  
    }
    body[iFile-1].dLL13N0 = dTmp;
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else {
    if (iFile > 0)
      AssignDefaultDouble(options,&body[iFile-1].dLL13N0,files->iNumInputs);
  }
}

void ReadLL13K0(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  // Lee + Leung 2013 radial epicyclic frequency
  /* This parameter cannot exist in primary file */
  int lTmp=-1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn,options->cName,&dTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile,options->cName,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);
    if (dTmp <= 0) {
      if (control->Io.iVerbose >= VERBERR)
        fprintf(stderr,"ERROR: %s must be greater than 0.\n",options->cName);
      LineExit(files->Infile[iFile].cIn,lTmp);  
    }
    body[iFile-1].dLL13K0 = dTmp;
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else {
    if (iFile > 0)
      AssignDefaultDouble(options,&body[iFile-1].dLL13K0,files->iNumInputs);
  }
}


void ReadLL13V0(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  // Lee + Leung 2013 vertical epicyclic frequency
  /* This parameter cannot exist in primary file */
  int lTmp=-1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn,options->cName,&dTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile,options->cName,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);
    if (dTmp <= 0) {
      if (control->Io.iVerbose >= VERBERR)
        fprintf(stderr,"ERROR: %s must be greater than 0.\n",options->cName);
      LineExit(files->Infile[iFile].cIn,lTmp);  
    }
    body[iFile-1].dLL13V0 = dTmp;
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else {
    if (iFile > 0)
      AssignDefaultDouble(options,&body[iFile-1].dLL13V0,files->iNumInputs);
  }
}

void ReadFreeInc(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  /* This parameter cannot exist in the primary file */
  /* Free inclination goes from 0 to 180 degrees */
  int lTmp=-1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn,options->cName,&dTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile,options->cName,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);
    if (control->Units[iFile].iAngle == 0) { // Input as radians
      if (dTmp < 0 || dTmp > PI) {
        if (control->Io.iVerbose >= VERBERR)
            fprintf(stderr,"ERROR: %s must be in the range [0,PI].\n",options->cName);
        LineExit(files->Infile[iFile].cIn,lTmp);        
      }
    } else { // Input as Degrees
      if (dTmp < 0 || dTmp > 180) {
        if (control->Io.iVerbose >= VERBERR)
            fprintf(stderr,"ERROR: %s must be in the range [0,180].\n",options->cName);
        LineExit(files->Infile[iFile].cIn,lTmp);        
      }
      /* Change to radians */
      dTmp *= DEGRAD;
    }

    body[iFile-1].dFreeInc = dTmp; 
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else 
    if (iFile > 0)
      body[iFile-1].dFreeInc = options->dDefault;
}  

void ReadHaltHolmanUnstable(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  /* This parameter cannot exist in primary file */
  int lTmp=-1;
  int bTmp;

  AddOptionBool(files->Infile[iFile].cIn,options->cName,&bTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile,options->cName,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);
    control->Halt[iFile-1].bHaltHolmanUnstable = bTmp;
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else
    if (iFile > 0)
      AssignDefaultInt(options,&control->Halt[iFile-1].bEnvelopeGone,files->iNumInputs);
}

/* Init Options BINARY */

void InitializeOptionsBinary(OPTIONS *options,fnReadOption fnRead[]) {
  int iOpt,iFile;

  sprintf(options[OPT_FREEECC].cName,"dFreeEcc");
  sprintf(options[OPT_FREEECC].cDescr,"Circumbinary planet free eccentricity");
  sprintf(options[OPT_FREEECC].cDefault,"0.0");
  options[OPT_FREEECC].dDefault = 0.0;
  options[OPT_FREEECC].iType = 2;
  options[OPT_FREEECC].iMultiFile = 1;
  fnRead[OPT_FREEECC] = &ReadFreeEcc;

  sprintf(options[OPT_FREEINC].cName,"dFreeInc");
  sprintf(options[OPT_FREEINC].cDescr,"Circumbinary planet free inclination");
  sprintf(options[OPT_FREEINC].cDefault,"0.0 degrees");
  options[OPT_FREEINC].dDefault = 0.0;
  options[OPT_FREEINC].iType = 2;
  options[OPT_FREEINC].iMultiFile = 1;
  fnRead[OPT_FREEINC] = &ReadFreeInc;

  sprintf(options[OPT_LL13N0].cName,"dLL13N0");
  sprintf(options[OPT_LL13N0].cDescr,"Lee+Leung 2013 Mean Motion");
  sprintf(options[OPT_LL13N0].cDefault,"1 /yr");
  options[OPT_LL13N0].dDefault = 1./YEARSEC;
  options[OPT_LL13N0].iType = 2;
  options[OPT_LL13N0].iMultiFile = 1;
  options[OPT_LL13N0].dNeg = 1./YEARSEC;
  sprintf(options[OPT_LL13N0].cNeg,"/Year");
  fnRead[OPT_LL13N0] = &ReadLL13N0;

  sprintf(options[OPT_LL13K0].cName,"dLL13K0");
  sprintf(options[OPT_LL13K0].cDescr,"Lee+Leung 2013 Radial Epicyclic Frequency");
  sprintf(options[OPT_LL13K0].cDefault,"1 /yr");
  options[OPT_LL13K0].dDefault = 1./YEARSEC;
  options[OPT_LL13K0].iType = 2;
  options[OPT_LL13K0].iMultiFile = 1;
  options[OPT_LL13K0].dNeg = 1./YEARSEC;
  sprintf(options[OPT_LL13K0].cNeg,"/Year");
  fnRead[OPT_LL13K0] = &ReadLL13K0;

  sprintf(options[OPT_LL13V0].cName,"dLL13V0");
  sprintf(options[OPT_LL13V0].cDescr,"Lee+Leung 2013 Radial Epicyclic Frequency");
  sprintf(options[OPT_LL13V0].cDefault,"1 /yr");
  options[OPT_LL13V0].dDefault = 1./YEARSEC;
  options[OPT_LL13V0].iType = 2;
  options[OPT_LL13V0].iMultiFile = 1;
  options[OPT_LL13V0].dNeg = 1./YEARSEC;
  sprintf(options[OPT_LL13V0].cNeg,"/Year");
  fnRead[OPT_LL13V0] = &ReadLL13V0;

  sprintf(options[OPT_HALTHOLMAN].cName,"bHaltHolmanUnstable");
  sprintf(options[OPT_HALTHOLMAN].cDescr,"Halt when CBP is Holman-Wiegert Unstable?");
  sprintf(options[OPT_HALTHOLMAN].cDefault,"0");
  options[OPT_HALTHOLMAN].iType = 0;
  fnRead[OPT_HALTHOLMAN] = &ReadHaltHolmanUnstable;

}

void ReadOptionsBinary(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,fnReadOption fnRead[],int iBody) {
  int iOpt;

  for (iOpt=OPTSTARTBINARY;iOpt<OPTENDBINARY;iOpt++) {
    if (options[iOpt].iType != -1) 
      fnRead[iOpt](body,control,files,&options[iOpt],system,iBody+1);
  }
}
    
/******************* Verify BINARY ******************/

void VerifyCBPR(BODY *body,OPTIONS *options,UPDATE *update,double dAge,fnUpdateVariable ***fnUpdate,int iBody) {

  update[iBody].iaType[update[iBody].iCBPR][0] = 0;
  update[iBody].iNumBodies[update[iBody].iCBPR][0] = 1;
  update[iBody].iaBody[update[iBody].iCBPR][0] = malloc(update[iBody].iNumBodies[update[iBody].iCBPR][0]*sizeof(int));
  update[iBody].iaBody[update[iBody].iCBPR][0][0] = iBody;

  update[iBody].pdCBPRBinary = &update[iBody].daDerivProc[update[iBody].iCBPR][0];
  fnUpdate[iBody][update[iBody].iCBPR][0] = &fdCBPRBinary;
}

void VerifyCBPZ(BODY *body,OPTIONS *options,UPDATE *update,double dAge,fnUpdateVariable ***fnUpdate,int iBody) {

  update[iBody].iaType[update[iBody].iCBPZ][0] = 0;
  update[iBody].iNumBodies[update[iBody].iCBPZ][0] = 1;
  update[iBody].iaBody[update[iBody].iCBPZ][0] = malloc(update[iBody].iNumBodies[update[iBody].iCBPZ][0]*sizeof(int));
  update[iBody].iaBody[update[iBody].iCBPZ][0][0] = iBody;

  update[iBody].pdCBPZBinary = &update[iBody].daDerivProc[update[iBody].iCBPZ][0];
  fnUpdate[iBody][update[iBody].iCBPZ][0] = &fdCBPZBinary;
}

void VerifyCBPPhi(BODY *body,OPTIONS *options,UPDATE *update,double dAge,fnUpdateVariable ***fnUpdate,int iBody) {

  update[iBody].iaType[update[iBody].iCBPPhi][0] = 0;
  update[iBody].iNumBodies[update[iBody].iCBPPhi][0] = 1;
  update[iBody].iaBody[update[iBody].iCBPPhi][0] = malloc(update[iBody].iNumBodies[update[iBody].iCBPPhi][0]*sizeof(int));
  update[iBody].iaBody[update[iBody].iCBPPhi][0][0] = iBody;

  update[iBody].pdCBPPhiBinary = &update[iBody].daDerivProc[update[iBody].iCBPPhi][0];
  fnUpdate[iBody][update[iBody].iCBPPhi][0] = &fdCBPPhiBinary;
}

void VerifyCBPRDot(BODY *body,OPTIONS *options,UPDATE *update,double dAge,fnUpdateVariable ***fnUpdate,int iBody) {

  update[iBody].iaType[update[iBody].iCBPRDot][0] = 0;
  update[iBody].iNumBodies[update[iBody].iCBPRDot][0] = 1;
  update[iBody].iaBody[update[iBody].iCBPRDot][0] = malloc(update[iBody].iNumBodies[update[iBody].iCBPRDot][0]*sizeof(int));
  update[iBody].iaBody[update[iBody].iCBPRDot][0][0] = iBody;

  update[iBody].pdCBPRDotBinary = &update[iBody].daDerivProc[update[iBody].iCBPRDot][0];
  fnUpdate[iBody][update[iBody].iCBPRDot][0] = &fdCBPRDotBinary;
}

void VerifyCBPZDot(BODY *body,OPTIONS *options,UPDATE *update,double dAge,fnUpdateVariable ***fnUpdate,int iBody) {

  update[iBody].iaType[update[iBody].iCBPZDot][0] = 0;
  update[iBody].iNumBodies[update[iBody].iCBPZDot][0] = 1;
  update[iBody].iaBody[update[iBody].iCBPZDot][0] = malloc(update[iBody].iNumBodies[update[iBody].iCBPZDot][0]*sizeof(int));
  update[iBody].iaBody[update[iBody].iCBPZDot][0][0] = iBody;

  update[iBody].pdCBPZDotBinary = &update[iBody].daDerivProc[update[iBody].iCBPZDot][0];
  fnUpdate[iBody][update[iBody].iCBPZDot][0] = &fdCBPZDotBinary;
}

void VerifyCBPPhiDot(BODY *body,OPTIONS *options,UPDATE *update,double dAge,fnUpdateVariable ***fnUpdate,int iBody) {
  
  update[iBody].iaType[update[iBody].iCBPPhiDot][0] = 0;
  update[iBody].iNumBodies[update[iBody].iCBPPhiDot][0] = 1;
  update[iBody].iaBody[update[iBody].iCBPPhiDot][0] = malloc(update[iBody].iNumBodies[update[iBody].iCBPPhiDot][0]*sizeof(int));
  update[iBody].iaBody[update[iBody].iCBPPhiDot][0][0] = iBody;

  update[iBody].pdCBPPhiDotBinary = &update[iBody].daDerivProc[update[iBody].iCBPPhiDot][0];
  fnUpdate[iBody][update[iBody].iCBPPhiDot][0] = &fdCBPPhiDotBinary;
}

void fnPropertiesBinary(BODY *body, UPDATE *update, int iBody){
  
  if(body[iBody].iBodyType == 0) // CBP
  {
    // Set CBP orbital elements, mean motion
    fdAssignOrbitalElements(body,iBody);
    body[iBody].dMeanMotion = fdSemiToMeanMotion(body[iBody].dR0,(body[0].dMass + body[1].dMass + body[iBody].dMass));         
  }
  else if(body[iBody].iBodyType == 1 && iBody == 1) // Binary
  {
    // Correctly set binary's mean motion
    body[iBody].dMeanMotion = fdSemiToMeanMotion(body[iBody].dSemi,(body[0].dMass+body[1].dMass));
  
    // Compute binary's eccentricity from Kecc and Hecc (since they are primary variables)
    body[iBody].dEcc = sqrt(pow(body[iBody].dKecc,2) + pow(body[iBody].dHecc,2));
  }
  else {
    return;
  }

}

void fnForceBehaviorBinary(BODY *body,EVOLVE *evolve,IO *io,SYSTEM *system,UPDATE *update,fnUpdateVariable ***fnUpdate,int iBody,int iModule){
// Anything here?
}

void VerifyBinary(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,OUTPUT *output,SYSTEM *system,UPDATE *update,fnUpdateVariable ***fnUpdate,int iBody,int iModule) {
  
  // If binary is being used, ALL bodies must have correct type
  if(iBody < 2) // Primary or secondary
  {
    if(body[iBody].iBodyType != 1)
    {
      if(control->Io.iVerbose >= VERBERR)
      {
        fprintf(stderr,"ERROR: In binary, bodies 0, 1 iBodyType must be 1 (star == 1).\n");
        fprintf(stderr,"Body 2 must be 0 (planet == 0).\n");
        fprintf(stderr,"iBody: %d iBodyType: %d\n",iBody,body[iBody].iBodyType);
      }
      exit(EXIT_INPUT);
    }
  }
  else // planets
  {
    if(body[iBody].iBodyType != 0)
    {
      if(control->Io.iVerbose >= VERBERR)
      {
        fprintf(stderr,"ERROR: In binary, bodies 0, 1 iBodyType must be 1 (star == 1).\n");
        fprintf(stderr,"Body number > 1 must be 0 (planet == 0).\n");
        fprintf(stderr,"iBody: %d iBodyType: %d\n",iBody,body[iBody].iBodyType);
      }
      exit(EXIT_INPUT);
    }
  }
 
  // If binary is being used, ALL bodies must have bBinary == 1
  int i;
  for(i = 0; i < control->Evolve.iNumBodies; i++)
  {
    if(body[i].bBinary == 0)
    {
      if(control->Io.iVerbose >= VERBERR)
      {
        fprintf(stderr,"ERROR: In binary, all bodies must have bBinary == 1.\n");
        fprintf(stderr,"body[i].bBinary == 0: %d\n",i);
      }
      exit(EXIT_INPUT);
    }
  }
  
  // Binary *should* only allow 3 bodies: 2 stars, 1 CBP since in LL13, cbps can't interact
  // But if the user wants to, they can have more, but make sure they're warned
  if(control->Evolve.iNumBodies > 3 && iBody > 2)
  {
    fprintf(stderr,"WARNING: In binary, Leung and Lee 2013 is a 3 body problem: 2 stars, 1 planet. Include additional planets at your own peril!\n");
  }

  // For CBP, setting dEcc, dInc doesn't matter since they are determined in part by dFreeEcc, dFreeInc
  if(body[iBody].iBodyType == 0)
  {
    if(body[iBody].dEcc != -1 || body[iBody].dInc != -1)
    {
      if(control->Io.iVerbose >= VERBERR)
      {
        fprintf(stderr,"WARNING: In binary setting dEcc, dInc for a circumbinary planet does not do anything.\n");
        fprintf(stderr,"Orbital evolution is dicated by the binary and the free inclination and eccentricity.\n");
      }
    }
  }

  // For binary, only the secondary should have the orbital elements set!
  if(body[iBody].iBodyType == 1)
  {
    if(iBody == 0) // Primary!
    {
      // These values default to -1
      if(fabs(body[iBody].dInc) + 1 < TINY || fabs(body[iBody].dEcc) + 1 < TINY || fabs(body[iBody].dSemi + 1) < TINY || fabs(body[iBody].dMeanMotion) + 1 < TINY)
      {
        if(control->Io.iVerbose >= VERBERR)
        {
          fprintf(stderr,"ERROR: In binary, binary orbital element information can ONLY be in the secondary star (iBody == 1).\n");
        }
        exit(EXIT_INPUT);
      }
    }
  }

  // Initialize the circumbinary planets
  if(body[iBody].iBodyType == 0) // Planets are added to matrix
  {
    // Add equations to the matrix

    // Call verifies to properly set up eqns in matrix
    VerifyCBPR(body,options,update,body[iBody].dAge,fnUpdate,iBody);
    VerifyCBPZ(body,options,update,body[iBody].dAge,fnUpdate,iBody);
    VerifyCBPPhi(body,options,update,body[iBody].dAge,fnUpdate,iBody);
    VerifyCBPRDot(body,options,update,body[iBody].dAge,fnUpdate,iBody);
    VerifyCBPZDot(body,options,update,body[iBody].dAge,fnUpdate,iBody);
    VerifyCBPPhiDot(body,options,update,body[iBody].dAge,fnUpdate,iBody);

    // Init parameters needed for subsequent cbp motion
    
    // dR0, dMeanMotion MUST be set before any of the frequencies
    body[iBody].dR0 = body[iBody].dSemi; // CBPs Guiding Radius initial equal to dSemi, must be set before N0,K0,V0 !!!
    body[iBody].dMeanMotion = fdSemiToMeanMotion(body[iBody].dR0,(body[0].dMass + body[1].dMass + body[iBody].dMass));
    body[iBody].dInc = body[iBody].dFreeInc; // CBP initial inc == free inclination
    body[iBody].dEcc = body[iBody].dFreeEcc; // CBP initial ecc == free ecc
    body[iBody].dLL13N0 = fdMeanMotionBinary(body,iBody);
    body[iBody].dLL13K0 = fdEpiFreqK(body,iBody);
    body[iBody].dLL13V0 = fdEpiFreqV(body,iBody);

    // Set up initial orbital elements that are primary variables
    body[iBody].dHecc = body[iBody].dEcc*sin(body[iBody].dLongP);
    body[iBody].dKecc = body[iBody].dEcc*cos(body[iBody].dLongP);

    // Set Planet initial positions, velocities according to LL13 theory
    int iaBody[1] = {iBody}; //  Pick out CBP
    body[iBody].dCBPR = fdCBPRBinary(body,system,iaBody);
    body[iBody].dCBPZ = fdCBPZBinary(body,system,iaBody);
    body[iBody].dCBPPhi = fdCBPPhiBinary(body,system,iaBody);
    body[iBody].dCBPRDot = fdCBPRDotBinary(body,system,iaBody);
    body[iBody].dCBPPhiDot = fdCBPPhiDotBinary(body,system,iaBody);
    body[iBody].dCBPZDot = fdCBPZDotBinary(body,system,iaBody);
 
    // Init orbital elements
    fdAssignOrbitalElements(body,iBody);
  
  }

  // Inits if the body is the secondary (sets required binary parameters)
  if(body[iBody].iBodyType == 1 && iBody == 1)
  {
    // Binary's inclination... in the plane or else
    body[iBody].dInc = 0.0; //2.0*asin(body[iBody].dSinc);
 
    // Set Initial Poincare H, K using imputted dEcc
    body[iBody].dHecc = body[iBody].dEcc*sin(body[iBody].dLongP);
    body[iBody].dKecc = body[iBody].dEcc*cos(body[iBody].dLongP);
    body[iBody].dEccSq = body[iBody].dEcc*body[iBody].dEcc;
  }

  // Other things that must be set
  control->fnForceBehavior[iBody][iModule] = &fnForceBehaviorBinary;
  control->Evolve.fnPropsAux[iBody][iModule] = &fnPropertiesBinary;
  control->Evolve.fnBodyCopy[iBody][iModule] = &BodyCopyBinary;
}

/**************** BINARY Update ****************/

void InitializeUpdateBinary(BODY *body, UPDATE *update, int iBody) {
  // Only planets should be in matrix
  if(body[iBody].iBodyType == 0)
  {
    if(update[iBody].iNumCBPR == 0)
      update[iBody].iNumVars++;
    update[iBody].iNumCBPR++;

    if(update[iBody].iNumCBPZ == 0)
      update[iBody].iNumVars++;
    update[iBody].iNumCBPZ++;

    if(update[iBody].iNumCBPPhi == 0)
      update[iBody].iNumVars++;
    update[iBody].iNumCBPPhi++;

    if(update[iBody].iNumCBPRDot == 0)
      update[iBody].iNumVars++;
    update[iBody].iNumCBPRDot++;

    if(update[iBody].iNumCBPZDot == 0)
      update[iBody].iNumVars++;
    update[iBody].iNumCBPZDot++;

    if(update[iBody].iNumCBPPhiDot == 0)
      update[iBody].iNumVars++;
    update[iBody].iNumCBPPhiDot++;
  }
}

void FinalizeUpdateCBPRBinary(BODY *body,UPDATE*update,int *iEqn,int iVar,int iBody,int iFoo) {
  update[iBody].iaModule[iVar][*iEqn] = BINARY;
  update[iBody].iNumCBPR = (*iEqn)++;
}

void FinalizeUpdateCBPZBinary(BODY *body,UPDATE*update,int *iEqn,int iVar,int iBody,int iFoo) {
  update[iBody].iaModule[iVar][*iEqn] = BINARY;
  update[iBody].iNumCBPZ = (*iEqn)++;
}

void FinalizeUpdateCBPPhiBinary(BODY *body,UPDATE*update,int *iEqn,int iVar,int iBody,int iFoo) {
  update[iBody].iaModule[iVar][*iEqn] = BINARY;
  update[iBody].iNumCBPPhi = (*iEqn)++;
}

void FinalizeUpdateCBPRDotBinary(BODY *body,UPDATE*update,int *iEqn,int iVar,int iBody,int iFoo) {
  update[iBody].iaModule[iVar][*iEqn] = BINARY;
  update[iBody].iNumCBPRDot = (*iEqn)++;
}

void FinalizeUpdateCBPZDotBinary(BODY *body,UPDATE*update,int *iEqn,int iVar,int iBody,int iFoo) {
  update[iBody].iaModule[iVar][*iEqn] = BINARY;
  update[iBody].iNumCBPZDot = (*iEqn)++;
}

void FinalizeUpdateCBPPhiDotBinary(BODY *body,UPDATE*update,int *iEqn,int iVar,int iBody,int iFoo) {
  update[iBody].iaModule[iVar][*iEqn] = BINARY;
  update[iBody].iNumCBPPhiDot = (*iEqn)++;
}

/***************** BINARY Halts *****************/


/*
 * If the CBP's dSemi is less than the Holman stability limit, it's unstable and
 * integration ends
 */
int fbHaltHolmanUnstable(BODY *body,EVOLVE *evolve,HALT *halt,IO *io,UPDATE *update,int iBody) {
  double a_crit = fdHolmanStability(body);
  
  // If the body is less than critical stability limit
  // Check stability for planets
  if(body[iBody].iBodyType == 0)
  {
    if(body[iBody].dSemi <= a_crit)
    {
      if(io->iVerbose >= VERBPROG) 
      {
        fprintf(stderr,"HALT: %s's dSemi: %lf AU, Holman-Wiegert critial a: %lf AU.\n",body[iBody].cName,body[iBody].dSemi/AUCM,a_crit/AUCM);
      }
      return 1;
    }
    return 0;
  }
  else // Doesn't apply to stars
    return 0;
}

void CountHaltsBinary(HALT *halt,int *iHalt) {
  if(halt->bHaltHolmanUnstable)
    (*iHalt)++;
}

void VerifyHaltBinary(BODY *body,CONTROL *control,OPTIONS *options,int iBody,int *iHalt) {
  if (control->Halt[iBody].bHaltHolmanUnstable)
    control->fnHalt[iBody][(*iHalt)++] = &fbHaltHolmanUnstable;
  
}

/************* BINARY Outputs ******************/

void HelpOutputBinary(OUTPUT *output) {
  int iOut;

  printf("\n ------ BINARY output ------\n");
  for (iOut=OUTSTARTBINARY;iOut<OUTENDBINARY;iOut++) 
    WriteHelpOutput(&output[iOut]);
}

void WriteFreeEcc(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  // Note: Only makes sense for planets (iBodyType == 0)
  if(body[iBody].iBodyType == 0)
    *dTmp = body[iBody].dFreeEcc;
  else
    *dTmp = -1;

  strcpy(cUnit,"");
}

void WriteFreeInc(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  // Note: Only makes sense for planets (iBodyType == 0)
  if(body[iBody].iBodyType == 0)
    *dTmp = body[iBody].dFreeInc;
  else
    *dTmp = -1;

  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else {
    *dTmp /= fdUnitsAngle(units->iAngle);
    fsUnitsAngle(units->iAngle,cUnit);
  }
}

void WriteInc(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[])
{
  // Note: Only makes sense for planets (iBodyType == 0)
  if(body[iBody].iBodyType == 0)
    *dTmp = body[iBody].dInc;
  else
    *dTmp = -1;

  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else {
    *dTmp /= fdUnitsAngle(units->iAngle);
    fsUnitsAngle(units->iAngle,cUnit);
  }
}

void WriteCBPPhi(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[])
{
  if(body[iBody].iBodyType == 0)
  {
    *dTmp = body[iBody].dCBPPhi;
  }
  else
    *dTmp = -1;

  if(output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else {
    *dTmp /= fdUnitsAngle(units->iAngle);
    fsUnitsAngle(units->iAngle,cUnit);
  }
}

void WriteCBPPhiDot(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {

  *dTmp = body[iBody].dCBPPhiDot;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else {
    *dTmp *= fdUnitsTime(units->iTime);
    fsUnitsRate(units->iTime,cUnit);
  }
}

void WriteLL13N0(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
 // Note: Only applies to planets (iBodyType == 0)
 if(body[iBody].iBodyType == 0)
   *dTmp = body[iBody].dLL13N0;
 else
   *dTmp = -1;

 if(output->bDoNeg[iBody]) {
   *dTmp *= output->dNeg;
   strcpy(cUnit,output->cNeg);
 } else {
   *dTmp *= fdUnitsTime(units->iTime);
   fsUnitsTime(units->iTime,cUnit);
 }
}


void WriteLL13K0(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
 // Note: Only applies to planets (iBodyType == 0)
 if(body[iBody].iBodyType == 0)
   *dTmp = body[iBody].dLL13K0;
 else
   *dTmp = -1;

 if(output->bDoNeg[iBody]) {
   *dTmp *= output->dNeg;
   strcpy(cUnit,output->cNeg);
 } else {
   *dTmp *= fdUnitsTime(units->iTime);
   fsUnitsTime(units->iTime,cUnit);
 }
}

void WriteLL13V0(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
 // Note: Only applies to planets (iBodyType == 0)
 if(body[iBody].iBodyType == 0)
   *dTmp = body[iBody].dLL13V0;
 else
   *dTmp = -1;

 if(output->bDoNeg[iBody]) {
   *dTmp *= output->dNeg;
   strcpy(cUnit,output->cNeg);
 } else {
   *dTmp *= fdUnitsTime(units->iTime);
   fsUnitsTime(units->iTime,cUnit);
 }
}

// Write the circumbinary planet orbital radius (CBPR)
void WriteCBPR(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {

  *dTmp = body[iBody].dCBPR;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else {
    *dTmp /= fdUnitsLength(units->iLength);
    fsUnitsLength(units->iLength,cUnit);
  }
}

void WriteCBPZ(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {

  *dTmp = body[iBody].dCBPZ;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else {
    *dTmp /= fdUnitsLength(units->iLength);
    fsUnitsLength(units->iLength,cUnit);
  }
}

void WriteCBPRDot(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {

  *dTmp = body[iBody].dCBPRDot;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else {
    *dTmp *= fdUnitsTime(units->iTime)/fdUnitsLength(units->iLength);
    fsUnitsVel(units,cUnit);
  }
}

void WriteCBPZDot(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  
  *dTmp = body[iBody].dCBPZDot;
  if(output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else {
    *dTmp *= fdUnitsTime(units->iTime)/fdUnitsLength(units->iLength);
    fsUnitsVel(units,cUnit);
  }
}

void InitializeOutputBinary(OUTPUT *output,fnWriteOutput fnWrite[])
{
  sprintf(output[OUT_FREEECC].cName,"FreeEcc");
  sprintf(output[OUT_FREEECC].cDescr,"Free Eccentricity");
  output[OUT_FREEECC].bNeg = 0;
  output[OUT_FREEECC].iNum = 1;
  fnWrite[OUT_FREEECC] = &WriteFreeEcc;

  sprintf(output[OUT_FREEINC].cName,"FreeInc");
  sprintf(output[OUT_FREEINC].cDescr,"Free Inclination");
  sprintf(output[OUT_FREEINC].cNeg,"Deg");
  output[OUT_FREEINC].bNeg = 1;
  output[OUT_FREEINC].dNeg = 1./DEGRAD;
  output[OUT_FREEINC].iNum = 1;
  fnWrite[OUT_FREEINC] = &WriteFreeInc;

  sprintf(output[OUT_BININC].cName,"dIncBinary");
  sprintf(output[OUT_BININC].cDescr,"Inclination");
  sprintf(output[OUT_BININC].cNeg,"Deg");
  output[OUT_BININC].bNeg = 1;
  output[OUT_BININC].dNeg = 1./DEGRAD;
  output[OUT_BININC].iNum = 1;
  fnWrite[OUT_BININC] = &WriteInc;

  sprintf(output[OUT_CBPPHI].cName,"CBPPhi");
  sprintf(output[OUT_CBPPHI].cDescr,"Circumbinary Planet Orbital Azimuthal Angle");
  sprintf(output[OUT_CBPPHI].cNeg,"Deg");
  output[OUT_CBPPHI].bNeg = 1;
  output[OUT_CBPPHI].dNeg = 1.0/DEGRAD;
  output[OUT_CBPPHI].iNum = 1;
  fnWrite[OUT_CBPPHI] = &WriteCBPPhi;

  sprintf(output[OUT_LL13N0].cName,"LL13N0");
  sprintf(output[OUT_LL13N0].cDescr,"Leung+Lee 2013 Mean Motion");
  sprintf(output[OUT_LL13N0].cNeg,"1/year");
  output[OUT_LL13N0].bNeg = 1;
  output[OUT_LL13N0].dNeg = 1./YEARSEC;
  output[OUT_LL13N0].iNum = 1;
  fnWrite[OUT_LL13N0] = &WriteLL13N0;

  sprintf(output[OUT_LL13K0].cName,"LL13K0");
  sprintf(output[OUT_LL13K0].cDescr,"Leung+Lee 2013 Radial epicyclic frequency");
  sprintf(output[OUT_LL13K0].cNeg,"1/year");
  output[OUT_LL13K0].bNeg = 1;
  output[OUT_LL13K0].dNeg = 1./YEARSEC;
  output[OUT_LL13K0].iNum = 1;
  fnWrite[OUT_LL13K0] = &WriteLL13K0;

  sprintf(output[OUT_LL13V0].cName,"LL13V0");
  sprintf(output[OUT_LL13V0].cDescr,"Leung+Lee 2013 vertical epicyclic frequency");
  sprintf(output[OUT_LL13V0].cNeg,"1/year");
  output[OUT_LL13V0].bNeg = 1;
  output[OUT_LL13V0].dNeg = 1./YEARSEC;
  output[OUT_LL13V0].iNum = 1;
  fnWrite[OUT_LL13V0] = &WriteLL13V0;

  sprintf(output[OUT_CBPR].cName,"CBPR");
  sprintf(output[OUT_CBPR].cDescr,"Circumbinary Planet Orbital Radius");
  output[OUT_CBPR].bNeg = 1;
  sprintf(output[OUT_CBPR].cNeg,"AU");
  output[OUT_CBPR].dNeg = 1.0/AUCM;
  output[OUT_CBPR].iNum = 1;
  fnWrite[OUT_CBPR] = &WriteCBPR;

  sprintf(output[OUT_CBPZ].cName,"CBPZ");
  sprintf(output[OUT_CBPZ].cDescr,"Circumbinary Planet Orbital Cylindrical Height");
  output[OUT_CBPZ].bNeg = 1;
  sprintf(output[OUT_CBPZ].cNeg,"AU");
  output[OUT_CBPZ].dNeg = 1.0/AUCM;
  output[OUT_CBPZ].iNum = 1;
  fnWrite[OUT_CBPZ] = &WriteCBPZ;
  
  sprintf(output[OUT_CBPRDOT].cName,"CBPRDot");
  sprintf(output[OUT_CBPRDOT].cDescr,"Circumbinary Radial Orbital Velocity");
  sprintf(output[OUT_CBPRDOT].cNeg,"/day");
  output[OUT_CBPRDOT].bNeg = 0;
  output[OUT_CBPRDOT].dNeg = DAYSEC;
  output[OUT_CBPRDOT].iNum = 1;
  fnWrite[OUT_CBPRDOT] = &WriteCBPRDot;

  sprintf(output[OUT_CBPZDOT].cName,"CBPZDot");
  sprintf(output[OUT_CBPZDOT].cDescr,"Circumbinary Z Orbital Velocity");
  sprintf(output[OUT_CBPZDOT].cNeg,"/day");
  output[OUT_CBPZDOT].bNeg = 0;
  output[OUT_CBPZDOT].dNeg = DAYSEC;
  output[OUT_CBPZDOT].iNum = 1;
  fnWrite[OUT_CBPZDOT] = &WriteCBPZDot;

  sprintf(output[OUT_CBPPHIDOT].cName,"CBPPhiDot");
  sprintf(output[OUT_CBPPHIDOT].cDescr,"Circumbinary Phi Angular Orbital Velocity");
  sprintf(output[OUT_CBPPHIDOT].cNeg,"/day");
  output[OUT_CBPPHIDOT].bNeg = 0;
  output[OUT_CBPPHIDOT].dNeg = DAYSEC;
  output[OUT_CBPPHIDOT].iNum = 1;
  fnWrite[OUT_CBPPHIDOT] = &WriteCBPPhiDot;

}

void FinalizeOutputFunctionBinary(OUTPUT *output,int iBody,int iModule) {
// Nothing
}

/************ BINARY Logging Functions **************/

void LogOptionsBinary(CONTROL *control, FILE *fp) {

fprintf(fp,"-------- BINARY Options -----\n\n");

}

void LogBinary(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UPDATE *update,fnWriteOutput fnWrite[],FILE *fp) {
  /* Anything here?*/
}

void LogBodyBinary(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UPDATE *update,fnWriteOutput fnWrite[],FILE *fp,int iBody) {
  int iOut;  
  fprintf(fp,"----- BINARY PARAMETERS (%s)------\n",body[iBody].cName);
  
  for (iOut=OUTSTARTBINARY;iOut<OUTENDBINARY;iOut++) {
    if (output[iOut].iNum > 0) 
      WriteLogEntry(body,control,&output[iOut],system,update,fnWrite[iOut],fp,iBody);
  }
}

void AddModuleBinary(MODULE *module,int iBody,int iModule) {

  module->iaModule[iBody][iModule] = BINARY;

  module->fnInitializeControl[iBody][iModule] = &InitializeControlBinary;
  module->fnInitializeUpdateTmpBody[iBody][iModule] = &InitializeUpdateTmpBodyBinary;

  module->fnCountHalts[iBody][iModule] = &CountHaltsBinary;
  module->fnReadOptions[iBody][iModule] = &ReadOptionsBinary;
  module->fnLogBody[iBody][iModule] = &LogBodyBinary;
  module->fnVerify[iBody][iModule] = &VerifyBinary;
  module->fnVerifyHalt[iBody][iModule] = &VerifyHaltBinary;

  module->fnInitializeBody[iBody][iModule] = &InitializeBodyBinary;
  module->fnInitializeUpdate[iBody][iModule] = &InitializeUpdateBinary;

  // Update functions
  module->fnFinalizeUpdateCBPR[iBody][iModule] = &FinalizeUpdateCBPRBinary;
  module->fnFinalizeUpdateCBPZ[iBody][iModule] = &FinalizeUpdateCBPZBinary;
  module->fnFinalizeUpdateCBPPhi[iBody][iModule] = &FinalizeUpdateCBPPhiBinary;
  module->fnFinalizeUpdateCBPPhiDot[iBody][iModule] = &FinalizeUpdateCBPPhiDotBinary;
  module->fnFinalizeUpdateCBPRDot[iBody][iModule] = &FinalizeUpdateCBPRDotBinary;
  module->fnFinalizeUpdateCBPZDot[iBody][iModule] = &FinalizeUpdateCBPZDotBinary;

  //module->fnIntializeOutputFunction[iBody][iModule] = &InitializeOutputBinary;
  module->fnFinalizeOutputFunction[iBody][iModule] = &FinalizeOutputFunctionBinary;
}

/************* BINARY Functions ************/

/* 
 * Useful math functions 
 */

/* 3D dot product */
double fdDot(double *a, double *b)
{
  return (a[0]*b[0] + a[1]*b[1] + a[2]*b[2]);
}

/* Kronecker Delta */
int fiDelta(int i, int j)
{
  if(i == j)
    return 1;
  else
    return 0;
}

/* Convert from cylindrical position coords to cartesian (3 dimensional) */
void fvCylToCartPos(double *daCylPos, double *dCartPos)
{
  dCartPos[0] = daCylPos[0]*cos(daCylPos[1]);
  dCartPos[1] = daCylPos[0]*sin(daCylPos[1]);
  dCartPos[2] = daCylPos[2];
}

/* Convert from cylindrical velocity coords to cartesian (3 dimensional) */
void fvCylToCartVel(double *daCylPos, double *daCylVel, double *dCartVel)
{
  dCartVel[0] = daCylVel[0]*cos(daCylPos[1]) - daCylPos[0]*sin(daCylPos[1])*daCylVel[1];
  dCartVel[1] = daCylVel[0]*sin(daCylPos[1]) + daCylPos[0]*cos(daCylPos[1])*daCylVel[1];
  dCartVel[2] = daCylVel[2];
}

/*
 * Functions for calculating orbital parameters assuming that the barycenter
 * of the binary is at the origin
 */

/* Calculate specific angular momentum 
 * h = r x v in cartesian coords
 */
void fvSpecificAngMom(double *r, double *v, double *h)
{
  // No return since resultant vector, h, is supplied
  cross(r,v,h); // r x v -> h
}

/* Calculate specific orbital energy
 * eps = v^2/2 - mu/|r|
 */
double fdSpecificOrbEng(BODY *body, int iBody)
{
  // For binary, iBody 0, 1 == stars, 2 == planet
  double mu = BIGG*(body[0].dMass + body[1].dMass + body[iBody].dMass); // Gravitational parameter
  double r[3] = {body[iBody].dCBPR,body[iBody].dCBPPhi,body[iBody].dCBPZ};
  double v[3] = {body[iBody].dCBPRDot,body[iBody].dCBPPhiDot,body[iBody].dCBPZDot};
  double rCart[3] = {0.0,0.0,0.0};
  double vCart[3] = {0.0,0.0,0.0};
     
  // Convert from cyl->cart coords
  fvCylToCartPos(r,rCart);
  fvCylToCartVel(r,v,vCart);
  
  double r_norm = sqrt(fdDot(rCart,rCart));
  
  double eng = fdDot(vCart,vCart)/2.0 - (mu/r_norm);
  return eng;
}

/* Compute and assign CBP's orbital elements */

void fdAssignOrbitalElements(BODY *body, int iBody)
{
  body[iBody].dSemi = fdComputeSemi(body,iBody);
  body[iBody].dEcc = fdComputeEcc(body,iBody);
  body[iBody].dInc = fdComputeInc(body,iBody);

  // LongA, ArgP -> LongP (needed for dHecc, dKecc)
  body[iBody].dLongA = fdComputeLongA(body,iBody);
  body[iBody].dArgP = fdComputeArgPeri(body,iBody);
  double LongP = body[iBody].dLongA + body[iBody].dArgP;
  while(LongP > 2.0*PI)
  {
    LongP -= 2.0*PI;
  }
  while(LongP < 0.0)
  {
    LongP += 2.0*PI;
  }
  body[iBody].dLongP = LongP;

  body[iBody].dEccSq = body[iBody].dEcc*body[iBody].dEcc; 
  // Set Poincare H, K
  body[iBody].dHecc = body[iBody].dEcc*sin(body[iBody].dLongP);
  body[iBody].dKecc = body[iBody].dEcc*cos(body[iBody].dLongP);
}

/* Compute a body's semimajor axis
 * a = -mu/(2*eps)
 */
double fdComputeSemi(BODY *body, int iBody)
{
  // For binary, iBody 0, 1 == stars, 2 == planet
  return -BIGG*(body[0].dMass + body[1].dMass + body[iBody].dMass)/(2.0*fdSpecificOrbEng(body,iBody));
}

/* Compute a body's orbital eccentricity
 * e = sqrt(1 + 2*eps*h*h/(mu*mu))
 */
double fdComputeEcc(BODY *body, int iBody)
{
  // For binary, iBody 0, 1 == stars, 2 == planet
  double mu = BIGG*(body[0].dMass + body[1].dMass + body[iBody].dMass); // Gravitational parameter
  double h[3];
  double r[3] = {body[iBody].dCBPR,body[iBody].dCBPPhi,body[iBody].dCBPZ};
  double v[3] = {body[iBody].dCBPRDot,body[iBody].dCBPPhiDot,body[iBody].dCBPZDot};
  double rCart[3];
  double vCart[3];
  
  // Convert from cyl->cart coords
  fvCylToCartPos(r,rCart);
  fvCylToCartVel(r,v,vCart);
  fvSpecificAngMom(rCart,vCart,h);
  
  return sqrt(1. + (2.*fdSpecificOrbEng(body,iBody)*fdDot(h,h))/(mu*mu));
}

/* Compute a body's inclincation
 * i = arccos(h_z/|h|)
 */
double fdComputeInc(BODY *body, int iBody)
{
  double h[3];
  double r[3] = {body[iBody].dCBPR,body[iBody].dCBPPhi,body[iBody].dCBPZ};
  double v[3] = {body[iBody].dCBPRDot,body[iBody].dCBPPhiDot,body[iBody].dCBPZDot};
  double rCart[3];
  double vCart[3];
     
  // Convert from cyl->cart coords
  fvCylToCartPos(r,rCart);
  fvCylToCartVel(r,v,vCart);
  
  fvSpecificAngMom(rCart,vCart,h);
  return acos(h[2]/sqrt(fdDot(h,h)));
}

/* Compute a body's longitude of ascending node
 * See: https://en.wikipedia.org/wiki/Longitude_of_the_ascending_node
 */
double fdComputeLongA(BODY *body, int iBody)
{
  double Omega = 0.0;
  double h[3];
  double r[3] = {body[iBody].dCBPR,body[iBody].dCBPPhi,body[iBody].dCBPZ};
  double v[3] = {body[iBody].dCBPRDot,body[iBody].dCBPPhiDot,body[iBody].dCBPZDot};
  double rCart[3];
  double vCart[3];

  // Convert from cyl->cart coords
  fvCylToCartPos(r,rCart);
  fvCylToCartVel(r,v,vCart);

  fvSpecificAngMom(rCart,vCart,h);
  double n[3] = {-h[1],h[0],0};

  // Case: |n| = 0
  double mag_n = sqrt(fdDot(n,n));
  if(fabs(mag_n) < TINY)
  {
    return Omega;
  }

  if(n[1] >= 0)
  {
    Omega = acos(n[0]/mag_n);
  }
  else
  {
    Omega = 2.0*PI - acos(n[0]/mag_n);
  }

  return Omega;
}

void fdComputeEccVector(BODY *body, double *evec, int iBody)
{
  double mu = BIGG*(body[0].dMass + body[1].dMass + body[iBody].dMass); // Gravitational parameter
  double h[3];

  double r[3] = {body[iBody].dCBPR,body[iBody].dCBPPhi,body[iBody].dCBPZ};
  double v[3] = {body[iBody].dCBPRDot,body[iBody].dCBPPhiDot,body[iBody].dCBPZDot};
  double rCart[3];
  double vCart[3];
     
  // Convert from cyl->cart coords
  fvCylToCartPos(r,rCart);
  fvCylToCartVel(r,v,vCart);

  fvSpecificAngMom(rCart,vCart,h);
  cross(vCart,h,evec);
  
  double mag_r = sqrt(fdDot(rCart,rCart));

  for(int i = 0; i < 3; i++)
  {
    evec[i] = evec[i]/mu - rCart[i]/mag_r;
  }
}

double fdComputeArgPeri(BODY *body, int iBody)
{
  double evec[3];
  double h[3];

  double r[3] = {body[iBody].dCBPR,body[iBody].dCBPPhi,body[iBody].dCBPZ};
  double v[3] = {body[iBody].dCBPRDot,body[iBody].dCBPPhiDot,body[iBody].dCBPZDot};
  double rCart[3];
  double vCart[3];
       
  // Convert from cyl->cart coords
  fvCylToCartPos(r,rCart);
  fvCylToCartVel(r,v,vCart);

  fvSpecificAngMom(rCart,vCart,h);
  fdComputeEccVector(body,evec,iBody);
  double n[3] = {-h[1],h[0],0};
  double mag_n = sqrt(fdDot(n,n));
  double mag_evec = sqrt(fdDot(evec,evec));

  // if LongA ~ 0, assume planar orbit
  if(fabs(fdComputeLongA(body,iBody)) < TINY)
  {
    if(h[2] > 0)
    {
      return atan2(evec[1],evec[0]);
    }
    else
    {
      return 2.0*PI - atan2(evec[1],evec[0]);
    }
  }
  else
  {
    if(evec[2] >= 0)
    {
      return acos(fdDot(n,evec)/(mag_n*mag_evec));
    }
    else
    {
      return 2.0*PI - acos(fdDot(n,evec)/(mag_n*mag_evec));
    }
  }
}

/* Convert mean anomaly M to eccentric anomaly E
 * by solving kepler equation using Newton's Method 
 */
double fdMeanToEccentric(double M, double e)
{
  // If e is 0, or close enough, return 
  if(e < TINY)
  {
    return M;
  }
  else if(e >= 1 || e < 0) // Should never happen, but better safe than sorry
  {
    fprintf(stderr,"ERROR: in fdMeanToEccentric (binary), eccentricity must be within [0,1). e: %e\n",e);
    exit(1);
  }
  
  double E0 = M/(1.0-e) - e*pow(M,3.)/(6.*pow(1.-e,4.)); // First guess via series expansion
  double E = E0;
  double dE = KEQNTOL + 1.0;
  int count = 0;

  while(dE > KEQNTOL)
  {
    // Compute E_n+1 (E) from E_n (E0)
    E = E0 - (E0 - e*sin(E0) - M)/(1. - e*cos(E0));
    dE = fabs(E-E0);
    E0 = E;
  
    count += 1;

    if(count <= 20) // Stop if too many iterations
    {
      fprintf(stderr,"ERROR: in fdMeanToEccentric, too many iterations to solve Kepler Equation\n");
      exit(1);
    }

  }
  
  return E;
}

/* Convert eccentric anomaly to true anomaly */
double fdEccToTrue(double E, double e)
{
  return 2.0*atan2(sqrt(1.-e)*cos(E/2.),sqrt(1.+e)*sin(E/2.));
}

/* 
 * Compute the dynamical stability limit, a_crit, first computed by
 * Holman and Wieget 1999 that depends on binary dSemi, dEcc
 * If CBP.dSemi < a_crit, planet is unstable and system should halt
 */
double fdHolmanStability(BODY *body)
{
  double a = body[1].dSemi;
  double e = body[1].dEcc;
  double mu = body[1].dMass/(body[0].dMass + body[1].dMass);

  return (1.6 + 5.1*e -2.22*e*e + 4.12*mu - 4.27*e*mu - 5.09*mu*mu + 4.61*e*e*mu*mu)*a;
}

/* Compute the mean anomaly M = n*t + phi where phi is an arbitrary offset */
double fdMeanAnomaly(double dMeanMotion, double dTime, double dPhi)
{
  return dMeanMotion * dTime + dPhi;
}

//Below are functions russell already defined in distorb that i'll reuse
//double fdLaplaceCoeff(double dAxRatio, int iIndexJ, double dIndexS)
//double fdDerivLaplaceCoeff(int iNthDeriv, double dAxRatio, int iIndexJ, double dIndexS)
// Note: for Laplace Coeff functions, they go as b^J_s(alpha) where J is an int, S is a half int double !!

/* 
 * Analytic equations from Leung+Lee 2013 that govern circumbinary planet (cbp) evolution
 * for 1 (!) cbp
 */

/* LL13 N0 */
double fdMeanMotionBinary(BODY *body, int iBody)
{
  // Define intermediate quantities
  double M = body[0].dMass + body[1].dMass;
  double alphaa = (body[1].dSemi*body[1].dMass/M)/body[iBody].dR0;
  double alphab = (body[1].dSemi*body[0].dMass/M)/body[iBody].dR0;

  double N0 = body[iBody].dMeanMotion*body[iBody].dMeanMotion/2.;

  double tmp1 = body[0].dMass*fdLaplaceCoeff(alphaa,0,0.5)/M;
  tmp1 += body[1].dMass*fdLaplaceCoeff(alphab,0,0.5)/M;

  double tmp2 = body[0].dMass*body[1].dMass*body[1].dSemi/(M*M*body[iBody].dR0);
  tmp2 *= (fdDerivLaplaceCoeff(1,alphaa,0,0.5) + fdDerivLaplaceCoeff(1,alphab,0,0.5));

  return sqrt(N0*(tmp1 + tmp2));
}

/* LL13 K0 */
double fdEpiFreqK(BODY *body, int iBody)
{
  //Define intermediate quantities
  double M = body[0].dMass + body[1].dMass;
  double alphaa = (body[1].dSemi*body[1].dMass/M)/body[iBody].dR0;
  double alphab = (body[1].dSemi*body[0].dMass/M)/body[iBody].dR0;
 
  double K0 = body[iBody].dMeanMotion*body[iBody].dMeanMotion/2.;

  double tmp1 = body[0].dMass*fdLaplaceCoeff(alphaa,0,0.5)/M;
  tmp1 += body[1].dMass*fdLaplaceCoeff(alphab,0,0.5)/M;

  double tmp2 = body[0].dMass*body[1].dMass*body[1].dSemi/(M*M*body[iBody].dR0);
  tmp2 *= (fdDerivLaplaceCoeff(1,alphaa,0,0.5) + fdDerivLaplaceCoeff(1,alphab,0,0.5));

  double tmp3 = body[0].dMass*body[1].dMass*body[1].dSemi*body[1].dSemi/(M*M*body[iBody].dR0*body[iBody].dR0);

  double tmp4 = body[1].dMass*fdDerivLaplaceCoeff(2,alphaa,0,0.5)/M;
  tmp4 += body[0].dMass*fdDerivLaplaceCoeff(2,alphab,0,0.5)/M;
  tmp3 *= tmp4;

  K0 *= (tmp1 - tmp2 - tmp3);
  return sqrt(K0);
}

/* LL13 V0 */
double fdEpiFreqV(BODY *body, int iBody)
{
  //Define intermediate quantities
  double M = body[0].dMass + body[1].dMass;
  double alphaa = (body[1].dSemi*body[1].dMass/M)/body[iBody].dR0;
  double alphab = (body[1].dSemi*body[0].dMass/M)/body[iBody].dR0;
     
  double V0 = body[iBody].dMeanMotion*body[iBody].dMeanMotion/2.;

  double tmp1 = body[0].dMass*fdLaplaceCoeff(alphaa,0,1.5)/M;
  tmp1 += body[1].dMass*fdLaplaceCoeff(alphab,0,1.5)/M;
  return sqrt(V0 * tmp1);
}

/* Circular (azimuthal) motion of the guiding center for a cbp: phi0 */
double fdPhi0(double dTime, double dMeanMotion, double dPsi)
{
  return dMeanMotion * dTime + dPsi;
}

/*
 * Functions to compute binary potentials and their derivatives w.r.t. radius
 */

/* LL13 Eqn 15: Binary potential component 0 */
double fdPot0(int j, int k, double R, BODY *body)
{
  //Define intermediate quantities
  double M = body[0].dMass + body[1].dMass;
  double alphaa = (body[1].dSemi*body[1].dMass/M)/R;
  double alphab = (body[1].dSemi*body[0].dMass/M)/R;

  double coeff = -(2. - fiDelta(k,0))/2.;
  coeff *= BIGG*(body[0].dMass + body[1].dMass)/R;

  double tmp1 = pow(-1.,k)*body[0].dMass*fdLaplaceCoeff(alphaa,k,(j+1.)/2.)/M;
  tmp1 += body[1].dMass*fdLaplaceCoeff(alphab,k,(j+1.)/2.)/M;

  return coeff*tmp1;

}

/* LL13 Eqn 15: d/dR of binary potential component 0 */
double fdPot0dR(int j, int k, double R, BODY *body)
{
  //Define intermediate quantities
  double M = body[0].dMass + body[1].dMass;
  double alphaa = (body[1].dSemi*body[1].dMass/M)/R;
  double alphab = (body[1].dSemi*body[0].dMass/M)/R;

  double coeff = -(2. - fiDelta(k,0))/2.;

  double f = pow(-1.,k)*body[0].dMass*fdLaplaceCoeff(alphaa,k,(j+1.)/2.)/M;
  f += body[1].dMass*fdLaplaceCoeff(alphab,k,(j+1.)/2.)/M;
  double g = BIGG*(body[0].dMass + body[1].dMass)/R;

  double g_prime = -g/R;
  double f_prime = -pow(-1.,k)*body[0].dMass*alphaa*R*fdDerivLaplaceCoeff(1,alphaa,k,(j+1.)/2.)/(R*R*M);
  f_prime += -body[1].dMass*alphab*R*fdDerivLaplaceCoeff(1,alphab,k,(j+1.)/2.)/(R*R*M);

  return coeff*(f_prime*g + g_prime*f);
}

/* LL13 eqn 16: Binary potential component 1 */
double fdPot1(int j, int k, double R, BODY *body)
{
  //Define intermediate quantities
  double M = body[0].dMass + body[1].dMass;
  double alphaa = (body[1].dSemi*body[1].dMass/M)/R;
  double alphab = (body[1].dSemi*body[0].dMass/M)/R;

  double coeff = -(2. - fiDelta(k,0))/2.;
  coeff *= BIGG*(body[0].dMass + body[1].dMass)/R;

  double tmp1 = pow(-1.,k)*body[0].dMass*alphaa*fdDerivLaplaceCoeff(1,alphaa,k,(j+1.)/2.)/M;
  tmp1 += body[1].dMass*alphab*fdDerivLaplaceCoeff(1,alphab,k,(j+1.)/2.)/M;

  return coeff*tmp1;
}

/* LL13 eqn 16: d/dR of binary potential component 1 */
double fdPot1dR(int j, int k, double R, BODY * body)
{
  //Define intermediate quantities
  double M = body[0].dMass + body[1].dMass;
  double alphaa = (body[1].dSemi*body[1].dMass/M)/R;
  double alphab = (body[1].dSemi*body[0].dMass/M)/R;

  double coeff = -(2. - fiDelta(k,0))/2.;

  // Break up big terms into multiple terms: x
  double f = pow(-1.,k)*body[0].dMass*alphaa/M;
  double g = fdDerivLaplaceCoeff(1,alphaa,k,(j+1.)/2.);
  double h = body[1].dMass*alphab/M;
  double i = fdDerivLaplaceCoeff(1,alphab,k,(j+1.)/2.);
  double l = BIGG*(body[0].dMass + body[1].dMass)/R;

  // Derivatives of the above terms w.r.t. R (and hence, alphas): xp
  double fp = -pow(-1.,k)*body[0].dMass*alphaa/(R*M);
  double gp = -alphaa*fdDerivLaplaceCoeff(2,alphaa,k,(j+1.)/2.)/R;
  double hp = -body[1].dMass*alphab/(R*M);
  double ip = -alphab*fdDerivLaplaceCoeff(2,alphab,k,(j+1.)/2.)/R;
  double lp = -BIGG*(body[0].dMass + body[1].dMass)/(R*R);

  return coeff*((fp*g + gp*f + hp*i + ip*h)*l + lp*(f*g + h*i));
}

/* LL13 function defined in paragraph in between eqn 24 and 25 which is like
 * the mean motion at R
 */
double fdn(double R, BODY *body)
{
  return sqrt(fdPot0dR(0,0,R,body)/R);
}

/*
 * Definitions for C, D terms from LL13
 * They are essentially combinations of different potential components
 * and their derivatives w.r.t R evaluated at the location of the cbp
 */

/* LL13 C0 as defined in eqn 28 */
double fdC0(BODY *body, int iBody)
{
  double c = -body[1].dEcc*fdPot1dR(0,0,body[iBody].dR0,body)/body[iBody].dR0;
  c /= (body[iBody].dLL13K0*body[iBody].dLL13K0 - body[1].dMeanMotion*body[1].dMeanMotion);
  return c;
}

/* LL13 C^0_k as defined by eqn 29 */
double fdC0k(int k, BODY *body, int iBody)
{
  double n = fdn(body[iBody].dR0,body);
  double c = fdPot0dR(0,k,body[iBody].dR0,body) + (2.*n*fdPot0(0,k,body[iBody].dR0,body))/(body[iBody].dR0*(n-body[1].dMeanMotion));
  c /= body[iBody].dR0;
  c /= (body[iBody].dLL13K0*body[iBody].dLL13K0 - k*k*pow(body[iBody].dLL13N0 - body[1].dMeanMotion,2));
  return c;
}

/* LL13 C^+_k as defined by eqn 30a (the + term) */
double fdCPk(int k, BODY *body, int iBody)
{
  double n = fdn(body[iBody].dR0,body);
  double tmp1 = k*fdPot0dR(0,k,body[iBody].dR0,body) - 0.5*fdPot1dR(0,k,body[iBody].dR0,body);
  double tmp2 = k*n*(2.*k*fdPot0(0,k,body[iBody].dR0,body) - fdPot1(0,k,body[iBody].dR0,body));
  tmp2 /= body[iBody].dR0*(k*n - (k+1.)*body[1].dMeanMotion);
  tmp1 += tmp2;

  double c = body[1].dEcc*tmp1/body[iBody].dR0;
  c /= body[iBody].dLL13K0*body[iBody].dLL13K0 - (k*body[iBody].dLL13N0 - (k+1.)*body[1].dMeanMotion)*(k*body[iBody].dLL13N0 - (k+1.)*body[1].dMeanMotion);
  return c;
}

/* LL13 C^-_k as defined by eqn 30b (the - term) */
double fdCMk(int k, BODY *body, int iBody)
{
  double n = fdn(body[iBody].dR0,body);
  double tmp1 = -k*fdPot0dR(0,k,body[iBody].dR0,body) - 0.5*fdPot1dR(0,k,body[iBody].dR0,body);
  double tmp2 = k*n*(-2.*k*fdPot0(0,k,body[iBody].dR0,body) - fdPot1(0,k,body[iBody].dR0,body));
  tmp2 /= body[iBody].dR0*(k*n - (k-1.)*body[1].dMeanMotion);
  tmp1 += tmp2;
   
  double c = body[1].dEcc*tmp1/body[iBody].dR0;
  c /= body[iBody].dLL13K0*body[iBody].dLL13K0 - (k*body[iBody].dLL13N0 - (k-1.)*body[1].dMeanMotion)*(k*body[iBody].dLL13N0 - (k-1.)*body[1].dMeanMotion);
  return c;
}

/* LL13 D0 as defined by eqn 32 */
double fdD0(BODY * body, int iBody)
{
  return 2.0*fdC0(body, iBody);
}

/* LL13 Dk0 as defined by eqn 33 */
double fdDk0(int k, BODY * body, int iBody)
{
  double n = fdn(body[iBody].dR0,body);
  double tmp1 = 2.0*fdC0k(k,body,iBody);
  double Dk = -fdPot0(0,k,body[iBody].dR0,body)/(body[iBody].dR0*body[iBody].dR0);
  Dk /= (n*(n - body[1].dMeanMotion));

  return (tmp1-Dk);
}

/* LL13 D+_k as defined by eqn 34a (the + term) */
double fdDPk(int k, BODY * body, int iBody)
{
  double n = fdn(body[iBody].dR0,body);
  double Dk = 2.0*fdCPk(k,body,iBody);

  double tmp1 = body[1].dEcc*(k*(2.*k*fdPot0(0,k,body[iBody].dR0,body) - fdPot1(0,k,body[iBody].dR0,body)));
  tmp1 /= 2.0*body[iBody].dR0*body[iBody].dR0*n * (k*n - (k+1.)*body[1].dMeanMotion);
  
  return Dk - tmp1;
}

/* LL13 D-_k as defined by eqn 34b (the - term) */
double fdDMk(int k, BODY * body, int iBody)
{
  double n = fdn(body[iBody].dR0,body);
  double Dk = 2.0*fdCMk(k,body,iBody);
 
  double tmp1 = body[1].dEcc*(k*(-2.*k*fdPot0(0,k,body[iBody].dR0,body) - fdPot1(0,k,body[iBody].dR0,body)));
  tmp1 /= 2.0*body[iBody].dR0*body[iBody].dR0*n * (k*n - (k-1.)*body[1].dMeanMotion);

  return Dk - tmp1;
}

/*
 * LL13 Functions to compute cylindrical positions, velocities
 * R, phi, z and RDot, phiDot, zDot
 */

/* Computes the CBP orbital radius */
double fdCBPRBinary(BODY *body,SYSTEM *system,int *iaBody) 
{ 
  int iBody = iaBody[0];

  // Note: Assume all phase values (phi, psi, etc...) are 0
  // Fine because they are arbitary offsets
  double dPsi = 0.0;
  double dTime = body[iBody].dAge; // Time == Age of the body

  // Useful intermediate quantities
  double M = fdMeanAnomaly(body[1].dMeanMotion,dTime,0);
  double phi0 = fdPhi0(dTime,body[iBody].dLL13N0,0);
  double varpi = body[1].dLongP;//body[0].dLongA + body[0].dArgP;

  double tmp1 = 1. - body[iBody].dFreeEcc*cos(body[iBody].dLL13K0*dTime + dPsi) - fdC0(body,iBody)*cos(M);
  double tmp2 = 0.0;
  double tmp3 = 0.0;

  int k;
  for(k = 1; k < K_MAX; k++)
  {
    tmp3 = fdC0k(k,body,iBody)*cos(k*(phi0 - M - varpi));
    tmp3 += fdCPk(k,body,iBody)*cos(k*(phi0 - varpi) - (k+1.)*M);
    tmp3 += fdCMk(k,body,iBody)*cos(k*(phi0 - varpi) - (k-1.)*M);
    tmp2 += tmp3;
  }

  return body[iBody].dR0*(tmp1-tmp2);
}

double fdCBPPhiBinary(BODY *body,SYSTEM *system,int *iaBody)
{
  int iBody = iaBody[0];

  // Note: Assume all phase values (phi, psi, etc...) are 0
  // Fine because they are arbitrary offsets
  double dPsi = 0.0;
  double dTime = body[iBody].dAge; // Time == Age of the body

  // Useful intermediate quantities
  double M = fdMeanAnomaly(body[1].dMeanMotion,dTime,0);
  double phi0 = fdPhi0(dTime,body[iBody].dLL13N0,0);
  double varpi = body[1].dLongP;//body[0].dLongA + body[0].dArgP;

  double phi = phi0 + 2.0*body[iBody].dLL13N0*body[iBody].dFreeEcc*sin(body[iBody].dLL13K0*dTime + dPsi)/body[iBody].dLL13K0;
  phi += body[iBody].dLL13N0*fdD0(body,iBody)*sin(M)/body[1].dMeanMotion;

  double tot = 0.0;
  double tmp1 = 0.0;
  int k;
  for(k = 1; k < K_MAX; k++)
  {
    tmp1 = body[iBody].dLL13N0*fdDk0(k,body,iBody)*sin(k*(phi0-M-varpi))/(k*(body[iBody].dLL13N0-body[1].dMeanMotion));
    tmp1 += body[iBody].dLL13N0*fdDPk(k,body,iBody)*sin(k*(phi0-varpi) - (k+1.)*M)/(k*body[iBody].dLL13N0 - (k+1.)*body[1].dMeanMotion);
    tmp1 += body[iBody].dLL13N0*fdDMk(k,body,iBody)*sin(k*(phi0-varpi) - (k-1.)*M)/(k*body[iBody].dLL13N0 - (k-1.)*body[1].dMeanMotion);
    tot += tmp1;
  }

  phi = phi + tot;

  return fmod(phi,2.0*PI);
}

double fdCBPZBinary(BODY *body,SYSTEM *system,int *iaBody)
{
  int iBody = iaBody[0];

  // Note: Assume all phase values (phi, psi, etc...) are 0
  // Fine because they are arbitrary offsets
  double dXi = 0.0;
  double dTime = body[iBody].dAge; // Time == Age of the body

  return body[iBody].dR0*body[iBody].dFreeInc*cos(body[iBody].dLL13V0*dTime + dXi);
}

double fdCBPRDotBinary(BODY *body,SYSTEM *system,int *iaBody)
{
  int iBody = iaBody[0];

  // Note: Assume all phase values (phi, psi, etc...) are 0
  // Fine because they are arbitrary offsets
  double dPsi = 0.0;
  double dPhi = 0.0;
  double dTime = body[iBody].dAge; // Time == Age of the body

  // Useful intermediate quantities
  double k0 = body[iBody].dLL13K0;
  double phi0 = fdPhi0(dTime,body[iBody].dLL13N0,0);
  double phi0_dot = body[iBody].dLL13N0;
  double M_dot = body[1].dMeanMotion;
  double M = fdMeanAnomaly(body[1].dMeanMotion,dTime,0);
  double varpi = body[1].dLongP;//body[0].dLongA + body[0].dArgP;

  double tmp1 = k0*body[iBody].dFreeEcc*sin(k0*dTime + dPsi) + fdC0(body,iBody)*sin(M)*M_dot;

  double tmp2 = 0.0; // Total sum
  double tmp3 = 0.0; // Intermediate sum for each k
  int k;
  for(k = 1; k < K_MAX; k++)
  {
    tmp3 = -fdC0k(k,body,iBody)*sin(k*(phi0-M-varpi))*k*(phi0_dot-M_dot);
    tmp3 -= fdCPk(k,body,iBody)*sin(k*(phi0-varpi)-(k+1.)*M)*(k*phi0_dot -(k+1.)*M_dot);
    tmp3 -= fdCMk(k,body,iBody)*sin(k*(phi0-varpi)-(k-1.)*M)*(k*phi0_dot -(k-1.)*M_dot);
    tmp2 += tmp3;
  }

  return body[iBody].dR0*(tmp1-tmp2);
}

double fdCBPPhiDotBinary(BODY *body,SYSTEM *system,int *iaBody)
{
  int iBody = iaBody[0];

  // Set arbitrary phase constants to 0
  double dPsi = 0.0;
  double dPhi = 0.0;
  double dTime = body[iBody].dAge; // Time == Age of the body

  // Useful intermediate quantities
  double k0 = body[iBody].dLL13K0;
  double phi0 = fdPhi0(dTime,body[iBody].dLL13N0,0);
  double phi0_dot = body[iBody].dLL13N0;
  double n0 = body[iBody].dLL13N0;
  double n = body[1].dMeanMotion;
  double M_dot = n;
  double M = fdMeanAnomaly(body[1].dMeanMotion,dTime,0);
  double varpi = body[1].dLongP;//body[0].dLongA + body[0].dArgP;

  double tmp1 = n0 + 2.0*n0*body[iBody].dFreeEcc*cos(k0*dTime + dPsi) + (n0/n)*fdD0(body,iBody)*cos(M)*M_dot;

  double tmp2 = 0.0; // Total loop sum
  double tmp3 = 0.0; // Intermediate loop sum
  int k;
  for(k = 1; k < K_MAX; k++)
  {
    tmp3 = (n0/(k*(n0-n)))*fdDk0(k,body,iBody)*cos(k*(phi0-M-varpi))*k*(phi0_dot-M_dot);
    tmp3 += (n0*fdDPk(k,body,iBody)/(k*n0 - (k+1.)*n))*cos(k*(phi0-varpi)-(k+1.)*M)*(k*phi0_dot-(k+1.)*M_dot);
    tmp3 += (n0*fdDMk(k,body,iBody)/(k*n0 - (k-1.)*n))*cos(k*(phi0-varpi)-(k-1.)*M)*(k*phi0_dot-(k-1.)*M_dot);
    tmp2 += tmp3;
  }

  return (tmp1+tmp2);
}

double fdCBPZDotBinary(BODY *body,SYSTEM *system,int *iaBody)
{
  int iBody = iaBody[0];

  // Set arbitrary phase constants to 0 for simplicity
  double dXi = 0.0;
  double dTime = body[iBody].dAge; // Time == Age of the body

  return -body[iBody].dR0*body[iBody].dFreeInc*sin(body[iBody].dLL13V0*dTime + dXi)*body[iBody].dLL13V0;
}

/* Misc Functions */

/* Compute the exact flux (as close to exact as you want)
 * received by the CBP from the 2 stars averaged over 1 CBP orbit
 * Assumes binary orb elements don't vary much over 1 CBP orbit
 */
double fdFluxExactBinary(BODY *body,SYSTEM *system,int *iaBody, double L0, double L1)
{
  // Define/init all variables 
  int iBody = iaBody[0];
  double period = 2.0*PI/body[iBody].dMeanMotion; // Period of CBP orbit
  double flux = 0.0;
  double step = period/FLUX_INT_MAX;
  double trueAnomaly = 0.0;
  double meanAnomaly = 0.0;
  double eccAnomaly = 0.0;
  double radius = 0.0;
  double r1 = 0.0, r2 = 0.0;
  double x1 = 0.0, x2 = 0.0, y1 = 0.0, y2 = 0.0; // Cartesian position of binary in plane
  double x = 0.0, y = 0.0; // Cartesian positon of cbp
  double r = 0.0, phi = 0.0, z = 0.0; // Cyl position of cbp
  double dAge = body[iBody].dAge; // Save body[iaBody[0]].dAge so this function doesn't actually change it

  // Loop over steps in CBP orbit, add flux due to each star at each step
  int i;
  for(i = 0; i < FLUX_INT_MAX; i++)
  {
    // Get binary position by solving kepler's eqn
    // mean -> ecc -> true anomaly
    meanAnomaly = body[1].dMeanMotion*body[iBody].dAge; // ignore arbitrary phase offset since P_bin << P_cbp for LL13 theory
    eccAnomaly = fdMeanToEccentric(meanAnomaly,body[1].dEcc);
    trueAnomaly = fdEccToTrue(eccAnomaly,body[1].dEcc);

    radius = body[1].dSemi * (1.0 - body[1].dEcc*body[1].dEcc);
    radius /= (1.0 + body[1].dEcc*cos(trueAnomaly));
    
    // Radial position of each star
    r1 = body[1].dMass*radius/(body[0].dMass+body[1].dMass);
    r2 = -body[0].dMass*radius/(body[0].dMass+body[1].dMass);

    // Cartesian position of stars (in plane, no z)
    x1 = r1*cos(trueAnomaly);
    x2 = r2*cos(trueAnomaly);
    y1 = r1*sin(trueAnomaly);
    y2 = r2*sin(trueAnomaly);

    // Compute CBP positon in cylindrical coords
    r = fdCBPRBinary(body,system,iaBody);
    phi = fdCBPPhiBinary(body,system,iaBody);
    z = fdCBPZBinary(body,system,iaBody);

    // Convert to cartesian
    x = r*cos(phi);
    y = r*sin(phi);
    // z = z

    // Compute squared distances from each star to CBP
    r1 = pow(x-x1,2.) + pow(y-y1,2.) + z*z;
    r2 = pow(x-x2,2.) + pow(y-y2,2.) + z*z;

    // Compute fluxes due to each star
    flux += (L0/(4.0*PI*r1) + L1/(4.0*PI*r2));
  
    // Increment body's age aka time
    body[iBody].dAge += step;
  }

  // Reset the body's dAge to what it was at the start of this function!
  body[iBody].dAge = dAge;

  return flux/FLUX_INT_MAX;
}

void binaryDebug(BODY * body)
{
  fprintf(stderr,"binary debug information:\n");
  fprintf(stderr,"r0: %lf.\n",body[2].dR0/AUCM);
  fprintf(stderr,"nk: %lf.\n",body[2].dMeanMotion*YEARSEC);
  fprintf(stderr,"n0: %lf.\n",body[2].dLL13N0*YEARSEC);
  fprintf(stderr,"k0: %lf.\n",body[2].dLL13K0*YEARSEC);
  fprintf(stderr,"v0: %lf.\n",body[2].dLL13V0*YEARSEC);
  
  fprintf(stderr,"n0/nk: %lf.\n",body[2].dLL13N0/body[2].dMeanMotion);
  fprintf(stderr,"k0/nk: %lf.\n",body[2].dLL13K0/body[2].dMeanMotion);
  fprintf(stderr,"v0/nk: %lf.\n",body[2].dLL13V0/body[2].dMeanMotion);

  fprintf(stderr,"C0: %lf.\n",fdC0(body,2));
  fprintf(stderr,"C01: %lf.\n",fdC0k(1,body,2));
  fprintf(stderr,"C02: %lf.\n",fdC0k(2,body,2));
  fprintf(stderr,"C03: %lf.\n",fdC0k(3,body,2));

  fprintf(stderr,"C1+: %lf.\n",fdCPk(1,body,2));
  fprintf(stderr,"C2+: %lf.\n",fdCPk(2,body,2));
  fprintf(stderr,"C3+: %lf.\n",fdCPk(3,body,2));

  fprintf(stderr,"C1-: %lf.\n",fdCMk(1,body,2));
  fprintf(stderr,"C2-: %lf.\n",fdCMk(2,body,2));
  fprintf(stderr,"C3-: %lf.\n",fdCMk(3,body,2));

}
