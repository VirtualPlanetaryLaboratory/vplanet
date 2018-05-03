/********************** DISTRES.C **********************/
/*
 * Russell Deitrick, February 2017
 *
 * Subroutines that control the integration of the resoanant orbital model.
*/

#include <stdio.h>
#include <math.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "vplanet.h"
#include "options.h"
#include "output.h"

void BodyCopyDistRes(BODY *dest,BODY *src,int iTideModel,int iNumBodies,int iBody) {
  int iIndex,iPert;

  dest[iBody].dMeanL = src[iBody].dMeanL;
//   dest[iBody].dMeanA = src[iBody].dMeanA;
}

void InitializeBodyDistRes(BODY *body,CONTROL *control,UPDATE *update,int iBody,int iModule) {

}

void InitializeUpdateTmpBodyDistRes(BODY *body,CONTROL *control,UPDATE *update,int iBody) {

}

/**************** DISTRES options ********************/

void ReadMeanL(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
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

    body[iFile-1].dMeanL = dTmp;
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else
    if (iFile > 0)
      body[iFile-1].dMeanL = options->dDefault;
}

void ReadResAvg(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  int lTmp=-1,bTmp;
  AddOptionBool(files->Infile[iFile].cIn,options->cName,&bTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {
    CheckDuplication(files,options,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);
    /* Option was found */
    system->bResAvg = bTmp;
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else
    AssignDefaultInt(options,&system->bResAvg,files->iNumInputs);
}

// void ReadMeanA(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
//   /* This parameter cannot exist in the primary file */
//   int lTmp=-1;
//   double dTmp;
//
//   AddOptionDouble(files->Infile[iFile].cIn,options->cName,&dTmp,&lTmp,control->Io.iVerbose);
//   if (lTmp >= 0) {
//     NotPrimaryInput(iFile,options->cName,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);
//     if (control->Units[iFile].iAngle == 0) {
//       if (dTmp < 0 || dTmp > 2*PI) {
//         if (control->Io.iVerbose >= VERBERR)
//             fprintf(stderr,"ERROR: %s must be in the range [0,2*PI].\n",options->cName);
//         LineExit(files->Infile[iFile].cIn,lTmp);
//       }
//     } else {
//       if (dTmp < 0 || dTmp > 360) {
//         if (control->Io.iVerbose >= VERBERR)
//             fprintf(stderr,"ERROR: %s must be in the range [0,360].\n",options->cName);
//         LineExit(files->Infile[iFile].cIn,lTmp);
//       }
//       /* Change to radians */
//       dTmp *= DEGRAD;
//     }
//
//     body[iFile-1].dMeanA = dTmp;
//     UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
//   } else
//     if (iFile > 0)
//       body[iFile-1].dMeanA = options->dDefault;
// }

void InitializeOptionsDistRes(OPTIONS *options,fnReadOption fnRead[]) {
  sprintf(options[OPT_MEANL].cName,"dMeanL");
  sprintf(options[OPT_MEANL].cDescr,"Mean Longitude");
  sprintf(options[OPT_MEANL].cDefault,"0");
  options[OPT_MEANL].dDefault = 0.0;
  options[OPT_MEANL].iType = 2;
  options[OPT_MEANL].iMultiFile = 1;
  fnRead[OPT_MEANL] = &ReadMeanL;

  sprintf(options[OPT_RESAVG].cName,"bResAvg");
  sprintf(options[OPT_RESAVG].cDescr,"Average over resonant arguments (suitable for circulation only)");
  sprintf(options[OPT_RESAVG].cDefault,"0");
  options[OPT_RESAVG].dDefault = 0;
  options[OPT_RESAVG].iType = 0;
  options[OPT_RESAVG].iMultiFile = 0;
  fnRead[OPT_RESAVG] = &ReadResAvg;

//   sprintf(options[OPT_MEANA].cName,"dMeanA");
//   sprintf(options[OPT_MEANA].cDescr,"Mean Anomaly");
//   sprintf(options[OPT_MEANA].cDefault,"0");
//   options[OPT_MEANA].dDefault = 0.0;
//   options[OPT_MEANA].iType = 2;
//   options[OPT_MEANA].iMultiFile = 1;
//   fnRead[OPT_MEANA] = &ReadMeanA;
}

void ReadOptionsDistRes(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,fnReadOption fnRead[],int iBody) {
  int iOpt;

  for (iOpt=OPTSTARTDISTRES;iOpt<OPTENDDISTRES;iOpt++) {
      if (options[iOpt].iType != -1) {
        fnRead[iOpt](body,control,files,&options[iOpt],system,iBody+1);
      }
  }
}


/******************* Verify DISTRES ******************/

/* In the following, iBody is the current body number that is getting assigned,
   iPert counts the number of bodies perturbing iBody, and iBodyPert is the
   body number of the current perturbing body. */

/* The indexing gets a bit confusing here. iPert = 0 to iGravPerts-1 correspond
   to all perturbing planets,  iPert = iGravPerts corresponds to the stellar general
   relativistic correction, if applied. */

void VerifyMeanLongAnom(BODY *body,CONTROL *control,OPTIONS *options,char cFile[],int iBody,int iVerbose) {
  /* First see if mean longitude set and mean anomaly not set, i.e. the user input the default parameters */
  if (options[OPT_MEANL].iLine[iBody+1] > -1 && options[OPT_MEANA].iLine[iBody+1] == -1) {
    body[iBody].dMeanA = body[iBody].dMeanL - body[iBody].dLongP;
    while (body[iBody].dMeanA < 0) {
      body[iBody].dMeanA += 2*PI;
    }
    return;
  }

  /* If none are set, raise error */
  if (options[OPT_MEANA].iLine[iBody+1] == -1 && options[OPT_MEANL].iLine[iBody+1] == -1) {
    if (iVerbose >= VERBERR)
      fprintf(stderr,"ERROR: Must set one of %s and %s in File: %s.\n",options[OPT_MEANA].cName,options[OPT_MEANL].cName, cFile);
    exit(EXIT_INPUT);
  }

  /* Were both set? */
  if (options[OPT_MEANA].iLine[iBody+1] > -1 && options[OPT_MEANL].iLine[iBody+1] > -1) {
    if (iVerbose >= VERBERR)
      fprintf(stderr,"ERROR: Cannot set both of %s and %s in File: %s.\n",options[OPT_MEANA].cName,options[OPT_MEANL].cName, cFile);
    exit(EXIT_INPUT);
  }

  /* Was MeanA set? */
  if (options[OPT_MEANA].iLine[iBody+1] > -1) {
    body[iBody].dMeanL = body[iBody].dMeanA + body[iBody].dLongP;
    while (body[iBody].dMeanL > 2*PI) {
      body[iBody].dMeanL -= 2*PI;
    }
    return;
  }
}

void InitializeSemiDistRes(BODY *body,UPDATE *update,int iBody,int iPert) {
  update[iBody].iaType[update[iBody].iSemi][update[iBody].iaSemiDistRes[iPert]] = 1;
  update[iBody].padDSemiDtDistRes[iPert] = &update[iBody].daDerivProc[update[iBody].iSemi][update[iBody].iaSemiDistRes[iPert]];
  update[iBody].iNumBodies[update[iBody].iSemi][update[iBody].iaSemiDistRes[iPert]]=2;
  update[iBody].iaBody[update[iBody].iSemi][update[iBody].iaSemiDistRes[iPert]] = malloc(update[iBody].iNumBodies[update[iBody].iSemi][update[iBody].iaSemiDistRes[iPert]]*sizeof(int));
  update[iBody].iaBody[update[iBody].iSemi][update[iBody].iaSemiDistRes[iPert]][0] = iBody;
  update[iBody].iaBody[update[iBody].iSemi][update[iBody].iaSemiDistRes[iPert]][1] = body[iBody].iaGravPerts[iPert];
}

void InitializeMeanLDistRes(BODY *body,UPDATE *update,int iBody,int iPert) {
  update[iBody].iaType[update[iBody].iMeanL][update[iBody].iaMeanLDistRes[iPert]] = 2;
  update[iBody].padDMeanLDtDistRes[iPert] = &update[iBody].daDerivProc[update[iBody].iMeanL][update[iBody].iaMeanLDistRes[iPert]];
  update[iBody].iNumBodies[update[iBody].iMeanL][update[iBody].iaMeanLDistRes[iPert]]=2;
  update[iBody].iaBody[update[iBody].iMeanL][update[iBody].iaMeanLDistRes[iPert]] = malloc(update[iBody].iNumBodies[update[iBody].iMeanL][update[iBody].iaMeanLDistRes[iPert]]*sizeof(int));
  update[iBody].iaBody[update[iBody].iMeanL][update[iBody].iaMeanLDistRes[iPert]][0] = iBody;
  update[iBody].iaBody[update[iBody].iMeanL][update[iBody].iaMeanLDistRes[iPert]][1] = body[iBody].iaGravPerts[iPert];
}

void InitializeHeccDistRes(BODY *body,UPDATE *update,int iBody,int iPert) {
  update[iBody].iaType[update[iBody].iHecc][update[iBody].iaHeccDistRes[iPert]] = 2;
  update[iBody].padDHeccDtDistRes[iPert] = &update[iBody].daDerivProc[update[iBody].iHecc][update[iBody].iaHeccDistRes[iPert]];
  update[iBody].iNumBodies[update[iBody].iHecc][update[iBody].iaHeccDistRes[iPert]]=2;
  update[iBody].iaBody[update[iBody].iHecc][update[iBody].iaHeccDistRes[iPert]] = malloc(update[iBody].iNumBodies[update[iBody].iHecc][update[iBody].iaHeccDistRes[iPert]]*sizeof(int));
  update[iBody].iaBody[update[iBody].iHecc][update[iBody].iaHeccDistRes[iPert]][0] = iBody;
  update[iBody].iaBody[update[iBody].iHecc][update[iBody].iaHeccDistRes[iPert]][1] = body[iBody].iaGravPerts[iPert];
}

void InitializeKeccDistRes(BODY *body,UPDATE *update,int iBody,int iPert) {
  update[iBody].iaType[update[iBody].iKecc][update[iBody].iaKeccDistRes[iPert]] = 2;
  update[iBody].padDKeccDtDistRes[iPert] = &update[iBody].daDerivProc[update[iBody].iKecc][update[iBody].iaKeccDistRes[iPert]];
  update[iBody].iNumBodies[update[iBody].iKecc][update[iBody].iaKeccDistRes[iPert]]=2;
  update[iBody].iaBody[update[iBody].iKecc][update[iBody].iaKeccDistRes[iPert]] = malloc(update[iBody].iNumBodies[update[iBody].iKecc][update[iBody].iaKeccDistRes[iPert]]*sizeof(int));
  update[iBody].iaBody[update[iBody].iKecc][update[iBody].iaKeccDistRes[iPert]][0] = iBody;
  update[iBody].iaBody[update[iBody].iKecc][update[iBody].iaKeccDistRes[iPert]][1] = body[iBody].iaGravPerts[iPert];
}

void InitializePincDistRes(BODY *body,UPDATE *update,int iBody,int iPert) {
  update[iBody].iaType[update[iBody].iPinc][update[iBody].iaPincDistRes[iPert]] = 2;
  update[iBody].padDPincDtDistRes[iPert] = &update[iBody].daDerivProc[update[iBody].iPinc][update[iBody].iaPincDistRes[iPert]];
  update[iBody].iNumBodies[update[iBody].iPinc][update[iBody].iaPincDistRes[iPert]]=2;
  update[iBody].iaBody[update[iBody].iPinc][update[iBody].iaPincDistRes[iPert]] = malloc(update[iBody].iNumBodies[update[iBody].iPinc][update[iBody].iaPincDistRes[iPert]]*sizeof(int));
  update[iBody].iaBody[update[iBody].iPinc][update[iBody].iaPincDistRes[iPert]][0] = iBody;
  update[iBody].iaBody[update[iBody].iPinc][update[iBody].iaPincDistRes[iPert]][1] = body[iBody].iaGravPerts[iPert];
}

void InitializeQincDistRes(BODY *body,UPDATE *update,int iBody,int iPert) {
  update[iBody].iaType[update[iBody].iQinc][update[iBody].iaQincDistRes[iPert]] = 2;
  update[iBody].padDQincDtDistRes[iPert] = &update[iBody].daDerivProc[update[iBody].iQinc][update[iBody].iaQincDistRes[iPert]];
  update[iBody].iNumBodies[update[iBody].iQinc][update[iBody].iaQincDistRes[iPert]]=2;
  update[iBody].iaBody[update[iBody].iQinc][update[iBody].iaQincDistRes[iPert]] = malloc(update[iBody].iNumBodies[update[iBody].iQinc][update[iBody].iaQincDistRes[iPert]]*sizeof(int));
  update[iBody].iaBody[update[iBody].iQinc][update[iBody].iaQincDistRes[iPert]][0] = iBody;
  update[iBody].iaBody[update[iBody].iQinc][update[iBody].iaQincDistRes[iPert]][1] = body[iBody].iaGravPerts[iPert];
}

void InitializeResAvg(BODY *body,EVOLVE *evolve,SYSTEM *system) {
  int iRes;

  system->daDistCos = malloc(RESNUM*sizeof(double*));
  system->daDistSin = malloc(RESNUM*sizeof(double*));
  system->daDistSec = malloc(RESNUM*sizeof(double*));
  system->daLibrFreq2 = malloc(RESNUM*sizeof(double*));
  system->daCircFreq = malloc(RESNUM*sizeof(double*));

  for (iRes=0;iRes<RESNUM;iRes++) {
    system->daDistCos[iRes] = malloc(fniNchoosek(evolve->iNumBodies-1,2)*sizeof(double));
    system->daDistSin[iRes] = malloc(fniNchoosek(evolve->iNumBodies-1,2)*sizeof(double));
    system->daDistSec[iRes] = malloc(fniNchoosek(evolve->iNumBodies-1,2)*sizeof(double));
    system->daLibrFreq2[iRes] = malloc(fniNchoosek(evolve->iNumBodies-1,2)*sizeof(double));
    system->daCircFreq[iRes] = malloc(fniNchoosek(evolve->iNumBodies-1,2)*sizeof(double));
  }
}

void CalcDistResFreqs(BODY *body, EVOLVE *evolve, SYSTEM *system) {
  int iBody, jBody, iRes, iPair;

  system->bResAvg = 0;   //dumb trick to bypass the if statement in fdCosLambda & fdSinLambda
  for (iRes=0;iRes<RESNUM;iRes++) {
    for (iBody=1;iBody<evolve->iNumBodies;iBody++) {
      for (jBody=iBody+1;jBody<evolve->iNumBodies;jBody++) {
        iPair = system->iaLaplaceN[iBody][jBody];
        if (system->iaResIndex[iRes][iPair]!=-1) {
          system->daDistCos[iRes][iPair] = fdDistfCos(body,system,iBody,jBody, system->iaResIndex[iRes][iPair],system->iaResOrder[iRes]);
          system->daDistSin[iRes][iPair] = fdDistfSin(body,system,iBody,jBody, system->iaResIndex[iRes][iPair],system->iaResOrder[iRes]);
          system->daDistSec[iRes][iPair] = sqrt(system->daDistCos[iRes][iPair]*\
              system->daDistCos[iRes][iPair]+system->daDistSin[iRes][iPair]*\
              system->daDistSin[iRes][iPair]);
          system->daLibrFreq2[iRes][iPair] = fdLibrFreq2(body,system,iBody,jBody,iRes);
          system->daCircFreq[iRes][iPair] = fdCircFreq(body,system,iBody,jBody,iRes);
        }
      }
    }
  }
  system->bResAvg = 1;
}

void VerifyDistResDerivatives(BODY *body,CONTROL *control,UPDATE *update,fnUpdateVariable ***fnUpdate,int iBody) {
  int iPert;

  if (iBody > 0) {
    for (iPert=0;iPert<body[iBody].iGravPerts;iPert++) {
      /* semi major axis */
      fnUpdate[iBody][update[iBody].iSemi][update[iBody].iaSemiDistRes[iPert]] = &fdDistResRD2DaDt;

      /* mean longitude */
      fnUpdate[iBody][update[iBody].iMeanL][update[iBody].iaMeanLDistRes[iPert]] = &fdDistResRD2DlDt;

      /* h = Ecc*sin(LongP) */
      fnUpdate[iBody][update[iBody].iHecc][update[iBody].iaHeccDistRes[iPert]] = &fdDistResRD2DhDt;

      /* k = Ecc*cos(LongP) */
      fnUpdate[iBody][update[iBody].iKecc][update[iBody].iaKeccDistRes[iPert]] = &fdDistResRD2DkDt;

      /* p = s*sin(LongA) */
      fnUpdate[iBody][update[iBody].iPinc][update[iBody].iaPincDistRes[iPert]] = &fdDistResRD2DpDt;

      /* q = s*cos(LongA) */
      fnUpdate[iBody][update[iBody].iQinc][update[iBody].iaQincDistRes[iPert]] = &fdDistResRD2DqDt;
    }
  }
}

void VerifyDistRes(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,OUTPUT *output,SYSTEM *system,UPDATE *update,fnUpdateVariable ***fnUpdate,int iBody,int iModule) {
  int iPert, jBody, j, jj, iRes, i;
  double dPerRat;

  VerifyMeanLongAnom(body,control,options,files->Infile[iBody+1].cIn,iBody,control->Io.iVerbose);

  if (iBody == 1) {
    for (j=26;j<LAPLNUM;j++) {
        system->fnLaplaceF[j] = malloc(1*sizeof(fnLaplaceFunction));
        system->fnLaplaceDeriv[j] = malloc(1*sizeof(fnLaplaceFunction));
    }
    system->fnLaplaceF[26][0] = &fdSemiMajAxF27;
    system->fnLaplaceF[27][0] = &fdSemiMajAxF28;
    system->fnLaplaceF[28][0] = &fdSemiMajAxF29;
    system->fnLaplaceF[29][0] = &fdSemiMajAxF30;
    system->fnLaplaceF[30][0] = &fdSemiMajAxF31;
    system->fnLaplaceF[31][0] = &fdSemiMajAxF32;
    system->fnLaplaceF[32][0] = &fdSemiMajAxF33;
    system->fnLaplaceF[33][0] = &fdSemiMajAxF34;
    system->fnLaplaceF[34][0] = &fdSemiMajAxF35;
    system->fnLaplaceF[35][0] = &fdSemiMajAxF36;
    system->fnLaplaceF[36][0] = &fdSemiMajAxF37;
    system->fnLaplaceF[37][0] = &fdSemiMajAxF38;
    system->fnLaplaceF[38][0] = &fdSemiMajAxF39;
    system->fnLaplaceF[39][0] = &fdSemiMajAxF40;
    system->fnLaplaceF[40][0] = &fdSemiMajAxF41;
    system->fnLaplaceF[41][0] = &fdSemiMajAxF42;
    system->fnLaplaceF[42][0] = &fdSemiMajAxF43;
    system->fnLaplaceF[43][0] = &fdSemiMajAxF44;
    system->fnLaplaceF[44][0] = &fdSemiMajAxF45;
    system->fnLaplaceF[48][0] = &fdSemiMajAxF49;
    system->fnLaplaceF[52][0] = &fdSemiMajAxF53;
    system->fnLaplaceF[56][0] = &fdSemiMajAxF57;
    system->fnLaplaceF[61][0] = &fdSemiMajAxF62;
    system->fnLaplaceF[81][0] = &fdSemiMajAxF82;
    system->fnLaplaceF[82][0] = &fdSemiMajAxF83;
    system->fnLaplaceF[83][0] = &fdSemiMajAxF84;
    system->fnLaplaceF[84][0] = &fdSemiMajAxF85;
    system->fnLaplaceF[85][0] = &fdSemiMajAxF86;
    system->fnLaplaceF[86][0] = &fdSemiMajAxF87;
    system->fnLaplaceF[87][0] = &fdSemiMajAxF88;
    system->fnLaplaceF[88][0] = &fdSemiMajAxF89;

    system->fnLaplaceDeriv[26][0] = &fdDSemiF27Dalpha;
    system->fnLaplaceDeriv[27][0] = &fdDSemiF28Dalpha;
    system->fnLaplaceDeriv[28][0] = &fdDSemiF29Dalpha;
    system->fnLaplaceDeriv[29][0] = &fdDSemiF30Dalpha;
    system->fnLaplaceDeriv[30][0] = &fdDSemiF31Dalpha;
    system->fnLaplaceDeriv[31][0] = &fdDSemiF32Dalpha;
    system->fnLaplaceDeriv[32][0] = &fdDSemiF33Dalpha;
    system->fnLaplaceDeriv[33][0] = &fdDSemiF34Dalpha;
    system->fnLaplaceDeriv[34][0] = &fdDSemiF35Dalpha;
    system->fnLaplaceDeriv[35][0] = &fdDSemiF36Dalpha;
    system->fnLaplaceDeriv[36][0] = &fdDSemiF37Dalpha;
    system->fnLaplaceDeriv[37][0] = &fdDSemiF38Dalpha;
    system->fnLaplaceDeriv[38][0] = &fdDSemiF39Dalpha;
    system->fnLaplaceDeriv[39][0] = &fdDSemiF40Dalpha;
    system->fnLaplaceDeriv[40][0] = &fdDSemiF41Dalpha;
    system->fnLaplaceDeriv[41][0] = &fdDSemiF42Dalpha;
    system->fnLaplaceDeriv[42][0] = &fdDSemiF43Dalpha;
    system->fnLaplaceDeriv[43][0] = &fdDSemiF44Dalpha;
    system->fnLaplaceDeriv[44][0] = &fdDSemiF45Dalpha;
    system->fnLaplaceDeriv[48][0] = &fdDSemiF49Dalpha;
    system->fnLaplaceDeriv[52][0] = &fdDSemiF53Dalpha;
    system->fnLaplaceDeriv[56][0] = &fdDSemiF57Dalpha;
    system->fnLaplaceDeriv[61][0] = &fdDSemiF62Dalpha;
    system->fnLaplaceDeriv[81][0] = &fdDSemiF82Dalpha;
    system->fnLaplaceDeriv[82][0] = &fdDSemiF83Dalpha;
    system->fnLaplaceDeriv[83][0] = &fdDSemiF84Dalpha;
    system->fnLaplaceDeriv[84][0] = &fdDSemiF85Dalpha;
    system->fnLaplaceDeriv[85][0] = &fdDSemiF86Dalpha;
    system->fnLaplaceDeriv[86][0] = &fdDSemiF87Dalpha;
    system->fnLaplaceDeriv[87][0] = &fdDSemiF88Dalpha;
    system->fnLaplaceDeriv[88][0] = &fdDSemiF89Dalpha;

    /* set up indexes for resonance */
    system->iaResIndex = malloc(RESNUM*sizeof(int*));
    system->iaResIndex[0] = malloc(fniNchoosek(control->Evolve.iNumBodies-1,2)*sizeof(int));
    system->iaResIndex[1] = malloc(fniNchoosek(control->Evolve.iNumBodies-1,2)*sizeof(int));
    system->iaResIndex[2] = malloc(fniNchoosek(control->Evolve.iNumBodies-1,2)*sizeof(int));
    system->iaResIndex[3] = malloc(fniNchoosek(control->Evolve.iNumBodies-1,2)*sizeof(int));
    system->iaResIndex[4] = malloc(fniNchoosek(control->Evolve.iNumBodies-1,2)*sizeof(int));
    system->iaResIndex[5] = malloc(fniNchoosek(control->Evolve.iNumBodies-1,2)*sizeof(int));
    system->iaResIndex[6] = malloc(fniNchoosek(control->Evolve.iNumBodies-1,2)*sizeof(int));
    system->iaResIndex[7] = malloc(fniNchoosek(control->Evolve.iNumBodies-1,2)*sizeof(int));
    system->iaResIndex[8] = malloc(fniNchoosek(control->Evolve.iNumBodies-1,2)*sizeof(int));
    system->iaResIndex[9] = malloc(fniNchoosek(control->Evolve.iNumBodies-1,2)*sizeof(int));
    system->iaResIndex[10] = malloc(fniNchoosek(control->Evolve.iNumBodies-1,2)*sizeof(int));

    system->iaResOrder = malloc(RESNUM*sizeof(int));
    system->iaResOrder[0] = 1;
    system->iaResOrder[1] = 1;
    system->iaResOrder[2] = 2;
    system->iaResOrder[3] = 2;
    system->iaResOrder[4] = 2;
    system->iaResOrder[5] = 2;
    system->iaResOrder[6] = 3;
    system->iaResOrder[7] = 3;
    system->iaResOrder[8] = 3;
    system->iaResOrder[9] = 3;
    system->iaResOrder[10] = 1;

    /*------------------------------*/

    for (iRes=1;iRes<=RESMAX;iRes++) {
      system->daLaplaceC[iRes] = malloc(fniNchoosek(control->Evolve.iNumBodies-1,2)*sizeof(double*));
      system->daLaplaceD[iRes] = malloc(fniNchoosek(control->Evolve.iNumBodies-1,2)*sizeof(double*));
      system->daAlpha0[iRes] = malloc(fniNchoosek(control->Evolve.iNumBodies-1,2)*sizeof(double*));
      for (i=0;i<fniNchoosek(control->Evolve.iNumBodies-1,2);i++) {
        system->daLaplaceC[iRes][i] = malloc(LAPLNUM*sizeof(double));
        system->daLaplaceD[iRes][i] = malloc(LAPLNUM*sizeof(double));
        system->daAlpha0[iRes][i] = malloc(LAPLNUM*sizeof(double));
      }
    }

  }

  if (iBody > 0) {
    control->fnPropsAux[iBody][iModule] = &PropsAuxDistOrb;

    for (iPert=0;iPert<body[iBody].iGravPerts;iPert++) {
        /* semi major axis */
        InitializeSemiDistRes(body,update,iBody,iPert);

        /* mean longitude */
        InitializeMeanLDistRes(body,update,iBody,iPert);

        /* h = Ecc*sin(LongP) */
        InitializeHeccDistRes(body,update,iBody,iPert);

        /* k = Ecc*cos(LongP) */
        InitializeKeccDistRes(body,update,iBody,iPert);

        /* p = s*sin(LongA) */
        InitializePincDistRes(body,update,iBody,iPert);

        /* q = s*cos(LongA) */
        InitializeQincDistRes(body,update,iBody,iPert);

        jBody = body[iBody].iaGravPerts[iPert];
        for (iRes=1;iRes<=RESMAX;iRes++) {
//           iRes = system->iaResIndex[jj][system->iaLaplaceN[iBody][jBody]];

          for (j=26;j<LAPLNUM;j++) {
            if (j<=44 || j==48 || j==52 || j==56 || j==61 || j>=81) {
              if (body[iBody].dSemi < body[jBody].dSemi) {
                  system->iaLaplaceN[iBody][jBody] = fniCombCount(iBody,jBody,control->Evolve.iNumBodies-1);
                  system->daLaplaceC[iRes][system->iaLaplaceN[iBody][jBody]][j] = system->fnLaplaceF[j][0](body[iBody].dSemi/body[jBody].dSemi,iRes);
                  system->daLaplaceD[iRes][system->iaLaplaceN[iBody][jBody]][j] = system->fnLaplaceDeriv[j][0](body[iBody].dSemi/body[jBody].dSemi,iRes);
                  system->daAlpha0[iRes][system->iaLaplaceN[iBody][jBody]][j] = body[iBody].dSemi/body[jBody].dSemi;
              } else if (body[iBody].dSemi > body[jBody].dSemi) {
                  system->iaLaplaceN[iBody][jBody] = fniCombCount(jBody,iBody,control->Evolve.iNumBodies-1);
                  system->daLaplaceC[iRes][system->iaLaplaceN[iBody][jBody]][j] = system->fnLaplaceF[j][0](body[jBody].dSemi/body[iBody].dSemi,iRes);
                  system->daLaplaceD[iRes][system->iaLaplaceN[iBody][jBody]][j] = system->fnLaplaceDeriv[j][0](body[jBody].dSemi/body[iBody].dSemi,iRes);
                  system->daAlpha0[iRes][system->iaLaplaceN[iBody][jBody]][j] = body[jBody].dSemi/body[iBody].dSemi;
              }
            }
          }
        }
    }

  }

  if (iBody==control->Evolve.iNumBodies-1) {
    CheckResonance(body,&control->Evolve,system);
    if (system->bResAvg == 1) {
      InitializeResAvg(body,&control->Evolve,system);
      CalcDistResFreqs(body,&control->Evolve,system);
    }
  }

  control->fnForceBehavior[iBody][iModule]=&ForceBehaviorDistRes;
  control->Evolve.fnBodyCopy[iBody][iModule]=&BodyCopyDistRes;
}

/***************** DISTRES Update *****************/
void InitializeUpdateDistRes(BODY *body,UPDATE *update,int iBody) {
  if (iBody > 0) {
    if (update[iBody].iNumSemi == 0)
      update[iBody].iNumVars++;
    update[iBody].iNumSemi += body[iBody].iGravPerts;

    if (update[iBody].iNumMeanL == 0)
      update[iBody].iNumVars++;
    update[iBody].iNumMeanL += body[iBody].iGravPerts;

    if (update[iBody].iNumHecc == 0)
      update[iBody].iNumVars++;
    update[iBody].iNumHecc += body[iBody].iGravPerts;

    if (update[iBody].iNumKecc == 0)
      update[iBody].iNumVars++;
    update[iBody].iNumKecc += body[iBody].iGravPerts;

    if (update[iBody].iNumPinc == 0)
      update[iBody].iNumVars++;
    update[iBody].iNumPinc += body[iBody].iGravPerts;

    if (update[iBody].iNumQinc == 0)
      update[iBody].iNumVars++;
    update[iBody].iNumQinc += body[iBody].iGravPerts;
  }
}

void FinalizeUpdateHeccDistRes(BODY *body,UPDATE *update,int *iEqn,int iVar,int iBody,int iFoo) {
  int iPert;

  update[iBody].padDHeccDtDistRes = malloc(body[iBody].iGravPerts*sizeof(double*));
  update[iBody].iaHeccDistRes = malloc(body[iBody].iGravPerts*sizeof(int));
  for (iPert=0;iPert<body[iBody].iGravPerts;iPert++) {
    update[iBody].iaModule[iVar][*iEqn] = DISTRES;
    update[iBody].iaHeccDistRes[iPert] = (*iEqn)++;
  }
}

void FinalizeUpdateKeccDistRes(BODY *body,UPDATE *update,int *iEqn,int iVar,int iBody,int iFoo) {
  int iPert;

  update[iBody].padDKeccDtDistRes = malloc(body[iBody].iGravPerts*sizeof(double*));
  update[iBody].iaKeccDistRes = malloc(body[iBody].iGravPerts*sizeof(int));
  for (iPert=0;iPert<body[iBody].iGravPerts;iPert++) {
    update[iBody].iaModule[iVar][*iEqn] = DISTRES;
    update[iBody].iaKeccDistRes[iPert] = (*iEqn)++;
  }
}

void FinalizeUpdatePincDistRes(BODY *body,UPDATE *update,int *iEqn,int iVar,int iBody,int iFoo) {
  int iPert;

  update[iBody].padDPincDtDistRes = malloc(body[iBody].iGravPerts*sizeof(double*));
  update[iBody].iaPincDistRes = malloc(body[iBody].iGravPerts*sizeof(int));
  for (iPert=0;iPert<body[iBody].iGravPerts;iPert++) {
    update[iBody].iaModule[iVar][*iEqn] = DISTRES;
    update[iBody].iaPincDistRes[iPert] = (*iEqn)++;
  }
}

void FinalizeUpdateQincDistRes(BODY *body,UPDATE *update,int *iEqn,int iVar,int iBody,int iFoo) {
  int iPert;

  update[iBody].padDQincDtDistRes = malloc(body[iBody].iGravPerts*sizeof(double*));
  update[iBody].iaQincDistRes = malloc(body[iBody].iGravPerts*sizeof(int));
  for (iPert=0;iPert<body[iBody].iGravPerts;iPert++) {
    update[iBody].iaModule[iVar][*iEqn] = DISTRES;
    update[iBody].iaQincDistRes[iPert] = (*iEqn)++;
  }
}

void FinalizeUpdateSemiDistRes(BODY *body,UPDATE *update,int *iEqn,int iVar,int iBody,int iFoo) {
  int iPert;

  update[iBody].padDSemiDtDistRes = malloc(body[iBody].iGravPerts*sizeof(double*));
  update[iBody].iaSemiDistRes = malloc(body[iBody].iGravPerts*sizeof(int));
  for (iPert=0;iPert<body[iBody].iGravPerts;iPert++) {
    update[iBody].iaModule[iVar][*iEqn] = DISTRES;
    update[iBody].iaSemiDistRes[iPert] = (*iEqn)++;
  }
}

void FinalizeUpdateMeanLDistRes(BODY *body,UPDATE *update,int *iEqn,int iVar,int iBody,int iFoo) {
  int iPert;

  update[iBody].padDMeanLDtDistRes = malloc(body[iBody].iGravPerts*sizeof(double*));
  update[iBody].iaMeanLDistRes = malloc(body[iBody].iGravPerts*sizeof(int));
  for (iPert=0;iPert<body[iBody].iGravPerts;iPert++) {
    update[iBody].iaModule[iVar][*iEqn] = DISTRES;
    update[iBody].iaMeanLDistRes[iPert] = (*iEqn)++;
  }
}


/***************** DISTRES Halts *****************/

void CountHaltsDistRes(HALT *halt,int *iNumHalts) {

}

void VerifyHaltDistRes(BODY *body,CONTROL *control,OPTIONS *options,int iBody,int *iHalt) {

}


/************* DISTRES Outputs ******************/

void WriteMeanL(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  *dTmp = body[iBody].dMeanL;

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

void WriteDMeanLDtDistRes(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  double dDeriv;
  int iPert;

  /* Ensure that we don't overwrite derivative */
  dDeriv=0;
  for (iPert=0;iPert<body[iBody].iGravPerts;iPert++)
    dDeriv += *(update[iBody].padDMeanLDtDistRes[iPert]);

  *dTmp = dDeriv;

  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else {
    *dTmp *= fdUnitsTime(units->iTime);
    *dTmp /= fdUnitsAngle(units->iAngle);
//     fsUnitsAngle(units->iAngle,cUnit);
    fsUnitsAngRate(units,cUnit);
  }
}

void InitializeOutputDistRes(OUTPUT *output,fnWriteOutput fnWrite[]) {
  sprintf(output[OUT_MEANL].cName,"MeanL");
  sprintf(output[OUT_MEANL].cDescr,"Body's Mean Longitude");
  sprintf(output[OUT_MEANL].cNeg,"Deg");
  output[OUT_MEANL].bNeg = 1;
  output[OUT_MEANL].dNeg = 1./DEGRAD;
  output[OUT_MEANL].iNum = 1;
  output[OUT_MEANL].iModuleBit = DISTRES;
  fnWrite[OUT_MEANL] = &WriteMeanL;

  sprintf(output[OUT_DMEANLDTDISTRES].cName,"DMeanLDtDistRes");
  sprintf(output[OUT_DMEANLDTDISTRES].cDescr,"Body's dlambda/dt in DistRes");
  sprintf(output[OUT_DMEANLDTDISTRES].cNeg,"deg/yr");
  output[OUT_DMEANLDTDISTRES].bNeg = 1;
  output[OUT_DMEANLDTDISTRES].dNeg = YEARSEC/DEGRAD;
  output[OUT_DMEANLDTDISTRES].iNum = 1;
  output[OUT_DMEANLDTDISTRES].iModuleBit = DISTRES;
  fnWrite[OUT_DMEANLDTDISTRES] = &WriteDMeanLDtDistRes;
}

/************ DISTRES Logging Functions **************/

void LogOptionsDistRes(CONTROL *control, FILE *fp) {

  fprintf(fp,"-------- DISTRES Options -----\n\n");

}

void LogDistRes(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UPDATE *update,fnWriteOutput fnWrite[],FILE *fp) {
  int iOut;

  fprintf(fp,"\n----- DISTRES PARAMETERS ------\n");
  for (iOut=OUTSTARTDISTRES;iOut<OUTBODYSTARTDISTRES;iOut++) {
    if (output[iOut].iNum > 0)
      WriteLogEntry(body,control,&output[iOut],system,update,fnWrite[iOut],fp,0);
  }
}

void LogBodyDistRes(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UPDATE *update,fnWriteOutput fnWrite[],FILE *fp,int iBody) {
  int iOut;

  fprintf(fp,"----- DISTRES PARAMETERS (%s)------\n",body[iBody].cName);
  for (iOut=OUTBODYSTARTDISTRES;iOut<OUTENDDISTRES;iOut++) {
    if (output[iOut].iNum > 0)
      WriteLogEntry(body,control,&output[iOut],system,update,fnWrite[iOut],fp,iBody);
  }
}

/************* MODULE Functions ***********/

void AddModuleDistRes(MODULE *module,int iBody,int iModule) {

  module->iaModule[iBody][iModule] = DISTRES;

  module->fnInitializeUpdateTmpBody[iBody][iModule] = &InitializeUpdateTmpBodyDistRes;
  module->fnCountHalts[iBody][iModule] = &CountHaltsDistRes;
  module->fnLogBody[iBody][iModule] = &LogBodyDistRes;

  module->fnReadOptions[iBody][iModule] = &ReadOptionsDistRes;
  module->fnVerify[iBody][iModule] = &VerifyDistRes;
  module->fnVerifyDerivatives[iBody][iModule] = &VerifyDistResDerivatives;
  module->fnVerifyHalt[iBody][iModule] = &VerifyHaltDistRes;

  module->fnInitializeBody[iBody][iModule] = &InitializeBodyDistRes;
  module->fnInitializeUpdate[iBody][iModule] = &InitializeUpdateDistRes;
  module->fnInitializeOutput[iBody][iModule] = &InitializeOutputDistRes;
  module->fnFinalizeUpdateHecc[iBody][iModule] = &FinalizeUpdateHeccDistRes;
  module->fnFinalizeUpdateKecc[iBody][iModule] = &FinalizeUpdateKeccDistRes;
  module->fnFinalizeUpdatePinc[iBody][iModule] = &FinalizeUpdatePincDistRes;
  module->fnFinalizeUpdateQinc[iBody][iModule] = &FinalizeUpdateQincDistRes;
  module->fnFinalizeUpdateSemi[iBody][iModule] = &FinalizeUpdateSemiDistRes;
  module->fnFinalizeUpdateMeanL[iBody][iModule] = &FinalizeUpdateMeanLDistRes;
}

/************* DistRes Functions ************/
void PropsAuxDistRes(BODY *body,EVOLVE *evolve,UPDATE *update,int iBody) {
//   if (iBody > 0) {
//     body[iBody].dMeanA = body[iBody].dMeanL - body[iBody].dLongP
//   }
}

void ForceBehaviorDistRes(BODY *body,EVOLVE *evolve,IO *io,SYSTEM *system,UPDATE *update,fnUpdateVariable ***fnUpdate,int iBody,int iModule) {
}

void CheckResonance(BODY *body, EVOLVE *evolve, SYSTEM *system) {
  int iBody, jBody;
  double dPerRat;
  double dTol = 0.05;

  for (iBody=1;iBody<evolve->iNumBodies;iBody++) {
    for (jBody=iBody+1;jBody<evolve->iNumBodies;jBody++) {
      if (body[iBody].dSemi > body[jBody].dSemi) {
        dPerRat = pow(body[iBody].dSemi/body[jBody].dSemi,1.5);
      } else {
        dPerRat = pow(body[jBody].dSemi/body[iBody].dSemi,1.5);
      }
      // 2:1 resonance in the 0th and 4th place
      if (dPerRat > (1.0-dTol)*2.0 && dPerRat < (1.0+dTol)*2.0) {
        system->iaResIndex[0][system->iaLaplaceN[iBody][jBody]] = 2;
        system->iaResIndex[4][system->iaLaplaceN[iBody][jBody]] = 4;
      } else {
        system->iaResIndex[0][system->iaLaplaceN[iBody][jBody]] = -1;
        system->iaResIndex[4][system->iaLaplaceN[iBody][jBody]] = -1;
      }
      // 3:2 resonance in the 1th and 5th place
      if (dPerRat > (1.0-dTol)*1.5 && dPerRat < (1.0+dTol)*1.5) {
        system->iaResIndex[1][system->iaLaplaceN[iBody][jBody]] = 3;
        system->iaResIndex[5][system->iaLaplaceN[iBody][jBody]] = 6;
      } else {
        system->iaResIndex[1][system->iaLaplaceN[iBody][jBody]] = -1;
        system->iaResIndex[5][system->iaLaplaceN[iBody][jBody]] = -1;
      }
      // 3:1 resonance in the 2th place
      if (dPerRat > (1.0-dTol)*3.0 && dPerRat < (1.0+dTol)*3.0) {
        system->iaResIndex[2][system->iaLaplaceN[iBody][jBody]] = 3;
      } else {
        system->iaResIndex[2][system->iaLaplaceN[iBody][jBody]] = -1;
      }
      // 5:3 in the 3th place
      if (dPerRat > (1.0-dTol)*(5./3) && dPerRat < (1.0+dTol)*(5./3)) {
        system->iaResIndex[3][system->iaLaplaceN[iBody][jBody]] = 5;
      } else {
        system->iaResIndex[3][system->iaLaplaceN[iBody][jBody]] = -1;
      }
      // 4:1 in the 6th place
      if (dPerRat > (1.0-dTol)*(4.) && dPerRat < (1.0+dTol)*(4.)) {
        system->iaResIndex[6][system->iaLaplaceN[iBody][jBody]] = 4;
      } else {
        system->iaResIndex[6][system->iaLaplaceN[iBody][jBody]] = -1;
      }
      // 8:5 in the 7th place
      if (dPerRat > (1.0-dTol)*(8./5) && dPerRat < (1.0+dTol)*(8./5)) {
        system->iaResIndex[7][system->iaLaplaceN[iBody][jBody]] = 8;
      } else {
        system->iaResIndex[7][system->iaLaplaceN[iBody][jBody]] = -1;
      }
      // 5:2 in the 8th place
      if (dPerRat > (1.0-dTol)*(5./2) && dPerRat < (1.0+dTol)*(5./2)) {
        system->iaResIndex[8][system->iaLaplaceN[iBody][jBody]] = 5;
      } else {
        system->iaResIndex[8][system->iaLaplaceN[iBody][jBody]] = -1;
      }
      // 7:4 in the 9th place
      if (dPerRat > (1.0-dTol)*(7./4) && dPerRat < (1.0+dTol)*(7./4)) {
        system->iaResIndex[9][system->iaLaplaceN[iBody][jBody]] = 7;
      } else {
        system->iaResIndex[9][system->iaLaplaceN[iBody][jBody]] = -1;
      }
      // missed one! 4:3 in the 10th place
      if (dPerRat > (1.0-dTol)*(4./3) && dPerRat < (1.0+dTol)*(4./3)) {
        system->iaResIndex[10][system->iaLaplaceN[iBody][jBody]] = 4;
      } else {
        system->iaResIndex[10][system->iaLaplaceN[iBody][jBody]] = -1;
      }

    }
  }
}

/*
 * Semi-major axis functions
 */
void RecalcLaplaceDistRes(BODY *body,EVOLVE *evolve,SYSTEM *system,int iVerbose) {
  int iBody, iPert, jBody, j, jj, iRes;
  double alpha1, dalpha;

  j = 0;
  for (iBody=1;iBody<evolve->iNumBodies-1;iBody++) {
    for (jBody=iBody+1;jBody<evolve->iNumBodies;jBody++) {
      if (body[iBody].dSemi < body[jBody].dSemi) {
        alpha1 = body[iBody].dSemi/body[jBody].dSemi;
      } else if (body[iBody].dSemi > body[jBody].dSemi) {
        alpha1 = body[jBody].dSemi/body[iBody].dSemi;
      }

      for (iRes=1;iRes<=RESMAX;iRes++) {
        //iRes = system->iaResIndex[jj][system->iaLaplaceN[iBody][jBody]];
        for (j=26;j<LAPLNUM;j++) {
          if (j<=44 || j==48 || j==52 || j==56 || j==61 || j>=81) {
            dalpha = fabs(alpha1 - system->daAlpha0[iRes][system->iaLaplaceN[iBody][jBody]][j]);
            if (dalpha > fabs(system->dDfcrit/system->daLaplaceD[iRes][system->iaLaplaceN[iBody][jBody]][j])) {
                system->daLaplaceC[iRes][system->iaLaplaceN[iBody][jBody]][j] = system->fnLaplaceF[j][0](alpha1,iRes);
                system->daLaplaceD[iRes][system->iaLaplaceN[iBody][jBody]][j] = system->fnLaplaceDeriv[j][0](alpha1,iRes);
                system->daAlpha0[iRes][system->iaLaplaceN[iBody][jBody]][j] = alpha1;
//                 if (iVerbose > VERBPROG)
// 		printf("Laplace function %d recalculated for bodies (%d, %d) at %f years\n",j+1,iBody,jBody,evolve->dTime/YEARSEC);
            }
          }
        }
      }
    }
  }
}

/*--------- second derivs of secular terms ----*/
double fdDSemiF1D2alpha(double dAxRatio, int iIndexJ) {
  return 0.5*fndDerivLaplaceCoeff(2,A(iIndexJ));  //fill in soon!!
}

double fdDSemiF2D2alpha(double dAxRatio, int iIndexJ) {
  return 1./8*((6.-4.*iIndexJ*iIndexJ)*fndDerivLaplaceCoeff(2,A(iIndexJ))+\
    6*dAxRatio*fndDerivLaplaceCoeff(3,A(iIndexJ))+\
    dAxRatio*dAxRatio*fndDerivLaplaceCoeff(4,A(iIndexJ)));
}

double fdDSemiF3D2alpha(double dAxRatio, int iIndexJ) {
  return 0.25*(-2*(fndDerivLaplaceCoeff(1,B(abs(iIndexJ-1)))+fndDerivLaplaceCoeff(1,B(iIndexJ+1)))\
    -dAxRatio*dAxRatio*(fndDerivLaplaceCoeff(2,B(abs(iIndexJ-1)))+fndDerivLaplaceCoeff(2,B(iIndexJ+1))));
}

double fdDSemiF10D2alpha(double dAxRatio, int iIndexJ) {
  return 0.25*((-4.+6.*iIndexJ+4.*iIndexJ*iIndexJ)*fndDerivLaplaceCoeff(2,A(iIndexJ+1))\
    -6*dAxRatio*fndDerivLaplaceCoeff(3,A(iIndexJ+1))\
    -dAxRatio*dAxRatio*fndDerivLaplaceCoeff(4,A(iIndexJ+1)));
}

double fdDSemiF14D2alpha(double dAxRatio, int iIndexJ) {
  return (2*fndDerivLaplaceCoeff(1,B(iIndexJ+1))+dAxRatio*fndDerivLaplaceCoeff(2,B(iIndexJ+1)));
}

/*--------- f27 ---------------------*/
double fdSemiMajAxF27(double dAxRatio, int iIndexJ) {
  return 0.5*(-2.*iIndexJ*fndLaplaceCoeff(A(iIndexJ)) - dAxRatio*fndDerivLaplaceCoeff(1,A(iIndexJ)));
}

double fdDSemiF27Dalpha(double dAxRatio, int iIndexJ) {
  return 0.5*((-2.*iIndexJ-1.)*fndDerivLaplaceCoeff(1,A(iIndexJ))-dAxRatio*fndDerivLaplaceCoeff(2,A(iIndexJ)));
}

/*--------- f28 ---------------------*/
double fdSemiMajAxF28(double dAxRatio, int iIndexJ) {
  return 1./16*((2*iIndexJ-10*iIndexJ*iIndexJ+8*iIndexJ*iIndexJ*iIndexJ)*fndLaplaceCoeff(A(iIndexJ))\
      +(3-7*iIndexJ+4*iIndexJ*iIndexJ)*dAxRatio*fndDerivLaplaceCoeff(1,A(iIndexJ))\
      -(2+2*iIndexJ)*dAxRatio*dAxRatio*fndDerivLaplaceCoeff(2,A(iIndexJ))\
      -dAxRatio*dAxRatio*dAxRatio*fndDerivLaplaceCoeff(3,A(iIndexJ)));
}

double fdDSemiF28Dalpha(double dAxRatio, int iIndexJ) {
  return 1./16*((3-5*iIndexJ-6*iIndexJ*iIndexJ+8*iIndexJ*iIndexJ*iIndexJ)*fndDerivLaplaceCoeff(1,A(iIndexJ))\
      -(1+11*iIndexJ-4*iIndexJ*iIndexJ)*dAxRatio*fndDerivLaplaceCoeff(2,A(iIndexJ))\
      -(5+2*iIndexJ)*dAxRatio*dAxRatio*fndDerivLaplaceCoeff(3,A(iIndexJ))\
      -dAxRatio*dAxRatio*dAxRatio*fndDerivLaplaceCoeff(4,A(iIndexJ)));;
}

/*--------- f29 ---------------------*/
double fdSemiMajAxF29(double dAxRatio, int iIndexJ) {
  return 1./8*(8*iIndexJ*iIndexJ*iIndexJ*fndLaplaceCoeff(A(iIndexJ))\
      +(-2-4*iIndexJ+4*iIndexJ*iIndexJ)*dAxRatio*fndDerivLaplaceCoeff(1,A(iIndexJ))\
      -(4+2*iIndexJ)*dAxRatio*dAxRatio*fndDerivLaplaceCoeff(2,A(iIndexJ))\
      -dAxRatio*dAxRatio*dAxRatio*fndDerivLaplaceCoeff(3,A(iIndexJ)));
}

double fdDSemiF29Dalpha(double dAxRatio, int iIndexJ) {
  return 1./8*((-2-4*iIndexJ+4*iIndexJ*iIndexJ+8*iIndexJ*iIndexJ*iIndexJ)*fndDerivLaplaceCoeff(1,A(iIndexJ))\
      -(10+8*iIndexJ-4*iIndexJ*iIndexJ)*dAxRatio*fndDerivLaplaceCoeff(2,A(iIndexJ))\
      -(7+2*iIndexJ)*dAxRatio*dAxRatio*fndDerivLaplaceCoeff(3,A(iIndexJ))\
      -dAxRatio*dAxRatio*dAxRatio*fndDerivLaplaceCoeff(4,A(iIndexJ)));;
}

/*--------- f30 ---------------------*/
double fdSemiMajAxF30(double dAxRatio, int iIndexJ) {
  return 1./4*((1+2*iIndexJ)*dAxRatio*(fndLaplaceCoeff(B(abs(iIndexJ-1)))+fndLaplaceCoeff(B(iIndexJ+1)))\
      +dAxRatio*dAxRatio*(fndDerivLaplaceCoeff(1,B(abs(iIndexJ-1)))+fndDerivLaplaceCoeff(1,B(iIndexJ+1))));
}

double fdDSemiF30Dalpha(double dAxRatio, int iIndexJ) {
  return 1./4*((1+2*iIndexJ)*(fndLaplaceCoeff(B(abs(iIndexJ-1)))+fndLaplaceCoeff(B(iIndexJ+1)))\
  +(3+2*iIndexJ)*dAxRatio*(fndDerivLaplaceCoeff(1,B(abs(iIndexJ-1)))+fndDerivLaplaceCoeff(1,B(iIndexJ+1)))\
    +dAxRatio*dAxRatio*(fndDerivLaplaceCoeff(2,B(abs(iIndexJ-1)))+fndDerivLaplaceCoeff(2,B(iIndexJ+1))));
}

/*--------- f31 ---------------------*/
double fdSemiMajAxF31(double dAxRatio, int iIndexJ) {
  return 0.5*((-1.0+2.*iIndexJ)*fndLaplaceCoeff(A(abs(iIndexJ-1))) + dAxRatio*fndDerivLaplaceCoeff(1,A(abs(iIndexJ-1))));
}

double fdDSemiF31Dalpha(double dAxRatio, int iIndexJ) {
  return 0.5*((2.*iIndexJ)*fndDerivLaplaceCoeff(1,A(abs(iIndexJ-1)))+dAxRatio*fndDerivLaplaceCoeff(2,A(abs(iIndexJ-1))));
}

/*--------- f32 ---------------------*/
double fdSemiMajAxF32(double dAxRatio, int iIndexJ) {
  return 1./8*((4-16*iIndexJ+20*iIndexJ*iIndexJ-8*iIndexJ*iIndexJ*iIndexJ)*fndLaplaceCoeff(A(abs(iIndexJ-1)))\
      +(-4+12*iIndexJ-4*iIndexJ*iIndexJ)*dAxRatio*fndDerivLaplaceCoeff(1,A(abs(iIndexJ-1)))\
      +(3+2*iIndexJ)*dAxRatio*dAxRatio*fndDerivLaplaceCoeff(2,A(abs(iIndexJ-1)))\
      +dAxRatio*dAxRatio*dAxRatio*fndDerivLaplaceCoeff(3,A(abs(iIndexJ-1))));
}

double fdDSemiF32Dalpha(double dAxRatio, int iIndexJ) {
  return 1./8*((-4*iIndexJ+16*iIndexJ*iIndexJ-\
      8*iIndexJ*iIndexJ*iIndexJ)*fndDerivLaplaceCoeff(1,A(abs(iIndexJ-1)))\
      +(2+16*iIndexJ-4*iIndexJ*iIndexJ)*dAxRatio*fndDerivLaplaceCoeff(2,A(abs(iIndexJ-1)))\
      +(6+2*iIndexJ)*dAxRatio*dAxRatio*fndDerivLaplaceCoeff(3,A(abs(iIndexJ-1)))\
      +dAxRatio*dAxRatio*dAxRatio*fndDerivLaplaceCoeff(4,A(abs(iIndexJ-1))));
}

/*--------- f33 ---------------------*/
double fdSemiMajAxF33(double dAxRatio, int iIndexJ) {
  return 1./16*((-2-iIndexJ+10*iIndexJ*iIndexJ-8*iIndexJ*iIndexJ*iIndexJ)*fndLaplaceCoeff(A(abs(iIndexJ-1)))\
      +(2+9*iIndexJ-4*iIndexJ*iIndexJ)*dAxRatio*fndDerivLaplaceCoeff(1,A(abs(iIndexJ-1)))\
      +(5+2*iIndexJ)*dAxRatio*dAxRatio*fndDerivLaplaceCoeff(2,A(abs(iIndexJ-1)))\
      +dAxRatio*dAxRatio*dAxRatio*fndDerivLaplaceCoeff(3,A(abs(iIndexJ-1))));
}

double fdDSemiF33Dalpha(double dAxRatio, int iIndexJ) {
  return 1./16*((8*iIndexJ+6*iIndexJ*iIndexJ-\
      8*iIndexJ*iIndexJ*iIndexJ)*fndDerivLaplaceCoeff(1,A(abs(iIndexJ-1)))\
      +(12+13*iIndexJ-4*iIndexJ*iIndexJ)*dAxRatio*fndDerivLaplaceCoeff(2,A(abs(iIndexJ-1)))\
      +(8+2*iIndexJ)*dAxRatio*dAxRatio*fndDerivLaplaceCoeff(3,A(abs(iIndexJ-1)))\
      +dAxRatio*dAxRatio*dAxRatio*fndDerivLaplaceCoeff(4,A(abs(iIndexJ-1))));
}

/*--------- f34 ---------------------*/
double fdSemiMajAxF34(double dAxRatio, int iIndexJ) {
  return 1./4*((-2*iIndexJ)*dAxRatio*(fndLaplaceCoeff(B(abs(iIndexJ-2)))+fndLaplaceCoeff(B(iIndexJ)))\
       -dAxRatio*dAxRatio*(fndDerivLaplaceCoeff(1,B(abs(iIndexJ-2)))+fndDerivLaplaceCoeff(1,B(iIndexJ))));
}

double fdDSemiF34Dalpha(double dAxRatio, int iIndexJ) {
  return 1./4*((-2*iIndexJ)*(fndLaplaceCoeff(B(abs(iIndexJ-2)))+fndLaplaceCoeff(B(iIndexJ)))\
  -(2+2*iIndexJ)*dAxRatio*(fndDerivLaplaceCoeff(1,B(abs(iIndexJ-2)))+fndDerivLaplaceCoeff(1,B(iIndexJ)))\
    -dAxRatio*dAxRatio*(fndDerivLaplaceCoeff(2,B(abs(iIndexJ-2)))+fndDerivLaplaceCoeff(2,B(iIndexJ))));
}

/*--------- f35 ---------------------*/
double fdSemiMajAxF35(double dAxRatio, int iIndexJ) {
  return 1./16*((1-iIndexJ-10*iIndexJ*iIndexJ-8*iIndexJ*iIndexJ*iIndexJ)*fndLaplaceCoeff(A(iIndexJ+1))\
      +(-1-iIndexJ-4*iIndexJ*iIndexJ)*dAxRatio*fndDerivLaplaceCoeff(1,A(iIndexJ+1))\
      +(3+2*iIndexJ)*dAxRatio*dAxRatio*fndDerivLaplaceCoeff(2,A(iIndexJ+1))\
      +dAxRatio*dAxRatio*dAxRatio*fndDerivLaplaceCoeff(3,A(iIndexJ+1)));
}

double fdDSemiF35Dalpha(double dAxRatio, int iIndexJ) {
  return 1./16*((-2*iIndexJ-14*iIndexJ*iIndexJ-8*iIndexJ*iIndexJ*iIndexJ)*fndDerivLaplaceCoeff(1,A(iIndexJ+1))\
      +(5+3*iIndexJ-4*iIndexJ*iIndexJ)*dAxRatio*fndDerivLaplaceCoeff(2,A(iIndexJ+1))\
      +(6+2*iIndexJ)*dAxRatio*dAxRatio*fndDerivLaplaceCoeff(3,A(iIndexJ+1))\
      +dAxRatio*dAxRatio*dAxRatio*fndDerivLaplaceCoeff(4,A(iIndexJ+1)));
}

/*--------- f36 ---------------------*/
double fdSemiMajAxF36(double dAxRatio, int iIndexJ) {
  return 1./16*((-8+32*iIndexJ-30*iIndexJ*iIndexJ+8*iIndexJ*iIndexJ*iIndexJ)*fndLaplaceCoeff(A(abs(iIndexJ-2)))\
      +(8-17*iIndexJ+4*iIndexJ*iIndexJ)*dAxRatio*fndDerivLaplaceCoeff(1,A(abs(iIndexJ-2)))\
      +(-4-2*iIndexJ)*dAxRatio*dAxRatio*fndDerivLaplaceCoeff(2,A(abs(iIndexJ-2)))\
      -dAxRatio*dAxRatio*dAxRatio*fndDerivLaplaceCoeff(3,A(abs(iIndexJ-2))));
}

double fdDSemiF36Dalpha(double dAxRatio, int iIndexJ) {
  return 1./16*((15*iIndexJ-26*iIndexJ*iIndexJ+\
      8*iIndexJ*iIndexJ*iIndexJ)*fndDerivLaplaceCoeff(1,A(abs(iIndexJ-2)))\
      -(21*iIndexJ-4*iIndexJ*iIndexJ)*dAxRatio*fndDerivLaplaceCoeff(2,A(abs(iIndexJ-2)))\
      -(7+2*iIndexJ)*dAxRatio*dAxRatio*fndDerivLaplaceCoeff(3,A(abs(iIndexJ-2)))\
      -dAxRatio*dAxRatio*dAxRatio*fndDerivLaplaceCoeff(4,A(abs(iIndexJ-2))));
}

/*--------- f37 ---------------------*/
double fdSemiMajAxF37(double dAxRatio, int iIndexJ) {
  return 1./4*((-5+2*iIndexJ)*dAxRatio*(fndLaplaceCoeff(B(abs(iIndexJ-1))))\
       -dAxRatio*dAxRatio*(fndDerivLaplaceCoeff(1,B(abs(iIndexJ-1)))));
}

double fdDSemiF37Dalpha(double dAxRatio, int iIndexJ) {
  return 1./4*((-5+2*iIndexJ)*(fndLaplaceCoeff(B(abs(iIndexJ-1))))\
  -(7-2*iIndexJ)*dAxRatio*(fndDerivLaplaceCoeff(1,B(abs(iIndexJ-1))))\
    -dAxRatio*dAxRatio*(fndDerivLaplaceCoeff(2,B(abs(iIndexJ-1)))));
}

/*--------- f38 ---------------------*/
double fdSemiMajAxF38(double dAxRatio, int iIndexJ) {
  return 1./4*((-2*iIndexJ)*dAxRatio*(fndLaplaceCoeff(B(iIndexJ)))\
       +dAxRatio*dAxRatio*(fndDerivLaplaceCoeff(1,B(iIndexJ))));
}

double fdDSemiF38Dalpha(double dAxRatio, int iIndexJ) {
  return 1./4*((-2*iIndexJ)*(fndLaplaceCoeff(B(iIndexJ)))\
  +(2-2*iIndexJ)*dAxRatio*(fndDerivLaplaceCoeff(1,B(iIndexJ)))\
    +dAxRatio*dAxRatio*(fndDerivLaplaceCoeff(2,B(iIndexJ))));
}

/*--------- f39 ---------------------*/
double fdSemiMajAxF39(double dAxRatio, int iIndexJ) {
  return 1./2*((-1-2*iIndexJ)*dAxRatio*(fndLaplaceCoeff(B(abs(iIndexJ-1))))\
       -dAxRatio*dAxRatio*(fndDerivLaplaceCoeff(1,B(abs(iIndexJ-1)))));
}

double fdDSemiF39Dalpha(double dAxRatio, int iIndexJ) {
  return 1./2*((-1-2*iIndexJ)*(fndLaplaceCoeff(B(abs(iIndexJ-1))))\
  -(3+2*iIndexJ)*dAxRatio*(fndDerivLaplaceCoeff(1,B(abs(iIndexJ-1))))\
    -dAxRatio*dAxRatio*(fndDerivLaplaceCoeff(2,B(abs(iIndexJ-1)))));
}

/*--------- f40 ---------------------*/
double fdSemiMajAxF40(double dAxRatio, int iIndexJ) {
  return 1./2*((-1-2*iIndexJ)*dAxRatio*(fndLaplaceCoeff(B(iIndexJ+1)))\
       -dAxRatio*dAxRatio*(fndDerivLaplaceCoeff(1,B(iIndexJ+1))));
}

double fdDSemiF40Dalpha(double dAxRatio, int iIndexJ) {
  return 1./2*((-1-2*iIndexJ)*(fndLaplaceCoeff(B(iIndexJ+1)))\
  -(3+2*iIndexJ)*dAxRatio*(fndDerivLaplaceCoeff(1,B(iIndexJ+1)))\
    -dAxRatio*dAxRatio*(fndDerivLaplaceCoeff(2,B(iIndexJ+1))));
}

/*--------- f41 ---------------------*/
double fdSemiMajAxF41(double dAxRatio, int iIndexJ) {
  return 1./2*((5-2*iIndexJ)*dAxRatio*(fndLaplaceCoeff(B(abs(iIndexJ-1))))\
       +dAxRatio*dAxRatio*(fndDerivLaplaceCoeff(1,B(abs(iIndexJ-1)))));
}

double fdDSemiF41Dalpha(double dAxRatio, int iIndexJ) {
  return 1./2*((5-2*iIndexJ)*(fndLaplaceCoeff(B(abs(iIndexJ-1))))\
  +(7-2*iIndexJ)*dAxRatio*(fndDerivLaplaceCoeff(1,B(abs(iIndexJ-1))))\
    +dAxRatio*dAxRatio*(fndDerivLaplaceCoeff(2,B(abs(iIndexJ-1)))));
}

/*--------- f42 ---------------------*/
double fdSemiMajAxF42(double dAxRatio, int iIndexJ) {
  return 1./2*((2*iIndexJ)*dAxRatio*(fndLaplaceCoeff(B(abs(iIndexJ-2))))\
       +dAxRatio*dAxRatio*(fndDerivLaplaceCoeff(1,B(abs(iIndexJ-2)))));
}

double fdDSemiF42Dalpha(double dAxRatio, int iIndexJ) {
  return 1./2*((2*iIndexJ)*(fndLaplaceCoeff(B(abs(iIndexJ-2))))\
  +(2+2*iIndexJ)*dAxRatio*(fndDerivLaplaceCoeff(1,B(abs(iIndexJ-2))))\
    +dAxRatio*dAxRatio*(fndDerivLaplaceCoeff(2,B(abs(iIndexJ-2)))));
}

/*--------- f43 ---------------------*/
double fdSemiMajAxF43(double dAxRatio, int iIndexJ) {
  return 1./2*((2*iIndexJ)*dAxRatio*(fndLaplaceCoeff(B(iIndexJ)))\
       +dAxRatio*dAxRatio*(fndDerivLaplaceCoeff(1,B(iIndexJ))));
}

double fdDSemiF43Dalpha(double dAxRatio, int iIndexJ) {
  return 1./2*((2*iIndexJ)*(fndLaplaceCoeff(B(iIndexJ)))\
  +(2+2*iIndexJ)*dAxRatio*(fndDerivLaplaceCoeff(1,B(iIndexJ)))\
    +dAxRatio*dAxRatio*(fndDerivLaplaceCoeff(2,B(iIndexJ))));
}

/*--------- f44 ---------------------*/
double fdSemiMajAxF44(double dAxRatio, int iIndexJ) {
  return 1./2*((2*iIndexJ)*dAxRatio*(fndLaplaceCoeff(B(iIndexJ)))\
       -dAxRatio*dAxRatio*(fndDerivLaplaceCoeff(1,B(iIndexJ))));
}

double fdDSemiF44Dalpha(double dAxRatio, int iIndexJ) {
  return 1./2*((2*iIndexJ)*(fndLaplaceCoeff(B(iIndexJ)))\
  +(-2+2*iIndexJ)*dAxRatio*(fndDerivLaplaceCoeff(1,B(iIndexJ)))\
    -dAxRatio*dAxRatio*(fndDerivLaplaceCoeff(2,B(iIndexJ))));
}

/*--------- f45 ---------------------*/
double fdSemiMajAxF45(double dAxRatio, int iIndexJ) {
  return 1./8*((-5.*iIndexJ+4.*iIndexJ*iIndexJ)*fndLaplaceCoeff(A(iIndexJ))\
        +(-2.+4.*iIndexJ)*dAxRatio*fndDerivLaplaceCoeff(1,A(iIndexJ))\
        +dAxRatio*dAxRatio*fndDerivLaplaceCoeff(2,A(iIndexJ)));
}

double fdDSemiF45Dalpha(double dAxRatio, int iIndexJ) {
  return 1./8*((-2.-1.*iIndexJ+4.*iIndexJ*iIndexJ)*fndDerivLaplaceCoeff(1,A(iIndexJ))\
        +(4*iIndexJ)*dAxRatio*fndDerivLaplaceCoeff(2,A(iIndexJ))\
        +dAxRatio*dAxRatio*fndDerivLaplaceCoeff(3,A(iIndexJ)));
}

/*--------- f49 ---------------------*/
double fdSemiMajAxF49(double dAxRatio, int iIndexJ) {
  return 1./4*((-2.+6.*iIndexJ-4.*iIndexJ*iIndexJ)*fndLaplaceCoeff(A(abs(iIndexJ-1)))\
         +(2.-4.*iIndexJ)*dAxRatio*fndDerivLaplaceCoeff(1,A(abs(iIndexJ-1)))\
         -dAxRatio*dAxRatio*fndDerivLaplaceCoeff(2,A(abs(iIndexJ-1))));
}

double fdDSemiF49Dalpha(double dAxRatio, int iIndexJ) {
  return 1./4*((2.*iIndexJ-4.*iIndexJ*iIndexJ)*fndDerivLaplaceCoeff(1,A(abs(iIndexJ-1)))\
         -4.*iIndexJ*dAxRatio*fndDerivLaplaceCoeff(2,A(abs(iIndexJ-1)))\
         -dAxRatio*dAxRatio*fndDerivLaplaceCoeff(3,A(abs(iIndexJ-1))));
}

/*--------- f53 ---------------------*/
double fdSemiMajAxF53(double dAxRatio, int iIndexJ) {
  return 1./8*((2.-7.*iIndexJ+4.*iIndexJ*iIndexJ)*fndLaplaceCoeff(A(abs(iIndexJ-2)))\
         +(-2.+4*iIndexJ)*dAxRatio*fndDerivLaplaceCoeff(1,A(abs(iIndexJ-2)))
         +dAxRatio*dAxRatio*fndDerivLaplaceCoeff(2,A(abs(iIndexJ-2))));
}

double fdDSemiF53Dalpha(double dAxRatio, int iIndexJ) {
  return 1./8*((-3.*iIndexJ+4.*iIndexJ*iIndexJ)*fndDerivLaplaceCoeff(1,A(abs(iIndexJ-2)))\
         +(4*iIndexJ)*dAxRatio*fndDerivLaplaceCoeff(2,A(abs(iIndexJ-2)))
         +dAxRatio*dAxRatio*fndDerivLaplaceCoeff(3,A(abs(iIndexJ-2))));
}

/*--------- f57 ---------------------*/
double fdSemiMajAxF57(double dAxRatio, int iIndexJ) {
  return 0.5*dAxRatio*fndLaplaceCoeff(B(abs(iIndexJ-1)));
}

double fdDSemiF57Dalpha(double dAxRatio, int iIndexJ) {
  return 0.5*(fndLaplaceCoeff(B(abs(iIndexJ-1)))+dAxRatio*fndDerivLaplaceCoeff(1,B(abs(iIndexJ-1))));
}

/*--------- f62 ---------------------*/
double fdSemiMajAxF62(double dAxRatio, int iIndexJ) {
  return -dAxRatio*fndLaplaceCoeff(B(abs(iIndexJ-1)));
}

double fdDSemiF62Dalpha(double dAxRatio, int iIndexJ) {
  return -1.*(fndLaplaceCoeff(B(abs(iIndexJ-1)))+dAxRatio*fndDerivLaplaceCoeff(1,B(abs(iIndexJ-1))));
}

/*--------- f82 ---------------------*/
double fdSemiMajAxF82(double dAxRatio, int iIndexJ) {
  return 1./48*( (-26*iIndexJ+30*iIndexJ*iIndexJ-8*iIndexJ*iIndexJ*iIndexJ)*fndLaplaceCoeff(A(iIndexJ)) \
        +(-9*dAxRatio+27*iIndexJ*dAxRatio-12*iIndexJ*iIndexJ*dAxRatio)*fndDerivLaplaceCoeff(1,A(iIndexJ)) \
        +6*(1-iIndexJ)*dAxRatio*dAxRatio*fndDerivLaplaceCoeff(2,A(iIndexJ)) \
        -dAxRatio*dAxRatio*dAxRatio*fndDerivLaplaceCoeff(3,A(iIndexJ)) );
}

double fdDSemiF82Dalpha(double dAxRatio, int iIndexJ) {
  return 1./48*( (-9+iIndexJ+18*iIndexJ*iIndexJ-8*iIndexJ*iIndexJ*iIndexJ)*fndDerivLaplaceCoeff(1,A(iIndexJ))\
        +(3+15*iIndexJ-12*iIndexJ*iIndexJ)*dAxRatio*fndDerivLaplaceCoeff(2,A(iIndexJ)) \
        +(3-6*iIndexJ)*dAxRatio*dAxRatio*fndDerivLaplaceCoeff(3,A(iIndexJ))\
        -dAxRatio*dAxRatio*dAxRatio*fndDerivLaplaceCoeff(4,A(iIndexJ)) );
}

/*--------- f83 ---------------------*/
double fdSemiMajAxF83(double dAxRatio, int iIndexJ) {
  return 1./16*( (-9+31*iIndexJ-30*iIndexJ*iIndexJ+8*iIndexJ*iIndexJ*iIndexJ)*fndLaplaceCoeff(A(abs(iIndexJ-1)))\
        +(9-25*iIndexJ+12*iIndexJ*iIndexJ)*dAxRatio*fndDerivLaplaceCoeff(1,A(abs(iIndexJ-1)))\
        +(-5+6*iIndexJ)*dAxRatio*dAxRatio*fndDerivLaplaceCoeff(2,A(abs(iIndexJ-1)))\
        +dAxRatio*dAxRatio*dAxRatio*fndDerivLaplaceCoeff(3,A(abs(iIndexJ-1))) );
}

double fdDSemiF83Dalpha(double dAxRatio, int iIndexJ) {
  return 1./16*( (6*iIndexJ-18*iIndexJ*iIndexJ+8*iIndexJ*iIndexJ*iIndexJ)*fndDerivLaplaceCoeff(1,A(abs(iIndexJ-1)))\
        +(-1-13*iIndexJ+12*iIndexJ*iIndexJ)*dAxRatio*fndDerivLaplaceCoeff(2,A(abs(iIndexJ-1)))\
        +(-2+6*iIndexJ)*dAxRatio*dAxRatio*fndDerivLaplaceCoeff(3,A(abs(iIndexJ-1))) \
        +dAxRatio*dAxRatio*dAxRatio*fndDerivLaplaceCoeff(4,A(abs(iIndexJ-1))) );
}

/*--------- f84 ---------------------*/
double fdSemiMajAxF84(double dAxRatio, int iIndexJ) {
  return 1./16*( (8-32*iIndexJ+30*iIndexJ*iIndexJ-8*iIndexJ*iIndexJ*iIndexJ)*fndLaplaceCoeff(A(abs(iIndexJ-2)))\
        +(-8+23*iIndexJ-12*iIndexJ*iIndexJ)*dAxRatio*fndDerivLaplaceCoeff(1,A(abs(iIndexJ-2)))\
        +(4-6*iIndexJ)*dAxRatio*dAxRatio*fndDerivLaplaceCoeff(2,A(abs(iIndexJ-2)))\
        -dAxRatio*dAxRatio*dAxRatio*fndDerivLaplaceCoeff(3,A(abs(iIndexJ-2))) );
}

double fdDSemiF84Dalpha(double dAxRatio, int iIndexJ) {
  return 1./16*( (-9*iIndexJ+18*iIndexJ*iIndexJ-8*iIndexJ*iIndexJ*iIndexJ)*fndDerivLaplaceCoeff(1,A(abs(iIndexJ-2)))\
        +(11*iIndexJ-12*iIndexJ*iIndexJ)*dAxRatio*fndDerivLaplaceCoeff(2,A(abs(iIndexJ-2)))\
        +(1-6*iIndexJ)*dAxRatio*dAxRatio*fndDerivLaplaceCoeff(3,A(abs(iIndexJ-2)))\
        -dAxRatio*dAxRatio*dAxRatio*fndDerivLaplaceCoeff(4,A(abs(iIndexJ-2))) );
}

/*--------- f85 ---------------------*/
double fdSemiMajAxF85(double dAxRatio, int iIndexJ) {
  return 1./48*( (-6+29*iIndexJ-30*iIndexJ*iIndexJ+8*iIndexJ*iIndexJ*iIndexJ)*fndLaplaceCoeff(A(abs(iIndexJ-3)))\
        +(6-21*iIndexJ+12*iIndexJ*iIndexJ)*dAxRatio*fndDerivLaplaceCoeff(1,A(abs(iIndexJ-3)))\
        +(-3+6*iIndexJ)*dAxRatio*dAxRatio*fndDerivLaplaceCoeff(2,A(abs(iIndexJ-3)))\
        +dAxRatio*dAxRatio*dAxRatio*fndDerivLaplaceCoeff(3,A(abs(iIndexJ-3))) );
}

double fdDSemiF85Dalpha(double dAxRatio, int iIndexJ) {
  return 1./48*( (8*iIndexJ-18*iIndexJ*iIndexJ+8*iIndexJ*iIndexJ*iIndexJ)*fndDerivLaplaceCoeff(1,A(abs(iIndexJ-3)))\
        +(-9*iIndexJ+12*iIndexJ*iIndexJ)*dAxRatio*fndDerivLaplaceCoeff(2,A(abs(iIndexJ-3)))\
        +6*iIndexJ*dAxRatio*dAxRatio*fndDerivLaplaceCoeff(3,A(abs(iIndexJ-3)))\
        +dAxRatio*dAxRatio*dAxRatio*fndDerivLaplaceCoeff(4,A(abs(iIndexJ-3))) );
}

/*--------- f86 ---------------------*/
double fdSemiMajAxF86(double dAxRatio, int iIndexJ) {
  return 1./4*( (3-2*iIndexJ)*dAxRatio*fndLaplaceCoeff(B(abs(iIndexJ-1)))\
        -dAxRatio*dAxRatio*fndDerivLaplaceCoeff(1,B(abs(iIndexJ-1))) );
}

double fdDSemiF86Dalpha(double dAxRatio, int iIndexJ) {
  return 1./4*( (3-2*iIndexJ)*fndLaplaceCoeff(B(abs(iIndexJ-1)))\
        +(1-2*iIndexJ)*dAxRatio*fndDerivLaplaceCoeff(1,B(abs(iIndexJ-1)))\
        -dAxRatio*dAxRatio*fndDerivLaplaceCoeff(2,B(abs(iIndexJ-1))) );
}

/*--------- f87 ---------------------*/
double fdSemiMajAxF87(double dAxRatio, int iIndexJ) {
  return 1./4*( 2*iIndexJ*dAxRatio*fndLaplaceCoeff(B(abs(iIndexJ-2))) \
        +dAxRatio*dAxRatio*fndDerivLaplaceCoeff(1,B(abs(iIndexJ-2))) );
}

double fdDSemiF87Dalpha(double dAxRatio, int iIndexJ) {
  return 1./4*( 2*iIndexJ*fndLaplaceCoeff(B(abs(iIndexJ-2))) \
        +(2+2*iIndexJ)*dAxRatio*fndDerivLaplaceCoeff(1,B(abs(iIndexJ-2)))\
        +dAxRatio*dAxRatio*fndDerivLaplaceCoeff(2,B(abs(iIndexJ-2))) );
}

/*--------- f88 ---------------------*/
double fdSemiMajAxF88(double dAxRatio, int iIndexJ) {
  return 1./2*( (-3+2*iIndexJ)*dAxRatio*fndLaplaceCoeff(B(abs(iIndexJ-1)))\
        +dAxRatio*dAxRatio*fndDerivLaplaceCoeff(1,B(abs(iIndexJ-1))) );
}

double fdDSemiF88Dalpha(double dAxRatio, int iIndexJ) {
  return 1./2*( (-3+2*iIndexJ)*fndLaplaceCoeff(B(abs(iIndexJ-1)))\
        +(-1+2*iIndexJ)*dAxRatio*fndDerivLaplaceCoeff(1,B(abs(iIndexJ-1)))\
        +dAxRatio*dAxRatio*fndDerivLaplaceCoeff(2,B(abs(iIndexJ-1))) );
}

/*--------- f89 ---------------------*/
double fdSemiMajAxF89(double dAxRatio, int iIndexJ) {
  return 1./2*( -2*iIndexJ*dAxRatio*fndLaplaceCoeff(B(abs(iIndexJ-2))) \
        -dAxRatio*dAxRatio*fndDerivLaplaceCoeff(1,B(abs(iIndexJ-2))) );
}

double fdDSemiF89Dalpha(double dAxRatio, int iIndexJ) {
  return 1./2*( -2*iIndexJ*fndLaplaceCoeff(B(abs(iIndexJ-2))) \
        -(2+2*iIndexJ)*dAxRatio*fndDerivLaplaceCoeff(1,B(abs(iIndexJ-2)))\
        -dAxRatio*dAxRatio*fndDerivLaplaceCoeff(2,B(abs(iIndexJ-2))) );
}

//----------------Disturbing function a h k p q l-------------------------------------------
double fdLambdaArg(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ, int iOrder) {
  if (body[iBody].dSemi < body[jBody].dSemi) {
    return iIndexJ*body[jBody].dMeanL + (iOrder-iIndexJ)*body[iBody].dMeanL;
  } else {
    return iIndexJ*body[iBody].dMeanL + (iOrder-iIndexJ)*body[jBody].dMeanL;
  }
}

double fdCosLambda(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ, int iOrder, int iRes) {
  int iPair = system->iaLaplaceN[iBody][jBody];
  if (system->bResAvg == 1) {
    return -0.5*system->daDistCos[iRes][iPair]/system->daDistSec[iRes][iPair]\
      *system->daLibrFreq2[iRes][iPair]/(system->daCircFreq[iRes][iPair]*system->daCircFreq[iRes][iPair]);
  } else {
    return cos(fdLambdaArg(body,system,iBody,jBody,iIndexJ,iOrder));
  }
}

double fdSinLambda(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ, int iOrder, int iRes) {
  int iPair = system->iaLaplaceN[iBody][jBody];
  if (system->bResAvg == 1) {
    return -0.5*system->daDistSin[iRes][iPair]/system->daDistSec[iRes][iPair]\
      *system->daLibrFreq2[iRes][iPair]/(system->daCircFreq[iRes][iPair]*system->daCircFreq[iRes][iPair]);
  } else {
    return sin(fdLambdaArg(body,system,iBody,jBody,iIndexJ,iOrder));
  }
}
//--------dR/dh-----------(inner body)------------------------------------------------------
double fdDdistDhDir11Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f28, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][27];
  f28 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][27] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][27]*dAlpha;

  return 2*body[iBody].dHecc*body[iBody].dKecc*f28;
}

double fdDdistDhDir11Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f27, f28, f29, f30, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][26];
  f27 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][26] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][26]*dAlpha;

  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][27];
  f28 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][27] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][27]*dAlpha;

  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][28];
  f29 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][28] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][28]*dAlpha;

  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][29];
  f30 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][29] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][29]*dAlpha;

  return (f27+(3*body[iBody].dHecc*body[iBody].dHecc+body[iBody].dKecc*body[iBody].dKecc)*f28\
    +(body[jBody].dHecc*body[jBody].dHecc+body[jBody].dKecc*body[jBody].dKecc)*f29\
    +(body[iBody].dPinc*body[iBody].dPinc+body[iBody].dQinc*body[iBody].dQinc\
    +body[jBody].dPinc*body[jBody].dPinc+body[jBody].dQinc*body[jBody].dQinc)*f30);
}

double fdDdistDhDir12Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f32, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][31];
  f32 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][31] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][31]*dAlpha;

  return 2*body[iBody].dHecc*f32*(body[jBody].dKecc);
}

double fdDdistDhDir12Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f32, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][31];
  f32 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][31] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][31]*dAlpha;

  return 2*body[iBody].dHecc*f32*(body[jBody].dHecc);
}

double fdDdistDhDir13Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f35, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][34];
  f35 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][34] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][34]*dAlpha;

  return f35*((-2*body[iBody].dHecc*body[jBody].dKecc+2*body[iBody].dKecc*body[jBody].dHecc));
}

double fdDdistDhDir13Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f35, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][34];
  f35 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][34] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][34]*dAlpha;

  return f35*((2*body[iBody].dHecc*body[jBody].dHecc+2*body[iBody].dKecc*body[jBody].dKecc));
}

double fdDdistDhDir14Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f36, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][35];
  f36 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][35] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][35]*dAlpha;

  return f36*((2*body[jBody].dHecc*body[jBody].dKecc));
}

double fdDdistDhDir14Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f36, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][35];
  f36 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][35] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][35]*dAlpha;

  return f36*(-(body[jBody].dKecc*body[jBody].dKecc-body[jBody].dHecc*body[jBody].dHecc));
}

double fdDdistDhDir15Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f37, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][36];
  f37 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][36] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][36]*dAlpha;

  return f37*((2*body[iBody].dPinc*body[iBody].dQinc));
}

double fdDdistDhDir15Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f37, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][36];
  f37 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][36] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][36]*dAlpha;

  return f37*(-(body[iBody].dQinc*body[iBody].dQinc-body[iBody].dPinc*body[iBody].dPinc));
}

double fdDdistDhDir17Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f39, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][38];
  f39 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][38] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][38]*dAlpha;

  return f39*(-(body[jBody].dPinc*body[iBody].dQinc-body[jBody].dQinc*body[iBody].dPinc));
}

double fdDdistDhDir17Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f39, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][38];
  f39 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][38] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][38]*dAlpha;

  return f39*((body[jBody].dQinc*body[iBody].dQinc+body[jBody].dPinc*body[iBody].dPinc));
}

double fdDdistDhDir18Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f40, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][39];
  f40 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][39] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][39]*dAlpha;

  return f40*(-(body[iBody].dPinc*body[jBody].dQinc-body[iBody].dQinc*body[jBody].dPinc));
}

double fdDdistDhDir18Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f40, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][39];
  f40 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][39] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][39]*dAlpha;

  return f40*((body[jBody].dQinc*body[iBody].dQinc+body[jBody].dPinc*body[iBody].dPinc));
}

double fdDdistDhDir19Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f41, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][40];
  f41 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][40] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][40]*dAlpha;

  return f41*((body[jBody].dPinc*body[iBody].dQinc+body[jBody].dQinc*body[iBody].dPinc));
}

double fdDdistDhDir19Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f41, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][40];
  f41 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][40] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][40]*dAlpha;

  return f41*((body[jBody].dPinc*body[iBody].dPinc-body[jBody].dQinc*body[iBody].dQinc));
}

double fdDdistDhDir113Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f37, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][36];
  f37 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][36] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][36]*dAlpha;

  return f37*((2*body[jBody].dPinc*body[jBody].dQinc));
}

double fdDdistDhDir113Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f37, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][36];
  f37 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][36] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][36]*dAlpha;

  return f37*(-(body[jBody].dQinc*body[jBody].dQinc-body[jBody].dPinc*body[jBody].dPinc));
}

double fdDdistDhExt13Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return 2*body[iBody].dHecc*(body[jBody].dKecc);
}

double fdDdistDhExt13Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return 2*body[iBody].dHecc*(body[jBody].dHecc);
}

double fdDdistDhExt16Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return -27./16*((2*body[jBody].dHecc*body[jBody].dKecc));
}

double fdDdistDhExt16Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return -27./16*(-(body[jBody].dKecc*body[jBody].dKecc-body[jBody].dHecc*body[jBody].dHecc));
}

double fdDdistDhDir21Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double dAlpha, f45;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][44];
  f45 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][44] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][44]*dAlpha;

  return f45*(-2.*body[iBody].dHecc);
}

double fdDdistDhDir21Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double dAlpha, f45;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][44];
  f45 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][44] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][44]*dAlpha;

  return f45*(2*body[iBody].dKecc);
}

double fdDdistDhDir22Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f49, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][48];
  f49 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][48] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][48]*dAlpha;

  return f49*(-body[jBody].dHecc);
}

double fdDdistDhDir22Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f49, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][48];
  f49 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][48] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][48]*dAlpha;

  return f49*(body[jBody].dKecc);
}

double fdDdistDhDir31Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f82, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][81];
  f82 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][81] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][81]*dAlpha;

  return f82*(-6*body[iBody].dHecc*body[iBody].dKecc);
}

double fdDdistDhDir31Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f82, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][81];
  f82 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][81] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][81]*dAlpha;

  return f82*((-3*body[iBody].dHecc*body[iBody].dHecc+3*body[iBody].dKecc*body[iBody].dKecc));
}

double fdDdistDhDir32Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f83, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][82];
  f83 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][82] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][82]*dAlpha;

  return f83*((-2*body[iBody].dHecc*body[jBody].dKecc-2*body[jBody].dHecc*body[iBody].dKecc));
}

double fdDdistDhDir32Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f83, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][82];
  f83 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][82] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][82]*dAlpha;

  return f83*((-2*body[iBody].dHecc*body[jBody].dHecc+2*body[iBody].dKecc*body[jBody].dKecc));
}

double fdDdistDhDir33Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f84, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][83];
  f84 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][83] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][83]*dAlpha;

  return f84*((-2*body[jBody].dHecc*body[jBody].dKecc));
}

double fdDdistDhDir33Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f84, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][83];
  f84 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][83] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][83]*dAlpha;

  return f84*((body[jBody].dKecc*body[jBody].dKecc-body[jBody].dHecc*body[jBody].dHecc));
}

double fdDdistDhDir35Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f86, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][85];
  f86 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][85] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][85]*dAlpha;

  return f86*(-2*body[iBody].dPinc*body[iBody].dQinc);
}

double fdDdistDhDir35Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f86, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][85];
  f86 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][85] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][85]*dAlpha;

  return f86*((body[iBody].dQinc*body[iBody].dQinc-body[iBody].dPinc*body[iBody].dPinc));
}

double fdDdistDhDir37Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f88, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][87];
  f88 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][87] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][87]*dAlpha;

  return f88*(-(body[jBody].dPinc*body[iBody].dQinc+body[jBody].dQinc*body[iBody].dPinc));
}

double fdDdistDhDir37Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f88, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][87];
  f88 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][87] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][87]*dAlpha;

  return f88*((body[jBody].dQinc*body[iBody].dQinc-body[jBody].dPinc*body[iBody].dPinc));
}

double fdDdistDhDir39Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f86, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][85];
  f86 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][85] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][85]*dAlpha;

  return f86*(-2*body[jBody].dPinc*body[jBody].dQinc);
}

double fdDdistDhDir39Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f86, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][85];
  f86 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][85] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][85]*dAlpha;

  return f86*((body[jBody].dQinc*body[jBody].dQinc-body[jBody].dPinc*body[jBody].dPinc));
}

double fdDdistresDHeccDirCos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ, int iOrder) {
  double y;

  if (iOrder == 1) {
    y = ( fdDdistDhDir11Cos(body, system, iBody, jBody, iIndexJ) \
      + fdDdistDhDir12Cos(body, system, iBody, jBody, iIndexJ)\
      + fdDdistDhDir13Cos(body, system, iBody, jBody, iIndexJ)\
      + fdDdistDhDir14Cos(body, system, iBody, jBody, iIndexJ) \
      + fdDdistDhDir15Cos(body, system, iBody, jBody, iIndexJ)\
      + fdDdistDhDir17Cos(body, system, iBody, jBody, iIndexJ)\
      + fdDdistDhDir18Cos(body, system, iBody, jBody, iIndexJ) \
      + fdDdistDhDir19Cos(body, system, iBody, jBody, iIndexJ)\
      + fdDdistDhDir113Cos(body, system, iBody, jBody, iIndexJ) );
  } else if (iOrder == 2) {
    y = ( fdDdistDhDir21Cos(body, system, iBody, jBody, iIndexJ) \
       + fdDdistDhDir22Cos(body, system, iBody, jBody, iIndexJ) );
  } else if (iOrder == 3) {
    y = ( fdDdistDhDir31Cos(body, system, iBody, jBody, iIndexJ) \
      + fdDdistDhDir32Cos(body, system, iBody, jBody, iIndexJ) \
      + fdDdistDhDir33Cos(body, system, iBody, jBody, iIndexJ) \
      + fdDdistDhDir35Cos(body, system, iBody, jBody, iIndexJ) \
      + fdDdistDhDir37Cos(body, system, iBody, jBody, iIndexJ) \
      + fdDdistDhDir39Cos(body, system, iBody, jBody, iIndexJ) );
  }
  return y;
}

double fdDdistresDHeccDirSin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ, int iOrder) {
  double y;

  if (iOrder == 1) {
    y = ( fdDdistDhDir11Sin(body, system, iBody, jBody, iIndexJ) \
      + fdDdistDhDir12Sin(body, system, iBody, jBody, iIndexJ)\
      + fdDdistDhDir13Sin(body, system, iBody, jBody, iIndexJ)\
      + fdDdistDhDir14Sin(body, system, iBody, jBody, iIndexJ) \
      + fdDdistDhDir15Sin(body, system, iBody, jBody, iIndexJ)\
      + fdDdistDhDir17Sin(body, system, iBody, jBody, iIndexJ)\
      + fdDdistDhDir18Sin(body, system, iBody, jBody, iIndexJ) \
      + fdDdistDhDir19Sin(body, system, iBody, jBody, iIndexJ)\
      + fdDdistDhDir113Sin(body, system, iBody, jBody, iIndexJ) );
  } else if (iOrder == 2) {
    y = ( fdDdistDhDir21Sin(body, system, iBody, jBody, iIndexJ) \
       + fdDdistDhDir22Sin(body, system, iBody, jBody, iIndexJ) );
  } else if (iOrder == 3) {
    y = ( fdDdistDhDir31Sin(body, system, iBody, jBody, iIndexJ) \
      + fdDdistDhDir32Sin(body, system, iBody, jBody, iIndexJ) \
      + fdDdistDhDir33Sin(body, system, iBody, jBody, iIndexJ) \
      + fdDdistDhDir35Sin(body, system, iBody, jBody, iIndexJ) \
      + fdDdistDhDir37Sin(body, system, iBody, jBody, iIndexJ) \
      + fdDdistDhDir39Sin(body, system, iBody, jBody, iIndexJ) );
  }
  return y;
}

double fdDdistresDHecc(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ, int iOrder, int iRes) {
  double y, yc, ys, dMfac, dSemiPrm;
  dMfac = KGAUSS*KGAUSS*body[jBody].dMass/MSUN;
  dSemiPrm = body[jBody].dSemi/AUCM;

  yc = fdDdistresDHeccDirCos(body,system,iBody,jBody,iIndexJ,iOrder);
  ys = fdDdistresDHeccDirSin(body,system,iBody,jBody,iIndexJ,iOrder);
  //check for relevant indirect terms
  if (iOrder == 1) {
    if (iIndexJ == 2) {
      yc += fdDdistDhExt13Cos(body,system,iBody,jBody,iIndexJ)*\
        body[iBody].dSemi/body[jBody].dSemi;
      ys += fdDdistDhExt13Sin(body,system,iBody,jBody,iIndexJ)*\
        body[iBody].dSemi/body[jBody].dSemi;
    } else if (iIndexJ == 3) {
      yc += fdDdistDhExt16Cos(body,system,iBody,jBody,iIndexJ)*\
        body[iBody].dSemi/body[jBody].dSemi;
      ys += fdDdistDhExt16Sin(body,system,iBody,jBody,iIndexJ)*\
        body[iBody].dSemi/body[jBody].dSemi;
    }
  }
  y = (yc*fdCosLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes) \
       + ys*fdSinLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)) * dMfac/dSemiPrm;
  return y;
}

//---------dR/dh'------------------------------------------------------------------

double fdDdistDhPrmDir11Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f29, dAlpha;
  dAlpha = body[jBody].dSemi/body[iBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][28];
  f29 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][28] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][28]*dAlpha;

  return 2*body[iBody].dHecc*f29*(body[jBody].dKecc);
}

double fdDdistDhPrmDir11Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f29, dAlpha;
  dAlpha = body[jBody].dSemi/body[iBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][28];
  f29 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][28] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][28]*dAlpha;

  return 2*body[iBody].dHecc*f29*(body[jBody].dHecc);
}

double fdDdistDhPrmDir12Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f31, f32, f33, f34, dAlpha;
  dAlpha = body[jBody].dSemi/body[iBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][32];
  f33 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][32] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][32]*dAlpha;

  return 2*body[iBody].dHecc*body[iBody].dKecc*f33;
}

double fdDdistDhPrmDir12Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f31, f32, f33, f34, dAlpha;
  dAlpha = body[jBody].dSemi/body[iBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][30];
  f31 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][30] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][30]*dAlpha;

  dAlpha = body[jBody].dSemi/body[iBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][31];
  f32 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][31] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][31]*dAlpha;

  dAlpha = body[jBody].dSemi/body[iBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][32];
  f33 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][32] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][32]*dAlpha;

  dAlpha = body[jBody].dSemi/body[iBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][33];
  f34 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][33] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][33]*dAlpha;

  return (f31+(body[jBody].dHecc*body[jBody].dHecc+body[jBody].dKecc*body[jBody].dKecc)*f32\
    +(3*body[iBody].dHecc*body[iBody].dHecc+body[iBody].dKecc*body[iBody].dKecc)*f33\
    +(body[iBody].dPinc*body[iBody].dPinc+body[iBody].dQinc*body[iBody].dQinc\
    +body[jBody].dPinc*body[jBody].dPinc+body[jBody].dQinc*body[jBody].dQinc)*f34);
}

double fdDdistDhPrmDir13Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f35, dAlpha;
  dAlpha = body[jBody].dSemi/body[iBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][34];
  f35 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][34] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][34]*dAlpha;

  return f35*((2*body[jBody].dHecc*body[jBody].dKecc));
}

double fdDdistDhPrmDir13Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f35, dAlpha;
  dAlpha = body[jBody].dSemi/body[iBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][34];
  f35 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][34] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][34]*dAlpha;

  return f35*(-(body[jBody].dKecc*body[jBody].dKecc-body[jBody].dHecc*body[jBody].dHecc));
}

double fdDdistDhPrmDir14Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f36, dAlpha;
  dAlpha = body[jBody].dSemi/body[iBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][35];
  f36 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][35] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][35]*dAlpha;

  return f36*((-2*body[iBody].dHecc*body[jBody].dKecc+2*body[iBody].dKecc*body[jBody].dHecc));
}

double fdDdistDhPrmDir14Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f36, dAlpha;
  dAlpha = body[jBody].dSemi/body[iBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][35];
  f36 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][35] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][35]*dAlpha;

  return f36*((2*body[iBody].dHecc*body[jBody].dHecc+2*body[iBody].dKecc*body[jBody].dKecc));
}

double fdDdistDhPrmDir16Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f38, dAlpha;
  dAlpha = body[jBody].dSemi/body[iBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][37];
  f38 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][37] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][37]*dAlpha;

  return f38*((2*body[jBody].dPinc*body[jBody].dQinc));
}

double fdDdistDhPrmDir16Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f38, dAlpha;
  dAlpha = body[jBody].dSemi/body[iBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][37];
  f38 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][37] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][37]*dAlpha;

  return f38*(-(body[jBody].dQinc*body[jBody].dQinc-body[jBody].dPinc*body[jBody].dPinc));
}

double fdDdistDhPrmDir110Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f42, dAlpha;
  dAlpha = body[jBody].dSemi/body[iBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][41];
  f42 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][41] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][41]*dAlpha;

  return f42*(-(body[iBody].dPinc*body[jBody].dQinc-body[iBody].dQinc*body[jBody].dPinc));
}

double fdDdistDhPrmDir110Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f42, dAlpha;
  dAlpha = body[jBody].dSemi/body[iBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][41];
  f42 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][41] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][41]*dAlpha;

  return f42*((body[jBody].dQinc*body[iBody].dQinc+body[jBody].dPinc*body[iBody].dPinc));
}

double fdDdistDhPrmDir111Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f43, dAlpha;
  dAlpha = body[jBody].dSemi/body[iBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][42];
  f43 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][42] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][42]*dAlpha;

  return f43*(-(body[jBody].dPinc*body[iBody].dQinc-body[jBody].dQinc*body[iBody].dPinc));
}

double fdDdistDhPrmDir111Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f43, dAlpha;
  dAlpha = body[jBody].dSemi/body[iBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][42];
  f43 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][42] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][42]*dAlpha;

  return f43*((body[jBody].dQinc*body[iBody].dQinc+body[jBody].dPinc*body[iBody].dPinc));
}


double fdDdistDhPrmDir112Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f44, dAlpha;
  dAlpha = body[jBody].dSemi/body[iBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][43];
  f44 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][43] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][43]*dAlpha;

  return f44*((body[iBody].dPinc*body[jBody].dQinc+body[iBody].dQinc*body[jBody].dPinc));
}

double fdDdistDhPrmDir112Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f44, dAlpha;
  dAlpha = body[jBody].dSemi/body[iBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][43];
  f44 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][43] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][43]*dAlpha;

  return f44*((body[jBody].dPinc*body[iBody].dPinc-body[jBody].dQinc*body[iBody].dQinc));
}

double fdDdistDhPrmDir114Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f38, dAlpha;
  dAlpha = body[jBody].dSemi/body[iBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][37];
  f38 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][37] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][37]*dAlpha;

  return f38*((2*body[iBody].dPinc*body[iBody].dQinc));
}

double fdDdistDhPrmDir114Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f38, dAlpha;
  dAlpha = body[jBody].dSemi/body[iBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][37];
  f38 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][37] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][37]*dAlpha;

  return f38*(-(body[iBody].dQinc*body[iBody].dQinc-body[iBody].dPinc*body[iBody].dPinc));
}

double fdDdistDhPrmInt13Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return 0.25*(3*body[iBody].dHecc*body[iBody].dKecc);
}

double fdDdistDhPrmInt13Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return 0.25*((-2+(body[jBody].dHecc*body[jBody].dHecc+body[jBody].dKecc*body[jBody].dKecc)\
    +1.5*(3*body[iBody].dHecc*body[iBody].dHecc+body[iBody].dKecc*body[iBody].dKecc)\
    +2*(body[jBody].dPinc*body[jBody].dPinc+body[jBody].dQinc*body[jBody].dQinc\
    +body[iBody].dPinc*body[iBody].dPinc+body[iBody].dQinc*body[iBody].dQinc)));
}

double fdDdistDhPrmInt16Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return -0.75*((-2*body[iBody].dHecc*body[jBody].dKecc+2*body[iBody].dKecc*body[jBody].dHecc));
}

double fdDdistDhPrmInt16Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return -0.75*((2*body[iBody].dHecc*body[jBody].dHecc+2*body[iBody].dKecc*body[jBody].dKecc));
}

double fdDdistDhPrmInt111Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return ((body[iBody].dPinc*body[jBody].dQinc-body[iBody].dQinc*body[jBody].dPinc));
}

double fdDdistDhPrmInt111Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return (-(body[jBody].dPinc*body[iBody].dPinc+body[jBody].dQinc*body[iBody].dQinc));
}

double fdDdistDhPrmDir22Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f49, dAlpha;
  dAlpha = body[jBody].dSemi/body[iBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][48];
  f49 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][48] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][48]*dAlpha;

  return f49*(-body[jBody].dHecc);
}

double fdDdistDhPrmDir22Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f49, dAlpha;
  dAlpha = body[jBody].dSemi/body[iBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][48];
  f49 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][48] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][48]*dAlpha;

  return f49*(body[jBody].dKecc);
}

double fdDdistDhPrmDir23Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f53, dAlpha;
  dAlpha = body[jBody].dSemi/body[iBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][52];
  f53 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][52] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][52]*dAlpha;

  return f53*(-2.*body[iBody].dHecc);
}

double fdDdistDhPrmDir23Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f53, dAlpha;
  dAlpha = body[jBody].dSemi/body[iBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][52];
  f53 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][52] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][52]*dAlpha;

  return f53*(2*body[iBody].dKecc);
}

double fdDdistDhPrmInt25Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return -3./8*(-2.*body[iBody].dHecc);
}

double fdDdistDhPrmInt25Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return -3./8*(2*body[iBody].dKecc);
}

double fdDdistDhPrmDir32Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f83, dAlpha;
  dAlpha = body[jBody].dSemi/body[iBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][82];
  f83 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][82] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][82]*dAlpha;

  return f83*((-2*body[jBody].dHecc*body[jBody].dKecc));
}

double fdDdistDhPrmDir32Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f83, dAlpha;
  dAlpha = body[jBody].dSemi/body[iBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][82];
  f83 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][82] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][82]*dAlpha;

  return f83*((body[jBody].dKecc*body[jBody].dKecc-body[jBody].dHecc*body[jBody].dHecc));
}

double fdDdistDhPrmDir33Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f84, dAlpha;
  dAlpha = body[jBody].dSemi/body[iBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][83];
  f84 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][83] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][83]*dAlpha;

  return f84*((-2*body[iBody].dHecc*body[jBody].dKecc-2*body[jBody].dHecc*body[iBody].dKecc));
}

double fdDdistDhPrmDir33Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f84, dAlpha;
  dAlpha = body[jBody].dSemi/body[iBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][83];
  f84 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][83] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][83]*dAlpha;

  return f84*((-2*body[iBody].dHecc*body[jBody].dHecc+2*body[jBody].dKecc*body[iBody].dKecc));
}

double fdDdistDhPrmDir34Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f85, dAlpha;
  dAlpha = body[jBody].dSemi/body[iBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][84];
  f85 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][84] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][84]*dAlpha;

  return f85*(-6*body[iBody].dHecc*body[iBody].dKecc);
}

double fdDdistDhPrmDir34Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f85, dAlpha;
  dAlpha = body[jBody].dSemi/body[iBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][84];
  f85 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][84] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][84]*dAlpha;

  return f85*((-3*body[iBody].dHecc*body[iBody].dHecc+3*body[iBody].dKecc*body[iBody].dKecc));
}

double fdDdistDhPrmDir36Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f87, dAlpha;
  dAlpha = body[jBody].dSemi/body[iBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][86];
  f87 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][86] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][86]*dAlpha;

  return f87*(-2*body[jBody].dPinc*body[jBody].dQinc);
}

double fdDdistDhPrmDir36Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f87, dAlpha;
  dAlpha = body[jBody].dSemi/body[iBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][86];
  f87 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][86] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][86]*dAlpha;

  return f87*((body[jBody].dQinc*body[jBody].dQinc-body[jBody].dPinc*body[jBody].dPinc));
}

double fdDdistDhPrmDir38Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f89, dAlpha;
  dAlpha = body[jBody].dSemi/body[iBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][88];
  f89 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][88] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][88]*dAlpha;

  return f89*(-(body[iBody].dPinc*body[jBody].dQinc+body[iBody].dQinc*body[jBody].dPinc));
}

double fdDdistDhPrmDir38Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f89, dAlpha;
  dAlpha = body[jBody].dSemi/body[iBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][88];
  f89 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][88] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][88]*dAlpha;

  return f89*((body[iBody].dQinc*body[jBody].dQinc-body[iBody].dPinc*body[jBody].dPinc));
}

double fdDdistDhPrmDir310Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f87, dAlpha;
  dAlpha = body[jBody].dSemi/body[iBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][86];
  f87 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][86] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][86]*dAlpha;

  return f87*( -2*body[iBody].dPinc*body[iBody].dQinc);
}

double fdDdistDhPrmDir310Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f87, dAlpha;
  dAlpha = body[jBody].dSemi/body[iBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][86];
  f87 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][86] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][86]*dAlpha;

  return f87*((body[iBody].dQinc*body[iBody].dQinc-body[iBody].dPinc*body[iBody].dPinc));
}

double fdDdistresDHeccPrimeDirCos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ, int iOrder) {
  double y;

  if (iOrder == 1) {
    y = ( fdDdistDhPrmDir11Cos(body, system, iBody, jBody, iIndexJ)\
          + fdDdistDhPrmDir12Cos(body, system, iBody, jBody, iIndexJ) \
          + fdDdistDhPrmDir13Cos(body, system, iBody, jBody, iIndexJ) \
          + fdDdistDhPrmDir14Cos(body, system, iBody, jBody, iIndexJ)\
          + fdDdistDhPrmDir16Cos(body, system, iBody, jBody, iIndexJ) \
          + fdDdistDhPrmDir110Cos(body, system, iBody, jBody, iIndexJ) \
          + fdDdistDhPrmDir111Cos(body, system, iBody, jBody, iIndexJ)\
          + fdDdistDhPrmDir112Cos(body, system, iBody, jBody, iIndexJ) \
          + fdDdistDhPrmDir114Cos(body, system, iBody, jBody, iIndexJ) );
  } else if (iOrder == 2) {
    y = ( fdDdistDhPrmDir22Cos(body, system, iBody, jBody, iIndexJ) \
      + fdDdistDhPrmDir23Cos(body, system, iBody, jBody, iIndexJ) );
  } else if (iOrder == 3) {
    y = ( fdDdistDhPrmDir32Cos(body, system, iBody, jBody, iIndexJ) \
      + fdDdistDhPrmDir33Cos(body, system, iBody, jBody, iIndexJ) \
      + fdDdistDhPrmDir34Cos(body, system, iBody, jBody, iIndexJ) \
      + fdDdistDhPrmDir36Cos(body, system, iBody, jBody, iIndexJ) \
      + fdDdistDhPrmDir38Cos(body, system, iBody, jBody, iIndexJ) \
      + fdDdistDhPrmDir310Cos(body, system, iBody, jBody, iIndexJ) );
  }
  return y;
}

double fdDdistresDHeccPrimeDirSin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ, int iOrder) {
  double y;

  if (iOrder == 1) {
    y = ( fdDdistDhPrmDir11Sin(body, system, iBody, jBody, iIndexJ)\
          + fdDdistDhPrmDir12Sin(body, system, iBody, jBody, iIndexJ) \
          + fdDdistDhPrmDir13Sin(body, system, iBody, jBody, iIndexJ) \
          + fdDdistDhPrmDir14Sin(body, system, iBody, jBody, iIndexJ)\
          + fdDdistDhPrmDir16Sin(body, system, iBody, jBody, iIndexJ) \
          + fdDdistDhPrmDir110Sin(body, system, iBody, jBody, iIndexJ) \
          + fdDdistDhPrmDir111Sin(body, system, iBody, jBody, iIndexJ)\
          + fdDdistDhPrmDir112Sin(body, system, iBody, jBody, iIndexJ) \
          + fdDdistDhPrmDir114Sin(body, system, iBody, jBody, iIndexJ) );
  } else if (iOrder == 2) {
    y = ( fdDdistDhPrmDir22Sin(body, system, iBody, jBody, iIndexJ) \
      + fdDdistDhPrmDir23Sin(body, system, iBody, jBody, iIndexJ) );
  } else if (iOrder == 3) {
    y = ( fdDdistDhPrmDir32Sin(body, system, iBody, jBody, iIndexJ) \
      + fdDdistDhPrmDir33Sin(body, system, iBody, jBody, iIndexJ) \
      + fdDdistDhPrmDir34Sin(body, system, iBody, jBody, iIndexJ) \
      + fdDdistDhPrmDir36Sin(body, system, iBody, jBody, iIndexJ) \
      + fdDdistDhPrmDir38Sin(body, system, iBody, jBody, iIndexJ) \
      + fdDdistDhPrmDir310Sin(body, system, iBody, jBody, iIndexJ) );
  }
  return y;
}

double fdDdistresDHeccPrime(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ, int iOrder, int iRes) {
  double y, yc, ys, dMfac, dSemiPrm;
  dMfac = KGAUSS*KGAUSS*body[jBody].dMass/MSUN;
  dSemiPrm = body[iBody].dSemi/AUCM;

  yc = fdDdistresDHeccPrimeDirCos(body,system,iBody, jBody,iIndexJ,iOrder);
  ys = fdDdistresDHeccPrimeDirSin(body,system,iBody, jBody,iIndexJ,iOrder);
  if (iOrder == 1) {
    if (iIndexJ == 2) {
      yc += (fdDdistDhPrmInt13Cos(body, system, iBody, jBody, iIndexJ)\
            +fdDdistDhPrmInt111Cos(body, system, iBody, jBody, iIndexJ) )\
            *body[iBody].dSemi*body[iBody].dSemi/(body[jBody].dSemi*body[jBody].dSemi);
      ys += (fdDdistDhPrmInt13Sin(body, system, iBody, jBody, iIndexJ)\
            +fdDdistDhPrmInt111Sin(body, system, iBody, jBody, iIndexJ) )\
            *body[iBody].dSemi*body[iBody].dSemi/(body[jBody].dSemi*body[jBody].dSemi);
    } else if (iIndexJ == 3) {
      yc += (fdDdistDhPrmInt16Cos(body, system, iBody, jBody, iIndexJ)) \
             *body[iBody].dSemi*body[iBody].dSemi/(body[jBody].dSemi*body[jBody].dSemi);
      ys += (fdDdistDhPrmInt16Sin(body, system, iBody, jBody, iIndexJ)) \
             *body[iBody].dSemi*body[iBody].dSemi/(body[jBody].dSemi*body[jBody].dSemi);
    }
  } else if (iOrder == 2) {
    if (iIndexJ == 3) {
      yc += (fdDdistDhPrmInt25Cos(body, system, iBody, jBody, iIndexJ))\
             *body[iBody].dSemi*body[iBody].dSemi/(body[jBody].dSemi*body[jBody].dSemi);
      ys += (fdDdistDhPrmInt25Sin(body, system, iBody, jBody, iIndexJ))\
             *body[iBody].dSemi*body[iBody].dSemi/(body[jBody].dSemi*body[jBody].dSemi);
    }
  }
  y = (yc*fdCosLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes) \
       + ys*fdSinLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)) * dMfac/dSemiPrm;
  return y;
}

//------------dR/dk--------------------------------------------------------------

double fdDdistDkDir11Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f27, f28, f29, f30, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][26];
  f27 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][26] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][26]*dAlpha;

  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][27];
  f28 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][27] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][27]*dAlpha;

  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][28];
  f29 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][28] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][28]*dAlpha;

  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][29];
  f30 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][29] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][29]*dAlpha;

  return (f27+(body[iBody].dHecc*body[iBody].dHecc+3*body[iBody].dKecc*body[iBody].dKecc)*f28\
    +(body[jBody].dHecc*body[jBody].dHecc+body[jBody].dKecc*body[jBody].dKecc)*f29\
    +(body[iBody].dPinc*body[iBody].dPinc+body[iBody].dQinc*body[iBody].dQinc\
    +body[jBody].dPinc*body[jBody].dPinc+body[jBody].dQinc*body[jBody].dQinc)*f30);
}

double fdDdistDkDir11Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f27, f28, f29, f30, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][27];
  f28 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][27] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][27]*dAlpha;

  return 2*body[iBody].dHecc*body[iBody].dKecc*f28;
}

double fdDdistDkDir12Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f32, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][31];
  f32 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][31] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][31]*dAlpha;

  return 2*body[iBody].dKecc*f32*(body[jBody].dKecc);
}

double fdDdistDkDir12Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f32, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][31];
  f32 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][31] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][31]*dAlpha;

  return 2*body[iBody].dKecc*f32*(body[jBody].dHecc);
}


double fdDdistDkDir13Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f35, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][34];
  f35 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][34] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][34]*dAlpha;

  return f35*((2*body[iBody].dKecc*body[jBody].dKecc+2*body[iBody].dHecc*body[jBody].dHecc));
}

double fdDdistDkDir13Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f35, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][34];
  f35 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][34] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][34]*dAlpha;

  return f35*(-(2*body[iBody].dKecc*body[jBody].dHecc-2*body[iBody].dHecc*body[jBody].dKecc));
}

double fdDdistDkDir14Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f36, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][35];
  f36 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][35] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][35]*dAlpha;

  return f36*((body[jBody].dKecc*body[jBody].dKecc-body[jBody].dHecc*body[jBody].dHecc));
}

double fdDdistDkDir14Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f36, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][35];
  f36 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][35] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][35]*dAlpha;

  return f36*((2*body[jBody].dHecc*body[jBody].dKecc));
}

double fdDdistDkDir15Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f37, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][36];
  f37 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][36] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][36]*dAlpha;

  return f37*((body[iBody].dQinc*body[iBody].dQinc-body[iBody].dPinc*body[iBody].dPinc));
}

double fdDdistDkDir15Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f37, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][36];
  f37 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][36] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][36]*dAlpha;

  return f37*((2*body[iBody].dPinc*body[iBody].dQinc));
}

double fdDdistDkDir17Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f39, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][38];
  f39 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][38] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][38]*dAlpha;

  return f39*((body[jBody].dQinc*body[iBody].dQinc+body[jBody].dPinc*body[iBody].dPinc));
}

double fdDdistDkDir17Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f39, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][38];
  f39 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][38] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][38]*dAlpha;

  return f39*((body[jBody].dPinc*body[iBody].dQinc-body[jBody].dQinc*body[iBody].dPinc));
}

double fdDdistDkDir18Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f40, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][39];
  f40 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][39] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][39]*dAlpha;

  return f40*((body[iBody].dQinc*body[jBody].dQinc+body[iBody].dPinc*body[jBody].dPinc));
}

double fdDdistDkDir18Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f40, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][39];
  f40 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][39] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][39]*dAlpha;

  return f40*((body[jBody].dQinc*body[iBody].dPinc-body[jBody].dPinc*body[iBody].dQinc));
}

double fdDdistDkDir19Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f41, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][40];
  f41 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][40] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][40]*dAlpha;

  return f41*((body[jBody].dQinc*body[iBody].dQinc-body[jBody].dPinc*body[iBody].dPinc));
}

double fdDdistDkDir19Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f41, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][40];
  f41 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][40] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][40]*dAlpha;

  return f41*((body[jBody].dQinc*body[iBody].dPinc+body[jBody].dPinc*body[iBody].dQinc));
}

double fdDdistDkDir113Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f37, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][36];
  f37 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][36] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][36]*dAlpha;

  return f37*((body[jBody].dQinc*body[jBody].dQinc-body[jBody].dPinc*body[jBody].dPinc));
}

double fdDdistDkDir113Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f37, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][36];
  f37 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][36] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][36]*dAlpha;

  return f37*((2*body[jBody].dPinc*body[jBody].dQinc));
}

double fdDdistDkExt13Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return 2*body[iBody].dKecc*(body[jBody].dKecc);
}

double fdDdistDkExt13Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return 2*body[iBody].dKecc*(body[jBody].dHecc);
}

double fdDdistDkExt16Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return -27./16*((body[jBody].dKecc*body[jBody].dKecc-body[jBody].dHecc*body[jBody].dHecc));
}

double fdDdistDkExt16Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return -27./16*((2*body[jBody].dHecc*body[jBody].dKecc));
}

double fdDdistDkDir21Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f45, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][44];
  f45 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][44] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][44]*dAlpha;

  return f45*(2.*body[iBody].dKecc);
}

double fdDdistDkDir21Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f45, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][44];
  f45 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][44] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][44]*dAlpha;

  return f45*(2*body[iBody].dHecc);
}

double fdDdistDkDir22Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f49, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][48];
  f49 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][48] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][48]*dAlpha;

  return f49*(body[jBody].dKecc);
}

double fdDdistDkDir22Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f49, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][48];
  f49 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][48] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][48]*dAlpha;

  return f49*(body[jBody].dHecc);
}

double fdDdistDkDir31Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f82, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][81];
  f82 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][81] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][81]*dAlpha;

  return f82*((3*body[iBody].dKecc*body[iBody].dKecc-3*body[iBody].dHecc*body[iBody].dHecc));
}

double fdDdistDkDir31Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f82, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][81];
  f82 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][81] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][81]*dAlpha;

  return f82*(6*body[iBody].dKecc*body[iBody].dHecc);
}

double fdDdistDkDir32Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f83, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][82];
  f83 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][82] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][82]*dAlpha;

  return f83*((2*body[iBody].dKecc*body[jBody].dKecc-2*body[iBody].dHecc*body[jBody].dHecc));
}

double fdDdistDkDir32Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f83, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][82];
  f83 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][82] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][82]*dAlpha;

  return f83*((2*body[iBody].dKecc*body[jBody].dHecc+2*body[jBody].dKecc*body[iBody].dHecc));
}

double fdDdistDkDir33Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f84, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][83];
  f84 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][83] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][83]*dAlpha;

  return f84*((body[jBody].dKecc*body[jBody].dKecc-body[jBody].dHecc*body[jBody].dHecc));
}

double fdDdistDkDir33Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f84, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][83];
  f84 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][83] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][83]*dAlpha;

  return f84*(2*body[jBody].dHecc*body[jBody].dHecc);
}

double fdDdistDkDir35Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f86, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][85];
  f86 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][85] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][85]*dAlpha;

  return f86*((body[iBody].dQinc*body[iBody].dQinc-body[iBody].dPinc*body[iBody].dPinc));
}

double fdDdistDkDir35Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f86, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][85];
  f86 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][85] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][85]*dAlpha;

  return f86*((2*body[iBody].dPinc*body[iBody].dQinc));
}

double fdDdistDkDir37Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f88, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][87];
  f88 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][87] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][87]*dAlpha;

  return f88*( (body[jBody].dQinc*body[iBody].dQinc-body[jBody].dPinc*body[iBody].dPinc));
}

double fdDdistDkDir37Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f88, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][87];
  f88 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][87] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][87]*dAlpha;

  return f88*((body[jBody].dPinc*body[iBody].dQinc+body[jBody].dQinc*body[iBody].dPinc));
}

double fdDdistDkDir39Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f86, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][85];
  f86 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][85] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][85]*dAlpha;

  return f86*( (body[jBody].dQinc*body[jBody].dQinc-body[jBody].dPinc*body[jBody].dPinc));
}

double fdDdistDkDir39Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f86, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][85];
  f86 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][85] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][85]*dAlpha;

  return f86*((2*body[jBody].dPinc*body[jBody].dQinc));
}

double fdDdistresDKeccDirCos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ, int iOrder) {
  double y;

  if (iOrder == 1) {
    y = ( fdDdistDkDir11Cos(body, system, iBody, jBody, iIndexJ) \
      + fdDdistDkDir12Cos(body, system, iBody, jBody, iIndexJ)\
      + fdDdistDkDir13Cos(body, system, iBody, jBody, iIndexJ)\
      + fdDdistDkDir14Cos(body, system, iBody, jBody, iIndexJ) \
      + fdDdistDkDir15Cos(body, system, iBody, jBody, iIndexJ)\
      + fdDdistDkDir17Cos(body, system, iBody, jBody, iIndexJ)\
      + fdDdistDkDir18Cos(body, system, iBody, jBody, iIndexJ) \
      + fdDdistDkDir19Cos(body, system, iBody, jBody, iIndexJ)\
      + fdDdistDkDir113Cos(body, system, iBody, jBody, iIndexJ) );
  } else if (iOrder == 2) {
    y = ( fdDdistDkDir21Cos(body, system, iBody, jBody, iIndexJ) \
    + fdDdistDkDir22Cos(body, system, iBody, jBody, iIndexJ) );
  } else if (iOrder == 3) {
    y = ( fdDdistDkDir31Cos(body, system, iBody, jBody, iIndexJ) \
      + fdDdistDkDir32Cos(body, system, iBody, jBody, iIndexJ) \
      + fdDdistDkDir33Cos(body, system, iBody, jBody, iIndexJ) \
      + fdDdistDkDir35Cos(body, system, iBody, jBody, iIndexJ) \
      + fdDdistDkDir37Cos(body, system, iBody, jBody, iIndexJ) \
      + fdDdistDkDir39Cos(body, system, iBody, jBody, iIndexJ) );
  }
  return y;
}

double fdDdistresDKeccDirSin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ, int iOrder) {
  double y;

  if (iOrder == 1) {
    y = ( fdDdistDkDir11Sin(body, system, iBody, jBody, iIndexJ) \
      + fdDdistDkDir12Sin(body, system, iBody, jBody, iIndexJ)\
      + fdDdistDkDir13Sin(body, system, iBody, jBody, iIndexJ)\
      + fdDdistDkDir14Sin(body, system, iBody, jBody, iIndexJ) \
      + fdDdistDkDir15Sin(body, system, iBody, jBody, iIndexJ)\
      + fdDdistDkDir17Sin(body, system, iBody, jBody, iIndexJ)\
      + fdDdistDkDir18Sin(body, system, iBody, jBody, iIndexJ) \
      + fdDdistDkDir19Sin(body, system, iBody, jBody, iIndexJ)\
      + fdDdistDkDir113Sin(body, system, iBody, jBody, iIndexJ) );
  } else if (iOrder == 2) {
    y = ( fdDdistDkDir21Sin(body, system, iBody, jBody, iIndexJ) \
    + fdDdistDkDir22Sin(body, system, iBody, jBody, iIndexJ) );
  } else if (iOrder == 3) {
    y = ( fdDdistDkDir31Sin(body, system, iBody, jBody, iIndexJ) \
      + fdDdistDkDir32Sin(body, system, iBody, jBody, iIndexJ) \
      + fdDdistDkDir33Sin(body, system, iBody, jBody, iIndexJ) \
      + fdDdistDkDir35Sin(body, system, iBody, jBody, iIndexJ) \
      + fdDdistDkDir37Sin(body, system, iBody, jBody, iIndexJ) \
      + fdDdistDkDir39Sin(body, system, iBody, jBody, iIndexJ) );
  }
  return y;
}

double fdDdistresDKecc(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ, int iOrder, int iRes) {
  double y, yc, ys, dMfac, dSemiPrm;
  dMfac = KGAUSS*KGAUSS*body[jBody].dMass/MSUN;
  dSemiPrm = body[jBody].dSemi/AUCM;

  yc = fdDdistresDKeccDirCos(body,system,iBody, jBody,iIndexJ,iOrder);
  ys = fdDdistresDKeccDirSin(body,system,iBody, jBody,iIndexJ,iOrder);
  if (iOrder == 1) {
    if (iIndexJ == 2) {
      yc += fdDdistDkExt13Cos(body, system, iBody, jBody, iIndexJ)\
            *body[iBody].dSemi/body[jBody].dSemi;
      ys += fdDdistDkExt13Sin(body, system, iBody, jBody, iIndexJ)\
            *body[iBody].dSemi/body[jBody].dSemi;
    } else if (iIndexJ == 3) {
      yc += fdDdistDkExt16Cos(body, system, iBody, jBody, iIndexJ)\
            *body[iBody].dSemi/body[jBody].dSemi;
      ys += fdDdistDkExt16Sin(body, system, iBody, jBody, iIndexJ)\
            *body[iBody].dSemi/body[jBody].dSemi;
    }
  }
  y = (yc*fdCosLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes) \
       + ys*fdSinLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)) * dMfac/dSemiPrm;
  return y;
}

//---------dR/dk'------------------------------------------------------------------

double fdDdistDkPrmDir11Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f29, dAlpha;
  dAlpha = body[jBody].dSemi/body[iBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][28];
  f29 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][28] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][28]*dAlpha;

  return 2*body[iBody].dKecc*f29*(body[jBody].dKecc);
}

double fdDdistDkPrmDir11Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f29, dAlpha;
  dAlpha = body[jBody].dSemi/body[iBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][28];
  f29 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][28] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][28]*dAlpha;

  return 2*body[iBody].dKecc*f29*(body[jBody].dHecc);
}

double fdDdistDkPrmDir12Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f31, f32, f33, f34, dAlpha;
  dAlpha = body[jBody].dSemi/body[iBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][30];
  f31 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][30] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][30]*dAlpha;

  dAlpha = body[jBody].dSemi/body[iBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][31];
  f32 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][31] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][31]*dAlpha;

  dAlpha = body[jBody].dSemi/body[iBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][32];
  f33 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][32] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][32]*dAlpha;

  dAlpha = body[jBody].dSemi/body[iBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][33];
  f34 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][33] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][33]*dAlpha;

  return (f31+(body[jBody].dHecc*body[jBody].dHecc+body[jBody].dKecc*body[jBody].dKecc)*f32\
    +(body[iBody].dHecc*body[iBody].dHecc+3*body[iBody].dKecc*body[iBody].dKecc)*f33\
    +(body[iBody].dPinc*body[iBody].dPinc+body[iBody].dQinc*body[iBody].dQinc\
    +body[jBody].dPinc*body[jBody].dPinc+body[jBody].dQinc*body[jBody].dQinc)*f34);
}

double fdDdistDkPrmDir12Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f31, f32, f33, f34, dAlpha;
  dAlpha = body[jBody].dSemi/body[iBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][32];
  f33 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][32] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][32]*dAlpha;

  return 2*body[iBody].dHecc*body[iBody].dKecc*f33;
}

double fdDdistDkPrmDir13Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f35, dAlpha;
  dAlpha = body[jBody].dSemi/body[iBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][34];
  f35 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][34] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][34]*dAlpha;

  return f35*((body[jBody].dKecc*body[jBody].dKecc-body[jBody].dHecc*body[jBody].dHecc));
}

double fdDdistDkPrmDir13Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f35, dAlpha;
  dAlpha = body[jBody].dSemi/body[iBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][34];
  f35 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][34] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][34]*dAlpha;

  return f35*((2*body[jBody].dHecc*body[jBody].dKecc));
}

double fdDdistDkPrmDir14Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f36, dAlpha;
  dAlpha = body[jBody].dSemi/body[iBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][35];
  f36 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][35] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][35]*dAlpha;

  return f36*((2*body[iBody].dKecc*body[jBody].dKecc+2*body[iBody].dHecc*body[jBody].dHecc));
}

double fdDdistDkPrmDir14Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f36, dAlpha;
  dAlpha = body[jBody].dSemi/body[iBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][35];
  f36 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][35] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][35]*dAlpha;

  return f36*(-(2*body[iBody].dKecc*body[jBody].dHecc-2*body[iBody].dHecc*body[jBody].dKecc));
}

double fdDdistDkPrmDir16Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f38, dAlpha;
  dAlpha = body[jBody].dSemi/body[iBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][37];
  f38 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][37] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][37]*dAlpha;

  return f38*((body[jBody].dQinc*body[jBody].dQinc-body[jBody].dPinc*body[jBody].dPinc));
}

double fdDdistDkPrmDir16Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f38, dAlpha;
  dAlpha = body[jBody].dSemi/body[iBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][37];
  f38 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][37] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][37]*dAlpha;

  return f38*((2*body[jBody].dPinc*body[jBody].dQinc));
}

double fdDdistDkPrmDir110Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f42, dAlpha;
  dAlpha = body[jBody].dSemi/body[iBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][41];
  f42 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][41] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][41]*dAlpha;

  return f42*((body[iBody].dQinc*body[jBody].dQinc+body[iBody].dPinc*body[jBody].dPinc));
}

double fdDdistDkPrmDir110Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f42, dAlpha;
  dAlpha = body[jBody].dSemi/body[iBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][41];
  f42 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][41] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][41]*dAlpha;

  return f42*((body[jBody].dQinc*body[iBody].dPinc-body[jBody].dPinc*body[iBody].dQinc));
}

double fdDdistDkPrmDir111Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f43, dAlpha;
  dAlpha = body[jBody].dSemi/body[iBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][42];
  f43 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][42] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][42]*dAlpha;

  return f43*((body[jBody].dQinc*body[iBody].dQinc+body[jBody].dPinc*body[iBody].dPinc));
}

double fdDdistDkPrmDir111Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f43, dAlpha;
  dAlpha = body[jBody].dSemi/body[iBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][42];
  f43 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][42] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][42]*dAlpha;

  return f43*((body[jBody].dPinc*body[iBody].dQinc-body[jBody].dQinc*body[iBody].dPinc));
}

double fdDdistDkPrmDir112Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f44, dAlpha;
  dAlpha = body[jBody].dSemi/body[iBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][43];
  f44 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][43] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][43]*dAlpha;

  return f44*((body[iBody].dQinc*body[jBody].dQinc-body[iBody].dPinc*body[jBody].dPinc));
}

double fdDdistDkPrmDir112Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f44, dAlpha;
  dAlpha = body[jBody].dSemi/body[iBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][43];
  f44 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][43] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][43]*dAlpha;

  return f44*((body[jBody].dQinc*body[iBody].dPinc+body[jBody].dPinc*body[iBody].dQinc));
}


double fdDdistDkPrmDir114Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f38, dAlpha;
  dAlpha = body[jBody].dSemi/body[iBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][37];
  f38 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][37] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][37]*dAlpha;

  return f38*((body[iBody].dQinc*body[iBody].dQinc-body[iBody].dPinc*body[iBody].dPinc));
}

double fdDdistDkPrmDir114Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f38, dAlpha;
  dAlpha = body[jBody].dSemi/body[iBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][37];
  f38 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][37] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][37]*dAlpha;

  return f38*((2*body[iBody].dPinc*body[iBody].dQinc));
}

double fdDdistDkPrmInt13Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return 0.25*((-2+(body[jBody].dHecc*body[jBody].dHecc+body[jBody].dKecc*body[jBody].dKecc)\
    +1.5*(body[iBody].dHecc*body[iBody].dHecc+3*body[iBody].dKecc*body[iBody].dKecc)\
    +2*(body[jBody].dPinc*body[jBody].dPinc+body[jBody].dQinc*body[jBody].dQinc\
    +body[iBody].dPinc*body[iBody].dPinc+body[iBody].dQinc*body[iBody].dQinc)));
}

double fdDdistDkPrmInt13Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return 0.25*(3*body[iBody].dHecc*body[iBody].dKecc);
}

double fdDdistDkPrmInt16Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return -0.75*((2*body[iBody].dKecc*body[jBody].dKecc+2*body[iBody].dHecc*body[jBody].dHecc));
}

double fdDdistDkPrmInt16Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return -0.75*(-(2*body[iBody].dKecc*body[jBody].dHecc-2*body[iBody].dHecc*body[jBody].dKecc));
}

double fdDdistDkPrmInt111Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
   return (-(body[iBody].dQinc*body[jBody].dQinc+body[iBody].dPinc*body[jBody].dPinc));
}

double fdDdistDkPrmInt111Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
   return (-(body[jBody].dQinc*body[iBody].dPinc-body[jBody].dPinc*body[iBody].dQinc));
}

double fdDdistDkPrmDir22Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f49, dAlpha;
  dAlpha = body[jBody].dSemi/body[iBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][48];
  f49 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][48] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][48]*dAlpha;

  return f49*(body[jBody].dKecc);
}

double fdDdistDkPrmDir22Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f49, dAlpha;
  dAlpha = body[jBody].dSemi/body[iBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][48];
  f49 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][48] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][48]*dAlpha;

  return f49*(body[jBody].dHecc);
}

double fdDdistDkPrmDir23Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f53, dAlpha;
  dAlpha = body[jBody].dSemi/body[iBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][52];
  f53 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][52] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][52]*dAlpha;

  return f53*(2.*body[iBody].dKecc);
}

double fdDdistDkPrmDir23Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f53, dAlpha;
  dAlpha = body[jBody].dSemi/body[iBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][52];
  f53 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][52] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][52]*dAlpha;

  return f53*(2*body[iBody].dHecc);
}

double fdDdistDkPrmInt25Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return -3./8*(2.*body[iBody].dKecc);
}

double fdDdistDkPrmInt25Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return -3./8*(2*body[iBody].dHecc);
}

double fdDdistDkPrmDir32Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f83, dAlpha;
  dAlpha = body[jBody].dSemi/body[iBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][82];
  f83 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][82] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][82]*dAlpha;

  return f83*((body[jBody].dKecc*body[jBody].dKecc-body[jBody].dHecc*body[jBody].dHecc));
}

double fdDdistDkPrmDir32Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f83, dAlpha;
  dAlpha = body[jBody].dSemi/body[iBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][82];
  f83 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][82] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][82]*dAlpha;

  return f83*((2*body[jBody].dHecc*body[jBody].dKecc));
}

double fdDdistDkPrmDir33Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f84, dAlpha;
  dAlpha = body[jBody].dSemi/body[iBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][83];
  f84 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][83] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][83]*dAlpha;

  return f84*((2*body[jBody].dKecc*body[iBody].dKecc-2*body[jBody].dHecc*body[iBody].dHecc));
}

double fdDdistDkPrmDir33Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f84, dAlpha;
  dAlpha = body[jBody].dSemi/body[iBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][83];
  f84 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][83] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][83]*dAlpha;

  return f84*((2*body[jBody].dHecc*body[iBody].dKecc+2*body[jBody].dKecc*body[iBody].dHecc));
}

double fdDdistDkPrmDir34Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f85, dAlpha;
  dAlpha = body[jBody].dSemi/body[iBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][84];
  f85 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][84] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][84]*dAlpha;

  return f85*((3*body[iBody].dKecc*body[iBody].dKecc-3*body[iBody].dHecc*body[iBody].dHecc));
}

double fdDdistDkPrmDir34Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f85, dAlpha;
  dAlpha = body[jBody].dSemi/body[iBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][84];
  f85 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][84] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][84]*dAlpha;

  return f85*(6*body[iBody].dKecc*body[iBody].dHecc);
}

double fdDdistDkPrmDir36Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f87, dAlpha;
  dAlpha = body[jBody].dSemi/body[iBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][86];
  f87 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][86] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][86]*dAlpha;

  return f87*((body[jBody].dQinc*body[jBody].dQinc-body[jBody].dPinc*body[jBody].dPinc));
}

double fdDdistDkPrmDir36Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f87, dAlpha;
  dAlpha = body[jBody].dSemi/body[iBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][86];
  f87 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][86] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][86]*dAlpha;

  return f87*((2*body[jBody].dPinc*body[jBody].dQinc));
}

double fdDdistDkPrmDir38Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f89, dAlpha;
  dAlpha = body[jBody].dSemi/body[iBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][88];
  f89 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][88] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][88]*dAlpha;

  return f89*( (body[iBody].dQinc*body[jBody].dQinc-body[jBody].dPinc*body[iBody].dPinc));
}

double fdDdistDkPrmDir38Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f89, dAlpha;
  dAlpha = body[jBody].dSemi/body[iBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][88];
  f89 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][88] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][88]*dAlpha;

  return f89*((body[iBody].dPinc*body[jBody].dQinc+body[iBody].dQinc*body[jBody].dPinc));
}

double fdDdistDkPrmDir310Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f87, dAlpha;
  dAlpha = body[jBody].dSemi/body[iBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][86];
  f87 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][86] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][86]*dAlpha;

  return f87*( (body[iBody].dQinc*body[iBody].dQinc-body[iBody].dPinc*body[iBody].dPinc));
}

double fdDdistDkPrmDir310Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f87, dAlpha;
  dAlpha = body[jBody].dSemi/body[iBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][86];
  f87 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][86] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][86]*dAlpha;

  return f87*((2*body[iBody].dPinc*body[iBody].dQinc));
}

double fdDdistresDKeccPrimeDirCos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ, int iOrder) {
  double y;

  if (iOrder == 1) {
    y = (fdDdistDkPrmDir11Cos(body, system, iBody, jBody, iIndexJ)\
          + fdDdistDkPrmDir12Cos(body, system, iBody, jBody, iIndexJ) \
          + fdDdistDkPrmDir13Cos(body, system, iBody, jBody, iIndexJ) \
          + fdDdistDkPrmDir14Cos(body, system, iBody, jBody, iIndexJ)\
          + fdDdistDkPrmDir16Cos(body, system, iBody, jBody, iIndexJ) \
          + fdDdistDkPrmDir110Cos(body, system, iBody, jBody, iIndexJ) \
          + fdDdistDkPrmDir111Cos(body, system, iBody, jBody, iIndexJ)\
          + fdDdistDkPrmDir112Cos(body, system, iBody, jBody, iIndexJ) \
          + fdDdistDkPrmDir114Cos(body, system, iBody, jBody, iIndexJ) );
  } else if (iOrder == 2) {
    y = ( fdDdistDkPrmDir22Cos(body, system, iBody, jBody, iIndexJ) \
    + fdDdistDkPrmDir23Cos(body, system, iBody, jBody, iIndexJ) );
  } else if (iOrder == 3) {
    y = ( fdDdistDkPrmDir32Cos(body, system, iBody, jBody, iIndexJ) \
      + fdDdistDkPrmDir33Cos(body, system, iBody, jBody, iIndexJ) \
      + fdDdistDkPrmDir34Cos(body, system, iBody, jBody, iIndexJ) \
      + fdDdistDkPrmDir36Cos(body, system, iBody, jBody, iIndexJ) \
      + fdDdistDkPrmDir38Cos(body, system, iBody, jBody, iIndexJ) \
      + fdDdistDkPrmDir310Cos(body, system, iBody, jBody, iIndexJ) );
  }
  return y;
}

double fdDdistresDKeccPrimeDirSin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ, int iOrder) {
  double y;

  if (iOrder == 1) {
    y = (fdDdistDkPrmDir11Sin(body, system, iBody, jBody, iIndexJ)\
          + fdDdistDkPrmDir12Sin(body, system, iBody, jBody, iIndexJ) \
          + fdDdistDkPrmDir13Sin(body, system, iBody, jBody, iIndexJ) \
          + fdDdistDkPrmDir14Sin(body, system, iBody, jBody, iIndexJ)\
          + fdDdistDkPrmDir16Sin(body, system, iBody, jBody, iIndexJ) \
          + fdDdistDkPrmDir110Sin(body, system, iBody, jBody, iIndexJ) \
          + fdDdistDkPrmDir111Sin(body, system, iBody, jBody, iIndexJ)\
          + fdDdistDkPrmDir112Sin(body, system, iBody, jBody, iIndexJ) \
          + fdDdistDkPrmDir114Sin(body, system, iBody, jBody, iIndexJ) );
  } else if (iOrder == 2) {
    y = ( fdDdistDkPrmDir22Sin(body, system, iBody, jBody, iIndexJ) \
    + fdDdistDkPrmDir23Sin(body, system, iBody, jBody, iIndexJ) );
  } else if (iOrder == 3) {
    y = ( fdDdistDkPrmDir32Sin(body, system, iBody, jBody, iIndexJ) \
      + fdDdistDkPrmDir33Sin(body, system, iBody, jBody, iIndexJ) \
      + fdDdistDkPrmDir34Sin(body, system, iBody, jBody, iIndexJ) \
      + fdDdistDkPrmDir36Sin(body, system, iBody, jBody, iIndexJ) \
      + fdDdistDkPrmDir38Sin(body, system, iBody, jBody, iIndexJ) \
      + fdDdistDkPrmDir310Sin(body, system, iBody, jBody, iIndexJ) );
  }
  return y;
}

double fdDdistresDKeccPrime(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ, int iOrder, int iRes) {
  double y, yc, ys, dMfac, dSemiPrm;
  dMfac = KGAUSS*KGAUSS*body[jBody].dMass/MSUN;
  dSemiPrm = body[iBody].dSemi/AUCM;

  yc = fdDdistresDKeccPrimeDirCos(body,system,iBody, jBody,iIndexJ,iOrder);
  ys = fdDdistresDKeccPrimeDirSin(body,system,iBody, jBody,iIndexJ,iOrder);
  if (iOrder == 1) {
    if (iIndexJ == 2) {
      yc += (fdDdistDkPrmInt13Cos(body, system, iBody, jBody, iIndexJ)\
            +fdDdistDkPrmInt111Cos(body, system, iBody, jBody, iIndexJ) )\
            *body[iBody].dSemi*body[iBody].dSemi/(body[jBody].dSemi*body[jBody].dSemi);
      ys += (fdDdistDkPrmInt13Sin(body, system, iBody, jBody, iIndexJ)\
            +fdDdistDkPrmInt111Sin(body, system, iBody, jBody, iIndexJ) )\
            *body[iBody].dSemi*body[iBody].dSemi/(body[jBody].dSemi*body[jBody].dSemi);
    } else if (iIndexJ == 3) {
      yc += (fdDdistDkPrmInt16Cos(body, system, iBody, jBody, iIndexJ))\
             *body[iBody].dSemi*body[iBody].dSemi/(body[jBody].dSemi*body[jBody].dSemi);
      ys += (fdDdistDkPrmInt16Sin(body, system, iBody, jBody, iIndexJ))\
             *body[iBody].dSemi*body[iBody].dSemi/(body[jBody].dSemi*body[jBody].dSemi);
    }
  } else if (iOrder == 2) {
    if (iIndexJ == 3) {
      yc += (fdDdistDkPrmInt25Cos(body, system, iBody, jBody, iIndexJ))\
             *body[iBody].dSemi*body[iBody].dSemi/(body[jBody].dSemi*body[jBody].dSemi);
      ys += (fdDdistDkPrmInt25Sin(body, system, iBody, jBody, iIndexJ))\
             *body[iBody].dSemi*body[iBody].dSemi/(body[jBody].dSemi*body[jBody].dSemi);
    }
  }
  y = (yc*fdCosLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes) \
       + ys*fdSinLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)) * dMfac/dSemiPrm;
  return y;
}

//---------dR/dp------------------------------------------------------------------

double fdDdistDpDir11Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f30, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][29];
  f30 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][29] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][29]*dAlpha;

  return 2*body[iBody].dPinc*f30*(body[iBody].dKecc);
}

double fdDdistDpDir11Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f30, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][29];
  f30 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][29] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][29]*dAlpha;

  return 2*body[iBody].dPinc*f30*(body[iBody].dHecc);
}

double fdDdistDpDir12Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f34, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][33];
  f34 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][33] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][33]*dAlpha;

  return 2*body[iBody].dPinc*f34*(body[jBody].dKecc);
}

double fdDdistDpDir12Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f34, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][33];
  f34 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][33] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][33]*dAlpha;

  return 2*body[iBody].dPinc*f34*(body[jBody].dHecc);
}

double fdDdistDpDir15Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f37, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][36];
  f37 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][36] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][36]*dAlpha;

  return f37*((-2*body[iBody].dPinc*body[iBody].dKecc+2*body[iBody].dQinc*body[iBody].dHecc));
}

double fdDdistDpDir15Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f37, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][36];
  f37 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][36] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][36]*dAlpha;

  return f37*((2*body[iBody].dPinc*body[iBody].dHecc+2*body[iBody].dQinc*body[iBody].dKecc));
}

double fdDdistDpDir16Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f38, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][37];
  f38 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][37] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][37]*dAlpha;

  return f38*((-2*body[iBody].dPinc*body[jBody].dKecc+2*body[iBody].dQinc*body[jBody].dHecc));
}

double fdDdistDpDir16Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f38, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][37];
  f38 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][37] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][37]*dAlpha;

  return f38*((2*body[iBody].dPinc*body[jBody].dHecc+2*body[iBody].dQinc*body[jBody].dKecc));
}

double fdDdistDpDir17Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f39, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][38];
  f39 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][38] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][38]*dAlpha;

  return f39*((body[jBody].dPinc*body[iBody].dKecc+body[jBody].dQinc*body[iBody].dHecc));
}

double fdDdistDpDir17Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f39, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][38];
  f39 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][38] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][38]*dAlpha;

  return f39*((body[jBody].dPinc*body[iBody].dHecc-body[jBody].dQinc*body[iBody].dKecc));
}

double fdDdistDpDir18Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f40, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][39];
  f40 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][39] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][39]*dAlpha;

  return f40*((body[jBody].dPinc*body[iBody].dKecc-body[jBody].dQinc*body[iBody].dHecc));
}

double fdDdistDpDir18Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f40, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][39];
  f40 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][39] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][39]*dAlpha;

  return f40*((body[jBody].dPinc*body[iBody].dHecc+body[jBody].dQinc*body[iBody].dKecc));
}

double fdDdistDpDir19Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f41, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][40];
  f41 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][40] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][40]*dAlpha;

  return f41*(-(body[jBody].dPinc*body[iBody].dKecc-body[jBody].dQinc*body[iBody].dHecc));
}

double fdDdistDpDir19Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f41, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][40];
  f41 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][40] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][40]*dAlpha;

  return f41*((body[jBody].dQinc*body[iBody].dKecc+body[jBody].dPinc*body[iBody].dHecc));
}

double fdDdistDpDir110Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f42, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][41];
  f42 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][41] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][41]*dAlpha;

  return f42*((body[jBody].dPinc*body[jBody].dKecc+body[jBody].dQinc*body[jBody].dHecc));
}

double fdDdistDpDir110Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f42, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][41];
  f42 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][41] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][41]*dAlpha;

  return f42*((body[jBody].dPinc*body[jBody].dHecc-body[jBody].dQinc*body[jBody].dKecc));
}

double fdDdistDpDir111Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f43, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][42];
  f43 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][42] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][42]*dAlpha;

  return f43*((body[jBody].dPinc*body[jBody].dKecc-body[jBody].dQinc*body[jBody].dHecc));
}

double fdDdistDpDir111Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f43, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][42];
  f43 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][42] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][42]*dAlpha;

  return f43*((body[jBody].dPinc*body[jBody].dHecc+body[jBody].dQinc*body[jBody].dKecc));
}

double fdDdistDpDir112Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f44, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][43];
  f44 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][43] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][43]*dAlpha;

  return f44*(-(body[jBody].dPinc*body[jBody].dKecc-body[jBody].dQinc*body[jBody].dHecc));
}

double fdDdistDpDir112Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f44, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][43];
  f44 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][43] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][43]*dAlpha;

  return f44*((body[jBody].dPinc*body[jBody].dHecc+body[jBody].dQinc*body[jBody].dKecc));
}

double fdDdistDpExt13Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return 4*(body[iBody].dPinc*(body[jBody].dKecc));
}

double fdDdistDpExt13Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return 4*(body[iBody].dPinc*(body[jBody].dHecc));
}

double fdDdistDpExt111Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return -4*((body[jBody].dPinc*body[jBody].dKecc+body[jBody].dQinc*body[jBody].dHecc));
}

double fdDdistDpExt111Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return 4*((-body[jBody].dPinc*body[jBody].dHecc+body[jBody].dQinc*body[jBody].dKecc));
}

double fdDdistDpDir24Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f57, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][56];
  f57 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][56] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][56]*dAlpha;

  return f57*(-2.*body[iBody].dPinc);
}

double fdDdistDpDir24Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f57, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][56];
  f57 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][56] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][56]*dAlpha;

  return f57*(2*body[iBody].dQinc);
}

double fdDdistDpDir25Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f62, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][61];
  f62 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][61] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][61]*dAlpha;

  return f62*(-body[jBody].dPinc);
}

double fdDdistDpDir25Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f62, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][61];
  f62 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][61] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][61]*dAlpha;

  return f62*(body[jBody].dQinc);
}

double fdDdistDpDir35Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f86, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][85];
  f86 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][85] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][85]*dAlpha;

  return f86*((-2*body[iBody].dKecc*body[iBody].dPinc-2*body[iBody].dHecc*body[iBody].dQinc));
}

double fdDdistDpDir35Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f86, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][85];
  f86 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][85] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][85]*dAlpha;

  return f86*((-2*body[iBody].dHecc*body[iBody].dPinc+2*body[iBody].dKecc*body[iBody].dQinc));
}

double fdDdistDpDir36Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f87, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][86];
  f87 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][86] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][86]*dAlpha;

  return f87*((-2*body[jBody].dKecc*body[iBody].dPinc-2*body[jBody].dHecc*body[iBody].dQinc));
}

double fdDdistDpDir36Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f87, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][86];
  f87 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][86] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][86]*dAlpha;

  return f87*((-2*body[jBody].dHecc*body[iBody].dPinc+2*body[jBody].dKecc*body[iBody].dQinc));
}

double fdDdistDpDir37Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f88, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][87];
  f88 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][87] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][87]*dAlpha;

  return f88*( (-body[jBody].dPinc*body[iBody].dKecc-body[jBody].dQinc*body[iBody].dHecc));
}

double fdDdistDpDir37Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f88, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][87];
  f88 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][87] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][87]*dAlpha;

  return f88*((-body[jBody].dPinc*body[iBody].dHecc+body[jBody].dQinc*body[iBody].dKecc));
}

double fdDdistDpDir38Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f89, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][88];
  f89 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][88] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][88]*dAlpha;

  return f89*( (-body[jBody].dPinc*body[jBody].dKecc-body[jBody].dQinc*body[jBody].dHecc));
}

double fdDdistDpDir38Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f89, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][88];
  f89 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][88] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][88]*dAlpha;

  return f89*((-body[jBody].dPinc*body[jBody].dHecc+body[jBody].dQinc*body[jBody].dKecc));
}

double fdDdistresDPincDirCos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ, int iOrder) {
  double y;

  if (iOrder == 1) {
    y =  (fdDdistDpDir11Cos(body, system, iBody, jBody, iIndexJ) \
    + fdDdistDpDir12Cos(body, system, iBody, jBody, iIndexJ)  \
    + fdDdistDpDir15Cos(body, system, iBody, jBody, iIndexJ) \
    + fdDdistDpDir16Cos(body, system, iBody, jBody, iIndexJ) \
    + fdDdistDpDir17Cos(body, system, iBody, jBody, iIndexJ) \
    + fdDdistDpDir18Cos(body, system, iBody, jBody, iIndexJ) \
    + fdDdistDpDir19Cos(body, system, iBody, jBody, iIndexJ) \
    + fdDdistDpDir110Cos(body, system, iBody, jBody, iIndexJ) \
    + fdDdistDpDir111Cos(body, system, iBody, jBody, iIndexJ) \
    + fdDdistDpDir112Cos(body, system, iBody, jBody, iIndexJ) );
  } else if (iOrder == 2) {
    y = ( fdDdistDpDir24Cos(body, system, iBody, jBody, iIndexJ) \
    + fdDdistDpDir25Cos(body, system, iBody, jBody, iIndexJ) );
  } else if (iOrder == 3) {
    y = ( fdDdistDpDir35Cos(body, system, iBody, jBody, iIndexJ) \
      + fdDdistDpDir36Cos(body, system, iBody, jBody, iIndexJ) \
      + fdDdistDpDir37Cos(body, system, iBody, jBody, iIndexJ) \
      + fdDdistDpDir38Cos(body, system, iBody, jBody, iIndexJ) );
  }
  return y;
}

double fdDdistresDPincDirSin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ, int iOrder) {
  double y;

  if (iOrder == 1) {
    y =  (fdDdistDpDir11Sin(body, system, iBody, jBody, iIndexJ) \
    + fdDdistDpDir12Sin(body, system, iBody, jBody, iIndexJ)  \
    + fdDdistDpDir15Sin(body, system, iBody, jBody, iIndexJ) \
    + fdDdistDpDir16Sin(body, system, iBody, jBody, iIndexJ) \
    + fdDdistDpDir17Sin(body, system, iBody, jBody, iIndexJ) \
    + fdDdistDpDir18Sin(body, system, iBody, jBody, iIndexJ) \
    + fdDdistDpDir19Sin(body, system, iBody, jBody, iIndexJ) \
    + fdDdistDpDir110Sin(body, system, iBody, jBody, iIndexJ) \
    + fdDdistDpDir111Sin(body, system, iBody, jBody, iIndexJ) \
    + fdDdistDpDir112Sin(body, system, iBody, jBody, iIndexJ) );
  } else if (iOrder == 2) {
    y = ( fdDdistDpDir24Sin(body, system, iBody, jBody, iIndexJ) \
    + fdDdistDpDir25Sin(body, system, iBody, jBody, iIndexJ) );
  } else if (iOrder == 3) {
    y = ( fdDdistDpDir35Sin(body, system, iBody, jBody, iIndexJ) \
      + fdDdistDpDir36Sin(body, system, iBody, jBody, iIndexJ) \
      + fdDdistDpDir37Sin(body, system, iBody, jBody, iIndexJ) \
      + fdDdistDpDir38Sin(body, system, iBody, jBody, iIndexJ) );
  }
  return y;
}


double fdDdistresDPinc(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ, int iOrder, int iRes) {
  double y, yc, ys, dMfac, dSemiPrm;
  dMfac = KGAUSS*KGAUSS*body[jBody].dMass/MSUN;
  dSemiPrm = body[jBody].dSemi/AUCM;

  yc = fdDdistresDPincDirCos(body,system,iBody, jBody,iIndexJ,iOrder);
  ys = fdDdistresDPincDirSin(body,system,iBody, jBody,iIndexJ,iOrder);

  if (iOrder == 1) {
    if (iIndexJ == 2) {
      yc += (fdDdistDpExt13Cos(body, system, iBody, jBody, iIndexJ) \
           +fdDdistDpExt111Cos(body, system, iBody, jBody, iIndexJ))\
            *body[iBody].dSemi/body[jBody].dSemi;
      ys += (fdDdistDpExt13Sin(body, system, iBody, jBody, iIndexJ) \
           +fdDdistDpExt111Sin(body, system, iBody, jBody, iIndexJ))\
            *body[iBody].dSemi/body[jBody].dSemi;
    }
  }

  y = (yc*fdCosLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes) \
       + ys*fdSinLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)) * dMfac/dSemiPrm;
  return y;
}


//---------dR/dp'------------------------------------------------------------------

double fdDdistDpPrmDir11Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f30, dAlpha;
  dAlpha = body[jBody].dSemi/body[iBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][29];
  f30 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][29] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][29]*dAlpha;

  return 2*body[iBody].dPinc*f30*(body[jBody].dKecc);
}

double fdDdistDpPrmDir11Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f30, dAlpha;
  dAlpha = body[jBody].dSemi/body[iBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][29];
  f30 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][29] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][29]*dAlpha;

  return 2*body[iBody].dPinc*f30*(body[jBody].dHecc);
}

double fdDdistDpPrmDir12Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f34, dAlpha;
  dAlpha = body[jBody].dSemi/body[iBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][33];
  f34 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][33] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][33]*dAlpha;

  return 2*body[iBody].dPinc*f34*(body[iBody].dKecc);
}

double fdDdistDpPrmDir12Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f34, dAlpha;
  dAlpha = body[jBody].dSemi/body[iBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][33];
  f34 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][33] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][33]*dAlpha;

  return 2*body[iBody].dPinc*f34*(body[iBody].dHecc);
}

double fdDdistDpPrmDir17Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f39, dAlpha;
  dAlpha = body[jBody].dSemi/body[iBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][38];
  f39 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][38] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][38]*dAlpha;

  return f39*((body[jBody].dPinc*body[jBody].dKecc-body[jBody].dQinc*body[jBody].dHecc));
}

double fdDdistDpPrmDir17Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f39, dAlpha;
  dAlpha = body[jBody].dSemi/body[iBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][38];
  f39 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][38] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][38]*dAlpha;

  return f39*((body[jBody].dPinc*body[jBody].dHecc+body[jBody].dQinc*body[jBody].dKecc));
}

double fdDdistDpPrmDir18Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f40, dAlpha;
  dAlpha = body[jBody].dSemi/body[iBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][39];
  f40 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][39] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][39]*dAlpha;

  return f40*((body[jBody].dPinc*body[jBody].dKecc+body[jBody].dQinc*body[jBody].dHecc));
}

double fdDdistDpPrmDir18Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f40, dAlpha;
  dAlpha = body[jBody].dSemi/body[iBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][39];
  f40 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][39] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][39]*dAlpha;

  return f40*((body[jBody].dPinc*body[jBody].dHecc-body[jBody].dQinc*body[jBody].dKecc));
}

double fdDdistDpPrmDir19Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f41, dAlpha;
  dAlpha = body[jBody].dSemi/body[iBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][40];
  f41 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][40] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][40]*dAlpha;

  return f41*((body[jBody].dQinc*body[jBody].dHecc-body[jBody].dPinc*body[jBody].dKecc));
}

double fdDdistDpPrmDir19Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f41, dAlpha;
  dAlpha = body[jBody].dSemi/body[iBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][40];
  f41 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][40] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][40]*dAlpha;

  return f41*((body[jBody].dPinc*body[jBody].dHecc+body[jBody].dQinc*body[jBody].dKecc));
}

double fdDdistDpPrmDir110Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f42, dAlpha;
  dAlpha = body[jBody].dSemi/body[iBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][41];
  f42 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][41] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][41]*dAlpha;

  return f42*((body[jBody].dPinc*body[iBody].dKecc-body[jBody].dQinc*body[iBody].dHecc));
}

double fdDdistDpPrmDir110Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f42, dAlpha;
  dAlpha = body[jBody].dSemi/body[iBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][41];
  f42 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][41] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][41]*dAlpha;

  return f42*((body[jBody].dPinc*body[iBody].dHecc+body[jBody].dQinc*body[iBody].dKecc));
}

double fdDdistDpPrmDir111Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f43, dAlpha;
  dAlpha = body[jBody].dSemi/body[iBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][42];
  f43 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][42] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][42]*dAlpha;

  return f43*((body[jBody].dPinc*body[iBody].dKecc+body[jBody].dQinc*body[iBody].dHecc));
}

double fdDdistDpPrmDir111Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f43, dAlpha;
  dAlpha = body[jBody].dSemi/body[iBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][42];
  f43 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][42] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][42]*dAlpha;

  return f43*((body[jBody].dPinc*body[iBody].dHecc-body[jBody].dQinc*body[iBody].dKecc));
}

double fdDdistDpPrmDir112Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f44, dAlpha;
  dAlpha = body[jBody].dSemi/body[iBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][43];
  f44 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][43] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][43]*dAlpha;

  return f44*((body[jBody].dQinc*body[iBody].dHecc-body[jBody].dPinc*body[iBody].dKecc));
}

double fdDdistDpPrmDir112Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f44, dAlpha;
  dAlpha = body[jBody].dSemi/body[iBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][43];
  f44 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][43] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][43]*dAlpha;

  return f44*((body[jBody].dPinc*body[iBody].dHecc+body[jBody].dQinc*body[iBody].dKecc));
}

double fdDdistDpPrmDir113Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f37, dAlpha;
  dAlpha = body[jBody].dSemi/body[iBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][36];
  f37 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][36] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][36]*dAlpha;

  return f37*((-2*body[iBody].dPinc*body[jBody].dKecc+2*body[iBody].dQinc*body[jBody].dHecc));
}

double fdDdistDpPrmDir113Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f37, dAlpha;
  dAlpha = body[jBody].dSemi/body[iBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][36];
  f37 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][36] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][36]*dAlpha;

  return f37*((2*body[iBody].dPinc*body[jBody].dHecc+2*body[iBody].dQinc*body[jBody].dKecc));
}

double fdDdistDpPrmDir114Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f38, dAlpha;
  dAlpha = body[jBody].dSemi/body[iBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][37];
  f38 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][37] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][37]*dAlpha;

  return f38*((-2*body[iBody].dPinc*body[iBody].dKecc+2*body[iBody].dQinc*body[iBody].dHecc));
}

double fdDdistDpPrmDir114Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f38, dAlpha;
  dAlpha = body[jBody].dSemi/body[iBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][37];
  f38 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][37] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][37]*dAlpha;

  return f38*((2*body[iBody].dPinc*body[iBody].dHecc+2*body[iBody].dQinc*body[iBody].dKecc));
}

double fdDdistDpPrmInt13Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return body[iBody].dPinc*(body[iBody].dKecc);
}

double fdDdistDpPrmInt13Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return body[iBody].dPinc*(body[iBody].dHecc);
}

double fdDdistDpPrmInt111Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return ((body[jBody].dQinc*body[iBody].dHecc-body[jBody].dPinc*body[iBody].dKecc));
}

double fdDdistDpPrmInt111Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return (-(body[jBody].dPinc*body[iBody].dHecc+body[jBody].dQinc*body[iBody].dKecc));
}

double fdDdistDpPrmDir25Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f62, dAlpha;
  dAlpha = body[jBody].dSemi/body[iBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][61];
  f62 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][61] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][61]*dAlpha;

  return f62*(-body[jBody].dPinc);
}

double fdDdistDpPrmDir25Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f62, dAlpha;
  dAlpha = body[jBody].dSemi/body[iBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][61];
  f62 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][61] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][61]*dAlpha;

  return f62*(body[jBody].dQinc);
}

double fdDdistDpPrmDir26Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f57, dAlpha;
  dAlpha = body[jBody].dSemi/body[iBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][56];
  f57 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][56] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][56]*dAlpha;

  return f57*(-2.*body[iBody].dPinc);
}

double fdDdistDpPrmDir26Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f57, dAlpha;
  dAlpha = body[jBody].dSemi/body[iBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][56];
  f57 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][56] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][56]*dAlpha;

  return f57*(2*body[iBody].dQinc);
}

double fdDdistDpPrmDir37Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f88, dAlpha;
  dAlpha = body[jBody].dSemi/body[iBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][87];
  f88 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][87] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][87]*dAlpha;

  return f88*( (-body[jBody].dPinc*body[jBody].dKecc-body[jBody].dQinc*body[jBody].dHecc));
}

double fdDdistDpPrmDir37Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f88, dAlpha;
  dAlpha = body[jBody].dSemi/body[iBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][87];
  f88 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][87] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][87]*dAlpha;

  return f88*( (-body[jBody].dPinc*body[jBody].dHecc+body[jBody].dQinc*body[jBody].dKecc));
}

double fdDdistDpPrmDir38Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f89, dAlpha;
  dAlpha = body[jBody].dSemi/body[iBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][88];
  f89 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][88] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][88]*dAlpha;

  return f89*( (-body[jBody].dPinc*body[iBody].dKecc-body[jBody].dQinc*body[iBody].dHecc));
}

double fdDdistDpPrmDir38Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f89, dAlpha;
  dAlpha = body[jBody].dSemi/body[iBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][88];
  f89 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][88] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][88]*dAlpha;

  return f89*((-body[jBody].dPinc*body[iBody].dHecc+body[jBody].dQinc*body[iBody].dKecc));
}

double fdDdistDpPrmDir39Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f86, dAlpha;
  dAlpha = body[jBody].dSemi/body[iBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][85];
  f86 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][85] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][85]*dAlpha;

  return f86*( (-2*body[iBody].dPinc*body[jBody].dKecc-2*body[jBody].dHecc*body[iBody].dQinc));
}

double fdDdistDpPrmDir39Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f86, dAlpha;
  dAlpha = body[jBody].dSemi/body[iBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][85];
  f86 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][85] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][85]*dAlpha;

  return f86*( (-2*body[iBody].dPinc*body[jBody].dHecc+2*body[jBody].dKecc*body[iBody].dQinc));
}

double fdDdistDpPrmDir310Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f87, dAlpha;
  dAlpha = body[jBody].dSemi/body[iBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][86];
  f87 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][86] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][86]*dAlpha;

  return f87*( (-2*body[iBody].dPinc*body[iBody].dKecc-2*body[iBody].dHecc*body[iBody].dQinc));
}

double fdDdistDpPrmDir310Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f87, dAlpha;
  dAlpha = body[jBody].dSemi/body[iBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][86];
  f87 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][86] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][86]*dAlpha;

  return f87*((-2*body[iBody].dPinc*body[iBody].dHecc+2*body[iBody].dKecc*body[iBody].dQinc));
}

double fdDdistresDPincPrimeDirCos(BODY *body, SYSTEM *system, int iBody,int jBody, int iIndexJ, int iOrder) {
  double y;

  if (iOrder == 1) {
    y = (fdDdistDpPrmDir11Cos(body, system, iBody, jBody, iIndexJ)\
          + fdDdistDpPrmDir12Cos(body, system, iBody, jBody, iIndexJ) \
          + fdDdistDpPrmDir17Cos(body, system, iBody, jBody, iIndexJ) \
          + fdDdistDpPrmDir18Cos(body, system, iBody, jBody, iIndexJ)\
          + fdDdistDpPrmDir19Cos(body, system, iBody, jBody, iIndexJ) \
          + fdDdistDpPrmDir110Cos(body, system, iBody, jBody, iIndexJ) \
          + fdDdistDpPrmDir111Cos(body, system, iBody, jBody, iIndexJ)\
          + fdDdistDpPrmDir112Cos(body, system, iBody, jBody, iIndexJ) \
          + fdDdistDpPrmDir113Cos(body, system, iBody, jBody, iIndexJ) \
          + fdDdistDpPrmDir114Cos(body, system, iBody, jBody, iIndexJ) );
  } else if (iOrder == 2) {
    y = ( fdDdistDpPrmDir25Cos(body, system, iBody, jBody, iIndexJ) \
      + fdDdistDpPrmDir26Cos(body, system, iBody, jBody, iIndexJ) );
  } else if (iOrder == 3) {
    y = ( fdDdistDpPrmDir37Cos(body, system, iBody, jBody, iIndexJ) \
      + fdDdistDpPrmDir38Cos(body, system, iBody, jBody, iIndexJ) \
      + fdDdistDpPrmDir39Cos(body, system, iBody, jBody, iIndexJ) \
      + fdDdistDpPrmDir310Cos(body, system, iBody, jBody, iIndexJ) );
  }
  return y;
}

double fdDdistresDPincPrimeDirSin(BODY *body, SYSTEM *system, int iBody,int jBody, int iIndexJ, int iOrder) {
  double y;

  if (iOrder == 1) {
    y = (fdDdistDpPrmDir11Sin(body, system, iBody, jBody, iIndexJ)\
          + fdDdistDpPrmDir12Sin(body, system, iBody, jBody, iIndexJ) \
          + fdDdistDpPrmDir17Sin(body, system, iBody, jBody, iIndexJ) \
          + fdDdistDpPrmDir18Sin(body, system, iBody, jBody, iIndexJ)\
          + fdDdistDpPrmDir19Sin(body, system, iBody, jBody, iIndexJ) \
          + fdDdistDpPrmDir110Sin(body, system, iBody, jBody, iIndexJ) \
          + fdDdistDpPrmDir111Sin(body, system, iBody, jBody, iIndexJ)\
          + fdDdistDpPrmDir112Sin(body, system, iBody, jBody, iIndexJ) \
          + fdDdistDpPrmDir113Sin(body, system, iBody, jBody, iIndexJ) \
          + fdDdistDpPrmDir114Sin(body, system, iBody, jBody, iIndexJ) );
  } else if (iOrder == 2) {
    y = ( fdDdistDpPrmDir25Sin(body, system, iBody, jBody, iIndexJ) \
      + fdDdistDpPrmDir26Sin(body, system, iBody, jBody, iIndexJ) );
  } else if (iOrder == 3) {
    y = ( fdDdistDpPrmDir37Sin(body, system, iBody, jBody, iIndexJ) \
      + fdDdistDpPrmDir38Sin(body, system, iBody, jBody, iIndexJ) \
      + fdDdistDpPrmDir39Sin(body, system, iBody, jBody, iIndexJ) \
      + fdDdistDpPrmDir310Sin(body, system, iBody, jBody, iIndexJ) );
  }
  return y;
}

double fdDdistresDPincPrime(BODY *body, SYSTEM *system, int iBody,int jBody, int iIndexJ, int iOrder, int iRes) {
  double y, yc, ys, dMfac, dSemiPrm;

  dMfac = KGAUSS*KGAUSS*body[jBody].dMass/MSUN;
  dSemiPrm = body[iBody].dSemi/AUCM;

  yc = fdDdistresDPincPrimeDirCos(body,system,iBody, jBody,iIndexJ,iOrder);
  ys = fdDdistresDPincPrimeDirSin(body,system,iBody, jBody,iIndexJ,iOrder);

  if (iOrder == 1) {
    if (iIndexJ == 2) {
      yc += (fdDdistDpPrmInt13Cos(body, system, iBody, jBody, iIndexJ)\
            +fdDdistDpPrmInt111Cos(body, system, iBody, jBody, iIndexJ) )\
            *body[iBody].dSemi*body[iBody].dSemi/(body[jBody].dSemi*body[jBody].dSemi);
      ys += (fdDdistDpPrmInt13Sin(body, system, iBody, jBody, iIndexJ)\
            +fdDdistDpPrmInt111Sin(body, system, iBody, jBody, iIndexJ) )\
            *body[iBody].dSemi*body[iBody].dSemi/(body[jBody].dSemi*body[jBody].dSemi);
    }
  }
  y = (yc*fdCosLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes) \
       + ys*fdSinLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)) * dMfac/dSemiPrm;
  return y;
}

//---------dR/dq------------------------------------------------------------------

double fdDdistDqDir11Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f30, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][29];
  f30 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][29] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][29]*dAlpha;

  return 2*body[iBody].dQinc*f30*(body[iBody].dKecc);
}

double fdDdistDqDir11Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f30, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][29];
  f30 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][29] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][29]*dAlpha;

  return 2*body[iBody].dQinc*f30*(body[iBody].dHecc);
}

double fdDdistDqDir12Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f34, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][33];
  f34 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][33] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][33]*dAlpha;

  return 2*body[iBody].dQinc*f34*(body[jBody].dKecc);
}

double fdDdistDqDir12Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f34, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][33];
  f34 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][33] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][33]*dAlpha;

  return 2*body[iBody].dQinc*f34*(body[jBody].dHecc);
}

double fdDdistDqDir15Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f37, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][36];
  f37 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][36] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][36]*dAlpha;

  return f37*((2*body[iBody].dQinc*body[iBody].dKecc+2*body[iBody].dPinc*body[iBody].dHecc));
}

double fdDdistDqDir15Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f37, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][36];
  f37 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][36] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][36]*dAlpha;

  return f37*(-(2*body[iBody].dQinc*body[iBody].dHecc-2*body[iBody].dPinc*body[iBody].dKecc));
}

double fdDdistDqDir16Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f38, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][37];
  f38 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][37] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][37]*dAlpha;

  return f38*((2*body[iBody].dQinc*body[jBody].dKecc+2*body[iBody].dPinc*body[jBody].dHecc));
}

double fdDdistDqDir16Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f38, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][37];
  f38 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][37] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][37]*dAlpha;

  return f38*(-(2*body[iBody].dQinc*body[jBody].dHecc-2*body[iBody].dPinc*body[jBody].dKecc));
}

double fdDdistDqDir17Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f39, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][38];
  f39 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][38] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][38]*dAlpha;

  return f39*((body[jBody].dQinc*body[iBody].dKecc-body[jBody].dPinc*body[iBody].dHecc));
}

double fdDdistDqDir17Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f39, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][38];
  f39 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][38] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][38]*dAlpha;

  return f39*((body[jBody].dQinc*body[iBody].dHecc+body[jBody].dPinc*body[iBody].dKecc));
}

double fdDdistDqDir18Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f40, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][39];
  f40 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][39] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][39]*dAlpha;

  return f40*((body[jBody].dQinc*body[iBody].dKecc+body[jBody].dPinc*body[iBody].dHecc));
}

double fdDdistDqDir18Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f40, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][39];
  f40 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][39] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][39]*dAlpha;

  return f40*((body[jBody].dQinc*body[iBody].dHecc-body[jBody].dPinc*body[iBody].dKecc));
}

double fdDdistDqDir19Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f41, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][40];
  f41 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][40] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][40]*dAlpha;

  return f41*((body[jBody].dQinc*body[iBody].dKecc+body[jBody].dPinc*body[iBody].dHecc));
}

double fdDdistDqDir19Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f41, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][40];
  f41 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][40] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][40]*dAlpha;

  return f41*((body[jBody].dPinc*body[iBody].dKecc-body[jBody].dQinc*body[iBody].dHecc));
}

double fdDdistDqDir110Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f42, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][41];
  f42 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][41] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][41]*dAlpha;

  return f42*((body[jBody].dQinc*body[jBody].dKecc-body[jBody].dPinc*body[jBody].dHecc));
}

double fdDdistDqDir110Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f42, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][41];
  f42 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][41] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][41]*dAlpha;

  return f42*((body[jBody].dQinc*body[jBody].dHecc+body[jBody].dPinc*body[jBody].dKecc));
}

double fdDdistDqDir111Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f43, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][42];
  f43 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][42] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][42]*dAlpha;

  return f43*((body[jBody].dQinc*body[jBody].dKecc+body[jBody].dPinc*body[jBody].dHecc));
}

double fdDdistDqDir111Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f43, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][42];
  f43 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][42] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][42]*dAlpha;

  return f43*((body[jBody].dQinc*body[jBody].dHecc-body[jBody].dPinc*body[jBody].dKecc));
}

double fdDdistDqDir112Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f44, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][43];
  f44 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][43] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][43]*dAlpha;

  return f44*((body[jBody].dQinc*body[jBody].dKecc+body[jBody].dPinc*body[jBody].dHecc));
}

double fdDdistDqDir112Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f44, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][43];
  f44 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][43] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][43]*dAlpha;

  return f44*((body[jBody].dPinc*body[jBody].dKecc-body[jBody].dQinc*body[jBody].dHecc));
}

double fdDdistDqExt13Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return 4*body[iBody].dQinc*(body[jBody].dKecc);
}

double fdDdistDqExt13Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return 4*body[iBody].dQinc*(body[jBody].dHecc);
}

double fdDdistDqExt111Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return -4*((body[jBody].dQinc*body[jBody].dKecc-body[jBody].dPinc*body[jBody].dHecc));
}

double fdDdistDqExt111Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return -4*((body[jBody].dPinc*body[jBody].dKecc+body[jBody].dQinc*body[jBody].dHecc));
}

double fdDdistDqDir24Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f57, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][56];
  f57 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][56] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][56]*dAlpha;

  return f57*(2.*body[iBody].dQinc);
}

double fdDdistDqDir24Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f57, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][56];
  f57 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][56] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][56]*dAlpha;

  return f57*(2*body[iBody].dPinc);
}

double fdDdistDqDir25Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f62, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][61];
  f62 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][61] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][61]*dAlpha;

  return f62*(body[jBody].dQinc);
}

double fdDdistDqDir25Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f62, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][61];
  f62 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][61] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][61]*dAlpha;

  return f62*(body[jBody].dPinc);
}

double fdDdistDqDir35Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f86, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][85];
  f86 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][85] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][85]*dAlpha;

  return f86*((2*body[iBody].dKecc*body[iBody].dQinc-2*body[iBody].dHecc*body[iBody].dPinc));
}

double fdDdistDqDir35Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f86, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][85];
  f86 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][85] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][85]*dAlpha;

  return f86*((2*body[iBody].dHecc*body[iBody].dQinc+2*body[iBody].dKecc*body[iBody].dPinc));
}

double fdDdistDqDir36Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f87, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][86];
  f87 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][86] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][86]*dAlpha;

  return f87*((2*body[jBody].dKecc*body[iBody].dQinc-2*body[jBody].dHecc*body[iBody].dPinc));
}

double fdDdistDqDir36Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f87, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][86];
  f87 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][86] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][86]*dAlpha;

  return f87*((2*body[jBody].dHecc*body[iBody].dQinc+2*body[jBody].dKecc*body[iBody].dPinc));
}

double fdDdistDqDir37Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f88, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][87];
  f88 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][87] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][87]*dAlpha;

  return f88*( (body[jBody].dQinc*body[iBody].dKecc-body[jBody].dPinc*body[iBody].dHecc));
}

double fdDdistDqDir37Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f88, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][87];
  f88 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][87] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][87]*dAlpha;

  return f88*((body[jBody].dQinc*body[iBody].dHecc+body[jBody].dPinc*body[iBody].dKecc));
}

double fdDdistDqDir38Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f89, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][88];
  f89 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][88] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][88]*dAlpha;

  return f89*( (body[jBody].dQinc*body[jBody].dKecc-body[jBody].dPinc*body[jBody].dHecc));
}

double fdDdistDqDir38Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f89, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][88];
  f89 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][88] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][88]*dAlpha;

  return f89*( (body[jBody].dQinc*body[jBody].dHecc+body[jBody].dPinc*body[jBody].dKecc));
}

double fdDdistresDQincDirCos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ, int iOrder) {
  double y;

  if (iOrder == 1) {
    y =  (fdDdistDqDir11Cos(body, system, iBody, jBody, iIndexJ) \
    + fdDdistDqDir12Cos(body, system, iBody, jBody, iIndexJ)  \
    + fdDdistDqDir15Cos(body, system, iBody, jBody, iIndexJ) \
    + fdDdistDqDir16Cos(body, system, iBody, jBody, iIndexJ) \
    + fdDdistDqDir17Cos(body, system, iBody, jBody, iIndexJ) \
    + fdDdistDqDir18Cos(body, system, iBody, jBody, iIndexJ) \
    + fdDdistDqDir19Cos(body, system, iBody, jBody, iIndexJ) \
    + fdDdistDqDir110Cos(body, system, iBody, jBody, iIndexJ) \
    + fdDdistDqDir111Cos(body, system, iBody, jBody, iIndexJ) \
    + fdDdistDqDir112Cos(body, system, iBody, jBody, iIndexJ) );
  } else if (iOrder==2) {
    y = ( fdDdistDqDir24Cos(body, system, iBody, jBody, iIndexJ) \
      + fdDdistDqDir25Cos(body, system, iBody, jBody, iIndexJ) );
  } else if (iOrder == 3) {
    y = ( fdDdistDqDir35Cos(body, system, iBody, jBody, iIndexJ) \
      + fdDdistDqDir36Cos(body, system, iBody, jBody, iIndexJ) \
      + fdDdistDqDir37Cos(body, system, iBody, jBody, iIndexJ) \
      + fdDdistDqDir38Cos(body, system, iBody, jBody, iIndexJ) );
  }
  return y;
}

double fdDdistresDQincDirSin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ, int iOrder) {
  double y;

  if (iOrder == 1) {
    y =  (fdDdistDqDir11Sin(body, system, iBody, jBody, iIndexJ) \
    + fdDdistDqDir12Sin(body, system, iBody, jBody, iIndexJ)  \
    + fdDdistDqDir15Sin(body, system, iBody, jBody, iIndexJ) \
    + fdDdistDqDir16Sin(body, system, iBody, jBody, iIndexJ) \
    + fdDdistDqDir17Sin(body, system, iBody, jBody, iIndexJ) \
    + fdDdistDqDir18Sin(body, system, iBody, jBody, iIndexJ) \
    + fdDdistDqDir19Sin(body, system, iBody, jBody, iIndexJ) \
    + fdDdistDqDir110Sin(body, system, iBody, jBody, iIndexJ) \
    + fdDdistDqDir111Sin(body, system, iBody, jBody, iIndexJ) \
    + fdDdistDqDir112Sin(body, system, iBody, jBody, iIndexJ) );
  } else if (iOrder==2) {
    y = ( fdDdistDqDir24Sin(body, system, iBody, jBody, iIndexJ) \
      + fdDdistDqDir25Sin(body, system, iBody, jBody, iIndexJ) );
  } else if (iOrder == 3) {
    y = ( fdDdistDqDir35Sin(body, system, iBody, jBody, iIndexJ) \
      + fdDdistDqDir36Sin(body, system, iBody, jBody, iIndexJ) \
      + fdDdistDqDir37Sin(body, system, iBody, jBody, iIndexJ) \
      + fdDdistDqDir38Sin(body, system, iBody, jBody, iIndexJ) );
  }
  return y;
}

double fdDdistresDQinc(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ, int iOrder, int iRes) {
  double y, yc, ys, dMfac, dSemiPrm;
  dMfac = KGAUSS*KGAUSS*body[jBody].dMass/MSUN;
  dSemiPrm = body[jBody].dSemi/AUCM;

  yc = fdDdistresDQincDirCos(body,system,iBody, jBody,iIndexJ,iOrder);
  ys = fdDdistresDQincDirSin(body,system,iBody, jBody,iIndexJ,iOrder);

  if (iOrder == 1) {
    if (iIndexJ == 2) {
      yc += (fdDdistDqExt13Cos(body, system, iBody, jBody, iIndexJ) \
          + fdDdistDqExt111Cos(body, system, iBody, jBody, iIndexJ))\
            *body[iBody].dSemi/body[jBody].dSemi;
      ys += (fdDdistDqExt13Sin(body, system, iBody, jBody, iIndexJ) \
          + fdDdistDqExt111Sin(body, system, iBody, jBody, iIndexJ))\
            *body[iBody].dSemi/body[jBody].dSemi;
    }
  }

  y = (yc*fdCosLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes) \
       + ys*fdSinLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)) * dMfac/dSemiPrm;
  return y;
}

//---------dR/dq'------------------------------------------------------------------

double fdDdistDqPrmDir11Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f30, dAlpha;
  dAlpha = body[jBody].dSemi/body[iBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][29];
  f30 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][29] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][29]*dAlpha;

  return 2*body[iBody].dQinc*f30*(body[jBody].dKecc);
}

double fdDdistDqPrmDir11Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f30, dAlpha;
  dAlpha = body[jBody].dSemi/body[iBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][29];
  f30 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][29] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][29]*dAlpha;

  return 2*body[iBody].dQinc*f30*(body[jBody].dHecc);
}

double fdDdistDqPrmDir12Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f34, dAlpha;
  dAlpha = body[jBody].dSemi/body[iBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][33];
  f34 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][33] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][33]*dAlpha;

  return 2*body[iBody].dQinc*f34*(body[iBody].dKecc);
}

double fdDdistDqPrmDir12Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f34, dAlpha;
  dAlpha = body[jBody].dSemi/body[iBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][33];
  f34 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][33] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][33]*dAlpha;

  return 2*body[iBody].dQinc*f34*(body[iBody].dHecc);
}

double fdDdistDqPrmDir17Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f39, dAlpha;
  dAlpha = body[jBody].dSemi/body[iBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][38];
  f39 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][38] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][38]*dAlpha;

  return f39*((body[jBody].dQinc*body[jBody].dKecc+body[jBody].dPinc*body[jBody].dHecc));
}

double fdDdistDqPrmDir17Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f39, dAlpha;
  dAlpha = body[jBody].dSemi/body[iBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][38];
  f39 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][38] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][38]*dAlpha;

  return f39*(body[jBody].dQinc*body[jBody].dHecc-body[jBody].dPinc*body[jBody].dKecc);
}

double fdDdistDqPrmDir18Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f40, dAlpha;
  dAlpha = body[jBody].dSemi/body[iBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][39];
  f40 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][39] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][39]*dAlpha;

  return f40*((body[jBody].dQinc*body[jBody].dKecc-body[jBody].dPinc*body[jBody].dHecc));
}

double fdDdistDqPrmDir18Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f40, dAlpha;
  dAlpha = body[jBody].dSemi/body[iBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][39];
  f40 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][39] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][39]*dAlpha;

  return f40*((body[jBody].dQinc*body[jBody].dHecc+body[jBody].dPinc*body[jBody].dKecc));
}

double fdDdistDqPrmDir19Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f41, dAlpha;
  dAlpha = body[jBody].dSemi/body[iBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][40];
  f41 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][40] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][40]*dAlpha;

  return f41*((body[jBody].dQinc*body[jBody].dKecc+body[jBody].dPinc*body[jBody].dHecc));
}

double fdDdistDqPrmDir19Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f41, dAlpha;
  dAlpha = body[jBody].dSemi/body[iBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][40];
  f41 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][40] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][40]*dAlpha;

  return f41*((body[jBody].dPinc*body[jBody].dKecc-body[jBody].dQinc*body[jBody].dHecc));
}

double fdDdistDqPrmDir110Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f42, dAlpha;
  dAlpha = body[jBody].dSemi/body[iBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][41];
  f42 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][41] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][41]*dAlpha;

  return f42*((body[jBody].dQinc*body[iBody].dKecc+body[jBody].dPinc*body[iBody].dHecc));
}

double fdDdistDqPrmDir110Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f42, dAlpha;
  dAlpha = body[jBody].dSemi/body[iBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][41];
  f42 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][41] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][41]*dAlpha;

  return f42*((body[jBody].dQinc*body[iBody].dHecc-body[jBody].dPinc*body[iBody].dKecc));
}

double fdDdistDqPrmDir111Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f43, dAlpha;
  dAlpha = body[jBody].dSemi/body[iBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][42];
  f43 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][42] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][42]*dAlpha;

  return f43*((body[jBody].dQinc*body[iBody].dKecc-body[jBody].dPinc*body[iBody].dHecc));
}

double fdDdistDqPrmDir111Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f43, dAlpha;
  dAlpha = body[jBody].dSemi/body[iBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][42];
  f43 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][42] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][42]*dAlpha;

  return f43*((body[jBody].dQinc*body[iBody].dHecc+body[jBody].dPinc*body[iBody].dKecc));
}

double fdDdistDqPrmDir112Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f44, dAlpha;
  dAlpha = body[jBody].dSemi/body[iBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][43];
  f44 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][43] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][43]*dAlpha;

  return f44*((body[jBody].dQinc*body[iBody].dKecc+body[jBody].dPinc*body[iBody].dHecc));
}

double fdDdistDqPrmDir112Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f44, dAlpha;
  dAlpha = body[jBody].dSemi/body[iBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][43];
  f44 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][43] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][43]*dAlpha;

  return f44*((body[jBody].dPinc*body[iBody].dKecc-body[jBody].dQinc*body[iBody].dHecc));
}

double fdDdistDqPrmDir113Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f37, dAlpha;
  dAlpha = body[jBody].dSemi/body[iBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][36];
  f37 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][36] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][36]*dAlpha;

  return f37*((2*body[iBody].dQinc*body[jBody].dKecc+2*body[iBody].dPinc*body[jBody].dHecc));
}

double fdDdistDqPrmDir113Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f37, dAlpha;
  dAlpha = body[jBody].dSemi/body[iBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][36];
  f37 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][36] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][36]*dAlpha;

  return f37*(-(2*body[iBody].dQinc*body[jBody].dHecc-2*body[iBody].dPinc*body[jBody].dKecc));
}

double fdDdistDqPrmDir114Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f38, dAlpha;
  dAlpha = body[jBody].dSemi/body[iBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][37];
  f38 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][37] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][37]*dAlpha;

  return f38*((2*body[iBody].dQinc*body[iBody].dKecc+2*body[iBody].dPinc*body[iBody].dHecc));
}

double fdDdistDqPrmDir114Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f38, dAlpha;
  dAlpha = body[jBody].dSemi/body[iBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][37];
  f38 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][37] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][37]*dAlpha;

  return f38*(-(2*body[iBody].dQinc*body[iBody].dHecc-2*body[iBody].dPinc*body[iBody].dKecc));
}

double fdDdistDqPrmInt13Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return body[iBody].dQinc*(body[iBody].dKecc);
}

double fdDdistDqPrmInt13Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return body[iBody].dQinc*(body[iBody].dHecc);
}

double fdDdistDqPrmInt111Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return (-(body[jBody].dQinc*body[iBody].dKecc+body[jBody].dPinc*body[iBody].dHecc));
}

double fdDdistDqPrmInt111Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return ((body[jBody].dPinc*body[iBody].dKecc-body[jBody].dQinc*body[iBody].dHecc));
}

double fdDdistDqPrmDir25Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f62, dAlpha;
  dAlpha = body[jBody].dSemi/body[iBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][61];
  f62 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][61] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][61]*dAlpha;

  return f62*(body[jBody].dQinc);
}

double fdDdistDqPrmDir25Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f62, dAlpha;
  dAlpha = body[jBody].dSemi/body[iBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][61];
  f62 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][61] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][61]*dAlpha;

  return f62*(body[jBody].dPinc);
}

double fdDdistDqPrmDir26Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f57, dAlpha;
  dAlpha = body[jBody].dSemi/body[iBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][56];
  f57 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][56] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][56]*dAlpha;

  return f57*(2.*body[iBody].dQinc);
}

double fdDdistDqPrmDir26Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f57, dAlpha;
  dAlpha = body[jBody].dSemi/body[iBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][56];
  f57 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][56] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][56]*dAlpha;

  return f57*(2.*body[iBody].dPinc);
}

double fdDdistDqPrmDir37Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f88, dAlpha;
  dAlpha = body[jBody].dSemi/body[iBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][87];
  f88 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][87] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][87]*dAlpha;

  return f88*( (body[jBody].dQinc*body[jBody].dKecc-body[jBody].dPinc*body[jBody].dHecc));
}

double fdDdistDqPrmDir37Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f88, dAlpha;
  dAlpha = body[jBody].dSemi/body[iBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][87];
  f88 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][87] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][87]*dAlpha;

  return f88*((body[jBody].dQinc*body[jBody].dHecc+body[jBody].dPinc*body[jBody].dKecc));
}

double fdDdistDqPrmDir38Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f89, dAlpha;
  dAlpha = body[jBody].dSemi/body[iBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][88];
  f89 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][88] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][88]*dAlpha;

  return f89*( (body[jBody].dQinc*body[iBody].dKecc-body[jBody].dPinc*body[iBody].dHecc));
}

double fdDdistDqPrmDir38Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f89, dAlpha;
  dAlpha = body[jBody].dSemi/body[iBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][88];
  f89 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][88] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][88]*dAlpha;

  return f89*( (body[jBody].dQinc*body[iBody].dHecc+body[jBody].dPinc*body[iBody].dKecc));
}

double fdDdistDqPrmDir39Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f86, dAlpha;
  dAlpha = body[jBody].dSemi/body[iBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][85];
  f86 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][85] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][85]*dAlpha;

  return f86*( (2*body[iBody].dQinc*body[jBody].dKecc-2*body[jBody].dHecc*body[iBody].dPinc));
}

double fdDdistDqPrmDir39Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f86, dAlpha;
  dAlpha = body[jBody].dSemi/body[iBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][85];
  f86 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][85] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][85]*dAlpha;

  return f86*((2*body[iBody].dQinc*body[jBody].dHecc+2*body[jBody].dKecc*body[iBody].dPinc));
}

double fdDdistDqPrmDir310Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f87, dAlpha;
  dAlpha = body[jBody].dSemi/body[iBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][86];
  f87 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][86] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][86]*dAlpha;

  return f87*( (2*body[iBody].dQinc*body[iBody].dKecc-2*body[iBody].dHecc*body[iBody].dPinc));
}

double fdDdistDqPrmDir310Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f87, dAlpha;
  dAlpha = body[jBody].dSemi/body[iBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][86];
  f87 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][86] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][86]*dAlpha;

  return f87*((2*body[iBody].dQinc*body[iBody].dHecc+2*body[iBody].dKecc*body[iBody].dPinc));
}

double fdDdistresDQincPrimeDirCos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ, int iOrder) {
  double y;

  if (iOrder == 1) {
    y = (fdDdistDqPrmDir11Cos(body, system, iBody, jBody, iIndexJ)\
          + fdDdistDqPrmDir12Cos(body, system, iBody, jBody, iIndexJ) \
          + fdDdistDqPrmDir17Cos(body, system, iBody, jBody, iIndexJ) \
          + fdDdistDqPrmDir18Cos(body, system, iBody, jBody, iIndexJ)\
          + fdDdistDqPrmDir19Cos(body, system, iBody, jBody, iIndexJ) \
          + fdDdistDqPrmDir110Cos(body, system, iBody, jBody, iIndexJ) \
          + fdDdistDqPrmDir111Cos(body, system, iBody, jBody, iIndexJ)\
          + fdDdistDqPrmDir112Cos(body, system, iBody, jBody, iIndexJ) \
          + fdDdistDqPrmDir113Cos(body, system, iBody, jBody, iIndexJ) \
          + fdDdistDqPrmDir114Cos(body, system, iBody, jBody, iIndexJ) );
  } else if (iOrder == 2) {
    y = ( fdDdistDqPrmDir25Cos(body, system, iBody, jBody, iIndexJ) \
      + fdDdistDqPrmDir26Cos(body, system, iBody, jBody, iIndexJ) );
  } else if (iOrder == 3) {
    y = ( fdDdistDqPrmDir37Cos(body, system, iBody, jBody, iIndexJ) \
    + fdDdistDqPrmDir38Cos(body, system, iBody, jBody, iIndexJ) \
    + fdDdistDqPrmDir39Cos(body, system, iBody, jBody, iIndexJ) \
    + fdDdistDqPrmDir310Cos(body, system, iBody, jBody, iIndexJ) );
  }
  return y;
}

double fdDdistresDQincPrimeDirSin(BODY *body, SYSTEM *system, int iBody,int jBody, int iIndexJ, int iOrder) {
  double y;

  if (iOrder == 1) {
    y = (fdDdistDqPrmDir11Sin(body, system, iBody, jBody, iIndexJ)\
          + fdDdistDqPrmDir12Sin(body, system, iBody, jBody, iIndexJ) \
          + fdDdistDqPrmDir17Sin(body, system, iBody, jBody, iIndexJ) \
          + fdDdistDqPrmDir18Sin(body, system, iBody, jBody, iIndexJ)\
          + fdDdistDqPrmDir19Sin(body, system, iBody, jBody, iIndexJ) \
          + fdDdistDqPrmDir110Sin(body, system, iBody, jBody, iIndexJ) \
          + fdDdistDqPrmDir111Sin(body, system, iBody, jBody, iIndexJ)\
          + fdDdistDqPrmDir112Sin(body, system, iBody, jBody, iIndexJ) \
          + fdDdistDqPrmDir113Sin(body, system, iBody, jBody, iIndexJ) \
          + fdDdistDqPrmDir114Sin(body, system, iBody, jBody, iIndexJ) );
  } else if (iOrder == 2) {
    y = ( fdDdistDqPrmDir25Sin(body, system, iBody, jBody, iIndexJ) \
      + fdDdistDqPrmDir26Sin(body, system, iBody, jBody, iIndexJ) );
  } else if (iOrder == 3) {
    y = ( fdDdistDqPrmDir37Sin(body, system, iBody, jBody, iIndexJ) \
    + fdDdistDqPrmDir38Sin(body, system, iBody, jBody, iIndexJ) \
    + fdDdistDqPrmDir39Sin(body, system, iBody, jBody, iIndexJ) \
    + fdDdistDqPrmDir310Sin(body, system, iBody, jBody, iIndexJ) );
  }
  return y;
}

double fdDdistresDQincPrime(BODY *body, SYSTEM *system, int iBody,int jBody, int iIndexJ, int iOrder, int iRes) {
  double y, yc, ys, dMfac, dSemiPrm;
  dMfac = KGAUSS*KGAUSS*body[jBody].dMass/MSUN;
  dSemiPrm = body[iBody].dSemi/AUCM;

  yc = fdDdistresDQincPrimeDirCos(body,system,iBody, jBody,iIndexJ,iOrder);
  ys = fdDdistresDQincPrimeDirSin(body,system,iBody, jBody,iIndexJ,iOrder);

  if (iOrder == 1) {
    if (iIndexJ == 2) {
      yc += (fdDdistDqPrmInt13Cos(body, system, iBody, jBody, iIndexJ)\
            +fdDdistDqPrmInt111Cos(body, system, iBody, jBody, iIndexJ) ) \
            *body[iBody].dSemi*body[iBody].dSemi/(body[jBody].dSemi*body[jBody].dSemi);
      ys += (fdDdistDqPrmInt13Sin(body, system, iBody, jBody, iIndexJ)\
            +fdDdistDqPrmInt111Sin(body, system, iBody, jBody, iIndexJ) ) \
            *body[iBody].dSemi*body[iBody].dSemi/(body[jBody].dSemi*body[jBody].dSemi);
    }
  }

  y = (yc*fdCosLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes) \
       + ys*fdSinLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)) * dMfac/dSemiPrm;
  return y;
}

//---------dR/dlambda------------------------------------------------------------------

double fdDdistDlDir11Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f27, f28, f29, f30, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][26];
  f27 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][26] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][26]*dAlpha;

  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][27];
  f28 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][27] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][27]*dAlpha;

  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][28];
  f29 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][28] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][28]*dAlpha;

  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][29];
  f30 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][29] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][29]*dAlpha;

  return (1.-iIndexJ)*(f27+(body[iBody].dHecc*body[iBody].dHecc+\
        body[iBody].dKecc*body[iBody].dKecc)*f28+(body[jBody].dHecc*body[jBody].dHecc+\
        body[jBody].dKecc*body[jBody].dKecc)*f29+(body[iBody].dPinc*body[iBody].dPinc+\
        body[iBody].dQinc*body[iBody].dQinc+body[jBody].dPinc*body[jBody].dPinc+\
        body[jBody].dQinc*body[jBody].dQinc)*f30)*(body[iBody].dHecc);
}

double fdDdistDlDir11Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f27, f28, f29, f30, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][26];
  f27 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][26] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][26]*dAlpha;

  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][27];
  f28 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][27] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][27]*dAlpha;

  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][28];
  f29 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][28] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][28]*dAlpha;

  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][29];
  f30 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][29] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][29]*dAlpha;

  return (1.-iIndexJ)*(f27+(body[iBody].dHecc*body[iBody].dHecc+\
        body[iBody].dKecc*body[iBody].dKecc)*f28+(body[jBody].dHecc*body[jBody].dHecc+\
        body[jBody].dKecc*body[jBody].dKecc)*f29+(body[iBody].dPinc*body[iBody].dPinc+\
        body[iBody].dQinc*body[iBody].dQinc+body[jBody].dPinc*body[jBody].dPinc+\
        body[jBody].dQinc*body[jBody].dQinc)*f30)*(-body[iBody].dKecc);
}

double fdDdistDlDir12Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f31, f32, f33, f34, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][30];
  f31 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][30] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][30]*dAlpha;

  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][31];
  f32 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][31] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][31]*dAlpha;

  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][32];
  f33 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][32] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][32]*dAlpha;

  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][33];
  f34 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][33] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][33]*dAlpha;

  return (1.-iIndexJ)*(f31+(body[iBody].dHecc*body[iBody].dHecc+\
        body[iBody].dKecc*body[iBody].dKecc)*f32+(body[jBody].dHecc*body[jBody].dHecc+\
        body[jBody].dKecc*body[jBody].dKecc)*f33+(body[iBody].dPinc*body[iBody].dPinc+\
        body[iBody].dQinc*body[iBody].dQinc+body[jBody].dPinc*body[jBody].dPinc+\
        body[jBody].dQinc*body[jBody].dQinc)*f34)*(body[jBody].dHecc);
}

double fdDdistDlDir12Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f31, f32, f33, f34, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][30];
  f31 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][30] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][30]*dAlpha;

  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][31];
  f32 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][31] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][31]*dAlpha;

  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][32];
  f33 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][32] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][32]*dAlpha;

  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][33];
  f34 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][33] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][33]*dAlpha;

  return (1.-iIndexJ)*(f31+(body[iBody].dHecc*body[iBody].dHecc+\
        body[iBody].dKecc*body[iBody].dKecc)*f32+(body[jBody].dHecc*body[jBody].dHecc+\
        body[jBody].dKecc*body[jBody].dKecc)*f33+(body[iBody].dPinc*body[iBody].dPinc+\
        body[iBody].dQinc*body[iBody].dQinc+body[jBody].dPinc*body[jBody].dPinc+\
        body[jBody].dQinc*body[jBody].dQinc)*f34)*(-body[jBody].dKecc);
}

double fdDdistDlDir13Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f35, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][34];
  f35 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][34] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][34]*dAlpha;

  return (1-iIndexJ)*f35*(-(body[jBody].dHecc*\
    (body[iBody].dKecc*body[iBody].dKecc-body[iBody].dHecc*body[iBody].dHecc)\
    -2*body[jBody].dKecc*body[iBody].dHecc*body[iBody].dKecc));
}

double fdDdistDlDir13Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f35, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][34];
  f35 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][34] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][34]*dAlpha;

  return (1-iIndexJ)*f35*\
    (-(body[jBody].dKecc*(body[iBody].dKecc*body[iBody].dKecc-body[iBody].dHecc*body[iBody].dHecc)\
    +2*body[jBody].dHecc*body[iBody].dHecc*body[iBody].dKecc));
}

double fdDdistDlDir14Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f36, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][35];
  f36 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][35] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][35]*dAlpha;

  return (1-iIndexJ)*f36*(-(body[iBody].dHecc*\
    (body[jBody].dKecc*body[jBody].dKecc-body[jBody].dHecc*body[jBody].dHecc)\
    -2*body[iBody].dKecc*body[jBody].dHecc*body[jBody].dKecc));
}

double fdDdistDlDir14Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f36, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][35];
  f36 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][35] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][35]*dAlpha;

  return (1-iIndexJ)*f36*\
    (-(body[iBody].dKecc*(body[jBody].dKecc*body[jBody].dKecc-body[jBody].dHecc*body[jBody].dHecc)\
    +2*body[iBody].dHecc*body[jBody].dHecc*body[jBody].dKecc));
}

double fdDdistDlDir15Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f37, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][36];
  f37 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][36] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][36]*dAlpha;

  return (1-iIndexJ)*f37*(-(body[iBody].dHecc*\
    (body[iBody].dQinc*body[iBody].dQinc-body[iBody].dPinc*body[iBody].dPinc)\
    -2*body[iBody].dKecc*body[iBody].dPinc*body[iBody].dQinc));
}

double fdDdistDlDir15Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f37, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][36];
  f37 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][36] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][36]*dAlpha;

  return (1-iIndexJ)*f37*\
    (-(body[iBody].dKecc*(body[iBody].dQinc*body[iBody].dQinc-body[iBody].dPinc*body[iBody].dPinc)\
    +2*body[iBody].dHecc*body[iBody].dPinc*body[iBody].dQinc));
}

double fdDdistDlDir16Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f38, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][37];
  f38 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][37] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][37]*dAlpha;

  return (1-iIndexJ)*f38*(-(body[jBody].dHecc*\
    (body[iBody].dQinc*body[iBody].dQinc-body[iBody].dPinc*body[iBody].dPinc)\
    -2*body[jBody].dKecc*body[iBody].dPinc*body[iBody].dQinc));
}

double fdDdistDlDir16Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f38, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][37];
  f38 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][37] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][37]*dAlpha;

  return (1-iIndexJ)*f38*\
    (-(body[jBody].dKecc*(body[iBody].dQinc*body[iBody].dQinc-body[iBody].dPinc*body[iBody].dPinc)\
    +2*body[jBody].dHecc*body[iBody].dPinc*body[iBody].dQinc));
}

double fdDdistDlDir17Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f39, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][38];
  f39 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][38] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][38]*dAlpha;

  return (1-iIndexJ)*f39*((body[iBody].dHecc*\
    (body[iBody].dQinc*body[jBody].dQinc+body[iBody].dPinc*body[jBody].dPinc)\
    +body[iBody].dKecc*(body[iBody].dQinc*body[jBody].dPinc-body[iBody].dPinc*body[jBody].dQinc)));
}

double fdDdistDlDir17Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f39, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][38];
  f39 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][38] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][38]*dAlpha;

  return (1-iIndexJ)*f39*\
    (-(body[iBody].dKecc*(body[iBody].dQinc*body[jBody].dQinc+body[iBody].dPinc*body[jBody].dPinc)\
    -body[iBody].dHecc*(body[iBody].dQinc*body[jBody].dPinc-body[iBody].dPinc*body[jBody].dQinc)));
}

double fdDdistDlDir18Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f40, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][39];
  f40 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][39] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][39]*dAlpha;

  return (1-iIndexJ)*f40*((body[iBody].dHecc*\
    (body[iBody].dQinc*body[jBody].dQinc+body[iBody].dPinc*body[jBody].dPinc)\
    +body[iBody].dKecc*(body[jBody].dQinc*body[iBody].dPinc-body[jBody].dPinc*body[iBody].dQinc)));
}

double fdDdistDlDir18Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f40, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][39];
  f40 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][39] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][39]*dAlpha;

  return (1-iIndexJ)*f40*\
    (-(body[iBody].dKecc*(body[iBody].dQinc*body[jBody].dQinc+body[iBody].dPinc*body[jBody].dPinc)\
    -body[iBody].dHecc*(body[jBody].dQinc*body[iBody].dPinc-body[jBody].dPinc*body[iBody].dQinc)));
}

double fdDdistDlDir19Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f41, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][40];
  f41 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][40] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][40]*dAlpha;

  return (1-iIndexJ)*f41*((body[iBody].dPinc*\
    (body[jBody].dQinc*body[iBody].dKecc+body[jBody].dPinc*body[iBody].dHecc)\
    +body[iBody].dQinc*(body[jBody].dPinc*body[iBody].dKecc-body[jBody].dQinc*body[iBody].dHecc)));
}

double fdDdistDlDir19Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f41, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][40];
  f41 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][40] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][40]*dAlpha;

  return (1-iIndexJ)*f41*\
    (-(body[iBody].dQinc*(body[jBody].dQinc*body[iBody].dKecc+body[jBody].dPinc*body[iBody].dHecc)\
    -body[iBody].dPinc*(body[jBody].dPinc*body[iBody].dKecc-body[jBody].dQinc*body[iBody].dHecc)));
}

double fdDdistDlDir110Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f42, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][41];
  f42 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][41] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][41]*dAlpha;

  return (1-iIndexJ)*f42*((body[jBody].dHecc*\
    (body[iBody].dQinc*body[jBody].dQinc+body[iBody].dPinc*body[jBody].dPinc)\
    +body[jBody].dKecc*(body[iBody].dQinc*body[jBody].dPinc-body[iBody].dPinc*body[jBody].dQinc)));
}

double fdDdistDlDir110Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f42, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][41];
  f42 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][41] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][41]*dAlpha;

  return (1-iIndexJ)*f42*\
    (-(body[jBody].dKecc*(body[iBody].dQinc*body[jBody].dQinc+body[iBody].dPinc*body[jBody].dPinc)\
    -body[jBody].dHecc*(body[iBody].dQinc*body[jBody].dPinc-body[iBody].dPinc*body[jBody].dQinc)));
}

double fdDdistDlDir111Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f43, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][42];
  f43 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][42] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][42]*dAlpha;

  return (1-iIndexJ)*f43*((body[jBody].dHecc*\
    (body[iBody].dQinc*body[jBody].dQinc+body[iBody].dPinc*body[jBody].dPinc)\
    +body[jBody].dKecc*(body[jBody].dQinc*body[iBody].dPinc-body[jBody].dPinc*body[iBody].dQinc)));
}

double fdDdistDlDir111Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f43, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][42];
  f43 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][42] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][42]*dAlpha;

  return (1-iIndexJ)*f43*\
    (-(body[jBody].dKecc*(body[iBody].dQinc*body[jBody].dQinc+body[iBody].dPinc*body[jBody].dPinc)\
    -body[jBody].dHecc*(body[jBody].dQinc*body[iBody].dPinc-body[jBody].dPinc*body[iBody].dQinc)));
}

double fdDdistDlDir112Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f44, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][43];
  f44 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][43] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][43]*dAlpha;

  return (1-iIndexJ)*f44*((body[iBody].dPinc*\
    (body[jBody].dQinc*body[jBody].dKecc+body[jBody].dPinc*body[jBody].dHecc)\
    +body[iBody].dQinc*(body[jBody].dPinc*body[jBody].dKecc-body[jBody].dQinc*body[jBody].dHecc)));
}

double fdDdistDlDir112Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f44, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][43];
  f44 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][43] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][43]*dAlpha;

  return (1-iIndexJ)*f44*\
    (-(body[iBody].dQinc*(body[jBody].dQinc*body[jBody].dKecc+body[jBody].dPinc*body[jBody].dHecc)\
    -body[iBody].dPinc*(body[jBody].dPinc*body[jBody].dKecc-body[jBody].dQinc*body[jBody].dHecc)));
}

double fdDdistDlDir113Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f37, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][36];
  f37 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][36] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][36]*dAlpha;

  return (1-iIndexJ)*f37*(-(body[iBody].dHecc*\
    (body[jBody].dQinc*body[jBody].dQinc-body[jBody].dPinc*body[jBody].dPinc)\
    -2*body[iBody].dKecc*body[jBody].dPinc*body[jBody].dQinc));
}

double fdDdistDlDir113Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f37, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][36];
  f37 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][36] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][36]*dAlpha;

  return (1-iIndexJ)*f37*\
    (-(body[iBody].dKecc*(body[jBody].dQinc*body[jBody].dQinc-body[jBody].dPinc*body[jBody].dPinc)\
    +2*body[iBody].dHecc*body[jBody].dPinc*body[jBody].dQinc));
}

double fdDdistDlDir114Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f38, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][37];
  f38 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][37] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][37]*dAlpha;

  return (1-iIndexJ)*f38*(-(body[jBody].dHecc*\
    (body[jBody].dQinc*body[jBody].dQinc-body[jBody].dPinc*body[jBody].dPinc)\
    -2*body[jBody].dKecc*body[jBody].dPinc*body[jBody].dQinc));
}

double fdDdistDlDir114Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f38, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][37];
  f38 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][37] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][37]*dAlpha;

  return (1-iIndexJ)*f38*\
    (-(body[jBody].dKecc*(body[jBody].dQinc*body[jBody].dQinc-body[jBody].dPinc*body[jBody].dPinc)\
    +2*body[jBody].dHecc*body[jBody].dPinc*body[jBody].dQinc));
}

double fdDdistDlExt13Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return -(-2+(body[iBody].dHecc*body[iBody].dHecc+body[iBody].dKecc*body[iBody].dKecc)+\
      1.5*(body[jBody].dHecc*body[jBody].dHecc+body[jBody].dKecc*body[jBody].dKecc)+\
      2.*(body[iBody].dPinc*body[iBody].dPinc+body[iBody].dQinc*body[iBody].dQinc+\
      body[jBody].dPinc*body[jBody].dPinc+body[jBody].dQinc*body[jBody].dQinc))\
        *(body[jBody].dHecc);
}

double fdDdistDlExt13Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return -(-2+(body[iBody].dHecc*body[iBody].dHecc+body[iBody].dKecc*body[iBody].dKecc)+\
      1.5*(body[jBody].dHecc*body[jBody].dHecc+body[jBody].dKecc*body[jBody].dKecc)+\
      2.*(body[iBody].dPinc*body[iBody].dPinc+body[iBody].dQinc*body[iBody].dQinc+\
      body[jBody].dPinc*body[jBody].dPinc+body[jBody].dQinc*body[jBody].dQinc))\
        *(-body[jBody].dKecc);
}

double fdDdistDlExt16Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
   return 27./8*((body[iBody].dHecc*\
    (-body[jBody].dKecc*body[jBody].dKecc+body[jBody].dHecc*body[jBody].dHecc)\
    +2*body[iBody].dKecc*body[jBody].dHecc*body[jBody].dKecc));
}

double fdDdistDlExt16Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
   return 27./8*(-(body[iBody].dKecc*(body[jBody].dKecc*body[jBody].dKecc-\
    body[jBody].dHecc*body[jBody].dHecc)\
    +2*body[iBody].dHecc*body[jBody].dHecc*body[jBody].dKecc));
}

double fdDdistDlExt111Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return -4*(-(body[iBody].dPinc*\
    (-body[jBody].dQinc*body[jBody].dKecc+body[jBody].dPinc*body[jBody].dHecc)\
    +body[iBody].dQinc*(body[jBody].dPinc*body[jBody].dKecc+body[jBody].dQinc*body[jBody].dHecc)));
}

double fdDdistDlExt111Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return -4*((body[iBody].dQinc*(body[jBody].dQinc*body[jBody].dKecc-body[jBody].dPinc*body[jBody].dHecc)\
    +body[iBody].dPinc*(body[jBody].dPinc*body[jBody].dKecc+body[jBody].dQinc*body[jBody].dHecc)));
}

double fdDdistDlDir21Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f45, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][44];
  f45 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][44] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][44]*dAlpha;

  return (2.-iIndexJ)*f45*(2*(body[iBody].dHecc*body[iBody].dKecc));
}

double fdDdistDlDir21Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f45, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][44];
  f45 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][44] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][44]*dAlpha;

  return (2.-iIndexJ)*f45*(-(body[iBody].dKecc*body[iBody].dKecc-body[iBody].dHecc*body[iBody].dHecc));
}

double fdDdistDlDir22Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f49, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][48];
  f49 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][48] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][48]*dAlpha;

  return (2.-iIndexJ)*f49*((body[iBody].dHecc*body[jBody].dKecc+body[iBody].dKecc*body[jBody].dHecc));
}

double fdDdistDlDir22Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f49, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][48];
  f49 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][48] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][48]*dAlpha;

  return (2.-iIndexJ)*f49*(-(body[iBody].dKecc*body[jBody].dKecc-body[iBody].dHecc*body[jBody].dHecc));
}

double fdDdistDlDir23Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f53, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][52];
  f53 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][52] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][52]*dAlpha;

  return (2.-iIndexJ)*f53*(2*(body[jBody].dHecc*body[jBody].dKecc));
}

double fdDdistDlDir23Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f53, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][52];
  f53 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][52] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][52]*dAlpha;

  return (2.-iIndexJ)*f53*(-(body[jBody].dKecc*body[jBody].dKecc-body[jBody].dHecc*body[jBody].dHecc));
}

double fdDdistDlDir24Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f57, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][56];
  f57 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][56] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][56]*dAlpha;

  return (2.-iIndexJ)*f57*(2*(body[iBody].dPinc*body[iBody].dQinc));
}

double fdDdistDlDir24Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f57, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][56];
  f57 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][56] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][56]*dAlpha;

  return (2.-iIndexJ)*f57*(-(body[iBody].dQinc*body[iBody].dQinc-body[iBody].dPinc*body[iBody].dPinc));
}

double fdDdistDlDir25Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f62, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][61];
  f62 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][61] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][61]*dAlpha;

  return (2.-iIndexJ)*f62*((body[jBody].dPinc*body[iBody].dQinc+body[iBody].dPinc*body[jBody].dQinc));
}

double fdDdistDlDir25Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f62, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][61];
  f62 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][61] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][61]*dAlpha;

  return (2.-iIndexJ)*f62*(-(body[iBody].dQinc*body[jBody].dQinc-body[iBody].dPinc*body[jBody].dPinc));
}

double fdDdistDlDir26Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f57, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][56];
  f57 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][56] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][56]*dAlpha;

  return (2.-iIndexJ)*f57*(2*(body[jBody].dPinc*body[jBody].dQinc));
}

double fdDdistDlDir26Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f57, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][56];
  f57 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][56] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][56]*dAlpha;

  return (2.-iIndexJ)*f57*(-(body[jBody].dQinc*body[jBody].dQinc-body[jBody].dPinc*body[jBody].dPinc));
}

double fdDdistDlExt25Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return 27./8*(2*(body[jBody].dHecc*body[jBody].dKecc));
}

double fdDdistDlExt25Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return 27./8*(-(body[jBody].dKecc*body[jBody].dKecc-body[jBody].dHecc*body[jBody].dHecc));
}

double fdDdistDlDir31Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f82, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][81];
  f82 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][81] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][81]*dAlpha;

  return (3-iIndexJ)*f82*((-body[iBody].dHecc*body[iBody].dHecc*body[iBody].dHecc\
          +3*body[iBody].dKecc*body[iBody].dKecc*body[iBody].dHecc));
}

double fdDdistDlDir31Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f82, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][81];
  f82 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][81] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][81]*dAlpha;

  return (3-iIndexJ)*f82*(-(body[iBody].dKecc*body[iBody].dKecc*body[iBody].dKecc\
          -3*body[iBody].dHecc*body[iBody].dHecc*body[iBody].dKecc));
}

double fdDdistDlDir32Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f83, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][82];
  f83 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][82] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][82]*dAlpha;

  return (3-iIndexJ)*f83*((body[jBody].dHecc*(body[iBody].dKecc*body[iBody].dKecc-body[iBody].dHecc*body[iBody].dHecc)\
          +2*body[jBody].dKecc*body[iBody].dKecc*body[iBody].dHecc));
}

double fdDdistDlDir32Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f83, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][82];
  f83 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][82] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][82]*dAlpha;

  return (3-iIndexJ)*f83*(-(body[jBody].dKecc*(body[iBody].dKecc*body[iBody].dKecc-body[iBody].dHecc*body[iBody].dHecc)\
          -2*body[jBody].dHecc*body[iBody].dHecc*body[iBody].dKecc));
}

double fdDdistDlDir33Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f84, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][83];
  f84 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][83] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][83]*dAlpha;

  return (3-iIndexJ)*f84*((body[iBody].dHecc*(body[jBody].dKecc*body[jBody].dKecc-body[jBody].dHecc*body[jBody].dHecc)\
          +2*body[iBody].dKecc*body[jBody].dKecc*body[jBody].dHecc));
}

double fdDdistDlDir33Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f84, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][83];
  f84 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][83] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][83]*dAlpha;

  return (3-iIndexJ)*f84*(-(body[iBody].dKecc*(body[jBody].dKecc*body[jBody].dKecc-body[jBody].dHecc*body[jBody].dHecc)\
          -2*body[iBody].dHecc*body[jBody].dHecc*body[jBody].dKecc));
}

double fdDdistDlDir34Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f85, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][84];
  f85 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][84] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][84]*dAlpha;

  return (3-iIndexJ)*f85*((-body[jBody].dHecc*body[jBody].dHecc*body[jBody].dHecc\
          +3*body[jBody].dKecc*body[jBody].dKecc*body[jBody].dHecc));
}

double fdDdistDlDir34Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f85, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][84];
  f85 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][84] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][84]*dAlpha;

  return (3-iIndexJ)*f85*(-(body[jBody].dKecc*body[jBody].dKecc*body[jBody].dKecc\
          -3*body[jBody].dHecc*body[jBody].dHecc*body[jBody].dKecc));
}

double fdDdistDlDir35Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f86, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][85];
  f86 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][85] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][85]*dAlpha;

  return (3-iIndexJ)*f86*((body[iBody].dHecc*(body[iBody].dQinc*body[iBody].dQinc-body[iBody].dPinc*body[iBody].dPinc)\
          +2*body[iBody].dKecc*body[iBody].dPinc*body[iBody].dQinc));
}

double fdDdistDlDir35Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f86, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][85];
  f86 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][85] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][85]*dAlpha;

  return (3-iIndexJ)*f86*(-(body[iBody].dKecc*(body[iBody].dQinc*body[iBody].dQinc-body[iBody].dPinc*body[iBody].dPinc)\
          -2*body[iBody].dHecc*body[iBody].dPinc*body[iBody].dQinc));
}

double fdDdistDlDir36Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f87, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][86];
  f87 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][86] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][86]*dAlpha;

  return (3-iIndexJ)*f87*((body[jBody].dHecc*(body[iBody].dQinc*body[iBody].dQinc-body[iBody].dPinc*body[iBody].dPinc)\
          +2*body[jBody].dKecc*body[iBody].dPinc*body[iBody].dQinc));
}

double fdDdistDlDir36Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f87, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][86];
  f87 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][86] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][86]*dAlpha;

  return (3-iIndexJ)*f87*(-(body[jBody].dKecc*(body[iBody].dQinc*body[iBody].dQinc-body[iBody].dPinc*body[iBody].dPinc)\
          -2*body[jBody].dHecc*body[iBody].dPinc*body[iBody].dQinc));
}

double fdDdistDlDir37Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f88, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][87];
  f88 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][87] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][87]*dAlpha;

  return (3-iIndexJ)*f88*((body[iBody].dHecc*(body[jBody].dQinc*body[iBody].dQinc-body[jBody].dPinc*body[iBody].dPinc)\
          +body[iBody].dKecc*(body[jBody].dPinc*body[iBody].dQinc+body[jBody].dQinc*body[iBody].dPinc)));
}

double fdDdistDlDir37Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f88, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][87];
  f88 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][87] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][87]*dAlpha;

  return (3-iIndexJ)*f88*(-(body[iBody].dKecc*(body[jBody].dQinc*body[iBody].dQinc-body[jBody].dPinc*body[iBody].dPinc)\
          -body[iBody].dHecc*(body[jBody].dPinc*body[iBody].dQinc+body[jBody].dQinc*body[iBody].dPinc)));
}

double fdDdistDlDir38Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f89, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][88];
  f89 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][88] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][88]*dAlpha;

  return (3-iIndexJ)*f89*((body[jBody].dHecc*(body[jBody].dQinc*body[iBody].dQinc-body[jBody].dPinc*body[iBody].dPinc)\
          +body[jBody].dKecc*(body[jBody].dPinc*body[iBody].dQinc+body[jBody].dQinc*body[iBody].dPinc)));
}

double fdDdistDlDir38Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f89, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][88];
  f89 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][88] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][88]*dAlpha;

  return (3-iIndexJ)*f89*(-(body[jBody].dKecc*(body[jBody].dQinc*body[iBody].dQinc-body[jBody].dPinc*body[iBody].dPinc)\
          -body[jBody].dHecc*(body[jBody].dPinc*body[iBody].dQinc+body[jBody].dQinc*body[iBody].dPinc)));
}

double fdDdistDlDir39Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f86, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][85];
  f86 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][85] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][85]*dAlpha;

  return (3-iIndexJ)*f86*((body[iBody].dHecc*(body[jBody].dQinc*body[jBody].dQinc-body[jBody].dPinc*body[jBody].dPinc)\
          +2*body[iBody].dKecc*body[jBody].dPinc*body[jBody].dQinc));
}

double fdDdistDlDir39Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f86, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][85];
  f86 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][85] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][85]*dAlpha;

  return (3-iIndexJ)*f86*(-(body[iBody].dKecc*(body[jBody].dQinc*body[jBody].dQinc-body[jBody].dPinc*body[jBody].dPinc)\
          -2*body[iBody].dHecc*body[jBody].dPinc*body[jBody].dQinc));
}

double fdDdistDlDir310Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f87, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][86];
  f87 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][86] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][86]*dAlpha;

  return (3-iIndexJ)*f87*((body[jBody].dHecc*(body[jBody].dQinc*body[jBody].dQinc-body[jBody].dPinc*body[jBody].dPinc)\
          +2*body[jBody].dKecc*body[jBody].dPinc*body[jBody].dQinc));
}

double fdDdistDlDir310Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f87, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->daAlpha0[iIndexJ][system->iaLaplaceN[iBody][jBody]][86];
  f87 = system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][86] + \
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][86]*dAlpha;

  return (3-iIndexJ)*f87*(-(body[jBody].dKecc*(body[jBody].dQinc*body[jBody].dQinc-body[jBody].dPinc*body[jBody].dPinc)\
          -2*body[jBody].dHecc*body[jBody].dPinc*body[jBody].dQinc));
}

double fdDdistresDLambdaDirCos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ, int iOrder) {
  double y;
  if (iOrder == 1) {
    y = ( fdDdistDlDir11Cos(body, system, iBody, jBody, iIndexJ)\
      + fdDdistDlDir12Cos(body, system, iBody, jBody, iIndexJ) \
      + fdDdistDlDir13Cos(body, system, iBody, jBody, iIndexJ)\
      + fdDdistDlDir14Cos(body, system, iBody, jBody, iIndexJ)\
      + fdDdistDlDir15Cos(body, system, iBody, jBody, iIndexJ)\
      + fdDdistDlDir16Cos(body, system, iBody, jBody, iIndexJ)\
      + fdDdistDlDir17Cos(body, system, iBody, jBody, iIndexJ)\
      + fdDdistDlDir18Cos(body, system, iBody, jBody, iIndexJ)\
      + fdDdistDlDir19Cos(body, system, iBody, jBody, iIndexJ)\
      + fdDdistDlDir110Cos(body, system, iBody, jBody, iIndexJ)\
      + fdDdistDlDir111Cos(body, system, iBody, jBody, iIndexJ)\
      + fdDdistDlDir112Cos(body, system, iBody, jBody, iIndexJ)\
      + fdDdistDlDir113Cos(body, system, iBody, jBody, iIndexJ)\
      + fdDdistDlDir114Cos(body, system, iBody, jBody, iIndexJ) );
  } else if (iOrder == 2) {
    y = ( fdDdistDlDir21Cos(body, system, iBody, jBody, iIndexJ) \
    + fdDdistDlDir22Cos(body, system, iBody, jBody, iIndexJ) \
    + fdDdistDlDir23Cos(body, system, iBody, jBody, iIndexJ) \
    + fdDdistDlDir24Cos(body, system, iBody, jBody, iIndexJ) \
    + fdDdistDlDir25Cos(body, system, iBody, jBody, iIndexJ) \
    + fdDdistDlDir26Cos(body, system, iBody, jBody, iIndexJ) );
  } else if (iOrder == 3) {
    y = ( fdDdistDlDir31Cos(body, system, iBody, jBody, iIndexJ) \
    + fdDdistDlDir32Cos(body, system, iBody, jBody, iIndexJ) \
    + fdDdistDlDir33Cos(body, system, iBody, jBody, iIndexJ) \
    + fdDdistDlDir34Cos(body, system, iBody, jBody, iIndexJ) \
    + fdDdistDlDir35Cos(body, system, iBody, jBody, iIndexJ) \
    + fdDdistDlDir36Cos(body, system, iBody, jBody, iIndexJ) \
    + fdDdistDlDir37Cos(body, system, iBody, jBody, iIndexJ) \
    + fdDdistDlDir38Cos(body, system, iBody, jBody, iIndexJ) \
    + fdDdistDlDir39Cos(body, system, iBody, jBody, iIndexJ) \
    + fdDdistDlDir310Cos(body, system, iBody, jBody, iIndexJ) );
  }
  return y;
}

double fdDdistresDLambdaDirSin(BODY *body, SYSTEM *system, int iBody,int jBody, int iIndexJ, int iOrder) {
  double y;
  if (iOrder == 1) {
    y = ( fdDdistDlDir11Sin(body, system, iBody, jBody, iIndexJ)\
      + fdDdistDlDir12Sin(body, system, iBody, jBody, iIndexJ) \
      + fdDdistDlDir13Sin(body, system, iBody, jBody, iIndexJ)\
      + fdDdistDlDir14Sin(body, system, iBody, jBody, iIndexJ)\
      + fdDdistDlDir15Sin(body, system, iBody, jBody, iIndexJ)\
      + fdDdistDlDir16Sin(body, system, iBody, jBody, iIndexJ)\
      + fdDdistDlDir17Sin(body, system, iBody, jBody, iIndexJ)\
      + fdDdistDlDir18Sin(body, system, iBody, jBody, iIndexJ)\
      + fdDdistDlDir19Sin(body, system, iBody, jBody, iIndexJ)\
      + fdDdistDlDir110Sin(body, system, iBody, jBody, iIndexJ)\
      + fdDdistDlDir111Sin(body, system, iBody, jBody, iIndexJ)\
      + fdDdistDlDir112Sin(body, system, iBody, jBody, iIndexJ)\
      + fdDdistDlDir113Sin(body, system, iBody, jBody, iIndexJ)\
      + fdDdistDlDir114Sin(body, system, iBody, jBody, iIndexJ) );
  } else if (iOrder == 2) {
    y = ( fdDdistDlDir21Sin(body, system, iBody, jBody, iIndexJ) \
    + fdDdistDlDir22Sin(body, system, iBody, jBody, iIndexJ) \
    + fdDdistDlDir23Sin(body, system, iBody, jBody, iIndexJ) \
    + fdDdistDlDir24Sin(body, system, iBody, jBody, iIndexJ) \
    + fdDdistDlDir25Sin(body, system, iBody, jBody, iIndexJ) \
    + fdDdistDlDir26Sin(body, system, iBody, jBody, iIndexJ) );
  } else if (iOrder == 3) {
    y = ( fdDdistDlDir31Sin(body, system, iBody, jBody, iIndexJ) \
    + fdDdistDlDir32Sin(body, system, iBody, jBody, iIndexJ) \
    + fdDdistDlDir33Sin(body, system, iBody, jBody, iIndexJ) \
    + fdDdistDlDir34Sin(body, system, iBody, jBody, iIndexJ) \
    + fdDdistDlDir35Sin(body, system, iBody, jBody, iIndexJ) \
    + fdDdistDlDir36Sin(body, system, iBody, jBody, iIndexJ) \
    + fdDdistDlDir37Sin(body, system, iBody, jBody, iIndexJ) \
    + fdDdistDlDir38Sin(body, system, iBody, jBody, iIndexJ) \
    + fdDdistDlDir39Sin(body, system, iBody, jBody, iIndexJ) \
    + fdDdistDlDir310Sin(body, system, iBody, jBody, iIndexJ) );
  }
  return y;
}

double fdDdistresDLambda(BODY *body, SYSTEM *system, int iBody,int jBody, int iIndexJ, int iOrder, int iRes) {
  double y, yc, ys, dMfac, dSemiPrm;
  dMfac = KGAUSS*KGAUSS*body[jBody].dMass/MSUN;
  dSemiPrm = body[jBody].dSemi/AUCM;

  yc = fdDdistresDLambdaDirCos(body,system,iBody, jBody,iIndexJ,iOrder);
  ys = fdDdistresDLambdaDirSin(body,system,iBody, jBody,iIndexJ,iOrder);
  if (iOrder == 1) {
    if (iIndexJ == 2) {
      yc += (fdDdistDlExt13Cos(body,system,iBody,jBody,iIndexJ)\
         + fdDdistDlExt111Cos(body,system,iBody,jBody,iIndexJ))\
            *body[iBody].dSemi/body[jBody].dSemi;
      ys += (fdDdistDlExt13Sin(body,system,iBody,jBody,iIndexJ)\
         + fdDdistDlExt111Sin(body,system,iBody,jBody,iIndexJ))\
            *body[iBody].dSemi/body[jBody].dSemi;
    } else if (iIndexJ == 3) {
      yc += (fdDdistDlExt16Cos(body,system,iBody,jBody,iIndexJ))\
            *body[iBody].dSemi/body[jBody].dSemi;
      ys += (fdDdistDlExt16Sin(body,system,iBody,jBody,iIndexJ))\
            *body[iBody].dSemi/body[jBody].dSemi;
    }
  } else if (iOrder == 2) {
    if (iIndexJ == 3) {
      yc += (fdDdistDlExt25Cos(body,system,iBody,jBody,iIndexJ))\
            *body[iBody].dSemi/body[jBody].dSemi;
      ys += (fdDdistDlExt25Sin(body,system,iBody,jBody,iIndexJ))\
            *body[iBody].dSemi/body[jBody].dSemi;
    }
  }

  y = (yc*fdCosLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes) \
       + ys*fdSinLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)) * dMfac/dSemiPrm;
  return y;
}

//---------dR/dlambda'------------------------------------------------------------------

double fdDdistDlPrmDir11Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return (1.*iIndexJ)/(1.-iIndexJ)*fdDdistDlDir11Cos(body,system,jBody,iBody,iIndexJ);
}

double fdDdistDlPrmDir11Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return (1.*iIndexJ)/(1.-iIndexJ)*fdDdistDlDir11Sin(body,system,jBody,iBody,iIndexJ);
}

double fdDdistDlPrmDir12Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return (1.*iIndexJ)/(1.-iIndexJ)*fdDdistDlDir12Cos(body,system,jBody,iBody,iIndexJ);
}

double fdDdistDlPrmDir12Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return (1.*iIndexJ)/(1.-iIndexJ)*fdDdistDlDir12Sin(body,system,jBody,iBody,iIndexJ);
}

double fdDdistDlPrmDir13Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return (1.*iIndexJ)/(1.-iIndexJ)*fdDdistDlDir13Cos(body,system,jBody,iBody,iIndexJ);
}

double fdDdistDlPrmDir13Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return (1.*iIndexJ)/(1.-iIndexJ)*fdDdistDlDir13Sin(body,system,jBody,iBody,iIndexJ);
}

double fdDdistDlPrmDir14Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return (1.*iIndexJ)/(1.-iIndexJ)*fdDdistDlDir14Cos(body,system,jBody,iBody,iIndexJ);
}

double fdDdistDlPrmDir14Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return (1.*iIndexJ)/(1.-iIndexJ)*fdDdistDlDir14Sin(body,system,jBody,iBody,iIndexJ);
}

double fdDdistDlPrmDir15Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return (1.*iIndexJ)/(1.-iIndexJ)*fdDdistDlDir15Cos(body,system,jBody,iBody,iIndexJ);
}

double fdDdistDlPrmDir15Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return (1.*iIndexJ)/(1.-iIndexJ)*fdDdistDlDir15Sin(body,system,jBody,iBody,iIndexJ);
}

double fdDdistDlPrmDir16Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return (1.*iIndexJ)/(1.-iIndexJ)*fdDdistDlDir16Cos(body,system,jBody,iBody,iIndexJ);
}

double fdDdistDlPrmDir16Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return (1.*iIndexJ)/(1.-iIndexJ)*fdDdistDlDir16Sin(body,system,jBody,iBody,iIndexJ);
}

double fdDdistDlPrmDir17Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return (1.*iIndexJ)/(1.-iIndexJ)*fdDdistDlDir17Cos(body,system,jBody,iBody,iIndexJ);
}

double fdDdistDlPrmDir17Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return (1.*iIndexJ)/(1.-iIndexJ)*fdDdistDlDir17Sin(body,system,jBody,iBody,iIndexJ);
}

double fdDdistDlPrmDir18Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return (1.*iIndexJ)/(1.-iIndexJ)*fdDdistDlDir18Cos(body,system,jBody,iBody,iIndexJ);
}

double fdDdistDlPrmDir18Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return (1.*iIndexJ)/(1.-iIndexJ)*fdDdistDlDir18Sin(body,system,jBody,iBody,iIndexJ);
}

double fdDdistDlPrmDir19Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return (1.*iIndexJ)/(1.-iIndexJ)*fdDdistDlDir19Cos(body,system,jBody,iBody,iIndexJ);
}

double fdDdistDlPrmDir19Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return (1.*iIndexJ)/(1.-iIndexJ)*fdDdistDlDir19Sin(body,system,jBody,iBody,iIndexJ);
}

double fdDdistDlPrmDir110Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return (1.*iIndexJ)/(1.-iIndexJ)*fdDdistDlDir110Cos(body,system,jBody,iBody,iIndexJ);
}

double fdDdistDlPrmDir110Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return (1.*iIndexJ)/(1.-iIndexJ)*fdDdistDlDir110Sin(body,system,jBody,iBody,iIndexJ);
}

double fdDdistDlPrmDir111Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return (1.*iIndexJ)/(1.-iIndexJ)*fdDdistDlDir111Cos(body,system,jBody,iBody,iIndexJ);
}

double fdDdistDlPrmDir111Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return (1.*iIndexJ)/(1.-iIndexJ)*fdDdistDlDir111Sin(body,system,jBody,iBody,iIndexJ);
}

double fdDdistDlPrmDir112Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return (1.*iIndexJ)/(1.-iIndexJ)*fdDdistDlDir112Cos(body,system,jBody,iBody,iIndexJ);
}

double fdDdistDlPrmDir112Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return (1.*iIndexJ)/(1.-iIndexJ)*fdDdistDlDir112Sin(body,system,jBody,iBody,iIndexJ);
}

double fdDdistDlPrmDir113Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return (1.*iIndexJ)/(1.-iIndexJ)*fdDdistDlDir113Cos(body,system,jBody,iBody,iIndexJ);
}

double fdDdistDlPrmDir113Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return (1.*iIndexJ)/(1.-iIndexJ)*fdDdistDlDir113Sin(body,system,jBody,iBody,iIndexJ);
}

double fdDdistDlPrmDir114Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return (1.*iIndexJ)/(1.-iIndexJ)*fdDdistDlDir114Cos(body,system,jBody,iBody,iIndexJ);
}

double fdDdistDlPrmDir114Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return (1.*iIndexJ)/(1.-iIndexJ)*fdDdistDlDir114Sin(body,system,jBody,iBody,iIndexJ);
}

double fdDdistDlPrmInt13Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return -0.5*fdDdistDlExt13Cos(body,system,jBody,iBody,iIndexJ);
}

double fdDdistDlPrmInt13Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return -0.5*fdDdistDlExt13Sin(body,system,jBody,iBody,iIndexJ);
}

double fdDdistDlPrmInt16Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return -2./3*fdDdistDlExt16Cos(body,system,jBody,iBody,iIndexJ);
}

double fdDdistDlPrmInt16Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return -2./3*fdDdistDlExt16Sin(body,system,jBody,iBody,iIndexJ);
}

double fdDdistDlPrmInt111Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return -0.5*fdDdistDlExt111Cos(body,system,jBody,iBody,iIndexJ);
}

double fdDdistDlPrmInt111Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return -0.5*fdDdistDlExt111Sin(body,system,jBody,iBody,iIndexJ);
}

double fdDdistDlPrmDir21Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return (1.*iIndexJ)/(2.-iIndexJ)*fdDdistDlDir21Cos(body,system,jBody,iBody,iIndexJ);
}

double fdDdistDlPrmDir21Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return (1.*iIndexJ)/(2.-iIndexJ)*fdDdistDlDir21Sin(body,system,jBody,iBody,iIndexJ);
}

double fdDdistDlPrmDir22Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return (1.*iIndexJ)/(2.-iIndexJ)*fdDdistDlDir22Cos(body,system,jBody,iBody,iIndexJ);
}

double fdDdistDlPrmDir22Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return (1.*iIndexJ)/(2.-iIndexJ)*fdDdistDlDir22Sin(body,system,jBody,iBody,iIndexJ);
}

double fdDdistDlPrmDir23Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return (1.*iIndexJ)/(2.-iIndexJ)*fdDdistDlDir23Cos(body,system,jBody,iBody,iIndexJ);
}

double fdDdistDlPrmDir23Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return (1.*iIndexJ)/(2.-iIndexJ)*fdDdistDlDir23Sin(body,system,jBody,iBody,iIndexJ);
}

double fdDdistDlPrmDir24Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return (1.*iIndexJ)/(2.-iIndexJ)*fdDdistDlDir24Cos(body,system,jBody,iBody,iIndexJ);
}

double fdDdistDlPrmDir24Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return (1.*iIndexJ)/(2.-iIndexJ)*fdDdistDlDir24Sin(body,system,jBody,iBody,iIndexJ);
}

double fdDdistDlPrmDir25Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return (1.*iIndexJ)/(2.-iIndexJ)*fdDdistDlDir25Cos(body,system,jBody,iBody,iIndexJ);
}

double fdDdistDlPrmDir25Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return (1.*iIndexJ)/(2.-iIndexJ)*fdDdistDlDir25Sin(body,system,jBody,iBody,iIndexJ);
}

double fdDdistDlPrmDir26Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return (1.*iIndexJ)/(2.-iIndexJ)*fdDdistDlDir26Cos(body,system,jBody,iBody,iIndexJ);
}

double fdDdistDlPrmDir26Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return (1.*iIndexJ)/(2.-iIndexJ)*fdDdistDlDir26Sin(body,system,jBody,iBody,iIndexJ);
}

double fdDdistDlPrmInt25Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return -1./3*fdDdistDlExt25Cos(body,system,jBody,iBody,iIndexJ);
}

double fdDdistDlPrmInt25Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return -1./3*fdDdistDlExt25Sin(body,system,jBody,iBody,iIndexJ);
}

double fdDdistDlPrmDir31Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return (1.*iIndexJ)/(3.-iIndexJ)*fdDdistDlDir31Cos(body,system,jBody,iBody,iIndexJ);
}

double fdDdistDlPrmDir31Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return (1.*iIndexJ)/(3.-iIndexJ)*fdDdistDlDir31Sin(body,system,jBody,iBody,iIndexJ);
}

double fdDdistDlPrmDir32Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return (1.*iIndexJ)/(3.-iIndexJ)*fdDdistDlDir32Cos(body,system,jBody,iBody,iIndexJ);
}

double fdDdistDlPrmDir32Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return (1.*iIndexJ)/(3.-iIndexJ)*fdDdistDlDir32Sin(body,system,jBody,iBody,iIndexJ);
}

double fdDdistDlPrmDir33Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return (1.*iIndexJ)/(3.-iIndexJ)*fdDdistDlDir33Cos(body,system,jBody,iBody,iIndexJ);
}

double fdDdistDlPrmDir33Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return (1.*iIndexJ)/(3.-iIndexJ)*fdDdistDlDir33Sin(body,system,jBody,iBody,iIndexJ);
}

double fdDdistDlPrmDir34Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return (1.*iIndexJ)/(3.-iIndexJ)*fdDdistDlDir34Cos(body,system,jBody,iBody,iIndexJ);
}

double fdDdistDlPrmDir34Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return (1.*iIndexJ)/(3.-iIndexJ)*fdDdistDlDir34Sin(body,system,jBody,iBody,iIndexJ);
}

double fdDdistDlPrmDir35Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return (1.*iIndexJ)/(3.-iIndexJ)*fdDdistDlDir35Cos(body,system,jBody,iBody,iIndexJ);
}

double fdDdistDlPrmDir35Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return (1.*iIndexJ)/(3.-iIndexJ)*fdDdistDlDir35Sin(body,system,jBody,iBody,iIndexJ);
}

double fdDdistDlPrmDir36Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return (1.*iIndexJ)/(3.-iIndexJ)*fdDdistDlDir36Cos(body,system,jBody,iBody,iIndexJ);
}

double fdDdistDlPrmDir36Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return (1.*iIndexJ)/(3.-iIndexJ)*fdDdistDlDir36Sin(body,system,jBody,iBody,iIndexJ);
}

double fdDdistDlPrmDir37Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return (1.*iIndexJ)/(3.-iIndexJ)*fdDdistDlDir37Cos(body,system,jBody,iBody,iIndexJ);
}

double fdDdistDlPrmDir37Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return (1.*iIndexJ)/(3.-iIndexJ)*fdDdistDlDir37Sin(body,system,jBody,iBody,iIndexJ);
}

double fdDdistDlPrmDir38Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return (1.*iIndexJ)/(3.-iIndexJ)*fdDdistDlDir38Cos(body,system,jBody,iBody,iIndexJ);
}

double fdDdistDlPrmDir38Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return (1.*iIndexJ)/(3.-iIndexJ)*fdDdistDlDir38Sin(body,system,jBody,iBody,iIndexJ);
}

double fdDdistDlPrmDir39Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return (1.*iIndexJ)/(3.-iIndexJ)*fdDdistDlDir39Cos(body,system,jBody,iBody,iIndexJ);
}

double fdDdistDlPrmDir39Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return (1.*iIndexJ)/(3.-iIndexJ)*fdDdistDlDir39Sin(body,system,jBody,iBody,iIndexJ);
}

double fdDdistDlPrmDir310Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return (1.*iIndexJ)/(3.-iIndexJ)*fdDdistDlDir310Cos(body,system,jBody,iBody,iIndexJ);
}

double fdDdistDlPrmDir310Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return (1.*iIndexJ)/(3.-iIndexJ)*fdDdistDlDir310Sin(body,system,jBody,iBody,iIndexJ);
}

double fdDdistresDLambdaPrimeDirCos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ, int iOrder) {
  double y;
  if (iOrder == 1) {
    y = ( fdDdistDlPrmDir11Cos(body, system, iBody, jBody, iIndexJ)\
      + fdDdistDlPrmDir12Cos(body, system, iBody, jBody, iIndexJ)\
      + fdDdistDlPrmDir13Cos(body, system, iBody, jBody, iIndexJ)\
      + fdDdistDlPrmDir14Cos(body, system, iBody, jBody, iIndexJ)\
      + fdDdistDlPrmDir15Cos(body, system, iBody, jBody, iIndexJ)\
      + fdDdistDlPrmDir16Cos(body, system, iBody, jBody, iIndexJ)\
      + fdDdistDlPrmDir17Cos(body, system, iBody, jBody, iIndexJ) \
      + fdDdistDlPrmDir18Cos(body, system, iBody, jBody, iIndexJ)\
      + fdDdistDlPrmDir19Cos(body, system, iBody, jBody, iIndexJ)\
      + fdDdistDlPrmDir110Cos(body, system, iBody, jBody, iIndexJ)\
      + fdDdistDlPrmDir111Cos(body, system, iBody, jBody, iIndexJ)\
      + fdDdistDlPrmDir112Cos(body, system, iBody, jBody, iIndexJ)\
      + fdDdistDlPrmDir113Cos(body, system, iBody, jBody, iIndexJ)\
      + fdDdistDlPrmDir114Cos(body, system, iBody, jBody, iIndexJ) );
  } else if (iOrder == 2) {
    y = ( fdDdistDlPrmDir21Cos(body, system, iBody, jBody, iIndexJ) \
    + fdDdistDlPrmDir22Cos(body, system, iBody, jBody, iIndexJ)
    + fdDdistDlPrmDir23Cos(body, system, iBody, jBody, iIndexJ) \
    + fdDdistDlPrmDir24Cos(body, system, iBody, jBody, iIndexJ)
    + fdDdistDlPrmDir25Cos(body, system, iBody, jBody, iIndexJ) \
    + fdDdistDlPrmDir26Cos(body, system, iBody, jBody, iIndexJ) );
  } else if (iOrder == 3) {
    y = ( fdDdistDlPrmDir31Cos(body, system, iBody, jBody, iIndexJ) \
    + fdDdistDlPrmDir32Cos(body, system, iBody, jBody, iIndexJ) \
    + fdDdistDlPrmDir33Cos(body, system, iBody, jBody, iIndexJ) \
    + fdDdistDlPrmDir34Cos(body, system, iBody, jBody, iIndexJ) \
    + fdDdistDlPrmDir35Cos(body, system, iBody, jBody, iIndexJ) \
    + fdDdistDlPrmDir36Cos(body, system, iBody, jBody, iIndexJ) \
    + fdDdistDlPrmDir37Cos(body, system, iBody, jBody, iIndexJ) \
    + fdDdistDlPrmDir38Cos(body, system, iBody, jBody, iIndexJ) \
    + fdDdistDlPrmDir39Cos(body, system, iBody, jBody, iIndexJ) \
    + fdDdistDlPrmDir310Cos(body, system, iBody, jBody, iIndexJ) );
  }
  return y;
}

double fdDdistresDLambdaPrimeDirSin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ, int iOrder) {
  double y;
  if (iOrder == 1) {
    y = ( fdDdistDlPrmDir11Sin(body, system, iBody, jBody, iIndexJ)\
      + fdDdistDlPrmDir12Sin(body, system, iBody, jBody, iIndexJ)\
      + fdDdistDlPrmDir13Sin(body, system, iBody, jBody, iIndexJ)\
      + fdDdistDlPrmDir14Sin(body, system, iBody, jBody, iIndexJ)\
      + fdDdistDlPrmDir15Sin(body, system, iBody, jBody, iIndexJ)\
      + fdDdistDlPrmDir16Sin(body, system, iBody, jBody, iIndexJ)\
      + fdDdistDlPrmDir17Sin(body, system, iBody, jBody, iIndexJ) \
      + fdDdistDlPrmDir18Sin(body, system, iBody, jBody, iIndexJ)\
      + fdDdistDlPrmDir19Sin(body, system, iBody, jBody, iIndexJ)\
      + fdDdistDlPrmDir110Sin(body, system, iBody, jBody, iIndexJ)\
      + fdDdistDlPrmDir111Sin(body, system, iBody, jBody, iIndexJ)\
      + fdDdistDlPrmDir112Sin(body, system, iBody, jBody, iIndexJ)\
      + fdDdistDlPrmDir113Sin(body, system, iBody, jBody, iIndexJ)\
      + fdDdistDlPrmDir114Sin(body, system, iBody, jBody, iIndexJ) );
  } else if (iOrder == 2) {
    y = ( fdDdistDlPrmDir21Sin(body, system, iBody, jBody, iIndexJ) \
    + fdDdistDlPrmDir22Sin(body, system, iBody, jBody, iIndexJ)
    + fdDdistDlPrmDir23Sin(body, system, iBody, jBody, iIndexJ) \
    + fdDdistDlPrmDir24Sin(body, system, iBody, jBody, iIndexJ)
    + fdDdistDlPrmDir25Sin(body, system, iBody, jBody, iIndexJ) \
    + fdDdistDlPrmDir26Sin(body, system, iBody, jBody, iIndexJ) );
  } else if (iOrder == 3) {
    y = ( fdDdistDlPrmDir31Sin(body, system, iBody, jBody, iIndexJ) \
    + fdDdistDlPrmDir32Sin(body, system, iBody, jBody, iIndexJ) \
    + fdDdistDlPrmDir33Sin(body, system, iBody, jBody, iIndexJ) \
    + fdDdistDlPrmDir34Sin(body, system, iBody, jBody, iIndexJ) \
    + fdDdistDlPrmDir35Sin(body, system, iBody, jBody, iIndexJ) \
    + fdDdistDlPrmDir36Sin(body, system, iBody, jBody, iIndexJ) \
    + fdDdistDlPrmDir37Sin(body, system, iBody, jBody, iIndexJ) \
    + fdDdistDlPrmDir38Sin(body, system, iBody, jBody, iIndexJ) \
    + fdDdistDlPrmDir39Sin(body, system, iBody, jBody, iIndexJ) \
    + fdDdistDlPrmDir310Sin(body, system, iBody, jBody, iIndexJ) );
  }
  return y;
}

double fdDdistresDLambdaPrime(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ, int iOrder, int iRes) {
  double y, yc, ys, dMfac, dSemiPrm;
  dMfac = KGAUSS*KGAUSS*body[jBody].dMass/MSUN;
  dSemiPrm = body[iBody].dSemi/AUCM;

  yc = fdDdistresDLambdaPrimeDirCos(body,system,iBody, jBody,iIndexJ,iOrder);
  ys = fdDdistresDLambdaPrimeDirSin(body,system,iBody, jBody,iIndexJ,iOrder);
  if (iOrder == 1) {
    if (iIndexJ == 2) {
       yc += (fdDdistDlPrmInt13Cos(body, system, iBody, jBody, iIndexJ)\
           + fdDdistDlPrmInt111Cos(body, system, iBody, jBody, iIndexJ))\
            *body[iBody].dSemi*body[iBody].dSemi/(body[jBody].dSemi*body[jBody].dSemi);
       ys += (fdDdistDlPrmInt13Sin(body, system, iBody, jBody, iIndexJ)\
           + fdDdistDlPrmInt111Sin(body, system, iBody, jBody, iIndexJ))\
            *body[iBody].dSemi*body[iBody].dSemi/(body[jBody].dSemi*body[jBody].dSemi);
    } else if (iIndexJ == 3) {
       yc += (fdDdistDlPrmInt16Cos(body, system, iBody, jBody, iIndexJ))\
            *body[iBody].dSemi*body[iBody].dSemi/(body[jBody].dSemi*body[jBody].dSemi);
       ys += (fdDdistDlPrmInt16Sin(body, system, iBody, jBody, iIndexJ))\
            *body[iBody].dSemi*body[iBody].dSemi/(body[jBody].dSemi*body[jBody].dSemi);
    }
  } else if (iOrder == 2) {
    if (iIndexJ == 3) {
       yc += (fdDdistDlPrmInt25Cos(body, system, iBody, jBody, iIndexJ))\
            *body[iBody].dSemi*body[iBody].dSemi/(body[jBody].dSemi*body[jBody].dSemi);
       ys += (fdDdistDlPrmInt25Sin(body, system, iBody, jBody, iIndexJ))\
            *body[iBody].dSemi*body[iBody].dSemi/(body[jBody].dSemi*body[jBody].dSemi);
    }
  }

  y = (yc*fdCosLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)\
       + ys*fdSinLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)) * dMfac/dSemiPrm;
  return y;
}

//---------dR/da------------------------------------------------------------------
double fdDdistDaDir01(BODY *body, SYSTEM *system, int iBody, int jBody) {
  return (system->daLaplaceD[0][system->iaLaplaceN[iBody][jBody]][0] + \
          system->daLaplaceD[0][system->iaLaplaceN[iBody][jBody]][1] * \
          (body[iBody].dKecc*body[iBody].dKecc+body[jBody].dKecc*body[jBody].dKecc+\
          body[iBody].dHecc*body[iBody].dHecc+body[jBody].dHecc*body[jBody].dHecc) +\
          system->daLaplaceD[0][system->iaLaplaceN[iBody][jBody]][2] * \
          (body[iBody].dPinc*body[iBody].dPinc+body[jBody].dPinc*body[jBody].dPinc+\
          body[iBody].dQinc*body[iBody].dQinc+body[jBody].dQinc*body[jBody].dQinc))\
          /body[jBody].dSemi*AUCM;
}

double fdDdistDaDir02(BODY *body, SYSTEM *system, int iBody, int jBody) {
  return (system->daLaplaceD[0][system->iaLaplaceN[iBody][jBody]][9] * \
          (body[iBody].dKecc*body[jBody].dKecc+body[iBody].dHecc*body[jBody].dHecc))\
          /body[jBody].dSemi*AUCM;
}

double fdDdistDaDir03(BODY *body, SYSTEM *system, int iBody, int jBody) {
  return (system->daLaplaceD[0][system->iaLaplaceN[iBody][jBody]][13] * \
          (body[iBody].dPinc*body[jBody].dPinc+body[iBody].dQinc*body[jBody].dQinc))\
          /body[jBody].dSemi*AUCM;
}

double fdD2distDa2Dir01(BODY *body, SYSTEM *system, int iBody, int jBody) {
  //second deriv wrt alpha, not a
  return (fdDSemiF1D2alpha(body[iBody].dSemi/body[jBody].dSemi,0) + \
          fdDSemiF2D2alpha(body[iBody].dSemi/body[jBody].dSemi,0) * \
          (body[iBody].dKecc*body[iBody].dKecc+body[jBody].dKecc*body[jBody].dKecc+\
          body[iBody].dHecc*body[iBody].dHecc+body[jBody].dHecc*body[jBody].dHecc) +\
          fdDSemiF3D2alpha(body[iBody].dSemi/body[jBody].dSemi,0) * \
          (body[iBody].dPinc*body[iBody].dPinc+body[jBody].dPinc*body[jBody].dPinc+\
          body[iBody].dQinc*body[iBody].dQinc+body[jBody].dQinc*body[jBody].dQinc));
}

double fdD2distDa2Dir02(BODY *body, SYSTEM *system, int iBody, int jBody) {
//second deriv wrt alpha, not a
  return (fdDSemiF10D2alpha(body[iBody].dSemi/body[jBody].dSemi,0) * \
          (body[iBody].dKecc*body[jBody].dKecc+body[iBody].dHecc*body[jBody].dHecc));
}

double fdD2distDa2Dir03(BODY *body, SYSTEM *system, int iBody, int jBody) {
//second deriv wrt alpha, not a
  return (fdDSemiF14D2alpha(body[iBody].dSemi/body[jBody].dSemi,0) * \
          (body[iBody].dPinc*body[jBody].dPinc+body[iBody].dQinc*body[jBody].dQinc));
}

double fdDdistDaDir11Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return AUCM/body[jBody].dSemi*(system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][26]\
        +(body[iBody].dHecc*body[iBody].dHecc+body[iBody].dKecc*body[iBody].dKecc)*\
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][27]\
        +(body[jBody].dHecc*body[jBody].dHecc+body[jBody].dKecc*body[jBody].dKecc)*\
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][28]\
        +(body[iBody].dPinc*body[iBody].dPinc+body[iBody].dQinc*body[iBody].dQinc+
        body[jBody].dPinc*body[jBody].dPinc+body[jBody].dQinc*body[jBody].dQinc)*\
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][29])*(body[iBody].dKecc);
}

double fdDdistDaDir11Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return AUCM/body[jBody].dSemi*(system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][26]\
        +(body[iBody].dHecc*body[iBody].dHecc+body[iBody].dKecc*body[iBody].dKecc)*\
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][27]\
        +(body[jBody].dHecc*body[jBody].dHecc+body[jBody].dKecc*body[jBody].dKecc)*\
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][28]\
        +(body[iBody].dPinc*body[iBody].dPinc+body[iBody].dQinc*body[iBody].dQinc+
        body[jBody].dPinc*body[jBody].dPinc+body[jBody].dQinc*body[jBody].dQinc)*\
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][29])*(body[iBody].dHecc);
}

double fdDdistDaDir12Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return AUCM/body[jBody].dSemi*(system->daLaplaceD[iIndexJ][system->iaLaplaceN[jBody][iBody]][30] \
        +(body[iBody].dHecc*body[iBody].dHecc+body[iBody].dKecc*body[iBody].dKecc)*\
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][31]\
        +(body[jBody].dHecc*body[jBody].dHecc+body[jBody].dKecc*body[jBody].dKecc)*\
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][32]\
        +(body[iBody].dPinc*body[iBody].dPinc+body[iBody].dQinc*body[iBody].dQinc+
        body[jBody].dPinc*body[jBody].dPinc+body[jBody].dQinc*body[jBody].dQinc)*\
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][33])*(body[jBody].dKecc);
}

double fdDdistDaDir12Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return AUCM/body[jBody].dSemi*(system->daLaplaceD[iIndexJ][system->iaLaplaceN[jBody][iBody]][30] \
        +(body[iBody].dHecc*body[iBody].dHecc+body[iBody].dKecc*body[iBody].dKecc)*\
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][31]\
        +(body[jBody].dHecc*body[jBody].dHecc+body[jBody].dKecc*body[jBody].dKecc)*\
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][32]\
        +(body[iBody].dPinc*body[iBody].dPinc+body[iBody].dQinc*body[iBody].dQinc+
        body[jBody].dPinc*body[jBody].dPinc+body[jBody].dQinc*body[jBody].dQinc)*\
        system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][33])*(body[jBody].dHecc);
}

double fdDdistDaDir13Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return AUCM/body[jBody].dSemi*system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][34]*\
    ((body[jBody].dKecc*(body[iBody].dKecc*body[iBody].dKecc-body[iBody].dHecc*body[iBody].dHecc)\
    +2*body[jBody].dHecc*body[iBody].dHecc*body[iBody].dKecc));
}

double fdDdistDaDir13Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return AUCM/body[jBody].dSemi*system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][34]*\
    (-(body[jBody].dHecc*\
    (body[iBody].dKecc*body[iBody].dKecc-body[iBody].dHecc*body[iBody].dHecc)\
    -2*body[jBody].dKecc*body[iBody].dHecc*body[iBody].dKecc));
}

double fdDdistDaDir14Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return AUCM/body[jBody].dSemi*system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][35]*\
    ((body[iBody].dKecc*(body[jBody].dKecc*body[jBody].dKecc-body[jBody].dHecc*body[jBody].dHecc)\
    +2*body[iBody].dHecc*body[jBody].dHecc*body[jBody].dKecc));
}

double fdDdistDaDir14Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return AUCM/body[jBody].dSemi*system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][35]*\
    (-(body[iBody].dHecc*\
    (body[jBody].dKecc*body[jBody].dKecc-body[jBody].dHecc*body[jBody].dHecc)\
    -2*body[iBody].dKecc*body[jBody].dHecc*body[jBody].dKecc));
}

double fdDdistDaDir15Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return AUCM/body[jBody].dSemi*system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][36]*\
    ((body[iBody].dKecc*(body[iBody].dQinc*body[iBody].dQinc-body[iBody].dPinc*body[iBody].dPinc)\
    +2*body[iBody].dHecc*body[iBody].dPinc*body[iBody].dQinc));
}

double fdDdistDaDir15Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return AUCM/body[jBody].dSemi*system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][36]*\
    (-(body[iBody].dHecc*\
    (body[iBody].dQinc*body[iBody].dQinc-body[iBody].dPinc*body[iBody].dPinc)\
    -2*body[iBody].dKecc*body[iBody].dPinc*body[iBody].dQinc));
}

double fdDdistDaDir16Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return AUCM/body[jBody].dSemi*system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][37]*\
    ((body[jBody].dKecc*(body[iBody].dQinc*body[iBody].dQinc-body[iBody].dPinc*body[iBody].dPinc)\
    +2*body[jBody].dHecc*body[iBody].dPinc*body[iBody].dQinc));
}

double fdDdistDaDir16Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return AUCM/body[jBody].dSemi*system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][37]*\
    (-(body[jBody].dHecc*\
    (body[iBody].dQinc*body[iBody].dQinc-body[iBody].dPinc*body[iBody].dPinc)\
    -2*body[jBody].dKecc*body[iBody].dPinc*body[iBody].dQinc));
}

double fdDdistDaDir17Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return AUCM/body[jBody].dSemi*system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][38]*\
    ((body[iBody].dKecc*(body[iBody].dQinc*body[jBody].dQinc+body[iBody].dPinc*body[jBody].dPinc)\
    -body[iBody].dHecc*(body[iBody].dQinc*body[jBody].dPinc-body[iBody].dPinc*body[jBody].dQinc)));
}

double fdDdistDaDir17Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return AUCM/body[jBody].dSemi*system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][38]*\
    ((body[iBody].dHecc*\
    (body[iBody].dQinc*body[jBody].dQinc+body[iBody].dPinc*body[jBody].dPinc)\
    +body[iBody].dKecc*(body[iBody].dQinc*body[jBody].dPinc-body[iBody].dPinc*body[jBody].dQinc)));
}

double fdDdistDaDir18Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return AUCM/body[jBody].dSemi*system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][39]*\
    ((body[iBody].dKecc*(body[iBody].dQinc*body[jBody].dQinc+body[iBody].dPinc*body[jBody].dPinc)\
    -body[iBody].dHecc*(body[jBody].dQinc*body[iBody].dPinc-body[jBody].dPinc*body[iBody].dQinc)));
}

double fdDdistDaDir18Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return AUCM/body[jBody].dSemi*system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][39]*\
    ((body[iBody].dHecc*\
    (body[iBody].dQinc*body[jBody].dQinc+body[iBody].dPinc*body[jBody].dPinc)\
    +body[iBody].dKecc*(body[jBody].dQinc*body[iBody].dPinc-body[jBody].dPinc*body[iBody].dQinc)));
}

double fdDdistDaDir19Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return AUCM/body[jBody].dSemi*system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][40]*\
    ((body[iBody].dQinc*(body[jBody].dQinc*body[iBody].dKecc+body[jBody].dPinc*body[iBody].dHecc)\
    -body[iBody].dPinc*(body[jBody].dPinc*body[iBody].dKecc-body[jBody].dQinc*body[iBody].dHecc)));
}

double fdDdistDaDir19Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return AUCM/body[jBody].dSemi*system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][40]*\
    ((body[iBody].dPinc*\
    (body[jBody].dQinc*body[iBody].dKecc+body[jBody].dPinc*body[iBody].dHecc)\
    +body[iBody].dQinc*(body[jBody].dPinc*body[iBody].dKecc-body[jBody].dQinc*body[iBody].dHecc)));
}

double fdDdistDaDir110Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return AUCM/body[jBody].dSemi*system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][41]*\
    ((body[jBody].dKecc*(body[iBody].dQinc*body[jBody].dQinc+body[iBody].dPinc*body[jBody].dPinc)\
    -body[jBody].dHecc*(body[iBody].dQinc*body[jBody].dPinc-body[iBody].dPinc*body[jBody].dQinc)));
}

double fdDdistDaDir110Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return AUCM/body[jBody].dSemi*system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][41]*\
    ((body[jBody].dHecc*\
    (body[iBody].dQinc*body[jBody].dQinc+body[iBody].dPinc*body[jBody].dPinc)\
    +body[jBody].dKecc*(body[iBody].dQinc*body[jBody].dPinc-body[iBody].dPinc*body[jBody].dQinc)));
}

double fdDdistDaDir111Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return AUCM/body[jBody].dSemi*system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][42]*\
    ((body[jBody].dKecc*(body[iBody].dQinc*body[jBody].dQinc+body[iBody].dPinc*body[jBody].dPinc)\
    -body[jBody].dHecc*(body[jBody].dQinc*body[iBody].dPinc-body[jBody].dPinc*body[iBody].dQinc)));
}

double fdDdistDaDir111Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return AUCM/body[jBody].dSemi*system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][42]*\
    ((body[jBody].dHecc*\
    (body[iBody].dQinc*body[jBody].dQinc+body[iBody].dPinc*body[jBody].dPinc)\
    +body[jBody].dKecc*(body[jBody].dQinc*body[iBody].dPinc-body[jBody].dPinc*body[iBody].dQinc)));
}

double fdDdistDaDir112Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return AUCM/body[jBody].dSemi*system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][43]*\
    ((body[iBody].dQinc*(body[jBody].dQinc*body[jBody].dKecc+body[jBody].dPinc*body[jBody].dHecc)\
    -body[iBody].dPinc*(body[jBody].dPinc*body[jBody].dKecc-body[jBody].dQinc*body[jBody].dHecc)));
}

double fdDdistDaDir112Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return AUCM/body[jBody].dSemi*system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][43]*\
    ((body[iBody].dPinc*\
    (body[jBody].dQinc*body[jBody].dKecc+body[jBody].dPinc*body[jBody].dHecc)\
    +body[iBody].dQinc*(body[jBody].dPinc*body[jBody].dKecc-body[jBody].dQinc*body[jBody].dHecc)));
}

double fdDdistDaDir113Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return AUCM/body[jBody].dSemi*system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][36]*\
    ((body[iBody].dKecc*(body[jBody].dQinc*body[jBody].dQinc-body[jBody].dPinc*body[jBody].dPinc)
    +2*body[iBody].dHecc*body[jBody].dPinc*body[jBody].dQinc));
}

double fdDdistDaDir113Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return AUCM/body[jBody].dSemi*system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][36]*\
    (-(body[iBody].dHecc*\
    (body[jBody].dQinc*body[jBody].dQinc-body[jBody].dPinc*body[jBody].dPinc)\
    -2*body[iBody].dKecc*body[jBody].dPinc*body[jBody].dQinc));
}

double fdDdistDaDir114Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return AUCM/body[jBody].dSemi*system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][37]*\
    ((body[jBody].dKecc*(body[jBody].dQinc*body[jBody].dQinc-body[jBody].dPinc*body[jBody].dPinc)\
    +2*body[jBody].dHecc*body[jBody].dPinc*body[jBody].dQinc));
}

double fdDdistDaDir114Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return AUCM/body[jBody].dSemi*system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][37]*\
    (-(body[jBody].dHecc*\
    (body[jBody].dQinc*body[jBody].dQinc-body[jBody].dPinc*body[jBody].dPinc)\
    -2*body[jBody].dKecc*body[jBody].dPinc*body[jBody].dQinc));
}

double fdDdistDaExt13Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return AUCM/body[jBody].dSemi*(body[jBody].dKecc)*\
      (-2+(body[iBody].dHecc*body[iBody].dHecc+body[iBody].dKecc*body[iBody].dKecc)+\
      1.5*(body[jBody].dHecc*body[jBody].dHecc+body[jBody].dKecc*body[jBody].dKecc)+\
      2.*(body[iBody].dPinc*body[iBody].dPinc+body[iBody].dQinc*body[iBody].dQinc+\
      body[jBody].dPinc*body[jBody].dPinc+body[jBody].dQinc*body[jBody].dQinc));
}

double fdDdistDaExt13Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return AUCM/body[jBody].dSemi*(body[jBody].dHecc)*\
      (-2+(body[iBody].dHecc*body[iBody].dHecc+body[iBody].dKecc*body[iBody].dKecc)+\
      1.5*(body[jBody].dHecc*body[jBody].dHecc+body[jBody].dKecc*body[jBody].dKecc)+\
      2.*(body[iBody].dPinc*body[iBody].dPinc+body[iBody].dQinc*body[iBody].dQinc+\
      body[jBody].dPinc*body[jBody].dPinc+body[jBody].dQinc*body[jBody].dQinc));
}

double fdDdistDaExt16Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return -27./16*AUCM/body[jBody].dSemi*\
    ((body[iBody].dKecc*(body[jBody].dKecc*body[jBody].dKecc-body[jBody].dHecc*body[jBody].dHecc)\
    +2*body[iBody].dHecc*body[jBody].dHecc*body[jBody].dKecc));
}

double fdDdistDaExt16Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return -27./16*AUCM/body[jBody].dSemi*\
    (-(body[iBody].dHecc*\
    (body[jBody].dKecc*body[jBody].dKecc-body[jBody].dHecc*body[jBody].dHecc)\
    -2*body[iBody].dKecc*body[jBody].dHecc*body[jBody].dKecc));
}

double fdDdistDaExt111Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return -4*AUCM/body[jBody].dSemi*\
    ((body[iBody].dQinc*(body[jBody].dQinc*body[jBody].dKecc-body[jBody].dPinc*body[jBody].dHecc)\
    +body[iBody].dPinc*(body[jBody].dPinc*body[jBody].dKecc+body[jBody].dQinc*body[jBody].dHecc)));
}

double fdDdistDaExt111Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return -4*AUCM/body[jBody].dSemi*\
    ((body[iBody].dPinc*\
    (-body[jBody].dQinc*body[jBody].dKecc+body[jBody].dPinc*body[jBody].dHecc)\
    +body[iBody].dQinc*(body[jBody].dPinc*body[jBody].dKecc+body[jBody].dQinc*body[jBody].dHecc)));
}

double fdDdistDaDir21Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return AUCM/body[jBody].dSemi*system->daLaplaceD[iIndexJ][system->iaLaplaceN[jBody][iBody]][44]\
          *((body[iBody].dKecc*body[iBody].dKecc-body[iBody].dHecc*body[iBody].dHecc));
}

double fdDdistDaDir21Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return AUCM/body[jBody].dSemi*system->daLaplaceD[iIndexJ][system->iaLaplaceN[jBody][iBody]][44]\
          *(2*(body[iBody].dHecc*body[iBody].dKecc));
}

double fdDdistDaDir22Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return AUCM/body[jBody].dSemi*system->daLaplaceD[iIndexJ][system->iaLaplaceN[jBody][iBody]][48]\
          *((body[iBody].dKecc*body[jBody].dKecc-body[iBody].dHecc*body[jBody].dHecc));
}

double fdDdistDaDir22Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return AUCM/body[jBody].dSemi*system->daLaplaceD[iIndexJ][system->iaLaplaceN[jBody][iBody]][48]\
          *((body[iBody].dHecc*body[jBody].dKecc+body[iBody].dKecc*body[jBody].dHecc));
}

double fdDdistDaDir23Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return AUCM/body[jBody].dSemi*system->daLaplaceD[iIndexJ][system->iaLaplaceN[jBody][iBody]][52]\
          *((body[jBody].dKecc*body[jBody].dKecc-body[jBody].dHecc*body[jBody].dHecc));
}

double fdDdistDaDir23Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return AUCM/body[jBody].dSemi*system->daLaplaceD[iIndexJ][system->iaLaplaceN[jBody][iBody]][52]\
          *(2*(body[jBody].dHecc*body[jBody].dKecc));
}

double fdDdistDaDir24Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return AUCM/body[jBody].dSemi*system->daLaplaceD[iIndexJ][system->iaLaplaceN[jBody][iBody]][56]\
          *((body[iBody].dQinc*body[iBody].dQinc-body[iBody].dPinc*body[iBody].dPinc));
}

double fdDdistDaDir24Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return AUCM/body[jBody].dSemi*system->daLaplaceD[iIndexJ][system->iaLaplaceN[jBody][iBody]][56]\
          *(2*(body[iBody].dPinc*body[iBody].dQinc));
}

double fdDdistDaDir25Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return AUCM/body[jBody].dSemi*system->daLaplaceD[iIndexJ][system->iaLaplaceN[jBody][iBody]][61]\
          *((body[iBody].dQinc*body[jBody].dQinc-body[iBody].dPinc*body[jBody].dPinc));
}

double fdDdistDaDir25Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return AUCM/body[jBody].dSemi*system->daLaplaceD[iIndexJ][system->iaLaplaceN[jBody][iBody]][61]\
          *((body[jBody].dPinc*body[iBody].dQinc+body[iBody].dPinc*body[jBody].dQinc));
}

double fdDdistDaDir26Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return AUCM/body[jBody].dSemi*system->daLaplaceD[iIndexJ][system->iaLaplaceN[jBody][iBody]][56]\
          *((body[jBody].dQinc*body[jBody].dQinc-body[jBody].dPinc*body[jBody].dPinc));
}

double fdDdistDaDir26Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return AUCM/body[jBody].dSemi*system->daLaplaceD[iIndexJ][system->iaLaplaceN[jBody][iBody]][56]\
          *(2*(body[jBody].dPinc*body[jBody].dQinc));
}

double fdDdistDaExt25Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return -27./8*AUCM/body[jBody].dSemi*\
    ((body[jBody].dKecc*body[jBody].dKecc-body[jBody].dHecc*body[jBody].dHecc));
}

double fdDdistDaExt25Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return -27./8*AUCM/body[jBody].dSemi*\
    (2*body[jBody].dHecc*body[jBody].dKecc);
}

double fdDdistDaDir31Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return AUCM/body[jBody].dSemi*system->daLaplaceD[iIndexJ][system->iaLaplaceN[jBody][iBody]][81]\
          *((body[iBody].dKecc*body[iBody].dKecc*body[iBody].dKecc\
          -3*body[iBody].dHecc*body[iBody].dHecc*body[iBody].dKecc));
}

double fdDdistDaDir31Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return AUCM/body[jBody].dSemi*system->daLaplaceD[iIndexJ][system->iaLaplaceN[jBody][iBody]][81]\
          *((-body[iBody].dHecc*body[iBody].dHecc*body[iBody].dHecc\
          +3*body[iBody].dKecc*body[iBody].dKecc*body[iBody].dHecc));
}

double fdDdistDaDir32Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return AUCM/body[jBody].dSemi*system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][82]\
          *((body[jBody].dKecc*(body[iBody].dKecc*body[iBody].dKecc-\
          body[iBody].dHecc*body[iBody].dHecc)\
          -2*body[jBody].dHecc*body[iBody].dHecc*body[iBody].dKecc));
}

double fdDdistDaDir32Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return AUCM/body[jBody].dSemi*system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][82]\
          *((body[jBody].dHecc*(body[iBody].dKecc*body[iBody].dKecc-\
          body[iBody].dHecc*body[iBody].dHecc)\
          +2*body[jBody].dKecc*body[iBody].dKecc*body[iBody].dHecc));
}

double fdDdistDaDir33Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return AUCM/body[jBody].dSemi*system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][83]\
      *((body[iBody].dKecc*(body[jBody].dKecc*body[jBody].dKecc-body[jBody].dHecc*body[jBody].dHecc)\
          -2*body[iBody].dHecc*body[jBody].dHecc*body[jBody].dKecc));
}

double fdDdistDaDir33Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return AUCM/body[jBody].dSemi*system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][83]\
      *((body[iBody].dHecc*(body[jBody].dKecc*body[jBody].dKecc-body[jBody].dHecc*body[jBody].dHecc)\
          +2*body[iBody].dKecc*body[jBody].dKecc*body[jBody].dHecc));
}

double fdDdistDaDir34Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return AUCM/body[jBody].dSemi*system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][84]\
          *((body[jBody].dKecc*body[jBody].dKecc*body[jBody].dKecc\
          -3*body[jBody].dHecc*body[jBody].dHecc*body[jBody].dKecc));
}

double fdDdistDaDir34Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return AUCM/body[jBody].dSemi*system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][84]\
          *((-body[jBody].dHecc*body[jBody].dHecc*body[jBody].dHecc\
          +3*body[jBody].dKecc*body[jBody].dKecc*body[jBody].dHecc));
}

double fdDdistDaDir35Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return AUCM/body[jBody].dSemi*system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][85]\
      *((body[iBody].dKecc*(body[iBody].dQinc*body[iBody].dQinc-body[iBody].dPinc*body[iBody].dPinc)\
          -2*body[iBody].dHecc*body[iBody].dPinc*body[iBody].dQinc));
}

double fdDdistDaDir35Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return AUCM/body[jBody].dSemi*system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][85]\
      *((body[iBody].dHecc*(body[iBody].dQinc*body[iBody].dQinc-body[iBody].dPinc*body[iBody].dPinc)\
          +2*body[iBody].dKecc*body[iBody].dPinc*body[iBody].dQinc));
}

double fdDdistDaDir36Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return AUCM/body[jBody].dSemi*system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][86]\
      *((body[jBody].dKecc*(body[iBody].dQinc*body[iBody].dQinc-body[iBody].dPinc*body[iBody].dPinc)\
          -2*body[jBody].dHecc*body[iBody].dPinc*body[iBody].dQinc));
}

double fdDdistDaDir36Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return AUCM/body[jBody].dSemi*system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][86]\
      *((body[jBody].dHecc*(body[iBody].dQinc*body[iBody].dQinc-body[iBody].dPinc*body[iBody].dPinc)\
          +2*body[jBody].dKecc*body[iBody].dPinc*body[iBody].dQinc));
}

double fdDdistDaDir37Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return AUCM/body[jBody].dSemi*system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][87]\
      *((body[iBody].dKecc*(body[jBody].dQinc*body[iBody].dQinc-body[jBody].dPinc*body[iBody].dPinc)\
        -body[iBody].dHecc*(body[jBody].dPinc*body[iBody].dQinc+body[jBody].dQinc*body[iBody].dPinc)));
}

double fdDdistDaDir37Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return AUCM/body[jBody].dSemi*system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][87]\
      *((body[iBody].dHecc*(body[jBody].dQinc*body[iBody].dQinc-body[jBody].dPinc*body[iBody].dPinc)\
      +body[iBody].dKecc*(body[jBody].dPinc*body[iBody].dQinc+body[jBody].dQinc*body[iBody].dPinc)));
}

double fdDdistDaDir38Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return AUCM/body[jBody].dSemi*system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][88]\
    *((body[jBody].dKecc*(body[jBody].dQinc*body[iBody].dQinc-body[jBody].dPinc*body[iBody].dPinc)\
    -body[jBody].dHecc*(body[jBody].dPinc*body[iBody].dQinc+body[jBody].dQinc*body[iBody].dPinc)));
}

double fdDdistDaDir38Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return AUCM/body[jBody].dSemi*system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][88]\
      *((body[jBody].dHecc*(body[jBody].dQinc*body[iBody].dQinc-body[jBody].dPinc*body[iBody].dPinc)\
      +body[jBody].dKecc*(body[jBody].dPinc*body[iBody].dQinc+body[jBody].dQinc*body[iBody].dPinc)));
}

double fdDdistDaDir39Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return AUCM/body[jBody].dSemi*system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][85]\
      *((body[iBody].dKecc*(body[jBody].dQinc*body[jBody].dQinc-body[jBody].dPinc*body[jBody].dPinc)\
          -2*body[iBody].dHecc*body[jBody].dPinc*body[jBody].dQinc));
}

double fdDdistDaDir39Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return AUCM/body[jBody].dSemi*system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][85]\
      *((body[iBody].dHecc*(body[jBody].dQinc*body[jBody].dQinc-body[jBody].dPinc*body[jBody].dPinc)\
          +2*body[iBody].dKecc*body[jBody].dPinc*body[jBody].dQinc));
}

double fdDdistDaDir310Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return AUCM/body[jBody].dSemi*system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][86]\
      *((body[jBody].dKecc*(body[jBody].dQinc*body[jBody].dQinc-body[jBody].dPinc*body[jBody].dPinc)\
          -2*body[jBody].dHecc*body[jBody].dPinc*body[jBody].dQinc));
}

double fdDdistDaDir310Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return AUCM/body[jBody].dSemi*system->daLaplaceD[iIndexJ][system->iaLaplaceN[iBody][jBody]][86]\
      *((body[jBody].dHecc*(body[jBody].dQinc*body[jBody].dQinc-body[jBody].dPinc*body[jBody].dPinc)\
          +2*body[jBody].dKecc*body[jBody].dPinc*body[jBody].dQinc));
}

double fdDdistresDSemi0th(BODY *body, SYSTEM *system, int iBody, int jBody) {
  double y, dMfac, dSemiPrm;
  dMfac = KGAUSS*KGAUSS*body[jBody].dMass/MSUN;
  dSemiPrm = body[jBody].dSemi/AUCM;
  y = ( fdDdistDaDir01(body, system, iBody, jBody)\
      + fdDdistDaDir02(body, system, iBody, jBody)\
      + fdDdistDaDir03(body, system, iBody, jBody) ) * dMfac/dSemiPrm;
  return y;
}

double fdDdistresDSemiDirCos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ, int iOrder) {
  double y;
  if (iOrder == 1) {
    y = ( fdDdistDaDir11Cos(body, system, iBody, jBody, iIndexJ)\
      + fdDdistDaDir12Cos(body, system, iBody, jBody, iIndexJ)\
      + fdDdistDaDir13Cos(body, system, iBody, jBody, iIndexJ)\
      + fdDdistDaDir14Cos(body, system, iBody, jBody, iIndexJ)\
      + fdDdistDaDir15Cos(body, system, iBody, jBody, iIndexJ)\
      + fdDdistDaDir16Cos(body, system, iBody, jBody, iIndexJ)\
      + fdDdistDaDir17Cos(body, system, iBody, jBody, iIndexJ)\
      + fdDdistDaDir18Cos(body, system, iBody, jBody, iIndexJ)\
      + fdDdistDaDir19Cos(body, system, iBody, jBody, iIndexJ)\
      + fdDdistDaDir110Cos(body, system, iBody, jBody, iIndexJ)\
      + fdDdistDaDir111Cos(body, system, iBody, jBody, iIndexJ)\
      + fdDdistDaDir112Cos(body, system, iBody, jBody, iIndexJ)\
      + fdDdistDaDir113Cos(body, system, iBody, jBody, iIndexJ)\
      + fdDdistDaDir114Cos(body, system, iBody, jBody, iIndexJ) );
  } else if (iOrder == 2) {
    y = ( fdDdistDaDir21Cos(body, system, iBody, jBody, iIndexJ) \
    + fdDdistDaDir22Cos(body, system, iBody, jBody, iIndexJ)
    + fdDdistDaDir23Cos(body, system, iBody, jBody, iIndexJ) \
    + fdDdistDaDir24Cos(body, system, iBody, jBody, iIndexJ)
    + fdDdistDaDir25Cos(body, system, iBody, jBody, iIndexJ) \
    + fdDdistDaDir26Cos(body, system, iBody, jBody, iIndexJ) );
  } else if (iOrder == 3) {
    y = ( fdDdistDaDir31Cos(body, system, iBody, jBody, iIndexJ) \
    + fdDdistDaDir32Cos(body, system, iBody, jBody, iIndexJ) \
    + fdDdistDaDir33Cos(body, system, iBody, jBody, iIndexJ) \
    + fdDdistDaDir34Cos(body, system, iBody, jBody, iIndexJ) \
    + fdDdistDaDir35Cos(body, system, iBody, jBody, iIndexJ) \
    + fdDdistDaDir36Cos(body, system, iBody, jBody, iIndexJ) \
    + fdDdistDaDir37Cos(body, system, iBody, jBody, iIndexJ) \
    + fdDdistDaDir38Cos(body, system, iBody, jBody, iIndexJ) \
    + fdDdistDaDir39Cos(body, system, iBody, jBody, iIndexJ) \
    + fdDdistDaDir310Cos(body, system, iBody, jBody, iIndexJ) );
  }

  return y;
}

double fdDdistresDSemiDirSin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ, int iOrder) {
  double y;
  if (iOrder == 1) {
    y = ( fdDdistDaDir11Sin(body, system, iBody, jBody, iIndexJ)\
      + fdDdistDaDir12Sin(body, system, iBody, jBody, iIndexJ)\
      + fdDdistDaDir13Sin(body, system, iBody, jBody, iIndexJ)\
      + fdDdistDaDir14Sin(body, system, iBody, jBody, iIndexJ)\
      + fdDdistDaDir15Sin(body, system, iBody, jBody, iIndexJ)\
      + fdDdistDaDir16Sin(body, system, iBody, jBody, iIndexJ)\
      + fdDdistDaDir17Sin(body, system, iBody, jBody, iIndexJ)\
      + fdDdistDaDir18Sin(body, system, iBody, jBody, iIndexJ)\
      + fdDdistDaDir19Sin(body, system, iBody, jBody, iIndexJ)\
      + fdDdistDaDir110Sin(body, system, iBody, jBody, iIndexJ)\
      + fdDdistDaDir111Sin(body, system, iBody, jBody, iIndexJ)\
      + fdDdistDaDir112Sin(body, system, iBody, jBody, iIndexJ)\
      + fdDdistDaDir113Sin(body, system, iBody, jBody, iIndexJ)\
      + fdDdistDaDir114Sin(body, system, iBody, jBody, iIndexJ) );
  } else if (iOrder == 2) {
    y = ( fdDdistDaDir21Sin(body, system, iBody, jBody, iIndexJ) \
    + fdDdistDaDir22Sin(body, system, iBody, jBody, iIndexJ)
    + fdDdistDaDir23Sin(body, system, iBody, jBody, iIndexJ) \
    + fdDdistDaDir24Sin(body, system, iBody, jBody, iIndexJ)
    + fdDdistDaDir25Sin(body, system, iBody, jBody, iIndexJ) \
    + fdDdistDaDir26Sin(body, system, iBody, jBody, iIndexJ) );
  } else if (iOrder == 3) {
    y = ( fdDdistDaDir31Sin(body, system, iBody, jBody, iIndexJ) \
    + fdDdistDaDir32Sin(body, system, iBody, jBody, iIndexJ) \
    + fdDdistDaDir33Sin(body, system, iBody, jBody, iIndexJ) \
    + fdDdistDaDir34Sin(body, system, iBody, jBody, iIndexJ) \
    + fdDdistDaDir35Sin(body, system, iBody, jBody, iIndexJ) \
    + fdDdistDaDir36Sin(body, system, iBody, jBody, iIndexJ) \
    + fdDdistDaDir37Sin(body, system, iBody, jBody, iIndexJ) \
    + fdDdistDaDir38Sin(body, system, iBody, jBody, iIndexJ) \
    + fdDdistDaDir39Sin(body, system, iBody, jBody, iIndexJ) \
    + fdDdistDaDir310Sin(body, system, iBody, jBody, iIndexJ) );
  }

  return y;
}

double fdDdistresDSemi(BODY *body, SYSTEM *system, int iBody,int jBody, int iIndexJ, int iOrder, int iRes) {
  double y, yc, ys, dMfac, dSemiPrm;
  dMfac = KGAUSS*KGAUSS*body[jBody].dMass/MSUN;
  dSemiPrm = body[jBody].dSemi/AUCM;

  yc = fdDdistresDSemiDirCos(body,system,iBody, jBody,iIndexJ,iOrder);
  ys = fdDdistresDSemiDirSin(body,system,iBody, jBody,iIndexJ,iOrder);

  if (iOrder == 1) {
    if (iIndexJ == 2) {
      yc += (fdDdistDaExt13Cos(body,system,iBody,jBody,iIndexJ)\
           +fdDdistDaExt111Cos(body,system,iBody,jBody,iIndexJ) );
      ys += (fdDdistDaExt13Sin(body,system,iBody,jBody,iIndexJ)\
           +fdDdistDaExt111Sin(body,system,iBody,jBody,iIndexJ) );
    } else if (iIndexJ == 3) {
      yc += (fdDdistDaExt16Cos(body,system,iBody,jBody,iIndexJ) );
      ys += (fdDdistDaExt16Sin(body,system,iBody,jBody,iIndexJ) );
    }
  } else if (iOrder == 2) {
    if (iIndexJ == 3) {
      yc += (fdDdistDaExt25Cos(body,system,iBody,jBody,iIndexJ) );
      ys += (fdDdistDaExt25Sin(body,system,iBody,jBody,iIndexJ) );
    }
  }

  y = (yc*fdCosLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes) \
       + ys*fdSinLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)) * dMfac/dSemiPrm;
  return y;
}

//---------dR/da'------------------------------------------------------------------
double fdDistDir01(BODY *body, SYSTEM *system, int iBody, int jBody) {
  return  (system->daLaplaceC[0][system->iaLaplaceN[iBody][jBody]][0] + \
          system->daLaplaceC[0][system->iaLaplaceN[iBody][jBody]][1] * \
          (body[iBody].dKecc*body[iBody].dKecc+body[jBody].dKecc*body[jBody].dKecc+\
          body[iBody].dHecc*body[iBody].dHecc+body[jBody].dHecc*body[jBody].dHecc) +\
          system->daLaplaceC[0][system->iaLaplaceN[iBody][jBody]][2] * \
          (body[iBody].dPinc*body[iBody].dPinc+body[jBody].dPinc*body[jBody].dPinc+\
          body[iBody].dQinc*body[iBody].dQinc+body[jBody].dQinc*body[jBody].dQinc));
}

double fdDistDir02(BODY *body, SYSTEM *system, int iBody, int jBody) {
  return (system->daLaplaceC[0][system->iaLaplaceN[iBody][jBody]][9] * \
          (body[iBody].dKecc*body[jBody].dKecc+body[iBody].dHecc*body[jBody].dHecc));
}

double fdDistDir03(BODY *body, SYSTEM *system, int iBody, int jBody) {
  return (system->daLaplaceC[0][system->iaLaplaceN[iBody][jBody]][13] * \
          (body[iBody].dPinc*body[jBody].dPinc+body[iBody].dQinc*body[jBody].dQinc));
}

double fdDdistDaPrmDir01(BODY *body, SYSTEM *system, int iBody, int jBody) {
  return -(body[jBody].dSemi/body[iBody].dSemi)*fdDdistDaDir01(body,system,jBody,iBody)-\
          1./body[iBody].dSemi*fdDistDir01(body,system,jBody,iBody)*AUCM;
}

double fdDdistDaPrmDir03(BODY *body, SYSTEM *system, int iBody, int jBody) {
  return -(body[jBody].dSemi/body[iBody].dSemi)*fdDdistDaDir03(body,system,jBody,iBody)-\
          1./body[iBody].dSemi*fdDistDir03(body,system,jBody,iBody)*AUCM;
}

double fdDdistDaPrmDir02(BODY *body, SYSTEM *system, int iBody, int jBody) {
  return -(body[jBody].dSemi/body[iBody].dSemi)*fdDdistDaDir02(body,system,jBody,iBody)-\
          1./body[iBody].dSemi*fdDistDir02(body,system,jBody,iBody)*AUCM;
}

double fdDistDir11Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return (system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][26]\
        +(body[iBody].dHecc*body[iBody].dHecc+body[iBody].dKecc*body[iBody].dKecc)*\
        system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][27]\
        +(body[jBody].dHecc*body[jBody].dHecc+body[jBody].dKecc*body[jBody].dKecc)*\
        system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][28]\
        +(body[iBody].dPinc*body[iBody].dPinc+body[iBody].dQinc*body[iBody].dQinc+
        body[jBody].dPinc*body[jBody].dPinc+body[jBody].dQinc*body[jBody].dQinc)*\
        system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][29])*(body[iBody].dKecc);
}

double fdDistDir11Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return (system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][26]\
        +(body[iBody].dHecc*body[iBody].dHecc+body[iBody].dKecc*body[iBody].dKecc)*\
        system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][27]\
        +(body[jBody].dHecc*body[jBody].dHecc+body[jBody].dKecc*body[jBody].dKecc)*\
        system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][28]\
        +(body[iBody].dPinc*body[iBody].dPinc+body[iBody].dQinc*body[iBody].dQinc+
        body[jBody].dPinc*body[jBody].dPinc+body[jBody].dQinc*body[jBody].dQinc)*\
        system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][29])*(body[iBody].dHecc);
}

double fdDistDir12Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return (system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][30]
        +(body[iBody].dHecc*body[iBody].dHecc+body[iBody].dKecc*body[iBody].dKecc)*\
        system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][31]\
        +(body[jBody].dHecc*body[jBody].dHecc+body[jBody].dKecc*body[jBody].dKecc)*\
        system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][32]\
        +(body[iBody].dPinc*body[iBody].dPinc+body[iBody].dQinc*body[iBody].dQinc+
        body[jBody].dPinc*body[jBody].dPinc+body[jBody].dQinc*body[jBody].dQinc)*\
        system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][33])*(body[jBody].dKecc);
}

double fdDistDir12Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return (system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][30]
        +(body[iBody].dHecc*body[iBody].dHecc+body[iBody].dKecc*body[iBody].dKecc)*\
        system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][31]\
        +(body[jBody].dHecc*body[jBody].dHecc+body[jBody].dKecc*body[jBody].dKecc)*\
        system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][32]\
        +(body[iBody].dPinc*body[iBody].dPinc+body[iBody].dQinc*body[iBody].dQinc+
        body[jBody].dPinc*body[jBody].dPinc+body[jBody].dQinc*body[jBody].dQinc)*\
        system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][33])*(body[jBody].dHecc);
}

double fdDistDir13Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][34]*\
    ((body[jBody].dKecc*(body[iBody].dKecc*body[iBody].dKecc-body[iBody].dHecc*body[iBody].dHecc)\
    +2*body[jBody].dHecc*body[iBody].dHecc*body[iBody].dKecc));
}

double fdDistDir13Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][34]*\
    (-(body[jBody].dHecc*\
    (body[iBody].dKecc*body[iBody].dKecc-body[iBody].dHecc*body[iBody].dHecc)\
    -2*body[jBody].dKecc*body[iBody].dHecc*body[iBody].dKecc));
}

double fdDistDir14Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][35]*\
    ((body[iBody].dKecc*(body[jBody].dKecc*body[jBody].dKecc-body[jBody].dHecc*body[jBody].dHecc)\
    +2*body[iBody].dHecc*body[jBody].dHecc*body[jBody].dKecc));
}

double fdDistDir14Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][35]*\
    (-(body[iBody].dHecc*\
    (body[jBody].dKecc*body[jBody].dKecc-body[jBody].dHecc*body[jBody].dHecc)\
    -2*body[iBody].dKecc*body[jBody].dHecc*body[jBody].dKecc));
}

double fdDistDir15Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][36]*\
    ((body[iBody].dKecc*(body[iBody].dQinc*body[iBody].dQinc-body[iBody].dPinc*body[iBody].dPinc)\
    +2*body[iBody].dHecc*body[iBody].dPinc*body[iBody].dQinc));
}

double fdDistDir15Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][36]*\
    (-(body[iBody].dHecc*\
    (body[iBody].dQinc*body[iBody].dQinc-body[iBody].dPinc*body[iBody].dPinc)\
    -2*body[iBody].dKecc*body[iBody].dPinc*body[iBody].dQinc));
}

double fdDistDir16Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][37]*\
    ((body[jBody].dKecc*(body[iBody].dQinc*body[iBody].dQinc-body[iBody].dPinc*body[iBody].dPinc)\
    +2*body[jBody].dHecc*body[iBody].dPinc*body[iBody].dQinc));
}

double fdDistDir16Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][37]*\
    (-(body[jBody].dHecc*\
    (body[iBody].dQinc*body[iBody].dQinc-body[iBody].dPinc*body[iBody].dPinc)\
    -2*body[jBody].dKecc*body[iBody].dPinc*body[iBody].dQinc));
}

double fdDistDir17Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][38]*\
    ((body[iBody].dKecc*(body[iBody].dQinc*body[jBody].dQinc+body[iBody].dPinc*body[jBody].dPinc)\
    -body[iBody].dHecc*(body[iBody].dQinc*body[jBody].dPinc-body[iBody].dPinc*body[jBody].dQinc)));
}

double fdDistDir17Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][38]*\
    ((body[iBody].dHecc*\
    (body[iBody].dQinc*body[jBody].dQinc+body[iBody].dPinc*body[jBody].dPinc)\
    +body[iBody].dKecc*(body[iBody].dQinc*body[jBody].dPinc-body[iBody].dPinc*body[jBody].dQinc)));
}

double fdDistDir18Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][39]*\
    ((body[iBody].dKecc*(body[iBody].dQinc*body[jBody].dQinc+body[iBody].dPinc*body[jBody].dPinc)\
    -body[iBody].dHecc*(body[jBody].dQinc*body[iBody].dPinc-body[jBody].dPinc*body[iBody].dQinc)));
}

double fdDistDir18Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][39]*\
    ((body[iBody].dHecc*\
    (body[iBody].dQinc*body[jBody].dQinc+body[iBody].dPinc*body[jBody].dPinc)\
    +body[iBody].dKecc*(body[jBody].dQinc*body[iBody].dPinc-body[jBody].dPinc*body[iBody].dQinc)));
}

double fdDistDir19Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][40]*\
    ((body[iBody].dQinc*(body[jBody].dQinc*body[iBody].dKecc+body[jBody].dPinc*body[iBody].dHecc)\
    -body[iBody].dPinc*(body[jBody].dPinc*body[iBody].dKecc-body[jBody].dQinc*body[iBody].dHecc)));
}

double fdDistDir19Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][40]*\
    ((body[iBody].dPinc*\
    (body[jBody].dQinc*body[iBody].dKecc+body[jBody].dPinc*body[iBody].dHecc)\
    +body[iBody].dQinc*(body[jBody].dPinc*body[iBody].dKecc-body[jBody].dQinc*body[iBody].dHecc)));
}

double fdDistDir110Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][41]*\
    ((body[jBody].dKecc*(body[iBody].dQinc*body[jBody].dQinc+body[iBody].dPinc*body[jBody].dPinc)\
    -body[jBody].dHecc*(body[iBody].dQinc*body[jBody].dPinc-body[iBody].dPinc*body[jBody].dQinc)));
}

double fdDistDir110Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][41]*\
    ((body[jBody].dHecc*\
    (body[iBody].dQinc*body[jBody].dQinc+body[iBody].dPinc*body[jBody].dPinc)\
    +body[jBody].dKecc*(body[iBody].dQinc*body[jBody].dPinc-body[iBody].dPinc*body[jBody].dQinc)));
}

double fdDistDir111Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][42]*\
    ((body[jBody].dKecc*(body[iBody].dQinc*body[jBody].dQinc+body[iBody].dPinc*body[jBody].dPinc)\
    -body[jBody].dHecc*(body[jBody].dQinc*body[iBody].dPinc-body[jBody].dPinc*body[iBody].dQinc)));
}

double fdDistDir111Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][42]*\
    ((body[jBody].dHecc*\
    (body[iBody].dQinc*body[jBody].dQinc+body[iBody].dPinc*body[jBody].dPinc)\
    +body[jBody].dKecc*(body[jBody].dQinc*body[iBody].dPinc-body[jBody].dPinc*body[iBody].dQinc)));
}

double fdDistDir112Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][43]*\
    ((body[iBody].dQinc*(body[jBody].dQinc*body[jBody].dKecc+body[jBody].dPinc*body[jBody].dHecc)\
    -body[iBody].dPinc*(body[jBody].dPinc*body[jBody].dKecc-body[jBody].dQinc*body[jBody].dHecc)));
}

double fdDistDir112Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][43]*\
    ((body[iBody].dPinc*\
    (body[jBody].dQinc*body[jBody].dKecc+body[jBody].dPinc*body[jBody].dHecc)\
    +body[iBody].dQinc*(body[jBody].dPinc*body[jBody].dKecc-body[jBody].dQinc*body[jBody].dHecc)));
}

double fdDistDir113Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][36]*\
    ((body[iBody].dKecc*(body[jBody].dQinc*body[jBody].dQinc-body[jBody].dPinc*body[jBody].dPinc)\
    +2*body[iBody].dHecc*body[jBody].dPinc*body[jBody].dQinc));
}

double fdDistDir113Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][36]*\
    (-(body[iBody].dHecc*\
    (body[jBody].dQinc*body[jBody].dQinc-body[jBody].dPinc*body[jBody].dPinc)\
    -2*body[iBody].dKecc*body[jBody].dPinc*body[jBody].dQinc));
}

double fdDistDir114Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][37]*\
    ((body[jBody].dKecc*(body[jBody].dQinc*body[jBody].dQinc-body[jBody].dPinc*body[jBody].dPinc)\
    +2*body[jBody].dHecc*body[jBody].dPinc*body[jBody].dQinc));
}

double fdDistDir114Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][37]*\
    (-(body[jBody].dHecc*\
    (body[jBody].dQinc*body[jBody].dQinc-body[jBody].dPinc*body[jBody].dPinc)\
    -2*body[jBody].dKecc*body[jBody].dPinc*body[jBody].dQinc));
}

double fdDdistDaPrmDir11Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return -body[jBody].dSemi/body[iBody].dSemi*fdDdistDaDir11Cos(body,system,jBody,iBody,iIndexJ)\
         -1./body[iBody].dSemi*fdDistDir11Cos(body,system,jBody,iBody,iIndexJ)*AUCM;
}

double fdDdistDaPrmDir11Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return -body[jBody].dSemi/body[iBody].dSemi*fdDdistDaDir11Sin(body,system,jBody,iBody,iIndexJ)\
         -1./body[iBody].dSemi*fdDistDir11Sin(body,system,jBody,iBody,iIndexJ)*AUCM;
}

double fdDdistDaPrmDir12Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return -body[jBody].dSemi/body[iBody].dSemi*fdDdistDaDir12Cos(body,system,jBody,iBody,iIndexJ)\
         -1./body[iBody].dSemi*fdDistDir12Cos(body,system,jBody,iBody,iIndexJ)*AUCM;
}

double fdDdistDaPrmDir12Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return -body[jBody].dSemi/body[iBody].dSemi*fdDdistDaDir12Sin(body,system,jBody,iBody,iIndexJ)\
         -1./body[iBody].dSemi*fdDistDir12Sin(body,system,jBody,iBody,iIndexJ)*AUCM;
}

double fdDdistDaPrmDir13Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return -body[jBody].dSemi/body[iBody].dSemi*fdDdistDaDir13Cos(body,system,jBody,iBody,iIndexJ)\
         -1./body[iBody].dSemi*fdDistDir13Cos(body,system,jBody,iBody,iIndexJ)*AUCM;
}

double fdDdistDaPrmDir13Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return -body[jBody].dSemi/body[iBody].dSemi*fdDdistDaDir13Sin(body,system,jBody,iBody,iIndexJ)\
         -1./body[iBody].dSemi*fdDistDir13Sin(body,system,jBody,iBody,iIndexJ)*AUCM;
}

double fdDdistDaPrmDir14Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return -body[jBody].dSemi/body[iBody].dSemi*fdDdistDaDir14Cos(body,system,jBody,iBody,iIndexJ)\
         -1./body[iBody].dSemi*fdDistDir14Cos(body,system,jBody,iBody,iIndexJ)*AUCM;
}

double fdDdistDaPrmDir14Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return -body[jBody].dSemi/body[iBody].dSemi*fdDdistDaDir14Sin(body,system,jBody,iBody,iIndexJ)\
         -1./body[iBody].dSemi*fdDistDir14Sin(body,system,jBody,iBody,iIndexJ)*AUCM;
}

double fdDdistDaPrmDir15Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return -body[jBody].dSemi/body[iBody].dSemi*fdDdistDaDir15Cos(body,system,jBody,iBody,iIndexJ)\
         -1./body[iBody].dSemi*fdDistDir15Cos(body,system,jBody,iBody,iIndexJ)*AUCM;
}

double fdDdistDaPrmDir15Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return -body[jBody].dSemi/body[iBody].dSemi*fdDdistDaDir15Sin(body,system,jBody,iBody,iIndexJ)\
         -1./body[iBody].dSemi*fdDistDir15Sin(body,system,jBody,iBody,iIndexJ)*AUCM;
}

double fdDdistDaPrmDir16Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return -body[jBody].dSemi/body[iBody].dSemi*fdDdistDaDir16Cos(body,system,jBody,iBody,iIndexJ)\
         -1./body[iBody].dSemi*fdDistDir16Cos(body,system,jBody,iBody,iIndexJ)*AUCM;
}

double fdDdistDaPrmDir16Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return -body[jBody].dSemi/body[iBody].dSemi*fdDdistDaDir16Sin(body,system,jBody,iBody,iIndexJ)\
         -1./body[iBody].dSemi*fdDistDir16Sin(body,system,jBody,iBody,iIndexJ)*AUCM;
}

double fdDdistDaPrmDir17Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return -body[jBody].dSemi/body[iBody].dSemi*fdDdistDaDir17Cos(body,system,jBody,iBody,iIndexJ)\
         -1./body[iBody].dSemi*fdDistDir17Cos(body,system,jBody,iBody,iIndexJ)*AUCM;
}

double fdDdistDaPrmDir17Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return -body[jBody].dSemi/body[iBody].dSemi*fdDdistDaDir17Sin(body,system,jBody,iBody,iIndexJ)\
         -1./body[iBody].dSemi*fdDistDir17Sin(body,system,jBody,iBody,iIndexJ)*AUCM;
}

double fdDdistDaPrmDir18Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return -body[jBody].dSemi/body[iBody].dSemi*fdDdistDaDir18Cos(body,system,jBody,iBody,iIndexJ)\
         -1./body[iBody].dSemi*fdDistDir18Cos(body,system,jBody,iBody,iIndexJ)*AUCM;
}

double fdDdistDaPrmDir18Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return -body[jBody].dSemi/body[iBody].dSemi*fdDdistDaDir18Sin(body,system,jBody,iBody,iIndexJ)\
         -1./body[iBody].dSemi*fdDistDir18Sin(body,system,jBody,iBody,iIndexJ)*AUCM;
}

double fdDdistDaPrmDir19Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return -body[jBody].dSemi/body[iBody].dSemi*fdDdistDaDir19Cos(body,system,jBody,iBody,iIndexJ)\
         -1./body[iBody].dSemi*fdDistDir19Cos(body,system,jBody,iBody,iIndexJ)*AUCM;
}

double fdDdistDaPrmDir19Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return -body[jBody].dSemi/body[iBody].dSemi*fdDdistDaDir19Sin(body,system,jBody,iBody,iIndexJ)\
         -1./body[iBody].dSemi*fdDistDir19Sin(body,system,jBody,iBody,iIndexJ)*AUCM;
}

double fdDdistDaPrmDir110Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return -body[jBody].dSemi/body[iBody].dSemi*fdDdistDaDir110Cos(body,system,jBody,iBody,iIndexJ)\
         -1./body[iBody].dSemi*fdDistDir110Cos(body,system,jBody,iBody,iIndexJ)*AUCM;
}

double fdDdistDaPrmDir110Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return -body[jBody].dSemi/body[iBody].dSemi*fdDdistDaDir110Sin(body,system,jBody,iBody,iIndexJ)\
         -1./body[iBody].dSemi*fdDistDir110Sin(body,system,jBody,iBody,iIndexJ)*AUCM;
}

double fdDdistDaPrmDir111Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return -body[jBody].dSemi/body[iBody].dSemi*fdDdistDaDir111Cos(body,system,jBody,iBody,iIndexJ)\
         -1./body[iBody].dSemi*fdDistDir111Cos(body,system,jBody,iBody,iIndexJ)*AUCM;
}

double fdDdistDaPrmDir111Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return -body[jBody].dSemi/body[iBody].dSemi*fdDdistDaDir111Sin(body,system,jBody,iBody,iIndexJ)\
         -1./body[iBody].dSemi*fdDistDir111Sin(body,system,jBody,iBody,iIndexJ)*AUCM;
}

double fdDdistDaPrmDir112Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return -body[jBody].dSemi/body[iBody].dSemi*fdDdistDaDir112Cos(body,system,jBody,iBody,iIndexJ)\
         -1./body[iBody].dSemi*fdDistDir112Cos(body,system,jBody,iBody,iIndexJ)*AUCM;
}

double fdDdistDaPrmDir112Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return -body[jBody].dSemi/body[iBody].dSemi*fdDdistDaDir112Sin(body,system,jBody,iBody,iIndexJ)\
         -1./body[iBody].dSemi*fdDistDir112Sin(body,system,jBody,iBody,iIndexJ)*AUCM;
}

double fdDdistDaPrmDir113Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return -body[jBody].dSemi/body[iBody].dSemi*fdDdistDaDir113Cos(body,system,jBody,iBody,iIndexJ)\
         -1./body[iBody].dSemi*fdDistDir113Cos(body,system,jBody,iBody,iIndexJ)*AUCM;
}

double fdDdistDaPrmDir113Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return -body[jBody].dSemi/body[iBody].dSemi*fdDdistDaDir113Sin(body,system,jBody,iBody,iIndexJ)\
         -1./body[iBody].dSemi*fdDistDir113Sin(body,system,jBody,iBody,iIndexJ)*AUCM;
}

double fdDdistDaPrmDir114Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return -body[jBody].dSemi/body[iBody].dSemi*fdDdistDaDir114Cos(body,system,jBody,iBody,iIndexJ)\
         -1./body[iBody].dSemi*fdDistDir114Cos(body,system,jBody,iBody,iIndexJ)*AUCM;
}

double fdDdistDaPrmDir114Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return -body[jBody].dSemi/body[iBody].dSemi*fdDdistDaDir114Sin(body,system,jBody,iBody,iIndexJ)\
         -1./body[iBody].dSemi*fdDistDir114Sin(body,system,jBody,iBody,iIndexJ)*AUCM;
}

double fdDdistDaPrmInt13Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return 1./2*body[iBody].dSemi*body[iBody].dSemi/(body[jBody].dSemi*body[jBody].dSemi)*\
        (fdDdistDaExt13Cos(body,system,jBody,iBody,iIndexJ));
}

double fdDdistDaPrmInt13Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return 1./2*body[iBody].dSemi*body[iBody].dSemi/(body[jBody].dSemi*body[jBody].dSemi)*\
        (fdDdistDaExt13Sin(body,system,jBody,iBody,iIndexJ));
}

double fdDdistDaPrmInt16Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return 8./9*body[iBody].dSemi*body[iBody].dSemi/(body[jBody].dSemi*body[jBody].dSemi)*\
        (fdDdistDaExt16Cos(body,system,jBody,iBody,iIndexJ));
}

double fdDdistDaPrmInt16Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return 8./9*body[iBody].dSemi*body[iBody].dSemi/(body[jBody].dSemi*body[jBody].dSemi)*\
        (fdDdistDaExt16Sin(body,system,jBody,iBody,iIndexJ));
}

double fdDdistDaPrmInt111Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return 1./2*body[iBody].dSemi*body[iBody].dSemi/(body[jBody].dSemi*body[jBody].dSemi)*\
        (fdDdistDaExt111Cos(body,system,jBody,iBody,iIndexJ));
}

double fdDdistDaPrmInt111Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return 1./2*body[iBody].dSemi*body[iBody].dSemi/(body[jBody].dSemi*body[jBody].dSemi)*\
        (fdDdistDaExt111Sin(body,system,jBody,iBody,iIndexJ));
}

double fdDistDir21Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  //jBody is the outer body
  return system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][44]*\
         ( (body[iBody].dKecc*body[iBody].dKecc-body[iBody].dHecc*body[iBody].dHecc) );
}

double fdDistDir21Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  //jBody is the outer body
  return system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][44]*\
         ( (2*body[iBody].dHecc*body[iBody].dKecc) );
}

double fdDistDir22Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][48]*\
         ( (body[jBody].dKecc*body[iBody].dKecc-body[jBody].dHecc*body[iBody].dHecc) );
}

double fdDistDir22Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][48]*\
         ( (body[iBody].dHecc*body[jBody].dKecc+body[iBody].dKecc*body[jBody].dHecc) );
}

double fdDistDir23Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  //jBody is the outer body
  return system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][52]*\
         ( (body[jBody].dKecc*body[jBody].dKecc-body[jBody].dHecc*body[jBody].dHecc) );
}

double fdDistDir23Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  //jBody is the outer body
  return system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][52]*\
         ( (2*body[jBody].dHecc*body[jBody].dKecc) );
}

double fdDistDir24Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  //jBody is the outer body
  return system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][56]*\
         ( (body[iBody].dQinc*body[iBody].dQinc-body[iBody].dPinc*body[iBody].dPinc) );
}

double fdDistDir24Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  //jBody is the outer body
  return system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][56]*\
         ( (2*body[iBody].dPinc*body[iBody].dQinc) );
}

double fdDistDir25Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][61]*\
         ( (body[jBody].dQinc*body[iBody].dQinc-body[jBody].dPinc*body[iBody].dPinc) );
}

double fdDistDir25Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][61]*\
         ( (body[iBody].dPinc*body[jBody].dQinc+body[iBody].dQinc*body[jBody].dPinc) );
}

double fdDistDir26Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  //jBody is the outer body
  return system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][56]*\
         ( (body[jBody].dQinc*body[jBody].dQinc-body[jBody].dPinc*body[jBody].dPinc) );
}

double fdDistDir26Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  //jBody is the outer body
  return system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][56]*\
         ( (2*body[jBody].dPinc*body[jBody].dQinc) );
}

double fdDdistDaPrmDir21Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return -body[jBody].dSemi/body[iBody].dSemi*fdDdistDaDir21Cos(body,system,jBody,iBody,iIndexJ)\
         -1./body[iBody].dSemi*fdDistDir21Cos(body,system,jBody,iBody,iIndexJ)*AUCM;
}

double fdDdistDaPrmDir21Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return -body[jBody].dSemi/body[iBody].dSemi*fdDdistDaDir21Sin(body,system,jBody,iBody,iIndexJ)\
         -1./body[iBody].dSemi*fdDistDir21Sin(body,system,jBody,iBody,iIndexJ)*AUCM;
}

double fdDdistDaPrmDir22Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return -body[jBody].dSemi/body[iBody].dSemi*fdDdistDaDir22Cos(body,system,jBody,iBody,iIndexJ)\
         -1./body[iBody].dSemi*fdDistDir22Cos(body,system,jBody,iBody,iIndexJ)*AUCM;
}

double fdDdistDaPrmDir22Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return -body[jBody].dSemi/body[iBody].dSemi*fdDdistDaDir22Sin(body,system,jBody,iBody,iIndexJ)\
         -1./body[iBody].dSemi*fdDistDir22Sin(body,system,jBody,iBody,iIndexJ)*AUCM;
}

double fdDdistDaPrmDir23Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return -body[jBody].dSemi/body[iBody].dSemi*fdDdistDaDir23Cos(body,system,jBody,iBody,iIndexJ)\
         -1./body[iBody].dSemi*fdDistDir23Cos(body,system,jBody,iBody,iIndexJ)*AUCM;
}

double fdDdistDaPrmDir23Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return -body[jBody].dSemi/body[iBody].dSemi*fdDdistDaDir23Sin(body,system,jBody,iBody,iIndexJ)\
         -1./body[iBody].dSemi*fdDistDir23Sin(body,system,jBody,iBody,iIndexJ)*AUCM;
}

double fdDdistDaPrmDir24Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return -body[jBody].dSemi/body[iBody].dSemi*fdDdistDaDir24Cos(body,system,jBody,iBody,iIndexJ)\
         -1./body[iBody].dSemi*fdDistDir24Cos(body,system,jBody,iBody,iIndexJ)*AUCM;
}

double fdDdistDaPrmDir24Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return -body[jBody].dSemi/body[iBody].dSemi*fdDdistDaDir24Sin(body,system,jBody,iBody,iIndexJ)\
         -1./body[iBody].dSemi*fdDistDir24Sin(body,system,jBody,iBody,iIndexJ)*AUCM;
}

double fdDdistDaPrmDir25Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return -body[jBody].dSemi/body[iBody].dSemi*fdDdistDaDir25Cos(body,system,jBody,iBody,iIndexJ)\
         -1./body[iBody].dSemi*fdDistDir25Cos(body,system,jBody,iBody,iIndexJ)*AUCM;
}

double fdDdistDaPrmDir25Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return -body[jBody].dSemi/body[iBody].dSemi*fdDdistDaDir25Sin(body,system,jBody,iBody,iIndexJ)\
         -1./body[iBody].dSemi*fdDistDir25Sin(body,system,jBody,iBody,iIndexJ)*AUCM;
}

double fdDdistDaPrmDir26Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return -body[jBody].dSemi/body[iBody].dSemi*fdDdistDaDir26Cos(body,system,jBody,iBody,iIndexJ)\
         -1./body[iBody].dSemi*fdDistDir26Cos(body,system,jBody,iBody,iIndexJ)*AUCM;
}

double fdDdistDaPrmDir26Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return -body[jBody].dSemi/body[iBody].dSemi*fdDdistDaDir26Sin(body,system,jBody,iBody,iIndexJ)\
         -1./body[iBody].dSemi*fdDistDir26Sin(body,system,jBody,iBody,iIndexJ)*AUCM;
}

double fdDdistDaPrmInt25Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return 2./9*body[iBody].dSemi*body[iBody].dSemi/(body[jBody].dSemi*body[jBody].dSemi)*\
        (fdDdistDaExt25Cos(body,system,jBody,iBody,iIndexJ));
}

double fdDdistDaPrmInt25Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return 2./9*body[iBody].dSemi*body[iBody].dSemi/(body[jBody].dSemi*body[jBody].dSemi)*\
        (fdDdistDaExt25Sin(body,system,jBody,iBody,iIndexJ));
}

double fdDistDir31Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  //jBody is the outer body
  return system->daLaplaceC[iIndexJ][system->iaLaplaceN[jBody][iBody]][81]\
          *((body[iBody].dKecc*body[iBody].dKecc*body[iBody].dKecc\
          -3*body[iBody].dHecc*body[iBody].dHecc*body[iBody].dKecc));
}

double fdDistDir31Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  //jBody is the outer body
  return system->daLaplaceC[iIndexJ][system->iaLaplaceN[jBody][iBody]][81]\
          *((-body[iBody].dHecc*body[iBody].dHecc*body[iBody].dHecc\
          +3*body[iBody].dKecc*body[iBody].dKecc*body[iBody].dHecc));
}

double fdDistDir32Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][82]\
      *((body[jBody].dKecc*(body[iBody].dKecc*body[iBody].dKecc-body[iBody].dHecc*body[iBody].dHecc)\
          -2*body[jBody].dHecc*body[iBody].dHecc*body[iBody].dKecc));
}

double fdDistDir32Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][82]\
      *((body[jBody].dHecc*(body[iBody].dKecc*body[iBody].dKecc-body[iBody].dHecc*body[iBody].dHecc)\
          +2*body[jBody].dKecc*body[iBody].dKecc*body[iBody].dHecc));
}

double fdDistDir33Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][83]\
      *((body[iBody].dKecc*(body[jBody].dKecc*body[jBody].dKecc-body[jBody].dHecc*body[jBody].dHecc)\
          -2*body[iBody].dHecc*body[jBody].dHecc*body[jBody].dKecc));
}

double fdDistDir33Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][83]\
      *((body[iBody].dHecc*(body[jBody].dKecc*body[jBody].dKecc-body[jBody].dHecc*body[jBody].dHecc)\
          +2*body[iBody].dKecc*body[jBody].dKecc*body[jBody].dHecc));
}

double fdDistDir34Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][84]\
          *((body[jBody].dKecc*body[jBody].dKecc*body[jBody].dKecc\
          -3*body[jBody].dHecc*body[jBody].dHecc*body[jBody].dKecc));
}

double fdDistDir34Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][84]\
          *((-body[jBody].dHecc*body[jBody].dHecc*body[jBody].dHecc\
          +3*body[jBody].dKecc*body[jBody].dKecc*body[jBody].dHecc));
}

double fdDistDir35Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][85]\
      *((body[iBody].dKecc*(body[iBody].dQinc*body[iBody].dQinc-body[iBody].dPinc*body[iBody].dPinc)\
          -2*body[iBody].dHecc*body[iBody].dPinc*body[iBody].dQinc));
}

double fdDistDir35Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][85]\
      *((body[iBody].dHecc*(body[iBody].dQinc*body[iBody].dQinc-body[iBody].dPinc*body[iBody].dPinc)\
          +2*body[iBody].dKecc*body[iBody].dPinc*body[iBody].dQinc));
}

double fdDistDir36Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][86]\
      *((body[jBody].dKecc*(body[iBody].dQinc*body[iBody].dQinc-body[iBody].dPinc*body[iBody].dPinc)\
          -2*body[jBody].dHecc*body[iBody].dPinc*body[iBody].dQinc));
}

double fdDistDir36Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][86]\
      *((body[jBody].dHecc*(body[iBody].dQinc*body[iBody].dQinc-body[iBody].dPinc*body[iBody].dPinc)\
          +2*body[jBody].dKecc*body[iBody].dPinc*body[iBody].dQinc));
}

double fdDistDir37Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][87]\
      *((body[iBody].dKecc*(body[jBody].dQinc*body[iBody].dQinc-body[jBody].dPinc*body[iBody].dPinc)\
        -body[iBody].dHecc*(body[jBody].dPinc*body[iBody].dQinc+body[jBody].dQinc*body[iBody].dPinc)));
}

double fdDistDir37Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][87]\
      *((body[iBody].dHecc*(body[jBody].dQinc*body[iBody].dQinc-body[jBody].dPinc*body[iBody].dPinc)\
        +body[iBody].dKecc*(body[jBody].dPinc*body[iBody].dQinc+body[jBody].dQinc*body[iBody].dPinc)));
}

double fdDistDir38Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][88]\
      *((body[jBody].dKecc*(body[jBody].dQinc*body[iBody].dQinc-body[jBody].dPinc*body[iBody].dPinc)\
       -body[jBody].dHecc*(body[jBody].dPinc*body[iBody].dQinc+body[jBody].dQinc*body[iBody].dPinc)));
}

double fdDistDir38Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][88]\
      *((body[jBody].dHecc*(body[jBody].dQinc*body[iBody].dQinc-body[jBody].dPinc*body[iBody].dPinc)\
       +body[jBody].dKecc*(body[jBody].dPinc*body[iBody].dQinc+body[jBody].dQinc*body[iBody].dPinc)));
}

double fdDistDir39Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][85]\
      *((body[iBody].dKecc*(body[jBody].dQinc*body[jBody].dQinc-body[jBody].dPinc*body[jBody].dPinc)\
          -2*body[iBody].dHecc*body[jBody].dPinc*body[jBody].dQinc));
}

double fdDistDir39Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][85]\
      *((body[iBody].dHecc*(body[jBody].dQinc*body[jBody].dQinc-body[jBody].dPinc*body[jBody].dPinc)\
          +2*body[iBody].dKecc*body[jBody].dPinc*body[jBody].dQinc));
}

double fdDistDir310Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][86]\
      *((body[jBody].dKecc*(body[jBody].dQinc*body[jBody].dQinc-body[jBody].dPinc*body[jBody].dPinc)\
          -2*body[jBody].dHecc*body[jBody].dPinc*body[jBody].dQinc));
}

double fdDistDir310Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return system->daLaplaceC[iIndexJ][system->iaLaplaceN[iBody][jBody]][86]\
      *((body[jBody].dHecc*(body[jBody].dQinc*body[jBody].dQinc-body[jBody].dPinc*body[jBody].dPinc)\
          +2*body[jBody].dKecc*body[jBody].dPinc*body[jBody].dQinc));
}

double fdDdistDaPrmDir31Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return -body[jBody].dSemi/body[iBody].dSemi*fdDdistDaDir31Cos(body,system,jBody,iBody,iIndexJ)\
         -1./body[iBody].dSemi*fdDistDir31Cos(body,system,jBody,iBody,iIndexJ)*AUCM;
}

double fdDdistDaPrmDir31Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return -body[jBody].dSemi/body[iBody].dSemi*fdDdistDaDir31Sin(body,system,jBody,iBody,iIndexJ)\
         -1./body[iBody].dSemi*fdDistDir31Sin(body,system,jBody,iBody,iIndexJ)*AUCM;
}

double fdDdistDaPrmDir32Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return -body[jBody].dSemi/body[iBody].dSemi*fdDdistDaDir32Cos(body,system,jBody,iBody,iIndexJ)\
         -1./body[iBody].dSemi*fdDistDir32Cos(body,system,jBody,iBody,iIndexJ)*AUCM;
}

double fdDdistDaPrmDir32Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return -body[jBody].dSemi/body[iBody].dSemi*fdDdistDaDir32Sin(body,system,jBody,iBody,iIndexJ)\
         -1./body[iBody].dSemi*fdDistDir32Sin(body,system,jBody,iBody,iIndexJ)*AUCM;
}

double fdDdistDaPrmDir33Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return -body[jBody].dSemi/body[iBody].dSemi*fdDdistDaDir33Cos(body,system,jBody,iBody,iIndexJ)\
         -1./body[iBody].dSemi*fdDistDir33Cos(body,system,jBody,iBody,iIndexJ)*AUCM;
}

double fdDdistDaPrmDir33Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return -body[jBody].dSemi/body[iBody].dSemi*fdDdistDaDir33Sin(body,system,jBody,iBody,iIndexJ)\
         -1./body[iBody].dSemi*fdDistDir33Sin(body,system,jBody,iBody,iIndexJ)*AUCM;
}

double fdDdistDaPrmDir34Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return -body[jBody].dSemi/body[iBody].dSemi*fdDdistDaDir34Cos(body,system,jBody,iBody,iIndexJ)\
         -1./body[iBody].dSemi*fdDistDir34Cos(body,system,jBody,iBody,iIndexJ)*AUCM;
}

double fdDdistDaPrmDir34Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return -body[jBody].dSemi/body[iBody].dSemi*fdDdistDaDir34Sin(body,system,jBody,iBody,iIndexJ)\
         -1./body[iBody].dSemi*fdDistDir34Sin(body,system,jBody,iBody,iIndexJ)*AUCM;
}

double fdDdistDaPrmDir35Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return -body[jBody].dSemi/body[iBody].dSemi*fdDdistDaDir35Cos(body,system,jBody,iBody,iIndexJ)\
         -1./body[iBody].dSemi*fdDistDir35Cos(body,system,jBody,iBody,iIndexJ)*AUCM;
}

double fdDdistDaPrmDir35Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return -body[jBody].dSemi/body[iBody].dSemi*fdDdistDaDir35Sin(body,system,jBody,iBody,iIndexJ)\
         -1./body[iBody].dSemi*fdDistDir35Sin(body,system,jBody,iBody,iIndexJ)*AUCM;
}

double fdDdistDaPrmDir36Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return -body[jBody].dSemi/body[iBody].dSemi*fdDdistDaDir36Cos(body,system,jBody,iBody,iIndexJ)\
         -1./body[iBody].dSemi*fdDistDir36Cos(body,system,jBody,iBody,iIndexJ)*AUCM;
}

double fdDdistDaPrmDir36Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return -body[jBody].dSemi/body[iBody].dSemi*fdDdistDaDir36Sin(body,system,jBody,iBody,iIndexJ)\
         -1./body[iBody].dSemi*fdDistDir36Sin(body,system,jBody,iBody,iIndexJ)*AUCM;
}

double fdDdistDaPrmDir37Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return -body[jBody].dSemi/body[iBody].dSemi*fdDdistDaDir37Cos(body,system,jBody,iBody,iIndexJ)\
         -1./body[iBody].dSemi*fdDistDir37Cos(body,system,jBody,iBody,iIndexJ)*AUCM;
}

double fdDdistDaPrmDir37Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return -body[jBody].dSemi/body[iBody].dSemi*fdDdistDaDir37Sin(body,system,jBody,iBody,iIndexJ)\
         -1./body[iBody].dSemi*fdDistDir37Sin(body,system,jBody,iBody,iIndexJ)*AUCM;
}

double fdDdistDaPrmDir38Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return -body[jBody].dSemi/body[iBody].dSemi*fdDdistDaDir38Cos(body,system,jBody,iBody,iIndexJ)\
         -1./body[iBody].dSemi*fdDistDir38Cos(body,system,jBody,iBody,iIndexJ)*AUCM;
}

double fdDdistDaPrmDir38Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return -body[jBody].dSemi/body[iBody].dSemi*fdDdistDaDir38Sin(body,system,jBody,iBody,iIndexJ)\
         -1./body[iBody].dSemi*fdDistDir38Sin(body,system,jBody,iBody,iIndexJ)*AUCM;
}

double fdDdistDaPrmDir39Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return -body[jBody].dSemi/body[iBody].dSemi*fdDdistDaDir39Cos(body,system,jBody,iBody,iIndexJ)\
         -1./body[iBody].dSemi*fdDistDir39Cos(body,system,jBody,iBody,iIndexJ)*AUCM;
}

double fdDdistDaPrmDir39Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return -body[jBody].dSemi/body[iBody].dSemi*fdDdistDaDir39Sin(body,system,jBody,iBody,iIndexJ)\
         -1./body[iBody].dSemi*fdDistDir39Sin(body,system,jBody,iBody,iIndexJ)*AUCM;
}

double fdDdistDaPrmDir310Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return -body[jBody].dSemi/body[iBody].dSemi*fdDdistDaDir310Cos(body,system,jBody,iBody,iIndexJ)\
         -1./body[iBody].dSemi*fdDistDir310Cos(body,system,jBody,iBody,iIndexJ)*AUCM;
}

double fdDdistDaPrmDir310Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return -body[jBody].dSemi/body[iBody].dSemi*fdDdistDaDir310Sin(body,system,jBody,iBody,iIndexJ)\
         -1./body[iBody].dSemi*fdDistDir310Sin(body,system,jBody,iBody,iIndexJ)*AUCM;
}

double fdDdistresDSemiPrime0th(BODY *body, SYSTEM *system, int iBody, int jBody) {
  double y, dMfac, dSemiPrm;
  dMfac = KGAUSS*KGAUSS*body[jBody].dMass/MSUN;
  dSemiPrm = body[iBody].dSemi/AUCM;
  y = ( fdDdistDaPrmDir01(body, system, iBody, jBody) \
      + fdDdistDaPrmDir02(body, system, iBody, jBody) \
      + fdDdistDaPrmDir03(body, system, iBody, jBody) ) * dMfac/dSemiPrm;
  return y;
}

double fdDdistresDSemiPrimeDirCos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ, int iOrder) {
  double y;
  if (iOrder == 1) {
    y = ( fdDdistDaPrmDir11Cos(body, system, iBody, jBody, iIndexJ)\
      + fdDdistDaPrmDir12Cos(body, system, iBody, jBody, iIndexJ)\
      + fdDdistDaPrmDir13Cos(body, system, iBody, jBody, iIndexJ)\
      + fdDdistDaPrmDir14Cos(body, system, iBody, jBody, iIndexJ)\
      + fdDdistDaPrmDir15Cos(body, system, iBody, jBody, iIndexJ)\
      + fdDdistDaPrmDir16Cos(body, system, iBody, jBody, iIndexJ)\
      + fdDdistDaPrmDir17Cos(body, system, iBody, jBody, iIndexJ)\
      + fdDdistDaPrmDir18Cos(body, system, iBody, jBody, iIndexJ)\
      + fdDdistDaPrmDir19Cos(body, system, iBody, jBody, iIndexJ)\
      + fdDdistDaPrmDir110Cos(body, system, iBody, jBody, iIndexJ)\
      + fdDdistDaPrmDir111Cos(body, system, iBody, jBody, iIndexJ)\
      + fdDdistDaPrmDir112Cos(body, system, iBody, jBody, iIndexJ)\
      + fdDdistDaPrmDir113Cos(body, system, iBody, jBody, iIndexJ)\
      + fdDdistDaPrmDir114Cos(body, system, iBody, jBody, iIndexJ) );
  } else if (iOrder == 2) {
    y = ( fdDdistDaPrmDir21Cos(body, system, iBody, jBody, iIndexJ) \
    + fdDdistDaPrmDir22Cos(body, system, iBody, jBody, iIndexJ)
    + fdDdistDaPrmDir23Cos(body, system, iBody, jBody, iIndexJ) \
    + fdDdistDaPrmDir24Cos(body, system, iBody, jBody, iIndexJ)
    + fdDdistDaPrmDir25Cos(body, system, iBody, jBody, iIndexJ) \
    + fdDdistDaPrmDir26Cos(body, system, iBody, jBody, iIndexJ) );
  } else if (iOrder == 3) {
    y = ( fdDdistDaPrmDir31Cos(body, system, iBody, jBody, iIndexJ) \
    + fdDdistDaPrmDir32Cos(body, system, iBody, jBody, iIndexJ) \
    + fdDdistDaPrmDir33Cos(body, system, iBody, jBody, iIndexJ) \
    + fdDdistDaPrmDir34Cos(body, system, iBody, jBody, iIndexJ) \
    + fdDdistDaPrmDir35Cos(body, system, iBody, jBody, iIndexJ) \
    + fdDdistDaPrmDir36Cos(body, system, iBody, jBody, iIndexJ) \
    + fdDdistDaPrmDir37Cos(body, system, iBody, jBody, iIndexJ) \
    + fdDdistDaPrmDir38Cos(body, system, iBody, jBody, iIndexJ) \
    + fdDdistDaPrmDir39Cos(body, system, iBody, jBody, iIndexJ) \
    + fdDdistDaPrmDir310Cos(body, system, iBody, jBody, iIndexJ) );
  }
  return y;
}

double fdDdistresDSemiPrimeDirSin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ, int iOrder) {
  double y;
  if (iOrder == 1) {
    y = ( fdDdistDaPrmDir11Sin(body, system, iBody, jBody, iIndexJ)\
      + fdDdistDaPrmDir12Sin(body, system, iBody, jBody, iIndexJ)\
      + fdDdistDaPrmDir13Sin(body, system, iBody, jBody, iIndexJ)\
      + fdDdistDaPrmDir14Sin(body, system, iBody, jBody, iIndexJ)\
      + fdDdistDaPrmDir15Sin(body, system, iBody, jBody, iIndexJ)\
      + fdDdistDaPrmDir16Sin(body, system, iBody, jBody, iIndexJ)\
      + fdDdistDaPrmDir17Sin(body, system, iBody, jBody, iIndexJ)\
      + fdDdistDaPrmDir18Sin(body, system, iBody, jBody, iIndexJ)\
      + fdDdistDaPrmDir19Sin(body, system, iBody, jBody, iIndexJ)\
      + fdDdistDaPrmDir110Sin(body, system, iBody, jBody, iIndexJ)\
      + fdDdistDaPrmDir111Sin(body, system, iBody, jBody, iIndexJ)\
      + fdDdistDaPrmDir112Sin(body, system, iBody, jBody, iIndexJ)\
      + fdDdistDaPrmDir113Sin(body, system, iBody, jBody, iIndexJ)\
      + fdDdistDaPrmDir114Sin(body, system, iBody, jBody, iIndexJ) );
  } else if (iOrder == 2) {
    y = ( fdDdistDaPrmDir21Sin(body, system, iBody, jBody, iIndexJ) \
    + fdDdistDaPrmDir22Sin(body, system, iBody, jBody, iIndexJ)
    + fdDdistDaPrmDir23Sin(body, system, iBody, jBody, iIndexJ) \
    + fdDdistDaPrmDir24Sin(body, system, iBody, jBody, iIndexJ)
    + fdDdistDaPrmDir25Sin(body, system, iBody, jBody, iIndexJ) \
    + fdDdistDaPrmDir26Sin(body, system, iBody, jBody, iIndexJ) );
  } else if (iOrder == 3) {
    y = ( fdDdistDaPrmDir31Sin(body, system, iBody, jBody, iIndexJ) \
    + fdDdistDaPrmDir32Sin(body, system, iBody, jBody, iIndexJ) \
    + fdDdistDaPrmDir33Sin(body, system, iBody, jBody, iIndexJ) \
    + fdDdistDaPrmDir34Sin(body, system, iBody, jBody, iIndexJ) \
    + fdDdistDaPrmDir35Sin(body, system, iBody, jBody, iIndexJ) \
    + fdDdistDaPrmDir36Sin(body, system, iBody, jBody, iIndexJ) \
    + fdDdistDaPrmDir37Sin(body, system, iBody, jBody, iIndexJ) \
    + fdDdistDaPrmDir38Sin(body, system, iBody, jBody, iIndexJ) \
    + fdDdistDaPrmDir39Sin(body, system, iBody, jBody, iIndexJ) \
    + fdDdistDaPrmDir310Sin(body, system, iBody, jBody, iIndexJ) );
  }
  return y;
}

double fdDdistresDSemiPrime(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ, int iOrder, int iRes) {
  double y, yc, ys, dMfac, dSemiPrm;
  dMfac = KGAUSS*KGAUSS*body[jBody].dMass/MSUN;
  dSemiPrm = body[iBody].dSemi/AUCM;

  yc = fdDdistresDSemiPrimeDirCos(body,system,iBody, jBody,iIndexJ,iOrder);
  ys = fdDdistresDSemiPrimeDirSin(body,system,iBody, jBody,iIndexJ,iOrder);
  if (iOrder == 1) {
    if (iIndexJ == 2) {
      yc += (fdDdistDaPrmInt13Cos(body, system, iBody, jBody, iIndexJ)\
            +fdDdistDaPrmInt111Cos(body, system, iBody, jBody, iIndexJ));
      ys += (fdDdistDaPrmInt13Sin(body, system, iBody, jBody, iIndexJ)\
            +fdDdistDaPrmInt111Sin(body, system, iBody, jBody, iIndexJ));
    } else if (iIndexJ == 3) {
      yc += (fdDdistDaPrmInt16Cos(body, system, iBody, jBody, iIndexJ));
      ys += (fdDdistDaPrmInt16Sin(body, system, iBody, jBody, iIndexJ));
    }
  } else if (iOrder == 2) {
    if (iIndexJ == 3) {
      yc += (fdDdistDaPrmInt25Cos(body, system, iBody, jBody, iIndexJ));
      ys += (fdDdistDaPrmInt25Sin(body, system, iBody, jBody, iIndexJ));
    }
  }

  y = (yc*fdCosLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes) \
       + ys*fdSinLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)) * dMfac/dSemiPrm;
  return y;
}

//-----------Eqns for circulation approximations (Malhotra+ 1989) when bResAvg = 1--------

double fdDistfCos(BODY *body,SYSTEM *system,int iBody,int jBody,int iIndexJ,int iOrder) {
  double y;
  if (iOrder == 1) {
    y = ( fdDistDir11Cos(body, system, iBody, jBody, iIndexJ)\
      + fdDistDir12Cos(body, system, iBody, jBody, iIndexJ)\
      + fdDistDir13Cos(body, system, iBody, jBody, iIndexJ)\
      + fdDistDir14Cos(body, system, iBody, jBody, iIndexJ)\
      + fdDistDir15Cos(body, system, iBody, jBody, iIndexJ)\
      + fdDistDir16Cos(body, system, iBody, jBody, iIndexJ)\
      + fdDistDir17Cos(body, system, iBody, jBody, iIndexJ)\
      + fdDistDir18Cos(body, system, iBody, jBody, iIndexJ)\
      + fdDistDir19Cos(body, system, iBody, jBody, iIndexJ)\
      + fdDistDir110Cos(body, system, iBody, jBody, iIndexJ)\
      + fdDistDir111Cos(body, system, iBody, jBody, iIndexJ)\
      + fdDistDir112Cos(body, system, iBody, jBody, iIndexJ)\
      + fdDistDir113Cos(body, system, iBody, jBody, iIndexJ)\
      + fdDistDir114Cos(body, system, iBody, jBody, iIndexJ) );
  } else if (iOrder == 2) {
    y = ( fdDistDir21Cos(body, system, iBody, jBody, iIndexJ) \
    + fdDistDir22Cos(body, system, iBody, jBody, iIndexJ)
    + fdDistDir23Cos(body, system, iBody, jBody, iIndexJ) \
    + fdDistDir24Cos(body, system, iBody, jBody, iIndexJ)
    + fdDistDir25Cos(body, system, iBody, jBody, iIndexJ) \
    + fdDistDir26Cos(body, system, iBody, jBody, iIndexJ) );
  } else if (iOrder == 3) {
    y = ( fdDistDir31Cos(body, system, iBody, jBody, iIndexJ) \
    + fdDistDir32Cos(body, system, iBody, jBody, iIndexJ) \
    + fdDistDir33Cos(body, system, iBody, jBody, iIndexJ) \
    + fdDistDir34Cos(body, system, iBody, jBody, iIndexJ) \
    + fdDistDir35Cos(body, system, iBody, jBody, iIndexJ) \
    + fdDistDir36Cos(body, system, iBody, jBody, iIndexJ) \
    + fdDistDir37Cos(body, system, iBody, jBody, iIndexJ) \
    + fdDistDir38Cos(body, system, iBody, jBody, iIndexJ) \
    + fdDistDir39Cos(body, system, iBody, jBody, iIndexJ) \
    + fdDistDir310Cos(body, system, iBody, jBody, iIndexJ) );
  }
  return y;
}

double fdDistfSin(BODY *body,SYSTEM *system,int iBody,int jBody,int iIndexJ,int iOrder) {
  double y;
  if (iOrder == 1) {
    y = ( fdDistDir11Sin(body, system, iBody, jBody, iIndexJ)\
      + fdDistDir12Sin(body, system, iBody, jBody, iIndexJ)\
      + fdDistDir13Sin(body, system, iBody, jBody, iIndexJ)\
      + fdDistDir14Sin(body, system, iBody, jBody, iIndexJ)\
      + fdDistDir15Sin(body, system, iBody, jBody, iIndexJ)\
      + fdDistDir16Sin(body, system, iBody, jBody, iIndexJ)\
      + fdDistDir17Sin(body, system, iBody, jBody, iIndexJ)\
      + fdDistDir18Sin(body, system, iBody, jBody, iIndexJ)\
      + fdDistDir19Sin(body, system, iBody, jBody, iIndexJ)\
      + fdDistDir110Sin(body, system, iBody, jBody, iIndexJ)\
      + fdDistDir111Sin(body, system, iBody, jBody, iIndexJ)\
      + fdDistDir112Sin(body, system, iBody, jBody, iIndexJ)\
      + fdDistDir113Sin(body, system, iBody, jBody, iIndexJ)\
      + fdDistDir114Sin(body, system, iBody, jBody, iIndexJ) );
  } else if (iOrder == 2) {
    y = ( fdDistDir21Sin(body, system, iBody, jBody, iIndexJ) \
    + fdDistDir22Sin(body, system, iBody, jBody, iIndexJ)
    + fdDistDir23Sin(body, system, iBody, jBody, iIndexJ) \
    + fdDistDir24Sin(body, system, iBody, jBody, iIndexJ)
    + fdDistDir25Sin(body, system, iBody, jBody, iIndexJ) \
    + fdDistDir26Sin(body, system, iBody, jBody, iIndexJ) );
  } else if (iOrder == 3) {
    y = ( fdDistDir31Sin(body, system, iBody, jBody, iIndexJ) \
    + fdDistDir32Sin(body, system, iBody, jBody, iIndexJ) \
    + fdDistDir33Sin(body, system, iBody, jBody, iIndexJ) \
    + fdDistDir34Sin(body, system, iBody, jBody, iIndexJ) \
    + fdDistDir35Sin(body, system, iBody, jBody, iIndexJ) \
    + fdDistDir36Sin(body, system, iBody, jBody, iIndexJ) \
    + fdDistDir37Sin(body, system, iBody, jBody, iIndexJ) \
    + fdDistDir38Sin(body, system, iBody, jBody, iIndexJ) \
    + fdDistDir39Sin(body, system, iBody, jBody, iIndexJ) \
    + fdDistDir310Sin(body, system, iBody, jBody, iIndexJ) );
  }
  return y;
}

double fdDistSec(BODY *body,SYSTEM *system,int iBody,int jBody) {
  double y;

  y = (fdDistDir01(body,system,iBody,jBody)+fdDistDir02(body,system,iBody,jBody)\
        +fdDistDir03(body,system,iBody,jBody));
  return y;
}

double fdDdistDalpha(BODY *body,SYSTEM *system,int iBody,int jBody) {
  double y;

  y = (fdDdistDaDir01(body,system,iBody,jBody)+fdDdistDaDir02(body,system,iBody,jBody)\
        +fdDdistDaDir03(body,system,iBody,jBody))*body[jBody].dSemi/AUCM;
  return y;
}

double fdD2distDalpha2(BODY *body,SYSTEM *system,int iBody,int jBody) {
  double y;

  y = (fdD2distDa2Dir01(body,system,iBody,jBody)+fdD2distDa2Dir02(body,system,iBody,jBody)\
        +fdD2distDa2Dir03(body,system,iBody,jBody));
  return y;
}

double fdLibrFreq2(BODY *body,SYSTEM *system,int iBody,int jBody,int iRes) {
  //calculate libration frequency of exact resonance (utilizes linear secular theory)
  double F11, F12, F21, F22;     //terms from linear secular theory
  double R, dRda, d2Rda2;        //derivs of 2nd order secular terms
  double dAlpha = body[iBody].dSemi/body[jBody].dSemi;
  double dMu, dMuPrm, dMu0;
  double df;                     //pythagorean sum of dist cos and sin prefactors
  double omega02;                //libration frequency squared
  int iOrder = system->iaResOrder[iRes];
  int iIndexJ = system->iaResIndex[iRes][system->iaLaplaceN[iBody][jBody]];
  dMu = body[iBody].dMass;
  dMuPrm = body[jBody].dMass;
  dMu0 = body[0].dMass;

  R = fdDistSec(body,system,iBody,jBody);
  dRda = fdDdistDalpha(body,system,iBody,jBody);
  d2Rda2 = fdD2distDalpha2(body,system,iBody,jBody);

  F11 = 1.0+2*dMuPrm/dMu0*(1./3*dAlpha*dAlpha*dRda+2./3*dAlpha*dAlpha*dAlpha*d2Rda2);
  F12 = -4./3*dMuPrm/dMu0*(2*pow(dAlpha,0.5)*dRda+pow(dAlpha,1.5)*d2Rda2);
  F21 = -4./3*dMu/dMu0*(2*pow(dAlpha,2.5)*dRda+pow(dAlpha,3.5)*d2Rda2);
  F22 = 1.0+2*dMu/dMu0*(R+7./3*dAlpha*dRda+2./3*dAlpha*dAlpha*d2Rda2);

  df = system->daDistSec[iRes][system->iaLaplaceN[iBody][jBody]];

  omega02 = -3*KGAUSS*KGAUSS/MSUN*AUCM*AUCM*AUCM/body[jBody].dSemi\
          *( iIndexJ*(F21*dMuPrm/(body[iBody].dSemi*body[iBody].dSemi)*(iOrder-iIndexJ)\
              +F22*dMu/(body[jBody].dSemi*body[jBody].dSemi)*iIndexJ)\
          +(iOrder-iIndexJ)*(F11*dMuPrm/(body[iBody].dSemi*body[iBody].dSemi)*(iOrder-iIndexJ)\
              +F12*dMu/(body[jBody].dSemi*body[jBody].dSemi)*iIndexJ) )*df;

  return omega02/(1.0*DAYSEC*DAYSEC);
}

double fdLambdaDotiRes(BODY *body,SYSTEM *system,int iBody,int jBody,int iRes) {
  double sum = 0.0, dMu, y;
  int iResIndex,iOrder;
  //Here, iBody = body in question, jBody = perturber
  dMu = KGAUSS*KGAUSS*(body[0].dMass+body[iBody].dMass)/MSUN;

  sum += sqrt(dMu/(body[iBody].dSemi/AUCM*body[iBody].dSemi/AUCM*body[iBody].dSemi/AUCM));
  y = fabs(1.-body[iBody].dHecc*body[iBody].dHecc-\
    body[iBody].dKecc*body[iBody].dKecc);
  iResIndex = system->iaResIndex[iRes][system->iaLaplaceN[iBody][jBody]];
  iOrder = system->iaResOrder[iRes];
  if (iResIndex == -1) {
    sum += 0.0;
  } else {
    if (body[iBody].dSemi < body[jBody].dSemi) {
      sum +=  -2.0*sqrt(body[iBody].dSemi/AUCM/dMu)\
             *(fdDdistresDSemi0th(body,system,iBody, jBody)+\
             fdDdistresDSemi(body,system,iBody, jBody,iResIndex,iOrder,iRes))\
             +( sqrt(y)/(1.+sqrt(y))*(body[iBody].dHecc*\
             fdDdistresDHecc(body,system,iBody, jBody,iResIndex,iOrder,iRes)\
             +body[iBody].dKecc*\
             fdDdistresDKecc(body,system,iBody, jBody,iResIndex,iOrder,iRes))\
             +0.5/sqrt(y)*(body[iBody].dPinc*\
             fdDdistresDPinc(body,system,iBody, jBody,iResIndex,iOrder,iRes)\
             +body[iBody].dQinc*\
             fdDdistresDQinc(body,system,iBody, jBody,iResIndex,iOrder,iRes)) )\
             /sqrt(dMu*body[iBody].dSemi/AUCM);
    } else if (body[iBody].dSemi > body[jBody].dSemi) {
      sum += -2.0*sqrt(body[iBody].dSemi/AUCM/dMu)\
            *(fdDdistresDSemiPrime0th(body,system,iBody, jBody)+\
            fdDdistresDSemiPrime(body,system,iBody, jBody,iResIndex,iOrder,iRes))\
            +( sqrt(y)/(1.+sqrt(y))*(body[iBody].dHecc*\
            fdDdistresDHeccPrime(body,system,iBody, jBody,iResIndex,iOrder,iRes)\
            +body[iBody].dKecc*\
            fdDdistresDKeccPrime(body,system,iBody, jBody,iResIndex,iOrder,iRes))\
            +0.5/sqrt(y)*(body[iBody].dPinc*\
            fdDdistresDPincPrime(body,system,iBody, jBody,iResIndex,iOrder,iRes)\
            +body[iBody].dQinc*\
            fdDdistresDQincPrime(body,system,iBody, jBody,iResIndex,iOrder,iRes)) )\
            /sqrt(dMu*body[iBody].dSemi/AUCM);
    }
  }

  return sum/DAYSEC;
}

double fdSemiDotiRes(BODY *body, SYSTEM *system, int iBody, int jBody, int iRes) {
  double sum = 0.0, dMu, y;
  int iResIndex,iOrder;
  //Here, iBody = body in question, jBody = perturber
  dMu = KGAUSS*KGAUSS*(body[0].dMass+body[iBody].dMass)/MSUN;

  iResIndex = system->iaResIndex[iRes][system->iaLaplaceN[iBody][jBody]];
  iOrder = system->iaResOrder[iRes];
  if (iResIndex == -1) {
    sum += 0.0;
  } else {
    if (body[iBody].dSemi < body[jBody].dSemi) {
      sum += 2.0*sqrt(body[iBody].dSemi/AUCM/dMu) * \
            fdDdistresDLambda(body,system,iBody, jBody,iResIndex,iOrder,iRes);
    } else if (body[iBody].dSemi > body[jBody].dSemi) {
      sum += 2.0*sqrt(body[iBody].dSemi/AUCM/dMu) * \
            fdDdistresDLambdaPrime(body,system,iBody, jBody,iResIndex,iOrder,iRes);
    }
  }

  return sum/DAYSEC;
}

double fdHeccDotiRes(BODY *body, SYSTEM *system, int iBody, int jBody, int iRes) {
  double sum = 0.0, dMu, y;
  int iResIndex,iOrder;
  //Here, iBody = body in question, jBody = perturber

  dMu = KGAUSS*KGAUSS*(body[0].dMass+body[iBody].dMass)/MSUN;
  y = fabs(1.-body[iBody].dHecc*body[iBody].dHecc-\
  body[iBody].dKecc*body[iBody].dKecc);
  iResIndex = system->iaResIndex[iRes][system->iaLaplaceN[iBody][jBody]];
  iOrder = system->iaResOrder[iRes];
  if (iResIndex == -1) {
    sum += 0.0;
  } else {
    if (body[iBody].dSemi < body[jBody].dSemi) {
      sum += ( sqrt(y)*fdDdistresDKecc(body,system, iBody, jBody,iResIndex,iOrder,iRes) + \
      body[iBody].dKecc*(body[iBody].dPinc*\
      fdDdistresDPinc(body, system, iBody, jBody,iResIndex,iOrder,iRes)\
      +body[iBody].dQinc*\
      fdDdistresDQinc(body, system, iBody, jBody,iResIndex,iOrder,iRes))/(2*sqrt(y)) \
      -body[iBody].dHecc*sqrt(y)/(1.+sqrt(y))*\
      fdDdistresDLambda(body,system,iBody, jBody,iResIndex,iOrder,iRes) )\
      / sqrt(dMu*body[iBody].dSemi/AUCM);

    } else if (body[iBody].dSemi > body[jBody].dSemi) {
      sum += ( sqrt(y)*\
      fdDdistresDKeccPrime(body, system, iBody, jBody,iResIndex,iOrder,iRes) + \
      body[iBody].dKecc*(body[iBody].dPinc*\
      fdDdistresDPincPrime(body, system, iBody, jBody,iResIndex,iOrder,iRes)\
      +body[iBody].dQinc*\
      fdDdistresDQincPrime(body, system, iBody, jBody,iResIndex,iOrder,iRes))/(2*sqrt(y)) \
      -body[iBody].dHecc*sqrt(y)/(1.+sqrt(y))*\
      fdDdistresDLambdaPrime(body,system,iBody, jBody,iResIndex,iOrder,iRes) )\
      / sqrt(dMu*body[iBody].dSemi/AUCM);
    }
  }

  return sum/DAYSEC;
}

double fdKeccDotiRes(BODY *body, SYSTEM *system, int iBody, int jBody, int iRes) {
  double sum = 0.0, dMu, y;
  int iResIndex,iOrder;

  dMu = KGAUSS*KGAUSS*(body[0].dMass+body[iBody].dMass)/MSUN;
  y = fabs(1.-body[iBody].dHecc*body[iBody].dHecc-\
  body[iBody].dKecc*body[iBody].dKecc);
  iResIndex = system->iaResIndex[iRes][system->iaLaplaceN[iBody][jBody]];
  iOrder = system->iaResOrder[iRes];
  if (iResIndex == -1) {
    sum += 0.0;
  } else {
    if (body[iBody].dSemi < body[jBody].dSemi) {
      sum += -( sqrt(y)*fdDdistresDHecc(body, system, iBody, jBody,iResIndex,iOrder,iRes)+\
      body[iBody].dHecc*(body[iBody].dPinc*\
      fdDdistresDPinc(body, system, iBody, jBody,iResIndex,iOrder,iRes)\
      +body[iBody].dQinc*\
      fdDdistresDQinc(body, system, iBody, jBody,iResIndex,iOrder,iRes))/(2*sqrt(y)) \
      +body[iBody].dKecc*sqrt(y)/(1.+sqrt(y))*\
      fdDdistresDLambda(body,system,iBody, jBody,iResIndex,iOrder,iRes) ) \
      / sqrt(dMu*body[iBody].dSemi/AUCM);

    } else if (body[iBody].dSemi > body[jBody].dSemi) {
      sum += -( sqrt(y)*\
      fdDdistresDHeccPrime(body, system, iBody, jBody,iResIndex,iOrder,iRes) + \
      body[iBody].dHecc*(body[iBody].dPinc*\
      fdDdistresDPincPrime(body, system, iBody, jBody,iResIndex,iOrder,iRes)\
      +body[iBody].dQinc*\
      fdDdistresDQincPrime(body, system, iBody, jBody,iResIndex,iOrder,iRes))/(2*sqrt(y)) \
      +body[iBody].dKecc*sqrt(y)/(1.+sqrt(y))*\
      fdDdistresDLambdaPrime(body,system,iBody, jBody,iResIndex,iOrder,iRes) )\
      / sqrt(dMu*body[iBody].dSemi/AUCM);
    }
  }


  return sum/DAYSEC;
}

double fdPincDotiRes(BODY *body, SYSTEM *system, int iBody, int jBody, int iRes) {
    double sum = 0.0, dMu, y;
    int iResIndex,iOrder;

    dMu = KGAUSS*KGAUSS*(body[0].dMass+body[iBody].dMass)/MSUN;
    y = fabs(1.-body[iBody].dHecc*body[iBody].dHecc-\
    body[iBody].dKecc*body[iBody].dKecc);
    iResIndex = system->iaResIndex[iRes][system->iaLaplaceN[iBody][jBody]];
    iOrder = system->iaResOrder[iRes];
    if (iResIndex == -1) {
      sum += 0.0;
    } else {
      if (body[iBody].dSemi < body[jBody].dSemi) {
        sum += ( body[iBody].dPinc*(-body[iBody].dKecc*\
        fdDdistresDHecc(body,system,iBody, jBody,iResIndex,iOrder,iRes)\
        +body[iBody].dHecc*\
        fdDdistresDKecc(body,system,iBody, jBody,iResIndex,iOrder,iRes))\
        +1.0/2.0*fdDdistresDQinc(body,system,iBody, jBody,iResIndex,iOrder,iRes)\
        -body[iBody].dPinc*\
        fdDdistresDLambda(body,system,iBody, jBody,iResIndex,iOrder,iRes) )\
        /(2*sqrt(dMu*body[iBody].dSemi/AUCM*(y)));
      } else if (body[iBody].dSemi > body[jBody].dSemi) {
        sum += ( body[iBody].dPinc*(-body[iBody].dKecc*\
        fdDdistresDHeccPrime(body,system,iBody, jBody,iResIndex,iOrder,iRes)\
        +body[iBody].dHecc*\
        fdDdistresDKeccPrime(body,system,iBody, jBody,iResIndex,iOrder,iRes))\
        +1.0/2.0*fdDdistresDQincPrime(body,system,iBody, jBody,iResIndex,iOrder,iRes)\
        -body[iBody].dPinc*\
        fdDdistresDLambdaPrime(body,system,iBody, jBody,iResIndex,iOrder,iRes) )\
        /(2*sqrt(dMu*body[iBody].dSemi/AUCM*(y)));
      }
    }

    return sum/DAYSEC;
}


double fdQincDotiRes(BODY *body, SYSTEM *system, int iBody, int jBody, int iRes) {
    double sum = 0.0, dMu, y;
    int iResIndex,iOrder;

    dMu = KGAUSS*KGAUSS*(body[0].dMass+body[iBody].dMass)/MSUN;
    y = fabs(1.-body[iBody].dHecc*body[iBody].dHecc-\
       body[iBody].dKecc*body[iBody].dKecc);
    iResIndex = system->iaResIndex[iRes][system->iaLaplaceN[iBody][jBody]];
    iOrder = system->iaResOrder[iRes];
    if (iResIndex == -1) {
      sum += 0.0;
    } else {
      if (body[iBody].dSemi < body[jBody].dSemi) {
        sum += ( body[iBody].dQinc*(-body[iBody].dKecc*\
        fdDdistresDHecc(body,system,iBody, jBody,iResIndex,iOrder,iRes)\
        +body[iBody].dHecc*\
        fdDdistresDKecc(body,system,iBody, jBody,iResIndex,iOrder,iRes))\
        -1.0/2.0*fdDdistresDPinc(body,system,iBody, jBody,iResIndex,iOrder,iRes)\
        -body[iBody].dQinc*\
        fdDdistresDLambda(body,system,iBody, jBody,iResIndex,iOrder,iRes) )\
        /(2*sqrt(dMu*body[iBody].dSemi/AUCM*(y)));
      } else if (body[iBody].dSemi > body[jBody].dSemi) {
        sum += ( body[iBody].dQinc*(-body[iBody].dKecc*\
        fdDdistresDHeccPrime(body,system,iBody, jBody,iResIndex,iOrder,iRes)\
        +body[iBody].dHecc*\
        fdDdistresDKeccPrime(body,system,iBody, jBody,iResIndex,iOrder,iRes))\
        -1.0/2.0*fdDdistresDPincPrime(body,system,iBody, jBody,iResIndex,iOrder,iRes) \
        -body[iBody].dQinc*\
        fdDdistresDLambdaPrime(body,system,iBody, jBody,iResIndex,iOrder,iRes) )\
        /(2*sqrt(dMu*body[iBody].dSemi/AUCM*(y)));
      }
    }

    return sum/DAYSEC;
}

double fdBetaDot(BODY *body,SYSTEM *system,int iBody,int jBody,int iRes) {
  double f, fc, fs, fcdot, fsdot;
  int iIndexJ, iOrder;
  f = system->daDistSec[iRes][system->iaLaplaceN[iBody][jBody]];
  fc = system->daDistCos[iRes][system->iaLaplaceN[iBody][jBody]];
  fs = system->daDistSin[iRes][system->iaLaplaceN[iBody][jBody]];
  iIndexJ = system->iaResIndex[iRes][system->iaLaplaceN[iBody][jBody]];
  iOrder = system->iaResOrder[iRes];

  fcdot = ( fdDdistresDHeccDirCos(body,system,iBody,jBody,iIndexJ,iOrder)*\
              fdHeccDotiRes(body,system,iBody,jBody,iRes)\
            +fdDdistresDKeccDirCos(body,system,iBody,jBody,iIndexJ,iOrder)*\
              fdKeccDotiRes(body,system,iBody,jBody,iRes)\
            +fdDdistresDPincDirCos(body,system,iBody,jBody,iIndexJ,iOrder)*\
              fdPincDotiRes(body,system,iBody,jBody,iRes)\
            +fdDdistresDQincDirCos(body,system,iBody,jBody,iIndexJ,iOrder)*\
              fdQincDotiRes(body,system,iBody,jBody,iRes)\
            +fdDdistresDSemiDirCos(body,system,iBody,jBody,iIndexJ,iOrder)*\
              fdSemiDotiRes(body,system,iBody,jBody,iRes)\
            +fdDdistresDHeccPrimeDirCos(body,system,jBody,iBody,iIndexJ,iOrder)*\
              fdHeccDotiRes(body,system,jBody,iBody,iRes)\
            +fdDdistresDKeccPrimeDirCos(body,system,jBody,iBody,iIndexJ,iOrder)*\
              fdKeccDotiRes(body,system,jBody,iBody,iRes)\
            +fdDdistresDPincPrimeDirCos(body,system,jBody,iBody,iIndexJ,iOrder)*\
              fdPincDotiRes(body,system,jBody,iBody,iRes)\
            +fdDdistresDQincPrimeDirCos(body,system,jBody,iBody,iIndexJ,iOrder)*\
              fdQincDotiRes(body,system,jBody,iBody,iRes)\
            +fdDdistresDSemiPrimeDirCos(body,system,jBody,iBody,iIndexJ,iOrder)*\
              fdSemiDotiRes(body,system,jBody,iBody,iRes) );

  fsdot = ( fdDdistresDHeccDirSin(body,system,iBody,jBody,iIndexJ,iOrder)*\
              fdHeccDotiRes(body,system,iBody,jBody,iRes)\
            +fdDdistresDKeccDirSin(body,system,iBody,jBody,iIndexJ,iOrder)*\
              fdKeccDotiRes(body,system,iBody,jBody,iRes)\
            +fdDdistresDPincDirSin(body,system,iBody,jBody,iIndexJ,iOrder)*\
              fdPincDotiRes(body,system,iBody,jBody,iRes)\
            +fdDdistresDQincDirSin(body,system,iBody,jBody,iIndexJ,iOrder)*\
              fdQincDotiRes(body,system,iBody,jBody,iRes)\
            +fdDdistresDSemiDirSin(body,system,iBody,jBody,iIndexJ,iOrder)*\
              fdSemiDotiRes(body,system,iBody,jBody,iRes)\
            +fdDdistresDHeccPrimeDirSin(body,system,jBody,iBody,iIndexJ,iOrder)*\
              fdHeccDotiRes(body,system,jBody,iBody,iRes)\
            +fdDdistresDKeccPrimeDirSin(body,system,jBody,iBody,iIndexJ,iOrder)*\
              fdKeccDotiRes(body,system,jBody,iBody,iRes)\
            +fdDdistresDPincPrimeDirSin(body,system,jBody,iBody,iIndexJ,iOrder)*\
              fdPincDotiRes(body,system,jBody,iBody,iRes)\
            +fdDdistresDQincPrimeDirSin(body,system,jBody,iBody,iIndexJ,iOrder)*\
              fdQincDotiRes(body,system,jBody,iBody,iRes)\
            +fdDdistresDSemiPrimeDirSin(body,system,jBody,iBody,iIndexJ,iOrder)*\
              fdSemiDotiRes(body,system,jBody,iBody,iRes) );

  return (fc*fsdot-fs*fcdot)/(f*f);
}

double fdCircFreq(BODY *body,SYSTEM *system,int iBody,int jBody,int iRes) {
  //iBody should be the inner body
  double omega, theta_dot, beta_dot;
  int iOrder = system->iaResOrder[iRes];
  int iIndexJ = system->iaResIndex[iRes][system->iaLaplaceN[iBody][jBody]];

  theta_dot = iIndexJ*fdLambdaDotiRes(body,system,jBody,iBody,iRes)\
              +(iOrder-iIndexJ)*fdLambdaDotiRes(body,system,iBody,jBody,iRes);

//   beta_dot = fdBetaDot(body,system,iBody,jBody,iRes);

  omega = -theta_dot;
  return omega;
}

//-------------------DistRes's equations in a h k p q l (2nd order direct integration RD2)--------------------
double fdDistResRD2DaDt(BODY *body, SYSTEM *system, int *iaBody) {
  double sum = 0.0, dMu, y;
  int iRes,iResIndex,iOrder;
  //Here, iaBody[0] = body in question, iaBody[1] = perturber
  dMu = KGAUSS*KGAUSS*(body[0].dMass+body[iaBody[0]].dMass)/MSUN;

  for (iRes=0;iRes<RESNUM;iRes++) {
    iResIndex = system->iaResIndex[iRes][system->iaLaplaceN[iaBody[0]][iaBody[1]]];
    iOrder = system->iaResOrder[iRes];
    if (iResIndex == -1) {
      sum += 0.0;
    } else {
      if (body[iaBody[0]].dSemi < body[iaBody[1]].dSemi) {
        sum += 2.0*sqrt(body[iaBody[0]].dSemi/AUCM/dMu) * \
              fdDdistresDLambda(body,system,iaBody[0], iaBody[1],iResIndex,iOrder,iRes);
      } else if (body[iaBody[0]].dSemi > body[iaBody[1]].dSemi) {
        sum += 2.0*sqrt(body[iaBody[0]].dSemi/AUCM/dMu) * \
              fdDdistresDLambdaPrime(body,system,iaBody[0], iaBody[1],iResIndex,iOrder,iRes);
      }
    }
  }

  return sum/DAYSEC*AUCM;
}

double fdDistResRD2DlDt(BODY *body, SYSTEM *system, int *iaBody) {
  double sum = 0.0, dMu, y;
  int iRes,iResIndex,iOrder;
  //Here, iaBody[0] = body in question, iaBody[1] = perturber
  dMu = KGAUSS*KGAUSS*(body[0].dMass+body[iaBody[0]].dMass)/MSUN;

  if (system->bResAvg==0) {
    sum += sqrt(dMu/(body[iaBody[0]].dSemi/AUCM*body[iaBody[0]].dSemi/AUCM*body[iaBody[0]].dSemi/AUCM));
  }
  y = fabs(1.-body[iaBody[0]].dHecc*body[iaBody[0]].dHecc-\
    body[iaBody[0]].dKecc*body[iaBody[0]].dKecc);
  for (iRes=0;iRes<RESNUM;iRes++) {
    iResIndex = system->iaResIndex[iRes][system->iaLaplaceN[iaBody[0]][iaBody[1]]];
    iOrder = system->iaResOrder[iRes];
    if (iResIndex == -1) {
      sum += 0.0;
    } else {
      if (body[iaBody[0]].dSemi < body[iaBody[1]].dSemi) {
        sum +=  -2.0*sqrt(body[iaBody[0]].dSemi/AUCM/dMu)\
               *(fdDdistresDSemi0th(body,system,iaBody[0],iaBody[1])+\
               fdDdistresDSemi(body,system,iaBody[0], iaBody[1],iResIndex,iOrder,iRes))\
               +( sqrt(y)/(1.+sqrt(y))*(body[iaBody[0]].dHecc*\
               fdDdistresDHecc(body,system,iaBody[0], iaBody[1],iResIndex,iOrder,iRes)\
               +body[iaBody[0]].dKecc*\
               fdDdistresDKecc(body,system,iaBody[0], iaBody[1],iResIndex,iOrder,iRes))\
               +0.5/sqrt(y)*(body[iaBody[0]].dPinc*\
               fdDdistresDPinc(body,system,iaBody[0], iaBody[1],iResIndex,iOrder,iRes)\
               +body[iaBody[0]].dQinc*\
               fdDdistresDQinc(body,system,iaBody[0], iaBody[1],iResIndex,iOrder,iRes)) )\
               /sqrt(dMu*body[iaBody[0]].dSemi/AUCM);
      } else if (body[iaBody[0]].dSemi > body[iaBody[1]].dSemi) {
        sum += -2.0*sqrt(body[iaBody[0]].dSemi/AUCM/dMu)\
              *(fdDdistresDSemiPrime0th(body,system,iaBody[0],iaBody[1])+\
              fdDdistresDSemiPrime(body,system,iaBody[0], iaBody[1],iResIndex,iOrder,iRes))\
              +( sqrt(y)/(1.+sqrt(y))*(body[iaBody[0]].dHecc*\
              fdDdistresDHeccPrime(body,system,iaBody[0], iaBody[1],iResIndex,iOrder,iRes)\
              +body[iaBody[0]].dKecc*\
              fdDdistresDKeccPrime(body,system,iaBody[0], iaBody[1],iResIndex,iOrder,iRes))\
              +0.5/sqrt(y)*(body[iaBody[0]].dPinc*\
              fdDdistresDPincPrime(body,system,iaBody[0], iaBody[1],iResIndex,iOrder,iRes)\
              +body[iaBody[0]].dQinc*\
              fdDdistresDQincPrime(body,system,iaBody[0], iaBody[1],iResIndex,iOrder,iRes)) )\
              /sqrt(dMu*body[iaBody[0]].dSemi/AUCM);
      }
    }
  }

  return sum/DAYSEC;

}

double fdDistResRD2DhDt(BODY *body, SYSTEM *system, int *iaBody) {
  double sum = 0.0, dMu, y;
  int iRes,iResIndex,iOrder;
  //Here, iaBody[0] = body in question, iaBody[1] = perturber

  dMu = KGAUSS*KGAUSS*(body[0].dMass+body[iaBody[0]].dMass)/MSUN;
  y = fabs(1.-body[iaBody[0]].dHecc*body[iaBody[0]].dHecc-\
    body[iaBody[0]].dKecc*body[iaBody[0]].dKecc);
  for (iRes=0;iRes<RESNUM;iRes++) {
    iResIndex = system->iaResIndex[iRes][system->iaLaplaceN[iaBody[0]][iaBody[1]]];
    iOrder = system->iaResOrder[iRes];
    if (iResIndex == -1) {
      sum += 0.0;
    } else {
      if (body[iaBody[0]].dSemi < body[iaBody[1]].dSemi) {
        sum += ( sqrt(y)*fdDdistresDKecc(body,system, iaBody[0], iaBody[1],iResIndex,iOrder,iRes) + \
        body[iaBody[0]].dKecc*(body[iaBody[0]].dPinc*\
        fdDdistresDPinc(body, system, iaBody[0], iaBody[1],iResIndex,iOrder,iRes)\
        +body[iaBody[0]].dQinc*\
        fdDdistresDQinc(body, system, iaBody[0], iaBody[1],iResIndex,iOrder,iRes))/(2*sqrt(y)) \
        -body[iaBody[0]].dHecc*sqrt(y)/(1.+sqrt(y))*\
        fdDdistresDLambda(body,system,iaBody[0], iaBody[1],iResIndex,iOrder,iRes) )\
        / sqrt(dMu*body[iaBody[0]].dSemi/AUCM);

      } else if (body[iaBody[0]].dSemi > body[iaBody[1]].dSemi) {
        sum += ( sqrt(y)*\
        fdDdistresDKeccPrime(body, system, iaBody[0], iaBody[1],iResIndex,iOrder,iRes) + \
        body[iaBody[0]].dKecc*(body[iaBody[0]].dPinc*\
        fdDdistresDPincPrime(body, system, iaBody[0], iaBody[1],iResIndex,iOrder,iRes)\
        +body[iaBody[0]].dQinc*\
        fdDdistresDQincPrime(body, system, iaBody[0], iaBody[1],iResIndex,iOrder,iRes))/(2*sqrt(y)) \
        -body[iaBody[0]].dHecc*sqrt(y)/(1.+sqrt(y))*\
        fdDdistresDLambdaPrime(body,system,iaBody[0], iaBody[1],iResIndex,iOrder,iRes) )\
        / sqrt(dMu*body[iaBody[0]].dSemi/AUCM);
      }
    }
  }
  return sum/DAYSEC;
}

double fdDistResRD2DkDt(BODY *body, SYSTEM *system, int *iaBody) {
  double sum = 0.0, dMu, y;
  int iRes,iResIndex,iOrder;

  dMu = KGAUSS*KGAUSS*(body[0].dMass+body[iaBody[0]].dMass)/MSUN;
  y = fabs(1.-body[iaBody[0]].dHecc*body[iaBody[0]].dHecc-\
    body[iaBody[0]].dKecc*body[iaBody[0]].dKecc);
  for (iRes=0;iRes<RESNUM;iRes++) {
    iResIndex = system->iaResIndex[iRes][system->iaLaplaceN[iaBody[0]][iaBody[1]]];
    iOrder = system->iaResOrder[iRes];
    if (iResIndex == -1) {
      sum += 0.0;
    } else {
      if (body[iaBody[0]].dSemi < body[iaBody[1]].dSemi) {
        sum += -( sqrt(y)*fdDdistresDHecc(body, system, iaBody[0], iaBody[1],iResIndex,iOrder,iRes)+\
        body[iaBody[0]].dHecc*(body[iaBody[0]].dPinc*\
        fdDdistresDPinc(body, system, iaBody[0], iaBody[1],iResIndex,iOrder,iRes)\
        +body[iaBody[0]].dQinc*\
        fdDdistresDQinc(body, system, iaBody[0], iaBody[1],iResIndex,iOrder,iRes))/(2*sqrt(y)) \
        +body[iaBody[0]].dKecc*sqrt(y)/(1.+sqrt(y))*\
        fdDdistresDLambda(body,system,iaBody[0], iaBody[1],iResIndex,iOrder,iRes) ) \
        / sqrt(dMu*body[iaBody[0]].dSemi/AUCM);

      } else if (body[iaBody[0]].dSemi > body[iaBody[1]].dSemi) {
        sum += -( sqrt(y)*\
        fdDdistresDHeccPrime(body, system, iaBody[0], iaBody[1],iResIndex,iOrder,iRes) + \
        body[iaBody[0]].dHecc*(body[iaBody[0]].dPinc*\
        fdDdistresDPincPrime(body, system, iaBody[0], iaBody[1],iResIndex,iOrder,iRes)\
        +body[iaBody[0]].dQinc*\
        fdDdistresDQincPrime(body, system, iaBody[0], iaBody[1],iResIndex,iOrder,iRes))/(2*sqrt(y)) \
        +body[iaBody[0]].dKecc*sqrt(y)/(1.+sqrt(y))*\
        fdDdistresDLambdaPrime(body,system,iaBody[0], iaBody[1],iResIndex,iOrder,iRes) )\
        / sqrt(dMu*body[iaBody[0]].dSemi/AUCM);
      }
    }
  }

  return sum/DAYSEC;
}

double fdDistResRD2DpDt(BODY *body, SYSTEM *system, int *iaBody) {
    double sum = 0.0, dMu, y;
    int iRes,iResIndex,iOrder;

    dMu = KGAUSS*KGAUSS*(body[0].dMass+body[iaBody[0]].dMass)/MSUN;
    y = fabs(1.-body[iaBody[0]].dHecc*body[iaBody[0]].dHecc-\
      body[iaBody[0]].dKecc*body[iaBody[0]].dKecc);
    for (iRes=0;iRes<RESNUM;iRes++) {
      iResIndex = system->iaResIndex[iRes][system->iaLaplaceN[iaBody[0]][iaBody[1]]];
      iOrder = system->iaResOrder[iRes];
      if (iResIndex == -1) {
        sum += 0.0;
      } else {
        if (body[iaBody[0]].dSemi < body[iaBody[1]].dSemi) {
          sum += ( body[iaBody[0]].dPinc*(-body[iaBody[0]].dKecc*\
          fdDdistresDHecc(body,system,iaBody[0], iaBody[1],iResIndex,iOrder,iRes)\
          +body[iaBody[0]].dHecc*\
          fdDdistresDKecc(body,system,iaBody[0], iaBody[1],iResIndex,iOrder,iRes))\
          +1.0/2.0*fdDdistresDQinc(body,system,iaBody[0], iaBody[1],iResIndex,iOrder,iRes)\
          -body[iaBody[0]].dPinc*\
          fdDdistresDLambda(body,system,iaBody[0], iaBody[1],iResIndex,iOrder,iRes) )\
          /(2*sqrt(dMu*body[iaBody[0]].dSemi/AUCM*(y)));
        } else if (body[iaBody[0]].dSemi > body[iaBody[1]].dSemi) {
          sum += ( body[iaBody[0]].dPinc*(-body[iaBody[0]].dKecc*\
          fdDdistresDHeccPrime(body,system,iaBody[0], iaBody[1],iResIndex,iOrder,iRes)\
          +body[iaBody[0]].dHecc*\
          fdDdistresDKeccPrime(body,system,iaBody[0], iaBody[1],iResIndex,iOrder,iRes))\
          +1.0/2.0*fdDdistresDQincPrime(body,system,iaBody[0], iaBody[1],iResIndex,iOrder,iRes)\
          -body[iaBody[0]].dPinc*\
          fdDdistresDLambdaPrime(body,system,iaBody[0], iaBody[1],iResIndex,iOrder,iRes) )\
          /(2*sqrt(dMu*body[iaBody[0]].dSemi/AUCM*(y)));
        }
      }
    }
    return sum/DAYSEC;
}


double fdDistResRD2DqDt(BODY *body, SYSTEM *system, int *iaBody) {
    double sum = 0.0, dMu, y;
    int iRes,iResIndex,iOrder;

    dMu = KGAUSS*KGAUSS*(body[0].dMass+body[iaBody[0]].dMass)/MSUN;
    y = fabs(1.-body[iaBody[0]].dHecc*body[iaBody[0]].dHecc-\
       body[iaBody[0]].dKecc*body[iaBody[0]].dKecc);
    for (iRes=0;iRes<RESNUM;iRes++) {
      iResIndex = system->iaResIndex[iRes][system->iaLaplaceN[iaBody[0]][iaBody[1]]];
      iOrder = system->iaResOrder[iRes];
      if (iResIndex == -1) {
        sum += 0.0;
      } else {
        if (body[iaBody[0]].dSemi < body[iaBody[1]].dSemi) {
          sum += ( body[iaBody[0]].dQinc*(-body[iaBody[0]].dKecc*\
          fdDdistresDHecc(body,system,iaBody[0], iaBody[1],iResIndex,iOrder,iRes)\
          +body[iaBody[0]].dHecc*\
          fdDdistresDKecc(body,system,iaBody[0], iaBody[1],iResIndex,iOrder,iRes))\
          -1.0/2.0*fdDdistresDPinc(body,system,iaBody[0], iaBody[1],iResIndex,iOrder,iRes)\
          -body[iaBody[0]].dQinc*\
          fdDdistresDLambda(body,system,iaBody[0], iaBody[1],iResIndex,iOrder,iRes) )\
          /(2*sqrt(dMu*body[iaBody[0]].dSemi/AUCM*(y)));
        } else if (body[iaBody[0]].dSemi > body[iaBody[1]].dSemi) {
          sum += ( body[iaBody[0]].dQinc*(-body[iaBody[0]].dKecc*\
          fdDdistresDHeccPrime(body,system,iaBody[0], iaBody[1],iResIndex,iOrder,iRes)\
          +body[iaBody[0]].dHecc*\
          fdDdistresDKeccPrime(body,system,iaBody[0], iaBody[1],iResIndex,iOrder,iRes))\
          -1.0/2.0*fdDdistresDPincPrime(body,system,iaBody[0], iaBody[1],iResIndex,iOrder,iRes) \
          -body[iaBody[0]].dQinc*\
          fdDdistresDLambdaPrime(body,system,iaBody[0], iaBody[1],iResIndex,iOrder,iRes) )\
          /(2*sqrt(dMu*body[iaBody[0]].dSemi/AUCM*(y)));
        }
      }
    }

    return sum/DAYSEC;
}

void CheckTermsRes1(BODY *body, SYSTEM *system, int iBody, int jBody, int iRes) {
  int iIndexJ = system->iaResIndex[iRes][system->iaLaplaceN[iBody][jBody]];
  int iOrder = system->iaResOrder[iRes];

  //------dh----------------first order--------
  printf("dR11dh = %e\n",(fdDdistDhDir11Cos(body, system, iBody, jBody, iIndexJ)*fdCosLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)+fdDdistDhDir11Sin(body, system, iBody, jBody, iIndexJ)*fdSinLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)));
  printf("dR12dh = %e\n",(fdDdistDhDir12Cos(body, system, iBody, jBody, iIndexJ)*fdCosLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)+fdDdistDhDir12Sin(body, system, iBody, jBody, iIndexJ)*fdSinLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)));
  printf("dR13dh = %e\n",(fdDdistDhDir13Cos(body, system, iBody, jBody, iIndexJ)*fdCosLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)+fdDdistDhDir13Sin(body, system, iBody, jBody, iIndexJ)*fdSinLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)));
  printf("dR14dh = %e\n",(fdDdistDhDir14Cos(body, system, iBody, jBody, iIndexJ)*fdCosLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)+fdDdistDhDir14Sin(body, system, iBody, jBody, iIndexJ)*fdSinLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)));
  printf("dR15dh = %e\n",(fdDdistDhDir15Cos(body, system, iBody, jBody, iIndexJ)*fdCosLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)+fdDdistDhDir15Sin(body, system, iBody, jBody, iIndexJ)*fdSinLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)));
//   printf("dR16dh = %e\n",(fdDdistDhDir16Cos(body, system, iBody, jBody, iIndexJ)*fdCosLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)+fdDdistDhDir16Sin(body, system, iBody, jBody, iIndexJ)*fdSinLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)));
  printf("dR17dh = %e\n",(fdDdistDhDir17Cos(body, system, iBody, jBody, iIndexJ)*fdCosLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)+fdDdistDhDir17Sin(body, system, iBody, jBody, iIndexJ)*fdSinLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)));
  printf("dR18dh = %e\n",(fdDdistDhDir18Cos(body, system, iBody, jBody, iIndexJ)*fdCosLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)+fdDdistDhDir18Sin(body, system, iBody, jBody, iIndexJ)*fdSinLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)));
  printf("dR19dh = %e\n",(fdDdistDhDir19Cos(body, system, iBody, jBody, iIndexJ)*fdCosLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)+fdDdistDhDir19Sin(body, system, iBody, jBody, iIndexJ)*fdSinLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)));
//   printf("dR110dh = %e\n",(fdDdistDhDir110Cos(body, system, iBody, jBody, iIndexJ)*fdCosLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)+fdDdistDhDir110Sin(body, system, iBody, jBody, iIndexJ)*fdSinLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)));
//   printf("dR111dh = %e\n",(fdDdistDhDir111Cos(body, system, iBody, jBody, iIndexJ)*fdCosLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)+fdDdistDhDir111Sin(body, system, iBody, jBody, iIndexJ)*fdSinLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)));
//   printf("dR112dh = %e\n",(fdDdistDhDir112Cos(body, system, iBody, jBody, iIndexJ)*fdCosLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)+fdDdistDhDir112Sin(body, system, iBody, jBody, iIndexJ)*fdSinLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)));
  printf("dR113dh = %e\n",(fdDdistDhDir113Cos(body, system, iBody, jBody, iIndexJ)*fdCosLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)+fdDdistDhDir113Sin(body, system, iBody, jBody, iIndexJ)*fdSinLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)));
//   printf("dR114dh = %e\n",(fdDdistDhDir114Cos(body, system, iBody, jBody, iIndexJ)*fdCosLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)+fdDdistDhDir114Sin(body, system, iBody, jBody, iIndexJ)*fdSinLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)));
  printf("dRE13dh = %e\n",(fdDdistDhExt13Cos(body, system, iBody, jBody, 2)*fdCosLambda(body,system,iBody,jBody,2,1,iRes)+fdDdistDhExt13Sin(body, system, iBody, jBody, 2)*fdSinLambda(body,system,iBody,jBody,2,1,iRes)));
  printf("dRE16dh = %e\n",(fdDdistDhExt16Cos(body, system, iBody, jBody, 3)*fdCosLambda(body,system,iBody,jBody,3,1,iRes)+fdDdistDhExt16Sin(body, system, iBody, jBody, 3)*fdSinLambda(body,system,iBody,jBody,3,1,iRes)));

  //------dk----------------first order--------
  printf("dR11dk = %e\n",(fdDdistDkDir11Cos(body, system, iBody, jBody, iIndexJ)*fdCosLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)+fdDdistDkDir11Sin(body, system, iBody, jBody, iIndexJ)*fdSinLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)));
  printf("dR12dk = %e\n",(fdDdistDkDir12Cos(body, system, iBody, jBody, iIndexJ)*fdCosLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)+fdDdistDkDir12Sin(body, system, iBody, jBody, iIndexJ)*fdSinLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)));
  printf("dR13dk = %e\n",(fdDdistDkDir13Cos(body, system, iBody, jBody, iIndexJ)*fdCosLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)+fdDdistDkDir13Sin(body, system, iBody, jBody, iIndexJ)*fdSinLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)));
  printf("dR14dk = %e\n",(fdDdistDkDir14Cos(body, system, iBody, jBody, iIndexJ)*fdCosLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)+fdDdistDkDir14Sin(body, system, iBody, jBody, iIndexJ)*fdSinLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)));
  printf("dR15dk = %e\n",(fdDdistDkDir15Cos(body, system, iBody, jBody, iIndexJ)*fdCosLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)+fdDdistDkDir15Sin(body, system, iBody, jBody, iIndexJ)*fdSinLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)));
//   printf("dR16dk = %e\n",(fdDdistDkDir16Cos(body, system, iBody, jBody, iIndexJ)*fdCosLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)+fdDdistDkDir16Sin(body, system, iBody, jBody, iIndexJ)*fdSinLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)));
  printf("dR17dk = %e\n",(fdDdistDkDir17Cos(body, system, iBody, jBody, iIndexJ)*fdCosLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)+fdDdistDkDir17Sin(body, system, iBody, jBody, iIndexJ)*fdSinLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)));
  printf("dR18dk = %e\n",(fdDdistDkDir18Cos(body, system, iBody, jBody, iIndexJ)*fdCosLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)+fdDdistDkDir18Sin(body, system, iBody, jBody, iIndexJ)*fdSinLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)));
  printf("dR19dk = %e\n",(fdDdistDkDir19Cos(body, system, iBody, jBody, iIndexJ)*fdCosLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)+fdDdistDkDir19Sin(body, system, iBody, jBody, iIndexJ)*fdSinLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)));
//   printf("dR110dk = %e\n",(fdDdistDkDir110Cos(body, system, iBody, jBody, iIndexJ)*fdCosLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)+fdDdistDkDir110Sin(body, system, iBody, jBody, iIndexJ)*fdSinLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)));
//   printf("dR111dk = %e\n",(fdDdistDkDir111Cos(body, system, iBody, jBody, iIndexJ)*fdCosLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)+fdDdistDkDir111Sin(body, system, iBody, jBody, iIndexJ)*fdSinLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)));
//   printf("dR112dk = %e\n",(fdDdistDkDir112Cos(body, system, iBody, jBody, iIndexJ)*fdCosLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)+fdDdistDkDir112Sin(body, system, iBody, jBody, iIndexJ)*fdSinLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)));
  printf("dR113dk = %e\n",(fdDdistDkDir113Cos(body, system, iBody, jBody, iIndexJ)*fdCosLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)+fdDdistDkDir113Sin(body, system, iBody, jBody, iIndexJ)*fdSinLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)));
//   printf("dR114dk = %e\n",(fdDdistDkDir114Cos(body, system, iBody, jBody, iIndexJ)*fdCosLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)+fdDdistDkDir114Sin(body, system, iBody, jBody, iIndexJ)*fdSinLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)));
  printf("dRE13dk = %e\n",(fdDdistDkExt13Cos(body, system, iBody, jBody, 2)*fdCosLambda(body,system,iBody,jBody,2,1,iRes)+fdDdistDkExt13Sin(body, system, iBody, jBody, 2)*fdSinLambda(body,system,iBody,jBody,2,1,iRes)));
  printf("dRE16dk = %e\n",(fdDdistDkExt16Cos(body, system, iBody, jBody, 3)*fdCosLambda(body,system,iBody,jBody,3,1,iRes)+fdDdistDkExt16Sin(body, system, iBody, jBody, 3)*fdSinLambda(body,system,iBody,jBody,3,1,iRes)));

  //------dp----------------first order--------
  printf("dR11dp = %e\n",(fdDdistDpDir11Cos(body, system, iBody, jBody, iIndexJ)*fdCosLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)+fdDdistDpDir11Sin(body, system, iBody, jBody, iIndexJ)*fdSinLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)));
  printf("dR12dp = %e\n",(fdDdistDpDir12Cos(body, system, iBody, jBody, iIndexJ)*fdCosLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)+fdDdistDpDir12Sin(body, system, iBody, jBody, iIndexJ)*fdSinLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)));
//   printf("dR13dp = %e\n",(fdDdistDpDir13Cos(body, system, iBody, jBody, iIndexJ)*fdCosLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)+fdDdistDpDir13Sin(body, system, iBody, jBody, iIndexJ)*fdSinLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)));
//   printf("dR14dp = %e\n",(fdDdistDpDir14Cos(body, system, iBody, jBody, iIndexJ)*fdCosLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)+fdDdistDpDir14Sin(body, system, iBody, jBody, iIndexJ)*fdSinLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)));
  printf("dR15dp = %e\n",(fdDdistDpDir15Cos(body, system, iBody, jBody, iIndexJ)*fdCosLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)+fdDdistDpDir15Sin(body, system, iBody, jBody, iIndexJ)*fdSinLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)));
  printf("dR16dp = %e\n",(fdDdistDpDir16Cos(body, system, iBody, jBody, iIndexJ)*fdCosLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)+fdDdistDpDir16Sin(body, system, iBody, jBody, iIndexJ)*fdSinLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)));
  printf("dR17dp = %e\n",(fdDdistDpDir17Cos(body, system, iBody, jBody, iIndexJ)*fdCosLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)+fdDdistDpDir17Sin(body, system, iBody, jBody, iIndexJ)*fdSinLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)));
  printf("dR18dp = %e\n",(fdDdistDpDir18Cos(body, system, iBody, jBody, iIndexJ)*fdCosLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)+fdDdistDpDir18Sin(body, system, iBody, jBody, iIndexJ)*fdSinLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)));
  printf("dR19dp = %e\n",(fdDdistDpDir19Cos(body, system, iBody, jBody, iIndexJ)*fdCosLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)+fdDdistDpDir19Sin(body, system, iBody, jBody, iIndexJ)*fdSinLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)));
  printf("dR110dp = %e\n",(fdDdistDpDir110Cos(body, system, iBody, jBody, iIndexJ)*fdCosLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)+fdDdistDpDir110Sin(body, system, iBody, jBody, iIndexJ)*fdSinLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)));
  printf("dR111dp = %e\n",(fdDdistDpDir111Cos(body, system, iBody, jBody, iIndexJ)*fdCosLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)+fdDdistDpDir111Sin(body, system, iBody, jBody, iIndexJ)*fdSinLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)));
  printf("dR112dp = %e\n",(fdDdistDpDir112Cos(body, system, iBody, jBody, iIndexJ)*fdCosLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)+fdDdistDpDir112Sin(body, system, iBody, jBody, iIndexJ)*fdSinLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)));
//   printf("dR113dp = %e\n",(fdDdistDpDir113Cos(body, system, iBody, jBody, iIndexJ)*fdCosLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)+fdDdistDpDir113Sin(body, system, iBody, jBody, iIndexJ)*fdSinLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)));
//   printf("dR114dp = %e\n",(fdDdistDpDir114Cos(body, system, iBody, jBody, iIndexJ)*fdCosLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)+fdDdistDpDir114Sin(body, system, iBody, jBody, iIndexJ)*fdSinLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)));
  printf("dRE13dp = %e\n",(fdDdistDpExt13Cos(body, system, iBody, jBody, 2)*fdCosLambda(body,system,iBody,jBody,2,1,iRes)+fdDdistDpExt13Sin(body, system, iBody, jBody, 2)*fdSinLambda(body,system,iBody,jBody,2,1,iRes)));
  printf("dRE111dp = %e\n",(fdDdistDpExt111Cos(body, system, iBody, jBody, 2)*fdCosLambda(body,system,iBody,jBody,2,1,iRes)+fdDdistDpExt111Sin(body, system, iBody, jBody, 2)*fdSinLambda(body,system,iBody,jBody,2,1,iRes)));

  //------dq----------------first order--------
  printf("dR11dq = %e\n",(fdDdistDqDir11Cos(body, system, iBody, jBody, iIndexJ)*fdCosLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)+fdDdistDqDir11Sin(body, system, iBody, jBody, iIndexJ)*fdSinLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)));
  printf("dR12dq = %e\n",(fdDdistDqDir12Cos(body, system, iBody, jBody, iIndexJ)*fdCosLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)+fdDdistDqDir12Sin(body, system, iBody, jBody, iIndexJ)*fdSinLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)));
//   printf("dR13dq = %e\n",(fdDdistDqDir13Cos(body, system, iBody, jBody, iIndexJ)*fdCosLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)+fdDdistDqDir13Sin(body, system, iBody, jBody, iIndexJ)*fdSinLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)));
//   printf("dR14dq = %e\n",(fdDdistDqDir14Cos(body, system, iBody, jBody, iIndexJ)*fdCosLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)+fdDdistDqDir14Sin(body, system, iBody, jBody, iIndexJ)*fdSinLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)));
  printf("dR15dq = %e\n",(fdDdistDqDir15Cos(body, system, iBody, jBody, iIndexJ)*fdCosLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)+fdDdistDqDir15Sin(body, system, iBody, jBody, iIndexJ)*fdSinLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)));
  printf("dR16dq = %e\n",(fdDdistDqDir16Cos(body, system, iBody, jBody, iIndexJ)*fdCosLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)+fdDdistDqDir16Sin(body, system, iBody, jBody, iIndexJ)*fdSinLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)));
  printf("dR17dq = %e\n",(fdDdistDqDir17Cos(body, system, iBody, jBody, iIndexJ)*fdCosLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)+fdDdistDqDir17Sin(body, system, iBody, jBody, iIndexJ)*fdSinLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)));
  printf("dR18dq = %e\n",(fdDdistDqDir18Cos(body, system, iBody, jBody, iIndexJ)*fdCosLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)+fdDdistDqDir18Sin(body, system, iBody, jBody, iIndexJ)*fdSinLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)));
  printf("dR19dq = %e\n",(fdDdistDqDir19Cos(body, system, iBody, jBody, iIndexJ)*fdCosLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)+fdDdistDqDir19Sin(body, system, iBody, jBody, iIndexJ)*fdSinLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)));
  printf("dR110dq = %e\n",(fdDdistDqDir110Cos(body, system, iBody, jBody, iIndexJ)*fdCosLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)+fdDdistDqDir110Sin(body, system, iBody, jBody, iIndexJ)*fdSinLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)));
  printf("dR111dq = %e\n",(fdDdistDqDir111Cos(body, system, iBody, jBody, iIndexJ)*fdCosLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)+fdDdistDqDir111Sin(body, system, iBody, jBody, iIndexJ)*fdSinLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)));
  printf("dR112dq = %e\n",(fdDdistDqDir112Cos(body, system, iBody, jBody, iIndexJ)*fdCosLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)+fdDdistDqDir112Sin(body, system, iBody, jBody, iIndexJ)*fdSinLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)));
//   printf("dR113dq = %e\n",(fdDdistDqDir113Cos(body, system, iBody, jBody, iIndexJ)*fdCosLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)+fdDdistDqDir113Sin(body, system, iBody, jBody, iIndexJ)*fdSinLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)));
//   printf("dR114dq = %e\n",(fdDdistDqDir114Cos(body, system, iBody, jBody, iIndexJ)*fdCosLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)+fdDdistDqDir114Sin(body, system, iBody, jBody, iIndexJ)*fdSinLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)));
  printf("dRE13dq = %e\n",(fdDdistDqExt13Cos(body, system, iBody, jBody, 2)*fdCosLambda(body,system,iBody,jBody,2,1,iRes)+fdDdistDqExt13Sin(body, system, iBody, jBody, 2)*fdSinLambda(body,system,iBody,jBody,2,1,iRes)));
  printf("dRE111dq = %e\n",(fdDdistDqExt111Cos(body, system, iBody, jBody, 2)*fdCosLambda(body,system,iBody,jBody,2,1,iRes)+fdDdistDqExt111Sin(body, system, iBody, jBody, 2)*fdSinLambda(body,system,iBody,jBody,2,1,iRes)));


  //------da----------------first order--------
  printf("dR11da = %e\n",(fdDdistDaDir11Cos(body, system, iBody, jBody, iIndexJ)*fdCosLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)+fdDdistDaDir11Sin(body, system, iBody, jBody, iIndexJ)*fdSinLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)));
  printf("dR12da = %e\n",(fdDdistDaDir12Cos(body, system, iBody, jBody, iIndexJ)*fdCosLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)+fdDdistDaDir12Sin(body, system, iBody, jBody, iIndexJ)*fdSinLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)));
  printf("dR13da = %e\n",(fdDdistDaDir13Cos(body, system, iBody, jBody, iIndexJ)*fdCosLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)+fdDdistDaDir13Sin(body, system, iBody, jBody, iIndexJ)*fdSinLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)));
  printf("dR14da = %e\n",(fdDdistDaDir14Cos(body, system, iBody, jBody, iIndexJ)*fdCosLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)+fdDdistDaDir14Sin(body, system, iBody, jBody, iIndexJ)*fdSinLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)));
  printf("dR15da = %e\n",(fdDdistDaDir15Cos(body, system, iBody, jBody, iIndexJ)*fdCosLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)+fdDdistDaDir15Sin(body, system, iBody, jBody, iIndexJ)*fdSinLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)));
  printf("dR16da = %e\n",(fdDdistDaDir16Cos(body, system, iBody, jBody, iIndexJ)*fdCosLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)+fdDdistDaDir16Sin(body, system, iBody, jBody, iIndexJ)*fdSinLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)));
  printf("dR17da = %e\n",(fdDdistDaDir17Cos(body, system, iBody, jBody, iIndexJ)*fdCosLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)+fdDdistDaDir17Sin(body, system, iBody, jBody, iIndexJ)*fdSinLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)));
  printf("dR18da = %e\n",(fdDdistDaDir18Cos(body, system, iBody, jBody, iIndexJ)*fdCosLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)+fdDdistDaDir18Sin(body, system, iBody, jBody, iIndexJ)*fdSinLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)));
  printf("dR19da = %e\n",(fdDdistDaDir19Cos(body, system, iBody, jBody, iIndexJ)*fdCosLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)+fdDdistDaDir19Sin(body, system, iBody, jBody, iIndexJ)*fdSinLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)));
  printf("dR110da = %e\n",(fdDdistDaDir110Cos(body, system, iBody, jBody, iIndexJ)*fdCosLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)+fdDdistDaDir110Sin(body, system, iBody, jBody, iIndexJ)*fdSinLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)));
  printf("dR111da = %e\n",(fdDdistDaDir111Cos(body, system, iBody, jBody, iIndexJ)*fdCosLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)+fdDdistDaDir111Sin(body, system, iBody, jBody, iIndexJ)*fdSinLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)));
  printf("dR112da = %e\n",(fdDdistDaDir112Cos(body, system, iBody, jBody, iIndexJ)*fdCosLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)+fdDdistDaDir112Sin(body, system, iBody, jBody, iIndexJ)*fdSinLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)));
  printf("dR113da = %e\n",(fdDdistDaDir113Cos(body, system, iBody, jBody, iIndexJ)*fdCosLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)+fdDdistDaDir113Sin(body, system, iBody, jBody, iIndexJ)*fdSinLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)));
  printf("dR114da = %e\n",(fdDdistDaDir114Cos(body, system, iBody, jBody, iIndexJ)*fdCosLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)+fdDdistDaDir114Sin(body, system, iBody, jBody, iIndexJ)*fdSinLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)));
  printf("dRE13da = %e\n",(fdDdistDaExt13Cos(body, system, iBody, jBody, 2)*fdCosLambda(body,system,iBody,jBody,2,1,iRes)+fdDdistDaExt13Sin(body, system, iBody, jBody, 2)*fdSinLambda(body,system,iBody,jBody,2,1,iRes)));
  printf("dRE16da = %e\n",(fdDdistDaExt16Cos(body, system, iBody, jBody, 3)*fdCosLambda(body,system,iBody,jBody,3,1,iRes)+fdDdistDaExt16Sin(body, system, iBody, jBody, 3)*fdSinLambda(body,system,iBody,jBody,3,1,iRes)));
  printf("dRE111da = %e\n",(fdDdistDaExt111Cos(body, system, iBody, jBody, 2)*fdCosLambda(body,system,iBody,jBody,2,1,iRes)+fdDdistDaExt111Sin(body, system, iBody, jBody, 2)*fdSinLambda(body,system,iBody,jBody,2,1,iRes)));

  //------dl----------------first order--------
  printf("dR11dl = %e\n",(fdDdistDlDir11Cos(body, system, iBody, jBody, iIndexJ)*fdCosLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)+fdDdistDlDir11Sin(body, system, iBody, jBody, iIndexJ)*fdSinLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)));
  printf("dR12dl = %e\n",(fdDdistDlDir12Cos(body, system, iBody, jBody, iIndexJ)*fdCosLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)+fdDdistDlDir12Sin(body, system, iBody, jBody, iIndexJ)*fdSinLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)));
  printf("dR13dl = %e\n",(fdDdistDlDir13Cos(body, system, iBody, jBody, iIndexJ)*fdCosLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)+fdDdistDlDir13Sin(body, system, iBody, jBody, iIndexJ)*fdSinLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)));
  printf("dR14dl = %e\n",(fdDdistDlDir14Cos(body, system, iBody, jBody, iIndexJ)*fdCosLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)+fdDdistDlDir14Sin(body, system, iBody, jBody, iIndexJ)*fdSinLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)));
  printf("dR15dl = %e\n",(fdDdistDlDir15Cos(body, system, iBody, jBody, iIndexJ)*fdCosLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)+fdDdistDlDir15Sin(body, system, iBody, jBody, iIndexJ)*fdSinLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)));
  printf("dR16dl = %e\n",(fdDdistDlDir16Cos(body, system, iBody, jBody, iIndexJ)*fdCosLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)+fdDdistDlDir16Sin(body, system, iBody, jBody, iIndexJ)*fdSinLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)));
  printf("dR17dl = %e\n",(fdDdistDlDir17Cos(body, system, iBody, jBody, iIndexJ)*fdCosLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)+fdDdistDlDir17Sin(body, system, iBody, jBody, iIndexJ)*fdSinLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)));
  printf("dR18dl = %e\n",(fdDdistDlDir18Cos(body, system, iBody, jBody, iIndexJ)*fdCosLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)+fdDdistDlDir18Sin(body, system, iBody, jBody, iIndexJ)*fdSinLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)));
  printf("dR19dl = %e\n",(fdDdistDlDir19Cos(body, system, iBody, jBody, iIndexJ)*fdCosLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)+fdDdistDlDir19Sin(body, system, iBody, jBody, iIndexJ)*fdSinLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)));
  printf("dR110dl = %e\n",(fdDdistDlDir110Cos(body, system, iBody, jBody, iIndexJ)*fdCosLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)+fdDdistDlDir110Sin(body, system, iBody, jBody, iIndexJ)*fdSinLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)));
  printf("dR111dl = %e\n",(fdDdistDlDir111Cos(body, system, iBody, jBody, iIndexJ)*fdCosLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)+fdDdistDlDir111Sin(body, system, iBody, jBody, iIndexJ)*fdSinLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)));
  printf("dR112dl = %e\n",(fdDdistDlDir112Cos(body, system, iBody, jBody, iIndexJ)*fdCosLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)+fdDdistDlDir112Sin(body, system, iBody, jBody, iIndexJ)*fdSinLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)));
  printf("dR113dl = %e\n",(fdDdistDlDir113Cos(body, system, iBody, jBody, iIndexJ)*fdCosLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)+fdDdistDlDir113Sin(body, system, iBody, jBody, iIndexJ)*fdSinLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)));
  printf("dR114dl = %e\n",(fdDdistDlDir114Cos(body, system, iBody, jBody, iIndexJ)*fdCosLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)+fdDdistDlDir114Sin(body, system, iBody, jBody, iIndexJ)*fdSinLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)));
  printf("dRE13dl = %e\n",(fdDdistDlExt13Cos(body, system, iBody, jBody, 2)*fdCosLambda(body,system,iBody,jBody,2,1,iRes)+fdDdistDlExt13Sin(body, system, iBody, jBody, 2)*fdSinLambda(body,system,iBody,jBody,2,1,iRes)));
  printf("dRE16dl = %e\n",(fdDdistDlExt16Cos(body, system, iBody, jBody, 3)*fdCosLambda(body,system,iBody,jBody,3,1,iRes)+fdDdistDlExt16Sin(body, system, iBody, jBody, 3)*fdSinLambda(body,system,iBody,jBody,3,1,iRes)));
  printf("dRE111dl = %e\n",(fdDdistDlExt111Cos(body, system, iBody, jBody, 2)*fdCosLambda(body,system,iBody,jBody,2,1,iRes)+fdDdistDlExt111Sin(body, system, iBody, jBody, 2)*fdSinLambda(body,system,iBody,jBody,2,1,iRes)));

  //------dhPrm----------------first order--------
  printf("dR11dhPrm = %e\n",(fdDdistDhPrmDir11Cos(body, system, jBody, iBody, iIndexJ)*fdCosLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)+fdDdistDhPrmDir11Sin(body, system, jBody, iBody, iIndexJ)*fdSinLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)));
  printf("dR12dhPrm = %e\n",(fdDdistDhPrmDir12Cos(body, system, jBody, iBody, iIndexJ)*fdCosLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)+fdDdistDhPrmDir12Sin(body, system, jBody, iBody, iIndexJ)*fdSinLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)));
  printf("dR13dhPrm = %e\n",(fdDdistDhPrmDir13Cos(body, system, jBody, iBody, iIndexJ)*fdCosLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)+fdDdistDhPrmDir13Sin(body, system, jBody, iBody, iIndexJ)*fdSinLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)));
  printf("dR14dhPrm = %e\n",(fdDdistDhPrmDir14Cos(body, system, jBody, iBody, iIndexJ)*fdCosLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)+fdDdistDhPrmDir14Sin(body, system, jBody, iBody, iIndexJ)*fdSinLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)));
//   printf("dR15dhPrm = %e\n",(fdDdistDhPrmDir15Cos(body, system, jBody, iBody, iIndexJ)*fdCosLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)+fdDdistDhPrmDir15Sin(body, system, jBody, iBody, iIndexJ)*fdSinLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)));
  printf("dR16dhPrm = %e\n",(fdDdistDhPrmDir16Cos(body, system, jBody, iBody, iIndexJ)*fdCosLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)+fdDdistDhPrmDir16Sin(body, system, jBody, iBody, iIndexJ)*fdSinLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)));
//   printf("dR17dhPrm = %e\n",(fdDdistDhPrmDir17Cos(body, system, jBody, iBody, iIndexJ)*fdCosLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)+fdDdistDhPrmDir17Sin(body, system, jBody, iBody, iIndexJ)*fdSinLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)));
//   printf("dR18dhPrm = %e\n",(fdDdistDhPrmDir18Cos(body, system, jBody, iBody, iIndexJ)*fdCosLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)+fdDdistDhPrmDir18Sin(body, system, jBody, iBody, iIndexJ)*fdSinLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)));
//   printf("dR19dhPrm = %e\n",(fdDdistDhPrmDir19Cos(body, system, jBody, iBody, iIndexJ)*fdCosLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)+fdDdistDhPrmDir19Sin(body, system, jBody, iBody, iIndexJ)*fdSinLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)));
  printf("dR110dhPrm = %e\n",(fdDdistDhPrmDir110Cos(body, system, jBody, iBody, iIndexJ)*fdCosLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)+fdDdistDhPrmDir110Sin(body, system, jBody, iBody, iIndexJ)*fdSinLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)));
  printf("dR111dhPrm = %e\n",(fdDdistDhPrmDir111Cos(body, system, jBody, iBody, iIndexJ)*fdCosLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)+fdDdistDhPrmDir111Sin(body, system, jBody, iBody, iIndexJ)*fdSinLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)));
  printf("dR112dhPrm = %e\n",(fdDdistDhPrmDir112Cos(body, system, jBody, iBody, iIndexJ)*fdCosLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)+fdDdistDhPrmDir112Sin(body, system, jBody, iBody, iIndexJ)*fdSinLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)));
//   printf("dR113dhPrm = %e\n",(fdDdistDhPrmDir113Cos(body, system, jBody, iBody, iIndexJ)*fdCosLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)+fdDdistDhPrmDir113Sin(body, system, jBody, iBody, iIndexJ)*fdSinLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)));
  printf("dR114dhPrm = %e\n",(fdDdistDhPrmDir114Cos(body, system, jBody, iBody, iIndexJ)*fdCosLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)+fdDdistDhPrmDir114Sin(body, system, jBody, iBody, iIndexJ)*fdSinLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)));
  printf("dRI13dhPrm = %e\n",(fdDdistDhPrmInt13Cos(body, system, jBody, iBody, 2)*fdCosLambda(body,system,iBody,jBody,2,1,iRes)+fdDdistDhPrmInt13Sin(body, system, jBody, iBody, 2)*fdSinLambda(body,system,iBody,jBody,2,1,iRes)));
  printf("dRI16dhPrm = %e\n",(fdDdistDhPrmInt16Cos(body, system, jBody, iBody, 3)*fdCosLambda(body,system,iBody,jBody,3,1,iRes)+fdDdistDhPrmInt16Sin(body, system, jBody, iBody, 3)*fdSinLambda(body,system,iBody,jBody,3,1,iRes)));
  printf("dRI111dhPrm = %e\n",(fdDdistDhPrmInt111Cos(body, system, jBody, iBody, 2)*fdCosLambda(body,system,iBody,jBody,2,1,iRes)+fdDdistDhPrmInt111Sin(body, system, jBody, iBody, 2)*fdSinLambda(body,system,iBody,jBody,2,1,iRes)));

  //------dkPrm----------------first order--------
  printf("dR11dkPrm = %e\n",(fdDdistDkPrmDir11Cos(body, system, jBody, iBody, iIndexJ)*fdCosLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)+fdDdistDkPrmDir11Sin(body, system, jBody, iBody, iIndexJ)*fdSinLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)));
  printf("dR12dkPrm = %e\n",(fdDdistDkPrmDir12Cos(body, system, jBody, iBody, iIndexJ)*fdCosLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)+fdDdistDkPrmDir12Sin(body, system, jBody, iBody, iIndexJ)*fdSinLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)));
  printf("dR13dkPrm = %e\n",(fdDdistDkPrmDir13Cos(body, system, jBody, iBody, iIndexJ)*fdCosLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)+fdDdistDkPrmDir13Sin(body, system, jBody, iBody, iIndexJ)*fdSinLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)));
  printf("dR14dkPrm = %e\n",(fdDdistDkPrmDir14Cos(body, system, jBody, iBody, iIndexJ)*fdCosLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)+fdDdistDkPrmDir14Sin(body, system, jBody, iBody, iIndexJ)*fdSinLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)));
//   printf("dR15dkPrm = %e\n",(fdDdistDkPrmDir15Cos(body, system, jBody, iBody, iIndexJ)*fdCosLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)+fdDdistDkPrmDir15Sin(body, system, jBody, iBody, iIndexJ)*fdSinLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)));
  printf("dR16dkPrm = %e\n",(fdDdistDkPrmDir16Cos(body, system, jBody, iBody, iIndexJ)*fdCosLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)+fdDdistDkPrmDir16Sin(body, system, jBody, iBody, iIndexJ)*fdSinLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)));
//   printf("dR17dkPrm = %e\n",(fdDdistDkPrmDir17Cos(body, system, jBody, iBody, iIndexJ)*fdCosLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)+fdDdistDkPrmDir17Sin(body, system, jBody, iBody, iIndexJ)*fdSinLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)));
//   printf("dR18dkPrm = %e\n",(fdDdistDkPrmDir18Cos(body, system, jBody, iBody, iIndexJ)*fdCosLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)+fdDdistDkPrmDir18Sin(body, system, jBody, iBody, iIndexJ)*fdSinLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)));
//   printf("dR19dkPrm = %e\n",(fdDdistDkPrmDir19Cos(body, system, jBody, iBody, iIndexJ)*fdCosLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)+fdDdistDkPrmDir19Sin(body, system, jBody, iBody, iIndexJ)*fdSinLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)));
  printf("dR110dkPrm = %e\n",(fdDdistDkPrmDir110Cos(body, system, jBody, iBody, iIndexJ)*fdCosLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)+fdDdistDkPrmDir110Sin(body, system, jBody, iBody, iIndexJ)*fdSinLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)));
  printf("dR111dkPrm = %e\n",(fdDdistDkPrmDir111Cos(body, system, jBody, iBody, iIndexJ)*fdCosLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)+fdDdistDkPrmDir111Sin(body, system, jBody, iBody, iIndexJ)*fdSinLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)));
  printf("dR112dkPrm = %e\n",(fdDdistDkPrmDir112Cos(body, system, jBody, iBody, iIndexJ)*fdCosLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)+fdDdistDkPrmDir112Sin(body, system, jBody, iBody, iIndexJ)*fdSinLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)));
//   printf("dR113dkPrm = %e\n",(fdDdistDkPrmDir113Cos(body, system, jBody, iBody, iIndexJ)*fdCosLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)+fdDdistDkPrmDir113Sin(body, system, jBody, iBody, iIndexJ)*fdSinLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)));
  printf("dR114dkPrm = %e\n",(fdDdistDkPrmDir114Cos(body, system, jBody, iBody, iIndexJ)*fdCosLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)+fdDdistDkPrmDir114Sin(body, system, jBody, iBody, iIndexJ)*fdSinLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)));
  printf("dRI13dkPrm = %e\n",(fdDdistDkPrmInt13Cos(body, system, jBody, iBody, 2)*fdCosLambda(body,system,iBody,jBody,2,1,iRes)+fdDdistDkPrmInt13Sin(body, system, jBody, iBody, 2)*fdSinLambda(body,system,iBody,jBody,2,1,iRes)));
  printf("dRI16dkPrm = %e\n",(fdDdistDkPrmInt16Cos(body, system, jBody, iBody, 3)*fdCosLambda(body,system,iBody,jBody,3,1,iRes)+fdDdistDkPrmInt16Sin(body, system, jBody, iBody, 3)*fdSinLambda(body,system,iBody,jBody,3,1,iRes)));
  printf("dRI111dkPrm = %e\n",(fdDdistDkPrmInt111Cos(body, system, jBody, iBody, 2)*fdCosLambda(body,system,iBody,jBody,2,1,iRes)+fdDdistDkPrmInt111Sin(body, system, jBody, iBody, 2)*fdSinLambda(body,system,iBody,jBody,2,1,iRes)));


  //------dpPrm----------------first order--------
  printf("dR11dpPrm = %e\n",(fdDdistDpPrmDir11Cos(body, system, jBody, iBody, iIndexJ)*fdCosLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)+fdDdistDpPrmDir11Sin(body, system, jBody, iBody, iIndexJ)*fdSinLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)));
  printf("dR12dpPrm = %e\n",(fdDdistDpPrmDir12Cos(body, system, jBody, iBody, iIndexJ)*fdCosLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)+fdDdistDpPrmDir12Sin(body, system, jBody, iBody, iIndexJ)*fdSinLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)));
//   printf("dR13dpPrm = %e\n",(fdDdistDpPrmDir13Cos(body, system, jBody, iBody, iIndexJ)*fdCosLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)+fdDdistDpPrmDir13Sin(body, system, jBody, iBody, iIndexJ)*fdSinLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)));
//   printf("dR14dpPrm = %e\n",(fdDdistDpPrmDir14Cos(body, system, jBody, iBody, iIndexJ)*fdCosLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)+fdDdistDpPrmDir14Sin(body, system, jBody, iBody, iIndexJ)*fdSinLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)));
//   printf("dR15dpPrm = %e\n",(fdDdistDpPrmDir15Cos(body, system, jBody, iBody, iIndexJ)*fdCosLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)+fdDdistDpPrmDir15Sin(body, system, jBody, iBody, iIndexJ)*fdSinLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)));
//   printf("dR16dpPrm = %e\n",(fdDdistDpPrmDir16Cos(body, system, jBody, iBody, iIndexJ)*fdCosLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)+fdDdistDpPrmDir16Sin(body, system, jBody, iBody, iIndexJ)*fdSinLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)));
  printf("dR17dpPrm = %e\n",(fdDdistDpPrmDir17Cos(body, system, jBody, iBody, iIndexJ)*fdCosLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)+fdDdistDpPrmDir17Sin(body, system, jBody, iBody, iIndexJ)*fdSinLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)));
  printf("dR18dpPrm = %e\n",(fdDdistDpPrmDir18Cos(body, system, jBody, iBody, iIndexJ)*fdCosLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)+fdDdistDpPrmDir18Sin(body, system, jBody, iBody, iIndexJ)*fdSinLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)));
  printf("dR19dpPrm = %e\n",(fdDdistDpPrmDir19Cos(body, system, jBody, iBody, iIndexJ)*fdCosLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)+fdDdistDpPrmDir19Sin(body, system, jBody, iBody, iIndexJ)*fdSinLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)));
  printf("dR110dpPrm = %e\n",(fdDdistDpPrmDir110Cos(body, system, jBody, iBody, iIndexJ)*fdCosLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)+fdDdistDpPrmDir110Sin(body, system, jBody, iBody, iIndexJ)*fdSinLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)));
  printf("dR111dpPrm = %e\n",(fdDdistDpPrmDir111Cos(body, system, jBody, iBody, iIndexJ)*fdCosLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)+fdDdistDpPrmDir111Sin(body, system, jBody, iBody, iIndexJ)*fdSinLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)));
  printf("dR112dpPrm = %e\n",(fdDdistDpPrmDir112Cos(body, system, jBody, iBody, iIndexJ)*fdCosLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)+fdDdistDpPrmDir112Sin(body, system, jBody, iBody, iIndexJ)*fdSinLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)));
  printf("dR113dpPrm = %e\n",(fdDdistDpPrmDir113Cos(body, system, jBody, iBody, iIndexJ)*fdCosLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)+fdDdistDpPrmDir113Sin(body, system, jBody, iBody, iIndexJ)*fdSinLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)));
  printf("dR114dpPrm = %e\n",(fdDdistDpPrmDir114Cos(body, system, jBody, iBody, iIndexJ)*fdCosLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)+fdDdistDpPrmDir114Sin(body, system, jBody, iBody, iIndexJ)*fdSinLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)));
  printf("dRI13dpPrm = %e\n",(fdDdistDpPrmInt13Cos(body, system, jBody, iBody, 2)*fdCosLambda(body,system,iBody,jBody,2,1,iRes)+fdDdistDpPrmInt13Sin(body, system, jBody, iBody, 2)*fdSinLambda(body,system,iBody,jBody,2,1,iRes)));
  printf("dRI111dpPrm = %e\n",(fdDdistDpPrmInt111Cos(body, system, jBody, iBody, 2)*fdCosLambda(body,system,iBody,jBody,2,1,iRes)+fdDdistDpPrmInt111Sin(body, system, jBody, iBody, 2)*fdSinLambda(body,system,iBody,jBody,2,1,iRes)));


  //------dqPrm----------------first order--------
  printf("dR11dqPrm = %e\n",(fdDdistDqPrmDir11Cos(body, system, jBody, iBody, iIndexJ)*fdCosLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)+fdDdistDqPrmDir11Sin(body, system, jBody, iBody, iIndexJ)*fdSinLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)));
  printf("dR12dqPrm = %e\n",(fdDdistDqPrmDir12Cos(body, system, jBody, iBody, iIndexJ)*fdCosLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)+fdDdistDqPrmDir12Sin(body, system, jBody, iBody, iIndexJ)*fdSinLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)));
//   printf("dR13dqPrm = %e\n",(fdDdistDqPrmDir13Cos(body, system, jBody, iBody, iIndexJ)*fdCosLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)+fdDdistDqPrmDir13Sin(body, system, jBody, iBody, iIndexJ)*fdSinLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)));
//   printf("dR14dqPrm = %e\n",(fdDdistDqPrmDir14Cos(body, system, jBody, iBody, iIndexJ)*fdCosLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)+fdDdistDqPrmDir14Sin(body, system, jBody, iBody, iIndexJ)*fdSinLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)));
//   printf("dR15dqPrm = %e\n",(fdDdistDqPrmDir15Cos(body, system, jBody, iBody, iIndexJ)*fdCosLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)+fdDdistDqPrmDir15Sin(body, system, jBody, iBody, iIndexJ)*fdSinLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)));
//   printf("dR16dqPrm = %e\n",(fdDdistDqPrmDir16Cos(body, system, jBody, iBody, iIndexJ)*fdCosLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)+fdDdistDqPrmDir16Sin(body, system, jBody, iBody, iIndexJ)*fdSinLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)));
  printf("dR17dqPrm = %e\n",(fdDdistDqPrmDir17Cos(body, system, jBody, iBody, iIndexJ)*fdCosLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)+fdDdistDqPrmDir17Sin(body, system, jBody, iBody, iIndexJ)*fdSinLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)));
  printf("dR18dqPrm = %e\n",(fdDdistDqPrmDir18Cos(body, system, jBody, iBody, iIndexJ)*fdCosLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)+fdDdistDqPrmDir18Sin(body, system, jBody, iBody, iIndexJ)*fdSinLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)));
  printf("dR19dqPrm = %e\n",(fdDdistDqPrmDir19Cos(body, system, jBody, iBody, iIndexJ)*fdCosLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)+fdDdistDqPrmDir19Sin(body, system, jBody, iBody, iIndexJ)*fdSinLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)));
  printf("dR110dqPrm = %e\n",(fdDdistDqPrmDir110Cos(body, system, jBody, iBody, iIndexJ)*fdCosLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)+fdDdistDqPrmDir110Sin(body, system, jBody, iBody, iIndexJ)*fdSinLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)));
  printf("dR111dqPrm = %e\n",(fdDdistDqPrmDir111Cos(body, system, jBody, iBody, iIndexJ)*fdCosLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)+fdDdistDqPrmDir111Sin(body, system, jBody, iBody, iIndexJ)*fdSinLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)));
  printf("dR112dqPrm = %e\n",(fdDdistDqPrmDir112Cos(body, system, jBody, iBody, iIndexJ)*fdCosLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)+fdDdistDqPrmDir112Sin(body, system, jBody, iBody, iIndexJ)*fdSinLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)));
  printf("dR113dqPrm = %e\n",(fdDdistDqPrmDir113Cos(body, system, jBody, iBody, iIndexJ)*fdCosLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)+fdDdistDqPrmDir113Sin(body, system, jBody, iBody, iIndexJ)*fdSinLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)));
  printf("dR114dqPrm = %e\n",(fdDdistDqPrmDir114Cos(body, system, jBody, iBody, iIndexJ)*fdCosLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)+fdDdistDqPrmDir114Sin(body, system, jBody, iBody, iIndexJ)*fdSinLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)));
   printf("dRI13dqPrm = %e\n",(fdDdistDqPrmInt13Cos(body, system, jBody, iBody, 2)*fdCosLambda(body,system,iBody,jBody,2,1,iRes)+fdDdistDqPrmInt13Sin(body, system, jBody, iBody, 2)*fdSinLambda(body,system,iBody,jBody,2,1,iRes)));
  printf("dRI111dqPrm = %e\n",(fdDdistDqPrmInt111Cos(body, system, jBody, iBody, 2)*fdCosLambda(body,system,iBody,jBody,2,1,iRes)+fdDdistDqPrmInt111Sin(body, system, jBody, iBody, 2)*fdSinLambda(body,system,iBody,jBody,2,1,iRes)));


  //------daPrm----------------first order--------
  printf("dR11daPrm = %e\n",(fdDdistDaPrmDir11Cos(body, system, jBody, iBody, iIndexJ)*fdCosLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)+fdDdistDaPrmDir11Sin(body, system, jBody, iBody, iIndexJ)*fdSinLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)));
  printf("dR12daPrm = %e\n",(fdDdistDaPrmDir12Cos(body, system, jBody, iBody, iIndexJ)*fdCosLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)+fdDdistDaPrmDir12Sin(body, system, jBody, iBody, iIndexJ)*fdSinLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)));
  printf("dR13daPrm = %e\n",(fdDdistDaPrmDir13Cos(body, system, jBody, iBody, iIndexJ)*fdCosLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)+fdDdistDaPrmDir13Sin(body, system, jBody, iBody, iIndexJ)*fdSinLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)));
  printf("dR14daPrm = %e\n",(fdDdistDaPrmDir14Cos(body, system, jBody, iBody, iIndexJ)*fdCosLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)+fdDdistDaPrmDir14Sin(body, system, jBody, iBody, iIndexJ)*fdSinLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)));
  printf("dR15daPrm = %e\n",(fdDdistDaPrmDir15Cos(body, system, jBody, iBody, iIndexJ)*fdCosLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)+fdDdistDaPrmDir15Sin(body, system, jBody, iBody, iIndexJ)*fdSinLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)));
  printf("dR16daPrm = %e\n",(fdDdistDaPrmDir16Cos(body, system, jBody, iBody, iIndexJ)*fdCosLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)+fdDdistDaPrmDir16Sin(body, system, jBody, iBody, iIndexJ)*fdSinLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)));
  printf("dR17daPrm = %e\n",(fdDdistDaPrmDir17Cos(body, system, jBody, iBody, iIndexJ)*fdCosLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)+fdDdistDaPrmDir17Sin(body, system, jBody, iBody, iIndexJ)*fdSinLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)));
  printf("dR18daPrm = %e\n",(fdDdistDaPrmDir18Cos(body, system, jBody, iBody, iIndexJ)*fdCosLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)+fdDdistDaPrmDir18Sin(body, system, jBody, iBody, iIndexJ)*fdSinLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)));
  printf("dR19daPrm = %e\n",(fdDdistDaPrmDir19Cos(body, system, jBody, iBody, iIndexJ)*fdCosLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)+fdDdistDaPrmDir19Sin(body, system, jBody, iBody, iIndexJ)*fdSinLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)));
  printf("dR110daPrm = %e\n",(fdDdistDaPrmDir110Cos(body, system, jBody, iBody, iIndexJ)*fdCosLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)+fdDdistDaPrmDir110Sin(body, system, jBody, iBody, iIndexJ)*fdSinLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)));
  printf("dR111daPrm = %e\n",(fdDdistDaPrmDir111Cos(body, system, jBody, iBody, iIndexJ)*fdCosLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)+fdDdistDaPrmDir111Sin(body, system, jBody, iBody, iIndexJ)*fdSinLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)));
  printf("dR112daPrm = %e\n",(fdDdistDaPrmDir112Cos(body, system, jBody, iBody, iIndexJ)*fdCosLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)+fdDdistDaPrmDir112Sin(body, system, jBody, iBody, iIndexJ)*fdSinLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)));
  printf("dR113daPrm = %e\n",(fdDdistDaPrmDir113Cos(body, system, jBody, iBody, iIndexJ)*fdCosLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)+fdDdistDaPrmDir113Sin(body, system, jBody, iBody, iIndexJ)*fdSinLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)));
  printf("dR114daPrm = %e\n",(fdDdistDaPrmDir114Cos(body, system, jBody, iBody, iIndexJ)*fdCosLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)+fdDdistDaPrmDir114Sin(body, system, jBody, iBody, iIndexJ)*fdSinLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)));
  printf("dRI13daPrm = %e\n",(fdDdistDaPrmInt13Cos(body, system, jBody, iBody, 2)*fdCosLambda(body,system,iBody,jBody,2,1,iRes)+fdDdistDaPrmInt13Sin(body, system, jBody, iBody, 2)*fdSinLambda(body,system,iBody,jBody,2,1,iRes)));
  printf("dRI16daPrm = %e\n",(fdDdistDaPrmInt16Cos(body, system, jBody, iBody, 3)*fdCosLambda(body,system,iBody,jBody,3,1,iRes)+fdDdistDaPrmInt16Sin(body, system, jBody, iBody, 3)*fdSinLambda(body,system,iBody,jBody,3,1,iRes)));
  printf("dRI111daPrm = %e\n",(fdDdistDaPrmInt111Cos(body, system, jBody, iBody, 2)*fdCosLambda(body,system,iBody,jBody,2,1,iRes)+fdDdistDaPrmInt111Sin(body, system, jBody, iBody, 2)*fdSinLambda(body,system,iBody,jBody,2,1,iRes)));



  //------dlPrm----------------first order--------
  printf("dR11dlPrm = %e\n",(fdDdistDlPrmDir11Cos(body, system, jBody, iBody, iIndexJ)*fdCosLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)+fdDdistDlPrmDir11Sin(body, system, jBody, iBody, iIndexJ)*fdSinLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)));
  printf("dR12dlPrm = %e\n",(fdDdistDlPrmDir12Cos(body, system, jBody, iBody, iIndexJ)*fdCosLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)+fdDdistDlPrmDir12Sin(body, system, jBody, iBody, iIndexJ)*fdSinLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)));
  printf("dR13dlPrm = %e\n",(fdDdistDlPrmDir13Cos(body, system, jBody, iBody, iIndexJ)*fdCosLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)+fdDdistDlPrmDir13Sin(body, system, jBody, iBody, iIndexJ)*fdSinLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)));
  printf("dR14dlPrm = %e\n",(fdDdistDlPrmDir14Cos(body, system, jBody, iBody, iIndexJ)*fdCosLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)+fdDdistDlPrmDir14Sin(body, system, jBody, iBody, iIndexJ)*fdSinLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)));
  printf("dR15dlPrm = %e\n",(fdDdistDlPrmDir15Cos(body, system, jBody, iBody, iIndexJ)*fdCosLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)+fdDdistDlPrmDir15Sin(body, system, jBody, iBody, iIndexJ)*fdSinLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)));
  printf("dR16dlPrm = %e\n",(fdDdistDlPrmDir16Cos(body, system, jBody, iBody, iIndexJ)*fdCosLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)+fdDdistDlPrmDir16Sin(body, system, jBody, iBody, iIndexJ)*fdSinLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)));
  printf("dR17dlPrm = %e\n",(fdDdistDlPrmDir17Cos(body, system, jBody, iBody, iIndexJ)*fdCosLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)+fdDdistDlPrmDir17Sin(body, system, jBody, iBody, iIndexJ)*fdSinLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)));
  printf("dR18dlPrm = %e\n",(fdDdistDlPrmDir18Cos(body, system, jBody, iBody, iIndexJ)*fdCosLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)+fdDdistDlPrmDir18Sin(body, system, jBody, iBody, iIndexJ)*fdSinLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)));
  printf("dR19dlPrm = %e\n",(fdDdistDlPrmDir19Cos(body, system, jBody, iBody, iIndexJ)*fdCosLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)+fdDdistDlPrmDir19Sin(body, system, jBody, iBody, iIndexJ)*fdSinLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)));
  printf("dR110dlPrm = %e\n",(fdDdistDlPrmDir110Cos(body, system, jBody, iBody, iIndexJ)*fdCosLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)+fdDdistDlPrmDir110Sin(body, system, jBody, iBody, iIndexJ)*fdSinLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)));
  printf("dR111dlPrm = %e\n",(fdDdistDlPrmDir111Cos(body, system, jBody, iBody, iIndexJ)*fdCosLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)+fdDdistDlPrmDir111Sin(body, system, jBody, iBody, iIndexJ)*fdSinLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)));
  printf("dR112dlPrm = %e\n",(fdDdistDlPrmDir112Cos(body, system, jBody, iBody, iIndexJ)*fdCosLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)+fdDdistDlPrmDir112Sin(body, system, jBody, iBody, iIndexJ)*fdSinLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)));
  printf("dR113dlPrm = %e\n",(fdDdistDlPrmDir113Cos(body, system, jBody, iBody, iIndexJ)*fdCosLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)+fdDdistDlPrmDir113Sin(body, system, jBody, iBody, iIndexJ)*fdSinLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)));
  printf("dR114dlPrm = %e\n",(fdDdistDlPrmDir114Cos(body, system, jBody, iBody, iIndexJ)*fdCosLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)+fdDdistDlPrmDir114Sin(body, system, jBody, iBody, iIndexJ)*fdSinLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)));
  printf("dRI13dlPrm = %e\n",(fdDdistDlPrmInt13Cos(body, system, jBody, iBody, 2)*fdCosLambda(body,system,iBody,jBody,2,1,iRes)+fdDdistDlPrmInt13Sin(body, system, jBody, iBody, 2)*fdSinLambda(body,system,iBody,jBody,2,1,iRes)));
  printf("dRI16dlPrm = %e\n",(fdDdistDlPrmInt16Cos(body, system, jBody, iBody, 3)*fdCosLambda(body,system,iBody,jBody,3,1,iRes)+fdDdistDlPrmInt16Sin(body, system, jBody, iBody, 3)*fdSinLambda(body,system,iBody,jBody,3,1,iRes)));
  printf("dRI111dlPrm = %e\n",(fdDdistDlPrmInt111Cos(body, system, jBody, iBody, 2)*fdCosLambda(body,system,iBody,jBody,2,1,iRes)+fdDdistDlPrmInt111Sin(body, system, jBody, iBody, 2)*fdSinLambda(body,system,iBody,jBody,2,1,iRes)));


}


void CheckTermsRes2(BODY *body, SYSTEM *system, int iBody, int jBody, int iRes) {
  int iIndexJ = system->iaResIndex[iRes][system->iaLaplaceN[iBody][jBody]];
  int iOrder = system->iaResOrder[iRes];

  //------dh----------------2nd order--------
  printf("dR21dh = %e\n",(fdDdistDhDir21Cos(body, system, iBody, jBody, iIndexJ)*fdCosLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)+fdDdistDhDir21Sin(body, system, iBody, jBody, iIndexJ)*fdSinLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)));
  printf("dR22dh = %e\n",(fdDdistDhDir22Cos(body, system, iBody, jBody, iIndexJ)*fdCosLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)+fdDdistDhDir22Sin(body, system, iBody, jBody, iIndexJ)*fdSinLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)));

  //------dk----------------2nd order--------
  printf("dR21dk = %e\n",(fdDdistDkDir21Cos(body, system, iBody, jBody, iIndexJ)*fdCosLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)+fdDdistDkDir21Sin(body, system, iBody, jBody, iIndexJ)*fdSinLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)));
  printf("dR22dk = %e\n",(fdDdistDkDir22Cos(body, system, iBody, jBody, iIndexJ)*fdCosLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)+fdDdistDkDir22Sin(body, system, iBody, jBody, iIndexJ)*fdSinLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)));

  //------dp----------------2nd order--------
  printf("dR24dp = %e\n",(fdDdistDpDir24Cos(body, system, iBody, jBody, iIndexJ)*fdCosLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)+fdDdistDpDir24Sin(body, system, iBody, jBody, iIndexJ)*fdSinLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)));
  printf("dR25dp = %e\n",(fdDdistDpDir25Cos(body, system, iBody, jBody, iIndexJ)*fdCosLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)+fdDdistDpDir25Sin(body, system, iBody, jBody, iIndexJ)*fdSinLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)));

  //------dq----------------2nd order--------
  printf("dR24dq = %e\n",(fdDdistDqDir24Cos(body, system, iBody, jBody, iIndexJ)*fdCosLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)+fdDdistDqDir24Sin(body, system, iBody, jBody, iIndexJ)*fdSinLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)));
  printf("dR25dq = %e\n",(fdDdistDqDir25Cos(body, system, iBody, jBody, iIndexJ)*fdCosLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)+fdDdistDqDir25Sin(body, system, iBody, jBody, iIndexJ)*fdSinLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)));

  //------da----------------2nd order--------
  printf("dR21da = %e\n",(fdDdistDaDir21Cos(body, system, iBody, jBody, iIndexJ)*fdCosLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)+fdDdistDaDir21Sin(body, system, iBody, jBody, iIndexJ)*fdSinLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)));
  printf("dR22da = %e\n",(fdDdistDaDir22Cos(body, system, iBody, jBody, iIndexJ)*fdCosLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)+fdDdistDaDir22Sin(body, system, iBody, jBody, iIndexJ)*fdSinLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)));
  printf("dR23da = %e\n",(fdDdistDaDir23Cos(body, system, iBody, jBody, iIndexJ)*fdCosLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)+fdDdistDaDir23Sin(body, system, iBody, jBody, iIndexJ)*fdSinLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)));
  printf("dR24da = %e\n",(fdDdistDaDir24Cos(body, system, iBody, jBody, iIndexJ)*fdCosLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)+fdDdistDaDir24Sin(body, system, iBody, jBody, iIndexJ)*fdSinLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)));
  printf("dR25da = %e\n",(fdDdistDaDir25Cos(body, system, iBody, jBody, iIndexJ)*fdCosLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)+fdDdistDaDir25Sin(body, system, iBody, jBody, iIndexJ)*fdSinLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)));
  printf("dR26da = %e\n",(fdDdistDaDir26Cos(body, system, iBody, jBody, iIndexJ)*fdCosLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)+fdDdistDaDir26Sin(body, system, iBody, jBody, iIndexJ)*fdSinLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)));
  printf("dRE25da = %e\n",(fdDdistDaExt25Cos(body, system, iBody, jBody, 3)*fdCosLambda(body,system,iBody,jBody,3,2,iRes)+fdDdistDaExt25Sin(body, system, iBody, jBody, 3)*fdSinLambda(body,system,iBody,jBody,3,2,iRes)));

  //------dl----------------2nd order--------
  printf("dR21dl = %e\n",(fdDdistDlDir21Cos(body, system, iBody, jBody, iIndexJ)*fdCosLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)+fdDdistDlDir21Sin(body, system, iBody, jBody, iIndexJ)*fdSinLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)));
  printf("dR22dl = %e\n",(fdDdistDlDir22Cos(body, system, iBody, jBody, iIndexJ)*fdCosLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)+fdDdistDlDir22Sin(body, system, iBody, jBody, iIndexJ)*fdSinLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)));
  printf("dR23dl = %e\n",(fdDdistDlDir23Cos(body, system, iBody, jBody, iIndexJ)*fdCosLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)+fdDdistDlDir23Sin(body, system, iBody, jBody, iIndexJ)*fdSinLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)));
  printf("dR24dl = %e\n",(fdDdistDlDir24Cos(body, system, iBody, jBody, iIndexJ)*fdCosLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)+fdDdistDlDir24Sin(body, system, iBody, jBody, iIndexJ)*fdSinLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)));
  printf("dR25dl = %e\n",(fdDdistDlDir25Cos(body, system, iBody, jBody, iIndexJ)*fdCosLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)+fdDdistDlDir25Sin(body, system, iBody, jBody, iIndexJ)*fdSinLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)));
  printf("dR26dl = %e\n",(fdDdistDlDir26Cos(body, system, iBody, jBody, iIndexJ)*fdCosLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)+fdDdistDlDir26Sin(body, system, iBody, jBody, iIndexJ)*fdSinLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)));
  printf("dRE25dl = %e\n",(fdDdistDlExt25Cos(body, system, iBody, jBody, 3)*fdCosLambda(body,system,iBody,jBody,3,2,iRes)+fdDdistDlExt25Sin(body, system, iBody, jBody, 3)*fdSinLambda(body,system,iBody,jBody,3,2,iRes)));

  //------dhp----------------2nd order--------
  printf("dR22dhPrm = %e\n",(fdDdistDhPrmDir22Cos(body, system, jBody, iBody, iIndexJ)*fdCosLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)+fdDdistDhPrmDir22Sin(body, system, jBody, iBody, iIndexJ)*fdSinLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)));
  printf("dR23dhPrm = %e\n",(fdDdistDhPrmDir23Cos(body, system, jBody, iBody, iIndexJ)*fdCosLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)+fdDdistDhPrmDir23Sin(body, system, jBody, iBody, iIndexJ)*fdSinLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)));
  printf("dRI25dhPrm = %e\n",(fdDdistDhPrmInt25Cos(body, system, jBody, iBody, 3)*fdCosLambda(body,system,iBody,jBody,3,2,iRes)+fdDdistDhPrmInt25Sin(body, system, jBody, iBody, 3)*fdSinLambda(body,system,iBody,jBody,3,2,iRes)));

  //------dkp----------------2nd order--------
  printf("dR22dkPrm = %e\n",(fdDdistDkPrmDir22Cos(body, system, jBody, iBody, iIndexJ)*fdCosLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)+fdDdistDkPrmDir22Sin(body, system, jBody, iBody, iIndexJ)*fdSinLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)));
  printf("dR23dkPrm = %e\n",(fdDdistDkPrmDir23Cos(body, system, jBody, iBody, iIndexJ)*fdCosLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)+fdDdistDkPrmDir23Sin(body, system, jBody, iBody, iIndexJ)*fdSinLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)));
  printf("dRI25dkPrm = %e\n",(fdDdistDkPrmInt25Cos(body, system, jBody, iBody, 3)*fdCosLambda(body,system,iBody,jBody,3,2,iRes)+fdDdistDkPrmInt25Sin(body, system, jBody, iBody, 3)*fdSinLambda(body,system,iBody,jBody,3,2,iRes)));

  //------dpp----------------2nd order--------
  printf("dR25dpPrm = %e\n",(fdDdistDpPrmDir25Cos(body, system, jBody, iBody, iIndexJ)*fdCosLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)+fdDdistDpPrmDir25Sin(body, system, jBody, iBody, iIndexJ)*fdSinLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)));
  printf("dR26dpPrm = %e\n",(fdDdistDpPrmDir26Cos(body, system, jBody, iBody, iIndexJ)*fdCosLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)+fdDdistDpPrmDir26Sin(body, system, jBody, iBody, iIndexJ)*fdSinLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)));

  //------dqp----------------2nd order--------
  printf("dR25dqPrm = %e\n",(fdDdistDqPrmDir25Cos(body, system, jBody, iBody, iIndexJ)*fdCosLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)+fdDdistDqPrmDir25Sin(body, system, jBody, iBody, iIndexJ)*fdSinLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)));
  printf("dR26dqPrm = %e\n",(fdDdistDqPrmDir26Cos(body, system, jBody, iBody, iIndexJ)*fdCosLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)+fdDdistDqPrmDir26Sin(body, system, jBody, iBody, iIndexJ)*fdSinLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)));

  //------dap----------------2nd order--------
  printf("dR21daPrm = %e\n",(fdDdistDaPrmDir21Cos(body, system, jBody, iBody, iIndexJ)*fdCosLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)+fdDdistDaPrmDir21Sin(body, system, jBody, iBody, iIndexJ)*fdSinLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)));
  printf("dR22daPrm = %e\n",(fdDdistDaPrmDir22Cos(body, system, jBody, iBody, iIndexJ)*fdCosLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)+fdDdistDaPrmDir22Sin(body, system, jBody, iBody, iIndexJ)*fdSinLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)));
  printf("dR23daPrm = %e\n",(fdDdistDaPrmDir23Cos(body, system, jBody, iBody, iIndexJ)*fdCosLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)+fdDdistDaPrmDir23Sin(body, system, jBody, iBody, iIndexJ)*fdSinLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)));
  printf("dR24daPrm = %e\n",(fdDdistDaPrmDir24Cos(body, system, jBody, iBody, iIndexJ)*fdCosLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)+fdDdistDaPrmDir24Sin(body, system, jBody, iBody, iIndexJ)*fdSinLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)));
  printf("dR25daPrm = %e\n",(fdDdistDaPrmDir25Cos(body, system, jBody, iBody, iIndexJ)*fdCosLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)+fdDdistDaPrmDir25Sin(body, system, jBody, iBody, iIndexJ)*fdSinLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)));
  printf("dR26daPrm = %e\n",(fdDdistDaPrmDir26Cos(body, system, jBody, iBody, iIndexJ)*fdCosLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)+fdDdistDaPrmDir26Sin(body, system, jBody, iBody, iIndexJ)*fdSinLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)));
  printf("dRI25daPrm = %e\n",(fdDdistDaPrmInt25Cos(body, system, jBody, iBody, 3)*fdCosLambda(body,system,iBody,jBody,3,2,iRes)+fdDdistDaPrmInt25Sin(body, system, jBody, iBody, 3)*fdSinLambda(body,system,iBody,jBody,3,2,iRes)));

  //------dlp----------------2nd order--------
  printf("dR21dlPrm = %e\n",(fdDdistDlPrmDir21Cos(body, system, jBody, iBody, iIndexJ)*fdCosLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)+fdDdistDlPrmDir21Sin(body, system, jBody, iBody, iIndexJ)*fdSinLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)));
  printf("dR22dlPrm = %e\n",(fdDdistDlPrmDir22Cos(body, system, jBody, iBody, iIndexJ)*fdCosLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)+fdDdistDlPrmDir22Sin(body, system, jBody, iBody, iIndexJ)*fdSinLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)));
  printf("dR23dlPrm = %e\n",(fdDdistDlPrmDir23Cos(body, system, jBody, iBody, iIndexJ)*fdCosLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)+fdDdistDlPrmDir23Sin(body, system, jBody, iBody, iIndexJ)*fdSinLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)));
  printf("dR24dlPrm = %e\n",(fdDdistDlPrmDir24Cos(body, system, jBody, iBody, iIndexJ)*fdCosLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)+fdDdistDlPrmDir24Sin(body, system, jBody, iBody, iIndexJ)*fdSinLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)));
  printf("dR25dlPrm = %e\n",(fdDdistDlPrmDir25Cos(body, system, jBody, iBody, iIndexJ)*fdCosLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)+fdDdistDlPrmDir25Sin(body, system, jBody, iBody, iIndexJ)*fdSinLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)));
  printf("dR26dlPrm = %e\n",(fdDdistDlPrmDir26Cos(body, system, jBody, iBody, iIndexJ)*fdCosLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)+fdDdistDlPrmDir26Sin(body, system, jBody, iBody, iIndexJ)*fdSinLambda(body,system,iBody,jBody,iIndexJ,iOrder,iRes)));
  printf("dRI25dlPrm = %e\n",(fdDdistDlPrmInt25Cos(body, system, jBody, iBody, 3)*fdCosLambda(body,system,iBody,jBody,3,2,iRes)+fdDdistDlPrmInt25Sin(body, system, jBody, iBody, 3)*fdSinLambda(body,system,iBody,jBody,3,2,iRes)));
}
