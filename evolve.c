#include <stdio.h>
#include <math.h>
#include <assert.h>
#include <stdlib.h>
#include "vplanet.h"

void InitializeVPLANET(CONTROL *control,UPDATE *update,VPLANET *vplanet) {
  int iSubStep,iBody;

  vplanet->tmpBody = malloc(control->iNumBodies*sizeof(BODY));
  vplanet->tmpUpdate = malloc(control->iNumBodies*sizeof(UPDATE));

  /* Currently this only matters for RK4 integration. This should
     be generalized for any integration method. */
  vplanet->dUpdate = malloc(4*sizeof(double**));
  for (iSubStep=0;iSubStep<4;iSubStep++) {
    vplanet->dUpdate[iSubStep] = malloc(control->iNumBodies*sizeof(double*));
    for (iBody=0;iBody<control->iNumBodies;iBody++) 
      vplanet->dUpdate[iSubStep][iBody]=malloc(update[iBody].iNum*sizeof(double));
  }

  /* Needs to be vectorized VPL */
  InitializeVplanetEqtide(control,vplanet);
}

void fvPropertiesAuxiliary(CONTROL *control,BODY *body) {

  /* Needs to be vectorized VPL */
  fvPropertiesEqtide(control,body);
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

double fdGetUpdateInfo(CONTROL *control,BODY *body,UPDATE *update,fnUpdateVariable ***fnUpdate) {
  int iBody,iVar,iEqn;
  INTEGR integr;
  double dVarNow,dMinNow,dMin=HUGE;

  integr = control->Integr;

  for (iBody=0;iBody<control->iNumBodies;iBody++) {
    if (update[iBody].iNum > 0) {
      for (iVar=0;iVar<update[iBody].iNum;iVar++) {
	/* XXX This looks broken -- should it also include [iEqn]? But can this be dumped?? */
	if (update[iBody].iType[iVar] == 0) {
	  dVarNow=*update[iBody].pdVar[iVar];
	  update[iBody].dDeriv[iVar][0] = fnUpdate[iBody][iVar][0](body,iBody);
	  dMinNow = fabs(dVarNow - update[iBody].dDeriv[iVar][0])/integr.dTimeStep;
 	  if (dMinNow < dMin)
	    dMin = dMinNow;
	} else {
	  for (iEqn=0;iEqn<update[iBody].iNumEqns[iVar];iEqn++) {
	    update[iBody].dDeriv[iVar][iEqn] = fnUpdate[iBody][iVar][iEqn](body,iBody);
	    if (update[iBody].dDeriv[iVar][iEqn] != 0) {
	      dMinNow = fabs((*(update[iBody].pdVar[iVar]))/update[iBody].dDeriv[iVar][iEqn]);
	      if (dMinNow < dMin) 
		dMin = dMinNow;
	    }
	  }
	}
      }
    }
  }

  return dMin;
}
 
void EulerStep(CONTROL *control,BODY *body,UPDATE *update,VPLANET *vplanet,fnUpdateVariable ***fnUpdate,int iDir,double *dDt) {
  int iBody,iVar,iEqn;
  double dTimeOut;

  fdGetUpdateInfo(control,body,update,fnUpdate);

  /* Adjust dt? */
  if (control->Integr.bVarDt) {
    dTimeOut = fdNextOutput(control->Integr.dTime,control->dOutputTime);
    /* This is minimum dynamical timescale */
    *dDt = fdGetUpdateInfo(control,body,update,fnUpdate); 
    *dDt = AssignDt(*dDt,(dTimeOut - control->Integr.dTime),control->Integr.dEta);
  } 

  for (iBody=0;iBody<control->iNumBodies;iBody++) {
    for (iVar=0;iVar<update[iBody].iNum;iVar++) {
      if (update[iBody].iType == 0) 
	*(update[iBody].pdVar[iVar]) = update[iBody].dDeriv[iVar][0];
      else {
	for (iEqn=0;iEqn<update[iBody].iNumEqns[iVar];iEqn++)
	  /* Update the parameter in the BODY struct! Be careful! */
	  *(update[iBody].pdVar[iVar]) += iDir*update[iBody].dDeriv[iVar][iEqn]*(*dDt);
      }
    }
  }
}

void RungeKutta4Step(CONTROL *control,BODY *body,UPDATE *update,VPLANET *vplanet,fnUpdateVariable ***fnUpdate,int iDir,double *dDt) {
  int iBody,iVar,iEqn,iSubStep;
  double dTimeOut,dFoo,dDelta;
  
  /* Create a copy of BODY array */
  fvBodyCopy(body,vplanet->tmpBody,control);

  /* Derivatives at start */
  *dDt = fdGetUpdateInfo(control,body,vplanet->tmpUpdate,fnUpdate);

  /* Adjust dt? */
  if (control->Integr.bVarDt) {
     dTimeOut = fdNextOutput(control->Integr.dTime,control->dOutputTime);
     /*  This is minimum dynamical timescale */
    *dDt = AssignDt(*dDt,(dTimeOut - control->Integr.dTime),control->Integr.dEta);
  } else
    *dDt = control->Integr.dTimeStep;
    
  for (iBody=0;iBody<control->iNumBodies;iBody++) {
    for (iVar=0;iVar<update[iBody].iNum;iVar++) {
      vplanet->dUpdate[0][iBody][iVar] = 0;
      for (iEqn=0;iEqn<update[iBody].iNumEqns[iVar];iEqn++) {
	vplanet->dUpdate[0][iBody][iVar] += (*dDt)*iDir*vplanet->tmpUpdate[iBody].dDeriv[iVar][iEqn];
      }
      /* This points to the correct parameter in tmpBody */
      *(vplanet->tmpUpdate[iBody].pdVar[iVar]) = *(update[iBody].pdVar[iVar]) + vplanet->dUpdate[0][iBody][iVar];
    }
  }

  /* First midpoint derivative */
  fvPropertiesAuxiliary(control,vplanet->tmpBody);
  /* Don't need this timestep info, so assign output to dFoo */
  dFoo = fdGetUpdateInfo(control,vplanet->tmpBody,vplanet->tmpUpdate,fnUpdate);

  for (iBody=0;iBody<control->iNumBodies;iBody++) {
    for (iVar=0;iVar<update[iBody].iNum;iVar++) {
      vplanet->dUpdate[1][iBody][iVar] = 0;
      for (iEqn=0;iEqn<update[iBody].iNumEqns[iVar];iEqn++) {
	vplanet->dUpdate[1][iBody][iVar] += (*dDt)*iDir*vplanet->tmpUpdate[iBody].dDeriv[iVar][iEqn];
      }
      *(vplanet->tmpUpdate[iBody].pdVar[iVar]) = *(update[iBody].pdVar[iVar]) + 0.5*vplanet->dUpdate[1][iBody][iVar];
    }
  }

  /* Second midpoint derivative */
  fvPropertiesAuxiliary(control,vplanet->tmpBody);
  dFoo = fdGetUpdateInfo(control,vplanet->tmpBody,vplanet->tmpUpdate,fnUpdate);

  for (iBody=0;iBody<control->iNumBodies;iBody++) {
    for (iVar=0;iVar<update[iBody].iNum;iVar++) {
      vplanet->dUpdate[2][iBody][iVar] = 0;
      for (iEqn=0;iEqn<update[iBody].iNumEqns[iVar];iEqn++) {
	vplanet->dUpdate[2][iBody][iVar] += (*dDt)*iDir*vplanet->tmpUpdate[iBody].dDeriv[iVar][iEqn];
      }
      *(vplanet->tmpUpdate[iBody].pdVar[iVar]) = *(update[iBody].pdVar[iVar]) + vplanet->dUpdate[2][iBody][iVar];
    }
  }

  /* Full step derivative */
  fvPropertiesAuxiliary(control,vplanet->tmpBody);
  dFoo = fdGetUpdateInfo(control,vplanet->tmpBody,vplanet->tmpUpdate,fnUpdate);

  for (iBody=0;iBody<control->iNumBodies;iBody++) {
    for (iVar=0;iVar<update[iBody].iNum;iVar++) {
      vplanet->dUpdate[3][iBody][iVar] = 0;
      for (iEqn=0;iEqn<update[iBody].iNumEqns[iVar];iEqn++) {
	vplanet->dUpdate[3][iBody][iVar] += (*dDt)*iDir*vplanet->tmpUpdate[iBody].dDeriv[iVar][iEqn];
      }
    }
  }

  /* Now do the update -- Note the pointer to the home of the actual variables!!! */
  for (iBody=0;iBody<control->iNumBodies;iBody++) {
    for (iVar=0;iVar<update[iBody].iNum;iVar++) {
      dDelta= 1./6*(vplanet->dUpdate[0][iBody][iVar] + 2*vplanet->dUpdate[1][iBody][iVar] + 2*vplanet->dUpdate[2][iBody][iVar] + vplanet->dUpdate[3][iBody][iVar]);
      *(update[iBody].pdVar[iVar]) += dDelta;
      /* Now store actual derivative */
      update[iBody].dDeriv[iVar][0] = dDelta/(*dDt);
    }
  }
}

/*
 * Evoltuion Subroutine
 */

void Evolve(CONTROL *control,BODY *body,SYSTEM *system,OUTPUT *output,FILES *files,UPDATE *update,fnUpdateVariable ***fnUpdate,VPLANET *vplanet,fnWriteOutput *fnWrite,fnIntegrate fnOneStep) {
 
  int i,iDir;
  double dTimeOut;
  double dDt,dFoo;
  double dEqSpinRate;

  control->Integr.nSteps=0;

  if (control->Integr.bDoForward)
    iDir=1;
  else
    iDir=-1;

  dTimeOut = fdNextOutput(control->Integr.dTime,control->dOutputTime);

  fvPropertiesAuxiliary(control,body);

  /* Adjust dt? */
  if (control->Integr.bVarDt) {
    /* This is minimum dynamical timescale */
    dDt = fdGetUpdateInfo(control,body,update,fnUpdate); 
    /* Now get time to next output */
    dTimeOut = fdNextOutput(control->Integr.dTime,control->dOutputTime);
    /* Now choose the correct timestep */
    dDt = AssignDt(dDt,(dTimeOut - control->Integr.dTime),control->Integr.dEta);
  } else
    dDt = control->Integr.dTimeStep;

  /* Write out initial conditions */

  WriteOutput(control,output,body,system,files,fnWrite,control->Integr.dTime,dDt,update);

  /* 
   *
   * Main loop begins here 
   *
   */

  while (control->Integr.dTime < control->Integr.dStopTime) {
    /* Manually adjust variables */
    /* Must be vectorized VPL */ 
    ForceBehaviorEqtide(body,control);

    /* Take one step */
    fnOneStep(control,body,update,vplanet,fnUpdate,iDir,&dDt);

    /* Halt? */
    if (control->Halt.bHalt) {
      if (bCheckHalt(control,vplanet,body,update,control->Integr.dTime)) {
	/* Use dummy variable as dDt is used for the integration.
	 * Here we just want the instantaneous derivatives.
	 * This should make the output self-consistent.
	 */
	dFoo = fdGetUpdateInfo(control,body,update,fnUpdate); 
	WriteOutput(control,output,body,system,files,fnWrite,control->Integr.dTime,control->dOutputTime/control->Integr.nSteps,update);
        return;
      }
    }

    control->Integr.dTime += dDt;
    system->dAge += iDir*dDt;
    control->Integr.nSteps++;

    /* Time for Output? */
    if (control->Integr.dTime >= dTimeOut) {
      dFoo = fdGetUpdateInfo(control,body,update,fnUpdate); 
      WriteOutput(control,output,body,system,files,fnWrite,control->Integr.dTime,control->dOutputTime/control->Integr.nSteps,update);
      dTimeOut = fdNextOutput(control->Integr.dTime,control->dOutputTime);
      control->Integr.nSteps=0;
    }

    /* Get tidal properties for next step -- first call 
       was prior to loop. */
    fvPropertiesAuxiliary(control,body);

  }

  if (control->iVerbose >= VERBPROG)
    printf("Evolution completed.\n");

}



