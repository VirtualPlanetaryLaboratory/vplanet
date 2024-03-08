/**
  @file update.c
  @brief Subroutines controlling the integration
  @author Rory Barnes ([RoryBarnes](https://github.com/RoryBarnes/))
  @date A long, long time ago
*/

#include "vplanet.h"

void InitializeUpdateBodyPerts(CONTROL *control, UPDATE *update, int iBody) {
  int iVar, iEqn;

  for (iVar = 0; iVar < update[iBody].iNumVars; iVar++) {
    for (iEqn = 0; iEqn < update[iBody].iNumEqns[iVar]; iEqn++) {
      control->Evolve.tmpUpdate[iBody].iaBody[iVar][iEqn] =
            malloc(update[iBody].iNumBodies[iVar][iEqn] * sizeof(int));
    }
  }
}

void InitializeUpdateTmpBody(BODY *body, CONTROL *control, MODULE *module,
                             UPDATE *update, int iBody) {
  int iModule;

  for (iModule = 0; iModule < module->iNumModules[iBody]; iModule++) {
    module->fnInitializeUpdateTmpBody[iBody][iModule](body, control, update,
                                                      iBody);
  }
}

void UpdateCopy(UPDATE *dest, UPDATE *src, int iNumBodies) {
  int iBody, iVar, iEqn, iPert;

  for (iBody = 0; iBody < iNumBodies; iBody++) {
    // The iNumX and iX members don't need to be copied

    dest[iBody].iNumVars = src[iBody].iNumVars;

    dest[iBody].iXobl = src[iBody].iXobl;
    dest[iBody].iYobl = src[iBody].iYobl;
    dest[iBody].iZobl = src[iBody].iZobl;
    dest[iBody].iHecc = src[iBody].iHecc;
    dest[iBody].iKecc = src[iBody].iKecc;
    dest[iBody].iPinc = src[iBody].iPinc;
    dest[iBody].iQinc = src[iBody].iQinc;

    for (iVar = 0; iVar < src[iBody].iNumVars; iVar++) {
      dest[iBody].iNumEqns[iVar] = src[iBody].iNumEqns[iVar];
      dest[iBody].iaVar[iVar]    = src[iBody].iaVar[iVar];

      for (iEqn = 0; iEqn < src[iBody].iNumEqns[iVar]; iEqn++) {
        dest[iBody].iNumBodies[iVar][iEqn] = src[iBody].iNumBodies[iVar][iEqn];
        dest[iBody].iaType[iVar][iEqn]     = src[iBody].iaType[iVar][iEqn];
        dest[iBody].iaModule[iVar][iEqn]   = src[iBody].iaModule[iVar][iEqn];
        for (iPert = 0; iPert < src[iBody].iNumBodies[iVar][iEqn]; iPert++) {
          dest[iBody].iaBody[iVar][iEqn][iPert] =
                src[iBody].iaBody[iVar][iEqn][iPert];
        }
      }
    }
  }
}

/* Someday we'll fix it so that the each primary variable just calls this function. The current impediment
is the fnFinalizeUpdateModule -- it doesn't exist yet. Currently each Primary Variable (PV) has its own
FinalizeUpdate function that makes passing it around more difficult. The "right" way to solve this is to 
create a generic fnFinalizeUpdateModule function that is 3 dimensions: iBody, iModule, iPV. We'd have to 
figure out the accounting for iPV, which is why I never did it. In the meantime, it sucks to have to
make any changes to InitializeUpdate because you have to go through ALL the PVs. -- RB, 21 Feb 22 

void AddPrimaryVariable(BODY *body,CONTROL *control,MODULE *module,UPDATE *update,fnUpdateVariable ***fnUpdate,
                  double *dPrimaryVariable, double *dTmpPrimaryVariable,fnFinalizeUpdateModule *finalize,
                   int *iPrimaryVariable,int iNumPrimaryVariable,int iBody,int *iVar,int iID) {
    *iPrimaryVariable = -1;
    if (iNumPrimaryVariable) {
      *iPrimaryVariable    = *iVar;
      update[iBody].iaVar[*iVar]    = iID;
      update[iBody].iNumEqns[*iVar] = iNumPrimaryVariable;
      update[iBody].pdVar[*iVar]    = dPrimaryVariable;
      update[iBody].iNumBodies[*iVar] =
            malloc(iNumPrimaryVariable * sizeof(int));
      update[iBody].iaBody[*iVar] =
            malloc(iNumPrimaryVariable * sizeof(int *));
      update[iBody].iaType[*iVar] =
            malloc(iNumPrimaryVariable * sizeof(int));
      update[iBody].iaModule[*iVar] =
            malloc(iNumPrimaryVariable * sizeof(int));

      if (control->Evolve.iOneStep == RUNGEKUTTA) {
        control->Evolve.tmpUpdate[iBody].pdVar[*iVar] =
              dTmpPrimaryVariable;
        control->Evolve.tmpUpdate[iBody].iNumBodies[*iVar] =
              malloc(iNumPrimaryVariable * sizeof(int));
        control->Evolve.tmpUpdate[iBody].daDerivProc[*iVar] =
              malloc(iNumPrimaryVariable * sizeof(double));
        control->Evolve.tmpUpdate[iBody].iaType[*iVar] =
              malloc(iNumPrimaryVariable * sizeof(int));
        control->Evolve.tmpUpdate[iBody].iaModule[*iVar] =
              malloc(iNumPrimaryVariable * sizeof(int));
        control->Evolve.tmpUpdate[iBody].iaBody[*iVar] =
              malloc(iNumPrimaryVariable * sizeof(int *));
        for (int iSubStep=0; iSubStep < 4; iSubStep++) {
            control->Evolve.daDerivProc[iSubStep][iBody][*iVar] =
                  malloc(update[iBody].iNumVars * sizeof(double));
        }
      }

      int iEqn = 0;
      for (int iModule = 0; iModule < module->iNumModules[iBody]; iModule++) {
            int iFoo;
        finalize(body, update, &iEqn,iVar, iBody, iFoo);
      }

      (*fnUpdate)[iBody][*iVar]        = malloc(iEqn * sizeof(fnUpdateVariable));
      update[iBody].daDerivProc[*iVar] = malloc(iEqn * sizeof(double));
      (*iVar)++;
    }
}
*/

void InitializeUpdate(BODY *body, CONTROL *control, MODULE *module,
                      UPDATE *update, fnUpdateVariable ****fnUpdate) {
  int iBody, iBodyPert, iVar, iEqn, iModule, iLat;
  int iNum, iSubStep;
  int iFoo = 0; // Dummy variable needed for some typedef'd functions

  // Allocate the firt dimension of the Update matrix to be the number of bodies
  *fnUpdate = malloc(control->Evolve.iNumBodies * sizeof(fnUpdateVariable **));

  // For each body, determine how many independent variables need updating.
  for (iBody = 0; iBody < control->Evolve.iNumBodies; iBody++) {
    // Assign dZero for any derivative pointers that need to point to 0
    update[iBody].dZero = 0;

    // Assume no modules to start
    // update[iBody].iNumEcc=0;
    update[iBody].iNum26AlMan          = 0;
    update[iBody].iNum26AlCore         = 0;
    update[iBody].iNum40KMan           = 0;
    update[iBody].iNum40KCore          = 0;
    update[iBody].iNum40KCrust         = 0;
    update[iBody].iNum232ThMan         = 0;
    update[iBody].iNum232ThCore        = 0;
    update[iBody].iNum232ThCrust       = 0;
    update[iBody].iNum235UMan          = 0;
    update[iBody].iNum235UCore         = 0;
    update[iBody].iNum235UCrust        = 0;
    update[iBody].iNum238UMan          = 0;
    update[iBody].iNum238UCore         = 0;
    update[iBody].iNum238UCrust        = 0;
    update[iBody].iNumCBPR             = 0;
    update[iBody].iNumCBPZ             = 0;
    update[iBody].iNumCBPPhi           = 0;
    update[iBody].iNumCBPRDot          = 0;
    update[iBody].iNumCBPZDot          = 0;
    update[iBody].iNumCBPPhiDot        = 0;
    update[iBody].iNumDynEllip         = 0;
    update[iBody].iNumEnvelopeMass     = 0;
    update[iBody].iNumHecc             = 0;
    update[iBody].iNumKecc             = 0;
    update[iBody].iNumLuminosity       = 0;
    update[iBody].iNumLXUV             = 0;
    update[iBody].iNumMass             = 0;
    update[iBody].iNumPinc             = 0;
    update[iBody].iNumQinc             = 0;
    update[iBody].iNumRadGyra          = 0;
    update[iBody].iNumRadius           = 0;
    update[iBody].iNumRot              = 0;
    update[iBody].iNumSemi             = 0;
    update[iBody].iNumSurfaceWaterMass = 0;
    update[iBody].iNumOxygenMass       = 0;
    update[iBody].iNumOxygenMantleMass = 0;
    update[iBody].iNumTemperature      = 0;
    update[iBody].iNumTMan             = 0;
    update[iBody].iNumTCore            = 0;
    update[iBody].iNumXobl             = 0;
    update[iBody].iNumYobl             = 0;
    update[iBody].iNumZobl             = 0;
    update[iBody].iNumEccX             = 0;
    update[iBody].iNumEccY             = 0;
    update[iBody].iNumEccZ             = 0;
    update[iBody].iNumAngMX            = 0;
    update[iBody].iNumAngMY            = 0;
    update[iBody].iNumAngMZ            = 0;

    update[iBody].iNumPositionX = 0;
    update[iBody].iNumPositionY = 0;
    update[iBody].iNumPositionZ = 0;
    update[iBody].iNumVelX      = 0;
    update[iBody].iNumVelY      = 0;
    update[iBody].iNumVelZ      = 0;

    /* HERE
     * set update of primary variables pointer to 0 ??
     */
    update[iBody].iNumWaterMassMOAtm    = 0;
    update[iBody].iNumWaterMassSol      = 0;
    update[iBody].iNumSurfTemp          = 0;
    update[iBody].iNumPotTemp           = 0;
    update[iBody].iNumSolidRadius       = 0;
    update[iBody].iNumOxygenMassMOAtm   = 0;
    update[iBody].iNumOxygenMassSol     = 0;
    update[iBody].iNumHydrogenMassSpace = 0;
    update[iBody].iNumOxygenMassSpace   = 0;
    update[iBody].iNumCO2MassMOAtm      = 0;
    update[iBody].iNumCO2MassSol        = 0;

    update[iBody].iNumLostAngMom = 0;
    update[iBody].iNumLostEng    = 0;
    update[iBody].iNumVars       = 0;

    /* First we must identify how many variables and models must be
       assigned so we can malloc the update struct. */
    for (iModule = 0; iModule < module->iNumModules[iBody]; iModule++) {
      module->fnInitializeUpdate[iBody][iModule](body, update, iBody);
    }

    // Initialize multi-module equations
    InitializeUpdateMulti(body, control, module, update, fnUpdate, iBody);

    // The second dimension of the Update matrix is the variables
    (*fnUpdate)[iBody] =
          malloc(update[iBody].iNumVars * sizeof(fnUpdateVariable *));
    update[iBody].iaVar    = malloc(update[iBody].iNumVars * sizeof(int));
    update[iBody].iNumEqns = malloc(update[iBody].iNumVars * sizeof(int));
    update[iBody].iaType   = malloc(update[iBody].iNumVars * sizeof(int *));
    update[iBody].iaModule = malloc(update[iBody].iNumVars * sizeof(int *));
    update[iBody].pdVar    = malloc(update[iBody].iNumVars * sizeof(double *));
    update[iBody].daDeriv  = malloc(update[iBody].iNumVars * sizeof(double));
    update[iBody].daDerivProc =
          malloc(update[iBody].iNumVars * sizeof(double *));
    update[iBody].iNumBodies = malloc(update[iBody].iNumVars * sizeof(int *));
    update[iBody].iaBody     = malloc(update[iBody].iNumVars * sizeof(int **));

    // May also have to allocate space for the temp UPDATE
    if (control->Evolve.iOneStep == RUNGEKUTTA) {
      control->Evolve.tmpUpdate[iBody].iaVar =
            malloc(update[iBody].iNumVars * sizeof(int));
      control->Evolve.tmpUpdate[iBody].iNumEqns =
            malloc(update[iBody].iNumVars * sizeof(double));
      control->Evolve.tmpUpdate[iBody].iaType =
            malloc(update[iBody].iNumVars * sizeof(int *));
      control->Evolve.tmpUpdate[iBody].iaModule =
            malloc(update[iBody].iNumVars * sizeof(int *));
      control->Evolve.tmpUpdate[iBody].pdVar =
            malloc(update[iBody].iNumVars * sizeof(double *));
      control->Evolve.tmpUpdate[iBody].daDeriv =
            malloc(update[iBody].iNumVars * sizeof(double *));
      control->Evolve.tmpUpdate[iBody].daDerivProc =
            malloc(update[iBody].iNumVars * sizeof(double *));
      control->Evolve.tmpUpdate[iBody].iNumBodies =
            malloc(update[iBody].iNumVars * sizeof(int *));
      control->Evolve.tmpUpdate[iBody].iaBody =
            malloc(update[iBody].iNumVars * sizeof(int **));
    }
    for (iSubStep = 0; iSubStep < 4; iSubStep++) {
      control->Evolve.daDeriv[iSubStep][iBody] =
            malloc(update[iBody].iNumVars * sizeof(double));
      control->Evolve.daDerivProc[iSubStep][iBody] =
            malloc(update[iBody].iNumVars * sizeof(double*));
    }

    /* Now we malloc some pointers, and perform some initializations for the
       UPDATE struct based on the primary variables required for each's
       planet's assigned modules. */

    iVar = 0;

    update[iBody].iVelX = -1;
    if (update[iBody].iNumVelX) {
      update[iBody].iVelX          = iVar;
      update[iBody].iaVar[iVar]    = VVELX;
      update[iBody].iNumEqns[iVar] = update[iBody].iNumVelX;
      update[iBody].pdVar[iVar]    = &body[iBody].dVelX;
      update[iBody].iNumBodies[iVar] =
            malloc(update[iBody].iNumVelX * sizeof(int));
      update[iBody].iaBody[iVar] =
            malloc(update[iBody].iNumVelX * sizeof(int *));
      update[iBody].iaType[iVar] = malloc(update[iBody].iNumVelX * sizeof(int));
      update[iBody].iaModule[iVar] =
            malloc(update[iBody].iNumVelX * sizeof(int));

      if (control->Evolve.iOneStep == RUNGEKUTTA) {
        control->Evolve.tmpUpdate[iBody].pdVar[iVar] =
              &control->Evolve.tmpBody[iBody].dVelX;
        control->Evolve.tmpUpdate[iBody].iNumBodies[iVar] =
              malloc(update[iBody].iNumVelX * sizeof(int));
        control->Evolve.tmpUpdate[iBody].daDerivProc[iVar] =
              malloc(update[iBody].iNumVelX * sizeof(double));
        control->Evolve.tmpUpdate[iBody].iaType[iVar] =
              malloc(update[iBody].iNumVelX * sizeof(int));
        control->Evolve.tmpUpdate[iBody].iaModule[iVar] =
              malloc(update[iBody].iNumVelX * sizeof(int));
        control->Evolve.tmpUpdate[iBody].iaBody[iVar] =
              malloc(update[iBody].iNumVelX * sizeof(int *));
        for (iSubStep=0; iSubStep < 4; iSubStep++) {
            control->Evolve.daDerivProc[iSubStep][iBody][iVar] =
                  malloc(update[iBody].iNumEqns[iVar] * sizeof(double));
        }
      }

      iEqn = 0;
      for (iModule = 0; iModule < module->iNumModules[iBody]; iModule++) {
        module->fnFinalizeUpdateVelX[iBody][iModule](body, update, &iEqn, iVar,
                                                     iBody, iFoo);
      }

      (*fnUpdate)[iBody][iVar]        = malloc(iEqn * sizeof(fnUpdateVariable));
      update[iBody].daDerivProc[iVar] = malloc(iEqn * sizeof(double));
      iVar++;
    }

    update[iBody].iVelY = -1;
    if (update[iBody].iNumVelY) {
      update[iBody].iVelY          = iVar;
      update[iBody].iaVar[iVar]    = VVELY;
      update[iBody].iNumEqns[iVar] = update[iBody].iNumVelY;
      update[iBody].pdVar[iVar]    = &body[iBody].dVelY;
      update[iBody].iNumBodies[iVar] =
            malloc(update[iBody].iNumVelY * sizeof(int));
      update[iBody].iaBody[iVar] =
            malloc(update[iBody].iNumVelY * sizeof(int *));
      update[iBody].iaType[iVar] = malloc(update[iBody].iNumVelY * sizeof(int));
      update[iBody].iaModule[iVar] =
            malloc(update[iBody].iNumVelY * sizeof(int));

      if (control->Evolve.iOneStep == RUNGEKUTTA) {
        control->Evolve.tmpUpdate[iBody].pdVar[iVar] =
              &control->Evolve.tmpBody[iBody].dVelY;
        control->Evolve.tmpUpdate[iBody].iNumBodies[iVar] =
              malloc(update[iBody].iNumVelY * sizeof(int));
        control->Evolve.tmpUpdate[iBody].daDerivProc[iVar] =
              malloc(update[iBody].iNumVelY * sizeof(double));
        control->Evolve.tmpUpdate[iBody].iaType[iVar] =
              malloc(update[iBody].iNumVelY * sizeof(int));
        control->Evolve.tmpUpdate[iBody].iaModule[iVar] =
              malloc(update[iBody].iNumVelY * sizeof(int));
        control->Evolve.tmpUpdate[iBody].iaBody[iVar] =
              malloc(update[iBody].iNumVelY * sizeof(int *));
        for (iSubStep=0; iSubStep < 4; iSubStep++) {
            control->Evolve.daDerivProc[iSubStep][iBody][iVar] =
                  malloc(update[iBody].iNumEqns[iVar] * sizeof(double));
        }
      }

      iEqn = 0;
      for (iModule = 0; iModule < module->iNumModules[iBody]; iModule++) {
        module->fnFinalizeUpdateVelY[iBody][iModule](body, update, &iEqn, iVar,
                                                     iBody, iFoo);
      }

      (*fnUpdate)[iBody][iVar]        = malloc(iEqn * sizeof(fnUpdateVariable));
      update[iBody].daDerivProc[iVar] = malloc(iEqn * sizeof(double));
      iVar++;
    }

    update[iBody].iVelZ = -1;
    if (update[iBody].iNumVelZ) {
      update[iBody].iVelZ          = iVar;
      update[iBody].iaVar[iVar]    = VVELZ;
      update[iBody].iNumEqns[iVar] = update[iBody].iNumVelZ;
      update[iBody].pdVar[iVar]    = &body[iBody].dVelZ;
      update[iBody].iNumBodies[iVar] =
            malloc(update[iBody].iNumVelZ * sizeof(int));
      update[iBody].iaBody[iVar] =
            malloc(update[iBody].iNumVelZ * sizeof(int *));
      update[iBody].iaType[iVar] = malloc(update[iBody].iNumVelZ * sizeof(int));
      update[iBody].iaModule[iVar] =
            malloc(update[iBody].iNumVelZ * sizeof(int));

      if (control->Evolve.iOneStep == RUNGEKUTTA) {
        control->Evolve.tmpUpdate[iBody].pdVar[iVar] =
              &control->Evolve.tmpBody[iBody].dVelZ;
        control->Evolve.tmpUpdate[iBody].iNumBodies[iVar] =
              malloc(update[iBody].iNumVelZ * sizeof(int));
        control->Evolve.tmpUpdate[iBody].daDerivProc[iVar] =
              malloc(update[iBody].iNumVelZ * sizeof(double));
        control->Evolve.tmpUpdate[iBody].iaType[iVar] =
              malloc(update[iBody].iNumVelZ * sizeof(int));
        control->Evolve.tmpUpdate[iBody].iaModule[iVar] =
              malloc(update[iBody].iNumVelZ * sizeof(int));
        control->Evolve.tmpUpdate[iBody].iaBody[iVar] =
              malloc(update[iBody].iNumVelZ * sizeof(int *));
        for (iSubStep=0; iSubStep < 4; iSubStep++) {
            control->Evolve.daDerivProc[iSubStep][iBody][iVar] =
                  malloc(update[iBody].iNumEqns[iVar] * sizeof(double));
        }
      }

      iEqn = 0;
      for (iModule = 0; iModule < module->iNumModules[iBody]; iModule++) {
        module->fnFinalizeUpdateVelZ[iBody][iModule](body, update, &iEqn, iVar,
                                                     iBody, iFoo);
      }

      (*fnUpdate)[iBody][iVar]        = malloc(iEqn * sizeof(fnUpdateVariable));
      update[iBody].daDerivProc[iVar] = malloc(iEqn * sizeof(double));
       iVar++;
    }

    update[iBody].iPositionX = -1;
    if (update[iBody].iNumPositionX) {
      update[iBody].iPositionX     = iVar;
      update[iBody].iaVar[iVar]    = VPOSITIONX;
      update[iBody].iNumEqns[iVar] = update[iBody].iNumPositionX;
      update[iBody].pdVar[iVar]    = &body[iBody].dPositionX;
      update[iBody].iNumBodies[iVar] =
            malloc(update[iBody].iNumPositionX * sizeof(int));
      update[iBody].iaBody[iVar] =
            malloc(update[iBody].iNumPositionX * sizeof(int *));
      update[iBody].iaType[iVar] =
            malloc(update[iBody].iNumPositionX * sizeof(int));
      update[iBody].iaModule[iVar] =
            malloc(update[iBody].iNumPositionX * sizeof(int));

      if (control->Evolve.iOneStep == RUNGEKUTTA) {
        control->Evolve.tmpUpdate[iBody].pdVar[iVar] =
              &control->Evolve.tmpBody[iBody].dPositionX;
        control->Evolve.tmpUpdate[iBody].iNumBodies[iVar] =
              malloc(update[iBody].iNumPositionX * sizeof(int));
        control->Evolve.tmpUpdate[iBody].daDerivProc[iVar] =
              malloc(update[iBody].iNumPositionX * sizeof(double));
        control->Evolve.tmpUpdate[iBody].iaType[iVar] =
              malloc(update[iBody].iNumPositionX * sizeof(int));
        control->Evolve.tmpUpdate[iBody].iaModule[iVar] =
              malloc(update[iBody].iNumPositionX * sizeof(int));
        control->Evolve.tmpUpdate[iBody].iaBody[iVar] =
              malloc(update[iBody].iNumPositionX * sizeof(int *));
        for (iSubStep=0; iSubStep < 4; iSubStep++) {
            control->Evolve.daDerivProc[iSubStep][iBody][iVar] =
                  malloc(update[iBody].iNumEqns[iVar] * sizeof(double));
        }
      }

      iEqn = 0;
      for (iModule = 0; iModule < module->iNumModules[iBody]; iModule++) {
        module->fnFinalizeUpdatePositionX[iBody][iModule](body, update, &iEqn,
                                                          iVar, iBody, iFoo);
      }

      (*fnUpdate)[iBody][iVar]        = malloc(iEqn * sizeof(fnUpdateVariable));
      update[iBody].daDerivProc[iVar] = malloc(iEqn * sizeof(double));
      iVar++;
    }

    update[iBody].iPositionY = -1;
    if (update[iBody].iNumPositionY) {
      update[iBody].iPositionY     = iVar;
      update[iBody].iaVar[iVar]    = VPOSITIONY;
      update[iBody].iNumEqns[iVar] = update[iBody].iNumPositionY;
      update[iBody].pdVar[iVar]    = &body[iBody].dPositionY;
      update[iBody].iNumBodies[iVar] =
            malloc(update[iBody].iNumPositionY * sizeof(int));
      update[iBody].iaBody[iVar] =
            malloc(update[iBody].iNumPositionY * sizeof(int *));
      update[iBody].iaType[iVar] =
            malloc(update[iBody].iNumPositionY * sizeof(int));
      update[iBody].iaModule[iVar] =
            malloc(update[iBody].iNumPositionY * sizeof(int));

      if (control->Evolve.iOneStep == RUNGEKUTTA) {
        control->Evolve.tmpUpdate[iBody].pdVar[iVar] =
              &control->Evolve.tmpBody[iBody].dPositionY;
        control->Evolve.tmpUpdate[iBody].iNumBodies[iVar] =
              malloc(update[iBody].iNumPositionY * sizeof(int));
        control->Evolve.tmpUpdate[iBody].daDerivProc[iVar] =
              malloc(update[iBody].iNumPositionY * sizeof(double));
        control->Evolve.tmpUpdate[iBody].iaType[iVar] =
              malloc(update[iBody].iNumPositionY * sizeof(int));
        control->Evolve.tmpUpdate[iBody].iaModule[iVar] =
              malloc(update[iBody].iNumPositionY * sizeof(int));
        control->Evolve.tmpUpdate[iBody].iaBody[iVar] =
              malloc(update[iBody].iNumPositionY * sizeof(int *));
        for (iSubStep=0; iSubStep < 4; iSubStep++) {
            control->Evolve.daDerivProc[iSubStep][iBody][iVar] =
                  malloc(update[iBody].iNumEqns[iVar] * sizeof(double));
        }
      }

      iEqn = 0;
      for (iModule = 0; iModule < module->iNumModules[iBody]; iModule++) {
        module->fnFinalizeUpdatePositionY[iBody][iModule](body, update, &iEqn,
                                                          iVar, iBody, iFoo);
      }

      (*fnUpdate)[iBody][iVar]        = malloc(iEqn * sizeof(fnUpdateVariable));
      update[iBody].daDerivProc[iVar] = malloc(iEqn * sizeof(double));
      iVar++;
    }

    update[iBody].iPositionZ = -1;
    if (update[iBody].iNumPositionZ) {
      update[iBody].iPositionZ     = iVar;
      update[iBody].iaVar[iVar]    = VPOSITIONZ;
      update[iBody].iNumEqns[iVar] = update[iBody].iNumPositionZ;
      update[iBody].pdVar[iVar]    = &body[iBody].dPositionZ;
      update[iBody].iNumBodies[iVar] =
            malloc(update[iBody].iNumPositionZ * sizeof(int));
      update[iBody].iaBody[iVar] =
            malloc(update[iBody].iNumPositionZ * sizeof(int *));
      update[iBody].iaType[iVar] =
            malloc(update[iBody].iNumPositionZ * sizeof(int));
      update[iBody].iaModule[iVar] =
            malloc(update[iBody].iNumPositionZ * sizeof(int));

      if (control->Evolve.iOneStep == RUNGEKUTTA) {
        control->Evolve.tmpUpdate[iBody].pdVar[iVar] =
              &control->Evolve.tmpBody[iBody].dPositionZ;
        control->Evolve.tmpUpdate[iBody].iNumBodies[iVar] =
              malloc(update[iBody].iNumPositionZ * sizeof(int));
        control->Evolve.tmpUpdate[iBody].daDerivProc[iVar] =
              malloc(update[iBody].iNumPositionZ * sizeof(double));
        control->Evolve.tmpUpdate[iBody].iaType[iVar] =
              malloc(update[iBody].iNumPositionZ * sizeof(int));
        control->Evolve.tmpUpdate[iBody].iaModule[iVar] =
              malloc(update[iBody].iNumPositionZ * sizeof(int));
        control->Evolve.tmpUpdate[iBody].iaBody[iVar] =
              malloc(update[iBody].iNumPositionZ * sizeof(int *));
        for (iSubStep=0; iSubStep < 4; iSubStep++) {
            control->Evolve.daDerivProc[iSubStep][iBody][iVar] =
                  malloc(update[iBody].iNumEqns[iVar] * sizeof(double));
        }
      }

      iEqn = 0;
      for (iModule = 0; iModule < module->iNumModules[iBody]; iModule++) {
        module->fnFinalizeUpdatePositionZ[iBody][iModule](body, update, &iEqn,
                                                          iVar, iBody, iFoo);
      }

      (*fnUpdate)[iBody][iVar]        = malloc(iEqn * sizeof(fnUpdateVariable));
      update[iBody].daDerivProc[iVar] = malloc(iEqn * sizeof(double));
      iVar++;
    }
    /* HERE MAGMOC
     * copy that for every primary variable
     */
    update[iBody].iWaterMassMOAtm = -1;
    if (update[iBody].iNumWaterMassMOAtm) {
      update[iBody].iWaterMassMOAtm = iVar;
      update[iBody].iaVar[iVar]     = VWATERMASSMOATM;
      update[iBody].iNumEqns[iVar]  = update[iBody].iNumWaterMassMOAtm;
      update[iBody].pdVar[iVar]     = &body[iBody].dWaterMassMOAtm;
      update[iBody].iNumBodies[iVar] =
            malloc(update[iBody].iNumWaterMassMOAtm * sizeof(int));
      update[iBody].iaBody[iVar] =
            malloc(update[iBody].iNumWaterMassMOAtm * sizeof(int *));
      update[iBody].iaType[iVar] =
            malloc(update[iBody].iNumWaterMassMOAtm * sizeof(int));
      update[iBody].iaModule[iVar] =
            malloc(update[iBody].iNumWaterMassMOAtm * sizeof(int));

      if (control->Evolve.iOneStep == RUNGEKUTTA) {
        control->Evolve.tmpUpdate[iBody].pdVar[iVar] =
              &control->Evolve.tmpBody[iBody].dWaterMassMOAtm;
        control->Evolve.tmpUpdate[iBody].iNumBodies[iVar] =
              malloc(update[iBody].iNumWaterMassMOAtm * sizeof(int));
        control->Evolve.tmpUpdate[iBody].daDerivProc[iVar] =
              malloc(update[iBody].iNumWaterMassMOAtm * sizeof(double));
        control->Evolve.tmpUpdate[iBody].iaType[iVar] =
              malloc(update[iBody].iNumWaterMassMOAtm * sizeof(int));
        control->Evolve.tmpUpdate[iBody].iaModule[iVar] =
              malloc(update[iBody].iNumWaterMassMOAtm * sizeof(int));
        control->Evolve.tmpUpdate[iBody].iaBody[iVar] =
              malloc(update[iBody].iNumWaterMassMOAtm * sizeof(int *));
        for (iSubStep=0; iSubStep < 4; iSubStep++) {
            control->Evolve.daDerivProc[iSubStep][iBody][iVar] =
                  malloc(update[iBody].iNumEqns[iVar] * sizeof(double));
        }
      }

      iEqn = 0;
      for (iModule = 0; iModule < module->iNumModules[iBody]; iModule++) {
        module->fnFinalizeUpdateWaterMassMOAtm[iBody][iModule](
              body, update, &iEqn, iVar, iBody, iFoo);
      }

      (*fnUpdate)[iBody][iVar]        = malloc(iEqn * sizeof(fnUpdateVariable));
      update[iBody].daDerivProc[iVar] = malloc(iEqn * sizeof(double));
      iVar++;
    }
    /* until HERE */
    update[iBody].iWaterMassSol = -1;
    if (update[iBody].iNumWaterMassSol) {
      update[iBody].iWaterMassSol  = iVar;
      update[iBody].iaVar[iVar]    = VWATERMASSSOL;
      update[iBody].iNumEqns[iVar] = update[iBody].iNumWaterMassSol;
      update[iBody].pdVar[iVar]    = &body[iBody].dWaterMassSol;
      update[iBody].iNumBodies[iVar] =
            malloc(update[iBody].iNumWaterMassSol * sizeof(int));
      update[iBody].iaBody[iVar] =
            malloc(update[iBody].iNumWaterMassSol * sizeof(int *));
      update[iBody].iaType[iVar] =
            malloc(update[iBody].iNumWaterMassSol * sizeof(int));
      update[iBody].iaModule[iVar] =
            malloc(update[iBody].iNumWaterMassSol * sizeof(int));

      if (control->Evolve.iOneStep == RUNGEKUTTA) {
        control->Evolve.tmpUpdate[iBody].pdVar[iVar] =
              &control->Evolve.tmpBody[iBody].dWaterMassSol;
        control->Evolve.tmpUpdate[iBody].iNumBodies[iVar] =
              malloc(update[iBody].iNumWaterMassSol * sizeof(int));
        control->Evolve.tmpUpdate[iBody].daDerivProc[iVar] =
              malloc(update[iBody].iNumWaterMassSol * sizeof(double));
        control->Evolve.tmpUpdate[iBody].iaType[iVar] =
              malloc(update[iBody].iNumWaterMassSol * sizeof(int));
        control->Evolve.tmpUpdate[iBody].iaModule[iVar] =
              malloc(update[iBody].iNumWaterMassSol * sizeof(int));
        control->Evolve.tmpUpdate[iBody].iaBody[iVar] =
              malloc(update[iBody].iNumWaterMassSol * sizeof(int *));
        for (iSubStep=0; iSubStep < 4; iSubStep++) {
            control->Evolve.daDerivProc[iSubStep][iBody][iVar] =
                  malloc(update[iBody].iNumEqns[iVar] * sizeof(double));
        }
      }

      iEqn = 0;
      for (iModule = 0; iModule < module->iNumModules[iBody]; iModule++) {
        module->fnFinalizeUpdateWaterMassSol[iBody][iModule](
              body, update, &iEqn, iVar, iBody, iFoo);
      }

      (*fnUpdate)[iBody][iVar]        = malloc(iEqn * sizeof(fnUpdateVariable));
      update[iBody].daDerivProc[iVar] = malloc(iEqn * sizeof(double));
      iVar++;
    }

    update[iBody].iSurfTemp = -1;
    if (update[iBody].iNumSurfTemp) {
      update[iBody].iSurfTemp      = iVar;
      update[iBody].iaVar[iVar]    = VSURFTEMP;
      update[iBody].iNumEqns[iVar] = update[iBody].iNumSurfTemp;
      update[iBody].pdVar[iVar]    = &body[iBody].dSurfTemp;
      update[iBody].iNumBodies[iVar] =
            malloc(update[iBody].iNumSurfTemp * sizeof(int));
      update[iBody].iaBody[iVar] =
            malloc(update[iBody].iNumSurfTemp * sizeof(int *));
      update[iBody].iaType[iVar] =
            malloc(update[iBody].iNumSurfTemp * sizeof(int));
      update[iBody].iaModule[iVar] =
            malloc(update[iBody].iNumSurfTemp * sizeof(int));

      if (control->Evolve.iOneStep == RUNGEKUTTA) {
        control->Evolve.tmpUpdate[iBody].pdVar[iVar] =
              &control->Evolve.tmpBody[iBody].dSurfTemp;
        control->Evolve.tmpUpdate[iBody].iNumBodies[iVar] =
              malloc(update[iBody].iNumSurfTemp * sizeof(int));
        control->Evolve.tmpUpdate[iBody].daDerivProc[iVar] =
              malloc(update[iBody].iNumSurfTemp * sizeof(double));
        control->Evolve.tmpUpdate[iBody].iaType[iVar] =
              malloc(update[iBody].iNumSurfTemp * sizeof(int));
        control->Evolve.tmpUpdate[iBody].iaModule[iVar] =
              malloc(update[iBody].iNumSurfTemp * sizeof(int));
        control->Evolve.tmpUpdate[iBody].iaBody[iVar] =
              malloc(update[iBody].iNumSurfTemp * sizeof(int *));
        for (iSubStep=0; iSubStep < 4; iSubStep++) {
            control->Evolve.daDerivProc[iSubStep][iBody][iVar] =
                  malloc(update[iBody].iNumEqns[iVar] * sizeof(double));
        }
      }

      iEqn = 0;
      for (iModule = 0; iModule < module->iNumModules[iBody]; iModule++) {
        module->fnFinalizeUpdateSurfTemp[iBody][iModule](body, update, &iEqn,
                                                         iVar, iBody, iFoo);
      }

      (*fnUpdate)[iBody][iVar]        = malloc(iEqn * sizeof(fnUpdateVariable));
      update[iBody].daDerivProc[iVar] = malloc(iEqn * sizeof(double));
      iVar++;
    }

    update[iBody].iSolidRadius = -1;
    if (update[iBody].iNumSolidRadius) {
      update[iBody].iSolidRadius   = iVar;
      update[iBody].iaVar[iVar]    = VSOLIDRADIUS;
      update[iBody].iNumEqns[iVar] = update[iBody].iNumSolidRadius;
      update[iBody].pdVar[iVar]    = &body[iBody].dSolidRadius;
      update[iBody].iNumBodies[iVar] =
            malloc(update[iBody].iNumSolidRadius * sizeof(int));
      update[iBody].iaBody[iVar] =
            malloc(update[iBody].iNumSolidRadius * sizeof(int *));
      update[iBody].iaType[iVar] =
            malloc(update[iBody].iNumSolidRadius * sizeof(int));
      update[iBody].iaModule[iVar] =
            malloc(update[iBody].iNumSolidRadius * sizeof(int));

      if (control->Evolve.iOneStep == RUNGEKUTTA) {
        control->Evolve.tmpUpdate[iBody].pdVar[iVar] =
              &control->Evolve.tmpBody[iBody].dSolidRadius;
        control->Evolve.tmpUpdate[iBody].iNumBodies[iVar] =
              malloc(update[iBody].iNumSolidRadius * sizeof(int));
        control->Evolve.tmpUpdate[iBody].daDerivProc[iVar] =
              malloc(update[iBody].iNumSolidRadius * sizeof(double));
        control->Evolve.tmpUpdate[iBody].iaType[iVar] =
              malloc(update[iBody].iNumSolidRadius * sizeof(int));
        control->Evolve.tmpUpdate[iBody].iaModule[iVar] =
              malloc(update[iBody].iNumSolidRadius * sizeof(int));
        control->Evolve.tmpUpdate[iBody].iaBody[iVar] =
              malloc(update[iBody].iNumSolidRadius * sizeof(int *));
        for (iSubStep=0; iSubStep < 4; iSubStep++) {
            control->Evolve.daDerivProc[iSubStep][iBody][iVar] =
                  malloc(update[iBody].iNumEqns[iVar] * sizeof(double));
        }
      }

      iEqn = 0;
      for (iModule = 0; iModule < module->iNumModules[iBody]; iModule++) {
        module->fnFinalizeUpdateSolidRadius[iBody][iModule](body, update, &iEqn,
                                                            iVar, iBody, iFoo);
      }

      (*fnUpdate)[iBody][iVar]        = malloc(iEqn * sizeof(fnUpdateVariable));
      update[iBody].daDerivProc[iVar] = malloc(iEqn * sizeof(double));
      iVar++;
    }

    update[iBody].iPotTemp = -1;
    if (update[iBody].iNumPotTemp) {
      update[iBody].iPotTemp       = iVar;
      update[iBody].iaVar[iVar]    = VPOTTEMP;
      update[iBody].iNumEqns[iVar] = update[iBody].iNumPotTemp;
      update[iBody].pdVar[iVar]    = &body[iBody].dPotTemp;
      update[iBody].iNumBodies[iVar] =
            malloc(update[iBody].iNumPotTemp * sizeof(int));
      update[iBody].iaBody[iVar] =
            malloc(update[iBody].iNumPotTemp * sizeof(int *));
      update[iBody].iaType[iVar] =
            malloc(update[iBody].iNumPotTemp * sizeof(int));
      update[iBody].iaModule[iVar] =
            malloc(update[iBody].iNumPotTemp * sizeof(int));

      if (control->Evolve.iOneStep == RUNGEKUTTA) {
        control->Evolve.tmpUpdate[iBody].pdVar[iVar] =
              &control->Evolve.tmpBody[iBody].dPotTemp;
        control->Evolve.tmpUpdate[iBody].iNumBodies[iVar] =
              malloc(update[iBody].iNumPotTemp * sizeof(int));
        control->Evolve.tmpUpdate[iBody].daDerivProc[iVar] =
              malloc(update[iBody].iNumPotTemp * sizeof(double));
        control->Evolve.tmpUpdate[iBody].iaType[iVar] =
              malloc(update[iBody].iNumPotTemp * sizeof(int));
        control->Evolve.tmpUpdate[iBody].iaModule[iVar] =
              malloc(update[iBody].iNumPotTemp * sizeof(int));
        control->Evolve.tmpUpdate[iBody].iaBody[iVar] =
              malloc(update[iBody].iNumPotTemp * sizeof(int *));
        for (iSubStep=0; iSubStep < 4; iSubStep++) {
            control->Evolve.daDerivProc[iSubStep][iBody][iVar] =
                  malloc(update[iBody].iNumEqns[iVar] * sizeof(double));
        }
      }

      iEqn = 0;
      for (iModule = 0; iModule < module->iNumModules[iBody]; iModule++) {
        module->fnFinalizeUpdatePotTemp[iBody][iModule](body, update, &iEqn,
                                                        iVar, iBody, iFoo);
      }

      (*fnUpdate)[iBody][iVar]        = malloc(iEqn * sizeof(fnUpdateVariable));
      update[iBody].daDerivProc[iVar] = malloc(iEqn * sizeof(double));
      iVar++;
    }

    update[iBody].iOxygenMassMOAtm = -1;
    if (update[iBody].iNumOxygenMassMOAtm) {
      update[iBody].iOxygenMassMOAtm = iVar;
      update[iBody].iaVar[iVar]      = VOXYGENMASSMOATM;
      update[iBody].iNumEqns[iVar]   = update[iBody].iNumOxygenMassMOAtm;
      update[iBody].pdVar[iVar]      = &body[iBody].dOxygenMassMOAtm;
      update[iBody].iNumBodies[iVar] =
            malloc(update[iBody].iNumOxygenMassMOAtm * sizeof(int));
      update[iBody].iaBody[iVar] =
            malloc(update[iBody].iNumOxygenMassMOAtm * sizeof(int *));
      update[iBody].iaType[iVar] =
            malloc(update[iBody].iNumOxygenMassMOAtm * sizeof(int));
      update[iBody].iaModule[iVar] =
            malloc(update[iBody].iNumOxygenMassMOAtm * sizeof(int));

      if (control->Evolve.iOneStep == RUNGEKUTTA) {
        control->Evolve.tmpUpdate[iBody].pdVar[iVar] =
              &control->Evolve.tmpBody[iBody].dOxygenMassMOAtm;
        control->Evolve.tmpUpdate[iBody].iNumBodies[iVar] =
              malloc(update[iBody].iNumOxygenMassMOAtm * sizeof(int));
        control->Evolve.tmpUpdate[iBody].daDerivProc[iVar] =
              malloc(update[iBody].iNumOxygenMassMOAtm * sizeof(double));
        control->Evolve.tmpUpdate[iBody].iaType[iVar] =
              malloc(update[iBody].iNumOxygenMassMOAtm * sizeof(int));
        control->Evolve.tmpUpdate[iBody].iaModule[iVar] =
              malloc(update[iBody].iNumOxygenMassMOAtm * sizeof(int));
        control->Evolve.tmpUpdate[iBody].iaBody[iVar] =
              malloc(update[iBody].iNumOxygenMassMOAtm * sizeof(int *));
        for (iSubStep=0; iSubStep < 4; iSubStep++) {
            control->Evolve.daDerivProc[iSubStep][iBody][iVar] =
                  malloc(update[iBody].iNumEqns[iVar] * sizeof(double));
        }
      }

      iEqn = 0;
      for (iModule = 0; iModule < module->iNumModules[iBody]; iModule++) {
        module->fnFinalizeUpdateOxygenMassMOAtm[iBody][iModule](
              body, update, &iEqn, iVar, iBody, iFoo);
      }

      (*fnUpdate)[iBody][iVar]        = malloc(iEqn * sizeof(fnUpdateVariable));
      update[iBody].daDerivProc[iVar] = malloc(iEqn * sizeof(double));
       iVar++;
    }
    /* until HERE */
    update[iBody].iOxygenMassSol = -1;
    if (update[iBody].iNumOxygenMassSol) {
      update[iBody].iOxygenMassSol = iVar;
      update[iBody].iaVar[iVar]    = VOXYGENMASSSOL;
      update[iBody].iNumEqns[iVar] = update[iBody].iNumOxygenMassSol;
      update[iBody].pdVar[iVar]    = &body[iBody].dOxygenMassSol;
      update[iBody].iNumBodies[iVar] =
            malloc(update[iBody].iNumOxygenMassSol * sizeof(int));
      update[iBody].iaBody[iVar] =
            malloc(update[iBody].iNumOxygenMassSol * sizeof(int *));
      update[iBody].iaType[iVar] =
            malloc(update[iBody].iNumOxygenMassSol * sizeof(int));
      update[iBody].iaModule[iVar] =
            malloc(update[iBody].iNumOxygenMassSol * sizeof(int));

      if (control->Evolve.iOneStep == RUNGEKUTTA) {
        control->Evolve.tmpUpdate[iBody].pdVar[iVar] =
              &control->Evolve.tmpBody[iBody].dOxygenMassSol;
        control->Evolve.tmpUpdate[iBody].iNumBodies[iVar] =
              malloc(update[iBody].iNumOxygenMassSol * sizeof(int));
        control->Evolve.tmpUpdate[iBody].daDerivProc[iVar] =
              malloc(update[iBody].iNumOxygenMassSol * sizeof(double));
        control->Evolve.tmpUpdate[iBody].iaType[iVar] =
              malloc(update[iBody].iNumOxygenMassSol * sizeof(int));
        control->Evolve.tmpUpdate[iBody].iaModule[iVar] =
              malloc(update[iBody].iNumOxygenMassSol * sizeof(int));
        control->Evolve.tmpUpdate[iBody].iaBody[iVar] =
              malloc(update[iBody].iNumOxygenMassSol * sizeof(int *));
        for (iSubStep=0; iSubStep < 4; iSubStep++) {
            control->Evolve.daDerivProc[iSubStep][iBody][iVar] =
                  malloc(update[iBody].iNumEqns[iVar] * sizeof(double));
        }
      }

      iEqn = 0;
      for (iModule = 0; iModule < module->iNumModules[iBody]; iModule++) {
        module->fnFinalizeUpdateOxygenMassSol[iBody][iModule](
              body, update, &iEqn, iVar, iBody, iFoo);
      }

      (*fnUpdate)[iBody][iVar]        = malloc(iEqn * sizeof(fnUpdateVariable));
      update[iBody].daDerivProc[iVar] = malloc(iEqn * sizeof(double));
      iVar++;
    }

    update[iBody].iHydrogenMassSpace = -1;
    if (update[iBody].iNumHydrogenMassSpace) {
      update[iBody].iHydrogenMassSpace = iVar;
      update[iBody].iaVar[iVar]        = VHYDROGENMASSSPACE;
      update[iBody].iNumEqns[iVar]     = update[iBody].iNumHydrogenMassSpace;
      update[iBody].pdVar[iVar]        = &body[iBody].dHydrogenMassSpace;
      update[iBody].iNumBodies[iVar] =
            malloc(update[iBody].iNumHydrogenMassSpace * sizeof(int));
      update[iBody].iaBody[iVar] =
            malloc(update[iBody].iNumHydrogenMassSpace * sizeof(int *));
      update[iBody].iaType[iVar] =
            malloc(update[iBody].iNumHydrogenMassSpace * sizeof(int));
      update[iBody].iaModule[iVar] =
            malloc(update[iBody].iNumHydrogenMassSpace * sizeof(int));

      if (control->Evolve.iOneStep == RUNGEKUTTA) {
        control->Evolve.tmpUpdate[iBody].pdVar[iVar] =
              &control->Evolve.tmpBody[iBody].dHydrogenMassSpace;
        control->Evolve.tmpUpdate[iBody].iNumBodies[iVar] =
              malloc(update[iBody].iNumHydrogenMassSpace * sizeof(int));
        control->Evolve.tmpUpdate[iBody].daDerivProc[iVar] =
              malloc(update[iBody].iNumHydrogenMassSpace * sizeof(double));
        control->Evolve.tmpUpdate[iBody].iaType[iVar] =
              malloc(update[iBody].iNumHydrogenMassSpace * sizeof(int));
        control->Evolve.tmpUpdate[iBody].iaModule[iVar] =
              malloc(update[iBody].iNumHydrogenMassSpace * sizeof(int));
        control->Evolve.tmpUpdate[iBody].iaBody[iVar] =
              malloc(update[iBody].iNumHydrogenMassSpace * sizeof(int *));
        for (iSubStep=0; iSubStep < 4; iSubStep++) {
            control->Evolve.daDerivProc[iSubStep][iBody][iVar] =
                  malloc(update[iBody].iNumEqns[iVar] * sizeof(double));
        }
      }

      iEqn = 0;
      for (iModule = 0; iModule < module->iNumModules[iBody]; iModule++) {
        module->fnFinalizeUpdateHydrogenMassSpace[iBody][iModule](
              body, update, &iEqn, iVar, iBody, iFoo);
      }

      (*fnUpdate)[iBody][iVar]        = malloc(iEqn * sizeof(fnUpdateVariable));
      update[iBody].daDerivProc[iVar] = malloc(iEqn * sizeof(double));
      iVar++;
    }

    update[iBody].iOxygenMassSpace = -1;
    if (update[iBody].iNumOxygenMassSpace) {
      update[iBody].iOxygenMassSpace = iVar;
      update[iBody].iaVar[iVar]      = VOXYGENMASSSPACE;
      update[iBody].iNumEqns[iVar]   = update[iBody].iNumOxygenMassSpace;
      update[iBody].pdVar[iVar]      = &body[iBody].dOxygenMassSpace;
      update[iBody].iNumBodies[iVar] =
            malloc(update[iBody].iNumOxygenMassSpace * sizeof(int));
      update[iBody].iaBody[iVar] =
            malloc(update[iBody].iNumOxygenMassSpace * sizeof(int *));
      update[iBody].iaType[iVar] =
            malloc(update[iBody].iNumOxygenMassSpace * sizeof(int));
      update[iBody].iaModule[iVar] =
            malloc(update[iBody].iNumOxygenMassSpace * sizeof(int));

      if (control->Evolve.iOneStep == RUNGEKUTTA) {
        control->Evolve.tmpUpdate[iBody].pdVar[iVar] =
              &control->Evolve.tmpBody[iBody].dOxygenMassSpace;
        control->Evolve.tmpUpdate[iBody].iNumBodies[iVar] =
              malloc(update[iBody].iNumOxygenMassSpace * sizeof(int));
        control->Evolve.tmpUpdate[iBody].daDerivProc[iVar] =
              malloc(update[iBody].iNumOxygenMassSpace * sizeof(double));
        control->Evolve.tmpUpdate[iBody].iaType[iVar] =
              malloc(update[iBody].iNumOxygenMassSpace * sizeof(int));
        control->Evolve.tmpUpdate[iBody].iaModule[iVar] =
              malloc(update[iBody].iNumOxygenMassSpace * sizeof(int));
        control->Evolve.tmpUpdate[iBody].iaBody[iVar] =
              malloc(update[iBody].iNumOxygenMassSpace * sizeof(int *));
        for (iSubStep=0; iSubStep < 4; iSubStep++) {
            control->Evolve.daDerivProc[iSubStep][iBody][iVar] =
                  malloc(update[iBody].iNumEqns[iVar] * sizeof(double));
        }
      }

      iEqn = 0;
      for (iModule = 0; iModule < module->iNumModules[iBody]; iModule++) {
        module->fnFinalizeUpdateOxygenMassSpace[iBody][iModule](
              body, update, &iEqn, iVar, iBody, iFoo);
      }

      (*fnUpdate)[iBody][iVar]        = malloc(iEqn * sizeof(fnUpdateVariable));
      update[iBody].daDerivProc[iVar] = malloc(iEqn * sizeof(double));
      iVar++;
    }

    update[iBody].iCO2MassMOAtm = -1;
    if (update[iBody].iNumCO2MassMOAtm) {
      update[iBody].iCO2MassMOAtm  = iVar;
      update[iBody].iaVar[iVar]    = VCO2MASSMOATM;
      update[iBody].iNumEqns[iVar] = update[iBody].iNumCO2MassMOAtm;
      update[iBody].pdVar[iVar]    = &body[iBody].dCO2MassMOAtm;
      update[iBody].iNumBodies[iVar] =
            malloc(update[iBody].iNumCO2MassMOAtm * sizeof(int));
      update[iBody].iaBody[iVar] =
            malloc(update[iBody].iNumCO2MassMOAtm * sizeof(int *));
      update[iBody].iaType[iVar] =
            malloc(update[iBody].iNumCO2MassMOAtm * sizeof(int));
      update[iBody].iaModule[iVar] =
            malloc(update[iBody].iNumCO2MassMOAtm * sizeof(int));

      if (control->Evolve.iOneStep == RUNGEKUTTA) {
        control->Evolve.tmpUpdate[iBody].pdVar[iVar] =
              &control->Evolve.tmpBody[iBody].dCO2MassMOAtm;
        control->Evolve.tmpUpdate[iBody].iNumBodies[iVar] =
              malloc(update[iBody].iNumCO2MassMOAtm * sizeof(int));
        control->Evolve.tmpUpdate[iBody].daDerivProc[iVar] =
              malloc(update[iBody].iNumCO2MassMOAtm * sizeof(double));
        control->Evolve.tmpUpdate[iBody].iaType[iVar] =
              malloc(update[iBody].iNumCO2MassMOAtm * sizeof(int));
        control->Evolve.tmpUpdate[iBody].iaModule[iVar] =
              malloc(update[iBody].iNumCO2MassMOAtm * sizeof(int));
        control->Evolve.tmpUpdate[iBody].iaBody[iVar] =
              malloc(update[iBody].iNumCO2MassMOAtm * sizeof(int *));
        for (iSubStep=0; iSubStep < 4; iSubStep++) {
            control->Evolve.daDerivProc[iSubStep][iBody][iVar] =
                  malloc(update[iBody].iNumEqns[iVar] * sizeof(double));
        }
      }

      iEqn = 0;
      for (iModule = 0; iModule < module->iNumModules[iBody]; iModule++) {
        module->fnFinalizeUpdateCO2MassMOAtm[iBody][iModule](
              body, update, &iEqn, iVar, iBody, iFoo);
      }

      (*fnUpdate)[iBody][iVar]        = malloc(iEqn * sizeof(fnUpdateVariable));
      update[iBody].daDerivProc[iVar] = malloc(iEqn * sizeof(double));
      iVar++;
    }

    update[iBody].iCO2MassSol = -1;
    if (update[iBody].iNumCO2MassSol) {
      update[iBody].iCO2MassSol    = iVar;
      update[iBody].iaVar[iVar]    = VCO2MASSSOL;
      update[iBody].iNumEqns[iVar] = update[iBody].iNumCO2MassSol;
      update[iBody].pdVar[iVar]    = &body[iBody].dCO2MassSol;
      update[iBody].iNumBodies[iVar] =
            malloc(update[iBody].iNumCO2MassSol * sizeof(int));
      update[iBody].iaBody[iVar] =
            malloc(update[iBody].iNumCO2MassSol * sizeof(int *));
      update[iBody].iaType[iVar] =
            malloc(update[iBody].iNumCO2MassSol * sizeof(int));
      update[iBody].iaModule[iVar] =
            malloc(update[iBody].iNumCO2MassSol * sizeof(int));

      if (control->Evolve.iOneStep == RUNGEKUTTA) {
        control->Evolve.tmpUpdate[iBody].pdVar[iVar] =
              &control->Evolve.tmpBody[iBody].dCO2MassSol;
        control->Evolve.tmpUpdate[iBody].iNumBodies[iVar] =
              malloc(update[iBody].iNumCO2MassSol * sizeof(int));
        control->Evolve.tmpUpdate[iBody].daDerivProc[iVar] =
              malloc(update[iBody].iNumCO2MassSol * sizeof(double));
        control->Evolve.tmpUpdate[iBody].iaType[iVar] =
              malloc(update[iBody].iNumCO2MassSol * sizeof(int));
        control->Evolve.tmpUpdate[iBody].iaModule[iVar] =
              malloc(update[iBody].iNumCO2MassSol * sizeof(int));
        control->Evolve.tmpUpdate[iBody].iaBody[iVar] =
              malloc(update[iBody].iNumCO2MassSol * sizeof(int *));
        for (iSubStep=0; iSubStep < 4; iSubStep++) {
            control->Evolve.daDerivProc[iSubStep][iBody][iVar] =
                  malloc(update[iBody].iNumEqns[iVar] * sizeof(double));
        }
      }

      iEqn = 0;
      for (iModule = 0; iModule < module->iNumModules[iBody]; iModule++) {
        module->fnFinalizeUpdateCO2MassSol[iBody][iModule](body, update, &iEqn,
                                                           iVar, iBody, iFoo);
      }

      (*fnUpdate)[iBody][iVar]        = malloc(iEqn * sizeof(fnUpdateVariable));
      update[iBody].daDerivProc[iVar] = malloc(iEqn * sizeof(double));
       iVar++;
    }
    /* HERE
     * End of MagmOc variables
     */

    /* 26Al Core */
    update[iBody].i26AlCore = -1;
    if (update[iBody].iNum26AlCore) {
      update[iBody].i26AlCore      = iVar;
      update[iBody].iaVar[iVar]    = VNUM26ALCORE;
      update[iBody].iNumEqns[iVar] = update[iBody].iNum26AlCore;
      update[iBody].pdVar[iVar]    = &body[iBody].d26AlNumCore;
      update[iBody].iNumBodies[iVar] =
            malloc(update[iBody].iNum26AlCore * sizeof(int));
      update[iBody].iaBody[iVar] =
            malloc(update[iBody].iNum26AlCore * sizeof(int *));
      update[iBody].iaType[iVar] =
            malloc(update[iBody].iNum26AlCore * sizeof(int));
      update[iBody].iaModule[iVar] =
            malloc(update[iBody].iNum26AlCore * sizeof(int));

      if (control->Evolve.iOneStep == RUNGEKUTTA) {
        control->Evolve.tmpUpdate[iBody].pdVar[iVar] =
              &control->Evolve.tmpBody[iBody].d26AlNumCore;
        control->Evolve.tmpUpdate[iBody].iNumBodies[iVar] =
              malloc(update[iBody].iNum26AlCore * sizeof(int));
        control->Evolve.tmpUpdate[iBody].daDerivProc[iVar] =
              malloc(update[iBody].iNum26AlCore * sizeof(double));
        control->Evolve.tmpUpdate[iBody].iaType[iVar] =
              malloc(update[iBody].iNum26AlCore * sizeof(int));
        control->Evolve.tmpUpdate[iBody].iaModule[iVar] =
              malloc(update[iBody].iNum26AlCore * sizeof(int));
        control->Evolve.tmpUpdate[iBody].iaBody[iVar] =
              malloc(update[iBody].iNum26AlCore * sizeof(int *));
        for (iSubStep=0; iSubStep < 4; iSubStep++) {
            control->Evolve.daDerivProc[iSubStep][iBody][iVar] =
                  malloc(update[iBody].iNumEqns[iVar] * sizeof(double));
        }
      }

      iEqn = 0;
      for (iModule = 0; iModule < module->iNumModules[iBody]; iModule++) {
        module->fnFinalizeUpdate26AlNumCore[iBody][iModule](body, update, &iEqn,
                                                            iVar, iBody, iFoo);
      }

      (*fnUpdate)[iBody][iVar]        = malloc(iEqn * sizeof(fnUpdateVariable));
      update[iBody].daDerivProc[iVar] = malloc(iEqn * sizeof(double));
      iVar++;
    }

    /* 26Al Mantle */
    update[iBody].i26AlMan = -1;
    if (update[iBody].iNum26AlMan) {
      update[iBody].i26AlMan       = iVar;
      update[iBody].iaVar[iVar]    = VNUM26ALMAN;
      update[iBody].iNumEqns[iVar] = update[iBody].iNum26AlMan;
      update[iBody].pdVar[iVar]    = &body[iBody].d26AlNumMan;
      update[iBody].iNumBodies[iVar] =
            malloc(update[iBody].iNum26AlMan * sizeof(int));
      update[iBody].iaBody[iVar] =
            malloc(update[iBody].iNum26AlMan * sizeof(int *));
      update[iBody].iaType[iVar] =
            malloc(update[iBody].iNum26AlMan * sizeof(int));
      update[iBody].iaModule[iVar] =
            malloc(update[iBody].iNum26AlMan * sizeof(int));

      if (control->Evolve.iOneStep == RUNGEKUTTA) {
        control->Evolve.tmpUpdate[iBody].pdVar[iVar] =
              &control->Evolve.tmpBody[iBody].d26AlNumMan;
        control->Evolve.tmpUpdate[iBody].iNumBodies[iVar] =
              malloc(update[iBody].iNum26AlMan * sizeof(int));
        control->Evolve.tmpUpdate[iBody].daDerivProc[iVar] =
              malloc(update[iBody].iNum26AlMan * sizeof(double));
        control->Evolve.tmpUpdate[iBody].iaType[iVar] =
              malloc(update[iBody].iNum26AlMan * sizeof(int));
        control->Evolve.tmpUpdate[iBody].iaModule[iVar] =
              malloc(update[iBody].iNum26AlMan * sizeof(int));
        control->Evolve.tmpUpdate[iBody].iaBody[iVar] =
              malloc(update[iBody].iNum26AlMan * sizeof(int *));
        for (iSubStep=0; iSubStep < 4; iSubStep++) {
            control->Evolve.daDerivProc[iSubStep][iBody][iVar] =
                  malloc(update[iBody].iNumEqns[iVar] * sizeof(double));
        }
      }

      iEqn = 0;
      for (iModule = 0; iModule < module->iNumModules[iBody]; iModule++) {
        module->fnFinalizeUpdate26AlNumMan[iBody][iModule](body, update, &iEqn,
                                                           iVar, iBody, iFoo);
      }

      (*fnUpdate)[iBody][iVar]        = malloc(iEqn * sizeof(fnUpdateVariable));
      update[iBody].daDerivProc[iVar] = malloc(iEqn * sizeof(double));
      iVar++;
    }

    /* 40K Core */
    update[iBody].i40KCore = -1;
    if (update[iBody].iNum40KCore) {
      update[iBody].i40KCore       = iVar;
      update[iBody].iaVar[iVar]    = VNUM40KCORE;
      update[iBody].iNumEqns[iVar] = update[iBody].iNum40KCore;
      update[iBody].pdVar[iVar]    = &body[iBody].d40KNumCore;
      update[iBody].iNumBodies[iVar] =
            malloc(update[iBody].iNum40KCore * sizeof(int));
      update[iBody].iaBody[iVar] =
            malloc(update[iBody].iNum40KCore * sizeof(int *));
      update[iBody].iaType[iVar] =
            malloc(update[iBody].iNum40KCore * sizeof(int));
      update[iBody].iaModule[iVar] =
            malloc(update[iBody].iNum40KCore * sizeof(int));

      if (control->Evolve.iOneStep == RUNGEKUTTA) {
        control->Evolve.tmpUpdate[iBody].pdVar[iVar] =
              &control->Evolve.tmpBody[iBody].d40KNumCore;
        control->Evolve.tmpUpdate[iBody].iNumBodies[iVar] =
              malloc(update[iBody].iNum40KCore * sizeof(int));
        control->Evolve.tmpUpdate[iBody].daDerivProc[iVar] =
              malloc(update[iBody].iNum40KCore * sizeof(double));
        control->Evolve.tmpUpdate[iBody].iaType[iVar] =
              malloc(update[iBody].iNum40KCore * sizeof(int));
        control->Evolve.tmpUpdate[iBody].iaModule[iVar] =
              malloc(update[iBody].iNum40KCore * sizeof(int));
        control->Evolve.tmpUpdate[iBody].iaBody[iVar] =
              malloc(update[iBody].iNum40KCore * sizeof(int *));
        for (iSubStep=0; iSubStep < 4; iSubStep++) {
            control->Evolve.daDerivProc[iSubStep][iBody][iVar] =
                  malloc(update[iBody].iNumEqns[iVar] * sizeof(double));
        }
      }

      iEqn = 0;
      for (iModule = 0; iModule < module->iNumModules[iBody]; iModule++) {
        module->fnFinalizeUpdate40KNumCore[iBody][iModule](body, update, &iEqn,
                                                           iVar, iBody, iFoo);
      }

      (*fnUpdate)[iBody][iVar]        = malloc(iEqn * sizeof(fnUpdateVariable));
      update[iBody].daDerivProc[iVar] = malloc(iEqn * sizeof(double));
      iVar++;
    }

    /* 40K Mantle */
    update[iBody].i40KMan = -1;
    if (update[iBody].iNum40KMan) {
      update[iBody].i40KMan        = iVar;
      update[iBody].iaVar[iVar]    = VNUM40KMAN;
      update[iBody].iNumEqns[iVar] = update[iBody].iNum40KMan;
      update[iBody].pdVar[iVar]    = &body[iBody].d40KNumMan;
      update[iBody].iNumBodies[iVar] =
            malloc(update[iBody].iNum40KMan * sizeof(int));
      update[iBody].iaBody[iVar] =
            malloc(update[iBody].iNum40KMan * sizeof(int *));
      update[iBody].iaType[iVar] =
            malloc(update[iBody].iNum40KMan * sizeof(int));
      update[iBody].iaModule[iVar] =
            malloc(update[iBody].iNum40KMan * sizeof(int));

      if (control->Evolve.iOneStep == RUNGEKUTTA) {
        control->Evolve.tmpUpdate[iBody].pdVar[iVar] =
              &control->Evolve.tmpBody[iBody].d40KNumMan;
        control->Evolve.tmpUpdate[iBody].iNumBodies[iVar] =
              malloc(update[iBody].iNum40KMan * sizeof(int));
        control->Evolve.tmpUpdate[iBody].daDerivProc[iVar] =
              malloc(update[iBody].iNum40KMan * sizeof(double));
        control->Evolve.tmpUpdate[iBody].iaType[iVar] =
              malloc(update[iBody].iNum40KMan * sizeof(int));
        control->Evolve.tmpUpdate[iBody].iaModule[iVar] =
              malloc(update[iBody].iNum40KMan * sizeof(int));
        control->Evolve.tmpUpdate[iBody].iaBody[iVar] =
              malloc(update[iBody].iNum40KMan * sizeof(int *));
        for (iSubStep=0; iSubStep < 4; iSubStep++) {
            control->Evolve.daDerivProc[iSubStep][iBody][iVar] =
                  malloc(update[iBody].iNumEqns[iVar] * sizeof(double));
        }
      }

      iEqn = 0;
      for (iModule = 0; iModule < module->iNumModules[iBody]; iModule++) {
        module->fnFinalizeUpdate40KNumMan[iBody][iModule](body, update, &iEqn,
                                                          iVar, iBody, iFoo);
      }

      (*fnUpdate)[iBody][iVar]        = malloc(iEqn * sizeof(fnUpdateVariable));
      update[iBody].daDerivProc[iVar] = malloc(iEqn * sizeof(double));
      iVar++;
    }

    /* 40K Crust */
    update[iBody].i40KCrust = -1;
    if (update[iBody].iNum40KCrust) {
      update[iBody].i40KCrust      = iVar;
      update[iBody].iaVar[iVar]    = VNUM40KCORE;
      update[iBody].iNumEqns[iVar] = update[iBody].iNum40KCrust;
      update[iBody].pdVar[iVar]    = &body[iBody].d40KNumCrust;
      update[iBody].iNumBodies[iVar] =
            malloc(update[iBody].iNum40KCrust * sizeof(int));
      update[iBody].iaBody[iVar] =
            malloc(update[iBody].iNum40KCrust * sizeof(int *));
      update[iBody].iaType[iVar] =
            malloc(update[iBody].iNum40KCrust * sizeof(int));
      update[iBody].iaModule[iVar] =
            malloc(update[iBody].iNum40KCrust * sizeof(int));

      if (control->Evolve.iOneStep == RUNGEKUTTA) {
        control->Evolve.tmpUpdate[iBody].pdVar[iVar] =
              &control->Evolve.tmpBody[iBody].d40KNumCrust;
        control->Evolve.tmpUpdate[iBody].iNumBodies[iVar] =
              malloc(update[iBody].iNum40KCrust * sizeof(int));
        control->Evolve.tmpUpdate[iBody].daDerivProc[iVar] =
              malloc(update[iBody].iNum40KCrust * sizeof(double));
        control->Evolve.tmpUpdate[iBody].iaType[iVar] =
              malloc(update[iBody].iNum40KCrust * sizeof(int));
        control->Evolve.tmpUpdate[iBody].iaModule[iVar] =
              malloc(update[iBody].iNum40KCrust * sizeof(int));
        control->Evolve.tmpUpdate[iBody].iaBody[iVar] =
              malloc(update[iBody].iNum40KCrust * sizeof(int *));
        for (iSubStep=0; iSubStep < 4; iSubStep++) {
            control->Evolve.daDerivProc[iSubStep][iBody][iVar] =
                  malloc(update[iBody].iNumEqns[iVar] * sizeof(double));
        }
      }

      iEqn = 0;
      for (iModule = 0; iModule < module->iNumModules[iBody]; iModule++) {
        module->fnFinalizeUpdate40KNumCrust[iBody][iModule](body, update, &iEqn,
                                                            iVar, iBody, iFoo);
      }

      (*fnUpdate)[iBody][iVar]        = malloc(iEqn * sizeof(fnUpdateVariable));
      update[iBody].daDerivProc[iVar] = malloc(iEqn * sizeof(double));
      iVar++;
    }

    /* 232Th Core */
    update[iBody].i232ThCore = -1;
    if (update[iBody].iNum232ThCore) {
      update[iBody].i232ThCore     = iVar;
      update[iBody].iaVar[iVar]    = VNUM232THCORE;
      update[iBody].iNumEqns[iVar] = update[iBody].iNum232ThCore;
      update[iBody].pdVar[iVar]    = &body[iBody].d232ThNumCore;
      update[iBody].iNumBodies[iVar] =
            malloc(update[iBody].iNum232ThCore * sizeof(int));
      update[iBody].iaBody[iVar] =
            malloc(update[iBody].iNum232ThCore * sizeof(int *));
      update[iBody].iaType[iVar] =
            malloc(update[iBody].iNum232ThCore * sizeof(int));
      update[iBody].iaModule[iVar] =
            malloc(update[iBody].iNum232ThCore * sizeof(int));

      if (control->Evolve.iOneStep == RUNGEKUTTA) {
        control->Evolve.tmpUpdate[iBody].pdVar[iVar] =
              &control->Evolve.tmpBody[iBody].d232ThNumCore;
        control->Evolve.tmpUpdate[iBody].iNumBodies[iVar] =
              malloc(update[iBody].iNum232ThCore * sizeof(int));
        control->Evolve.tmpUpdate[iBody].daDerivProc[iVar] =
              malloc(update[iBody].iNum232ThCore * sizeof(double));
        control->Evolve.tmpUpdate[iBody].iaType[iVar] =
              malloc(update[iBody].iNum232ThCore * sizeof(int));
        control->Evolve.tmpUpdate[iBody].iaModule[iVar] =
              malloc(update[iBody].iNum232ThCore * sizeof(int));
        control->Evolve.tmpUpdate[iBody].iaBody[iVar] =
              malloc(update[iBody].iNum232ThCore * sizeof(int *));
        for (iSubStep=0; iSubStep < 4; iSubStep++) {
            control->Evolve.daDerivProc[iSubStep][iBody][iVar] =
                  malloc(update[iBody].iNumEqns[iVar] * sizeof(double));
        }
      }

      iEqn = 0;
      for (iModule = 0; iModule < module->iNumModules[iBody]; iModule++) {
        module->fnFinalizeUpdate232ThNumCore[iBody][iModule](
              body, update, &iEqn, iVar, iBody, iFoo);
      }

      (*fnUpdate)[iBody][iVar]        = malloc(iEqn * sizeof(fnUpdateVariable));
      update[iBody].daDerivProc[iVar] = malloc(iEqn * sizeof(double));
      iVar++;
    }

    /* 232Th Mantle */
    update[iBody].i232ThMan = -1;
    if (update[iBody].iNum232ThMan) {
      update[iBody].i232ThMan      = iVar;
      update[iBody].iaVar[iVar]    = VNUM232THMAN;
      update[iBody].iNumEqns[iVar] = update[iBody].iNum232ThMan;
      update[iBody].pdVar[iVar]    = &body[iBody].d232ThNumMan;
      update[iBody].iNumBodies[iVar] =
            malloc(update[iBody].iNum232ThMan * sizeof(int));
      update[iBody].iaBody[iVar] =
            malloc(update[iBody].iNum232ThMan * sizeof(int *));
      update[iBody].iaType[iVar] =
            malloc(update[iBody].iNum232ThMan * sizeof(int));
      update[iBody].iaModule[iVar] =
            malloc(update[iBody].iNum232ThMan * sizeof(int));

      if (control->Evolve.iOneStep == RUNGEKUTTA) {
        control->Evolve.tmpUpdate[iBody].pdVar[iVar] =
              &control->Evolve.tmpBody[iBody].d232ThNumMan;
        control->Evolve.tmpUpdate[iBody].iNumBodies[iVar] =
              malloc(update[iBody].iNum232ThMan * sizeof(int));
        control->Evolve.tmpUpdate[iBody].daDerivProc[iVar] =
              malloc(update[iBody].iNum232ThMan * sizeof(double));
        control->Evolve.tmpUpdate[iBody].iaType[iVar] =
              malloc(update[iBody].iNum232ThMan * sizeof(int));
        control->Evolve.tmpUpdate[iBody].iaModule[iVar] =
              malloc(update[iBody].iNum232ThMan * sizeof(int));
        control->Evolve.tmpUpdate[iBody].iaBody[iVar] =
              malloc(update[iBody].iNum232ThMan * sizeof(int *));
        for (iSubStep=0; iSubStep < 4; iSubStep++) {
            control->Evolve.daDerivProc[iSubStep][iBody][iVar] =
                  malloc(update[iBody].iNumEqns[iVar] * sizeof(double));
        }
      }

      iEqn = 0;
      for (iModule = 0; iModule < module->iNumModules[iBody]; iModule++) {
        module->fnFinalizeUpdate232ThNumMan[iBody][iModule](body, update, &iEqn,
                                                            iVar, iBody, iFoo);
      }

      (*fnUpdate)[iBody][iVar]        = malloc(iEqn * sizeof(fnUpdateVariable));
      update[iBody].daDerivProc[iVar] = malloc(iEqn * sizeof(double));
      iVar++;
    }

    /* 232Th Crust */
    update[iBody].i232ThCrust = -1;
    if (update[iBody].iNum232ThCrust) {
      update[iBody].i232ThCrust    = iVar;
      update[iBody].iaVar[iVar]    = VNUM232THCORE;
      update[iBody].iNumEqns[iVar] = update[iBody].iNum232ThCrust;
      update[iBody].pdVar[iVar]    = &body[iBody].d232ThNumCrust;
      update[iBody].iNumBodies[iVar] =
            malloc(update[iBody].iNum232ThCrust * sizeof(int));
      update[iBody].iaBody[iVar] =
            malloc(update[iBody].iNum232ThCrust * sizeof(int *));
      update[iBody].iaType[iVar] =
            malloc(update[iBody].iNum232ThCrust * sizeof(int));
      update[iBody].iaModule[iVar] =
            malloc(update[iBody].iNum232ThCrust * sizeof(int));

      if (control->Evolve.iOneStep == RUNGEKUTTA) {
        control->Evolve.tmpUpdate[iBody].pdVar[iVar] =
              &control->Evolve.tmpBody[iBody].d232ThNumCrust;
        control->Evolve.tmpUpdate[iBody].iNumBodies[iVar] =
              malloc(update[iBody].iNum232ThCrust * sizeof(int));
        control->Evolve.tmpUpdate[iBody].daDerivProc[iVar] =
              malloc(update[iBody].iNum232ThCrust * sizeof(double));
        control->Evolve.tmpUpdate[iBody].iaType[iVar] =
              malloc(update[iBody].iNum232ThCrust * sizeof(int));
        control->Evolve.tmpUpdate[iBody].iaModule[iVar] =
              malloc(update[iBody].iNum232ThCrust * sizeof(int));
        control->Evolve.tmpUpdate[iBody].iaBody[iVar] =
              malloc(update[iBody].iNum232ThCrust * sizeof(int *));
        for (iSubStep=0; iSubStep < 4; iSubStep++) {
            control->Evolve.daDerivProc[iSubStep][iBody][iVar] =
                  malloc(update[iBody].iNumEqns[iVar] * sizeof(double));
        }
      }

      iEqn = 0;
      for (iModule = 0; iModule < module->iNumModules[iBody]; iModule++) {
        module->fnFinalizeUpdate232ThNumCrust[iBody][iModule](
              body, update, &iEqn, iVar, iBody, iFoo);
      }

      (*fnUpdate)[iBody][iVar]        = malloc(iEqn * sizeof(fnUpdateVariable));
      update[iBody].daDerivProc[iVar] = malloc(iEqn * sizeof(double));
      iVar++;
    }

    /* 235U Core */
    update[iBody].i235UCore = -1;
    if (update[iBody].iNum235UCore) {
      update[iBody].i235UCore      = iVar;
      update[iBody].iaVar[iVar]    = VNUM235UCORE;
      update[iBody].iNumEqns[iVar] = update[iBody].iNum235UCore;
      update[iBody].pdVar[iVar]    = &body[iBody].d235UNumCore;
      update[iBody].iNumBodies[iVar] =
            malloc(update[iBody].iNum235UCore * sizeof(int));
      update[iBody].iaBody[iVar] =
            malloc(update[iBody].iNum235UCore * sizeof(int *));
      update[iBody].iaType[iVar] =
            malloc(update[iBody].iNum235UCore * sizeof(int));
      update[iBody].iaModule[iVar] =
            malloc(update[iBody].iNum235UCore * sizeof(int));

      if (control->Evolve.iOneStep == RUNGEKUTTA) {
        control->Evolve.tmpUpdate[iBody].pdVar[iVar] =
              &control->Evolve.tmpBody[iBody].d235UNumCore;
        control->Evolve.tmpUpdate[iBody].iNumBodies[iVar] =
              malloc(update[iBody].iNum235UCore * sizeof(int));
        control->Evolve.tmpUpdate[iBody].daDerivProc[iVar] =
              malloc(update[iBody].iNum235UCore * sizeof(double));
        control->Evolve.tmpUpdate[iBody].iaType[iVar] =
              malloc(update[iBody].iNum235UCore * sizeof(int));
        control->Evolve.tmpUpdate[iBody].iaModule[iVar] =
              malloc(update[iBody].iNum235UCore * sizeof(int));
        control->Evolve.tmpUpdate[iBody].iaBody[iVar] =
              malloc(update[iBody].iNum235UCore * sizeof(int *));
        for (iSubStep=0; iSubStep < 4; iSubStep++) {
            control->Evolve.daDerivProc[iSubStep][iBody][iVar] =
                  malloc(update[iBody].iNumEqns[iVar] * sizeof(double));
        }
      }

      iEqn = 0;
      for (iModule = 0; iModule < module->iNumModules[iBody]; iModule++) {
        module->fnFinalizeUpdate235UNumCore[iBody][iModule](body, update, &iEqn,
                                                            iVar, iBody, iFoo);
      }

      (*fnUpdate)[iBody][iVar]        = malloc(iEqn * sizeof(fnUpdateVariable));
      update[iBody].daDerivProc[iVar] = malloc(iEqn * sizeof(double));
      iVar++;
    }

    /* 235U Mantle */
    update[iBody].i235UMan = -1;
    if (update[iBody].iNum235UMan) {
      update[iBody].i235UMan       = iVar;
      update[iBody].iaVar[iVar]    = VNUM235UMAN;
      update[iBody].iNumEqns[iVar] = update[iBody].iNum235UMan;
      update[iBody].pdVar[iVar]    = &body[iBody].d235UNumMan;
      update[iBody].iNumBodies[iVar] =
            malloc(update[iBody].iNum235UMan * sizeof(int));
      update[iBody].iaBody[iVar] =
            malloc(update[iBody].iNum235UMan * sizeof(int *));
      update[iBody].iaType[iVar] =
            malloc(update[iBody].iNum235UMan * sizeof(int));
      update[iBody].iaModule[iVar] =
            malloc(update[iBody].iNum235UMan * sizeof(int));

      if (control->Evolve.iOneStep == RUNGEKUTTA) {
        control->Evolve.tmpUpdate[iBody].pdVar[iVar] =
              &control->Evolve.tmpBody[iBody].d235UNumMan;
        control->Evolve.tmpUpdate[iBody].iNumBodies[iVar] =
              malloc(update[iBody].iNum235UMan * sizeof(int));
        control->Evolve.tmpUpdate[iBody].daDerivProc[iVar] =
              malloc(update[iBody].iNum235UMan * sizeof(double));
        control->Evolve.tmpUpdate[iBody].iaType[iVar] =
              malloc(update[iBody].iNum235UMan * sizeof(int));
        control->Evolve.tmpUpdate[iBody].iaModule[iVar] =
              malloc(update[iBody].iNum235UMan * sizeof(int));
        control->Evolve.tmpUpdate[iBody].iaBody[iVar] =
              malloc(update[iBody].iNum235UMan * sizeof(int *));
        for (iSubStep=0; iSubStep < 4; iSubStep++) {
            control->Evolve.daDerivProc[iSubStep][iBody][iVar] =
                  malloc(update[iBody].iNumEqns[iVar] * sizeof(double));
        }
      }

      iEqn = 0;
      for (iModule = 0; iModule < module->iNumModules[iBody]; iModule++) {
        module->fnFinalizeUpdate235UNumMan[iBody][iModule](body, update, &iEqn,
                                                           iVar, iBody, iFoo);
      }

      (*fnUpdate)[iBody][iVar]        = malloc(iEqn * sizeof(fnUpdateVariable));
      update[iBody].daDerivProc[iVar] = malloc(iEqn * sizeof(double));
      iVar++;
    }

    /* 235U Crust */
    update[iBody].i235UCrust = -1;
    if (update[iBody].iNum235UCrust) {
      update[iBody].i235UCrust     = iVar;
      update[iBody].iaVar[iVar]    = VNUM235UCORE;
      update[iBody].iNumEqns[iVar] = update[iBody].iNum235UCrust;
      update[iBody].pdVar[iVar]    = &body[iBody].d235UNumCrust;
      update[iBody].iNumBodies[iVar] =
            malloc(update[iBody].iNum235UCrust * sizeof(int));
      update[iBody].iaBody[iVar] =
            malloc(update[iBody].iNum235UCrust * sizeof(int *));
      update[iBody].iaType[iVar] =
            malloc(update[iBody].iNum235UCrust * sizeof(int));
      update[iBody].iaModule[iVar] =
            malloc(update[iBody].iNum235UCrust * sizeof(int));

      if (control->Evolve.iOneStep == RUNGEKUTTA) {
        control->Evolve.tmpUpdate[iBody].pdVar[iVar] =
              &control->Evolve.tmpBody[iBody].d235UNumCrust;
        control->Evolve.tmpUpdate[iBody].iNumBodies[iVar] =
              malloc(update[iBody].iNum235UCrust * sizeof(int));
        control->Evolve.tmpUpdate[iBody].daDerivProc[iVar] =
              malloc(update[iBody].iNum235UCrust * sizeof(double));
        control->Evolve.tmpUpdate[iBody].iaType[iVar] =
              malloc(update[iBody].iNum235UCrust * sizeof(int));
        control->Evolve.tmpUpdate[iBody].iaModule[iVar] =
              malloc(update[iBody].iNum235UCrust * sizeof(int));
        control->Evolve.tmpUpdate[iBody].iaBody[iVar] =
              malloc(update[iBody].iNum235UCrust * sizeof(int *));
        for (iSubStep=0; iSubStep < 4; iSubStep++) {
            control->Evolve.daDerivProc[iSubStep][iBody][iVar] =
                  malloc(update[iBody].iNumEqns[iVar] * sizeof(double));
        }
      }

      iEqn = 0;
      for (iModule = 0; iModule < module->iNumModules[iBody]; iModule++) {
        module->fnFinalizeUpdate235UNumCrust[iBody][iModule](
              body, update, &iEqn, iVar, iBody, iFoo);
      }

      (*fnUpdate)[iBody][iVar]        = malloc(iEqn * sizeof(fnUpdateVariable));
      update[iBody].daDerivProc[iVar] = malloc(iEqn * sizeof(double));
      iVar++;
    }

    /* 238U Core */
    update[iBody].i238UCore = -1;
    if (update[iBody].iNum238UCore) {
      update[iBody].i238UCore      = iVar;
      update[iBody].iaVar[iVar]    = VNUM238UCORE;
      update[iBody].iNumEqns[iVar] = update[iBody].iNum238UCore;
      update[iBody].pdVar[iVar]    = &body[iBody].d238UNumCore;
      update[iBody].iNumBodies[iVar] =
            malloc(update[iBody].iNum238UCore * sizeof(int));
      update[iBody].iaBody[iVar] =
            malloc(update[iBody].iNum238UCore * sizeof(int *));
      update[iBody].iaType[iVar] =
            malloc(update[iBody].iNum238UCore * sizeof(int));
      update[iBody].iaModule[iVar] =
            malloc(update[iBody].iNum238UCore * sizeof(int));

      if (control->Evolve.iOneStep == RUNGEKUTTA) {
        control->Evolve.tmpUpdate[iBody].pdVar[iVar] =
              &control->Evolve.tmpBody[iBody].d238UNumCore;
        control->Evolve.tmpUpdate[iBody].iNumBodies[iVar] =
              malloc(update[iBody].iNum238UCore * sizeof(int));
        control->Evolve.tmpUpdate[iBody].daDerivProc[iVar] =
              malloc(update[iBody].iNum238UCore * sizeof(double));
        control->Evolve.tmpUpdate[iBody].iaType[iVar] =
              malloc(update[iBody].iNum238UCore * sizeof(int));
        control->Evolve.tmpUpdate[iBody].iaModule[iVar] =
              malloc(update[iBody].iNum238UCore * sizeof(int));
        control->Evolve.tmpUpdate[iBody].iaBody[iVar] =
              malloc(update[iBody].iNum238UCore * sizeof(int *));
        for (iSubStep=0; iSubStep < 4; iSubStep++) {
            control->Evolve.daDerivProc[iSubStep][iBody][iVar] =
                  malloc(update[iBody].iNumEqns[iVar] * sizeof(double));
        }
      }

      iEqn = 0;
      for (iModule = 0; iModule < module->iNumModules[iBody]; iModule++) {
        module->fnFinalizeUpdate238UNumCore[iBody][iModule](body, update, &iEqn,
                                                            iVar, iBody, iFoo);
      }

      (*fnUpdate)[iBody][iVar]        = malloc(iEqn * sizeof(fnUpdateVariable));
      update[iBody].daDerivProc[iVar] = malloc(iEqn * sizeof(double));
      iVar++;
    }

    /* 238U Mantle */
    update[iBody].i238UMan = -1;
    if (update[iBody].iNum238UMan) {
      update[iBody].i238UMan       = iVar;
      update[iBody].iaVar[iVar]    = VNUM238UMAN;
      update[iBody].iNumEqns[iVar] = update[iBody].iNum238UMan;
      update[iBody].pdVar[iVar]    = &body[iBody].d238UNumMan;
      update[iBody].iNumBodies[iVar] =
            malloc(update[iBody].iNum238UMan * sizeof(int));
      update[iBody].iaBody[iVar] =
            malloc(update[iBody].iNum238UMan * sizeof(int *));
      update[iBody].iaType[iVar] =
            malloc(update[iBody].iNum238UMan * sizeof(int));
      update[iBody].iaModule[iVar] =
            malloc(update[iBody].iNum238UMan * sizeof(int));

      if (control->Evolve.iOneStep == RUNGEKUTTA) {
        control->Evolve.tmpUpdate[iBody].pdVar[iVar] =
              &control->Evolve.tmpBody[iBody].d238UNumMan;
        control->Evolve.tmpUpdate[iBody].iNumBodies[iVar] =
              malloc(update[iBody].iNum238UMan * sizeof(int));
        control->Evolve.tmpUpdate[iBody].daDerivProc[iVar] =
              malloc(update[iBody].iNum238UMan * sizeof(double));
        control->Evolve.tmpUpdate[iBody].iaType[iVar] =
              malloc(update[iBody].iNum238UMan * sizeof(int));
        control->Evolve.tmpUpdate[iBody].iaModule[iVar] =
              malloc(update[iBody].iNum238UMan * sizeof(int));
        control->Evolve.tmpUpdate[iBody].iaBody[iVar] =
              malloc(update[iBody].iNum238UMan * sizeof(int *));
        for (iSubStep=0; iSubStep < 4; iSubStep++) {
            control->Evolve.daDerivProc[iSubStep][iBody][iVar] =
                  malloc(update[iBody].iNumEqns[iVar] * sizeof(double));
        }
      }

      iEqn = 0;
      for (iModule = 0; iModule < module->iNumModules[iBody]; iModule++) {
        module->fnFinalizeUpdate238UNumMan[iBody][iModule](body, update, &iEqn,
                                                           iVar, iBody, iFoo);
      }

      (*fnUpdate)[iBody][iVar]        = malloc(iEqn * sizeof(fnUpdateVariable));
      update[iBody].daDerivProc[iVar] = malloc(iEqn * sizeof(double));
      iVar++;
    }

    /* 238U Crust */
    update[iBody].i238UCrust = -1;
    if (update[iBody].iNum238UCrust) {
      update[iBody].i238UCrust     = iVar;
      update[iBody].iaVar[iVar]    = VNUM238UCORE;
      update[iBody].iNumEqns[iVar] = update[iBody].iNum238UCrust;
      update[iBody].pdVar[iVar]    = &body[iBody].d238UNumCrust;
      update[iBody].iNumBodies[iVar] =
            malloc(update[iBody].iNum238UCrust * sizeof(int));
      update[iBody].iaBody[iVar] =
            malloc(update[iBody].iNum238UCrust * sizeof(int *));
      update[iBody].iaType[iVar] =
            malloc(update[iBody].iNum238UCrust * sizeof(int));
      update[iBody].iaModule[iVar] =
            malloc(update[iBody].iNum238UCrust * sizeof(int));

      if (control->Evolve.iOneStep == RUNGEKUTTA) {
        control->Evolve.tmpUpdate[iBody].pdVar[iVar] =
              &control->Evolve.tmpBody[iBody].d238UNumCrust;
        control->Evolve.tmpUpdate[iBody].iNumBodies[iVar] =
              malloc(update[iBody].iNum238UCrust * sizeof(int));
        control->Evolve.tmpUpdate[iBody].daDerivProc[iVar] =
              malloc(update[iBody].iNum238UCrust * sizeof(double));
        control->Evolve.tmpUpdate[iBody].iaType[iVar] =
              malloc(update[iBody].iNum238UCrust * sizeof(int));
        control->Evolve.tmpUpdate[iBody].iaModule[iVar] =
              malloc(update[iBody].iNum238UCrust * sizeof(int));
        control->Evolve.tmpUpdate[iBody].iaBody[iVar] =
              malloc(update[iBody].iNum238UCrust * sizeof(int *));
        for (iSubStep=0; iSubStep < 4; iSubStep++) {
            control->Evolve.daDerivProc[iSubStep][iBody][iVar] =
                  malloc(update[iBody].iNumEqns[iVar] * sizeof(double));
        }
      }

      iEqn = 0;
      for (iModule = 0; iModule < module->iNumModules[iBody]; iModule++) {
        module->fnFinalizeUpdate238UNumCrust[iBody][iModule](
              body, update, &iEqn, iVar, iBody, iFoo);
      }

      (*fnUpdate)[iBody][iVar]        = malloc(iEqn * sizeof(fnUpdateVariable));
      update[iBody].daDerivProc[iVar] = malloc(iEqn * sizeof(double));
      iVar++;
    }

    // Atmospheric Envelope Mass
    update[iBody].iEnvelopeMass = -1;
    if (update[iBody].iNumEnvelopeMass) {
      update[iBody].iEnvelopeMass  = iVar;
      update[iBody].iaVar[iVar]    = VENVELOPEMASS;
      update[iBody].iNumEqns[iVar] = update[iBody].iNumEnvelopeMass;
      update[iBody].pdVar[iVar]    = &body[iBody].dEnvelopeMass;
      update[iBody].iNumBodies[iVar] =
            malloc(update[iBody].iNumEnvelopeMass * sizeof(int));
      update[iBody].iaBody[iVar] =
            malloc(update[iBody].iNumEnvelopeMass * sizeof(int *));
      update[iBody].iaType[iVar] =
            malloc(update[iBody].iNumEnvelopeMass * sizeof(int));
      update[iBody].iaModule[iVar] =
            malloc(update[iBody].iNumEnvelopeMass * sizeof(int));

      if (control->Evolve.iOneStep == RUNGEKUTTA) {
        control->Evolve.tmpUpdate[iBody].pdVar[iVar] =
              &control->Evolve.tmpBody[iBody].dEnvelopeMass;
        control->Evolve.tmpUpdate[iBody].iNumBodies[iVar] =
              malloc(update[iBody].iNumEnvelopeMass * sizeof(int));
        control->Evolve.tmpUpdate[iBody].daDerivProc[iVar] =
              malloc(update[iBody].iNumEnvelopeMass * sizeof(double));
        control->Evolve.tmpUpdate[iBody].iaType[iVar] =
              malloc(update[iBody].iNumEnvelopeMass * sizeof(int));
        control->Evolve.tmpUpdate[iBody].iaModule[iVar] =
              malloc(update[iBody].iNumEnvelopeMass * sizeof(int));
        control->Evolve.tmpUpdate[iBody].iaBody[iVar] =
              malloc(update[iBody].iNumEnvelopeMass * sizeof(int *));
        for (iSubStep=0; iSubStep < 4; iSubStep++) {
            control->Evolve.daDerivProc[iSubStep][iBody][iVar] =
                  malloc(update[iBody].iNumEqns[iVar] * sizeof(double));
        }
      }

      iEqn = 0;
      for (iModule = 0; iModule < module->iNumModules[iBody]; iModule++) {
        module->fnFinalizeUpdateEnvelopeMass[iBody][iModule](
              body, update, &iEqn, iVar, iBody, iFoo);
      }

      (*fnUpdate)[iBody][iVar]        = malloc(iEqn * sizeof(fnUpdateVariable));
      update[iBody].daDerivProc[iVar] = malloc(iEqn * sizeof(double));
      iVar++;
    }

    /* Dynamical ellipticity. There are 3 possibilities for dynamical
       ellipticity: 1) It is a constant; 2) It is calculated from dViscUMan,
       which is a constant and set by the option dViscUman (part of DISTROT);
       3) It is calculated from dViscUMan, which is set by the mantle
       temperature, i.e. THERMINT must be called. Only option #3 requires
       DynEllip to be a primary variable. */

    update[iBody].iDynEllip = -1;
    if (update[iBody].iNumDynEllip) {
      update[iBody].iDynEllip      = iVar;
      update[iBody].iaVar[iVar]    = VDYNELLIP;
      update[iBody].iNumEqns[iVar] = update[iBody].iNumDynEllip;
      update[iBody].pdVar[iVar]    = &body[iBody].dDynEllip;
      update[iBody].iNumBodies[iVar] =
            malloc(update[iBody].iNumDynEllip * sizeof(int));
      update[iBody].iaBody[iVar] =
            malloc(update[iBody].iNumDynEllip * sizeof(int *));
      update[iBody].iaType[iVar] =
            malloc(update[iBody].iNumDynEllip * sizeof(int));
      update[iBody].iaModule[iVar] =
            malloc(update[iBody].iNumDynEllip * sizeof(int));

      if (control->Evolve.iOneStep == RUNGEKUTTA) {
        control->Evolve.tmpUpdate[iBody].pdVar[iVar] =
              &control->Evolve.tmpBody[iBody].dDynEllip;
        control->Evolve.tmpUpdate[iBody].iNumBodies[iVar] =
              malloc(update[iBody].iNumDynEllip * sizeof(int));
        control->Evolve.tmpUpdate[iBody].daDerivProc[iVar] =
              malloc(update[iBody].iNumDynEllip * sizeof(double));
        control->Evolve.tmpUpdate[iBody].iaType[iVar] =
              malloc(update[iBody].iNumDynEllip * sizeof(int));
        control->Evolve.tmpUpdate[iBody].iaModule[iVar] =
              malloc(update[iBody].iNumDynEllip * sizeof(int));
        control->Evolve.tmpUpdate[iBody].iaBody[iVar] =
              malloc(update[iBody].iNumDynEllip * sizeof(int *));
        for (iSubStep=0; iSubStep < 4; iSubStep++) {
            control->Evolve.daDerivProc[iSubStep][iBody][iVar] =
                  malloc(update[iBody].iNumEqns[iVar] * sizeof(double));
        }
      }

      iEqn = 0;
      for (iModule = 0; iModule < module->iNumModules[iBody]; iModule++) {
        module->fnFinalizeUpdateDynEllip[iBody][iModule](body, update, &iEqn,
                                                         iVar, iBody, iFoo);
      }

      (*fnUpdate)[iBody][iVar]        = malloc(iEqn * sizeof(fnUpdateVariable));
      update[iBody].daDerivProc[iVar] = malloc(iEqn * sizeof(double));
      iVar++;
    }

    // Poincare's h (Hecc)
    update[iBody].iHecc = -1;
    if (update[iBody].iNumHecc) {
      update[iBody].iHecc          = iVar;
      update[iBody].iaVar[iVar]    = VHECC;
      update[iBody].iNumEqns[iVar] = update[iBody].iNumHecc;
      update[iBody].pdVar[iVar]    = &body[iBody].dHecc;
      update[iBody].iNumBodies[iVar] =
            malloc(update[iBody].iNumHecc * sizeof(int));
      update[iBody].iaBody[iVar] =
            malloc(update[iBody].iNumHecc * sizeof(int *));
      update[iBody].iaType[iVar] = malloc(update[iBody].iNumHecc * sizeof(int));
      update[iBody].iaModule[iVar] =
            malloc(update[iBody].iNumHecc * sizeof(int));

      if (control->Evolve.iOneStep == RUNGEKUTTA) {
        control->Evolve.tmpUpdate[iBody].pdVar[iVar] =
              &control->Evolve.tmpBody[iBody].dHecc;
        control->Evolve.tmpUpdate[iBody].iNumBodies[iVar] =
              malloc(update[iBody].iNumHecc * sizeof(int));
        control->Evolve.tmpUpdate[iBody].daDerivProc[iVar] =
              malloc(update[iBody].iNumHecc * sizeof(double));
        control->Evolve.tmpUpdate[iBody].iaType[iVar] =
              malloc(update[iBody].iNumHecc * sizeof(int));
        control->Evolve.tmpUpdate[iBody].iaModule[iVar] =
              malloc(update[iBody].iNumHecc * sizeof(int));
        control->Evolve.tmpUpdate[iBody].iaBody[iVar] =
              malloc(update[iBody].iNumHecc * sizeof(int *));
        for (iSubStep=0; iSubStep < 4; iSubStep++) {
            control->Evolve.daDerivProc[iSubStep][iBody][iVar] =
                  malloc(update[iBody].iNumEqns[iVar] * sizeof(double));
        }
      }

      iEqn = 0;
      for (iModule = 0; iModule < module->iNumModules[iBody]; iModule++) {
        module->fnFinalizeUpdateHecc[iBody][iModule](body, update, &iEqn, iVar,
                                                     iBody, iFoo);
      }

      (*fnUpdate)[iBody][iVar]        = malloc(iEqn * sizeof(fnUpdateVariable));
      update[iBody].daDerivProc[iVar] = malloc(iEqn * sizeof(double));
      iVar++;
    }

    // Poincare's k (Kecc)
    update[iBody].iKecc = -1;
    if (update[iBody].iNumKecc) {
      update[iBody].iKecc          = iVar;
      update[iBody].iaVar[iVar]    = VKECC;
      update[iBody].iNumEqns[iVar] = update[iBody].iNumKecc;
      update[iBody].pdVar[iVar]    = &body[iBody].dKecc;
      update[iBody].iNumBodies[iVar] =
            malloc(update[iBody].iNumKecc * sizeof(int));
      update[iBody].iaBody[iVar] =
            malloc(update[iBody].iNumKecc * sizeof(int *));
      update[iBody].iaType[iVar] = malloc(update[iBody].iNumKecc * sizeof(int));
      update[iBody].iaModule[iVar] =
            malloc(update[iBody].iNumKecc * sizeof(int));

      if (control->Evolve.iOneStep == RUNGEKUTTA) {
        control->Evolve.tmpUpdate[iBody].pdVar[iVar] =
              &control->Evolve.tmpBody[iBody].dKecc;
        control->Evolve.tmpUpdate[iBody].iNumBodies[iVar] =
              malloc(update[iBody].iNumKecc * sizeof(int));
        control->Evolve.tmpUpdate[iBody].daDerivProc[iVar] =
              malloc(update[iBody].iNumKecc * sizeof(double));
        control->Evolve.tmpUpdate[iBody].iaType[iVar] =
              malloc(update[iBody].iNumKecc * sizeof(int));
        control->Evolve.tmpUpdate[iBody].iaModule[iVar] =
              malloc(update[iBody].iNumKecc * sizeof(int));
        control->Evolve.tmpUpdate[iBody].iaBody[iVar] =
              malloc(update[iBody].iNumKecc * sizeof(int *));
        for (iSubStep=0; iSubStep < 4; iSubStep++) {
            control->Evolve.daDerivProc[iSubStep][iBody][iVar] =
                  malloc(update[iBody].iNumEqns[iVar] * sizeof(double));
        }
      }

      iEqn = 0;
      for (iModule = 0; iModule < module->iNumModules[iBody]; iModule++) {
        module->fnFinalizeUpdateKecc[iBody][iModule](body, update, &iEqn, iVar,
                                                     iBody, iFoo);
      }

      (*fnUpdate)[iBody][iVar]        = malloc(iEqn * sizeof(fnUpdateVariable));
      update[iBody].daDerivProc[iVar] = malloc(iEqn * sizeof(double));
      iVar++;
    }

    // Luminosity:
    /* Someday we'll fix this, here's a template
    AddPrimaryVariable(body,control,module,update,fnUpdate,&body[iBody].dLuminosity,&control->Evolve.tmpBody[iBody].dLuminosity,
                       &update[iBody].iLuminosity,&module->fnFinalizeUpdate[iBody][iModule][iVar | VLUMINOSITY?],
                       update[iBody].iNumLuminosity,iBody,iVar,VLUMINOSITY);
    */
    
    update[iBody].iLuminosity = -1;
    if (update[iBody].iNumLuminosity) {
      update[iBody].iLuminosity    = iVar;
      update[iBody].iaVar[iVar]    = VLUMINOSITY;
      update[iBody].iNumEqns[iVar] = update[iBody].iNumLuminosity;
      update[iBody].pdVar[iVar]    = &body[iBody].dLuminosity;
      update[iBody].iNumBodies[iVar] =
            malloc(update[iBody].iNumLuminosity * sizeof(int));
      update[iBody].iaBody[iVar] =
            malloc(update[iBody].iNumLuminosity * sizeof(int *));
      update[iBody].iaType[iVar] =
            malloc(update[iBody].iNumLuminosity * sizeof(int));
      update[iBody].iaModule[iVar] =
            malloc(update[iBody].iNumLuminosity * sizeof(int));

      if (control->Evolve.iOneStep == RUNGEKUTTA) {
        control->Evolve.tmpUpdate[iBody].pdVar[iVar] =
              &control->Evolve.tmpBody[iBody].dLuminosity;
        control->Evolve.tmpUpdate[iBody].iNumBodies[iVar] =
              malloc(update[iBody].iNumLuminosity * sizeof(int));
        control->Evolve.tmpUpdate[iBody].daDerivProc[iVar] =
              malloc(update[iBody].iNumLuminosity * sizeof(double));
        control->Evolve.tmpUpdate[iBody].iaType[iVar] =
              malloc(update[iBody].iNumLuminosity * sizeof(int));
        control->Evolve.tmpUpdate[iBody].iaModule[iVar] =
              malloc(update[iBody].iNumLuminosity * sizeof(int));
        control->Evolve.tmpUpdate[iBody].iaBody[iVar] =
              malloc(update[iBody].iNumLuminosity * sizeof(int *));
        for (iSubStep=0; iSubStep < 4; iSubStep++) {
            control->Evolve.daDerivProc[iSubStep][iBody][iVar] =
                  malloc(update[iBody].iNumEqns[iVar] * sizeof(double));
        }
      }

      iEqn = 0;
      for (iModule = 0; iModule < module->iNumModules[iBody]; iModule++) {
        module->fnFinalizeUpdateLuminosity[iBody][iModule](body, update, &iEqn,
                                                           iVar, iBody, iFoo);
      }

      (*fnUpdate)[iBody][iVar]        = malloc(iEqn * sizeof(fnUpdateVariable));
      update[iBody].daDerivProc[iVar] = malloc(iEqn * sizeof(double));
      iVar++;
    }

    /* Saved for forced oscillations with POISE
    // Obliquity
    update[iBody].iObl = -1;
    if (update[iBody].iNumObl) {
      update[iBody].iObl = iVar;
      update[iBody].iaVar[iVar] = VOBL;
      update[iBody].iNumEqns[iVar] = update[iBody].iNumObl;
      update[iBody].pdVar[iVar] = &body[iBody].dObliquity;
      update[iBody].iNumBodies[iVar] =
    malloc(update[iBody].iNumObl*sizeof(int)); update[iBody].iaBody[iVar] =
    malloc(update[iBody].iNumObl*sizeof(int*)); update[iBody].iaType[iVar] =
    malloc(update[iBody].iNumObl*sizeof(int)); update[iBody].iaModule[iVar] =
    malloc(update[iBody].iNumObl*sizeof(int));

      if (control->Evolve.iOneStep == RUNGEKUTTA) {
        control->Evolve.tmpUpdate[iBody].pdVar[iVar] =
    &control->Evolve.tmpBody[iBody].dObliquity;
        control->Evolve.tmpUpdate[iBody].iNumBodies[iVar] =
    malloc(update[iBody].iNumObl*sizeof(int));
        control->Evolve.tmpUpdate[iBody].daDerivProc[iVar] =
    malloc(update[iBody].iNumObl*sizeof(double));
        control->Evolve.tmpUpdate[iBody].iaType[iVar] =
    malloc(update[iBody].iNumObl*sizeof(int));
        control->Evolve.tmpUpdate[iBody].iaModule[iVar] =
    malloc(update[iBody].iNumObl*sizeof(int));
        control->Evolve.tmpUpdate[iBody].iaBody[iVar] =
    malloc(update[iBody].iNumObl*sizeof(int*));
      }

      iEqn=0;
      for (iModule=0;iModule<module->iNumModules[iBody];iModule++)
        module->fnFinalizeUpdateObl[iBody][iModule](body,update,&iEqn,iVar,iBody);

      (*fnUpdate)[iBody][iVar]=malloc(iEqn*sizeof(fnUpdateVariable));
      update[iBody].daDerivProc[iVar]=malloc(iEqn*sizeof(double));
      iVar++;
    }
    */

    // Poincare's p (Pinc)
    update[iBody].iPinc = -1;
    if (update[iBody].iNumPinc) {
      update[iBody].iPinc          = iVar;
      update[iBody].iaVar[iVar]    = VPINC;
      update[iBody].iNumEqns[iVar] = update[iBody].iNumPinc;
      update[iBody].pdVar[iVar]    = &body[iBody].dPinc;
      update[iBody].iNumBodies[iVar] =
            malloc(update[iBody].iNumPinc * sizeof(int));
      update[iBody].iaBody[iVar] =
            malloc(update[iBody].iNumPinc * sizeof(int *));
      update[iBody].iaType[iVar] = malloc(update[iBody].iNumPinc * sizeof(int));
      update[iBody].iaModule[iVar] =
            malloc(update[iBody].iNumPinc * sizeof(int));

      if (control->Evolve.iOneStep == RUNGEKUTTA) {
        control->Evolve.tmpUpdate[iBody].pdVar[iVar] =
              &control->Evolve.tmpBody[iBody].dPinc;
        control->Evolve.tmpUpdate[iBody].iNumBodies[iVar] =
              malloc(update[iBody].iNumPinc * sizeof(int));
        control->Evolve.tmpUpdate[iBody].daDerivProc[iVar] =
              malloc(update[iBody].iNumPinc * sizeof(double));
        control->Evolve.tmpUpdate[iBody].iaType[iVar] =
              malloc(update[iBody].iNumPinc * sizeof(int));
        control->Evolve.tmpUpdate[iBody].iaModule[iVar] =
              malloc(update[iBody].iNumPinc * sizeof(int));
        control->Evolve.tmpUpdate[iBody].iaBody[iVar] =
              malloc(update[iBody].iNumPinc * sizeof(int *));
        for (iSubStep=0; iSubStep < 4; iSubStep++) {
            control->Evolve.daDerivProc[iSubStep][iBody][iVar] =
                  malloc(update[iBody].iNumEqns[iVar] * sizeof(double));
        }
      }

      iEqn = 0;
      for (iModule = 0; iModule < module->iNumModules[iBody]; iModule++) {
        module->fnFinalizeUpdatePinc[iBody][iModule](body, update, &iEqn, iVar,
                                                     iBody, iFoo);
      }

      (*fnUpdate)[iBody][iVar]        = malloc(iEqn * sizeof(fnUpdateVariable));
      update[iBody].daDerivProc[iVar] = malloc(iEqn * sizeof(double));
      iVar++;
    }

    // Poincare's q (Qinc)
    update[iBody].iQinc = -1;
    if (update[iBody].iNumQinc) {
      update[iBody].iQinc          = iVar;
      update[iBody].iaVar[iVar]    = VQINC;
      update[iBody].iNumEqns[iVar] = update[iBody].iNumQinc;
      update[iBody].pdVar[iVar]    = &body[iBody].dQinc;
      update[iBody].iNumBodies[iVar] =
            malloc(update[iBody].iNumQinc * sizeof(int));
      update[iBody].iaBody[iVar] =
            malloc(update[iBody].iNumQinc * sizeof(int *));
      update[iBody].iaType[iVar] = malloc(update[iBody].iNumQinc * sizeof(int));
      update[iBody].iaModule[iVar] =
            malloc(update[iBody].iNumQinc * sizeof(int));

      if (control->Evolve.iOneStep == RUNGEKUTTA) {
        control->Evolve.tmpUpdate[iBody].pdVar[iVar] =
              &control->Evolve.tmpBody[iBody].dQinc;
        control->Evolve.tmpUpdate[iBody].iNumBodies[iVar] =
              malloc(update[iBody].iNumQinc * sizeof(int));
        control->Evolve.tmpUpdate[iBody].daDerivProc[iVar] =
              malloc(update[iBody].iNumQinc * sizeof(double));
        control->Evolve.tmpUpdate[iBody].iaType[iVar] =
              malloc(update[iBody].iNumQinc * sizeof(int));
        control->Evolve.tmpUpdate[iBody].iaModule[iVar] =
              malloc(update[iBody].iNumQinc * sizeof(int));
        control->Evolve.tmpUpdate[iBody].iaBody[iVar] =
              malloc(update[iBody].iNumQinc * sizeof(int *));
        for (iSubStep=0; iSubStep < 4; iSubStep++) {
            control->Evolve.daDerivProc[iSubStep][iBody][iVar] =
                  malloc(update[iBody].iNumEqns[iVar] * sizeof(double));
        }
      }

      iEqn = 0;
      for (iModule = 0; iModule < module->iNumModules[iBody]; iModule++) {
        module->fnFinalizeUpdateQinc[iBody][iModule](body, update, &iEqn, iVar,
                                                     iBody, iFoo);
      }

      (*fnUpdate)[iBody][iVar]        = malloc(iEqn * sizeof(fnUpdateVariable));
      update[iBody].daDerivProc[iVar] = malloc(iEqn * sizeof(double));
      iVar++;
    }

    // Radius:
    update[iBody].iRadius = -1;
    if (update[iBody].iNumRadius) {
      update[iBody].iRadius        = iVar;
      update[iBody].iaVar[iVar]    = VRADIUS;
      update[iBody].iNumEqns[iVar] = update[iBody].iNumRadius;
      update[iBody].pdVar[iVar]    = &body[iBody].dRadius;
      update[iBody].iNumBodies[iVar] =
            malloc(update[iBody].iNumRadius * sizeof(int));
      update[iBody].iaBody[iVar] =
            malloc(update[iBody].iNumRadius * sizeof(int *));
      update[iBody].iaType[iVar] =
            malloc(update[iBody].iNumRadius * sizeof(int));
      update[iBody].iaModule[iVar] =
            malloc(update[iBody].iNumRadius * sizeof(int));

      if (control->Evolve.iOneStep == RUNGEKUTTA) {
        control->Evolve.tmpUpdate[iBody].pdVar[iVar] =
              &control->Evolve.tmpBody[iBody].dRadius;
        control->Evolve.tmpUpdate[iBody].iNumBodies[iVar] =
              malloc(update[iBody].iNumRadius * sizeof(int));
        control->Evolve.tmpUpdate[iBody].daDerivProc[iVar] =
              malloc(update[iBody].iNumRadius * sizeof(double));
        control->Evolve.tmpUpdate[iBody].iaType[iVar] =
              malloc(update[iBody].iNumRadius * sizeof(int));
        control->Evolve.tmpUpdate[iBody].iaModule[iVar] =
              malloc(update[iBody].iNumRadius * sizeof(int));
        control->Evolve.tmpUpdate[iBody].iaBody[iVar] =
              malloc(update[iBody].iNumRadius * sizeof(int *));
      for (iSubStep=0; iSubStep < 4; iSubStep++) {
            control->Evolve.daDerivProc[iSubStep][iBody][iVar] =
                  malloc(update[iBody].iNumEqns[iVar] * sizeof(double));
        }
      }

      iEqn = 0;
      for (iModule = 0; iModule < module->iNumModules[iBody]; iModule++) {
        module->fnFinalizeUpdateRadius[iBody][iModule](body, update, &iEqn,
                                                       iVar, iBody, iFoo);
      }

      (*fnUpdate)[iBody][iVar]        = malloc(iEqn * sizeof(fnUpdateVariable));
      update[iBody].daDerivProc[iVar] = malloc(iEqn * sizeof(double));
      iVar++;
    }

    // Mass:
    update[iBody].iMass = -1;
    if (update[iBody].iNumMass) {
      update[iBody].iMass          = iVar;
      update[iBody].iaVar[iVar]    = VMASS;
      update[iBody].iNumEqns[iVar] = update[iBody].iNumMass;
      update[iBody].pdVar[iVar]    = &body[iBody].dMass;
      update[iBody].iNumBodies[iVar] =
            malloc(update[iBody].iNumMass * sizeof(int));
      update[iBody].iaBody[iVar] =
            malloc(update[iBody].iNumMass * sizeof(int *));
      update[iBody].iaType[iVar] = malloc(update[iBody].iNumMass * sizeof(int));
      update[iBody].iaModule[iVar] =
            malloc(update[iBody].iNumMass * sizeof(int));

      if (control->Evolve.iOneStep == RUNGEKUTTA) {
        control->Evolve.tmpUpdate[iBody].pdVar[iVar] =
              &control->Evolve.tmpBody[iBody].dMass;
        control->Evolve.tmpUpdate[iBody].iNumBodies[iVar] =
              malloc(update[iBody].iNumMass * sizeof(int));
        control->Evolve.tmpUpdate[iBody].daDerivProc[iVar] =
              malloc(update[iBody].iNumMass * sizeof(double));
        control->Evolve.tmpUpdate[iBody].iaType[iVar] =
              malloc(update[iBody].iNumMass * sizeof(int));
        control->Evolve.tmpUpdate[iBody].iaModule[iVar] =
              malloc(update[iBody].iNumMass * sizeof(int));
        control->Evolve.tmpUpdate[iBody].iaBody[iVar] =
              malloc(update[iBody].iNumMass * sizeof(int *));
        for (iSubStep=0; iSubStep < 4; iSubStep++) {
            control->Evolve.daDerivProc[iSubStep][iBody][iVar] =
                  malloc(update[iBody].iNumEqns[iVar] * sizeof(double));
        }
      }

      iEqn = 0;
      for (iModule = 0; iModule < module->iNumModules[iBody]; iModule++) {
        module->fnFinalizeUpdateMass[iBody][iModule](body, update, &iEqn, iVar,
                                                     iBody, iFoo);
      }

      (*fnUpdate)[iBody][iVar]        = malloc(iEqn * sizeof(fnUpdateVariable));
      update[iBody].daDerivProc[iVar] = malloc(iEqn * sizeof(double));
      iVar++;
    }

    // Rotation Rate
    update[iBody].iRot = -1;
    if (update[iBody].iNumRot) {
      update[iBody].iRot           = iVar;
      update[iBody].iaVar[iVar]    = VROT;
      update[iBody].iNumEqns[iVar] = update[iBody].iNumRot;
      update[iBody].pdVar[iVar]    = &body[iBody].dRotRate;
      update[iBody].iNumBodies[iVar] =
            malloc(update[iBody].iNumRot * sizeof(int));
      update[iBody].iaBody[iVar] =
            malloc(update[iBody].iNumRot * sizeof(int *));
      update[iBody].iaType[iVar] = malloc(update[iBody].iNumRot * sizeof(int));
      update[iBody].iaModule[iVar] =
            malloc(update[iBody].iNumRot * sizeof(int));

      if (control->Evolve.iOneStep == RUNGEKUTTA) {
        control->Evolve.tmpUpdate[iBody].pdVar[iVar] =
              &control->Evolve.tmpBody[iBody].dRotRate;
        control->Evolve.tmpUpdate[iBody].iNumBodies[iVar] =
              malloc(update[iBody].iNumRot * sizeof(int));
        control->Evolve.tmpUpdate[iBody].daDerivProc[iVar] =
              malloc(update[iBody].iNumRot * sizeof(double));
        control->Evolve.tmpUpdate[iBody].iaType[iVar] =
              malloc(update[iBody].iNumRot * sizeof(int));
        control->Evolve.tmpUpdate[iBody].iaModule[iVar] =
              malloc(update[iBody].iNumRot * sizeof(int));
        control->Evolve.tmpUpdate[iBody].iaBody[iVar] =
              malloc(update[iBody].iNumRot * sizeof(int *));
        for (iSubStep=0; iSubStep < 4; iSubStep++) {
            control->Evolve.daDerivProc[iSubStep][iBody][iVar] =
                  malloc(update[iBody].iNumEqns[iVar] * sizeof(double));
        }
      }

      iEqn = 0;
      for (iModule = 0; iModule < module->iNumModules[iBody]; iModule++) {
        module->fnFinalizeUpdateRot[iBody][iModule](body, update, &iEqn, iVar,
                                                    iBody, iFoo);
      }

      (*fnUpdate)[iBody][iVar]        = malloc(iEqn * sizeof(fnUpdateVariable));
      update[iBody].daDerivProc[iVar] = malloc(iEqn * sizeof(double));
      iVar++;
    }

    // Semi-major Axis
    update[iBody].iSemi = -1;
    if (update[iBody].iNumSemi) {
      update[iBody].iSemi          = iVar;
      update[iBody].iaVar[iVar]    = VSEMI;
      update[iBody].iNumEqns[iVar] = update[iBody].iNumSemi;
      update[iBody].pdVar[iVar]    = &body[iBody].dSemi;
      update[iBody].iNumBodies[iVar] =
            malloc(update[iBody].iNumSemi * sizeof(int));
      update[iBody].iaBody[iVar] =
            malloc(update[iBody].iNumSemi * sizeof(int *));
      update[iBody].iaType[iVar] = malloc(update[iBody].iNumSemi * sizeof(int));
      update[iBody].iaModule[iVar] =
            malloc(update[iBody].iNumSemi * sizeof(int));

      if (control->Evolve.iOneStep == RUNGEKUTTA) {
        control->Evolve.tmpUpdate[iBody].pdVar[iVar] =
              &control->Evolve.tmpBody[iBody].dSemi;
        control->Evolve.tmpUpdate[iBody].iNumBodies[iVar] =
              malloc(update[iBody].iNumSemi * sizeof(int));
        control->Evolve.tmpUpdate[iBody].daDerivProc[iVar] =
              malloc(update[iBody].iNumSemi * sizeof(double));
        control->Evolve.tmpUpdate[iBody].iaType[iVar] =
              malloc(update[iBody].iNumSemi * sizeof(int));
        control->Evolve.tmpUpdate[iBody].iaModule[iVar] =
              malloc(update[iBody].iNumSemi * sizeof(int));
        control->Evolve.tmpUpdate[iBody].iaBody[iVar] =
              malloc(update[iBody].iNumSemi * sizeof(int *));
        for (iSubStep=0; iSubStep < 4; iSubStep++) {
            control->Evolve.daDerivProc[iSubStep][iBody][iVar] =
                  malloc(update[iBody].iNumEqns[iVar] * sizeof(double));
        }
      }

      iEqn = 0;
      for (iModule = 0; iModule < module->iNumModules[iBody]; iModule++) {
        module->fnFinalizeUpdateSemi[iBody][iModule](body, update, &iEqn, iVar,
                                                     iBody, iFoo);
      }

      // Add multi-module dSemi eqns here
      FinalizeUpdateMultiEqStSemi(body, update, &iEqn, iVar, iBody, iFoo,
                                  fnUpdate);

      (*fnUpdate)[iBody][iVar]        = malloc(iEqn * sizeof(fnUpdateVariable));
      update[iBody].daDerivProc[iVar] = malloc(iEqn * sizeof(double));
      iVar++;
    }

    // Surface Water Mass
    update[iBody].iSurfaceWaterMass = -1;
    if (update[iBody].iNumSurfaceWaterMass) {
      update[iBody].iSurfaceWaterMass = iVar;
      update[iBody].iaVar[iVar]       = VSURFACEWATERMASS;
      update[iBody].iNumEqns[iVar]    = update[iBody].iNumSurfaceWaterMass;
      update[iBody].pdVar[iVar]       = &body[iBody].dSurfaceWaterMass;
      update[iBody].iNumBodies[iVar] =
            malloc(update[iBody].iNumSurfaceWaterMass * sizeof(int));
      update[iBody].iaBody[iVar] =
            malloc(update[iBody].iNumSurfaceWaterMass * sizeof(int *));
      update[iBody].iaType[iVar] =
            malloc(update[iBody].iNumSurfaceWaterMass * sizeof(int));
      update[iBody].iaModule[iVar] =
            malloc(update[iBody].iNumSurfaceWaterMass * sizeof(int));

      if (control->Evolve.iOneStep == RUNGEKUTTA) {
        control->Evolve.tmpUpdate[iBody].pdVar[iVar] =
              &control->Evolve.tmpBody[iBody].dSurfaceWaterMass;
        control->Evolve.tmpUpdate[iBody].iNumBodies[iVar] =
              malloc(update[iBody].iNumSurfaceWaterMass * sizeof(int));
        control->Evolve.tmpUpdate[iBody].daDerivProc[iVar] =
              malloc(update[iBody].iNumSurfaceWaterMass * sizeof(double));
        control->Evolve.tmpUpdate[iBody].iaType[iVar] =
              malloc(update[iBody].iNumSurfaceWaterMass * sizeof(int));
        control->Evolve.tmpUpdate[iBody].iaModule[iVar] =
              malloc(update[iBody].iNumSurfaceWaterMass * sizeof(int));
        control->Evolve.tmpUpdate[iBody].iaBody[iVar] =
              malloc(update[iBody].iNumSurfaceWaterMass * sizeof(int *));
        for (iSubStep=0; iSubStep < 4; iSubStep++) {
            control->Evolve.daDerivProc[iSubStep][iBody][iVar] =
                  malloc(update[iBody].iNumEqns[iVar] * sizeof(double));
        }
      }

      iEqn = 0;
      for (iModule = 0; iModule < module->iNumModules[iBody]; iModule++) {
        module->fnFinalizeUpdateSurfaceWaterMass[iBody][iModule](
              body, update, &iEqn, iVar, iBody, iFoo);
      }

      (*fnUpdate)[iBody][iVar]        = malloc(iEqn * sizeof(fnUpdateVariable));
      update[iBody].daDerivProc[iVar] = malloc(iEqn * sizeof(double));
      iVar++;
    }

    // Oxygen Mass
    update[iBody].iOxygenMass = -1;
    if (update[iBody].iNumOxygenMass) {
      update[iBody].iOxygenMass    = iVar;
      update[iBody].iaVar[iVar]    = VOXYGENMASS;
      update[iBody].iNumEqns[iVar] = update[iBody].iNumOxygenMass;
      update[iBody].pdVar[iVar]    = &body[iBody].dOxygenMass;
      update[iBody].iNumBodies[iVar] =
            malloc(update[iBody].iNumOxygenMass * sizeof(int));
      update[iBody].iaBody[iVar] =
            malloc(update[iBody].iNumOxygenMass * sizeof(int *));
      update[iBody].iaType[iVar] =
            malloc(update[iBody].iNumOxygenMass * sizeof(int));
      update[iBody].iaModule[iVar] =
            malloc(update[iBody].iNumOxygenMass * sizeof(int));

      if (control->Evolve.iOneStep == RUNGEKUTTA) {
        control->Evolve.tmpUpdate[iBody].pdVar[iVar] =
              &control->Evolve.tmpBody[iBody].dOxygenMass;
        control->Evolve.tmpUpdate[iBody].iNumBodies[iVar] =
              malloc(update[iBody].iNumOxygenMass * sizeof(int));
        control->Evolve.tmpUpdate[iBody].daDerivProc[iVar] =
              malloc(update[iBody].iNumOxygenMass * sizeof(double));
        control->Evolve.tmpUpdate[iBody].iaType[iVar] =
              malloc(update[iBody].iNumOxygenMass * sizeof(int));
        control->Evolve.tmpUpdate[iBody].iaModule[iVar] =
              malloc(update[iBody].iNumOxygenMass * sizeof(int));
        control->Evolve.tmpUpdate[iBody].iaBody[iVar] =
              malloc(update[iBody].iNumOxygenMass * sizeof(int *));
        for (iSubStep=0; iSubStep < 4; iSubStep++) {
            control->Evolve.daDerivProc[iSubStep][iBody][iVar] =
                  malloc(update[iBody].iNumEqns[iVar] * sizeof(double));
        }              
      }

      iEqn = 0;
      for (iModule = 0; iModule < module->iNumModules[iBody]; iModule++) {
        module->fnFinalizeUpdateOxygenMass[iBody][iModule](body, update, &iEqn,
                                                           iVar, iBody, iFoo);
      }

      (*fnUpdate)[iBody][iVar]        = malloc(iEqn * sizeof(fnUpdateVariable));
      update[iBody].daDerivProc[iVar] = malloc(iEqn * sizeof(double));
      iVar++;
    }

    // Oxygen Mantle Mass
    update[iBody].iOxygenMantleMass = -1;
    if (update[iBody].iNumOxygenMantleMass) {
      update[iBody].iOxygenMantleMass = iVar;
      update[iBody].iaVar[iVar]       = VOXYGENMANTLEMASS;
      update[iBody].iNumEqns[iVar]    = update[iBody].iNumOxygenMantleMass;
      update[iBody].pdVar[iVar]       = &body[iBody].dOxygenMantleMass;
      update[iBody].iNumBodies[iVar] =
            malloc(update[iBody].iNumOxygenMantleMass * sizeof(int));
      update[iBody].iaBody[iVar] =
            malloc(update[iBody].iNumOxygenMantleMass * sizeof(int *));
      update[iBody].iaType[iVar] =
            malloc(update[iBody].iNumOxygenMantleMass * sizeof(int));
      update[iBody].iaModule[iVar] =
            malloc(update[iBody].iNumOxygenMantleMass * sizeof(int));

      if (control->Evolve.iOneStep == RUNGEKUTTA) {
        control->Evolve.tmpUpdate[iBody].pdVar[iVar] =
              &control->Evolve.tmpBody[iBody].dOxygenMantleMass;
        control->Evolve.tmpUpdate[iBody].iNumBodies[iVar] =
              malloc(update[iBody].iNumOxygenMantleMass * sizeof(int));
        control->Evolve.tmpUpdate[iBody].daDerivProc[iVar] =
              malloc(update[iBody].iNumOxygenMantleMass * sizeof(double));
        control->Evolve.tmpUpdate[iBody].iaType[iVar] =
              malloc(update[iBody].iNumOxygenMantleMass * sizeof(int));
        control->Evolve.tmpUpdate[iBody].iaModule[iVar] =
              malloc(update[iBody].iNumOxygenMantleMass * sizeof(int));
        control->Evolve.tmpUpdate[iBody].iaBody[iVar] =
              malloc(update[iBody].iNumOxygenMantleMass * sizeof(int *));
        for (iSubStep=0; iSubStep < 4; iSubStep++) {
            control->Evolve.daDerivProc[iSubStep][iBody][iVar] =
                  malloc(update[iBody].iNumEqns[iVar] * sizeof(double));
        }
      }

      iEqn = 0;
      for (iModule = 0; iModule < module->iNumModules[iBody]; iModule++) {
        module->fnFinalizeUpdateOxygenMantleMass[iBody][iModule](
              body, update, &iEqn, iVar, iBody, iFoo);
      }

      (*fnUpdate)[iBody][iVar]        = malloc(iEqn * sizeof(fnUpdateVariable));
      update[iBody].daDerivProc[iVar] = malloc(iEqn * sizeof(double));
      iVar++;
    }

    // Stellar Temperature:
    update[iBody].iTemperature = -1;
    if (update[iBody].iNumTemperature) {
      update[iBody].iTemperature   = iVar;
      update[iBody].iaVar[iVar]    = VTEMPERATURE;
      update[iBody].iNumEqns[iVar] = update[iBody].iNumTemperature;
      update[iBody].pdVar[iVar]    = &body[iBody].dTemperature;
      update[iBody].iNumBodies[iVar] =
            malloc(update[iBody].iNumTemperature * sizeof(int));
      update[iBody].iaBody[iVar] =
            malloc(update[iBody].iNumTemperature * sizeof(int *));
      update[iBody].iaType[iVar] =
            malloc(update[iBody].iNumTemperature * sizeof(int));
      update[iBody].iaModule[iVar] =
            malloc(update[iBody].iNumTemperature * sizeof(int));

      if (control->Evolve.iOneStep == RUNGEKUTTA) {
        control->Evolve.tmpUpdate[iBody].pdVar[iVar] =
              &control->Evolve.tmpBody[iBody].dTemperature;
        control->Evolve.tmpUpdate[iBody].iNumBodies[iVar] =
              malloc(update[iBody].iNumTemperature * sizeof(int));
        control->Evolve.tmpUpdate[iBody].daDerivProc[iVar] =
              malloc(update[iBody].iNumTemperature * sizeof(double));
        control->Evolve.tmpUpdate[iBody].iaType[iVar] =
              malloc(update[iBody].iNumTemperature * sizeof(int));
        control->Evolve.tmpUpdate[iBody].iaModule[iVar] =
              malloc(update[iBody].iNumTemperature * sizeof(int));
        control->Evolve.tmpUpdate[iBody].iaBody[iVar] =
              malloc(update[iBody].iNumTemperature * sizeof(int *));
        for (iSubStep=0; iSubStep < 4; iSubStep++) {
            control->Evolve.daDerivProc[iSubStep][iBody][iVar] =
                  malloc(update[iBody].iNumEqns[iVar] * sizeof(double));
        }
      }

      iEqn = 0;
      for (iModule = 0; iModule < module->iNumModules[iBody]; iModule++) {
        module->fnFinalizeUpdateTemperature[iBody][iModule](body, update, &iEqn,
                                                            iVar, iBody, iFoo);
      }

      (*fnUpdate)[iBody][iVar]        = malloc(iEqn * sizeof(fnUpdateVariable));
      update[iBody].daDerivProc[iVar] = malloc(iEqn * sizeof(double));
      iVar++;
    }

    // Stellar radius of gyration:
    update[iBody].iRadGyra = -1;
    if (update[iBody].iNumRadGyra) {
      update[iBody].iRadGyra       = iVar;
      update[iBody].iaVar[iVar]    = VRADGYRA;
      update[iBody].iNumEqns[iVar] = update[iBody].iNumRadGyra;
      update[iBody].pdVar[iVar]    = &body[iBody].dRadGyra;
      update[iBody].iNumBodies[iVar] =
            malloc(update[iBody].iNumRadGyra * sizeof(int));
      update[iBody].iaBody[iVar] =
            malloc(update[iBody].iNumRadGyra * sizeof(int *));
      update[iBody].iaType[iVar] =
            malloc(update[iBody].iNumRadGyra * sizeof(int));
      update[iBody].iaModule[iVar] =
            malloc(update[iBody].iNumRadGyra * sizeof(int));

      if (control->Evolve.iOneStep == RUNGEKUTTA) {
        control->Evolve.tmpUpdate[iBody].pdVar[iVar] =
              &control->Evolve.tmpBody[iBody].dRadGyra;
        control->Evolve.tmpUpdate[iBody].iNumBodies[iVar] =
              malloc(update[iBody].iNumRadGyra * sizeof(int));
        control->Evolve.tmpUpdate[iBody].daDerivProc[iVar] =
              malloc(update[iBody].iNumRadGyra * sizeof(double));
        control->Evolve.tmpUpdate[iBody].iaType[iVar] =
              malloc(update[iBody].iNumRadGyra * sizeof(int));
        control->Evolve.tmpUpdate[iBody].iaModule[iVar] =
              malloc(update[iBody].iNumRadGyra * sizeof(int));
        control->Evolve.tmpUpdate[iBody].iaBody[iVar] =
              malloc(update[iBody].iNumRadGyra * sizeof(int *));
        for (iSubStep=0; iSubStep < 4; iSubStep++) {
            control->Evolve.daDerivProc[iSubStep][iBody][iVar] =
                  malloc(update[iBody].iNumEqns[iVar] * sizeof(double));
        }
      }

      iEqn = 0;
      for (iModule = 0; iModule < module->iNumModules[iBody]; iModule++) {
        module->fnFinalizeUpdateRadGyra[iBody][iModule](body, update, &iEqn,
                                                        iVar, iBody, iFoo);
      }

      (*fnUpdate)[iBody][iVar]        = malloc(iEqn * sizeof(fnUpdateVariable));
      update[iBody].daDerivProc[iVar] = malloc(iEqn * sizeof(double));
      iVar++;
    }

    /* Core Temperature (TCore) */
    update[iBody].iTCore = -1;
    if (update[iBody].iNumTCore) {
      update[iBody].iTCore         = iVar;
      update[iBody].iaVar[iVar]    = VTCORE;
      update[iBody].iNumEqns[iVar] = update[iBody].iNumTCore;
      update[iBody].pdVar[iVar]    = &body[iBody].dTCore;
      update[iBody].iNumBodies[iVar] =
            malloc(update[iBody].iNumTCore * sizeof(int));
      update[iBody].iaBody[iVar] =
            malloc(update[iBody].iNumTCore * sizeof(int *));
      update[iBody].iaType[iVar] =
            malloc(update[iBody].iNumTCore * sizeof(int));
      update[iBody].iaModule[iVar] =
            malloc(update[iBody].iNumTCore * sizeof(int));

      if (control->Evolve.iOneStep == RUNGEKUTTA) {
        control->Evolve.tmpUpdate[iBody].pdVar[iVar] =
              &control->Evolve.tmpBody[iBody].dTCore;
        control->Evolve.tmpUpdate[iBody].iNumBodies[iVar] =
              malloc(update[iBody].iNumTCore * sizeof(int));
        control->Evolve.tmpUpdate[iBody].daDerivProc[iVar] =
              malloc(update[iBody].iNumTCore * sizeof(double));
        control->Evolve.tmpUpdate[iBody].iaType[iVar] =
              malloc(update[iBody].iNumTCore * sizeof(int));
        control->Evolve.tmpUpdate[iBody].iaModule[iVar] =
              malloc(update[iBody].iNumTCore * sizeof(int));
        control->Evolve.tmpUpdate[iBody].iaBody[iVar] =
              malloc(update[iBody].iNumTCore * sizeof(int *));
        for (iSubStep=0; iSubStep < 4; iSubStep++) {
            control->Evolve.daDerivProc[iSubStep][iBody][iVar] =
                  malloc(update[iBody].iNumEqns[iVar] * sizeof(double));
        }
      }

      iEqn = 0;
      for (iModule = 0; iModule < module->iNumModules[iBody]; iModule++) {
        module->fnFinalizeUpdateTCore[iBody][iModule](body, update, &iEqn, iVar,
                                                      iBody, iFoo);
      }

      (*fnUpdate)[iBody][iVar]        = malloc(iEqn * sizeof(fnUpdateVariable));
      update[iBody].daDerivProc[iVar] = malloc(iEqn * sizeof(double));
      iVar++;
    }

    /* Mantle Temperature (TMan) */
    update[iBody].iTMan = -1;
    if (update[iBody].iNumTMan) {
      update[iBody].iTMan          = iVar;
      update[iBody].iaVar[iVar]    = VTMAN;
      update[iBody].iNumEqns[iVar] = update[iBody].iNumTMan;
      update[iBody].pdVar[iVar]    = &body[iBody].dTMan;
      update[iBody].iNumBodies[iVar] =
            malloc(update[iBody].iNumTMan * sizeof(int));
      update[iBody].iaBody[iVar] =
            malloc(update[iBody].iNumTMan * sizeof(int *));
      update[iBody].iaType[iVar] = malloc(update[iBody].iNumTMan * sizeof(int));
      update[iBody].iaModule[iVar] =
            malloc(update[iBody].iNumTMan * sizeof(int));

      if (control->Evolve.iOneStep == RUNGEKUTTA) {
        control->Evolve.tmpUpdate[iBody].pdVar[iVar] =
              &control->Evolve.tmpBody[iBody].dTMan;
        control->Evolve.tmpUpdate[iBody].iNumBodies[iVar] =
              malloc(update[iBody].iNumTMan * sizeof(int));
        control->Evolve.tmpUpdate[iBody].daDerivProc[iVar] =
              malloc(update[iBody].iNumTMan * sizeof(double));
        control->Evolve.tmpUpdate[iBody].iaType[iVar] =
              malloc(update[iBody].iNumTMan * sizeof(int));
        control->Evolve.tmpUpdate[iBody].iaModule[iVar] =
              malloc(update[iBody].iNumTMan * sizeof(int));
        control->Evolve.tmpUpdate[iBody].iaBody[iVar] =
              malloc(update[iBody].iNumTMan * sizeof(int *));
        for (iSubStep=0; iSubStep < 4; iSubStep++) {
            control->Evolve.daDerivProc[iSubStep][iBody][iVar] =
                  malloc(update[iBody].iNumEqns[iVar] * sizeof(double));
        }
      }

      iEqn = 0;
      for (iModule = 0; iModule < module->iNumModules[iBody]; iModule++) {
        module->fnFinalizeUpdateTMan[iBody][iModule](body, update, &iEqn, iVar,
                                                     iBody, iFoo);
      }

      (*fnUpdate)[iBody][iVar]        = malloc(iEqn * sizeof(fnUpdateVariable));
      update[iBody].daDerivProc[iVar] = malloc(iEqn * sizeof(double));
      iVar++;
    }

    // DistRot's X (Xobl)
    update[iBody].iXobl = -1;
    if (update[iBody].iNumXobl) {
      update[iBody].iXobl          = iVar;
      update[iBody].iaVar[iVar]    = VXOBL;
      update[iBody].iNumEqns[iVar] = update[iBody].iNumXobl;
      update[iBody].pdVar[iVar]    = &body[iBody].dXobl;
      update[iBody].iNumBodies[iVar] =
            malloc(update[iBody].iNumXobl * sizeof(int));
      update[iBody].iaBody[iVar] =
            malloc(update[iBody].iNumXobl * sizeof(int *));
      update[iBody].iaType[iVar] = malloc(update[iBody].iNumXobl * sizeof(int));
      update[iBody].iaModule[iVar] =
            malloc(update[iBody].iNumXobl * sizeof(int));

      if (control->Evolve.iOneStep == RUNGEKUTTA) {
        control->Evolve.tmpUpdate[iBody].pdVar[iVar] =
              &control->Evolve.tmpBody[iBody].dXobl;
        control->Evolve.tmpUpdate[iBody].iNumBodies[iVar] =
              malloc(update[iBody].iNumXobl * sizeof(int));
        control->Evolve.tmpUpdate[iBody].daDerivProc[iVar] =
              malloc(update[iBody].iNumXobl * sizeof(double));
        control->Evolve.tmpUpdate[iBody].iaType[iVar] =
              malloc(update[iBody].iNumXobl * sizeof(int));
        control->Evolve.tmpUpdate[iBody].iaModule[iVar] =
              malloc(update[iBody].iNumXobl * sizeof(int));
        control->Evolve.tmpUpdate[iBody].iaBody[iVar] =
              malloc(update[iBody].iNumXobl * sizeof(int *));
        control->Evolve.tmpUpdate[iBody].iXobl = iVar;
        for (iSubStep=0; iSubStep < 4; iSubStep++) {
            control->Evolve.daDerivProc[iSubStep][iBody][iVar] =
                  malloc(update[iBody].iNumEqns[iVar] * sizeof(double));
        }
      }

      iEqn = 0;
      for (iModule = 0; iModule < module->iNumModules[iBody]; iModule++) {
        module->fnFinalizeUpdateXobl[iBody][iModule](body, update, &iEqn, iVar,
                                                     iBody, iFoo);
      }

      (*fnUpdate)[iBody][iVar]        = malloc(iEqn * sizeof(fnUpdateVariable));
      update[iBody].daDerivProc[iVar] = malloc(iEqn * sizeof(double));
      iVar++;
    }

    // DistRot's Y (Yobl)
    update[iBody].iYobl = -1;
    if (update[iBody].iNumYobl) {
      update[iBody].iYobl          = iVar;
      update[iBody].iaVar[iVar]    = VYOBL;
      update[iBody].iNumEqns[iVar] = update[iBody].iNumYobl;
      update[iBody].pdVar[iVar]    = &body[iBody].dYobl;
      update[iBody].iNumBodies[iVar] =
            malloc(update[iBody].iNumYobl * sizeof(int));
      update[iBody].iaBody[iVar] =
            malloc(update[iBody].iNumYobl * sizeof(int *));
      update[iBody].iaType[iVar] = malloc(update[iBody].iNumYobl * sizeof(int));
      update[iBody].iaModule[iVar] =
            malloc(update[iBody].iNumYobl * sizeof(int));

      if (control->Evolve.iOneStep == RUNGEKUTTA) {
        control->Evolve.tmpUpdate[iBody].pdVar[iVar] =
              &control->Evolve.tmpBody[iBody].dYobl;
        control->Evolve.tmpUpdate[iBody].iNumBodies[iVar] =
              malloc(update[iBody].iNumYobl * sizeof(int));
        control->Evolve.tmpUpdate[iBody].daDerivProc[iVar] =
              malloc(update[iBody].iNumYobl * sizeof(double));
        control->Evolve.tmpUpdate[iBody].iaType[iVar] =
              malloc(update[iBody].iNumYobl * sizeof(int));
        control->Evolve.tmpUpdate[iBody].iaModule[iVar] =
              malloc(update[iBody].iNumYobl * sizeof(int));
        control->Evolve.tmpUpdate[iBody].iaBody[iVar] =
              malloc(update[iBody].iNumYobl * sizeof(int *));
        control->Evolve.tmpUpdate[iBody].iYobl = iVar;
        for (iSubStep=0; iSubStep < 4; iSubStep++) {
            control->Evolve.daDerivProc[iSubStep][iBody][iVar] =
                  malloc(update[iBody].iNumEqns[iVar] * sizeof(double));
        }
      }

      iEqn = 0;
      for (iModule = 0; iModule < module->iNumModules[iBody]; iModule++) {
        module->fnFinalizeUpdateYobl[iBody][iModule](body, update, &iEqn, iVar,
                                                     iBody, iFoo);
      }

      (*fnUpdate)[iBody][iVar]        = malloc(iEqn * sizeof(fnUpdateVariable));
      update[iBody].daDerivProc[iVar] = malloc(iEqn * sizeof(double));
      iVar++;
    }

    // DistRot's Z (Zobl)
    update[iBody].iZobl = -1;
    if (update[iBody].iNumZobl) {
      update[iBody].iZobl          = iVar;
      update[iBody].iaVar[iVar]    = VZOBL;
      update[iBody].iNumEqns[iVar] = update[iBody].iNumZobl;
      update[iBody].pdVar[iVar]    = &body[iBody].dZobl;
      update[iBody].iNumBodies[iVar] =
            malloc(update[iBody].iNumZobl * sizeof(int));
      update[iBody].iaBody[iVar] =
            malloc(update[iBody].iNumZobl * sizeof(int *));
      update[iBody].iaType[iVar] = malloc(update[iBody].iNumZobl * sizeof(int));
      update[iBody].iaModule[iVar] =
            malloc(update[iBody].iNumZobl * sizeof(int));

      if (control->Evolve.iOneStep == RUNGEKUTTA) {
        control->Evolve.tmpUpdate[iBody].pdVar[iVar] =
              &control->Evolve.tmpBody[iBody].dZobl;
        control->Evolve.tmpUpdate[iBody].iNumBodies[iVar] =
              malloc(update[iBody].iNumZobl * sizeof(int));
        control->Evolve.tmpUpdate[iBody].daDerivProc[iVar] =
              malloc(update[iBody].iNumZobl * sizeof(double));
        control->Evolve.tmpUpdate[iBody].iaType[iVar] =
              malloc(update[iBody].iNumZobl * sizeof(int));
        control->Evolve.tmpUpdate[iBody].iaModule[iVar] =
              malloc(update[iBody].iNumZobl * sizeof(int));
        control->Evolve.tmpUpdate[iBody].iaBody[iVar] =
              malloc(update[iBody].iNumZobl * sizeof(int *));
        control->Evolve.tmpUpdate[iBody].iZobl = iVar;
        for (iSubStep=0; iSubStep < 4; iSubStep++) {
            control->Evolve.daDerivProc[iSubStep][iBody][iVar] =
                  malloc(update[iBody].iNumEqns[iVar] * sizeof(double));
        }
      }

      iEqn = 0;
      for (iModule = 0; iModule < module->iNumModules[iBody]; iModule++) {
        module->fnFinalizeUpdateZobl[iBody][iModule](body, update, &iEqn, iVar,
                                                     iBody, iFoo);
      }

      (*fnUpdate)[iBody][iVar]        = malloc(iEqn * sizeof(fnUpdateVariable));
      update[iBody].daDerivProc[iVar] = malloc(iEqn * sizeof(double));
      iVar++;
    }

    // Add binary stuff here

    // Binary's CBP's Orbital Radiys (dCBPR)
    update[iBody].iCBPR = -1;
    if (update[iBody].iNumCBPR) {
      update[iBody].iCBPR          = iVar;
      update[iBody].iaVar[iVar]    = VCBPR;
      update[iBody].iNumEqns[iVar] = update[iBody].iNumCBPR;
      update[iBody].pdVar[iVar]    = &body[iBody].dCBPR;
      update[iBody].iNumBodies[iVar] =
            malloc(update[iBody].iNumCBPR * sizeof(int));
      update[iBody].iaBody[iVar] =
            malloc(update[iBody].iNumCBPR * sizeof(int *));
      update[iBody].iaType[iVar] = malloc(update[iBody].iNumCBPR * sizeof(int));
      update[iBody].iaModule[iVar] =
            malloc(update[iBody].iNumCBPR * sizeof(int));

      if (control->Evolve.iOneStep == RUNGEKUTTA) {
        control->Evolve.tmpUpdate[iBody].pdVar[iVar] =
              &control->Evolve.tmpBody[iBody].dCBPR;
        control->Evolve.tmpUpdate[iBody].iNumBodies[iVar] =
              malloc(update[iBody].iNumCBPR * sizeof(int));
        control->Evolve.tmpUpdate[iBody].daDerivProc[iVar] =
              malloc(update[iBody].iNumCBPR * sizeof(double));
        control->Evolve.tmpUpdate[iBody].iaType[iVar] =
              malloc(update[iBody].iNumCBPR * sizeof(int));
        control->Evolve.tmpUpdate[iBody].iaModule[iVar] =
              malloc(update[iBody].iNumCBPR * sizeof(int));
        control->Evolve.tmpUpdate[iBody].iaBody[iVar] =
              malloc(update[iBody].iNumCBPR * sizeof(int *));
        for (iSubStep=0; iSubStep < 4; iSubStep++) {
            control->Evolve.daDerivProc[iSubStep][iBody][iVar] =
                  malloc(update[iBody].iNumEqns[iVar] * sizeof(double));
        }
      }

      iEqn = 0;
      for (iModule = 0; iModule < module->iNumModules[iBody]; iModule++) {
        module->fnFinalizeUpdateCBPR[iBody][iModule](body, update, &iEqn, iVar,
                                                     iBody, iFoo);
      }

      (*fnUpdate)[iBody][iVar]        = malloc(iEqn * sizeof(fnUpdateVariable));
      update[iBody].daDerivProc[iVar] = malloc(iEqn * sizeof(double));
      iVar++;
    }


    // Binary's CBP's Orbital height above/below plane (dCBPZ)
    update[iBody].iCBPZ = -1;
    if (update[iBody].iNumCBPZ) {
      update[iBody].iCBPZ          = iVar;
      update[iBody].iaVar[iVar]    = VCBPZ;
      update[iBody].iNumEqns[iVar] = update[iBody].iNumCBPZ;
      update[iBody].pdVar[iVar]    = &body[iBody].dCBPZ;
      update[iBody].iNumBodies[iVar] =
            malloc(update[iBody].iNumCBPZ * sizeof(int));
      update[iBody].iaBody[iVar] =
            malloc(update[iBody].iNumCBPZ * sizeof(int *));
      update[iBody].iaType[iVar] = malloc(update[iBody].iNumCBPZ * sizeof(int));
      update[iBody].iaModule[iVar] =
            malloc(update[iBody].iNumCBPZ * sizeof(int));

      if (control->Evolve.iOneStep == RUNGEKUTTA) {
        control->Evolve.tmpUpdate[iBody].pdVar[iVar] =
              &control->Evolve.tmpBody[iBody].dCBPZ;
        control->Evolve.tmpUpdate[iBody].iNumBodies[iVar] =
              malloc(update[iBody].iNumCBPZ * sizeof(int));
        control->Evolve.tmpUpdate[iBody].daDerivProc[iVar] =
              malloc(update[iBody].iNumCBPZ * sizeof(double));
        control->Evolve.tmpUpdate[iBody].iaType[iVar] =
              malloc(update[iBody].iNumCBPZ * sizeof(int));
        control->Evolve.tmpUpdate[iBody].iaModule[iVar] =
              malloc(update[iBody].iNumCBPZ * sizeof(int));
        control->Evolve.tmpUpdate[iBody].iaBody[iVar] =
              malloc(update[iBody].iNumCBPZ * sizeof(int *));
        for (iSubStep=0; iSubStep < 4; iSubStep++) {
            control->Evolve.daDerivProc[iSubStep][iBody][iVar] =
                  malloc(update[iBody].iNumEqns[iVar] * sizeof(double));
        }
      }

      iEqn = 0;
      for (iModule = 0; iModule < module->iNumModules[iBody]; iModule++) {
        module->fnFinalizeUpdateCBPZ[iBody][iModule](body, update, &iEqn, iVar,
                                                     iBody, iFoo);
      }

      (*fnUpdate)[iBody][iVar]        = malloc(iEqn * sizeof(fnUpdateVariable));
      update[iBody].daDerivProc[iVar] = malloc(iEqn * sizeof(double));
      iVar++;
    }


    // Binary's CBP's Orbital azimuthal angle (dCBPPhi)
    update[iBody].iCBPPhi = -1;
    if (update[iBody].iNumCBPPhi) {
      update[iBody].iCBPPhi        = iVar;
      update[iBody].iaVar[iVar]    = VCBPPHI;
      update[iBody].iNumEqns[iVar] = update[iBody].iNumCBPPhi;
      update[iBody].pdVar[iVar]    = &body[iBody].dCBPPhi;
      update[iBody].iNumBodies[iVar] =
            malloc(update[iBody].iNumCBPPhi * sizeof(int));
      update[iBody].iaBody[iVar] =
            malloc(update[iBody].iNumCBPPhi * sizeof(int *));
      update[iBody].iaType[iVar] =
            malloc(update[iBody].iNumCBPPhi * sizeof(int));
      update[iBody].iaModule[iVar] =
            malloc(update[iBody].iNumCBPPhi * sizeof(int));

      if (control->Evolve.iOneStep == RUNGEKUTTA) {
        control->Evolve.tmpUpdate[iBody].pdVar[iVar] =
              &control->Evolve.tmpBody[iBody].dCBPPhi;
        control->Evolve.tmpUpdate[iBody].iNumBodies[iVar] =
              malloc(update[iBody].iNumCBPPhi * sizeof(int));
        control->Evolve.tmpUpdate[iBody].daDerivProc[iVar] =
              malloc(update[iBody].iNumCBPPhi * sizeof(double));
        control->Evolve.tmpUpdate[iBody].iaType[iVar] =
              malloc(update[iBody].iNumCBPPhi * sizeof(int));
        control->Evolve.tmpUpdate[iBody].iaModule[iVar] =
              malloc(update[iBody].iNumCBPPhi * sizeof(int));
        control->Evolve.tmpUpdate[iBody].iaBody[iVar] =
              malloc(update[iBody].iNumCBPPhi * sizeof(int *));
        for (iSubStep=0; iSubStep < 4; iSubStep++) {
            control->Evolve.daDerivProc[iSubStep][iBody][iVar] =
                  malloc(update[iBody].iNumEqns[iVar] * sizeof(double));
        }
      }

      iEqn = 0;
      for (iModule = 0; iModule < module->iNumModules[iBody]; iModule++) {
        module->fnFinalizeUpdateCBPPhi[iBody][iModule](body, update, &iEqn,
                                                       iVar, iBody, iFoo);
      }

      (*fnUpdate)[iBody][iVar]        = malloc(iEqn * sizeof(fnUpdateVariable));
      update[iBody].daDerivProc[iVar] = malloc(iEqn * sizeof(double));
      iVar++;
    }


    // Binary's CBP's Orbital Radial velocity (dCBPRDot)
    update[iBody].iCBPRDot = -1;
    if (update[iBody].iNumCBPRDot) {
      update[iBody].iCBPRDot       = iVar;
      update[iBody].iaVar[iVar]    = VCBPRDOT;
      update[iBody].iNumEqns[iVar] = update[iBody].iNumCBPRDot;
      update[iBody].pdVar[iVar]    = &body[iBody].dCBPRDot;
      update[iBody].iNumBodies[iVar] =
            malloc(update[iBody].iNumCBPRDot * sizeof(int));
      update[iBody].iaBody[iVar] =
            malloc(update[iBody].iNumCBPRDot * sizeof(int *));
      update[iBody].iaType[iVar] =
            malloc(update[iBody].iNumCBPRDot * sizeof(int));
      update[iBody].iaModule[iVar] =
            malloc(update[iBody].iNumCBPRDot * sizeof(int));

      if (control->Evolve.iOneStep == RUNGEKUTTA) {
        control->Evolve.tmpUpdate[iBody].pdVar[iVar] =
              &control->Evolve.tmpBody[iBody].dCBPRDot;
        control->Evolve.tmpUpdate[iBody].iNumBodies[iVar] =
              malloc(update[iBody].iNumCBPRDot * sizeof(int));
        control->Evolve.tmpUpdate[iBody].daDerivProc[iVar] =
              malloc(update[iBody].iNumCBPRDot * sizeof(double));
        control->Evolve.tmpUpdate[iBody].iaType[iVar] =
              malloc(update[iBody].iNumCBPRDot * sizeof(int));
        control->Evolve.tmpUpdate[iBody].iaModule[iVar] =
              malloc(update[iBody].iNumCBPRDot * sizeof(int));
        control->Evolve.tmpUpdate[iBody].iaBody[iVar] =
              malloc(update[iBody].iNumCBPRDot * sizeof(int *));
        for (iSubStep=0; iSubStep < 4; iSubStep++) {
            control->Evolve.daDerivProc[iSubStep][iBody][iVar] =
                  malloc(update[iBody].iNumEqns[iVar] * sizeof(double));
        }
      }

      iEqn = 0;
      for (iModule = 0; iModule < module->iNumModules[iBody]; iModule++) {
        module->fnFinalizeUpdateCBPRDot[iBody][iModule](body, update, &iEqn,
                                                        iVar, iBody, iFoo);
      }

      (*fnUpdate)[iBody][iVar]        = malloc(iEqn * sizeof(fnUpdateVariable));
      update[iBody].daDerivProc[iVar] = malloc(iEqn * sizeof(double));
      iVar++;
    }


    // Binary's CBP's Orbital z orbital velocity (dCBPZDot)
    update[iBody].iCBPZDot = -1;
    if (update[iBody].iNumCBPZDot) {
      update[iBody].iCBPZDot       = iVar;
      update[iBody].iaVar[iVar]    = VCBPZDOT;
      update[iBody].iNumEqns[iVar] = update[iBody].iNumCBPZDot;
      update[iBody].pdVar[iVar]    = &body[iBody].dCBPZDot;
      update[iBody].iNumBodies[iVar] =
            malloc(update[iBody].iNumCBPZDot * sizeof(int));
      update[iBody].iaBody[iVar] =
            malloc(update[iBody].iNumCBPZDot * sizeof(int *));
      update[iBody].iaType[iVar] =
            malloc(update[iBody].iNumCBPZDot * sizeof(int));
      update[iBody].iaModule[iVar] =
            malloc(update[iBody].iNumCBPZDot * sizeof(int));

      if (control->Evolve.iOneStep == RUNGEKUTTA) {
        control->Evolve.tmpUpdate[iBody].pdVar[iVar] =
              &control->Evolve.tmpBody[iBody].dCBPZDot;
        control->Evolve.tmpUpdate[iBody].iNumBodies[iVar] =
              malloc(update[iBody].iNumCBPZDot * sizeof(int));
        control->Evolve.tmpUpdate[iBody].daDerivProc[iVar] =
              malloc(update[iBody].iNumCBPZDot * sizeof(double));
        control->Evolve.tmpUpdate[iBody].iaType[iVar] =
              malloc(update[iBody].iNumCBPZDot * sizeof(int));
        control->Evolve.tmpUpdate[iBody].iaModule[iVar] =
              malloc(update[iBody].iNumCBPZDot * sizeof(int));
        control->Evolve.tmpUpdate[iBody].iaBody[iVar] =
              malloc(update[iBody].iNumCBPZDot * sizeof(int *));
        for (iSubStep=0; iSubStep < 4; iSubStep++) {
            control->Evolve.daDerivProc[iSubStep][iBody][iVar] =
                  malloc(update[iBody].iNumEqns[iVar] * sizeof(double));
        }
      }

      iEqn = 0;
      for (iModule = 0; iModule < module->iNumModules[iBody]; iModule++) {
        module->fnFinalizeUpdateCBPZDot[iBody][iModule](body, update, &iEqn,
                                                        iVar, iBody, iFoo);
      }

      (*fnUpdate)[iBody][iVar]        = malloc(iEqn * sizeof(fnUpdateVariable));
      update[iBody].daDerivProc[iVar] = malloc(iEqn * sizeof(double));
      iVar++;
    }


    // Binary's CBP's Orbital Angular velocity (dCBPPhiDot)
    update[iBody].iCBPPhiDot = -1;
    if (update[iBody].iNumCBPPhiDot) {
      update[iBody].iCBPPhiDot     = iVar;
      update[iBody].iaVar[iVar]    = VCBPPHIDOT;
      update[iBody].iNumEqns[iVar] = update[iBody].iNumCBPPhiDot;
      update[iBody].pdVar[iVar]    = &body[iBody].dCBPPhiDot;
      update[iBody].iNumBodies[iVar] =
            malloc(update[iBody].iNumCBPPhiDot * sizeof(int));
      update[iBody].iaBody[iVar] =
            malloc(update[iBody].iNumCBPPhiDot * sizeof(int *));
      update[iBody].iaType[iVar] =
            malloc(update[iBody].iNumCBPPhiDot * sizeof(int));
      update[iBody].iaModule[iVar] =
            malloc(update[iBody].iNumCBPPhiDot * sizeof(int));

      if (control->Evolve.iOneStep == RUNGEKUTTA) {
        control->Evolve.tmpUpdate[iBody].pdVar[iVar] =
              &control->Evolve.tmpBody[iBody].dCBPPhiDot;
        control->Evolve.tmpUpdate[iBody].iNumBodies[iVar] =
              malloc(update[iBody].iNumCBPPhiDot * sizeof(int));
        control->Evolve.tmpUpdate[iBody].daDerivProc[iVar] =
              malloc(update[iBody].iNumCBPPhiDot * sizeof(double));
        control->Evolve.tmpUpdate[iBody].iaType[iVar] =
              malloc(update[iBody].iNumCBPPhiDot * sizeof(int));
        control->Evolve.tmpUpdate[iBody].iaModule[iVar] =
              malloc(update[iBody].iNumCBPPhiDot * sizeof(int));
        control->Evolve.tmpUpdate[iBody].iaBody[iVar] =
              malloc(update[iBody].iNumCBPPhiDot * sizeof(int *));
        for (iSubStep=0; iSubStep < 4; iSubStep++) {
            control->Evolve.daDerivProc[iSubStep][iBody][iVar] =
                  malloc(update[iBody].iNumEqns[iVar] * sizeof(double));
        }
      }

      iEqn = 0;
      for (iModule = 0; iModule < module->iNumModules[iBody]; iModule++) {
        module->fnFinalizeUpdateCBPPhiDot[iBody][iModule](body, update, &iEqn,
                                                          iVar, iBody, iFoo);
      }

      (*fnUpdate)[iBody][iVar]        = malloc(iEqn * sizeof(fnUpdateVariable));
      update[iBody].daDerivProc[iVar] = malloc(iEqn * sizeof(double));
      iVar++;
    }

    // POISE's ice mass
    //    update[iBody].iIceMass = -1;
    //     if (update[iBody].iNumIceMass) {
    //       /* XXX hack to get ice sheets working, since since these don't get
    //       malloced until verify */ control->Evolve.tmpBody[iBody].daIceMass =
    //       malloc(body[iBody].iNumLats*sizeof(double)); body[iBody].daIceMass
    //       = malloc(body[iBody].iNumLats*sizeof(double));
    //
    //       update[iBody].iIceMass = iVar;
    //       control->Evolve.tmpUpdate[iBody].iIceMass = iVar;
    //       for (iLat=0;iLat<body[iBody].iNumLats;iLat++) {
    //         update[iBody].iaIceMass[iLat] = iVar;
    //         update[iBody].iaVar[iVar] = VICEMASS;
    //         update[iBody].iNumEqns[iVar] = update[iBody].iNumIceMass;
    //         update[iBody].pdVar[iVar] = &body[iBody].daIceMass[iLat];
    //         update[iBody].iNumBodies[iVar] =
    //         malloc(update[iBody].iNumIceMass*sizeof(int));
    //         update[iBody].iaBody[iVar] =
    //         malloc(update[iBody].iNumIceMass*sizeof(int*));
    //         update[iBody].iaType[iVar] =
    //         malloc(update[iBody].iNumIceMass*sizeof(int));
    //         update[iBody].iaModule[iVar] =
    //         malloc(update[iBody].iNumIceMass*sizeof(int));
    //
    //         if (control->Evolve.iOneStep == RUNGEKUTTA) {
    //
    //           control->Evolve.tmpUpdate[iBody].pdVar[iVar] =
    //           &control->Evolve.tmpBody[iBody].daIceMass[iLat];
    //           control->Evolve.tmpUpdate[iBody].iNumBodies[iVar] =
    //           malloc(update[iBody].iNumIceMass*sizeof(int));
    //           control->Evolve.tmpUpdate[iBody].daDerivProc[iVar] =
    //           malloc(update[iBody].iNumIceMass*sizeof(double));
    //           control->Evolve.tmpUpdate[iBody].iaType[iVar] =
    //           malloc(update[iBody].iNumIceMass*sizeof(int));
    //           control->Evolve.tmpUpdate[iBody].iaModule[iVar] =
    //           malloc(update[iBody].iNumIceMass*sizeof(int));
    //           control->Evolve.tmpUpdate[iBody].iaBody[iVar] =
    //           malloc(update[iBody].iNumIceMass*sizeof(int*));
    //         }
    //
    //         iEqn=0;
    //         for (iModule=0;iModule<module->iNumModules[iBody];iModule++)
    //           module->fnFinalizeUpdateIceMass[iBody][iModule](body,update,&iEqn,iVar,iBody,iLat);
    //
    //         (*fnUpdate)[iBody][iVar]=malloc(iEqn*sizeof(fnUpdateVariable));
    //         update[iBody].daDerivProc[iVar]=malloc(iEqn*sizeof(double));
    //         iVar++;
    //       }
    //
    //     }

    // Eccentricity vector x
    update[iBody].iEccX = -1;
    if (update[iBody].iNumEccX) {
      update[iBody].iEccX          = iVar;
      update[iBody].iaVar[iVar]    = VECCX;
      update[iBody].iNumEqns[iVar] = update[iBody].iNumEccX;
      update[iBody].pdVar[iVar]    = &body[iBody].dEccX;
      update[iBody].iNumBodies[iVar] =
            malloc(update[iBody].iNumEccX * sizeof(int));
      update[iBody].iaBody[iVar] =
            malloc(update[iBody].iNumEccX * sizeof(int *));
      update[iBody].iaType[iVar] = malloc(update[iBody].iNumEccX * sizeof(int));
      update[iBody].iaModule[iVar] =
            malloc(update[iBody].iNumEccX * sizeof(int));

      if (control->Evolve.iOneStep == RUNGEKUTTA) {
        control->Evolve.tmpUpdate[iBody].pdVar[iVar] =
              &control->Evolve.tmpBody[iBody].dEccX;
        control->Evolve.tmpUpdate[iBody].iNumBodies[iVar] =
              malloc(update[iBody].iNumEccX * sizeof(int));
        control->Evolve.tmpUpdate[iBody].daDerivProc[iVar] =
              malloc(update[iBody].iNumEccX * sizeof(double));
        control->Evolve.tmpUpdate[iBody].iaType[iVar] =
              malloc(update[iBody].iNumEccX * sizeof(int));
        control->Evolve.tmpUpdate[iBody].iaModule[iVar] =
              malloc(update[iBody].iNumEccX * sizeof(int));
        control->Evolve.tmpUpdate[iBody].iaBody[iVar] =
              malloc(update[iBody].iNumEccX * sizeof(int *));
        for (iSubStep=0; iSubStep < 4; iSubStep++) {
            control->Evolve.daDerivProc[iSubStep][iBody][iVar] =
                  malloc(update[iBody].iNumEqns[iVar] * sizeof(double));
        }
      }

      iEqn = 0;
      for (iModule = 0; iModule < module->iNumModules[iBody]; iModule++) {
        module->fnFinalizeUpdateEccX[iBody][iModule](body, update, &iEqn, iVar,
                                                     iBody, iFoo);
      }

      (*fnUpdate)[iBody][iVar]        = malloc(iEqn * sizeof(fnUpdateVariable));
      update[iBody].daDerivProc[iVar] = malloc(iEqn * sizeof(double));
      iVar++;
    }

    // Eccentricity vector y
    update[iBody].iEccY = -1;
    if (update[iBody].iNumEccY) {
      update[iBody].iEccY          = iVar;
      update[iBody].iaVar[iVar]    = VECCY;
      update[iBody].iNumEqns[iVar] = update[iBody].iNumEccY;
      update[iBody].pdVar[iVar]    = &body[iBody].dEccY;
      update[iBody].iNumBodies[iVar] =
            malloc(update[iBody].iNumEccY * sizeof(int));
      update[iBody].iaBody[iVar] =
            malloc(update[iBody].iNumEccY * sizeof(int *));
      update[iBody].iaType[iVar] = malloc(update[iBody].iNumEccY * sizeof(int));
      update[iBody].iaModule[iVar] =
            malloc(update[iBody].iNumEccY * sizeof(int));

      if (control->Evolve.iOneStep == RUNGEKUTTA) {
        control->Evolve.tmpUpdate[iBody].pdVar[iVar] =
              &control->Evolve.tmpBody[iBody].dEccY;
        control->Evolve.tmpUpdate[iBody].iNumBodies[iVar] =
              malloc(update[iBody].iNumEccY * sizeof(int));
        control->Evolve.tmpUpdate[iBody].daDerivProc[iVar] =
              malloc(update[iBody].iNumEccY * sizeof(double));
        control->Evolve.tmpUpdate[iBody].iaType[iVar] =
              malloc(update[iBody].iNumEccY * sizeof(int));
        control->Evolve.tmpUpdate[iBody].iaModule[iVar] =
              malloc(update[iBody].iNumEccY * sizeof(int));
        control->Evolve.tmpUpdate[iBody].iaBody[iVar] =
              malloc(update[iBody].iNumEccY * sizeof(int *));
        for (iSubStep=0; iSubStep < 4; iSubStep++) {
            control->Evolve.daDerivProc[iSubStep][iBody][iVar] =
                  malloc(update[iBody].iNumEqns[iVar] * sizeof(double));
        }
      }

      iEqn = 0;
      for (iModule = 0; iModule < module->iNumModules[iBody]; iModule++) {
        module->fnFinalizeUpdateEccY[iBody][iModule](body, update, &iEqn, iVar,
                                                     iBody, iFoo);
      }

      (*fnUpdate)[iBody][iVar]        = malloc(iEqn * sizeof(fnUpdateVariable));
      update[iBody].daDerivProc[iVar] = malloc(iEqn * sizeof(double));
      iVar++;
    }

    // Eccentricity vector z
    update[iBody].iEccZ = -1;
    if (update[iBody].iNumEccZ) {
      update[iBody].iEccZ          = iVar;
      update[iBody].iaVar[iVar]    = VECCZ;
      update[iBody].iNumEqns[iVar] = update[iBody].iNumEccZ;
      update[iBody].pdVar[iVar]    = &body[iBody].dEccZ;
      update[iBody].iNumBodies[iVar] =
            malloc(update[iBody].iNumEccZ * sizeof(int));
      update[iBody].iaBody[iVar] =
            malloc(update[iBody].iNumEccZ * sizeof(int *));
      update[iBody].iaType[iVar] = malloc(update[iBody].iNumEccZ * sizeof(int));
      update[iBody].iaModule[iVar] =
            malloc(update[iBody].iNumEccZ * sizeof(int));

      if (control->Evolve.iOneStep == RUNGEKUTTA) {
        control->Evolve.tmpUpdate[iBody].pdVar[iVar] =
              &control->Evolve.tmpBody[iBody].dEccZ;
        control->Evolve.tmpUpdate[iBody].iNumBodies[iVar] =
              malloc(update[iBody].iNumEccZ * sizeof(int));
        control->Evolve.tmpUpdate[iBody].daDerivProc[iVar] =
              malloc(update[iBody].iNumEccZ * sizeof(double));
        control->Evolve.tmpUpdate[iBody].iaType[iVar] =
              malloc(update[iBody].iNumEccZ * sizeof(int));
        control->Evolve.tmpUpdate[iBody].iaModule[iVar] =
              malloc(update[iBody].iNumEccZ * sizeof(int));
        control->Evolve.tmpUpdate[iBody].iaBody[iVar] =
              malloc(update[iBody].iNumEccZ * sizeof(int *));
        for (iSubStep=0; iSubStep < 4; iSubStep++) {
            control->Evolve.daDerivProc[iSubStep][iBody][iVar] =
                  malloc(update[iBody].iNumEqns[iVar] * sizeof(double));
        }
      }

      iEqn = 0;
      for (iModule = 0; iModule < module->iNumModules[iBody]; iModule++) {
        module->fnFinalizeUpdateEccZ[iBody][iModule](body, update, &iEqn, iVar,
                                                     iBody, iFoo);
      }

      (*fnUpdate)[iBody][iVar]        = malloc(iEqn * sizeof(fnUpdateVariable));
      update[iBody].daDerivProc[iVar] = malloc(iEqn * sizeof(double));
      iVar++;
    }

    // Angular momentum x component
    update[iBody].iAngMX = -1;
    if (update[iBody].iNumAngMX) {
      update[iBody].iAngMX         = iVar;
      update[iBody].iaVar[iVar]    = VANGMX;
      update[iBody].iNumEqns[iVar] = update[iBody].iNumAngMX;
      update[iBody].pdVar[iVar]    = &body[iBody].dAngMX;
      update[iBody].iNumBodies[iVar] =
            malloc(update[iBody].iNumAngMX * sizeof(int));
      update[iBody].iaBody[iVar] =
            malloc(update[iBody].iNumAngMX * sizeof(int *));
      update[iBody].iaType[iVar] =
            malloc(update[iBody].iNumAngMX * sizeof(int));
      update[iBody].iaModule[iVar] =
            malloc(update[iBody].iNumAngMX * sizeof(int));

      if (control->Evolve.iOneStep == RUNGEKUTTA) {
        control->Evolve.tmpUpdate[iBody].pdVar[iVar] =
              &control->Evolve.tmpBody[iBody].dAngMX;
        control->Evolve.tmpUpdate[iBody].iNumBodies[iVar] =
              malloc(update[iBody].iNumAngMX * sizeof(int));
        control->Evolve.tmpUpdate[iBody].daDerivProc[iVar] =
              malloc(update[iBody].iNumAngMX * sizeof(double));
        control->Evolve.tmpUpdate[iBody].iaType[iVar] =
              malloc(update[iBody].iNumAngMX * sizeof(int));
        control->Evolve.tmpUpdate[iBody].iaModule[iVar] =
              malloc(update[iBody].iNumAngMX * sizeof(int));
        control->Evolve.tmpUpdate[iBody].iaBody[iVar] =
              malloc(update[iBody].iNumAngMX * sizeof(int *));
        for (iSubStep=0; iSubStep < 4; iSubStep++) {
            control->Evolve.daDerivProc[iSubStep][iBody][iVar] =
                  malloc(update[iBody].iNumEqns[iVar] * sizeof(double));
        }
      }

      iEqn = 0;
      for (iModule = 0; iModule < module->iNumModules[iBody]; iModule++) {
        module->fnFinalizeUpdateAngMX[iBody][iModule](body, update, &iEqn, iVar,
                                                      iBody, iFoo);
      }

      (*fnUpdate)[iBody][iVar]        = malloc(iEqn * sizeof(fnUpdateVariable));
      update[iBody].daDerivProc[iVar] = malloc(iEqn * sizeof(double));
      iVar++;
    }

    // Angular momentum y component
    update[iBody].iAngMY = -1;
    if (update[iBody].iNumAngMY) {
      update[iBody].iAngMY         = iVar;
      update[iBody].iaVar[iVar]    = VANGMY;
      update[iBody].iNumEqns[iVar] = update[iBody].iNumAngMY;
      update[iBody].pdVar[iVar]    = &body[iBody].dAngMY;
      update[iBody].iNumBodies[iVar] =
            malloc(update[iBody].iNumAngMY * sizeof(int));
      update[iBody].iaBody[iVar] =
            malloc(update[iBody].iNumAngMY * sizeof(int *));
      update[iBody].iaType[iVar] =
            malloc(update[iBody].iNumAngMY * sizeof(int));
      update[iBody].iaModule[iVar] =
            malloc(update[iBody].iNumAngMY * sizeof(int));

      if (control->Evolve.iOneStep == RUNGEKUTTA) {
        control->Evolve.tmpUpdate[iBody].pdVar[iVar] =
              &control->Evolve.tmpBody[iBody].dAngMY;
        control->Evolve.tmpUpdate[iBody].iNumBodies[iVar] =
              malloc(update[iBody].iNumAngMY * sizeof(int));
        control->Evolve.tmpUpdate[iBody].daDerivProc[iVar] =
              malloc(update[iBody].iNumAngMY * sizeof(double));
        control->Evolve.tmpUpdate[iBody].iaType[iVar] =
              malloc(update[iBody].iNumAngMY * sizeof(int));
        control->Evolve.tmpUpdate[iBody].iaModule[iVar] =
              malloc(update[iBody].iNumAngMY * sizeof(int));
        control->Evolve.tmpUpdate[iBody].iaBody[iVar] =
              malloc(update[iBody].iNumAngMY * sizeof(int *));
        for (iSubStep=0; iSubStep < 4; iSubStep++) {
            control->Evolve.daDerivProc[iSubStep][iBody][iVar] =
                  malloc(update[iBody].iNumEqns[iVar] * sizeof(double));
        }
      }

      iEqn = 0;
      for (iModule = 0; iModule < module->iNumModules[iBody]; iModule++) {
        module->fnFinalizeUpdateAngMY[iBody][iModule](body, update, &iEqn, iVar,
                                                      iBody, iFoo);
      }

      (*fnUpdate)[iBody][iVar]        = malloc(iEqn * sizeof(fnUpdateVariable));
      update[iBody].daDerivProc[iVar] = malloc(iEqn * sizeof(double));
      iVar++;
    }

    // Angular momentum z component
    update[iBody].iAngMZ = -1;
    if (update[iBody].iNumAngMZ) {
      update[iBody].iAngMZ         = iVar;
      update[iBody].iaVar[iVar]    = VANGMZ;
      update[iBody].iNumEqns[iVar] = update[iBody].iNumAngMZ;
      update[iBody].pdVar[iVar]    = &body[iBody].dAngMZ;
      update[iBody].iNumBodies[iVar] =
            malloc(update[iBody].iNumAngMZ * sizeof(int));
      update[iBody].iaBody[iVar] =
            malloc(update[iBody].iNumAngMZ * sizeof(int *));
      update[iBody].iaType[iVar] =
            malloc(update[iBody].iNumAngMZ * sizeof(int));
      update[iBody].iaModule[iVar] =
            malloc(update[iBody].iNumAngMZ * sizeof(int));

      if (control->Evolve.iOneStep == RUNGEKUTTA) {
        control->Evolve.tmpUpdate[iBody].pdVar[iVar] =
              &control->Evolve.tmpBody[iBody].dAngMZ;
        control->Evolve.tmpUpdate[iBody].iNumBodies[iVar] =
              malloc(update[iBody].iNumAngMZ * sizeof(int));
        control->Evolve.tmpUpdate[iBody].daDerivProc[iVar] =
              malloc(update[iBody].iNumAngMZ * sizeof(double));
        control->Evolve.tmpUpdate[iBody].iaType[iVar] =
              malloc(update[iBody].iNumAngMZ * sizeof(int));
        control->Evolve.tmpUpdate[iBody].iaModule[iVar] =
              malloc(update[iBody].iNumAngMZ * sizeof(int));
        control->Evolve.tmpUpdate[iBody].iaBody[iVar] =
              malloc(update[iBody].iNumAngMZ * sizeof(int *));
        for (iSubStep=0; iSubStep < 4; iSubStep++) {
            control->Evolve.daDerivProc[iSubStep][iBody][iVar] =
                  malloc(update[iBody].iNumEqns[iVar] * sizeof(double));
        }
      }

      iEqn = 0;
      for (iModule = 0; iModule < module->iNumModules[iBody]; iModule++) {
        module->fnFinalizeUpdateAngMZ[iBody][iModule](body, update, &iEqn, iVar,
                                                      iBody, iFoo);
      }

      (*fnUpdate)[iBody][iVar]        = malloc(iEqn * sizeof(fnUpdateVariable));
      update[iBody].daDerivProc[iVar] = malloc(iEqn * sizeof(double));
      iVar++;
    }

    // XUV Luminosity
    /* This one is tricky because it is an auxiliary property
       of STELLAR, but a primary variable of FLARE. */

    // Rodrigo: TODO TODO TODO: This will not work, since it will overwrite
    // dLXUV in body[iBody]!!!

    update[iBody].iLXUV = -1;
    if (update[iBody].iNumLXUV) {
      update[iBody].iLXUV          = iVar;
      update[iBody].iaVar[iVar]    = VLXUV;
      update[iBody].iNumEqns[iVar] = update[iBody].iNumLXUV;
      update[iBody].pdVar[iVar] =
            &body[iBody].dLXUVFlare; // Note this does NOT point dLXUV
      update[iBody].iNumBodies[iVar] =
            malloc(update[iBody].iNumLXUV * sizeof(int));
      update[iBody].iaBody[iVar] =
            malloc(update[iBody].iNumLXUV * sizeof(int *));
      update[iBody].iaType[iVar] = malloc(update[iBody].iNumLXUV * sizeof(int));
      update[iBody].iaModule[iVar] =
            malloc(update[iBody].iNumLXUV * sizeof(int));

      if (control->Evolve.iOneStep == RUNGEKUTTA) {
        control->Evolve.tmpUpdate[iBody].pdVar[iVar] =
              &control->Evolve.tmpBody[iBody].dLXUV;
        control->Evolve.tmpUpdate[iBody].iNumBodies[iVar] =
              malloc(update[iBody].iNumLXUV * sizeof(int));
        control->Evolve.tmpUpdate[iBody].daDerivProc[iVar] =
              malloc(update[iBody].iNumLXUV * sizeof(double));
        control->Evolve.tmpUpdate[iBody].iaType[iVar] =
              malloc(update[iBody].iNumLXUV * sizeof(int));
        control->Evolve.tmpUpdate[iBody].iaModule[iVar] =
              malloc(update[iBody].iNumLXUV * sizeof(int));
        control->Evolve.tmpUpdate[iBody].iaBody[iVar] =
              malloc(update[iBody].iNumLXUV * sizeof(int *));
        for (iSubStep=0; iSubStep < 4; iSubStep++) {
            control->Evolve.daDerivProc[iSubStep][iBody][iVar] =
                  malloc(update[iBody].iNumEqns[iVar] * sizeof(double));
        }
      }

      iEqn = 0;
      for (iModule = 0; iModule < module->iNumModules[iBody]; iModule++) {
        module->fnFinalizeUpdateLXUV[iBody][iModule](body, update, &iEqn, iVar,
                                                     iBody, iFoo);
      }

      (*fnUpdate)[iBody][iVar]        = malloc(iEqn * sizeof(fnUpdateVariable));
      update[iBody].daDerivProc[iVar] = malloc(iEqn * sizeof(double));
      iVar++;
    }

    /* Lost Angular Momentum */
    update[iBody].iLostAngMom = -1;
    if (update[iBody].iNumLostAngMom) {
      update[iBody].iLostAngMom    = iVar;
      update[iBody].iaVar[iVar]    = VLOSTANGMOM;
      update[iBody].iNumEqns[iVar] = update[iBody].iNumLostAngMom;
      update[iBody].pdVar[iVar]    = &body[iBody].dLostAngMom;
      update[iBody].iNumBodies[iVar] =
            malloc(update[iBody].iNumLostAngMom * sizeof(int));
      update[iBody].iaBody[iVar] =
            malloc(update[iBody].iNumLostAngMom * sizeof(int *));
      update[iBody].iaType[iVar] =
            malloc(update[iBody].iNumLostAngMom * sizeof(int));
      update[iBody].iaModule[iVar] =
            malloc(update[iBody].iNumLostAngMom * sizeof(int));

      if (control->Evolve.iOneStep == RUNGEKUTTA) {
        control->Evolve.tmpUpdate[iBody].pdVar[iVar] =
              &control->Evolve.tmpBody[iBody].dLostAngMom;
        control->Evolve.tmpUpdate[iBody].iNumBodies[iVar] =
              malloc(update[iBody].iNumLostAngMom * sizeof(int));
        control->Evolve.tmpUpdate[iBody].daDerivProc[iVar] =
              malloc(update[iBody].iNumLostAngMom * sizeof(double));
        control->Evolve.tmpUpdate[iBody].iaType[iVar] =
              malloc(update[iBody].iNumLostAngMom * sizeof(int));
        control->Evolve.tmpUpdate[iBody].iaModule[iVar] =
              malloc(update[iBody].iNumLostAngMom * sizeof(int));
        control->Evolve.tmpUpdate[iBody].iaBody[iVar] =
              malloc(update[iBody].iNumLostAngMom * sizeof(int *));
        for (iSubStep=0; iSubStep < 4; iSubStep++) {
            control->Evolve.daDerivProc[iSubStep][iBody][iVar] =
                  malloc(update[iBody].iNumEqns[iVar] * sizeof(double));
        }
      }

      iEqn = 0;
      for (iModule = 0; iModule < module->iNumModules[iBody]; iModule++) {
        module->fnFinalizeUpdateLostAngMom[iBody][iModule](body, update, &iEqn,
                                                           iVar, iBody, iFoo);
      }

      (*fnUpdate)[iBody][iVar]        = malloc(iEqn * sizeof(fnUpdateVariable));
      update[iBody].daDerivProc[iVar] = malloc(iEqn * sizeof(double));
      iVar++;
    }

    /* Lost Energy */
    update[iBody].iLostEng = -1;
    if (update[iBody].iNumLostEng) {
      update[iBody].iLostEng       = iVar;
      update[iBody].iaVar[iVar]    = VLOSTENG;
      update[iBody].iNumEqns[iVar] = update[iBody].iNumLostEng;
      update[iBody].pdVar[iVar]    = &body[iBody].dLostEng;
      update[iBody].iNumBodies[iVar] =
            malloc(update[iBody].iNumLostEng * sizeof(int));
      update[iBody].iaBody[iVar] =
            malloc(update[iBody].iNumLostEng * sizeof(int *));
      update[iBody].iaType[iVar] =
            malloc(update[iBody].iNumLostEng * sizeof(int));
      update[iBody].iaModule[iVar] =
            malloc(update[iBody].iNumLostEng * sizeof(int));

      if (control->Evolve.iOneStep == RUNGEKUTTA) {
        control->Evolve.tmpUpdate[iBody].pdVar[iVar] =
              &control->Evolve.tmpBody[iBody].dLostEng;
        control->Evolve.tmpUpdate[iBody].iNumBodies[iVar] =
              malloc(update[iBody].iNumLostEng * sizeof(int));
        control->Evolve.tmpUpdate[iBody].daDerivProc[iVar] =
              malloc(update[iBody].iNumLostEng * sizeof(double));
        control->Evolve.tmpUpdate[iBody].iaType[iVar] =
              malloc(update[iBody].iNumLostEng * sizeof(int));
        control->Evolve.tmpUpdate[iBody].iaModule[iVar] =
              malloc(update[iBody].iNumLostEng * sizeof(int));
        control->Evolve.tmpUpdate[iBody].iaBody[iVar] =
              malloc(update[iBody].iNumLostEng * sizeof(int *));
        for (iSubStep=0; iSubStep < 4; iSubStep++) {
            control->Evolve.daDerivProc[iSubStep][iBody][iVar] =
                  malloc(update[iBody].iNumEqns[iVar] * sizeof(double));
        }
      }

      iEqn = 0;
      for (iModule = 0; iModule < module->iNumModules[iBody]; iModule++) {
        module->fnFinalizeUpdateLostEng[iBody][iModule](body, update, &iEqn,
                                                        iVar, iBody, iFoo);
      }

      (*fnUpdate)[iBody][iVar]        = malloc(iEqn * sizeof(fnUpdateVariable));
      update[iBody].daDerivProc[iVar] = malloc(iEqn * sizeof(double));
      iVar++;
    }

    // Finalize adding multi-module equations to the matrix
    FinalizeUpdateMulti(body, control, module, update, fnUpdate, &iVar, iBody,
                        iFoo);
  }
}
