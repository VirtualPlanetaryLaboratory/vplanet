/********************** RADHEAT.C **********************/
/*
 * Rory Barnes, Sat Jun 28 07:07:38 PDT 2014
 *
 * Subroutines that control the integration of the 
 * radiogenic heating model. Also includes subroutines 
 * that switch between the two models.
*/

#include <stdio.h>
#include <math.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "vplanet.h"

void  InitializeControlRadheat(CONTROL *control) {
  /* Nothing for now, but this subroutine is necessary for module loops. */
}

void BodyCopyRadheat(BODY *dest,BODY *src,int foo,int iBody) {
  dest[iBody].d40KNum = src[iBody].d40KNum;
  dest[iBody].d40KConst = src[iBody].d40KConst;

  dest[iBody].d232ThNum = src[iBody].d232ThNum;
  dest[iBody].d232ThConst = src[iBody].d232ThConst;

  dest[iBody].d238UNum = src[iBody].d238UNum;
  dest[iBody].d238UConst = src[iBody].d238UConst;

  dest[iBody].d235UNum = src[iBody].d235UNum;  //PED
  dest[iBody].d235UConst = src[iBody].d235UConst; //PED
}

void InitializeBodyRadheat(BODY *body,CONTROL *control,UPDATE *update,int iBody,int iModule) {
}

void InitializeUpdateTmpBodyRadheat(BODY *body,CONTROL *control,UPDATE *update,int iBody) {
}

/**************** RADHEAT options ********************/

/* Potassium */

void Read40KPower(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  /* This parameter cannot exist in primary file */
  /* Must verify in conjuction with 40KMass and 40KNum */
  int lTmp=-1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn,options->cName,&dTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {   //if line num of option ge 0
    NotPrimaryInput(iFile,options->cName,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);
    if (dTmp < 0)   //if input value lt 0
      body[iFile-1].d40KPower = dTmp*dNegativeDouble(*options,files->Infile[iFile].cIn,control->Io.iVerbose);
   else
       //      body[iFile-1].d40KPower = dTmp*fdUnitsMass(control->Units[iFile].iMass);
       //CHANGED units Mass to Power.
       body[iFile-1].d40KPower = dTmp*fdUnitsPower(control->Units[iFile].iTime,control->Units[iFile].iMass,control->Units[iFile].iLength);
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else
      if (iFile > 0)  //if line num not ge 0, then if iFile gt 0, then set default.
      body[iFile-1].d40KPower = options->dDefault;
}

void Read40KMass(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  /* This parameter cannot exist in primary file */
  /* Must verify in conjuction with 40KPower and 40KNum */
  int lTmp=-1;
  double dTmp;

  printf("%s \n",files->Infile[iFile].cIn);
  AddOptionDouble(files->Infile[iFile].cIn,options->cName,&dTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile,options->cName,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);
    if (dTmp < 0)
      body[iFile-1].d40KMass = dTmp*dNegativeDouble(*options,files->Infile[iFile].cIn,control->Io.iVerbose);
   else
      body[iFile-1].d40KMass = dTmp*fdUnitsMass(control->Units[iFile].iMass);
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else
    if (iFile > 0)
      body[iFile-1].d40KMass = options->dDefault;
}

void Read40KNum(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  /* This parameter cannot exist in primary file */
  /* Must verify in conjuction with 40KPower and 40KNum */
  int lTmp=-1;
  double dTmp;

  printf("%s \n",files->Infile[iFile].cIn);
  //  fflush(stdout);
  AddOptionDouble(files->Infile[iFile].cIn,options->cName,&dTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile,options->cName,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);
    if (dTmp < 0)
	body[iFile-1].d40KNum = dTmp*dNegativeDouble(*options,files->Infile[iFile].cIn,control->Io.iVerbose);  //dTmp=input value, dNegativeDouble=-dNeg (default Value).
    else
	body[iFile-1].d40KNum = dTmp;   //units of num are num!
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else
    if (iFile > 0)
      body[iFile-1].d40KNum = options->dDefault;
}

/* Thorium */

void Read232ThPower(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  /* This parameter cannot exist in primary file */
  /* Must verify in conjuction with 232ThMass and 232ThNum */
  int lTmp=-1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn,options->cName,&dTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile,options->cName,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);
    if (dTmp < 0)
      body[iFile-1].d232ThPower = dTmp*dNegativeDouble(*options,files->Infile[iFile].cIn,control->Io.iVerbose);
   else
       //      body[iFile-1].d232ThPower = dTmp*fdUnitsMass(control->Units[iFile].iMass);
       body[iFile-1].d232ThPower = dTmp*fdUnitsPower(control->Units[iFile].iTime,control->Units[iFile].iMass,control->Units[iFile].iLength);
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else
    if (iFile > 0)
      body[iFile-1].d232ThPower = options->dDefault;
}

void Read232ThMass(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  /* This parameter cannot exist in primary file */
  /* Must verify in conjuction with 232ThPower and 232ThNum */
  int lTmp=-1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn,options->cName,&dTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile,options->cName,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);
    if (dTmp < 0)
      body[iFile-1].d232ThMass = dTmp*dNegativeDouble(*options,files->Infile[iFile].cIn,control->Io.iVerbose);
   else
      body[iFile-1].d232ThMass = dTmp*fdUnitsMass(control->Units[iFile].iMass);
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else
    if (iFile > 0)
      body[iFile-1].d232ThMass = options->dDefault;
}

void Read232ThNum(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  /* This parameter cannot exist in primary file */
  /* Must verify in conjuction with 232ThPower and 232ThMass */
  int lTmp=-1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn,options->cName,&dTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile,options->cName,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);
    if (dTmp < 0)
      body[iFile-1].d232ThNum = dTmp*dNegativeDouble(*options,files->Infile[iFile].cIn,control->Io.iVerbose);
   else
      body[iFile-1].d232ThNum = dTmp;
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else
    if (iFile > 0)
      body[iFile-1].d232ThNum = options->dDefault;
}

/* Uranium 238 */

void Read238UPower(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  /* This parameter cannot exist in primary file */
  /* Must verify in conjuction with 238UMass and 232UNum */
  int lTmp=-1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn,options->cName,&dTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile,options->cName,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);
    if (dTmp < 0)
      body[iFile-1].d238UPower = dTmp*dNegativeDouble(*options,files->Infile[iFile].cIn,control->Io.iVerbose);
   else
       //      body[iFile-1].d238UPower = dTmp*fdUnitsMass(control->Units[iFile].iMass);
       body[iFile-1].d238UPower = dTmp*fdUnitsPower(control->Units[iFile].iTime,control->Units[iFile].iMass,control->Units[iFile].iLength);
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else
    if (iFile > 0)
      body[iFile-1].d238UPower = options->dDefault;
}

void Read238UMass(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  /* This parameter cannot exist in primary file */
  /* Must verify in conjuction with 238UPower and 238UNum */
  int lTmp=-1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn,options->cName,&dTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile,options->cName,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);
    if (dTmp < 0)
      body[iFile-1].d238UMass = dTmp*dNegativeDouble(*options,files->Infile[iFile].cIn,control->Io.iVerbose);
   else
      body[iFile-1].d238UMass = dTmp*fdUnitsMass(control->Units[iFile].iMass);
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else
    if (iFile > 0)
      body[iFile-1].d238UMass = options->dDefault;
}

void Read238UNum(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  /* This parameter cannot exist in primary file */
  /* Must verify in conjuction with 238UPower and 238UMass */
  int lTmp=-1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn,options->cName,&dTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile,options->cName,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);
    if (dTmp < 0)
      body[iFile-1].d238UNum = dTmp*dNegativeDouble(*options,files->Infile[iFile].cIn,control->Io.iVerbose);
   else
      body[iFile-1].d238UNum = dTmp;
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else
    if (iFile > 0)
      body[iFile-1].d238UNum = options->dDefault;
}

/* Uranium 235 PED */

void Read235UPower(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  /* This parameter cannot exist in primary file */
  /* Must verify in conjuction with 235UMass and 232UNum */
  int lTmp=-1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn,options->cName,&dTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile,options->cName,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);
    if (dTmp < 0)
      body[iFile-1].d235UPower = dTmp*dNegativeDouble(*options,files->Infile[iFile].cIn,control->Io.iVerbose);
   else
       //      body[iFile-1].d235UPower = dTmp*fdUnitsMass(control->Units[iFile].iMass);
       body[iFile-1].d235UPower = dTmp*fdUnitsPower(control->Units[iFile].iTime,control->Units[iFile].iMass,control->Units[iFile].iLength);
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else
    if (iFile > 0)
      body[iFile-1].d235UPower = options->dDefault;
}

void Read235UMass(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  /* This parameter cannot exist in primary file */
  /* Must verify in conjuction with 235UPower and 235UNum */
  int lTmp=-1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn,options->cName,&dTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile,options->cName,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);
    if (dTmp < 0)
      body[iFile-1].d235UMass = dTmp*dNegativeDouble(*options,files->Infile[iFile].cIn,control->Io.iVerbose);
   else
      body[iFile-1].d235UMass = dTmp*fdUnitsMass(control->Units[iFile].iMass);
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else
    if (iFile > 0)
      body[iFile-1].d235UMass = options->dDefault;
}

void Read235UNum(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  /* This parameter cannot exist in primary file */
  /* Must verify in conjuction with 235UPower and 235UMass */
  int lTmp=-1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn,options->cName,&dTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {
      NotPrimaryInput(iFile,options->cName,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);
      if (dTmp < 0)
	  body[iFile-1].d235UNum = dTmp*dNegativeDouble(*options,files->Infile[iFile].cIn,control->Io.iVerbose);
      else
	  body[iFile-1].d235UNum = dTmp;
      UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else
      if (iFile > 0)
	  body[iFile-1].d235UNum = options->dDefault;
}

/* Initiatlize Input Options */

void InitializeOptionsRadheat(OPTIONS *options,fnReadOption fnRead[]) {
  int iOpt,iFile;

  sprintf(options[OPT_40KMASS].cName,"d40KMass");
  sprintf(options[OPT_40KMASS].cDescr,"Initial Mass of 40K");
  sprintf(options[OPT_40KMASS].cDefault,"Primordial Earth: xxx");
  options[OPT_40KMASS].iType = 2;
  options[OPT_40KMASS].iMultiFile = 1;
  options[OPT_40KMASS].dNeg = EMASSMAN40K;
  options[OPT_40KMASS].dDefault = 0;  //DO THIS FOR ALL!!!
  sprintf(options[OPT_40KMASS].cNeg,"Earth Masses");
  fnRead[OPT_40KMASS] = &Read40KMass;
  
  sprintf(options[OPT_40KNUM].cName,"d40KNum");
  sprintf(options[OPT_40KNUM].cDescr,"Initial Number of 40K Atoms");
  sprintf(options[OPT_40KNUM].cDefault,"Primordial Earth: xxx");
  options[OPT_40KNUM].iType = 2;
  options[OPT_40KNUM].iMultiFile = 1;
  options[OPT_40KNUM].dNeg = ENUMMAN40K;
  options[OPT_40KNUM].dDefault = 0;
  sprintf(options[OPT_40KNUM].cNeg,"Primordial Earth 40K Number");
  fnRead[OPT_40KNUM] = &Read40KNum;
  
  sprintf(options[OPT_40KPOWER].cName,"d40KPower");
  sprintf(options[OPT_40KPOWER].cDescr,"Initial Power Production from 40K Atoms");
  sprintf(options[OPT_40KPOWER].cDefault,"Primordial Earth: xx TW");
  options[OPT_40KPOWER].iType = 2;
  options[OPT_40KPOWER].iMultiFile = 1;
  options[OPT_40KPOWER].dNeg = EPOWERMAN40K;  //Earth's POWER of 40K
  options[OPT_40KPOWER].dDefault = 0;
  sprintf(options[OPT_40KPOWER].cNeg,"TW");
  fnRead[OPT_40KPOWER] = &Read40KPower;
  
  sprintf(options[OPT_232THMASS].cName,"d232ThMass");
  sprintf(options[OPT_232THMASS].cDescr,"Initial Mass of 232Th");
  sprintf(options[OPT_232THMASS].cDefault,"Primordial Earth: xxx");
  options[OPT_232THMASS].iType = 2;
  options[OPT_232THMASS].iMultiFile = 1;
  options[OPT_232THMASS].dNeg = EMASSMAN232TH;
  options[OPT_232THMASS].dDefault = 0;
  sprintf(options[OPT_232THMASS].cNeg,"Earth Masses");
  fnRead[OPT_232THMASS] = &Read232ThMass; 
  
  sprintf(options[OPT_232THNUM].cName,"d232ThNum");
  sprintf(options[OPT_232THNUM].cDescr,"Initial Number of 232Th Atoms");
  sprintf(options[OPT_232THNUM].cDefault,"Primordial Earth: xxx");
  options[OPT_232THNUM].iType = 2;
  options[OPT_232THNUM].iMultiFile = 1;
  options[OPT_232THNUM].dNeg = ENUMMAN232TH;
  options[OPT_232THNUM].dDefault = 0;
  sprintf(options[OPT_232THNUM].cNeg,"Primordial Earth 232Th Number");
  fnRead[OPT_232THNUM] = &Read232ThNum;
  
  sprintf(options[OPT_232THPOWER].cName,"d232ThPower");
  sprintf(options[OPT_232THPOWER].cDescr,"Initial Power Production from 232Th Atoms");
  sprintf(options[OPT_232THPOWER].cDefault,"Primordial Earth: xx TW");
  options[OPT_232THPOWER].iType = 2;
  options[OPT_232THPOWER].iMultiFile = 1;
  options[OPT_232THPOWER].dNeg = EPOWERMAN232TH;  //1e12*1e-7; // cgs
  options[OPT_232THPOWER].dDefault = 0;
  sprintf(options[OPT_232THPOWER].cNeg,"TW");
  fnRead[OPT_232THPOWER] = &Read232ThPower;

  sprintf(options[OPT_238UMASS].cName,"d238UMass");
  sprintf(options[OPT_238UMASS].cDescr,"Initial Mass of 238U");
  sprintf(options[OPT_238UMASS].cDefault,"Primordial Earth: xxx");
  options[OPT_238UMASS].iType = 2;
  options[OPT_238UMASS].iMultiFile = 1;
  options[OPT_238UMASS].dNeg = EMASSMAN238U;
  options[OPT_238UMASS].dDefault = 0;
  sprintf(options[OPT_238UMASS].cNeg,"Earth Masses");
  fnRead[OPT_238UMASS] = &Read238UMass;
  
  sprintf(options[OPT_238UNUM].cName,"d238UNum");
  sprintf(options[OPT_238UNUM].cDescr,"Initial Number of 238U Atoms");
  sprintf(options[OPT_238UNUM].cDefault,"1");
  options[OPT_238UNUM].dDefault = 0;
  options[OPT_238UNUM].iType = 2;
  options[OPT_238UNUM].iMultiFile = 1;
  options[OPT_238UNUM].dNeg = ENUMMAN238U;
  options[OPT_238UNUM].dDefault = 0;
  sprintf(options[OPT_238UNUM].cNeg,"Primordial Earth 238U Numer");
  fnRead[OPT_238UNUM] = &Read238UNum;

  sprintf(options[OPT_238UPOWER].cName,"d238UPower");  //section added PED
  sprintf(options[OPT_238UPOWER].cDescr,"Initial Power Production from 238U Atoms");
  sprintf(options[OPT_238UPOWER].cDefault,"Primordial Earth: xx TW");
  options[OPT_238UPOWER].iType = 2;
  options[OPT_238UPOWER].iMultiFile = 1;
  options[OPT_238UPOWER].dNeg = EPOWERMAN238U;   //1e12*1e-7; // cgs
  options[OPT_238UPOWER].dDefault = 0;
  sprintf(options[OPT_238UPOWER].cNeg,"TW");
  fnRead[OPT_238UPOWER] = &Read238UPower;

  sprintf(options[OPT_235UMASS].cName,"d235UMass");  //PED
  sprintf(options[OPT_235UMASS].cDescr,"Initial Mass of 235U");
  sprintf(options[OPT_235UMASS].cDefault,"Primordial Earth: xxx");
  options[OPT_235UMASS].iType = 2;
  options[OPT_235UMASS].iMultiFile = 1;
  options[OPT_235UMASS].dNeg = EMASSMAN235U;
  options[OPT_235UMASS].dDefault = 0;
  sprintf(options[OPT_235UMASS].cNeg,"Earth Masses");
  fnRead[OPT_235UMASS] = &Read235UMass;
  
  sprintf(options[OPT_235UNUM].cName,"d235UNum");  //PED
  sprintf(options[OPT_235UNUM].cDescr,"Initial Number of 235U Atoms");
  sprintf(options[OPT_235UNUM].cDefault,"1");
  options[OPT_235UNUM].iType = 2;
  options[OPT_235UNUM].iMultiFile = 1;
  options[OPT_235UNUM].dNeg = ENUMMAN235U;
  options[OPT_235UNUM].dDefault = 0;
  sprintf(options[OPT_235UNUM].cNeg,"Primordial Earth 235U Numer");
  fnRead[OPT_235UNUM] = &Read235UNum;

  sprintf(options[OPT_235UPOWER].cName,"d235UPower");  //section added PED
  sprintf(options[OPT_235UPOWER].cDescr,"Initial Power Production from 235U Atoms");
  sprintf(options[OPT_235UPOWER].cDefault,"Primordial Earth: xx TW");
  options[OPT_235UPOWER].iType = 2;
  options[OPT_235UPOWER].iMultiFile = 1;
  options[OPT_235UPOWER].dNeg = EPOWERMAN235U;   //1e12*1e-7; // cgs
  options[OPT_235UPOWER].dDefault = 0;
  sprintf(options[OPT_235UPOWER].cNeg,"TW");
  fnRead[OPT_235UPOWER] = &Read235UPower;

  
}

void ReadOptionsRadheat(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,fnReadOption fnRead[],int iBody) {
  int iOpt;

  for (iOpt=OPTSTARTRADHEAT;iOpt<OPTENDRADHEAT;iOpt++) {
    if (options[iOpt].iType != -1) 
      fnRead[iOpt](body,control,files,&options[iOpt],system,iBody+1);
  }
}
    
/******************* Verify RADHEAT ******************/

void VerifyRotationRadheat(BODY *body,CONTROL *control,OPTIONS *options,char cFile[],int iBody) {
  /* Nothing */
}

void NotMassAndNum(OPTIONS *options,int iMass,int iNum,int iBody) {
    if (options[iMass].iLine[iBody] >= 0 && options[iNum].iLine[iBody] >= 0) 
      DoubleLineExit(options[iMass].cFile[iBody],options[iNum].cFile[iBody],options[iMass].iLine[iBody],options[iNum].iLine[iBody]);
}

void Assign40KNum(BODY *body,OPTIONS *options,double dAge,int iBody) {

  if (options[OPT_40KMASS].iLine[iBody+1] >= 0) {
      //  I think here you need to define body.40KNum bc only the default value of 40Kmass has been chosen by user and set.
      //      printf("40KMass=%e, MASS40K=%e, 40KNum=%e\n",body[iBody].d40KMass,MASS40K,body[iBody].d40KNum);
      body[iBody].d40KNum=body[iBody].d40KMass/(MASS40K);
      printf("40KMass set, body[iBody].d40KNum=%e, ENUMMAN40K=%e\n",body[iBody].d40KNum,ENUMMAN40K);
  }

  if (options[OPT_40KNUM].iLine[iBody+1] >= 0) {
      // Do nothing bc default Num set.
  }

  if (options[OPT_40KPOWER].iLine[iBody+1] >= 0) {
      body[iBody].d40KNum=body[iBody].d40KPower/(ENERGY40K)*(HALFLIFE40K);
      printf("40KPower set, .d40KPower=%e, .d40KNum=%e, ENUMMAN40K=%e\n",body[iBody].d40KPower,body[iBody].d40KNum,ENUMMAN40K);
      //      exit(1);
  }
  
  body[iBody].d40KConst = fd40KConstant(body[iBody].d40KNum,dAge);  //moved from above.
}

void Assign232ThNum(BODY *body,OPTIONS *options,double dAge,int iBody) {

  if (options[OPT_232THMASS].iLine[iBody+1] >= 0) {
      //    printf("232ThMass not implemented.\n");
      //    exit(1);
    body[iBody].d232ThNum=body[iBody].d232ThMass/(MASS232TH);
    printf("232ThMass set, body[iBody].d232ThNum=%e, ENUMMAN232TH=%e\n",body[iBody].d232ThNum,ENUMMAN232TH);
  }

  if (options[OPT_232THNUM].iLine[iBody+1] >= 0) {
      //Do nothing, use default.
  }

  if (options[OPT_232THPOWER].iLine[iBody+1] >= 0) {
      //    printf("232ThPower not implemented.\n");
      //    exit(1);
    body[iBody].d232ThNum=body[iBody].d232ThPower/(ENERGY232TH)*(HALFLIFE232TH);
    printf("232ThPower set, .d232ThPower=%e, .d232ThNum=%e, ENUMMAN232TH=%e\n",body[iBody].d232ThPower,body[iBody].d232ThNum,ENUMMAN232TH);
  }
  body[iBody].d232ThConst = fd232ThConstant(body[iBody].d232ThNum,dAge);
}

void Assign238UNum(BODY *body,OPTIONS *options,double dAge,int iBody) {

  if (options[OPT_238UMASS].iLine[iBody+1] >= 0) {
      /*    printf("238UMass not implemented.\n");
	    exit(1);*/
      body[iBody].d238UNum=body[iBody].d238UMass/(MASS238U);
      printf("238UMass set, body[iBody].d238UNum=%e, ENUMMAN238U=%e\n",body[iBody].d238UNum,ENUMMAN238U);
  }

  if (options[OPT_238UNUM].iLine[iBody+1] >= 0) {

  }

  if (options[OPT_238UPOWER].iLine[iBody+1] >= 0) {
      /*    printf("238UPower not implemented.\n");
	    exit(1);*/
      body[iBody].d238UNum=body[iBody].d238UPower/(ENERGY238U)*(HALFLIFE238U);
      printf("238UPower set, .d238UPower=%e, .d238UNum=%e, ENUMMAN238U=%e\n",body[iBody].d238UPower,body[iBody].d238UNum,ENUMMAN238U);
  }
  body[iBody].d238UConst = fd238UConstant(body[iBody].d238UNum,dAge);
}

void Assign235UNum(BODY *body,OPTIONS *options,double dAge,int iBody) {  //PED

  if (options[OPT_235UMASS].iLine[iBody+1] >= 0) {
      /*    printf("235UMass not implemented.\n");
	    exit(1);*/
    body[iBody].d235UNum=body[iBody].d235UMass/(MASS235U);
    printf("235UMass set, body[iBody].d235UNum=%e, ENUMMAN235U=%e\n",body[iBody].d235UNum,ENUMMAN235U);
  }

  if (options[OPT_235UNUM].iLine[iBody+1] >= 0) {
  }

  if (options[OPT_235UPOWER].iLine[iBody+1] >= 0) {
      /*    printf("235UPower not implemented.\n");
	    exit(1);*/
    body[iBody].d235UNum=body[iBody].d235UPower/(ENERGY235U)*(HALFLIFE235U);
    printf("235UPower set, .d235UPower=%e, .d235UNum=%e, ENUMMAN235U=%e\n",body[iBody].d235UPower,body[iBody].d235UNum,ENUMMAN235U);
  }
  body[iBody].d235UConst = fd235UConstant(body[iBody].d235UNum,dAge);
}

void Verify40K(BODY *body,OPTIONS *options,UPDATE *update,double dAge,fnUpdateVariable ***fnUpdate,int iBody) {

  Assign40KNum(body,options,dAge,iBody);

  update[iBody].iaType[update[iBody].i40K][0] = 1;
  update[iBody].iNumBodies[update[iBody].i40K][0]=1;
  update[iBody].iaBody[update[iBody].i40K][0] = malloc(update[iBody].iNumBodies[update[iBody].i40K][0]*sizeof(int));
  update[iBody].iaBody[update[iBody].i40K][0][0]=iBody;

  update[iBody].pdD40KNumDt = &update[iBody].daDerivProc[update[iBody].i40K][0];
  fnUpdate[iBody][update[iBody].i40K][0] = &fdD40KNumDt;
}

void Verify232Th(BODY *body,OPTIONS *options,UPDATE *update,double dAge,fnUpdateVariable ***fnUpdate,int iBody) {

  Assign232ThNum(body,options,dAge,iBody);

  update[iBody].iaType[update[iBody].i232Th][0] = 1;
  update[iBody].iNumBodies[update[iBody].i232Th][0]=1;
  update[iBody].iaBody[update[iBody].i232Th][0] = malloc(update[iBody].iNumBodies[update[iBody].i232Th][0]*sizeof(int));
  update[iBody].iaBody[update[iBody].i232Th][0][0]=iBody;
  
  update[iBody].pdD232ThNumDt = &update[iBody].daDerivProc[update[iBody].i232Th][0];
  fnUpdate[iBody][update[iBody].i232Th][0] = &fdD232ThNumDt;
}

void Verify238U(BODY *body,OPTIONS *options,UPDATE *update,double dAge,fnUpdateVariable ***fnUpdate,int iBody) {

  Assign238UNum(body,options,dAge,iBody);

  update[iBody].iaType[update[iBody].i238U][0] = 1;
  update[iBody].iNumBodies[update[iBody].i238U][0]=1;
  update[iBody].iaBody[update[iBody].i238U][0] = malloc(update[iBody].iNumBodies[update[iBody].i238U][0]*sizeof(int));
  update[iBody].iaBody[update[iBody].i238U][0][0]=iBody;
  
  update[iBody].pdD238UNumDt = &update[iBody].daDerivProc[update[iBody].i238U][0];
  fnUpdate[iBody][update[iBody].i238U][0] = &fdD238UNumDt;
}

void Verify235U(BODY *body,OPTIONS *options,UPDATE *update,double dAge,fnUpdateVariable ***fnUpdate,int iBody) { //PED

  Assign235UNum(body,options,dAge,iBody);

  update[iBody].iaType[update[iBody].i235U][0] = 1;
  update[iBody].iNumBodies[update[iBody].i235U][0]=1;
  update[iBody].iaBody[update[iBody].i235U][0] = malloc(update[iBody].iNumBodies[update[iBody].i235U][0]*sizeof(int));
  update[iBody].iaBody[update[iBody].i235U][0][0]=iBody;
  
  update[iBody].pdD235UNumDt = &update[iBody].daDerivProc[update[iBody].i235U][0];
  fnUpdate[iBody][update[iBody].i235U][0] = &fdD235UNumDt;
}

/*
double fdGetModuleIntRadheat(UPDATE *update,int iBody) {
  int iModule;

  for (iModule=0;iModule<update[iBody].iNumModules;iModule++) {
    if (update[iBody]->iaModule[iModule] == RADHEAT)
      return iModule;
  }

  fprintf(stderr,"ERROR: Radheat not found for body #%d.\n",iBody);
  exit(1);
}
*/
void fnPropertiesRadheat(BODY *body,int iBody) {
  /* Nothing */
}

void fnForceBehaviorRadheat(BODY *body,EVOLVE *evolve,IO *io,int iBody,int iModule) {
  if (body[iBody].d40KNum < 0.5)
    body[iBody].d40KNum = 0;

  if (body[iBody].d232ThNum < 0.5)
    body[iBody].d232ThNum = 0;

  if (body[iBody].d238UNum < 0.5)
    body[iBody].d238UNum = 0;

  if (body[iBody].d235UNum < 0.5) //PED
    body[iBody].d235UNum = 0;
}

void VerifyRadheat(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,OUTPUT *output,SYSTEM *system,UPDATE *update,fnUpdateVariable ***fnUpdate,int iBody,int iModule) {
  int bRadheat=0;

  /* Cannot set 2 or more of Power, Mass and Number for any isotope */

  /*XXX Need a VerifyOneOfThree subroutine */

  /* Radheat is active for this body if this subroutine is called. */

  if (body[iBody].d40KNum > 0 || body[iBody].d40KMass > 0 || body[iBody].d40KPower > 0) {
    NotMassAndNum(options,OPT_40KMASS,OPT_40KNUM,iBody+1);
    Verify40K(body,options,update,system->dAge,fnUpdate,iBody);
    bRadheat = 1;
  }

  if (body[iBody].d232ThNum > 0 || body[iBody].d232ThMass > 0 || body[iBody].d232ThPower > 0) {
    NotMassAndNum(options,OPT_232THMASS,OPT_232THNUM,iBody+1);
    Verify232Th(body,options,update,system->dAge,fnUpdate,iBody);
    bRadheat = 1;
  }

  if (body[iBody].d238UNum > 0 || body[iBody].d238UMass > 0 || body[iBody].d238UPower > 0) {
      NotMassAndNum(options,OPT_238UMASS,OPT_238UNUM,iBody+1);
      Verify238U(body,options,update,system->dAge,fnUpdate,iBody);
      bRadheat = 1;
  }

  if (body[iBody].d235UNum > 0 || body[iBody].d235UMass > 0 || body[iBody].d235UPower > 0) {  //PED
      NotMassAndNum(options,OPT_235UMASS,OPT_235UNUM,iBody+1);
      Verify235U(body,options,update,system->dAge,fnUpdate,iBody);
      bRadheat = 1;
  }

  if (!bRadheat && control->Io.iVerbose >= VERBINPUT) 
    fprintf(stderr,"WARNING: RADHEAT called for body %s, but no radiogenic species present.\n",body[iBody].cName);

  control->fnForceBehavior[iBody][iModule] = &fnForceBehaviorRadheat;
  control->Evolve.fnAuxProps[iBody][iModule] = &fnPropertiesRadheat;
  control->Evolve.fnBodyCopy[iBody][iModule] = &BodyCopyRadheat;

  output[OUT_SURFENFLUX].fnOutput[iBody][iModule] = &fdSurfEnFluxRadheat;
}


void InitializeModuleRadheat(CONTROL *control,MODULE *module) {
  /* Anything Here? */
}

/**************** RADHEAT update ****************/

void InitializeUpdateRadheat(BODY *body,UPDATE *update,int iBody) {
  /* Initially allow all radiogenic heat sources to be present. If any are 0, 
     or < dMinRadPower, they will me removed from update[iBody] in 
     ForceBehavior.
  */
  if (body[iBody].d40KNum > 0 || body[iBody].d40KMass > 0 || body[iBody].d40KPower > 0) {
    update[iBody].iNumVars++;
    update[iBody].iNum40K++;
  }
  if (body[iBody].d232ThNum > 0 || body[iBody].d232ThMass > 0 || body[iBody].d232ThPower > 0) {
    update[iBody].iNumVars++;
    update[iBody].iNum232Th++;
  }
  if (body[iBody].d238UNum > 0 || body[iBody].d238UMass > 0 || body[iBody].d238UPower > 0) {
    update[iBody].iNumVars++;
    update[iBody].iNum238U++;
  }
  if (body[iBody].d235UNum > 0 || body[iBody].d235UMass > 0 || body[iBody].d235UPower > 0) {  //PED
    update[iBody].iNumVars++;
    update[iBody].iNum235U++;
  }
}

void FinalizeUpdateEccRadheat(BODY *body,UPDATE *update,int *iEqn,int iVar,int iBody) {
  /* Nothing */
}

void FinalizeUpdate40KNumRadheat(BODY *body,UPDATE*update,int *iEqn,int iVar,int iBody) {
  update[iBody].iaModule[iVar][*iEqn] = RAD40K;
  update[iBody].iNum40K = (*iEqn)++;
}

void FinalizeUpdate232ThNumRadheat(BODY *body,UPDATE*update,int *iEqn,int iVar,int iBody) {
  update[iBody].iaModule[iVar][*iEqn] = RAD232TH;
  update[iBody].iNum232Th = (*iEqn)++;
}

void FinalizeUpdate238UNumRadheat(BODY *body,UPDATE*update,int *iEqn,int iVar,int iBody) {
  update[iBody].iaModule[iVar][*iEqn] = RAD238U;
  update[iBody].iNum238U = (*iEqn)++;
}

void FinalizeUpdate235UNumRadheat(BODY *body,UPDATE*update,int *iEqn,int iVar,int iBody) {  //PED
  update[iBody].iaModule[iVar][*iEqn] = RAD235U;
  update[iBody].iNum235U = (*iEqn)++;
}

void FinalizeUpdateOblRadheat(BODY *body,UPDATE *update,int *iEqn,int iVar,int iBody) {
  /* Nothing */
}

void FinalizeUpdateRotRadheat(BODY *body,UPDATE *update,int *iEqn,int iVar,int iBody) {
  /* Nothing */
}

void FinalizeUpdateSemiRadheat(BODY *body,UPDATE *update,int *iEqn,int iVar,int iBody) {
  /* Nothing */
}


/***************** RADHEAT Halts *****************/

/* Minimum 40K Powering? */

int fbHaltMin40KPower(BODY *body,EVOLVE *evolve,HALT *halt,IO *io,UPDATE *update,int iBody) {

  if (body[iBody].d40KPower < halt->dMin40KPower) {
    if (io->iVerbose >= VERBPROG) {
      printf("HALT: %s's 40K Power =  ",body[iBody].cName);
      fprintd(stdout,body[iBody].d40KPower,io->iSciNot,io->iDigits);
      printf(" < ");
      fprintd(stdout,halt->dMin40KPower,io->iSciNot,io->iDigits);
      printf(".\n");
    }
    return 1;
  }
  return 0;
}        

/* Minimum 232Th Powering? */

int fbHaltMin232ThPower(BODY *body,EVOLVE *evolve,HALT *halt,IO *io,UPDATE *update,int iBody) {

  if (body[iBody].d232ThPower < halt->dMin232ThPower) {
    if (io->iVerbose >= VERBPROG) {
      printf("HALT: %s's 232Th Power =  ",body[iBody].cName);
      fprintd(stdout,body[iBody].d232ThPower,io->iSciNot,io->iDigits);
      printf(" < ");
      fprintd(stdout,halt->dMin232ThPower,io->iSciNot,io->iDigits);
      printf(".\n");
    }
    return 1;
  }
  return 0;
}        

/* Minimum 238U Powering? */

int fbHaltMin238UPower(BODY *body,EVOLVE *evolve,HALT *halt,IO *io,UPDATE *update,int iBody) {

  if (body[iBody].d238UPower < halt->dMin238UPower) {
    if (io->iVerbose >= VERBPROG) {
      printf("HALT: %s's 238U Power =  ",body[iBody].cName);
      fprintd(stdout,body[iBody].d238UPower,io->iSciNot,io->iDigits);
      printf(" < ");
      fprintd(stdout,halt->dMin238UPower,io->iSciNot,io->iDigits);
      printf(".\n");
    }
    return 1;
  }
  return 0;
}        

/* Minimum 235U Powering? PED */

int fbHaltMin235UPower(BODY *body,EVOLVE *evolve,HALT *halt,IO *io,UPDATE *update,int iBody) {

  if (body[iBody].d235UPower < halt->dMin235UPower) {
    if (io->iVerbose >= VERBPROG) {
      printf("HALT: %s's 235U Power =  ",body[iBody].cName);
      fprintd(stdout,body[iBody].d235UPower,io->iSciNot,io->iDigits);
      printf(" < ");
      fprintd(stdout,halt->dMin235UPower,io->iSciNot,io->iDigits);
      printf(".\n");
    }
    return 1;
  }
  return 0;
}        

void CountHaltsRadHeat(HALT *halt,int *iHalt) {
  if (halt->dMin40KPower >= 0)
    (iHalt)++;
  if (halt->dMin232ThPower >=0)
    (iHalt)++;
  if (halt->dMin238UPower >= 0)
    (iHalt)++;
  if (halt->dMin235UPower >= 0)  //PED
    (iHalt)++;
}

void VerifyHaltRadheat(BODY *body,CONTROL *control,OPTIONS *options,int iBody,int *iHalt) {

  if (control->Halt[iBody].dMin40KPower > 0)
    control->fnHalt[iBody][(*iHalt)++] = &fbHaltMin40KPower;
  if (control->Halt[iBody].dMin232ThPower > 0)
    control->fnHalt[iBody][(*iHalt)++] = &fbHaltMin232ThPower;
  if (control->Halt[iBody].dMin238UPower > 0)
    control->fnHalt[iBody][(*iHalt)++] = &fbHaltMin238UPower;
  if (control->Halt[iBody].dMin235UPower > 0)  //PED
    control->fnHalt[iBody][(*iHalt)++] = &fbHaltMin235UPower;
}

/************* RADHEAT Outputs ******************/

void HelpOutputRadheat(OUTPUT *output) {
  int iOut;

  printf("\n ------ RADHEAT output ------\n");
  for (iOut=OUTSTARTRADHEAT;iOut<OUTENDRADHEAT;iOut++) 
    WriteHelpOutput(&output[iOut]);
}

/* Potassium */

void Write40KPower(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  /* Get total power from 40K */

  *dTmp = -(*(update[iBody].pdD40KNumDt))*ENERGY40K;

  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else { 
    *dTmp /= fdUnitsPower(units->iTime,units->iMass,units->iLength);
    fsUnitsPower(units,cUnit);
  }
}

void Write40KEnFlux(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  /* Get surface heat flux from 40K */

  *dTmp = body[iBody].d40KPower/(4*PI*body[iBody].dRadius*body[iBody].dRadius);

  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else {
    *dTmp /= fdUnitsEnergyFlux(units->iTime,units->iMass,units->iLength);
    fsUnitsEnergyFlux(units,cUnit);
  }
}

void WriteD40KNumDt(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {

  *dTmp = *(update[iBody].pdD40KNumDt);

  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else {
    *dTmp *= fdUnitsTime(units->iTime);
    fsUnitsRate(units->iTime,cUnit);
  }
}

void Write40KTimescale(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  *dTmp=-1;
}

void Write40KMass(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {

  *dTmp = body[iBody].d40KNum*MASS40K;

  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else {
    *dTmp /= fdUnitsMass(units->iMass);
    fsUnitsMass(units->iMass,cUnit);
  }
}

void WriteD40KPowerDt(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {

  *dTmp = -1;
}

void Write40KNum(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  *dTmp = body[iBody].d40KNum;

  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  }
  /* else nothing, as it is a number */
}

/* Thorium */

void Write232ThPower(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  /* Get total heat from 232Th */
  *dTmp = -(*(update[iBody].pdD232ThNumDt))*ENERGY232TH;

  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else {
    *dTmp /= fdUnitsPower(units->iTime,units->iMass,units->iLength);
    fsUnitsPower(units,cUnit);
  }
}

void Write232ThEnFlux(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  /* Get surface heat flux from 232Th */
  *dTmp = body[iBody].d232ThPower/(4*PI*body[iBody].dRadius*body[iBody].dRadius);

  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else {
    *dTmp /= fdUnitsEnergyFlux(units->iTime,units->iMass,units->iLength);
    fsUnitsEnergyFlux(units,cUnit);
  }
}

void WriteD232ThNumDt(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  *dTmp = *(update[iBody].pdD232ThNumDt);

  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else {
    *dTmp *= fdUnitsTime(units->iTime);
    fsUnitsRate(units->iTime,cUnit);
  }
}

void WritedD232ThPowerDt(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  *dTmp=-1;
}

void Write232ThTimescale(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  *dTmp=-1;
}

void Write232ThMass(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  *dTmp = body[iBody].d232ThNum*MASS232TH;

  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else {
    *dTmp /= fdUnitsMass(units->iMass);
    fsUnitsMass(units->iMass,cUnit);
  }
}

void Write232ThNum(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  *dTmp = body[iBody].d232ThNum;

  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  }
  /* else nothing, as it is a number */
}

/* Uranium 238 */

void Write238UPower(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  /* Get total power from 238U */
  *dTmp = -(*(update[iBody].pdD238UNumDt))*ENERGY238U;

  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else {
    *dTmp /= fdUnitsPower(units->iTime,units->iMass,units->iLength);
    fsUnitsPower(units,cUnit);
  }
}

void Write238UEnFlux(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  /* Get surface heat flux from 238U */
  *dTmp = body[iBody].d238UPower/(4*PI*body[iBody].dRadius*body[iBody].dRadius);

  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else {
    *dTmp /= fdUnitsEnergyFlux(units->iTime,units->iMass,units->iLength);
    fsUnitsEnergyFlux(units,cUnit);
  }
}

void WriteD238UNumDt(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  *dTmp = *(update[iBody].pdD238UNumDt);

  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else {
    *dTmp *= fdUnitsTime(units->iTime);
    fsUnitsRate(units->iTime,cUnit);
  }
}


void WritedD238UPowerDt(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  *dTmp=-1;
}

void Write238UTimescale(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  *dTmp=-1;
}

void Write238UMass(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  *dTmp = body[iBody].d238UNum*MASS238U;

  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else {
    *dTmp /= fdUnitsMass(units->iMass);
    fsUnitsMass(units->iMass,cUnit);
  }
}

void Write238UNum(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  *dTmp = body[iBody].d238UNum;

  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  }
  /* else nothing, as it is a number */
}

/* Uranium 235 PED */

void Write235UPower(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  /* Get total power from 235U */
  *dTmp = -(*(update[iBody].pdD235UNumDt))*ENERGY235U;

  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else {
    *dTmp /= fdUnitsPower(units->iTime,units->iMass,units->iLength);
    fsUnitsPower(units,cUnit);
  }
}

void Write235UEnFlux(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  /* Get surface heat flux from 235U */
  *dTmp = body[iBody].d235UPower/(4*PI*body[iBody].dRadius*body[iBody].dRadius);

  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else {
    *dTmp /= fdUnitsEnergyFlux(units->iTime,units->iMass,units->iLength);
    fsUnitsEnergyFlux(units,cUnit);
  }
}

void WriteD235UNumDt(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  *dTmp = *(update[iBody].pdD235UNumDt);

  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else {
    *dTmp *= fdUnitsTime(units->iTime);
    fsUnitsRate(units->iTime,cUnit);
  }
}


void WritedD235UPowerDt(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  *dTmp=-1;
}

void Write235UTimescale(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  *dTmp=-1;
}

void Write235UMass(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  *dTmp = body[iBody].d235UNum*MASS235U;

  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else {
    *dTmp /= fdUnitsMass(units->iMass);
    fsUnitsMass(units->iMass,cUnit);
  }
}

void Write235UNum(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  *dTmp = body[iBody].d235UNum;

  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  }
  /* else nothing, as it is a number */
}

/* Totals */
void WriteRadPower(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  /* Total Radiogenic Power Production */
  *dTmp = fdInternalPowerRadheat(body,system,update,iBody,iBody);

  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else {
    *dTmp /= fdUnitsPower(units->iTime,units->iMass,units->iLength);
    fsUnitsPower(units,cUnit);
  }
}

void WriteSurfEnFluxRadheat(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  /* Radiogenic Surface Energy Flux */
  *dTmp = fdSurfEnFluxRadheat(body,system,update,iBody,iBody);

  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else {
    *dTmp /= fdUnitsEnergyFlux(units->iTime,units->iMass,units->iLength);
    fsUnitsEnergyFlux(units,cUnit);
  }
}

void InitializeOutputRadheat(OUTPUT *output,fnWriteOutput fnWrite[]) {

  /* Potassium */
    //  PED:  Do these default numbers matter??  If so they need to be changed.
  sprintf(output[OUT_40KPOWER].cName,"40KPower");
  sprintf(output[OUT_40KPOWER].cDescr,"Total Power Generated by 40K");
  sprintf(output[OUT_40KPOWER].cNeg,"TW");
  output[OUT_40KPOWER].bNeg = 1;
  output[OUT_40KPOWER].dNeg = 1e-19; // ergs/s -> TW 
  output[OUT_40KPOWER].iNum = 1;
  fnWrite[OUT_40KPOWER] = &Write40KPower;
  
  sprintf(output[OUT_40KENFLUX].cName,"40KEnFlux");
  sprintf(output[OUT_40KENFLUX].cDescr,"Surface Energy Flux from 40K");
  sprintf(output[OUT_40KENFLUX].cNeg,"W/m^2");
  output[OUT_40KENFLUX].bNeg = 1;
  output[OUT_40KENFLUX].dNeg = 1e-3;
  output[OUT_40KENFLUX].iNum = 1;
  fnWrite[OUT_40KENFLUX] = &Write40KEnFlux;

  sprintf(output[OUT_40KDPOWERDT].cName,"D40KPowerDt");
  sprintf(output[OUT_40KDPOWERDT].cDescr,"Time Rate of Change of 40K Power Generation");
  sprintf(output[OUT_40KDPOWERDT].cNeg,"TW/Gyr");
  output[OUT_40KDPOWERDT].bNeg = 1;
  output[OUT_40KDPOWERDT].dNeg = 1e12/1e9; /* XXX */
  output[OUT_40KDPOWERDT].iNum = 1;
  fnWrite[OUT_40KDPOWERDT] = &WriteD40KPowerDt;
  
  sprintf(output[OUT_40KDNUMDT].cName,"D40KNumDt");
  sprintf(output[OUT_40KDNUMDT].cDescr,"Time Rate of Change of 40K Heat Generation");
  sprintf(output[OUT_40KDNUMDT].cNeg,"/Gyr");
  output[OUT_40KDNUMDT].bNeg = 1;
  output[OUT_40KDNUMDT].dNeg = YEARSEC*1e9; 
  output[OUT_40KDNUMDT].iNum = 1;
  fnWrite[OUT_40KDNUMDT] = &WriteD40KNumDt;
  
  sprintf(output[OUT_40KTIME].cName,"40KTimescale");
  sprintf(output[OUT_40KTIME].cDescr,"Timescale for 40K Power Generation");
  sprintf(output[OUT_40KTIME].cNeg,"Gyr");
  output[OUT_40KTIME].bNeg = 1;
  output[OUT_40KTIME].dNeg = YEARSEC*1e9;
  output[OUT_40KTIME].iNum = 1;
  fnWrite[OUT_40KTIME] = &Write40KTimescale;
  
  sprintf(output[OUT_40KMASS].cName,"40KMass");
  sprintf(output[OUT_40KMASS].cDescr,"Total Mass of 40K");
  sprintf(output[OUT_40KMASS].cNeg,"Earth Masses");
  output[OUT_40KMASS].bNeg = 1;
  output[OUT_40KMASS].dNeg = MEARTH;
  output[OUT_40KMASS].iNum = 1;
  fnWrite[OUT_40KMASS] = &Write40KMass;
  
  sprintf(output[OUT_40KNUM].cName,"40KNumber");
  sprintf(output[OUT_40KNUM].cDescr,"Total Number of 40K Atoms");
  sprintf(output[OUT_40KNUM].cNeg,"Initial Primordial Earth Number");
  output[OUT_40KNUM].bNeg = 1;
  output[OUT_40KNUM].dNeg = EMASSMAN40K/MASS40K;
  output[OUT_40KNUM].iNum = 1;
  fnWrite[OUT_40KNUM] = &Write40KNum;

  /* Thorium */

  sprintf(output[OUT_232THPOWER].cName,"232ThPower");
  sprintf(output[OUT_232THPOWER].cDescr,"Total Power Generated by 232Th");
  sprintf(output[OUT_232THPOWER].cNeg,"TW");
  output[OUT_232THPOWER].bNeg = 1;
  output[OUT_232THPOWER].dNeg = 1e-19; 
  output[OUT_232THPOWER].iNum = 1;
  fnWrite[OUT_232THPOWER] = &Write232ThPower;
  
  sprintf(output[OUT_232THENFLUX].cName,"232ThEnFlux");
  sprintf(output[OUT_232THENFLUX].cDescr,"Surface Energy Flux from 232Th");
  sprintf(output[OUT_232THENFLUX].cNeg,"W/m^2");
  output[OUT_232THENFLUX].bNeg = 1;
  output[OUT_232THENFLUX].dNeg = 1e-3;
  output[OUT_232THENFLUX].iNum = 1;
  fnWrite[OUT_232THENFLUX] = &Write232ThEnFlux;

  sprintf(output[OUT_232THDNUMDT].cName,"D232ThNumDt");
  sprintf(output[OUT_232THDNUMDT].cDescr,"Time Rate of Change of the number of 232Th nuclei");
  sprintf(output[OUT_232THDNUMDT].cNeg,"/Gyr");
  output[OUT_232THDNUMDT].bNeg = 1;
  output[OUT_232THDNUMDT].dNeg = YEARSEC*1e9;  
  output[OUT_232THDNUMDT].iNum = 1;
  fnWrite[OUT_232THDNUMDT] = &WriteD232ThNumDt;

  sprintf(output[OUT_232THTIME].cName,"232ThTimescale");
  sprintf(output[OUT_232THTIME].cDescr,"Timescale for 232Th Power Generation");
  sprintf(output[OUT_232THTIME].cNeg,"Gyr");
  output[OUT_232THTIME].bNeg = 1;
  output[OUT_232THTIME].dNeg = YEARSEC*1e9;
  output[OUT_232THTIME].iNum = 1;
  fnWrite[OUT_232THTIME] = &Write232ThTimescale;
  
  sprintf(output[OUT_232THMASS].cName,"232ThMass");
  sprintf(output[OUT_232THMASS].cDescr,"Total Mass of 232Th");
  sprintf(output[OUT_232THMASS].cNeg,"Earth Masses");
  output[OUT_232THMASS].bNeg = 1;
  output[OUT_232THMASS].dNeg = MEARTH;
  output[OUT_232THMASS].iNum = 1;
  fnWrite[OUT_232THMASS] = &Write232ThMass;
  
  sprintf(output[OUT_232THNUM].cName,"232ThNumber");
  sprintf(output[OUT_232THNUM].cDescr,"Total Number of 232Th Atoms");
  sprintf(output[OUT_232THNUM].cNeg,"Initial Primordial Earth Number");
  output[OUT_232THNUM].bNeg = 1;
  output[OUT_232THNUM].dNeg = EMASSMAN232TH/MASS232TH;
  output[OUT_232THNUM].iNum = 1;
  fnWrite[OUT_232THNUM] = &Write232ThNum;

  /* Uranium 238 */

  sprintf(output[OUT_238UPOWER].cName,"238UPower");
  sprintf(output[OUT_238UPOWER].cDescr,"Total Power Generated by 238U");
  sprintf(output[OUT_238UPOWER].cNeg,"TW");
  output[OUT_238UPOWER].bNeg = 1;
  output[OUT_238UPOWER].dNeg = 1e-19; 
  output[OUT_238UPOWER].iNum = 1;
  fnWrite[OUT_238UPOWER] = &Write238UPower;
  
  sprintf(output[OUT_238UENFLUX].cName,"238UEnFlux");
  sprintf(output[OUT_238UENFLUX].cDescr,"Surface Energy Flux due to 238U");
  sprintf(output[OUT_238UENFLUX].cNeg,"W/m^2");
  output[OUT_238UENFLUX].bNeg = 1;
  output[OUT_238UENFLUX].dNeg = 1e-3;
  output[OUT_238UENFLUX].iNum = 1;
  fnWrite[OUT_238UENFLUX] = &Write238UEnFlux;

  sprintf(output[OUT_238UDNUMDT].cName,"D238UNumDt");
  sprintf(output[OUT_238UDNUMDT].cDescr,"Time Rate of Change of the Number of 238U Nuclei");
  sprintf(output[OUT_238UDNUMDT].cNeg,"Gyr");
  output[OUT_238UDNUMDT].bNeg = 1;
  output[OUT_238UDNUMDT].dNeg = YEARSEC*1e9;
  output[OUT_238UDNUMDT].iNum = 1;
  fnWrite[OUT_238UDNUMDT] = &WriteD238UNumDt;

  sprintf(output[OUT_238UTIME].cName,"238UTimescale");
  sprintf(output[OUT_238UTIME].cDescr,"Timescale for 238U Power Generation");
  sprintf(output[OUT_238UTIME].cNeg,"Gyr");
  output[OUT_238UTIME].bNeg = 1;
  output[OUT_238UTIME].dNeg = YEARSEC*1e9;
  output[OUT_238UTIME].iNum = 1;
  fnWrite[OUT_238UTIME] = &Write238UTimescale;
  
  sprintf(output[OUT_238UMASS].cName,"238UMass");
  sprintf(output[OUT_238UMASS].cDescr,"Total Mass of 238U");
  sprintf(output[OUT_238UMASS].cNeg,"Earth Masses");
  output[OUT_238UMASS].bNeg = 1;
  output[OUT_238UMASS].dNeg = MEARTH;
  output[OUT_238UMASS].iNum = 1;
  fnWrite[OUT_238UMASS] = &Write238UMass;
  
  sprintf(output[OUT_238UNUM].cName,"238UNumber");
  sprintf(output[OUT_238UNUM].cDescr,"Total Number of 238U Atoms");
  sprintf(output[OUT_238UNUM].cNeg,"Initial Primordial Earth Number");
  output[OUT_238UNUM].bNeg = 1;
  output[OUT_238UNUM].dNeg = 1;
  output[OUT_238UNUM].iNum = 1;
  fnWrite[OUT_238UNUM] = &Write238UNum;


    /* Uranium 235 PED */
  
  sprintf(output[OUT_235UPOWER].cName,"235UPower");
  sprintf(output[OUT_235UPOWER].cDescr,"Total Power Generated by 235U");
  sprintf(output[OUT_235UPOWER].cNeg,"TW");
  output[OUT_235UPOWER].bNeg = 1;
  output[OUT_235UPOWER].dNeg = 1e-19; 
  output[OUT_235UPOWER].iNum = 1;
  fnWrite[OUT_235UPOWER] = &Write235UPower;
  
  sprintf(output[OUT_235UENFLUX].cName,"235UEnFlux");
  sprintf(output[OUT_235UENFLUX].cDescr,"Surface Energy Flux due to 235U");
  sprintf(output[OUT_235UENFLUX].cNeg,"W/m^2");
  output[OUT_235UENFLUX].bNeg = 1;
  output[OUT_235UENFLUX].dNeg = 1e-3;
  output[OUT_235UENFLUX].iNum = 1;
  fnWrite[OUT_235UENFLUX] = &Write235UEnFlux;

  sprintf(output[OUT_235UDNUMDT].cName,"D235UNumDt");
  sprintf(output[OUT_235UDNUMDT].cDescr,"Time Rate of Change of the Number of 235U Nuclei");
  sprintf(output[OUT_235UDNUMDT].cNeg,"Gyr");
  output[OUT_235UDNUMDT].bNeg = 1;
  output[OUT_235UDNUMDT].dNeg = YEARSEC*1e9;
  output[OUT_235UDNUMDT].iNum = 1;
  fnWrite[OUT_235UDNUMDT] = &WriteD235UNumDt;

  sprintf(output[OUT_235UTIME].cName,"235UTimescale");
  sprintf(output[OUT_235UTIME].cDescr,"Timescale for 235U Power Generation");
  sprintf(output[OUT_235UTIME].cNeg,"Gyr");
  output[OUT_235UTIME].bNeg = 1;
  output[OUT_235UTIME].dNeg = YEARSEC*1e9;
  output[OUT_235UTIME].iNum = 1;
  fnWrite[OUT_235UTIME] = &Write235UTimescale;
  
  sprintf(output[OUT_235UMASS].cName,"235UMass");
  sprintf(output[OUT_235UMASS].cDescr,"Total Mass of 235U");
  sprintf(output[OUT_235UMASS].cNeg,"Earth Masses");
  output[OUT_235UMASS].bNeg = 1;
  output[OUT_235UMASS].dNeg = MEARTH;
  output[OUT_235UMASS].iNum = 1;
  fnWrite[OUT_235UMASS] = &Write235UMass;
  
  sprintf(output[OUT_235UNUM].cName,"235UNumber");
  sprintf(output[OUT_235UNUM].cDescr,"Total Number of 235U Atoms");
  sprintf(output[OUT_235UNUM].cNeg,"Initial Primordial Earth Number");
  output[OUT_235UNUM].bNeg = 1;
  output[OUT_235UNUM].dNeg = 1;
  output[OUT_235UNUM].iNum = 1;
  fnWrite[OUT_235UNUM] = &Write235UNum;
  
      
  /* Totals */
  
  sprintf(output[OUT_RADPOWER].cName,"RadPower");
  sprintf(output[OUT_RADPOWER].cDescr,"Total Power Generated by Radiogenic Nuclides");
  sprintf(output[OUT_RADPOWER].cNeg,"TW");
  output[OUT_RADPOWER].bNeg = 1;
  output[OUT_RADPOWER].dNeg = 1e-19; 
  output[OUT_RADPOWER].iNum = 1;
  fnWrite[OUT_RADPOWER] = &WriteRadPower;

  sprintf(output[OUT_SURFENFLUXRADHEAT].cName,"SurfEnFluxRadheat");
  sprintf(output[OUT_SURFENFLUXRADHEAT].cDescr,"Total Power Generated by Radiogenic Nuclides");
  sprintf(output[OUT_SURFENFLUXRADHEAT].cNeg,"W/m^2");
  output[OUT_SURFENFLUXRADHEAT].bNeg = 1;
  output[OUT_SURFENFLUXRADHEAT].dNeg = 1e-3; 
  output[OUT_SURFENFLUXRADHEAT].iNum = 1;
  fnWrite[OUT_SURFENFLUXRADHEAT] = &WriteSurfEnFluxRadheat;

}

void FinalizeOutputFunctionRadheat(OUTPUT *output,int iBody,int iModule) {
  output[OUT_SURFENFLUX].fnOutput[iBody][iModule] = &fdSurfEnFluxRadheat;
}

/************ RADHEAT Logging Functions **************/

void LogOptionsRadheat(CONTROL *control, FILE *fp) {

  /* Anything here?

  fprintf(fp,"-------- RADHEAT Options -----\n\n");
  */
}

void LogRadheat(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UPDATE *update,fnWriteOutput fnWrite[],FILE *fp) {

  /* Anything here? 
  int iOut;

  fprintf(fp,"\n----- RADHEAT PARAMETERS ------\n");
  for (iOut=OUTSTARTRADHEAT;iOut<OUTBODYSTARTRADHEAT;iOut++) {
    if (output[iOut].iNum > 0)
      WriteLogEntry(control,output[iOut],body,system,fnWrite[iOut],fp,update,0);
  }
  */
}

void LogBodyRadheat(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UPDATE *update,fnWriteOutput fnWrite[],FILE *fp,int iBody) {
  int iOut;

  fprintf(fp,"----- RADHEAT PARAMETERS (%s)------\n",body[iBody].cName);
  for (iOut=OUTSTARTRADHEAT;iOut<OUTENDRADHEAT;iOut++) {
    if (output[iOut].iNum > 0) 
      WriteLogEntry(body,control,&output[iOut],system,update,fnWrite[iOut],fp,iBody);
    /*
    fprintf(fp,"40K Constant: ");
    fprintd(fp,body[iBody].d40KConst,control->Io.iSciNot,control->Io.iDigits);
    fprintf(fp,"\n");

    fprintf(fp,"232Th Constant: ");
    fprintd(fp,body[iBody].d232ThConst,control->Io.iSciNot,control->Io.iDigits);
    fprintf(fp,"\n");

    fprintf(fp,"238U Constant: ");
    fprintd(fp,body[iBody].d238UConst,control->Io.iSciNot,control->Io.iDigits);
    fprintf(fp,"\n");
    */
  }
}

void AddModuleRadheat(MODULE *module,int iBody,int iModule) {

  module->iaModule[iBody][iModule] = RADHEAT;

  module->fnInitializeControl[iBody][iModule] = &InitializeControlRadheat;
  module->fnInitializeUpdateTmpBody[iBody][iModule] = &InitializeUpdateTmpBodyRadheat;

  module->fnCountHalts[iBody][iModule] = &CountHaltsRadHeat;
  module->fnReadOptions[iBody][iModule] = &ReadOptionsRadheat;
  module->fnLogBody[iBody][iModule] = &LogBodyRadheat;
  module->fnVerify[iBody][iModule] = &VerifyRadheat;
  module->fnVerifyHalt[iBody][iModule] = &VerifyHaltRadheat;
  module->fnVerifyRotation[iBody][iModule] = &VerifyRotationRadheat;

  module->fnInitializeBody[iBody][iModule] = &InitializeBodyRadheat;
  module->fnInitializeUpdate[iBody][iModule] = &InitializeUpdateRadheat;
  module->fnFinalizeUpdate40KNum[iBody][iModule] = &FinalizeUpdate40KNumRadheat;
  module->fnFinalizeUpdate232ThNum[iBody][iModule] = &FinalizeUpdate232ThNumRadheat;
  module->fnFinalizeUpdate238UNum[iBody][iModule] = &FinalizeUpdate238UNumRadheat;
  module->fnFinalizeUpdate235UNum[iBody][iModule] = &FinalizeUpdate235UNumRadheat;  //PED

  // Now include other primary variables not used by RADHEAT 
  module->fnFinalizeUpdateEcc[iBody][iModule] = &FinalizeUpdateEccRadheat;
  module->fnFinalizeUpdateObl[iBody][iModule] = &FinalizeUpdateOblRadheat;
  module->fnFinalizeUpdateRot[iBody][iModule] = &FinalizeUpdateRotRadheat;
  module->fnFinalizeUpdateSemi[iBody][iModule] = &FinalizeUpdateSemiRadheat;

  //module->fnIntializeOutputFunction[iBody][iModule] = &InitializeOutputFunctionRadheat;
  module->fnFinalizeOutputFunction[iBody][iModule] = &FinalizeOutputFunctionRadheat;

}

/************* RADHEAT Functions ************/

// N = N_0 * exp(-t/lambda)
// dN/dt = -(N_0/lambda) * exp(-t/lambda)

double fdInternalPowerRadheat(BODY *body,SYSTEM *system,UPDATE *update,int iBody,int iFoo) {

  return -(*(update[iBody].pdD238UNumDt))*ENERGY238U - (*(update[iBody].pdD235UNumDt))*ENERGY235U - (*(update[iBody].pdD232ThNumDt))*ENERGY232TH - (*(update[iBody].pdD40KNumDt))*ENERGY40K;
}

/* This is part of output[OUT_SURFENFLUX].fnOutput */
double fdSurfEnFluxRadheat(BODY *body,SYSTEM *system,UPDATE *update,int iBody,int iFoo) {
  double dPower;

  dPower = fdInternalPowerRadheat(body,system,update,iBody,iFoo);

  return dPower/(4*PI*body[iBody].dRadius*body[iBody].dRadius);
}

double fdRadheatConst(double dNum,double dAge,double dHalfLife) {
  return dNum/(exp(-dAge/dHalfLife));
}

// PED COMMENT: shouldn't dPower be dNum below?  Prolly doesn't really matter it's a dummy anyway.
/*double fd40KConstant(double dPower,double dAge) {  
    return fdRadheatConst(dPower,dAge,HALFLIFE40K);   //redirects to fdRadheatConst
    }*/
double fd40KConstant(double dNum,double dAge) {  
    return fdRadheatConst(dNum,dAge,HALFLIFE40K);   //redirects to fdRadheatConst
}

double fd232ThConstant(double dNum,double dAge) {  //PED: changed dPower to dNum.
  return fdRadheatConst(dNum,dAge,HALFLIFE232TH);  //redirects to fdRadheatConst
}

double fd238UConstant(double dNum,double dAge) {  //PED: changed dPower to dNum.
  return fdRadheatConst(dNum,dAge,HALFLIFE238U);  //redirects to fdRadheatConst
}

double fd235UConstant(double dNum,double dAge) {  //PED: changed dPower to dNum.
  return fdRadheatConst(dNum,dAge,HALFLIFE235U);  //redirects to fdRadheatConst
}

double fdDNumRadDt(double dConst,double dHalfLife,double dAge) {
  return -dConst/dHalfLife*exp(-dAge/dHalfLife);
}

double fdRadPower(double dConst,double dHalfLife,double dAge) {
    return dConst*exp(-dHalfLife/dAge);                      //Here const=N_0*energy_i*lambda, where energy_i=erg/num.
}

double fdRadEnFlux(double dConst,double dHalfLife,double dAge,double dRadius) {
  return fdRadPower(dConst,dHalfLife,dAge)/(4*PI*dRadius*dRadius);
}

double fd40KPower(BODY *body,SYSTEM *system,int *iaBody,int iBody) {
  return fdRadPower(body[iBody].d40KConst,HALFLIFE40K,system->dAge);   //redirects to fdRadPower
}

double fd232ThPower(BODY *body,SYSTEM *system,int iBody) {
  return fdRadPower(body[iBody].d232ThConst,HALFLIFE232TH,system->dAge);    //redirects to fdRadPower
}

double fd238UPower(BODY *body,SYSTEM *system,int iBody) {
  return fdRadPower(body[iBody].d238UConst,HALFLIFE238U,system->dAge);    //redirects to fdRadPower
}

double fd235UPower(BODY *body,SYSTEM *system,int iBody) {
  return fdRadPower(body[iBody].d235UConst,HALFLIFE235U,system->dAge);    //redirects to fdRadPower
}

double fd40KEnFlux(BODY *body,SYSTEM *system,int *iaBody,int iBody) {
  return fdRadEnFlux(body[iBody].d40KConst,HALFLIFE40K,system->dAge,body[iBody].dRadius);
}

double fd232ThEnFlux(BODY *body,SYSTEM *system,int iBody) {
  return fdRadEnFlux(body[iBody].d232ThConst,HALFLIFE232TH,system->dAge,body[iBody].dRadius);
}

double fd238UEnFlux(BODY *body,SYSTEM *system,int iBody) {
  return fdRadEnFlux(body[iBody].d238UConst,HALFLIFE238U,system->dAge,body[iBody].dRadius);
}

double fd235UEnFlux(BODY *body,SYSTEM *system,int iBody) {
  return fdRadEnFlux(body[iBody].d235UConst,HALFLIFE235U,system->dAge,body[iBody].dRadius);
}

double fdD40KNumDt(BODY *body,SYSTEM *system,int *iaBody,int iNumBodies) {
  return fdDNumRadDt(body[iaBody[0]].d40KConst,HALFLIFE40K,system->dAge);
}

double fdD232ThNumDt(BODY *body,SYSTEM *system,int *iaBody,int iNumBodies) {
  return fdDNumRadDt(body[iaBody[0]].d232ThConst,HALFLIFE232TH,system->dAge);
}

double fdD238UNumDt(BODY *body,SYSTEM *system,int *iaBody,int iNumBodies) {
  return fdDNumRadDt(body[iaBody[0]].d238UConst,HALFLIFE238U,system->dAge);
}

double fdD235UNumDt(BODY *body,SYSTEM *system,int *iaBody,int iNumBodies) {
  return fdDNumRadDt(body[iaBody[0]].d235UConst,HALFLIFE235U,system->dAge);
}

/* Number derivatives? */
