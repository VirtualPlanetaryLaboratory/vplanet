/********************** POISE.C **********************/
/*
 * Russell Deitrick, September 10, 2015
 *
 * Subroutines that control the energy balance model for climate 
*/

#include <stdio.h>
#include <math.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "vplanet.h"
#include "options.h"
#include "output.h"

void InitializeControlPoise(CONTROL *control) {
  /* Not sure if I need anything here yet */
}

void InitializeModulePoise(CONTROL *control,MODULE *module) {
  /* Anything here? */
}

void BodyCopyPoise(BODY *dest,BODY *src,int iTideModel,int iBody) {
  /* Not needed for annual model? */
}

void InitializeBodyPoise(BODY *body,CONTROL *control,UPDATE *update,int iBody,int iModule) {
}

void InitializeUpdateTmpBodyPoise(BODY *body,CONTROL *control,UPDATE *update,int iBody) {
}

/**************** POISE options ********************/

void ReadLatCellNum(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  int lTmp=-1,iTmp;
  AddOptionInt(files->Infile[iFile].cIn,options->cName,&iTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile,options->cName,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);
    /* Option was found */
    body[iFile-1].iNumLats = iTmp;
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else
    AssignDefaultInt(options,&body[iFile-1].iNumLats,files->iNumInputs);
}

void ReadPlanckA(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  /* This parameter cannot exist in primary file */
  int lTmp=-1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn,options->cName,&dTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile,options->cName,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);
    body[iFile-1].dPlanckA = dTmp;
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else
    if (iFile > 0)
      body[iFile-1].dPlanckA = options->dDefault;
}

void ReadPlanckB(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  /* This parameter cannot exist in primary file */
  int lTmp=-1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn,options->cName,&dTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile,options->cName,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);
    body[iFile-1].dPlanckB = dTmp;
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else
    if (iFile > 0)
      body[iFile-1].dPlanckB = options->dDefault;
}

void ReadDiffusion(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  /* This parameter cannot exist in primary file */
  int lTmp=-1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn,options->cName,&dTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile,options->cName,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);
    body[iFile-1].dDiffCoeff = dTmp;
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else
    if (iFile > 0)
      body[iFile-1].dDiffCoeff = options->dDefault;
}

void ReadFixIceLat(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  /* This parameter cannot exist in primary file */
  int lTmp=-1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn,options->cName,&dTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile,options->cName,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);
    body[iFile-1].dFixIceLat = dTmp;
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else
    if (iFile > 0)
      body[iFile-1].dFixIceLat = options->dDefault;
}

void ReadHeatCapAnn(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  /* This parameter cannot exist in primary file */
  int lTmp=-1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn,options->cName,&dTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile,options->cName,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);
    body[iFile-1].dHeatCapAnn = dTmp;
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else
    if (iFile > 0)
      body[iFile-1].dHeatCapAnn = options->dDefault;
}

void ReadHadley(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  int lTmp=-1,bTmp;
  AddOptionBool(files->Infile[iFile].cIn,options->cName,&bTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile,options->cName,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);
    /* Option was found */
    body[iFile-1].bHadley = bTmp;
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else
    AssignDefaultInt(options,&body[iFile-1].bHadley,files->iNumInputs);
}

void ReadColdStart(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  int lTmp=-1,bTmp;
  AddOptionBool(files->Infile[iFile].cIn,options->cName,&bTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile,options->cName,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);
    /* Option was found */
    body[iFile-1].bColdStart = bTmp;
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else
    AssignDefaultInt(options,&body[iFile-1].bColdStart,files->iNumInputs);
}

void ReadAlbedoZA(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  int lTmp=-1,bTmp;
  AddOptionBool(files->Infile[iFile].cIn,options->cName,&bTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile,options->cName,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);
    /* Option was found */
    body[iFile-1].bAlbedoZA = bTmp;
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else
    AssignDefaultInt(options,&body[iFile-1].bAlbedoZA,files->iNumInputs);
}

void ReadJormungand(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  /* This parameter cannot exist in primary file */
  int lTmp=-1, bTmp;

  AddOptionBool(files->Infile[iFile].cIn,options->cName,&bTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile,options->cName,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);
    body[iFile-1].bJormungand = bTmp;
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else
    AssignDefaultInt(options,&body[iFile-1].bAlbedoZA,files->iNumInputs);
}

// void ReadGridOutput(FILES *files,OPTIONS *options,OUTPUT *output,int iFile,int iVerbose) {
//   int i,j,count,iLen,iNumIndices=0,bNeg[MAXARRAY],ok=0;
//   int k,iOut,*lTmp;
//   char saTmp[MAXARRAY][OPTLEN],cTmp[OPTLEN],cOption[MAXARRAY][OPTLEN],cOut[OPTLEN];
//   int iLen1,iLen2;
// 
//   lTmp = malloc(MAXLINES*sizeof(int));
// 
//   AddOptionStringArray(files->Infile[iFile].cIn,options[OPT_GRIDOUTPUT].cName,saTmp,&iNumIndices,&files->Infile[iFile].iNumLines,lTmp,iVerbose);
//   output[OUT_GRIDOUTPUT].GridOutput = malloc(iNumIndices*sizeof(GRIDOUTPUT)); 
//   if (lTmp[0] >= 0) {
//     NotPrimaryInput(iFile,options[OPT_GRIDOUTPUT].cName,files->Infile[iFile].cIn,lTmp[0],iVerbose);
// 
//     /* First remove and record negative signs */
//     for (i=0;i<iNumIndices;i++) {
//       if (saTmp[i][0] == 45) {
//         /* Option is negative */
//         bNeg[i] = 1;
//         /* Now remove negative sign */
//         for (j=0;j<strlen(saTmp[i]);j++) 
//         saTmp[i][j] = saTmp[i][j+1];
//         saTmp[i][strlen(saTmp[i])] = 0;
//       } else
//         bNeg[i] = 0;
//     }
//     
//     /* Check for ambiguity */
//     for (i=0;i<iNumIndices;i++) {
//       count=0; /* Number of possibilities */
//       for (j=0;j<OPTLEN;j++)
//         cTmp[j]=0;
//       strcpy(cTmp,saTmp[i]);
//       for (j=GROUTSTARTPOISE;j<GROUTENDPOISE;j++) {
//         for (k=0;k<OPTLEN;k++)
//           cOut[k]=0;
//         strcpy(cOut,output[OUT_GRIDOUTPUT].GridOutput[j].cName);
//         iLen1=strlen(cOut);
//         iLen2=strlen(cTmp);
//         /* Check for perfect match */
//         if ( (iLen1 == iLen2) && (memcmp(sLower(cTmp),sLower(cOut),strlen(cOut)) == 0)) {
//           /* Output option found! */
//           strcpy(cOption[count],output[j].cName);
//           count = 1;
//           iOut = j;
//           j = NUMOUT; /* Poor man's break! */
//         } else {
//           if (iLen1 < iLen2)
//             iLen=iLen1;
//           else
//             iLen=iLen2;
//           
//           if (memcmp(sLower(cTmp),sLower(cOut),iLen) == 0 && iLen1 > iLen2) {
//             /* Output option found! */
//             strcpy(cOption[count],output[j].cName);
//             count++;
//             iOut = j;
//           }
//         }
//       }
//       
//       if (count > 1) {
//         /* More than one possibility */
//         if (iVerbose >= VERBERR) {
//           fprintf(stderr,"ERROR: Output option \"%s\" is ambiguous. Options are ",saTmp[i]);
//           for (j=0;j<count;j++) {
//             fprintf(stderr,"%s",cOption[j]);
//             if (j < count-1)
//               fprintf(stderr,", ");
//           }
//           fprintf(stderr,".\n");
//         }
//         LineExit(files->Infile[iFile].cIn,lTmp[0]);
//       }
//       
//       if (!count) {
//         /* Option not found */
//         if (iVerbose >= VERBERR) 
//           fprintf(stderr,"ERROR: Unknown output option \"%s\".\n",saTmp[i]);
//         LineExit(files->Infile[iFile].cIn,lTmp[0]);
//       }
//       
//       if (count == 1) {
//         /* Unique option */
//     
//     /* Verify and record negative options */
//     if (bNeg[i]) {
//       // Is the negative option allowed?
//       if (!output[iOut].bNeg) { /* No */
//         if (iVerbose >= VERBERR) {
//           fprintf(stderr,"ERROR: Output option %s ",saTmp[i]);
//           if (strlen(saTmp[i]) < strlen(output[iOut].cName))
//             fprintf(stderr,"(= %s) ",output[iOut].cName);
//           fprintf(stderr,"cannot be negative.\n");
//         }
//         LineExit(files->Infile[iFile].cIn,lTmp[0]);
//       } else { // Yes, initialize bDoNeg to true
//         output[iOut].bDoNeg[iFile-1] = 1;
//       }
//     } else { // Negative option not set, initialize bDoNeg to false
//         output[iOut].bDoNeg[iFile-1] = 0;
//     }   
//         files->Outfile[iFile-1].caCol[i][0]='\0';
//         strcpy(files->Outfile[iFile-1].caCol[i],output[iOut].cName);
//       }
//     }
//     files->Outfile[iFile-1].iNumCols = iNumIndices;
//     UpdateFoundOptionMulti(&files->Infile[iFile],&options[OPT_OUTPUTORDER],lTmp,files->Infile[iFile].iNumLines,iFile);
//   }
//   free(lTmp);
// }

void InitializeOptionsPoise(OPTIONS *options,fnReadOption fnRead[]) {
  sprintf(options[OPT_LATCELLNUM].cName,"iLatCellNum");
  sprintf(options[OPT_LATCELLNUM].cDescr,"Number of latitude cells used in climate model");
  sprintf(options[OPT_LATCELLNUM].cDefault,"50");
  options[OPT_LATCELLNUM].dDefault = 50;
  options[OPT_LATCELLNUM].iType = 1;  
  options[OPT_LATCELLNUM].iMultiFile = 1;   
  fnRead[OPT_LATCELLNUM] = &ReadLatCellNum;
  
  sprintf(options[OPT_PLANCKA].cName,"dPlanckA");
  sprintf(options[OPT_PLANCKA].cDescr,"Constant 'A' used in OLR calculation");
  sprintf(options[OPT_PLANCKA].cDefault,"203.3");
  options[OPT_PLANCKA].dDefault = 203.3;
  options[OPT_PLANCKA].iType = 2;  
  options[OPT_PLANCKA].iMultiFile = 1;   
  fnRead[OPT_PLANCKA] = &ReadPlanckA;
  
  sprintf(options[OPT_PLANCKB].cName,"dPlanckB");
  sprintf(options[OPT_PLANCKB].cDescr,"Sensitivity 'B' used in OLR calculation");
  sprintf(options[OPT_PLANCKB].cDefault,"2.09");
  options[OPT_PLANCKB].dDefault = 2.09;
  options[OPT_PLANCKB].iType = 2;  
  options[OPT_PLANCKB].iMultiFile = 1;   
  fnRead[OPT_PLANCKB] = &ReadPlanckB;
  
  sprintf(options[OPT_DIFFUSION].cName,"dDiffusion");
  sprintf(options[OPT_DIFFUSION].cDescr,"Heat diffusion coefficient");
  sprintf(options[OPT_DIFFUSION].cDefault,"0.44");
  options[OPT_DIFFUSION].dDefault = 0.44;
  options[OPT_DIFFUSION].iType = 2;  
  options[OPT_DIFFUSION].iMultiFile = 1;   
  fnRead[OPT_DIFFUSION] = &ReadDiffusion;
  
  sprintf(options[OPT_HADLEY].cName,"bHadley");
  sprintf(options[OPT_HADLEY].cDescr,"Enable Hadley circulation");
  sprintf(options[OPT_HADLEY].cDefault,"1");
  options[OPT_HADLEY].dDefault = 1;
  options[OPT_HADLEY].iType = 0;  
  options[OPT_HADLEY].iMultiFile = 1;   
  fnRead[OPT_HADLEY] = &ReadHadley;
  
  sprintf(options[OPT_COLDSTART].cName,"bColdStart");
  sprintf(options[OPT_COLDSTART].cDescr,"Start from snowball Earth conditions");
  sprintf(options[OPT_COLDSTART].cDefault,"0");
  options[OPT_COLDSTART].dDefault = 0;
  options[OPT_COLDSTART].iType = 0;  
  options[OPT_COLDSTART].iMultiFile = 1;   
  fnRead[OPT_COLDSTART] = &ReadColdStart;
  
  sprintf(options[OPT_FIXICELAT].cName,"dFixIceLat");
  sprintf(options[OPT_FIXICELAT].cDescr,"Fixes ice line latitude to a set value");
  sprintf(options[OPT_FIXICELAT].cDefault,"None");
  options[OPT_FIXICELAT].dDefault = 0;
  options[OPT_FIXICELAT].iType = 2;  
  options[OPT_FIXICELAT].iMultiFile = 1;   
  fnRead[OPT_FIXICELAT] = &ReadFixIceLat;
  
  sprintf(options[OPT_ALBEDOZA].cName,"bAlbedoZA");
  sprintf(options[OPT_ALBEDOZA].cDescr,"Use albedo based on zenith angle");
  sprintf(options[OPT_ALBEDOZA].cDefault,"0");
  options[OPT_ALBEDOZA].dDefault = 0;
  options[OPT_ALBEDOZA].iType = 0;  
  options[OPT_ALBEDOZA].iMultiFile = 1;   
  fnRead[OPT_ALBEDOZA] = &ReadAlbedoZA;
  
  sprintf(options[OPT_JORMUNGAND].cName,"bJormungand");
  sprintf(options[OPT_JORMUNGAND].cDescr,"With dFixIceLat, fixes ice latitude with cold equator");
  sprintf(options[OPT_ALBEDOZA].cDefault,"0");
  options[OPT_ALBEDOZA].dDefault = 0;
  options[OPT_ALBEDOZA].iType = 0;  
  options[OPT_ALBEDOZA].iMultiFile = 1;   
  fnRead[OPT_ALBEDOZA] = &ReadJormungand;
  
  sprintf(options[OPT_HEATCAPANN].cName,"dHeatCapAnn");
  sprintf(options[OPT_HEATCAPANN].cDescr,"Surface heat capacity in annual model");
  sprintf(options[OPT_HEATCAPANN].cDefault,"0.2");
  options[OPT_HEATCAPANN].dDefault = 0.2;
  options[OPT_HEATCAPANN].iType = 2;  
  options[OPT_HEATCAPANN].iMultiFile = 1;   
  fnRead[OPT_HEATCAPANN] = &ReadHeatCapAnn;
  
//   sprintf(options[OPT_GRIDOUTPUT].cName,"saGridOutput");
//   sprintf(options[OPT_GRIDOUTPUT].cDescr,"Outputs latitudinal params in special files");
//   sprintf(options[OPT_GRIDOUTPUT].cDefault,"None");
//   options[OPT_GRIDOUTPUT].iType = 14;
//   options[OPT_GRIDOUTPUT].iMultiFile = 1;
   
}

void ReadOptionsPoise(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,fnReadOption fnRead[],int iBody) {
  int iOpt;

  for (iOpt=OPTSTARTPOISE;iOpt<OPTENDPOISE;iOpt++) { 
      if (options[iOpt].iType != -1) {
        fnRead[iOpt](body,control,files,&options[iOpt],system,iBody+1);
      }
  }
//   ReadGridOutput(files,options,output,iBody+1,control->Io.iVerbose);
}
    

/******************* Verify POISE ******************/
void VerifyAlbedo(BODY *body, OPTIONS *options, char cFile[], int iBody, int iVerbose) {
  /* If all of bColdstart, dFixIceLat, and bAlbedoZA are set, exit */
  if (options[OPT_COLDSTART].iLine[iBody+1] > -1 && options[OPT_FIXICELAT].iLine[iBody+1] > -1 && options[OPT_ALBEDOZA].iLine[iBody+1] > -1) {
    if (iVerbose >= VERBERR) 
      fprintf(stderr,"ERROR: Only one of %s, %s, and %s can be set in File: %s\n", options[OPT_COLDSTART].cName, options[OPT_FIXICELAT].cName, options[OPT_ALBEDOZA].cName, cFile);
    exit(EXIT_INPUT);
  }
    
  /* Any two of bColdstart, dFixIceLat, and bAlbedoZA are set, exit */ 
  if (options[OPT_COLDSTART].iLine[iBody+1] > -1 && options[OPT_FIXICELAT].iLine[iBody+1] > -1 && options[OPT_ALBEDOZA].iLine[iBody+1] == -1) {
    if (iVerbose >= VERBERR) 
      fprintf(stderr,"ERROR: Only one of %s and %s can be set in File: %s\n", options[OPT_COLDSTART].cName, options[OPT_FIXICELAT].cName, cFile);
    exit(EXIT_INPUT);
  }

  if (options[OPT_COLDSTART].iLine[iBody+1] > -1 && options[OPT_FIXICELAT].iLine[iBody+1] == -1 && options[OPT_ALBEDOZA].iLine[iBody+1] > -1) {
    if (iVerbose >= VERBERR) 
      fprintf(stderr,"ERROR: Only one of %s and %s can be set in File: %s\n", options[OPT_COLDSTART].cName, options[OPT_ALBEDOZA].cName, cFile);
    exit(EXIT_INPUT);
  } 
  
  if (options[OPT_COLDSTART].iLine[iBody+1] == -1 && options[OPT_FIXICELAT].iLine[iBody+1] > -1 && options[OPT_ALBEDOZA].iLine[iBody+1] > -1) {
    if (iVerbose >= VERBERR) 
      fprintf(stderr,"ERROR: Only one of %s and %s can be set in File: %s\n", options[OPT_FIXICELAT].cName, options[OPT_ALBEDOZA].cName, cFile);
    exit(EXIT_INPUT);
  } 
  
  /* If bJormungand is set, is dFixIceLat also set? If not, exit */
  if (options[OPT_JORMUNGAND].iLine[iBody+1] > -1 && options[OPT_FIXICELAT].iLine[iBody+1] == -1) {
    if (iVerbose >= VERBERR) 
      fprintf(stderr,"ERROR: If %s is set, %s must also be set in File: %s\n", options[OPT_JORMUNGAND].cName, options[OPT_FIXICELAT].cName, cFile);
    exit(EXIT_INPUT);
  }
}

void InitializeLatGrid(BODY *body, int iBody) {
  double delta_x, SinLat;
  int i;
  delta_x = 2.0/body[iBody].iNumLats;
  
  body[iBody].daLats = malloc(body[iBody].iNumLats*sizeof(double));
  
  for (i=0;i<body[iBody].iNumLats;i++) {
    SinLat = (-1.0 + delta_x/2.) + i*delta_x; 
    body[iBody].daLats[i] = asin(SinLat);  
  }
}
 
void InitializeClimateParams(BODY *body, int iBody) {
  int i;
  double Toffset, xboundary;
  
  body[iBody].daTemp = malloc(body[iBody].iNumLats*sizeof(double));   
  body[iBody].daDiffusion = malloc((body[iBody].iNumLats+1)*sizeof(double));
  body[iBody].daAlbedo = malloc(body[iBody].iNumLats*sizeof(double));
  body[iBody].daInsol = malloc(body[iBody].iNumLats*sizeof(double*));
  body[iBody].daAnnualInsol = malloc(body[iBody].iNumLats*sizeof(double));
  body[iBody].daTGrad = malloc(body[iBody].iNumLats*sizeof(double)); 
  body[iBody].daFlux = malloc(body[iBody].iNumLats*sizeof(double));  
  body[iBody].daFluxIn = malloc(body[iBody].iNumLats*sizeof(double)); 
  body[iBody].daFluxOut = malloc(body[iBody].iNumLats*sizeof(double)); 
  body[iBody].daDivFlux = malloc(body[iBody].iNumLats*sizeof(double));  
    
  body[iBody].iNDays = (int)floor(body[iBody].dRotRate/body[iBody].dMeanMotion); //number of days in year
  
  if (body[iBody].bColdStart) {
    Toffset = -40.0;
  } else {
    Toffset = 0.0;
  }
  
  body[iBody].dTGlobal = 0.0;
  body[iBody].dAlbedoGlobal = 0.0;
  body[iBody].daDiffusion[0] = body[iBody].dDiffCoeff;
  for (i=0;i<=body[iBody].iNumLats;i++) {
    if (i!=body[iBody].iNumLats)
      body[iBody].daTemp[i] = 20.*(1.0-2*pow(sin(body[iBody].daLats[i]),2))+Toffset;
      body[iBody].dTGlobal += body[iBody].daTemp[i]/body[iBody].iNumLats;
      body[iBody].daInsol[i] = malloc(body[iBody].iNDays*sizeof(double));
      
    body[iBody].daDiffusion[i] = body[iBody].dDiffCoeff;   
    if (body[iBody].bHadley) {
      // XXX not self-consistent with rotation rate!
      xboundary = -1.0 + i*2.0/body[iBody].iNumLats;
      body[iBody].daDiffusion[i] += body[iBody].dDiffCoeff*9.*exp(-pow((xboundary/sin(25.*DEGRAD)),6));
    }
  }
  
  Albedo(body,iBody);
  for (i=0;i<body[iBody].iNumLats;i++) {
    body[iBody].dAlbedoGlobal += body[iBody].daAlbedo[i];
  }
  body[iBody].dAlbedoGlobal /= body[iBody].iNumLats;
  
  AnnualInsolation(body, iBody);

} 
      
void VerifyPoise(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,OUTPUT *output,SYSTEM *system,UPDATE *update,fnUpdateVariable ***fnUpdate,int iBody,int iModule) {
  int i, j=0, iPert=0, jBody=0;
  
  VerifyAlbedo(body,options,files->Infile[iBody+1].cIn,iBody,control->Io.iVerbose);
  
  /* Initialize climate arrays */
  InitializeLatGrid(body, iBody);
  InitializeClimateParams(body, iBody);
  
  /* POISE annual has no primary variables! 
     The climate simulation is done in entirely in ForceBehavior. */  
  control->Evolve.fnPropsAux[iBody][iModule] = &PropertiesPoise;

  control->fnForceBehavior[iBody][iModule]=&ForceBehaviorPoise;
  control->Evolve.fnBodyCopy[iBody][iModule]=&BodyCopyPoise;
}


/***************** POISE Update *****************/
void InitializeUpdatePoise(BODY *body,UPDATE *update,int iBody) {

} 

/***************** POISE Halts *****************/

void CountHaltsPoise(HALT *halt,int *iNumHalts) { 
}

void VerifyHaltPoise(BODY *body,CONTROL *control,OPTIONS *options,int iBody,int *iHalt) {
}


/************* POISE Outputs ******************/

void WriteTGlobal(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  /* Get TGlobal */
  *dTmp = body[iBody].dTGlobal;
  if (output->bDoNeg[iBody]) {
    /* Units already in Celsius (POISE uses Celsius) */
    strcpy(cUnit,output->cNeg);
  } else { 
    *dTmp = fdUnitsTemp(*dTmp, 1, 0);
    fsUnitsTime(0,cUnit);
  }
}

void WriteAlbedoGlobal(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  /* Get AlbedoGlobal */
  *dTmp = body[iBody].dAlbedoGlobal;
}

void WriteTempLat(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  *dTmp = body[iBody].daTemp[body[iBody].iWriteLat];
  
  if (output->bDoNeg[iBody]) {
    /* Units already in Celsius (POISE uses Celsius) */
    strcpy(cUnit,output->cNeg);
  } else { 
    *dTmp = fdUnitsTemp(*dTmp, 1, 0);
    fsUnitsTime(0,cUnit);
  }
}

void WriteLatitude(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  *dTmp = body[iBody].daLats[body[iBody].iWriteLat];
  
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else {
    *dTmp /= fdUnitsAngle(units->iAngle);
    fsUnitsAngle(units->iAngle,cUnit);
  }
}

void WriteAlbedoLat(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  *dTmp = body[iBody].daAlbedo[body[iBody].iWriteLat];
}
  
void WriteFluxInGlobal(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  *dTmp = body[iBody].dFluxInGlobal;

  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else {
  }
}  
  
void WriteFluxOutGlobal(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  *dTmp = body[iBody].dFluxOutGlobal;

  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else {
  }
}    
  
void WriteAnnualInsol(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  *dTmp = body[iBody].daAnnualInsol[body[iBody].iWriteLat];

  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else {
  }
}
  
void WriteDailyInsol(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  char cOut[NAMELEN];
  FILE *fp;
  int iLat,iDay;
   
  sprintf(cOut,"%s.%s.DailyInsolInit",system->cName,body[iBody].cName);

  fp = fopen(cOut,"w");
  for (iDay=0;iDay<body[iBody].iNDays;iDay++) {
    for (iLat=0;iLat<body[iBody].iNumLats;iLat++) {
      fprintd(fp,body[iBody].daInsol[iLat][iDay],control->Io.iSciNot,control->Io.iDigits);
      fprintf(fp," ");
    }
    fprintf(fp,"\n");
  }
}
  
void WriteFluxMerid(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  *dTmp = body[iBody].daFlux[body[iBody].iWriteLat];
  
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else {
  }
}
  
void WriteFluxIn(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  *dTmp = body[iBody].daFluxIn[body[iBody].iWriteLat];
  
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else {
  }
}

void WriteFluxOut(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  *dTmp = body[iBody].daFluxOut[body[iBody].iWriteLat];
  
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else {
  }
}
  
void WriteDivFlux(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  *dTmp = body[iBody].daDivFlux[body[iBody].iWriteLat];
  
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else {
  }
}       
  
void InitializeOutputPoise(OUTPUT *output,fnWriteOutput fnWrite[]) {
  sprintf(output[OUT_TGLOBAL].cName,"TGlobal");
  sprintf(output[OUT_TGLOBAL].cDescr,"Global mean temperature from POISE");
  sprintf(output[OUT_TGLOBAL].cNeg,"C");
  output[OUT_TGLOBAL].bNeg = 1;
  output[OUT_TGLOBAL].dNeg = 1; //conversion is hardcoded in write function
  output[OUT_TGLOBAL].iNum = 1;
  fnWrite[OUT_TGLOBAL] = &WriteTGlobal;
  
  sprintf(output[OUT_ALBEDOGLOBAL].cName,"AlbedoGlobal");
  sprintf(output[OUT_ALBEDOGLOBAL].cDescr,"Global mean bond albedo from POISE");
  output[OUT_ALBEDOGLOBAL].bNeg = 0;
  output[OUT_ALBEDOGLOBAL].iNum = 1;
  fnWrite[OUT_ALBEDOGLOBAL] = &WriteAlbedoGlobal;
  
  sprintf(output[OUT_FLUXINGLOBAL].cName,"FluxInGlobal");
  sprintf(output[OUT_FLUXINGLOBAL].cDescr,"Global mean flux in (insol*(1-albedo)) from POISE");
  sprintf(output[OUT_FLUXINGLOBAL].cNeg,"pirate-ninjas/m^2");
  output[OUT_FLUXINGLOBAL].bNeg = 1;
  output[OUT_FLUXINGLOBAL].dNeg = 1/40.55185;
  output[OUT_FLUXINGLOBAL].iNum = 1;
  fnWrite[OUT_FLUXINGLOBAL] = &WriteFluxInGlobal;
  
  sprintf(output[OUT_FLUXOUTGLOBAL].cName,"FluxOutGlobal");
  sprintf(output[OUT_FLUXOUTGLOBAL].cDescr,"Global mean flux out from POISE");
  sprintf(output[OUT_FLUXOUTGLOBAL].cNeg,"pirate-ninjas/m^2");
  output[OUT_FLUXOUTGLOBAL].bNeg = 1;
  output[OUT_FLUXOUTGLOBAL].dNeg = 1/40.55185;
  output[OUT_FLUXOUTGLOBAL].iNum = 1;
  fnWrite[OUT_FLUXOUTGLOBAL] = &WriteFluxOutGlobal;
  
  sprintf(output[OUT_TEMPLAT].cName,"TempLat");
  sprintf(output[OUT_TEMPLAT].cDescr,"Surface temperature by latitude.");
  sprintf(output[OUT_TEMPLAT].cNeg,"C");
  output[OUT_TEMPLAT].bNeg = 1;
  output[OUT_TEMPLAT].dNeg = 1; //conversion is hardcoded in write function
  output[OUT_TEMPLAT].iNum = 1;
  output[OUT_TEMPLAT].bGrid = 1;
  fnWrite[OUT_TEMPLAT] = &WriteTempLat; 
  
  sprintf(output[OUT_LATITUDE].cName,"Latitude");
  sprintf(output[OUT_LATITUDE].cDescr,"Latitude.");
  sprintf(output[OUT_LATITUDE].cNeg,"Degrees");
  output[OUT_LATITUDE].bNeg = 1;
  output[OUT_LATITUDE].dNeg = 1/DEGRAD; 
  output[OUT_LATITUDE].iNum = 1;
  output[OUT_LATITUDE].bGrid = 1;
  fnWrite[OUT_LATITUDE] = &WriteLatitude; 

  sprintf(output[OUT_ALBEDOLAT].cName,"AlbedoLat");
  sprintf(output[OUT_ALBEDOLAT].cDescr,"Surface albedo by latitude.");
  output[OUT_ALBEDOLAT].bNeg = 0;
  output[OUT_ALBEDOLAT].iNum = 1;
  output[OUT_ALBEDOLAT].bGrid = 1;
  fnWrite[OUT_ALBEDOLAT] = &WriteAlbedoLat; 
  
  sprintf(output[OUT_ANNUALINSOL].cName,"AnnInsol");
  sprintf(output[OUT_ANNUALINSOL].cDescr,"Annual insolation by latitude.");
  sprintf(output[OUT_ANNUALINSOL].cNeg,"pirate-ninjas/m^2");
  output[OUT_ANNUALINSOL].bNeg = 1;
  output[OUT_ANNUALINSOL].dNeg = 1/40.55185;
  output[OUT_ANNUALINSOL].iNum = 1;
  output[OUT_ANNUALINSOL].bGrid = 1;
  fnWrite[OUT_ANNUALINSOL] = &WriteAnnualInsol; 
  
  sprintf(output[OUT_FLUXMERID].cName,"FluxMerid");
  sprintf(output[OUT_FLUXMERID].cDescr,"Meridional flux by latitude");
  sprintf(output[OUT_FLUXMERID].cNeg,"PW");
  output[OUT_FLUXMERID].bNeg = 1;
  output[OUT_FLUXMERID].dNeg = 1e-15;
  output[OUT_FLUXMERID].iNum = 1;
  output[OUT_FLUXMERID].bGrid = 1;
  fnWrite[OUT_FLUXMERID] = &WriteFluxMerid;  
  
  sprintf(output[OUT_FLUXIN].cName,"FluxIn");
  sprintf(output[OUT_FLUXIN].cDescr,"Incoming flux by latitude");
  sprintf(output[OUT_FLUXIN].cNeg,"pirate-ninjas/m^2");
  output[OUT_FLUXIN].bNeg = 1;
  output[OUT_FLUXIN].dNeg = 1/40.55185;
  output[OUT_FLUXIN].iNum = 1;
  output[OUT_FLUXMERID].bGrid = 1;
  fnWrite[OUT_FLUXIN] = &WriteFluxIn; 
  
  sprintf(output[OUT_FLUXOUT].cName,"FluxOut");
  sprintf(output[OUT_FLUXOUT].cDescr,"Outgoing flux by latitude");
  sprintf(output[OUT_ANNUALINSOL].cNeg,"pirate-ninjas/m^2");
  output[OUT_ANNUALINSOL].bNeg = 1;
  output[OUT_ANNUALINSOL].dNeg = 1/40.55185;
  output[OUT_FLUXOUT].iNum = 1;
  output[OUT_FLUXOUT].bGrid = 1;
  fnWrite[OUT_FLUXOUT] = &WriteFluxOut; 
  
  sprintf(output[OUT_DIVFLUX].cName,"DivFlux");
  sprintf(output[OUT_DIVFLUX].cDescr,"Divergence of flux by latitude");
  sprintf(output[OUT_ANNUALINSOL].cNeg,"pirate-ninjas/m^2");
  output[OUT_ANNUALINSOL].bNeg = 1;
  output[OUT_ANNUALINSOL].dNeg = 1/40.55185;
  output[OUT_DIVFLUX].iNum = 1;
  output[OUT_DIVFLUX].bGrid = 1;
  fnWrite[OUT_DIVFLUX] = &WriteDivFlux; 
  
}

void FinalizeOutputFunctionPoise(OUTPUT *output,int iBody,int iModule) {
  
}


/************ POISE Logging Functions **************/

void LogOptionsPoise(CONTROL *control, FILE *fp) {

  fprintf(fp,"-------- POISE Options -----\n\n");
  
}

void LogPoise(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UPDATE *update,fnWriteOutput fnWrite[],FILE *fp) {
  int iOut;

  fprintf(fp,"\n----- POISE PARAMETERS ------\n");
  for (iOut=OUTSTARTPOISE;iOut<OUTBODYSTARTPOISE;iOut++) {
    if (output[iOut].iNum > 0)
      WriteLogEntry(body,control,&output[iOut],system,update,fnWrite[iOut],fp,0);
  }
}

void LogBodyPoise(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UPDATE *update,fnWriteOutput fnWrite[],FILE *fp,int iBody) {
  int iOut;

  fprintf(fp,"-----POISE PARAMETERS (%s)------\n",body[iBody].cName);
  for (iOut=OUTBODYSTARTPOISE;iOut<OUTENDPOISE;iOut++) {
    if (output[iOut].iNum > 0) 
      WriteLogEntry(body,control,&output[iOut],system,update,fnWrite[iOut],fp,iBody);
  }
}

/************* MODULE Functions ***********/

void AddModulePoise(MODULE *module,int iBody,int iModule) {

  module->iaModule[iBody][iModule] = POISE;

  module->fnInitializeControl[iBody][iModule] = &InitializeControlPoise;
  module->fnInitializeUpdateTmpBody[iBody][iModule] = &InitializeUpdateTmpBodyPoise;
  module->fnCountHalts[iBody][iModule] = &CountHaltsPoise;
  module->fnLogBody[iBody][iModule] = &LogBodyPoise;

  module->fnReadOptions[iBody][iModule] = &ReadOptionsPoise;
  module->fnVerify[iBody][iModule] = &VerifyPoise;
  module->fnVerifyHalt[iBody][iModule] = &VerifyHaltPoise;

  module->fnInitializeBody[iBody][iModule] = &InitializeBodyPoise;
  module->fnInitializeUpdate[iBody][iModule] = &InitializeUpdatePoise;
  module->fnInitializeOutput[iBody][iModule] = &InitializeOutputPoise;

  module->fnFinalizeOutputFunction[iBody][iModule] = &FinalizeOutputFunctionPoise;

}

/************* POISE Functions ***********/

void PropertiesPoise(BODY *body,UPDATE *update,int iBody) {  
}

void ForceBehaviorPoise(BODY *body,EVOLVE *evolve,IO *io,SYSTEM *system,int iBody,int iModule) {
  PoiseClimate(body,iBody);
}

double true2eccA(double TrueA, double Ecc) {
  double cosE;
  cosE = (cos(TrueA) + Ecc) / (1.0 + Ecc*cos(TrueA));
  
  if (TrueA < PI) {
    return acos(cosE);
  } else {
    return (2*PI - acos(cosE));
  }
} 

void DailyInsolation(BODY *body, int iBody, int iDay) {
  int j;
  double Sconst, sin_delta, cos_delta, tan_delta, delta, HA;
  
  Sconst = body[0].dLuminosity / (4.*PI*pow(body[iBody].dSemi,2));
    
  sin_delta = sin(body[iBody].dObliquity)*sin(body[iBody].dTrueL);
  cos_delta = sqrt(1.0-pow(sin_delta,2));
  tan_delta = sin_delta/cos_delta;
  delta = asin(sin_delta);
  
  for (j=0;j<body[iBody].iNumLats;j++) {
    if (delta > 0.0) {
      /* Northern summer */
      if (body[iBody].daLats[j] >= (PI/2.-delta)) { 
        /* white night/no sunset */
        body[iBody].daInsol[j][iDay] = Sconst*sin(body[iBody].daLats[j])*sin_delta/pow(body[iBody].dAstroDist,2);
      } else if (-body[iBody].daLats[j] >= (PI/2.-delta) && body[iBody].daLats[j] < 0.0) {
        /* polar darkness/no sunrise */
        body[iBody].daInsol[j][iDay] = 0.0;
      } else {
        /* regular day/night cycle */
        HA = acos(-tan(body[iBody].daLats[j])*tan_delta);
        body[iBody].daInsol[j][iDay] = Sconst * (HA*sin(body[iBody].daLats[j])*sin_delta + \
            cos(body[iBody].daLats[j])*cos_delta*sin(HA)) / (PI*pow(body[iBody].dAstroDist,2));
      }
    } else {
      /* Southern summer */
      if (-body[iBody].daLats[j] >= (PI/2.+delta) && body[iBody].daLats[j] < 0.0) { 
        /* white night/no sunset */
        body[iBody].daInsol[j][iDay] = Sconst*sin(body[iBody].daLats[j])*sin_delta/pow(body[iBody].dAstroDist,2);
      } else if (body[iBody].daLats[j] >= (PI/2.+delta)) {
        /* polar darkness/no sunrise */
        body[iBody].daInsol[j][iDay] = 0.0;
      } else {
        /* regular day/night cycle */
        HA = acos(-tan(body[iBody].daLats[j])*tan_delta);
        body[iBody].daInsol[j][iDay] = Sconst * (HA*sin(body[iBody].daLats[j])*sin_delta + \
            cos(body[iBody].daLats[j])*cos_delta*sin(HA)) / (PI*pow(body[iBody].dAstroDist,2));
      }
    }
  }
}
          
void AnnualInsolation(BODY *body, int iBody) {
  int i, j;
  double LongP, TrueA, EccA, MeanL;
  
  LongP = body[iBody].dLongP + PI; //Pericenter, relative to direction of primary at spring equinox

  body[iBody].dTrueL = -PI/2;        //starts the year at the (northern) winter solstice
  TrueA = body[iBody].dTrueL - LongP;
  while (TrueA < 0.0) TrueA += 2*PI;
  body[iBody].dEcc = sqrt(pow(body[iBody].dHecc,2)+pow(body[iBody].dKecc,2));
  body[iBody].dObliquity = atan2(sqrt(pow(body[iBody].dXobl,2)+pow(body[iBody].dYobl,2)),body[iBody].dZobl);
  EccA = true2eccA(TrueA, body[iBody].dEcc);
  MeanL = EccA - body[iBody].dEcc*sin(EccA) + LongP;
  
  for (j=0;j<body[iBody].iNumLats;j++)
    body[iBody].daAnnualInsol[j] = 0.0;
  
  for (i=0;i<=body[iBody].iNDays;i++) {
    if (i!=0) {
      MeanL = MeanL + 2*PI/body[iBody].iNDays;
      /* This will only work for the secular orbital model. 
         Will need to be changed when/if resonances are added. */
      body[iBody].dMeanA = MeanL - LongP;
      kepler_eqn(body, iBody);
      EccA = body[iBody].dEccA;
      while (EccA >= 2*PI) EccA -= 2*PI;
      while (EccA < 0.0) EccA += 2*PI;
      if (EccA > PI) {
        TrueA = 2*PI - acos((cos(EccA) - body[iBody].dEcc)/(1.0 - body[iBody].dEcc*cos(EccA)));
      } else {
        TrueA = acos((cos(EccA) - body[iBody].dEcc)/(1.0 - body[iBody].dEcc*cos(EccA)));
      }      
      body[iBody].dTrueL = TrueA + LongP;     
    }
    
    while (body[iBody].dTrueL > 2*PI) body[iBody].dTrueL -= 2*PI;
    while (body[iBody].dTrueL < 0.0) body[iBody].dTrueL += 2*PI;
          
    // planet-star distance (units of semi-major axis):  
    body[iBody].dAstroDist = (1.0 - pow(body[iBody].dEcc,2))/(1.0+body[iBody].dEcc*cos(TrueA)); 
    
    DailyInsolation(body, iBody, i);
    
    for (j=0;j<body[iBody].iNumLats;j++) {
      body[iBody].daAnnualInsol[j] += body[iBody].daInsol[j][i]/((double)body[iBody].iNDays);
    }
  }
}

void Albedo(BODY *body, int iBody) {
 int i;
 
 for (i=0;i<body[iBody].iNumLats;i++) { 
    if (body[iBody].dFixIceLat) {
       /* If user sets ice line latitude to be fixed, is it a cold pole or cold equator planet? */
       if (body[iBody].bJormungand) {
          /* Cold equator */
          if (fabs(body[iBody].daLats[i]) < body[iBody].dFixIceLat) {
            body[iBody].daAlbedo[i] = 0.6;
          } else {
            body[iBody].daAlbedo[i] = 0.3;
          }
       } else {
          /* Cold poles */
          if (fabs(body[iBody].daLats[i]) > body[iBody].dFixIceLat) {
            body[iBody].daAlbedo[i] = 0.6;
          } else {
            body[iBody].daAlbedo[i] = 0.3;
          }
       }
    } else if (body[iBody].bAlbedoZA) {
       /* Use albedo based on zenith angle */
       body[iBody].daAlbedo[i] = 0.31+0.04*(3*pow(sin(body[iBody].daLats[i]),2)-1);
    } else {
       if (body[iBody].daTemp[i] <= -10.0) {
         body[iBody].daAlbedo[i] = 0.6;
       } else {
         body[iBody].daAlbedo[i] = 0.3;
       }
    }
  }
}
    
// void DiagMatrix(double *v, double **M, int n, int offset) {
//   /* Construct a square diagonal nxn matrix from vector v
//      Diagonal can be offset  */
//   int i;
//    
//   for (i=0;i<(n-abs(offset));i++) {
//     if (offset >= 0) {
//       M[i][i+offset] = v[i];
//     } else {
//       M[i+offset][i] = v[i];
//     }
//   }
// }

void MatrixInvert(double **Mcopy, double **invM, int n) {
  double *unitv;
  int i, j, *rowswap;
  float parity;
  
  unitv = malloc(n*sizeof(double));
  rowswap = malloc(n*sizeof(int));
  
  ludcmp(Mcopy,n,rowswap,&parity);
  for (i=0;i<n;i++) {
    for (j=0;j<n;j++) {
      if (j==i) {
        unitv[j] = 1.0;
      } else {
        unitv[j] = 0.0;
      }
    }
    lubksb(Mcopy,n,rowswap,unitv);
    for (j=0;j<n;j++) {
      invM[j][i] = unitv[j];
    }
  }
}

void TempGradient(BODY *body, double delta_x, int iBody) {
  int i;
  
  body[iBody].daTGrad[0] = (body[iBody].daTemp[1]-body[iBody].daTemp[0])/(delta_x);
  for (i=1;i<body[iBody].iNumLats-1;i++) {
    body[iBody].daTGrad[i] = (body[iBody].daTemp[i+1]-body[iBody].daTemp[i-1])/(2*delta_x); 
  }
  body[iBody].daTGrad[body[iBody].iNumLats-1] = (body[iBody].daTemp[body[iBody].iNumLats-1]-\
                          body[iBody].daTemp[body[iBody].iNumLats-2])/(delta_x);
}  
    
void PoiseClimate(BODY *body, int iBody) {
  double delta_t, delta_x, xboundary, Tchange, tmpTglobal;
  double *lambda, **M, **Mcopy, **Mdiff, **invM,  *SourceF, *TempTerms, *tmpTemp, *tmpTempTerms, *Dmidpt; 
  int Nmax, i, j, n, k;
  
  /* Get cuurent climate parameters */
  Albedo(body, iBody);
  AnnualInsolation(body, iBody);
  
  delta_t = 1.5/body[iBody].iNumLats;
  delta_x = 2.0/body[iBody].iNumLats;
  Nmax = 1000;
  
  lambda = malloc((body[iBody].iNumLats+1)*sizeof(double));
  M = malloc(body[iBody].iNumLats*sizeof(double*));
  Mdiff = malloc(body[iBody].iNumLats*sizeof(double*)); // matrix of heat diffusion terms only
  Mcopy = malloc(body[iBody].iNumLats*sizeof(double*));
  invM = malloc(body[iBody].iNumLats*sizeof(double*));
  SourceF = malloc(body[iBody].iNumLats*sizeof(double));
  TempTerms = malloc(body[iBody].iNumLats*sizeof(double));
  tmpTemp = malloc(body[iBody].iNumLats*sizeof(double));
  tmpTempTerms = malloc(body[iBody].iNumLats*sizeof(double));
  Dmidpt = malloc(body[iBody].iNumLats*sizeof(double));
  
  /* Setup matrices, source function, temperature terms, global mean */
  for (i=0;i<body[iBody].iNumLats+1;i++) {
    xboundary = -1.0 + i*2.0/body[iBody].iNumLats;
    lambda[i] = body[iBody].daDiffusion[i]*(1.0-pow(xboundary,2))/(pow(delta_x,2));
  }
  
  body[iBody].dTGlobal = 0.0;
  for (i=0;i<body[iBody].iNumLats;i++) {
    M[i] = malloc(body[iBody].iNumLats*sizeof(double));
    Mdiff[i] = malloc(body[iBody].iNumLats*sizeof(double));
    Mcopy[i] = malloc(body[iBody].iNumLats*sizeof(double));
    invM[i] = malloc(body[iBody].iNumLats*sizeof(double));
    TempTerms[i] = 0.0;
    
    for (j=0;j<body[iBody].iNumLats;j++) {
      if (j==i) {
        M[i][j] = (-body[iBody].dPlanckB-lambda[i+1]-lambda[i])/body[iBody].dHeatCapAnn;
        Mdiff[i][j] = (-lambda[i+1]-lambda[i]);
        Mcopy[i][j] = -1.0/delta_t;
      } else if (j==(i+1)) {
        M[i][j] = lambda[j]/body[iBody].dHeatCapAnn;
        Mdiff[i][j] = lambda[j];
        Mcopy[i][j] = 0.0;
      } else if (j==(i-1)) {
        M[i][j] = lambda[i]/body[iBody].dHeatCapAnn;
        Mdiff[i][j] = lambda[i];
        Mcopy[i][j] = 0.0;
      } else {
        M[i][j] = 0.0;
        Mdiff[i][j] = 0.0;
        Mcopy[i][j] = 0.0;
      }
      Mcopy[i][j] += 0.5*M[i][j];
      TempTerms[i] += M[i][j]*body[iBody].daTemp[j];
    }
    SourceF[i] = ((1.0-body[iBody].daAlbedo[i])*body[iBody].daAnnualInsol[i] - \
                         body[iBody].dPlanckA)/body[iBody].dHeatCapAnn;
    TempTerms[i] += SourceF[i];
    body[iBody].dTGlobal += body[iBody].daTemp[i]/body[iBody].iNumLats;
  }
  
  MatrixInvert(Mcopy, invM, body[iBody].iNumLats);
    
  /* Relaxation to equilibrium */
  n = 1;
  Tchange = 1.0;
  while (fabs(Tchange) > 1e-12) {
    tmpTglobal = 0.0;
    for (i=0;i<body[iBody].iNumLats;i++) {
      tmpTemp[i] = 0.0;
      tmpTempTerms[i] = SourceF[i];
      
      for (j=0;j<body[iBody].iNumLats;j++) {
        tmpTemp[i] += -invM[i][j]*(0.5*(TempTerms[j]+SourceF[j])+body[iBody].daTemp[j]/delta_t);
        tmpTempTerms[i] += M[i][j]*body[iBody].daTemp[j];
      }
      tmpTglobal += tmpTemp[i]/body[iBody].iNumLats;
    }
    Tchange = tmpTglobal - body[iBody].dTGlobal;
    
    /* Update albedo, source function, temperature, temperature terms, and global mean */
    Albedo(body,iBody);
    for (i=0;i<body[iBody].iNumLats;i++) {
      SourceF[i] = ((1.0-body[iBody].daAlbedo[i])*body[iBody].daAnnualInsol[i] - \
                         body[iBody].dPlanckA)/body[iBody].dHeatCapAnn;
      body[iBody].daTemp[i] = tmpTemp[i];
      TempTerms[i] = tmpTempTerms[i];
      body[iBody].dTGlobal = tmpTglobal;
    }
    if (n >= Nmax) {
      fprintf(stderr,"POISE solution not converged before max iterations reached.");
      exit(EXIT_INPUT);
    }
    n++;
  }
  
  /* Calculate some interesting quantities */
  body[iBody].dFluxInGlobal = 0.0;
  body[iBody].dFluxOutGlobal = 0.0;
  body[iBody].dAlbedoGlobal = 0.0;
  TempGradient(body, delta_x, iBody);
  for (i=0;i<body[iBody].iNumLats;i++) {
    Dmidpt[i] = 0.5*(body[iBody].daDiffusion[i+1]+body[iBody].daDiffusion[i]);
    body[iBody].daFlux[i] = -2.*PI*pow(body[iBody].dRadius,2)*sqrt(1.0-pow(sin(body[iBody].daLats[i]),2)) * \
                            Dmidpt[i]*body[iBody].daTGrad[i];
    body[iBody].daFluxIn[i] = (1.0 - body[iBody].daAlbedo[i])*body[iBody].daAnnualInsol[i];
    body[iBody].daFluxOut[i] = body[iBody].dPlanckA + body[iBody].dPlanckB*body[iBody].daTemp[i];                       
    
    body[iBody].dFluxInGlobal += body[iBody].daFluxIn[i]/body[iBody].iNumLats;
    body[iBody].dFluxOutGlobal += body[iBody].daFluxOut[i]/body[iBody].iNumLats;
    
    body[iBody].daDivFlux[i] = 0.0;
    for (j=0;j<body[iBody].iNumLats;j++) {
      body[iBody].daDivFlux[i] += -Mdiff[i][j]*body[iBody].daTemp[j];
    }
    body[iBody].dAlbedoGlobal += body[iBody].daAlbedo[i]/body[iBody].iNumLats;
  } 
}
      
  
  

