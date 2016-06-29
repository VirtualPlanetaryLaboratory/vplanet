/********************** HALT.C ***********************/
/*
 * Rory Barnes, Wed May  7 16:19:48 PDT 2014
 *
 * This file contains all the checks for dynamical states
 * that result in the termination of the program.
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "vplanet.h"

#define max(a,b) \
     ({ __typeof__ (a) _a = (a); \
             __typeof__ (b) _b = (b); \
           _a > _b ? _a : _b; })

int fiNumHalts(HALT *halt,MODULE *module,int iBody) {
  int iModule,iNumHalts=0;

  /* Multi-module halts */
  if (halt->bMerge)
    iNumHalts++;
  if (halt->dMinObl >= 0)
    iNumHalts++;
  if (halt->dMaxEcc < 1)
    iNumHalts++;
  if (halt->dMinSemi > 0)
    iNumHalts++;
  if (halt->dMinEcc > 0) 
    iNumHalts++;
  if (halt->bPosDeDt)
    iNumHalts++;
/* XXX not implemented yet.
  if (halt->dMinIntEn > 0)
    iNumHalts++;
*/

  for (iModule=0;iModule<module->iNumModules[iBody];iModule++)
    module->fnCountHalts[iBody][iModule](halt,&iNumHalts);
  
  return iNumHalts;
}

void InitializeHalts(CONTROL *control,MODULE *module) {
  
  control->fnHalt=malloc(control->Evolve.iNumBodies*sizeof(fnHaltModule*));

}

/****************** HALTS *********************/

/* Minimum obliquity? */
int HaltMinObl(BODY *body,EVOLVE *evolve,HALT *halt,IO *io,UPDATE *update,int iBody) {
  if (body[iBody].dObliquity < halt->dMinObl) {
    if (io->iVerbose >= VERBPROG) {
      printf("HALT: Body %s's Obliquity = ",body[iBody].cName);
      fprintd(stdout,body[iBody].dObliquity,io->iSciNot,io->iDigits);
      printf(", < minimum obliquity = ");
      fprintd(stdout,halt->dMinObl,io->iSciNot,io->iDigits);
      printf(" at %.2e years.\n",evolve->dTime/YEARSEC);
    }
    return 1;
  }

  return 0;
}

/* Maximum Eccentricity? */
int HaltMaxEcc(BODY *body,EVOLVE *evolve,HALT *halt,IO *io,UPDATE *update,int iBody) {  
  // XXX is EccSq defined here
  if (sqrt(pow(body[iBody].dHecc,2)+pow(body[iBody].dKecc,2)) >= halt->dMaxEcc) {
    if (io->iVerbose >= VERBPROG) {
      printf("HALT: e[%d] = ",iBody);
      fprintd(stdout,sqrt(pow(body[iBody].dHecc,2)+pow(body[iBody].dKecc,2)),io->iSciNot,io->iDigits);
      printf(", > max e = ");
      fprintd(stdout,halt->dMaxEcc,io->iSciNot,io->iDigits);
      printf(" at %.2e years\n",evolve->dTime/YEARSEC);
    }
    return 1;
  }

  return 0;
}

/* Minimum Eccentricity? */
int HaltMinEcc(BODY *body,EVOLVE *evolve,HALT *halt,IO *io,UPDATE *update,int iBody) {
  //XXX Is dEccSq defined here?
  if (sqrt(pow(body[iBody].dHecc,2)+pow(body[iBody].dKecc,2)) <= halt->dMinEcc) {
    if (io->iVerbose >= VERBPROG) {
      printf("HALT: e = ");
      fprintd(stdout,sqrt(body[iBody].dEccSq),io->iSciNot,io->iDigits);
      printf(", < min e = ");
      fprintd(stdout,halt->dMinEcc,io->iSciNot,io->iDigits);
      printf(" at %.2e years\n",evolve->dTime/YEARSEC);
    }
    return 1;
  }

  return 0;
}

/* Minimum Semi-Major Axis? */
int HaltMinSemi(BODY *body,EVOLVE *evolve,HALT *halt,IO *io,UPDATE *update,int iBody) {
  if (body[iBody].dSemi <= halt->dMinSemi) {
    if (io->iVerbose >= VERBPROG) {
      printf("HALT: a = ");
      fprintd(stdout,sqrt(body[iBody].dEccSq),io->iSciNot,io->iDigits);
      printf(", < min a = ");
      fprintd(stdout,halt->dMinSemi,io->iSciNot,io->iDigits);
      printf(" at %.2e years\n",evolve->dTime/YEARSEC);
    }
    return 1;
  }

  return 0;
}

/* Minimum Internal Power? XXX Rewrite with radheat and thermint written 
int HaltMinIntEn(BODY *body,EVOLVE *evolve,HALT *halt,IO *io,UPDATE *update,int iBody) {
  if (body[iBody].dIntEn <= halt->dMinIntEn) {
    if (io->iVerbose >= VERBPROG) {
      printf("HALT: e = ");
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
int HaltPosDeccDt(BODY *body,EVOLVE *evolve,HALT *halt,IO *io,UPDATE *update,int iBody) {

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

/* Merge? */
int HaltMerge(BODY *body,EVOLVE *evolve,HALT *halt,IO *io,UPDATE *update,int iBody) {
  // Is iBody not using binary?
  if(body[iBody].bBinary == 0) {
    if (body[iBody].dSemi*(1.-sqrt(body[iBody].dEccSq)) <= (body[0].dRadius + body[iBody].dRadius) && halt->bMerge) { /* Merge! */
      if (io->iVerbose > VERBPROG) 
        printf("HALT: Merge at %.2e years!\n",evolve->dTime/YEARSEC);

      return 1;
    }
  }
  // Check for merge when planet is using binary
  // This also checks if stars merged, for simplicity
  else if(body[iBody].bBinary == 1 && body[iBody].iBodyType == 0) { // Using binary, is planet
    double max_radius = max(body[0].dRadius,body[1].dRadius);
    if((body[iBody].dSemi*(1.-sqrt(body[iBody].dEccSq)) <= (body[1].dSemi + max_radius + body[iBody].dRadius) || body[0].dRadius+body[1].dRadius >= body[1].dSemi) && halt->bMerge) { /* Merge! */
        if(io->iVerbose > VERBPROG)
          printf("HALT: Merge at %.2e years! %e,%d\n",evolve->dTime/YEARSEC,body[iBody].dEccSq,iBody);
          printf("cbp.dSemi: %e, bin.dSemi: %e, max_radius: %e\n",body[iBody].dSemi/AUCM,body[1].dSemi/AUCM,max_radius/AUCM);

        return 1;
      }
  }
  return 0;
}

/******************* Verify *************************/

void VerifyHalts(BODY *body,CONTROL *control,MODULE *module,OPTIONS *options) {
  int iBody,iModule,iHalt,iHalt0,iHaltNow,iHaltMaxEcc=0,iNumMaxEcc=0;

  for (iBody=0;iBody<control->Evolve.iNumBodies;iBody++) {
    // First calculate how many halts for this body

    // First get all the simple cases
    control->Halt[iBody].iNumHalts = fiNumHalts(&control->Halt[iBody],module,iBody);

    /* This needs to become module->VerifyCountHaltMaxEccModule?
     Only necessary if DISTORB called */

    /* Some bodies may have additional halts because of a halt for a
       different body. E.g., distorb requires MaxEcc set for all 
       bodies */
    if (control->Halt[iBody].dMaxEcc < 1) {
      iHaltMaxEcc=iBody; // Body # that has HaltMaxEcc
      iNumMaxEcc++;      // How many files contain HaltMaxEcc?
    }
  }

  if (iHaltMaxEcc) {
    // HaltMaxEcc was set in at least 1 file, but can only be in 1
    if (iNumMaxEcc > 1 && control->Io.iVerbose >= VERBERR) {
      fprintf(stderr,"ERROR: %s set in %d files; should only be set in one. The maximum value for the eccentricity of all non-primary body will be MAXECCDISTORB\n.",options[OPT_HALTMAXECC].cName,iNumMaxEcc);
      exit(EXIT_INPUT);
    }

    // Now add 1 to each iNumHalts
    for (iBody=1;iBody<control->Evolve.iNumBodies;iBody++) {
      if (iBody != iHaltMaxEcc)
	control->Halt[iBody].iNumHalts++;
    }
  }

  // Now malloc and assign fnHalt function vector
  for (iBody=0;iBody<control->Evolve.iNumBodies;iBody++) {
    control->fnHalt[iBody] = malloc(control->Halt[iBody].iNumHalts*sizeof(fnHaltModule));

    // Now assign the function vector
    iHaltNow=0;
  
    if (control->Halt[iBody].bMerge)
      control->fnHalt[iBody][iHaltNow++] = &HaltMerge;
    if (control->Halt[iBody].dMinObl >= 0)
      control->fnHalt[iBody][iHaltNow++] = &HaltMinObl;
    if (control->Halt[iBody].dMaxEcc < 1)
      control->fnHalt[iBody][iHaltNow++] = &HaltMaxEcc;
    if (control->Halt[iBody].dMinSemi > 0)
      control->fnHalt[iBody][iHaltNow++] = &HaltMinSemi;
    if (control->Halt[iBody].dMinEcc > 0)
      control->fnHalt[iBody][iHaltNow++] = &HaltMinEcc;
    if (control->Halt[iBody].bPosDeDt)
      control->fnHalt[iBody][iHaltNow++] = &HaltPosDeccDt;
    /* XXX Should be changed with thermint completed
       if (control->Halt[iBody].dMinIntEn > 0)
       control->fnHalt[iBody][iHaltNow++] = &HaltMinIntEn;
    */

    for (iModule=0;iModule<module->iNumModules[iBody];iModule++)
      module->fnVerifyHalt[iBody][iModule](body,control,options,iBody,&iHaltNow);

    /* This needs to become VerifyMultiBodyHalts, as should only
     be applied if DISTORB called. This problem is hard! */

    if (iHaltMaxEcc) {
      if (iBody != iHaltMaxEcc) {
	control->Halt[iBody].dMaxEcc = control->Halt[iHaltMaxEcc].dMaxEcc;
	control->fnHalt[iBody][iHaltNow++] = &HaltMaxEcc;
      }
    }
  }




  /*    Now make sure all set halts apply to the selected modules 
     XXX This sounds nice, but for now, it is left out
  if (control->Halt.bMerge) {
    if (!module->iEqtide) {
      if (control->Io.iVerbose > 3)
        fprintf(stderr,"WARNING: %s set to 1, but no selected module allows merging.\n",options[OPT_HALTMERGE].cName);
    }
  }

  if (control->Halt.bDblSync >= 0) {
    if (!module->iEqtide) {
      if (control->Io.iVerbose > 3)
        fprintf(stderr,"WARNING: %s set to 1, but no selected module allows rotational damping.\n",options[OPT_HALTMAXECC].cName);
    }
  }

  for (iBody=0;iBody<control->Evolve.iNumBodies;iBody++) {
    if (control->Halt.bTideLock[iBody] >= 0) {
      if (!module->iEqtide) {
        if (control->Io.iVerbose > 3)
          fprintf(stderr,"WARNING: %s set to 1 for %s, but no selected module allows eccentricity evolution.\n",options[OPT_HALTMAXECC].cName,body[iBody].cName);
      }
    }

    if (control->Halt.bSync[iBody] >= 0) {
      if (!module->iEqtide) {
        if (control->Io.iVerbose > 3)
          fprintf(stderr,"WARNING: %s set to 1 for %s, but no selected module allows rotational damping.\n",options[OPT_HALTSYNC].cName,body[iBody].cName);
      }
    }

    if (control->Halt.bPosDeDt[iBody] >= 0) {
      if (!module->iEqtide) {
        if (control->Io.iVerbose > 3)
          fprintf(stderr,"WARNING: %s set to 1 for %s, but no selected module allows eccentricity evolution.\n",options[OPT_HALTPOSDEDT].cName,body[iBody].cName);
      }
    }

    if (control->Halt.bMinEcc[iBody] >= 0) {
      if (!module->iEqtide) {
        if (control->Io.iVerbose > 3)
          fprintf(stderr,"WARNING: %s set to 1 for %s, but no selected module allows rotational damping.\n",options[OPT_HALTMINECC].cName,body[iBody].cName);
      }
    }

    if (control->Halt.dMinObl[iBody] >= 0) {
      if (!module->iEqtide) {
        if (control->Io.iVerbose > 3)
          fprintf(stderr,"WARNING: %s set to 1, but no selected module allows rotational damping.\n",options[OPT_HALTMINOBL].cName);
      }
    }
    
    if (control->Halt.dMaxEcc[iBody] >= 0) {
      if (!module->iEqtide) {
        if (control->Io.iVerbose > 3)

          fprintf(stderr,"WARNING: %s set to 1, but no selected module allows eccentricity evolution.\n",options[OPT_HALTMAXECC].cName);
      }
    }
  }
  */
}

/************** Check for Halts *********************/

int fbCheckHalt(BODY *body,CONTROL *control,UPDATE *update) {
  int iBody,iHalt;
  //double dMeanMotion;

  //dMeanMotion = fdSemiToMeanMotion(body[1].dSemi,(body[0].dMass+body[1].dMass));

  for (iBody=0;iBody<control->Evolve.iNumBodies;iBody++) {
    for (iHalt=0;iHalt<control->Halt[iBody].iNumHalts;iHalt++) {
      //if (control->Halt[iBody].fnHalt(body,&control->Halt[iBody],update,&control->Evolve,&control->Io,iBody))
      if (control->fnHalt[iBody][iHalt](body,&control->Evolve,&control->Halt[iBody],&control->Io,update,iBody))
        return 1;
    }
  }

  return 0;
}
