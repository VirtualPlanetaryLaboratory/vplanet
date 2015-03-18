#include <stdio.h>
#include <math.h>
#include <assert.h>
#include <stdlib.h>
#include "vplanet.h"

void PropsAuxGeneral(BODY *body,CONTROL *control) {
  int iBody;

  for (iBody=0;iBody<control->Evolve.iNumBodies;iBody++) {
    if (iBody != 0) {
      body[iBody].dMeanMotion = fdSemiToMeanMotion(body[iBody].dSemi,(body[0].dMass+body[iBody].dMass));
    }

    /* If small enough, set some quantities to zero */
    /* Generalize! fnPropsAuxMinMax? */
    if (control->Evolve.dMinValue > 0) {
      if (body[iBody].dEcc < control->Evolve.dMinValue)
	body[iBody].dEcc = 0;
      if (body[iBody].dObliquity < control->Evolve.dMinValue)
	body[iBody].dObliquity = 0;
    }
  }
}

void PropertiesAuxiliary(BODY *body,CONTROL *control) {
  int iBody,iModule;

  PropsAuxGeneral(body,control);

  /* Get properties from each module */
  for (iBody=0;iBody<control->Evolve.iNumBodies;iBody++) {
    for (iModule=0;iModule<control->Evolve.iNumModules[iBody];iModule++)
      control->Evolve.fnAuxProps[iBody][iModule](body,iBody);
  }
}

void UpdateTmpBody(BODY *tmpBody,CONTROL *control,UPDATE *update) {
  int iBody;

  for (iBody=0;iBody<control->Evolve.iNumBodies;iBody++) {
    /* XXX Only update active variables? */
    tmpBody[iBody].dSemi=*(control->Evolve.tmpUpdate[iBody].pdVar[update[iBody].iSemi]);
    tmpBody[iBody].dEcc=*(control->Evolve.tmpUpdate[iBody].pdVar[update[iBody].iEcc]);
    tmpBody[iBody].d40KNum=*(control->Evolve.tmpUpdate[iBody].pdVar[update[iBody].iNum40K]);
    tmpBody[iBody].d232ThNum=*(control->Evolve.tmpUpdate[iBody].pdVar[update[iBody].iNum232Th]);
    tmpBody[iBody].d238UNum=*(control->Evolve.tmpUpdate[iBody].pdVar[update[iBody].iNum238U]);
    tmpBody[iBody].dRotRate=*(control->Evolve.tmpUpdate[iBody].pdVar[update[iBody].iRot]);
    tmpBody[iBody].dObliquity=*(control->Evolve.tmpUpdate[iBody].pdVar[update[iBody].iObl]);
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
  double dVarNow,dMinNow,dMin=HUGE;

  integr = control->Evolve;

  // XXXX Change Eqn to Proc?

  for (iBody=0;iBody<control->Evolve.iNumBodies;iBody++) {
    if (update[iBody].iNumVars > 0) {
      for (iVar=0;iVar<update[iBody].iNumVars;iVar++) {
	for (iEqn=0;iEqn<update[iBody].iNumEqns[iVar];iEqn++) {
	  if (update[iBody].iaType[iVar][iEqn] == 0) {
	    /* The parameter does not require a derivative, but is 
	       calculated explicitly as a function of age. */
	    dVarNow=*update[iBody].pdVar[iVar];
	    update[iBody].daDerivProc[iVar][iEqn] = fnUpdate[iBody][iVar][iEqn](body,system,update[iBody].iaBody[iVar][iEqn],update[iBody].iNumBodies[iVar][iEqn]);
	    
	    if (control->Evolve.bFirstStep) {
	      dMin = integr.dTimeStep;
	      control->Evolve.bFirstStep = 0;
	    } else {
	      dMinNow = dVarNow/(fabs(dVarNow - update[iBody].daDerivProc[iVar][iEqn])/integr.dTimeStep);
	      if (dMinNow < dMin)
		dMin = dMinNow;
	    }
	  } else {
	    // The parameter is controlled by a time derivative
	    for (iEqn=0;iEqn<update[iBody].iNumEqns[iVar];iEqn++) {
	      update[iBody].daDerivProc[iVar][iEqn] = fnUpdate[iBody][iVar][iEqn](body,system,update[iBody].iaBody[iVar][iEqn],update[iBody].iNumBodies[iVar][iEqn]);
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
 
void EulerStep(BODY *body,CONTROL *control,SYSTEM *system,UPDATE *update,fnUpdateVariable ***fnUpdate,int iDir,double *dDt) {
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

void RungeKutta4Step(BODY *body,CONTROL *control,SYSTEM *system,UPDATE *update,fnUpdateVariable ***fnUpdate,int iDir,double *dDt) {
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
      /* While we're in this loop, move each parameter to the midpoint 
	 of the timestep */
      *(control->Evolve.tmpUpdate[iBody].pdVar[iVar]) = *(update[iBody].pdVar[iVar]) + 0.5*(*dDt)*control->Evolve.daDeriv[0][iBody][iVar];
    }
  }

  /* First midpoint derivative.*/
  /* Now copy new parameters into control->Evolve.tmpBody, and 
     calculate new auxiliary properties. XXX Unnecessary? 
     UpdateTmpBody(control->Evolve.tmpBody,control,update); */
  PropertiesAuxiliary(control->Evolve.tmpBody,control);

  /* Don't need this timestep info, so assign output to dFoo */
  dFoo = fdGetUpdateInfo(control->Evolve.tmpBody,control,system,control->Evolve.tmpUpdate,fnUpdate);

  for (iBody=0;iBody<control->Evolve.iNumBodies;iBody++) {
    for (iVar=0;iVar<update[iBody].iNumVars;iVar++) {
      control->Evolve.daDeriv[1][iBody][iVar] = 0;
      for (iEqn=0;iEqn<update[iBody].iNumEqns[iVar];iEqn++) {
	control->Evolve.daDeriv[1][iBody][iVar] += iDir*control->Evolve.tmpUpdate[iBody].daDerivProc[iVar][iEqn];
      }
      /* While we're in this loop, move each parameter to the midpoint 
	 of the timestep based on the midpoint derivative. */
      *(control->Evolve.tmpUpdate[iBody].pdVar[iVar]) = *(update[iBody].pdVar[iVar]) + 0.5*(*dDt)*control->Evolve.daDeriv[1][iBody][iVar];
    }
  }

  /* Second midpoint derivative */
  // UpdateTmpBody(control->Evolve.tmpBody,control,update);
  PropertiesAuxiliary(control->Evolve.tmpBody,control);
  dFoo = fdGetUpdateInfo(control->Evolve.tmpBody,control,system,control->Evolve.tmpUpdate,fnUpdate);

  for (iBody=0;iBody<control->Evolve.iNumBodies;iBody++) {
    for (iVar=0;iVar<update[iBody].iNumVars;iVar++) {
      control->Evolve.daDeriv[2][iBody][iVar] = 0;
      for (iEqn=0;iEqn<update[iBody].iNumEqns[iVar];iEqn++) {
	control->Evolve.daDeriv[2][iBody][iVar] += iDir*control->Evolve.tmpUpdate[iBody].daDerivProc[iVar][iEqn];
      }
      /* While we're in this loop, move each parameter to the end of 
	 the timestep based on the second midpoint derivative. */
      *(control->Evolve.tmpUpdate[iBody].pdVar[iVar]) = *(update[iBody].pdVar[iVar]) + *dDt*control->Evolve.daDeriv[2][iBody][iVar];
    }
  }

  /* Full step derivative */
  // UpdateTmpBody(control->Evolve.tmpBody,control,update);
  PropertiesAuxiliary(control->Evolve.tmpBody,control);
  dFoo = fdGetUpdateInfo(control->Evolve.tmpBody,control,system,control->Evolve.tmpUpdate,fnUpdate);

  for (iBody=0;iBody<control->Evolve.iNumBodies;iBody++) {
    for (iVar=0;iVar<update[iBody].iNumVars;iVar++) {
      control->Evolve.daDeriv[3][iBody][iVar] = 0;
      for (iEqn=0;iEqn<update[iBody].iNumEqns[iVar];iEqn++) {
	control->Evolve.daDeriv[3][iBody][iVar] += iDir*control->Evolve.tmpUpdate[iBody].daDerivProc[iVar][iEqn];
      }
    }
  }

  /* Now do the update -- Note the pointer to the home of the actual variables!!! */
  for (iBody=0;iBody<control->Evolve.iNumBodies;iBody++) {
    for (iVar=0;iVar<update[iBody].iNumVars;iVar++) {
      update[iBody].daDeriv[iVar] = 1./6*(control->Evolve.daDeriv[0][iBody][iVar] + 2*control->Evolve.daDeriv[1][iBody][iVar] + 
      2*control->Evolve.daDeriv[2][iBody][iVar] + control->Evolve.daDeriv[3][iBody][iVar]);

      *(update[iBody].pdVar[iVar]) += update[iBody].daDeriv[iVar]*(*dDt);
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

  PropertiesAuxiliary(body,control);

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
    fnOneStep(body,control,system,update,fnUpdate,iDir,&dDt);

    /* Manually adjust variables for each module*/
    for (iBody=0;iBody<control->Evolve.iNumBodies;iBody++) {
      for (iModule=0;iModule<control->Evolve.iNumModules[iBody];iModule++)
	control->fnForceBehavior[iBody][iModule](body,&control->Evolve,&control->Io,iBody,iModule);
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

    control->Evolve.dTime += dDt;
    system->dAge += iDir*dDt;
    control->Evolve.nSteps++;

    /* Time for Output? */
    if (control->Evolve.dTime >= dTimeOut) {
      dFoo = fdGetUpdateInfo(body,control,system,update,fnUpdate); 
      WriteOutput(body,control,files,output,system,update,fnWrite,control->Evolve.dTime,control->Io.dOutputTime/control->Evolve.nSteps);
      dTimeOut = fdNextOutput(control->Evolve.dTime,control->Io.dOutputTime);
      control->Evolve.nSteps=0;
    }

    /* Get tidal properties for next step -- first call 
       was prior to loop. */
    PropertiesAuxiliary(body,control);

  }

  if (control->Io.iVerbose >= VERBPROG)
    printf("Evolution completed.\n");
}



