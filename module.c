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

void VerifyRotationNULL(BODY *body,CONTROL *control,OPTIONS *options,char cFile[],int iBody) {
  /* Nothing */
}

double fdReturnOutputZero(BODY *body,SYSTEM *system,UPDATE *update,int iBody,int iBody1) {
  return 0;
}

void InitializeModule(MODULE *module,int iNumBodies) {
  int iBody;

  module->iNumModules = malloc(iNumBodies*sizeof(int));
  module->iaModule = malloc(iNumBodies*sizeof(int*));  

  // Function pointer vectors
  module->fnInitializeUpdate = malloc(iNumBodies*sizeof(fnInitializeUpdateModule));
  module->fnInitializeOutput = malloc(iNumBodies*sizeof(fnInitializeOutputModule*));
  module->fnFinalizeOutputFunction = malloc(iNumBodies*sizeof(fnFinalizeOutputFunctionModule*));
  module->fnFinalizeUpdate40KNumCore = malloc(iNumBodies*sizeof(fnFinalizeUpdate40KNumCoreModule));
  module->fnFinalizeUpdate40KNumMan = malloc(iNumBodies*sizeof(fnFinalizeUpdate40KNumManModule));
  module->fnFinalizeUpdate232ThNumCore = malloc(iNumBodies*sizeof(fnFinalizeUpdate232ThNumCoreModule));
  module->fnFinalizeUpdate232ThNumMan = malloc(iNumBodies*sizeof(fnFinalizeUpdate232ThNumManModule));
  module->fnFinalizeUpdate235UNumCore = malloc(iNumBodies*sizeof(fnFinalizeUpdate235UNumCoreModule));
  module->fnFinalizeUpdate235UNumMan = malloc(iNumBodies*sizeof(fnFinalizeUpdate235UNumManModule)); 
  module->fnFinalizeUpdate238UNumCore = malloc(iNumBodies*sizeof(fnFinalizeUpdate238UNumCoreModule));
  module->fnFinalizeUpdate238UNumMan = malloc(iNumBodies*sizeof(fnFinalizeUpdate238UNumManModule));
  module->fnFinalizeUpdateHecc = malloc(iNumBodies*sizeof(fnFinalizeUpdateHeccModule));
  module->fnFinalizeUpdateKecc = malloc(iNumBodies*sizeof(fnFinalizeUpdateKeccModule));
  module->fnFinalizeUpdatePinc = malloc(iNumBodies*sizeof(fnFinalizeUpdatePincModule));
  module->fnFinalizeUpdateQinc = malloc(iNumBodies*sizeof(fnFinalizeUpdateQincModule));
  module->fnFinalizeUpdateRot = malloc(iNumBodies*sizeof(fnFinalizeUpdateRotModule));
  module->fnFinalizeUpdateSemi = malloc(iNumBodies*sizeof(fnFinalizeUpdateSemiModule ));
  module->fnFinalizeUpdateTMan = malloc(iNumBodies*sizeof(fnFinalizeUpdateTManModule));
  module->fnFinalizeUpdateTCore = malloc(iNumBodies*sizeof(fnFinalizeUpdateTCoreModule));
  module->fnFinalizeUpdateXobl = malloc(iNumBodies*sizeof(fnFinalizeUpdateXoblModule));
  module->fnFinalizeUpdateYobl = malloc(iNumBodies*sizeof(fnFinalizeUpdateYoblModule));
  module->fnFinalizeUpdateZobl = malloc(iNumBodies*sizeof(fnFinalizeUpdateZoblModule));
  
  module->fnFinalizeUpdateSurfaceWaterMass = malloc(iNumBodies*sizeof(fnFinalizeUpdateSurfaceWaterMassModule));
  module->fnFinalizeUpdateLuminosity = malloc(iNumBodies*sizeof(fnFinalizeUpdateLuminosityModule));
  module->fnFinalizeUpdateRadius = malloc(iNumBodies*sizeof(fnFinalizeUpdateRadiusModule));

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

  /************************
   * ADD NEW MODULES HERE *
   ************************/

  if (body[iBody].bEqtide)
    iNumModules++;
  if (body[iBody].bDistOrb)
    iNumModules++;
  if (body[iBody].bDistRot)
    iNumModules++;
  if (body[iBody].bRadheat)
    iNumModules++;
  if (body[iBody].bThermint)
    iNumModules++;
  if (body[iBody].bAtmEsc)
    iNumModules++;
  if (body[iBody].bStellar)
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

  // Finalize Primary Variable Functions
  //module->fnFinalizeUpdateEcc[iBody] = malloc(iNumModules*sizeof(fnFinalizeUpdateEccModule));
  module->fnFinalizeUpdate40KNumCore[iBody] = malloc(iNumModules*sizeof(fnFinalizeUpdate40KNumCoreModule));
  module->fnFinalizeUpdate40KNumMan[iBody] = malloc(iNumModules*sizeof(fnFinalizeUpdate40KNumManModule));
  module->fnFinalizeUpdate232ThNumCore[iBody] = malloc(iNumModules*sizeof(fnFinalizeUpdate232ThNumCoreModule));
  module->fnFinalizeUpdate232ThNumMan[iBody] = malloc(iNumModules*sizeof(fnFinalizeUpdate232ThNumManModule));
  module->fnFinalizeUpdate235UNumCore[iBody] = malloc(iNumModules*sizeof(fnFinalizeUpdate235UNumCoreModule));
  module->fnFinalizeUpdate235UNumMan[iBody] = malloc(iNumModules*sizeof(fnFinalizeUpdate235UNumManModule));  
  module->fnFinalizeUpdate238UNumCore[iBody] = malloc(iNumModules*sizeof(fnFinalizeUpdate238UNumCoreModule));
  module->fnFinalizeUpdate238UNumMan[iBody] = malloc(iNumModules*sizeof(fnFinalizeUpdate238UNumManModule));
  module->fnFinalizeUpdateHecc[iBody] = malloc(iNumModules*sizeof(fnFinalizeUpdateHeccModule));
  module->fnFinalizeUpdateKecc[iBody] = malloc(iNumModules*sizeof(fnFinalizeUpdateKeccModule));
  module->fnFinalizeUpdatePinc[iBody] = malloc(iNumModules*sizeof(fnFinalizeUpdatePincModule));
  module->fnFinalizeUpdateQinc[iBody] = malloc(iNumModules*sizeof(fnFinalizeUpdateQincModule));
  module->fnFinalizeUpdateRot[iBody] = malloc(iNumModules*sizeof(fnFinalizeUpdateRotModule));
  module->fnFinalizeUpdateSemi[iBody] = malloc(iNumModules*sizeof(fnFinalizeUpdateSemiModule));
  module->fnFinalizeUpdateTMan[iBody] = malloc(iNumModules*sizeof(fnFinalizeUpdateTManModule));
  module->fnFinalizeUpdateTCore[iBody] = malloc(iNumModules*sizeof(fnFinalizeUpdateTCoreModule));
  module->fnFinalizeUpdateXobl[iBody] = malloc(iNumModules*sizeof(fnFinalizeUpdateXoblModule));
  module->fnFinalizeUpdateYobl[iBody] = malloc(iNumModules*sizeof(fnFinalizeUpdateYoblModule));
  module->fnFinalizeUpdateZobl[iBody] = malloc(iNumModules*sizeof(fnFinalizeUpdateZoblModule));
  module->fnFinalizeUpdateLuminosity[iBody] = malloc(iNumModules*sizeof(fnFinalizeUpdateLuminosityModule));
  module->fnFinalizeUpdateSurfaceWaterMass[iBody] = malloc(iNumModules*sizeof(fnFinalizeUpdateSurfaceWaterMassModule));
  module->fnFinalizeUpdateRadius[iBody] = malloc(iNumModules*sizeof(fnFinalizeUpdateRadiusModule));
  
  /* Initialize all FinalizeUpdate functions to null. The modules that
     need them will replace them in AddModule. */
  for(iModule = 0; iModule < iNumModules; iModule++) {
    module->fnFinalizeUpdate40KNumCore[iBody][iModule] = &FinalizeUpdateNULL;
    module->fnFinalizeUpdate40KNumMan[iBody][iModule] = &FinalizeUpdateNULL;
    module->fnFinalizeUpdate232ThNumCore[iBody][iModule] = &FinalizeUpdateNULL;
    module->fnFinalizeUpdate232ThNumMan[iBody][iModule] = &FinalizeUpdateNULL;
    module->fnFinalizeUpdate235UNumCore[iBody][iModule] = &FinalizeUpdateNULL;
    module->fnFinalizeUpdate235UNumMan[iBody][iModule] = &FinalizeUpdateNULL;
    module->fnFinalizeUpdate238UNumCore[iBody][iModule] = &FinalizeUpdateNULL;
    module->fnFinalizeUpdate238UNumMan[iBody][iModule] = &FinalizeUpdateNULL;
    module->fnFinalizeUpdateHecc[iBody][iModule] = &FinalizeUpdateNULL;
    module->fnFinalizeUpdateKecc[iBody][iModule] = &FinalizeUpdateNULL;
    module->fnFinalizeUpdatePinc[iBody][iModule] = &FinalizeUpdateNULL;
    module->fnFinalizeUpdateQinc[iBody][iModule] = &FinalizeUpdateNULL;
    module->fnFinalizeUpdateRot[iBody][iModule] = &FinalizeUpdateNULL;
    module->fnFinalizeUpdateSemi[iBody][iModule] = &FinalizeUpdateNULL;
    module->fnFinalizeUpdateTMan[iBody][iModule] = &FinalizeUpdateNULL;
    module->fnFinalizeUpdateTCore[iBody][iModule] = &FinalizeUpdateNULL;
    module->fnFinalizeUpdateXobl[iBody][iModule] = &FinalizeUpdateNULL;
    module->fnFinalizeUpdateYobl[iBody][iModule] = &FinalizeUpdateNULL;
    module->fnFinalizeUpdateZobl[iBody][iModule] = &FinalizeUpdateNULL;
    module->fnFinalizeUpdateLuminosity[iBody][iModule] = &FinalizeUpdateNULL;
    module->fnFinalizeUpdateSurfaceWaterMass[iBody][iModule] = &FinalizeUpdateNULL;
    module->fnFinalizeUpdateRadius[iBody][iModule] = &FinalizeUpdateNULL;
    module->fnVerifyRotation[iBody][iModule] = &VerifyRotationNULL;
  }

  /************************
   * ADD NEW MODULES HERE *
   ************************/

  iModule = 0;
  if (body[iBody].bEqtide) {
    AddModuleEqtide(module,iBody,iModule);
    module->iaModule[iBody][iModule++] = EQTIDE;
  }
  if (body[iBody].bDistOrb) {
    AddModuleDistOrb(module,iBody,iModule);
    module->iaModule[iBody][iModule++] = DISTORB;
  }
   if (body[iBody].bDistRot) {
    AddModuleDistRot(module,iBody,iModule);
    module->iaModule[iBody][iModule++] = DISTROT;
  }
  if (body[iBody].bRadheat) {
    AddModuleRadheat(module,iBody,iModule);
    module->iaModule[iBody][iModule++] = RADHEAT;
  }
  if (body[iBody].bThermint) {
      AddModuleThermint(module,iBody,iModule);
    module->iaModule[iBody][iModule++] = THERMINT;
  }
  if (body[iBody].bAtmEsc) {
    AddModuleAtmEsc(module,iBody,iModule);
    module->iaModule[iBody][iModule++] = ATMESC;
  }
  if (body[iBody].bStellar) {
    AddModuleStellar(module,iBody,iModule);
    module->iaModule[iBody][iModule++] = STELLAR;
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
      } else if (memcmp(sLower(saTmp[iModule]),"distorb",8) == 0) {
	body[iFile-1].bDistOrb = 1;
      } else if (memcmp(sLower(saTmp[iModule]),"distrot",6) == 0) {
	body[iFile-1].bDistRot = 1;
      } else if (memcmp(sLower(saTmp[iModule]),"thermint",8) == 0) {
	body[iFile-1].bThermint = 1;
      } else if (memcmp(sLower(saTmp[iModule]),"atmesc",6) == 0) {
	body[iFile-1].bAtmEsc = 1;
	    } else if (memcmp(sLower(saTmp[iModule]),"stellar",7) == 0) {
	body[iFile-1].bStellar = 1;
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

void InitializeBodyModules(BODY **body,int iNumBodies) {
  int iBody;

  for (iBody=0;iBody<iNumBodies;iBody++) {
      (*body)[iBody].bEqtide = 0;
      (*body)[iBody].bDistOrb = 0;
      (*body)[iBody].bDistRot = 0;
      (*body)[iBody].bRadheat = 0;
      (*body)[iBody].bThermint = 0;
  }
}

/*
 * Verify multi-module dependencies
 */

void VerifyModuleMultiDistOrbDistRot(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,int iBody,int *iModuleProps,int *iModuleForce) {

  if (body[iBody].bDistRot) {
    if (!body[iBody].bDistOrb) {
      fprintf(stderr,"ERROR: Module DISTROT selected for %s, but DISTORB not selected.\n",body[iBody].cName);
      exit(EXIT_INPUT);
    }
  }
}

void VerifyModuleMultiRadheatThermint(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,int iBody,int *iModuleProps,int *iModuleForce) {

  /* This will need modification if material can move between layers */

  if (body[iBody].bThermint) {
    if (!body[iBody].bRadheat) {
      if (control->Io.iVerbose > VERBINPUT)
	fprintf(stderr,"WARNING: Module THERMINT selected for %s, but RADHEAT not selected.\n",body[iBody].cName);
      body[iBody].dPowRadiogCore = 0;
      body[iBody].dPowRadiogMan = 0;
    } else
      control->Evolve.fnPropsAuxMulti[iBody][(*iModuleProps)++] = &PropsAuxRadheatThermint;
  }
}

void VerifyModuleMultiEqtideThermint(BODY *body,CONTROL *control,FILES *files,MODULE *module,OPTIONS *options,int iBody,int *iModuleProps,int *iModuleForce) {
  int iEqtide;

  if (body[iBody].bEqtide) {
    if (!body[iBody].bThermint) 
      // Set Im(k_2) here
      body[iBody].dImK2=body[iBody].dK2/body[iBody].dTidalQ;
    else { // Thermint and Eqtide called
      /* When Thermint and Eqtide are called together, care must be taken as 
	 Im(k_2) must be known in order to calculate TidalZ. As the individual 
	 module PropsAux are called prior to PropsAuxMulti, we must call the 
	 "PropsAuxEqtide" function after Im(k_2) is called. Thus, we replace
	 "PropsAuxEqtide" with PropsAuxNULL and call "PropsAuxEqtide" in
	 PropsAuxEqtideThermint. */
      iEqtide = fiGetModuleIntEqtide(module,iBody);
      control->Evolve.fnPropsAux[iBody][iEqtide] = &PropsAuxNULL;
      control->Evolve.fnPropsAuxMulti[iBody][(*iModuleProps)++] = &PropsAuxEqtideThermint;
    }
  }
}

void VerifyModuleMultiEqtideDistOrb(BODY *body,CONTROL *control,FILES *files,MODULE *module,OPTIONS *options,int iBody,int *iModuleProps,int *iModuleForce) {
  if (body[iBody].bEqtide) {
    if (body[iBody].bDistOrb) {
      control->fnForceBehaviorMulti[iBody][(*iModuleForce)++] = &ForceBehaviorEqtideDistOrb;
    }
  }
}

void VerifyModuleMulti(BODY *body,CONTROL *control,FILES *files,MODULE *module,OPTIONS *options,int iBody) {
  int iNumMultiProps=0,iNumMultiForce=0;

  if (module->iNumModules[iBody] > 1) {
    /* XXX Note that the number of elements here is really a permutation, 
       but this should work for a while. */
    control->Evolve.fnPropsAuxMulti[iBody] = malloc(2*module->iNumModules[iBody]*sizeof(fnPropsAuxModule*));
    control->fnForceBehaviorMulti[iBody] = malloc(2*module->iNumModules[iBody]*sizeof(fnForceBehaviorModule*));
  }

  /* Now verify. Even if only module is called, we still need to call
     these functions as some default behavior is set if other modules aren't
     called. */
  VerifyModuleMultiDistOrbDistRot(body,control,files,options,iBody,&iNumMultiProps,&iNumMultiForce);
  
  VerifyModuleMultiRadheatThermint(body,control,files,options,iBody,&iNumMultiProps,&iNumMultiForce);
  
  VerifyModuleMultiEqtideDistOrb(body,control,files,module,options,iBody,&iNumMultiProps,&iNumMultiForce);

  VerifyModuleMultiEqtideThermint(body,control,files,module,options,iBody,&iNumMultiProps,&iNumMultiForce);
  
  control->Evolve.iNumMultiProps[iBody] = iNumMultiProps;
  control->iNumMultiForce[iBody] = iNumMultiForce;
  if (control->Io.iVerbose >= VERBALL)
    fprintf(stdout,"All of %s's modules verified.\n",body[iBody].cName);
}

/*
 * Auxiliary Properties for multi-module calculations
 */

void PropsAuxEqtideThermint(BODY *body,UPDATE *update,int iBody) {
    body[iBody].dImK2 = fdImk2Man(body,iBody);
    PropsAuxCPL(body,update,iBody);
    body[iBody].dTidePower = fdCPLTidePower(body,iBody);
}

/* This does not seem to be necessary
void PropertiesDistOrbDistRot(BODY *body,UPDATE *update,int iBody) {
  body[iBody].dEccSq = body[iBody].dHecc*body[iBody].dHecc + body[iBody].dKecc*body[iBody].dKecc;
}
*/

void PropsAuxRadheatThermint(BODY *body,UPDATE *update,int iBody) {
  body[iBody].dPowRadiogCore = fdRadPowerCore(body,update,iBody);
  body[iBody].dPowRadiogMan = fdRadPowerMan(body,update,iBody);
}

/*
 * Force Behavior for multi-module calculations
 */

void ForceBehaviorEqtideDistOrb(BODY *body,EVOLVE *evolve,IO *io,int iFoo,int iBar) {
  /*  
  // Insert Russell's code here.
  printf("Entered  ForceBehaviorEqtideLagrange.\n");

// XXX This function 
void RecalcLaplace(BODY *body, SYSTEM *system, int *iaBody) {
  double alpha1, dalpha;
  int j = 0;

  if (body[iaBody[0]].dSemi < body[iaBody[1]].dSemi) {
      alpha1 = body[iaBody[0]].dSemi/body[iaBody[1]].dSemi;
  } else if (body[iaBody[0]].dSemi > body[iaBody[1]].dSemi) {
      alpha1 = body[iaBody[1]].dSemi/body[iaBody[0]].dSemi;
  }
    
  for (j=0;j<LAPLNUM;j++) {
    dalpha = fabs(alpha1 - system->dmAlpha0[system->imLaplaceN[iaBody[0]][iaBody[1]]][j]);
    if (dalpha > system->dDfcrit/system->dmLaplaceD[system->imLaplaceN[iaBody[0]][iaBody[1]]][j]) {
	system->dmLaplaceC[system->imLaplaceN[iaBody[0]][iaBody[1]]][j] = 
	system->fnLaplaceF[j][0](alpha1, 0);
		
	system->dmLaplaceD[system->imLaplaceN[iaBody[0]][iaBody[1]]][j] = 
	system->fnLaplaceDeriv[j][0](alpha1, 0);
		
	system->dmAlpha0[system->imLaplaceN[iaBody[0]][iaBody[1]]][j] = alpha1;
    }
  }
  */
} 
