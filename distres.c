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
    system->iResIndex = malloc(RESNUM*sizeof(int*));
    system->iResIndex[0] = malloc(Nchoosek(control->Evolve.iNumBodies-1,2)*sizeof(int));
    system->iResIndex[1] = malloc(Nchoosek(control->Evolve.iNumBodies-1,2)*sizeof(int));
    system->iResIndex[2] = malloc(Nchoosek(control->Evolve.iNumBodies-1,2)*sizeof(int));
    system->iResIndex[3] = malloc(Nchoosek(control->Evolve.iNumBodies-1,2)*sizeof(int));
    system->iResIndex[4] = malloc(Nchoosek(control->Evolve.iNumBodies-1,2)*sizeof(int));
    system->iResIndex[5] = malloc(Nchoosek(control->Evolve.iNumBodies-1,2)*sizeof(int));
    system->iResIndex[6] = malloc(Nchoosek(control->Evolve.iNumBodies-1,2)*sizeof(int));
    system->iResIndex[7] = malloc(Nchoosek(control->Evolve.iNumBodies-1,2)*sizeof(int));
    system->iResIndex[8] = malloc(Nchoosek(control->Evolve.iNumBodies-1,2)*sizeof(int));
    system->iResIndex[9] = malloc(Nchoosek(control->Evolve.iNumBodies-1,2)*sizeof(int));

    system->iResOrder = malloc(RESNUM*sizeof(int));
    system->iResOrder[0] = 1;
    system->iResOrder[1] = 1;    
    system->iResOrder[2] = 2;
    system->iResOrder[3] = 2;
    system->iResOrder[4] = 2;
    system->iResOrder[5] = 2;
    system->iResOrder[6] = 3;
    system->iResOrder[7] = 3;
    system->iResOrder[8] = 3;
    system->iResOrder[9] = 3;

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
    control->fnPropsAux[iBody][iModule] = &PropsAuxDistOrb;

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
            if (j<=44 || j==48 || j==52 || j==56 || j==61 || j>=81) {
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
        system->iResIndex[0][system->imLaplaceN[iBody][jBody]] = 2;
        system->iResIndex[4][system->imLaplaceN[iBody][jBody]] = 4;
      } else {
        system->iResIndex[0][system->imLaplaceN[iBody][jBody]] = -1;
        system->iResIndex[4][system->imLaplaceN[iBody][jBody]] = -1;
      }
      // 3:2 resonance in the 1th and 5th place
      if (dPerRat > (1.0-dTol)*1.5 && dPerRat < (1.0+dTol)*1.5) {
        system->iResIndex[1][system->imLaplaceN[iBody][jBody]] = 3;
        system->iResIndex[5][system->imLaplaceN[iBody][jBody]] = 6;
      } else {
        system->iResIndex[1][system->imLaplaceN[iBody][jBody]] = -1;
        system->iResIndex[5][system->imLaplaceN[iBody][jBody]] = -1;
      } 
      // 3:1 resonance in the 2th place
      if (dPerRat > (1.0-dTol)*3.0 && dPerRat < (1.0+dTol)*3.0) {
        system->iResIndex[2][system->imLaplaceN[iBody][jBody]] = 3;
      } else {
        system->iResIndex[2][system->imLaplaceN[iBody][jBody]] = -1;
      }    
      // 5:3 in the 3th place
      if (dPerRat > (1.0-dTol)*(5./3) && dPerRat < (1.0+dTol)*(5./3)) {
        system->iResIndex[3][system->imLaplaceN[iBody][jBody]] = 5;
      } else {
        system->iResIndex[3][system->imLaplaceN[iBody][jBody]] = -1;
      } 
      // 4:1 in the 6th place
      if (dPerRat > (1.0-dTol)*(4.) && dPerRat < (1.0+dTol)*(4.)) {
        system->iResIndex[6][system->imLaplaceN[iBody][jBody]] = 4;
      } else {
        system->iResIndex[6][system->imLaplaceN[iBody][jBody]] = -1;
      } 
      // 8:5 in the 7th place
      if (dPerRat > (1.0-dTol)*(8./5) && dPerRat < (1.0+dTol)*(8./5)) {
        system->iResIndex[7][system->imLaplaceN[iBody][jBody]] = 8;
      } else {
        system->iResIndex[7][system->imLaplaceN[iBody][jBody]] = -1;
      }
      // 5:2 in the 8th place
      if (dPerRat > (1.0-dTol)*(5./2) && dPerRat < (1.0+dTol)*(5./2)) {
        system->iResIndex[8][system->imLaplaceN[iBody][jBody]] = 5;
      } else {
        system->iResIndex[8][system->imLaplaceN[iBody][jBody]] = -1;
      }
      // 7:4 in the 8th place
      if (dPerRat > (1.0-dTol)*(7./4) && dPerRat < (1.0+dTol)*(7./4)) {
        system->iResIndex[9][system->imLaplaceN[iBody][jBody]] = 7;
      } else {
        system->iResIndex[9][system->imLaplaceN[iBody][jBody]] = -1;
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
          if (j<=44 || j==48 || j==52 || j==56 || j==61 || j>=81) {
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

/*--------- f28 ---------------------*/
double fdSemiMajAxF28(double dAxRatio, int iIndexJ) {
  return 1./16*((2*iIndexJ-10*iIndexJ*iIndexJ+8*iIndexJ*iIndexJ*iIndexJ)*fdLaplaceCoeff(A(iIndexJ))\
      +(3-7*iIndexJ+4*iIndexJ*iIndexJ)*dAxRatio*fdDerivLaplaceCoeff(1,A(iIndexJ))\
      -(2+2*iIndexJ)*dAxRatio*dAxRatio*fdDerivLaplaceCoeff(2,A(iIndexJ))\
      -dAxRatio*dAxRatio*dAxRatio*fdDerivLaplaceCoeff(3,A(iIndexJ)));
}

double fdDSemiF28Dalpha(double dAxRatio, int iIndexJ) {
  return 1./16*((3-5*iIndexJ-6*iIndexJ*iIndexJ+8*iIndexJ*iIndexJ*iIndexJ)*fdDerivLaplaceCoeff(1,A(iIndexJ))\
      -(1+11*iIndexJ-4*iIndexJ*iIndexJ)*dAxRatio*fdDerivLaplaceCoeff(2,A(iIndexJ))\
      -(5+2*iIndexJ)*dAxRatio*dAxRatio*fdDerivLaplaceCoeff(3,A(iIndexJ))\
      -dAxRatio*dAxRatio*dAxRatio*fdDerivLaplaceCoeff(4,A(iIndexJ)));;
}

/*--------- f29 ---------------------*/
double fdSemiMajAxF29(double dAxRatio, int iIndexJ) {
  return 1./8*(8*iIndexJ*iIndexJ*iIndexJ*fdLaplaceCoeff(A(iIndexJ))\
      +(-2-4*iIndexJ+4*iIndexJ*iIndexJ)*dAxRatio*fdDerivLaplaceCoeff(1,A(iIndexJ))\
      -(4+2*iIndexJ)*dAxRatio*dAxRatio*fdDerivLaplaceCoeff(2,A(iIndexJ))\
      -dAxRatio*dAxRatio*dAxRatio*fdDerivLaplaceCoeff(3,A(iIndexJ)));
}

double fdDSemiF29Dalpha(double dAxRatio, int iIndexJ) {
  return 1./8*((-2-4*iIndexJ+4*iIndexJ*iIndexJ+8*iIndexJ*iIndexJ*iIndexJ)*fdDerivLaplaceCoeff(1,A(iIndexJ))\
      -(10+8*iIndexJ-4*iIndexJ*iIndexJ)*dAxRatio*fdDerivLaplaceCoeff(2,A(iIndexJ))\
      -(7+2*iIndexJ)*dAxRatio*dAxRatio*fdDerivLaplaceCoeff(3,A(iIndexJ))\
      -dAxRatio*dAxRatio*dAxRatio*fdDerivLaplaceCoeff(4,A(iIndexJ)));;
}

/*--------- f30 ---------------------*/
double fdSemiMajAxF30(double dAxRatio, int iIndexJ) {
  return 1./4*((1+2*iIndexJ)*dAxRatio*(fdLaplaceCoeff(B(abs(iIndexJ-1)))+fdLaplaceCoeff(B(iIndexJ+1)))\
      +dAxRatio*dAxRatio*(fdDerivLaplaceCoeff(1,B(abs(iIndexJ-1)))+fdDerivLaplaceCoeff(1,B(iIndexJ+1))));
}

double fdDSemiF30Dalpha(double dAxRatio, int iIndexJ) {
  return 1./4*((1+2*iIndexJ)*(fdLaplaceCoeff(B(abs(iIndexJ-1)))+fdLaplaceCoeff(B(iIndexJ+1)))\
  +(3+2*iIndexJ)*dAxRatio*(fdDerivLaplaceCoeff(1,B(abs(iIndexJ-1)))+fdDerivLaplaceCoeff(1,B(iIndexJ+1)))\
    +dAxRatio*dAxRatio*(fdDerivLaplaceCoeff(2,B(abs(iIndexJ-1)))+fdDerivLaplaceCoeff(2,B(iIndexJ+1))));
}

/*--------- f31 ---------------------*/
double fdSemiMajAxF31(double dAxRatio, int iIndexJ) {
  return 0.5*((-1.0+2.*iIndexJ)*fdLaplaceCoeff(A(abs(iIndexJ-1))) + dAxRatio*fdDerivLaplaceCoeff(1,A(abs(iIndexJ-1))));
}

double fdDSemiF31Dalpha(double dAxRatio, int iIndexJ) {
  return 0.5*((2.*iIndexJ)*fdDerivLaplaceCoeff(1,A(abs(iIndexJ-1)))+dAxRatio*fdDerivLaplaceCoeff(2,A(abs(iIndexJ-1))));
}

/*--------- f32 ---------------------*/
double fdSemiMajAxF32(double dAxRatio, int iIndexJ) {
  return 1./8*((4-16*iIndexJ+20*iIndexJ*iIndexJ-8*iIndexJ*iIndexJ*iIndexJ)*fdLaplaceCoeff(A(abs(iIndexJ-1)))\
      +(-4+12*iIndexJ-4*iIndexJ*iIndexJ)*dAxRatio*fdDerivLaplaceCoeff(1,A(abs(iIndexJ-1)))\
      +(3+2*iIndexJ)*dAxRatio*dAxRatio*fdDerivLaplaceCoeff(2,A(abs(iIndexJ-1)))\
      +dAxRatio*dAxRatio*dAxRatio*fdDerivLaplaceCoeff(3,A(abs(iIndexJ-1))));
}

double fdDSemiF32Dalpha(double dAxRatio, int iIndexJ) {
  return 1./8*((-4*iIndexJ+16*iIndexJ*iIndexJ-\
      8*iIndexJ*iIndexJ*iIndexJ)*fdDerivLaplaceCoeff(1,A(abs(iIndexJ-1)))\
      +(2+16*iIndexJ-4*iIndexJ*iIndexJ)*dAxRatio*fdDerivLaplaceCoeff(2,A(abs(iIndexJ-1)))\
      +(6+2*iIndexJ)*dAxRatio*dAxRatio*fdDerivLaplaceCoeff(3,A(abs(iIndexJ-1)))\
      +dAxRatio*dAxRatio*dAxRatio*fdDerivLaplaceCoeff(4,A(abs(iIndexJ-1))));
}

/*--------- f33 ---------------------*/
double fdSemiMajAxF33(double dAxRatio, int iIndexJ) {
  return 1./16*((-2-iIndexJ+10*iIndexJ*iIndexJ-8*iIndexJ*iIndexJ*iIndexJ)*fdLaplaceCoeff(A(abs(iIndexJ-1)))\
      +(2+9*iIndexJ-4*iIndexJ*iIndexJ)*dAxRatio*fdDerivLaplaceCoeff(1,A(abs(iIndexJ-1)))\
      +(5+2*iIndexJ)*dAxRatio*dAxRatio*fdDerivLaplaceCoeff(2,A(abs(iIndexJ-1)))\
      +dAxRatio*dAxRatio*dAxRatio*fdDerivLaplaceCoeff(3,A(abs(iIndexJ-1))));
}

double fdDSemiF33Dalpha(double dAxRatio, int iIndexJ) {
  return 1./16*((8*iIndexJ+6*iIndexJ*iIndexJ-\
      8*iIndexJ*iIndexJ*iIndexJ)*fdDerivLaplaceCoeff(1,A(abs(iIndexJ-1)))\
      +(12+13*iIndexJ-4*iIndexJ*iIndexJ)*dAxRatio*fdDerivLaplaceCoeff(2,A(abs(iIndexJ-1)))\
      +(8+2*iIndexJ)*dAxRatio*dAxRatio*fdDerivLaplaceCoeff(3,A(abs(iIndexJ-1)))\
      +dAxRatio*dAxRatio*dAxRatio*fdDerivLaplaceCoeff(4,A(abs(iIndexJ-1))));
}

/*--------- f34 ---------------------*/
double fdSemiMajAxF34(double dAxRatio, int iIndexJ) {
  return 1./4*((-2*iIndexJ)*dAxRatio*(fdLaplaceCoeff(B(abs(iIndexJ-2)))+fdLaplaceCoeff(B(iIndexJ)))\
       -dAxRatio*dAxRatio*(fdDerivLaplaceCoeff(1,B(abs(iIndexJ-2)))+fdDerivLaplaceCoeff(1,B(iIndexJ))));
}

double fdDSemiF34Dalpha(double dAxRatio, int iIndexJ) {
  return 1./4*((-2*iIndexJ)*(fdLaplaceCoeff(B(abs(iIndexJ-2)))+fdLaplaceCoeff(B(iIndexJ)))\
  -(2+2*iIndexJ)*dAxRatio*(fdDerivLaplaceCoeff(1,B(abs(iIndexJ-2)))+fdDerivLaplaceCoeff(1,B(iIndexJ)))\
    -dAxRatio*dAxRatio*(fdDerivLaplaceCoeff(2,B(abs(iIndexJ-2)))+fdDerivLaplaceCoeff(2,B(iIndexJ))));
}

/*--------- f35 ---------------------*/
double fdSemiMajAxF35(double dAxRatio, int iIndexJ) {
  return 1./16*((1-iIndexJ-10*iIndexJ*iIndexJ-8*iIndexJ*iIndexJ*iIndexJ)*fdLaplaceCoeff(A(iIndexJ+1))\
      +(-1-iIndexJ-4*iIndexJ*iIndexJ)*dAxRatio*fdDerivLaplaceCoeff(1,A(iIndexJ+1))\
      +(3+2*iIndexJ)*dAxRatio*dAxRatio*fdDerivLaplaceCoeff(2,A(iIndexJ+1))\
      +dAxRatio*dAxRatio*dAxRatio*fdDerivLaplaceCoeff(3,A(iIndexJ+1)));
}

double fdDSemiF35Dalpha(double dAxRatio, int iIndexJ) {
  return 1./16*((-2*iIndexJ-14*iIndexJ*iIndexJ-8*iIndexJ*iIndexJ*iIndexJ)*fdDerivLaplaceCoeff(1,A(iIndexJ+1))\
      +(5+3*iIndexJ-4*iIndexJ*iIndexJ)*dAxRatio*fdDerivLaplaceCoeff(2,A(iIndexJ+1))\
      +(6+2*iIndexJ)*dAxRatio*dAxRatio*fdDerivLaplaceCoeff(3,A(iIndexJ+1))\
      +dAxRatio*dAxRatio*dAxRatio*fdDerivLaplaceCoeff(4,A(iIndexJ+1)));
}

/*--------- f36 ---------------------*/
double fdSemiMajAxF36(double dAxRatio, int iIndexJ) {
  return 1./16*((-8+32*iIndexJ-30*iIndexJ*iIndexJ+8*iIndexJ*iIndexJ*iIndexJ)*fdLaplaceCoeff(A(abs(iIndexJ-2)))\
      +(8-17*iIndexJ+4*iIndexJ*iIndexJ)*dAxRatio*fdDerivLaplaceCoeff(1,A(abs(iIndexJ-2)))\
      +(-4-2*iIndexJ)*dAxRatio*dAxRatio*fdDerivLaplaceCoeff(2,A(abs(iIndexJ-2)))\
      -dAxRatio*dAxRatio*dAxRatio*fdDerivLaplaceCoeff(3,A(abs(iIndexJ-2))));
}

double fdDSemiF36Dalpha(double dAxRatio, int iIndexJ) {
  return 1./16*((15*iIndexJ-26*iIndexJ*iIndexJ+\
      8*iIndexJ*iIndexJ*iIndexJ)*fdDerivLaplaceCoeff(1,A(abs(iIndexJ-2)))\
      -(21*iIndexJ-4*iIndexJ*iIndexJ)*dAxRatio*fdDerivLaplaceCoeff(2,A(abs(iIndexJ-2)))\
      -(7+2*iIndexJ)*dAxRatio*dAxRatio*fdDerivLaplaceCoeff(3,A(abs(iIndexJ-2)))\
      -dAxRatio*dAxRatio*dAxRatio*fdDerivLaplaceCoeff(4,A(abs(iIndexJ-2))));
}

/*--------- f37 ---------------------*/
double fdSemiMajAxF37(double dAxRatio, int iIndexJ) {
  return 1./4*((-5+2*iIndexJ)*dAxRatio*(fdLaplaceCoeff(B(abs(iIndexJ-1))))\
       -dAxRatio*dAxRatio*(fdDerivLaplaceCoeff(1,B(abs(iIndexJ-1)))));
}

double fdDSemiF37Dalpha(double dAxRatio, int iIndexJ) {
  return 1./4*((-5+2*iIndexJ)*(fdLaplaceCoeff(B(abs(iIndexJ-1))))\
  -(7-2*iIndexJ)*dAxRatio*(fdDerivLaplaceCoeff(1,B(abs(iIndexJ-1))))\
    -dAxRatio*dAxRatio*(fdDerivLaplaceCoeff(2,B(abs(iIndexJ-1)))));
}

/*--------- f38 ---------------------*/
double fdSemiMajAxF38(double dAxRatio, int iIndexJ) {
  return 1./4*((-2*iIndexJ)*dAxRatio*(fdLaplaceCoeff(B(iIndexJ)))\
       +dAxRatio*dAxRatio*(fdDerivLaplaceCoeff(1,B(iIndexJ))));
}

double fdDSemiF38Dalpha(double dAxRatio, int iIndexJ) {
  return 1./4*((-2*iIndexJ)*(fdLaplaceCoeff(B(iIndexJ)))\
  +(2-2*iIndexJ)*dAxRatio*(fdDerivLaplaceCoeff(1,B(iIndexJ)))\
    +dAxRatio*dAxRatio*(fdDerivLaplaceCoeff(2,B(iIndexJ))));
}

/*--------- f39 ---------------------*/
double fdSemiMajAxF39(double dAxRatio, int iIndexJ) {
  return 1./2*((-1-2*iIndexJ)*dAxRatio*(fdLaplaceCoeff(B(abs(iIndexJ-1))))\
       -dAxRatio*dAxRatio*(fdDerivLaplaceCoeff(1,B(abs(iIndexJ-1)))));
}

double fdDSemiF39Dalpha(double dAxRatio, int iIndexJ) {
  return 1./2*((-1-2*iIndexJ)*(fdLaplaceCoeff(B(abs(iIndexJ-1))))\
  -(3+2*iIndexJ)*dAxRatio*(fdDerivLaplaceCoeff(1,B(abs(iIndexJ-1))))\
    -dAxRatio*dAxRatio*(fdDerivLaplaceCoeff(2,B(abs(iIndexJ-1)))));
}

/*--------- f40 ---------------------*/
double fdSemiMajAxF40(double dAxRatio, int iIndexJ) {
  return 1./2*((-1-2*iIndexJ)*dAxRatio*(fdLaplaceCoeff(B(iIndexJ+1)))\
       -dAxRatio*dAxRatio*(fdDerivLaplaceCoeff(1,B(iIndexJ+1))));
}

double fdDSemiF40Dalpha(double dAxRatio, int iIndexJ) {
  return 1./2*((-1-2*iIndexJ)*(fdLaplaceCoeff(B(iIndexJ+1)))\
  -(3+2*iIndexJ)*dAxRatio*(fdDerivLaplaceCoeff(1,B(iIndexJ+1)))\
    -dAxRatio*dAxRatio*(fdDerivLaplaceCoeff(2,B(iIndexJ+1))));
}

/*--------- f41 ---------------------*/
double fdSemiMajAxF41(double dAxRatio, int iIndexJ) {
  return 1./2*((5-2*iIndexJ)*dAxRatio*(fdLaplaceCoeff(B(abs(iIndexJ-1))))\
       +dAxRatio*dAxRatio*(fdDerivLaplaceCoeff(1,B(abs(iIndexJ-1)))));
}

double fdDSemiF41Dalpha(double dAxRatio, int iIndexJ) {
  return 1./2*((5-2*iIndexJ)*(fdLaplaceCoeff(B(abs(iIndexJ-1))))\
  +(7-2*iIndexJ)*dAxRatio*(fdDerivLaplaceCoeff(1,B(abs(iIndexJ-1))))\
    +dAxRatio*dAxRatio*(fdDerivLaplaceCoeff(2,B(abs(iIndexJ-1)))));
}

/*--------- f42 ---------------------*/
double fdSemiMajAxF42(double dAxRatio, int iIndexJ) {
  return 1./2*((2*iIndexJ)*dAxRatio*(fdLaplaceCoeff(B(abs(iIndexJ-2))))\
       +dAxRatio*dAxRatio*(fdDerivLaplaceCoeff(1,B(abs(iIndexJ-2)))));
}

double fdDSemiF42Dalpha(double dAxRatio, int iIndexJ) {
  return 1./2*((2*iIndexJ)*(fdLaplaceCoeff(B(abs(iIndexJ-2))))\
  +(2+2*iIndexJ)*dAxRatio*(fdDerivLaplaceCoeff(1,B(abs(iIndexJ-2))))\
    +dAxRatio*dAxRatio*(fdDerivLaplaceCoeff(2,B(abs(iIndexJ-2)))));
}

/*--------- f43 ---------------------*/
double fdSemiMajAxF43(double dAxRatio, int iIndexJ) {
  return 1./2*((2*iIndexJ)*dAxRatio*(fdLaplaceCoeff(B(iIndexJ)))\
       +dAxRatio*dAxRatio*(fdDerivLaplaceCoeff(1,B(iIndexJ))));
}

double fdDSemiF43Dalpha(double dAxRatio, int iIndexJ) {
  return 1./2*((2*iIndexJ)*(fdLaplaceCoeff(B(iIndexJ)))\
  +(2+2*iIndexJ)*dAxRatio*(fdDerivLaplaceCoeff(1,B(iIndexJ)))\
    +dAxRatio*dAxRatio*(fdDerivLaplaceCoeff(2,B(iIndexJ))));
}

/*--------- f44 ---------------------*/
double fdSemiMajAxF44(double dAxRatio, int iIndexJ) {
  return 1./2*((2*iIndexJ)*dAxRatio*(fdLaplaceCoeff(B(iIndexJ)))\
       -dAxRatio*dAxRatio*(fdDerivLaplaceCoeff(1,B(iIndexJ))));
}

double fdDSemiF44Dalpha(double dAxRatio, int iIndexJ) {
  return 1./2*((2*iIndexJ)*(fdLaplaceCoeff(B(iIndexJ)))\
  +(-2+2*iIndexJ)*dAxRatio*(fdDerivLaplaceCoeff(1,B(iIndexJ)))\
    -dAxRatio*dAxRatio*(fdDerivLaplaceCoeff(2,B(iIndexJ))));
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
double fdDdistDhDir11Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f28, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][27];
  f28 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][27] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][27]*dAlpha;       

  return 2*body[iBody].dHecc*body[iBody].dKecc*f28;
}

double fdDdistDhDir11Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f27, f28, f29, f30, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][26];
  f27 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][26] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][26]*dAlpha;
        
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][27];
  f28 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][27] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][27]*dAlpha;      
        
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][28];
  f29 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][28] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][28]*dAlpha;  

  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][29];
  f30 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][29] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][29]*dAlpha;  

  return (f27+(3*body[iBody].dHecc*body[iBody].dHecc+body[iBody].dKecc*body[iBody].dKecc)*f28\
    +(body[jBody].dHecc*body[jBody].dHecc+body[jBody].dKecc*body[jBody].dKecc)*f29\
    +(body[iBody].dPinc*body[iBody].dPinc+body[iBody].dQinc*body[iBody].dQinc\
    +body[jBody].dPinc*body[jBody].dPinc+body[jBody].dQinc*body[jBody].dQinc)*f30);
}

double fdDdistDhDir12Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f32, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][31];
  f32 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][31] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][31]*dAlpha;
  
  return 2*body[iBody].dHecc*f32*(body[jBody].dKecc);        
}

double fdDdistDhDir12Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f32, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][31];
  f32 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][31] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][31]*dAlpha;
  
  return 2*body[iBody].dHecc*f32*(body[jBody].dHecc);        
}

double fdDdistDhDir13Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f35, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][34];
  f35 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][34] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][34]*dAlpha;  

  return f35*((-2*body[iBody].dHecc*body[jBody].dKecc+2*body[iBody].dKecc*body[jBody].dHecc));     
}

double fdDdistDhDir13Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f35, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][34];
  f35 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][34] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][34]*dAlpha;  

  return f35*((2*body[iBody].dHecc*body[jBody].dHecc+2*body[iBody].dKecc*body[jBody].dKecc));     
}

double fdDdistDhDir14Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f36, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][35];
  f36 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][35] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][35]*dAlpha;  

  return f36*((2*body[jBody].dHecc*body[jBody].dKecc));     
}

double fdDdistDhDir14Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f36, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][35];
  f36 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][35] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][35]*dAlpha;  

  return f36*(-(body[jBody].dKecc*body[jBody].dKecc-body[jBody].dHecc*body[jBody].dHecc));     
}

double fdDdistDhDir15Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f37, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][36];
  f37 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][36] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][36]*dAlpha;  

  return f37*((2*body[iBody].dPinc*body[iBody].dQinc));     
}

double fdDdistDhDir15Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f37, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][36];
  f37 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][36] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][36]*dAlpha;  

  return f37*(-(body[iBody].dQinc*body[iBody].dQinc-body[iBody].dPinc*body[iBody].dPinc));     
}

double fdDdistDhDir17Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f39, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][38];
  f39 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][38] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][38]*dAlpha;  

  return f39*(-(body[jBody].dPinc*body[iBody].dQinc-body[jBody].dQinc*body[iBody].dPinc));     
}

double fdDdistDhDir17Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f39, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][38];
  f39 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][38] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][38]*dAlpha;  

  return f39*((body[jBody].dQinc*body[iBody].dQinc+body[jBody].dPinc*body[iBody].dPinc));     
}

double fdDdistDhDir18Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f40, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][39];
  f40 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][39] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][39]*dAlpha;  

  return f40*(-(body[iBody].dPinc*body[jBody].dQinc-body[iBody].dQinc*body[jBody].dPinc));     
}

double fdDdistDhDir18Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f40, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][39];
  f40 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][39] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][39]*dAlpha;  

  return f40*((body[jBody].dQinc*body[iBody].dQinc+body[jBody].dPinc*body[iBody].dPinc));     
}

double fdDdistDhDir19Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f41, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][40];
  f41 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][40] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][40]*dAlpha;  

  return f41*((body[jBody].dPinc*body[iBody].dQinc+body[jBody].dQinc*body[iBody].dPinc));     
}

double fdDdistDhDir19Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f41, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][40];
  f41 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][40] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][40]*dAlpha;  

  return f41*((body[jBody].dPinc*body[iBody].dPinc-body[jBody].dQinc*body[iBody].dQinc));     
}

double fdDdistDhDir113Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f37, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][36];
  f37 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][36] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][36]*dAlpha;  

  return f37*((2*body[jBody].dPinc*body[jBody].dQinc));     
}

double fdDdistDhDir113Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f37, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][36];
  f37 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][36] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][36]*dAlpha;  

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
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][44];
  f45 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][44] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][44]*dAlpha;
  
  return f45*(-2.*body[iBody].dHecc);
}

double fdDdistDhDir21Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double dAlpha, f45;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][44];
  f45 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][44] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][44]*dAlpha;
  
  return f45*(2*body[iBody].dKecc);
}

double fdDdistDhDir22Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f49, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][48];
  f49 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][48] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][48]*dAlpha;
  
  return f49*(-body[jBody].dHecc);
}

double fdDdistDhDir22Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f49, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][48];
  f49 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][48] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][48]*dAlpha;
  
  return f49*(body[jBody].dKecc);
}

double fdDdistDhDir31Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f82, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][81];
  f82 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][81] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][81]*dAlpha;
  
  return f82*(-6*body[iBody].dHecc*body[iBody].dKecc);
}

double fdDdistDhDir31Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f82, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][81];
  f82 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][81] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][81]*dAlpha;
  
  return f82*((-3*body[iBody].dHecc*body[iBody].dHecc+3*body[iBody].dKecc*body[iBody].dKecc));
}
  
double fdDdistDhDir32Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f83, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][82];
  f83 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][82] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][82]*dAlpha;
  
  return f83*((-2*body[iBody].dHecc*body[jBody].dKecc-2*body[jBody].dHecc*body[iBody].dKecc));
}

double fdDdistDhDir32Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f83, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][82];
  f83 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][82] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][82]*dAlpha;
  
  return f83*((-2*body[iBody].dHecc*body[jBody].dHecc+2*body[iBody].dKecc*body[jBody].dKecc));
} 

double fdDdistDhDir33Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f84, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][83];
  f84 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][83] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][83]*dAlpha;
  
  return f84*((-2*body[jBody].dHecc*body[jBody].dKecc));
}

double fdDdistDhDir33Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f84, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][83];
  f84 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][83] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][83]*dAlpha;
  
  return f84*((body[jBody].dKecc*body[jBody].dKecc-body[jBody].dHecc*body[jBody].dHecc));
}
  
double fdDdistDhDir35Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f86, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][85];
  f86 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][85] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][85]*dAlpha;
  
  return f86*(-2*body[iBody].dPinc*body[iBody].dQinc);
}
 
double fdDdistDhDir35Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f86, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][85];
  f86 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][85] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][85]*dAlpha;
  
  return f86*((body[iBody].dQinc*body[iBody].dQinc-body[iBody].dPinc*body[iBody].dPinc));
}

double fdDdistDhDir37Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f88, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][87];
  f88 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][87] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][87]*dAlpha;
  
  return f88*(-(body[jBody].dPinc*body[iBody].dQinc+body[jBody].dQinc*body[iBody].dPinc));
}

double fdDdistDhDir37Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f88, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][87];
  f88 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][87] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][87]*dAlpha;
  
  return f88*((body[jBody].dQinc*body[iBody].dQinc-body[jBody].dPinc*body[iBody].dPinc));
}

double fdDdistDhDir39Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f86, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][85];
  f86 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][85] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][85]*dAlpha;
  
  return f86*(-2*body[jBody].dPinc*body[jBody].dQinc);
}

double fdDdistDhDir39Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f86, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][85];
  f86 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][85] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][85]*dAlpha;
  
  return f86*((body[jBody].dQinc*body[jBody].dQinc-body[jBody].dPinc*body[jBody].dPinc));
}
  
double fdDdistresDHeccDirCos(BODY *body, SYSTEM *system, int *iaBody, int iIndexJ, int iOrder) {
  double y;
  
  if (iOrder == 1) {
    y = ( fdDdistDhDir11Cos(body, system, iaBody[0], iaBody[1], iIndexJ) \
      + fdDdistDhDir12Cos(body, system, iaBody[0], iaBody[1], iIndexJ)\
      + fdDdistDhDir13Cos(body, system, iaBody[0], iaBody[1], iIndexJ)\
      + fdDdistDhDir14Cos(body, system, iaBody[0], iaBody[1], iIndexJ) \
      + fdDdistDhDir15Cos(body, system, iaBody[0], iaBody[1], iIndexJ)\
      + fdDdistDhDir17Cos(body, system, iaBody[0], iaBody[1], iIndexJ)\
      + fdDdistDhDir18Cos(body, system, iaBody[0], iaBody[1], iIndexJ) \
      + fdDdistDhDir19Cos(body, system, iaBody[0], iaBody[1], iIndexJ)\
      + fdDdistDhDir113Cos(body, system, iaBody[0], iaBody[1], iIndexJ) );
  } else if (iOrder == 2) {
    y = ( fdDdistDhDir21Cos(body, system, iaBody[0], iaBody[1], iIndexJ) \
       + fdDdistDhDir22Cos(body, system, iaBody[0], iaBody[1], iIndexJ) );
  } else if (iOrder == 3) {
    y = ( fdDdistDhDir31Cos(body, system, iaBody[0], iaBody[1], iIndexJ) \
      + fdDdistDhDir32Cos(body, system, iaBody[0], iaBody[1], iIndexJ) \
      + fdDdistDhDir33Cos(body, system, iaBody[0], iaBody[1], iIndexJ) \
      + fdDdistDhDir35Cos(body, system, iaBody[0], iaBody[1], iIndexJ) \
      + fdDdistDhDir37Cos(body, system, iaBody[0], iaBody[1], iIndexJ) \
      + fdDdistDhDir39Cos(body, system, iaBody[0], iaBody[1], iIndexJ) );
  }
  return y;
}

double fdDdistresDHeccDirSin(BODY *body, SYSTEM *system, int *iaBody, int iIndexJ, int iOrder) {
  double y;
  
  if (iOrder == 1) {
    y = ( fdDdistDhDir11Sin(body, system, iaBody[0], iaBody[1], iIndexJ) \
      + fdDdistDhDir12Sin(body, system, iaBody[0], iaBody[1], iIndexJ)\
      + fdDdistDhDir13Sin(body, system, iaBody[0], iaBody[1], iIndexJ)\
      + fdDdistDhDir14Sin(body, system, iaBody[0], iaBody[1], iIndexJ) \
      + fdDdistDhDir15Sin(body, system, iaBody[0], iaBody[1], iIndexJ)\
      + fdDdistDhDir17Sin(body, system, iaBody[0], iaBody[1], iIndexJ)\
      + fdDdistDhDir18Sin(body, system, iaBody[0], iaBody[1], iIndexJ) \
      + fdDdistDhDir19Sin(body, system, iaBody[0], iaBody[1], iIndexJ)\
      + fdDdistDhDir113Sin(body, system, iaBody[0], iaBody[1], iIndexJ) );
  } else if (iOrder == 2) {
    y = ( fdDdistDhDir21Sin(body, system, iaBody[0], iaBody[1], iIndexJ) \
       + fdDdistDhDir22Sin(body, system, iaBody[0], iaBody[1], iIndexJ) );
  } else if (iOrder == 3) {
    y = ( fdDdistDhDir31Sin(body, system, iaBody[0], iaBody[1], iIndexJ) \
      + fdDdistDhDir32Sin(body, system, iaBody[0], iaBody[1], iIndexJ) \
      + fdDdistDhDir33Sin(body, system, iaBody[0], iaBody[1], iIndexJ) \
      + fdDdistDhDir35Sin(body, system, iaBody[0], iaBody[1], iIndexJ) \
      + fdDdistDhDir37Sin(body, system, iaBody[0], iaBody[1], iIndexJ) \
      + fdDdistDhDir39Sin(body, system, iaBody[0], iaBody[1], iIndexJ) );
  }
  return y;
}

double fdDdistresDHecc(BODY *body, SYSTEM *system, int *iaBody, int iIndexJ, int iOrder) {
  double y, yc, ys, dMfac, dSemiPrm;
  dMfac = KGAUSS*KGAUSS*body[iaBody[1]].dMass/MSUN;
  dSemiPrm = body[iaBody[1]].dSemi/AUCM;
  
  yc = fdDdistresDHeccDirCos(body,system,iaBody,iIndexJ,iOrder);
  ys = fdDdistresDHeccDirSin(body,system,iaBody,iIndexJ,iOrder);
  //check for relevant indirect terms
  if (iOrder == 1) {
    if (iIndexJ == 2) {
      yc += fdDdistDhExt13Cos(body,system,iaBody[0],iaBody[1],iIndexJ)*\
        body[iaBody[0]].dSemi/body[iaBody[1]].dSemi;
      ys += fdDdistDhExt13Sin(body,system,iaBody[0],iaBody[1],iIndexJ)*\
        body[iaBody[0]].dSemi/body[iaBody[1]].dSemi;
    } else if (iIndexJ == 3) {
      yc += fdDdistDhExt16Cos(body,system,iaBody[0],iaBody[1],iIndexJ)*\
        body[iaBody[0]].dSemi/body[iaBody[1]].dSemi;
      ys += fdDdistDhExt16Sin(body,system,iaBody[0],iaBody[1],iIndexJ)*\
        body[iaBody[0]].dSemi/body[iaBody[1]].dSemi;
    }
  }
  y = (yc*cos(fdLambdaArg(body,system,iaBody[0],iaBody[1],iIndexJ,iOrder)) \
       + ys*sin(fdLambdaArg(body,system,iaBody[0],iaBody[1],iIndexJ,iOrder))) * dMfac/dSemiPrm; 
  return y;
}

//---------dR/dh'------------------------------------------------------------------

double fdDdistDhPrmDir11Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f29, dAlpha;
  dAlpha = body[jBody].dSemi/body[iBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][28];
  f29 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][28] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][28]*dAlpha;
  
  return 2*body[iBody].dHecc*f29*(body[jBody].dKecc);
}

double fdDdistDhPrmDir11Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f29, dAlpha;
  dAlpha = body[jBody].dSemi/body[iBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][28];
  f29 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][28] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][28]*dAlpha;
  
  return 2*body[iBody].dHecc*f29*(body[jBody].dHecc);
}

double fdDdistDhPrmDir12Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f31, f32, f33, f34, dAlpha;
  dAlpha = body[jBody].dSemi/body[iBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][32];
  f33 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][32] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][32]*dAlpha;  

  return 2*body[iBody].dHecc*body[iBody].dKecc*f33;
}

double fdDdistDhPrmDir12Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f31, f32, f33, f34, dAlpha;
  dAlpha = body[jBody].dSemi/body[iBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][30];
  f31 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][30] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][30]*dAlpha;
        
  dAlpha = body[jBody].dSemi/body[iBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][31];
  f32 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][31] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][31]*dAlpha;      
        
  dAlpha = body[jBody].dSemi/body[iBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][32];
  f33 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][32] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][32]*dAlpha;  

  dAlpha = body[jBody].dSemi/body[iBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][33];
  f34 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][33] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][33]*dAlpha;  

  return (f31+(body[jBody].dHecc*body[jBody].dHecc+body[jBody].dKecc*body[jBody].dKecc)*f32\
    +(3*body[iBody].dHecc*body[iBody].dHecc+body[iBody].dKecc*body[iBody].dKecc)*f33\
    +(body[iBody].dPinc*body[iBody].dPinc+body[iBody].dQinc*body[iBody].dQinc\
    +body[jBody].dPinc*body[jBody].dPinc+body[jBody].dQinc*body[jBody].dQinc)*f34)\
    *sin(fdLambdaArg(body,system,iBody,jBody,iIndexJ,1));
}

double fdDdistDhPrmDir13Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f35, dAlpha;
  dAlpha = body[jBody].dSemi/body[iBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][34];
  f35 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][34] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][34]*dAlpha;  

  return f35*((2*body[jBody].dHecc*body[jBody].dKecc));     
}

double fdDdistDhPrmDir13Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f35, dAlpha;
  dAlpha = body[jBody].dSemi/body[iBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][34];
  f35 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][34] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][34]*dAlpha;  

  return f35*(-(body[jBody].dKecc*body[jBody].dKecc-body[jBody].dHecc*body[jBody].dHecc));     
}

double fdDdistDhPrmDir14Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f36, dAlpha;
  dAlpha = body[jBody].dSemi/body[iBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][35];
  f36 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][35] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][35]*dAlpha;  

  return f36*((-2*body[iBody].dHecc*body[jBody].dKecc+2*body[iBody].dKecc*body[jBody].dHecc));     
}

double fdDdistDhPrmDir14Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f36, dAlpha;
  dAlpha = body[jBody].dSemi/body[iBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][35];
  f36 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][35] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][35]*dAlpha;  

  return f36*((2*body[iBody].dHecc*body[jBody].dHecc+2*body[iBody].dKecc*body[jBody].dKecc));     
}

double fdDdistDhPrmDir16Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f38, dAlpha;
  dAlpha = body[jBody].dSemi/body[iBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][37];
  f38 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][37] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][37]*dAlpha;  

  return f38*((2*body[jBody].dPinc*body[jBody].dQinc));     
}

double fdDdistDhPrmDir16Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f38, dAlpha;
  dAlpha = body[jBody].dSemi/body[iBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][37];
  f38 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][37] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][37]*dAlpha;  

  return f38*(-(body[jBody].dQinc*body[jBody].dQinc-body[jBody].dPinc*body[jBody].dPinc));     
}

double fdDdistDhPrmDir110Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f42, dAlpha;
  dAlpha = body[jBody].dSemi/body[iBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][41];
  f42 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][41] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][41]*dAlpha;  

  return f42*(-(body[iBody].dPinc*body[jBody].dQinc-body[iBody].dQinc*body[jBody].dPinc));     
}

double fdDdistDhPrmDir110Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f42, dAlpha;
  dAlpha = body[jBody].dSemi/body[iBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][41];
  f42 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][41] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][41]*dAlpha;  

  return f42*((body[jBody].dQinc*body[iBody].dQinc+body[jBody].dPinc*body[iBody].dPinc));     
}

double fdDdistDhPrmDir111Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f43, dAlpha;
  dAlpha = body[jBody].dSemi/body[iBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][42];
  f43 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][42] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][42]*dAlpha;  

  return f43*(-(body[jBody].dPinc*body[iBody].dQinc-body[jBody].dQinc*body[iBody].dPinc));     
}

double fdDdistDhPrmDir111Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f43, dAlpha;
  dAlpha = body[jBody].dSemi/body[iBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][42];
  f43 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][42] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][42]*dAlpha;  

  return f43*((body[jBody].dQinc*body[iBody].dQinc+body[jBody].dPinc*body[iBody].dPinc));     
}


double fdDdistDhPrmDir112Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f44, dAlpha;
  dAlpha = body[jBody].dSemi/body[iBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][43];
  f44 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][43] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][43]*dAlpha;  

  return f44*((body[iBody].dPinc*body[jBody].dQinc+body[iBody].dQinc*body[jBody].dPinc));     
}

double fdDdistDhPrmDir112Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f44, dAlpha;
  dAlpha = body[jBody].dSemi/body[iBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][43];
  f44 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][43] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][43]*dAlpha;  

  return f44*((body[jBody].dPinc*body[iBody].dPinc-body[jBody].dQinc*body[iBody].dQinc));     
}

double fdDdistDhPrmDir114Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f38, dAlpha;
  dAlpha = body[jBody].dSemi/body[iBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][37];
  f38 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][37] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][37]*dAlpha;  

  return f38*((2*body[iBody].dPinc*body[iBody].dQinc));     
}

double fdDdistDhPrmDir114Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f38, dAlpha;
  dAlpha = body[jBody].dSemi/body[iBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][37];
  f38 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][37] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][37]*dAlpha;  

  return f38*(-(body[iBody].dQinc*body[iBody].dQinc-body[iBody].dPinc*body[iBody].dPinc));     
}

double fdDdistDhPrmInt13Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return 0.25*(3*body[iBody].dHecc*body[iBody].dKecc);
}

double fdDdistDhPrmInt13Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return 0.25*((-2+(body[jBody].dHecc*body[jBody].dHecc+body[jBody].dKecc*body[jBody].dKecc)\
    +1.5*(3*body[iBody].dHecc*body[iBody].dHecc+body[iBody].dKecc*body[iBody].dKecc)\
    +2*(body[jBody].dPinc*body[jBody].dPinc+body[jBody].dQinc*body[jBody].dQinc\
    +body[iBody].dPinc*body[iBody].dPinc+body[iBody].dQinc*body[iBody].dQinc))\
      *sin(fdLambdaArg(body,system,iBody,jBody,iIndexJ,1)));
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
  dAlpha = body[jBody].dSemi/body[iBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][48];
  f49 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][48] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][48]*dAlpha;
  
  return f49*(-body[jBody].dHecc);
}

double fdDdistDhPrmDir22Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f49, dAlpha;
  dAlpha = body[jBody].dSemi/body[iBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][48];
  f49 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][48] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][48]*dAlpha;
  
  return f49*(body[jBody].dKecc);
}

double fdDdistDhPrmDir23Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f53, dAlpha;
  dAlpha = body[jBody].dSemi/body[iBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][52];
  f53 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][52] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][52]*dAlpha;

  return f53*(-2.*body[iBody].dHecc);
}

double fdDdistDhPrmDir23Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f53, dAlpha;
  dAlpha = body[jBody].dSemi/body[iBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][52];
  f53 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][52] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][52]*dAlpha;

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
  dAlpha = body[jBody].dSemi/body[iBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][82];
  f83 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][82] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][82]*dAlpha;
  
  return f83*((-2*body[jBody].dHecc*body[jBody].dKecc));
}

double fdDdistDhPrmDir32Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f83, dAlpha;
  dAlpha = body[jBody].dSemi/body[iBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][82];
  f83 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][82] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][82]*dAlpha;
  
  return f83*((body[jBody].dKecc*body[jBody].dKecc-body[jBody].dHecc*body[jBody].dHecc));
}

double fdDdistDhPrmDir33Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f84, dAlpha;
  dAlpha = body[jBody].dSemi/body[iBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][83];
  f84 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][83] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][83]*dAlpha;
  
  return f84*((-2*body[iBody].dHecc*body[jBody].dKecc-2*body[jBody].dHecc*body[iBody].dKecc));
}

double fdDdistDhPrmDir33Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f84, dAlpha;
  dAlpha = body[jBody].dSemi/body[iBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][83];
  f84 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][83] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][83]*dAlpha;
  
  return f84*((-2*body[iBody].dHecc*body[jBody].dHecc+2*body[jBody].dKecc*body[iBody].dKecc));
}  

double fdDdistDhPrmDir34Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f85, dAlpha;
  dAlpha = body[jBody].dSemi/body[iBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][84];
  f85 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][84] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][84]*dAlpha;
  
  return f85*(-6*body[iBody].dHecc*body[iBody].dKecc);
}

double fdDdistDhPrmDir34Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f85, dAlpha;
  dAlpha = body[jBody].dSemi/body[iBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][84];
  f85 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][84] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][84]*dAlpha;
  
  return f85*((-3*body[iBody].dHecc*body[iBody].dHecc+3*body[iBody].dKecc*body[iBody].dKecc));
}

double fdDdistDhPrmDir36Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f87, dAlpha;
  dAlpha = body[jBody].dSemi/body[iBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][86];
  f87 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][86] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][86]*dAlpha;
  
  return f87*(-2*body[jBody].dPinc*body[jBody].dQinc);
}

double fdDdistDhPrmDir36Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f87, dAlpha;
  dAlpha = body[jBody].dSemi/body[iBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][86];
  f87 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][86] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][86]*dAlpha;
  
  return f87*((body[jBody].dQinc*body[jBody].dQinc-body[jBody].dPinc*body[jBody].dPinc));
}

double fdDdistDhPrmDir38Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f89, dAlpha;
  dAlpha = body[jBody].dSemi/body[iBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][88];
  f89 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][88] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][88]*dAlpha;
  
  return f89*(-(body[iBody].dPinc*body[jBody].dQinc+body[iBody].dQinc*body[jBody].dPinc));
}

double fdDdistDhPrmDir38Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f89, dAlpha;
  dAlpha = body[jBody].dSemi/body[iBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][88];
  f89 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][88] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][88]*dAlpha;
  
  return f89*((body[iBody].dQinc*body[jBody].dQinc-body[iBody].dPinc*body[jBody].dPinc));
}

double fdDdistDhPrmDir310Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f87, dAlpha;
  dAlpha = body[jBody].dSemi/body[iBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][86];
  f87 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][86] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][86]*dAlpha;
  
  return f87*( -2*body[iBody].dPinc*body[iBody].dQinc);
}

double fdDdistDhPrmDir310Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f87, dAlpha;
  dAlpha = body[jBody].dSemi/body[iBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][86];
  f87 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][86] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][86]*dAlpha;
  
  return f87*((body[iBody].dQinc*body[iBody].dQinc-body[iBody].dPinc*body[iBody].dPinc));
}

double fdDdistresDHeccPrimeDirCos(BODY *body, SYSTEM *system, int *iaBody, int iIndexJ, int iOrder) {
  double y;

  if (iOrder == 1) {
    y = ( fdDdistDhPrmDir11Cos(body, system, iaBody[0], iaBody[1], iIndexJ)\
          + fdDdistDhPrmDir12Cos(body, system, iaBody[0], iaBody[1], iIndexJ) \
          + fdDdistDhPrmDir13Cos(body, system, iaBody[0], iaBody[1], iIndexJ) \
          + fdDdistDhPrmDir14Cos(body, system, iaBody[0], iaBody[1], iIndexJ)\
          + fdDdistDhPrmDir16Cos(body, system, iaBody[0], iaBody[1], iIndexJ) \
          + fdDdistDhPrmDir110Cos(body, system, iaBody[0], iaBody[1], iIndexJ) \
          + fdDdistDhPrmDir111Cos(body, system, iaBody[0], iaBody[1], iIndexJ)\
          + fdDdistDhPrmDir112Cos(body, system, iaBody[0], iaBody[1], iIndexJ) \
          + fdDdistDhPrmDir114Cos(body, system, iaBody[0], iaBody[1], iIndexJ) );
  } else if (iOrder == 2) {
    y = ( fdDdistDhPrmDir22Cos(body, system, iaBody[0], iaBody[1], iIndexJ) \
      + fdDdistDhPrmDir23Cos(body, system, iaBody[0], iaBody[1], iIndexJ) );
  } else if (iOrder == 3) {
    y = ( fdDdistDhPrmDir32Cos(body, system, iaBody[0], iaBody[1], iIndexJ) \
      + fdDdistDhPrmDir33Cos(body, system, iaBody[0], iaBody[1], iIndexJ) \
      + fdDdistDhPrmDir34Cos(body, system, iaBody[0], iaBody[1], iIndexJ) \
      + fdDdistDhPrmDir36Cos(body, system, iaBody[0], iaBody[1], iIndexJ) \
      + fdDdistDhPrmDir38Cos(body, system, iaBody[0], iaBody[1], iIndexJ) \
      + fdDdistDhPrmDir310Cos(body, system, iaBody[0], iaBody[1], iIndexJ) );
  }
  return y;
}

double fdDdistresDHeccPrimeDirSin(BODY *body, SYSTEM *system, int *iaBody, int iIndexJ, int iOrder) {
  double y;

  if (iOrder == 1) {
    y = ( fdDdistDhPrmDir11Sin(body, system, iaBody[0], iaBody[1], iIndexJ)\
          + fdDdistDhPrmDir12Sin(body, system, iaBody[0], iaBody[1], iIndexJ) \
          + fdDdistDhPrmDir13Sin(body, system, iaBody[0], iaBody[1], iIndexJ) \
          + fdDdistDhPrmDir14Sin(body, system, iaBody[0], iaBody[1], iIndexJ)\
          + fdDdistDhPrmDir16Sin(body, system, iaBody[0], iaBody[1], iIndexJ) \
          + fdDdistDhPrmDir110Sin(body, system, iaBody[0], iaBody[1], iIndexJ) \
          + fdDdistDhPrmDir111Sin(body, system, iaBody[0], iaBody[1], iIndexJ)\
          + fdDdistDhPrmDir112Sin(body, system, iaBody[0], iaBody[1], iIndexJ) \
          + fdDdistDhPrmDir114Sin(body, system, iaBody[0], iaBody[1], iIndexJ) );
  } else if (iOrder == 2) {
    y = ( fdDdistDhPrmDir22Sin(body, system, iaBody[0], iaBody[1], iIndexJ) \
      + fdDdistDhPrmDir23Sin(body, system, iaBody[0], iaBody[1], iIndexJ) );
  } else if (iOrder == 3) {
    y = ( fdDdistDhPrmDir32Sin(body, system, iaBody[0], iaBody[1], iIndexJ) \
      + fdDdistDhPrmDir33Sin(body, system, iaBody[0], iaBody[1], iIndexJ) \
      + fdDdistDhPrmDir34Sin(body, system, iaBody[0], iaBody[1], iIndexJ) \
      + fdDdistDhPrmDir36Sin(body, system, iaBody[0], iaBody[1], iIndexJ) \
      + fdDdistDhPrmDir38Sin(body, system, iaBody[0], iaBody[1], iIndexJ) \
      + fdDdistDhPrmDir310Sin(body, system, iaBody[0], iaBody[1], iIndexJ) );
  }
  return y;
}

double fdDdistresDHeccPrime(BODY *body, SYSTEM *system, int *iaBody, int iIndexJ, int iOrder) {
  double y, yc, ys, dMfac, dSemiPrm;
  dMfac = KGAUSS*KGAUSS*body[iaBody[1]].dMass/MSUN;
  dSemiPrm = body[iaBody[0]].dSemi/AUCM;
 
  yc = fdDdistresDHeccPrimeDirCos(body,system,iaBody,iIndexJ,iOrder);
  ys = fdDdistresDHeccPrimeDirSin(body,system,iaBody,iIndexJ,iOrder);
  if (iOrder == 1) {
    if (iIndexJ == 2) {
      yc += (fdDdistDhPrmInt13Cos(body, system, iaBody[0], iaBody[1], iIndexJ)\
            +fdDdistDhPrmInt111Cos(body, system, iaBody[0], iaBody[1], iIndexJ) )\
            /(body[iaBody[1]].dSemi*body[iaBody[1]].dSemi/AUCM/AUCM);
      ys += (fdDdistDhPrmInt13Sin(body, system, iaBody[0], iaBody[1], iIndexJ)\
            +fdDdistDhPrmInt111Sin(body, system, iaBody[0], iaBody[1], iIndexJ) )\
            /(body[iaBody[1]].dSemi*body[iaBody[1]].dSemi/AUCM/AUCM);                
    } else if (iIndexJ == 3) {
      yc += (fdDdistDhPrmInt16Cos(body, system, iaBody[0], iaBody[1], iIndexJ)) \
             /(body[iaBody[1]].dSemi*body[iaBody[1]].dSemi/AUCM/AUCM);
      ys += (fdDdistDhPrmInt16Sin(body, system, iaBody[0], iaBody[1], iIndexJ)) \
             /(body[iaBody[1]].dSemi*body[iaBody[1]].dSemi/AUCM/AUCM);
    }
  } else if (iOrder == 2) {
    if (iIndexJ == 3) {
      yc += (fdDdistDhPrmInt25Cos(body, system, iaBody[0], iaBody[1], iIndexJ))\
             /(body[iaBody[1]].dSemi*body[iaBody[1]].dSemi/AUCM/AUCM);
      ys += (fdDdistDhPrmInt25Sin(body, system, iaBody[0], iaBody[1], iIndexJ))\
             /(body[iaBody[1]].dSemi*body[iaBody[1]].dSemi/AUCM/AUCM);
    }
  }
  y = (yc*cos(fdLambdaArg(body,system,iaBody[0],iaBody[1],iIndexJ,iOrder)) \
       + ys*sin(fdLambdaArg(body,system,iaBody[0],iaBody[1],iIndexJ,iOrder))) * dMfac/dSemiPrm;
  return y;
}

//------------dR/dk--------------------------------------------------------------

double fdDdistDkDir11Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f27, f28, f29, f30, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][26];
  f27 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][26] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][26]*dAlpha;
        
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][27];
  f28 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][27] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][27]*dAlpha;      
        
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][28];
  f29 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][28] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][28]*dAlpha;  

  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][29];
  f30 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][29] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][29]*dAlpha;
  
  return (f27+(body[iBody].dHecc*body[iBody].dHecc+3*body[iBody].dKecc*body[iBody].dKecc)*f28\
    +(body[jBody].dHecc*body[jBody].dHecc+body[jBody].dKecc*body[jBody].dKecc)*f29\
    +(body[iBody].dPinc*body[iBody].dPinc+body[iBody].dQinc*body[iBody].dQinc\
    +body[jBody].dPinc*body[jBody].dPinc+body[jBody].dQinc*body[jBody].dQinc)*f30);
}

double fdDdistDkDir11Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f27, f28, f29, f30, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][27];
  f28 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][27] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][27]*dAlpha;      
  
  return 2*body[iBody].dHecc*body[iBody].dKecc*f28;
}

double fdDdistDkDir12Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f32, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][31];
  f32 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][31] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][31]*dAlpha;
  
  return 2*body[iBody].dKecc*f32*(body[jBody].dKecc);     
}

double fdDdistDkDir12Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f32, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][31];
  f32 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][31] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][31]*dAlpha;
  
  return 2*body[iBody].dKecc*f32*(body[jBody].dHecc);     
}


double fdDdistDkDir13Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f35, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][34];
  f35 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][34] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][34]*dAlpha;  

  return f35*((2*body[iBody].dKecc*body[jBody].dKecc+2*body[iBody].dHecc*body[jBody].dHecc));     
}

double fdDdistDkDir13Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f35, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][34];
  f35 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][34] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][34]*dAlpha;  

  return f35*(-(2*body[iBody].dKecc*body[jBody].dHecc-2*body[iBody].dHecc*body[jBody].dKecc));     
}

double fdDdistDkDir14Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f36, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][35];
  f36 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][35] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][35]*dAlpha;  

  return f36*((body[jBody].dKecc*body[jBody].dKecc-body[jBody].dHecc*body[jBody].dHecc));     
}

double fdDdistDkDir14Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f36, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][35];
  f36 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][35] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][35]*dAlpha;  

  return f36*((2*body[jBody].dHecc*body[jBody].dKecc));     
}

double fdDdistDkDir15Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f37, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][36];
  f37 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][36] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][36]*dAlpha;  

  return f37*((body[iBody].dQinc*body[iBody].dQinc-body[iBody].dPinc*body[iBody].dPinc));     
}

double fdDdistDkDir15Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f37, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][36];
  f37 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][36] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][36]*dAlpha;  

  return f37*((2*body[iBody].dPinc*body[iBody].dQinc));     
}

double fdDdistDkDir17Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f39, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][38];
  f39 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][38] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][38]*dAlpha;  

  return f39*((body[jBody].dQinc*body[iBody].dQinc+body[jBody].dPinc*body[iBody].dPinc));     
}

double fdDdistDkDir17Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f39, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][38];
  f39 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][38] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][38]*dAlpha;  

  return f39*((body[jBody].dPinc*body[iBody].dQinc-body[jBody].dQinc*body[iBody].dPinc));     
}

double fdDdistDkDir18Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f40, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][39];
  f40 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][39] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][39]*dAlpha;  

  return f40*((body[iBody].dQinc*body[jBody].dQinc+body[iBody].dPinc*body[jBody].dPinc));     
}

double fdDdistDkDir18Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f40, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][39];
  f40 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][39] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][39]*dAlpha;  

  return f40*((body[jBody].dQinc*body[iBody].dPinc-body[jBody].dPinc*body[iBody].dQinc));     
}

double fdDdistDkDir19Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f41, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][40];
  f41 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][40] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][40]*dAlpha;  

  return f41*((body[jBody].dQinc*body[iBody].dQinc-body[jBody].dPinc*body[iBody].dPinc));     
}

double fdDdistDkDir19Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f41, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][40];
  f41 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][40] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][40]*dAlpha;  

  return f41*((body[jBody].dQinc*body[iBody].dPinc+body[jBody].dPinc*body[iBody].dQinc));     
}

double fdDdistDkDir113Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f37, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][36];
  f37 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][36] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][36]*dAlpha;  

  return f37*((body[jBody].dQinc*body[jBody].dQinc-body[jBody].dPinc*body[jBody].dPinc));     
}

double fdDdistDkDir113Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f37, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][36];
  f37 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][36] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][36]*dAlpha;  

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
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][44];
  f45 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][44] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][44]*dAlpha;
  
  return f45*(2.*body[iBody].dKecc);
}

double fdDdistDkDir21Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f45, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][44];
  f45 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][44] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][44]*dAlpha;
  
  return f45*(2*body[iBody].dHecc);
}

double fdDdistDkDir22Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f49, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][48];
  f49 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][48] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][48]*dAlpha;
  
  return f49*(body[jBody].dKecc);
}

double fdDdistDkDir22Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f49, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][48];
  f49 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][48] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][48]*dAlpha;
  
  return f49*(body[jBody].dHecc);
}

double fdDdistDkDir31Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f82, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][81];
  f82 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][81] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][81]*dAlpha;
  
  return f82*((3*body[iBody].dKecc*body[iBody].dKecc-3*body[iBody].dHecc*body[iBody].dHecc)); 
}

double fdDdistDkDir31Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f82, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][81];
  f82 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][81] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][81]*dAlpha;
  
  return f82*(6*body[iBody].dKecc*body[iBody].dHecc); 
}  

double fdDdistDkDir32Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f83, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][82];
  f83 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][82] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][82]*dAlpha;
  
  return f83*((2*body[iBody].dKecc*body[jBody].dKecc-2*body[iBody].dHecc*body[jBody].dHecc));
}

double fdDdistDkDir32Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f83, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][82];
  f83 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][82] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][82]*dAlpha;
  
  return f83*((2*body[iBody].dKecc*body[jBody].dHecc+2*body[jBody].dKecc*body[iBody].dHecc));
} 

double fdDdistDkDir33Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f84, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][83];
  f84 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][83] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][83]*dAlpha;
  
  return f84*((body[jBody].dKecc*body[jBody].dKecc-body[jBody].dHecc*body[jBody].dHecc));
} 

double fdDdistDkDir33Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f84, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][83];
  f84 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][83] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][83]*dAlpha;
  
  return f84*(2*body[jBody].dHecc*body[jBody].dHecc);
} 

double fdDdistDkDir35Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f86, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][85];
  f86 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][85] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][85]*dAlpha;
  
  return f86*((body[iBody].dQinc*body[iBody].dQinc-body[iBody].dPinc*body[iBody].dPinc));
}

double fdDdistDkDir35Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f86, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][85];
  f86 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][85] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][85]*dAlpha;
  
  return f86*((2*body[iBody].dPinc*body[iBody].dQinc));
}

double fdDdistDkDir37Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f88, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][87];
  f88 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][87] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][87]*dAlpha;
  
  return f88*( (body[jBody].dQinc*body[iBody].dQinc-body[jBody].dPinc*body[iBody].dPinc));
}

double fdDdistDkDir37Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f88, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][87];
  f88 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][87] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][87]*dAlpha;
  
  return f88*((body[jBody].dPinc*body[iBody].dQinc+body[jBody].dQinc*body[iBody].dPinc));
}

double fdDdistDkDir39Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f86, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][85];
  f86 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][85] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][85]*dAlpha;
  
  return f86*( (body[jBody].dQinc*body[jBody].dQinc-body[jBody].dPinc*body[jBody].dPinc));
}

double fdDdistDkDir39Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f86, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][85];
  f86 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][85] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][85]*dAlpha;
  
  return f86*((2*body[jBody].dPinc*body[jBody].dQinc));
}

double fdDdistresDKeccDirCos(BODY *body, SYSTEM *system, int *iaBody, int iIndexJ, int iOrder) {
  double y;

  if (iOrder == 1) {
    y = ( fdDdistDkDir11Cos(body, system, iaBody[0], iaBody[1], iIndexJ) \
      + fdDdistDkDir12Cos(body, system, iaBody[0], iaBody[1], iIndexJ)\
      + fdDdistDkDir13Cos(body, system, iaBody[0], iaBody[1], iIndexJ)\
      + fdDdistDkDir14Cos(body, system, iaBody[0], iaBody[1], iIndexJ) \
      + fdDdistDkDir15Cos(body, system, iaBody[0], iaBody[1], iIndexJ)\
      + fdDdistDkDir17Cos(body, system, iaBody[0], iaBody[1], iIndexJ)\
      + fdDdistDkDir18Cos(body, system, iaBody[0], iaBody[1], iIndexJ) \
      + fdDdistDkDir19Cos(body, system, iaBody[0], iaBody[1], iIndexJ)\
      + fdDdistDkDir113Cos(body, system, iaBody[0], iaBody[1], iIndexJ) );
  } else if (iOrder == 2) {
    y = ( fdDdistDkDir21Cos(body, system, iaBody[0], iaBody[1], iIndexJ) \
    + fdDdistDkDir22Cos(body, system, iaBody[0], iaBody[1], iIndexJ) );
  } else if (iOrder == 3) {
    y = ( fdDdistDkDir31Cos(body, system, iaBody[0], iaBody[1], iIndexJ) \
      + fdDdistDkDir32Cos(body, system, iaBody[0], iaBody[1], iIndexJ) \
      + fdDdistDkDir33Cos(body, system, iaBody[0], iaBody[1], iIndexJ) \
      + fdDdistDkDir35Cos(body, system, iaBody[0], iaBody[1], iIndexJ) \
      + fdDdistDkDir37Cos(body, system, iaBody[0], iaBody[1], iIndexJ) \
      + fdDdistDkDir39Cos(body, system, iaBody[0], iaBody[1], iIndexJ) );
  }
  return y;
}

double fdDdistresDKeccDirSin(BODY *body, SYSTEM *system, int *iaBody, int iIndexJ, int iOrder) {
  double y;

  if (iOrder == 1) {
    y = ( fdDdistDkDir11Sin(body, system, iaBody[0], iaBody[1], iIndexJ) \
      + fdDdistDkDir12Sin(body, system, iaBody[0], iaBody[1], iIndexJ)\
      + fdDdistDkDir13Sin(body, system, iaBody[0], iaBody[1], iIndexJ)\
      + fdDdistDkDir14Sin(body, system, iaBody[0], iaBody[1], iIndexJ) \
      + fdDdistDkDir15Sin(body, system, iaBody[0], iaBody[1], iIndexJ)\
      + fdDdistDkDir17Sin(body, system, iaBody[0], iaBody[1], iIndexJ)\
      + fdDdistDkDir18Sin(body, system, iaBody[0], iaBody[1], iIndexJ) \
      + fdDdistDkDir19Sin(body, system, iaBody[0], iaBody[1], iIndexJ)\
      + fdDdistDkDir113Sin(body, system, iaBody[0], iaBody[1], iIndexJ) );
  } else if (iOrder == 2) {
    y = ( fdDdistDkDir21Sin(body, system, iaBody[0], iaBody[1], iIndexJ) \
    + fdDdistDkDir22Sin(body, system, iaBody[0], iaBody[1], iIndexJ) );
  } else if (iOrder == 3) {
    y = ( fdDdistDkDir31Sin(body, system, iaBody[0], iaBody[1], iIndexJ) \
      + fdDdistDkDir32Sin(body, system, iaBody[0], iaBody[1], iIndexJ) \
      + fdDdistDkDir33Sin(body, system, iaBody[0], iaBody[1], iIndexJ) \
      + fdDdistDkDir35Sin(body, system, iaBody[0], iaBody[1], iIndexJ) \
      + fdDdistDkDir37Sin(body, system, iaBody[0], iaBody[1], iIndexJ) \
      + fdDdistDkDir39Sin(body, system, iaBody[0], iaBody[1], iIndexJ) );
  }
  return y;
}

double fdDdistresDKecc(BODY *body, SYSTEM *system, int *iaBody, int iIndexJ, int iOrder) {
  double y, yc, ys, dMfac, dSemiPrm;
  dMfac = KGAUSS*KGAUSS*body[iaBody[1]].dMass/MSUN;
  dSemiPrm = body[iaBody[1]].dSemi/AUCM;
  
  yc = fdDdistresDKeccDirCos(body,system,iaBody,iIndexJ,iOrder);
  ys = fdDdistresDKeccDirSin(body,system,iaBody,iIndexJ,iOrder);
  if (iOrder == 1) {
    if (iIndexJ == 2) {
      yc += fdDdistDkExt13Cos(body, system, iaBody[0], iaBody[1], iIndexJ)\
            *body[iaBody[0]].dSemi/body[iaBody[1]].dSemi;
      ys += fdDdistDkExt13Sin(body, system, iaBody[0], iaBody[1], iIndexJ)\
            *body[iaBody[0]].dSemi/body[iaBody[1]].dSemi;
    } else if (iIndexJ == 3) {
      yc += fdDdistDkExt16Cos(body, system, iaBody[0], iaBody[1], iIndexJ)\
            *body[iaBody[0]].dSemi/body[iaBody[1]].dSemi;
      ys += fdDdistDkExt16Sin(body, system, iaBody[0], iaBody[1], iIndexJ)\
            *body[iaBody[0]].dSemi/body[iaBody[1]].dSemi;
    }
  }
  y = (yc*cos(fdLambdaArg(body,system,iaBody[0],iaBody[1],iIndexJ,iOrder)) \
       + ys*sin(fdLambdaArg(body,system,iaBody[0],iaBody[1],iIndexJ,iOrder))) * dMfac/dSemiPrm; 
  return y;
}

//---------dR/dk'------------------------------------------------------------------

double fdDdistDkPrmDir11Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f29, dAlpha;
  dAlpha = body[jBody].dSemi/body[iBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][28];
  f29 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][28] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][28]*dAlpha;
  
  return 2*body[iBody].dKecc*f29*(body[jBody].dKecc);
}

double fdDdistDkPrmDir11Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f29, dAlpha;
  dAlpha = body[jBody].dSemi/body[iBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][28];
  f29 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][28] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][28]*dAlpha;
  
  return 2*body[iBody].dKecc*f29*(body[jBody].dHecc);
}

double fdDdistDkPrmDir12Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f31, f32, f33, f34, dAlpha;
  dAlpha = body[jBody].dSemi/body[iBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][30];
  f31 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][30] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][30]*dAlpha;
        
  dAlpha = body[jBody].dSemi/body[iBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][31];
  f32 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][31] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][31]*dAlpha;      
        
  dAlpha = body[jBody].dSemi/body[iBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][32];
  f33 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][32] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][32]*dAlpha;  

  dAlpha = body[jBody].dSemi/body[iBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][33];
  f34 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][33] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][33]*dAlpha;  

  return (f31+(body[jBody].dHecc*body[jBody].dHecc+body[jBody].dKecc*body[jBody].dKecc)*f32\
    +(body[iBody].dHecc*body[iBody].dHecc+3*body[iBody].dKecc*body[iBody].dKecc)*f33\
    +(body[iBody].dPinc*body[iBody].dPinc+body[iBody].dQinc*body[iBody].dQinc\
    +body[jBody].dPinc*body[jBody].dPinc+body[jBody].dQinc*body[jBody].dQinc)*f34);
}

double fdDdistDkPrmDir12Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f31, f32, f33, f34, dAlpha;
  dAlpha = body[jBody].dSemi/body[iBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][32];
  f33 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][32] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][32]*dAlpha;  

  return 2*body[iBody].dHecc*body[iBody].dKecc*f33;
}

double fdDdistDkPrmDir13Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f35, dAlpha;
  dAlpha = body[jBody].dSemi/body[iBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][34];
  f35 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][34] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][34]*dAlpha;  

  return f35*((body[jBody].dKecc*body[jBody].dKecc-body[jBody].dHecc*body[jBody].dHecc));     
}

double fdDdistDkPrmDir13Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f35, dAlpha;
  dAlpha = body[jBody].dSemi/body[iBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][34];
  f35 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][34] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][34]*dAlpha;  

  return f35*((2*body[jBody].dHecc*body[jBody].dKecc));     
}

double fdDdistDkPrmDir14Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f36, dAlpha;
  dAlpha = body[jBody].dSemi/body[iBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][35];
  f36 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][35] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][35]*dAlpha;  

  return f36*((2*body[iBody].dKecc*body[jBody].dKecc+2*body[iBody].dHecc*body[jBody].dHecc));     
}

double fdDdistDkPrmDir14Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f36, dAlpha;
  dAlpha = body[jBody].dSemi/body[iBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][35];
  f36 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][35] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][35]*dAlpha;  

  return f36*(-(2*body[iBody].dKecc*body[jBody].dHecc-2*body[iBody].dHecc*body[jBody].dKecc));     
}

double fdDdistDkPrmDir16Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f38, dAlpha;
  dAlpha = body[jBody].dSemi/body[iBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][37];
  f38 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][37] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][37]*dAlpha;  

  return f38*((body[jBody].dQinc*body[jBody].dQinc-body[jBody].dPinc*body[jBody].dPinc));     
}

double fdDdistDkPrmDir16Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f38, dAlpha;
  dAlpha = body[jBody].dSemi/body[iBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][37];
  f38 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][37] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][37]*dAlpha;  

  return f38*((2*body[jBody].dPinc*body[jBody].dQinc));     
}

double fdDdistDkPrmDir110Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f42, dAlpha;
  dAlpha = body[jBody].dSemi/body[iBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][41];
  f42 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][41] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][41]*dAlpha;  

  return f42*((body[iBody].dQinc*body[jBody].dQinc+body[iBody].dPinc*body[jBody].dPinc));     
}

double fdDdistDkPrmDir110Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f42, dAlpha;
  dAlpha = body[jBody].dSemi/body[iBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][41];
  f42 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][41] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][41]*dAlpha;  

  return f42*((body[jBody].dQinc*body[iBody].dPinc-body[jBody].dPinc*body[iBody].dQinc));     
}

double fdDdistDkPrmDir111Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f43, dAlpha;
  dAlpha = body[jBody].dSemi/body[iBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][42];
  f43 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][42] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][42]*dAlpha;  

  return f43*((body[jBody].dQinc*body[iBody].dQinc+body[jBody].dPinc*body[iBody].dPinc));     
}

double fdDdistDkPrmDir111Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f43, dAlpha;
  dAlpha = body[jBody].dSemi/body[iBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][42];
  f43 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][42] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][42]*dAlpha;  

  return f43*((body[jBody].dPinc*body[iBody].dQinc-body[jBody].dQinc*body[iBody].dPinc));     
}

double fdDdistDkPrmDir112Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f44, dAlpha;
  dAlpha = body[jBody].dSemi/body[iBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][43];
  f44 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][43] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][43]*dAlpha;  

  return f44*((body[iBody].dQinc*body[jBody].dQinc-body[iBody].dPinc*body[jBody].dPinc));     
}

double fdDdistDkPrmDir112Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f44, dAlpha;
  dAlpha = body[jBody].dSemi/body[iBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][43];
  f44 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][43] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][43]*dAlpha;  

  return f44*((body[jBody].dQinc*body[iBody].dPinc+body[jBody].dPinc*body[iBody].dQinc));     
}


double fdDdistDkPrmDir114Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f38, dAlpha;
  dAlpha = body[jBody].dSemi/body[iBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][37];
  f38 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][37] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][37]*dAlpha;  

  return f38*((body[iBody].dQinc*body[iBody].dQinc-body[iBody].dPinc*body[iBody].dPinc));     
}

double fdDdistDkPrmDir114Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f38, dAlpha;
  dAlpha = body[jBody].dSemi/body[iBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][37];
  f38 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][37] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][37]*dAlpha;  

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
  dAlpha = body[jBody].dSemi/body[iBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][48];
  f49 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][48] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][48]*dAlpha;
  
  return f49*(body[jBody].dKecc);
}

double fdDdistDkPrmDir22Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f49, dAlpha;
  dAlpha = body[jBody].dSemi/body[iBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][48];
  f49 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][48] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][48]*dAlpha;
  
  return f49*(body[jBody].dHecc);
}

double fdDdistDkPrmDir23Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f53, dAlpha;
  dAlpha = body[jBody].dSemi/body[iBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][52];
  f53 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][52] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][52]*dAlpha;
  
  return f53*(2.*body[iBody].dKecc);
}

double fdDdistDkPrmDir23Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f53, dAlpha;
  dAlpha = body[jBody].dSemi/body[iBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][52];
  f53 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][52] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][52]*dAlpha;
  
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
  dAlpha = body[jBody].dSemi/body[iBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][82];
  f83 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][82] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][82]*dAlpha;
  
  return f83*((body[jBody].dKecc*body[jBody].dKecc-body[jBody].dHecc*body[jBody].dHecc));
} 

double fdDdistDkPrmDir32Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f83, dAlpha;
  dAlpha = body[jBody].dSemi/body[iBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][82];
  f83 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][82] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][82]*dAlpha;
  
  return f83*((2*body[jBody].dHecc*body[jBody].dKecc));
}

double fdDdistDkPrmDir33Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f84, dAlpha;
  dAlpha = body[jBody].dSemi/body[iBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][83];
  f84 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][83] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][83]*dAlpha;
  
  return f84*((2*body[jBody].dKecc*body[iBody].dKecc-2*body[jBody].dHecc*body[iBody].dHecc));
}

double fdDdistDkPrmDir33Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f84, dAlpha;
  dAlpha = body[jBody].dSemi/body[iBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][83];
  f84 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][83] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][83]*dAlpha;
  
  return f84*((2*body[jBody].dHecc*body[iBody].dKecc+2*body[jBody].dKecc*body[iBody].dHecc));
}

double fdDdistDkPrmDir34Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f85, dAlpha;
  dAlpha = body[jBody].dSemi/body[iBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][84];
  f85 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][84] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][84]*dAlpha;
  
  return f85*((3*body[iBody].dKecc*body[iBody].dKecc-3*body[iBody].dHecc*body[iBody].dHecc)); 
}

double fdDdistDkPrmDir34Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f85, dAlpha;
  dAlpha = body[jBody].dSemi/body[iBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][84];
  f85 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][84] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][84]*dAlpha;
  
  return f85*(6*body[iBody].dKecc*body[iBody].dHecc); 
}  

double fdDdistDkPrmDir36Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f87, dAlpha;
  dAlpha = body[jBody].dSemi/body[iBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][86];
  f87 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][86] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][86]*dAlpha;
  
  return f87*((body[jBody].dQinc*body[jBody].dQinc-body[jBody].dPinc*body[jBody].dPinc));
}

double fdDdistDkPrmDir36Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f87, dAlpha;
  dAlpha = body[jBody].dSemi/body[iBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][86];
  f87 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][86] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][86]*dAlpha;
  
  return f87*((2*body[jBody].dPinc*body[jBody].dQinc));
}

double fdDdistDkPrmDir38Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f89, dAlpha;
  dAlpha = body[jBody].dSemi/body[iBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][88];
  f89 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][88] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][88]*dAlpha;
  
  return f89*( (body[iBody].dQinc*body[jBody].dQinc-body[jBody].dPinc*body[iBody].dPinc));
}

double fdDdistDkPrmDir38Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f89, dAlpha;
  dAlpha = body[jBody].dSemi/body[iBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][88];
  f89 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][88] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][88]*dAlpha;
  
  return f89*((body[iBody].dPinc*body[jBody].dQinc+body[iBody].dQinc*body[jBody].dPinc));
}

double fdDdistDkPrmDir310Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f87, dAlpha;
  dAlpha = body[jBody].dSemi/body[iBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][86];
  f87 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][86] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][86]*dAlpha;
  
  return f87*( (body[iBody].dQinc*body[iBody].dQinc-body[iBody].dPinc*body[iBody].dPinc));
}

double fdDdistDkPrmDir310Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f87, dAlpha;
  dAlpha = body[jBody].dSemi/body[iBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][86];
  f87 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][86] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][86]*dAlpha;
  
  return f87*((2*body[iBody].dPinc*body[iBody].dQinc));
} 

double fdDdistresDKeccPrimeDirCos(BODY *body, SYSTEM *system, int *iaBody, int iIndexJ, int iOrder) {
  double y;
  
  if (iOrder == 1) {
    y = (fdDdistDkPrmDir11Cos(body, system, iaBody[0], iaBody[1], iIndexJ)\
          + fdDdistDkPrmDir12Cos(body, system, iaBody[0], iaBody[1], iIndexJ) \
          + fdDdistDkPrmDir13Cos(body, system, iaBody[0], iaBody[1], iIndexJ) \
          + fdDdistDkPrmDir14Cos(body, system, iaBody[0], iaBody[1], iIndexJ)\
          + fdDdistDkPrmDir16Cos(body, system, iaBody[0], iaBody[1], iIndexJ) \
          + fdDdistDkPrmDir110Cos(body, system, iaBody[0], iaBody[1], iIndexJ) \
          + fdDdistDkPrmDir111Cos(body, system, iaBody[0], iaBody[1], iIndexJ)\
          + fdDdistDkPrmDir112Cos(body, system, iaBody[0], iaBody[1], iIndexJ) \
          + fdDdistDkPrmDir114Cos(body, system, iaBody[0], iaBody[1], iIndexJ) );
  } else if (iOrder == 2) {
    y = ( fdDdistDkPrmDir22Cos(body, system, iaBody[0], iaBody[1], iIndexJ) \
    + fdDdistDkPrmDir23Cos(body, system, iaBody[0], iaBody[1], iIndexJ) );
  } else if (iOrder == 3) {
    y = ( fdDdistDkPrmDir32Cos(body, system, iaBody[0], iaBody[1], iIndexJ) \
      + fdDdistDkPrmDir33Cos(body, system, iaBody[0], iaBody[1], iIndexJ) \
      + fdDdistDkPrmDir34Cos(body, system, iaBody[0], iaBody[1], iIndexJ) \
      + fdDdistDkPrmDir36Cos(body, system, iaBody[0], iaBody[1], iIndexJ) \
      + fdDdistDkPrmDir38Cos(body, system, iaBody[0], iaBody[1], iIndexJ) \
      + fdDdistDkPrmDir310Cos(body, system, iaBody[0], iaBody[1], iIndexJ) );
  }
  return y;
}

double fdDdistresDKeccPrimeDirSin(BODY *body, SYSTEM *system, int *iaBody, int iIndexJ, int iOrder) {
  double y;
  
  if (iOrder == 1) {
    y = (fdDdistDkPrmDir11Sin(body, system, iaBody[0], iaBody[1], iIndexJ)\
          + fdDdistDkPrmDir12Sin(body, system, iaBody[0], iaBody[1], iIndexJ) \
          + fdDdistDkPrmDir13Sin(body, system, iaBody[0], iaBody[1], iIndexJ) \
          + fdDdistDkPrmDir14Sin(body, system, iaBody[0], iaBody[1], iIndexJ)\
          + fdDdistDkPrmDir16Sin(body, system, iaBody[0], iaBody[1], iIndexJ) \
          + fdDdistDkPrmDir110Sin(body, system, iaBody[0], iaBody[1], iIndexJ) \
          + fdDdistDkPrmDir111Sin(body, system, iaBody[0], iaBody[1], iIndexJ)\
          + fdDdistDkPrmDir112Sin(body, system, iaBody[0], iaBody[1], iIndexJ) \
          + fdDdistDkPrmDir114Sin(body, system, iaBody[0], iaBody[1], iIndexJ) );
  } else if (iOrder == 2) {
    y = ( fdDdistDkPrmDir22Sin(body, system, iaBody[0], iaBody[1], iIndexJ) \
    + fdDdistDkPrmDir23Sin(body, system, iaBody[0], iaBody[1], iIndexJ) );
  } else if (iOrder == 3) {
    y = ( fdDdistDkPrmDir32Sin(body, system, iaBody[0], iaBody[1], iIndexJ) \
      + fdDdistDkPrmDir33Sin(body, system, iaBody[0], iaBody[1], iIndexJ) \
      + fdDdistDkPrmDir34Sin(body, system, iaBody[0], iaBody[1], iIndexJ) \
      + fdDdistDkPrmDir36Sin(body, system, iaBody[0], iaBody[1], iIndexJ) \
      + fdDdistDkPrmDir38Sin(body, system, iaBody[0], iaBody[1], iIndexJ) \
      + fdDdistDkPrmDir310Sin(body, system, iaBody[0], iaBody[1], iIndexJ) );
  }
  return y;
}

double fdDdistresDKeccPrime(BODY *body, SYSTEM *system, int *iaBody, int iIndexJ, int iOrder) {
  double y, yc, ys, dMfac, dSemiPrm;
  dMfac = KGAUSS*KGAUSS*body[iaBody[1]].dMass/MSUN;
  dSemiPrm = body[iaBody[0]].dSemi/AUCM;
  
  yc = fdDdistresDKeccPrimeDirCos(body,system,iaBody,iIndexJ,iOrder);
  ys = fdDdistresDKeccPrimeDirSin(body,system,iaBody,iIndexJ,iOrder);
  if (iOrder == 1) {
    if (iIndexJ == 2) {
      yc += (fdDdistDkPrmInt13Cos(body, system, iaBody[0], iaBody[1], iIndexJ)\
            +fdDdistDkPrmInt111Cos(body, system, iaBody[0], iaBody[1], iIndexJ) )\
            /(body[iaBody[1]].dSemi*body[iaBody[1]].dSemi/AUCM/AUCM);
      ys += (fdDdistDkPrmInt13Sin(body, system, iaBody[0], iaBody[1], iIndexJ)\
            +fdDdistDkPrmInt111Sin(body, system, iaBody[0], iaBody[1], iIndexJ) )\
            /(body[iaBody[1]].dSemi*body[iaBody[1]].dSemi/AUCM/AUCM);
    } else if (iIndexJ == 3) {
      yc += (fdDdistDkPrmInt16Cos(body, system, iaBody[0], iaBody[1], iIndexJ))\
             /(body[iaBody[1]].dSemi*body[iaBody[1]].dSemi/AUCM/AUCM);
      ys += (fdDdistDkPrmInt16Sin(body, system, iaBody[0], iaBody[1], iIndexJ))\
             /(body[iaBody[1]].dSemi*body[iaBody[1]].dSemi/AUCM/AUCM);
    }
  } else if (iOrder == 2) {
    if (iIndexJ == 3) {
      yc += (fdDdistDkPrmInt25Cos(body, system, iaBody[0], iaBody[1], iIndexJ))\
             /(body[iaBody[1]].dSemi*body[iaBody[1]].dSemi/AUCM/AUCM);
      ys += (fdDdistDkPrmInt25Sin(body, system, iaBody[0], iaBody[1], iIndexJ))\
             /(body[iaBody[1]].dSemi*body[iaBody[1]].dSemi/AUCM/AUCM);
    }
  } 
  y = (yc*cos(fdLambdaArg(body,system,iaBody[0],iaBody[1],iIndexJ,iOrder)) \
       + ys*sin(fdLambdaArg(body,system,iaBody[0],iaBody[1],iIndexJ,iOrder))) * dMfac/dSemiPrm;
  return y;
}

//---------dR/dp------------------------------------------------------------------

double fdDdistDpDir11Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f30, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][29];
  f30 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][29] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][29]*dAlpha;
  
  return 2*body[iBody].dPinc*f30*(body[iBody].dKecc);
}

double fdDdistDpDir11Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f30, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][29];
  f30 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][29] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][29]*dAlpha;
  
  return 2*body[iBody].dPinc*f30*(body[iBody].dHecc);
}

double fdDdistDpDir12Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f34, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][33];
  f34 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][33] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][33]*dAlpha;
  
  return 2*body[iBody].dPinc*f34*(body[jBody].dKecc);
}

double fdDdistDpDir12Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f34, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][33];
  f34 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][33] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][33]*dAlpha;
  
  return 2*body[iBody].dPinc*f34*(body[jBody].dHecc);
}

double fdDdistDpDir15Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f37, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][36];
  f37 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][36] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][36]*dAlpha;  

  return f37*((-2*body[iBody].dPinc*body[iBody].dKecc+2*body[iBody].dQinc*body[iBody].dHecc));     
}

double fdDdistDpDir15Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f37, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][36];
  f37 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][36] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][36]*dAlpha;  

  return f37*((2*body[iBody].dPinc*body[iBody].dHecc+2*body[iBody].dQinc*body[iBody].dKecc));     
}

double fdDdistDpDir16Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f38, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][37];
  f38 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][37] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][37]*dAlpha;  

  return f38*((-2*body[iBody].dPinc*body[jBody].dKecc+2*body[iBody].dQinc*body[jBody].dHecc));     
}

double fdDdistDpDir16Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f38, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][37];
  f38 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][37] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][37]*dAlpha;  

  return f38*((2*body[iBody].dPinc*body[jBody].dHecc+2*body[iBody].dQinc*body[jBody].dKecc));     
}

double fdDdistDpDir17Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f39, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][38];
  f39 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][38] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][38]*dAlpha;  

  return f39*((body[jBody].dPinc*body[iBody].dKecc+body[jBody].dQinc*body[iBody].dHecc));     
}

double fdDdistDpDir17Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f39, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][38];
  f39 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][38] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][38]*dAlpha;  

  return f39*((body[jBody].dPinc*body[iBody].dHecc-body[jBody].dQinc*body[iBody].dKecc));     
}

double fdDdistDpDir18Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f40, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][39];
  f40 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][39] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][39]*dAlpha;  

  return f40*((body[jBody].dPinc*body[iBody].dKecc-body[jBody].dQinc*body[iBody].dHecc));     
}

double fdDdistDpDir18Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f40, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][39];
  f40 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][39] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][39]*dAlpha;  

  return f40*((body[jBody].dPinc*body[iBody].dHecc+body[jBody].dQinc*body[iBody].dKecc));     
}

double fdDdistDpDir19Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f41, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][40];
  f41 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][40] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][40]*dAlpha;  

  return f41*(-(body[jBody].dPinc*body[iBody].dKecc-body[jBody].dQinc*body[iBody].dHecc));     
}

double fdDdistDpDir19Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f41, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][40];
  f41 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][40] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][40]*dAlpha;  

  return f41*((body[jBody].dQinc*body[iBody].dKecc+body[jBody].dPinc*body[iBody].dHecc));     
}

double fdDdistDpDir110Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f42, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][41];
  f42 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][41] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][41]*dAlpha;  

  return f42*((body[jBody].dPinc*body[jBody].dKecc+body[jBody].dQinc*body[jBody].dHecc));     
}

double fdDdistDpDir110Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f42, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][41];
  f42 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][41] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][41]*dAlpha;  

  return f42*((body[jBody].dPinc*body[jBody].dHecc-body[jBody].dQinc*body[jBody].dKecc));     
}

double fdDdistDpDir111Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f43, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][42];
  f43 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][42] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][42]*dAlpha;  

  return f43*((body[jBody].dPinc*body[jBody].dKecc-body[jBody].dQinc*body[jBody].dHecc));     
}

double fdDdistDpDir111Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f43, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][42];
  f43 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][42] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][42]*dAlpha;  

  return f43*((body[jBody].dPinc*body[jBody].dHecc+body[jBody].dQinc*body[jBody].dKecc));     
}

double fdDdistDpDir112Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f44, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][43];
  f44 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][43] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][43]*dAlpha;  

  return f44*(-(body[jBody].dPinc*body[jBody].dKecc-body[jBody].dQinc*body[jBody].dHecc));     
}

double fdDdistDpDir112Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f44, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][43];
  f44 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][43] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][43]*dAlpha;  

  return f44*((body[jBody].dPinc*body[jBody].dHecc+body[jBody].dQinc*body[jBody].dKecc));     
}

double fdDdistDpExt13Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return 4*body[iBody].dPinc*(body[jBody].dKecc);     
}

double fdDdistDpExt13Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return 4*body[iBody].dPinc*(body[jBody].dHecc);     
}

double fdDdistDpExt111Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return -4*((body[jBody].dPinc*body[jBody].dKecc+body[jBody].dQinc*body[jBody].dHecc));     
}

double fdDdistDpExt111Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return -4*((-body[jBody].dPinc*body[jBody].dHecc+body[jBody].dQinc*body[jBody].dKecc));     
}

double fdDdistDpDir24Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f57, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][56];
  f57 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][56] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][56]*dAlpha;
  
  return f57*(-2.*body[iBody].dPinc);
}

double fdDdistDpDir24Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f57, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][56];
  f57 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][56] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][56]*dAlpha;
  
  return f57*(2*body[iBody].dQinc);
}

double fdDdistDpDir25Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f62, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][61];
  f62 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][61] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][61]*dAlpha;

  return f62*(-body[jBody].dPinc);
}

double fdDdistDpDir25Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f62, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][61];
  f62 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][61] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][61]*dAlpha;

  return f62*(body[jBody].dQinc);
}

double fdDdistDpDir35Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f86, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][85];
  f86 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][85] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][85]*dAlpha;
  
  return f86*((-2*body[iBody].dKecc*body[iBody].dPinc-2*body[iBody].dHecc*body[iBody].dQinc));
}

double fdDdistDpDir35Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f86, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][85];
  f86 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][85] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][85]*dAlpha;
  
  return f86*((-2*body[iBody].dHecc*body[iBody].dPinc+2*body[iBody].dKecc*body[iBody].dQinc));
}

double fdDdistDpDir36Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f87, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][86];
  f87 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][86] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][86]*dAlpha;
  
  return f87*((-2*body[jBody].dKecc*body[iBody].dPinc-2*body[jBody].dHecc*body[iBody].dQinc));
}

double fdDdistDpDir36Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f87, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][86];
  f87 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][86] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][86]*dAlpha;
  
  return f87*((-2*body[jBody].dHecc*body[iBody].dPinc+2*body[jBody].dKecc*body[iBody].dQinc));
}

double fdDdistDpDir37Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f88, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][87];
  f88 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][87] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][87]*dAlpha;
  
  return f88*( (-body[jBody].dPinc*body[iBody].dKecc-body[jBody].dQinc*body[iBody].dHecc));
}

double fdDdistDpDir37Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f88, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][87];
  f88 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][87] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][87]*dAlpha;
  
  return f88*((-body[jBody].dPinc*body[iBody].dHecc+body[jBody].dQinc*body[iBody].dKecc));
}

double fdDdistDpDir38Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f89, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][88];
  f89 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][88] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][88]*dAlpha;
  
  return f89*( (-body[jBody].dPinc*body[jBody].dKecc-body[jBody].dQinc*body[jBody].dHecc));
}

double fdDdistDpDir38Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f89, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][88];
  f89 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][88] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][88]*dAlpha;
  
  return f89*((-body[jBody].dPinc*body[jBody].dHecc+body[jBody].dQinc*body[jBody].dKecc));
}

double fdDdistresDPincDirCos(BODY *body, SYSTEM *system, int *iaBody, int iIndexJ, int iOrder) {
  double y;
 
  if (iOrder == 1) {
    y =  (fdDdistDpDir11Cos(body, system, iaBody[0], iaBody[1], iIndexJ) \
    + fdDdistDpDir12Cos(body, system, iaBody[0], iaBody[1], iIndexJ)  \
    + fdDdistDpDir15Cos(body, system, iaBody[0], iaBody[1], iIndexJ) \
    + fdDdistDpDir16Cos(body, system, iaBody[0], iaBody[1], iIndexJ) \
    + fdDdistDpDir17Cos(body, system, iaBody[0], iaBody[1], iIndexJ) \
    + fdDdistDpDir18Cos(body, system, iaBody[0], iaBody[1], iIndexJ) \
    + fdDdistDpDir19Cos(body, system, iaBody[0], iaBody[1], iIndexJ) \
    + fdDdistDpDir110Cos(body, system, iaBody[0], iaBody[1], iIndexJ) \
    + fdDdistDpDir111Cos(body, system, iaBody[0], iaBody[1], iIndexJ) \
    + fdDdistDpDir112Cos(body, system, iaBody[0], iaBody[1], iIndexJ) );
  } else if (iOrder == 2) {  
    y = ( fdDdistDpDir24Cos(body, system, iaBody[0], iaBody[1], iIndexJ) \
    + fdDdistDpDir25Cos(body, system, iaBody[0], iaBody[1], iIndexJ) );
  } else if (iOrder == 3) {
    y = ( fdDdistDpDir35Cos(body, system, iaBody[0], iaBody[1], iIndexJ) \
      + fdDdistDpDir36Cos(body, system, iaBody[0], iaBody[1], iIndexJ) \
      + fdDdistDpDir37Cos(body, system, iaBody[0], iaBody[1], iIndexJ) \
      + fdDdistDpDir38Cos(body, system, iaBody[0], iaBody[1], iIndexJ) );
  }
  return y;
}

double fdDdistresDPincDirSin(BODY *body, SYSTEM *system, int *iaBody, int iIndexJ, int iOrder) {
  double y;
 
  if (iOrder == 1) {
    y =  (fdDdistDpDir11Sin(body, system, iaBody[0], iaBody[1], iIndexJ) \
    + fdDdistDpDir12Sin(body, system, iaBody[0], iaBody[1], iIndexJ)  \
    + fdDdistDpDir15Sin(body, system, iaBody[0], iaBody[1], iIndexJ) \
    + fdDdistDpDir16Sin(body, system, iaBody[0], iaBody[1], iIndexJ) \
    + fdDdistDpDir17Sin(body, system, iaBody[0], iaBody[1], iIndexJ) \
    + fdDdistDpDir18Sin(body, system, iaBody[0], iaBody[1], iIndexJ) \
    + fdDdistDpDir19Sin(body, system, iaBody[0], iaBody[1], iIndexJ) \
    + fdDdistDpDir110Sin(body, system, iaBody[0], iaBody[1], iIndexJ) \
    + fdDdistDpDir111Sin(body, system, iaBody[0], iaBody[1], iIndexJ) \
    + fdDdistDpDir112Sin(body, system, iaBody[0], iaBody[1], iIndexJ) );
  } else if (iOrder == 2) {  
    y = ( fdDdistDpDir24Sin(body, system, iaBody[0], iaBody[1], iIndexJ) \
    + fdDdistDpDir25Sin(body, system, iaBody[0], iaBody[1], iIndexJ) );
  } else if (iOrder == 3) {
    y = ( fdDdistDpDir35Sin(body, system, iaBody[0], iaBody[1], iIndexJ) \
      + fdDdistDpDir36Sin(body, system, iaBody[0], iaBody[1], iIndexJ) \
      + fdDdistDpDir37Sin(body, system, iaBody[0], iaBody[1], iIndexJ) \
      + fdDdistDpDir38Sin(body, system, iaBody[0], iaBody[1], iIndexJ) );
  }
  return y;
}


double fdDdistresDPinc(BODY *body, SYSTEM *system, int *iaBody, int iIndexJ, int iOrder) {
  double y, yc, ys, dMfac, dSemiPrm;
  dMfac = KGAUSS*KGAUSS*body[iaBody[1]].dMass/MSUN;
  dSemiPrm = body[iaBody[1]].dSemi/AUCM;

  yc = fdDdistresDPincDirCos(body,system,iaBody,iIndexJ,iOrder);
  ys = fdDdistresDPincDirSin(body,system,iaBody,iIndexJ,iOrder);
  
  if (iOrder == 1) {
    if (iIndexJ == 2) {
      yc += (fdDdistDpExt13Cos(body, system, iaBody[0], iaBody[1], iIndexJ) \
           +fdDdistDpExt111Cos(body, system, iaBody[0], iaBody[1], iIndexJ))\
            *body[iaBody[0]].dSemi/body[iaBody[1]].dSemi;
      ys += (fdDdistDpExt13Sin(body, system, iaBody[0], iaBody[1], iIndexJ) \
           +fdDdistDpExt111Sin(body, system, iaBody[0], iaBody[1], iIndexJ))\
            *body[iaBody[0]].dSemi/body[iaBody[1]].dSemi;
    }
  } 
  
  y = (yc*cos(fdLambdaArg(body,system,iaBody[0],iaBody[1],iIndexJ,iOrder)) \
       + ys*sin(fdLambdaArg(body,system,iaBody[0],iaBody[1],iIndexJ,iOrder))) * dMfac/dSemiPrm;
  return y;
}


//---------dR/dp'------------------------------------------------------------------

double fdDdistDpPrmDir11Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f30, dAlpha;
  dAlpha = body[jBody].dSemi/body[iBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][29];
  f30 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][29] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][29]*dAlpha;
  
  return 2*body[iBody].dPinc*f30*(body[jBody].dKecc);
}

double fdDdistDpPrmDir11Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f30, dAlpha;
  dAlpha = body[jBody].dSemi/body[iBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][29];
  f30 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][29] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][29]*dAlpha;
  
  return 2*body[iBody].dPinc*f30*(body[jBody].dHecc);
}

double fdDdistDpPrmDir12Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f34, dAlpha;
  dAlpha = body[jBody].dSemi/body[iBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][33];
  f34 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][33] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][33]*dAlpha;
  
  return 2*body[iBody].dPinc*f34*(body[iBody].dKecc);
}

double fdDdistDpPrmDir12Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f34, dAlpha;
  dAlpha = body[jBody].dSemi/body[iBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][33];
  f34 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][33] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][33]*dAlpha;
  
  return 2*body[iBody].dPinc*f34*(body[iBody].dHecc);
}

double fdDdistDpPrmDir17Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f39, dAlpha;
  dAlpha = body[jBody].dSemi/body[iBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][38];
  f39 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][38] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][38]*dAlpha;  

  return f39*((body[jBody].dPinc*body[jBody].dKecc-body[jBody].dQinc*body[jBody].dHecc));     
}

double fdDdistDpPrmDir17Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f39, dAlpha;
  dAlpha = body[jBody].dSemi/body[iBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][38];
  f39 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][38] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][38]*dAlpha;  

  return f39*((body[jBody].dPinc*body[jBody].dHecc+body[jBody].dQinc*body[jBody].dKecc));     
}

double fdDdistDpPrmDir18Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f40, dAlpha;
  dAlpha = body[jBody].dSemi/body[iBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][39];
  f40 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][39] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][39]*dAlpha;  

  return f40*((body[jBody].dPinc*body[jBody].dKecc+body[jBody].dQinc*body[jBody].dHecc));     
}

double fdDdistDpPrmDir18Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f40, dAlpha;
  dAlpha = body[jBody].dSemi/body[iBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][39];
  f40 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][39] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][39]*dAlpha;  

  return f40*((body[jBody].dPinc*body[jBody].dHecc-body[jBody].dQinc*body[jBody].dKecc));     
}

double fdDdistDpPrmDir19Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f41, dAlpha;
  dAlpha = body[jBody].dSemi/body[iBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][40];
  f41 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][40] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][40]*dAlpha;  

  return f41*((body[jBody].dQinc*body[jBody].dHecc-body[jBody].dPinc*body[jBody].dKecc));     
}

double fdDdistDpPrmDir19Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f41, dAlpha;
  dAlpha = body[jBody].dSemi/body[iBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][40];
  f41 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][40] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][40]*dAlpha;  

  return f41*((body[jBody].dPinc*body[jBody].dHecc+body[jBody].dQinc*body[jBody].dKecc));     
}

double fdDdistDpPrmDir110Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f42, dAlpha;
  dAlpha = body[jBody].dSemi/body[iBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][41];
  f42 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][41] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][41]*dAlpha;  

  return f42*((body[jBody].dPinc*body[iBody].dKecc-body[jBody].dQinc*body[iBody].dHecc));     
}

double fdDdistDpPrmDir110Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f42, dAlpha;
  dAlpha = body[jBody].dSemi/body[iBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][41];
  f42 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][41] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][41]*dAlpha;  

  return f42*((body[jBody].dPinc*body[iBody].dHecc+body[jBody].dQinc*body[iBody].dKecc));     
}

double fdDdistDpPrmDir111Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f43, dAlpha;
  dAlpha = body[jBody].dSemi/body[iBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][42];
  f43 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][42] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][42]*dAlpha;  

  return f43*((body[jBody].dPinc*body[iBody].dKecc+body[jBody].dQinc*body[iBody].dHecc));     
}

double fdDdistDpPrmDir111Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f43, dAlpha;
  dAlpha = body[jBody].dSemi/body[iBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][42];
  f43 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][42] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][42]*dAlpha;  

  return f43*((body[jBody].dPinc*body[iBody].dHecc-body[jBody].dQinc*body[iBody].dKecc));     
}

double fdDdistDpPrmDir112Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f44, dAlpha;
  dAlpha = body[jBody].dSemi/body[iBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][43];
  f44 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][43] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][43]*dAlpha;  

  return f44*((body[jBody].dQinc*body[iBody].dHecc-body[jBody].dPinc*body[iBody].dKecc));     
}

double fdDdistDpPrmDir112Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f44, dAlpha;
  dAlpha = body[jBody].dSemi/body[iBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][43];
  f44 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][43] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][43]*dAlpha;  

  return f44*((body[jBody].dPinc*body[iBody].dHecc+body[jBody].dQinc*body[iBody].dKecc));     
}

double fdDdistDpPrmDir113Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f37, dAlpha;
  dAlpha = body[jBody].dSemi/body[iBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][36];
  f37 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][36] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][36]*dAlpha;  

  return f37*((-2*body[iBody].dPinc*body[jBody].dKecc+2*body[iBody].dQinc*body[jBody].dHecc));     
}

double fdDdistDpPrmDir113Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f37, dAlpha;
  dAlpha = body[jBody].dSemi/body[iBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][36];
  f37 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][36] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][36]*dAlpha;  

  return f37*((2*body[iBody].dPinc*body[jBody].dHecc+2*body[iBody].dQinc*body[jBody].dKecc));     
}

double fdDdistDpPrmDir114Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f38, dAlpha;
  dAlpha = body[jBody].dSemi/body[iBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][37];
  f38 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][37] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][37]*dAlpha;  

  return f38*((-2*body[iBody].dPinc*body[iBody].dKecc+2*body[iBody].dQinc*body[iBody].dHecc));     
}

double fdDdistDpPrmDir114Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f38, dAlpha;
  dAlpha = body[jBody].dSemi/body[iBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][37];
  f38 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][37] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][37]*dAlpha;  

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
  dAlpha = body[jBody].dSemi/body[iBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][61];
  f62 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][61] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][61]*dAlpha;

  return f62*(-body[jBody].dPinc);
}

double fdDdistDpPrmDir25Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f62, dAlpha;
  dAlpha = body[jBody].dSemi/body[iBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][61];
  f62 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][61] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][61]*dAlpha;

  return f62*(body[jBody].dQinc);
}

double fdDdistDpPrmDir26Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f57, dAlpha;
  dAlpha = body[jBody].dSemi/body[iBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][56];
  f57 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][56] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][56]*dAlpha;
  
  return f57*(-2.*body[iBody].dPinc);
}

double fdDdistDpPrmDir26Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f57, dAlpha;
  dAlpha = body[jBody].dSemi/body[iBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][56];
  f57 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][56] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][56]*dAlpha;
  
  return f57*(2*body[iBody].dQinc);
}

double fdDdistDpPrmDir37Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f88, dAlpha;
  dAlpha = body[jBody].dSemi/body[iBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][87];
  f88 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][87] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][87]*dAlpha;
  
  return f88*( (-body[jBody].dPinc*body[jBody].dKecc-body[jBody].dQinc*body[jBody].dHecc));
}

double fdDdistDpPrmDir37Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f88, dAlpha;
  dAlpha = body[jBody].dSemi/body[iBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][87];
  f88 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][87] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][87]*dAlpha;
  
  return f88*( (-body[jBody].dPinc*body[jBody].dHecc+body[jBody].dQinc*body[jBody].dKecc));
}

double fdDdistDpPrmDir38Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f89, dAlpha;
  dAlpha = body[jBody].dSemi/body[iBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][88];
  f89 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][88] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][88]*dAlpha;
  
  return f89*( (-body[jBody].dPinc*body[iBody].dKecc-body[jBody].dQinc*body[iBody].dHecc));
}

double fdDdistDpPrmDir38Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f89, dAlpha;
  dAlpha = body[jBody].dSemi/body[iBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][88];
  f89 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][88] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][88]*dAlpha;
  
  return f89*((-body[jBody].dPinc*body[iBody].dHecc+body[jBody].dQinc*body[iBody].dKecc));
}

double fdDdistDpPrmDir39Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f86, dAlpha;
  dAlpha = body[jBody].dSemi/body[iBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][85];
  f86 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][85] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][85]*dAlpha;
  
  return f86*( (-2*body[iBody].dPinc*body[jBody].dKecc-2*body[jBody].dHecc*body[iBody].dQinc));
}

double fdDdistDpPrmDir39Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f86, dAlpha;
  dAlpha = body[jBody].dSemi/body[iBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][85];
  f86 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][85] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][85]*dAlpha;
  
  return f86*( (-2*body[iBody].dPinc*body[jBody].dHecc+2*body[jBody].dKecc*body[iBody].dQinc));
}

double fdDdistDpPrmDir310Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f87, dAlpha;
  dAlpha = body[jBody].dSemi/body[iBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][86];
  f87 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][86] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][86]*dAlpha;
  
  return f87*( (-2*body[iBody].dPinc*body[iBody].dKecc-2*body[iBody].dHecc*body[iBody].dQinc));
}

double fdDdistDpPrmDir310Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f87, dAlpha;
  dAlpha = body[jBody].dSemi/body[iBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][86];
  f87 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][86] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][86]*dAlpha;
  
  return f87*((-2*body[iBody].dPinc*body[iBody].dHecc+2*body[iBody].dKecc*body[iBody].dQinc));
}

double fdDdistresDPincPrimeDirCos(BODY *body, SYSTEM *system, int *iaBody, int iIndexJ, int iOrder) {
  double y;
  
  if (iOrder == 1) {
    y = (fdDdistDpPrmDir11Cos(body, system, iaBody[0], iaBody[1], iIndexJ)\
          + fdDdistDpPrmDir12Cos(body, system, iaBody[0], iaBody[1], iIndexJ) \
          + fdDdistDpPrmDir17Cos(body, system, iaBody[0], iaBody[1], iIndexJ) \
          + fdDdistDpPrmDir18Cos(body, system, iaBody[0], iaBody[1], iIndexJ)\
          + fdDdistDpPrmDir19Cos(body, system, iaBody[0], iaBody[1], iIndexJ) \
          + fdDdistDpPrmDir110Cos(body, system, iaBody[0], iaBody[1], iIndexJ) \
          + fdDdistDpPrmDir111Cos(body, system, iaBody[0], iaBody[1], iIndexJ)\
          + fdDdistDpPrmDir112Cos(body, system, iaBody[0], iaBody[1], iIndexJ) \
          + fdDdistDpPrmDir113Cos(body, system, iaBody[0], iaBody[1], iIndexJ) \
          + fdDdistDpPrmDir114Cos(body, system, iaBody[0], iaBody[1], iIndexJ) );
  } else if (iOrder == 2) { 
    y = ( fdDdistDpPrmDir25Cos(body, system, iaBody[0], iaBody[1], iIndexJ) \
      + fdDdistDpPrmDir26Cos(body, system, iaBody[0], iaBody[1], iIndexJ) );
  } else if (iOrder == 3) {
    y = ( fdDdistDpPrmDir37Cos(body, system, iaBody[0], iaBody[1], iIndexJ) \
      + fdDdistDpPrmDir38Cos(body, system, iaBody[0], iaBody[1], iIndexJ) \
      + fdDdistDpPrmDir39Cos(body, system, iaBody[0], iaBody[1], iIndexJ) \
      + fdDdistDpPrmDir310Cos(body, system, iaBody[0], iaBody[1], iIndexJ) );
  }
  return y;
}

double fdDdistresDPincPrimeDirSin(BODY *body, SYSTEM *system, int *iaBody, int iIndexJ, int iOrder) {
  double y;
  
  if (iOrder == 1) {
    y = (fdDdistDpPrmDir11Sin(body, system, iaBody[0], iaBody[1], iIndexJ)\
          + fdDdistDpPrmDir12Sin(body, system, iaBody[0], iaBody[1], iIndexJ) \
          + fdDdistDpPrmDir17Sin(body, system, iaBody[0], iaBody[1], iIndexJ) \
          + fdDdistDpPrmDir18Sin(body, system, iaBody[0], iaBody[1], iIndexJ)\
          + fdDdistDpPrmDir19Sin(body, system, iaBody[0], iaBody[1], iIndexJ) \
          + fdDdistDpPrmDir110Sin(body, system, iaBody[0], iaBody[1], iIndexJ) \
          + fdDdistDpPrmDir111Sin(body, system, iaBody[0], iaBody[1], iIndexJ)\
          + fdDdistDpPrmDir112Sin(body, system, iaBody[0], iaBody[1], iIndexJ) \
          + fdDdistDpPrmDir113Sin(body, system, iaBody[0], iaBody[1], iIndexJ) \
          + fdDdistDpPrmDir114Sin(body, system, iaBody[0], iaBody[1], iIndexJ) );
  } else if (iOrder == 2) { 
    y = ( fdDdistDpPrmDir25Sin(body, system, iaBody[0], iaBody[1], iIndexJ) \
      + fdDdistDpPrmDir26Sin(body, system, iaBody[0], iaBody[1], iIndexJ) );
  } else if (iOrder == 3) {
    y = ( fdDdistDpPrmDir37Sin(body, system, iaBody[0], iaBody[1], iIndexJ) \
      + fdDdistDpPrmDir38Sin(body, system, iaBody[0], iaBody[1], iIndexJ) \
      + fdDdistDpPrmDir39Sin(body, system, iaBody[0], iaBody[1], iIndexJ) \
      + fdDdistDpPrmDir310Sin(body, system, iaBody[0], iaBody[1], iIndexJ) );
  }
  return y;
}

double fdDdistresDPincPrime(BODY *body, SYSTEM *system, int *iaBody, int iIndexJ, int iOrder) {
  double y, yc, ys, dMfac, dSemiPrm;

  dMfac = KGAUSS*KGAUSS*body[iaBody[1]].dMass/MSUN;
  dSemiPrm = body[iaBody[0]].dSemi/AUCM;
  
  yc = fdDdistresDPincPrimeDirCos(body,system,iaBody,iIndexJ,iOrder);
  ys = fdDdistresDPincPrimeDirSin(body,system,iaBody,iIndexJ,iOrder);
  
  if (iOrder == 1) {
    if (iIndexJ == 2) {
      yc += (fdDdistDpPrmInt13Cos(body, system, iaBody[0], iaBody[1], iIndexJ)\
            +fdDdistDpPrmInt111Cos(body, system, iaBody[0], iaBody[1], iIndexJ) )\
            /(body[iaBody[1]].dSemi*body[iaBody[1]].dSemi/AUCM/AUCM);
      ys += (fdDdistDpPrmInt13Sin(body, system, iaBody[0], iaBody[1], iIndexJ)\
            +fdDdistDpPrmInt111Sin(body, system, iaBody[0], iaBody[1], iIndexJ) )\
            /(body[iaBody[1]].dSemi*body[iaBody[1]].dSemi/AUCM/AUCM);
    }
  }
  y = (yc*cos(fdLambdaArg(body,system,iaBody[0],iaBody[1],iIndexJ,iOrder)) \
       + ys*sin(fdLambdaArg(body,system,iaBody[0],iaBody[1],iIndexJ,iOrder))) * dMfac/dSemiPrm;
  return y;
}

//---------dR/dq------------------------------------------------------------------

double fdDdistDqDir11Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f30, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][29];
  f30 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][29] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][29]*dAlpha;
  
  return 2*body[iBody].dQinc*f30*(body[iBody].dKecc);
}

double fdDdistDqDir11Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f30, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][29];
  f30 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][29] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][29]*dAlpha;
  
  return 2*body[iBody].dQinc*f30*(body[iBody].dHecc);
}

double fdDdistDqDir12Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f34, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][33];
  f34 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][33] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][33]*dAlpha;
  
  return 2*body[iBody].dQinc*f34*(body[jBody].dKecc);
}

double fdDdistDqDir12Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f34, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][33];
  f34 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][33] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][33]*dAlpha;
  
  return 2*body[iBody].dQinc*f34*(body[jBody].dHecc);
}

double fdDdistDqDir15Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f37, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][36];
  f37 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][36] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][36]*dAlpha;  

  return f37*((2*body[iBody].dQinc*body[iBody].dKecc+2*body[iBody].dPinc*body[iBody].dHecc));     
}

double fdDdistDqDir15Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f37, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][36];
  f37 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][36] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][36]*dAlpha;  

  return f37*(-(2*body[iBody].dQinc*body[iBody].dHecc-2*body[iBody].dPinc*body[iBody].dKecc));     
}

double fdDdistDqDir16Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f38, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][37];
  f38 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][37] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][37]*dAlpha;  

  return f38*((2*body[iBody].dQinc*body[jBody].dKecc+2*body[iBody].dPinc*body[jBody].dHecc));     
}

double fdDdistDqDir16Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f38, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][37];
  f38 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][37] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][37]*dAlpha;  

  return f38*(-(2*body[iBody].dQinc*body[jBody].dHecc-2*body[iBody].dPinc*body[jBody].dKecc));     
}

double fdDdistDqDir17Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f39, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][38];
  f39 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][38] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][38]*dAlpha;  

  return f39*((body[jBody].dQinc*body[iBody].dKecc-body[jBody].dPinc*body[iBody].dHecc));     
}

double fdDdistDqDir17Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f39, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][38];
  f39 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][38] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][38]*dAlpha;  

  return f39*((body[jBody].dQinc*body[iBody].dHecc+body[jBody].dPinc*body[iBody].dKecc));     
}

double fdDdistDqDir18Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f40, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][39];
  f40 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][39] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][39]*dAlpha;  

  return f40*((body[jBody].dQinc*body[iBody].dKecc+body[jBody].dPinc*body[iBody].dHecc));     
}

double fdDdistDqDir18Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f40, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][39];
  f40 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][39] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][39]*dAlpha;  

  return f40*((body[jBody].dQinc*body[iBody].dHecc-body[jBody].dPinc*body[iBody].dKecc));     
}

double fdDdistDqDir19Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f41, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][40];
  f41 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][40] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][40]*dAlpha;  

  return f41*((body[jBody].dQinc*body[iBody].dKecc+body[jBody].dPinc*body[iBody].dHecc));     
}

double fdDdistDqDir19Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f41, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][40];
  f41 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][40] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][40]*dAlpha;  

  return f41*((body[jBody].dPinc*body[iBody].dKecc-body[jBody].dQinc*body[iBody].dHecc));     
}

double fdDdistDqDir110Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f42, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][41];
  f42 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][41] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][41]*dAlpha;  

  return f42*((body[jBody].dQinc*body[jBody].dKecc-body[jBody].dPinc*body[jBody].dHecc));     
}

double fdDdistDqDir110Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f42, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][41];
  f42 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][41] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][41]*dAlpha;  

  return f42*((body[jBody].dQinc*body[jBody].dHecc+body[jBody].dPinc*body[jBody].dKecc));     
}

double fdDdistDqDir111Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f43, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][42];
  f43 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][42] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][42]*dAlpha;  

  return f43*((body[jBody].dQinc*body[jBody].dKecc+body[jBody].dPinc*body[jBody].dHecc));     
}

double fdDdistDqDir111Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f43, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][42];
  f43 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][42] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][42]*dAlpha;  

  return f43*((body[jBody].dQinc*body[jBody].dHecc-body[jBody].dPinc*body[jBody].dKecc));     
}

double fdDdistDqDir112Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f44, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][43];
  f44 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][43] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][43]*dAlpha;  

  return f44*((body[jBody].dQinc*body[jBody].dKecc+body[jBody].dPinc*body[jBody].dHecc));     
}

double fdDdistDqDir112Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f44, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][43];
  f44 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][43] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][43]*dAlpha;  

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
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][56];
  f57 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][56] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][56]*dAlpha;

  return f57*(2.*body[iBody].dQinc);
}

double fdDdistDqDir24Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f57, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][56];
  f57 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][56] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][56]*dAlpha;

  return f57*(2*body[iBody].dPinc);
}

double fdDdistDqDir25Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f62, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][61];
  f62 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][61] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][61]*dAlpha;

  return f62*(body[jBody].dQinc);
}

double fdDdistDqDir25Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f62, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][61];
  f62 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][61] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][61]*dAlpha;

  return f62*(body[jBody].dPinc);
}

double fdDdistDqDir35Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f86, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][85];
  f86 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][85] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][85]*dAlpha;
  
  return f86*((2*body[iBody].dKecc*body[iBody].dQinc-2*body[iBody].dHecc*body[iBody].dPinc));
}

double fdDdistDqDir35Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f86, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][85];
  f86 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][85] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][85]*dAlpha;
  
  return f86*((2*body[iBody].dHecc*body[iBody].dQinc+2*body[iBody].dKecc*body[iBody].dPinc));
}

double fdDdistDqDir36Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f87, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][86];
  f87 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][86] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][86]*dAlpha;
  
  return f87*((2*body[jBody].dKecc*body[iBody].dQinc-2*body[jBody].dHecc*body[iBody].dPinc));
}

double fdDdistDqDir36Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f87, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][86];
  f87 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][86] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][86]*dAlpha;
  
  return f87*((2*body[jBody].dHecc*body[iBody].dQinc+2*body[jBody].dKecc*body[iBody].dPinc));
}

double fdDdistDqDir37Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f88, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][87];
  f88 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][87] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][87]*dAlpha;
  
  return f88*( (body[jBody].dQinc*body[iBody].dKecc-body[jBody].dPinc*body[iBody].dHecc));
}

double fdDdistDqDir37Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f88, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][87];
  f88 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][87] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][87]*dAlpha;
  
  return f88*((body[jBody].dQinc*body[iBody].dHecc+body[jBody].dPinc*body[iBody].dKecc));
}

double fdDdistDqDir38Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f89, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][88];
  f89 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][88] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][88]*dAlpha;
  
  return f89*( (body[jBody].dQinc*body[jBody].dKecc-body[jBody].dPinc*body[jBody].dHecc));
}

double fdDdistDqDir38Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f89, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][88];
  f89 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][88] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][88]*dAlpha;
  
  return f89*( (body[jBody].dQinc*body[jBody].dHecc+body[jBody].dPinc*body[jBody].dKecc));
}

double fdDdistresDQincDirCos(BODY *body, SYSTEM *system, int *iaBody, int iIndexJ, int iOrder) {
  double y;
  
  if (iOrder == 1) {
    y =  (fdDdistDqDir11Cos(body, system, iaBody[0], iaBody[1], iIndexJ) \
    + fdDdistDqDir12Cos(body, system, iaBody[0], iaBody[1], iIndexJ)  \
    + fdDdistDqDir15Cos(body, system, iaBody[0], iaBody[1], iIndexJ) \
    + fdDdistDqDir16Cos(body, system, iaBody[0], iaBody[1], iIndexJ) \
    + fdDdistDqDir17Cos(body, system, iaBody[0], iaBody[1], iIndexJ) \
    + fdDdistDqDir18Cos(body, system, iaBody[0], iaBody[1], iIndexJ) \
    + fdDdistDqDir19Cos(body, system, iaBody[0], iaBody[1], iIndexJ) \
    + fdDdistDqDir110Cos(body, system, iaBody[0], iaBody[1], iIndexJ) \
    + fdDdistDqDir111Cos(body, system, iaBody[0], iaBody[1], iIndexJ) \
    + fdDdistDqDir112Cos(body, system, iaBody[0], iaBody[1], iIndexJ) );
  } else if (iOrder==2) {
    y = ( fdDdistDqDir24Cos(body, system, iaBody[0], iaBody[1], iIndexJ) \
      + fdDdistDqDir25Cos(body, system, iaBody[0], iaBody[1], iIndexJ) );
  } else if (iOrder == 3) {
    y = ( fdDdistDqDir35Cos(body, system, iaBody[0], iaBody[1], iIndexJ) \
      + fdDdistDqDir36Cos(body, system, iaBody[0], iaBody[1], iIndexJ) \
      + fdDdistDqDir37Cos(body, system, iaBody[0], iaBody[1], iIndexJ) \
      + fdDdistDqDir38Cos(body, system, iaBody[0], iaBody[1], iIndexJ) );
  }
  return y;
}

double fdDdistresDQincDirSin(BODY *body, SYSTEM *system, int *iaBody, int iIndexJ, int iOrder) {
  double y;
  
  if (iOrder == 1) {
    y =  (fdDdistDqDir11Sin(body, system, iaBody[0], iaBody[1], iIndexJ) \
    + fdDdistDqDir12Sin(body, system, iaBody[0], iaBody[1], iIndexJ)  \
    + fdDdistDqDir15Sin(body, system, iaBody[0], iaBody[1], iIndexJ) \
    + fdDdistDqDir16Sin(body, system, iaBody[0], iaBody[1], iIndexJ) \
    + fdDdistDqDir17Sin(body, system, iaBody[0], iaBody[1], iIndexJ) \
    + fdDdistDqDir18Sin(body, system, iaBody[0], iaBody[1], iIndexJ) \
    + fdDdistDqDir19Sin(body, system, iaBody[0], iaBody[1], iIndexJ) \
    + fdDdistDqDir110Sin(body, system, iaBody[0], iaBody[1], iIndexJ) \
    + fdDdistDqDir111Sin(body, system, iaBody[0], iaBody[1], iIndexJ) \
    + fdDdistDqDir112Sin(body, system, iaBody[0], iaBody[1], iIndexJ) );
  } else if (iOrder==2) {
    y = ( fdDdistDqDir24Sin(body, system, iaBody[0], iaBody[1], iIndexJ) \
      + fdDdistDqDir25Sin(body, system, iaBody[0], iaBody[1], iIndexJ) );
  } else if (iOrder == 3) {
    y = ( fdDdistDqDir35Sin(body, system, iaBody[0], iaBody[1], iIndexJ) \
      + fdDdistDqDir36Sin(body, system, iaBody[0], iaBody[1], iIndexJ) \
      + fdDdistDqDir37Sin(body, system, iaBody[0], iaBody[1], iIndexJ) \
      + fdDdistDqDir38Sin(body, system, iaBody[0], iaBody[1], iIndexJ) );
  }
  return y;
}

double fdDdistresDQinc(BODY *body, SYSTEM *system, int *iaBody, int iIndexJ, int iOrder) {
  double y, yc, ys, dMfac, dSemiPrm;
  dMfac = KGAUSS*KGAUSS*body[iaBody[1]].dMass/MSUN;
  dSemiPrm = body[iaBody[1]].dSemi/AUCM;

  yc = fdDdistresDQincDirCos(body,system,iaBody,iIndexJ,iOrder);
  ys = fdDdistresDQincDirSin(body,system,iaBody,iIndexJ,iOrder); 
  
  if (iOrder == 1) {
    if (iIndexJ == 2) {
      yc += (fdDdistDqExt13Cos(body, system, iaBody[0], iaBody[1], iIndexJ) \
          + fdDdistDqExt111Cos(body, system, iaBody[0], iaBody[1], iIndexJ))\
            *body[iaBody[0]].dSemi/body[iaBody[1]].dSemi;
      ys += (fdDdistDqExt13Sin(body, system, iaBody[0], iaBody[1], iIndexJ) \
          + fdDdistDqExt111Sin(body, system, iaBody[0], iaBody[1], iIndexJ))\
            *body[iaBody[0]].dSemi/body[iaBody[1]].dSemi;
    }
  } 
  
  y = (yc*cos(fdLambdaArg(body,system,iaBody[0],iaBody[1],iIndexJ,iOrder)) \
       + ys*sin(fdLambdaArg(body,system,iaBody[0],iaBody[1],iIndexJ,iOrder))) * dMfac/dSemiPrm;
  return y;
}

//---------dR/dq'------------------------------------------------------------------

double fdDdistDqPrmDir11Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f30, dAlpha;
  dAlpha = body[jBody].dSemi/body[iBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][29];
  f30 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][29] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][29]*dAlpha;
  
  return 2*body[iBody].dQinc*f30*(body[jBody].dKecc);
}

double fdDdistDqPrmDir11Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f30, dAlpha;
  dAlpha = body[jBody].dSemi/body[iBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][29];
  f30 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][29] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][29]*dAlpha;
  
  return 2*body[iBody].dQinc*f30*(body[jBody].dHecc);
}

double fdDdistDqPrmDir12Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f34, dAlpha;
  dAlpha = body[jBody].dSemi/body[iBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][33];
  f34 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][33] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][33]*dAlpha;
  
  return 2*body[iBody].dQinc*f34*(body[iBody].dKecc);
}

double fdDdistDqPrmDir122Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f34, dAlpha;
  dAlpha = body[jBody].dSemi/body[iBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][33];
  f34 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][33] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][33]*dAlpha;
  
  return 2*body[iBody].dQinc*f34*(body[iBody].dHecc);
}

double fdDdistDqPrmDir17Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f39, dAlpha;
  dAlpha = body[jBody].dSemi/body[iBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][38];
  f39 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][38] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][38]*dAlpha;  

  return f39*((body[jBody].dQinc*body[jBody].dKecc+body[jBody].dPinc*body[jBody].dHecc));     
}

double fdDdistDqPrmDir17Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f39, dAlpha;
  dAlpha = body[jBody].dSemi/body[iBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][38];
  f39 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][38] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][38]*dAlpha;  

  return f39*(body[jBody].dQinc*body[jBody].dHecc-body[jBody].dPinc*body[jBody].dKecc);     
}

double fdDdistDqPrmDir18Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f40, dAlpha;
  dAlpha = body[jBody].dSemi/body[iBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][39];
  f40 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][39] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][39]*dAlpha;  

  return f40*((body[jBody].dQinc*body[jBody].dKecc-body[jBody].dPinc*body[jBody].dHecc));     
}

double fdDdistDqPrmDir18Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f40, dAlpha;
  dAlpha = body[jBody].dSemi/body[iBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][39];
  f40 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][39] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][39]*dAlpha;  

  return f40*((body[jBody].dQinc*body[jBody].dHecc+body[jBody].dPinc*body[jBody].dKecc));     
}

double fdDdistDqPrmDir19Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f41, dAlpha;
  dAlpha = body[jBody].dSemi/body[iBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][40];
  f41 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][40] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][40]*dAlpha;  

  return f41*((body[jBody].dQinc*body[jBody].dKecc+body[jBody].dPinc*body[jBody].dHecc));     
}

double fdDdistDqPrmDir19Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f41, dAlpha;
  dAlpha = body[jBody].dSemi/body[iBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][40];
  f41 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][40] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][40]*dAlpha;  

  return f41*((body[jBody].dPinc*body[jBody].dKecc-body[jBody].dQinc*body[jBody].dHecc));     
}

double fdDdistDqPrmDir110Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f42, dAlpha;
  dAlpha = body[jBody].dSemi/body[iBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][41];
  f42 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][41] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][41]*dAlpha;  

  return f42*((body[jBody].dQinc*body[iBody].dKecc+body[jBody].dPinc*body[iBody].dHecc));     
}

double fdDdistDqPrmDir110Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f42, dAlpha;
  dAlpha = body[jBody].dSemi/body[iBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][41];
  f42 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][41] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][41]*dAlpha;  

  return f42*((body[jBody].dQinc*body[iBody].dHecc-body[jBody].dPinc*body[iBody].dKecc));     
}

double fdDdistDqPrmDir111Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f43, dAlpha;
  dAlpha = body[jBody].dSemi/body[iBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][42];
  f43 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][42] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][42]*dAlpha;  

  return f43*((body[jBody].dQinc*body[iBody].dKecc-body[jBody].dPinc*body[iBody].dHecc));     
}

double fdDdistDqPrmDir111Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f43, dAlpha;
  dAlpha = body[jBody].dSemi/body[iBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][42];
  f43 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][42] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][42]*dAlpha;  

  return f43*((body[jBody].dQinc*body[iBody].dHecc+body[jBody].dPinc*body[iBody].dKecc));     
}

double fdDdistDqPrmDir112Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f44, dAlpha;
  dAlpha = body[jBody].dSemi/body[iBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][43];
  f44 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][43] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][43]*dAlpha;  

  return f44*((body[jBody].dQinc*body[iBody].dKecc+body[jBody].dPinc*body[iBody].dHecc));     
}

double fdDdistDqPrmDir112Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f44, dAlpha;
  dAlpha = body[jBody].dSemi/body[iBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][43];
  f44 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][43] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][43]*dAlpha;  

  return f44*((body[jBody].dPinc*body[iBody].dKecc-body[jBody].dQinc*body[iBody].dHecc));     
}

double fdDdistDqPrmDir113Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f37, dAlpha;
  dAlpha = body[jBody].dSemi/body[iBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][36];
  f37 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][36] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][36]*dAlpha;  

  return f37*((2*body[iBody].dQinc*body[jBody].dKecc+2*body[iBody].dPinc*body[jBody].dHecc));     
}

double fdDdistDqPrmDir113Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f37, dAlpha;
  dAlpha = body[jBody].dSemi/body[iBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][36];
  f37 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][36] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][36]*dAlpha;  

  return f37*(-(2*body[iBody].dQinc*body[jBody].dHecc-2*body[iBody].dPinc*body[jBody].dKecc));     
}

double fdDdistDqPrmDir114Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f38, dAlpha;
  dAlpha = body[jBody].dSemi/body[iBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][37];
  f38 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][37] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][37]*dAlpha;  

  return f38*((2*body[iBody].dQinc*body[iBody].dKecc+2*body[iBody].dPinc*body[iBody].dHecc));     
}

double fdDdistDqPrmDir114Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f38, dAlpha;
  dAlpha = body[jBody].dSemi/body[iBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][37];
  f38 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][37] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][37]*dAlpha;  

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
  dAlpha = body[jBody].dSemi/body[iBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][61];
  f62 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][61] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][61]*dAlpha;

  return f62*(body[jBody].dQinc);
}

double fdDdistDqPrmDir25Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f62, dAlpha;
  dAlpha = body[jBody].dSemi/body[iBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][61];
  f62 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][61] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][61]*dAlpha;

  return f62*(body[jBody].dPinc);
}

double fdDdistDqPrmDir26Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f57, dAlpha;
  dAlpha = body[jBody].dSemi/body[iBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][56];
  f57 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][56] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][56]*dAlpha;

  return f57*(2.*body[iBody].dQinc);
}

double fdDdistDqPrmDir26Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f57, dAlpha;
  dAlpha = body[jBody].dSemi/body[iBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][56];
  f57 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][56] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][56]*dAlpha;

  return f57*(2.*body[iBody].dPinc);
}

double fdDdistDqPrmDir37Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f88, dAlpha;
  dAlpha = body[jBody].dSemi/body[iBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][87];
  f88 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][87] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][87]*dAlpha;
  
  return f88*( (body[jBody].dQinc*body[jBody].dKecc-body[jBody].dPinc*body[jBody].dHecc));
}

double fdDdistDqPrmDir37Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f88, dAlpha;
  dAlpha = body[jBody].dSemi/body[iBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][87];
  f88 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][87] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][87]*dAlpha;
  
  return f88*((body[jBody].dQinc*body[jBody].dHecc+body[jBody].dPinc*body[jBody].dKecc));
}

double fdDdistDqPrmDir38Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f89, dAlpha;
  dAlpha = body[jBody].dSemi/body[iBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][88];
  f89 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][88] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][88]*dAlpha;
  
  return f89*( (body[jBody].dQinc*body[iBody].dKecc-body[jBody].dPinc*body[iBody].dHecc));
}

double fdDdistDqPrmDir38Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f89, dAlpha;
  dAlpha = body[jBody].dSemi/body[iBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][88];
  f89 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][88] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][88]*dAlpha;
  
  return f89*( (body[jBody].dQinc*body[iBody].dHecc+body[jBody].dPinc*body[iBody].dKecc));
}

double fdDdistDqPrmDir39Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f86, dAlpha;
  dAlpha = body[jBody].dSemi/body[iBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][85];
  f86 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][85] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][85]*dAlpha;
  
  return f86*( (2*body[iBody].dQinc*body[jBody].dKecc-2*body[jBody].dHecc*body[iBody].dPinc));
}

double fdDdistDqPrmDir39Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f86, dAlpha;
  dAlpha = body[jBody].dSemi/body[iBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][85];
  f86 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][85] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][85]*dAlpha;
  
  return f86*((2*body[iBody].dQinc*body[jBody].dHecc+2*body[jBody].dKecc*body[iBody].dPinc));
}

double fdDdistDqPrmDir310Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f87, dAlpha;
  dAlpha = body[jBody].dSemi/body[iBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][86];
  f87 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][86] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][86]*dAlpha;
  
  return f87*( (2*body[iBody].dQinc*body[iBody].dKecc-2*body[iBody].dHecc*body[iBody].dPinc));
}

double fdDdistDqPrmDir310Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f87, dAlpha;
  dAlpha = body[jBody].dSemi/body[iBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][86];
  f87 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][86] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][86]*dAlpha;
  
  return f87*((2*body[iBody].dQinc*body[iBody].dHecc+2*body[iBody].dKecc*body[iBody].dPinc));
}

double fdDdistresDQincPrimeDirCos(BODY *body, SYSTEM *system, int *iaBody, int iIndexJ, int iOrder) {
  double y;
  
  if (iOrder == 1) {
    y = (fdDdistDqPrmDir11Cos(body, system, iaBody[0], iaBody[1], iIndexJ)\
          + fdDdistDqPrmDir12Cos(body, system, iaBody[0], iaBody[1], iIndexJ) \
          + fdDdistDqPrmDir17Cos(body, system, iaBody[0], iaBody[1], iIndexJ) \
          + fdDdistDqPrmDir18Cos(body, system, iaBody[0], iaBody[1], iIndexJ)\
          + fdDdistDqPrmDir19Cos(body, system, iaBody[0], iaBody[1], iIndexJ) \
          + fdDdistDqPrmDir110Cos(body, system, iaBody[0], iaBody[1], iIndexJ) \
          + fdDdistDqPrmDir111Cos(body, system, iaBody[0], iaBody[1], iIndexJ)\
          + fdDdistDqPrmDir112Cos(body, system, iaBody[0], iaBody[1], iIndexJ) \
          + fdDdistDqPrmDir113Cos(body, system, iaBody[0], iaBody[1], iIndexJ) \
          + fdDdistDqPrmDir114Cos(body, system, iaBody[0], iaBody[1], iIndexJ) );
  } else if (iOrder == 2) {
    y = ( fdDdistDqPrmDir25Cos(body, system, iaBody[0], iaBody[1], iIndexJ) \
      + fdDdistDqPrmDir26Cos(body, system, iaBody[0], iaBody[1], iIndexJ) );
  } else if (iOrder == 3) {
    y = ( fdDdistDqPrmDir37Cos(body, system, iaBody[0], iaBody[1], iIndexJ) \
    + fdDdistDqPrmDir38Cos(body, system, iaBody[0], iaBody[1], iIndexJ) \
    + fdDdistDqPrmDir39Cos(body, system, iaBody[0], iaBody[1], iIndexJ) \
    + fdDdistDqPrmDir310Cos(body, system, iaBody[0], iaBody[1], iIndexJ) );
  }
  return y;
}

double fdDdistresDQincPrimeDirSin(BODY *body, SYSTEM *system, int *iaBody, int iIndexJ, int iOrder) {
  double y;
  
  if (iOrder == 1) {
    y = (fdDdistDqPrmDir11Sin(body, system, iaBody[0], iaBody[1], iIndexJ)\
          + fdDdistDqPrmDir12Sin(body, system, iaBody[0], iaBody[1], iIndexJ) \
          + fdDdistDqPrmDir17Sin(body, system, iaBody[0], iaBody[1], iIndexJ) \
          + fdDdistDqPrmDir18Sin(body, system, iaBody[0], iaBody[1], iIndexJ)\
          + fdDdistDqPrmDir19Sin(body, system, iaBody[0], iaBody[1], iIndexJ) \
          + fdDdistDqPrmDir110Sin(body, system, iaBody[0], iaBody[1], iIndexJ) \
          + fdDdistDqPrmDir111Sin(body, system, iaBody[0], iaBody[1], iIndexJ)\
          + fdDdistDqPrmDir112Sin(body, system, iaBody[0], iaBody[1], iIndexJ) \
          + fdDdistDqPrmDir113Sin(body, system, iaBody[0], iaBody[1], iIndexJ) \
          + fdDdistDqPrmDir114Sin(body, system, iaBody[0], iaBody[1], iIndexJ) );
  } else if (iOrder == 2) {
    y = ( fdDdistDqPrmDir25Sin(body, system, iaBody[0], iaBody[1], iIndexJ) \
      + fdDdistDqPrmDir26Sin(body, system, iaBody[0], iaBody[1], iIndexJ) );
  } else if (iOrder == 3) {
    y = ( fdDdistDqPrmDir37Sin(body, system, iaBody[0], iaBody[1], iIndexJ) \
    + fdDdistDqPrmDir38Sin(body, system, iaBody[0], iaBody[1], iIndexJ) \
    + fdDdistDqPrmDir39Sin(body, system, iaBody[0], iaBody[1], iIndexJ) \
    + fdDdistDqPrmDir310Sin(body, system, iaBody[0], iaBody[1], iIndexJ) );
  }
  return y;
}

double fdDdistresDQincPrime(BODY *body, SYSTEM *system, int *iaBody, int iIndexJ, int iOrder) {
  double y, yc, ys, dMfac, dSemiPrm;
  dMfac = KGAUSS*KGAUSS*body[iaBody[1]].dMass/MSUN;
  dSemiPrm = body[iaBody[0]].dSemi/AUCM;
  
  yc = fdDdistresDQincPrimeDirCos(body,system,iaBody,iIndexJ,iOrder);
  ys = fdDdistresDQincPrimeDirSin(body,system,iaBody,iIndexJ,iOrder);
  
  if (iOrder == 1) {
    if (iIndexJ == 2) {
      yc += (fdDdistDqPrmInt13Cos(body, system, iaBody[0], iaBody[1], iIndexJ)\
            +fdDdistDqPrmInt111Cos(body, system, iaBody[0], iaBody[1], iIndexJ) ) \
            /(body[iaBody[1]].dSemi*body[iaBody[1]].dSemi/AUCM/AUCM);
      ys += (fdDdistDqPrmInt13Sin(body, system, iaBody[0], iaBody[1], iIndexJ)\
            +fdDdistDqPrmInt111Sin(body, system, iaBody[0], iaBody[1], iIndexJ) ) \
            /(body[iaBody[1]].dSemi*body[iaBody[1]].dSemi/AUCM/AUCM);
    }
  }
  
  y = (yc*cos(fdLambdaArg(body,system,iaBody[0],iaBody[1],iIndexJ,iOrder)) \
       + ys*sin(fdLambdaArg(body,system,iaBody[0],iaBody[1],iIndexJ,iOrder))) * dMfac/dSemiPrm;
  return y;
}

//---------dR/dlambda------------------------------------------------------------------

double fdDdistDlDir11Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f27, f28, f29, f30, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][26];
  f27 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][26] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][26]*dAlpha;
        
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][27];
  f28 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][27] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][27]*dAlpha;      
        
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][28];
  f29 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][28] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][28]*dAlpha;  

  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][29];
  f30 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][29] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][29]*dAlpha;
  
  return (1.-iIndexJ)*(f27+(body[iBody].dHecc*body[iBody].dHecc+\
        body[iBody].dKecc*body[iBody].dKecc)*f28+(body[jBody].dHecc*body[jBody].dHecc+\
        body[jBody].dKecc*body[jBody].dKecc)*f29+(body[iBody].dPinc*body[iBody].dPinc+\
        body[iBody].dQinc*body[iBody].dQinc+body[jBody].dPinc*body[jBody].dPinc+\
        body[jBody].dQinc*body[jBody].dQinc)*f30)*(body[iBody].dHecc);
}

double fdDdistDlDir11Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f27, f28, f29, f30, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][26];
  f27 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][26] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][26]*dAlpha;
        
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][27];
  f28 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][27] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][27]*dAlpha;      
        
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][28];
  f29 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][28] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][28]*dAlpha;  

  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][29];
  f30 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][29] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][29]*dAlpha;
  
  return (1.-iIndexJ)*(f27+(body[iBody].dHecc*body[iBody].dHecc+\
        body[iBody].dKecc*body[iBody].dKecc)*f28+(body[jBody].dHecc*body[jBody].dHecc+\
        body[jBody].dKecc*body[jBody].dKecc)*f29+(body[iBody].dPinc*body[iBody].dPinc+\
        body[iBody].dQinc*body[iBody].dQinc+body[jBody].dPinc*body[jBody].dPinc+\
        body[jBody].dQinc*body[jBody].dQinc)*f30)*(-body[iBody].dKecc);
}

double fdDdistDlDir12Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f31, f32, f33, f34, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][30];
  f31 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][30] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][30]*dAlpha;  

  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][31];
  f32 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][31] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][31]*dAlpha;      
        
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][32];
  f33 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][32] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][32]*dAlpha;  

  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][33];
  f34 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][33] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][33]*dAlpha;

  return (1.-iIndexJ)*(f31+(body[iBody].dHecc*body[iBody].dHecc+\
        body[iBody].dKecc*body[iBody].dKecc)*f32+(body[jBody].dHecc*body[jBody].dHecc+\
        body[jBody].dKecc*body[jBody].dKecc)*f33+(body[iBody].dPinc*body[iBody].dPinc+\
        body[iBody].dQinc*body[iBody].dQinc+body[jBody].dPinc*body[jBody].dPinc+\
        body[jBody].dQinc*body[jBody].dQinc)*f34)*(body[jBody].dHecc);
}

double fdDdistDlDir12Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f31, f32, f33, f34, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][30];
  f31 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][30] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][30]*dAlpha;  

  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][31];
  f32 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][31] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][31]*dAlpha;      
        
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][32];
  f33 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][32] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][32]*dAlpha;  

  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][33];
  f34 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][33] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][33]*dAlpha;

  return (1.-iIndexJ)*(f31+(body[iBody].dHecc*body[iBody].dHecc+\
        body[iBody].dKecc*body[iBody].dKecc)*f32+(body[jBody].dHecc*body[jBody].dHecc+\
        body[jBody].dKecc*body[jBody].dKecc)*f33+(body[iBody].dPinc*body[iBody].dPinc+\
        body[iBody].dQinc*body[iBody].dQinc+body[jBody].dPinc*body[jBody].dPinc+\
        body[jBody].dQinc*body[jBody].dQinc)*f34)*(-body[jBody].dKecc);
}

double fdDdistDlDir13Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f35, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][34];
  f35 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][34] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][34]*dAlpha;
  
  return (1-iIndexJ)*f35*(-(body[jBody].dHecc*\
    (body[iBody].dKecc*body[iBody].dKecc-body[iBody].dHecc*body[iBody].dHecc)\
    -2*body[jBody].dKecc*body[iBody].dHecc*body[iBody].dKecc));
}

double fdDdistDlDir13Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f35, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][34];
  f35 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][34] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][34]*dAlpha;
  
  return (1-iIndexJ)*f35*\
    (-(body[jBody].dKecc*(body[iBody].dKecc*body[iBody].dKecc-body[iBody].dHecc*body[iBody].dHecc)\
    +2*body[jBody].dHecc*body[iBody].dHecc*body[iBody].dKecc));
}

double fdDdistDlDir14Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f36, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][35];
  f36 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][35] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][35]*dAlpha;
  
  return (1-iIndexJ)*f36*(-(body[iBody].dHecc*\
    (body[jBody].dKecc*body[jBody].dKecc-body[jBody].dHecc*body[jBody].dHecc)\
    -2*body[iBody].dKecc*body[jBody].dHecc*body[jBody].dKecc));
}

double fdDdistDlDir14Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f36, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][35];
  f36 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][35] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][35]*dAlpha;
  
  return (1-iIndexJ)*f36*\
    (-(body[iBody].dKecc*(body[jBody].dKecc*body[jBody].dKecc-body[jBody].dHecc*body[jBody].dHecc)\
    +2*body[iBody].dHecc*body[jBody].dHecc*body[jBody].dKecc));
}

double fdDdistDlDir15Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f37, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][36];
  f37 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][36] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][36]*dAlpha;
  
  return (1-iIndexJ)*f37*(-(body[iBody].dHecc*\
    (body[iBody].dQinc*body[iBody].dQinc-body[iBody].dPinc*body[iBody].dPinc)\
    -2*body[iBody].dKecc*body[iBody].dPinc*body[iBody].dQinc));
}

double fdDdistDlDir15Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f37, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][36];
  f37 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][36] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][36]*dAlpha;
  
  return (1-iIndexJ)*f37*\
    (-(body[iBody].dKecc*(body[iBody].dQinc*body[iBody].dQinc-body[iBody].dPinc*body[iBody].dPinc)\
    +2*body[iBody].dHecc*body[iBody].dPinc*body[iBody].dQinc));
}

double fdDdistDlDir16Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f38, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][37];
  f38 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][37] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][37]*dAlpha;  
  
  return (1-iIndexJ)*f38*(-(body[jBody].dHecc*\
    (body[iBody].dQinc*body[iBody].dQinc-body[iBody].dPinc*body[iBody].dPinc)\
    -2*body[jBody].dKecc*body[iBody].dPinc*body[iBody].dQinc));
}

double fdDdistDlDir16Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f38, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][37];
  f38 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][37] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][37]*dAlpha;  
  
  return (1-iIndexJ)*f38*\
    (-(body[jBody].dKecc*(body[iBody].dQinc*body[iBody].dQinc-body[iBody].dPinc*body[iBody].dPinc)\
    +2*body[jBody].dHecc*body[iBody].dPinc*body[iBody].dQinc));
}

double fdDdistDlDir17Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f39, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][38];
  f39 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][38] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][38]*dAlpha;  
  
  return (1-iIndexJ)*f39*((body[iBody].dHecc*\
    (body[iBody].dQinc*body[jBody].dQinc+body[iBody].dPinc*body[jBody].dPinc)\
    +body[iBody].dKecc*(body[iBody].dQinc*body[jBody].dPinc-body[iBody].dPinc*body[jBody].dQinc)));
}

double fdDdistDlDir17Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f39, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][38];
  f39 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][38] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][38]*dAlpha;  
  
  return (1-iIndexJ)*f39*\
    (-(body[iBody].dKecc*(body[iBody].dQinc*body[jBody].dQinc+body[iBody].dPinc*body[jBody].dPinc)\
    -body[iBody].dHecc*(body[iBody].dQinc*body[jBody].dPinc-body[iBody].dPinc*body[jBody].dQinc)));
}

double fdDdistDlDir18Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f40, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][39];
  f40 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][39] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][39]*dAlpha;  
  
  return (1-iIndexJ)*f40*((body[iBody].dHecc*\
    (body[iBody].dQinc*body[jBody].dQinc+body[iBody].dPinc*body[jBody].dPinc)\
    +body[iBody].dKecc*(body[jBody].dQinc*body[iBody].dPinc-body[jBody].dPinc*body[iBody].dQinc)));
}

double fdDdistDlDir18Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f40, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][39];
  f40 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][39] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][39]*dAlpha;  
  
  return (1-iIndexJ)*f40*\
    (-(body[iBody].dKecc*(body[iBody].dQinc*body[jBody].dQinc+body[iBody].dPinc*body[jBody].dPinc)\
    -body[iBody].dHecc*(body[jBody].dQinc*body[iBody].dPinc-body[jBody].dPinc*body[iBody].dQinc)));
}

double fdDdistDlDir19Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f41, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][40];
  f41 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][40] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][40]*dAlpha;  
  
  return (1-iIndexJ)*f41*((body[iBody].dPinc*\
    (body[jBody].dQinc*body[iBody].dKecc+body[jBody].dPinc*body[iBody].dHecc)\
    +body[iBody].dQinc*(body[jBody].dPinc*body[iBody].dKecc-body[jBody].dQinc*body[iBody].dHecc)));
}

double fdDdistDlDir19Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f41, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][40];
  f41 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][40] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][40]*dAlpha;  
  
  return (1-iIndexJ)*f41*\
    (-(body[iBody].dQinc*(body[jBody].dQinc*body[iBody].dKecc+body[jBody].dPinc*body[iBody].dHecc)\
    -body[iBody].dPinc*(body[jBody].dPinc*body[iBody].dKecc-body[jBody].dQinc*body[iBody].dHecc)));
}

double fdDdistDlDir110Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f42, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][41];
  f42 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][41] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][41]*dAlpha;  
  
  return (1-iIndexJ)*f42*((body[jBody].dHecc*\
    (body[iBody].dQinc*body[jBody].dQinc+body[iBody].dPinc*body[jBody].dPinc)\
    +body[jBody].dKecc*(body[iBody].dQinc*body[jBody].dPinc-body[iBody].dPinc*body[jBody].dQinc)));
}

double fdDdistDlDir110Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f42, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][41];
  f42 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][41] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][41]*dAlpha;  
  
  return (1-iIndexJ)*f42*\
    (-(body[jBody].dKecc*(body[iBody].dQinc*body[jBody].dQinc+body[iBody].dPinc*body[jBody].dPinc)\
    -body[jBody].dHecc*(body[iBody].dQinc*body[jBody].dPinc-body[iBody].dPinc*body[jBody].dQinc)));
}

double fdDdistDlDir111Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f43, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][42];
  f43 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][42] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][42]*dAlpha;  
  
  return (1-iIndexJ)*f43*((body[jBody].dHecc*\
    (body[iBody].dQinc*body[jBody].dQinc+body[iBody].dPinc*body[jBody].dPinc)\
    +body[jBody].dKecc*(body[jBody].dQinc*body[iBody].dPinc-body[jBody].dPinc*body[iBody].dQinc)));
}

double fdDdistDlDir111Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f43, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][42];
  f43 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][42] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][42]*dAlpha;  
  
  return (1-iIndexJ)*f43*\
    (-(body[jBody].dKecc*(body[iBody].dQinc*body[jBody].dQinc+body[iBody].dPinc*body[jBody].dPinc)\
    -body[jBody].dHecc*(body[jBody].dQinc*body[iBody].dPinc-body[jBody].dPinc*body[iBody].dQinc)));
}

double fdDdistDlDir112Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f44, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][43];
  f44 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][43] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][43]*dAlpha;  
  
  return (1-iIndexJ)*f44*((body[iBody].dPinc*\
    (body[jBody].dQinc*body[jBody].dKecc+body[jBody].dPinc*body[jBody].dHecc)\
    +body[iBody].dQinc*(body[jBody].dPinc*body[jBody].dKecc-body[jBody].dQinc*body[jBody].dHecc)));
}

double fdDdistDlDir112Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f44, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][43];
  f44 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][43] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][43]*dAlpha;  
  
  return (1-iIndexJ)*f44*\
    (-(body[iBody].dQinc*(body[jBody].dQinc*body[jBody].dKecc+body[jBody].dPinc*body[jBody].dHecc)\
    -body[iBody].dPinc*(body[jBody].dPinc*body[jBody].dKecc-body[jBody].dQinc*body[jBody].dHecc)));
}

double fdDdistDlDir113Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f37, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][36];
  f37 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][36] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][36]*dAlpha;
          
  return (1-iIndexJ)*f37*(-(body[iBody].dHecc*\
    (body[jBody].dQinc*body[jBody].dQinc-body[jBody].dPinc*body[jBody].dPinc)\
    -2*body[iBody].dKecc*body[jBody].dPinc*body[jBody].dQinc));
}

double fdDdistDlDir113Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f37, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][36];
  f37 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][36] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][36]*dAlpha;
          
  return (1-iIndexJ)*f37*\
    (-(body[iBody].dKecc*(body[jBody].dQinc*body[jBody].dQinc-body[jBody].dPinc*body[jBody].dPinc)\
    +2*body[iBody].dHecc*body[jBody].dPinc*body[jBody].dQinc));
}

double fdDdistDlDir114Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f38, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][37];
  f38 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][37] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][37]*dAlpha;  
  
  return (1-iIndexJ)*f38*(-(body[jBody].dHecc*\
    (body[jBody].dQinc*body[jBody].dQinc-body[jBody].dPinc*body[jBody].dPinc)\
    -2*body[jBody].dKecc*body[jBody].dPinc*body[jBody].dQinc));
}

double fdDdistDlDir114Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f38, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][37];
  f38 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][37] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][37]*dAlpha;  
  
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
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][44];
  f45 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][44] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][44]*dAlpha;  
  
  return (2.-iIndexJ)*f45*(2*(body[iBody].dHecc*body[iBody].dKecc));
}

double fdDdistDlDir21Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f45, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][44];
  f45 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][44] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][44]*dAlpha;  
  
  return (2.-iIndexJ)*f45*(-(body[iBody].dKecc*body[iBody].dKecc-body[iBody].dHecc*body[iBody].dHecc));
}

double fdDdistDlDir22Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f49, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][48];
  f49 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][48] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][48]*dAlpha;  
  
  return (2.-iIndexJ)*f49*((body[iBody].dHecc*body[jBody].dKecc+body[iBody].dKecc*body[jBody].dHecc));
}

double fdDdistDlDir22Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f49, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][48];
  f49 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][48] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][48]*dAlpha;  
  
  return (2.-iIndexJ)*f49*(-(body[iBody].dKecc*body[jBody].dKecc-body[iBody].dHecc*body[jBody].dHecc));
}

double fdDdistDlDir23Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f53, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][52];
  f53 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][52] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][52]*dAlpha;  
  
  return (2.-iIndexJ)*f53*(2*(body[jBody].dHecc*body[jBody].dKecc));
}

double fdDdistDlDir23Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f53, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][52];
  f53 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][52] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][52]*dAlpha;  
  
  return (2.-iIndexJ)*f53*(-(body[jBody].dKecc*body[jBody].dKecc-body[jBody].dHecc*body[jBody].dHecc));
}

double fdDdistDlDir24Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f57, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][56];
  f57 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][56] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][56]*dAlpha;  
  
  return (2.-iIndexJ)*f57*(2*(body[iBody].dPinc*body[iBody].dQinc));
}

double fdDdistDlDir24Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f57, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][56];
  f57 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][56] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][56]*dAlpha;  
  
  return (2.-iIndexJ)*f57*(-(body[iBody].dQinc*body[iBody].dQinc-body[iBody].dPinc*body[iBody].dPinc));
}

double fdDdistDlDir25Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f62, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][61];
  f62 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][61] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][61]*dAlpha; 
  
  return (2.-iIndexJ)*f62*((body[jBody].dPinc*body[iBody].dQinc+body[iBody].dPinc*body[jBody].dQinc));
}

double fdDdistDlDir25Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f62, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][61];
  f62 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][61] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][61]*dAlpha; 
  
  return (2.-iIndexJ)*f62*(-(body[iBody].dQinc*body[jBody].dQinc-body[iBody].dPinc*body[jBody].dPinc));
}

double fdDdistDlDir26Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f57, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][56];
  f57 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][56] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][56]*dAlpha; 
  
  return (2.-iIndexJ)*f57*(2*(body[jBody].dPinc*body[jBody].dQinc));
}

double fdDdistDlDir26Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f57, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][56];
  f57 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][56] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][56]*dAlpha; 
  
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
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][81];
  f82 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][81] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][81]*dAlpha;
        
  return (3-iIndexJ)*f82*((-body[iBody].dHecc*body[iBody].dHecc*body[iBody].dHecc\
          +3*body[iBody].dKecc*body[iBody].dKecc*body[iBody].dHecc));
}

double fdDdistDlDir31Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f82, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][81];
  f82 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][81] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][81]*dAlpha;
        
  return (3-iIndexJ)*f82*(-(body[iBody].dKecc*body[iBody].dKecc*body[iBody].dKecc\
          -3*body[iBody].dHecc*body[iBody].dHecc*body[iBody].dKecc));
}

double fdDdistDlDir32Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f83, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][82];
  f83 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][82] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][82]*dAlpha;
        
  return (3-iIndexJ)*f83*((body[jBody].dHecc*(body[iBody].dKecc*body[iBody].dKecc-body[iBody].dHecc*body[iBody].dHecc)\
          +2*body[jBody].dKecc*body[iBody].dKecc*body[iBody].dHecc));
}

double fdDdistDlDir32Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  double f83, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][82];
  f83 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][82] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][82]*dAlpha;
        
  return (3-iIndexJ)*f83*(-(body[jBody].dKecc*(body[iBody].dKecc*body[iBody].dKecc-body[iBody].dHecc*body[iBody].dHecc)\
          -2*body[jBody].dHecc*body[iBody].dHecc*body[iBody].dKecc));
}

double fdDdistDlDir33Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {        
  double f84, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][83];
  f84 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][83] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][83]*dAlpha; 
 
  return (3-iIndexJ)*f84*((body[iBody].dHecc*(body[jBody].dKecc*body[jBody].dKecc-body[jBody].dHecc*body[jBody].dHecc)\
          +2*body[iBody].dKecc*body[jBody].dKecc*body[jBody].dHecc));
}

double fdDdistDlDir33Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {        
  double f84, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][83];
  f84 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][83] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][83]*dAlpha; 
 
  return (3-iIndexJ)*f84*(-(body[iBody].dKecc*(body[jBody].dKecc*body[jBody].dKecc-body[jBody].dHecc*body[jBody].dHecc)\
          -2*body[iBody].dHecc*body[jBody].dHecc*body[jBody].dKecc));
}

double fdDdistDlDir34Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {        
  double f85, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][84];
  f85 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][84] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][84]*dAlpha; 
  
  return (3-iIndexJ)*f85*((-body[jBody].dHecc*body[jBody].dHecc*body[jBody].dHecc\
          +3*body[jBody].dKecc*body[jBody].dKecc*body[jBody].dHecc));
}

double fdDdistDlDir34Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {        
  double f85, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][84];
  f85 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][84] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][84]*dAlpha; 
  
  return (3-iIndexJ)*f85*(-(body[jBody].dKecc*body[jBody].dKecc*body[jBody].dKecc\
          -3*body[jBody].dHecc*body[jBody].dHecc*body[jBody].dKecc));
}

double fdDdistDlDir35Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {        
  double f86, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][85];
  f86 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][85] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][85]*dAlpha; 
 
  return (3-iIndexJ)*f86*((body[iBody].dHecc*(body[iBody].dQinc*body[iBody].dQinc-body[iBody].dPinc*body[iBody].dPinc)\
          +2*body[iBody].dKecc*body[iBody].dPinc*body[iBody].dQinc));
}

double fdDdistDlDir35Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {        
  double f86, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][85];
  f86 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][85] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][85]*dAlpha; 
 
  return (3-iIndexJ)*f86*(-(body[iBody].dKecc*(body[iBody].dQinc*body[iBody].dQinc-body[iBody].dPinc*body[iBody].dPinc)\
          -2*body[iBody].dHecc*body[iBody].dPinc*body[iBody].dQinc));
}

double fdDdistDlDir36Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {        
  double f87, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][86];
  f87 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][86] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][86]*dAlpha; 
 
  return (3-iIndexJ)*f87*((body[jBody].dHecc*(body[iBody].dQinc*body[iBody].dQinc-body[iBody].dPinc*body[iBody].dPinc)\
          +2*body[jBody].dKecc*body[iBody].dPinc*body[iBody].dQinc));
}

double fdDdistDlDir36Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {        
  double f87, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][86];
  f87 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][86] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][86]*dAlpha; 
 
  return (3-iIndexJ)*f87*(-(body[jBody].dKecc*(body[iBody].dQinc*body[iBody].dQinc-body[iBody].dPinc*body[iBody].dPinc)\
          -2*body[jBody].dHecc*body[iBody].dPinc*body[iBody].dQinc));
}

double fdDdistDlDir37Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {        
  double f88, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][87];
  f88 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][87] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][87]*dAlpha;
  
  return (3-iIndexJ)*f88*((body[iBody].dHecc*(body[jBody].dQinc*body[iBody].dQinc-body[jBody].dPinc*body[iBody].dPinc)\
          +body[iBody].dKecc*(body[jBody].dPinc*body[iBody].dQinc+body[jBody].dQinc*body[iBody].dPinc)));
}

double fdDdistDlDir37Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {        
  double f88, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][87];
  f88 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][87] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][87]*dAlpha;
  
  return (3-iIndexJ)*f88*(-(body[iBody].dKecc*(body[jBody].dQinc*body[iBody].dQinc-body[jBody].dPinc*body[iBody].dPinc)\
          -body[iBody].dHecc*(body[jBody].dPinc*body[iBody].dQinc+body[jBody].dQinc*body[iBody].dPinc)));
}

double fdDdistDlDir38Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {        
  double f89, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][88];
  f89 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][88] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][88]*dAlpha;
  
  return (3-iIndexJ)*f89*((body[jBody].dHecc*(body[jBody].dQinc*body[iBody].dQinc-body[jBody].dPinc*body[iBody].dPinc)\
          +body[jBody].dKecc*(body[jBody].dPinc*body[iBody].dQinc+body[jBody].dQinc*body[iBody].dPinc)));
}

double fdDdistDlDir38Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {        
  double f89, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][88];
  f89 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][88] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][88]*dAlpha;
  
  return (3-iIndexJ)*f89*(-(body[jBody].dKecc*(body[jBody].dQinc*body[iBody].dQinc-body[jBody].dPinc*body[iBody].dPinc)\
          -body[jBody].dHecc*(body[jBody].dPinc*body[iBody].dQinc+body[jBody].dQinc*body[iBody].dPinc)));
}

double fdDdistDlDir39Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {        
  double f86, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][85];
  f86 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][85] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][85]*dAlpha;  
 
  return (3-iIndexJ)*f86*((body[iBody].dHecc*(body[jBody].dQinc*body[jBody].dQinc-body[jBody].dPinc*body[jBody].dPinc)\
          +2*body[iBody].dKecc*body[jBody].dPinc*body[jBody].dQinc));
}

double fdDdistDlDir39Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {        
  double f86, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][85];
  f86 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][85] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][85]*dAlpha;  
 
  return (3-iIndexJ)*f86*(-(body[iBody].dKecc*(body[jBody].dQinc*body[jBody].dQinc-body[jBody].dPinc*body[jBody].dPinc)\
          -2*body[iBody].dHecc*body[jBody].dPinc*body[jBody].dQinc));
}

double fdDdistDlDir310Cos(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {        
  double f87, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][86];
  f87 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][86] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][86]*dAlpha; 
  
  return (3-iIndexJ)*f87*((body[jBody].dHecc*(body[jBody].dQinc*body[jBody].dQinc-body[jBody].dPinc*body[jBody].dPinc)\
          +2*body[jBody].dKecc*body[jBody].dPinc*body[jBody].dQinc));
}

double fdDdistDlDir310Sin(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {        
  double f87, dAlpha;
  dAlpha = body[iBody].dSemi/body[jBody].dSemi - system->dmAlpha0[iIndexJ][system->imLaplaceN[iBody][jBody]][86];
  f87 = system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][86] + \
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][86]*dAlpha; 
  
  return (3-iIndexJ)*f87*(-(body[jBody].dKecc*(body[jBody].dQinc*body[jBody].dQinc-body[jBody].dPinc*body[jBody].dPinc)\
          -2*body[jBody].dHecc*body[jBody].dPinc*body[jBody].dQinc));
}

double fdDdistresDLambdaDirCos(BODY *body, SYSTEM *system, int *iaBody, int iIndexJ, int iOrder) {
  double y;
  if (iOrder == 1) {
    y = ( fdDdistDlDir11Cos(body, system, iaBody[0], iaBody[1], iIndexJ)\
      + fdDdistDlDir12Cos(body, system, iaBody[0], iaBody[1], iIndexJ) \
      + fdDdistDlDir13Cos(body, system, iaBody[0], iaBody[1], iIndexJ)\
      + fdDdistDlDir14Cos(body, system, iaBody[0], iaBody[1], iIndexJ)\
      + fdDdistDlDir15Cos(body, system, iaBody[0], iaBody[1], iIndexJ)\
      + fdDdistDlDir16Cos(body, system, iaBody[0], iaBody[1], iIndexJ)\
      + fdDdistDlDir17Cos(body, system, iaBody[0], iaBody[1], iIndexJ)\
      + fdDdistDlDir18Cos(body, system, iaBody[0], iaBody[1], iIndexJ)\
      + fdDdistDlDir19Cos(body, system, iaBody[0], iaBody[1], iIndexJ)\
      + fdDdistDlDir110Cos(body, system, iaBody[0], iaBody[1], iIndexJ)\
      + fdDdistDlDir111Cos(body, system, iaBody[0], iaBody[1], iIndexJ)\
      + fdDdistDlDir112Cos(body, system, iaBody[0], iaBody[1], iIndexJ)\
      + fdDdistDlDir113Cos(body, system, iaBody[0], iaBody[1], iIndexJ)\
      + fdDdistDlDir114Cos(body, system, iaBody[0], iaBody[1], iIndexJ) );
  } else if (iOrder == 2) {
    y = ( fdDdistDlDir21Cos(body, system, iaBody[0], iaBody[1], iIndexJ) \
    + fdDdistDlDir22Cos(body, system, iaBody[0], iaBody[1], iIndexJ) \
    + fdDdistDlDir23Cos(body, system, iaBody[0], iaBody[1], iIndexJ) \
    + fdDdistDlDir24Cos(body, system, iaBody[0], iaBody[1], iIndexJ) \
    + fdDdistDlDir25Cos(body, system, iaBody[0], iaBody[1], iIndexJ) \
    + fdDdistDlDir26Cos(body, system, iaBody[0], iaBody[1], iIndexJ) );
  } else if (iOrder == 3) {
    y = ( fdDdistDlDir31Cos(body, system, iaBody[0], iaBody[1], iIndexJ) \
    + fdDdistDlDir32Cos(body, system, iaBody[0], iaBody[1], iIndexJ) \
    + fdDdistDlDir33Cos(body, system, iaBody[0], iaBody[1], iIndexJ) \
    + fdDdistDlDir34Cos(body, system, iaBody[0], iaBody[1], iIndexJ) \
    + fdDdistDlDir35Cos(body, system, iaBody[0], iaBody[1], iIndexJ) \
    + fdDdistDlDir36Cos(body, system, iaBody[0], iaBody[1], iIndexJ) \
    + fdDdistDlDir37Cos(body, system, iaBody[0], iaBody[1], iIndexJ) \
    + fdDdistDlDir38Cos(body, system, iaBody[0], iaBody[1], iIndexJ) \
    + fdDdistDlDir39Cos(body, system, iaBody[0], iaBody[1], iIndexJ) \
    + fdDdistDlDir310Cos(body, system, iaBody[0], iaBody[1], iIndexJ) );
  }
  return y;
}

double fdDdistresDLambdaDirSin(BODY *body, SYSTEM *system, int *iaBody, int iIndexJ, int iOrder) {
  double y;
  if (iOrder == 1) {
    y = ( fdDdistDlDir11Sin(body, system, iaBody[0], iaBody[1], iIndexJ)\
      + fdDdistDlDir12Sin(body, system, iaBody[0], iaBody[1], iIndexJ) \
      + fdDdistDlDir13Sin(body, system, iaBody[0], iaBody[1], iIndexJ)\
      + fdDdistDlDir14Sin(body, system, iaBody[0], iaBody[1], iIndexJ)\
      + fdDdistDlDir15Sin(body, system, iaBody[0], iaBody[1], iIndexJ)\
      + fdDdistDlDir16Sin(body, system, iaBody[0], iaBody[1], iIndexJ)\
      + fdDdistDlDir17Sin(body, system, iaBody[0], iaBody[1], iIndexJ)\
      + fdDdistDlDir18Sin(body, system, iaBody[0], iaBody[1], iIndexJ)\
      + fdDdistDlDir19Sin(body, system, iaBody[0], iaBody[1], iIndexJ)\
      + fdDdistDlDir110Sin(body, system, iaBody[0], iaBody[1], iIndexJ)\
      + fdDdistDlDir111Sin(body, system, iaBody[0], iaBody[1], iIndexJ)\
      + fdDdistDlDir112Sin(body, system, iaBody[0], iaBody[1], iIndexJ)\
      + fdDdistDlDir113Sin(body, system, iaBody[0], iaBody[1], iIndexJ)\
      + fdDdistDlDir114Sin(body, system, iaBody[0], iaBody[1], iIndexJ) );
  } else if (iOrder == 2) {
    y = ( fdDdistDlDir21Sin(body, system, iaBody[0], iaBody[1], iIndexJ) \
    + fdDdistDlDir22Sin(body, system, iaBody[0], iaBody[1], iIndexJ) \
    + fdDdistDlDir23Sin(body, system, iaBody[0], iaBody[1], iIndexJ) \
    + fdDdistDlDir24Sin(body, system, iaBody[0], iaBody[1], iIndexJ) \
    + fdDdistDlDir25Sin(body, system, iaBody[0], iaBody[1], iIndexJ) \
    + fdDdistDlDir26Sin(body, system, iaBody[0], iaBody[1], iIndexJ) );
  } else if (iOrder == 3) {
    y = ( fdDdistDlDir31Sin(body, system, iaBody[0], iaBody[1], iIndexJ) \
    + fdDdistDlDir32Sin(body, system, iaBody[0], iaBody[1], iIndexJ) \
    + fdDdistDlDir33Sin(body, system, iaBody[0], iaBody[1], iIndexJ) \
    + fdDdistDlDir34Sin(body, system, iaBody[0], iaBody[1], iIndexJ) \
    + fdDdistDlDir35Sin(body, system, iaBody[0], iaBody[1], iIndexJ) \
    + fdDdistDlDir36Sin(body, system, iaBody[0], iaBody[1], iIndexJ) \
    + fdDdistDlDir37Sin(body, system, iaBody[0], iaBody[1], iIndexJ) \
    + fdDdistDlDir38Sin(body, system, iaBody[0], iaBody[1], iIndexJ) \
    + fdDdistDlDir39Sin(body, system, iaBody[0], iaBody[1], iIndexJ) \
    + fdDdistDlDir310Sin(body, system, iaBody[0], iaBody[1], iIndexJ) );
  }
  return y;
}

double fdDdistresDLambda(BODY *body, SYSTEM *system, int *iaBody, int iIndexJ, int iOrder) {
  double y, dMfac, dSemiPrm;
  dMfac = KGAUSS*KGAUSS*body[iaBody[1]].dMass/MSUN;
  dSemiPrm = body[iaBody[1]].dSemi/AUCM;
  
  yc = fdDdistresDLambdaDirCos(body,system,iaBody,iIndexJ,iOrder);
  ys = fdDdistresDLambdaDirSin(body,system,iaBody,iIndexJ,iOrder); 
  if (iOrder == 1) {
    if (iIndexJ == 2) {
      yc += (fdDdistDlExt13Cos(body,system,iaBody[0],iaBody[1],iIndexJ)\
         + fdDdistDlExt111Cos(body,system,iaBody[0],iaBody[1],iIndexJ))\
            *body[iaBody[0]].dSemi/body[iaBody[1]].dSemi;
      ys += (fdDdistDlExt13Sin(body,system,iaBody[0],iaBody[1],iIndexJ)\
         + fdDdistDlExt111Sin(body,system,iaBody[0],iaBody[1],iIndexJ))\
            *body[iaBody[0]].dSemi/body[iaBody[1]].dSemi;
    } else if (iIndexJ == 3) {
      yc += (fdDdistDlExt16Cos(body,system,iaBody[0],iaBody[1],iIndexJ))\
            *body[iaBody[0]].dSemi/body[iaBody[1]].dSemi;
      ys += (fdDdistDlExt16Sin(body,system,iaBody[0],iaBody[1],iIndexJ))\
            *body[iaBody[0]].dSemi/body[iaBody[1]].dSemi;
    }
  } else if (iOrder == 2) {
    if (iIndexJ == 3) {
      yc += (fdDdistDlExt25Cos(body,system,iaBody[0],iaBody[1],iIndexJ))\
            *body[iaBody[0]].dSemi/body[iaBody[1]].dSemi;
      ys += (fdDdistDlExt25Sin(body,system,iaBody[0],iaBody[1],iIndexJ))\
            *body[iaBody[0]].dSemi/body[iaBody[1]].dSemi;
    }
  }
  
  y = (yc*cos(fdLambdaArg(body,system,iaBody[0],iaBody[1],iIndexJ,iOrder)) \
       + ys*sin(fdLambdaArg(body,system,iaBody[0],iaBody[1],iIndexJ,iOrder))) * dMfac/dSemiPrm;
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

double fdDdistresDLambdaPrimeDirCos(BODY *body, SYSTEM *system, int *iaBody, int iIndexJ, int iOrder) {
  double y;
  if (iOrder == 1) { 
    y = ( fdDdistDlPrmDir11Cos(body, system, iaBody[0], iaBody[1], iIndexJ)\
      + fdDdistDlPrmDir12Cos(body, system, iaBody[0], iaBody[1], iIndexJ)\
      + fdDdistDlPrmDir13Cos(body, system, iaBody[0], iaBody[1], iIndexJ)\
      + fdDdistDlPrmDir14Cos(body, system, iaBody[0], iaBody[1], iIndexJ)\
      + fdDdistDlPrmDir15Cos(body, system, iaBody[0], iaBody[1], iIndexJ)\
      + fdDdistDlPrmDir16Cos(body, system, iaBody[0], iaBody[1], iIndexJ)\
      + fdDdistDlPrmDir17Cos(body, system, iaBody[0], iaBody[1], iIndexJ) \
      + fdDdistDlPrmDir18Cos(body, system, iaBody[0], iaBody[1], iIndexJ)\
      + fdDdistDlPrmDir19Cos(body, system, iaBody[0], iaBody[1], iIndexJ)\
      + fdDdistDlPrmDir110Cos(body, system, iaBody[0], iaBody[1], iIndexJ)\
      + fdDdistDlPrmDir111Cos(body, system, iaBody[0], iaBody[1], iIndexJ)\
      + fdDdistDlPrmDir112Cos(body, system, iaBody[0], iaBody[1], iIndexJ)\
      + fdDdistDlPrmDir113Cos(body, system, iaBody[0], iaBody[1], iIndexJ)\
      + fdDdistDlPrmDir114Cos(body, system, iaBody[0], iaBody[1], iIndexJ) );
  } else if (iOrder == 2) {
    y = ( fdDdistDlPrmDir21Cos(body, system, iaBody[0], iaBody[1], iIndexJ) \
    + fdDdistDlPrmDir22Cos(body, system, iaBody[0], iaBody[1], iIndexJ)
    + fdDdistDlPrmDir23Cos(body, system, iaBody[0], iaBody[1], iIndexJ) \
    + fdDdistDlPrmDir24Cos(body, system, iaBody[0], iaBody[1], iIndexJ)
    + fdDdistDlPrmDir25Cos(body, system, iaBody[0], iaBody[1], iIndexJ) \
    + fdDdistDlPrmDir26Cos(body, system, iaBody[0], iaBody[1], iIndexJ) );
  } else if (iOrder == 3) {
    y = ( fdDdistDlPrmDir31Cos(body, system, iaBody[0], iaBody[1], iIndexJ) \
    + fdDdistDlPrmDir32Cos(body, system, iaBody[0], iaBody[1], iIndexJ) \
    + fdDdistDlPrmDir33Cos(body, system, iaBody[0], iaBody[1], iIndexJ) \
    + fdDdistDlPrmDir34Cos(body, system, iaBody[0], iaBody[1], iIndexJ) \
    + fdDdistDlPrmDir35Cos(body, system, iaBody[0], iaBody[1], iIndexJ) \
    + fdDdistDlPrmDir36Cos(body, system, iaBody[0], iaBody[1], iIndexJ) \
    + fdDdistDlPrmDir37Cos(body, system, iaBody[0], iaBody[1], iIndexJ) \
    + fdDdistDlPrmDir38Cos(body, system, iaBody[0], iaBody[1], iIndexJ) \
    + fdDdistDlPrmDir39Cos(body, system, iaBody[0], iaBody[1], iIndexJ) \
    + fdDdistDlPrmDir310Cos(body, system, iaBody[0], iaBody[1], iIndexJ) );
  }
  return y;
}

double fdDdistresDLambdaPrimeDirSin(BODY *body, SYSTEM *system, int *iaBody, int iIndexJ, int iOrder) {
  double y;
  if (iOrder == 1) { 
    y = ( fdDdistDlPrmDir11Sin(body, system, iaBody[0], iaBody[1], iIndexJ)\
      + fdDdistDlPrmDir12Sin(body, system, iaBody[0], iaBody[1], iIndexJ)\
      + fdDdistDlPrmDir13Sin(body, system, iaBody[0], iaBody[1], iIndexJ)\
      + fdDdistDlPrmDir14Sin(body, system, iaBody[0], iaBody[1], iIndexJ)\
      + fdDdistDlPrmDir15Sin(body, system, iaBody[0], iaBody[1], iIndexJ)\
      + fdDdistDlPrmDir16Sin(body, system, iaBody[0], iaBody[1], iIndexJ)\
      + fdDdistDlPrmDir17Sin(body, system, iaBody[0], iaBody[1], iIndexJ) \
      + fdDdistDlPrmDir18Sin(body, system, iaBody[0], iaBody[1], iIndexJ)\
      + fdDdistDlPrmDir19Sin(body, system, iaBody[0], iaBody[1], iIndexJ)\
      + fdDdistDlPrmDir110Sin(body, system, iaBody[0], iaBody[1], iIndexJ)\
      + fdDdistDlPrmDir111Sin(body, system, iaBody[0], iaBody[1], iIndexJ)\
      + fdDdistDlPrmDir112Sin(body, system, iaBody[0], iaBody[1], iIndexJ)\
      + fdDdistDlPrmDir113Sin(body, system, iaBody[0], iaBody[1], iIndexJ)\
      + fdDdistDlPrmDir114Sin(body, system, iaBody[0], iaBody[1], iIndexJ) );
  } else if (iOrder == 2) {
    y = ( fdDdistDlPrmDir21Sin(body, system, iaBody[0], iaBody[1], iIndexJ) \
    + fdDdistDlPrmDir22Sin(body, system, iaBody[0], iaBody[1], iIndexJ)
    + fdDdistDlPrmDir23Sin(body, system, iaBody[0], iaBody[1], iIndexJ) \
    + fdDdistDlPrmDir24Sin(body, system, iaBody[0], iaBody[1], iIndexJ)
    + fdDdistDlPrmDir25Sin(body, system, iaBody[0], iaBody[1], iIndexJ) \
    + fdDdistDlPrmDir26Sin(body, system, iaBody[0], iaBody[1], iIndexJ) );
  } else if (iOrder == 3) {
    y = ( fdDdistDlPrmDir31Sin(body, system, iaBody[0], iaBody[1], iIndexJ) \
    + fdDdistDlPrmDir32Sin(body, system, iaBody[0], iaBody[1], iIndexJ) \
    + fdDdistDlPrmDir33Sin(body, system, iaBody[0], iaBody[1], iIndexJ) \
    + fdDdistDlPrmDir34Sin(body, system, iaBody[0], iaBody[1], iIndexJ) \
    + fdDdistDlPrmDir35Sin(body, system, iaBody[0], iaBody[1], iIndexJ) \
    + fdDdistDlPrmDir36Sin(body, system, iaBody[0], iaBody[1], iIndexJ) \
    + fdDdistDlPrmDir37Sin(body, system, iaBody[0], iaBody[1], iIndexJ) \
    + fdDdistDlPrmDir38Sin(body, system, iaBody[0], iaBody[1], iIndexJ) \
    + fdDdistDlPrmDir39Sin(body, system, iaBody[0], iaBody[1], iIndexJ) \
    + fdDdistDlPrmDir310Sin(body, system, iaBody[0], iaBody[1], iIndexJ) );
  }
  return y;
}

double fdDdistresDLambdaPrime(BODY *body, SYSTEM *system, int *iaBody, int iIndexJ, int iOrder) {
  double y, yc, ys, dMfac, dSemiPrm;
  dMfac = KGAUSS*KGAUSS*body[iaBody[1]].dMass/MSUN;
  dSemiPrm = body[iaBody[0]].dSemi/AUCM;
  
  yc = fdDdistresDLambdaPrimeDirCos(body,system,iaBody,iIndexJ,iOrder);
  ys = fdDdistresDLambdaPrimeDirSin(body,system,iaBody,iIndexJ,iOrder); 
  if (iOrder == 1) { 
    if (iIndexJ == 2) {
       yc += (fdDdistDlPrmInt13Cos(body, system, iaBody[0], iaBody[1], iIndexJ)\
           + fdDdistDlPrmInt111Cos(body, system, iaBody[0], iaBody[1], iIndexJ))\
            /(body[iaBody[1]].dSemi*body[iaBody[1]].dSemi/AUCM/AUCM);
       ys += (fdDdistDlPrmInt13Sin(body, system, iaBody[0], iaBody[1], iIndexJ)\
           + fdDdistDlPrmInt111Sin(body, system, iaBody[0], iaBody[1], iIndexJ))\
            /(body[iaBody[1]].dSemi*body[iaBody[1]].dSemi/AUCM/AUCM);
    } else if (iIndexJ == 3) {
       yc += (fdDdistDlPrmInt16Cos(body, system, iaBody[0], iaBody[1], iIndexJ))\
            /(body[iaBody[1]].dSemi*body[iaBody[1]].dSemi/AUCM/AUCM);
       ys += (fdDdistDlPrmInt16Sin(body, system, iaBody[0], iaBody[1], iIndexJ))\
            /(body[iaBody[1]].dSemi*body[iaBody[1]].dSemi/AUCM/AUCM);
    }
  } else if (iOrder == 2) {
    if (iIndexJ == 3) {
       yc += (fdDdistDlPrmInt25Cos(body, system, iaBody[0], iaBody[1], iIndexJ))\
            /(body[iaBody[1]].dSemi*body[iaBody[1]].dSemi/AUCM/AUCM);
       ys += (fdDdistDlPrmInt25Sin(body, system, iaBody[0], iaBody[1], iIndexJ))\
            /(body[iaBody[1]].dSemi*body[iaBody[1]].dSemi/AUCM/AUCM);
    }
  }
  
  y = (yc*cos(fdLambdaArg(body,system,iaBody[0],iaBody[1],iIndexJ,iOrder)) \
       + ys*sin(fdLambdaArg(body,system,iaBody[0],iaBody[1],iIndexJ,iOrder))) * dMfac/dSemiPrm;
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
  return AUCM/body[jBody].dSemi*(system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][26]\
        +(body[iBody].dHecc*body[iBody].dHecc+body[iBody].dKecc*body[iBody].dKecc)*\
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][27]\
        +(body[jBody].dHecc*body[jBody].dHecc+body[jBody].dKecc*body[jBody].dKecc)*\
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][28]\
        +(body[iBody].dPinc*body[iBody].dPinc+body[iBody].dQinc*body[iBody].dQinc+
        body[jBody].dPinc*body[jBody].dPinc+body[jBody].dQinc*body[jBody].dQinc)*\
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][29])\
         *(body[iBody].dKecc*cos(fdLambdaArg(body,system,iBody,jBody,iIndexJ,1))\
         +body[iBody].dHecc*sin(fdLambdaArg(body,system,iBody,jBody,iIndexJ,1)));
}

double fdDdistDaDir12(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return AUCM/body[jBody].dSemi*(system->dmLaplaceD[iIndexJ][system->imLaplaceN[jBody][iBody]][30] \
        +(body[iBody].dHecc*body[iBody].dHecc+body[iBody].dKecc*body[iBody].dKecc)*\
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][31]\
        +(body[jBody].dHecc*body[jBody].dHecc+body[jBody].dKecc*body[jBody].dKecc)*\
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][32]\
        +(body[iBody].dPinc*body[iBody].dPinc+body[iBody].dQinc*body[iBody].dQinc+
        body[jBody].dPinc*body[jBody].dPinc+body[jBody].dQinc*body[jBody].dQinc)*\
        system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][33])
         *(body[jBody].dKecc*cos(fdLambdaArg(body,system,iBody,jBody,iIndexJ,1))\
         +body[jBody].dHecc*sin(fdLambdaArg(body,system,iBody,jBody,iIndexJ,1)));
}

double fdDdistDaDir13(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return AUCM/body[jBody].dSemi*system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][34]*\
    ((body[jBody].dKecc*(body[iBody].dKecc*body[iBody].dKecc-body[iBody].dHecc*body[iBody].dHecc)\
    +2*body[jBody].dHecc*body[iBody].dHecc*body[iBody].dKecc)\
    *cos(fdLambdaArg(body,system,iBody,jBody,iIndexJ,1))-(body[jBody].dHecc*\
    (body[iBody].dKecc*body[iBody].dKecc-body[iBody].dHecc*body[iBody].dHecc)\
    -2*body[jBody].dKecc*body[iBody].dHecc*body[iBody].dKecc)\
    *sin(fdLambdaArg(body,system,iBody,jBody,iIndexJ,1)));
}

double fdDdistDaDir14(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return AUCM/body[jBody].dSemi*system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][35]*\
    ((body[iBody].dKecc*(body[jBody].dKecc*body[jBody].dKecc-body[jBody].dHecc*body[jBody].dHecc)\
    +2*body[iBody].dHecc*body[jBody].dHecc*body[jBody].dKecc)\
    *cos(fdLambdaArg(body,system,iBody,jBody,iIndexJ,1))-(body[iBody].dHecc*\
    (body[jBody].dKecc*body[jBody].dKecc-body[jBody].dHecc*body[jBody].dHecc)\
    -2*body[iBody].dKecc*body[jBody].dHecc*body[jBody].dKecc)\
    *sin(fdLambdaArg(body,system,iBody,jBody,iIndexJ,1)));
}

double fdDdistDaDir15(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return AUCM/body[jBody].dSemi*system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][36]*\
    ((body[iBody].dKecc*(body[iBody].dQinc*body[iBody].dQinc-body[iBody].dPinc*body[iBody].dPinc)\
    +2*body[iBody].dHecc*body[iBody].dPinc*body[iBody].dQinc)\
    *cos(fdLambdaArg(body,system,iBody,jBody,iIndexJ,1))-(body[iBody].dHecc*\
    (body[iBody].dQinc*body[iBody].dQinc-body[iBody].dPinc*body[iBody].dPinc)\
    -2*body[iBody].dKecc*body[iBody].dPinc*body[iBody].dQinc)\
    *sin(fdLambdaArg(body,system,iBody,jBody,iIndexJ,1)));
}

double fdDdistDaDir16(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return AUCM/body[jBody].dSemi*system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][37]*\
    ((body[jBody].dKecc*(body[iBody].dQinc*body[iBody].dQinc-body[iBody].dPinc*body[iBody].dPinc)\
    +2*body[jBody].dHecc*body[iBody].dPinc*body[iBody].dQinc)\
    *cos(fdLambdaArg(body,system,iBody,jBody,iIndexJ,1))-(body[jBody].dHecc*\
    (body[iBody].dQinc*body[iBody].dQinc-body[iBody].dPinc*body[iBody].dPinc)\
    -2*body[jBody].dKecc*body[iBody].dPinc*body[iBody].dQinc)\
    *sin(fdLambdaArg(body,system,iBody,jBody,iIndexJ,1)));
}

double fdDdistDaDir17(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return AUCM/body[jBody].dSemi*system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][38]*\
    ((body[iBody].dKecc*(body[iBody].dQinc*body[jBody].dQinc+body[iBody].dPinc*body[jBody].dPinc)\
    -body[iBody].dHecc*(body[iBody].dQinc*body[jBody].dPinc-body[iBody].dPinc*body[jBody].dQinc))\
    *cos(fdLambdaArg(body,system,iBody,jBody,iIndexJ,1))+(body[iBody].dHecc*\
    (body[iBody].dQinc*body[jBody].dQinc+body[iBody].dPinc*body[jBody].dPinc)\
    +body[iBody].dKecc*(body[iBody].dQinc*body[jBody].dPinc-body[iBody].dPinc*body[jBody].dQinc))\
    *sin(fdLambdaArg(body,system,iBody,jBody,iIndexJ,1)));
}

double fdDdistDaDir18(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return AUCM/body[jBody].dSemi*system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][39]*\
    ((body[iBody].dKecc*(body[iBody].dQinc*body[jBody].dQinc+body[iBody].dPinc*body[jBody].dPinc)\
    -body[iBody].dHecc*(body[jBody].dQinc*body[iBody].dPinc-body[jBody].dPinc*body[iBody].dQinc))\
    *cos(fdLambdaArg(body,system,iBody,jBody,iIndexJ,1))+(body[iBody].dHecc*\
    (body[iBody].dQinc*body[jBody].dQinc+body[iBody].dPinc*body[jBody].dPinc)\
    +body[iBody].dKecc*(body[jBody].dQinc*body[iBody].dPinc-body[jBody].dPinc*body[iBody].dQinc))\
    *sin(fdLambdaArg(body,system,iBody,jBody,iIndexJ,1)));
}

double fdDdistDaDir19(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return AUCM/body[jBody].dSemi*system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][40]*\
    ((body[iBody].dQinc*(body[jBody].dQinc*body[iBody].dKecc+body[jBody].dPinc*body[iBody].dHecc)\
    -body[iBody].dPinc*(body[jBody].dPinc*body[iBody].dKecc-body[jBody].dQinc*body[iBody].dHecc))\
    *cos(fdLambdaArg(body,system,iBody,jBody,iIndexJ,1))+(body[iBody].dPinc*\
    (body[jBody].dQinc*body[iBody].dKecc+body[jBody].dPinc*body[iBody].dHecc)\
    +body[iBody].dQinc*(body[jBody].dPinc*body[iBody].dKecc-body[jBody].dQinc*body[iBody].dHecc))\
    *sin(fdLambdaArg(body,system,iBody,jBody,iIndexJ,1)));
}

double fdDdistDaDir110(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return AUCM/body[jBody].dSemi*system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][41]*\
    ((body[jBody].dKecc*(body[iBody].dQinc*body[jBody].dQinc+body[iBody].dPinc*body[jBody].dPinc)\
    -body[jBody].dHecc*(body[iBody].dQinc*body[jBody].dPinc-body[iBody].dPinc*body[jBody].dQinc))\
    *cos(fdLambdaArg(body,system,iBody,jBody,iIndexJ,1))+(body[jBody].dHecc*\
    (body[iBody].dQinc*body[jBody].dQinc+body[iBody].dPinc*body[jBody].dPinc)\
    +body[jBody].dKecc*(body[iBody].dQinc*body[jBody].dPinc-body[iBody].dPinc*body[jBody].dQinc))\
    *sin(fdLambdaArg(body,system,iBody,jBody,iIndexJ,1)));
}

double fdDdistDaDir111(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return AUCM/body[jBody].dSemi*system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][42]*\
    ((body[jBody].dKecc*(body[iBody].dQinc*body[jBody].dQinc+body[iBody].dPinc*body[jBody].dPinc)\
    -body[jBody].dHecc*(body[jBody].dQinc*body[iBody].dPinc-body[jBody].dPinc*body[iBody].dQinc))\
    *cos(fdLambdaArg(body,system,iBody,jBody,iIndexJ,1))+(body[jBody].dHecc*\
    (body[iBody].dQinc*body[jBody].dQinc+body[iBody].dPinc*body[jBody].dPinc)\
    +body[jBody].dKecc*(body[jBody].dQinc*body[iBody].dPinc-body[jBody].dPinc*body[iBody].dQinc))\
    *sin(fdLambdaArg(body,system,iBody,jBody,iIndexJ,1)));
}

double fdDdistDaDir112(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return AUCM/body[jBody].dSemi*system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][43]*\
    ((body[iBody].dQinc*(body[jBody].dQinc*body[jBody].dKecc+body[jBody].dPinc*body[jBody].dHecc)\
    -body[iBody].dPinc*(body[jBody].dPinc*body[jBody].dKecc-body[jBody].dQinc*body[jBody].dHecc))\
    *cos(fdLambdaArg(body,system,iBody,jBody,iIndexJ,1))+(body[iBody].dPinc*\
    (body[jBody].dQinc*body[jBody].dKecc+body[jBody].dPinc*body[jBody].dHecc)\
    +body[iBody].dQinc*(body[jBody].dPinc*body[jBody].dKecc-body[jBody].dQinc*body[jBody].dHecc))\
    *sin(fdLambdaArg(body,system,iBody,jBody,iIndexJ,1)));
}

double fdDdistDaDir113(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return AUCM/body[jBody].dSemi*system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][36]*\
    ((body[iBody].dKecc*(body[jBody].dQinc*body[jBody].dQinc-body[jBody].dPinc*body[jBody].dPinc)\
    +2*body[iBody].dHecc*body[jBody].dPinc*body[jBody].dQinc)\
    *cos(fdLambdaArg(body,system,iBody,jBody,iIndexJ,1))-(body[iBody].dHecc*\
    (body[jBody].dQinc*body[jBody].dQinc-body[jBody].dPinc*body[jBody].dPinc)\
    -2*body[iBody].dKecc*body[jBody].dPinc*body[jBody].dQinc)\
    *sin(fdLambdaArg(body,system,iBody,jBody,iIndexJ,1)));
}

double fdDdistDaDir114(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return AUCM/body[jBody].dSemi*system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][37]*\
    ((body[jBody].dKecc*(body[jBody].dQinc*body[jBody].dQinc-body[jBody].dPinc*body[jBody].dPinc)\
    +2*body[jBody].dHecc*body[jBody].dPinc*body[jBody].dQinc)\
    *cos(fdLambdaArg(body,system,iBody,jBody,iIndexJ,1))-(body[jBody].dHecc*\
    (body[jBody].dQinc*body[jBody].dQinc-body[jBody].dPinc*body[jBody].dPinc)\
    -2*body[jBody].dKecc*body[jBody].dPinc*body[jBody].dQinc)\
    *sin(fdLambdaArg(body,system,iBody,jBody,iIndexJ,1)));
}

double fdDdistDaExt13(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return AUCM/body[jBody].dSemi*(body[jBody].dKecc*\
      cos(fdLambdaArg(body,system,iBody,jBody,iIndexJ,1))+body[jBody].dHecc*\
      sin(fdLambdaArg(body,system,iBody,jBody,iIndexJ,1)))*\
      (-2+(body[iBody].dHecc*body[iBody].dHecc+body[iBody].dKecc*body[iBody].dKecc)+\
      1.5*(body[jBody].dHecc*body[jBody].dHecc+body[jBody].dKecc*body[jBody].dKecc)+\
      2.*(body[iBody].dPinc*body[iBody].dPinc+body[iBody].dQinc*body[iBody].dQinc+\
      body[jBody].dPinc*body[jBody].dPinc+body[jBody].dQinc*body[jBody].dQinc));
}

double fdDdistDaExt16(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return -27./16*AUCM/body[jBody].dSemi*\
    ((body[iBody].dKecc*(body[jBody].dKecc*body[jBody].dKecc-body[jBody].dHecc*body[jBody].dHecc)\
    +2*body[iBody].dHecc*body[jBody].dHecc*body[jBody].dKecc)\
    *cos(fdLambdaArg(body,system,iBody,jBody,iIndexJ,1))-(body[iBody].dHecc*\
    (body[jBody].dKecc*body[jBody].dKecc-body[jBody].dHecc*body[jBody].dHecc)\
    -2*body[iBody].dKecc*body[jBody].dHecc*body[jBody].dKecc)\
    *sin(fdLambdaArg(body,system,iBody,jBody,iIndexJ,1)));
}

double fdDdistDaExt111(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return -4*AUCM/body[jBody].dSemi*\
    ((body[iBody].dQinc*(body[jBody].dQinc*body[jBody].dKecc-body[jBody].dPinc*body[jBody].dHecc)\
    +body[iBody].dPinc*(body[jBody].dPinc*body[jBody].dKecc+body[jBody].dQinc*body[jBody].dHecc))\
    *cos(fdLambdaArg(body,system,iBody,jBody,iIndexJ,1))+(body[iBody].dPinc*\
    (-body[jBody].dQinc*body[jBody].dKecc+body[jBody].dPinc*body[jBody].dHecc)\
    +body[iBody].dQinc*(body[jBody].dPinc*body[jBody].dKecc+body[jBody].dQinc*body[jBody].dHecc))\
    *sin(fdLambdaArg(body,system,iBody,jBody,iIndexJ,1)));
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

double fdDdistDaExt25(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return -27./8*AUCM/body[jBody].dSemi*\
    ((body[jBody].dKecc*body[jBody].dKecc-body[jBody].dHecc*body[jBody].dHecc)\
    *cos(fdLambdaArg(body,system,iBody,jBody,iIndexJ,2))+
    2*body[jBody].dHecc*body[jBody].dKecc\
    *sin(fdLambdaArg(body,system,iBody,jBody,iIndexJ,2)));
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
  return AUCM/body[jBody].dSemi*system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][85]\
          *(cos(fdLambdaArg(body,system,iBody,jBody,iIndexJ,3))\
          *(body[iBody].dKecc*(body[jBody].dQinc*body[jBody].dQinc-body[jBody].dPinc*body[jBody].dPinc)\
          -2*body[iBody].dHecc*body[jBody].dPinc*body[jBody].dQinc)
          +sin(fdLambdaArg(body,system,iBody,jBody,iIndexJ,3))\
          *(body[iBody].dHecc*(body[jBody].dQinc*body[jBody].dQinc-body[jBody].dPinc*body[jBody].dPinc)\
          +2*body[iBody].dKecc*body[jBody].dPinc*body[jBody].dQinc));
}

double fdDdistDaDir310(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {        
  return AUCM/body[jBody].dSemi*system->dmLaplaceD[iIndexJ][system->imLaplaceN[iBody][jBody]][86]\
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
      + fdDdistDaDir12(body, system, iaBody[0], iaBody[1], iIndexJ)\
      + fdDdistDaDir13(body, system, iaBody[0], iaBody[1], iIndexJ)\
      + fdDdistDaDir14(body, system, iaBody[0], iaBody[1], iIndexJ)\
      + fdDdistDaDir15(body, system, iaBody[0], iaBody[1], iIndexJ)\
      + fdDdistDaDir16(body, system, iaBody[0], iaBody[1], iIndexJ)\
      + fdDdistDaDir17(body, system, iaBody[0], iaBody[1], iIndexJ)\
      + fdDdistDaDir18(body, system, iaBody[0], iaBody[1], iIndexJ)\
      + fdDdistDaDir19(body, system, iaBody[0], iaBody[1], iIndexJ)\
      + fdDdistDaDir110(body, system, iaBody[0], iaBody[1], iIndexJ)\
      + fdDdistDaDir111(body, system, iaBody[0], iaBody[1], iIndexJ)\
      + fdDdistDaDir112(body, system, iaBody[0], iaBody[1], iIndexJ)\
      + fdDdistDaDir113(body, system, iaBody[0], iaBody[1], iIndexJ)\
      + fdDdistDaDir114(body, system, iaBody[0], iaBody[1], iIndexJ) ) * dMfac/dSemiPrm;
    if (iIndexJ == 2) {
      y += (fdDdistDaExt13(body,system,iaBody[0],iaBody[1],iIndexJ)\
           +fdDdistDaExt111(body,system,iaBody[0],iaBody[1],iIndexJ) )*dMfac/dSemiPrm;
    } else if (iIndexJ == 3) {
      y += (fdDdistDaExt16(body,system,iaBody[0],iaBody[1],iIndexJ) )*dMfac/dSemiPrm;
    }
  } else if (iOrder == 2) {
    y = ( fdDdistDaDir21(body, system, iaBody[0], iaBody[1], iIndexJ) \
    + fdDdistDaDir22(body, system, iaBody[0], iaBody[1], iIndexJ)
    + fdDdistDaDir23(body, system, iaBody[0], iaBody[1], iIndexJ) \
    + fdDdistDaDir24(body, system, iaBody[0], iaBody[1], iIndexJ)
    + fdDdistDaDir25(body, system, iaBody[0], iaBody[1], iIndexJ) \
    + fdDdistDaDir26(body, system, iaBody[0], iaBody[1], iIndexJ) ) * dMfac/dSemiPrm;
    if (iIndexJ == 3) {
      y += (fdDdistDaExt25(body,system,iaBody[0],iaBody[1],iIndexJ) )*dMfac/dSemiPrm;
    }
  } else if (iOrder == 3) {
    y = ( fdDdistDaDir31(body, system, iaBody[0], iaBody[1], iIndexJ) \
    + fdDdistDaDir32(body, system, iaBody[0], iaBody[1], iIndexJ) \
    + fdDdistDaDir33(body, system, iaBody[0], iaBody[1], iIndexJ) \
    + fdDdistDaDir34(body, system, iaBody[0], iaBody[1], iIndexJ) \
    + fdDdistDaDir35(body, system, iaBody[0], iaBody[1], iIndexJ) \
    + fdDdistDaDir36(body, system, iaBody[0], iaBody[1], iIndexJ) \
    + fdDdistDaDir37(body, system, iaBody[0], iaBody[1], iIndexJ) \
    + fdDdistDaDir38(body, system, iaBody[0], iaBody[1], iIndexJ) \
    + fdDdistDaDir39(body, system, iaBody[0], iaBody[1], iIndexJ) \
    + fdDdistDaDir310(body, system, iaBody[0], iaBody[1], iIndexJ) ) * dMfac/dSemiPrm;
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
  return (system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][26]\
        +(body[iBody].dHecc*body[iBody].dHecc+body[iBody].dKecc*body[iBody].dKecc)*\
        system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][27]\
        +(body[jBody].dHecc*body[jBody].dHecc+body[jBody].dKecc*body[jBody].dKecc)*\
        system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][28]\
        +(body[iBody].dPinc*body[iBody].dPinc+body[iBody].dQinc*body[iBody].dQinc+
        body[jBody].dPinc*body[jBody].dPinc+body[jBody].dQinc*body[jBody].dQinc)*\
        system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][29])* \
          (body[iBody].dKecc*cos(fdLambdaArg(body,system,iBody,jBody,iIndexJ,1)) +\
          body[iBody].dHecc*sin(fdLambdaArg(body,system,iBody,jBody,iIndexJ,1)));
}

double fdDistDir12(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return (system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][30]
        +(body[iBody].dHecc*body[iBody].dHecc+body[iBody].dKecc*body[iBody].dKecc)*\
        system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][31]\
        +(body[jBody].dHecc*body[jBody].dHecc+body[jBody].dKecc*body[jBody].dKecc)*\
        system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][32]\
        +(body[iBody].dPinc*body[iBody].dPinc+body[iBody].dQinc*body[iBody].dQinc+
        body[jBody].dPinc*body[jBody].dPinc+body[jBody].dQinc*body[jBody].dQinc)*\
        system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][33])* \
          (body[jBody].dKecc*cos(fdLambdaArg(body,system,iBody,jBody,iIndexJ,1)) +\
          body[jBody].dHecc*sin(fdLambdaArg(body,system,iBody,jBody,iIndexJ,1)));
}

double fdDistDir13(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][34]*\
    ((body[jBody].dKecc*(body[iBody].dKecc*body[iBody].dKecc-body[iBody].dHecc*body[iBody].dHecc)\
    +2*body[jBody].dHecc*body[iBody].dHecc*body[iBody].dKecc)\
    *cos(fdLambdaArg(body,system,iBody,jBody,iIndexJ,1))-(body[jBody].dHecc*\
    (body[iBody].dKecc*body[iBody].dKecc-body[iBody].dHecc*body[iBody].dHecc)\
    -2*body[jBody].dKecc*body[iBody].dHecc*body[iBody].dKecc)\
    *sin(fdLambdaArg(body,system,iBody,jBody,iIndexJ,1)));
}

double fdDistDir14(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][35]*\
    ((body[iBody].dKecc*(body[jBody].dKecc*body[jBody].dKecc-body[jBody].dHecc*body[jBody].dHecc)\
    +2*body[iBody].dHecc*body[jBody].dHecc*body[jBody].dKecc)\
    *cos(fdLambdaArg(body,system,iBody,jBody,iIndexJ,1))-(body[iBody].dHecc*\
    (body[jBody].dKecc*body[jBody].dKecc-body[jBody].dHecc*body[jBody].dHecc)\
    -2*body[iBody].dKecc*body[jBody].dHecc*body[jBody].dKecc)\
    *sin(fdLambdaArg(body,system,iBody,jBody,iIndexJ,1)));
}

double fdDistDir15(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][36]*\
    ((body[iBody].dKecc*(body[iBody].dQinc*body[iBody].dQinc-body[iBody].dPinc*body[iBody].dPinc)\
    +2*body[iBody].dHecc*body[iBody].dPinc*body[iBody].dQinc)\
    *cos(fdLambdaArg(body,system,iBody,jBody,iIndexJ,1))-(body[iBody].dHecc*\
    (body[iBody].dQinc*body[iBody].dQinc-body[iBody].dPinc*body[iBody].dPinc)\
    -2*body[iBody].dKecc*body[iBody].dPinc*body[iBody].dQinc)\
    *sin(fdLambdaArg(body,system,iBody,jBody,iIndexJ,1)));
}

double fdDistDir16(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][37]*\
    ((body[jBody].dKecc*(body[iBody].dQinc*body[iBody].dQinc-body[iBody].dPinc*body[iBody].dPinc)\
    +2*body[jBody].dHecc*body[iBody].dPinc*body[iBody].dQinc)\
    *cos(fdLambdaArg(body,system,iBody,jBody,iIndexJ,1))-(body[jBody].dHecc*\
    (body[iBody].dQinc*body[iBody].dQinc-body[iBody].dPinc*body[iBody].dPinc)\
    -2*body[jBody].dKecc*body[iBody].dPinc*body[iBody].dQinc)\
    *sin(fdLambdaArg(body,system,iBody,jBody,iIndexJ,1)));
}

double fdDistDir17(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][38]*\
    ((body[iBody].dKecc*(body[iBody].dQinc*body[jBody].dQinc+body[iBody].dPinc*body[jBody].dPinc)\
    -body[iBody].dHecc*(body[iBody].dQinc*body[jBody].dPinc-body[iBody].dPinc*body[jBody].dQinc))\
    *cos(fdLambdaArg(body,system,iBody,jBody,iIndexJ,1))+(body[iBody].dHecc*\
    (body[iBody].dQinc*body[jBody].dQinc+body[iBody].dPinc*body[jBody].dPinc)\
    +body[iBody].dKecc*(body[iBody].dQinc*body[jBody].dPinc-body[iBody].dPinc*body[jBody].dQinc))\
    *sin(fdLambdaArg(body,system,iBody,jBody,iIndexJ,1)));
}

double fdDistDir18(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][39]*\
    ((body[iBody].dKecc*(body[iBody].dQinc*body[jBody].dQinc+body[iBody].dPinc*body[jBody].dPinc)\
    -body[iBody].dHecc*(body[jBody].dQinc*body[iBody].dPinc-body[jBody].dPinc*body[iBody].dQinc))\
    *cos(fdLambdaArg(body,system,iBody,jBody,iIndexJ,1))+(body[iBody].dHecc*\
    (body[iBody].dQinc*body[jBody].dQinc+body[iBody].dPinc*body[jBody].dPinc)\
    +body[iBody].dKecc*(body[jBody].dQinc*body[iBody].dPinc-body[jBody].dPinc*body[iBody].dQinc))\
    *sin(fdLambdaArg(body,system,iBody,jBody,iIndexJ,1)));
}

double fdDistDir19(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][40]*\
    ((body[iBody].dQinc*(body[jBody].dQinc*body[iBody].dKecc+body[jBody].dPinc*body[iBody].dHecc)\
    -body[iBody].dPinc*(body[jBody].dPinc*body[iBody].dKecc-body[jBody].dQinc*body[iBody].dHecc))\
    *cos(fdLambdaArg(body,system,iBody,jBody,iIndexJ,1))+(body[iBody].dPinc*\
    (body[jBody].dQinc*body[iBody].dKecc+body[jBody].dPinc*body[iBody].dHecc)\
    +body[iBody].dQinc*(body[jBody].dPinc*body[iBody].dKecc-body[jBody].dQinc*body[iBody].dHecc))\
    *sin(fdLambdaArg(body,system,iBody,jBody,iIndexJ,1)));
}

double fdDistDir110(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][41]*\
    ((body[jBody].dKecc*(body[iBody].dQinc*body[jBody].dQinc+body[iBody].dPinc*body[jBody].dPinc)\
    -body[jBody].dHecc*(body[iBody].dQinc*body[jBody].dPinc-body[iBody].dPinc*body[jBody].dQinc))\
    *cos(fdLambdaArg(body,system,iBody,jBody,iIndexJ,1))+(body[jBody].dHecc*\
    (body[iBody].dQinc*body[jBody].dQinc+body[iBody].dPinc*body[jBody].dPinc)\
    +body[jBody].dKecc*(body[iBody].dQinc*body[jBody].dPinc-body[iBody].dPinc*body[jBody].dQinc))\
    *sin(fdLambdaArg(body,system,iBody,jBody,iIndexJ,1)));
}

double fdDistDir111(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][42]*\
    ((body[jBody].dKecc*(body[iBody].dQinc*body[jBody].dQinc+body[iBody].dPinc*body[jBody].dPinc)\
    -body[jBody].dHecc*(body[jBody].dQinc*body[iBody].dPinc-body[jBody].dPinc*body[iBody].dQinc))\
    *cos(fdLambdaArg(body,system,iBody,jBody,iIndexJ,1))+(body[jBody].dHecc*\
    (body[iBody].dQinc*body[jBody].dQinc+body[iBody].dPinc*body[jBody].dPinc)\
    +body[jBody].dKecc*(body[jBody].dQinc*body[iBody].dPinc-body[jBody].dPinc*body[iBody].dQinc))\
    *sin(fdLambdaArg(body,system,iBody,jBody,iIndexJ,1)));
}

double fdDistDir112(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][43]*\
    ((body[iBody].dQinc*(body[jBody].dQinc*body[jBody].dKecc+body[jBody].dPinc*body[jBody].dHecc)\
    -body[iBody].dPinc*(body[jBody].dPinc*body[jBody].dKecc-body[jBody].dQinc*body[jBody].dHecc))\
    *cos(fdLambdaArg(body,system,iBody,jBody,iIndexJ,1))+(body[iBody].dPinc*\
    (body[jBody].dQinc*body[jBody].dKecc+body[jBody].dPinc*body[jBody].dHecc)\
    +body[iBody].dQinc*(body[jBody].dPinc*body[jBody].dKecc-body[jBody].dQinc*body[jBody].dHecc))\
    *sin(fdLambdaArg(body,system,iBody,jBody,iIndexJ,1)));
}

double fdDistDir113(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][36]*\
    ((body[iBody].dKecc*(body[jBody].dQinc*body[jBody].dQinc-body[jBody].dPinc*body[jBody].dPinc)\
    +2*body[iBody].dHecc*body[jBody].dPinc*body[jBody].dQinc)\
    *cos(fdLambdaArg(body,system,iBody,jBody,iIndexJ,1))-(body[iBody].dHecc*\
    (body[jBody].dQinc*body[jBody].dQinc-body[jBody].dPinc*body[jBody].dPinc)\
    -2*body[iBody].dKecc*body[jBody].dPinc*body[jBody].dQinc)\
    *sin(fdLambdaArg(body,system,iBody,jBody,iIndexJ,1)));
}

double fdDistDir114(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][37]*\
    ((body[jBody].dKecc*(body[jBody].dQinc*body[jBody].dQinc-body[jBody].dPinc*body[jBody].dPinc)\
    +2*body[jBody].dHecc*body[jBody].dPinc*body[jBody].dQinc)\
    *cos(fdLambdaArg(body,system,iBody,jBody,iIndexJ,1))-(body[jBody].dHecc*\
    (body[jBody].dQinc*body[jBody].dQinc-body[jBody].dPinc*body[jBody].dPinc)\
    -2*body[jBody].dKecc*body[jBody].dPinc*body[jBody].dQinc)\
    *sin(fdLambdaArg(body,system,iBody,jBody,iIndexJ,1)));
}

double fdDdistDaPrmDir11(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return -body[jBody].dSemi/body[iBody].dSemi*fdDdistDaDir11(body,system,jBody,iBody,iIndexJ)\
         -1./body[iBody].dSemi*fdDistDir11(body,system,jBody,iBody,iIndexJ)*AUCM;
}

double fdDdistDaPrmDir12(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return -body[jBody].dSemi/body[iBody].dSemi*fdDdistDaDir12(body,system,jBody,iBody,iIndexJ)\
         -1./body[iBody].dSemi*fdDistDir12(body,system,jBody,iBody,iIndexJ)*AUCM;
}

double fdDdistDaPrmDir13(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return -body[jBody].dSemi/body[iBody].dSemi*fdDdistDaDir13(body,system,jBody,iBody,iIndexJ)\
         -1./body[iBody].dSemi*fdDistDir13(body,system,jBody,iBody,iIndexJ)*AUCM;
}

double fdDdistDaPrmDir14(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return -body[jBody].dSemi/body[iBody].dSemi*fdDdistDaDir14(body,system,jBody,iBody,iIndexJ)\
         -1./body[iBody].dSemi*fdDistDir14(body,system,jBody,iBody,iIndexJ)*AUCM;
}

double fdDdistDaPrmDir15(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return -body[jBody].dSemi/body[iBody].dSemi*fdDdistDaDir15(body,system,jBody,iBody,iIndexJ)\
         -1./body[iBody].dSemi*fdDistDir15(body,system,jBody,iBody,iIndexJ)*AUCM;
}

double fdDdistDaPrmDir16(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return -body[jBody].dSemi/body[iBody].dSemi*fdDdistDaDir16(body,system,jBody,iBody,iIndexJ)\
         -1./body[iBody].dSemi*fdDistDir16(body,system,jBody,iBody,iIndexJ)*AUCM;
}

double fdDdistDaPrmDir17(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return -body[jBody].dSemi/body[iBody].dSemi*fdDdistDaDir17(body,system,jBody,iBody,iIndexJ)\
         -1./body[iBody].dSemi*fdDistDir17(body,system,jBody,iBody,iIndexJ)*AUCM;
}

double fdDdistDaPrmDir18(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return -body[jBody].dSemi/body[iBody].dSemi*fdDdistDaDir18(body,system,jBody,iBody,iIndexJ)\
         -1./body[iBody].dSemi*fdDistDir18(body,system,jBody,iBody,iIndexJ)*AUCM;
}

double fdDdistDaPrmDir19(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return -body[jBody].dSemi/body[iBody].dSemi*fdDdistDaDir19(body,system,jBody,iBody,iIndexJ)\
         -1./body[iBody].dSemi*fdDistDir19(body,system,jBody,iBody,iIndexJ)*AUCM;
}

double fdDdistDaPrmDir110(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return -body[jBody].dSemi/body[iBody].dSemi*fdDdistDaDir110(body,system,jBody,iBody,iIndexJ)\
         -1./body[iBody].dSemi*fdDistDir110(body,system,jBody,iBody,iIndexJ)*AUCM;
}

double fdDdistDaPrmDir111(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return -body[jBody].dSemi/body[iBody].dSemi*fdDdistDaDir111(body,system,jBody,iBody,iIndexJ)\
         -1./body[iBody].dSemi*fdDistDir111(body,system,jBody,iBody,iIndexJ)*AUCM;
}

double fdDdistDaPrmDir112(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return -body[jBody].dSemi/body[iBody].dSemi*fdDdistDaDir112(body,system,jBody,iBody,iIndexJ)\
         -1./body[iBody].dSemi*fdDistDir112(body,system,jBody,iBody,iIndexJ)*AUCM;
}

double fdDdistDaPrmDir113(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return -body[jBody].dSemi/body[iBody].dSemi*fdDdistDaDir113(body,system,jBody,iBody,iIndexJ)\
         -1./body[iBody].dSemi*fdDistDir113(body,system,jBody,iBody,iIndexJ)*AUCM;
}

double fdDdistDaPrmDir114(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return -body[jBody].dSemi/body[iBody].dSemi*fdDdistDaDir114(body,system,jBody,iBody,iIndexJ)\
         -1./body[iBody].dSemi*fdDistDir114(body,system,jBody,iBody,iIndexJ)*AUCM;
}

double fdDdistDaPrmInt13(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return 1./4*AUCM*body[iBody].dSemi/(body[jBody].dSemi*body[jBody].dSemi)*\
        (fdDdistDaExt13(body,system,jBody,iBody,iIndexJ));
}

double fdDdistDaPrmInt16(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return 4./9*AUCM*body[iBody].dSemi/(body[jBody].dSemi*body[jBody].dSemi)*\
        (fdDdistDaExt16(body,system,jBody,iBody,iIndexJ));
}

double fdDdistDaPrmInt111(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return 1./4*AUCM*body[iBody].dSemi/(body[jBody].dSemi*body[jBody].dSemi)*\
        (fdDdistDaExt111(body,system,jBody,iBody,iIndexJ));
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

double fdDdistDaPrmInt25(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return 1./9*AUCM*body[iBody].dSemi/(body[jBody].dSemi*body[jBody].dSemi)*\
        (fdDdistDaExt25(body,system,jBody,iBody,iIndexJ));
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
  return system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][85]\
          *(cos(fdLambdaArg(body,system,iBody,jBody,iIndexJ,3))\
          *(body[iBody].dKecc*(body[jBody].dQinc*body[jBody].dQinc-body[jBody].dPinc*body[jBody].dPinc)\
          -2*body[iBody].dHecc*body[jBody].dPinc*body[jBody].dQinc)
          +sin(fdLambdaArg(body,system,iBody,jBody,iIndexJ,3))\
          *(body[iBody].dHecc*(body[jBody].dQinc*body[jBody].dQinc-body[jBody].dPinc*body[jBody].dPinc)\
          +2*body[iBody].dKecc*body[jBody].dPinc*body[jBody].dQinc));
}

double fdDistDir310(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {        
  return system->dmLaplaceC[iIndexJ][system->imLaplaceN[iBody][jBody]][86]\
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
      + fdDdistDaPrmDir12(body, system, iaBody[0], iaBody[1], iIndexJ)\
      + fdDdistDaPrmDir13(body, system, iaBody[0], iaBody[1], iIndexJ)\
      + fdDdistDaPrmDir14(body, system, iaBody[0], iaBody[1], iIndexJ)\
      + fdDdistDaPrmDir15(body, system, iaBody[0], iaBody[1], iIndexJ)\
      + fdDdistDaPrmDir16(body, system, iaBody[0], iaBody[1], iIndexJ)\
      + fdDdistDaPrmDir17(body, system, iaBody[0], iaBody[1], iIndexJ)\
      + fdDdistDaPrmDir18(body, system, iaBody[0], iaBody[1], iIndexJ)\
      + fdDdistDaPrmDir19(body, system, iaBody[0], iaBody[1], iIndexJ)\
      + fdDdistDaPrmDir110(body, system, iaBody[0], iaBody[1], iIndexJ)\
      + fdDdistDaPrmDir111(body, system, iaBody[0], iaBody[1], iIndexJ)\
      + fdDdistDaPrmDir112(body, system, iaBody[0], iaBody[1], iIndexJ)\
      + fdDdistDaPrmDir113(body, system, iaBody[0], iaBody[1], iIndexJ)\
      + fdDdistDaPrmDir114(body, system, iaBody[0], iaBody[1], iIndexJ) ) * dMfac/dSemiPrm;
    if (iIndexJ == 2) {
      y += (fdDdistDaPrmInt13(body, system, iaBody[0], iaBody[1], iIndexJ)\
            +fdDdistDaPrmInt111(body, system, iaBody[0], iaBody[1], iIndexJ)) * dMfac;
    } else if (iIndexJ == 3) {
      y += (fdDdistDaPrmInt16(body, system, iaBody[0], iaBody[1], iIndexJ)) * dMfac;
    }
  } else if (iOrder == 2) {
    y = ( fdDdistDaPrmDir21(body, system, iaBody[0], iaBody[1], iIndexJ) \
    + fdDdistDaPrmDir22(body, system, iaBody[0], iaBody[1], iIndexJ)
    + fdDdistDaPrmDir23(body, system, iaBody[0], iaBody[1], iIndexJ) \
    + fdDdistDaPrmDir24(body, system, iaBody[0], iaBody[1], iIndexJ)
    + fdDdistDaPrmDir25(body, system, iaBody[0], iaBody[1], iIndexJ) \
    + fdDdistDaPrmDir26(body, system, iaBody[0], iaBody[1], iIndexJ) ) * dMfac/dSemiPrm;
    if (iIndexJ == 3) {
      y += (fdDdistDaPrmInt25(body, system, iaBody[0], iaBody[1], iIndexJ)) * dMfac;
    }
  } else if (iOrder == 3) {
    y = ( fdDdistDaPrmDir31(body, system, iaBody[0], iaBody[1], iIndexJ) \
    + fdDdistDaPrmDir32(body, system, iaBody[0], iaBody[1], iIndexJ) \
    + fdDdistDaPrmDir33(body, system, iaBody[0], iaBody[1], iIndexJ) \
    + fdDdistDaPrmDir34(body, system, iaBody[0], iaBody[1], iIndexJ) \
    + fdDdistDaPrmDir35(body, system, iaBody[0], iaBody[1], iIndexJ) \
    + fdDdistDaPrmDir36(body, system, iaBody[0], iaBody[1], iIndexJ) \
    + fdDdistDaPrmDir37(body, system, iaBody[0], iaBody[1], iIndexJ) \
    + fdDdistDaPrmDir38(body, system, iaBody[0], iaBody[1], iIndexJ) \
    + fdDdistDaPrmDir39(body, system, iaBody[0], iaBody[1], iIndexJ) \
    + fdDdistDaPrmDir310(body, system, iaBody[0], iaBody[1], iIndexJ) ) * dMfac/dSemiPrm;
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
        sum +=  -2.0*sqrt(body[iaBody[0]].dSemi/AUCM/dMu)\
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

