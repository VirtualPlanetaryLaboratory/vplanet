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
// 
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
//   
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
    system->fnLaplaceF[27][0] = &fdSemiMajAxF27;  // dummy
    system->fnLaplaceF[28][0] = &fdSemiMajAxF27;  // dummy
    system->fnLaplaceF[29][0] = &fdSemiMajAxF27;  // dummy
    system->fnLaplaceF[30][0] = &fdSemiMajAxF31;
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
    system->fnLaplaceDeriv[27][0] = &fdDSemiF27Dalpha; //dummy
    system->fnLaplaceDeriv[28][0] = &fdDSemiF27Dalpha; //dummy
    system->fnLaplaceDeriv[29][0] = &fdDSemiF27Dalpha; //dummy
    system->fnLaplaceDeriv[30][0] = &fdDSemiF31Dalpha;
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
    system->iResIndex = malloc(RESNUM*sizeof(int*));
    system->iResIndex[0] = malloc(Nchoosek(control->Evolve.iNumBodies-1,2)*sizeof(int));
    system->iResIndex[1] = malloc(Nchoosek(control->Evolve.iNumBodies-1,2)*sizeof(int));
    system->iResIndex[2] = malloc(Nchoosek(control->Evolve.iNumBodies-1,2)*sizeof(int));
    system->iResIndex[3] = malloc(Nchoosek(control->Evolve.iNumBodies-1,2)*sizeof(int));
    system->iResIndex[4] = malloc(Nchoosek(control->Evolve.iNumBodies-1,2)*sizeof(int));
    system->iResIndex[5] = malloc(Nchoosek(control->Evolve.iNumBodies-1,2)*sizeof(int));

    system->iResOrder = malloc(RESNUM*sizeof(int));
    system->iResOrder[0] = 1;
    system->iResOrder[1] = 1;    
    system->iResOrder[2] = 2;
    system->iResOrder[3] = 2;
    system->iResOrder[4] = 2;
    system->iResOrder[5] = 2;

    /*------------------------------*/
    
    for (iRes=1;iRes<=RESMAX;iRes++) {
      system->dmLaplaceC[iRes] = malloc(Nchoosek(control->Evolve.iNumBodies-1,2)*sizeof(double*));
      system->dmLaplaceD[iRes] = malloc(Nchoosek(control->Evolve.iNumBodies-1,2)*sizeof(double*));
      system->dmAlpha0[iRes] = malloc(Nchoosek(control->Evolve.iNumBodies-1,2)*sizeof(double*));
      for (i=0;i<Nchoosek(control->Evolve.iNumBodies-1,2);i++) {
        system->dmLaplaceC[iRes][i] = malloc(LAPLNUM*sizeof(double));
        system->dmLaplaceD[iRes][i] = malloc(LAPLNUM*sizeof(double));
        system->dmAlpha0[iRes][i] = malloc(LAPLNUM*sizeof(double));
      }
    }
    
  }
  
  if (iBody > 0) {
    for (iPert=0;iPert<body[iBody].iGravPerts;iPert++) {        
        /* semi major axis */
        InitializeSemiDistRes(body,update,iBody,iPert);
        fnUpdate[iBody][update[iBody].iSemi][update[iBody].iaSemiDistRes[iPert]] = &fdDistResRD2DaDt;
        
        /* mean longitude */
        InitializeMeanLDistRes(body,update,iBody,iPert);
        fnUpdate[iBody][update[iBody].iMeanL][update[iBody].iaMeanLDistRes[iPert]] = &fdDistResRD2DlDt;
        
        /* h = Ecc*sin(LongP) */
        InitializeHeccDistRes(body,update,iBody,iPert);
        fnUpdate[iBody][update[iBody].iHecc][update[iBody].iaHeccDistRes[iPert]] = &fdDistResRD2DhDt;
        
        /* k = Ecc*cos(LongP) */
        InitializeKeccDistRes(body,update,iBody,iPert);
        fnUpdate[iBody][update[iBody].iKecc][update[iBody].iaKeccDistRes[iPert]] = &fdDistResRD2DkDt;
        
        /* p = s*sin(LongA) */
        InitializePincDistRes(body,update,iBody,iPert);
        fnUpdate[iBody][update[iBody].iPinc][update[iBody].iaPincDistRes[iPert]] = &fdDistResRD2DpDt;
        
        /* q = s*cos(LongA) */
        InitializeQincDistRes(body,update,iBody,iPert);
        fnUpdate[iBody][update[iBody].iQinc][update[iBody].iaQincDistRes[iPert]] = &fdDistResRD2DqDt;
        
        jBody = body[iBody].iaGravPerts[iPert];
        for (iRes=1;iRes<=RESMAX;iRes++) {
//           iRes = system->iResIndex[jj][system->imLaplaceN[iBody][jBody]];
          
          for (j=26;j<LAPLNUM;j++) {
            if (j==26 || j==30 || j==44 || j==48 || j==52 || j==56 || j==61 || j>=81) {
              if (body[iBody].dSemi < body[jBody].dSemi) {  
                  system->imLaplaceN[iBody][jBody] = CombCount(iBody,jBody,control->Evolve.iNumBodies-1);
                  system->dmLaplaceC[iRes][system->imLaplaceN[iBody][jBody]][j] = system->fnLaplaceF[j][0](body[iBody].dSemi/body[jBody].dSemi,iRes);
                  system->dmLaplaceD[iRes][system->imLaplaceN[iBody][jBody]][j] = system->fnLaplaceDeriv[j][0](body[iBody].dSemi/body[jBody].dSemi,iRes);    
                  system->dmAlpha0[iRes][system->imLaplaceN[iBody][jBody]][j] = body[iBody].dSemi/body[jBody].dSemi;
              } else if (body[iBody].dSemi > body[jBody].dSemi) {
                  system->imLaplaceN[iBody][jBody] = CombCount(jBody,iBody,control->Evolve.iNumBodies-1);
                  system->dmLaplaceC[iRes][system->imLaplaceN[iBody][jBody]][j] = system->fnLaplaceF[j][0](body[jBody].dSemi/body[iBody].dSemi,iRes);
                  system->dmLaplaceD[iRes][system->imLaplaceN[iBody][jBody]][j] = system->fnLaplaceDeriv[j][0](body[jBody].dSemi/body[iBody].dSemi,iRes);  
                  system->dmAlpha0[iRes][system->imLaplaceN[iBody][jBody]][j] = body[jBody].dSemi/body[iBody].dSemi;
              }
            }
          }
        }
    }
    
  }
  
  if (iBody==control->Evolve.iNumBodies-1) {
    CheckResonance(body,&control->Evolve,system); 
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
  
  for (iBody=1;iBody<evolve->iNumBodies;iBody++) {
    for (jBody=iBody+1;jBody<evolve->iNumBodies;jBody++) {
      if (body[iBody].dSemi > body[jBody].dSemi) {
        dPerRat = pow(body[iBody].dSemi/body[jBody].dSemi,1.5);
      } else { 
        dPerRat = pow(body[jBody].dSemi/body[iBody].dSemi,1.5);
      }
      // 2:1 resonance in the 0th place
      if (dPerRat > 0.9*2.0 && dPerRat < 1.1*2.0) {
        system->iResIndex[0][system->imLaplaceN[iBody][jBody]] = 2;
        system->iResIndex[4][system->imLaplaceN[iBody][jBody]] = 4;
      } else {
        system->iResIndex[0][system->imLaplaceN[iBody][jBody]] = -1;
        system->iResIndex[4][system->imLaplaceN[iBody][jBody]] = -1;
      }
      // 3:2 resonance in the 1th place
      if (dPerRat > 0.9*1.5 && dPerRat < 1.1*1.5) {
        system->iResIndex[1][system->imLaplaceN[iBody][jBody]] = 3;
        system->iResIndex[5][system->imLaplaceN[iBody][jBody]] = 6;
      } else {
        system->iResIndex[1][system->imLaplaceN[iBody][jBody]] = -1;
        system->iResIndex[5][system->imLaplaceN[iBody][jBody]] = -1;
      } 
      // 3:1 resonance in the 2th place
      if (dPerRat > 0.9*3.0 && dPerRat < 1.1*3.0) {
        system->iResIndex[2][system->imLaplaceN[iBody][jBody]] = 3;
      } else {
        system->iResIndex[2][system->imLaplaceN[iBody][jBody]] = -1;
      }    
      // 5:3 in the 3th place
      if (dPerRat > 0.9*(5./3) && dPerRat < 1.1*(5./3)) {
        system->iResIndex[3][system->imLaplaceN[iBody][jBody]] = 5;
      } else {
        system->iResIndex[3][system->imLaplaceN[iBody][jBody]] = -1;
      }   
    }
  }
}

/* 
 * Semi-major axis functions
 */
void RecalcLaplaceDistRes(BODY *body, CONTROL *control, SYSTEM *system) {
  int iBody, iPert, jBody, j, jj, iRes;
  
  for (iBody=1;iBody<control->Evolve.iNumBodies;iBody++) {
    for (iPert=0;iPert<body[iBody].iGravPerts;iPert++){ 
      jBody = body[iBody].iaGravPerts[iPert];
      for (jj=0;jj<RESNUM;jj++) {
      //xxx needs fixed!
        iRes = system->iResIndex[jj][system->imLaplaceN[iBody][jBody]];
        for (j=26;j<LAPLNUM;j++) {
          if (j==26 || j==30 || j==44 || j==48 || j==52 || j==56 || j==61) {
            if (body[iBody].dSemi < body[jBody].dSemi) {  
                system->imLaplaceN[iBody][jBody] = CombCount(iBody,jBody,control->Evolve.iNumBodies-1);
                system->dmLaplaceC[iRes][system->imLaplaceN[iBody][jBody]][j] = system->fnLaplaceF[j][0](body[iBody].dSemi/body[jBody].dSemi,iRes);
                system->dmLaplaceD[iRes][system->imLaplaceN[iBody][jBody]][j] = system->fnLaplaceDeriv[j][0](body[iBody].dSemi/body[jBody].dSemi,iRes);    
                system->dmAlpha0[iRes][system->imLaplaceN[iBody][jBody]][j] = body[iBody].dSemi/body[jBody].dSemi;
            } else if (body[iBody].dSemi > body[jBody].dSemi) {
                system->imLaplaceN[iBody][jBody] = CombCount(jBody,iBody,control->Evolve.iNumBodies-1);
                system->dmLaplaceC[iRes][system->imLaplaceN[iBody][jBody]][j] = system->fnLaplaceF[j][0](body[jBody].dSemi/body[iBody].dSemi,iRes);
                system->dmLaplaceD[iRes][system->imLaplaceN[iBody][jBody]][j] = system->fnLaplaceDeriv[j][0](body[jBody].dSemi/body[iBody].dSemi,iRes);  
                system->dmAlpha0[iRes][system->imLaplaceN[iBody][jBody]][j] = body[jBody].dSemi/body[iBody].dSemi;
            }
          }
        }
      }
    }
  }
}

#define A(iIndexJ) dAxRatio,iIndexJ,0.5
#define B(iIndexJ) dAxRatio,iIndexJ,1.5
#define C(iIndexJ) dAxRatio,iIndexJ,2.5

// A -> s = 1/2
// B -> s = 3/2
// C -> s = 5/2

/*--------- f27 ---------------------*/
double fdSemiMajAxF27(double dAxRatio, int iIndexJ) {
  return 0.5*(-2.*iIndexJ*fdLaplaceCoeff(A(iIndexJ)) - dAxRatio*fdDerivLaplaceCoeff(1,A(iIndexJ)));
}

double fdDSemiF27Dalpha(double dAxRatio, int iIndexJ) {
  return 0.5*((-2.*iIndexJ-1.)*fdDerivLaplaceCoeff(1,A(iIndexJ))-dAxRatio*fdDerivLaplaceCoeff(2,A(iIndexJ)));
}

/*--------- f31 ---------------------*/
double fdSemiMajAxF31(double dAxRatio, int iIndexJ) {
  return 0.5*((-1.0+2.*iIndexJ)*fdLaplaceCoeff(A(abs(iIndexJ-1))) + dAxRatio*fdDerivLaplaceCoeff(1,A(abs(iIndexJ-1))));
}

double fdDSemiF31Dalpha(double dAxRatio, int iIndexJ) {
  return 0.5*((2.*iIndexJ)*fdDerivLaplaceCoeff(1,A(abs(iIndexJ-1)))+dAxRatio*fdDerivLaplaceCoeff(2,A(abs(iIndexJ-1))));
}

/*--------- f45 ---------------------*/
double fdSemiMajAxF45(double dAxRatio, int iIndexJ) {
  return 1./8*((-5.*iIndexJ+4.*iIndexJ*iIndexJ)*fdLaplaceCoeff(A(iIndexJ))\
        +(-2.+4.*iIndexJ)*dAxRatio*fdDerivLaplaceCoeff(1,A(iIndexJ))\
        +dAxRatio*dAxRatio*fdDerivLaplaceCoeff(2,A(iIndexJ)));
}

double fdDSemiF45Dalpha(double dAxRatio, int iIndexJ) {
  return 1./8*((-2.-1.*iIndexJ+4.*iIndexJ*iIndexJ)*fdDerivLaplaceCoeff(1,A(iIndexJ))\
        +(4*iIndexJ)*dAxRatio*fdDerivLaplaceCoeff(2,A(iIndexJ))\
        +dAxRatio*dAxRatio*fdDerivLaplaceCoeff(3,A(iIndexJ)));
}

/*--------- f49 ---------------------*/
double fdSemiMajAxF49(double dAxRatio, int iIndexJ) {
  return 1./4*((-2.+6.*iIndexJ-4.*iIndexJ*iIndexJ)*fdLaplaceCoeff(A(abs(iIndexJ-1)))\
         +(2.-4.*iIndexJ)*dAxRatio*fdDerivLaplaceCoeff(1,A(abs(iIndexJ-1)))\
         -dAxRatio*dAxRatio*fdDerivLaplaceCoeff(2,A(abs(iIndexJ-1))));
}

double fdDSemiF49Dalpha(double dAxRatio, int iIndexJ) {
  return 1./4*((2.*iIndexJ-4.*iIndexJ*iIndexJ)*fdDerivLaplaceCoeff(1,A(abs(iIndexJ-1)))\
         -4.*iIndexJ*dAxRatio*fdDerivLaplaceCoeff(2,A(abs(iIndexJ-1)))\
         -dAxRatio*dAxRatio*fdDerivLaplaceCoeff(3,A(abs(iIndexJ-1))));
}

/*--------- f53 ---------------------*/
double fdSemiMajAxF53(double dAxRatio, int iIndexJ) {
  return 1./8*((2.-7.*iIndexJ+4.*iIndexJ*iIndexJ)*fdLaplaceCoeff(A(abs(iIndexJ-2)))\
         +(-2.+4*iIndexJ)*dAxRatio*fdDerivLaplaceCoeff(1,A(abs(iIndexJ-2)))
         +dAxRatio*dAxRatio*fdDerivLaplaceCoeff(2,A(abs(iIndexJ-2))));
}

double fdDSemiF53Dalpha(double dAxRatio, int iIndexJ) {
  return 1./8*((-3.*iIndexJ+4.*iIndexJ*iIndexJ)*fdDerivLaplaceCoeff(1,A(abs(iIndexJ-2)))\
         +(4*iIndexJ)*dAxRatio*fdDerivLaplaceCoeff(2,A(abs(iIndexJ-2)))
         +dAxRatio*dAxRatio*fdDerivLaplaceCoeff(3,A(abs(iIndexJ-2))));
}

/*--------- f57 ---------------------*/
double fdSemiMajAxF57(double dAxRatio, int iIndexJ) {
  return 0.5*dAxRatio*fdLaplaceCoeff(B(abs(iIndexJ-1)));
}

double fdDSemiF57Dalpha(double dAxRatio, int iIndexJ) {
  return 0.5*(fdLaplaceCoeff(B(abs(iIndexJ-1)))+dAxRatio*fdDerivLaplaceCoeff(1,B(abs(iIndexJ-1))));
}

/*--------- f62 ---------------------*/
double fdSemiMajAxF62(double dAxRatio, int iIndexJ) {
  return -dAxRatio*fdLaplaceCoeff(B(abs(iIndexJ-1)));
}

double fdDSemiF62Dalpha(double dAxRatio, int iIndexJ) {
  return -1.*(fdLaplaceCoeff(B(abs(iIndexJ-1)))+dAxRatio*fdDerivLaplaceCoeff(1,B(abs(iIndexJ-1))));
}

/*--------- f82 ---------------------*/
double fdSemiMajAxF82(double dAxRatio, int iIndexJ) {
  return 1./48*( (-26*iIndexJ+30*iIndexJ*iIndexJ-8*iIndexJ*iIndexJ*iIndexJ)*fdLaplaceCoeff(A(iIndexJ)) \
        +(-9*dAxRatio+27*iIndexJ*dAxRatio-12*iIndexJ*iIndexJ*dAxRatio)*fdDerivLaplaceCoeff(1,A(iIndexJ)) \
        +6*(1-iIndexJ)*dAxRatio*dAxRatio*fdDerivLaplaceCoeff(2,A(iIndexJ)) \
        -dAxRatio*dAxRatio*dAxRatio*fdDerivLaplaceCoeff(3,A(iIndexJ)) );
}

double fdDSemiF82Dalpha(double dAxRatio, int iIndexJ) {
  return 1./48*( (-9+iIndexJ+18*iIndexJ*iIndexJ-8*iIndexJ*iIndexJ*iIndexJ)*fdDerivLaplaceCoeff(1,A(iIndexJ))\
        +(3+15*iIndexJ-12*iIndexJ*iIndexJ)*dAxRatio*fdDerivLaplaceCoeff(2,A(iIndexJ)) \
        +(3-6*iIndexJ)*dAxRatio*dAxRatio*fdDerivLaplaceCoeff(3,A(iIndexJ))\
        -dAxRatio*dAxRatio*dAxRatio*fdDerivLaplaceCoeff(4,A(iIndexJ)) );
}

/*--------- f83 ---------------------*/
double fdSemiMajAxF83(double dAxRatio, int iIndexJ) {
  return 1./16*( (-9+31*iIndexJ-30*iIndexJ*iIndexJ+8*iIndexJ*iIndexJ*iIndexJ)*fdLaplaceCoeff(A(abs(iIndexJ-1)))\
        +(9-25*iIndexJ+12*iIndexJ*iIndexJ)*dAxRatio*fdDerivLaplaceCoeff(1,A(abs(iIndexJ-1)))\
        +(-5+6*iIndexJ)*dAxRatio*dAxRatio*fdDerivLaplaceCoeff(2,A(abs(iIndexJ-1)))\
        +dAxRatio*dAxRatio*dAxRatio*fdDerivLaplaceCoeff(3,A(abs(iIndexJ-1))) );
}

double fdDSemiF83Dalpha(double dAxRatio, int iIndexJ) {
  return 1./16*( (6*iIndexJ-18*iIndexJ*iIndexJ+8*iIndexJ*iIndexJ*iIndexJ)*fdDerivLaplaceCoeff(1,A(abs(iIndexJ-1)))\
        +(-1-13*iIndexJ+12*iIndexJ*iIndexJ)*dAxRatio*fdDerivLaplaceCoeff(2,A(abs(iIndexJ-1)))\
        +(-2+6*iIndexJ)*dAxRatio*dAxRatio*fdDerivLaplaceCoeff(3,A(abs(iIndexJ-1))) \
        +dAxRatio*dAxRatio*dAxRatio*fdDerivLaplaceCoeff(4,A(abs(iIndexJ-1))) );
}

/*--------- f84 ---------------------*/
double fdSemiMajAxF84(double dAxRatio, int iIndexJ) {
  return 1./16*( (8-32*iIndexJ+30*iIndexJ*iIndexJ-8*iIndexJ*iIndexJ*iIndexJ)*fdLaplaceCoeff(A(abs(iIndexJ-2)))\
        +(-8+23*iIndexJ-12*iIndexJ*iIndexJ)*dAxRatio*fdDerivLaplaceCoeff(1,A(abs(iIndexJ-2)))\
        +(4-6*iIndexJ)*dAxRatio*dAxRatio*fdDerivLaplaceCoeff(2,A(abs(iIndexJ-2)))\
        -dAxRatio*dAxRatio*dAxRatio*fdDerivLaplaceCoeff(3,A(abs(iIndexJ-2))) );  
}

double fdDSemiF84Dalpha(double dAxRatio, int iIndexJ) {
  return 1./16*( (-9*iIndexJ+18*iIndexJ*iIndexJ-8*iIndexJ*iIndexJ*iIndexJ)*fdDerivLaplaceCoeff(1,A(abs(iIndexJ-2)))\
        +(11*iIndexJ-12*iIndexJ*iIndexJ)*dAxRatio*fdDerivLaplaceCoeff(2,A(abs(iIndexJ-2)))\
        +(1-6*iIndexJ)*dAxRatio*dAxRatio*fdDerivLaplaceCoeff(3,A(abs(iIndexJ-2)))\
        -dAxRatio*dAxRatio*dAxRatio*fdDerivLaplaceCoeff(4,A(abs(iIndexJ-2))) );
}

/*--------- f85 ---------------------*/
double fdSemiMajAxF85(double dAxRatio, int iIndexJ) {
  return 1./48*( (-6+29*iIndexJ-30*iIndexJ*iIndexJ+8*iIndexJ*iIndexJ*iIndexJ)*fdLaplaceCoeff(A(abs(iIndexJ-3)))\
        +(6-21*iIndexJ+12*iIndexJ*iIndexJ)*dAxRatio*fdDerivLaplaceCoeff(1,A(abs(iIndexJ-3)))\
        +(-3+6*iIndexJ)*dAxRatio*dAxRatio*fdDerivLaplaceCoeff(2,A(abs(iIndexJ-3)))\
        +dAxRatio*dAxRatio*dAxRatio*fdDerivLaplaceCoeff(3,A(abs(iIndexJ-3))) );
}

double fdDSemiF85Dalpha(double dAxRatio, int iIndexJ) {
  return 1./48*( (8*iIndexJ-18*iIndexJ*iIndexJ+8*iIndexJ*iIndexJ*iIndexJ)*fdDerivLaplaceCoeff(1,A(abs(iIndexJ-3)))\
        +(-9*iIndexJ+12*iIndexJ*iIndexJ)*dAxRatio*fdDerivLaplaceCoeff(2,A(abs(iIndexJ-3)))\
        +6*iIndexJ*dAxRatio*dAxRatio*fdDerivLaplaceCoeff(3,A(abs(iIndexJ-3)))\
        +dAxRatio*dAxRatio*dAxRatio*fdDerivLaplaceCoeff(4,A(abs(iIndexJ-3))) );
}

/*--------- f86 ---------------------*/
double fdSemiMajAxF86(double dAxRatio, int iIndexJ) {
  return 1./4*( (3-2*iIndexJ)*dAxRatio*fdLaplaceCoeff(B(abs(iIndexJ-1)))\
        -dAxRatio*dAxRatio*fdDerivLaplaceCoeff(1,B(abs(iIndexJ-1))) );
}

double fdDSemiF86Dalpha(double dAxRatio, int iIndexJ) {
  return 1./4*( (3-2*iIndexJ)*fdLaplaceCoeff(B(abs(iIndexJ-1)))\
        +(1-2*iIndexJ)*dAxRatio*fdDerivLaplaceCoeff(1,B(abs(iIndexJ-1)))\
        -dAxRatio*dAxRatio*fdDerivLaplaceCoeff(2,B(abs(iIndexJ-1))) );
}

/*--------- f87 ---------------------*/
double fdSemiMajAxF87(double dAxRatio, int iIndexJ) {
  return 1./4*( 2*iIndexJ*dAxRatio*fdLaplaceCoeff(B(abs(iIndexJ-2))) \
        +dAxRatio*dAxRatio*fdDerivLaplaceCoeff(1,B(abs(iIndexJ-2))) );
}

double fdDSemiF87Dalpha(double dAxRatio, int iIndexJ) {
  return 1./4*( 2*iIndexJ*fdLaplaceCoeff(B(abs(iIndexJ-2))) \
        +(2+2*iIndexJ)*dAxRatio*fdDerivLaplaceCoeff(1,B(abs(iIndexJ-2)))\
        +dAxRatio*dAxRatio*fdDerivLaplaceCoeff(2,B(abs(iIndexJ-2))) );
}

/*--------- f88 ---------------------*/
double fdSemiMajAxF88(double dAxRatio, int iIndexJ) {
  return 1./2*( (-3+2*iIndexJ)*dAxRatio*fdLaplaceCoeff(B(abs(iIndexJ-1)))\
        +dAxRatio*dAxRatio*fdDerivLaplaceCoeff(1,B(abs(iIndexJ-1))) );
}

double fdDSemiF88Dalpha(double dAxRatio, int iIndexJ) {
  return 1./2*( (-3+2*iIndexJ)*fdLaplaceCoeff(B(abs(iIndexJ-1)))\
        +(-1+2*iIndexJ)*dAxRatio*fdDerivLaplaceCoeff(1,B(abs(iIndexJ-1)))\
        +dAxRatio*dAxRatio*fdDerivLaplaceCoeff(2,B(abs(iIndexJ-1))) );
}

/*--------- f89 ---------------------*/
double fdSemiMajAxF89(double dAxRatio, int iIndexJ) {
  return 1./2*( -2*iIndexJ*dAxRatio*fdLaplaceCoeff(B(abs(iIndexJ-2))) \
        -dAxRatio*dAxRatio*fdDerivLaplaceCoeff(1,B(abs(iIndexJ-2))) );
}

double fdDSemiF89Dalpha(double dAxRatio, int iIndexJ) {
  return 1./2*( -2*iIndexJ*fdLaplaceCoeff(B(abs(iIndexJ-2))) \
        -(2+2*iIndexJ)*dAxRatio*fdDerivLaplaceCoeff(1,B(abs(iIndexJ-2)))\
        -dAxRatio*dAxRatio*fdDerivLaplaceCoeff(2,B(abs(iIndexJ-2))) );
}

//----------------Disturbing function a h k p q l-------------------------------------------
double fdLambdaArg(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ, int iOrder) {
  if (body[iBody].dSemi < body[jBody].dSemi) {
    return iIndexJ*body[jBody].dMeanL + (iOrder-iIndexJ)*body[iBody].dMeanL;
  } else {
    return iIndexJ*body[iBody].dMeanL + (iOrder-iIndexJ)*body[jBody].dMeanL;
  }
}

//--------dR/dh-----------(inner body)------------------------------------------------------
double fdDdistDhDir11(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f27, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][26];
  f27 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][26] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][26]*dAlpha;

  return f27*sin(fdLambdaArg(body,system,iBody,jBody,iIndexJ,1));
}

double fdDdistDhDir12(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return 0;
}

double fdDdistDhDir21(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double dAlpha, f45;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][44];
  f45 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][44] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][44]*dAlpha;
  
  return f45*(-2.*body[iBody].dHecc*cos(fdLambdaArg(body,system,iBody,jBody,iIndexJ,2))
          +2*body[iBody].dKecc*sin(fdLambdaArg(body,system,iBody,jBody,iIndexJ,2)));
}

double fdDdistDhDir22(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f49, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][48];
  f49 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][48] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][48]*dAlpha;
  
  return f49*(-body[jBody].dHecc*cos(fdLambdaArg(body,system,iBody,jBody,iIndexJ,2))
          +body[jBody].dKecc*sin(fdLambdaArg(body,system,iBody,jBody,iIndexJ,2)));
}

double fdDdistDhDir23(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return 0;
}
  
double fdDdistDhDir24(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return 0;
}  

double fdDdistDhDir25(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return 0;
} 

double fdDdistDhDir26(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return 0;
}   

double fdDdistDhDir31(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f82, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][81];
  f82 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][81] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][81]*dAlpha;
  
  return f82*(-6*body[iBody].dHecc*body[iBody].dKecc*cos(fdLambdaArg(body,system,iBody,jBody,iIndexJ,3))\
          +(-3*body[iBody].dHecc*body[iBody].dHecc+3*body[iBody].dKecc*body[iBody].dKecc)\
          *sin(fdLambdaArg(body,system,iBody,jBody,iIndexJ,3)));
}
  
double fdDdistDhDir32(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f83, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][82];
  f83 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][82] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][82]*dAlpha;
  
  return f83*((-2*body[iBody].dHecc*body[jBody].dKecc-2*body[jBody].dHecc*body[iBody].dKecc)\
          *cos(fdLambdaArg(body,system,iBody,jBody,iIndexJ,3))\
          +(-2*body[iBody].dHecc*body[jBody].dHecc+2*body[iBody].dKecc*body[jBody].dKecc)\
          *sin(fdLambdaArg(body,system,iBody,jBody,iIndexJ,3)));
} 

double fdDdistDhDir33(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f84, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][83];
  f84 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][83] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][83]*dAlpha;
  
  return f84*((-2*body[jBody].dHecc*body[jBody].dKecc)\
          *cos(fdLambdaArg(body,system,iBody,jBody,iIndexJ,3))\
          +(body[jBody].dKecc*body[jBody].dKecc-body[jBody].dHecc*body[jBody].dHecc)\
          *sin(fdLambdaArg(body,system,iBody,jBody,iIndexJ,3)));
}  
  
double fdDdistDhDir35(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f86, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][85];
  f86 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][85] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][85]*dAlpha;
  
  return f86*(-2*body[iBody].dPinc*body[iBody].dQinc\
          *cos(fdLambdaArg(body,system,iBody,jBody,iIndexJ,3))\
          +(body[iBody].dQinc*body[iBody].dQinc-body[iBody].dPinc*body[iBody].dPinc)\
          *sin(fdLambdaArg(body,system,iBody,jBody,iIndexJ,3)));
}

double fdDdistDhDir37(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f88, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][87];
  f88 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][87] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][87]*dAlpha;
  
  return f88*( -(body[jBody].dPinc*body[iBody].dQinc+body[jBody].dQinc*body[iBody].dPinc)\
          *cos(fdLambdaArg(body,system,iBody,jBody,iIndexJ,3))\
          +(body[jBody].dQinc*body[iBody].dQinc-body[jBody].dPinc*body[iBody].dPinc)\
          *sin(fdLambdaArg(body,system,iBody,jBody,iIndexJ,3)));
}

double fdDdistDhDir39(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f86, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][85];
  f86 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][85] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][85]*dAlpha;
  
  return f86*( -2*body[jBody].dPinc*body[jBody].dQinc\
          *cos(fdLambdaArg(body,system,iBody,jBody,iIndexJ,3))\
          +(body[jBody].dQinc*body[jBody].dQinc-body[jBody].dPinc*body[jBody].dPinc)\
          *sin(fdLambdaArg(body,system,iBody,jBody,iIndexJ,3)));
}
  
double fdDdistresDHecc(BODY *body, SYSTEM *system, int *iaBody, int iIndexJ, int iOrder) {
  double y, dMfac, dSemiPrm;
  dMfac = KGAUSS*KGAUSS*body[iaBody[1]].dMass/MSUN;
  dSemiPrm = body[iaBody[1]].dSemi/AUCM;
  if (iOrder == 1) {
    y = ( fdDdistDhDir11(body, system, iaBody[0], iaBody[1], iIndexJ) ) * dMfac/dSemiPrm;
  } else if (iOrder == 2) {
    y = ( fdDdistDhDir21(body, system, iaBody[0], iaBody[1], iIndexJ) \
    + fdDdistDhDir22(body, system, iaBody[0], iaBody[1], iIndexJ) ) * dMfac/dSemiPrm;
  }
  return y;
}

//---------dR/dh'------------------------------------------------------------------

double fdDdistDhPrmDir11(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return 0;  //true to second order (temporary)
}

double fdDdistDhPrmDir12(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f31, dAlpha;
  dAlpha = body[jBody].dSemi/body[iBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][30];
  f31 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][30] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][30]*dAlpha;
  
  return f31*sin(fdLambdaArg(body,system,iBody,jBody,iIndexJ,1));
}

double fdDdistDhPrmInt13(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return -0.5*sin(fdLambdaArg(body,system,iBody,jBody,iIndexJ,1));
}

double fdDdistDhPrmDir21(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return 0;  //true to second order (temporary)
}

double fdDdistDhPrmDir22(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f49, dAlpha;
  dAlpha = body[jBody].dSemi/body[iBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][48];
  f49 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][48] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][48]*dAlpha;
  
  return f49*(-body[jBody].dHecc*cos(fdLambdaArg(body,system,iBody,jBody,iIndexJ,2))
          +body[jBody].dKecc*sin(fdLambdaArg(body,system,iBody,jBody,iIndexJ,2)));
}

double fdDdistDhPrmDir23(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f53, dAlpha;
  dAlpha = body[jBody].dSemi/body[iBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][52];
  f53 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][52] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][52]*dAlpha;

  return f53*(-2.*body[iBody].dHecc*cos(fdLambdaArg(body,system,iBody,jBody,iIndexJ,2))
          +2*body[iBody].dKecc*sin(fdLambdaArg(body,system,iBody,jBody,iIndexJ,2)));
}

double fdDdistDhPrmDir24(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return 0;
}

double fdDdistDhPrmDir25(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return 0;
}

double fdDdistDhPrmDir26(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return 0;
}

double fdDdistDhPrmDir32(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f83, dAlpha;
  dAlpha = body[jBody].dSemi/body[iBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][82];
  f83 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][82] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][82]*dAlpha;
  
  return f83*((-2*body[jBody].dHecc*body[jBody].dKecc)\
          *cos(fdLambdaArg(body,system,jBody,iBody,iIndexJ,3))\
          +(body[jBody].dKecc*body[jBody].dKecc-body[jBody].dHecc*body[jBody].dHecc)\
          *sin(fdLambdaArg(body,system,jBody,iBody,iIndexJ,3)));
}

double fdDdistDhPrmDir33(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f84, dAlpha;
  dAlpha = body[jBody].dSemi/body[iBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][83];
  f84 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][83] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][83]*dAlpha;
  
  return f84*((-2*body[iBody].dHecc*body[jBody].dKecc-2*body[jBody].dHecc*body[iBody].dKecc)\
          *cos(fdLambdaArg(body,system,jBody,iBody,iIndexJ,3))\
          +(-2*body[iBody].dHecc*body[jBody].dHecc+2*body[jBody].dKecc*body[iBody].dKecc)\
          *sin(fdLambdaArg(body,system,jBody,iBody,iIndexJ,3)));
} 

double fdDdistDhPrmDir34(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f85, dAlpha;
  dAlpha = body[jBody].dSemi/body[iBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][84];
  f85 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][84] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][84]*dAlpha;
  
  return f85*(-6*body[iBody].dHecc*body[iBody].dKecc*cos(fdLambdaArg(body,system,iBody,jBody,iIndexJ,3))\
          +(-3*body[iBody].dHecc*body[iBody].dHecc+3*body[iBody].dKecc*body[iBody].dKecc)\
          *sin(fdLambdaArg(body,system,iBody,jBody,iIndexJ,3)));
}

double fdDdistDhPrmDir36(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f87, dAlpha;
  dAlpha = body[jBody].dSemi/body[iBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][86];
  f87 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][86] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][86]*dAlpha;
  
  return f87*(-2*body[jBody].dPinc*body[jBody].dQinc\
          *cos(fdLambdaArg(body,system,iBody,jBody,iIndexJ,3))\
          +(body[jBody].dQinc*body[jBody].dQinc-body[jBody].dPinc*body[jBody].dPinc)\
          *sin(fdLambdaArg(body,system,iBody,jBody,iIndexJ,3)));
}

double fdDdistDhPrmDir38(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f89, dAlpha;
  dAlpha = body[jBody].dSemi/body[iBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][88];
  f89 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][88] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][88]*dAlpha;
  
  return f89*( -(body[iBody].dPinc*body[jBody].dQinc+body[iBody].dQinc*body[jBody].dPinc)\
          *cos(fdLambdaArg(body,system,iBody,jBody,iIndexJ,3))\
          +(body[iBody].dQinc*body[jBody].dQinc-body[iBody].dPinc*body[jBody].dPinc)\
          *sin(fdLambdaArg(body,system,iBody,jBody,iIndexJ,3)));
}

double fdDdistDhPrmDir310(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f87, dAlpha;
  dAlpha = body[jBody].dSemi/body[iBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][86];
  f87 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][86] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][86]*dAlpha;
  
  return f87*( -2*body[iBody].dPinc*body[iBody].dQinc\
          *cos(fdLambdaArg(body,system,iBody,jBody,iIndexJ,3))\
          +(body[iBody].dQinc*body[iBody].dQinc-body[iBody].dPinc*body[iBody].dPinc)\
          *sin(fdLambdaArg(body,system,iBody,jBody,iIndexJ,3)));
}  

double fdDdistresDHeccPrime(BODY *body, SYSTEM *system, int *iaBody, int iIndexJ, int iOrder) {
  double y, dMfac, dSemiPrm;
  dMfac = KGAUSS*KGAUSS*body[iaBody[1]].dMass/MSUN;
  dSemiPrm = body[iaBody[0]].dSemi/AUCM;
  if (iOrder == 1) {
    y = ( fdDdistDhPrmDir12(body, system, iaBody[0], iaBody[1], iIndexJ) ) * dMfac/dSemiPrm;
    if (iIndexJ == 2) {
      y += (fdDdistDhPrmInt13(body, system, iaBody[0], iaBody[1], iIndexJ)) * dMfac*dSemiPrm\
          /(body[iaBody[1]].dSemi*body[iaBody[1]].dSemi/AUCM/AUCM);
    }
  } else if (iOrder == 2) {
    y = ( fdDdistDhPrmDir22(body, system, iaBody[0], iaBody[1], iIndexJ) \
      + fdDdistDhPrmDir23(body, system, iaBody[0], iaBody[1], iIndexJ) ) * dMfac/dSemiPrm;
  }
  return y;
}

//------------dR/dk--------------------------------------------------------------

double fdDdistDkDir11(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f27, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][26];
  f27 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][26] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][26]*dAlpha;
  
  return f27*cos(fdLambdaArg(body,system,iBody,jBody,iIndexJ,1));
}

double fdDdistDkDir12(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return 0;
}

double fdDdistDkDir21(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f45, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][44];
  f45 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][44] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][44]*dAlpha;
  
  return f45*(2.*body[iBody].dKecc*cos(fdLambdaArg(body,system,iBody,jBody,iIndexJ,2))
          +2*body[iBody].dHecc*sin(fdLambdaArg(body,system,iBody,jBody,iIndexJ,2)));
}

double fdDdistDkDir22(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f49, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][48];
  f49 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][48] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][48]*dAlpha;
  
  return f49*(body[jBody].dKecc*cos(fdLambdaArg(body,system,iBody,jBody,iIndexJ,2))
          +body[jBody].dHecc*sin(fdLambdaArg(body,system,iBody,jBody,iIndexJ,2)));
}

double fdDdistDkDir23(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return 0;
}

double fdDdistDkDir24(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return 0;
}

double fdDdistDkDir25(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return 0;
}

double fdDdistDkDir26(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return 0;
}

double fdDdistDkDir31(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f82, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][81];
  f82 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][81] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][81]*dAlpha;
  
  return f82*((3*body[iBody].dKecc*body[iBody].dKecc-3*body[iBody].dHecc*body[iBody].dHecc)\
          *cos(fdLambdaArg(body,system,iBody,jBody,iIndexJ,3)) \
          +6*body[iBody].dKecc*body[iBody].dHecc*sin(fdLambdaArg(body,system,iBody,jBody,iIndexJ,3))); 
}  

double fdDdistDkDir32(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f83, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][82];
  f83 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][82] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][82]*dAlpha;
  
  return f83*((2*body[iBody].dKecc*body[jBody].dKecc-2*body[iBody].dHecc*body[jBody].dHecc)\
          *cos(fdLambdaArg(body,system,iBody,jBody,iIndexJ,3))\
          +(2*body[iBody].dKecc*body[jBody].dHecc+2*body[jBody].dKecc*body[iBody].dHecc)\
          *sin(fdLambdaArg(body,system,iBody,jBody,iIndexJ,3)));
} 

double fdDdistDkDir33(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f84, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][83];
  f84 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][83] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][83]*dAlpha;
  
  return f84*((body[jBody].dKecc*body[jBody].dKecc-body[jBody].dHecc*body[jBody].dHecc)\
          *cos(fdLambdaArg(body,system,iBody,jBody,iIndexJ,3))\
          +(2*body[jBody].dHecc*body[jBody].dHecc)\
          *sin(fdLambdaArg(body,system,iBody,jBody,iIndexJ,3)));
} 

double fdDdistDkDir35(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f86, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][85];
  f86 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][85] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][85]*dAlpha;
  
  return f86*((body[iBody].dQinc*body[iBody].dQinc-body[iBody].dPinc*body[iBody].dPinc)\
          *cos(fdLambdaArg(body,system,iBody,jBody,iIndexJ,3))\
          +(2*body[iBody].dPinc*body[iBody].dQinc)\
          *sin(fdLambdaArg(body,system,iBody,jBody,iIndexJ,3)));
}

double fdDdistDkDir37(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f88, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][87];
  f88 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][87] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][87]*dAlpha;
  
  return f88*( (body[jBody].dQinc*body[iBody].dQinc-body[jBody].dPinc*body[iBody].dPinc)\
          *cos(fdLambdaArg(body,system,iBody,jBody,iIndexJ,3))\
          +(body[jBody].dPinc*body[iBody].dQinc+body[jBody].dQinc*body[iBody].dPinc)\
          *sin(fdLambdaArg(body,system,iBody,jBody,iIndexJ,3)));
}

double fdDdistDkDir39(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f86, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][85];
  f86 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][85] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][85]*dAlpha;
  
  return f86*( (body[jBody].dQinc*body[jBody].dQinc-body[jBody].dPinc*body[jBody].dPinc)\
          *cos(fdLambdaArg(body,system,iBody,jBody,iIndexJ,3))\
          +(2*body[jBody].dPinc*body[jBody].dQinc)\
          *sin(fdLambdaArg(body,system,iBody,jBody,iIndexJ,3)));
}

double fdDdistresDKecc(BODY *body, SYSTEM *system, int *iaBody, int iIndexJ, int iOrder) {
  double y, dMfac, dSemiPrm;
  dMfac = KGAUSS*KGAUSS*body[iaBody[1]].dMass/MSUN;
  dSemiPrm = body[iaBody[1]].dSemi/AUCM;
  if (iOrder == 1) {
    y = ( fdDdistDkDir11(body, system, iaBody[0], iaBody[1], iIndexJ) ) * dMfac/dSemiPrm;
  } else if (iOrder == 2) {
    y = ( fdDdistDkDir21(body, system, iaBody[0], iaBody[1], iIndexJ) \
    + fdDdistDkDir22(body, system, iaBody[0], iaBody[1], iIndexJ) ) * dMfac/dSemiPrm;
  }
  return y;
}

//---------dR/dk'------------------------------------------------------------------

double fdDdistDkPrmDir11(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return 0;  //true to second order (temporary)
}

double fdDdistDkPrmDir12(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f31, dAlpha;
  dAlpha = body[jBody].dSemi/body[iBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][30];
  f31 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][30] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][30]*dAlpha;
  
  return f31*cos(fdLambdaArg(body,system,iBody,jBody,iIndexJ,1));
}

double fdDdistDkPrmInt13(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return -0.5*cos(fdLambdaArg(body,system,iBody,jBody,iIndexJ,1));
}

double fdDdistDkPrmDir21(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return 0;  //true to second order (temporary)
}

double fdDdistDkPrmDir22(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f49, dAlpha;
  dAlpha = body[jBody].dSemi/body[iBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][48];
  f49 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][48] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][48]*dAlpha;
  
  return f49*(body[jBody].dKecc*cos(fdLambdaArg(body,system,iBody,jBody,iIndexJ,2))
          +body[jBody].dHecc*sin(fdLambdaArg(body,system,iBody,jBody,iIndexJ,2)));
}

double fdDdistDkPrmDir23(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f53, dAlpha;
  dAlpha = body[jBody].dSemi/body[iBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][52];
  f53 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][52] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][52]*dAlpha;
  
  return f53*(2.*body[iBody].dKecc*cos(fdLambdaArg(body,system,iBody,jBody,iIndexJ,2))
          +2*body[iBody].dHecc*sin(fdLambdaArg(body,system,iBody,jBody,iIndexJ,2)));
}

double fdDdistDkPrmDir24(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return 0;  //true to second order (temporary)
}

double fdDdistDkPrmDir25(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return 0;  //true to second order (temporary)
}

double fdDdistDkPrmDir26(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return 0;  //true to second order (temporary)
}

double fdDdistDkPrmDir32(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f83, dAlpha;
  dAlpha = body[jBody].dSemi/body[iBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][82];
  f83 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][82] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][82]*dAlpha;
  
  return f83*((body[jBody].dKecc*body[jBody].dKecc-body[jBody].dHecc*body[jBody].dHecc)\
          *cos(fdLambdaArg(body,system,jBody,iBody,iIndexJ,3))\
          +(2*body[jBody].dHecc*body[jBody].dKecc)\
          *sin(fdLambdaArg(body,system,jBody,iBody,iIndexJ,3)));
} 

double fdDdistDkPrmDir33(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f84, dAlpha;
  dAlpha = body[jBody].dSemi/body[iBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][83];
  f84 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][83] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][83]*dAlpha;
  
  return f84*((2*body[jBody].dKecc*body[iBody].dKecc-2*body[jBody].dHecc*body[iBody].dHecc)\
          *cos(fdLambdaArg(body,system,jBody,iBody,iIndexJ,3))\
          +(2*body[jBody].dHecc*body[iBody].dKecc+2*body[jBody].dKecc*body[iBody].dHecc)\
          *sin(fdLambdaArg(body,system,jBody,iBody,iIndexJ,3)));
}

double fdDdistDkPrmDir34(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f85, dAlpha;
  dAlpha = body[jBody].dSemi/body[iBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][84];
  f85 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][84] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][84]*dAlpha;
  
  return f85*((3*body[iBody].dKecc*body[iBody].dKecc-3*body[iBody].dHecc*body[iBody].dHecc)\
          *cos(fdLambdaArg(body,system,iBody,jBody,iIndexJ,3)) \
          +6*body[iBody].dKecc*body[iBody].dHecc*sin(fdLambdaArg(body,system,iBody,jBody,iIndexJ,3))); 
}  

double fdDdistDkPrmDir36(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f87, dAlpha;
  dAlpha = body[jBody].dSemi/body[iBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][86];
  f87 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][86] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][86]*dAlpha;
  
  return f87*((body[jBody].dQinc*body[jBody].dQinc-body[jBody].dPinc*body[jBody].dPinc)\
          *cos(fdLambdaArg(body,system,iBody,jBody,iIndexJ,3))\
          +(2*body[jBody].dPinc*body[jBody].dQinc)\
          *sin(fdLambdaArg(body,system,iBody,jBody,iIndexJ,3)));
}

double fdDdistDkPrmDir38(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f89, dAlpha;
  dAlpha = body[jBody].dSemi/body[iBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][88];
  f89 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][88] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][88]*dAlpha;
  
  return f89*( (body[iBody].dQinc*body[jBody].dQinc-body[jBody].dPinc*body[iBody].dPinc)\
          *cos(fdLambdaArg(body,system,iBody,jBody,iIndexJ,3))\
          +(body[iBody].dPinc*body[jBody].dQinc+body[iBody].dQinc*body[jBody].dPinc)\
          *sin(fdLambdaArg(body,system,iBody,jBody,iIndexJ,3)));
}

double fdDdistDkPrmDir310(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f87, dAlpha;
  dAlpha = body[jBody].dSemi/body[iBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][86];
  f87 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][86] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][86]*dAlpha;
  
  return f87*( (body[iBody].dQinc*body[iBody].dQinc-body[iBody].dPinc*body[iBody].dPinc)\
          *cos(fdLambdaArg(body,system,iBody,jBody,iIndexJ,3))\
          +(2*body[iBody].dPinc*body[iBody].dQinc)\
          *sin(fdLambdaArg(body,system,iBody,jBody,iIndexJ,3)));
} 

double fdDdistresDKeccPrime(BODY *body, SYSTEM *system, int *iaBody, int iIndexJ, int iOrder) {
  double y, dMfac, dSemiPrm;
  dMfac = KGAUSS*KGAUSS*body[iaBody[1]].dMass/MSUN;
  dSemiPrm = body[iaBody[0]].dSemi/AUCM;
  if (iOrder == 1) {
    y = ( fdDdistDkPrmDir12(body, system, iaBody[0], iaBody[1], iIndexJ) ) * dMfac/dSemiPrm;
    if (iIndexJ == 2) {
      y += fdDdistDkPrmInt13(body, system, iaBody[0], iaBody[1], iIndexJ) * dMfac*dSemiPrm\
          /(body[iaBody[1]].dSemi*body[iaBody[1]].dSemi/AUCM/AUCM);
    }
  } else if (iOrder == 2) {
    y = ( fdDdistDkPrmDir22(body, system, iaBody[0], iaBody[1], iIndexJ) \
    + fdDdistDkPrmDir23(body, system, iaBody[0], iaBody[1], iIndexJ) ) * dMfac/dSemiPrm;
  }
  return y;
}

//---------dR/dp------------------------------------------------------------------

double fdDdistDpDir11(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return 0;  //true to second order (temporary)
}

double fdDdistDpDir12(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return 0;  //true to second order (temporary)
}

double fdDdistDpDir21(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return 0;  //true to second order (temporary)
}

double fdDdistDpDir22(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return 0;  //true to second order (temporary)
}

double fdDdistDpDir23(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return 0;  //true to second order (temporary)
}

double fdDdistDpDir24(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f57, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][56];
  f57 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][56] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][56]*dAlpha;
  
  return f57*(-2.*body[iBody].dPinc*cos(fdLambdaArg(body,system,iBody,jBody,iIndexJ,2))\
          +2*body[iBody].dQinc*sin(fdLambdaArg(body,system,iBody,jBody,iIndexJ,2)));
}

double fdDdistDpDir25(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f62, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][61];
  f62 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][61] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][61]*dAlpha;

  return f62*(-body[jBody].dPinc*cos(fdLambdaArg(body,system,iBody,jBody,iIndexJ,2))\
          +body[jBody].dQinc*sin(fdLambdaArg(body,system,iBody,jBody,iIndexJ,2)));
}

double fdDdistDpDir26(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return 0;  //true to second order (temporary)
}

double fdDdistDpDir35(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f86, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][85];
  f86 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][85] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][85]*dAlpha;
  
  return f86*((-2*body[iBody].dKecc*body[iBody].dPinc-2*body[iBody].dHecc*body[iBody].dQinc)\
          *cos(fdLambdaArg(body,system,iBody,jBody,iIndexJ,3))\
          +(-2*body[iBody].dHecc*body[iBody].dPinc+body[iBody].dKecc*body[iBody].dQinc)\
          *sin(fdLambdaArg(body,system,iBody,jBody,iIndexJ,3)));
}

double fdDdistDpDir36(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f87, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][86];
  f87 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][86] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][86]*dAlpha;
  
  return f87*((-2*body[jBody].dKecc*body[iBody].dPinc-2*body[jBody].dHecc*body[iBody].dQinc)\
          *cos(fdLambdaArg(body,system,iBody,jBody,iIndexJ,3))\
          +(-2*body[jBody].dHecc*body[iBody].dPinc+body[jBody].dKecc*body[iBody].dQinc)\
          *sin(fdLambdaArg(body,system,iBody,jBody,iIndexJ,3)));
}

double fdDdistDpDir37(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f88, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][87];
  f88 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][87] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][87]*dAlpha;
  
  return f88*( (-body[jBody].dPinc*body[iBody].dKecc-body[jBody].dQinc*body[iBody].dHecc)\
          *cos(fdLambdaArg(body,system,iBody,jBody,iIndexJ,3))\
          +(-body[jBody].dPinc*body[iBody].dHecc+body[jBody].dQinc*body[iBody].dKecc)\
          *sin(fdLambdaArg(body,system,iBody,jBody,iIndexJ,3)));
}

double fdDdistDpDir38(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f89, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][88];
  f89 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][88] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][88]*dAlpha;
  
  return f89*( (-body[jBody].dPinc*body[jBody].dKecc-body[jBody].dQinc*body[jBody].dHecc)\
          *cos(fdLambdaArg(body,system,iBody,jBody,iIndexJ,3))\
          +(-body[jBody].dPinc*body[jBody].dHecc+body[jBody].dQinc*body[jBody].dKecc)\
          *sin(fdLambdaArg(body,system,iBody,jBody,iIndexJ,3)));
}

double fdDdistresDPinc(BODY *body, SYSTEM *system, int *iaBody, int iIndexJ, int iOrder) {
  if (iOrder == 1) {
    return 0;
  } else if (iOrder == 2) {  
    double y, dMfac, dSemiPrm;
    dMfac = KGAUSS*KGAUSS*body[iaBody[1]].dMass/MSUN;
    dSemiPrm = body[iaBody[1]].dSemi/AUCM;

    y = ( fdDdistDpDir24(body, system, iaBody[0], iaBody[1], iIndexJ) \
    + fdDdistDpDir25(body, system, iaBody[0], iaBody[1], iIndexJ) ) * dMfac/dSemiPrm;
  
    return y;
  }
}


//---------dR/dp'------------------------------------------------------------------

double fdDdistDpPrmDir11(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return 0;  //true to second order (temporary)
}

double fdDdistDpPrmDir12(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return 0;  //true to second order (temporary)
}

double fdDdistDpPrmDir21(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return 0;  //true to second order (temporary)
}

double fdDdistDpPrmDir22(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return 0;  //true to second order (temporary)
}

double fdDdistDpPrmDir23(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return 0;  //true to second order (temporary)
}

double fdDdistDpPrmDir24(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return 0;  //true to second order (temporary)
}

double fdDdistDpPrmDir25(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f62, dAlpha;
  dAlpha = body[jBody].dSemi/body[iBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][61];
  f62 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][61] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][61]*dAlpha;

  return f62*(-body[jBody].dPinc*cos(fdLambdaArg(body,system,iBody,jBody,iIndexJ,2))\
          +body[jBody].dQinc*sin(fdLambdaArg(body,system,iBody,jBody,iIndexJ,2)));
}

double fdDdistDpPrmDir26(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f57, dAlpha;
  dAlpha = body[jBody].dSemi/body[iBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][56];
  f57 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][56] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][56]*dAlpha;
  
  return f57*(-2.*body[iBody].dPinc*cos(fdLambdaArg(body,system,iBody,jBody,iIndexJ,2))\
          +2*body[iBody].dQinc*sin(fdLambdaArg(body,system,iBody,jBody,iIndexJ,2)));
}

double fdDdistDpPrmDir37(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f88, dAlpha;
  dAlpha = body[jBody].dSemi/body[iBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][87];
  f88 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][87] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][87]*dAlpha;
  
  return f88*( (-body[jBody].dPinc*body[jBody].dKecc-body[jBody].dQinc*body[jBody].dHecc)\
          *cos(fdLambdaArg(body,system,iBody,jBody,iIndexJ,3))\
          +(-body[jBody].dPinc*body[jBody].dHecc+body[jBody].dQinc*body[jBody].dKecc)\
          *sin(fdLambdaArg(body,system,iBody,jBody,iIndexJ,3)));
}

double fdDdistDpPrmDir38(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f89, dAlpha;
  dAlpha = body[jBody].dSemi/body[iBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][88];
  f89 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][88] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][88]*dAlpha;
  
  return f89*( (-body[jBody].dPinc*body[iBody].dKecc-body[jBody].dQinc*body[iBody].dHecc)\
          *cos(fdLambdaArg(body,system,iBody,jBody,iIndexJ,3))\
          +(-body[jBody].dPinc*body[iBody].dHecc+body[jBody].dQinc*body[iBody].dKecc)\
          *sin(fdLambdaArg(body,system,iBody,jBody,iIndexJ,3)));
}

double fdDdistDpPrmDir39(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f86, dAlpha;
  dAlpha = body[jBody].dSemi/body[iBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][85];
  f86 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][85] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][85]*dAlpha;
  
  return f86*( (-2*body[jBody].dPinc*body[iBody].dKecc-2*body[iBody].dHecc*body[jBody].dQinc)\
          *cos(fdLambdaArg(body,system,iBody,jBody,iIndexJ,3))\
          +(-2*body[jBody].dPinc*body[iBody].dHecc+2*body[iBody].dKecc*body[jBody].dQinc)\
          *sin(fdLambdaArg(body,system,iBody,jBody,iIndexJ,3)));
}

double fdDdistDpPrmDir310(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f87, dAlpha;
  dAlpha = body[jBody].dSemi/body[iBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][86];
  f87 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][86] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][86]*dAlpha;
  
  return f87*( (-2*body[iBody].dPinc*body[iBody].dKecc-2*body[iBody].dHecc*body[iBody].dQinc)\
          *cos(fdLambdaArg(body,system,iBody,jBody,iIndexJ,3))\
          +(-2*body[iBody].dPinc*body[iBody].dHecc+2*body[iBody].dKecc*body[iBody].dQinc)\
          *sin(fdLambdaArg(body,system,iBody,jBody,iIndexJ,3)));
}

double fdDdistresDPincPrime(BODY *body, SYSTEM *system, int *iaBody, int iIndexJ, int iOrder) {
  if (iOrder == 1) {
    return 0;
  } else if (iOrder == 2) {
    double y, dMfac, dSemiPrm;

    dMfac = KGAUSS*KGAUSS*body[iaBody[1]].dMass/MSUN;
    dSemiPrm = body[iaBody[0]].dSemi/AUCM;
    y = ( fdDdistDpPrmDir25(body, system, iaBody[0], iaBody[1], iIndexJ) \
      + fdDdistDpPrmDir26(body, system, iaBody[0], iaBody[1], iIndexJ) ) * dMfac/dSemiPrm;
    
    return y;
  }
}

//---------dR/dq------------------------------------------------------------------

double fdDdistDqDir11(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return 0;  //true to second order (temporary)
}

double fdDdistDqDir12(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return 0;  //true to second order (temporary)
}

double fdDdistDqDir21(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return 0;  //true to second order (temporary)
}

double fdDdistDqDir22(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return 0;  //true to second order (temporary)
}

double fdDdistDqDir23(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return 0;  //true to second order (temporary)
}

double fdDdistDqDir24(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f57, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][56];
  f57 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][56] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][56]*dAlpha;

  return f57*(2.*body[iBody].dQinc*cos(fdLambdaArg(body,system,iBody,jBody,iIndexJ,2))\
          +2*body[iBody].dPinc*sin(fdLambdaArg(body,system,iBody,jBody,iIndexJ,2)));
}

double fdDdistDqDir25(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f62, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][61];
  f62 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][61] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][61]*dAlpha;

  return f62*(body[jBody].dQinc*cos(fdLambdaArg(body,system,iBody,jBody,iIndexJ,2))\
          +body[jBody].dPinc*sin(fdLambdaArg(body,system,iBody,jBody,iIndexJ,2)));
}

double fdDdistDqDir26(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return 0;  //true to second order (temporary)
}

double fdDdistDqDir35(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f86, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][85];
  f86 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][85] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][85]*dAlpha;
  
  return f86*((2*body[iBody].dKecc*body[iBody].dQinc-2*body[iBody].dHecc*body[iBody].dPinc)\
          *cos(fdLambdaArg(body,system,iBody,jBody,iIndexJ,3))\
          +(2*body[iBody].dHecc*body[iBody].dQinc+body[iBody].dKecc*body[iBody].dPinc)\
          *sin(fdLambdaArg(body,system,iBody,jBody,iIndexJ,3)));
}

double fdDdistDqDir36(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f87, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][86];
  f87 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][86] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][86]*dAlpha;
  
  return f87*((2*body[jBody].dKecc*body[iBody].dQinc-2*body[jBody].dHecc*body[iBody].dPinc)\
          *cos(fdLambdaArg(body,system,iBody,jBody,iIndexJ,3))\
          +(2*body[jBody].dHecc*body[iBody].dQinc+body[jBody].dKecc*body[iBody].dPinc)\
          *sin(fdLambdaArg(body,system,iBody,jBody,iIndexJ,3)));
}

double fdDdistDqDir37(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f88, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][87];
  f88 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][87] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][87]*dAlpha;
  
  return f88*( (body[jBody].dQinc*body[iBody].dKecc-body[jBody].dPinc*body[iBody].dHecc)\
          *cos(fdLambdaArg(body,system,iBody,jBody,iIndexJ,3))\
          +(body[jBody].dQinc*body[iBody].dHecc+body[jBody].dPinc*body[iBody].dKecc)\
          *sin(fdLambdaArg(body,system,iBody,jBody,iIndexJ,3)));
}

double fdDdistDqDir38(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f89, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][88];
  f89 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][88] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][88]*dAlpha;
  
  return f89*( (body[jBody].dQinc*body[jBody].dKecc-body[jBody].dPinc*body[jBody].dHecc)\
          *cos(fdLambdaArg(body,system,iBody,jBody,iIndexJ,3))\
          +(body[jBody].dQinc*body[jBody].dHecc+body[jBody].dPinc*body[jBody].dKecc)\
          *sin(fdLambdaArg(body,system,iBody,jBody,iIndexJ,3)));
}

double fdDdistresDQinc(BODY *body, SYSTEM *system, int *iaBody, int iIndexJ, int iOrder) {
  if (iOrder == 1) {
    return 0;
  } else if (iOrder==2) {
    double y, dMfac, dSemiPrm;
    dMfac = KGAUSS*KGAUSS*body[iaBody[1]].dMass/MSUN;
    dSemiPrm = body[iaBody[1]].dSemi/AUCM;
 
      y = ( fdDdistDqDir24(body, system, iaBody[0], iaBody[1], iIndexJ) \
      + fdDdistDqDir25(body, system, iaBody[0], iaBody[1], iIndexJ) ) * dMfac/dSemiPrm;
  
    return y;
  }
}

//---------dR/dq'------------------------------------------------------------------

double fdDdistDqPrmDir11(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return 0;  //true to second order (temporary)
}

double fdDdistDqPrmDir12(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return 0;  //true to second order (temporary)
}

double fdDdistDqPrmDir21(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return 0;  //true to second order (temporary)
}

double fdDdistDqPrmDir22(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return 0;  //true to second order (temporary)
}

double fdDdistDqPrmDir23(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return 0;  //true to second order (temporary)
}

double fdDdistDqPrmDir24(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return 0;  //true to second order (temporary)
}

double fdDdistDqPrmDir25(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f62, dAlpha;
  dAlpha = body[jBody].dSemi/body[iBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][61];
  f62 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][61] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][61]*dAlpha;

  return f62*(body[jBody].dQinc*cos(fdLambdaArg(body,system,iBody,jBody,iIndexJ,2))\
          +body[jBody].dPinc*sin(fdLambdaArg(body,system,iBody,jBody,iIndexJ,2)));
}

double fdDdistDqPrmDir26(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f57, dAlpha;
  dAlpha = body[jBody].dSemi/body[iBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][56];
  f57 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][56] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][56]*dAlpha;

  return f57*(2.*body[iBody].dQinc*cos(fdLambdaArg(body,system,iBody,jBody,iIndexJ,2))\
          +2*body[iBody].dPinc*sin(fdLambdaArg(body,system,iBody,jBody,iIndexJ,2)));
}

double fdDdistDqPrmDir37(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f88, dAlpha;
  dAlpha = body[jBody].dSemi/body[iBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][87];
  f88 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][87] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][87]*dAlpha;
  
  return f88*( (body[jBody].dQinc*body[jBody].dKecc-body[jBody].dPinc*body[jBody].dHecc)\
          *cos(fdLambdaArg(body,system,iBody,jBody,iIndexJ,3))\
          +(body[jBody].dQinc*body[jBody].dHecc+body[jBody].dPinc*body[jBody].dKecc)\
          *sin(fdLambdaArg(body,system,iBody,jBody,iIndexJ,3)));
}

double fdDdistDqPrmDir38(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f89, dAlpha;
  dAlpha = body[jBody].dSemi/body[iBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][88];
  f89 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][88] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][88]*dAlpha;
  
  return f89*( (body[jBody].dQinc*body[iBody].dKecc-body[jBody].dPinc*body[iBody].dHecc)\
          *cos(fdLambdaArg(body,system,iBody,jBody,iIndexJ,3))\
          +(body[jBody].dQinc*body[iBody].dHecc+body[jBody].dPinc*body[iBody].dKecc)\
          *sin(fdLambdaArg(body,system,iBody,jBody,iIndexJ,3)));
}

double fdDdistDqPrmDir39(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f86, dAlpha;
  dAlpha = body[jBody].dSemi/body[iBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][85];
  f86 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][85] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][85]*dAlpha;
  
  return f86*( (2*body[jBody].dQinc*body[iBody].dKecc-2*body[iBody].dHecc*body[jBody].dPinc)\
          *cos(fdLambdaArg(body,system,iBody,jBody,iIndexJ,3))\
          +(2*body[jBody].dQinc*body[iBody].dHecc+2*body[iBody].dKecc*body[jBody].dPinc)\
          *sin(fdLambdaArg(body,system,iBody,jBody,iIndexJ,3)));
}

double fdDdistDqPrmDir310(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f87, dAlpha;
  dAlpha = body[jBody].dSemi/body[iBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][86];
  f87 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][86] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][86]*dAlpha;
  
  return f87*( (2*body[iBody].dQinc*body[iBody].dKecc-2*body[iBody].dHecc*body[iBody].dPinc)\
          *cos(fdLambdaArg(body,system,iBody,jBody,iIndexJ,3))\
          +(2*body[iBody].dQinc*body[iBody].dHecc+2*body[iBody].dKecc*body[iBody].dPinc)\
          *sin(fdLambdaArg(body,system,iBody,jBody,iIndexJ,3)));
}

double fdDdistresDQincPrime(BODY *body, SYSTEM *system, int *iaBody, int iIndexJ, int iOrder) {
  if (iOrder == 1) {
    return 0;
  } else if (iOrder == 2) {
    double y, dMfac, dSemiPrm;
    dMfac = KGAUSS*KGAUSS*body[iaBody[1]].dMass/MSUN;
    dSemiPrm = body[iaBody[0]].dSemi/AUCM;
    y = ( fdDdistDqPrmDir25(body, system, iaBody[0], iaBody[1], iIndexJ) \
      + fdDdistDqPrmDir26(body, system, iaBody[0], iaBody[1], iIndexJ) ) * dMfac/dSemiPrm;
    return y;
  }
}

//---------dR/dlambda------------------------------------------------------------------

double fdDdistDlDir11(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f27, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][26];
  f27 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][26] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][26]*dAlpha;
  
  return (1.-iIndexJ)*f27\
          *(-body[iBody].dKecc*sin(fdLambdaArg(body,system,iBody,jBody,iIndexJ,1))\
          +(body[iBody].dHecc*cos(fdLambdaArg(body,system,iBody,jBody,iIndexJ,1))));
}

double fdDdistDlDir12(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f31, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][30];
  f31 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][30] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][30]*dAlpha;  

  return (1.-iIndexJ)*f31\
          *(-body[jBody].dKecc*sin(fdLambdaArg(body,system,iBody,jBody,iIndexJ,1))\
          +(body[jBody].dHecc*cos(fdLambdaArg(body,system,iBody,jBody,iIndexJ,1))));
}

double fdDdistDlExt13(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return 2*(-body[jBody].dKecc*sin(fdLambdaArg(body,system,iBody,jBody,iIndexJ,1))+\
          body[jBody].dHecc*cos(fdLambdaArg(body,system,iBody,jBody,iIndexJ,1)));
}

double fdDdistDlDir21(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f45, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][44];
  f45 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][44] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][44]*dAlpha;  
  
  return (2.-iIndexJ)*f45\
          *(-(body[iBody].dKecc*body[iBody].dKecc-body[iBody].dHecc*body[iBody].dHecc)\
          *sin(fdLambdaArg(body,system,iBody,jBody,iIndexJ,2))\
          +2*(body[iBody].dHecc*body[iBody].dKecc*cos(fdLambdaArg(body,system,iBody,jBody,iIndexJ,2))));
}

double fdDdistDlDir22(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f49, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][48];
  f49 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][48] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][48]*dAlpha;  
  
  return (2.-iIndexJ)*f49\
          *(-(body[iBody].dKecc*body[jBody].dKecc-body[iBody].dHecc*body[jBody].dHecc)\
          *sin(fdLambdaArg(body,system,iBody,jBody,iIndexJ,2))\
          +(body[iBody].dHecc*body[jBody].dKecc+body[iBody].dKecc*body[jBody].dHecc)\
          *cos(fdLambdaArg(body,system,iBody,jBody,iIndexJ,2)));
}

double fdDdistDlDir23(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f53, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][52];
  f53 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][52] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][52]*dAlpha;  
  
  return (2.-iIndexJ)*f53\
          *(-(body[jBody].dKecc*body[jBody].dKecc-body[jBody].dHecc*body[jBody].dHecc)\
          *sin(fdLambdaArg(body,system,iBody,jBody,iIndexJ,2))\
          +2*(body[jBody].dHecc*body[jBody].dKecc*cos(fdLambdaArg(body,system,iBody,jBody,iIndexJ,2))));
}

double fdDdistDlDir24(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f57, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][56];
  f57 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][56] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][56]*dAlpha;  
  
  return (2.-iIndexJ)*f57\
          *(-(body[iBody].dQinc*body[iBody].dQinc-body[iBody].dPinc*body[iBody].dPinc)\
          *sin(fdLambdaArg(body,system,iBody,jBody,iIndexJ,2))\
          +2*(body[iBody].dPinc*body[iBody].dQinc*cos(fdLambdaArg(body,system,iBody,jBody,iIndexJ,2))));
}

double fdDdistDlDir25(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f62, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][61];
  f62 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][61] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][61]*dAlpha; 
  
  return (2.-iIndexJ)*f62\
          *(-(body[iBody].dQinc*body[jBody].dQinc-body[iBody].dPinc*body[jBody].dPinc)\
          *sin(fdLambdaArg(body,system,iBody,jBody,iIndexJ,2))\
          +((body[jBody].dPinc*body[iBody].dQinc+body[iBody].dPinc*body[jBody].dQinc)\
          *cos(fdLambdaArg(body,system,iBody,jBody,iIndexJ,2))));
}

double fdDdistDlDir26(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f57, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][56];
  f57 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][56] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][56]*dAlpha; 
  
  return (2.-iIndexJ)*f57\
          *(-(body[jBody].dQinc*body[jBody].dQinc-body[jBody].dPinc*body[jBody].dPinc)\
          *sin(fdLambdaArg(body,system,iBody,jBody,iIndexJ,2))\
          +2*(body[jBody].dPinc*body[jBody].dQinc*cos(fdLambdaArg(body,system,iBody,jBody,iIndexJ,2))));
}

double fdDdistDlDir31(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f82, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][81];
  f82 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][81] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][81]*dAlpha;
        
  return (3-iIndexJ)*f82*(-sin(fdLambdaArg(body,system,iBody,jBody,iIndexJ,3))\
          *(body[iBody].dKecc*body[iBody].dKecc*body[iBody].dKecc\
          -3*body[iBody].dHecc*body[iBody].dHecc*body[iBody].dKecc)
          +cos(fdLambdaArg(body,system,iBody,jBody,iIndexJ,3))\
          *(-body[iBody].dHecc*body[iBody].dHecc*body[iBody].dHecc\
          +3*body[iBody].dKecc*body[iBody].dKecc*body[iBody].dHecc));
}

double fdDdistDlDir32(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f83, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][82];
  f83 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][82] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][82]*dAlpha;
        
  return (3-iIndexJ)*f83*(-sin(fdLambdaArg(body,system,iBody,jBody,iIndexJ,3))\
          *(body[jBody].dKecc*(body[iBody].dKecc*body[iBody].dKecc-body[iBody].dHecc*body[iBody].dHecc)\
          -2*body[jBody].dHecc*body[iBody].dHecc*body[iBody].dKecc)
          +cos(fdLambdaArg(body,system,iBody,jBody,iIndexJ,3))\
          *(body[jBody].dHecc*(body[iBody].dKecc*body[iBody].dKecc-body[iBody].dHecc*body[iBody].dHecc)\
          +2*body[jBody].dKecc*body[iBody].dKecc*body[iBody].dHecc));
}

double fdDdistDlDir33(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {        
  double f84, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][83];
  f84 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][83] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][83]*dAlpha; 
 
  return (3-iIndexJ)*f84*(-sin(fdLambdaArg(body,system,iBody,jBody,iIndexJ,3))\
          *(body[iBody].dKecc*(body[jBody].dKecc*body[jBody].dKecc-body[jBody].dHecc*body[jBody].dHecc)\
          -2*body[iBody].dHecc*body[jBody].dHecc*body[jBody].dKecc)
          +cos(fdLambdaArg(body,system,iBody,jBody,iIndexJ,3))\
          *(body[iBody].dHecc*(body[jBody].dKecc*body[jBody].dKecc-body[jBody].dHecc*body[jBody].dHecc)\
          +2*body[iBody].dKecc*body[jBody].dKecc*body[jBody].dHecc));
}

double fdDdistDlDir34(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {        
  double f85, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][84];
  f85 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][84] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][84]*dAlpha; 
  
  return (3-iIndexJ)*f85*(-sin(fdLambdaArg(body,system,iBody,jBody,iIndexJ,3))\
          *(body[jBody].dKecc*body[jBody].dKecc*body[jBody].dKecc\
          -3*body[jBody].dHecc*body[jBody].dHecc*body[jBody].dKecc)
          +cos(fdLambdaArg(body,system,iBody,jBody,iIndexJ,3))\
          *(-body[jBody].dHecc*body[jBody].dHecc*body[jBody].dHecc\
          +3*body[jBody].dKecc*body[jBody].dKecc*body[jBody].dHecc));
}

double fdDdistDlDir35(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {        
  double f86, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][85];
  f86 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][85] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][85]*dAlpha; 
 
  return (3-iIndexJ)*f86*(-sin(fdLambdaArg(body,system,iBody,jBody,iIndexJ,3))\
          *(body[iBody].dKecc*(body[iBody].dQinc*body[iBody].dQinc-body[iBody].dPinc*body[iBody].dPinc)\
          -2*body[iBody].dHecc*body[iBody].dPinc*body[iBody].dQinc)
          +cos(fdLambdaArg(body,system,iBody,jBody,iIndexJ,3))\
          *(body[iBody].dHecc*(body[iBody].dQinc*body[iBody].dQinc-body[iBody].dPinc*body[iBody].dPinc)\
          +2*body[iBody].dKecc*body[iBody].dPinc*body[iBody].dQinc));
}

double fdDdistDlDir36(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {        
  double f87, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][86];
  f87 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][86] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][86]*dAlpha; 
 
  return (3-iIndexJ)*f87*(-sin(fdLambdaArg(body,system,iBody,jBody,iIndexJ,3))\
          *(body[iBody].dKecc*(body[iBody].dQinc*body[iBody].dQinc-body[iBody].dPinc*body[iBody].dPinc)\
          -2*body[iBody].dHecc*body[iBody].dPinc*body[iBody].dQinc)
          +cos(fdLambdaArg(body,system,iBody,jBody,iIndexJ,3))\
          *(body[iBody].dHecc*(body[iBody].dQinc*body[iBody].dQinc-body[iBody].dPinc*body[iBody].dPinc)\
          +2*body[iBody].dKecc*body[iBody].dPinc*body[iBody].dQinc));
}

double fdDdistDlDir37(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {        
  double f88, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][87];
  f88 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][87] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][87]*dAlpha;
  
  return (3-iIndexJ)*f88*(-sin(fdLambdaArg(body,system,iBody,jBody,iIndexJ,3))\
          *(body[iBody].dKecc*(body[jBody].dQinc*body[iBody].dQinc-body[jBody].dPinc*body[iBody].dPinc)\
          -body[iBody].dHecc*(body[jBody].dPinc*body[iBody].dQinc+body[jBody].dQinc*body[iBody].dPinc))
          +cos(fdLambdaArg(body,system,iBody,jBody,iIndexJ,3))\
          *(body[iBody].dHecc*(body[jBody].dQinc*body[iBody].dQinc-body[jBody].dPinc*body[iBody].dPinc)\
          +body[iBody].dKecc*(body[jBody].dPinc*body[iBody].dQinc+body[jBody].dQinc*body[iBody].dPinc)));
}

double fdDdistDlDir38(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {        
  double f89, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][88];
  f89 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][88] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][88]*dAlpha;
  
  return (3-iIndexJ)*f89*(-sin(fdLambdaArg(body,system,iBody,jBody,iIndexJ,3))\
          *(body[jBody].dKecc*(body[jBody].dQinc*body[iBody].dQinc-body[jBody].dPinc*body[iBody].dPinc)\
          -body[jBody].dHecc*(body[jBody].dPinc*body[iBody].dQinc+body[jBody].dQinc*body[iBody].dPinc))
          +cos(fdLambdaArg(body,system,iBody,jBody,iIndexJ,3))\
          *(body[jBody].dHecc*(body[jBody].dQinc*body[iBody].dQinc-body[jBody].dPinc*body[iBody].dPinc)\
          +body[jBody].dKecc*(body[jBody].dPinc*body[iBody].dQinc+body[jBody].dQinc*body[iBody].dPinc)));
}

double fdDdistDlDir39(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {        
  double f86, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][85];
  f86 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][85] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][85]*dAlpha;  
 
  return (3-iIndexJ)*f86*(-sin(fdLambdaArg(body,system,iBody,jBody,iIndexJ,3))\
          *(body[iBody].dKecc*(body[jBody].dQinc*body[jBody].dQinc-body[jBody].dPinc*body[jBody].dPinc)\
          -2*body[iBody].dHecc*body[jBody].dPinc*body[jBody].dQinc)
          +cos(fdLambdaArg(body,system,iBody,jBody,iIndexJ,3))\
          *(body[iBody].dHecc*(body[jBody].dQinc*body[jBody].dQinc-body[jBody].dPinc*body[jBody].dPinc)\
          +2*body[iBody].dKecc*body[jBody].dPinc*body[jBody].dQinc));
}

double fdDdistDlDir310(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {        
  double f87, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][86];
  f87 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][86] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][86]*dAlpha; 
  
  return (3-iIndexJ)*f87*(-sin(fdLambdaArg(body,system,iBody,jBody,iIndexJ,3))\
          *(body[jBody].dKecc*(body[jBody].dQinc*body[jBody].dQinc-body[jBody].dPinc*body[jBody].dPinc)\
          -2*body[jBody].dHecc*body[jBody].dPinc*body[jBody].dQinc)
          +cos(fdLambdaArg(body,system,iBody,jBody,iIndexJ,3))\
          *(body[jBody].dHecc*(body[jBody].dQinc*body[jBody].dQinc-body[jBody].dPinc*body[jBody].dPinc)\
          +2*body[jBody].dKecc*body[jBody].dPinc*body[jBody].dQinc));
}

double fdDdistresDLambda(BODY *body, SYSTEM *system, int *iaBody, int iIndexJ, int iOrder) {
  double y, dMfac, dSemiPrm;
  dMfac = KGAUSS*KGAUSS*body[iaBody[1]].dMass/MSUN;
  dSemiPrm = body[iaBody[1]].dSemi/AUCM;
  if (iOrder == 1) {
    y = ( fdDdistDlDir11(body, system, iaBody[0], iaBody[1], iIndexJ)\
      + fdDdistDlDir12(body, system, iaBody[0], iaBody[1], iIndexJ) ) * dMfac/dSemiPrm;
    if (iIndexJ == 2) {
      y += fdDdistDlExt13(body,system,iaBody[0],iaBody[1],iIndexJ)*dMfac/dSemiPrm\
            *body[iaBody[0]].dSemi/body[iaBody[1]].dSemi;
    }
  } else if (iOrder == 2) {
    y = ( fdDdistDlDir21(body, system, iaBody[0], iaBody[1], iIndexJ) \
    + fdDdistDlDir22(body, system, iaBody[0], iaBody[1], iIndexJ) \
    + fdDdistDlDir23(body, system, iaBody[0], iaBody[1], iIndexJ) \
    + fdDdistDlDir24(body, system, iaBody[0], iaBody[1], iIndexJ) \
    + fdDdistDlDir25(body, system, iaBody[0], iaBody[1], iIndexJ) \
    + fdDdistDlDir26(body, system, iaBody[0], iaBody[1], iIndexJ) ) * dMfac/dSemiPrm;
  } else if (iOrder == 3) {
    y = ( fdDdistDlDir31(body, system, iaBody[0], iaBody[1], iIndexJ) \
    + fdDdistDlDir32(body, system, iaBody[0], iaBody[1], iIndexJ) \
    + fdDdistDlDir33(body, system, iaBody[0], iaBody[1], iIndexJ) \
    + fdDdistDlDir34(body, system, iaBody[0], iaBody[1], iIndexJ) \
    + fdDdistDlDir35(body, system, iaBody[0], iaBody[1], iIndexJ) \
    + fdDdistDlDir36(body, system, iaBody[0], iaBody[1], iIndexJ) \
    + fdDdistDlDir37(body, system, iaBody[0], iaBody[1], iIndexJ) \
    + fdDdistDlDir38(body, system, iaBody[0], iaBody[1], iIndexJ) \
    + fdDdistDlDir39(body, system, iaBody[0], iaBody[1], iIndexJ) \
    + fdDdistDlDir310(body, system, iaBody[0], iaBody[1], iIndexJ) ) * dMfac/dSemiPrm;
  }
  return y;
}

//---------dR/dlambda'------------------------------------------------------------------

double fdDdistDlPrmDir11(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return (1.*iIndexJ)/(1.-iIndexJ)*fdDdistDlDir11(body,system,jBody,iBody,iIndexJ);
}

double fdDdistDlPrmDir12(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return (1.*iIndexJ)/(1.-iIndexJ)*fdDdistDlDir12(body,system,jBody,iBody,iIndexJ);
}

double fdDdistDlPrmInt13(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return body[iBody].dKecc*sin(fdLambdaArg(body,system,jBody,iBody,iIndexJ,1))-\
          body[iBody].dHecc*cos(fdLambdaArg(body,system,jBody,iBody,iIndexJ,1));
}

double fdDdistDlPrmDir21(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return (1.*iIndexJ)/(2.-iIndexJ)*fdDdistDlDir21(body,system,jBody,iBody,iIndexJ);
}

double fdDdistDlPrmDir22(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return (1.*iIndexJ)/(2.-iIndexJ)*fdDdistDlDir22(body,system,jBody,iBody,iIndexJ);
}

double fdDdistDlPrmDir23(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return (1.*iIndexJ)/(2.-iIndexJ)*fdDdistDlDir23(body,system,jBody,iBody,iIndexJ);
}

double fdDdistDlPrmDir24(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return (1.*iIndexJ)/(2.-iIndexJ)*fdDdistDlDir24(body,system,jBody,iBody,iIndexJ);
}

double fdDdistDlPrmDir25(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return (1.*iIndexJ)/(2.-iIndexJ)*fdDdistDlDir25(body,system,jBody,iBody,iIndexJ);
}

double fdDdistDlPrmDir26(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return (1.*iIndexJ)/(2.-iIndexJ)*fdDdistDlDir26(body,system,jBody,iBody,iIndexJ);
}

double fdDdistDlPrmDir31(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return (1.*iIndexJ)/(3.-iIndexJ)*fdDdistDlDir31(body,system,jBody,iBody,iIndexJ);
}

double fdDdistDlPrmDir32(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return (1.*iIndexJ)/(3.-iIndexJ)*fdDdistDlDir32(body,system,jBody,iBody,iIndexJ);
}

double fdDdistDlPrmDir33(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return (1.*iIndexJ)/(3.-iIndexJ)*fdDdistDlDir33(body,system,jBody,iBody,iIndexJ);
}

double fdDdistDlPrmDir34(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return (1.*iIndexJ)/(3.-iIndexJ)*fdDdistDlDir34(body,system,jBody,iBody,iIndexJ);
}

double fdDdistDlPrmDir35(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return (1.*iIndexJ)/(3.-iIndexJ)*fdDdistDlDir35(body,system,jBody,iBody,iIndexJ);
}

double fdDdistDlPrmDir36(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return (1.*iIndexJ)/(3.-iIndexJ)*fdDdistDlDir36(body,system,jBody,iBody,iIndexJ);
}

double fdDdistDlPrmDir37(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return (1.*iIndexJ)/(3.-iIndexJ)*fdDdistDlDir37(body,system,jBody,iBody,iIndexJ);
}

double fdDdistDlPrmDir38(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return (1.*iIndexJ)/(3.-iIndexJ)*fdDdistDlDir38(body,system,jBody,iBody,iIndexJ);
}

double fdDdistDlPrmDir39(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return (1.*iIndexJ)/(3.-iIndexJ)*fdDdistDlDir39(body,system,jBody,iBody,iIndexJ);
}

double fdDdistDlPrmDir310(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return (1.*iIndexJ)/(3.-iIndexJ)*fdDdistDlDir310(body,system,jBody,iBody,iIndexJ);
}

double fdDdistresDLambdaPrime(BODY *body, SYSTEM *system, int *iaBody, int iIndexJ, int iOrder) {
  double y, dMfac, dSemiPrm;
  dMfac = KGAUSS*KGAUSS*body[iaBody[1]].dMass/MSUN;
  dSemiPrm = body[iaBody[0]].dSemi/AUCM;
  if (iOrder == 1) { 
    y = ( fdDdistDlPrmDir11(body, system, iaBody[0], iaBody[1], iIndexJ)\
      + fdDdistDlPrmDir12(body, system, iaBody[0], iaBody[1], iIndexJ) ) * dMfac/dSemiPrm;
    if (iIndexJ == 2) {
       y += fdDdistDlPrmInt13(body, system, iaBody[0], iaBody[1], iIndexJ) * dMfac*dSemiPrm\
            /(body[iaBody[1]].dSemi*body[iaBody[1]].dSemi/AUCM/AUCM);
    }
  } else if (iOrder == 2) {
    y = ( fdDdistDlPrmDir21(body, system, iaBody[0], iaBody[1], iIndexJ) \
    + fdDdistDlPrmDir22(body, system, iaBody[0], iaBody[1], iIndexJ)
    + fdDdistDlPrmDir23(body, system, iaBody[0], iaBody[1], iIndexJ) \
    + fdDdistDlPrmDir24(body, system, iaBody[0], iaBody[1], iIndexJ)
    + fdDdistDlPrmDir25(body, system, iaBody[0], iaBody[1], iIndexJ) \
    + fdDdistDlPrmDir26(body, system, iaBody[0], iaBody[1], iIndexJ) ) * dMfac/dSemiPrm;
  } else if (iOrder == 3) {
    y = ( fdDdistDlPrmDir31(body, system, iaBody[0], iaBody[1], iIndexJ) \
    + fdDdistDlPrmDir32(body, system, iaBody[0], iaBody[1], iIndexJ) \
    + fdDdistDlPrmDir33(body, system, iaBody[0], iaBody[1], iIndexJ) \
    + fdDdistDlPrmDir34(body, system, iaBody[0], iaBody[1], iIndexJ) \
    + fdDdistDlPrmDir35(body, system, iaBody[0], iaBody[1], iIndexJ) \
    + fdDdistDlPrmDir36(body, system, iaBody[0], iaBody[1], iIndexJ) \
    + fdDdistDlPrmDir37(body, system, iaBody[0], iaBody[1], iIndexJ) \
    + fdDdistDlPrmDir38(body, system, iaBody[0], iaBody[1], iIndexJ) \
    + fdDdistDlPrmDir39(body, system, iaBody[0], iaBody[1], iIndexJ) \
    + fdDdistDlPrmDir310(body, system, iaBody[0], iaBody[1], iIndexJ) ) * dMfac/dSemiPrm;
  }
  return y;
}

//---------dR/da------------------------------------------------------------------
double fdDdistDaDir01(BODY *body, SYSTEM *system, int iBody, int jBody) {
  return (system->dmLaplaceD[0][system->imLaplaceN[iBody][jBody]][0] + \
          system->dmLaplaceD[0][system->imLaplaceN[iBody][jBody]][1] * \
          (body[iBody].dKecc*body[iBody].dKecc+body[jBody].dKecc*body[jBody].dKecc+\
          body[iBody].dHecc*body[iBody].dHecc+body[jBody].dHecc*body[jBody].dHecc) +\
          system->dmLaplaceD[0][system->imLaplaceN[iBody][jBody]][2] * \
          (body[iBody].dPinc*body[iBody].dPinc+body[jBody].dPinc*body[jBody].dPinc+\
          body[iBody].dQinc*body[iBody].dQinc+body[jBody].dQinc*body[jBody].dQinc))\
          /body[jBody].dSemi*AUCM;
}

double fdDdistDaDir02(BODY *body, SYSTEM *system, int iBody, int jBody) {
  return (system->dmLaplaceD[0][system->imLaplaceN[iBody][jBody]][9] * \
          (body[iBody].dKecc*body[jBody].dKecc+body[iBody].dHecc*body[jBody].dHecc))\
          /body[jBody].dSemi*AUCM;
}

double fdDdistDaDir03(BODY *body, SYSTEM *system, int iBody, int jBody) {
  return (system->dmLaplaceD[0][system->imLaplaceN[iBody][jBody]][13] * \
          (body[iBody].dPinc*body[jBody].dPinc+body[iBody].dQinc*body[jBody].dQinc))\
          /body[jBody].dSemi*AUCM;
}

double fdDdistDaDir11(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return AUCM/body[jBody].dSemi*system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][26] \
         *(body[iBody].dKecc*cos(fdLambdaArg(body,system,iBody,jBody,iIndexJ,1))\
         +body[iBody].dHecc*sin(fdLambdaArg(body,system,iBody,jBody,iIndexJ,1)));
}

double fdDdistDaDir12(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return AUCM/body[jBody].dSemi*system->dmLaplaceD[iIndexJ][system->imLaplaceN[jBody][iBody]][30] \
         *(body[jBody].dKecc*cos(fdLambdaArg(body,system,iBody,jBody,iIndexJ,1))\
         +body[jBody].dHecc*sin(fdLambdaArg(body,system,iBody,jBody,iIndexJ,1)));
}

double fdDdistDaExt13(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return -AUCM/body[jBody].dSemi*2*(body[jBody].dKecc*\
      cos(fdLambdaArg(body,system,iBody,jBody,iIndexJ,1))+body[jBody].dHecc*\
      sin(fdLambdaArg(body,system,iBody,jBody,iIndexJ,1)));
}

double fdDdistDaDir21(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return AUCM/body[jBody].dSemi*system->dmLaplaceD[iIndexJ][system->imLaplaceN[jBody][iBody]][44]\
          *((body[iBody].dKecc*body[iBody].dKecc-body[iBody].dHecc*body[iBody].dHecc)\
          *cos(fdLambdaArg(body,system,iBody,jBody,iIndexJ,2))\
          +2*(body[iBody].dHecc*body[iBody].dKecc*sin(fdLambdaArg(body,system,iBody,jBody,iIndexJ,2))));
}

double fdDdistDaDir22(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return AUCM/body[jBody].dSemi*system->dmLaplaceD[iIndexJ][system->imLaplaceN[jBody][iBody]][48]\
          *((body[iBody].dKecc*body[jBody].dKecc-body[iBody].dHecc*body[jBody].dHecc)\
          *cos(fdLambdaArg(body,system,iBody,jBody,iIndexJ,2))\
          +(body[iBody].dHecc*body[jBody].dKecc+body[iBody].dKecc*body[jBody].dHecc)\
          *sin(fdLambdaArg(body,system,iBody,jBody,iIndexJ,2)));
}

double fdDdistDaDir23(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return AUCM/body[jBody].dSemi*system->dmLaplaceD[iIndexJ][system->imLaplaceN[jBody][iBody]][52]\
          *((body[jBody].dKecc*body[jBody].dKecc-body[jBody].dHecc*body[jBody].dHecc)\
          *cos(fdLambdaArg(body,system,iBody,jBody,iIndexJ,2))\
          +2*(body[jBody].dHecc*body[jBody].dKecc*sin(fdLambdaArg(body,system,iBody,jBody,iIndexJ,2))));
}

double fdDdistDaDir24(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return AUCM/body[jBody].dSemi*system->dmLaplaceD[iIndexJ][system->imLaplaceN[jBody][iBody]][56]\
          *((body[iBody].dQinc*body[iBody].dQinc-body[iBody].dPinc*body[iBody].dPinc)\
          *cos(fdLambdaArg(body,system,iBody,jBody,iIndexJ,2))\
          +2*(body[iBody].dPinc*body[iBody].dQinc*sin(fdLambdaArg(body,system,iBody,jBody,iIndexJ,2))));
}

double fdDdistDaDir25(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return AUCM/body[jBody].dSemi*system->dmLaplaceD[iIndexJ][system->imLaplaceN[jBody][iBody]][61]\
          *((body[iBody].dQinc*body[jBody].dQinc-body[iBody].dPinc*body[jBody].dPinc)\
          *cos(fdLambdaArg(body,system,iBody,jBody,iIndexJ,2))\
          +((body[jBody].dPinc*body[iBody].dQinc+body[iBody].dPinc*body[jBody].dQinc)\
          *sin(fdLambdaArg(body,system,iBody,jBody,iIndexJ,2))));
}

double fdDdistDaDir26(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return AUCM/body[jBody].dSemi*system->dmLaplaceD[iIndexJ][system->imLaplaceN[jBody][iBody]][56]\
          *((body[jBody].dQinc*body[jBody].dQinc-body[jBody].dPinc*body[jBody].dPinc)\
          *cos(fdLambdaArg(body,system,iBody,jBody,iIndexJ,2))\
          +2*(body[jBody].dPinc*body[jBody].dQinc*sin(fdLambdaArg(body,system,iBody,jBody,iIndexJ,2))));
}

double fdDdistDaDir31(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return AUCM/body[jBody].dSemi*system->dmLaplaceD[iIndexJ][system->imLaplaceN[jBody][iBody]][81]\
          *(cos(fdLambdaArg(body,system,iBody,jBody,iIndexJ,3))\
          *(body[iBody].dKecc*body[iBody].dKecc*body[iBody].dKecc\
          -3*body[iBody].dHecc*body[iBody].dHecc*body[iBody].dKecc)\
          +sin(fdLambdaArg(body,system,iBody,jBody,iIndexJ,3))\
          *(-body[iBody].dHecc*body[iBody].dHecc*body[iBody].dHecc\
          +3*body[iBody].dKecc*body[iBody].dKecc*body[iBody].dHecc));
}

double fdDdistDaDir32(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {        
  return AUCM/body[jBody].dSemi*system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][82]\
          *(cos(fdLambdaArg(body,system,iBody,jBody,iIndexJ,3))\
          *(body[jBody].dKecc*(body[iBody].dKecc*body[iBody].dKecc-body[iBody].dHecc*body[iBody].dHecc)\
          -2*body[jBody].dHecc*body[iBody].dHecc*body[iBody].dKecc)
          +sin(fdLambdaArg(body,system,iBody,jBody,iIndexJ,3))\
          *(body[jBody].dHecc*(body[iBody].dKecc*body[iBody].dKecc-body[iBody].dHecc*body[iBody].dHecc)\
          +2*body[jBody].dKecc*body[iBody].dKecc*body[iBody].dHecc));
}

double fdDdistDaDir33(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {        
  return AUCM/body[jBody].dSemi*system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][83]\
          *(cos(fdLambdaArg(body,system,iBody,jBody,iIndexJ,3))\
          *(body[iBody].dKecc*(body[jBody].dKecc*body[jBody].dKecc-body[jBody].dHecc*body[jBody].dHecc)\
          -2*body[iBody].dHecc*body[jBody].dHecc*body[jBody].dKecc)
          +sin(fdLambdaArg(body,system,iBody,jBody,iIndexJ,3))\
          *(body[iBody].dHecc*(body[jBody].dKecc*body[jBody].dKecc-body[jBody].dHecc*body[jBody].dHecc)\
          +2*body[iBody].dKecc*body[jBody].dKecc*body[jBody].dHecc));
}

double fdDdistDaDir34(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {        
  return AUCM/body[jBody].dSemi*system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][84]\
          *(cos(fdLambdaArg(body,system,iBody,jBody,iIndexJ,3))\
          *(body[jBody].dKecc*body[jBody].dKecc*body[jBody].dKecc\
          -3*body[jBody].dHecc*body[jBody].dHecc*body[jBody].dKecc)
          +sin(fdLambdaArg(body,system,iBody,jBody,iIndexJ,3))\
          *(-body[jBody].dHecc*body[jBody].dHecc*body[jBody].dHecc\
          +3*body[jBody].dKecc*body[jBody].dKecc*body[jBody].dHecc));
}

double fdDdistDaDir35(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {        
  return AUCM/body[jBody].dSemi*system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][85]\
          *(cos(fdLambdaArg(body,system,iBody,jBody,iIndexJ,3))\
          *(body[iBody].dKecc*(body[iBody].dQinc*body[iBody].dQinc-body[iBody].dPinc*body[iBody].dPinc)\
          -2*body[iBody].dHecc*body[iBody].dPinc*body[iBody].dQinc)
          +sin(fdLambdaArg(body,system,iBody,jBody,iIndexJ,3))\
          *(body[iBody].dHecc*(body[iBody].dQinc*body[iBody].dQinc-body[iBody].dPinc*body[iBody].dPinc)\
          +2*body[iBody].dKecc*body[iBody].dPinc*body[iBody].dQinc));
}

double fdDdistDaDir36(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {        
  return AUCM/body[jBody].dSemi*system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][86]\
          *(cos(fdLambdaArg(body,system,iBody,jBody,iIndexJ,3))\
          *(body[jBody].dKecc*(body[iBody].dQinc*body[iBody].dQinc-body[iBody].dPinc*body[iBody].dPinc)\
          -2*body[jBody].dHecc*body[iBody].dPinc*body[iBody].dQinc)
          +sin(fdLambdaArg(body,system,iBody,jBody,iIndexJ,3))\
          *(body[jBody].dHecc*(body[iBody].dQinc*body[iBody].dQinc-body[iBody].dPinc*body[iBody].dPinc)\
          +2*body[jBody].dKecc*body[iBody].dPinc*body[iBody].dQinc));
}

double fdDdistDaDir37(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {        
  return AUCM/body[jBody].dSemi*system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][87]\
          *(cos(fdLambdaArg(body,system,iBody,jBody,iIndexJ,3))\
          *(body[iBody].dKecc*(body[jBody].dQinc*body[iBody].dQinc-body[jBody].dPinc*body[iBody].dPinc)\
          -body[iBody].dHecc*(body[jBody].dPinc*body[iBody].dQinc+body[jBody].dQinc*body[iBody].dPinc))
          +sin(fdLambdaArg(body,system,iBody,jBody,iIndexJ,3))\
          *(body[iBody].dHecc*(body[jBody].dQinc*body[iBody].dQinc-body[jBody].dPinc*body[iBody].dPinc)\
          +body[iBody].dKecc*(body[jBody].dPinc*body[iBody].dQinc+body[jBody].dQinc*body[iBody].dPinc)));
}

double fdDdistDaDir38(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {        
  return AUCM/body[jBody].dSemi*system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][88]\
          *(cos(fdLambdaArg(body,system,iBody,jBody,iIndexJ,3))\
          *(body[jBody].dKecc*(body[jBody].dQinc*body[iBody].dQinc-body[jBody].dPinc*body[iBody].dPinc)\
          -body[jBody].dHecc*(body[jBody].dPinc*body[iBody].dQinc+body[jBody].dQinc*body[iBody].dPinc))
          +sin(fdLambdaArg(body,system,iBody,jBody,iIndexJ,3))\
          *(body[jBody].dHecc*(body[jBody].dQinc*body[iBody].dQinc-body[jBody].dPinc*body[iBody].dPinc)\
          +body[jBody].dKecc*(body[jBody].dPinc*body[iBody].dQinc+body[jBody].dQinc*body[iBody].dPinc)));
}

double fdDdistDaDir39(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {        
  return AUCM/body[jBody].dSemi*system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][86]\
          *(cos(fdLambdaArg(body,system,iBody,jBody,iIndexJ,3))\
          *(body[iBody].dKecc*(body[jBody].dQinc*body[jBody].dQinc-body[jBody].dPinc*body[jBody].dPinc)\
          -2*body[iBody].dHecc*body[jBody].dPinc*body[jBody].dQinc)
          +sin(fdLambdaArg(body,system,iBody,jBody,iIndexJ,3))\
          *(body[iBody].dHecc*(body[jBody].dQinc*body[jBody].dQinc-body[jBody].dPinc*body[jBody].dPinc)\
          +2*body[iBody].dKecc*body[jBody].dPinc*body[jBody].dQinc));
}

double fdDdistDaDir310(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {        
  return AUCM/body[jBody].dSemi*system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][87]\
          *(cos(fdLambdaArg(body,system,iBody,jBody,iIndexJ,3))\
          *(body[jBody].dKecc*(body[jBody].dQinc*body[jBody].dQinc-body[jBody].dPinc*body[jBody].dPinc)\
          -2*body[jBody].dHecc*body[jBody].dPinc*body[jBody].dQinc)
          +sin(fdLambdaArg(body,system,iBody,jBody,iIndexJ,3))\
          *(body[jBody].dHecc*(body[jBody].dQinc*body[jBody].dQinc-body[jBody].dPinc*body[jBody].dPinc)\
          +2*body[jBody].dKecc*body[jBody].dPinc*body[jBody].dQinc));
}

double fdDdistresDSemi0th(BODY *body, SYSTEM *system, int *iaBody) {
  double y, dMfac, dSemiPrm;
  dMfac = KGAUSS*KGAUSS*body[iaBody[1]].dMass/MSUN;
  dSemiPrm = body[iaBody[1]].dSemi/AUCM;
  y = ( fdDdistDaDir01(body, system, iaBody[0], iaBody[1])\
      + fdDdistDaDir02(body, system, iaBody[0], iaBody[1])\
      + fdDdistDaDir03(body, system, iaBody[0], iaBody[1]) ) * dMfac/dSemiPrm;
  return y;
}

double fdDdistresDSemi(BODY *body, SYSTEM *system, int *iaBody, int iIndexJ, int iOrder) {
  double y, dMfac, dSemiPrm;
  dMfac = KGAUSS*KGAUSS*body[iaBody[1]].dMass/MSUN;
  dSemiPrm = body[iaBody[1]].dSemi/AUCM;
  if (iOrder == 1) {
    y = ( fdDdistDaDir11(body, system, iaBody[0], iaBody[1], iIndexJ)\
      + fdDdistDaDir12(body, system, iaBody[0], iaBody[1], iIndexJ) ) * dMfac/dSemiPrm;
    if (iIndexJ == 2) {
      y += (fdDdistDaExt13(body,system,iaBody[0],iaBody[1],iIndexJ))*dMfac/dSemiPrm;
    }
  } else if (iOrder == 2) {
    y = ( fdDdistDaDir21(body, system, iaBody[0], iaBody[1], iIndexJ) \
    + fdDdistDaDir22(body, system, iaBody[0], iaBody[1], iIndexJ)
    + fdDdistDaDir23(body, system, iaBody[0], iaBody[1], iIndexJ) \
    + fdDdistDaDir24(body, system, iaBody[0], iaBody[1], iIndexJ)
    + fdDdistDaDir25(body, system, iaBody[0], iaBody[1], iIndexJ) \
    + fdDdistDaDir26(body, system, iaBody[0], iaBody[1], iIndexJ) ) * dMfac/dSemiPrm;
  }
  return y;
}

//---------dR/da'------------------------------------------------------------------
double fdDistDir01(BODY *body, SYSTEM *system, int iBody, int jBody) {
  return  (system->dmLaplaceC[0][system->imLaplaceN[iBody][jBody]][0] + \
          system->dmLaplaceC[0][system->imLaplaceN[iBody][jBody]][1] * \
          (body[iBody].dKecc*body[iBody].dKecc+body[jBody].dKecc*body[jBody].dKecc+\
          body[iBody].dHecc*body[iBody].dHecc+body[jBody].dHecc*body[jBody].dHecc) +\
          system->dmLaplaceC[0][system->imLaplaceN[iBody][jBody]][2] * \
          (body[iBody].dPinc*body[iBody].dPinc+body[jBody].dPinc*body[jBody].dPinc+\
          body[iBody].dQinc*body[iBody].dQinc+body[jBody].dQinc*body[jBody].dQinc));
}

double fdDistDir02(BODY *body, SYSTEM *system, int iBody, int jBody) {
  return (system->dmLaplaceC[0][system->imLaplaceN[iBody][jBody]][9] * \
          (body[iBody].dKecc*body[jBody].dKecc+body[iBody].dHecc*body[jBody].dHecc));
}

double fdDistDir03(BODY *body, SYSTEM *system, int iBody, int jBody) {
  return (system->dmLaplaceC[0][system->imLaplaceN[iBody][jBody]][13] * \
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

double fdDistDir11(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][26] * \
          (body[iBody].dKecc*cos(fdLambdaArg(body,system,iBody,jBody,iIndexJ,1)) +\
          body[iBody].dHecc*sin(fdLambdaArg(body,system,iBody,jBody,iIndexJ,1)));
}

double fdDistDir12(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][30] * \
          (body[jBody].dKecc*cos(fdLambdaArg(body,system,iBody,jBody,iIndexJ,1)) +\
          body[jBody].dHecc*sin(fdLambdaArg(body,system,iBody,jBody,iIndexJ,1)));
}

double fdDdistDaPrmDir11(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return -body[jBody].dSemi/body[iBody].dSemi*fdDdistDaDir11(body,system,jBody,iBody,iIndexJ)\
         -1./body[iBody].dSemi*fdDistDir11(body,system,jBody,iBody,iIndexJ)*AUCM;
}

double fdDdistDaPrmDir12(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return -body[jBody].dSemi/body[iBody].dSemi*fdDdistDaDir12(body,system,jBody,iBody,iIndexJ)\
         -1./body[iBody].dSemi*fdDistDir12(body,system,jBody,iBody,iIndexJ)*AUCM;
}

double fdDdistDaPrmInt13(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return -1./2*AUCM*AUCM/(body[jBody].dSemi*body[jBody].dSemi)*(body[iBody].dKecc*\
        cos(fdLambdaArg(body,system,jBody,iBody,iIndexJ,1))+body[iBody].dHecc*\
        sin(fdLambdaArg(body,system,jBody,iBody,iIndexJ,1)));
}

double fdDistDir21(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  //jBody is the outer body
  return system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][44]*\
         ( (body[iBody].dKecc*body[iBody].dKecc-body[iBody].dHecc*body[iBody].dHecc)*\
            cos(fdLambdaArg(body,system,iBody,jBody,iIndexJ,2)) + \
            2*body[iBody].dHecc*body[iBody].dKecc*\
            sin(fdLambdaArg(body,system,iBody,jBody,iIndexJ,2)) );
}

double fdDistDir22(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][48]*\
         ( (body[jBody].dKecc*body[iBody].dKecc-body[jBody].dHecc*body[iBody].dHecc)*\
            cos(fdLambdaArg(body,system,iBody,jBody,iIndexJ,2)) + \
            (body[iBody].dHecc*body[jBody].dKecc+body[iBody].dKecc*body[jBody].dHecc)*\
            sin(fdLambdaArg(body,system,iBody,jBody,iIndexJ,2)) );
}

double fdDistDir23(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  //jBody is the outer body
  return system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][52]*\
         ( (body[jBody].dKecc*body[jBody].dKecc-body[jBody].dHecc*body[jBody].dHecc)*\
            cos(fdLambdaArg(body,system,iBody,jBody,iIndexJ,2)) + \
            2*body[jBody].dHecc*body[jBody].dKecc*\
            sin(fdLambdaArg(body,system,iBody,jBody,iIndexJ,2)) );
}

double fdDistDir24(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  //jBody is the outer body
  return system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][56]*\
         ( (body[iBody].dQinc*body[iBody].dQinc-body[iBody].dPinc*body[iBody].dPinc)*\
            cos(fdLambdaArg(body,system,iBody,jBody,iIndexJ,2)) + \
            2*body[iBody].dPinc*body[iBody].dQinc*\
            sin(fdLambdaArg(body,system,iBody,jBody,iIndexJ,2)) );
}

double fdDistDir25(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][61]*\
         ( (body[jBody].dQinc*body[iBody].dQinc-body[jBody].dPinc*body[iBody].dPinc)*\
            cos(fdLambdaArg(body,system,iBody,jBody,iIndexJ,2)) + \
            (body[iBody].dPinc*body[jBody].dQinc+body[iBody].dQinc*body[jBody].dPinc)*\
            sin(fdLambdaArg(body,system,iBody,jBody,iIndexJ,2)) );
}

double fdDistDir26(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  //jBody is the outer body
  return system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][56]*\
         ( (body[jBody].dQinc*body[jBody].dQinc-body[jBody].dPinc*body[jBody].dPinc)*\
            cos(fdLambdaArg(body,system,iBody,jBody,iIndexJ,2)) + \
            2*body[jBody].dPinc*body[jBody].dQinc*\
            sin(fdLambdaArg(body,system,iBody,jBody,iIndexJ,2)) );
}

double fdDdistDaPrmDir21(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return -body[jBody].dSemi/body[iBody].dSemi*fdDdistDaDir21(body,system,jBody,iBody,iIndexJ)\
         -1./body[iBody].dSemi*fdDistDir21(body,system,jBody,iBody,iIndexJ)*AUCM;
}

double fdDdistDaPrmDir22(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return -body[jBody].dSemi/body[iBody].dSemi*fdDdistDaDir22(body,system,jBody,iBody,iIndexJ)\
         -1./body[iBody].dSemi*fdDistDir22(body,system,jBody,iBody,iIndexJ)*AUCM;
}

double fdDdistDaPrmDir23(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return -body[jBody].dSemi/body[iBody].dSemi*fdDdistDaDir23(body,system,jBody,iBody,iIndexJ)\
         -1./body[iBody].dSemi*fdDistDir23(body,system,jBody,iBody,iIndexJ)*AUCM;
}

double fdDdistDaPrmDir24(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return -body[jBody].dSemi/body[iBody].dSemi*fdDdistDaDir24(body,system,jBody,iBody,iIndexJ)\
         -1./body[iBody].dSemi*fdDistDir24(body,system,jBody,iBody,iIndexJ)*AUCM;
}

double fdDdistDaPrmDir25(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return -body[jBody].dSemi/body[iBody].dSemi*fdDdistDaDir25(body,system,jBody,iBody,iIndexJ)\
         -1./body[iBody].dSemi*fdDistDir25(body,system,jBody,iBody,iIndexJ)*AUCM;
}

double fdDdistDaPrmDir26(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return -body[jBody].dSemi/body[iBody].dSemi*fdDdistDaDir26(body,system,jBody,iBody,iIndexJ)\
         -1./body[iBody].dSemi*fdDistDir26(body,system,jBody,iBody,iIndexJ)*AUCM;
}

double fdDistDir31(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  //jBody is the outer body
  return system->dmLaplaceC[iIndexJ][system->imLaplaceN[jBody][iBody]][81]\
          *(cos(fdLambdaArg(body,system,iBody,jBody,iIndexJ,3))\
          *(body[iBody].dKecc*body[iBody].dKecc*body[iBody].dKecc\
          -3*body[iBody].dHecc*body[iBody].dHecc*body[iBody].dKecc)\
          +sin(fdLambdaArg(body,system,iBody,jBody,iIndexJ,3))\
          *(-body[iBody].dHecc*body[iBody].dHecc*body[iBody].dHecc\
          +3*body[iBody].dKecc*body[iBody].dKecc*body[iBody].dHecc));
}

double fdDistDir32(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {        
  return system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][82]\
          *(cos(fdLambdaArg(body,system,iBody,jBody,iIndexJ,3))\
          *(body[jBody].dKecc*(body[iBody].dKecc*body[iBody].dKecc-body[iBody].dHecc*body[iBody].dHecc)\
          -2*body[jBody].dHecc*body[iBody].dHecc*body[iBody].dKecc)
          +sin(fdLambdaArg(body,system,iBody,jBody,iIndexJ,3))\
          *(body[jBody].dHecc*(body[iBody].dKecc*body[iBody].dKecc-body[iBody].dHecc*body[iBody].dHecc)\
          +2*body[jBody].dKecc*body[iBody].dKecc*body[iBody].dHecc));
}

double fdDistDir33(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {        
  return system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][83]\
          *(cos(fdLambdaArg(body,system,iBody,jBody,iIndexJ,3))\
          *(body[iBody].dKecc*(body[jBody].dKecc*body[jBody].dKecc-body[jBody].dHecc*body[jBody].dHecc)\
          -2*body[iBody].dHecc*body[jBody].dHecc*body[jBody].dKecc)
          +sin(fdLambdaArg(body,system,iBody,jBody,iIndexJ,3))\
          *(body[iBody].dHecc*(body[jBody].dKecc*body[jBody].dKecc-body[jBody].dHecc*body[jBody].dHecc)\
          +2*body[iBody].dKecc*body[jBody].dKecc*body[jBody].dHecc));
}

double fdDistDir34(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {        
  return system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][84]\
          *(cos(fdLambdaArg(body,system,iBody,jBody,iIndexJ,3))\
          *(body[jBody].dKecc*body[jBody].dKecc*body[jBody].dKecc\
          -3*body[jBody].dHecc*body[jBody].dHecc*body[jBody].dKecc)
          +sin(fdLambdaArg(body,system,iBody,jBody,iIndexJ,3))\
          *(-body[jBody].dHecc*body[jBody].dHecc*body[jBody].dHecc\
          +3*body[jBody].dKecc*body[jBody].dKecc*body[jBody].dHecc));
}

double fdDistDir35(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {        
  return system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][85]\
          *(cos(fdLambdaArg(body,system,iBody,jBody,iIndexJ,3))\
          *(body[iBody].dKecc*(body[iBody].dQinc*body[iBody].dQinc-body[iBody].dPinc*body[iBody].dPinc)\
          -2*body[iBody].dHecc*body[iBody].dPinc*body[iBody].dQinc)
          +sin(fdLambdaArg(body,system,iBody,jBody,iIndexJ,3))\
          *(body[iBody].dHecc*(body[iBody].dQinc*body[iBody].dQinc-body[iBody].dPinc*body[iBody].dPinc)\
          +2*body[iBody].dKecc*body[iBody].dPinc*body[iBody].dQinc));
}

double fdDistDir36(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {        
  return system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][86]\
          *(cos(fdLambdaArg(body,system,iBody,jBody,iIndexJ,3))\
          *(body[jBody].dKecc*(body[iBody].dQinc*body[iBody].dQinc-body[iBody].dPinc*body[iBody].dPinc)\
          -2*body[jBody].dHecc*body[iBody].dPinc*body[iBody].dQinc)
          +sin(fdLambdaArg(body,system,iBody,jBody,iIndexJ,3))\
          *(body[jBody].dHecc*(body[iBody].dQinc*body[iBody].dQinc-body[iBody].dPinc*body[iBody].dPinc)\
          +2*body[jBody].dKecc*body[iBody].dPinc*body[iBody].dQinc));
}

double fdDistDir37(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {        
  return system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][87]\
          *(cos(fdLambdaArg(body,system,iBody,jBody,iIndexJ,3))\
          *(body[iBody].dKecc*(body[jBody].dQinc*body[iBody].dQinc-body[jBody].dPinc*body[iBody].dPinc)\
          -body[iBody].dHecc*(body[jBody].dPinc*body[iBody].dQinc+body[jBody].dQinc*body[iBody].dPinc))
          +sin(fdLambdaArg(body,system,iBody,jBody,iIndexJ,3))\
          *(body[iBody].dHecc*(body[jBody].dQinc*body[iBody].dQinc-body[jBody].dPinc*body[iBody].dPinc)\
          +body[iBody].dKecc*(body[jBody].dPinc*body[iBody].dQinc+body[jBody].dQinc*body[iBody].dPinc)));
}

double fdDistDir38(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {        
  return system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][88]\
          *(cos(fdLambdaArg(body,system,iBody,jBody,iIndexJ,3))\
          *(body[jBody].dKecc*(body[jBody].dQinc*body[iBody].dQinc-body[jBody].dPinc*body[iBody].dPinc)\
          -body[jBody].dHecc*(body[jBody].dPinc*body[iBody].dQinc+body[jBody].dQinc*body[iBody].dPinc))
          +sin(fdLambdaArg(body,system,iBody,jBody,iIndexJ,3))\
          *(body[jBody].dHecc*(body[jBody].dQinc*body[iBody].dQinc-body[jBody].dPinc*body[iBody].dPinc)\
          +body[jBody].dKecc*(body[jBody].dPinc*body[iBody].dQinc+body[jBody].dQinc*body[iBody].dPinc)));
}

double fdDistDir39(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {        
  return system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][86]\
          *(cos(fdLambdaArg(body,system,iBody,jBody,iIndexJ,3))\
          *(body[iBody].dKecc*(body[jBody].dQinc*body[jBody].dQinc-body[jBody].dPinc*body[jBody].dPinc)\
          -2*body[iBody].dHecc*body[jBody].dPinc*body[jBody].dQinc)
          +sin(fdLambdaArg(body,system,iBody,jBody,iIndexJ,3))\
          *(body[iBody].dHecc*(body[jBody].dQinc*body[jBody].dQinc-body[jBody].dPinc*body[jBody].dPinc)\
          +2*body[iBody].dKecc*body[jBody].dPinc*body[jBody].dQinc));
}

double fdDistDir310(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {        
  return system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][87]\
          *(cos(fdLambdaArg(body,system,iBody,jBody,iIndexJ,3))\
          *(body[jBody].dKecc*(body[jBody].dQinc*body[jBody].dQinc-body[jBody].dPinc*body[jBody].dPinc)\
          -2*body[jBody].dHecc*body[jBody].dPinc*body[jBody].dQinc)
          +sin(fdLambdaArg(body,system,iBody,jBody,iIndexJ,3))\
          *(body[jBody].dHecc*(body[jBody].dQinc*body[jBody].dQinc-body[jBody].dPinc*body[jBody].dPinc)\
          +2*body[jBody].dKecc*body[jBody].dPinc*body[jBody].dQinc));
}

double fdDdistDaPrmDir31(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return -body[jBody].dSemi/body[iBody].dSemi*fdDdistDaDir31(body,system,jBody,iBody,iIndexJ)\
         -1./body[iBody].dSemi*fdDistDir31(body,system,jBody,iBody,iIndexJ)*AUCM;
}

double fdDdistDaPrmDir32(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return -body[jBody].dSemi/body[iBody].dSemi*fdDdistDaDir32(body,system,jBody,iBody,iIndexJ)\
         -1./body[iBody].dSemi*fdDistDir32(body,system,jBody,iBody,iIndexJ)*AUCM;
}

double fdDdistDaPrmDir33(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return -body[jBody].dSemi/body[iBody].dSemi*fdDdistDaDir33(body,system,jBody,iBody,iIndexJ)\
         -1./body[iBody].dSemi*fdDistDir33(body,system,jBody,iBody,iIndexJ)*AUCM;
}

double fdDdistDaPrmDir34(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return -body[jBody].dSemi/body[iBody].dSemi*fdDdistDaDir34(body,system,jBody,iBody,iIndexJ)\
         -1./body[iBody].dSemi*fdDistDir34(body,system,jBody,iBody,iIndexJ)*AUCM;
}

double fdDdistDaPrmDir35(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return -body[jBody].dSemi/body[iBody].dSemi*fdDdistDaDir35(body,system,jBody,iBody,iIndexJ)\
         -1./body[iBody].dSemi*fdDistDir35(body,system,jBody,iBody,iIndexJ)*AUCM;
}

double fdDdistDaPrmDir36(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return -body[jBody].dSemi/body[iBody].dSemi*fdDdistDaDir36(body,system,jBody,iBody,iIndexJ)\
         -1./body[iBody].dSemi*fdDistDir36(body,system,jBody,iBody,iIndexJ)*AUCM;
}

double fdDdistDaPrmDir37(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return -body[jBody].dSemi/body[iBody].dSemi*fdDdistDaDir37(body,system,jBody,iBody,iIndexJ)\
         -1./body[iBody].dSemi*fdDistDir37(body,system,jBody,iBody,iIndexJ)*AUCM;
}

double fdDdistDaPrmDir38(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return -body[jBody].dSemi/body[iBody].dSemi*fdDdistDaDir38(body,system,jBody,iBody,iIndexJ)\
         -1./body[iBody].dSemi*fdDistDir38(body,system,jBody,iBody,iIndexJ)*AUCM;
}

double fdDdistDaPrmDir39(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return -body[jBody].dSemi/body[iBody].dSemi*fdDdistDaDir39(body,system,jBody,iBody,iIndexJ)\
         -1./body[iBody].dSemi*fdDistDir39(body,system,jBody,iBody,iIndexJ)*AUCM;
}

double fdDdistDaPrmDir310(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return -body[jBody].dSemi/body[iBody].dSemi*fdDdistDaDir310(body,system,jBody,iBody,iIndexJ)\
         -1./body[iBody].dSemi*fdDistDir310(body,system,jBody,iBody,iIndexJ)*AUCM;
}

double fdDdistresDSemiPrime0th(BODY *body, SYSTEM *system, int *iaBody) {
  double y, dMfac, dSemiPrm;
  dMfac = KGAUSS*KGAUSS*body[iaBody[1]].dMass/MSUN;
  dSemiPrm = body[iaBody[0]].dSemi/AUCM;
  y = ( fdDdistDaPrmDir01(body, system, iaBody[0], iaBody[1]) \
      + fdDdistDaPrmDir02(body, system, iaBody[0], iaBody[1]) \
      + fdDdistDaPrmDir03(body, system, iaBody[0], iaBody[1]) ) * dMfac/dSemiPrm;
  return y;
}

double fdDdistresDSemiPrime(BODY *body, SYSTEM *system, int *iaBody, int iIndexJ, int iOrder) {
  double y, dMfac, dSemiPrm;
  dMfac = KGAUSS*KGAUSS*body[iaBody[1]].dMass/MSUN;
  dSemiPrm = body[iaBody[0]].dSemi/AUCM;
  if (iOrder == 1) {
    y = ( fdDdistDaPrmDir11(body, system, iaBody[0], iaBody[1], iIndexJ)\
      + fdDdistDaPrmDir12(body, system, iaBody[0], iaBody[1], iIndexJ) ) * dMfac/dSemiPrm;
    if (iIndexJ == 2) {
      y += fdDdistDaPrmInt13(body, system, iaBody[0], iaBody[1], iIndexJ) * dMfac;
    }
  } else if (iOrder == 2) {
    y = ( fdDdistDaPrmDir21(body, system, iaBody[0], iaBody[1], iIndexJ) \
    + fdDdistDaPrmDir22(body, system, iaBody[0], iaBody[1], iIndexJ)
    + fdDdistDaPrmDir23(body, system, iaBody[0], iaBody[1], iIndexJ) \
    + fdDdistDaPrmDir24(body, system, iaBody[0], iaBody[1], iIndexJ)
    + fdDdistDaPrmDir25(body, system, iaBody[0], iaBody[1], iIndexJ) \
    + fdDdistDaPrmDir26(body, system, iaBody[0], iaBody[1], iIndexJ) ) * dMfac/dSemiPrm;
  }
  return y;
}

//-------------------DistRes's equations in a h k p q l (2nd order direct integration RD2)--------------------
double fdDistResRD2DaDt(BODY *body, SYSTEM *system, int *iaBody) {
  double sum = 0.0, dMu, y;
  int iRes,iResIndex,iOrder;
  //Here, iaBody[0] = body in question, iaBody[1] = perturber
  dMu = KGAUSS*KGAUSS*(body[0].dMass+body[iaBody[0]].dMass)/MSUN;
  
  for (iRes=0;iRes<RESNUM;iRes++) {
    iResIndex = system->iResIndex[iRes][system->imLaplaceN[iaBody[0]][iaBody[1]]];
    iOrder = system->iResOrder[iRes];
    if (iResIndex == -1) {
      sum += 0.0;
    } else {
      if (body[iaBody[0]].dSemi < body[iaBody[1]].dSemi) {
        sum += 2.0*sqrt(body[iaBody[0]].dSemi/AUCM/dMu) * \
              fdDdistresDLambda(body,system,iaBody,iResIndex,iOrder); 
      } else if (body[iaBody[0]].dSemi > body[iaBody[1]].dSemi) {
        sum += 2.0*sqrt(body[iaBody[0]].dSemi/AUCM/dMu) * \
              fdDdistresDLambdaPrime(body,system,iaBody,iResIndex,iOrder);
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
  
  sum += sqrt(dMu/(body[iaBody[0]].dSemi/AUCM*body[iaBody[0]].dSemi/AUCM*body[iaBody[0]].dSemi/AUCM));
  y = fabs(1.-body[iaBody[0]].dHecc*body[iaBody[0]].dHecc-\
    body[iaBody[0]].dKecc*body[iaBody[0]].dKecc);
  for (iRes=0;iRes<RESNUM;iRes++) {
    iResIndex = system->iResIndex[iRes][system->imLaplaceN[iaBody[0]][iaBody[1]]];
    iOrder = system->iResOrder[iRes];
    if (iResIndex == -1) {
      sum += 0.0;
    } else {
      if (body[iaBody[0]].dSemi < body[iaBody[1]].dSemi) {
        sum += -2.0*sqrt(body[iaBody[0]].dSemi/AUCM/dMu)\
              *(fdDdistresDSemi0th(body,system,iaBody)+\
              fdDdistresDSemi(body,system,iaBody,iResIndex,iOrder))\
              +( sqrt(y)/(1.+sqrt(y))*(body[iaBody[0]].dHecc*\
              fdDdistresDHecc(body,system,iaBody,iResIndex,iOrder)\
              +body[iaBody[0]].dKecc*\
              fdDdistresDKecc(body,system,iaBody,iResIndex,iOrder))\
              +0.5/sqrt(y)*(body[iaBody[0]].dPinc*\
              fdDdistresDPinc(body,system,iaBody,iResIndex,iOrder)\
              +body[iaBody[0]].dQinc*\
              fdDdistresDQinc(body,system,iaBody,iResIndex,iOrder)) )\
              /sqrt(dMu*body[iaBody[0]].dSemi/AUCM);     
      } else if (body[iaBody[0]].dSemi > body[iaBody[1]].dSemi) {
        sum += -2.0*sqrt(body[iaBody[0]].dSemi/AUCM/dMu)\
              *(fdDdistresDSemiPrime0th(body,system,iaBody)+\
              fdDdistresDSemiPrime(body,system,iaBody,iResIndex,iOrder))\
              +( sqrt(y)/(1.+sqrt(y))*(body[iaBody[0]].dHecc*\
              fdDdistresDHeccPrime(body,system,iaBody,iResIndex,iOrder)\
              +body[iaBody[0]].dKecc*\
              fdDdistresDKeccPrime(body,system,iaBody,iResIndex,iOrder))\
              +0.5/sqrt(y)*(body[iaBody[0]].dPinc*\
              fdDdistresDPincPrime(body,system,iaBody,iResIndex,iOrder)\
              +body[iaBody[0]].dQinc*\
              fdDdistresDQincPrime(body,system,iaBody,iResIndex,iOrder)) )\
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
    iResIndex = system->iResIndex[iRes][system->imLaplaceN[iaBody[0]][iaBody[1]]];
    iOrder = system->iResOrder[iRes];
    if (iResIndex == -1) {
      sum += 0.0;
    } else {
      if (body[iaBody[0]].dSemi < body[iaBody[1]].dSemi) {
        sum += ( sqrt(y)*fdDdistresDKecc(body,system, iaBody,iResIndex,iOrder) + \
        body[iaBody[0]].dKecc*(body[iaBody[0]].dPinc*\
        fdDdistresDPinc(body, system, iaBody,iResIndex,iOrder)\
        +body[iaBody[0]].dQinc*\
        fdDdistresDQinc(body, system, iaBody,iResIndex,iOrder))/(2*sqrt(y)) \
        -body[iaBody[0]].dHecc*sqrt(y)/(1.+sqrt(y))*\
        fdDdistresDLambda(body,system,iaBody,iResIndex,iOrder) )\
        / sqrt(dMu*body[iaBody[0]].dSemi/AUCM);
      
      } else if (body[iaBody[0]].dSemi > body[iaBody[1]].dSemi) {
        sum += ( sqrt(y)*\
        fdDdistresDKeccPrime(body, system, iaBody,iResIndex,iOrder) + \
        body[iaBody[0]].dKecc*(body[iaBody[0]].dPinc*\
        fdDdistresDPincPrime(body, system, iaBody,iResIndex,iOrder)\
        +body[iaBody[0]].dQinc*\
        fdDdistresDQincPrime(body, system, iaBody,iResIndex,iOrder))/(2*sqrt(y)) \
        -body[iaBody[0]].dHecc*sqrt(y)/(1.+sqrt(y))*\
        fdDdistresDLambdaPrime(body,system,iaBody,iResIndex,iOrder) )\
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
    iResIndex = system->iResIndex[iRes][system->imLaplaceN[iaBody[0]][iaBody[1]]];
    iOrder = system->iResOrder[iRes];
    if (iResIndex == -1) {
      sum += 0.0;
    } else {
      if (body[iaBody[0]].dSemi < body[iaBody[1]].dSemi) {
        sum += -( sqrt(y)*fdDdistresDHecc(body, system, iaBody,iResIndex,iOrder)+\
        body[iaBody[0]].dHecc*(body[iaBody[0]].dPinc*\
        fdDdistresDPinc(body, system, iaBody,iResIndex,iOrder)\
        +body[iaBody[0]].dQinc*\
        fdDdistresDQinc(body, system, iaBody,iResIndex,iOrder))/(2*sqrt(y)) \
        +body[iaBody[0]].dKecc*sqrt(y)/(1.+sqrt(y))*\
        fdDdistresDLambda(body,system,iaBody,iResIndex,iOrder) ) \
        / sqrt(dMu*body[iaBody[0]].dSemi/AUCM);
      
      } else if (body[iaBody[0]].dSemi > body[iaBody[1]].dSemi) {
        sum += -( sqrt(y)*\
        fdDdistresDHeccPrime(body, system, iaBody,iResIndex,iOrder) + \
        body[iaBody[0]].dHecc*(body[iaBody[0]].dPinc*\
        fdDdistresDPincPrime(body, system, iaBody,iResIndex,iOrder)\
        +body[iaBody[0]].dQinc*\
        fdDdistresDQincPrime(body, system, iaBody,iResIndex,iOrder))/(2*sqrt(y)) \
        +body[iaBody[0]].dKecc*sqrt(y)/(1.+sqrt(y))*\
        fdDdistresDLambdaPrime(body,system,iaBody,iResIndex,iOrder) )\
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
      iResIndex = system->iResIndex[iRes][system->imLaplaceN[iaBody[0]][iaBody[1]]];
      iOrder = system->iResOrder[iRes];
      if (iResIndex == -1) {
        sum += 0.0;
      } else {
        if (body[iaBody[0]].dSemi < body[iaBody[1]].dSemi) {
          sum += ( body[iaBody[0]].dPinc*(-body[iaBody[0]].dKecc*\
          fdDdistresDHecc(body,system,iaBody,iResIndex,iOrder)\
          +body[iaBody[0]].dHecc*\
          fdDdistresDKecc(body,system,iaBody,iResIndex,iOrder))\
          +1.0/2.0*fdDdistresDQinc(body,system,iaBody,iResIndex,iOrder)\
          -body[iaBody[0]].dPinc*\
          fdDdistresDLambda(body,system,iaBody,iResIndex,iOrder) )\
          /(2*sqrt(dMu*body[iaBody[0]].dSemi/AUCM*(y)));
        } else if (body[iaBody[0]].dSemi > body[iaBody[1]].dSemi) {
          sum += ( body[iaBody[0]].dPinc*(-body[iaBody[0]].dKecc*\
          fdDdistresDHeccPrime(body,system,iaBody,iResIndex,iOrder)\
          +body[iaBody[0]].dHecc*\
          fdDdistresDKeccPrime(body,system,iaBody,iResIndex,iOrder))\
          +1.0/2.0*fdDdistresDQincPrime(body,system,iaBody,iResIndex,iOrder)\
          -body[iaBody[0]].dPinc*\
          fdDdistresDLambdaPrime(body,system,iaBody,iResIndex,iOrder) )\
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
      iResIndex = system->iResIndex[iRes][system->imLaplaceN[iaBody[0]][iaBody[1]]];
      iOrder = system->iResOrder[iRes];
      if (iResIndex == -1) {
        sum += 0.0;
      } else {
        if (body[iaBody[0]].dSemi < body[iaBody[1]].dSemi) {
          sum += ( body[iaBody[0]].dQinc*(-body[iaBody[0]].dKecc*\
          fdDdistresDHecc(body,system,iaBody,iResIndex,iOrder)\
          +body[iaBody[0]].dHecc*\
          fdDdistresDKecc(body,system,iaBody,iResIndex,iOrder))\
          -1.0/2.0*fdDdistresDPinc(body,system,iaBody,iResIndex,iOrder)\
          -body[iaBody[0]].dQinc*\
          fdDdistresDLambda(body,system,iaBody,iResIndex,iOrder) )\
          /(2*sqrt(dMu*body[iaBody[0]].dSemi/AUCM*(y)));
        } else if (body[iaBody[0]].dSemi > body[iaBody[1]].dSemi) {
          sum += ( body[iaBody[0]].dQinc*(-body[iaBody[0]].dKecc*\
          fdDdistresDHeccPrime(body,system,iaBody,iResIndex,iOrder)\
          +body[iaBody[0]].dHecc*\
          fdDdistresDKeccPrime(body,system,iaBody,iResIndex,iOrder))\
          -1.0/2.0*fdDdistresDPincPrime(body,system,iaBody,iResIndex,iOrder) \
          -body[iaBody[0]].dQinc*\
          fdDdistresDLambdaPrime(body,system,iaBody,iResIndex,iOrder) )\
          /(2*sqrt(dMu*body[iaBody[0]].dSemi/AUCM*(y)));
        }
      }
    }
   
    return sum/DAYSEC;
}

