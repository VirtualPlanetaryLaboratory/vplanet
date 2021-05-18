/**
 @file halt.c

 @brief This file contains all the checks for dynamical states
        that result in the termination of the program.

 @author Rory Barnes ([RoryBarnes](https://github.com/RoryBarnes/))

 @date Mar 7 2014

*/

#include "vplanet.h"

int fiNumHalts(HALT *halt, MODULE *module, int iBody) {
  /* Count the number of halts VPLANET will check for during the integration */
  int iModule;       // Dummy variable iterating over modules
  int iNumHalts = 0; // Counts number of halts

  /* Multi-module halts */
  if (halt->bMerge) {
    iNumHalts++;
  }
  if (halt->dMinObl >= 0) {
    iNumHalts++;
  }
  if (halt->dMaxEcc < 1) {
    iNumHalts++;
  }
  if (halt->dMaxMutualInc > 0) {
    iNumHalts++;
  }
  if (halt->dMinSemi > 0) {
    iNumHalts++;
  }
  if (halt->dMinEcc > 0) {
    iNumHalts++;
  }
  if (halt->bPosDeDt) {
    iNumHalts++;
  }

  for (iModule = 0; iModule < module->iNumModules[iBody]; iModule++) {
    module->fnCountHalts[iBody][iModule](halt, &iNumHalts);
  }

  return iNumHalts;
}

void InitializeHalts(CONTROL *control, MODULE *module) {
  // Malloc memory for halt function pointers
  int iBody;

  control->fnHalt = malloc(control->Evolve.iNumBodies * sizeof(fnHaltModule *));
  for (iBody = 0; iBody < control->Evolve.iNumBodies; iBody++) {
    control->Halt[iBody].dMaxMutualInc = 0;
  }
}

/****************** HALTS *********************/

int HaltMinObl(BODY *body, EVOLVE *evolve, HALT *halt, IO *io, UPDATE *update,
               fnUpdateVariable ***fnUpdate, int iBody) {
  /* Halt simulation if body reaches minimum obliquity. */
  if (body[iBody].dObliquity < halt->dMinObl) {
    if (io->iVerbose >= VERBPROG) {
      printf("HALT: Body %s's Obliquity = ", body[iBody].cName);
      fprintd(stdout, body[iBody].dObliquity, io->iSciNot, io->iDigits);
      printf(", < minimum obliquity = ");
      fprintd(stdout, halt->dMinObl, io->iSciNot, io->iDigits);
      printf(" at %.2e years.\n", evolve->dTime / YEARSEC);
    }
    return 1;
  }

  return 0;
}

/* Maximum Eccentricity? */
int fniHaltMaxEcc(BODY *body, EVOLVE *evolve, HALT *halt, IO *io,
                  UPDATE *update, fnUpdateVariable ***fnUpdate, int iBody) {
  /* Halt simulation if body reaches maximum orbital eccentricity. */
  if (sqrt(pow(body[iBody].dHecc, 2) + pow(body[iBody].dKecc, 2)) >=
      halt->dMaxEcc) {
    if (io->iVerbose >= VERBPROG) {
      printf("HALT: e[%d] = ", iBody);
      fprintd(stdout,
              sqrt(pow(body[iBody].dHecc, 2) + pow(body[iBody].dKecc, 2)),
              io->iSciNot, io->iDigits);
      printf(", > max e = ");
      fprintd(stdout, halt->dMaxEcc, io->iSciNot, io->iDigits);
      printf(" at %.2e years\n", evolve->dTime / YEARSEC);
    }
    return 1;
  }

  return 0;
}

int HaltMinEcc(BODY *body, EVOLVE *evolve, HALT *halt, IO *io, UPDATE *update,
               fnUpdateVariable ***fnUpdate, int iBody) {
  /* Halt simulation if body reaches minimum eccentricity. */
  if (sqrt(pow(body[iBody].dHecc, 2) + pow(body[iBody].dKecc, 2)) <=
      halt->dMinEcc) {
    if (io->iVerbose >= VERBPROG) {
      printf("HALT: e = ");
      fprintd(stdout, sqrt(body[iBody].dEccSq), io->iSciNot, io->iDigits);
      printf(", < min e = ");
      fprintd(stdout, halt->dMinEcc, io->iSciNot, io->iDigits);
      printf(" at %.2e years\n", evolve->dTime / YEARSEC);
    }
    return 1;
  }

  return 0;
}

int HaltMinSemi(BODY *body, EVOLVE *evolve, HALT *halt, IO *io, UPDATE *update,
                fnUpdateVariable ***fnUpdate, int iBody) {
  /* Halt simulation if body reaches minimum Semi-Major Axis? */
  if (body[iBody].dSemi <= halt->dMinSemi) {
    if (io->iVerbose >= VERBPROG) {
      printf("HALT: a = ");
      fprintd(stdout, sqrt(body[iBody].dEccSq), io->iSciNot, io->iDigits);
      printf(", < min a = ");
      fprintd(stdout, halt->dMinSemi, io->iSciNot, io->iDigits);
      printf(" at %.2e years\n", evolve->dTime / YEARSEC);
    }
    return 1;
  }

  return 0;
}

/* Minimum Internal Power? Rewrite with radheat and thermint written
int HaltMinIntEn(BODY *body,EVOLVE *evolve,HALT *halt,IO *io,UPDATE *update,int
iBody) { if (body[iBody].dIntEn <= halt->dMinIntEn) { if (io->iVerbose >=
VERBPROG) { printf("HALT: e = ");
      fprintd(stdout,body[iBody].dIntEn,io->iSciNot,io->iDigits);
      printf(", < min e = ");
      fprintd(stdout,halt->dMinIntEn,io->iSciNot,io->iDigits);
      printf(" at %.2e years\n",evolve->dTime/YEARSEC);
    }
    return 1;
  }

  return 0;
}
*/

/* Positive de/dt? */
int HaltPosDeccDt(BODY *body, EVOLVE *evolve, HALT *halt, IO *io,
                  UPDATE *update, fnUpdateVariable ***fnUpdate, int iBody) {

  /* XXX This needs to be redone with Hecc and Kecc
  if (update[iBody].daDeriv[update[iBody].iEcc] > 0 && halt->bPosDeDt) {
    if (io->iVerbose >= VERBPROG) {
      printf("HALT: de/dt = ");
      fprintd(stdout,update[iBody].daDeriv[update[iBody].iEcc],io->iSciNot,io->iDigits);
      printf(" at %.2e years\n",evolve->dTime/YEARSEC);
    }
    return 1;
  }

  */
  return 0;
}

int HaltMerge(BODY *body, EVOLVE *evolve, HALT *halt, IO *io, UPDATE *update,
              fnUpdateVariable ***fnUpdate, int iBody) {
  /* Halt if two bodies, i.e. planet and star, merge? */

  /* Sometimes integration overshoots and dSemi becomes NaN -> merger */
  if (body[iBody].dSemi != body[iBody].dSemi) {
    if (halt->bMerge) {
      if (io->iVerbose > VERBPROG) {
        printf("HALT: Merge at %.2e years!\n", evolve->dTime / YEARSEC);
        printf("Numerical merger: %s's dSemi became a NaN! Try decreasing dEta "
               "by "
               "a factor of 10.\n",
               body[iBody].cName);
      }
      return 1;
    } else {
      if (io->iVerbose > VERBPROG) {
        printf("Bodies %s and %s merged at %.2e years!\n", body[0].cName,
               body[iBody].cName, evolve->dTime / YEARSEC);
      }
      fdMergePlanet(body, update, fnUpdate, iBody);
    }
  }

  // Is iBody not using binary?
  if (!body[iBody].bBinary) {
    if (body[iBody].dSemi * (1. - sqrt(body[iBody].dEccSq)) <=
        (body[0].dRadius + body[iBody].dRadius)) { /* Merge! */
      if (halt->bMerge) {
        if (io->iVerbose > VERBPROG) {
          printf("HALT: Bodies %s and %s merged at %.2e years!\n",
                 body[0].cName, body[iBody].cName, evolve->dTime / YEARSEC);
        }
        return 1;
      } else {
        if (io->iVerbose > VERBPROG) {
          printf("Bodies %s and %s merged at %.2e years!\n", body[0].cName,
                 body[iBody].cName, evolve->dTime / YEARSEC);
        }
        fdMergePlanet(body, update, fnUpdate, iBody);
      }
    }

    // Check for merge when planet is using binary
    // This also checks if stars merged, for simplicity
  } else if (body[iBody].bBinary == 1 &&
             body[iBody].iBodyType == 0) { // Using binary, is planet
    double max_radius = max(body[0].dRadius, body[1].dRadius);
    if ((body[iBody].dSemi * (1. - sqrt(body[iBody].dEccSq)) <=
         (body[1].dSemi + max_radius + body[iBody].dRadius)) &&
        halt->bMerge) { /* Merge! */
      if (io->iVerbose > VERBPROG) {
        printf("HALT: Merge at %.2e years! %e,%d\n", evolve->dTime / YEARSEC,
               body[iBody].dEccSq, iBody);
        printf("cbp.dSemi: %e, bin.dSemi: %e, max_radius: %e\n",
               body[iBody].dSemi / AUM, body[1].dSemi / AUM, max_radius / AUM);
      }
      return 1;
    }
  } else if (body[iBody].bBinary && body[iBody].iBodyType == 1 &&
             iBody == 1) { // Did binary merge?
    // Merge if sum of radii greater than binary perihelion distance
    if ((body[0].dRadius + body[1].dRadius >=
         (1.0 - body[1].dEcc) * body[1].dSemi) &&
        halt->bMerge) {
      if (io->iVerbose > VERBPROG) {
        fprintf(stderr,
                "Binary merged at %.2e years!  Semimajor axis [km]: %e.\n",
                evolve->dTime / YEARSEC, body[iBody].dSemi);
        fprintf(stderr, "Stellar radii [km]: %e, %e. \n", body[0].dRadius,
                body[1].dRadius);
      }
      return 1;
    }
  }

  return 0;
}

/******************* Verify *************************/

void VerifyHalts(BODY *body, CONTROL *control, MODULE *module,
                 OPTIONS *options) {
  /* Verify halt functions to ensure they are valid and make sense given the
   * enabled modules and the system architecture */
  int iBody, iModule, iHalt, iHalt0, iHaltNow; // Dummy counting variables
  int iHaltMaxEcc = 0; // Counts number of halt max eccentricity
  int iNumMaxEcc  = 0; // Counts number of max eccentricity occurences

  for (iBody = 0; iBody < control->Evolve.iNumBodies; iBody++) {
    // First calculate how many halts for this body

    // First get all the simple cases
    control->Halt[iBody].iNumHalts =
          fiNumHalts(&control->Halt[iBody], module, iBody);

    /* This needs to become module->VerifyCountHaltMaxEccModule?
     Only necessary if DISTORB called */

    /* Some bodies may have additional halts because of a halt for a
       different body. E.g., distorb requires MaxEcc set for all
       bodies */
    if (control->Halt[iBody].dMaxEcc < 1) {
      iHaltMaxEcc = iBody; // Body # that has HaltMaxEcc
      iNumMaxEcc++;        // How many files contain HaltMaxEcc?
    }
  }

  if (iHaltMaxEcc) {
    // HaltMaxEcc was set in at least 1 file, but can only be in 1
    if (iNumMaxEcc > 1 && control->Io.iVerbose >= VERBERR) {
      fprintf(stderr,
              "ERROR: %s set in %d files; should only be set in one. The "
              "maximum value for the eccentricity of all non-primary body will "
              "be MAXECCDISTORB\n.",
              options[OPT_HALTMAXECC].cName, iNumMaxEcc);
      exit(EXIT_INPUT);
    }

    // Now add 1 to each iNumHalts
    for (iBody = 1; iBody < control->Evolve.iNumBodies; iBody++) {
      if (iBody != iHaltMaxEcc) {
        control->Halt[iBody].iNumHalts++;
      }
    }
  }

  if (control->Halt[0].dMaxMutualInc > 0) {
    if (control->Evolve.iNumBodies == 0) {
      fprintf(stderr,
              "ERROR: %s set, but only 1 body present.\n",
              options[OPT_HALTMAXMUTUALINC].cName);
      exit(EXIT_INPUT);
    }
  }

  // Now malloc and assign fnHalt function vector
  for (iBody = 0; iBody < control->Evolve.iNumBodies; iBody++) {
    control->fnHalt[iBody] =
          malloc(control->Halt[iBody].iNumHalts * sizeof(fnHaltModule));

    // Now assign the function vector
    iHaltNow = 0;

    if (control->Halt[iBody].bMerge) {
      control->fnHalt[iBody][iHaltNow++] = &HaltMerge;
    }
    if (control->Halt[iBody].dMinObl >= 0) {
      control->fnHalt[iBody][iHaltNow++] = &HaltMinObl;
    }
    if (control->Halt[iBody].dMaxEcc < 1) {
      control->fnHalt[iBody][iHaltNow++] = &fniHaltMaxEcc;
    }
    if (control->Halt[iBody].dMaxMutualInc > 0) {
      /* Note the different approach here. These fn live in their module file.
         We set initially to DistOrb since it cannot be applied to the central
         body. SpiNBody, however, can be, so if SpiNBody is set, change to that
         halt function. */
      control->fnHalt[iBody][iHaltNow++] = &fbHaltMaxMutualIncDistorb;
      if (body[iBody].bSpiNBody) {
        control->fnHalt[iBody][iHaltNow++] = &fbHaltMaxMutualIncSpiNBody;
      }
    }
    if (control->Halt[iBody].dMinSemi > 0) {
      control->fnHalt[iBody][iHaltNow++] = &HaltMinSemi;
    }
    if (control->Halt[iBody].dMinEcc > 0) {
      control->fnHalt[iBody][iHaltNow++] = &HaltMinEcc;
    }
    if (control->Halt[iBody].bPosDeDt) {
      control->fnHalt[iBody][iHaltNow++] = &HaltPosDeccDt;
    }
    /* Should be changed with thermint completed
       if (control->Halt[iBody].dMinIntEn > 0)
       control->fnHalt[iBody][iHaltNow++] = &HaltMinIntEn;
    */

    for (iModule = 0; iModule < module->iNumModules[iBody]; iModule++) {
      module->fnVerifyHalt[iBody][iModule](body, control, options, iBody,
                                           &iHaltNow);
    }

    if (iHaltMaxEcc) {
      if (iBody != iHaltMaxEcc) {
        control->Halt[iBody].dMaxEcc       = control->Halt[iHaltMaxEcc].dMaxEcc;
        control->fnHalt[iBody][iHaltNow++] = &fniHaltMaxEcc;
      }
    }
  }
}

/************** Check for Halts *********************/

int fbCheckHalt(BODY *body, CONTROL *control, UPDATE *update,
                fnUpdateVariable ***fnUpdate) {
  int iBody, iHalt;

  for (iBody = 0; iBody < control->Evolve.iNumBodies; iBody++) {
    for (iHalt = 0; iHalt < control->Halt[iBody].iNumHalts; iHalt++) {
      if (control->fnHalt[iBody][iHalt](body, &control->Evolve,
                                        &control->Halt[iBody], &control->Io,
                                        update, fnUpdate, iBody)) {
        return 1;
      }
    }
  }

  return 0;
}
