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
  dest[iBody].d40KNumMan = src[iBody].d40KNumMan;
  dest[iBody].d40KConstMan = src[iBody].d40KConstMan;

  dest[iBody].d232ThNumMan = src[iBody].d232ThNumMan;
  dest[iBody].d232ThConstMan = src[iBody].d232ThConstMan;

  dest[iBody].d238UNumMan = src[iBody].d238UNumMan;
  dest[iBody].d238UConstMan = src[iBody].d238UConstMan;

  dest[iBody].d235UNumMan = src[iBody].d235UNumMan;  //PED
  dest[iBody].d235UConstMan = src[iBody].d235UConstMan; //PED
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
      body[iFile-1].d40KPowerMan = dTmp*dNegativeDouble(*options,files->Infile[iFile].cIn,control->Io.iVerbose);
   else
       //      body[iFile-1].d40KPowerMan = dTmp*fdUnitsMass(control->Units[iFile].iMass);
       //CHANGED units Mass to Power.
       body[iFile-1].d40KPowerMan = dTmp*fdUnitsPower(control->Units[iFile].iTime,control->Units[iFile].iMass,control->Units[iFile].iLength);
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else
      if (iFile > 0)  //if line num not ge 0, then if iFile gt 0, then set default.
      body[iFile-1].d40KPowerMan = options->dDefault;
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
      body[iFile-1].d40KMassMan = dTmp*dNegativeDouble(*options,files->Infile[iFile].cIn,control->Io.iVerbose);
   else
      body[iFile-1].d40KMassMan = dTmp*fdUnitsMass(control->Units[iFile].iMass);
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else
    if (iFile > 0)
      body[iFile-1].d40KMassMan = options->dDefault;
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
	body[iFile-1].d40KNumMan = dTmp*dNegativeDouble(*options,files->Infile[iFile].cIn,control->Io.iVerbose);  //dTmp=input value, dNegativeDouble=-dNeg (default Value).
    else
	body[iFile-1].d40KNumMan = dTmp;   //units of num are num!
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else
    if (iFile > 0)
      body[iFile-1].d40KNumMan = options->dDefault;
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
      body[iFile-1].d232ThPowerMan = dTmp*dNegativeDouble(*options,files->Infile[iFile].cIn,control->Io.iVerbose);
   else
       //      body[iFile-1].d232ThPower = dTmp*fdUnitsMass(control->Units[iFile].iMass);
       body[iFile-1].d232ThPowerMan = dTmp*fdUnitsPower(control->Units[iFile].iTime,control->Units[iFile].iMass,control->Units[iFile].iLength);
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else
    if (iFile > 0)
      body[iFile-1].d232ThPowerMan = options->dDefault;
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
      body[iFile-1].d232ThMassMan = dTmp*dNegativeDouble(*options,files->Infile[iFile].cIn,control->Io.iVerbose);
   else
      body[iFile-1].d232ThMassMan = dTmp*fdUnitsMass(control->Units[iFile].iMass);
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else
    if (iFile > 0)
      body[iFile-1].d232ThMassMan = options->dDefault;
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
      body[iFile-1].d232ThNumMan = dTmp*dNegativeDouble(*options,files->Infile[iFile].cIn,control->Io.iVerbose);
   else
      body[iFile-1].d232ThNumMan = dTmp;
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else
    if (iFile > 0)
      body[iFile-1].d232ThNumMan = options->dDefault;
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
      body[iFile-1].d238UPowerMan = dTmp*dNegativeDouble(*options,files->Infile[iFile].cIn,control->Io.iVerbose);
   else
       //      body[iFile-1].d238UPower = dTmp*fdUnitsMass(control->Units[iFile].iMass);
       body[iFile-1].d238UPowerMan = dTmp*fdUnitsPower(control->Units[iFile].iTime,control->Units[iFile].iMass,control->Units[iFile].iLength);
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else
    if (iFile > 0)
      body[iFile-1].d238UPowerMan = options->dDefault;
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
      body[iFile-1].d238UMassMan = dTmp*dNegativeDouble(*options,files->Infile[iFile].cIn,control->Io.iVerbose);
   else
      body[iFile-1].d238UMassMan = dTmp*fdUnitsMass(control->Units[iFile].iMass);
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else
    if (iFile > 0)
      body[iFile-1].d238UMassMan = options->dDefault;
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
      body[iFile-1].d238UNumMan = dTmp*dNegativeDouble(*options,files->Infile[iFile].cIn,control->Io.iVerbose);
   else
      body[iFile-1].d238UNumMan = dTmp;
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else
    if (iFile > 0)
      body[iFile-1].d238UNumMan = options->dDefault;
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
      body[iFile-1].d235UPowerMan = dTmp*dNegativeDouble(*options,files->Infile[iFile].cIn,control->Io.iVerbose);
   else
       //      body[iFile-1].d235UPower = dTmp*fdUnitsMass(control->Units[iFile].iMass);
       body[iFile-1].d235UPowerMan = dTmp*fdUnitsPower(control->Units[iFile].iTime,control->Units[iFile].iMass,control->Units[iFile].iLength);
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else
    if (iFile > 0)
      body[iFile-1].d235UPowerMan = options->dDefault;
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
      body[iFile-1].d235UMassMan = dTmp*dNegativeDouble(*options,files->Infile[iFile].cIn,control->Io.iVerbose);
   else
      body[iFile-1].d235UMassMan = dTmp*fdUnitsMass(control->Units[iFile].iMass);
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else
    if (iFile > 0)
      body[iFile-1].d235UMassMan = options->dDefault;
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
	  body[iFile-1].d235UNumMan = dTmp*dNegativeDouble(*options,files->Infile[iFile].cIn,control->Io.iVerbose);
      else
	  body[iFile-1].d235UNumMan = dTmp;
      UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else
      if (iFile > 0)
	  body[iFile-1].d235UNumMan = options->dDefault;
}

/* Initiatlize Input Options */

void InitializeOptionsRadheat(OPTIONS *options,fnReadOption fnRead[]) {
  int iOpt,iFile;

  sprintf(options[OPT_40KMASSMAN].cName,"d40KMassMan");
  sprintf(options[OPT_40KMASSMAN].cDescr,"Initial Mass of 40K");
  sprintf(options[OPT_40KMASSMAN].cDefault,"Primordial Earth: xxx");
  options[OPT_40KMASSMAN].iType = 2;
  options[OPT_40KMASSMAN].iMultiFile = 1;
  options[OPT_40KMASSMAN].dNeg = EMASSMAN40K;
  options[OPT_40KMASSMAN].dDefault = 0;  //DO THIS FOR ALL!!!
  sprintf(options[OPT_40KMASSMAN].cNeg,"Earth Masses");
  fnRead[OPT_40KMASSMAN] = &Read40KMass;
  
  sprintf(options[OPT_40KNUMMAN].cName,"d40KNumMan");
  sprintf(options[OPT_40KNUMMAN].cDescr,"Initial Number of 40K Atoms");
  sprintf(options[OPT_40KNUMMAN].cDefault,"Primordial Earth: xxx");
  options[OPT_40KNUMMAN].iType = 2;
  options[OPT_40KNUMMAN].iMultiFile = 1;
  options[OPT_40KNUMMAN].dNeg = ENUMMAN40K;
  options[OPT_40KNUMMAN].dDefault = 0;
  sprintf(options[OPT_40KNUMMAN].cNeg,"Primordial Earth 40K Number");
  fnRead[OPT_40KNUMMAN] = &Read40KNum;
  
  sprintf(options[OPT_40KPOWERMAN].cName,"d40KPowerMan");
  sprintf(options[OPT_40KPOWERMAN].cDescr,"Initial Power Production from 40K Atoms");
  sprintf(options[OPT_40KPOWERMAN].cDefault,"Primordial Earth: xx TW");
  options[OPT_40KPOWERMAN].iType = 2;
  options[OPT_40KPOWERMAN].iMultiFile = 1;
  options[OPT_40KPOWERMAN].dNeg = EPOWERMAN40K;  //Earth's POWER of 40K
  options[OPT_40KPOWERMAN].dDefault = 0;
  sprintf(options[OPT_40KPOWERMAN].cNeg,"TW");
  fnRead[OPT_40KPOWERMAN] = &Read40KPower;
  
  sprintf(options[OPT_232THMASSMAN].cName,"d232ThMassMan");
  sprintf(options[OPT_232THMASSMAN].cDescr,"Initial Mass of 232Th");
  sprintf(options[OPT_232THMASSMAN].cDefault,"Primordial Earth: xxx");
  options[OPT_232THMASSMAN].iType = 2;
  options[OPT_232THMASSMAN].iMultiFile = 1;
  options[OPT_232THMASSMAN].dNeg = EMASSMAN232TH;
  options[OPT_232THMASSMAN].dDefault = 0;
  sprintf(options[OPT_232THMASSMAN].cNeg,"Earth Masses");
  fnRead[OPT_232THMASSMAN] = &Read232ThMass; 
  
  sprintf(options[OPT_232THNUMMAN].cName,"d232ThNumMan");
  sprintf(options[OPT_232THNUMMAN].cDescr,"Initial Number of 232Th Atoms");
  sprintf(options[OPT_232THNUMMAN].cDefault,"Primordial Earth: xxx");
  options[OPT_232THNUMMAN].iType = 2;
  options[OPT_232THNUMMAN].iMultiFile = 1;
  options[OPT_232THNUMMAN].dNeg = ENUMMAN232TH;
  options[OPT_232THNUMMAN].dDefault = 0;
  sprintf(options[OPT_232THNUMMAN].cNeg,"Primordial Earth 232Th Number");
  fnRead[OPT_232THNUMMAN] = &Read232ThNum;
  
  sprintf(options[OPT_232THPOWERMAN].cName,"d232ThPowerMan");
  sprintf(options[OPT_232THPOWERMAN].cDescr,"Initial Power Production from 232Th Atoms");
  sprintf(options[OPT_232THPOWERMAN].cDefault,"Primordial Earth: xx TW");
  options[OPT_232THPOWERMAN].iType = 2;
  options[OPT_232THPOWERMAN].iMultiFile = 1;
  options[OPT_232THPOWERMAN].dNeg = EPOWERMAN232TH;  //1e12*1e-7; // cgs
  options[OPT_232THPOWERMAN].dDefault = 0;
  sprintf(options[OPT_232THPOWERMAN].cNeg,"TW");
  fnRead[OPT_232THPOWERMAN] = &Read232ThPower;

  sprintf(options[OPT_238UMASSMAN].cName,"d238UMassMan");
  sprintf(options[OPT_238UMASSMAN].cDescr,"Initial Mass of 238U");
  sprintf(options[OPT_238UMASSMAN].cDefault,"Primordial Earth: xxx");
  options[OPT_238UMASSMAN].iType = 2;
  options[OPT_238UMASSMAN].iMultiFile = 1;
  options[OPT_238UMASSMAN].dNeg = EMASSMAN238U;
  options[OPT_238UMASSMAN].dDefault = 0;
  sprintf(options[OPT_238UMASSMAN].cNeg,"Earth Masses");
  fnRead[OPT_238UMASSMAN] = &Read238UMass;
  
  sprintf(options[OPT_238UNUMMAN].cName,"d238UNumMan");
  sprintf(options[OPT_238UNUMMAN].cDescr,"Initial Number of 238U Atoms");
  sprintf(options[OPT_238UNUMMAN].cDefault,"1");
  options[OPT_238UNUMMAN].dDefault = 0;
  options[OPT_238UNUMMAN].iType = 2;
  options[OPT_238UNUMMAN].iMultiFile = 1;
  options[OPT_238UNUMMAN].dNeg = ENUMMAN238U;
  options[OPT_238UNUMMAN].dDefault = 0;
  sprintf(options[OPT_238UNUMMAN].cNeg,"Primordial Earth 238U Numer");
  fnRead[OPT_238UNUMMAN] = &Read238UNum;

  sprintf(options[OPT_238UPOWERMAN].cName,"d238UPowerMan");  //section added PED
  sprintf(options[OPT_238UPOWERMAN].cDescr,"Initial Power Production from 238U Atoms");
  sprintf(options[OPT_238UPOWERMAN].cDefault,"Primordial Earth: xx TW");
  options[OPT_238UPOWERMAN].iType = 2;
  options[OPT_238UPOWERMAN].iMultiFile = 1;
  options[OPT_238UPOWERMAN].dNeg = EPOWERMAN238U;   //1e12*1e-7; // cgs
  options[OPT_238UPOWERMAN].dDefault = 0;
  sprintf(options[OPT_238UPOWERMAN].cNeg,"TW");
  fnRead[OPT_238UPOWERMAN] = &Read238UPower;

  sprintf(options[OPT_235UMASSMAN].cName,"d235UMassMan");  //PED
  sprintf(options[OPT_235UMASSMAN].cDescr,"Initial Mass of 235U");
  sprintf(options[OPT_235UMASSMAN].cDefault,"Primordial Earth: xxx");
  options[OPT_235UMASSMAN].iType = 2;
  options[OPT_235UMASSMAN].iMultiFile = 1;
  options[OPT_235UMASSMAN].dNeg = EMASSMAN235U;
  options[OPT_235UMASSMAN].dDefault = 0;
  sprintf(options[OPT_235UMASSMAN].cNeg,"Earth Masses");
  fnRead[OPT_235UMASSMAN] = &Read235UMass;
  
  sprintf(options[OPT_235UNUMMAN].cName,"d235UNumMan");  //PED
  sprintf(options[OPT_235UNUMMAN].cDescr,"Initial Number of 235U Atoms");
  sprintf(options[OPT_235UNUMMAN].cDefault,"1");
  options[OPT_235UNUMMAN].iType = 2;
  options[OPT_235UNUMMAN].iMultiFile = 1;
  options[OPT_235UNUMMAN].dNeg = ENUMMAN235U;
  options[OPT_235UNUMMAN].dDefault = 0;
  sprintf(options[OPT_235UNUMMAN].cNeg,"Primordial Earth 235U Numer");
  fnRead[OPT_235UNUMMAN] = &Read235UNum;

  sprintf(options[OPT_235UPOWERMAN].cName,"d235UPowerMan");  //section added PED
  sprintf(options[OPT_235UPOWERMAN].cDescr,"Initial Power Production from 235U Atoms");
  sprintf(options[OPT_235UPOWERMAN].cDefault,"Primordial Earth: xx TW");
  options[OPT_235UPOWERMAN].iType = 2;
  options[OPT_235UPOWERMAN].iMultiFile = 1;
  options[OPT_235UPOWERMAN].dNeg = EPOWERMAN235U;   //1e12*1e-7; // cgs
  options[OPT_235UPOWERMAN].dDefault = 0;
  sprintf(options[OPT_235UPOWERMAN].cNeg,"TW");
  fnRead[OPT_235UPOWERMAN] = &Read235UPower;

  
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

  if (options[OPT_40KMASSMAN].iLine[iBody+1] >= 0) {
      //  I think here you need to define body.40KNum bc only the default value of 40Kmass has been chosen by user and set.
      //      printf("40KMass=%e, MASS40K=%e, 40KNum=%e\n",body[iBody].d40KMass,MASS40K,body[iBody].d40KNum);
      body[iBody].d40KNumMan=body[iBody].d40KMassMan/(MASS40K);
      printf("40KMassMan set, body[iBody].d40KNumMan=%e, ENUMMAN40K=%e\n",body[iBody].d40KNumMan,ENUMMAN40K);
  }

  if (options[OPT_40KNUMMAN].iLine[iBody+1] >= 0) {
      // Do nothing bc default Num set.
      printf("40KNumMan set, body[iBody].d40KNumMan=%e, ENUMMAN40K=%e\n",body[iBody].d40KNumMan,ENUMMAN40K);
  }

  if (options[OPT_40KPOWERMAN].iLine[iBody+1] >= 0) {
      body[iBody].d40KNumMan=body[iBody].d40KPowerMan/(ENERGY40K)*(HALFLIFE40K);
      printf("40KPowerMan set, .d40KPowerMan=%e, .d40KNumMan=%e, ENUMMAN40K=%e\n",body[iBody].d40KPowerMan,body[iBody].d40KNumMan,ENUMMAN40K);
      //      exit(1);
  }
  
  //  body[iBody].d40KConstMan = fd40KConstant(body[iBody].d40KNumMan,dAge);  //moved from above.
    body[iBody].d40KConstMan = fd40KConstantMan(body[iBody].d40KNumMan,dAge);  //moved from above.
}

void Assign232ThNum(BODY *body,OPTIONS *options,double dAge,int iBody) {

  if (options[OPT_232THMASSMAN].iLine[iBody+1] >= 0) {
      //    printf("232ThMass not implemented.\n");
      //    exit(1);
    body[iBody].d232ThNumMan=body[iBody].d232ThMassMan/(MASS232TH);
    printf("232ThMassMan set, body[iBody].d232ThNumMan=%e, ENUMMAN232TH=%e\n",body[iBody].d232ThNumMan,ENUMMAN232TH);
  }

  if (options[OPT_232THNUMMAN].iLine[iBody+1] >= 0) {
      //Do nothing, use default.
      printf("232ThNumMan set, body[iBody].d232ThKNumMan=%e, ENUMMAN232TH=%e\n",body[iBody].d232ThNumMan,ENUMMAN232TH);
  }

  if (options[OPT_232THPOWERMAN].iLine[iBody+1] >= 0) {
      //    printf("232ThPower not implemented.\n");
      //    exit(1);
    body[iBody].d232ThNumMan=body[iBody].d232ThPowerMan/(ENERGY232TH)*(HALFLIFE232TH);
    printf("232ThPowerMan set, .d232ThPowerMan=%e, .d232ThNumMan=%e, ENUMMAN232TH=%e\n",body[iBody].d232ThPowerMan,body[iBody].d232ThNumMan,ENUMMAN232TH);
  }
  //  body[iBody].d232ThConstMan = fd232ThConstant(body[iBody].d232ThNumMan,dAge);
    body[iBody].d232ThConstMan = fd232ThConstantMan(body[iBody].d232ThNumMan,dAge);
}

void Assign238UNum(BODY *body,OPTIONS *options,double dAge,int iBody) {

  if (options[OPT_238UMASSMAN].iLine[iBody+1] >= 0) {
      /*    printf("238UMass not implemented.\n");
	    exit(1);*/
      body[iBody].d238UNumMan=body[iBody].d238UMassMan/(MASS238U);
      printf("238UMassMan set, body[iBody].d238UNumMan=%e, ENUMMAN238U=%e\n",body[iBody].d238UNumMan,ENUMMAN238U);
  }

  if (options[OPT_238UNUMMAN].iLine[iBody+1] >= 0) {
      printf("238UNumMan set, body[iBody].d238UNumMan=%e, ENUMMAN238U=%e\n",body[iBody].d238UNumMan,ENUMMAN238U);
  }

  if (options[OPT_238UPOWERMAN].iLine[iBody+1] >= 0) {
      /*    printf("238UPower not implemented.\n");
	    exit(1);*/
      body[iBody].d238UNumMan=body[iBody].d238UPowerMan/(ENERGY238U)*(HALFLIFE238U);
      printf("238UPowerMan set, .d238UPowerMan=%e, .d238UNumMan=%e, ENUMMAN238U=%e\n",body[iBody].d238UPowerMan,body[iBody].d238UNumMan,ENUMMAN238U);
  }
  //  body[iBody].d238UConstMan = fd238UConstant(body[iBody].d238UNumMan,dAge);
    body[iBody].d238UConstMan = fd238UConstantMan(body[iBody].d238UNumMan,dAge);
}

void Assign235UNum(BODY *body,OPTIONS *options,double dAge,int iBody) {  //PED

  if (options[OPT_235UMASSMAN].iLine[iBody+1] >= 0) {
      /*    printf("235UMass not implemented.\n");
	    exit(1);*/
    body[iBody].d235UNumMan=body[iBody].d235UMassMan/(MASS235U);
    printf("235UMassMan set, body[iBody].d235UNumMan=%e, ENUMMAN235U=%e\n",body[iBody].d235UNumMan,ENUMMAN235U);
  }

  if (options[OPT_235UNUMMAN].iLine[iBody+1] >= 0) {
    printf("235UNumMan set, body[iBody].d235UNumMan=%e, ENUMMAN235U=%e\n",body[iBody].d235UNumMan,ENUMMAN235U);
  }

  if (options[OPT_235UPOWERMAN].iLine[iBody+1] >= 0) {
      /*    printf("235UPower not implemented.\n");
	    exit(1);*/
    body[iBody].d235UNumMan=body[iBody].d235UPowerMan/(ENERGY235U)*(HALFLIFE235U);
    printf("235UPowerMan set, .d235UPowerMan=%e, .d235UNumMan=%e, ENUMMAN235U=%e\n",body[iBody].d235UPowerMan,body[iBody].d235UNumMan,ENUMMAN235U);
  }
  //  body[iBody].d235UConstMan = fd235UConstant(body[iBody].d235UNumMan,dAge);
  body[iBody].d235UConstMan = fd235UConstantMan(body[iBody].d235UNumMan,dAge);
}

void Verify40K(BODY *body,OPTIONS *options,UPDATE *update,double dAge,fnUpdateVariable ***fnUpdate,int iBody) {

  Assign40KNum(body,options,dAge,iBody);

  update[iBody].iaType[update[iBody].i40K][0] = 1;
  update[iBody].iNumBodies[update[iBody].i40K][0]=1;
  update[iBody].iaBody[update[iBody].i40K][0] = malloc(update[iBody].iNumBodies[update[iBody].i40K][0]*sizeof(int));
  update[iBody].iaBody[update[iBody].i40K][0][0]=iBody;

  update[iBody].pdD40KNumManDt = &update[iBody].daDerivProc[update[iBody].i40K][0];
  fnUpdate[iBody][update[iBody].i40K][0] = &fdD40KNumManDt;
}

void Verify232Th(BODY *body,OPTIONS *options,UPDATE *update,double dAge,fnUpdateVariable ***fnUpdate,int iBody) {

  Assign232ThNum(body,options,dAge,iBody);

  update[iBody].iaType[update[iBody].i232Th][0] = 1;
  update[iBody].iNumBodies[update[iBody].i232Th][0]=1;
  update[iBody].iaBody[update[iBody].i232Th][0] = malloc(update[iBody].iNumBodies[update[iBody].i232Th][0]*sizeof(int));
  update[iBody].iaBody[update[iBody].i232Th][0][0]=iBody;
  
  update[iBody].pdD232ThNumManDt = &update[iBody].daDerivProc[update[iBody].i232Th][0];
  fnUpdate[iBody][update[iBody].i232Th][0] = &fdD232ThNumManDt;
}

void Verify238U(BODY *body,OPTIONS *options,UPDATE *update,double dAge,fnUpdateVariable ***fnUpdate,int iBody) {

  Assign238UNum(body,options,dAge,iBody);

  update[iBody].iaType[update[iBody].i238U][0] = 1;
  update[iBody].iNumBodies[update[iBody].i238U][0]=1;
  update[iBody].iaBody[update[iBody].i238U][0] = malloc(update[iBody].iNumBodies[update[iBody].i238U][0]*sizeof(int));
  update[iBody].iaBody[update[iBody].i238U][0][0]=iBody;
  
  update[iBody].pdD238UNumManDt = &update[iBody].daDerivProc[update[iBody].i238U][0];
  fnUpdate[iBody][update[iBody].i238U][0] = &fdD238UNumManDt;
}

void Verify235U(BODY *body,OPTIONS *options,UPDATE *update,double dAge,fnUpdateVariable ***fnUpdate,int iBody) { //PED

  Assign235UNum(body,options,dAge,iBody);

  update[iBody].iaType[update[iBody].i235U][0] = 1;
  update[iBody].iNumBodies[update[iBody].i235U][0]=1;
  update[iBody].iaBody[update[iBody].i235U][0] = malloc(update[iBody].iNumBodies[update[iBody].i235U][0]*sizeof(int));
  update[iBody].iaBody[update[iBody].i235U][0][0]=iBody;
  
  update[iBody].pdD235UNumManDt = &update[iBody].daDerivProc[update[iBody].i235U][0];
  fnUpdate[iBody][update[iBody].i235U][0] = &fdD235UNumManDt;
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
  if (body[iBody].d40KNumMan < 0.5)
    body[iBody].d40KNumMan = 0;

  if (body[iBody].d232ThNumMan < 0.5)
    body[iBody].d232ThNumMan = 0;

  if (body[iBody].d238UNumMan < 0.5)
    body[iBody].d238UNumMan = 0;

  if (body[iBody].d235UNumMan < 0.5) //PED
    body[iBody].d235UNumMan = 0;
}

void VerifyRadheat(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,OUTPUT *output,SYSTEM *system,UPDATE *update,fnUpdateVariable ***fnUpdate,int iBody,int iModule) {
  int bRadheat=0;

  /* Cannot set 2 or more of Power, Mass and Number for any isotope */

  /*XXX Need a VerifyOneOfThree subroutine */

  /* Radheat is active for this body if this subroutine is called. */

  if (body[iBody].d40KNumMan > 0 || body[iBody].d40KMassMan > 0 || body[iBody].d40KPowerMan > 0) {
    NotMassAndNum(options,OPT_40KMASSMAN,OPT_40KNUMMAN,iBody+1);
    Verify40K(body,options,update,system->dAge,fnUpdate,iBody);
    bRadheat = 1;
  }

  if (body[iBody].d232ThNumMan > 0 || body[iBody].d232ThMassMan > 0 || body[iBody].d232ThPowerMan > 0) {
    NotMassAndNum(options,OPT_232THMASSMAN,OPT_232THNUMMAN,iBody+1);
    Verify232Th(body,options,update,system->dAge,fnUpdate,iBody);
    bRadheat = 1;
  }

  if (body[iBody].d238UNumMan > 0 || body[iBody].d238UMassMan > 0 || body[iBody].d238UPowerMan > 0) {
      NotMassAndNum(options,OPT_238UMASSMAN,OPT_238UNUMMAN,iBody+1);
      Verify238U(body,options,update,system->dAge,fnUpdate,iBody);
      bRadheat = 1;
  }

  if (body[iBody].d235UNumMan > 0 || body[iBody].d235UMassMan > 0 || body[iBody].d235UPowerMan > 0) {  //PED
      NotMassAndNum(options,OPT_235UMASSMAN,OPT_235UNUMMAN,iBody+1);
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
  if (body[iBody].d40KNumMan > 0 || body[iBody].d40KMassMan > 0 || body[iBody].d40KPowerMan > 0) {
    update[iBody].iNumVars++;
    update[iBody].iNum40KMan++;
  }
  if (body[iBody].d232ThNumMan > 0 || body[iBody].d232ThMassMan > 0 || body[iBody].d232ThPowerMan > 0) {
    update[iBody].iNumVars++;
    update[iBody].iNum232ThMan++;
  }
  if (body[iBody].d238UNumMan > 0 || body[iBody].d238UMassMan > 0 || body[iBody].d238UPowerMan > 0) {
    update[iBody].iNumVars++;
    update[iBody].iNum238UMan++;
  }
  if (body[iBody].d235UNumMan > 0 || body[iBody].d235UMassMan > 0 || body[iBody].d235UPowerMan > 0) {  //PED
    update[iBody].iNumVars++;
    update[iBody].iNum235UMan++;
  }
}

void FinalizeUpdateEccRadheat(BODY *body,UPDATE *update,int *iEqn,int iVar,int iBody) {
  /* Nothing */
}

void FinalizeUpdate40KNumRadheat(BODY *body,UPDATE*update,int *iEqn,int iVar,int iBody) {
  update[iBody].iaModule[iVar][*iEqn] = RAD40K;
  update[iBody].iNum40KMan = (*iEqn)++;
}

void FinalizeUpdate232ThNumRadheat(BODY *body,UPDATE*update,int *iEqn,int iVar,int iBody) {
  update[iBody].iaModule[iVar][*iEqn] = RAD232TH;
  update[iBody].iNum232ThMan = (*iEqn)++;
}

void FinalizeUpdate238UNumRadheat(BODY *body,UPDATE*update,int *iEqn,int iVar,int iBody) {
  update[iBody].iaModule[iVar][*iEqn] = RAD238U;
  update[iBody].iNum238UMan = (*iEqn)++;
}

void FinalizeUpdate235UNumRadheat(BODY *body,UPDATE*update,int *iEqn,int iVar,int iBody) {  //PED
  update[iBody].iaModule[iVar][*iEqn] = RAD235U;
  update[iBody].iNum235UMan = (*iEqn)++;
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

  if (body[iBody].d40KPowerMan < halt->dMin40KPower) {
    if (io->iVerbose >= VERBPROG) {
      printf("HALT: %s's 40K Power =  ",body[iBody].cName);
      fprintd(stdout,body[iBody].d40KPowerMan,io->iSciNot,io->iDigits);
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

  if (body[iBody].d232ThPowerMan < halt->dMin232ThPower) {
    if (io->iVerbose >= VERBPROG) {
      printf("HALT: %s's 232Th Power =  ",body[iBody].cName);
      fprintd(stdout,body[iBody].d232ThPowerMan,io->iSciNot,io->iDigits);
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

  if (body[iBody].d238UPowerMan < halt->dMin238UPower) {
    if (io->iVerbose >= VERBPROG) {
      printf("HALT: %s's 238U Power =  ",body[iBody].cName);
      fprintd(stdout,body[iBody].d238UPowerMan,io->iSciNot,io->iDigits);
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

  if (body[iBody].d235UPowerMan < halt->dMin235UPower) {
    if (io->iVerbose >= VERBPROG) {
      printf("HALT: %s's 235U Power =  ",body[iBody].cName);
      fprintd(stdout,body[iBody].d235UPowerMan,io->iSciNot,io->iDigits);
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

void Write40KPowerMan(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  /* Get total power from 40K */

  *dTmp = -(*(update[iBody].pdD40KNumManDt))*ENERGY40K;

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

  *dTmp = body[iBody].d40KPowerMan/(4*PI*body[iBody].dRadius*body[iBody].dRadius);

  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else {
    *dTmp /= fdUnitsEnergyFlux(units->iTime,units->iMass,units->iLength);
    fsUnitsEnergyFlux(units,cUnit);
  }
}

void WriteD40KNumDt(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {

  *dTmp = *(update[iBody].pdD40KNumManDt);

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

void Write40KMassMan(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {

  *dTmp = body[iBody].d40KNumMan*MASS40K;

  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else {
    *dTmp /= fdUnitsMass(units->iMass);
    fsUnitsMass(units->iMass,cUnit);
  }
}

//PED: this subr is not finished?
void WriteD40KPowerDt(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  *dTmp = -1;
}

void Write40KNumMan(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  *dTmp = body[iBody].d40KNumMan;

  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  }
  /* else nothing, as it is a number */
}

/* Thorium */

void Write232ThPowerMan(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  /* Get total heat from 232Th */
  *dTmp = -(*(update[iBody].pdD232ThNumManDt))*ENERGY232TH;

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
  *dTmp = body[iBody].d232ThPowerMan/(4*PI*body[iBody].dRadius*body[iBody].dRadius);

  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else {
    *dTmp /= fdUnitsEnergyFlux(units->iTime,units->iMass,units->iLength);
    fsUnitsEnergyFlux(units,cUnit);
  }
}

void WriteD232ThNumDt(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  *dTmp = *(update[iBody].pdD232ThNumManDt);

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

void Write232ThMassMan(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  *dTmp = body[iBody].d232ThNumMan*MASS232TH;

  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else {
    *dTmp /= fdUnitsMass(units->iMass);
    fsUnitsMass(units->iMass,cUnit);
  }
}

void Write232ThNumMan(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  *dTmp = body[iBody].d232ThNumMan;

  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  }
  /* else nothing, as it is a number */
}

/* Uranium 238 */

void Write238UPowerMan(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  /* Get total power from 238U */
  *dTmp = -(*(update[iBody].pdD238UNumManDt))*ENERGY238U;

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
  *dTmp = body[iBody].d238UPowerMan/(4*PI*body[iBody].dRadius*body[iBody].dRadius);

  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else {
    *dTmp /= fdUnitsEnergyFlux(units->iTime,units->iMass,units->iLength);
    fsUnitsEnergyFlux(units,cUnit);
  }
}

void WriteD238UNumDt(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  *dTmp = *(update[iBody].pdD238UNumManDt);

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

void Write238UMassMan(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  *dTmp = body[iBody].d238UNumMan*MASS238U;

  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else {
    *dTmp /= fdUnitsMass(units->iMass);
    fsUnitsMass(units->iMass,cUnit);
  }
}

void Write238UNumMan(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  *dTmp = body[iBody].d238UNumMan;

  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  }
  /* else nothing, as it is a number */
}

/* Uranium 235 PED */

void Write235UPowerMan(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  /* Get total power from 235U */
  *dTmp = -(*(update[iBody].pdD235UNumManDt))*ENERGY235U;

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
  *dTmp = body[iBody].d235UPowerMan/(4*PI*body[iBody].dRadius*body[iBody].dRadius);

  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else {
    *dTmp /= fdUnitsEnergyFlux(units->iTime,units->iMass,units->iLength);
    fsUnitsEnergyFlux(units,cUnit);
  }
}

void WriteD235UNumDt(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  *dTmp = *(update[iBody].pdD235UNumManDt);

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

void Write235UMassMan(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  *dTmp = body[iBody].d235UNumMan*MASS235U;

  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else {
    *dTmp /= fdUnitsMass(units->iMass);
    fsUnitsMass(units->iMass,cUnit);
  }
}

void Write235UNumMan(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  *dTmp = body[iBody].d235UNumMan;

  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  }
  /* else nothing, as it is a number */
}

/* Totals */
void WriteRadPowerMan(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
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
  sprintf(output[OUT_40KPOWERMAN].cName,"40KPowerMan");
  sprintf(output[OUT_40KPOWERMAN].cDescr,"Total Power Generated by 40K");
  sprintf(output[OUT_40KPOWERMAN].cNeg,"TW");
  output[OUT_40KPOWERMAN].bNeg = 1;
  output[OUT_40KPOWERMAN].dNeg = 1e-19; // ergs/s -> TW 
  output[OUT_40KPOWERMAN].iNum = 1;
  fnWrite[OUT_40KPOWERMAN] = &Write40KPowerMan;
  
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
  fnWrite[OUT_40KDPOWERDT] = &WriteD40KPowerDt;  //PED: that subr is not written yet?
  
  sprintf(output[OUT_40KDNUMDT].cName,"D40KNumManDt");
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
  
  sprintf(output[OUT_40KMASSMAN].cName,"40KMassMan");
  sprintf(output[OUT_40KMASSMAN].cDescr,"Total Mass of 40K");
  sprintf(output[OUT_40KMASSMAN].cNeg,"Earth Masses");
  output[OUT_40KMASSMAN].bNeg = 1;
  output[OUT_40KMASSMAN].dNeg = MEARTH;
  output[OUT_40KMASSMAN].iNum = 1;
  fnWrite[OUT_40KMASSMAN] = &Write40KMassMan;
  
  sprintf(output[OUT_40KNUMMAN].cName,"40KNumMan");
  sprintf(output[OUT_40KNUMMAN].cDescr,"Total Number of 40K Atoms");
  sprintf(output[OUT_40KNUMMAN].cNeg,"Initial Primordial Earth Number");
  output[OUT_40KNUMMAN].bNeg = 1;
  output[OUT_40KNUMMAN].dNeg = EMASSMAN40K/MASS40K;
  output[OUT_40KNUMMAN].iNum = 1;
  fnWrite[OUT_40KNUMMAN] = &Write40KNumMan;

  /* Thorium */

  sprintf(output[OUT_232THPOWERMAN].cName,"232ThPowerMan");
  sprintf(output[OUT_232THPOWERMAN].cDescr,"Total Power Generated by 232Th");
  sprintf(output[OUT_232THPOWERMAN].cNeg,"TW");
  output[OUT_232THPOWERMAN].bNeg = 1;
  output[OUT_232THPOWERMAN].dNeg = 1e-19; 
  output[OUT_232THPOWERMAN].iNum = 1;
  fnWrite[OUT_232THPOWERMAN] = &Write232ThPowerMan;
  
  sprintf(output[OUT_232THENFLUX].cName,"232ThEnFlux");
  sprintf(output[OUT_232THENFLUX].cDescr,"Surface Energy Flux from 232Th");
  sprintf(output[OUT_232THENFLUX].cNeg,"W/m^2");
  output[OUT_232THENFLUX].bNeg = 1;
  output[OUT_232THENFLUX].dNeg = 1e-3;
  output[OUT_232THENFLUX].iNum = 1;
  fnWrite[OUT_232THENFLUX] = &Write232ThEnFlux;

  sprintf(output[OUT_232THDNUMDT].cName,"D232ThNumManDt");
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
  
  sprintf(output[OUT_232THMASSMAN].cName,"232ThMassMan");
  sprintf(output[OUT_232THMASSMAN].cDescr,"Total Mass of 232Th");
  sprintf(output[OUT_232THMASSMAN].cNeg,"Earth Masses");
  output[OUT_232THMASSMAN].bNeg = 1;
  output[OUT_232THMASSMAN].dNeg = MEARTH;
  output[OUT_232THMASSMAN].iNum = 1;
  fnWrite[OUT_232THMASSMAN] = &Write232ThMassMan;
  
  sprintf(output[OUT_232THNUMMAN].cName,"232ThNumMan");
  sprintf(output[OUT_232THNUMMAN].cDescr,"Total Number of 232Th Atoms");
  sprintf(output[OUT_232THNUMMAN].cNeg,"Initial Primordial Earth Number");
  output[OUT_232THNUMMAN].bNeg = 1;
  output[OUT_232THNUMMAN].dNeg = EMASSMAN232TH/MASS232TH;
  output[OUT_232THNUMMAN].iNum = 1;
  fnWrite[OUT_232THNUMMAN] = &Write232ThNumMan;

  /* Uranium 238 */

  sprintf(output[OUT_238UPOWERMAN].cName,"238UPowerMan");
  sprintf(output[OUT_238UPOWERMAN].cDescr,"Total Power Generated by 238U");
  sprintf(output[OUT_238UPOWERMAN].cNeg,"TW");
  output[OUT_238UPOWERMAN].bNeg = 1;
  output[OUT_238UPOWERMAN].dNeg = 1e-19; 
  output[OUT_238UPOWERMAN].iNum = 1;
  fnWrite[OUT_238UPOWERMAN] = &Write238UPowerMan;
  
  sprintf(output[OUT_238UENFLUX].cName,"238UEnFlux");
  sprintf(output[OUT_238UENFLUX].cDescr,"Surface Energy Flux due to 238U");
  sprintf(output[OUT_238UENFLUX].cNeg,"W/m^2");
  output[OUT_238UENFLUX].bNeg = 1;
  output[OUT_238UENFLUX].dNeg = 1e-3;
  output[OUT_238UENFLUX].iNum = 1;
  fnWrite[OUT_238UENFLUX] = &Write238UEnFlux;

  sprintf(output[OUT_238UDNUMDT].cName,"D238UNumManDt");
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
  
  sprintf(output[OUT_238UMASSMAN].cName,"238UMassMan");
  sprintf(output[OUT_238UMASSMAN].cDescr,"Total Mass of 238U");
  sprintf(output[OUT_238UMASSMAN].cNeg,"Earth Masses");
  output[OUT_238UMASSMAN].bNeg = 1;
  output[OUT_238UMASSMAN].dNeg = MEARTH;
  output[OUT_238UMASSMAN].iNum = 1;
  fnWrite[OUT_238UMASSMAN] = &Write238UMassMan;
  
  sprintf(output[OUT_238UNUMMAN].cName,"238UNumMan");
  sprintf(output[OUT_238UNUMMAN].cDescr,"Total Number of 238U Atoms");
  sprintf(output[OUT_238UNUMMAN].cNeg,"Initial Primordial Earth Number");
  output[OUT_238UNUMMAN].bNeg = 1;
  output[OUT_238UNUMMAN].dNeg = 1;
  output[OUT_238UNUMMAN].iNum = 1;
  fnWrite[OUT_238UNUMMAN] = &Write238UNumMan;


    /* Uranium 235 PED */
  
  sprintf(output[OUT_235UPOWERMAN].cName,"235UPowerMan");
  sprintf(output[OUT_235UPOWERMAN].cDescr,"Total Power Generated by 235U");
  sprintf(output[OUT_235UPOWERMAN].cNeg,"TW");
  output[OUT_235UPOWERMAN].bNeg = 1;
  output[OUT_235UPOWERMAN].dNeg = 1e-19; 
  output[OUT_235UPOWERMAN].iNum = 1;
  fnWrite[OUT_235UPOWERMAN] = &Write235UPowerMan;
  
  sprintf(output[OUT_235UENFLUX].cName,"235UEnFlux");
  sprintf(output[OUT_235UENFLUX].cDescr,"Surface Energy Flux due to 235U");
  sprintf(output[OUT_235UENFLUX].cNeg,"W/m^2");
  output[OUT_235UENFLUX].bNeg = 1;
  output[OUT_235UENFLUX].dNeg = 1e-3;
  output[OUT_235UENFLUX].iNum = 1;
  fnWrite[OUT_235UENFLUX] = &Write235UEnFlux;

  sprintf(output[OUT_235UDNUMDT].cName,"D235UNumManDt");
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
  
  sprintf(output[OUT_235UMASSMAN].cName,"235UMassMan");
  sprintf(output[OUT_235UMASSMAN].cDescr,"Total Mass of 235U");
  sprintf(output[OUT_235UMASSMAN].cNeg,"Earth Masses");
  output[OUT_235UMASSMAN].bNeg = 1;
  output[OUT_235UMASSMAN].dNeg = MEARTH;
  output[OUT_235UMASSMAN].iNum = 1;
  fnWrite[OUT_235UMASSMAN] = &Write235UMassMan;
  
  sprintf(output[OUT_235UNUMMAN].cName,"235UNumMan");
  sprintf(output[OUT_235UNUMMAN].cDescr,"Total Number of 235U Atoms");
  sprintf(output[OUT_235UNUMMAN].cNeg,"Initial Primordial Earth Number");
  output[OUT_235UNUMMAN].bNeg = 1;
  output[OUT_235UNUMMAN].dNeg = 1;
  output[OUT_235UNUMMAN].iNum = 1;
  fnWrite[OUT_235UNUMMAN] = &Write235UNumMan;
  
      
  /* Totals */
  
  sprintf(output[OUT_RADPOWERMAN].cName,"RadPowerMan");
  sprintf(output[OUT_RADPOWERMAN].cDescr,"Total Power Generated by Radiogenic Nuclides in Mantle");
  sprintf(output[OUT_RADPOWERMAN].cNeg,"TW");
  output[OUT_RADPOWERMAN].bNeg = 1;
  output[OUT_RADPOWERMAN].dNeg = 1e-19; 
  output[OUT_RADPOWERMAN].iNum = 1;
  fnWrite[OUT_RADPOWERMAN] = &WriteRadPowerMan;

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

  return -(*(update[iBody].pdD238UNumManDt))*ENERGY238U - (*(update[iBody].pdD235UNumManDt))*ENERGY235U - (*(update[iBody].pdD232ThNumManDt))*ENERGY232TH - (*(update[iBody].pdD40KNumManDt))*ENERGY40K;
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

double fd40KConstantMan(double dNum,double dAge) {  
    return fdRadheatConst(dNum,dAge,HALFLIFE40K);   //redirects to fdRadheatConst
}

double fd232ThConstantMan(double dNum,double dAge) {  //PED: changed dPower to dNum.
  return fdRadheatConst(dNum,dAge,HALFLIFE232TH);  //redirects to fdRadheatConst
}

double fd238UConstantMan(double dNum,double dAge) {  //PED: changed dPower to dNum.
  return fdRadheatConst(dNum,dAge,HALFLIFE238U);  //redirects to fdRadheatConst
}

double fd235UConstantMan(double dNum,double dAge) {  //PED: changed dPower to dNum.
  return fdRadheatConst(dNum,dAge,HALFLIFE235U);  //redirects to fdRadheatConst
}

double fdDNumRadDt(double dConst,double dHalfLife,double dAge) {  //dN/dt, can be used by any radioactive system?
  return -dConst/dHalfLife*exp(-dAge/dHalfLife);
}

double fdRadPower(double dConst,double dHalfLife,double dAge) {
    return dConst*exp(-dHalfLife/dAge);                      //Here const=N_0*energy_i*lambda, where energy_i=erg/num.
}

double fdRadEnFlux(double dConst,double dHalfLife,double dAge,double dRadius) {
  return fdRadPower(dConst,dHalfLife,dAge)/(4*PI*dRadius*dRadius);
}

double fd40KPowerMan(BODY *body,SYSTEM *system,int *iaBody,int iBody) {
  return fdRadPower(body[iBody].d40KConstMan,HALFLIFE40K,system->dAge);   //redirects to fdRadPower
}

double fd232ThPowerMan(BODY *body,SYSTEM *system,int iBody) {
  return fdRadPower(body[iBody].d232ThConstMan,HALFLIFE232TH,system->dAge);    //redirects to fdRadPower
}

double fd238UPowerMan(BODY *body,SYSTEM *system,int iBody) {
  return fdRadPower(body[iBody].d238UConstMan,HALFLIFE238U,system->dAge);    //redirects to fdRadPower
}

double fd235UPowerMan(BODY *body,SYSTEM *system,int iBody) {
  return fdRadPower(body[iBody].d235UConstMan,HALFLIFE235U,system->dAge);    //redirects to fdRadPower
}

//PED commenting these f out bc they aren't being used (5/11/15)

double fd40KEnFlux(BODY *body,SYSTEM *system,int *iaBody,int iBody) {
  return fdRadEnFlux(body[iBody].d40KConstMan,HALFLIFE40K,system->dAge,body[iBody].dRadius);
}

double fd232ThEnFlux(BODY *body,SYSTEM *system,int iBody) {
  return fdRadEnFlux(body[iBody].d232ThConstMan,HALFLIFE232TH,system->dAge,body[iBody].dRadius);
}

double fd238UEnFlux(BODY *body,SYSTEM *system,int iBody) {
  return fdRadEnFlux(body[iBody].d238UConstMan,HALFLIFE238U,system->dAge,body[iBody].dRadius);
}

double fd235UEnFlux(BODY *body,SYSTEM *system,int iBody) {
  return fdRadEnFlux(body[iBody].d235UConstMan,HALFLIFE235U,system->dAge,body[iBody].dRadius);
}


double fdD40KNumManDt(BODY *body,SYSTEM *system,int *iaBody,int iNumBodies) {
  return fdDNumRadDt(body[iaBody[0]].d40KConstMan,HALFLIFE40K,system->dAge);
}

double fdD232ThNumManDt(BODY *body,SYSTEM *system,int *iaBody,int iNumBodies) {
  return fdDNumRadDt(body[iaBody[0]].d232ThConstMan,HALFLIFE232TH,system->dAge);
}

double fdD238UNumManDt(BODY *body,SYSTEM *system,int *iaBody,int iNumBodies) {
  return fdDNumRadDt(body[iaBody[0]].d238UConstMan,HALFLIFE238U,system->dAge);
}

double fdD235UNumManDt(BODY *body,SYSTEM *system,int *iaBody,int iNumBodies) {
  return fdDNumRadDt(body[iaBody[0]].d235UConstMan,HALFLIFE235U,system->dAge);
}

/* Number derivatives? */
