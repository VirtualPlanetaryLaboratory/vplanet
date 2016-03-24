#include <stdio.h>
#include <math.h>
#include <assert.h>
#include <stdlib.h>
#include "vplanet.h"

/* lines where something like iBody == 0 occurs
 * ~18
 */

void PropsAuxNULL(BODY *body,UPDATE *update,int iBody) {
}

void PropsAuxGeneral(BODY *body,CONTROL *control) {
  int iBody;

  for (iBody=0;iBody<control->Evolve.iNumBodies;iBody++) {
    if(body[iBody].bBinary){ // Considering binary systems
      if(body[iBody].iBodyType == 0) // CBP
      {
        // Set CBP orbital elements, mean motion
        fdAssignOrbitalElements(body,iBody);
        body[iBody].dMeanMotion = fdSemiToMeanMotion(body[iBody].dSemi,(body[0].dMass+body[1].dMass+body[iBody].dMass));
      
      }
      else if(body[iBody].iBodyType == 1 && iBody == 0) // Primary
      {
        // Correctly set binary's mean motion
        body[iBody].dMeanMotion = fdSemiToMeanMotion(body[iBody].dSemi,(body[0].dMass+body[1].dMass));
      
        // Compute binary's eccentricity from Kecc and Hecc (since they are primary variables)
        body[iBody].dEcc = sqrt(pow(body[iBody].dKecc,2) + pow(body[iBody].dHecc,2));
      }
      else {}
    }
    else { // Not considering binary systems
    if (iBody != 0) {
      body[iBody].dMeanMotion = fdSemiToMeanMotion(body[iBody].dSemi,(body[0].dMass+body[iBody].dMass));
    }
  }
  }
}

void PropertiesAuxiliary(BODY *body,CONTROL *control,UPDATE *update) {
  int iBody,iModule;

  PropsAuxGeneral(body,control);

  /* Get properties from each module */
  for (iBody=0;iBody<control->Evolve.iNumBodies;iBody++) {
    // Uni-module properties
    for (iModule=0;iModule<control->Evolve.iNumModules[iBody];iModule++)
      control->Evolve.fnPropsAux[iBody][iModule](body,update,iBody);

    // Multi-module properties
    for (iModule=0;iModule<control->Evolve.iNumMultiProps[iBody];iModule++)
      control->Evolve.fnPropsAuxMulti[iBody][iModule](body,update,iBody);
  }


}

/*
 * Integration Control
 */

double AssignDt(double dMin,double dNextOutput,double dEta) {
  int i;

  dMin = dEta * dMin;
  if (dNextOutput < dMin)
    dMin = dNextOutput;

  return dMin;
}

double fdNextOutput(double dTime,double dOutputInterval) {
  int nSteps;
  
  /* Number of output so far */
  nSteps = (int)(dTime/dOutputInterval);
  /* Next output is one more */
  return (nSteps+1)*dOutputInterval;
}

/* fdGetUpdateInfo fills the Update arrays with the derivatives 
 * or new values. It returns the smallest timescale for use 
 * in variable timestepping. Probably should have a different 
 * version for rigid time-stepping, but that should probably 
 * never be used anyway. 
*/

double fdGetUpdateInfo(BODY *body,CONTROL *control,SYSTEM *system,UPDATE *update,fnUpdateVariable ***fnUpdate) {
  
  int iBody,iVar,iEqn;
  EVOLVE integr;
  double dVarNow,dMinNow,dMin=HUGE,dVarTotal;
  double amp = 0.0;

  integr = control->Evolve;

  // XXXX Change Eqn to Proc?

  for (iBody=0;iBody<control->Evolve.iNumBodies;iBody++) {
    if (update[iBody].iNumVars > 0) {
      for (iVar=0;iVar<update[iBody].iNumVars;iVar++) {
          if (update[iBody].iaType[iVar][0] == 0) {
            // The parameter does not require a derivative, but is calculated explicitly as a function of age.
            dVarNow = *update[iBody].pdVar[iVar];
            for (iEqn=0;iEqn<update[iBody].iNumEqns[iVar];iEqn++) {
              update[iBody].daDerivProc[iVar][iEqn] = fnUpdate[iBody][iVar][iEqn](body,system,update[iBody].iaBody[iVar][iEqn]);
            }
            if (control->Evolve.bFirstStep) {
              dMin = integr.dTimeStep;
              control->Evolve.bFirstStep = 0;
            } else {
              /* Sum over all equations giving new value of the variable */
              dVarTotal = 0.;
              for (iEqn=0;iEqn<update[iBody].iNumEqns[iVar];iEqn++) {
                dVarTotal += update[iBody].daDerivProc[iVar][iEqn];
              }
              // Prevent division by zero
              if (dVarNow != dVarTotal) {
                dMinNow = fabs(dVarNow/((dVarNow - dVarTotal)/integr.dTimeStep));
                if (dMinNow < dMin)
                  dMin = dMinNow;
              }
            }
          } else if (update[iBody].iaType[iVar][0] == 10) { // 10 because binary
              // The parameter does not require a derivative, but is calculated explicitly as a function of age.
              // and the parameter can oscillate through 0 (like CBP position, velocities)
              dVarNow = *update[iBody].pdVar[iVar];
              // Something like amp = body.CBPAmp[iVar];
              for (iEqn=0;iEqn<update[iBody].iNumEqns[iVar];iEqn++) {
                update[iBody].daDerivProc[iVar][iEqn] = fnUpdate[iBody][iVar][iEqn](body,system,update[iBody].iaBody[iVar][iEqn]);
              }
              if (control->Evolve.bFirstStep) {
                dMin = integr.dTimeStep;
                control->Evolve.bFirstStep = 0;
              } else {
                /* Sum over all equations giving new value of the variable */
                dVarTotal = 0.;
                for (iEqn=0;iEqn<update[iBody].iNumEqns[iVar];iEqn++) {
                  dVarTotal += update[iBody].daDerivProc[iVar][iEqn];
                }
                // Prevent division by zero
                if (dVarNow != dVarTotal) {
                  dMinNow = fabs(dVarNow/((dVarNow - dVarTotal)/integr.dTimeStep));
                  if (dMinNow < dMin)
                    dMin = dMinNow;
                }
              }
          } else if (update[iBody].iaType[iVar][0] == 3) {
          /* The parameter does not require a derivative, but is calculated explicitly as a function of age.
             Also, is a sinusoidal quantity (e.g. h,k,p,q in DistOrb) */
            dVarNow = *update[iBody].pdVar[iVar];
            for (iEqn=0;iEqn<update[iBody].iNumEqns[iVar];iEqn++) {
              update[iBody].daDerivProc[iVar][iEqn] = fnUpdate[iBody][iVar][iEqn](body,system,update[iBody].iaBody[iVar][iEqn]);
            }
            if (control->Evolve.bFirstStep) {
              dMin = integr.dTimeStep;
              control->Evolve.bFirstStep = 0;
            } else {
              /* Sum over all equations giving new value of the variable */
              dVarTotal = 0.;
              for (iEqn=0;iEqn<update[iBody].iNumEqns[iVar];iEqn++) {
                dVarTotal += update[iBody].daDerivProc[iVar][iEqn];
              }
              // Prevent division by zero
              if (dVarNow != dVarTotal) {
                dMinNow = fabs(1.0/((dVarNow - dVarTotal)/integr.dTimeStep));
                if (dMinNow < dMin)
                  dMin = dMinNow;
              }
            }
          } else {  
            for (iEqn=0;iEqn<update[iBody].iNumEqns[iVar];iEqn++) {
              if (update[iBody].iaType[iVar][iEqn] == 2) {
                // The parameter is a "polar/sinusoidal quantity" controlled by a time derivative
                update[iBody].daDerivProc[iVar][iEqn] = fnUpdate[iBody][iVar][iEqn](body,system,update[iBody].iaBody[iVar][iEqn]);
                //if (update[iBody].daDerivProc[iVar][iEqn] != 0 && *(update[iBody].pdVar[iVar]) != 0) {
                if (update[iBody].daDerivProc[iVar][iEqn] != 0) {
                  dMinNow = fabs(1.0/update[iBody].daDerivProc[iVar][iEqn]);
                  if (dMinNow < dMin) 
                    dMin = dMinNow;
                }
              } else if (update[iBody].iaType[iVar][iEqn] == 4) {
                // unique type for ice sheets to prevent small amounts of ice -> dDt -> 0
                update[iBody].daDerivProc[iVar][iEqn] = fnUpdate[iBody][iVar][iEqn](body,system,update[iBody].iaBody[iVar][iEqn]);
                if (update[iBody].daDerivProc[iVar][iEqn] != 0 && iVar != update[iBody].iIceMass) {
                  dMinNow = fabs(pow(body[iBody].dRadius*2.0/body[iBody].iNumLats,2)/ \
                    (2*(body[iBody].daIceFlowMid[iVar-update[iBody].iIceMass+1]+\
                        body[iBody].daBasalFlowMid[iVar-update[iBody].iIceMass+1])));
                  if (dMinNow < dMin) {
                    if (dMinNow < 5*(2*PI/body[iBody].dMeanMotion)/control->Evolve.dEta) {
                        dMin = 5*(2*PI/body[iBody].dMeanMotion)/control->Evolve.dEta;
                    } else {
                        dMin = dMinNow;
                    }
                  }
                }
              } else if (update[iBody].iaType[iVar][iEqn] == 9) {
                // enforce a minimum step size for ice sheets, otherwise dDt -> 0 real fast
                update[iBody].daDerivProc[iVar][iEqn] = fnUpdate[iBody][iVar][iEqn](body,system,update[iBody].iaBody[iVar][iEqn]);
                if (update[iBody].daDerivProc[iVar][iEqn] != 0 && *(update[iBody].pdVar[iVar]) != 0) {
                  dMinNow = fabs((*(update[iBody].pdVar[iVar]))/update[iBody].daDerivProc[iVar][iEqn]);
                  if (dMinNow < dMin) {
                    if (dMinNow < 5*(2*PI/body[iBody].dMeanMotion)/control->Evolve.dEta) {
                      dMin = 5*(2*PI/body[iBody].dMeanMotion)/control->Evolve.dEta;
                    } else {
                      dMin = dMinNow;
                    }
                  }
                }
              } else {
                // The parameter is controlled by a time derivative
                update[iBody].daDerivProc[iVar][iEqn] = fnUpdate[iBody][iVar][iEqn](body,system,update[iBody].iaBody[iVar][iEqn]);
                if (update[iBody].daDerivProc[iVar][iEqn] != 0 && *(update[iBody].pdVar[iVar]) != 0) {
                  dMinNow = fabs((*(update[iBody].pdVar[iVar]))/update[iBody].daDerivProc[iVar][iEqn]);
                  if (dMinNow < dMin) 
                    dMin = dMinNow;
                }
              }
            }
          }
      }
    }
  }

  return dMin;
}
 
void EulerStep(BODY *body,CONTROL *control,SYSTEM *system,UPDATE *update,fnUpdateVariable ***fnUpdate,double *dDt,int iDir) {
  int iBody,iVar,iEqn;
  double dTimeOut;

  /* Adjust dt? */
  if (control->Evolve.bVarDt) {
    dTimeOut = fdNextOutput(control->Evolve.dTime,control->Io.dOutputTime);
    /* This is minimum dynamical timescale */
    *dDt = fdGetUpdateInfo(body,control,system,update,fnUpdate); 
    *dDt = AssignDt(*dDt,(dTimeOut - control->Evolve.dTime),control->Evolve.dEta);
  } 

  for (iBody=0;iBody<control->Evolve.iNumBodies;iBody++) {
    for (iVar=0;iVar<update[iBody].iNumVars;iVar++) {
      for (iEqn=0;iEqn<update[iBody].iNumEqns[iVar];iEqn++) {
        if (update[iBody].iaType[iVar][iEqn] == 0) 
          /* XXX This looks broken */
          *(update[iBody].pdVar[iVar]) = update[iBody].daDerivProc[iVar][iEqn];
        else {
          /* Update the parameter in the BODY struct! Be careful! */
          *(update[iBody].pdVar[iVar]) += iDir*update[iBody].daDerivProc[iVar][iEqn]*(*dDt);
        }
      }
    }
  }
}

void RungeKutta4Step(BODY *body,CONTROL *control,SYSTEM *system,UPDATE *update,fnUpdateVariable ***fnUpdate,double *dDt,int iDir) {
  int iBody,iVar,iEqn,iSubStep;
  double dTimeOut,dFoo,dDelta;

  /* Create a copy of BODY array */
  BodyCopy(control->Evolve.tmpBody,body,&control->Evolve);

  /* Derivatives at start */
  *dDt = fdGetUpdateInfo(body,control,system,control->Evolve.tmpUpdate,fnUpdate);

  /* Adjust dt? */
  if (control->Evolve.bVarDt) {
     dTimeOut = fdNextOutput(control->Evolve.dTime,control->Io.dOutputTime);
     /*  This is minimum dynamical timescale */
     *dDt = AssignDt(*dDt,(dTimeOut - control->Evolve.dTime),control->Evolve.dEta);
  } else
    *dDt = control->Evolve.dTimeStep;
    
  control->Evolve.dCurrentDt = *dDt;  
  /* XXX Should each eqn be updated separately? Each parameter at a 
     midpoint is moved by all the modules operating on it together.
     Does RK4 require the equations to be independent over the full step? */

  for (iBody=0;iBody<control->Evolve.iNumBodies;iBody++) {
    for (iVar=0;iVar<update[iBody].iNumVars;iVar++) {
      control->Evolve.daDeriv[0][iBody][iVar] = 0;
      for (iEqn=0;iEqn<update[iBody].iNumEqns[iVar];iEqn++) {
        // XXX Set update.dDxDtModule here?
        control->Evolve.daDeriv[0][iBody][iVar] += iDir*control->Evolve.tmpUpdate[iBody].daDerivProc[iVar][iEqn];
        //control->Evolve.daTmpVal[0][iBody][iVar] += (*dDt)*iDir*control->Evolve.tmpUpdate[iBody].daDeriv[iVar][iEqn];
      }
      
      if (update[iBody].iaType[iVar][0] == 0 || update[iBody].iaType[iVar][0] == 3 || update[iBody].iaType[iVar][0] == 10){
        // LUGER: Note that this is the VALUE of the variable getting passed, contrary to what the names suggest
        // These values are updated in the tmpUpdate struct so that equations which are dependent upon them will be 
        // evaluated with higher accuracy
        *(control->Evolve.tmpUpdate[iBody].pdVar[iVar]) = control->Evolve.daDeriv[0][iBody][iVar];      
      } else {       
        /* While we're in this loop, move each parameter to the midpoint of the timestep */
        *(control->Evolve.tmpUpdate[iBody].pdVar[iVar]) = *(update[iBody].pdVar[iVar]) + 0.5*(*dDt)*control->Evolve.daDeriv[0][iBody][iVar];
      }
    }
  }

  /* First midpoint derivative.*/
  PropertiesAuxiliary(control->Evolve.tmpBody,control,update);

  /* Don't need this timestep info, so assign output to dFoo */
  dFoo = fdGetUpdateInfo(control->Evolve.tmpBody,control,system,control->Evolve.tmpUpdate,fnUpdate);

  for (iBody=0;iBody<control->Evolve.iNumBodies;iBody++) {
    for (iVar=0;iVar<update[iBody].iNumVars;iVar++) {
      control->Evolve.daDeriv[1][iBody][iVar] = 0;
      for (iEqn=0;iEqn<update[iBody].iNumEqns[iVar];iEqn++) {
        control->Evolve.daDeriv[1][iBody][iVar] += iDir*control->Evolve.tmpUpdate[iBody].daDerivProc[iVar][iEqn];
      }
      
      if (update[iBody].iaType[iVar][0] == 0 || update[iBody].iaType[iVar][0] == 3 || update[iBody].iaType[iVar][0] == 10){
        // LUGER: Note that this is the VALUE of the variable getting passed, contrary to what the names suggest
        // These values are updated in the tmpUpdate struct so that equations which are dependent upon them will be 
        // evaluated with higher accuracy
        *(control->Evolve.tmpUpdate[iBody].pdVar[iVar]) = control->Evolve.daDeriv[1][iBody][iVar];
      } else {
        /* While we're in this loop, move each parameter to the midpoint 
        of the timestep based on the midpoint derivative. */
        *(control->Evolve.tmpUpdate[iBody].pdVar[iVar]) = *(update[iBody].pdVar[iVar]) + 0.5*(*dDt)*control->Evolve.daDeriv[1][iBody][iVar];
      }
    }
  }

  /* Second midpoint derivative */
  PropertiesAuxiliary(control->Evolve.tmpBody,control,update);
  dFoo = fdGetUpdateInfo(control->Evolve.tmpBody,control,system,control->Evolve.tmpUpdate,fnUpdate);

  for (iBody=0;iBody<control->Evolve.iNumBodies;iBody++) {
    for (iVar=0;iVar<update[iBody].iNumVars;iVar++) {
      control->Evolve.daDeriv[2][iBody][iVar] = 0;
      for (iEqn=0;iEqn<update[iBody].iNumEqns[iVar];iEqn++) {
        control->Evolve.daDeriv[2][iBody][iVar] += iDir*control->Evolve.tmpUpdate[iBody].daDerivProc[iVar][iEqn];
      }
      
      if (update[iBody].iaType[iVar][0] == 0 || update[iBody].iaType[iVar][0] == 3 || update[iBody].iaType[iVar][0] == 10){  
        // LUGER: Note that this is the VALUE of the variable getting passed, contrary to what the names suggest
        // These values are updated in the tmpUpdate struct so that equations which are dependent upon them will be 
        // evaluated with higher accuracy
        *(control->Evolve.tmpUpdate[iBody].pdVar[iVar]) = control->Evolve.daDeriv[2][iBody][iVar];
      } else {   
        /* While we're in this loop, move each parameter to the end of 
        the timestep based on the second midpoint derivative. */
        *(control->Evolve.tmpUpdate[iBody].pdVar[iVar]) = *(update[iBody].pdVar[iVar]) + *dDt*control->Evolve.daDeriv[2][iBody][iVar];
      }
    }
  }
  /* Full step derivative */
  PropertiesAuxiliary(control->Evolve.tmpBody,control,update);
  dFoo = fdGetUpdateInfo(control->Evolve.tmpBody,control,system,control->Evolve.tmpUpdate,fnUpdate);

  for (iBody=0;iBody<control->Evolve.iNumBodies;iBody++) {
    for (iVar=0;iVar<update[iBody].iNumVars;iVar++) {
      
      if (update[iBody].iaType[iVar][0] == 0 || update[iBody].iaType[iVar][0] == 3 || update[iBody].iaType[iVar][0] == 10){
        // NOTHING!
      } else {
        control->Evolve.daDeriv[3][iBody][iVar] = 0;
        for (iEqn=0;iEqn<update[iBody].iNumEqns[iVar];iEqn++) {
          control->Evolve.daDeriv[3][iBody][iVar] += iDir*control->Evolve.tmpUpdate[iBody].daDerivProc[iVar][iEqn];
        }
      }
    }
  }
  /* Now do the update -- Note the pointer to the home of the actual variables!!! */
  for (iBody=0;iBody<control->Evolve.iNumBodies;iBody++) {
    for (iVar=0;iVar<update[iBody].iNumVars;iVar++) {
      update[iBody].daDeriv[iVar] = 1./6*(control->Evolve.daDeriv[0][iBody][iVar] + 2*control->Evolve.daDeriv[1][iBody][iVar] + 
      2*control->Evolve.daDeriv[2][iBody][iVar] + control->Evolve.daDeriv[3][iBody][iVar]);
      
      if (update[iBody].iaType[iVar][0] == 0 || update[iBody].iaType[iVar][0] == 3 || update[iBody].iaType[iVar][0] == 10){
        // LUGER: Note that this is the VALUE of the variable getting passed, contrary to what the names suggest
        *(update[iBody].pdVar[iVar]) = control->Evolve.daDeriv[0][iBody][iVar];
      } else {
        *(update[iBody].pdVar[iVar]) += update[iBody].daDeriv[iVar]*(*dDt);
      }
    }
  }
}

/*
 * Evolution Subroutine
 */

void Evolve(BODY *body,CONTROL *control,FILES *files,OUTPUT *output,SYSTEM *system,UPDATE *update,fnUpdateVariable ***fnUpdate,fnWriteOutput *fnWrite,fnIntegrate fnOneStep) {
  int iDir,iBody,iModule;
  double dTimeOut;
  double dDt,dFoo;
  double dEqSpinRate;
  
  control->Evolve.nSteps=0;

  if (control->Evolve.bDoForward)
    iDir=1;
  else
    iDir=-1;

  dTimeOut = fdNextOutput(control->Evolve.dTime,control->Io.dOutputTime);

  PropertiesAuxiliary(body,control,update);
  
  /* Adjust dt? */
  if (control->Evolve.bVarDt) {
    /* This is minimum dynamical timescale */
    dDt = fdGetUpdateInfo(body,control,system,update,fnUpdate); 
    /* Now get time to next output */
    dTimeOut = fdNextOutput(control->Evolve.dTime,control->Io.dOutputTime);
    /* Now choose the correct timestep */
    dDt = AssignDt(dDt,(dTimeOut - control->Evolve.dTime),control->Evolve.dEta);
  } else
    dDt = control->Evolve.dTimeStep;

  /* Write out initial conditions */

  WriteOutput(body,control,files,output,system,update,fnWrite,control->Evolve.dTime,dDt);

  /* If Runge-Kutta need to copy actual update to that in 
     control->Evolve. This transfer all the meta-data about the 
     struct. */
  UpdateCopy(control->Evolve.tmpUpdate,update,control->Evolve.iNumBodies);

  /* 
   *
   * Main loop begins here 
   *
   */

  while (control->Evolve.dTime < control->Evolve.dStopTime) {
    /* Take one step */
    fnOneStep(body,control,system,update,fnUpdate,&dDt,iDir);
    for (iBody=0;iBody<control->Evolve.iNumBodies;iBody++) {
      for (iModule=0;iModule<control->Evolve.iNumModules[iBody];iModule++)
        control->fnForceBehavior[iBody][iModule](body,&control->Evolve,&control->Io,system,update,iBody,iModule);

      for (iModule=0;iModule<control->iNumMultiForce[iBody];iModule++)
        control->fnForceBehaviorMulti[iBody][iModule](body,&control->Evolve,&control->Io,system,update,iModule,iBody);
    }

    /* Halt? */
    if (fbCheckHalt(body,control,update)) {
      /* Use dummy variable as dDt is used for the integration.
       * Here we just want the instantaneous derivatives.
       * This should make the output self-consistent.
       */
      dFoo = fdGetUpdateInfo(body,control,system,update,fnUpdate); 
      WriteOutput(body,control,files,output,system,update,fnWrite,control->Evolve.dTime,control->Io.dOutputTime/control->Evolve.nSteps);
      return;
    }

    for (iBody=0;iBody<control->Evolve.iNumBodies;iBody++)
      body[iBody].dAge += iDir*dDt;

    control->Evolve.dTime += dDt;
    control->Evolve.nSteps++;

    // if (control->Evolve.dTime >= 360*YEARSEC) {
//       printf("stop\n");
//     }
        
    /* Time for Output? */
    if (control->Evolve.dTime >= dTimeOut) {
      dFoo = fdGetUpdateInfo(body,control,system,update,fnUpdate); 
      WriteOutput(body,control,files,output,system,update,fnWrite,control->Evolve.dTime,control->Io.dOutputTime/control->Evolve.nSteps);
      dTimeOut = fdNextOutput(control->Evolve.dTime,control->Io.dOutputTime);
      control->Evolve.nSteps=0;
    }

    /* Get auxiliary properties for next step -- first call 
       was prior to loop. */
    PropertiesAuxiliary(body,control,update);

  }

  if (control->Io.iVerbose >= VERBPROG)
    printf("Evolution completed.\n");
}



