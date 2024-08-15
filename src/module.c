/**
 @file module.c

 @brief Functions to manipulate the MODULE struct.

 @author Rory Barnes ([RoryBarnes](https://github.com/RoryBarnes/))

 @date Jan 19 2015

*/

#include "vplanet.h"

/* NULL functions for all module function pointer matrices. All pointers are
   initialized to point to these functions. Modules that require them reset
   the pointers in AddModuleX. */

void InitializeControlNULL(CONTROL *control, int iBody) {
  // Nothing
}

void InitializeBodyNULL(BODY *body, CONTROL *control, UPDATE *update, int iBody,
                        int iModule) {
}

void InitializeModuleNULL(CONTROL *control, MODULE *module) {
}

void InitializeUpdateTmpBodyNULL(BODY *body, CONTROL *control, UPDATE *update,
                                 int iBody) {
}

void FinalizeUpdateNULL(BODY *body, UPDATE *update, int *iEqn, int iVar,
                        int iBody, int iFoo) {
  /* Nothing */
}

void PropsAuxNULL(BODY *body, EVOLVE *evolve, IO *io, UPDATE *update,
                  int iFoo) {
}

// Functions that are helpful for integrations

double fdReturnOutputZero(BODY *body, SYSTEM *system, UPDATE *update, int iBody,
                          int iBody1) {
  return 0;
}

double fndUpdateFunctionTiny(BODY *body, SYSTEM *system, int *iaBody) {
  return dTINY;
}

// Reset function pointer to return dTINY
void SetDerivTiny(fnUpdateVariable ***fnUpdate, int iBody, int iVar, int iEqn) {
  fnUpdate[iBody][iVar][iEqn] = &fndUpdateFunctionTiny;
}

void InitializeModule(BODY *body, CONTROL *control, MODULE *module) {
  int iBody, iNumBodies;

  iNumBodies = control->Evolve.iNumBodies;

  module->iNumModules      = malloc(iNumBodies * sizeof(int));
  module->iNumManageDerivs = malloc(iNumBodies * sizeof(int));
  module->iaModule         = malloc(iNumBodies * sizeof(int *));
  module->iBitSum          = malloc(iNumBodies * sizeof(int *));

  module->iaEqtide        = malloc(iNumBodies * sizeof(int));
  module->iaDistOrb       = malloc(iNumBodies * sizeof(int));
  module->iaDistRot       = malloc(iNumBodies * sizeof(int));
  module->iaRadheat       = malloc(iNumBodies * sizeof(int));
  module->iaThermint      = malloc(iNumBodies * sizeof(int));
  module->iaAtmEsc        = malloc(iNumBodies * sizeof(int));
  module->iaStellar       = malloc(iNumBodies * sizeof(int));
  module->iaPoise         = malloc(iNumBodies * sizeof(int));
  module->iaBinary        = malloc(iNumBodies * sizeof(int));
  module->iaFlare         = malloc(iNumBodies * sizeof(int));
  module->iaGalHabit      = malloc(iNumBodies * sizeof(int));
  module->iaSpiNBody      = malloc(iNumBodies * sizeof(int));
  module->iaMagmOc        = malloc(iNumBodies * sizeof(int));
  module->iaEqtideStellar = malloc(iNumBodies * sizeof(int));

  // Initialize some of the recently malloc'd values in module
  for (iBody = 0; iBody < iNumBodies; iBody++) {
    // Allow parameters that require no module
    module->iBitSum[iBody] = 1;

    // Set module numbers to -1. They will be changed in FinalizeModule() if
    // appropriate
    module->iaEqtide[iBody]        = -1;
    module->iaDistOrb[iBody]       = -1;
    module->iaDistRot[iBody]       = -1;
    module->iaRadheat[iBody]       = -1;
    module->iaThermint[iBody]      = -1;
    module->iaAtmEsc[iBody]        = -1;
    module->iaStellar[iBody]       = -1;
    module->iaPoise[iBody]         = -1;
    module->iaBinary[iBody]        = -1;
    module->iaFlare[iBody]         = -1;
    module->iaGalHabit[iBody]      = -1;
    module->iaSpiNBody[iBody]      = -1;
    module->iaMagmOc[iBody]        = -1;
    module->iaEqtideStellar[iBody] = -1;
  }

  // Function pointer vectors
  module->fnInitializeUpdate =
        malloc(iNumBodies * sizeof(fnInitializeUpdateModule));
  module->fnInitializeOutput =
        malloc(iNumBodies * sizeof(fnInitializeOutputModule *));

  // Finalize Primary Variable Functions
  // module->fnFinalizeUpdateEcc =
  // malloc(iNumBodies*sizeof(fnFinalizeUpdateEccModule));
  module->fnFinalizeUpdate26AlNumCore =
        malloc(iNumBodies * sizeof(fnFinalizeUpdate26AlNumCoreModule));
  module->fnFinalizeUpdate26AlNumMan =
        malloc(iNumBodies * sizeof(fnFinalizeUpdate26AlNumManModule));
  module->fnFinalizeUpdate40KNumCore =
        malloc(iNumBodies * sizeof(fnFinalizeUpdate40KNumCoreModule));
  module->fnFinalizeUpdate40KNumCrust =
        malloc(iNumBodies * sizeof(fnFinalizeUpdate40KNumCrustModule));
  module->fnFinalizeUpdate40KNumMan =
        malloc(iNumBodies * sizeof(fnFinalizeUpdate40KNumManModule));
  module->fnFinalizeUpdate232ThNumCore =
        malloc(iNumBodies * sizeof(fnFinalizeUpdate232ThNumCoreModule));
  module->fnFinalizeUpdate232ThNumCrust =
        malloc(iNumBodies * sizeof(fnFinalizeUpdate232ThNumCrustModule));
  module->fnFinalizeUpdate232ThNumMan =
        malloc(iNumBodies * sizeof(fnFinalizeUpdate232ThNumManModule));
  module->fnFinalizeUpdate235UNumCore =
        malloc(iNumBodies * sizeof(fnFinalizeUpdate235UNumCoreModule));
  module->fnFinalizeUpdate235UNumCrust =
        malloc(iNumBodies * sizeof(fnFinalizeUpdate235UNumCrustModule));
  module->fnFinalizeUpdate235UNumMan =
        malloc(iNumBodies * sizeof(fnFinalizeUpdate235UNumManModule));
  module->fnFinalizeUpdate238UNumCore =
        malloc(iNumBodies * sizeof(fnFinalizeUpdate238UNumCoreModule));
  module->fnFinalizeUpdate238UNumCrust =
        malloc(iNumBodies * sizeof(fnFinalizeUpdate238UNumCrustModule));
  module->fnFinalizeUpdate238UNumMan =
        malloc(iNumBodies * sizeof(fnFinalizeUpdate238UNumManModule));

  module->fnFinalizeUpdateCBPR =
        malloc(iNumBodies * sizeof(fnFinalizeUpdateCBPRModule));
  module->fnFinalizeUpdateCBPZ =
        malloc(iNumBodies * sizeof(fnFinalizeUpdateCBPZModule));
  module->fnFinalizeUpdateCBPPhi =
        malloc(iNumBodies * sizeof(fnFinalizeUpdateCBPPhiModule));
  module->fnFinalizeUpdateCBPRDot =
        malloc(iNumBodies * sizeof(fnFinalizeUpdateCBPRDotModule));
  module->fnFinalizeUpdateCBPZDot =
        malloc(iNumBodies * sizeof(fnFinalizeUpdateCBPZDotModule));
  module->fnFinalizeUpdateCBPPhiDot =
        malloc(iNumBodies * sizeof(fnFinalizeUpdateCBPPhiDotModule));

  module->fnFinalizeUpdateDynEllip =
        malloc(iNumBodies * sizeof(fnFinalizeUpdateDynEllipModule));
  module->fnFinalizeUpdateEnvelopeMass =
        malloc(iNumBodies * sizeof(fnFinalizeUpdateEnvelopeMassModule));
  module->fnFinalizeUpdateHecc =
        malloc(iNumBodies * sizeof(fnFinalizeUpdateHeccModule));
  module->fnFinalizeUpdateKecc =
        malloc(iNumBodies * sizeof(fnFinalizeUpdateKeccModule));
  module->fnFinalizeUpdateLuminosity =
        malloc(iNumBodies * sizeof(fnFinalizeUpdateLuminosityModule));
  module->fnFinalizeUpdateLXUV =
        malloc(iNumBodies * sizeof(fnFinalizeUpdateIceMassModule));

  module->fnFinalizeUpdateMass =
        malloc(iNumBodies * sizeof(fnFinalizeUpdateMassModule));
  module->fnFinalizeUpdateOxygenMass =
        malloc(iNumBodies * sizeof(fnFinalizeUpdateOxygenMassModule));
  module->fnFinalizeUpdateOxygenMantleMass =
        malloc(iNumBodies * sizeof(fnFinalizeUpdateOxygenMantleMassModule));
  module->fnFinalizeUpdatePinc =
        malloc(iNumBodies * sizeof(fnFinalizeUpdatePincModule));
  module->fnFinalizeUpdateQinc =
        malloc(iNumBodies * sizeof(fnFinalizeUpdateQincModule));
  module->fnFinalizeUpdateRadius =
        malloc(iNumBodies * sizeof(fnFinalizeUpdateRadiusModule));
  module->fnFinalizeUpdateRadGyra =
        malloc(iNumBodies * sizeof(fnFinalizeUpdateRadGyraModule));
  module->fnFinalizeUpdateRot =
        malloc(iNumBodies * sizeof(fnFinalizeUpdateRotModule));
  module->fnFinalizeUpdateSemi =
        malloc(iNumBodies * sizeof(fnFinalizeUpdateSemiModule));
  module->fnFinalizeUpdateLostAngMom =
        malloc(iNumBodies * sizeof(fnFinalizeUpdateLostAngMomModule));
  module->fnFinalizeUpdateLostEng =
        malloc(iNumBodies * sizeof(fnFinalizeUpdateLostEngModule));

  module->fnFinalizeUpdateSurfaceWaterMass =
        malloc(iNumBodies * sizeof(fnFinalizeUpdateSurfaceWaterMassModule));
  module->fnFinalizeUpdateTemperature =
        malloc(iNumBodies * sizeof(fnFinalizeUpdateTemperatureModule));
  module->fnFinalizeUpdateTMan =
        malloc(iNumBodies * sizeof(fnFinalizeUpdateTManModule));
  module->fnFinalizeUpdateTCore =
        malloc(iNumBodies * sizeof(fnFinalizeUpdateTCoreModule));
  module->fnFinalizeUpdateXobl =
        malloc(iNumBodies * sizeof(fnFinalizeUpdateXoblModule));
  module->fnFinalizeUpdateYobl =
        malloc(iNumBodies * sizeof(fnFinalizeUpdateYoblModule));
  module->fnFinalizeUpdateZobl =
        malloc(iNumBodies * sizeof(fnFinalizeUpdateZoblModule));

  module->fnFinalizeUpdateEccX =
        malloc(iNumBodies * sizeof(fnFinalizeUpdateEccXModule));
  module->fnFinalizeUpdateEccY =
        malloc(iNumBodies * sizeof(fnFinalizeUpdateEccYModule));
  module->fnFinalizeUpdateEccZ =
        malloc(iNumBodies * sizeof(fnFinalizeUpdateEccZModule));
  module->fnFinalizeUpdateAngMX =
        malloc(iNumBodies * sizeof(fnFinalizeUpdateAngMXModule));
  module->fnFinalizeUpdateAngMY =
        malloc(iNumBodies * sizeof(fnFinalizeUpdateAngMYModule));
  module->fnFinalizeUpdateAngMZ =
        malloc(iNumBodies * sizeof(fnFinalizeUpdateAngMZModule));

  module->fnFinalizeUpdatePositionX =
        malloc(iNumBodies * sizeof(fnFinalizeUpdatePositionXModule));
  module->fnFinalizeUpdatePositionY =
        malloc(iNumBodies * sizeof(fnFinalizeUpdatePositionYModule));
  module->fnFinalizeUpdatePositionZ =
        malloc(iNumBodies * sizeof(fnFinalizeUpdatePositionZModule));
  module->fnFinalizeUpdateVelX =
        malloc(iNumBodies * sizeof(fnFinalizeUpdateVelXModule));
  module->fnFinalizeUpdateVelY =
        malloc(iNumBodies * sizeof(fnFinalizeUpdateVelYModule));
  module->fnFinalizeUpdateVelZ =
        malloc(iNumBodies * sizeof(fnFinalizeUpdateVelZModule));

  module->fnFinalizeUpdateWaterMassMOAtm =
        malloc(iNumBodies * sizeof(fnFinalizeUpdateWaterMassMOAtmModule));
  module->fnFinalizeUpdateWaterMassSol =
        malloc(iNumBodies * sizeof(fnFinalizeUpdateWaterMassSolModule));
  module->fnFinalizeUpdateSurfTemp =
        malloc(iNumBodies * sizeof(fnFinalizeUpdateSurfTempModule));
  module->fnFinalizeUpdatePotTemp =
        malloc(iNumBodies * sizeof(fnFinalizeUpdatePotTempModule));
  module->fnFinalizeUpdateSolidRadius =
        malloc(iNumBodies * sizeof(fnFinalizeUpdateSolidRadiusModule));
  module->fnFinalizeUpdateOxygenMassMOAtm =
        malloc(iNumBodies * sizeof(fnFinalizeUpdateOxygenMassMOAtmModule));
  module->fnFinalizeUpdateOxygenMassSol =
        malloc(iNumBodies * sizeof(fnFinalizeUpdateOxygenMassSolModule));
  module->fnFinalizeUpdateHydrogenMassSpace =
        malloc(iNumBodies * sizeof(fnFinalizeUpdateHydrogenMassSpaceModule));
  module->fnFinalizeUpdateOxygenMassSpace =
        malloc(iNumBodies * sizeof(fnFinalizeUpdateOxygenMassSpaceModule));
  module->fnFinalizeUpdateCO2MassMOAtm =
        malloc(iNumBodies * sizeof(fnFinalizeUpdateCO2MassMOAtmModule));
  module->fnFinalizeUpdateCO2MassSol =
        malloc(iNumBodies * sizeof(fnFinalizeUpdateCO2MassSolModule));

  // Function Pointer Matrices
  module->fnLogBody = malloc(iNumBodies * sizeof(fnLogBodyModule *));
  module->fnInitializeBody =
        malloc(iNumBodies * sizeof(fnInitializeBodyModule *));
  module->fnInitializeControl =
        malloc(iNumBodies * sizeof(fnInitializeControlModule *));
  module->fnInitializeUpdateTmpBody =
        malloc(iNumBodies * sizeof(fnInitializeUpdateTmpBodyModule *));
  module->fnCountHalts  = malloc(iNumBodies * sizeof(fnCountHaltsModule *));
  module->fnReadOptions = malloc(iNumBodies * sizeof(fnReadOptionsModule *));
  module->fnVerify      = malloc(iNumBodies * sizeof(fnVerifyModule *));
  module->fnAssignDerivatives =
        malloc(iNumBodies * sizeof(fnManageModuleDerivatives *));
  module->fnNullDerivatives =
        malloc(iNumBodies * sizeof(fnManageModuleDerivatives *));
  module->fnVerifyHalt = malloc(iNumBodies * sizeof(fnVerifyHaltModule *));

  /* Assume no modules per body to start */
  for (iBody = 0; iBody < iNumBodies; iBody++) {
    module->iNumModules[iBody]      = 0;
    module->iNumManageDerivs[iBody] = 0;
  }

  InitializeBodyModules(&body, iNumBodies);
}

void FinalizeModule(BODY *body, CONTROL *control, MODULE *module, int iBody) {
  int iModule = 0, iNumModules = 0, iNumModuleMulti = 0;

  /************************
   * ADD NEW MODULES HERE *
   ************************/

  if (body[iBody].bEqtide) {
    iNumModules++;
  }
  if (body[iBody].bDistOrb) {
    iNumModules++;
  }
  if (body[iBody].bDistRot) {
    iNumModules++;
  }
  if (body[iBody].bRadheat) {
    iNumModules++;
  }
  if (body[iBody].bThermint) {
    iNumModules++;
  }
  if (body[iBody].bAtmEsc) {
    iNumModules++;
  }
  if (body[iBody].bStellar) {
    iNumModules++;
  }
  if (body[iBody].bPoise) {
    iNumModules++;
  }
  if (body[iBody].bBinary) {
    iNumModules++;
  }
  if (body[iBody].bFlare) {
    iNumModules++;
  }
  if (body[iBody].bGalHabit) {
    iNumModules++;
  }
  if (body[iBody].bSpiNBody) {
    iNumModules++;
  }
  if (body[iBody].bMagmOc) {
    iNumModules++;
  }
  if (body[iBody].bEqtide && body[iBody].bStellar) {
    iNumModuleMulti++;
  }

  module->iNumModules[iBody]      = iNumModules;
  module->iNumManageDerivs[iBody] = iNumModules + iNumModuleMulti;
  module->iaModule[iBody]         = malloc(iNumModules * sizeof(int));

  module->fnLogBody[iBody] = malloc(iNumModules * sizeof(fnLogBodyModule));
  module->fnInitializeControl[iBody] =
        malloc(iNumModules * sizeof(fnInitializeControlModule));
  module->fnInitializeOutput[iBody] =
        malloc(iNumModules * sizeof(fnInitializeOutputModule));
  module->fnInitializeUpdateTmpBody[iBody] =
        malloc(iNumModules * sizeof(fnInitializeUpdateTmpBodyModule));

  module->fnCountHalts[iBody] =
        malloc(iNumModules * sizeof(fnCountHaltsModule));
  module->fnReadOptions[iBody] =
        malloc(iNumModules * sizeof(fnReadOptionsModule));
  module->fnVerify[iBody] = malloc(iNumModules * sizeof(fnVerifyModule));
  module->fnAssignDerivatives[iBody] = malloc(
        (iNumModules + iNumModuleMulti) * sizeof(fnManageModuleDerivatives));
  module->fnNullDerivatives[iBody] = malloc((iNumModules + iNumModuleMulti) *
                                            sizeof(fnManageModuleDerivatives));
  module->fnVerifyHalt[iBody] =
        malloc(iNumModules * sizeof(fnVerifyHaltModule));

  module->fnInitializeBody[iBody] =
        malloc(iNumModules * sizeof(fnInitializeBodyModule));
  module->fnInitializeUpdate[iBody] =
        malloc(iNumModules * sizeof(fnInitializeUpdateModule));

  // Finalize Primary Variable Functions
  module->fnFinalizeUpdate26AlNumCore[iBody] =
        malloc(iNumModules * sizeof(fnFinalizeUpdate26AlNumCoreModule));
  module->fnFinalizeUpdate26AlNumMan[iBody] =
        malloc(iNumModules * sizeof(fnFinalizeUpdate26AlNumManModule));
  module->fnFinalizeUpdate40KNumCore[iBody] =
        malloc(iNumModules * sizeof(fnFinalizeUpdate40KNumCoreModule));
  module->fnFinalizeUpdate40KNumCrust[iBody] =
        malloc(iNumModules * sizeof(fnFinalizeUpdate40KNumCrustModule));
  module->fnFinalizeUpdate40KNumMan[iBody] =
        malloc(iNumModules * sizeof(fnFinalizeUpdate40KNumManModule));
  module->fnFinalizeUpdate232ThNumCore[iBody] =
        malloc(iNumModules * sizeof(fnFinalizeUpdate232ThNumCoreModule));
  module->fnFinalizeUpdate232ThNumCrust[iBody] =
        malloc(iNumModules * sizeof(fnFinalizeUpdate232ThNumCrustModule));
  module->fnFinalizeUpdate232ThNumMan[iBody] =
        malloc(iNumModules * sizeof(fnFinalizeUpdate232ThNumManModule));
  module->fnFinalizeUpdate235UNumCore[iBody] =
        malloc(iNumModules * sizeof(fnFinalizeUpdate235UNumCoreModule));
  module->fnFinalizeUpdate235UNumCrust[iBody] =
        malloc(iNumModules * sizeof(fnFinalizeUpdate235UNumCrustModule));
  module->fnFinalizeUpdate235UNumMan[iBody] =
        malloc(iNumModules * sizeof(fnFinalizeUpdate235UNumManModule));
  module->fnFinalizeUpdate238UNumCore[iBody] =
        malloc(iNumModules * sizeof(fnFinalizeUpdate238UNumCoreModule));
  module->fnFinalizeUpdate238UNumCrust[iBody] =
        malloc(iNumModules * sizeof(fnFinalizeUpdate238UNumCrustModule));
  module->fnFinalizeUpdate238UNumMan[iBody] =
        malloc(iNumModules * sizeof(fnFinalizeUpdate238UNumManModule));

  module->fnFinalizeUpdateCBPR[iBody] =
        malloc(iNumModules * sizeof(fnFinalizeUpdateCBPRModule));
  module->fnFinalizeUpdateCBPZ[iBody] =
        malloc(iNumModules * sizeof(fnFinalizeUpdateCBPZModule));
  module->fnFinalizeUpdateCBPPhi[iBody] =
        malloc(iNumModules * sizeof(fnFinalizeUpdateCBPPhiModule));
  module->fnFinalizeUpdateCBPRDot[iBody] =
        malloc(iNumModules * sizeof(fnFinalizeUpdateCBPRDotModule));
  module->fnFinalizeUpdateCBPZDot[iBody] =
        malloc(iNumModules * sizeof(fnFinalizeUpdateCBPZDotModule));
  module->fnFinalizeUpdateCBPPhiDot[iBody] =
        malloc(iNumModules * sizeof(fnFinalizeUpdateCBPPhiDotModule));

  module->fnFinalizeUpdateDynEllip[iBody] =
        malloc(iNumModules * sizeof(fnFinalizeUpdateDynEllipModule));
  module->fnFinalizeUpdateEnvelopeMass[iBody] =
        malloc(iNumModules * sizeof(fnFinalizeUpdateEnvelopeMassModule));
  module->fnFinalizeUpdateHecc[iBody] =
        malloc(iNumModules * sizeof(fnFinalizeUpdateHeccModule));
  module->fnFinalizeUpdateKecc[iBody] =
        malloc(iNumModules * sizeof(fnFinalizeUpdateKeccModule));
  module->fnFinalizeUpdateLuminosity[iBody] =
        malloc(iNumModules * sizeof(fnFinalizeUpdateLuminosityModule));
  module->fnFinalizeUpdateLXUV[iBody] =
        malloc(iNumModules * sizeof(fnFinalizeUpdateMassModule));
  module->fnFinalizeUpdateMass[iBody] =
        malloc(iNumModules * sizeof(fnFinalizeUpdateMassModule));
  module->fnFinalizeUpdateOxygenMass[iBody] =
        malloc(iNumModules * sizeof(fnFinalizeUpdateOxygenMassModule));
  module->fnFinalizeUpdateOxygenMantleMass[iBody] =
        malloc(iNumModules * sizeof(fnFinalizeUpdateOxygenMantleMassModule));
  module->fnFinalizeUpdatePinc[iBody] =
        malloc(iNumModules * sizeof(fnFinalizeUpdatePincModule));
  module->fnFinalizeUpdateQinc[iBody] =
        malloc(iNumModules * sizeof(fnFinalizeUpdateQincModule));

  module->fnFinalizeUpdateRadius[iBody] =
        malloc(iNumModules * sizeof(fnFinalizeUpdateRadiusModule));
  module->fnFinalizeUpdateRadGyra[iBody] =
        malloc(iNumModules * sizeof(fnFinalizeUpdateRadGyraModule));
  module->fnFinalizeUpdateRot[iBody] =
        malloc(iNumModules * sizeof(fnFinalizeUpdateRotModule));
  module->fnFinalizeUpdateSemi[iBody] =
        malloc(iNumModules * sizeof(fnFinalizeUpdateSemiModule));
  module->fnFinalizeUpdateSurfaceWaterMass[iBody] =
        malloc(iNumModules * sizeof(fnFinalizeUpdateSurfaceWaterMassModule));
  module->fnFinalizeUpdateTCore[iBody] =
        malloc(iNumModules * sizeof(fnFinalizeUpdateTCoreModule));
  module->fnFinalizeUpdateTMan[iBody] =
        malloc(iNumModules * sizeof(fnFinalizeUpdateTManModule));
  module->fnFinalizeUpdateXobl[iBody] =
        malloc(iNumModules * sizeof(fnFinalizeUpdateXoblModule));
  module->fnFinalizeUpdateYobl[iBody] =
        malloc(iNumModules * sizeof(fnFinalizeUpdateYoblModule));
  module->fnFinalizeUpdateZobl[iBody] =
        malloc(iNumModules * sizeof(fnFinalizeUpdateZoblModule));
  module->fnFinalizeUpdateTemperature[iBody] =
        malloc(iNumModules * sizeof(fnFinalizeUpdateTemperatureModule));
  module->fnFinalizeUpdateLostAngMom[iBody] =
        malloc(iNumModules * sizeof(fnFinalizeUpdateLostAngMomModule));
  module->fnFinalizeUpdateLostEng[iBody] =
        malloc(iNumModules * sizeof(fnFinalizeUpdateLostEngModule));

  module->fnFinalizeUpdateEccX[iBody] =
        malloc(iNumModules * sizeof(fnFinalizeUpdateEccXModule));
  module->fnFinalizeUpdateEccY[iBody] =
        malloc(iNumModules * sizeof(fnFinalizeUpdateEccYModule));
  module->fnFinalizeUpdateEccZ[iBody] =
        malloc(iNumModules * sizeof(fnFinalizeUpdateEccZModule));
  module->fnFinalizeUpdateAngMX[iBody] =
        malloc(iNumModules * sizeof(fnFinalizeUpdateAngMXModule));
  module->fnFinalizeUpdateAngMY[iBody] =
        malloc(iNumModules * sizeof(fnFinalizeUpdateAngMYModule));
  module->fnFinalizeUpdateAngMZ[iBody] =
        malloc(iNumModules * sizeof(fnFinalizeUpdateAngMZModule));

  module->fnFinalizeUpdatePositionX[iBody] =
        malloc(iNumModules * sizeof(fnFinalizeUpdatePositionXModule));
  module->fnFinalizeUpdatePositionY[iBody] =
        malloc(iNumModules * sizeof(fnFinalizeUpdatePositionYModule));
  module->fnFinalizeUpdatePositionZ[iBody] =
        malloc(iNumModules * sizeof(fnFinalizeUpdatePositionZModule));
  module->fnFinalizeUpdateVelX[iBody] =
        malloc(iNumModules * sizeof(fnFinalizeUpdateVelXModule));
  module->fnFinalizeUpdateVelY[iBody] =
        malloc(iNumModules * sizeof(fnFinalizeUpdateVelYModule));
  module->fnFinalizeUpdateVelZ[iBody] =
        malloc(iNumModules * sizeof(fnFinalizeUpdateVelZModule));

  module->fnFinalizeUpdateWaterMassMOAtm[iBody] =
        malloc(iNumModules * sizeof(fnFinalizeUpdateWaterMassMOAtmModule));
  module->fnFinalizeUpdateWaterMassSol[iBody] =
        malloc(iNumModules * sizeof(fnFinalizeUpdateWaterMassSolModule));
  module->fnFinalizeUpdateSurfTemp[iBody] =
        malloc(iNumModules * sizeof(fnFinalizeUpdateSurfTempModule));
  module->fnFinalizeUpdatePotTemp[iBody] =
        malloc(iNumModules * sizeof(fnFinalizeUpdatePotTempModule));
  module->fnFinalizeUpdateSolidRadius[iBody] =
        malloc(iNumModules * sizeof(fnFinalizeUpdateSolidRadiusModule));
  module->fnFinalizeUpdateOxygenMassMOAtm[iBody] =
        malloc(iNumModules * sizeof(fnFinalizeUpdateOxygenMassMOAtmModule));
  module->fnFinalizeUpdateOxygenMassSol[iBody] =
        malloc(iNumModules * sizeof(fnFinalizeUpdateOxygenMassSolModule));
  module->fnFinalizeUpdateHydrogenMassSpace[iBody] =
        malloc(iNumModules * sizeof(fnFinalizeUpdateHydrogenMassSpaceModule));
  module->fnFinalizeUpdateOxygenMassSpace[iBody] =
        malloc(iNumModules * sizeof(fnFinalizeUpdateOxygenMassSpaceModule));
  module->fnFinalizeUpdateCO2MassMOAtm[iBody] =
        malloc(iNumModules * sizeof(fnFinalizeUpdateCO2MassMOAtmModule));
  module->fnFinalizeUpdateCO2MassSol[iBody] =
        malloc(iNumModules * sizeof(fnFinalizeUpdateCO2MassSolModule));

  for (iModule = 0; iModule < (iNumModules); iModule++) {
    /* Initialize all module functions pointers to point to their respective
       NULL function. The modules that need actual function will replace them
       in AddModule. */

    module->fnInitializeControl[iBody][iModule] = &InitializeControlNULL;
    module->fnInitializeUpdateTmpBody[iBody][iModule] =
          &InitializeUpdateTmpBodyNULL;
    module->fnInitializeBody[iBody][iModule] = &InitializeBodyNULL;

    module->fnFinalizeUpdate26AlNumCore[iBody][iModule]   = &FinalizeUpdateNULL;
    module->fnFinalizeUpdate26AlNumMan[iBody][iModule]    = &FinalizeUpdateNULL;
    module->fnFinalizeUpdate40KNumCore[iBody][iModule]    = &FinalizeUpdateNULL;
    module->fnFinalizeUpdate40KNumCrust[iBody][iModule]   = &FinalizeUpdateNULL;
    module->fnFinalizeUpdate40KNumMan[iBody][iModule]     = &FinalizeUpdateNULL;
    module->fnFinalizeUpdate232ThNumCore[iBody][iModule]  = &FinalizeUpdateNULL;
    module->fnFinalizeUpdate232ThNumCrust[iBody][iModule] = &FinalizeUpdateNULL;
    module->fnFinalizeUpdate232ThNumMan[iBody][iModule]   = &FinalizeUpdateNULL;
    module->fnFinalizeUpdate235UNumCore[iBody][iModule]   = &FinalizeUpdateNULL;
    module->fnFinalizeUpdate235UNumCrust[iBody][iModule]  = &FinalizeUpdateNULL;
    module->fnFinalizeUpdate235UNumMan[iBody][iModule]    = &FinalizeUpdateNULL;
    module->fnFinalizeUpdate238UNumCore[iBody][iModule]   = &FinalizeUpdateNULL;
    module->fnFinalizeUpdate238UNumCrust[iBody][iModule]  = &FinalizeUpdateNULL;
    module->fnFinalizeUpdate238UNumMan[iBody][iModule]    = &FinalizeUpdateNULL;

    module->fnFinalizeUpdateCBPR[iBody][iModule]      = &FinalizeUpdateNULL;
    module->fnFinalizeUpdateCBPZ[iBody][iModule]      = &FinalizeUpdateNULL;
    module->fnFinalizeUpdateCBPPhi[iBody][iModule]    = &FinalizeUpdateNULL;
    module->fnFinalizeUpdateCBPRDot[iBody][iModule]   = &FinalizeUpdateNULL;
    module->fnFinalizeUpdateCBPZDot[iBody][iModule]   = &FinalizeUpdateNULL;
    module->fnFinalizeUpdateCBPPhiDot[iBody][iModule] = &FinalizeUpdateNULL;

    module->fnFinalizeUpdateDynEllip[iBody][iModule]     = &FinalizeUpdateNULL;
    module->fnFinalizeUpdateEnvelopeMass[iBody][iModule] = &FinalizeUpdateNULL;
    module->fnFinalizeUpdateHecc[iBody][iModule]         = &FinalizeUpdateNULL;
    module->fnFinalizeUpdateKecc[iBody][iModule]         = &FinalizeUpdateNULL;
    module->fnFinalizeUpdateLuminosity[iBody][iModule]   = &FinalizeUpdateNULL;
    module->fnFinalizeUpdateLXUV[iBody][iModule]         = &FinalizeUpdateNULL;
    module->fnFinalizeUpdateMass[iBody][iModule]         = &FinalizeUpdateNULL;
    module->fnFinalizeUpdateOxygenMass[iBody][iModule]   = &FinalizeUpdateNULL;
    module->fnFinalizeUpdateOxygenMantleMass[iBody][iModule] =
          &FinalizeUpdateNULL;
    module->fnFinalizeUpdatePinc[iBody][iModule] = &FinalizeUpdateNULL;
    module->fnFinalizeUpdateQinc[iBody][iModule] = &FinalizeUpdateNULL;

    module->fnFinalizeUpdateRadius[iBody][iModule]  = &FinalizeUpdateNULL;
    module->fnFinalizeUpdateRadGyra[iBody][iModule] = &FinalizeUpdateNULL;
    module->fnFinalizeUpdateRot[iBody][iModule]     = &FinalizeUpdateNULL;
    module->fnFinalizeUpdateSemi[iBody][iModule]    = &FinalizeUpdateNULL;
    module->fnFinalizeUpdateSurfaceWaterMass[iBody][iModule] =
          &FinalizeUpdateNULL;
    module->fnFinalizeUpdateTemperature[iBody][iModule] = &FinalizeUpdateNULL;
    module->fnFinalizeUpdateTMan[iBody][iModule]        = &FinalizeUpdateNULL;
    module->fnFinalizeUpdateTCore[iBody][iModule]       = &FinalizeUpdateNULL;
    module->fnFinalizeUpdateXobl[iBody][iModule]        = &FinalizeUpdateNULL;
    module->fnFinalizeUpdateYobl[iBody][iModule]        = &FinalizeUpdateNULL;
    module->fnFinalizeUpdateZobl[iBody][iModule]        = &FinalizeUpdateNULL;
    module->fnFinalizeUpdateLostAngMom[iBody][iModule]  = &FinalizeUpdateNULL;
    module->fnFinalizeUpdateLostEng[iBody][iModule]     = &FinalizeUpdateNULL;

    module->fnFinalizeUpdateEccX[iBody][iModule]  = &FinalizeUpdateNULL;
    module->fnFinalizeUpdateEccY[iBody][iModule]  = &FinalizeUpdateNULL;
    module->fnFinalizeUpdateEccZ[iBody][iModule]  = &FinalizeUpdateNULL;
    module->fnFinalizeUpdateAngMX[iBody][iModule] = &FinalizeUpdateNULL;
    module->fnFinalizeUpdateAngMY[iBody][iModule] = &FinalizeUpdateNULL;
    module->fnFinalizeUpdateAngMZ[iBody][iModule] = &FinalizeUpdateNULL;

    module->fnFinalizeUpdatePositionX[iBody][iModule] = &FinalizeUpdateNULL;
    module->fnFinalizeUpdatePositionY[iBody][iModule] = &FinalizeUpdateNULL;
    module->fnFinalizeUpdatePositionZ[iBody][iModule] = &FinalizeUpdateNULL;
    module->fnFinalizeUpdateVelX[iBody][iModule]      = &FinalizeUpdateNULL;
    module->fnFinalizeUpdateVelY[iBody][iModule]      = &FinalizeUpdateNULL;
    module->fnFinalizeUpdateVelZ[iBody][iModule]      = &FinalizeUpdateNULL;

    module->fnFinalizeUpdateWaterMassMOAtm[iBody][iModule] =
          &FinalizeUpdateNULL;
    module->fnFinalizeUpdateWaterMassSol[iBody][iModule] = &FinalizeUpdateNULL;
    module->fnFinalizeUpdateSurfTemp[iBody][iModule]     = &FinalizeUpdateNULL;
    module->fnFinalizeUpdatePotTemp[iBody][iModule]      = &FinalizeUpdateNULL;
    module->fnFinalizeUpdateSolidRadius[iBody][iModule]  = &FinalizeUpdateNULL;
    module->fnFinalizeUpdateOxygenMassMOAtm[iBody][iModule] =
          &FinalizeUpdateNULL;
    module->fnFinalizeUpdateOxygenMassSol[iBody][iModule] = &FinalizeUpdateNULL;
    module->fnFinalizeUpdateHydrogenMassSpace[iBody][iModule] =
          &FinalizeUpdateNULL;
    module->fnFinalizeUpdateOxygenMassSpace[iBody][iModule] =
          &FinalizeUpdateNULL;
    module->fnFinalizeUpdateCO2MassMOAtm[iBody][iModule] = &FinalizeUpdateNULL;
    module->fnFinalizeUpdateCO2MassSol[iBody][iModule]   = &FinalizeUpdateNULL;
  }
}

void AddModulesMulti(BODY *body, CONTROL *control, MODULE *module, int iBody,
                     int *iModule) {

  if (body[iBody].bEqtide && body[iBody].bStellar) {
    module->fnAssignDerivatives[iBody][*iModule] =
          &AssignEqtideStellarDerivatives;
    module->fnNullDerivatives[iBody][*iModule] = &NullEqtideStellarDerivatives;
    module->iaEqtideStellar[iBody]             = *iModule;
    (*iModule)++;
  }
}

void AddModules(BODY *body, CONTROL *control, MODULE *module) {
  int iBody, iModule;

  /************************
   * ADD NEW MODULES HERE *
   ************************/

  for (iBody = 0; iBody < control->Evolve.iNumBodies; iBody++) {
    iModule = 0;

    if (body[iBody].bEqtide) {
      AddModuleEqtide(control, module, iBody, iModule);
      module->iaEqtide[iBody]            = iModule;
      module->iaModule[iBody][iModule++] = EQTIDE;
    }
    if (body[iBody].bDistOrb) {
      AddModuleDistOrb(control, module, iBody, iModule);
      module->iaDistOrb[iBody]           = iModule;
      module->iaModule[iBody][iModule++] = DISTORB;
    }
    if (body[iBody].bDistRot) {
      AddModuleDistRot(control, module, iBody, iModule);
      module->iaDistRot[iBody]           = iModule;
      module->iaModule[iBody][iModule++] = DISTROT;
    }
    if (body[iBody].bRadheat) {
      fvAddModuleRadheat(control, module, iBody, iModule);
      module->iaRadheat[iBody]           = iModule;
      module->iaModule[iBody][iModule++] = RADHEAT;
    }
    if (body[iBody].bThermint) {
      fvAddModuleThermint(control, module, iBody, iModule);
      module->iaThermint[iBody]          = iModule;
      module->iaModule[iBody][iModule++] = THERMINT;
    }
    if (body[iBody].bAtmEsc) {
      AddModuleAtmEsc(control, module, iBody, iModule);
      module->iaAtmEsc[iBody]            = iModule;
      module->iaModule[iBody][iModule++] = ATMESC;
    }
    if (body[iBody].bStellar) {
      AddModuleStellar(control, module, iBody, iModule);
      module->iaStellar[iBody]           = iModule;
      module->iaModule[iBody][iModule++] = STELLAR;
    }
    if (body[iBody].bPoise) {
      AddModulePoise(control, module, iBody, iModule);
      module->iaPoise[iBody]             = iModule;
      module->iaModule[iBody][iModule++] = POISE;
    }
    if (body[iBody].bBinary) {
      AddModuleBinary(control, module, iBody, iModule);
      module->iaBinary[iBody]            = iModule;
      module->iaModule[iBody][iModule++] = BINARY;
    }
    if (body[iBody].bFlare) {
      AddModuleFlare(control, module, iBody, iModule);
      module->iaFlare[iBody]             = iModule;
      module->iaModule[iBody][iModule++] = FLARE;
    }
    if (body[iBody].bGalHabit) {
      AddModuleGalHabit(control, module, iBody, iModule);
      module->iaGalHabit[iBody]          = iModule;
      module->iaModule[iBody][iModule++] = GALHABIT;
    }
    if (body[iBody].bSpiNBody) {
      AddModuleSpiNBody(control, module, iBody, iModule);
      module->iaSpiNBody[iBody]          = iModule;
      module->iaModule[iBody][iModule++] = SPINBODY;
    }
    if (body[iBody].bMagmOc) {
      AddModuleMagmOc(module, iBody, iModule);
      module->iaMagmOc[iBody]            = iModule;
      module->iaModule[iBody][iModule++] = MAGMOC;
    }
    AddModulesMulti(body, control, module, iBody, &iModule);
  }
}

void ReadModules(BODY *body, CONTROL *control, FILES *files, MODULE *module,
                 OPTIONS *options, int iFile) {
  int iNumIndices = 0, iNumLines = 0, iModule;
  int *lTmp;
  char **saTmp;

  lTmp = malloc(MAXLINES * sizeof(int));

  AddOptionStringArray(files->Infile[iFile].cIn, options->cName, &saTmp,
                       &iNumIndices, &iNumLines, lTmp, control->Io.iVerbose);
  if (lTmp[0] >= 0) {
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp[0],
                    control->Io.iVerbose);
    if (iNumIndices == 0) {
      if (control->Io.iVerbose >= VERBERR) {
        fprintf(stderr, "ERROR: No modules input to option %s.\n",
                options->cName);
      }
      LineExit(files->Infile[iFile].cIn, lTmp[0]);
    }

    /* The iBitSum field is to check that every output parameter is part
       of the input modules. The variables EQTIDE, RADHEAT, etc. are set
       in vpanet.h. Each module-specific parameter is given the appropriate
       bit in InitializeOutput. In ReadOutputOrder the field
       output->iModuleBit is compared bitwise to module->iBitSum.
       Parameters that can be specified for multiple modules are set to 1.
       iBitSum for each body is set 1 in InitializeModule.
    */

    for (iModule = 0; iModule < iNumIndices; iModule++) {

      /************************
       * ADD NEW MODULES HERE *
       ************************/

      int iModuleLength = strlen(saTmp[iModule]);
      // if (memcmp(sLower(saTmp[iModule]), "eqtide", iModuleLength) == 0) {
      if (strcmp(sLower(saTmp[iModule]), "eqtide") == 0) {
        body[iFile - 1].bEqtide = 1;
        module->iBitSum[iFile - 1] += EQTIDE;
      // } else if (memcmp(sLower(saTmp[iModule]), "radheat", iModuleLength) == 0) {
      } else if (strcmp(sLower(saTmp[iModule]), "radheat") == 0) {
        body[iFile - 1].bRadheat = 1;
        module->iBitSum[iFile - 1] += RADHEAT;
      //} else if (memcmp(sLower(saTmp[iModule]), "distorb", iModuleLength) == 0) {
      } else if (strcmp(sLower(saTmp[iModule]), "distorb") == 0) {
        body[iFile - 1].bDistOrb = 1;
        module->iBitSum[iFile - 1] += DISTORB;
      // } else if (memcmp(sLower(saTmp[iModule]), "distrot", iModuleLength) == 0) {
      } else if (strcmp(sLower(saTmp[iModule]), "distrot") == 0) {
        body[iFile - 1].bDistRot = 1;
        module->iBitSum[iFile - 1] += DISTROT;
      // } else if (memcmp(sLower(saTmp[iModule]), "thermint", iModuleLength) == 0) {
      } else if (strcmp(sLower(saTmp[iModule]), "thermint") == 0) {
        body[iFile - 1].bThermint = 1;
        module->iBitSum[iFile - 1] += THERMINT;
      // } else if (memcmp(sLower(saTmp[iModule]), "atmesc", iModuleLength) == 0) {
      } else if (strcmp(sLower(saTmp[iModule]), "atmesc") == 0) {
        body[iFile - 1].bAtmEsc = 1;
        module->iBitSum[iFile - 1] += ATMESC;
      // } else if (memcmp(sLower(saTmp[iModule]), "stellar", iModuleLength) == 0) {
      } else if (strcmp(sLower(saTmp[iModule]), "stellar") == 0) {
        body[iFile - 1].bStellar = 1;
        module->iBitSum[iFile - 1] += STELLAR;
      // } else if (memcmp(sLower(saTmp[iModule]), "poise", iModuleLength) == 0) {
      } else if (strcmp(sLower(saTmp[iModule]), "poise") == 0) {
        body[iFile - 1].bPoise = 1;
        module->iBitSum[iFile - 1] += POISE;
      // } else if (memcmp(sLower(saTmp[iModule]), "binary", iModuleLength) == 0) {
      } else if (strcmp(sLower(saTmp[iModule]), "binary") == 0) {
        body[iFile - 1].bBinary = 1;
        module->iBitSum[iFile - 1] += BINARY;
      // } else if (memcmp(sLower(saTmp[iModule]), "flare", iModuleLength) == 0) {
      } else if (strcmp(sLower(saTmp[iModule]), "flare") == 0) {
        body[iFile - 1].bFlare = 1;
        module->iBitSum[iFile - 1] += FLARE;
      // } else if (memcmp(sLower(saTmp[iModule]), "galhabit", iModuleLength) == 0) {
      } else if (strcmp(sLower(saTmp[iModule]), "galhabit") == 0) {
        body[iFile - 1].bGalHabit = 1;
        module->iBitSum[iFile - 1] += GALHABIT;
      // } else if (memcmp(sLower(saTmp[iModule]), "spinbody", iModuleLength) == 0) {
      } else if (strcmp(sLower(saTmp[iModule]), "spinbody") == 0) {
        body[iFile - 1].bSpiNBody = 1;
        module->iBitSum[iFile - 1] += SPINBODY;
      // } else if (memcmp(sLower(saTmp[iModule]), "magmoc", iModuleLength) == 0) {
      } else if (strcmp(sLower(saTmp[iModule]), "magmoc") == 0) {
        body[iFile - 1].bMagmOc = 1;
        module->iBitSum[iFile - 1] += MAGMOC;
      } else {
        if (control->Io.iVerbose >= VERBERR) {
          fprintf(stderr, "ERROR: Unknown Module %s provided to %s.\n",
                  saTmp[iModule], options->cName);
        }
        LineExit(files->Infile[iFile].cIn, lTmp[0]);
      }
    }
    UpdateFoundOptionMulti(&files->Infile[iFile], options, lTmp, iNumLines,
                           iFile);

  } else {
    if (control->Io.iVerbose >= VERBERR && iFile > 0) {
      fprintf(stderr,
              "WARNING: %s not present in file %s. No evolution will occur for "
              "this body.\n",
              options->cName, files->Infile[iFile].cIn);
    }
  }
  free(lTmp);
  free(saTmp);
}

void PrintModuleList(FILE *file, int iBitSum, int bPadString) {
  int space   = 0;
  int nspaces = 65;
  int iSpace;

  if (iBitSum & ATMESC) {
    if (space) {
      fprintf(file, " ");
    }
    space++;
    fprintf(file, "AtmEsc");
    nspaces -= strlen("AtmEsc");
  }
  if (iBitSum & BINARY) {
    if (space) {
      fprintf(file, " ");
    }
    space++;
    fprintf(file, "BINARY");
    nspaces -= strlen("BINARY");
  }
  if (iBitSum & DISTORB) {
    if (space) {
      fprintf(file, " ");
    }
    space++;
    fprintf(file, "DistOrb");
    nspaces -= strlen("DistOrb");
  }
  if (iBitSum & DISTROT) {
    if (space) {
      fprintf(file, " ");
    }
    space++;
    fprintf(file, "DistRot");
    nspaces -= strlen("DistRot");
  }
  if (iBitSum & EQTIDE) {
    if (space) {
      fprintf(file, " ");
    }
    space++;
    fprintf(file, "EqTide");
    nspaces -= strlen("EqTide");
  }
  if (iBitSum & FLARE) {
    if (space) {
      fprintf(file, " ");
    }
    space++;
    fprintf(file, "FLARE");
    nspaces -= strlen("FLARE");
  }
  if (iBitSum & GALHABIT) {
    if (space) {
      fprintf(file, " ");
    }
    space++;
    fprintf(file, "GalHabit");
    nspaces -= strlen("GalHabit");
  }
  if (iBitSum & MAGMOC) {
    if (space) {
      fprintf(file, " ");
    }
    space++;
    fprintf(file, "MagmOc");
    nspaces -= strlen("MagmOc");
  }
  if (iBitSum & POISE) {
    if (space) {
      fprintf(file, " ");
    }
    space++;
    fprintf(file, "POISE");
    nspaces -= strlen("POISE");
  }
  if (iBitSum & RADHEAT) {
    if (space) {
      fprintf(file, " ");
    }
    space++;
    fprintf(file, "RadHeat");
    nspaces -= strlen("RadHeat");
  }
  if (iBitSum & SPINBODY) {
    if (space) {
      fprintf(file, " ");
    }
    space++;
    fprintf(file, "SpiNBody");
    nspaces -= strlen("SpiNBody");
  }
  if (iBitSum & STELLAR) {
    if (space) {
      fprintf(file, " ");
    }
    space++;
    fprintf(file, "STELLAR");
    nspaces -= strlen("STELLAR");
  }
  if (iBitSum & THERMINT) {
    if (space) {
      fprintf(file, " ");
    }
    space++;
    fprintf(file, "ThermInt");
    nspaces -= strlen("ThermInt");
  }
  if (bPadString) {
    if (space)
      nspaces -= (space - 1);
    for (iSpace = 0; iSpace < nspaces; iSpace++) {
      printf(" ");
    }
  }
}

void InitializeBodyModules(BODY **body, int iNumBodies) {
  int iBody;

  for (iBody = 0; iBody < iNumBodies; iBody++) {
    (*body)[iBody].bAtmEsc   = 0;
    (*body)[iBody].bBinary   = 0;
    (*body)[iBody].bDistOrb  = 0;
    (*body)[iBody].bDistRot  = 0;
    (*body)[iBody].bEqtide   = 0;
    (*body)[iBody].bFlare    = 0;
    (*body)[iBody].bGalHabit = 0;
    (*body)[iBody].bPoise    = 0;
    (*body)[iBody].bRadheat  = 0;
    (*body)[iBody].bStellar  = 0;
    (*body)[iBody].bThermint = 0;
    (*body)[iBody].bSpiNBody = 0;
    (*body)[iBody].bMagmOc   = 0;
  }
}

/*
 * Verify multi-module dependencies
 */

void VerifyModuleMultiSpiNBodyAtmEsc(BODY *body, UPDATE *update,
                                     CONTROL *control, FILES *files,
                                     OPTIONS *options, int iBody,
                                     int *iModuleProps, int *iModuleForce) {
  if (body[iBody].bSpiNBody && body[iBody].bAtmEsc) {
    control->fnForceBehaviorMulti[iBody][(*iModuleForce)++] =
          &ForceBehaviorSpiNBodyAtmEsc;
  }
}


void VerifyModuleMultiDistOrbDistRot(BODY *body, UPDATE *update,
                                     CONTROL *control, FILES *files,
                                     OPTIONS *options, int iBody,
                                     int *iModuleProps, int *iModuleForce) {

  if (body[iBody].bDistRot) {
    if (!body[iBody].bDistOrb) {
      if (!body[iBody].bReadOrbitData) {
        fprintf(stderr,
                "ERROR: Module DISTROT selected for %s, but DISTORB not "
                "selected and bReadOrbitData = 0.\n",
                body[iBody].cName);
        exit(EXIT_INPUT);
      }
    } else {
      if (body[iBody].bReadOrbitData) {
        fprintf(stderr,
                "ERROR: Cannot set both DISTORB and bReadOrbitData for body "
                "%s.\n",
                body[iBody].cName);
        exit(EXIT_INPUT);
      }
    }
  }
}

void VerifyModuleMultiEqtideDistRot(BODY *body, UPDATE *update,
                                    CONTROL *control, FILES *files,
                                    OPTIONS *options, int iBody,
                                    int *iModuleProps, int *iModuleForce) {
  if (body[iBody].bDistRot && body[iBody].bEqtide) {
    if (body[iBody].bReadOrbitData) {
      fprintf(stderr,
              "ERROR: Cannot set both EQTIDE and bReadOrbitData for body %s.\n",
              body[iBody].cName);
      exit(EXIT_INPUT);
    }

    control->fnPropsAuxMulti[iBody][(*iModuleProps)++] = &PropsAuxEqtideDistRot;
  }
}

void VerifyModuleMultiRadheatThermint(BODY *body, UPDATE *update,
                                      CONTROL *control, FILES *files,
                                      OPTIONS *options, int iBody,
                                      int *iModuleProps, int *iModuleForce) {

  if (body[iBody].bThermint) {
    if (!body[iBody].bRadheat) {
      if (control->Io.iVerbose > VERBINPUT) {
        fprintf(stderr,
                "WARNING: Module THERMINT selected for %s, but RADHEAT not "
                "selected.\n",
                body[iBody].cName);
      }
      body[iBody].dRadPowerCore  = 0.;
      body[iBody].dRadPowerCrust = 0.;
      body[iBody].dRadPowerMan   = 0.;
    } else {
      control->fnPropsAuxMulti[iBody][(*iModuleProps)++] =
            &PropsAuxRadheatThermint;
    }
  }
}

void VerifyModuleMultiEqtideThermint(BODY *body, UPDATE *update,
                                     CONTROL *control, FILES *files,
                                     MODULE *module, OPTIONS *options,
                                     int iBody, int *iModuleProps,
                                     int *iModuleForce) {

  if (body[iBody].bEqtide && body[iBody].bThermint) {
    if (control->Evolve.iEqtideModel == CTL) {
      fprintf(stderr,"ERROR: The CTL EqTide model cannot be coupled to ThermInt.\n");
      exit(EXIT_INPUT);
    }
  }

  body[iBody].dTidalPowMan = 0;

  if (body[iBody].bEqtide && body[iBody].bThermint) {
    if (options[OPT_TIDALQ].iLine[iBody + 1] != -1) {
      if (control->Io.iVerbose >= VERBINPUT) {
        fprintf(stderr,
                "INFO: Option %s set, but module ThermInt also selected. The "
                "tidal Q will be calculated by Thermint.\n",
                options[OPT_TIDALQ].cName);
      }
    }

    if (options[OPT_K2].iLine[iBody + 1] != -1) {
      if (control->Io.iVerbose >= VERBINPUT) {
        fprintf(stderr,
                "INFO: Option %s set, but module ThermInt also selected. ",
                options[OPT_K2].cName);
        fprintf(stderr,
                "The Love number k_2 will be calculated by Thermint.\n");
      }
    }

    control->fnPropsAuxMulti[iBody][(*iModuleProps)++] =
          &PropsAuxEqtideThermint;
  }
}

void VerifyModuleMultiEqtideDistOrb(BODY *body, UPDATE *update,
                                    CONTROL *control, FILES *files,
                                    MODULE *module, OPTIONS *options, int iBody,
                                    int *iModuleProps, int *iModuleForce) {
  if (body[iBody].bEqtide) {
    if (body[iBody].bDistOrb) {
      control->fnForceBehaviorMulti[iBody][(*iModuleForce)++] =
            &ForceBehaviorEqtideDistOrb;
    }
  }
}

void AssignEqtideStellarDerivatives(BODY *body, EVOLVE *evolve, UPDATE *update,
                                    fnUpdateVariable ***fnUpdate, int iBody) {
  if ((body[iBody].iBodyType == 1) && (iBody == 1)) {
    fnUpdate[iBody][update[iBody].iSemi][update[iBody].iSemiEqSt] =
          &fdSemiDtEqSt;
  }
}

void NullEqtideStellarDerivatives(BODY *body, EVOLVE *evolve, UPDATE *update,
                                  fnUpdateVariable ***fnUpdate, int iBody) {
  if ((body[iBody].iBodyType == 1) && (iBody == 1)) {
    fnUpdate[iBody][update[iBody].iSemi][update[iBody].iSemiEqSt] =
          &fndUpdateFunctionTiny;
  }
}


void VerifyModuleMultiEqtideStellar(BODY *body, UPDATE *update,
                                    CONTROL *control, FILES *files,
                                    MODULE *module, OPTIONS *options, int iBody,
                                    int *iModuleProps, int *iModuleForce) {
  int iOtherBody, iEqn;

  if (body[iBody].bEqtide) {
    if (body[iBody].bStellar) {

      // Only call EQTIDE and STELLAR together for 1st 2 bodies for a binary
      // star system
      if (iBody > 1) {
        if (control->Io.iVerbose >= VERBINPUT) {
          fprintf(stderr,
                  "ERROR: Can't call EQTIDE and STELLAR for body %d.  Only 0 "
                  "and 1 for a binary system!\n",
                  iBody);
        }
        exit(EXIT_INPUT);
      }

      // If you're using stellar and eqtide and this isn't the primary body, it
      // better have iBodyType == 1 (for a star)
      if (iBody > 0 && body[iBody].iBodyType != 1) {
        if (control->Io.iVerbose >= VERBINPUT) {
          fprintf(stderr, "ERROR: If both stellar AND eqtide are set and iBody "
                          "> 0, MUST set iBodyType == 1 for stars\n");
        }
        exit(EXIT_INPUT);
      }

      // Can't have any ocean, envelope tidal parameters set
      // Body is a star, but has an ocean or an envelope!
      if (body[iBody].bOcean || body[iBody].bEnv) {
        if (control->Io.iVerbose >= VERBINPUT) {
          fprintf(stderr, "ERROR: If both stellar AND eqtide are set, body "
                          "cannot set bOceanTides or bEnvTides!\n");
        }
        LineExit(files->Infile[iBody + 1].cIn,
                 options[OPT_MODULES].iLine[iBody + 1]);
      }

      if (options[OPT_TIDALQOCEAN].iLine[iBody + 1] > -1) {
        if (control->Io.iVerbose >= VERBINPUT) {
          fprintf(stderr, "ERROR: %s set, but this body is a star!.\n",
                  options[OPT_TIDALQOCEAN].cName);
        }
        exit(EXIT_INPUT);
      }
      if (options[OPT_K2OCEAN].iLine[iBody + 1] > -1) {
        if (control->Io.iVerbose >= VERBINPUT) {
          fprintf(stderr, "ERROR: %s set, but this body is a star!.\n",
                  options[OPT_K2OCEAN].cName);
        }
        exit(EXIT_INPUT);
      }
      if (options[OPT_TIDALQENV].iLine[iBody + 1] > -1) {
        if (control->Io.iVerbose >= VERBINPUT) {
          fprintf(stderr, "ERROR: %s set, but this body is a star!.\n",
                  options[OPT_TIDALQENV].cName);
        }
        exit(EXIT_INPUT);
      }
      if (options[OPT_K2ENV].iLine[iBody + 1] > -1) {
        if (control->Io.iVerbose >= VERBINPUT) {
          fprintf(stderr, "ERROR: %s set, but this body is a star!.\n",
                  options[OPT_K2ENV].cName);
        }
        exit(EXIT_INPUT);
      }

      // ALl the options are ok! Add in the necessary AuxProps
      control->fnPropsAuxMulti[iBody][(*iModuleProps)++] =
            &PropsAuxEqtideStellar;
    }
  }

  // This equation only valid if EQTIDE, and STELLAR used for 2nd body
  if ((body[iBody].iBodyType == 1) && body[iBody].bStellar &&
      body[iBody].bEqtide && iBody == 1) {
    /* Add change in semi-major axis due to EQTIDE-STELLAR coupling */

    // Other star (primary) can also influence this equation
    iOtherBody = 0;

    // Add dSemi-major axis dt from Eqtide-Stellar coupling to matrix
    update[iBody].iaType[update[iBody].iSemi][update[iBody].iSemiEqSt] = 1;
    update[iBody].iNumBodies[update[iBody].iSemi][update[iBody].iSemiEqSt] =
          2; // Both stars
    update[iBody].iaBody[update[iBody].iSemi][update[iBody].iSemiEqSt] = malloc(
          update[iBody]
                .iNumBodies[update[iBody].iSemi][update[iBody].iSemiEqSt] *
          sizeof(int));
    update[iBody].iaBody[update[iBody].iSemi][update[iBody].iSemiEqSt][0] =
          iBody;
    update[iBody].iaBody[update[iBody].iSemi][update[iBody].iSemiEqSt][1] =
          iOtherBody;
    update[iBody].pdDsemiDtEqSt =
          &update[iBody]
                 .daDerivProc[update[iBody].iSemi][update[iBody].iSemiEqSt];
  }
}

void VerifyModuleMultiAtmescEqtide(BODY *body, UPDATE *update, CONTROL *control,
                                   FILES *files, MODULE *module,
                                   OPTIONS *options, int iBody,
                                   int *iModuleProps, int *iModuleForce) {
  /* Ensure that if using Lopez et al radius models, ATMESC uses Lopez et al
   * radius for atmespheric escape while EQTIDE uses a "tidal radius" as it is
   * likely that the radius of the envelope does not really impact tides since
   * the tidal evolution has such a strong (r^5) radius dependent.  Effectively,
   * we assume that the core/ocean size of stuff controls that evolution while
   * the atmosphere contributes to the Tidal Q, k_2 and Im(k_2) for the world
   */

  // dImK2Env has no read options, so must initialize here!
  body[iBody].dImK2Env = 1;

  // If this is the star (body 0 or body 1 in binary), tidal radius == radius
  if (iBody == 0 || (body[iBody].bBinary && iBody == 1) ||
      (body[iBody].bStellar)) {
    body[iBody].dTidalRadius = body[iBody].dRadius;
    return;
  }

  if (body[iBody].bEqtide) {
    // CTL hack.  Pretty sure EQTIDE CTL is broken, but this will prevent
    // additional hiccups Ignore everything after this
    if (control->Evolve.iEqtideModel == CTL) {
      body[iBody].dTidalRadius = body[iBody].dRadius;
      return;
    }

    // Using ATMESC?
    if (body[iBody].bAtmEsc) {

      // if user wants to include tides due to envelope: (do same with oceans)
      if (body[iBody].bEnv) {
        // they better have defined k2Env, tidalqenv, denvmass
        if (!(options[OPT_TIDALQENV].iLine[iBody + 1] > -1)) {
          fprintf(stderr, "ERROR: if bEnvTides == 1, must specify %s.\n",
                  options[OPT_TIDALQENV].cName);
          exit(EXIT_INPUT);
        }
        // k2env not set
        else if (!(options[OPT_K2ENV].iLine[iBody + 1] > -1)) {
          fprintf(stderr, "ERROR: if bEnvTides == 1, must specify %s.\n",
                  options[OPT_K2ENV].cName);
          exit(EXIT_INPUT);
        }
        // envmass not set
        else if (!(options[OPT_ENVELOPEMASS].iLine[iBody + 1] > -1)) {
          fprintf(stderr, "ERROR: if bEnvTides == 1, must specify %s.\n",
                  options[OPT_ENVELOPEMASS].cName);
          exit(EXIT_INPUT);
        }
      }

      if (body[iBody].bOcean) {
        // they better have defined k2Ocean, tidalqocean, dSurfaceWaterMass
        // XXX Use option.cName instead of, e.g. bOceanTides
        if (options[OPT_TIDALQOCEAN].iLine[iBody + 1] == -1) {
          fprintf(stderr, "ERROR: if %s == 1, must specify %s.\n",
                  options[OPT_OCEANTIDES].cName,
                  options[OPT_TIDALQOCEAN].cName);
          exit(EXIT_INPUT);
        } else if (options[OPT_SURFACEWATERMASS].iLine[iBody + 1] == -1) {
          fprintf(stderr, "ERROR: if %s == 1, must specify %s.\n",
                  options[OPT_OCEANTIDES].cName,
                  options[OPT_SURFACEWATERMASS].cName);
          exit(EXIT_INPUT);
        } else if (options[OPT_K2OCEAN].iLine[iBody + 1] == -1) {
          fprintf(stderr, "ERROR: if %s == 1, must specify %s.\n",
                  options[OPT_OCEANTIDES].cName, options[OPT_K2OCEAN].cName);
          exit(EXIT_INPUT);
        }
      }
      // now lets check there's actually an envelope
      // there is not an envelope!!
      if (!(body[iBody].dEnvelopeMass > body[iBody].dMinEnvelopeMass)) {
        body[iBody].bEnv = 0;
      } else {
        body[iBody].bEnv     = 1;
        body[iBody].dImK2Env = body[iBody].dK2Env / body[iBody].dTidalQEnv;
      }
      // what about an ocean?
      if (body[iBody].dSurfaceWaterMass <= body[iBody].dMinSurfaceWaterMass) {
        body[iBody].bOcean = 0;
      } else {
        /*
// MAGMOC HERE
        if (!body[iBody].bMagmOc) {
          // This is probably a terrible fix. To merge MagmOc we need a better
          // strategy for water reservoirs
          body[iBody].bOcean = 1;
        }
        body[iBody].dImK2Ocean = body[iBody].dK2Ocean /
body[iBody].dTidalQOcean;
// MASTER HERE
        // Only activate ocean if user indicated they wanted to
        if (options[OPT_TIDALQOCEAN].iLine[iBody+1] > -1 &&
            options[OPT_K2OCEAN].iLine[iBody+1] > -1) {
          body[iBody].bOcean = 1;
          body[iBody].dImK2Ocean = body[iBody].dK2Ocean /
body[iBody].dTidalQOcean;
        }
// ENDS HERE
*/
        // Rory's trial
        if (!body[iBody].bMagmOc) {
          // Only activate ocean if user indicated they wanted to
          if (options[OPT_TIDALQOCEAN].iLine[iBody + 1] > -1 &&
              options[OPT_K2OCEAN].iLine[iBody + 1] > -1) {
            body[iBody].bOcean = 1;
            body[iBody].dImK2Ocean =
                  body[iBody].dK2Ocean / body[iBody].dTidalQOcean;
          }
        }
        // Rory's trial ends here.
      }

      // there's definitely at least gonna be some rock...
      body[iBody].dImK2Man = body[iBody].dK2Man / body[iBody].dTidalQMan;

      // if there is an envelope/ocean, we calculate ImK2Env/ImK2Ocean
      if (body[iBody].bEnv && (body[iBody].dTidalQ != body[iBody].dTidalQEnv)) {
        if (control->Io.iVerbose > 1) {
          fprintf(stderr, "Using dTidalQEnv for %s.\n", body[iBody].cName);
        }
        body[iBody].dTidalQ  = body[iBody].dTidalQEnv;
        body[iBody].dK2      = body[iBody].dK2Env;
        body[iBody].dImK2Env = body[iBody].dK2Env / body[iBody].dTidalQEnv;
        body[iBody].dImK2    = body[iBody].dImK2Env;
      } else {
        if (body[iBody].bOcean &&
            (body[iBody].dTidalQ != body[iBody].dTidalQOcean)) {
          fprintf(stderr, "Using dTidalQOcean for %s.\n", body[iBody].cName);
          body[iBody].dTidalQ = body[iBody].dTidalQOcean;
          body[iBody].dImK2Ocean =
                body[iBody].dK2Ocean / body[iBody].dTidalQOcean;
          body[iBody].dImK2 = body[iBody].dImK2Ocean;
          body[iBody].dK2   = body[iBody].dK2Ocean;
        } else if (!body[iBody].bEnv && !body[iBody].bOcean &&
                   (body[iBody].dTidalQ != body[iBody].dTidalQMan) &&
                   (iBody != 0)) {
          fprintf(stderr, "Using dTidalQMan for %s.\n", body[iBody].cName);
          // now we just use dTidalQMan and dK2Man
          body[iBody].dImK2Man = body[iBody].dK2Man / body[iBody].dTidalQMan;
          body[iBody].dTidalQ  = body[iBody].dTidalQMan;
          body[iBody].dK2      = body[iBody].dK2Man;
          body[iBody].dImK2    = body[iBody].dImK2Man;
        }
      }
      // Using tidal radus
      if (body[iBody].bUseTidalRadius) {
        // If any tidal radius option is set, the other must be set as well!
        if (!((options[OPT_TIDALRADIUS].iLine[iBody + 1] > -1) &&
              (options[OPT_TIDALRADIUS].iLine[iBody + 1] > -1))) {
          fprintf(stderr, "ERROR: if bTidalRadius == 1, must set %s.\n",
                  options[OPT_TIDALRADIUS].cName);
          exit(EXIT_INPUT);
        }
      }


      // Not using tidal radius
      else {
        // Since no tidal radius specified, dRadius better be (or a radius
        // evolution model or a mass radius relation)
        if (!(options[OPT_RADIUS].iLine[iBody + 1] > -1) &&
            !(options[OPT_PLANETRADIUSMODEL].iLine[iBody + 1] > -1) &&
            !(options[OPT_MASSRAD].iLine[iBody + 1] > -1)) {
          fprintf(stderr,
                  "ERROR: Using EQTIDE and bUseTidalRadius == 0 but %s or %s "
                  "not set!\n",
                  options[OPT_RADIUS].cName,
                  options[OPT_PLANETRADIUSMODEL].cName);
          exit(EXIT_INPUT);
        }

        // If dTidalRadius set, warn user since it's not considered
        if (options[OPT_TIDALRADIUS].iLine[iBody + 1] > -1) {
          if (control->Io.iVerbose >= VERBINPUT) {
            fprintf(stderr,
                    "WARNING: %s set but disregarded since bUseTidalRadius == "
                    "0.\n",
                    options[OPT_TIDALRADIUS].cName);
          }
        }

        // No tidal radius given -> just use specified radius
        body[iBody].dTidalRadius = body[iBody].dRadius;
      }

      // now we need to do verify steps to make sure everything is set:
      // Sets behavior for changing between dTidalQEnv, dTidalQOcean, and
      // dTidalQMan
      control->fnForceBehaviorMulti[iBody][(*iModuleForce)++] =
            &ForceBehaviorEqtideAtmesc;

      // Set a PropsAuxMultiAtmescEqtide here that controls dRadius/dTidalRadius
      control->fnPropsAuxMulti[iBody][(*iModuleProps)++] =
            &PropsAuxAtmescEqtide;

    }


    // Not using ATMESC
    else {
      // Using tidal radius without atmesc doesn't make sense, just need to set
      // radius Ignore if using stellar with some radius relation
      if (!(options[OPT_RADIUS].iLine[iBody + 1] > -1) &&
          !(options[OPT_MASSRAD].iLine[iBody + 1] > -1)) {
        fprintf(stderr, "ERROR: Using EQTIDE but neither %s or %s is set!\n",
                options[OPT_RADIUS].cName, options[OPT_MASSRAD].cName);
        exit(EXIT_INPUT);
      }

      // If dTidalRadius or bUseTidalRadius set, ignore and warn user as they do
      // nothing
      if ((options[OPT_USETIDALRADIUS].iLine[iBody + 1] > -1) ||
          (options[OPT_TIDALRADIUS].iLine[iBody + 1] > -1)) {
        if (control->Io.iVerbose >= VERBINPUT) {
          fprintf(stderr,
                  "WARNING: %s and/or %s set for EQTIDE while ATMESC not used "
                  "and hence will be ignored.\n",
                  options[OPT_USETIDALRADIUS].cName,
                  options[OPT_TIDALRADIUS].cName);
        }
      }

      // TidalRadius == radius as without ATMESC, planet radius doesn't evolve
      body[iBody].dTidalRadius = body[iBody].dRadius;
    }
  }
}
void VerifyModuleMultiAtmescEqtideThermint(BODY *body, UPDATE *update,
                                           CONTROL *control, FILES *files,
                                           MODULE *module, OPTIONS *options,
                                           int iBody, int *iModuleProps,
                                           int *iModuleForce) {

  // If you're using alllll of these, include the force behavior!
  // Also, you MUST have surface water information set if you're using
  // bOceanTides Note: VerifyEqtideThermint handles all things oceans

  // dImK2Env has no ReadOption, so we must initialize it here.

  if (body[iBody].bAtmEsc && body[iBody].bEqtide && body[iBody].bThermint) {
    body[iBody].dImK2Env = 1;

    if (body[iBody].bEqtide) {
      if (body[iBody].bThermint) {
        if (body[iBody].bAtmEsc) {
          // If modelling envelope tides
          if (body[iBody].bEnv) {
            // Make sure both dK2Env AND dTidalQEnv are set, otherwise exit
            if (!(options[OPT_TIDALQENV].iLine[iBody + 1] > -1 &&
                  options[OPT_K2ENV].iLine[iBody + 1] > -1)) {
              fprintf(stderr, "ERROR: %s and/or %s not set.\n",
                      options[OPT_TIDALQENV].cName, options[OPT_K2ENV].cName);
              fprintf(stderr, "Must both be set when using EQTIDE, THERMINT "
                              "and ATMESC with bEnvTides == True.\n");
              exit(EXIT_INPUT);
            }

            // Otherwise, we're good! set ImK2 for the envelope component
            body[iBody].dImK2Env = body[iBody].dK2Env / body[iBody].dTidalQEnv;
          }
          // Not modelling envelope tides
          else {
            // Envelope tidal parameters can't be set
            if (options[OPT_TIDALQENV].iLine[iBody + 1] > -1 ||
                options[OPT_K2ENV].iLine[iBody + 1] > -1) {
              if (control->Io.iVerbose >= VERBINPUT) {
                fprintf(stderr, "ERROR: %s or %s set, but bEnvTides == 0.\n",
                        options[OPT_TIDALQENV].cName, options[OPT_K2ENV].cName);
                exit(EXIT_INPUT);
              }
            }

            // Default to a tidal Q of ~ 1.0e6 (won't get used)
            body[iBody].dK2Env     = 1.0;
            body[iBody].dImK2Env   = 1.0e-6;
            body[iBody].dTidalQEnv = 1.0e6;
          }

          // Set function pointers so models play nice

          // Set force behavior
          control->fnForceBehaviorMulti[iBody][(*iModuleForce)++] =
                &ForceBehaviorAtmescEqtideThermint;

          // Switch PropAuxEqtideThermint -> PropsAuxAtmescEqtideThermint
          // XXX Hack! This has been PropsAuxAtmescEqtideThermint, but I think
          // it can be cut.
          control->fnPropsAuxMulti[iBody][(*iModuleProps)++] =
                &PropsAuxEqtideThermint;
        }
        // No AtmEsc
        else {
          // Can't have any tidal envelope parameters set
          if (options[OPT_TIDALQENV].iLine[iBody + 1] > -1 ||
              options[OPT_K2ENV].iLine[iBody + 1] > -1) {
            if (control->Io.iVerbose >= VERBINPUT) {
              fprintf(stderr, "ERROR: %s or %s set, but bOceanTides == 0.\n",
                      options[OPT_TIDALQENV].cName, options[OPT_K2ENV].cName);
              exit(EXIT_INPUT);
            }
          }

          // Zero things out so envelope can't play a role
          body[iBody].dK2Env     = 0.0;
          body[iBody].dImK2Env   = 0.0;
          body[iBody].dTidalQEnv = -1.0;
        }
      }
    }

    // Call PropsAuxAtmescThermint to initialize interior Properties
    if (iBody > 0) {
      fvPropsAuxThermint(body, &control->Evolve, &control->Io, update, iBody);
    }
  }
}

/*void VerifyModuleMultiFlareStellar(BODY *body, UPDATE *update, CONTROL
*control, FILES *files, MODULE *module, OPTIONS *options, int iBody, int
*iModuleProps, int *iModuleForce) {

  if (body[iBody].bFlare) {
    if (!body[iBody].bStellar) {
      fprintf(stderr,
              "ERROR: Must include module STELLAR to run module FLARE.\n");
      LineExit(files->Infile[iBody + 1].cIn,
               options[OPT_MODULES].iLine[iBody + 1]);
    } else {
      control->fnPropsAuxMulti[iBody][(*iModuleProps)++] =
            &PropsAuxFlareStellar;
    }
  }
}*/

/*
 * Binary + Stellar coupling.  If using binary, only 1st 2 bodies (0, 1) can be
 * stars
 */
void VerifyModuleMultiBinaryStellar(BODY *body, UPDATE *update,
                                    CONTROL *control, FILES *files,
                                    MODULE *module, OPTIONS *options, int iBody,
                                    int *iModuleProps, int *iModuleForce) {

  if (body[iBody].bBinary) {
    if (body[iBody].bStellar) {
      if (iBody > 1) {
        fprintf(stderr,
                "ERROR: Only bodies 0 and 1 can be stars when using binary!\n");
        LineExit(files->Infile[iBody + 1].cIn,
                 options[OPT_MODULES].iLine[iBody + 1]);
      }
    }
  }
}

/*
 * Binary + Eqtide: If using both, only stars (iBodyType == 1, iBody <= 1) can
 * use eqtide
 */
void VerifyModuleMultiBinaryEqtide(BODY *body, UPDATE *update, CONTROL *control,
                                   FILES *files, MODULE *module,
                                   OPTIONS *options, int iBody,
                                   int *iModuleProps, int *iModuleForce) {
  // If binary AND eqtide are called for a body, the body MUST be a star
  if (body[iBody].bBinary) {
    if (body[iBody].bEqtide) {
      // Body isn't a star!
      if (body[iBody].iBodyType != 1 || iBody > 1) {
        fprintf(stderr, "ERROR: If both binary AND eqtide are used for a body, "
                        "the body MUST be a star.\n");
        fprintf(stderr,
                "Errant body iBody, bBinary, bEqtide:, bStellar %d, %d, %d, "
                "%d.\n",
                iBody, body[iBody].bBinary, body[iBody].bEqtide,
                body[iBody].bStellar);
        LineExit(files->Infile[iBody + 1].cIn,
                 options[OPT_MODULES].iLine[iBody + 1]);
      }
    }
  }
}

void VerifyModuleMultiSpiNBodyDistOrb(BODY *body, UPDATE *update,
                                      CONTROL *control, FILES *files,
                                      OPTIONS *options, int iBody,
                                      int *iModuleProps, int *iModuleForce) {
  int iTmpBody;
  // This gets done repeatedly, but should be only done once
  control->Evolve.bSpiNBodyDistOrb = 0;

  // Since the star will not have DistOrb called, only check for planets
  for (iTmpBody = 1; iTmpBody < control->Evolve.iNumBodies; iTmpBody++) {
    if (body[iTmpBody].bSpiNBody && body[iTmpBody].bDistOrb) {
      control->Evolve.bSpiNBodyDistOrb = 1;
      // Start with DistOrb, not SpiNBody
      control->Evolve.bUsingDistOrb  = 1;
      control->Evolve.bUsingSpiNBody = 0;

      // This initializes Eqtide to be run with SpiNBody
      // if (body[iTmpBody].bEqtide) {
      //  control->Evolve.bStepEqtide = 0; // Initialize Eqtide Step as not yet
      //  taken
      //}
      // Set Mean Longitude so that Mean Anomaly can be tracked through DistOrb
      body[iTmpBody].dMeanL = body[iTmpBody].dMeanA + body[iTmpBody].dLongP;
    }
  }
  if (body[iBody].bSpiNBody && body[iBody].bDistOrb) {
    control->fnPropsAuxMulti[iBody][(*iModuleProps)++] =
          &PropsAuxSpiNBodyDistOrb;
    control->fnForceBehaviorMulti[iBody][(*iModuleForce)++] =
          &ForceBehaviorSpiNBodyDistOrb;
  }
}

void VerifyModuleMultiMagmOcAtmEsc(BODY *body, UPDATE *update, CONTROL *control,
                                   FILES *files, MODULE *module,
                                   OPTIONS *options, int iBody,
                                   int *iModuleProps, int *iModuleForce) {
  if (body[iBody].bMagmOc) {
    if (!body[iBody].bAtmEsc) {
      if (control->Io.iVerbose > VERBINPUT) {
        fprintf(stderr,
                "WARNING: Module MagmOc selected for %s, but AtmEsc not "
                "selected.\n",
                body[iBody].cName);
      }
      body[iBody].dWaterMassEsc  = 0;
      body[iBody].dOxygenMassEsc = 0;
    } else {
      control->fnPropsAuxMulti[iBody][(*iModuleProps)++] =
            &PropsAuxMagmOcAtmEsc;
    }
  }
}

/** Verify that selected modules are compatable */

void VerifyModuleCompatability(BODY *body, CONTROL *control, FILES *files,
                               MODULE *module, OPTIONS *options, int iBody) {

  // Binary
  if (body[iBody].bBinary) {
    if (body[iBody].bSpiNBody) {
      if (control->Io.iVerbose >= VERBERR) {
        fprintf(stderr, "ERROR: Modules Binary and SpiNbody cannot be applied "
                        "to the same body.\n");
      }
      LineExit(files->Infile[iBody + 1].cIn,
               options[OPT_MODULES].iLine[iBody + 1]);
    }
    if (body[iBody].bPoise) {
      if (control->Io.iVerbose >= VERBERR) {
        fprintf(stderr, "ERROR: Modules Binary and Poise cannot be applied to "
                        "the same body.\n");
      }
      LineExit(files->Infile[iBody + 1].cIn,
               options[OPT_MODULES].iLine[iBody + 1]);
    }
  }

  if (body[iBody].bAtmEsc) {
    if (body[iBody].bPoise) {
      if (control->Io.iVerbose >= VERBERR) {
        fprintf(stderr, "ERROR: Modules AtmEsc and POISE cannot be applied to "
                        "the same body.\n");
      }
      LineExit(files->Infile[iBody + 1].cIn,
               options[OPT_MODULES].iLine[iBody + 1]);
    }
  }

  // Distorb
  if (body[iBody].bDistOrb) {
    if (body[iBody].bSpiNBody) {
      if (control->Io.iVerbose >= VERBERR) {
        fprintf(stderr, "ERROR: Modules DistOrb and SpiNbody cannot be applied "
                        "to the same body.\n");
      }
      LineExit(files->Infile[iBody + 1].cIn,
               options[OPT_MODULES].iLine[iBody + 1]);
    }
  }

  // Galhabit
  if (body[iBody].bGalHabit && module->iNumModules[iBody] > 1) {
    if (control->Io.iVerbose >= VERBERR) {
      fprintf(stderr, "ERROR: No other modules are permitted with GalHabit.\n");
    }
    LineExit(files->Infile[iBody + 1].cIn,
             options[OPT_MODULES].iLine[iBody + 1]);
  }

  // Stellar
  if (body[iBody].bStellar) {
    if (body[iBody].bAtmEsc) {
      if (control->Io.iVerbose >= VERBERR) {
        fprintf(stderr, "ERROR: Modules Stellar and AtmEsc cannot be applied "
                        "to the same body.\n");
      }
      LineExit(files->Infile[iBody + 1].cIn,
               options[OPT_MODULES].iLine[iBody + 1]);
    }
    if (body[iBody].bDistOrb) {
      if (control->Io.iVerbose >= VERBERR) {
        fprintf(stderr, "ERROR: Modules Stellar and DistOrb cannot be applied "
                        "to the same body.\n");
      }
      LineExit(files->Infile[iBody + 1].cIn,
               options[OPT_MODULES].iLine[iBody + 1]);
    }
    if (body[iBody].bDistRot) {
      if (control->Io.iVerbose >= VERBERR) {
        fprintf(stderr, "ERROR: Modules Stellar and DistRot cannot be applied "
                        "to the same body.\n");
      }
      LineExit(files->Infile[iBody + 1].cIn,
               options[OPT_MODULES].iLine[iBody + 1]);
    }
    if (body[iBody].bPoise) {
      if (control->Io.iVerbose >= VERBERR) {
        fprintf(stderr, "ERROR: Modules Stellar and Poise cannot be applied to "
                        "the same body.\n");
      }
      LineExit(files->Infile[iBody + 1].cIn,
               options[OPT_MODULES].iLine[iBody + 1]);
    }
    if (body[iBody].bRadheat) {
      if (control->Io.iVerbose >= VERBERR) {
        fprintf(stderr, "ERROR: Modules Stellar and RadHeat cannot be applied "
                        "to the same body.\n");
      }
      LineExit(files->Infile[iBody + 1].cIn,
               options[OPT_MODULES].iLine[iBody + 1]);
    }
    /* This combo seems OK, and is necessary to calculate instellation. I'll
    leave it commented for now. if (body[iBody].bSpiNBody) { if
    (control->Io.iVerbose >= VERBERR) fprintf(stderr,"ERROR: Modules Stellar and
    SpiNBody cannot be applied to the same body.\n");
      LineExit(files->Infile[iBody+1].cIn,options[OPT_MODULES].iLine[iBody+1]);
    } */
    if (body[iBody].bThermint) {
      if (control->Io.iVerbose >= VERBERR) {
        fprintf(stderr, "ERROR: Modules Stellar and ThermInt cannot be applied "
                        "to the same body.\n");
      }
      LineExit(files->Infile[iBody + 1].cIn,
               options[OPT_MODULES].iLine[iBody + 1]);
    }
  }

  // Magmoc
  if (body[iBody].bMagmOc) {
    if (body[iBody].bThermint) {
      if (control->Io.iVerbose >= VERBERR) {
        fprintf(stderr, "ERROR: Modules MagmOc and ThermInt cannot be applied "
                        "to the same body.\n");
      }
      LineExit(files->Infile[iBody + 1].cIn,
               options[OPT_MODULES].iLine[iBody + 1]);
    }
  }
}

void VerifyModuleMulti(BODY *body, UPDATE *update, CONTROL *control,
                       FILES *files, MODULE *module, OPTIONS *options,
                       int iBody, fnUpdateVariable ****fnUpdate) {

  int iNumMultiProps = 0, iNumMultiForce = 0;

  if (module->iNumModules[iBody] > 0) {
    /* XXX Note that the number of elements here is really a permutation,
       but this should work for a while. */
    control->fnPropsAuxMulti[iBody] =
          malloc(2 * module->iNumModules[iBody] * sizeof(fnPropsAuxModule *));
    control->fnForceBehaviorMulti[iBody] = malloc(
          2 * module->iNumModules[iBody] * sizeof(fnForceBehaviorModule *));
  }

  /* Now verify. Even if only module is called, we still need to call
     these functions as some default behavior is set if other modules aren't
     called. */

  VerifyModuleMultiSpiNBodyAtmEsc(body, update, control, files, options, iBody,
                                  &iNumMultiProps, &iNumMultiForce);

  VerifyModuleMultiSpiNBodyDistOrb(body, update, control, files, options, iBody,
                                   &iNumMultiProps, &iNumMultiForce);

  VerifyModuleMultiDistOrbDistRot(body, update, control, files, options, iBody,
                                  &iNumMultiProps, &iNumMultiForce);

  VerifyModuleMultiEqtideDistRot(body, update, control, files, options, iBody,
                                 &iNumMultiProps, &iNumMultiForce);

  VerifyModuleMultiRadheatThermint(body, update, control, files, options, iBody,
                                   &iNumMultiProps, &iNumMultiForce);

  VerifyModuleMultiEqtideDistOrb(body, update, control, files, module, options,
                                 iBody, &iNumMultiProps, &iNumMultiForce);

  VerifyModuleMultiEqtideDistRot(body, update, control, files, options, iBody,
                                 &iNumMultiProps, &iNumMultiForce);

  VerifyModuleMultiAtmescEqtide(body, update, control, files, module, options,
                                iBody, &iNumMultiProps, &iNumMultiForce);

  VerifyModuleMultiEqtideThermint(body, update, control, files, module, options,
                                  iBody, &iNumMultiProps, &iNumMultiForce);

  // Always call after VerifyModuleMultiEqtideThermint !!
  VerifyModuleMultiAtmescEqtideThermint(body, update, control, files, module,
                                        options, iBody, &iNumMultiProps,
                                        &iNumMultiForce);

  // VerifyModuleMultiFlareStellar(body, update, control, files, module,
  // options,
  //                                iBody, &iNumMultiProps, &iNumMultiForce);

  VerifyModuleMultiBinaryEqtide(body, update, control, files, module, options,
                                iBody, &iNumMultiProps, &iNumMultiForce);

  VerifyModuleMultiEqtideStellar(body, update, control, files, module, options,
                                 iBody, &iNumMultiProps, &iNumMultiForce);

  VerifyModuleMultiBinaryStellar(body, update, control, files, module, options,
                                 iBody, &iNumMultiProps, &iNumMultiForce);

  VerifyModuleMultiMagmOcAtmEsc(body, update, control, files, module, options,
                                iBody, &iNumMultiProps, &iNumMultiForce);

  control->iNumMultiProps[iBody] = iNumMultiProps;
  control->iNumMultiForce[iBody] = iNumMultiForce;
  if (control->Io.iVerbose >= VERBALL) {
    fprintf(stdout, "All of %s's modules verified.\n", body[iBody].cName);
  }
}

/*
 * Auxiliary Properties for multi-module calculations
 */

void PropsAuxSpiNbodyEqtide(BODY *body, EVOLVE *evolve, IO *io, UPDATE *update,
                            int iBody) {
  // Nothing to see here...
}

void PropsAuxSpiNBodyDistOrb(BODY *body, EVOLVE *evolve, IO *io, UPDATE *update,
                             int iBody) {
}

void PropsAuxAtmescEqtide(BODY *body, EVOLVE *evolve, IO *io, UPDATE *update,
                          int iBody) {
  // This function controls how tidal radius is set.

  // If bUseTidalRadius == 0, dTidalRadius <- dRadius
  if (!body[iBody].bUseTidalRadius) {
    body[iBody].dTidalRadius = body[iBody].dRadius;
  }
}

/** Calculate auxiliary properties if EqTide and ThermInt are called. At present
  this funciton only needs to calculate Im(k_2), possibly including the effects
  of an ocean and envelope. */
void PropsAuxEqtideThermint(BODY *body, EVOLVE *evolve, IO *io, UPDATE *update,
                            int iBody) {

  body[iBody].dK2Man     = fdK2Man(body, iBody);
  body[iBody].dTidalQMan = fdTidalQMan(body, iBody);
  body[iBody].dImK2Man   = fdImK2ManThermint(body, iBody);

  body[iBody].dImK2 = fdImK2Total(body, iBody);
}

void PropsAuxRadheatThermint(BODY *body, EVOLVE *evolve, IO *io, UPDATE *update,
                             int iBody) {
  body[iBody].dRadPowerCore  = fdRadPowerCore(update, iBody);
  body[iBody].dRadPowerCrust = fdRadPowerCrust(update, iBody);
  body[iBody].dRadPowerMan   = fdRadPowerMan(update, iBody);
}

void PropsAuxEqtideDistorb(BODY *body, EVOLVE *evolve, IO *io, UPDATE *update,
                           int iBody) {
  body[iBody].dEccSq = body[iBody].dHecc * body[iBody].dHecc +
                       body[iBody].dKecc * body[iBody].dKecc;
}

void PropsAuxEqtideDistRot(BODY *body, EVOLVE *evolve, IO *io, UPDATE *update,
                           int iBody) {
  if (body[iBody].bCalcDynEllip) {
    body[iBody].dDynEllip = CalcDynEllipEq(body, iBody);
  }
}


void PropsAuxEqtideStellar(BODY *body, EVOLVE *evolve, IO *io, UPDATE *update,
                           int iBody) {
  // In stellar, radius can change depending on model so make sure tidal radius
  // knows that
  body[iBody].dTidalRadius = body[iBody].dRadius;
}

void PropsAuxFlareStellar(BODY *body, EVOLVE *evolve, IO *io, UPDATE *update,
                          int iBody) {
  SYSTEM system; // dummy for LXUVStellar
  // body[iBody].dLXUV = fdLXUVStellar(body,&system,update,iBody,iBody) +
  // body[iBody].dLXUVFlare;
}

void PropsAuxMagmOcAtmEsc(BODY *body, EVOLVE *evolve, IO *io, UPDATE *update,
                          int iBody) {
  if (body[iBody].bMagmOc && body[iBody].bAtmEsc) {

    // Use Water and Oxygen mass from magmoc for atmesc
    body[iBody].dSurfaceWaterMass = body[iBody].dWaterMassAtm;
    body[iBody].dOxygenMass       = body[iBody].dOxygenMassAtm;

    // Use atmospheric skin temperature
    body[iBody].dThermTemp = body[iBody].dEffTempAtm * pow(0.5, 0.25);

    // Call PropsAux from Atmesc (or fnPropsAuxAtmEsc)
    fnPropsAuxAtmEsc(body, evolve, io, update, iBody);

    // Water and Oxygen mass lost through atmospheric escape
    if ((body[iBody].bRunaway) &&
        (body[iBody].dSurfaceWaterMass > body[iBody].dMinSurfaceWaterMass)) {
      // This takes care of both energy-limited and diffusion limited escape!
      body[iBody].dWaterMassEsc =
            -(9. / (1 + 8 * body[iBody].dOxygenEta)) * body[iBody].dMDotWater;
    } else {
      body[iBody].dWaterMassEsc = 0;
    }

    if ((body[iBody].bRunaway) && (!body[iBody].bInstantO2Sink) &&
        (body[iBody].dSurfaceWaterMass > body[iBody].dMinSurfaceWaterMass)) {
      if (body[iBody].iWaterLossModel == ATMESC_LB15) {
        // Rodrigo and Barnes (2015)
        if (body[iBody].dCrossoverMass >= 16 * ATOMMASS) {
          double BDIFF = 4.8e19 * pow(body[iBody].dFlowTemp, 0.75);
          body[iBody].dOxygenMassEsc = (320. * PI * BIGG * ATOMMASS * ATOMMASS *
                                        BDIFF * body[iBody].dMass) /
                                       (KBOLTZ * body[iBody].dFlowTemp);
        } else {
          body[iBody].dOxygenMassEsc = (8 - 8 * body[iBody].dOxygenEta) /
                                       (1 + 8 * body[iBody].dOxygenEta) *
                                       body[iBody].dMDotWater;
        }
      } else {
        // Exact
        body[iBody].dOxygenMassEsc = (8 - 8 * body[iBody].dOxygenEta) /
                                     (1 + 8 * body[iBody].dOxygenEta) *
                                     body[iBody].dMDotWater;
      }
    } else {
      body[iBody].dOxygenMassEsc = 0;
    }

    if (body[iBody].bPlanetDesiccated) {
      body[iBody].dWaterMassEsc  = 0;
      body[iBody].dOxygenMassEsc = 0;
    }

    /* Get inner edge of the habitable zone (Runaway greenhouse) */
    double dFlux = fdHZRG14(body, iBody);
    body[iBody].dHZInnerEdge =
          pow(4 * PI * dFlux /
                    (body[0].dLuminosity *
                     pow((1 - body[iBody].dEcc * body[iBody].dEcc), 0.5)),
              -0.5);
  }
}

/*
 * Force Behavior for multi-module calculations
 */

void ForceBehaviorSpiNBodyDistOrb(BODY *body, MODULE *module, EVOLVE *evolve,
                                  IO *io, SYSTEM *system, UPDATE *update,
                                  fnUpdateVariable ***fnUpdate, int iFoo,
                                  int iBar) {
  int iBody, jBody, bOldUsingDistOrb, bOldUsingSpiNBody;
  double dMinOrbPeriod, *dDt;


  bOldUsingDistOrb  = evolve->bUsingDistOrb;
  bOldUsingSpiNBody = evolve->bUsingSpiNBody;

  // If using DistOrb, check to see if we should be using SpiNBody
  if (evolve->bUsingDistOrb) {
    for (iBody = 0; iBody < evolve->iNumBodies; iBody++) {
      body[iBody].dOrbPeriod =
            fdSemiToPeriod(body[iBody].dSemi, (body[0].dMass));
    }

    for (iBody = 0; iBody < evolve->iNumBodies; iBody++) {
      if ((body[iBody].dEcc * body[iBody].dInc) > 0.07 ||
          (body[iBody].dEcc > 0.25) || (body[iBody].dInc > .3)) {
        // If eccentricity and Inclination are too high, switch to SpiNBody
        // This needs reworked to properly calculate mutual inclination
        evolve->bUsingDistOrb  = 0;
        evolve->bUsingSpiNBody = 1;
      }
      for (jBody = 1; jBody < evolve->iNumBodies; jBody++) {
        if ((jBody != iBody) && (iBody != 0)) {
          // If there is a resonance, use SpiNBody
          if ((fmod(body[iBody].dOrbPeriod, body[jBody].dOrbPeriod) < 0.01) ||
              (fmod(body[jBody].dOrbPeriod, body[iBody].dOrbPeriod) < 0.01)) {
            evolve->bUsingDistOrb  = 0;
            evolve->bUsingSpiNBody = 1;
          }
        }
      }
      if ((evolve->dTime / evolve->dStopTime > 0.25) &&
          (evolve->dTime / evolve->dStopTime < 0.75)) {
        // This is hacked in to test model switching
        evolve->bUsingDistOrb  = 0;
        evolve->bUsingSpiNBody = 1;
      }
    }
  } else if (evolve->bUsingSpiNBody) {
    for (iBody = 0; iBody < evolve->iNumBodies; iBody++) {
      if (evolve->dTime / evolve->dStopTime >= 0.75) {
        // This is hacked in to test model switching
        evolve->bUsingDistOrb  = 1;
        evolve->bUsingSpiNBody = 0;
      }
    }
  }

  if (evolve->bUsingDistOrb && !(bOldUsingDistOrb)) {
    // If using DistOrb now, but not earlier, then change derivatives to use
    // DistOrb
    printf("Switching to DistOrb from SpiNBody at time %f years.\n",
           (evolve->dTime / YEARSEC));

    // Calculate Orbital Elements:
    if (!evolve->bFirstStep) {
      for (iBody = 0; iBody < evolve->iNumBodies; iBody++) {
        // Need to convert from Osc Elems into Barycentric
        Bary2OrbElems(body, iBody);
      }
    }

    // Null all derivatives then enable just SpiNBody
    for (iBody = 0; iBody < evolve->iNumBodies; iBody++) {
      // The star will not use DistOrb, so check before re-verifying
      if (body[iBody].bSpiNBody) {
        module->fnNullDerivatives[iBody][module->iaSpiNBody[iBody]](
              body, evolve, update, fnUpdate, iBody);
      }
      if (body[iBody].bDistOrb) {
        module->fnAssignDerivatives[iBody][module->iaDistOrb[iBody]](
              body, evolve, update, fnUpdate, iBody);
      }
    }

  } else if (evolve->bUsingSpiNBody && !(bOldUsingSpiNBody)) {
    // Else if using SpiNBody now, but not earlier, then change derivatives to
    // use SpiNBody
    printf("Switching to SpiNBody from DistOrb at time %f years.\n",
           (evolve->dTime / YEARSEC));

    // Update SpiNBody Coords
    fndUpdateSpiNBodyCoords(body, evolve);

    for (iBody = 0; iBody < evolve->iNumBodies; iBody++) {
      if (body[iBody].bDistOrb) {
        module->fnNullDerivatives[iBody][module->iaDistOrb[iBody]](
              body, evolve, update, fnUpdate, iBody);
      }
      if (body[iBody].bSpiNBody) {
        module->fnAssignDerivatives[iBody][module->iaSpiNBody[iBody]](
              body, evolve, update, fnUpdate, iBody);
      }
    }
  }

  // Check to see if we need to take an EqTide multistep
  if (evolve->bUsingSpiNBody) {
    // Is it time to calculate a new Eqtide step?
    //   if (evolve->bStepEqtide) {
    //     for (iBody=0; iBody<evolve->iNumBodies; iBody++) {
    //       // Need to convert from Osc Elems into Barycentric
    //       Bary2OrbElems(body,iBody);
    //     }
    //
    //     iSpiNBody = FindFamily(FAMSPINBODY,1,evolve->baFamily,update);
    //     iDistOrb = FindFamily(FAMDISTORB,1,evolve->baFamily,update);
    //
    //     // Null out all functions except EqTide
    //     for (iBody=0; iBody<evolve->iNumBodies; iBody++) {
    //
    //       // If SpiNBody is running, DistOrb pointers must already be Nulled
    //       out
    //
    //       // Null out SpiNBody function pointers
    //       fnUpdate[iBody][update[iSpiNBody][iBody].iPositionX][0] =
    //       &fndUpdateFunctionTiny;
    //       fnUpdate[iBody][update[iSpiNBody][iBody].iPositionY][0] =
    //       &fndUpdateFunctionTiny;
    //       fnUpdate[iBody][update[iSpiNBody][iBody].iPositionZ][0] =
    //       &fndUpdateFunctionTiny;
    //       fnUpdate[iBody][update[iSpiNBody][iBody].iVelX][0] =
    //       &fndUpdateFunctionTiny;
    //       fnUpdate[iBody][update[iSpiNBody][iBody].iVelY][0] =
    //       &fndUpdateFunctionTiny;
    //       fnUpdate[iBody][update[iSpiNBody][iBody].iVelZ][0] =
    //       &fndUpdateFunctionTiny;
    //     }
    //
    //     dMinOrbPeriod = dHUGE;
    //     for (iBody=0; iBody<evolve->iNumBodies; iBody++) {
    //       // Calculate the minimum Orbital period
    //       if (body[iBody].dOrbPeriod < dMinOrbPeriod) {
    //         dMinOrbPeriod = body[iBody].dOrbPeriod;
    //       }
    //     }
    //
    //     *dDt = 100*dMinOrbPeriod;
    //
    //     // Run a single RK4 step for Eqtide
    //     SingleRungeKutta4Step(body,evolve,io,system,update,fnUpdate,dDt,1,iDistOrb);
    //
    //
    //     evolve->bStepEqtide = 0;
    //   }
  }
}

void ForceBehaviorSpiNBodyAtmEsc(BODY *body, MODULE *module, EVOLVE *evolve,
                                 IO *io, SYSTEM *system, UPDATE *update,
                                 fnUpdateVariable ***fnUpdate, int iBody,
                                 int iModule) {

  // Need to get orbital elements for AtmEsc to use for escape
  Bary2OrbElems(body, iBody);
}

void ForceBehaviorEqtideDistOrb(BODY *body, MODULE *module, EVOLVE *evolve,
                                IO *io, SYSTEM *system, UPDATE *update,
                                fnUpdateVariable ***fnUpdate, int iBody,
                                int iModule) {
  if (evolve->iDistOrbModel == RD4) {
    RecalcLaplace(body, evolve, system, io->iVerbose);
  } else if (evolve->iDistOrbModel == LL2) {
    RecalcEigenVals(body, evolve, system);
  };
}

void ForceBehaviorEqtideAtmesc(BODY *body, MODULE *module, EVOLVE *evolve,
                               IO *io, SYSTEM *system, UPDATE *update,
                               fnUpdateVariable ***fnUpdate, int iBody,
                               int iModule) {

  // We think there is an envelope, but there isn't!
  if (body[iBody].bEnv &&
      (body[iBody].dEnvelopeMass <= body[iBody].dMinEnvelopeMass)) {
    if (io->iVerbose >= VERBPROG) {
      fprintf(stderr, "%s's envelope lost at t = %.2e years!\n",
              body[iBody].cName, evolve->dTime / YEARSEC);
    }
    body[iBody].bEnv     = 0;
    body[iBody].dImK2Env = 0;
    // Adjust Im(k_2)
    body[iBody].dImK2 = fdImK2Total(body, iBody);
  }

  // We think there's an ocean, but there isn't!!
  if (body[iBody].bOcean &&
      (body[iBody].dSurfaceWaterMass <= body[iBody].dMinSurfaceWaterMass)) {
    if (io->iVerbose > VERBPROG) {
      fprintf(stderr, "Ocean lost at t = %.2e years!\n",
              evolve->dTime / YEARSEC);
    }
    body[iBody].bOcean     = 0;
    body[iBody].dImK2Ocean = 0;
    // Adjust Im(k_2)
    body[iBody].dImK2 = fdImK2Total(body, iBody);
  }

  /* Old way, in which Q is set by top layer. need a switch for this.
  for (iBody = 1; iBody<=evolve->iNumBodies-1; iBody++) {

    // We think there is an envelope, but there isnt!
    if (body[iBody].bEnv && (body[iBody].dEnvelopeMass <=
  body[iBody].dMinEnvelopeMass)) { if (io->iVerbose > VERBERR) {
        fprintf(stderr,"Envelope lost! Changing dTidalQ to:");
      }
      body[iBody].bEnv = 0;

      // is there an ocean? lets set tidalq to that!
      if (body[iBody].bOcean && (body[iBody].dSurfaceWaterMass >
  body[iBody].dMinSurfaceWaterMass)) { if (io->iVerbose > VERBERR) {
          fprintf(stderr," dTidalQOcean,\n");
        }
        body[iBody].dTidalQ = body[iBody].dTidalQOcean;
        body[iBody].dK2 = body[iBody].dK2Ocean;
        body[iBody].dImK2 = body[iBody].dImK2Ocean;
      }
      // there is not ocean, so lets use dTidalQRock!
      else {
        if (io->iVerbose > VERBERR) {
          fprintf(stderr," dTidalQRock.\n");
        }
        body[iBody].dTidalQ = body[iBody].dTidalQRock;
        body[iBody].dK2 = body[iBody].dK2Rock;
        body[iBody].dImK2 = body[iBody].dImK2Rock;
      }
    }
    // we think theres an ocean, but there isnt!!
    else if (body[iBody].bOcean && (body[iBody].dSurfaceWaterMass <=
  body[iBody].dMinSurfaceWaterMass)) { if (io->iVerbose > VERBERR) {
        fprintf(stderr,"Ocean Lost! Switching dTidalQ to: dTidalQRock.\n");
      }
      body[iBody].dTidalQ = body[iBody].dTidalQRock;
      body[iBody].dK2 = body[iBody].dK2Rock;
      body[iBody].dImK2 = body[iBody].dImK2Rock;
      body[iBody].bOcean = 0;
    }
  }
*/
}

/* XXX RB: I think this subroutine can be cut. We want a
ForceBehaviorStellarEqtide that determines if a planet enters/exits a runaway
greenhouse so as to turn on/off bOceanTides. */
void ForceBehaviorAtmescEqtideThermint(BODY *body, MODULE *module,
                                       EVOLVE *evolve, IO *io, SYSTEM *system,
                                       UPDATE *update,
                                       fnUpdateVariable ***fnUpdate, int iBody,
                                       int iModule) {

  // Loop over non-star bodies
  // Keeps track of whether or not bOceanTides or bEnvTides were initially set
  // to ensure they don't get turned back on by force behavior
  // If oceans or envelope weren't initially set to be modeled, their Q ==
  // -dHUGE
  int bOceans = 0;
  int bEnv    = 0;

  for (iBody = 1; iBody < evolve->iNumBodies; iBody++) {
    // If body 1 is a star (iBodyType==1), pass
    if (iBody == 1 && body[iBody].iBodyType == 1) {
      continue;
    }

    // Ocean check
    if (body[iBody].dTidalQOcean < 0) {
      bOceans = 0;
    } else {
      bOceans = 1;
    }
    /* But if in a runaway greenhouse, no ocean. Note that this is a bit of a
      fudge: If the planet's surface temperature is between 70 and 100 C, then
      it could have oceans *and* be in an RG. */
    if (evolve->bFirstStep) {
      // RG -> no ocean tides
      if (fdInstellation(body, iBody) >= fdHZRG14(body, iBody)) {
        bOceans = 0;
      }
    }

    // Envelope check
    if (body[iBody].dTidalQEnv < 0) {
      bEnv = 0;
    } else {
      bEnv = 1;
    }

    // Note: With these checks, only do anything if user intended to model them
    // i.e. if bOceanTides == 1 from initial conditions

    // Case: No water -> no ocean tides
    if (bOceans &&
        (body[iBody].dSurfaceWaterMass <= body[iBody].dMinSurfaceWaterMass)) {
      body[iBody].bOcean = 0;
    } else if (bOceans &&
               (body[iBody].dSurfaceWaterMass >
                body[iBody].dMinSurfaceWaterMass) &&
               body[iBody].bRunaway) {
      // Case: Water but it's in the atmosphere: RUNAWAY GREENHOUSE (this is
      // when body actively loses water!)
      body[iBody].bOcean = 0;
    } else if (bOceans &&
               (body[iBody].dSurfaceWaterMass >
                body[iBody].dMinSurfaceWaterMass) &&
               !body[iBody].bRunaway) {
      // Case: Water on the surface! (this is when body does NOT actively lose
      // water!)
      body[iBody].bOcean = 1;
    }

    // Check to see if the envelope is gone: when dEnvelopeMass <=
    // dMinEnvelopeMass
    if (bEnv && (body[iBody].dEnvelopeMass <= body[iBody].dMinEnvelopeMass)) {
      body[iBody].bEnv = 0;
    }
    // Still have the envelope!
    else if (bEnv &&
             (body[iBody].dEnvelopeMass > body[iBody].dMinEnvelopeMass)) {
      body[iBody].bEnv = 1;
    }

    // Enfore that they are mutually exclusive
    // i.e. if using EnvTides or an envelope exists, ocean can't do anything
    if (body[iBody].bEnv ||
        (body[iBody].dEnvelopeMass > body[iBody].dMinEnvelopeMass)) {
      body[iBody].bOcean = 0;
    }
  }
}

/*
 *
 * Functions required to add multi-body equations to the matrix
 *
 */

/*! Eqtide-Stellar semi-major axis derivative set-up*/
void InitializeUpdateEqStSemi(BODY *body, UPDATE *update, int iBody) {
  // Only valid if EQTIDE, and STELLAR used and iBodyType == 1
  if ((body[iBody].iBodyType == 1) && body[iBody].bStellar &&
      body[iBody].bEqtide && iBody == 1) {
    if (update[iBody].iNumSemi == 0) {
      update[iBody].iNumVars++;
    }
    update[iBody].iNumSemi++;
  }
}


/*! Finalize update (for malloc-ing) for Eq-st semi-major axis derivative */
void FinalizeUpdateMultiEqStSemi(BODY *body, UPDATE *update, int *iEqn,
                                 int iVar, int iBody, int iFoo,
                                 fnUpdateVariable ****fnUpdate) {
  if ((body[iBody].iBodyType == 1) && body[iBody].bStellar &&
      body[iBody].bEqtide && iBody == 1) {
    /* Add change in semi-major axis due to EQTIDE-STELLAR coupling */
    update[iBody].iaModule[iVar][(*iEqn)] = EQTIDE + STELLAR;
    update[iBody].iSemiEqSt               = (*iEqn);
    (*iEqn)++;
  }
}

/* GENERAL MULTI-MODULE EQUATION INITIALIZATION/FINALIZATION */

/*!
 * Initialize adding mutli-module equations to the matrix.
 */
void InitializeUpdateMulti(BODY *body, CONTROL *control, MODULE *module,
                           UPDATE *update, fnUpdateVariable ****fnUpdate,
                           int iBody) {
  // Initialize update struct to accomodate multi-module equations
  InitializeUpdateEqStSemi(body, update, iBody);

  /* More equations here! */
}

/*!
 * Finalize adding mutli-module equations to the matrix.
 */
void FinalizeUpdateMulti(BODY *body, CONTROL *control, MODULE *module,
                         UPDATE *update, fnUpdateVariable ****fnUpdate,
                         int *iVar, int iBody, int iFoo) {

  /* Add more equations below! */
}
