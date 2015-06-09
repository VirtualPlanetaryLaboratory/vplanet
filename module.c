/******************** MODULE.C **************************/
/*
 * Rory Barnes, Mon Jan 19 13:00:54 PST 2015
 *
 * Functions to manipulate the MODULE struct.
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "vplanet.h"

void FinalizeUpdateNULL(BODY *body,UPDATE *update,int *iEqn,int iVar,int iBody) {
  /* Nothing */
}

void InitializeModule(MODULE *module,int iNumBodies) {
  int iBody;

  module->iNumModules = malloc(iNumBodies*sizeof(int));
  module->iaModule = malloc(iNumBodies*sizeof(int*));  

  // Function pointer vectors
  module->fnInitializeUpdate = malloc(iNumBodies*sizeof(fnInitializeUpdateModule));
  module->fnInitializeOutput = malloc(iNumBodies*sizeof(fnInitializeOutputModule*));
  module->fnFinalizeOutputFunction = malloc(iNumBodies*sizeof(fnFinalizeOutputFunctionModule*));

  module->fnFinalizeUpdateEcc = malloc(iNumBodies*sizeof(fnFinalizeUpdateEccModule));
  module->fnFinalizeUpdateObl = malloc(iNumBodies*sizeof(fnFinalizeUpdateOblModule));
  module->fnFinalizeUpdateRot = malloc(iNumBodies*sizeof(fnFinalizeUpdateRotModule));
  module->fnFinalizeUpdateSemi = malloc(iNumBodies*sizeof(fnFinalizeUpdateSemiModule ));

  module->fnFinalizeUpdate40KNum = malloc(iNumBodies*sizeof(fnFinalizeUpdate40KNumModule));
  module->fnFinalizeUpdate232ThNum = malloc(iNumBodies*sizeof(fnFinalizeUpdate232ThNumModule));
  module->fnFinalizeUpdate238UNum = malloc(iNumBodies*sizeof(fnFinalizeUpdate238UNumModule));

  module->fnFinalizeUpdateNumberOfOrcs = malloc(iNumBodies*sizeof(fnFinalizeUpdateNumberOfOrcsModule));

  // Function Pointer Matrices
  module->fnLogBody = malloc(iNumBodies*sizeof(fnLogBodyModule*));
  module->fnInitializeBody = malloc(iNumBodies*sizeof(fnInitializeBodyModule*));
  module->fnInitializeControl = malloc(iNumBodies*sizeof(fnInitializeControlModule*));
  module->fnInitializeUpdateTmpBody = malloc(iNumBodies*sizeof(fnInitializeUpdateTmpBodyModule*));
  module->fnCountHalts = malloc(iNumBodies*sizeof(fnCountHaltsModule*));
  module->fnReadOptions = malloc(iNumBodies*sizeof(fnReadOptionsModule*));
  module->fnVerify = malloc(iNumBodies*sizeof(fnVerifyModule*));
  module->fnVerifyHalt = malloc(iNumBodies*sizeof(fnVerifyHaltModule*));
  module->fnVerifyRotation = malloc(iNumBodies*sizeof(fnVerifyRotationModule*));

  /* Assume no modules per body to start */
  for (iBody=0;iBody<iNumBodies;iBody++) 
    module->iNumModules[iBody]=0;
}

void FinalizeModule(BODY *body,MODULE *module,int iBody) {
  int iModule=0,iNumModules = 0;

  if (body[iBody].bEqtide)
    iNumModules++;
  if (body[iBody].bRadheat)
    iNumModules++;
  if (body[iBody].bAtmEsc)
    iNumModules++;

  module->iNumModules[iBody] = iNumModules;
  module->iaModule[iBody] = malloc(iNumModules*sizeof(int));

  module->fnLogBody[iBody] = malloc(iNumModules*sizeof(fnLogBodyModule));
  module->fnInitializeControl[iBody] = malloc(iNumModules*sizeof(fnInitializeControlModule));
  module->fnInitializeOutput[iBody] = malloc(iNumModules*sizeof(fnInitializeOutputModule));
  module->fnInitializeUpdateTmpBody[iBody] = malloc(iNumModules*sizeof(fnInitializeUpdateTmpBodyModule));

  module->fnFinalizeOutputFunction[iBody] = malloc(iNumModules*sizeof(fnInitializeOutputFunctionModule));

  module->fnCountHalts[iBody] = malloc(iNumModules*sizeof(fnCountHaltsModule));
  module->fnReadOptions[iBody] = malloc(iNumModules*sizeof(fnReadOptionsModule));
  module->fnVerify[iBody] = malloc(iNumModules*sizeof(fnVerifyModule));

  module->fnVerify[iBody] = malloc(iNumModules*sizeof(fnVerifyModule));
  module->fnVerifyHalt[iBody] = malloc(iNumModules*sizeof(fnVerifyHaltModule));
  module->fnVerifyRotation[iBody] = malloc(iNumModules*sizeof(fnVerifyRotationModule));

  module->fnInitializeBody[iBody] = malloc(iNumModules*sizeof(fnInitializeBodyModule));
  module->fnInitializeUpdate[iBody] = malloc(iNumModules*sizeof(fnInitializeUpdateModule));
  module->fnFinalizeUpdateEcc[iBody] = malloc(iNumModules*sizeof(fnFinalizeUpdateEccModule));
  module->fnFinalizeUpdateObl[iBody] = malloc(iNumModules*sizeof(fnFinalizeUpdateOblModule));
  module->fnFinalizeUpdateRot[iBody] = malloc(iNumModules*sizeof(fnFinalizeUpdateRotModule));
  module->fnFinalizeUpdateSemi[iBody] = malloc(iNumModules*sizeof(fnFinalizeUpdateSemiModule));

  module->fnFinalizeUpdate40KNum[iBody] = malloc(iNumModules*sizeof(fnFinalizeUpdate40KNumModule));
  module->fnFinalizeUpdate232ThNum[iBody] = malloc(iNumModules*sizeof(fnFinalizeUpdate232ThNumModule));
  module->fnFinalizeUpdate238UNum[iBody] = malloc(iNumModules*sizeof(fnFinalizeUpdate238UNumModule));
  
  module->fnFinalizeUpdateNumberOfOrcs[iBody] = malloc(iNumModules*sizeof(fnFinalizeUpdateNumberOfOrcsModule));

  for(iModule = 0; iModule < iNumModules; iModule++) {
    module->fnFinalizeUpdateEcc[iBody][iModule] = &FinalizeUpdateNULL;
    module->fnFinalizeUpdateObl[iBody][iModule] = &FinalizeUpdateNULL;
    module->fnFinalizeUpdateRot[iBody][iModule] = &FinalizeUpdateNULL;
    module->fnFinalizeUpdateSemi[iBody][iModule] = &FinalizeUpdateNULL;
    module->fnFinalizeUpdate40KNum[iBody][iModule] = &FinalizeUpdateNULL;
    module->fnFinalizeUpdate232ThNum[iBody][iModule] = &FinalizeUpdateNULL;
    module->fnFinalizeUpdate238UNum[iBody][iModule] = &FinalizeUpdateNULL;
    module->fnFinalizeUpdateNumberOfOrcs[iBody][iModule] = &FinalizeUpdateNULL;
    }

  /************************
   * ADD NEW MODULES HERE *
   ************************/

  iModule = 0;
  if (body[iBody].bEqtide) {
    AddModuleEqtide(module,iBody,iModule);
    module->iaModule[iBody][iModule++] = EQTIDE;
  }
  if (body[iBody].bRadheat) {
    AddModuleRadheat(module,iBody,iModule);
    module->iaModule[iBody][iModule++] = RADHEAT;
  }
  if (body[iBody].bAtmEsc) {
    AddModuleAtmEsc(module,iBody,iModule);
    module->iaModule[iBody][iModule++] = ATMESC;
  }
}

void ReadModules(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,int iFile){
  int iNumIndices=0,iNumLines=0,iModule;
  int *lTmp;
  char saTmp[MAXARRAY][OPTLEN];

  lTmp=malloc(MAXLINES*sizeof(int));

  AddOptionStringArray(files->Infile[iFile].cIn,options->cName,saTmp,&iNumIndices,&iNumLines,lTmp,control->Io.iVerbose);
  if (lTmp[0] >= 0) {
    NotPrimaryInput(iFile,options->cName,files->Infile[iFile].cIn,lTmp[0],control->Io.iVerbose);
    if (iNumIndices == 0) {
      if (control->Io.iVerbose >= VERBERR)
	fprintf(stderr,"ERROR: No modules input to option %s.\n",options->cName);
      LineExit(files->Infile[iFile].cIn,lTmp[0]);
    }

    for (iModule=0;iModule<iNumIndices;iModule++) {

      /************************
       * ADD NEW MODULES HERE *
       ************************/

      if (memcmp(sLower(saTmp[iModule]),"eqtide",6) == 0) {
	body[iFile-1].bEqtide = 1;
      } else if (memcmp(sLower(saTmp[iModule]),"radheat",7) == 0) {
	body[iFile-1].bRadheat = 1;
      } else if (memcmp(sLower(saTmp[iModule]),"atmesc",6) == 0) {
	body[iFile-1].bAtmEsc = 1;
      } else {
	if (control->Io.iVerbose >= VERBERR)
	  fprintf(stderr,"ERROR: Unknown Module %s provided to %s.\n",saTmp[iModule],options->cName);
	LineExit(files->Infile[iFile].cIn,lTmp[0]);
      }
    }
    UpdateFoundOptionMulti(&files->Infile[iFile],options,lTmp,iNumLines,0);
  } else {
    if (control->Io.iVerbose >= VERBERR && iFile > 0) 
      fprintf(stderr,"WARNING: %s not present in file %s. No evolution will occur for this body.\n",options->cName,files->Infile[iFile].cIn);
  }

  free(lTmp);
}

