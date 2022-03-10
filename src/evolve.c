/**
  @file evolve.c

  @brief This file contains all the core VPLANET integration routines including
  the timestepping algorithm and the Runge-Kutta Integration scheme.

  @author Rory Barnes ([RoryBarnes](https://github.com/RoryBarnes/))

  @date May 2014

*/
#define NUM_THREADS 4

#include "vplanet.h"

void PropsAuxGeneral(BODY *body, CONTROL *control) {
  /* Recompute the mean motion, necessary for most modules */
  int iBody; // Dummy counting variable

  for (iBody = 0; iBody < control->Evolve.iNumBodies; iBody++) {
    if (iBody != 0 && body[iBody].bBinary == 0) {
      body[iBody].dMeanMotion = fdSemiToMeanMotion(
            body[iBody].dSemi, (body[0].dMass + body[iBody].dMass));
    }
  }
}

void PropertiesAuxiliary(BODY *body, CONTROL *control, SYSTEM *system,
                         UPDATE *update) {
  /* Evaluate single and multi-module auxialliary functions to update parameters
   * of interest such as mean motion.
   */
  int iBody, iModule; // Dummy counter variables

  PropsAuxGeneral(body, control);

  /* Get properties from each module */
  for (iBody = 0; iBody < control->Evolve.iNumBodies; iBody++) {
    // Uni-module properties
    for (iModule = 0; iModule < control->Evolve.iNumModules[iBody]; iModule++) {
      control->fnPropsAux[iBody][iModule](body, &control->Evolve, &control->Io,
                                          update, iBody);
    }

    // Multi-module properties
    for (iModule = 0; iModule < control->iNumMultiProps[iBody]; iModule++) {
      control->fnPropsAuxMulti[iBody][iModule](body, &control->Evolve,
                                               &control->Io, update, iBody);
    }
  }
}

void CalculateDerivatives(BODY *body, SYSTEM *system, UPDATE *update,
                          fnUpdateVariable ***fnUpdate, int iNumBodies) {
  int iBody, iVar, iEqn;

  for (iBody = 0; iBody < iNumBodies; iBody++) {
    for (iVar = 0; iVar < update[iBody].iNumVars; iVar++) {
      update[iBody].daDeriv[iVar] = 0;
      for (iEqn = 0; iEqn < update[iBody].iNumEqns[iVar]; iEqn++) {
        update[iBody].daDerivProc[iVar][iEqn] = fnUpdate[iBody][iVar][iEqn](
              body, system, update[iBody].iaBody[iVar][iEqn]);
        update[iBody].daDeriv[iVar] += update[iBody].daDerivProc[iVar][iEqn];
      }
    }
  }
  iBody = 0;
}

void CheckProgress(BODY *body, CONTROL *control, SYSTEM *system,
                   UPDATE *update) {
  int iBody, jBody;

  if (control->Io.iVerbose >= VERBPROG && !control->Io.bMutualIncMessage &&
      control->Io.dMaxMutualInc > 0) {

    // If made it here, more than 1 body must be present
    if (body[1].bSpiNBody) {
      // Calculate orbital elements
      for (iBody = 0; iBody < control->Evolve.iNumBodies; iBody++) {
        cart2osc(body, iBody);
      }
    }

    // Skip central body
    for (iBody = 1; iBody < control->Evolve.iNumBodies; iBody++) {
      for (jBody = iBody + 1; jBody < control->Evolve.iNumBodies; jBody++) {
        // 1 to check progress, not halt
        if (fbCheckMaxMutualInc(body, &control->Evolve, control->Halt,
                                &control->Io, iBody, jBody, 1)) {
          /*
        if (control->Io.iVerbose >= VERBPROG) {
          printf("WARNING: Mutual inclination of %s and %s exceeds ",
              body[iBody].cName,body[jBody].cName);
          fprintd(stdout,control->Io.dMaxMutualInc,control->Io.iSciNot,
              control->Io.iDigits);
          printf(" at t = %.2e years.\n",control->Evolve.dTime);
        }
*/
          control->Io.bMutualIncMessage = 1;
        }
      }
    }
  }
}

/*
 * Integration Control
 */

double AssignDt(double dMin, double dNextOutput, double dEta) {
  /* Compute the next timestep, dt, making sure it's not larger than the output
   * cadence */

  dMin = dEta * dMin;
  if (dNextOutput < dMin) {
    dMin = dNextOutput;
  }
  return dMin;
}

double fdGetTimeStep(BODY *body, CONTROL *control, SYSTEM *system,
                     UPDATE *update, fnUpdateVariable ***fnUpdate) {
  /* Fills the Update arrays with the derivatives
   * or new values. It returns the smallest timescale for use
   * in variable timestepping. Uses either a 4th order Runge-Kutte integrator or
   * an Euler step.
   */

  int iBody, iVar, iEqn; // Dummy counting variables
  EVOLVE
  integr; // Dummy EVOLVE struct so we don't have to dereference control a lot
  double dVarNow, dMinNow, dMin = dHUGE,
                           dVarTotal; // Intermediate storage variables

  integr = control->Evolve;

  dMin = dHUGE;

  for (iBody = 0; iBody < control->Evolve.iNumBodies; iBody++) {
    if (update[iBody].iNumVars > 0) {
      for (iVar = 0; iVar < update[iBody].iNumVars; iVar++) {

        // The parameter does not require a derivative, but is calculated
        // explicitly as a function of age.
        /*
        printf("%d %d\n",iBody,iVar);
        fflush(stdout);
        */
        if (update[iBody].iaType[iVar][0] == 0) {
          dVarNow = *update[iBody].pdVar[iVar];
          for (iEqn = 0; iEqn < update[iBody].iNumEqns[iVar]; iEqn++) {
            update[iBody].daDerivProc[iVar][iEqn] = fnUpdate[iBody][iVar][iEqn](
                  body, system, update[iBody].iaBody[iVar][iEqn]);
          }
          if (control->Evolve.bFirstStep) {
            dMin                       = integr.dTimeStep;
            control->Evolve.bFirstStep = 0;
          } else {
            /* Sum over all equations giving new value of the variable */
            dVarTotal = 0.;
            for (iEqn = 0; iEqn < update[iBody].iNumEqns[iVar]; iEqn++) {
              dVarTotal += update[iBody].daDerivProc[iVar][iEqn];
            }
            // Prevent division by zero
            if (dVarNow != dVarTotal) {
              dMinNow =
                    fabs(dVarNow / ((dVarNow - dVarTotal) / integr.dTimeStep));
              if (dMinNow < dMin) {
                dMin = dMinNow;
              }
            }
          }
          /* Equations that are integrated in the matrix but are NOT allowed to
           dictate timestepping.  These are derived quantities, like lost
           energy, that must be integrated as primary variables to keep track of
           them properly, i.e. lost energy depends on changing radii, which are
           integrated.  But in this case, since they are derived quantities,
           they should NOT participate in timestep selection - dflemin3
         */
        } else if (update[iBody].iaType[iVar][0] == 5) {
          // continue;
          for (iEqn = 0; iEqn < update[iBody].iNumEqns[iVar]; iEqn++) {
            update[iBody].daDerivProc[iVar][iEqn] = fnUpdate[iBody][iVar][iEqn](
                  body, system, update[iBody].iaBody[iVar][iEqn]);
          }
          /* Integration for binary, where parameters can be computed via
         derivatives, or as an explicit function of age */
        } else if (update[iBody].iaType[iVar][0] == 10) {
          /* Equations not in matrix, computing things as explicit function of
           time, so we set dMin to time until next output
           Figure out time until next output */
          dMinNow = control->Io.dNextOutput;
          if (dMinNow < dMin) {
            dMin = dMinNow;
          }
          /* The parameter does not require a derivative, but is calculated
            explicitly as a function of age and is a sinusoidal quantity
            (e.g. h,k,p,q in DistOrb) */
        } else if (update[iBody].iaType[iVar][0] == 3) {
          dVarNow = *update[iBody].pdVar[iVar];
          for (iEqn = 0; iEqn < update[iBody].iNumEqns[iVar]; iEqn++) {
            update[iBody].daDerivProc[iVar][iEqn] = fnUpdate[iBody][iVar][iEqn](
                  body, system, update[iBody].iaBody[iVar][iEqn]);
          }
          if (control->Evolve.bFirstStep) {
            dMin                       = integr.dTimeStep;
            control->Evolve.bFirstStep = 0;
          } else {
            /* Sum over all equations giving new value of the variable */
            dVarTotal = 0.;
            for (iEqn = 0; iEqn < update[iBody].iNumEqns[iVar]; iEqn++) {
              dVarTotal += update[iBody].daDerivProc[iVar][iEqn];
            }
            // Prevent division by zero
            if (dVarNow != dVarTotal) {
              dMinNow = fabs(1.0 / ((dVarNow - dVarTotal) / integr.dTimeStep));
              if (dMinNow < dMin) {
                dMin = dMinNow;
              }
            }
          }
          /* The parameter is a "polar/sinusoidal quantity" and
            controlled by a time derivative */
        } else {
          for (iEqn = 0; iEqn < update[iBody].iNumEqns[iVar]; iEqn++) {
            if (update[iBody].iaType[iVar][iEqn] == 2) {
              update[iBody].daDerivProc[iVar][iEqn] =
                    fnUpdate[iBody][iVar][iEqn](
                          body, system, update[iBody].iaBody[iVar][iEqn]);
              // if (update[iBody].daDerivProc[iVar][iEqn] != 0 &&
              // *(update[iBody].pdVar[iVar]) != 0) {
              if (update[iBody].daDerivProc[iVar][iEqn] != 0) {
                /* ?Obl require special treatment because they can
                    overconstrain obliquity and PrecA */
                if (iVar == update[iBody].iXobl ||
                    iVar == update[iBody].iYobl ||
                    iVar == update[iBody].iZobl) {
                  if (body[iBody].dObliquity != 0) {
                    dMinNow = fabs(sin(body[iBody].dObliquity) /
                                   update[iBody].daDerivProc[iVar][iEqn]);
                  } else { // Obliquity is 0, so its evolution shouldn't impact
                           // the timestep
                    dMinNow = dHUGE;
                  }
                } else if (iVar == update[iBody].iHecc ||
                           iVar == update[iBody].iKecc) {
                  if (body[iBody].dEcc != 0) {
                    dMinNow = fabs(body[iBody].dEcc /
                                   update[iBody].daDerivProc[iVar][iEqn]);
                  } else { // Eccentricity is 0, so its evolution shouldn't
                           // impact the timestep
                    dMinNow = dHUGE;
                  }
                } else {
                  dMinNow = fabs(1.0 / update[iBody].daDerivProc[iVar][iEqn]);
                }
                if (dMinNow < dMin) {
                  dMin = dMinNow;
                }
              }
              // enforce a minimum step size for ice sheets, otherwise dDt -> 0
              // real fast
            } else if (update[iBody].iaType[iVar][iEqn] == 9) {
              update[iBody].daDerivProc[iVar][iEqn] =
                    fnUpdate[iBody][iVar][iEqn](
                          body, system, update[iBody].iaBody[iVar][iEqn]);
              if (update[iBody].daDerivProc[iVar][iEqn] != 0 &&
                  *(update[iBody].pdVar[iVar]) != 0) {
                dMinNow = fabs((*(update[iBody].pdVar[iVar])) /
                               update[iBody].daDerivProc[iVar][iEqn]);
                if (dMinNow < dMin) {
                  if (dMinNow < control->Halt[iBody].iMinIceDt *
                                      (2 * PI / body[iBody].dMeanMotion) /
                                      control->Evolve.dEta) {
                    dMin = control->Halt[iBody].iMinIceDt *
                           (2 * PI / body[iBody].dMeanMotion) /
                           control->Evolve.dEta;
                  } else {
                    dMin = dMinNow;
                  }
                }
              }
              /* SpiNBody timestep: As x,y,z can cross over 0, the usual
                  x/(dx/dt) timstep doesn't work. This version compares the
                 orbital
                  radius to velocity. Probably room for improvement here. */
            } else if (update[iBody].iaType[iVar][iEqn] == 7) {
              if ((control->Evolve.bSpiNBodyDistOrb == 0) ||
                  (control->Evolve.bUsingSpiNBody == 1)) {
                update[iBody].daDerivProc[iVar][iEqn] =
                      fnUpdate[iBody][iVar][iEqn](
                            body, system, update[iBody].iaBody[iVar][iEqn]);
                dMinNow =
                      sqrt((body[iBody].dPositionX * body[iBody].dPositionX +
                            body[iBody].dPositionY * body[iBody].dPositionY +
                            body[iBody].dPositionZ * body[iBody].dPositionZ) /
                           (body[iBody].dVelX * body[iBody].dVelX +
                            body[iBody].dVelY * body[iBody].dVelY +
                            body[iBody].dVelZ * body[iBody].dVelZ));
                if (dMinNow < dMin) {
                  dMin = dMinNow;
                }
              }
            } else {
              // The parameter is controlled by a time derivative
              update[iBody].daDerivProc[iVar][iEqn] =
                    fnUpdate[iBody][iVar][iEqn](
                          body, system, update[iBody].iaBody[iVar][iEqn]);
              if (!bFloatComparison(update[iBody].daDerivProc[iVar][iEqn],
                                    0.0) &&
                  !bFloatComparison(*(update[iBody].pdVar[iVar]), 0.0)) {
                dMinNow = fabs((*(update[iBody].pdVar[iVar])) /
                               update[iBody].daDerivProc[iVar][iEqn]);
                if (dMinNow < dMin) {
                  dMin = dMinNow;
                }
              }
            }
          } // for loop
        }   // else polar/sinusoidal
      }     // for iNumVars
    }       // if (update[iBody].iNumVars > 0)
  }         // for loop iNumBodies

  return dMin;
}


void fdGetUpdateInfo(BODY *body, CONTROL *control, SYSTEM *system,
                     UPDATE *update, fnUpdateVariable ***fnUpdate) {
  /* Fills the Update arrays with the derivatives
   * or new values..
   */

  int iBody, iVar, iEqn, iNumBodies, iNumVars,
        iNumEqns; // Dummy counting variables
  EVOLVE
  integr; // Dummy EVOLVE struct so we don't have to dereference control a lot
  double dVarNow, dMinNow, dMin = dHUGE,
                           dVarTotal; // Intermediate storage variables

  integr = control->Evolve;

  iNumBodies = control->Evolve.iNumBodies;
  for (iBody = 0; iBody < iNumBodies; iBody++) {
    if (update[iBody].iNumVars > 0) {
      iNumVars = update[iBody].iNumVars;
      for (iVar = 0; iVar < iNumVars; iVar++) {
        iNumEqns = update[iBody].iNumEqns[iVar];
        for (iEqn = 0; iEqn < iNumEqns; iEqn++) {
          update[iBody].daDerivProc[iVar][iEqn] = fnUpdate[iBody][iVar][iEqn](
                body, system, update[iBody].iaBody[iVar][iEqn]);
        }
      }
    }
  }
}


void EulerStep(BODY *body, CONTROL *control, SYSTEM *system, UPDATE *update,
               fnUpdateVariable ***fnUpdate, double *dDt, int iDir) {
  /* Compute and apply an Euler update step to a given parameter (x = dx/dt *
   * dt) */
  int iBody, iVar, iEqn;
  double dFoo;

  /* Adjust dt? */
  if (control->Evolve.bVarDt) {
    /* dDt is the dynamical timescale */
    *dDt = fdGetTimeStep(body, control, system, update, fnUpdate);
    *dDt = AssignDt(*dDt, (control->Io.dNextOutput - control->Evolve.dTime),
                    control->Evolve.dEta);
  }

  for (iBody = 0; iBody < control->Evolve.iNumBodies; iBody++) {
    for (iVar = 0; iVar < update[iBody].iNumVars; iVar++) {
      for (iEqn = 0; iEqn < update[iBody].iNumEqns[iVar]; iEqn++) {
        if (update[iBody].iaType[iVar][iEqn] == 0) {
          *(update[iBody].pdVar[iVar]) = update[iBody].daDerivProc[iVar][iEqn];
        } else {
          /* Update the parameter in the BODY struct! Be careful! */
          *(update[iBody].pdVar[iVar]) +=
                iDir * update[iBody].daDerivProc[iVar][iEqn] * (*dDt);
        }
      }
    }
  }
}

void RungeKutta4Step(BODY *body, CONTROL *control, SYSTEM *system,
                     UPDATE *update, fnUpdateVariable ***fnUpdate, double *dDt,
                     int iDir) {
  /* Compute and apply a 4th order Runge-Kutta update step a given parameter. */
  int iBody, iVar, iEqn, iSubStep, iNumBodies, iNumVars, iNumEqns;
  double dFoo, dDelta;

  EVOLVE *evolve = &(
        control->Evolve); // Save Evolve as a variable for speed and legibility

  /* Create a copy of BODY array */
  BodyCopy(evolve->tmpBody, body, &control->Evolve);

  /* Derivatives at start */
  *dDt = fdGetTimeStep(body, control, system, control->Evolve.tmpUpdate,
                       fnUpdate);

  /* Adjust dt? */
  if (evolve->bVarDt) {
    /*  This is minimum dynamical timescale */
    *dDt = AssignDt(*dDt, (control->Io.dNextOutput - evolve->dTime),
                    evolve->dEta);
  } else {
    *dDt = evolve->dTimeStep;
  }

  evolve->dCurrentDt = *dDt;
  iNumBodies         = evolve->iNumBodies;
#pragma omp parallel for num_threads(NUM_THREADS) private(iNumVars, iNumEqns,  \
                                                          iVar, iEqn)
  for (iBody = 0; iBody < iNumBodies; iBody++) {
    // int thread_num = omp_get_thread_num();
    // int cpu_num = sched_getcpu();
    // printf("Thread %3d is running on CPU %3d\n", thread_num, cpu_num);
    double daDerivVar;
    iNumVars = update[iBody].iNumVars;
    for (iVar = 0; iVar < iNumVars; iVar++) {
      daDerivVar = 0;
      iNumEqns   = update[iBody].iNumEqns[iVar];
      for (iEqn = 0; iEqn < iNumEqns; iEqn++) {
        daDerivVar += iDir * evolve->tmpUpdate[iBody].daDerivProc[iVar][iEqn];
        evolve->daDerivProc[0][iBody][iVar][iEqn] = evolve->tmpUpdate[iBody].daDerivProc[iVar][iEqn];
      }
      evolve->daDeriv[0][iBody][iVar] = daDerivVar;
    }
  }

  for (iBody = 0; iBody < iNumBodies; iBody++) {
    iNumVars = update[iBody].iNumVars;
    for (iVar = 0; iVar < iNumVars; iVar++) {
      if (update[iBody].iaType[iVar][0] == 0 ||
          update[iBody].iaType[iVar][0] == 3 ||
          update[iBody].iaType[iVar][0] == 10) {
        // LUGER: Note that this is the VALUE of the variable getting passed,
        // contrary to what the names suggest These values are updated in the
        // tmpUpdate struct so that equations which are dependent upon them will
        // be evaluated with higher accuracy
        *(evolve->tmpUpdate[iBody].pdVar[iVar]) =
              evolve->daDeriv[0][iBody][iVar];
      } else {
        /* While we're in this loop, move each parameter to the midpoint of the
         * timestep */
        *(evolve->tmpUpdate[iBody].pdVar[iVar]) =
              *(update[iBody].pdVar[iVar]) +
              0.5 * (*dDt) * evolve->daDeriv[0][iBody][iVar];
      }
    }
  }

  /* First midpoint derivative.*/
  PropertiesAuxiliary(evolve->tmpBody, control, system, update);

  fdGetUpdateInfo(evolve->tmpBody, control, system, evolve->tmpUpdate,
                  fnUpdate);

#pragma omp parallel for num_threads(NUM_THREADS) private(iNumVars, iNumEqns,  \
                                                          iVar, iEqn)
  for (iBody = 0; iBody < iNumBodies; iBody++) {
    iNumVars = update[iBody].iNumVars;
    double daDerivVar;
    for (iVar = 0; iVar < iNumVars; iVar++) {
      daDerivVar = 0;
      iNumEqns   = update[iBody].iNumEqns[iVar];
      for (iEqn = 0; iEqn < iNumEqns; iEqn++) {
        daDerivVar += iDir * evolve->tmpUpdate[iBody].daDerivProc[iVar][iEqn];
        evolve->daDerivProc[1][iBody][iVar][iEqn] = evolve->tmpUpdate[iBody].daDerivProc[iVar][iEqn];
      }
      evolve->daDeriv[1][iBody][iVar] = daDerivVar;
    }
  }

  for (iBody = 0; iBody < iNumBodies; iBody++) {
    iNumVars = update[iBody].iNumVars;
    for (iVar = 0; iVar < iNumVars; iVar++) {

      if (update[iBody].iaType[iVar][0] == 0 ||
          update[iBody].iaType[iVar][0] == 3 ||
          update[iBody].iaType[iVar][0] == 10) {
        // LUGER: Note that this is the VALUE of the variable getting passed,
        // contrary to what the names suggest These values are updated in the
        // tmpUpdate struct so that equations which are dependent upon them will
        // be evaluated with higher accuracy
        *(evolve->tmpUpdate[iBody].pdVar[iVar]) =
              evolve->daDeriv[1][iBody][iVar];
      } else {
        /* While we're in this loop, move each parameter to the midpoint
        of the timestep based on the midpoint derivative. */
        *(evolve->tmpUpdate[iBody].pdVar[iVar]) =
              *(update[iBody].pdVar[iVar]) +
              0.5 * (*dDt) * evolve->daDeriv[1][iBody][iVar];
      }
    }
  }

  /* Second midpoint derivative */
  PropertiesAuxiliary(evolve->tmpBody, control, system, update);

  fdGetUpdateInfo(evolve->tmpBody, control, system, evolve->tmpUpdate,
                  fnUpdate);

#pragma omp parallel for num_threads(NUM_THREADS) private(iNumVars, iNumEqns,  \
                                                          iVar, iEqn)
  for (iBody = 0; iBody < iNumBodies; iBody++) {
    iNumVars = update[iBody].iNumVars;
    double daDerivVar;
    for (iVar = 0; iVar < iNumVars; iVar++) {
      daDerivVar = 0;
      iNumEqns   = update[iBody].iNumEqns[iVar];
      for (iEqn = 0; iEqn < iNumEqns; iEqn++) {
        daDerivVar += iDir * evolve->tmpUpdate[iBody].daDerivProc[iVar][iEqn];
        evolve->daDerivProc[2][iBody][iVar][iEqn] = evolve->tmpUpdate[iBody].daDerivProc[iVar][iEqn];
      }
      evolve->daDeriv[2][iBody][iVar] = daDerivVar;
    }
  }

  for (iBody = 0; iBody < iNumBodies; iBody++) {
    iNumVars = update[iBody].iNumVars;
    for (iVar = 0; iVar < iNumVars; iVar++) {

      if (update[iBody].iaType[iVar][0] == 0 ||
          update[iBody].iaType[iVar][0] == 3 ||
          update[iBody].iaType[iVar][0] == 10) {
        // LUGER: Note that this is the VALUE of the variable getting passed,
        // contrary to what the names suggest These values are updated in the
        // tmpUpdate struct so that equations which are dependent upon them will
        // be evaluated with higher accuracy
        *(evolve->tmpUpdate[iBody].pdVar[iVar]) =
              evolve->daDeriv[2][iBody][iVar];
      } else {
        /* While we're in this loop, move each parameter to the end of
        the timestep based on the second midpoint derivative. */
        *(evolve->tmpUpdate[iBody].pdVar[iVar]) =
              *(update[iBody].pdVar[iVar]) +
              *dDt * evolve->daDeriv[2][iBody][iVar];
      }
    }
  }

  /* Full step derivative */
  PropertiesAuxiliary(evolve->tmpBody, control, system, update);

  fdGetUpdateInfo(evolve->tmpBody, control, system, evolve->tmpUpdate,
                  fnUpdate);

#pragma omp parallel for num_threads(NUM_THREADS) private(iNumVars, iNumEqns,  \
                                                          iVar, iEqn)
  for (iBody = 0; iBody < iNumBodies; iBody++) {
    double daDerivVar;
    iNumVars = update[iBody].iNumVars;
    for (iVar = 0; iVar < iNumVars; iVar++) {
      daDerivVar = 0;
      if (update[iBody].iaType[iVar][0] == 0 ||
          update[iBody].iaType[iVar][0] == 3 ||
          update[iBody].iaType[iVar][0] == 10) {
        // NOTHING!
      } else {
        evolve->daDeriv[3][iBody][iVar] = 0;
        iNumEqns                        = update[iBody].iNumEqns[iVar];
        for (iEqn = 0; iEqn < iNumEqns; iEqn++) {
          daDerivVar += iDir * evolve->tmpUpdate[iBody].daDerivProc[iVar][iEqn];
          evolve->daDerivProc[3][iBody][iVar][iEqn] = evolve->tmpUpdate[iBody].daDerivProc[iVar][iEqn];
        }
        evolve->daDeriv[3][iBody][iVar] = daDerivVar;
      }
    }
  }
  /* Now do the update -- Note the pointer to the home of the actual
   * variables!!! */
  for (iBody = 0; iBody < iNumBodies; iBody++) {
    iNumVars = update[iBody].iNumVars;
    for (iVar = 0; iVar < iNumVars; iVar++) {
      update[iBody].daDeriv[iVar] = 1. / 6 *
                                    (evolve->daDeriv[0][iBody][iVar] +
                                     2 * evolve->daDeriv[1][iBody][iVar] +
                                     2 * evolve->daDeriv[2][iBody][iVar] +
                                     evolve->daDeriv[3][iBody][iVar]);
      for (iEqn = 0; iEqn < update[iBody].iNumEqns[iVar]; iEqn++) {
        update[iBody].daDerivProc[iVar][iEqn] = 1. / 6 *
                                    (evolve->daDerivProc[0][iBody][iVar][iEqn] +
                                     2 * evolve->daDerivProc[1][iBody][iVar][iEqn] +
                                     2 * evolve->daDerivProc[2][iBody][iVar][iEqn] +
                                     evolve->daDerivProc[3][iBody][iVar][iEqn]);
      }

      if (update[iBody].iaType[iVar][0] == 0 ||
          update[iBody].iaType[iVar][0] == 3 ||
          update[iBody].iaType[iVar][0] == 10) {
        // LUGER: Note that this is the VALUE of the variable getting passed,
        // contrary to what the names suggest
        *(update[iBody].pdVar[iVar]) = evolve->daDeriv[0][iBody][iVar];
      } else {
        *(update[iBody].pdVar[iVar]) += update[iBody].daDeriv[iVar] * (*dDt);
      }
    }
  }
}

/*
 * Evolution Subroutine
 */

void Evolve(BODY *body, CONTROL *control, FILES *files, MODULE *module,
            OUTPUT *output, SYSTEM *system, UPDATE *update,
            fnUpdateVariable ***fnUpdate, fnWriteOutput *fnWrite,
            fnIntegrate fnOneStep) {
  /* Master evolution routine that controls the simulation integration. */
  int iDir, iBody, iModule, nSteps; // Dummy counting variables
  double dDt, dFoo;                 // Next timestep, dummy variable
  double dEqSpinRate;               // Store the equilibrium spin rate

  nSteps = 0;

  if (control->Evolve.bDoForward) {
    iDir = 1;
  } else {
    iDir = -1;
  }

  PropertiesAuxiliary(body, control, system, update);
  control->Io.dNextOutput = control->Evolve.dTime + control->Io.dOutputTime;

  // Get derivatives at start, useful for logging
  dDt = fdGetTimeStep(body, control, system, update, fnUpdate);


  /* Adjust dt? */
  if (control->Evolve.bVarDt) {
    /* Now choose the correct timestep */
    dDt = AssignDt(dDt, (control->Io.dNextOutput - control->Evolve.dTime),
                   control->Evolve.dEta);
  } else {
    dDt = control->Evolve.dTimeStep;
  }

  /* Write out initial conditions */
  WriteOutput(body, control, files, output, system, update, fnWrite,
              control->Evolve.dTime, dDt);

  /* If Runge-Kutta need to copy actual update to that in
     control->Evolve. This transfer all the meta-data about the
     struct. */
  UpdateCopy(control->Evolve.tmpUpdate, update, control->Evolve.iNumBodies);

  /*
   *
   * Main loop begins here
   *
   */

  while (control->Evolve.dTime < control->Evolve.dStopTime) {
    /* Take one step */
    fnOneStep(body, control, system, update, fnUpdate, &dDt, iDir);

    for (iBody = 0; iBody < control->Evolve.iNumBodies; iBody++) {
      for (iModule = 0; iModule < control->Evolve.iNumModules[iBody];
           iModule++) {
        control->fnForceBehavior[iBody][iModule](body, module, &control->Evolve,
                                                 &control->Io, system, update,
                                                 fnUpdate, iBody, iModule);
      }

      for (iModule = 0; iModule < control->iNumMultiForce[iBody]; iModule++) {
        control->fnForceBehaviorMulti[iBody][iModule](
              body, module, &control->Evolve, &control->Io, system, update,
              fnUpdate, iBody, iModule);
      }
    }

    fdGetUpdateInfo(body, control, system, update, fnUpdate);

    /* Halt? */
    if (fbCheckHalt(body, control, update, fnUpdate)) {
      fdGetUpdateInfo(body, control, system, update, fnUpdate);
      WriteOutput(body, control, files, output, system, update, fnWrite,
                  control->Evolve.dTime,
                  control->Io.dOutputTime / control->Evolve.nSteps);
      return;
    }

    for (iBody = 0; iBody < control->Evolve.iNumBodies; iBody++) {
      body[iBody].dAge += iDir * dDt;
    }

    control->Evolve.dTime += dDt;
    nSteps++;

    /* Time for Output? */
    if (control->Evolve.dTime >= control->Io.dNextOutput) {
      control->Evolve.nSteps += nSteps;
      WriteOutput(body, control, files, output, system, update, fnWrite,
                  control->Evolve.dTime,
                  control->Io.dOutputTime / control->Evolve.nSteps);
      // Timesteps are synchronized with the output time, so this statement is
      // sufficient
      control->Io.dNextOutput += control->Io.dOutputTime;
      nSteps = 0;
    }

    /* Get auxiliary properties for next step -- first call
       was prior to loop. */
    PropertiesAuxiliary(body, control, system, update);

    // If control->Evolve.bFirstStep hasn't been switched off by now, do so.
    if (control->Evolve.bFirstStep) {
      control->Evolve.bFirstStep = 0;
    }

    // Any variables reached an interesting value?
    CheckProgress(body, control, system, update);
  }

  if (control->Io.iVerbose >= VERBPROG) {
    printf("Evolution completed.\n");
  }
  //     printf("%d\n",body[1].iBadImpulse);
}
