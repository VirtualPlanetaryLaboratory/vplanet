/*********************** MAIN.C *****************************/
/*
 * Rory Barnes, Wed May  7 16:22:34 PDT 2014
 *
 * Also includes the onboard help catalog. 
*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include <string.h>
#include "vplanet.h"

/* Here's where it all begins */

int main(int argc,char *argv[]) {
  int i,j,iv,iq,fi;
  double f;
  OPTIONS *options;
  OUTPUT *output;
  CONTROL control;
  UPDATE *update;
  BODY *body;
  VPLANET vplanet;
  FILES files;
  SYSTEM system;
  fnReadOption fnRead[MODULEOPTEND];
  fnWriteOutput fnWrite[MODULEOUTEND];
  fnUpdateVariable ***fnUpdate; 
  fnIntegrate fnOneStep;

  output = malloc(NUMOUT*sizeof(OUTPUT));
  options = malloc(MODULEOPTEND*sizeof(OPTIONS));

  /* These will need to be modified in VPLANET */
  InitializeOutput(output,fnWrite);
  InitializeOptions(options,fnRead);

  /* Set to IntegrationMethod to 0, so default can be
     assigned if necessary */
  control.iOneStep = 0;

  /* This will be a mess in VPLANET! */
  fnUpdate=malloc(2*sizeof(fnUpdateVariable**));
  for (i=0;i<2;i++) {
    fnUpdate[i]=malloc(4*sizeof(fnUpdateVariable*));
    for (j=0;j<4;j++) {
      fnUpdate[i][j]=malloc(1*sizeof(fnUpdateVariable));
    }
  }

  strcpy(files.cExe,argv[0]);

  if (argc == 1) {
    fprintf(stderr,"ERROR: Incorrect number of arguments. Usage: %s [-verbose] [-help] file [file].\n",argv[0]);
    exit(EXIT_EXE);
  }

  iv=-1;
  iq=-1;
  fi=-1;
  control.iVerbose = -1;
  control.bOverwrite = -1;

  /* Check for flags */
  for (i=1;i<argc;i++) {
    if (memcmp(argv[i],"-v",2) == 0) {
      control.iVerbose = 5;
      iv=i;
    }
    if (memcmp(argv[i],"-q",2) == 0) {
      control.iVerbose = 0;
      iq=i;
    }
    if (memcmp(argv[i],"-f",2) == 0) {
      control.bOverwrite = 1;
      fi=i;
    }
    if (memcmp(argv[i],"-h",2) == 0) {
      Help(output,options,files.cExe);
      exit(0);
    }
  }	  

  if (iq != -1 && iv != -1) {
    fprintf(stderr,"ERROR: -v and -q cannot be set simultaneously.\n");
    exit(EXIT_EXE);
  }

  for (i=1;i<argc;i++) {
    if (i != iv && i != iq && i != fi) 
      strcpy(files.Infile[0].cIn,argv[i]);
  }

  /* Read input files */

  ReadOptions(options,&control,&vplanet,&body,&update,&files,&system,output,fnRead);
  
  /* Check that user options are mutually compatible */

  VerifyOptions(options,output,body,&vplanet,&system,&files,&control,&fnOneStep,update,fnUpdate);
  
  /* Everything is OK; begin vplanet. */
  InitializeVPLANET(&control,update,&vplanet);

  control.Integr.dTime=0;

  if (control.iVerbose >= VERBINPUT) 
    printf("Read %s.\n",files.Infile[0].cIn);
  if (control.bLog) {
    WriteLog(&control,body,&system,output,options,&files,fnWrite,update,fnUpdate,0);
    if (control.iVerbose >= VERBPROG)
      printf("Log file written.\n");
  }

  /* Perform evolution */

  if (control.Integr.bDoForward || control.Integr.bDoBackward) {
    Evolve(&control,body,&system,output,&files,update,fnUpdate,&vplanet,fnWrite,fnOneStep);

    if (control.bLog) {
      WriteLog(&control,body,&system,output,options,&files,fnWrite,update,fnUpdate,1);
      if (control.iVerbose >= VERBPROG)
	printf("Log file updated.\n");
    }
  }

  exit(0);
}
