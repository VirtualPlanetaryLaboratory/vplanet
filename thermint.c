/********************** THERMINT.C **********************/
/*
 * Peter Driscoll 6/1/15
 *
 * Thermal evolution of the interior (mantle+core)
 *
*/

#include <stdio.h>
#include <math.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "vplanet.h"

void InitializeBodyThermint(BODY *body,CONTROL *control,UPDATE *update,int iBody,int iModule) {

  /* A non-eqtide run requires this to be 0 to start. If eqtide is called,
     then the value will be updated in PropsAuxMultiEqtideThermint. */

  body[iBody].dTidalPowMan = 0;
}


void BodyCopyThermint(BODY *dest,BODY *src,int foo,int iNumBodies,int iBody) {
  dest[iBody].dTMan = src[iBody].dTMan;
  dest[iBody].dTCore = src[iBody].dTCore;
  /* Constants */
  dest[iBody].dViscJumpMan = src[iBody].dViscJumpMan;
  dest[iBody].dViscRef = src[iBody].dViscRef;
  dest[iBody].dTrefLind = src[iBody].dTrefLind;
  dest[iBody].dDTChiRef = src[iBody].dDTChiRef;
  dest[iBody].dEruptEff = src[iBody].dEruptEff;
  dest[iBody].dViscMeltDelta = src[iBody].dViscMeltDelta;
  dest[iBody].dViscMeltB = src[iBody].dViscMeltB;
  dest[iBody].dViscMeltPhis = src[iBody].dViscMeltPhis;
  dest[iBody].dViscMeltGamma = src[iBody].dViscMeltGamma;
  dest[iBody].dViscMeltXi = src[iBody].dViscMeltXi;
  dest[iBody].dStagLid = src[iBody].dStagLid;
  dest[iBody].dManHFlowPref = src[iBody].dManHFlowPref;
  /* Aux Props Variables */
  dest[iBody].dTUMan=src[iBody].dTUMan;
  dest[iBody].dTLMan=src[iBody].dTLMan;
  dest[iBody].dTCMB=src[iBody].dTCMB;
  dest[iBody].dTJumpUMan=src[iBody].dTJumpUMan;
  dest[iBody].dTJumpLMan=src[iBody].dTJumpLMan;
  dest[iBody].dSignTJumpUMan=src[iBody].dSignTJumpUMan;
  dest[iBody].dSignTJumpLMan=src[iBody].dSignTJumpLMan;
  dest[iBody].dViscUManArr=src[iBody].dViscUManArr;
  dest[iBody].dViscUMan=src[iBody].dViscUMan;
  dest[iBody].dViscLMan=src[iBody].dViscLMan;
  dest[iBody].dViscMMan=src[iBody].dViscMMan;
  dest[iBody].dBLUMan=src[iBody].dBLUMan;
  dest[iBody].dBLLMan=src[iBody].dBLLMan;
  dest[iBody].dShmodUMan=src[iBody].dShmodUMan;
  dest[iBody].dTsolUMan=src[iBody].dTsolUMan;
  dest[iBody].dTsolLMan=src[iBody].dTsolLMan;
  dest[iBody].dTliqUMan=src[iBody].dTliqUMan;
  dest[iBody].dTliqLMan=src[iBody].dTliqLMan;
  dest[iBody].dFMeltUMan=src[iBody].dFMeltUMan;
  dest[iBody].dFMeltLMan=src[iBody].dFMeltLMan;
  dest[iBody].dMeltfactorUMan=src[iBody].dMeltfactorUMan;
  dest[iBody].dMeltfactorLMan=src[iBody].dMeltfactorLMan;
  dest[iBody].dFixMeltfactorUMan=src[iBody].dFixMeltfactorUMan;
  dest[iBody].dDepthMeltMan=src[iBody].dDepthMeltMan;
  dest[iBody].dTDepthMeltMan=src[iBody].dTDepthMeltMan;
  dest[iBody].dTJumpMeltMan=src[iBody].dTJumpMeltMan;
  dest[iBody].dMeltMassFluxMan=src[iBody].dMeltMassFluxMan;
  dest[iBody].dRayleighMan=src[iBody].dRayleighMan;
  /* Tides XXX Should these live in eqtide? They're really only for when
   Q is allowed to change, which requires thermint. */
  dest[iBody].dK2Man=src[iBody].dK2Man;
  dest[iBody].dImk2Man=src[iBody].dImk2Man;
  /* Heat Flows */
  /* Mantle */
  dest[iBody].dHfluxUMan=src[iBody].dHfluxUMan;
  dest[iBody].dHfluxLMan=src[iBody].dHfluxLMan;
  dest[iBody].dHfluxCMB=src[iBody].dHfluxCMB;
  dest[iBody].dHflowUMan=src[iBody].dHflowUMan;
  dest[iBody].dHflowLMan=src[iBody].dHflowLMan;
  dest[iBody].dHflowCMB=src[iBody].dHflowCMB;
  dest[iBody].dTidalPowMan=src[iBody].dTidalPowMan;
  dest[iBody].dHflowLatentMan=src[iBody].dHflowLatentMan;
  dest[iBody].dHflowMeltMan=src[iBody].dHflowMeltMan;
  dest[iBody].dHflowSecMan=src[iBody].dHflowSecMan;
  dest[iBody].dHflowSurf=src[iBody].dHflowSurf;
  /* Core */
  dest[iBody].dRIC=src[iBody].dRIC;
  dest[iBody].dDRICDTCMB=src[iBody].dDRICDTCMB;
  dest[iBody].dMassICDot=src[iBody].dMassICDot;
  dest[iBody].dHflowLatentIC=src[iBody].dHflowLatentIC;
  dest[iBody].dPowerGravIC=src[iBody].dPowerGravIC;
  dest[iBody].dChiOC=src[iBody].dChiOC;
  dest[iBody].dChiIC=src[iBody].dChiIC;
  dest[iBody].dMassOC=src[iBody].dMassOC;
  dest[iBody].dMassIC=src[iBody].dMassIC;
  dest[iBody].dMassChiOC=src[iBody].dMassChiOC;
  dest[iBody].dMassChiIC=src[iBody].dMassChiIC;
  dest[iBody].dDTChi=src[iBody].dDTChi;
  dest[iBody].dHfluxCMBAd=src[iBody].dHfluxCMBAd;
  dest[iBody].dHfluxCMBConv=src[iBody].dHfluxCMBConv;
  dest[iBody].dThermConductOC=src[iBody].dThermConductOC;
  dest[iBody].dRICDot=src[iBody].dRICDot;
  dest[iBody].dCoreBuoyTherm=src[iBody].dCoreBuoyTherm;
  dest[iBody].dCoreBuoyCompo=src[iBody].dCoreBuoyCompo;
  dest[iBody].dCoreBuoyTotal=src[iBody].dCoreBuoyTotal;
  dest[iBody].dGravICB=src[iBody].dGravICB;
  dest[iBody].dMagMomCoef=src[iBody].dMagMomCoef;
  dest[iBody].dMagMom=src[iBody].dMagMom;
}

/**************** RADHEAT options ********************/

/* Initial Mantle & Core Temperature */

void ReadTMan(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  int lTmp=-1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn,options->cName,&dTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {   //if line num of option ge 0
    NotPrimaryInput(iFile,options->cName,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);
    if (dTmp < 0)   //if input value lt 0
      body[iFile-1].dTMan = dTmp*dNegativeDouble(*options,files->Infile[iFile].cIn,control->Io.iVerbose);
    else
      body[iFile-1].dTMan = fdUnitsTemp(dTmp,control->Units[iFile].iTemp,0);
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else
    if (iFile > 0)  //if line num not ge 0, then if iFile gt 0, then set default.
      body[iFile-1].dTMan = options->dDefault;
}

void ReadTCore(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  int lTmp=-1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn,options->cName,&dTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {   //if line num of option ge 0
    NotPrimaryInput(iFile,options->cName,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);
    if (dTmp < 0)   //if input value lt 0
      body[iFile-1].dTCore = dTmp*dNegativeDouble(*options,files->Infile[iFile].cIn,control->Io.iVerbose);
   else
       body[iFile-1].dTCore = fdUnitsTemp(dTmp,control->Units[iFile].iTemp,0);
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else
      if (iFile > 0)  //if line num not ge 0, then if iFile gt 0, then set default.
      body[iFile-1].dTCore = options->dDefault;
}
void ReadViscJumpMan(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  int lTmp=-1;
  double dTmp;
  AddOptionDouble(files->Infile[iFile].cIn,options->cName,&dTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {   //if line num of option ge 0
    NotPrimaryInput(iFile,options->cName,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);
    if (dTmp < 0)   //if input value lt 0
      body[iFile-1].dViscJumpMan = dTmp*dNegativeDouble(*options,files->Infile[iFile].cIn,control->Io.iVerbose);
   else
     body[iFile-1].dViscJumpMan = dTmp;  //no units.
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else
      if (iFile > 0)  //if line num not ge 0, then if iFile gt 0, then set default.
      body[iFile-1].dViscJumpMan = options->dDefault;
}
void ReadViscRef(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  int lTmp=-1;
  double dTmp;
  AddOptionDouble(files->Infile[iFile].cIn,options->cName,&dTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {   //if line num of option ge 0
    NotPrimaryInput(iFile,options->cName,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);
    if (dTmp < 0)   //if input value lt 0
      body[iFile-1].dViscRef = dTmp*dNegativeDouble(*options,files->Infile[iFile].cIn,control->Io.iVerbose);
   else
     body[iFile-1].dViscRef = dTmp;  //no units.
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else
      if (iFile > 0)  //if line num not ge 0, then if iFile gt 0, then set default.
      body[iFile-1].dViscRef = options->dDefault;
}

void ReadTrefLind(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  int lTmp=-1;
  double dTmp;
  AddOptionDouble(files->Infile[iFile].cIn,options->cName,&dTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {   //if line num of option ge 0
    NotPrimaryInput(iFile,options->cName,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);
    if (dTmp < 0)   //if input value lt 0
      body[iFile-1].dTrefLind = dTmp*dNegativeDouble(*options,files->Infile[iFile].cIn,control->Io.iVerbose);
   else
     body[iFile-1].dTrefLind = dTmp;  //no units.
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else
      if (iFile > 0)  //if line num not ge 0, then if iFile gt 0, then set default.
      body[iFile-1].dTrefLind = options->dDefault;
}
void ReadDTChiRef(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  int lTmp=-1;
  double dTmp;
  AddOptionDouble(files->Infile[iFile].cIn,options->cName,&dTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {   //if line num of option ge 0
    NotPrimaryInput(iFile,options->cName,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);
    if (dTmp < 0)   //if input value lt 0
      body[iFile-1].dDTChiRef = dTmp*dNegativeDouble(*options,files->Infile[iFile].cIn,control->Io.iVerbose);
   else
     body[iFile-1].dDTChiRef = dTmp;  //no units.
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else
      if (iFile > 0)  //if line num not ge 0, then if iFile gt 0, then set default.
      body[iFile-1].dDTChiRef = options->dDefault;
}

void ReadEruptEff(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  int lTmp=-1;
  double dTmp;
  AddOptionDouble(files->Infile[iFile].cIn,options->cName,&dTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {   //if line num of option ge 0
    NotPrimaryInput(iFile,options->cName,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);
    if (dTmp < 0)   //if input value lt 0
      body[iFile-1].dEruptEff = dTmp*dNegativeDouble(*options,files->Infile[iFile].cIn,control->Io.iVerbose);
   else
     body[iFile-1].dEruptEff = dTmp;  //no units.
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else
      if (iFile > 0)  //if line num not ge 0, then if iFile gt 0, then set default.
      body[iFile-1].dEruptEff = options->dDefault;
}

void ReadViscMeltPhis(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  int lTmp=-1;
  double dTmp;
  AddOptionDouble(files->Infile[iFile].cIn,options->cName,&dTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {   //if line num of option ge 0
    NotPrimaryInput(iFile,options->cName,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);
    if (dTmp < 0)   //if input value lt 0
      body[iFile-1].dViscMeltPhis = dTmp*dNegativeDouble(*options,files->Infile[iFile].cIn,control->Io.iVerbose);
   else
     body[iFile-1].dViscMeltPhis = dTmp;  //no units.
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else
      if (iFile > 0)  //if line num not ge 0, then if iFile gt 0, then set default.
      body[iFile-1].dViscMeltPhis = options->dDefault;
}
void ReadViscMeltXi(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  int lTmp=-1;
  double dTmp;
  AddOptionDouble(files->Infile[iFile].cIn,options->cName,&dTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {   //if line num of option ge 0
    NotPrimaryInput(iFile,options->cName,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);
    if (dTmp < 0)   //if input value lt 0
      body[iFile-1].dViscMeltXi = dTmp*dNegativeDouble(*options,files->Infile[iFile].cIn,control->Io.iVerbose);
   else
     body[iFile-1].dViscMeltXi = dTmp;  //no units.
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else
      if (iFile > 0)  //if line num not ge 0, then if iFile gt 0, then set default.
      body[iFile-1].dViscMeltXi = options->dDefault;
}
void ReadViscMeltGamma(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  int lTmp=-1;
  double dTmp;
  AddOptionDouble(files->Infile[iFile].cIn,options->cName,&dTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {   //if line num of option ge 0
    NotPrimaryInput(iFile,options->cName,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);
    if (dTmp < 0)   //if input value lt 0
      body[iFile-1].dViscMeltGamma = dTmp*dNegativeDouble(*options,files->Infile[iFile].cIn,control->Io.iVerbose);
   else
     body[iFile-1].dViscMeltGamma = dTmp;  //no units.
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else
      if (iFile > 0)  //if line num not ge 0, then if iFile gt 0, then set default.
      body[iFile-1].dViscMeltGamma = options->dDefault;
}
void ReadViscMeltDelta(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  int lTmp=-1;
  double dTmp;
  AddOptionDouble(files->Infile[iFile].cIn,options->cName,&dTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {   //if line num of option ge 0
    NotPrimaryInput(iFile,options->cName,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);
    if (dTmp < 0)   //if input value lt 0
      body[iFile-1].dViscMeltDelta = dTmp*dNegativeDouble(*options,files->Infile[iFile].cIn,control->Io.iVerbose);
   else
     body[iFile-1].dViscMeltDelta = dTmp;  //no units.
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else
      if (iFile > 0)  //if line num not ge 0, then if iFile gt 0, then set default.
      body[iFile-1].dViscMeltDelta = options->dDefault;
}
void ReadViscMeltB(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  int lTmp=-1;
  double dTmp;
  AddOptionDouble(files->Infile[iFile].cIn,options->cName,&dTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {   //if line num of option ge 0
    NotPrimaryInput(iFile,options->cName,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);
    if (dTmp < 0)   //if input value lt 0
      body[iFile-1].dViscMeltB = dTmp*dNegativeDouble(*options,files->Infile[iFile].cIn,control->Io.iVerbose);
   else
     body[iFile-1].dViscMeltB = dTmp;  //no units.
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else
      if (iFile > 0)  //if line num not ge 0, then if iFile gt 0, then set default.
      body[iFile-1].dViscMeltB = options->dDefault;
}

void ReadFixMeltfactorUMan(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  int lTmp=-1;
  double dTmp;
  AddOptionDouble(files->Infile[iFile].cIn,options->cName,&dTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {   //if line num of option ge 0
    NotPrimaryInput(iFile,options->cName,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);
    if (dTmp < 0)   //if input value lt 0
      body[iFile-1].dFixMeltfactorUMan = dTmp*dNegativeDouble(*options,files->Infile[iFile].cIn,control->Io.iVerbose);
   else
     body[iFile-1].dFixMeltfactorUMan = dTmp;  //no units.
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else
      if (iFile > 0)  //if line num not ge 0, then if iFile gt 0, then set default.
      body[iFile-1].dFixMeltfactorUMan = options->dDefault;
}
void ReadMeltfactorUMan(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  /* This input option only applies when FixMeltfactorUMan=1 */
  int lTmp=-1;
  double dTmp;
  AddOptionDouble(files->Infile[iFile].cIn,options->cName,&dTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {   //if line num of option ge 0
    NotPrimaryInput(iFile,options->cName,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);
    if (dTmp < 0)   //if input value lt 0
      body[iFile-1].dMeltfactorUMan = dTmp*dNegativeDouble(*options,files->Infile[iFile].cIn,control->Io.iVerbose);
   else
     body[iFile-1].dMeltfactorUMan = dTmp;  //no units.
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else
      if (iFile > 0)  //if line num not ge 0, then if iFile gt 0, then set default.
      body[iFile-1].dMeltfactorUMan = options->dDefault;
}
void ReadStagLid(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  int lTmp=-1;
  double dTmp;
  AddOptionDouble(files->Infile[iFile].cIn,options->cName,&dTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {   //if line num of option ge 0
    NotPrimaryInput(iFile,options->cName,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);
    if (dTmp < 0)   //if input value lt 0
      body[iFile-1].dStagLid = dTmp*dNegativeDouble(*options,files->Infile[iFile].cIn,control->Io.iVerbose);
   else
     body[iFile-1].dStagLid = dTmp;  //no units.
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else
      if (iFile > 0)  //if line num not ge 0, then if iFile gt 0, then set default.
      body[iFile-1].dStagLid = options->dDefault;
}
void ReadManHFlowPref(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  int lTmp=-1;
  double dTmp;
  AddOptionDouble(files->Infile[iFile].cIn,options->cName,&dTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {   //if line num of option ge 0
    NotPrimaryInput(iFile,options->cName,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);
    if (dTmp < 0)   //if input value lt 0
      body[iFile-1].dManHFlowPref = dTmp*dNegativeDouble(*options,files->Infile[iFile].cIn,control->Io.iVerbose);
   else
     body[iFile-1].dManHFlowPref = dTmp;  //no units.
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else
      if (iFile > 0)  //if line num not ge 0, then if iFile gt 0, then set default.
      body[iFile-1].dManHFlowPref = options->dDefault;
}
void ReadMagMomCoef(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  int lTmp=-1;
  double dTmp;
  AddOptionDouble(files->Infile[iFile].cIn,options->cName,&dTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {   //if line num of option ge 0
    NotPrimaryInput(iFile,options->cName,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);
    if (dTmp < 0)   //if input value lt 0
      body[iFile-1].dMagMomCoef = dTmp*dNegativeDouble(*options,files->Infile[iFile].cIn,control->Io.iVerbose);
   else
     body[iFile-1].dMagMomCoef = dTmp;  //no units.
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else
      if (iFile > 0)  //if line num not ge 0, then if iFile gt 0, then set default.
      body[iFile-1].dMagMomCoef = options->dDefault;
}

void ReadHaltMinTMan(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  /* This parameter cannot exist in primary file */
  /* Must verify in conjuction with 235UPower and 235UMass */
  int lTmp=-1;
  double dTmp;
  AddOptionDouble(files->Infile[iFile].cIn,options->cName,&dTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {
      NotPrimaryInput(iFile,options->cName,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);
      if (dTmp < 0)
	control->Halt[iFile-1].dMinTMan = dTmp*dNegativeDouble(*options,files->Infile[iFile].cIn,control->Io.iVerbose);
      else
	control->Halt[iFile-1].dMinTMan = fdUnitsTemp(dTmp,control->Units[iFile].iTemp,0);

      UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else
      if (iFile > 0)
       control->Halt[iFile-1].dMinTMan = options->dDefault;
}

void ReadHaltMinTCore(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  /* This parameter cannot exist in primary file */
  /* Must verify in conjuction with 235UPower and 235UMass */
  int lTmp=-1;
  double dTmp;
  AddOptionDouble(files->Infile[iFile].cIn,options->cName,&dTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {
      NotPrimaryInput(iFile,options->cName,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);
      if (dTmp < 0)
	control->Halt[iFile-1].dMinTCore = dTmp*dNegativeDouble(*options,files->Infile[iFile].cIn,control->Io.iVerbose);
      else
	control->Halt[iFile-1].dMinTCore = fdUnitsTemp(dTmp,control->Units[iFile].iTemp,0);

      UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else
      if (iFile > 0)
       control->Halt[iFile-1].dMinTCore = options->dDefault;
}

/* Initiatlize Input Options */

void InitializeOptionsThermint(OPTIONS *options,fnReadOption fnRead[]) {
  int iOpt,iFile;

  /* TMan */
  sprintf(options[OPT_TMAN].cName,"dTMan");
  sprintf(options[OPT_TMAN].cDescr,"Initial Mantle Temperature");
  sprintf(options[OPT_TMAN].cDefault,"Default=3000");
  options[OPT_TMAN].iType = 2;
  options[OPT_TMAN].iMultiFile = 1;
  options[OPT_TMAN].dNeg = 3000.0;  //Not sure about this??
  options[OPT_TMAN].dDefault = 3000.0; 
  sprintf(options[OPT_TMAN].cNeg,"Default=3000");
  fnRead[OPT_TMAN] = &ReadTMan;

   /* TCore */
  sprintf(options[OPT_TCORE].cName,"dTCore");
  sprintf(options[OPT_TCORE].cDescr,"Initial Core Temperature");
  sprintf(options[OPT_TCORE].cDefault,"Default=6000");
  options[OPT_TCORE].iType = 2;
  options[OPT_TCORE].iMultiFile = 1;
  options[OPT_TCORE].dNeg = 6000.0;  //Not sure about this??
  options[OPT_TCORE].dDefault = 6000.0; 
  sprintf(options[OPT_TCORE].cNeg,"Default=6000");
  fnRead[OPT_TCORE] = &ReadTCore;

   /* ViscJumpMan */
  sprintf(options[OPT_VISCJUMPMAN].cName,"dViscJumpMan");
  sprintf(options[OPT_VISCJUMPMAN].cDescr,"ViscJumpMan");
  sprintf(options[OPT_VISCJUMPMAN].cDefault,"Default is VISCJUMPMAN");
  options[OPT_VISCJUMPMAN].iType = 2;
  options[OPT_VISCJUMPMAN].iMultiFile = 1;
  options[OPT_VISCJUMPMAN].dNeg = VISCJUMPMAN;
  options[OPT_VISCJUMPMAN].dDefault = VISCJUMPMAN; 
  sprintf(options[OPT_VISCJUMPMAN].cNeg,"Default");
  fnRead[OPT_VISCJUMPMAN] = &ReadViscJumpMan;

   /* ViscRef */
  sprintf(options[OPT_VISCREF].cName,"dViscRef");
  sprintf(options[OPT_VISCREF].cDescr,"ViscRef");
  sprintf(options[OPT_VISCREF].cDefault,"Default=1");
  options[OPT_VISCREF].iType = 2;
  options[OPT_VISCREF].iMultiFile = 1;
  options[OPT_VISCREF].dNeg = VISCREF;
  options[OPT_VISCREF].dDefault = VISCREF; 
  sprintf(options[OPT_VISCREF].cNeg,"Default value in thermint.h");
  fnRead[OPT_VISCREF] = &ReadViscRef;

   /* TrefLind */
  sprintf(options[OPT_TREFLIND].cName,"dTrefLind");
  sprintf(options[OPT_TREFLIND].cDescr,"TrefLind");
  sprintf(options[OPT_TREFLIND].cDefault,"Default in thermint.h");
  options[OPT_TREFLIND].iType = 2;
  options[OPT_TREFLIND].iMultiFile = 1;
  options[OPT_TREFLIND].dNeg = 1;
  options[OPT_TREFLIND].dDefault = TREFLIND; 
  sprintf(options[OPT_TREFLIND].cNeg,"Default in thermint.h");
  fnRead[OPT_TREFLIND] = &ReadTrefLind;

  /* DTChiRef */
  sprintf(options[OPT_DTCHIREF].cName,"dDTChiRef");
  sprintf(options[OPT_DTCHIREF].cDescr,"DTChiRef");
  sprintf(options[OPT_DTCHIREF].cDefault,"Default in thermint.h");
  options[OPT_DTCHIREF].iType = 2;
  options[OPT_DTCHIREF].iMultiFile = 1;
  options[OPT_DTCHIREF].dNeg = 1;
  options[OPT_DTCHIREF].dDefault = DTCHIREF; 
  sprintf(options[OPT_DTCHIREF].cNeg,"Default in thermint.h");
  fnRead[OPT_DTCHIREF] = &ReadDTChiRef;

  /* EruptEff */
  sprintf(options[OPT_ERUPTEFF].cName,"dEruptEff");
  sprintf(options[OPT_ERUPTEFF].cDescr,"Melt Eruption Efficiency");
  sprintf(options[OPT_ERUPTEFF].cDefault,"Default is ERUPTEFF");
  options[OPT_ERUPTEFF].iType = 2;
  options[OPT_ERUPTEFF].iMultiFile = 1;
  options[OPT_ERUPTEFF].dNeg = ERUPTEFF;
  options[OPT_ERUPTEFF].dDefault = ERUPTEFF; 
  sprintf(options[OPT_ERUPTEFF].cNeg,"Default is ERUPTEFF");
  fnRead[OPT_ERUPTEFF] = &ReadEruptEff;

  /* ViscMeltPhis */
  sprintf(options[OPT_VISCMELTPHIS].cName,"dViscMeltPhis");
  sprintf(options[OPT_VISCMELTPHIS].cDescr,"Viscosity Melt Factor Phi star");
  sprintf(options[OPT_VISCMELTPHIS].cDefault,"Default is VISCMELTPHIS");
  options[OPT_VISCMELTPHIS].iType = 2;
  options[OPT_VISCMELTPHIS].iMultiFile = 1;
  options[OPT_VISCMELTPHIS].dNeg = VISCMELTPHIS;
  options[OPT_VISCMELTPHIS].dDefault = VISCMELTPHIS; 
  sprintf(options[OPT_VISCMELTPHIS].cNeg,"Default is VISCMELTPHIS");
  fnRead[OPT_VISCMELTPHIS] = &ReadViscMeltPhis;
  /* ViscMeltXi */
  sprintf(options[OPT_VISCMELTXI].cName,"dViscMeltXi");
  sprintf(options[OPT_VISCMELTXI].cDescr,"Viscosity Melt Factor Xi");
  sprintf(options[OPT_VISCMELTXI].cDefault,"Default is VISCMELTXI");
  options[OPT_VISCMELTXI].iType = 2;
  options[OPT_VISCMELTXI].iMultiFile = 1;
  options[OPT_VISCMELTXI].dNeg = VISCMELTXI;
  options[OPT_VISCMELTXI].dDefault = VISCMELTXI; 
  sprintf(options[OPT_VISCMELTXI].cNeg,"Default is VISCMELTXI");
  fnRead[OPT_VISCMELTXI] = &ReadViscMeltXi;
  /* ViscMeltGamma */
  sprintf(options[OPT_VISCMELTGAMMA].cName,"dViscMeltGamma");
  sprintf(options[OPT_VISCMELTGAMMA].cDescr,"Viscosity Melt Factor Gamma");
  sprintf(options[OPT_VISCMELTGAMMA].cDefault,"Default is VISCMELTGAMMA");
  options[OPT_VISCMELTGAMMA].iType = 2;
  options[OPT_VISCMELTGAMMA].iMultiFile = 1;
  options[OPT_VISCMELTGAMMA].dNeg = VISCMELTGAMMA;
  options[OPT_VISCMELTGAMMA].dDefault = VISCMELTGAMMA; 
  sprintf(options[OPT_VISCMELTGAMMA].cNeg,"Default is VISCMELTGAMMA");
  fnRead[OPT_VISCMELTGAMMA] = &ReadViscMeltGamma;
  /* ViscMeltDelta */
  sprintf(options[OPT_VISCMELTDELTA].cName,"dViscMeltDelta");
  sprintf(options[OPT_VISCMELTDELTA].cDescr,"Viscosity Melt Factor Delta");
  sprintf(options[OPT_VISCMELTDELTA].cDefault,"Default is VISCMELTDELTA");
  options[OPT_VISCMELTDELTA].iType = 2;
  options[OPT_VISCMELTDELTA].iMultiFile = 1;
  options[OPT_VISCMELTDELTA].dNeg = VISCMELTDELTA;
  options[OPT_VISCMELTDELTA].dDefault = VISCMELTDELTA; 
  sprintf(options[OPT_VISCMELTDELTA].cNeg,"Default is VISCMELTDELTA");
  fnRead[OPT_VISCMELTDELTA] = &ReadViscMeltDelta;
  /* ViscMeltB */
  sprintf(options[OPT_VISCMELTB].cName,"dViscMeltB");
  sprintf(options[OPT_VISCMELTB].cDescr,"Viscosity Melt Factor B");
  sprintf(options[OPT_VISCMELTB].cDefault,"Default is VISCMELTB");
  options[OPT_VISCMELTB].iType = 2;
  options[OPT_VISCMELTB].iMultiFile = 1;
  options[OPT_VISCMELTB].dNeg = VISCMELTB;
  options[OPT_VISCMELTB].dDefault = VISCMELTB; 
  sprintf(options[OPT_VISCMELTB].cNeg,"Default is VISCMELTB");
  fnRead[OPT_VISCMELTB] = &ReadViscMeltB;
  
  /* MeltfactorUMan */
  sprintf(options[OPT_MELTFACTORUMAN].cName,"dMeltfactorUMan");
  sprintf(options[OPT_MELTFACTORUMAN].cDescr,"Viscosity Melt Factor");
  sprintf(options[OPT_MELTFACTORUMAN].cDefault,"Default is MELTFACTORUMAN");
  options[OPT_MELTFACTORUMAN].iType = 2;
  options[OPT_MELTFACTORUMAN].iMultiFile = 1;
  options[OPT_MELTFACTORUMAN].dNeg = MELTFACTORUMAN;
  options[OPT_MELTFACTORUMAN].dDefault = MELTFACTORUMAN; 
  sprintf(options[OPT_MELTFACTORUMAN].cNeg,"Default is MELTFACTORUMAN");
  fnRead[OPT_MELTFACTORUMAN] = &ReadMeltfactorUMan;
  /* FixMeltfactorUMan */
  sprintf(options[OPT_FIXMELTFACTORUMAN].cName,"dFixMeltfactorUMan");
  sprintf(options[OPT_FIXMELTFACTORUMAN].cDescr,"Fix Viscosity Melt Factor");
  sprintf(options[OPT_FIXMELTFACTORUMAN].cDefault,"Default is FIXMELTFACTORUMAN");
  options[OPT_FIXMELTFACTORUMAN].iType = 2;
  options[OPT_FIXMELTFACTORUMAN].iMultiFile = 1;
  options[OPT_FIXMELTFACTORUMAN].dNeg = FIXMELTFACTORUMAN;
  options[OPT_FIXMELTFACTORUMAN].dDefault = FIXMELTFACTORUMAN; 
  sprintf(options[OPT_FIXMELTFACTORUMAN].cNeg,"Default is FIXMELTFACTORUMAN");
  fnRead[OPT_FIXMELTFACTORUMAN] = &ReadFixMeltfactorUMan;

  /* StagLid */
  sprintf(options[OPT_STAGLID].cName,"dStagLid");
  sprintf(options[OPT_STAGLID].cDescr,"Stagnant Lid Switch");
  sprintf(options[OPT_STAGLID].cDefault,"Default is STAGLID");
  options[OPT_STAGLID].iType = 2;
  options[OPT_STAGLID].iMultiFile = 1;
  options[OPT_STAGLID].dNeg = STAGLID;
  options[OPT_STAGLID].dDefault = STAGLID; 
  sprintf(options[OPT_STAGLID].cNeg,"Default is STAGLID");
  fnRead[OPT_STAGLID] = &ReadStagLid;
  /* ManHFlowPref */
  sprintf(options[OPT_MANHFLOWPREF].cName,"dManHFlowPref");
  sprintf(options[OPT_MANHFLOWPREF].cDescr,"Mantle HFlow Prefix");
  sprintf(options[OPT_MANHFLOWPREF].cDefault,"Default is MANHFLOWPREF");
  options[OPT_MANHFLOWPREF].iType = 2;
  options[OPT_MANHFLOWPREF].iMultiFile = 1;
  options[OPT_MANHFLOWPREF].dNeg = MANHFLOWPREF;
  options[OPT_MANHFLOWPREF].dDefault = MANHFLOWPREF; 
  sprintf(options[OPT_MANHFLOWPREF].cNeg,"Default is MANHFLOWPREF");
  fnRead[OPT_MANHFLOWPREF] = &ReadManHFlowPref;

   /* MagMomCoef */
  sprintf(options[OPT_MAGMOMCOEF].cName,"dMagMomCoef");
  sprintf(options[OPT_MAGMOMCOEF].cDescr,"Magnetic Moment Coefficient");
  sprintf(options[OPT_MAGMOMCOEF].cDefault,"Default is MAGMOMCOEF");
  options[OPT_MAGMOMCOEF].iType = 2;
  options[OPT_MAGMOMCOEF].iMultiFile = 1;
  options[OPT_MAGMOMCOEF].dNeg = MAGMOMCOEF;
  options[OPT_MAGMOMCOEF].dDefault = MAGMOMCOEF; 
  sprintf(options[OPT_MAGMOMCOEF].cNeg,"Default is MAGMOMCOEF");
  fnRead[OPT_MAGMOMCOEF] = &ReadMagMomCoef;
  
  /* Halt at Minimum Mantle Temperature */
  sprintf(options[OPT_HALTMINTMAN].cName,"dHaltMinTMan");
  sprintf(options[OPT_HALTMINTMAN].cDescr,"Halt at Minimum Mantle Temperature");
  sprintf(options[OPT_HALTMINTMAN].cDefault,"0 K");
  options[OPT_HALTMINTMAN].iType = 2;
  options[OPT_HALTMINTMAN].iMultiFile = 1;
  options[OPT_HALTMINTMAN].dNeg = 1;
  options[OPT_HALTMINTMAN].dDefault = 0; 
  sprintf(options[OPT_HALTMINTMAN].cNeg,"0");
  fnRead[OPT_HALTMINTMAN] = &ReadHaltMinTMan;

  /* Halt at Minimum Core Temperature */
  sprintf(options[OPT_HALTMINTCORE].cName,"dHaltMinTCore");
  sprintf(options[OPT_HALTMINTCORE].cDescr,"Halt at Minimum Core Temperature");
  sprintf(options[OPT_HALTMINTCORE].cDefault,"0 K");
  options[OPT_HALTMINTCORE].iType = 2;
  options[OPT_HALTMINTCORE].iMultiFile = 1;
  options[OPT_HALTMINTCORE].dNeg = 1;
  options[OPT_HALTMINTCORE].dDefault = 0; 
  sprintf(options[OPT_HALTMINTCORE].cNeg,"0");
  fnRead[OPT_HALTMINTCORE] = &ReadHaltMinTCore;

}

void ReadOptionsThermint(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,fnReadOption fnRead[],int iBody) {
  int iOpt;

  for (iOpt=OPTSTARTTHERMINT;iOpt<OPTENDTHERMINT;iOpt++) {
    if (options[iOpt].iType != -1) 
      fnRead[iOpt](body,control,files,&options[iOpt],system,iBody+1);
  }
}
    
/******************* Verify THERMINT ******************/

/* Is this necessary? */
void AssignTMan(BODY *body,OPTIONS *options,double dAge,int iBody) {
    /* Mantle */
    /*    if (options[OPT_TMAN].iLine[iBody+1] >= 0) {
        body[iBody].d40KNumMan=body[iBody].d40KMassMan/(MASS40K);
        printf("40KMassMan set, body[iBody].d40KNumMan=%e, ENUMMAN40K=%e\n",body[iBody].d40KNumMan,ENUMMAN40K);
    }
    body[iBody].dTMan = fd40KConstant(body[iBody].d40KNumMan,dAge);  //Get the constant given num and age.
    */
}


void VerifyTMan(BODY *body,OPTIONS *options,UPDATE *update,double dAge,fnUpdateVariable ***fnUpdate,int iBody) {
    //  AssignTMan(body,options,dAge,iBody);
    /* Mantle */
    update[iBody].iaType[update[iBody].iTMan][0] = 1; //iaType=0 for prescribed evolution, =1 for differential evolution (normal)
    update[iBody].iNumBodies[update[iBody].iTMan][0]=1;
    update[iBody].iaBody[update[iBody].iTMan][0] = malloc(update[iBody].iNumBodies[update[iBody].iTMan][0]*sizeof(int)); //iaBody is the number of bodies that are affected by this variable.
    update[iBody].iaBody[update[iBody].iTMan][0][0]=iBody;
    update[iBody].pdTDotMan = &update[iBody].daDerivProc[update[iBody].iTMan][0];
    fnUpdate[iBody][update[iBody].iTMan][0] = &fdTDotMan;
}

void VerifyTCore(BODY *body,OPTIONS *options,UPDATE *update,double dAge,fnUpdateVariable ***fnUpdate,int iBody) {
    //  AssignTCore(body,options,dAge,iBody);
    /* Core */
    update[iBody].iaType[update[iBody].iTCore][0] = 1; //iaType=0 for prescribed evolution, =1 for differential evolution (normal)
    update[iBody].iNumBodies[update[iBody].iTCore][0]=1;
    update[iBody].iaBody[update[iBody].iTCore][0] = malloc(update[iBody].iNumBodies[update[iBody].iTCore][0]*sizeof(int)); //iaBody is the number of bodies that are affected by this variable.
    update[iBody].iaBody[update[iBody].iTCore][0][0]=iBody;
    update[iBody].pdTDotCore = &update[iBody].daDerivProc[update[iBody].iTCore][0];
    fnUpdate[iBody][update[iBody].iTCore][0] = &fdTDotCore;
}



/******************************  AUX PROPS  ***********************************************/
/* Auxiliary Properties */
void PropsAuxThermint(BODY *body,EVOLVE *evolve,UPDATE *update,int iBody) {
  /* Scalar Properties */
  body[iBody].dTUMan=fdTUMan(body,iBody);
  body[iBody].dTLMan=fdTLMan(body,iBody);
  body[iBody].dTCMB=fdTCMB(body,iBody);
  body[iBody].dTJumpUMan=fdTJumpUMan(body,iBody);
  body[iBody].dTJumpLMan=fdTJumpLMan(body,iBody);
  body[iBody].dSignTJumpUMan=fdSignTJumpUMan(body,iBody);
  body[iBody].dSignTJumpLMan=fdSignTJumpLMan(body,iBody);
  //  body[iBody].dViscJumpMan=fdViscJumpMan(body,iBody);
  if (body[iBody].dMeltfactorUMan==0)
      body[iBody].dMeltfactorUMan=1.;  //initialize to avoid visc=visc/meltfactor crash.
  if (body[iBody].dMeltfactorLMan==0)
      body[iBody].dMeltfactorLMan=1.;  //initialize to avoid visc=visc/meltfactor crash.
  /* Loop through melt calculation once to get dependence of visc on melt. */
  int i=0, nloop=1;
  for (i=0;i<nloop;i++) {
      body[iBody].dViscUManArr=fdViscUManArr(body,iBody);
      body[iBody].dViscUMan=fdViscUMan(body,iBody);
      body[iBody].dViscLMan=fdViscLMan(body,iBody);
      body[iBody].dBLUMan=fdBLUMan(body,iBody);
      body[iBody].dBLLMan=fdBLLMan(body,iBody);
      body[iBody].dShmodUMan=fdShmodUMan(body,iBody);
      body[iBody].dTsolUMan=fdTsolUMan(body,iBody);
      body[iBody].dTliqUMan=fdTliqUMan(body,iBody);
      body[iBody].dFMeltUMan=fdFMeltUMan(body,iBody);
      body[iBody].dMeltfactorUMan=fdMeltfactorUMan(body,iBody);
      //      body[iBody].dFixMeltfactorUMan=fdFixMeltfactorUMan(body,iBody);
      body[iBody].dTsolLMan=fdTsolLMan(body,iBody);
      body[iBody].dTliqLMan=fdTliqLMan(body,iBody);
      body[iBody].dFMeltLMan=fdFMeltLMan(body,iBody);
      body[iBody].dMeltfactorLMan=fdMeltfactorLMan(body,iBody);
  }
  body[iBody].dDepthMeltMan=fdDepthMeltMan(body,iBody);
  body[iBody].dTDepthMeltMan=fdTDepthMeltMan(body,iBody);
  body[iBody].dTJumpMeltMan=fdTJumpMeltMan(body,iBody);
  body[iBody].dMeltMassFluxMan=fdMeltMassFluxMan(body,iBody);
  body[iBody].dViscMMan=fdViscMMan(body,iBody);
  body[iBody].dRayleighMan=fdRayleighMan(body,iBody);

  /* Heat Flows */
  /* Mantle */
  body[iBody].dHfluxUMan=fdHfluxUMan(body,iBody);
  body[iBody].dHfluxLMan=fdHfluxLMan(body,iBody);
  body[iBody].dHfluxCMB=fdHfluxCMB(body,iBody);
  body[iBody].dHflowUMan=fdHflowUMan(body,iBody);
  body[iBody].dHflowLMan=fdHflowLMan(body,iBody);
  body[iBody].dHflowCMB=fdHflowCMB(body,iBody);
  body[iBody].dHflowLatentMan=fdHflowLatentMan(body,update,iBody);
  body[iBody].dHflowMeltMan=fdHflowMeltMan(body,iBody);
  body[iBody].dHflowSecMan=fdHflowSecMan(body,iBody);
  body[iBody].dHflowSurf=fdHflowSurf(body,iBody);
  /* Core */
  /* Iterate on Core chemistry before R_ICB */
  body[iBody].dMassIC=fdMassIC(body,iBody);
  body[iBody].dMassOC=fdMassOC(body,iBody);
  body[iBody].dMassChiOC=fdMassChiOC(body,iBody);
  body[iBody].dMassChiIC=fdMassChiIC(body,iBody);
  body[iBody].dChiOC=fdChiOC(body,iBody);       
  body[iBody].dChiIC=fdChiIC(body,iBody);
  body[iBody].dDTChi=fdDTChi(body,iBody);
  body[iBody].dRIC=fdRIC(body,iBody);
  body[iBody].dDRICDTCMB=fdDRICDTCMB(body,iBody);
  body[iBody].dMassICDot=fdMassICDot(body,update,iBody);
  body[iBody].dHflowLatentIC=fdHflowLatentIC(body,update,iBody);
  body[iBody].dPowerGravIC=fdPowerGravIC(body,update,iBody);
  body[iBody].dHfluxCMBAd=fdHfluxCMBAd(body,iBody);
  body[iBody].dHfluxCMBConv=fdHfluxCMBConv(body,iBody);
  body[iBody].dThermConductOC=fdThermConductOC(body,iBody);
  body[iBody].dRICDot=fdRICDot(body,update,iBody);
  body[iBody].dGravICB=fdGravICB(body,iBody);
  body[iBody].dCoreBuoyTherm=fdCoreBuoyTherm(body,iBody);
  body[iBody].dCoreBuoyCompo=fdCoreBuoyCompo(body,iBody);
  body[iBody].dCoreBuoyTotal=fdCoreBuoyTotal(body,iBody);
  body[iBody].dMagMom=fdMagMom(body,iBody);
}

void fnForceBehaviorThermint(BODY *body,EVOLVE *evolve,IO *io,SYSTEM *system,UPDATE *update,fnUpdateVariable ***fnUpdate,int iBody,int iModule) {
  // XXX Reset fnUpdate functions to SetDerivTiny?
  if (body[iBody].dTMan < 0.5)
    body[iBody].dTMan = 0;
  if (body[iBody].dTCore < 0.5)
    body[iBody].dTCore = 0;
}

void VerifyThermint(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,OUTPUT *output,SYSTEM *system,UPDATE *update,fnUpdateVariable ***fnUpdate,int iBody,int iModule) {
  VerifyTMan(body,options,update,body[iBody].dAge,fnUpdate,iBody);  //Verify Man.
  VerifyTCore(body,options,update,body[iBody].dAge,fnUpdate,iBody);  //Verify Core.

  control->fnForceBehavior[iBody][iModule] = &fnForceBehaviorThermint;
  control->fnPropsAux[iBody][iModule] = &PropsAuxThermint;
  control->Evolve.fnBodyCopy[iBody][iModule] = &BodyCopyThermint;
  //  output[OUT_TDOTMAN].fnOutput[iBody][iModule] = &fdTDotMan;
}

/**************** THERMINT update ****************/

void InitializeUpdateThermint(BODY *body,UPDATE *update,int iBody) {
  /* Initially allow all radiogenic heat sources to be present. If any are 0, 
     or < dMinRadPower, they will me removed from update[iBody] in 
     ForceBehavior.
  */
  if (body[iBody].dTMan > 0) {
    update[iBody].iNumVars++;
    update[iBody].iNumTMan++;  //Why is iNumTMan incremented here and below?
  }
  if (body[iBody].dTCore > 0) {
    update[iBody].iNumVars++;
    update[iBody].iNumTCore++;  
  }
}

void FinalizeUpdateTManThermint(BODY *body,UPDATE*update,int *iEqn,int iVar,int iBody,int iFoo) {
  update[iBody].iaModule[iVar][*iEqn] = TMAN;
  update[iBody].iNumTMan = (*iEqn)++;
}

void FinalizeUpdateTCoreThermint(BODY *body,UPDATE*update,int *iEqn,int iVar,int iBody,int iFoo) {
  update[iBody].iaModule[iVar][*iEqn] = TCORE;
  update[iBody].iNumTCore = (*iEqn)++;
}


/***************** RADHEAT Halts *****************/

/* Minimum TMan */
int fbHaltMinTMan(BODY *body,EVOLVE *evolve,HALT *halt,IO *io,UPDATE *update,int iBody) {
  if (body[iBody].dTMan < halt->dMinTMan) {
    if (io->iVerbose >= VERBPROG) {
      printf("HALT: %s's TMan =  ",body[iBody].cName);
      fprintd(stdout,body[iBody].dTMan,io->iSciNot,io->iDigits);
      printf(" < ");
      fprintd(stdout,halt->dMinTMan,io->iSciNot,io->iDigits);
      printf(".\n");
    }
    return 1;
  }
  return 0;
}

int fbHaltMinTCore(BODY *body,EVOLVE *evolve,HALT *halt,IO *io,UPDATE *update,int iBody) {
  if (body[iBody].dTCore < halt->dMinTCore) {
    if (io->iVerbose >= VERBPROG) {
      printf("HALT: %s's TCore =  ",body[iBody].cName);
      fprintd(stdout,body[iBody].dTCore,io->iSciNot,io->iDigits);
      printf(" < ");
      fprintd(stdout,halt->dMinTCore,io->iSciNot,io->iDigits);
      printf(".\n");
    }
    return 1;
  }
  return 0;
}        

// XXX Should change these to bHaltTMan as there is no real need to check

void CountHaltsThermint(HALT *halt,int *iNumHalts) {
  if (halt->dMinTMan >= 0)
    (*iNumHalts)++;
  if (halt->dMinTCore >= 0)
    (*iNumHalts)++;
}

void VerifyHaltThermint(BODY *body,CONTROL *control,OPTIONS *options,int iBody,int *iHalt) {
  if (control->Halt[iBody].dMinTMan >= 0)
    control->fnHalt[iBody][(*iHalt)++] = &fbHaltMinTMan;
  if (control->Halt[iBody].dMinTCore >= 0)
    control->fnHalt[iBody][(*iHalt)++] = &fbHaltMinTCore;
}

/************* THERMINT Outputs ******************/

void HelpOutputThermint(OUTPUT *output) {
  int iOut;

  printf("\n ------ THERMINT output ------\n");
  for (iOut=OUTSTARTTHERMINT;iOut<OUTENDTHERMINT;iOut++) 
    WriteHelpOutput(&output[iOut]);
}

/* NOTE: If you write a new Write subroutine here you need to add the associate 
   block of initialization in InitializeOutputThermint below */

void WriteTMan(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  /* Get TMan */
  *dTmp = body[iBody].dTMan;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else { 
      /*      *dTmp /= fdUnitsTemp(body[iBody].dTman,0,units->iTemp);  //set "iOldType" to 0, second input var, arbitarily.
    fsUnitsTemp(units->iTemp,cUnit);
      */
  }
}
void WriteTUMan(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  /* Get TUMan */
    *dTmp = body[iBody].dTUMan;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else { }
}
void WriteTLMan(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  /* Get TLMan */
    *dTmp = body[iBody].dTLMan;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else { }
}
void WriteTJumpUMan(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  /* Get TLMan */
    *dTmp = body[iBody].dTJumpUMan;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else { }
}
void WriteTJumpLMan(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  /* Get TLMan */
    *dTmp = body[iBody].dTJumpLMan;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else { }
}
void WriteSignTJumpUMan(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  /* Get TLMan */
    *dTmp = body[iBody].dSignTJumpUMan;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else { }
}
void WriteSignTJumpLMan(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  /* Get TLMan */
    *dTmp = body[iBody].dSignTJumpLMan;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else { }
}

void WriteTCMB(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  /* Get TCore */
  *dTmp = body[iBody].dTCMB;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else { }
}
void WriteTCore(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  /* Get TCore */
  *dTmp = body[iBody].dTCore;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else { }
}

void WriteViscUManArr(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  /* Get ViscUManArr */
    *dTmp = body[iBody].dViscUManArr;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else { }
}
void WriteViscUMan(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  /* Get ViscUMan */
    *dTmp = body[iBody].dViscUMan;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else { }
}
void WriteViscLMan(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  /* Get TLMan */
    *dTmp = body[iBody].dViscLMan;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else { }
}
void WriteViscMMan(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  /* Get TMMan */
    *dTmp = body[iBody].dViscMMan;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else { }
}
void WriteViscJumpMan(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  /* Get TLMan */
    *dTmp = body[iBody].dViscJumpMan;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else { }
}
void WriteBLUMan(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
    *dTmp = body[iBody].dBLUMan;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else { }
}
void WriteBLLMan(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
    *dTmp = body[iBody].dBLLMan;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else { }
}
void WriteShmodUMan(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
    *dTmp = body[iBody].dShmodUMan;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else { }
}
void WriteFMeltUMan(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
    *dTmp = body[iBody].dFMeltUMan;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else { }
}
void WriteFMeltLMan(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
    *dTmp = body[iBody].dFMeltLMan;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else { }
}
void WriteMeltfactorUMan(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
    *dTmp = body[iBody].dMeltfactorUMan;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else { }
}
void WriteMeltfactorLMan(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
    *dTmp = body[iBody].dMeltfactorLMan;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else { }
}
void WriteDepthMeltMan(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
    *dTmp = body[iBody].dDepthMeltMan;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else { }
}
void WriteTDepthMeltMan(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
    *dTmp = body[iBody].dTDepthMeltMan;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else { }
}
void WriteTJumpMeltMan(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
    *dTmp = body[iBody].dTJumpMeltMan;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else { }
}
void WriteMeltMassFluxMan(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
    *dTmp = body[iBody].dMeltMassFluxMan;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else { }
}
void WriteRayleighMan(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
    *dTmp = body[iBody].dRayleighMan;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else { }
}
void WriteEruptEff(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
    *dTmp = body[iBody].dEruptEff;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else { }
}
void WriteK2Man(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
    *dTmp = body[iBody].dK2Man;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else { }
}
void WriteImk2Man(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  *dTmp = body[iBody].dImk2Man;
  strcpy(cUnit,"");
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else { }
}
void WriteRIC(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
    *dTmp = body[iBody].dRIC;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else { }
}
void WriteDRICDTCMB(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
    *dTmp = body[iBody].dDRICDTCMB;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else { }
}
void WriteChiOC(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  *dTmp = body[iBody].dChiOC;
  //  *dTmp = body[iBody].dRIC;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else { }
}
void WriteChiIC(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
    *dTmp = body[iBody].dChiIC;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else { }
}
void WriteMassOC(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
    *dTmp = body[iBody].dMassOC;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else { }
}
void WriteMassIC(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
    *dTmp = body[iBody].dMassIC;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else { }
}
void WriteMassChiOC(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
    *dTmp = body[iBody].dMassChiOC;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else { }
}
void WriteMassChiIC(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
    *dTmp = body[iBody].dMassChiIC;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else { }
}
void WriteDTChi(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
    *dTmp = body[iBody].dDTChi;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else { }
}
void WriteThermConductOC(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
    *dTmp = body[iBody].dThermConductOC;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else { }
}
void WriteCoreBuoyTherm(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
    *dTmp = body[iBody].dCoreBuoyTherm;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else { }
}
void WriteCoreBuoyCompo(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
    *dTmp = body[iBody].dCoreBuoyCompo;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else { }
}
void WriteCoreBuoyTotal(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
    *dTmp = body[iBody].dCoreBuoyTotal;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else { }
}
void WriteGravICB(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
    *dTmp = body[iBody].dGravICB;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else { }
}
void WriteMagMom(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
    *dTmp = body[iBody].dMagMom;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else { }
}
void WriteRICDot(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
    *dTmp = body[iBody].dRICDot;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else { }
}

/* Heat Flows/Fluxes */
void WriteHfluxUMan(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
    *dTmp = body[iBody].dHfluxUMan;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else { }
}
void WriteHfluxLMan(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
    *dTmp = body[iBody].dHfluxLMan;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else { }
}
void WriteHfluxCMB(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
    *dTmp = body[iBody].dHfluxCMB;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else { }
}
void WriteHfluxCMBAd(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
    *dTmp = body[iBody].dHfluxCMBAd;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else { }
}
void WriteHfluxCMBConv(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
    *dTmp = body[iBody].dHfluxCMBConv;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else { }
}

void WriteHflowUMan(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
    *dTmp = body[iBody].dHflowUMan;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else { }
}
void WriteHflowSurf(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
    *dTmp = body[iBody].dHflowSurf;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else { }
}

void WriteHflowLMan(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
    *dTmp = body[iBody].dHflowLMan;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else { }
}
void WriteHflowCMB(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
    *dTmp = body[iBody].dHflowCMB;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else { }
}
void WriteHflowLatentMan(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
    *dTmp = body[iBody].dHflowLatentMan;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else { }
}
void WriteHflowMeltMan(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
    *dTmp = body[iBody].dHflowMeltMan;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else { }
}

void WriteHflowSecMan(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  *dTmp = body[iBody].dHflowSecMan;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else { }
}

/* All tidal phenomena should live in eqtide.c
void WriteTidalPowMan(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
    *dTmp = body[iBody].dTidalPowMan;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else { }
}
*/

void WriteHflowLatentIC(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
    *dTmp = body[iBody].dHflowLatentIC;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else { }
}
void WritePowerGravIC(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
    *dTmp = body[iBody].dPowerGravIC;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else { }
}

void WriteTDotMan(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  /* Get TDotMan */
  *dTmp = (*(update[iBody].pdTDotMan));
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else {
      /*
      *dTmp /= fdUnitsTemp(body[iBody].dTman,0,units->iTemp)/fdUnitsTime(units->iTime);
      fsUnitsTempRate(units->iTemp,cUnit);  // only handles Temp/s, need to add yr and Gyr options.
      */
  }
}
void WriteTDotCore(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  /* Get TDotCore */
  *dTmp = (*(update[iBody].pdTDotCore));
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else {
      /*
      *dTmp /= fdUnitsTemp(body[iBody].dTman,0,units->iTemp)/fdUnitsTime(units->iTime);
      fsUnitsTempRate(units->iTemp,cUnit);  // only handles Temp/s, need to add yr and Gyr options.
      */
  }
}
/** Write Constants. */
void WriteTrefLind(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
    *dTmp = body[iBody].dTrefLind;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else { }
}



void InitializeOutputThermint(OUTPUT *output,fnWriteOutput fnWrite[]) {

  sprintf(output[OUT_TMAN].cName,"TMan");
  sprintf(output[OUT_TMAN].cDescr,"Mantle Temperature");
  sprintf(output[OUT_TMAN].cNeg,"K");
  output[OUT_TMAN].bNeg = 1;
  output[OUT_TMAN].dNeg = 1; // default units are K. 
  output[OUT_TMAN].iNum = 1;
  output[OUT_TMAN].iModuleBit = THERMINT;
  fnWrite[OUT_TMAN] = &WriteTMan;

  sprintf(output[OUT_TUMAN].cName,"TUMan");
  sprintf(output[OUT_TUMAN].cDescr,"Upper Mantle Temperature");
  sprintf(output[OUT_TUMAN].cNeg,"K");
  output[OUT_TUMAN].bNeg = 1;
  output[OUT_TUMAN].dNeg = 1; // default units are K. 
  output[OUT_TUMAN].iNum = 1;
  output[OUT_TUMAN].iModuleBit = THERMINT;
  fnWrite[OUT_TUMAN] = &WriteTUMan;

  sprintf(output[OUT_TLMAN].cName,"TLMan");
  sprintf(output[OUT_TLMAN].cDescr,"Lower Mantle Temperature");
  sprintf(output[OUT_TLMAN].cNeg,"K");
  output[OUT_TLMAN].bNeg = 1;
  output[OUT_TLMAN].dNeg = 1; // default units are K. 
  output[OUT_TLMAN].iNum = 1;
  output[OUT_TLMAN].iModuleBit = THERMINT;
  fnWrite[OUT_TLMAN] = &WriteTLMan;

  sprintf(output[OUT_TJUMPUMAN].cName,"TJumpUMan");
  sprintf(output[OUT_TJUMPUMAN].cDescr,"Upper Mantle Temperature Jump");
  sprintf(output[OUT_TJUMPUMAN].cNeg,"K");
  output[OUT_TJUMPUMAN].bNeg = 1;
  output[OUT_TJUMPUMAN].dNeg = 1; // default units are K. 
  output[OUT_TJUMPUMAN].iNum = 1;
  output[OUT_TJUMPUMAN].iModuleBit = THERMINT;
  fnWrite[OUT_TJUMPUMAN] = &WriteTJumpUMan;

  sprintf(output[OUT_TJUMPLMAN].cName,"TJumpLMan");
  sprintf(output[OUT_TJUMPLMAN].cDescr,"Lower Mantle Temperature Jump");
  sprintf(output[OUT_TJUMPLMAN].cNeg,"K");
  output[OUT_TJUMPLMAN].bNeg = 1;
  output[OUT_TJUMPLMAN].dNeg = 1; // default units are K. 
  output[OUT_TJUMPLMAN].iNum = 1;
  output[OUT_TJUMPLMAN].iModuleBit = THERMINT;
  fnWrite[OUT_TJUMPLMAN] = &WriteTJumpLMan;

  sprintf(output[OUT_SIGNTJUMPUMAN].cName,"SignTJumpUMan");
  sprintf(output[OUT_SIGNTJUMPUMAN].cDescr,"Sign of Upper Mantle Temperature Jump");
  sprintf(output[OUT_SIGNTJUMPUMAN].cNeg,"K");
  output[OUT_SIGNTJUMPUMAN].bNeg = 1;
  output[OUT_SIGNTJUMPUMAN].dNeg = 1; // default units are K. 
  output[OUT_SIGNTJUMPUMAN].iNum = 1;
  output[OUT_SIGNTJUMPUMAN].iModuleBit = THERMINT;
  fnWrite[OUT_SIGNTJUMPUMAN] = &WriteSignTJumpUMan;

  sprintf(output[OUT_SIGNTJUMPLMAN].cName,"SignTJumpLMan");
  sprintf(output[OUT_SIGNTJUMPLMAN].cDescr,"Sign of Lower Mantle Temperature Jump");
  sprintf(output[OUT_SIGNTJUMPLMAN].cNeg,"K");
  output[OUT_SIGNTJUMPLMAN].bNeg = 1;
  output[OUT_SIGNTJUMPLMAN].dNeg = 1; // default units are K. 
  output[OUT_SIGNTJUMPLMAN].iNum = 1;
  output[OUT_SIGNTJUMPLMAN].iModuleBit = THERMINT;
  fnWrite[OUT_SIGNTJUMPLMAN] = &WriteSignTJumpLMan;

  sprintf(output[OUT_TCMB].cName,"TCMB");
  sprintf(output[OUT_TCMB].cDescr,"CMB Temperature");
  sprintf(output[OUT_TCMB].cNeg,"K");
  output[OUT_TCMB].bNeg = 1;
  output[OUT_TCMB].dNeg = 1; // default units are K. 
  output[OUT_TCMB].iNum = 1;
  output[OUT_TCMB].iModuleBit = THERMINT;
  fnWrite[OUT_TCMB] = &WriteTCMB;

  sprintf(output[OUT_TCORE].cName,"TCore");
  sprintf(output[OUT_TCORE].cDescr,"Core Temperature");
  sprintf(output[OUT_TCORE].cNeg,"K");
  output[OUT_TCORE].bNeg = 1;
  output[OUT_TCORE].dNeg = 1; // default units are K. 
  output[OUT_TCORE].iNum = 1;
  output[OUT_TCORE].iModuleBit = THERMINT;
  fnWrite[OUT_TCORE] = &WriteTCore;

  sprintf(output[OUT_VISCUMAN].cName,"ViscUMan");
  sprintf(output[OUT_VISCUMAN].cDescr,"Upper Mantle Viscosity");
  sprintf(output[OUT_VISCUMAN].cNeg,"m^2/s");
  output[OUT_VISCUMAN].bNeg = 1;
  output[OUT_VISCUMAN].dNeg = 1; 
  output[OUT_VISCUMAN].iNum = 1;
  output[OUT_VISCUMAN].iModuleBit = THERMINT;
  fnWrite[OUT_VISCUMAN] = &WriteViscUMan;
  /* ViscUManArr Arrhenius Only */
  sprintf(output[OUT_VISCUMANARR].cName,"ViscUManArr");
  sprintf(output[OUT_VISCUMANARR].cDescr,"Upper Mantle Arrhenius Viscosity");
  sprintf(output[OUT_VISCUMANARR].cNeg,"m^2/s");
  output[OUT_VISCUMANARR].bNeg = 1;
  output[OUT_VISCUMANARR].dNeg = 1; 
  output[OUT_VISCUMANARR].iNum = 1;
  output[OUT_VISCUMANARR].iModuleBit = THERMINT;
  fnWrite[OUT_VISCUMANARR] = &WriteViscUManArr;
  /* ViscLMan */
  sprintf(output[OUT_VISCLMAN].cName,"ViscLMan");
  sprintf(output[OUT_VISCLMAN].cDescr,"Lower Mantle Viscosity");
  sprintf(output[OUT_VISCLMAN].cNeg,"m^2/s");
  output[OUT_VISCLMAN].bNeg = 1;
  output[OUT_VISCLMAN].dNeg = 1; 
  output[OUT_VISCLMAN].iNum = 1;
  output[OUT_VISCLMAN].iModuleBit = THERMINT;
  fnWrite[OUT_VISCLMAN] = &WriteViscLMan;
  /* ViscMMan */
  sprintf(output[OUT_VISCMMAN].cName,"ViscMMan");
  sprintf(output[OUT_VISCMMAN].cDescr,"Average (mid) Mantle Viscosity");
  sprintf(output[OUT_VISCMMAN].cNeg,"m^2/s");
  output[OUT_VISCMMAN].bNeg = 1;
  output[OUT_VISCMMAN].dNeg = 1; 
  output[OUT_VISCMMAN].iNum = 1;
  output[OUT_VISCMMAN].iModuleBit = THERMINT;
  fnWrite[OUT_VISCMMAN] = &WriteViscMMan;

  sprintf(output[OUT_VISCJUMPMAN].cName,"ViscJumpMan");
  sprintf(output[OUT_VISCJUMPMAN].cDescr,"Mantle Viscosity Ratio");
  sprintf(output[OUT_VISCJUMPMAN].cNeg,"nd");
  output[OUT_VISCJUMPMAN].bNeg = 1;
  output[OUT_VISCJUMPMAN].dNeg = 1; 
  output[OUT_VISCJUMPMAN].iNum = 1;
  output[OUT_VISCJUMPMAN].iModuleBit = THERMINT;
  fnWrite[OUT_VISCJUMPMAN] = &WriteViscJumpMan;

  sprintf(output[OUT_BLUMAN].cName,"BLUMan");
  sprintf(output[OUT_BLUMAN].cDescr,"Boundary Layer Thickness Upper Mantle");
  sprintf(output[OUT_BLUMAN].cNeg,"km");
  output[OUT_BLUMAN].bNeg = 1;
  output[OUT_BLUMAN].dNeg = 1e-3;  //KM; 
  output[OUT_BLUMAN].iNum = 1;
  output[OUT_BLUMAN].iModuleBit = THERMINT;
  fnWrite[OUT_BLUMAN] = &WriteBLUMan;

  /* BLLMan */
  sprintf(output[OUT_BLLMAN].cName,"BLLMan");
  sprintf(output[OUT_BLLMAN].cDescr,"Boundary Layer Thickness Lower Mantle");
  sprintf(output[OUT_BLLMAN].cNeg,"km");
  output[OUT_BLLMAN].bNeg = 1;
  output[OUT_BLLMAN].dNeg = 1e-3; 
  output[OUT_BLLMAN].iNum = 1;
  output[OUT_BLLMAN].iModuleBit = THERMINT;
  fnWrite[OUT_BLLMAN] = &WriteBLLMan;
  /* ShmodUMan */
  sprintf(output[OUT_SHMODUMAN].cName,"ShmodUMan");
  sprintf(output[OUT_SHMODUMAN].cDescr,"Shear Modulus Upper Mantle");
  sprintf(output[OUT_SHMODUMAN].cNeg,"Pa");
  output[OUT_SHMODUMAN].bNeg = 1;
  output[OUT_SHMODUMAN].dNeg = 1; 
  output[OUT_SHMODUMAN].iNum = 1;
  output[OUT_SHMODUMAN].iModuleBit = THERMINT;
  fnWrite[OUT_SHMODUMAN] = &WriteShmodUMan;
  /* FMeltUMan */
  sprintf(output[OUT_FMELTUMAN].cName,"FMeltUMan");
  sprintf(output[OUT_FMELTUMAN].cDescr,"Melt Fraction Upper Mantle");
  sprintf(output[OUT_FMELTUMAN].cNeg,"nd");
  output[OUT_FMELTUMAN].bNeg = 1;
  output[OUT_FMELTUMAN].dNeg = 1; 
  output[OUT_FMELTUMAN].iNum = 1;
  output[OUT_FMELTUMAN].iModuleBit = THERMINT;
  fnWrite[OUT_FMELTUMAN] = &WriteFMeltUMan;
  /* FMeltLMan */
  sprintf(output[OUT_FMELTLMAN].cName,"FMeltLMan");
  sprintf(output[OUT_FMELTLMAN].cDescr,"Melt Fraction Lower Mantle");
  sprintf(output[OUT_FMELTLMAN].cNeg,"nd");
  output[OUT_FMELTLMAN].bNeg = 1;
  output[OUT_FMELTLMAN].dNeg = 1; 
  output[OUT_FMELTLMAN].iNum = 1;
  output[OUT_FMELTLMAN].iModuleBit = THERMINT;
  fnWrite[OUT_FMELTLMAN] = &WriteFMeltLMan;
  /* MeltfactorUMan */
  sprintf(output[OUT_MELTFACTORUMAN].cName,"MeltfactorUMan");
  sprintf(output[OUT_MELTFACTORUMAN].cDescr,"Melt Factor Upper Mantle");
  sprintf(output[OUT_MELTFACTORUMAN].cNeg,"nd");
  output[OUT_MELTFACTORUMAN].bNeg = 1;
  output[OUT_MELTFACTORUMAN].dNeg = 1; 
  output[OUT_MELTFACTORUMAN].iNum = 1;
  output[OUT_MELTFACTORUMAN].iModuleBit = THERMINT;
  fnWrite[OUT_MELTFACTORUMAN] = &WriteMeltfactorUMan;
  /* MeltfactorLMan */
  sprintf(output[OUT_MELTFACTORLMAN].cName,"MeltfactorLMan");
  sprintf(output[OUT_MELTFACTORLMAN].cDescr,"Melt Factor Lower Mantle");
  sprintf(output[OUT_MELTFACTORLMAN].cNeg,"nd");
  output[OUT_MELTFACTORLMAN].bNeg = 1;
  output[OUT_MELTFACTORLMAN].dNeg = 1; 
  output[OUT_MELTFACTORLMAN].iNum = 1;
  output[OUT_MELTFACTORLMAN].iModuleBit = THERMINT;
  fnWrite[OUT_MELTFACTORLMAN] = &WriteMeltfactorLMan;
  /* DepthMeltMan */
  sprintf(output[OUT_DEPTHMELTMAN].cName,"DepthMeltMan");
  sprintf(output[OUT_DEPTHMELTMAN].cDescr,"Depth to base of UM Melt Region");
  sprintf(output[OUT_DEPTHMELTMAN].cNeg,"m");
  output[OUT_DEPTHMELTMAN].bNeg = 1;
  output[OUT_DEPTHMELTMAN].dNeg = 1; 
  output[OUT_DEPTHMELTMAN].iNum = 1;
  output[OUT_DEPTHMELTMAN].iModuleBit = THERMINT;
  fnWrite[OUT_DEPTHMELTMAN] = &WriteDepthMeltMan;
  /* TDepthMeltMan */
  sprintf(output[OUT_TDEPTHMELTMAN].cName,"TDepthMeltMan");
  sprintf(output[OUT_TDEPTHMELTMAN].cDescr,"Temp at base of UM Melt Region");
  sprintf(output[OUT_TDEPTHMELTMAN].cNeg,"K");
  output[OUT_TDEPTHMELTMAN].bNeg = 1;
  output[OUT_TDEPTHMELTMAN].dNeg = 1; 
  output[OUT_TDEPTHMELTMAN].iNum = 1;
  output[OUT_TDEPTHMELTMAN].iModuleBit = THERMINT;
  fnWrite[OUT_TDEPTHMELTMAN] = &WriteTDepthMeltMan;
  /* TJumpMeltMan */
  sprintf(output[OUT_TJUMPMELTMAN].cName,"TJumpMeltMan");
  sprintf(output[OUT_TJUMPMELTMAN].cDescr,"Temp Jump across UM Melt Region");
  sprintf(output[OUT_TJUMPMELTMAN].cNeg,"K");
  output[OUT_TJUMPMELTMAN].bNeg = 1;
  output[OUT_TJUMPMELTMAN].dNeg = 1; 
  output[OUT_TJUMPMELTMAN].iNum = 1;
  output[OUT_TJUMPMELTMAN].iModuleBit = THERMINT;
  fnWrite[OUT_TJUMPMELTMAN] = &WriteTJumpMeltMan;
  /* MeltMassFluxMan */
  sprintf(output[OUT_MELTMASSFLUXMAN].cName,"MeltMassFluxMan");
  sprintf(output[OUT_MELTMASSFLUXMAN].cDescr,"Mantle Melt Mass Flux");
  sprintf(output[OUT_MELTMASSFLUXMAN].cNeg,"kg/s");
  output[OUT_MELTMASSFLUXMAN].bNeg = 1;
  output[OUT_MELTMASSFLUXMAN].dNeg = 1; 
  output[OUT_MELTMASSFLUXMAN].iNum = 1;
  output[OUT_MELTMASSFLUXMAN].iModuleBit = THERMINT;
  fnWrite[OUT_MELTMASSFLUXMAN] = &WriteMeltMassFluxMan;
  
  /* EruptEff */
  sprintf(output[OUT_ERUPTEFF].cName,"EruptEff");
  sprintf(output[OUT_ERUPTEFF].cDescr,"Mantle Melt Eruption Efficiency");
  sprintf(output[OUT_ERUPTEFF].cNeg,"nd");
  output[OUT_ERUPTEFF].bNeg = 1;
  output[OUT_ERUPTEFF].dNeg = 1; 
  output[OUT_ERUPTEFF].iNum = 1;
  output[OUT_ERUPTEFF].iModuleBit = THERMINT;
  fnWrite[OUT_ERUPTEFF] = &WriteEruptEff;
  /* RayleighMan */
  sprintf(output[OUT_RAYLEIGHMAN].cName,"RayleighMan");
  sprintf(output[OUT_RAYLEIGHMAN].cDescr,"Mantle Rayleigh Number");
  sprintf(output[OUT_RAYLEIGHMAN].cNeg,"nd");
  output[OUT_RAYLEIGHMAN].bNeg = 1;
  output[OUT_RAYLEIGHMAN].dNeg = 1; 
  output[OUT_RAYLEIGHMAN].iNum = 1;
  output[OUT_RAYLEIGHMAN].iModuleBit = THERMINT;
  fnWrite[OUT_RAYLEIGHMAN] = &WriteRayleighMan;

  
  /* K2Man */
  sprintf(output[OUT_K2MAN].cName,"K2Man");
  sprintf(output[OUT_K2MAN].cDescr,"Real Love Number k2 Mantle");
  sprintf(output[OUT_K2MAN].cNeg,"nd");
  output[OUT_K2MAN].bNeg = 1;
  output[OUT_K2MAN].dNeg = 1; 
  output[OUT_K2MAN].iNum = 1;
  output[OUT_K2MAN].iModuleBit = THERMINT;
  fnWrite[OUT_K2MAN] = &WriteK2Man;
  /* Imk2Man */
  sprintf(output[OUT_IMK2MAN].cName,"Imk2Man");
  sprintf(output[OUT_IMK2MAN].cDescr,"Imaginary Love Number k2 Mantle");
  sprintf(output[OUT_IMK2MAN].cNeg,"nd");
  output[OUT_IMK2MAN].bNeg = 1;
  output[OUT_IMK2MAN].dNeg = 1; 
  output[OUT_IMK2MAN].iNum = 1;
  output[OUT_IMK2MAN].iModuleBit = THERMINT;
  fnWrite[OUT_IMK2MAN] = &WriteImk2Man;

  /*  CORE WRITES */
  /* RIC */
  sprintf(output[OUT_RIC].cName,"RIC");
  sprintf(output[OUT_RIC].cDescr,"Inner Core Radius");
  sprintf(output[OUT_RIC].cNeg,"km");
  output[OUT_RIC].bNeg = 1;
  output[OUT_RIC].dNeg = 1e-3; 
  output[OUT_RIC].iNum = 1;
  output[OUT_RIC].iModuleBit = THERMINT;
  fnWrite[OUT_RIC] = &WriteRIC;
  /* DRICDTCMB */
  sprintf(output[OUT_DRICDTCMB].cName,"DRICDTCMB");
  sprintf(output[OUT_DRICDTCMB].cDescr,"d(R_ic)/d(T_cmb)");
  sprintf(output[OUT_DRICDTCMB].cNeg,"m/K");
  output[OUT_DRICDTCMB].bNeg = 1;
  output[OUT_DRICDTCMB].dNeg = 1; 
  output[OUT_DRICDTCMB].iNum = 1;
  output[OUT_DRICDTCMB].iModuleBit = THERMINT;
  fnWrite[OUT_DRICDTCMB] = &WriteDRICDTCMB;
  /* ChiOC */
  sprintf(output[OUT_CHIOC].cName,"ChiOC");
  sprintf(output[OUT_CHIOC].cDescr,"Light Element Concentration in Outer Core");
  sprintf(output[OUT_CHIOC].cNeg,"nd");
  output[OUT_CHIOC].bNeg = 1;
  output[OUT_CHIOC].dNeg = 1; 
  output[OUT_CHIOC].iNum = 1;
  output[OUT_CHIOC].iModuleBit = THERMINT;
  fnWrite[OUT_CHIOC] = &WriteChiOC;
  /* ChiIC */
  sprintf(output[OUT_CHIIC].cName,"ChiIC");
  sprintf(output[OUT_CHIIC].cDescr,"Light Element Concentration in Inner Core");
  sprintf(output[OUT_CHIIC].cNeg,"nd");
  output[OUT_CHIIC].bNeg = 1;
  output[OUT_CHIIC].dNeg = 1; 
  output[OUT_CHIIC].iNum = 1;
  output[OUT_CHIIC].iModuleBit = THERMINT;
  fnWrite[OUT_CHIIC] = &WriteChiIC;
  /* MassOC */
  sprintf(output[OUT_MASSOC].cName,"MassOC");
  sprintf(output[OUT_MASSOC].cDescr,"Mass of Outer Core");
  sprintf(output[OUT_MASSOC].cNeg,"EMASSOC");
  output[OUT_MASSOC].bNeg = 1;
  output[OUT_MASSOC].dNeg = 1/(EMASSOC); 
  output[OUT_MASSOC].iNum = 1;
  output[OUT_MASSOC].iModuleBit = THERMINT;
  fnWrite[OUT_MASSOC] = &WriteMassOC;
  /* MassIC */
  sprintf(output[OUT_MASSIC].cName,"MassIC");
  sprintf(output[OUT_MASSIC].cDescr,"Mass of Inner Core");
  sprintf(output[OUT_MASSIC].cNeg,"EMASSIC");
  output[OUT_MASSIC].bNeg = 1;
  output[OUT_MASSIC].dNeg = 1/(EMASSIC); 
  output[OUT_MASSIC].iNum = 1;
  output[OUT_MASSIC].iModuleBit = THERMINT;
  fnWrite[OUT_MASSIC] = &WriteMassIC;
  /* MassChiOC */
  sprintf(output[OUT_MASSCHIOC].cName,"MassChiOC");
  sprintf(output[OUT_MASSCHIOC].cDescr,"Mass of Chi in Outer Core");
  sprintf(output[OUT_MASSCHIOC].cNeg,"EMASSOC_CHI");
  output[OUT_MASSCHIOC].bNeg = 1;
  output[OUT_MASSCHIOC].dNeg = 1/(EMASSOC_CHI); 
  output[OUT_MASSCHIOC].iNum = 1;
  output[OUT_MASSCHIOC].iModuleBit = THERMINT;
  fnWrite[OUT_MASSCHIOC] = &WriteMassChiOC;
  /* MassChiIC */
  sprintf(output[OUT_MASSCHIIC].cName,"MassChiIC");
  sprintf(output[OUT_MASSCHIIC].cDescr,"Mass of Chi in Inner Core");
  sprintf(output[OUT_MASSCHIIC].cNeg,"EMASSIC_CHI");
  output[OUT_MASSCHIIC].bNeg = 1;
  output[OUT_MASSCHIIC].dNeg = 1/(EMASSIC_CHI); 
  output[OUT_MASSCHIIC].iNum = 1;
  output[OUT_MASSCHIIC].iModuleBit = THERMINT;
  fnWrite[OUT_MASSCHIIC] = &WriteMassChiIC;
  /* DTChi */
  sprintf(output[OUT_DTCHI].cName,"DTChi");
  sprintf(output[OUT_DTCHI].cDescr,"Core Liquidus Depression");
  sprintf(output[OUT_DTCHI].cNeg,"K");
  output[OUT_DTCHI].bNeg = 1;
  output[OUT_DTCHI].dNeg = 1; 
  output[OUT_DTCHI].iNum = 1;
  output[OUT_DTCHI].iModuleBit = THERMINT;
  fnWrite[OUT_DTCHI] = &WriteDTChi;
  /* CoreBuoyTherm */
  sprintf(output[OUT_COREBUOYTHERM].cName,"CoreBuoyTherm");
  sprintf(output[OUT_COREBUOYTHERM].cDescr,"Core Thermal Buoyancy Flux");
  sprintf(output[OUT_COREBUOYTHERM].cNeg,"m^2/s^3");
  output[OUT_COREBUOYTHERM].bNeg = 1;
  output[OUT_COREBUOYTHERM].dNeg = 1; 
  output[OUT_COREBUOYTHERM].iNum = 1;
  output[OUT_COREBUOYTHERM].iModuleBit = THERMINT;
  fnWrite[OUT_COREBUOYTHERM] = &WriteCoreBuoyTherm;
  /* CoreBuoyCompo */
  sprintf(output[OUT_COREBUOYCOMPO].cName,"CoreBuoyCompo");
  sprintf(output[OUT_COREBUOYCOMPO].cDescr,"Core Compositional Buoyancy Flux");
  sprintf(output[OUT_COREBUOYCOMPO].cNeg,"m^2/s^3");
  output[OUT_COREBUOYCOMPO].bNeg = 1;
  output[OUT_COREBUOYCOMPO].dNeg = 1; 
  output[OUT_COREBUOYCOMPO].iNum = 1;
  output[OUT_COREBUOYCOMPO].iModuleBit = THERMINT;
  fnWrite[OUT_COREBUOYCOMPO] = &WriteCoreBuoyCompo;
  /* CoreBuoyTotal */
  sprintf(output[OUT_COREBUOYTOTAL].cName,"CoreBuoyTotal");
  sprintf(output[OUT_COREBUOYTOTAL].cDescr,"Core Total Buoyancy Flux");
  sprintf(output[OUT_COREBUOYTOTAL].cNeg,"m^2/s^3");
  output[OUT_COREBUOYTOTAL].bNeg = 1;
  output[OUT_COREBUOYTOTAL].dNeg = 1; 
  output[OUT_COREBUOYTOTAL].iNum = 1;
  output[OUT_COREBUOYTOTAL].iModuleBit = THERMINT;
  fnWrite[OUT_COREBUOYTOTAL] = &WriteCoreBuoyTotal;
  /* GravICB */
  sprintf(output[OUT_GRAVICB].cName,"GravICB");
  sprintf(output[OUT_GRAVICB].cDescr,"ICB Gravity");
  sprintf(output[OUT_GRAVICB].cNeg,"m/s^2");
  output[OUT_GRAVICB].bNeg = 1;
  output[OUT_GRAVICB].dNeg = 1; 
  output[OUT_GRAVICB].iNum = 1;
  output[OUT_GRAVICB].iModuleBit = THERMINT;
  fnWrite[OUT_GRAVICB] = &WriteGravICB;
  /* MagMom */
  sprintf(output[OUT_MAGMOM].cName,"MagMom");
  sprintf(output[OUT_MAGMOM].cDescr,"Core Magnetic Moment");
  sprintf(output[OUT_MAGMOM].cNeg,"EMAGMOM");
  output[OUT_MAGMOM].bNeg = 1;
  output[OUT_MAGMOM].dNeg = 1./(EMAGMOM); 
  output[OUT_MAGMOM].iNum = 1;
  output[OUT_MAGMOM].iModuleBit = THERMINT;
  fnWrite[OUT_MAGMOM] = &WriteMagMom;
  /* RICDot */
  sprintf(output[OUT_RICDOT].cName,"RICDot");
  sprintf(output[OUT_RICDOT].cDescr,"Inner Core growth rate");
  sprintf(output[OUT_RICDOT].cNeg,"m/s");
  output[OUT_RICDOT].bNeg = 1;
  output[OUT_RICDOT].dNeg = 1; 
  output[OUT_RICDOT].iNum = 1;
  output[OUT_RICDOT].iModuleBit = THERMINT;
  fnWrite[OUT_RICDOT] = &WriteRICDot;
  
  /* Heat Fluxes/Flows */
  /* HFluxUMan */
  sprintf(output[OUT_HFLUXUMAN].cName,"HfluxUMan");
  sprintf(output[OUT_HFLUXUMAN].cDescr,"Heat Flux Upper Mantle");
  sprintf(output[OUT_HFLUXUMAN].cNeg,"W/m^2");
  output[OUT_HFLUXUMAN].bNeg = 1;
  output[OUT_HFLUXUMAN].dNeg = 1;  
  output[OUT_HFLUXUMAN].iNum = 1;
  output[OUT_HFLUXUMAN].iModuleBit = THERMINT;
  fnWrite[OUT_HFLUXUMAN] = &WriteHfluxUMan;
  /* HFluxLMan */
  sprintf(output[OUT_HFLUXLMAN].cName,"HfluxLMan");
  sprintf(output[OUT_HFLUXLMAN].cDescr,"Heat Flux Lower Mantle");
  sprintf(output[OUT_HFLUXLMAN].cNeg,"W/m^2");
  output[OUT_HFLUXLMAN].bNeg = 1;
  output[OUT_HFLUXLMAN].dNeg = 1;
  output[OUT_HFLUXLMAN].iNum = 1;
  output[OUT_HFLUXLMAN].iModuleBit = THERMINT;
  fnWrite[OUT_HFLUXLMAN] = &WriteHfluxLMan;
  /* HFluxCMB */
  sprintf(output[OUT_HFLUXCMB].cName,"HfluxCMB");
  sprintf(output[OUT_HFLUXCMB].cDescr,"Heat Flux Core-Mantle Boundary");
  sprintf(output[OUT_HFLUXCMB].cNeg,"W/m^2");
  output[OUT_HFLUXCMB].bNeg = 1;
  output[OUT_HFLUXCMB].dNeg = 1;
  output[OUT_HFLUXCMB].iNum = 1;
  output[OUT_HFLUXCMB].iModuleBit = THERMINT;
  fnWrite[OUT_HFLUXCMB] = &WriteHfluxCMB;
  /* HfluxCMBAd */
  sprintf(output[OUT_HFLUXCMBAD].cName,"HfluxCMBAd");
  sprintf(output[OUT_HFLUXCMBAD].cDescr,"Adiabatic Heat Flux Core-Mantle Boundary");
  sprintf(output[OUT_HFLUXCMBAD].cNeg,"W/m^2");
  output[OUT_HFLUXCMBAD].bNeg = 1;
  output[OUT_HFLUXCMBAD].dNeg = 1;
  output[OUT_HFLUXCMBAD].iNum = 1;
  output[OUT_HFLUXCMBAD].iModuleBit = THERMINT;
  fnWrite[OUT_HFLUXCMBAD] = &WriteHfluxCMBAd;
  /* HfluxCMBConv */
  sprintf(output[OUT_HFLUXCMBCONV].cName,"HfluxCMBConv");
  sprintf(output[OUT_HFLUXCMBCONV].cDescr,"Super-Adiabatic (convective) Heat Flux Core-Mantle Boundary");
  sprintf(output[OUT_HFLUXCMBCONV].cNeg,"W/m^2");
  output[OUT_HFLUXCMBCONV].bNeg = 1;
  output[OUT_HFLUXCMBCONV].dNeg = 1;
  output[OUT_HFLUXCMBCONV].iNum = 1;
  output[OUT_HFLUXCMBCONV].iModuleBit = THERMINT;
  fnWrite[OUT_HFLUXCMBCONV] = &WriteHfluxCMBConv;
  /* ThermConductOC */
  sprintf(output[OUT_THERMCONDUCTOC].cName,"ThermConductOC");
  sprintf(output[OUT_THERMCONDUCTOC].cDescr,"Thermal Conductivity OC");
  sprintf(output[OUT_THERMCONDUCTOC].cNeg,"W/m/K");
  output[OUT_THERMCONDUCTOC].bNeg = 1;
  output[OUT_THERMCONDUCTOC].dNeg = 1;
  output[OUT_THERMCONDUCTOC].iNum = 1;
  output[OUT_THERMCONDUCTOC].iModuleBit = THERMINT;
  fnWrite[OUT_THERMCONDUCTOC] = &WriteThermConductOC;
  /* HflowUMan */
  sprintf(output[OUT_HFLOWUMAN].cName,"HflowUMan");
  sprintf(output[OUT_HFLOWUMAN].cDescr,"Heat Flow Upper Mantle");
  sprintf(output[OUT_HFLOWUMAN].cNeg,"TW");
  output[OUT_HFLOWUMAN].bNeg = 1;
  output[OUT_HFLOWUMAN].dNeg = 1e-12;
  output[OUT_HFLOWUMAN].iNum = 1;
  output[OUT_HFLOWUMAN].iModuleBit = THERMINT;
  fnWrite[OUT_HFLOWUMAN] = &WriteHflowUMan;
  /* HFlowLMan */
  sprintf(output[OUT_HFLOWLMAN].cName,"HflowLMan");
  sprintf(output[OUT_HFLOWLMAN].cDescr,"Heat Flow Lower Mantle");
  sprintf(output[OUT_HFLOWLMAN].cNeg,"TW");
  output[OUT_HFLOWLMAN].bNeg = 1;
  output[OUT_HFLOWLMAN].dNeg = 1e-12;
  output[OUT_HFLOWLMAN].iNum = 1;
  output[OUT_HFLOWLMAN].iModuleBit = THERMINT;
  fnWrite[OUT_HFLOWLMAN] = &WriteHflowLMan;
  /* HFlowCMB */
  sprintf(output[OUT_HFLOWCMB].cName,"HflowCMB");
  sprintf(output[OUT_HFLOWCMB].cDescr,"Heat Flow Core-Mantle Boundary");
  sprintf(output[OUT_HFLOWCMB].cNeg,"TW");
  output[OUT_HFLOWCMB].bNeg = 1;
  output[OUT_HFLOWCMB].dNeg = 1e-12;
  output[OUT_HFLOWCMB].iNum = 1;
  output[OUT_HFLOWCMB].iModuleBit = THERMINT;
  fnWrite[OUT_HFLOWCMB] = &WriteHflowCMB;
  /* HflowLatentMan */
  sprintf(output[OUT_HFLOWLATENTMAN].cName,"HflowLatentMan");
  sprintf(output[OUT_HFLOWLATENTMAN].cDescr,"Latent Heat Flow Mantle");
  sprintf(output[OUT_HFLOWLATENTMAN].cNeg,"TW");
  output[OUT_HFLOWLATENTMAN].bNeg = 1;
  output[OUT_HFLOWLATENTMAN].dNeg = 1e-12;
  output[OUT_HFLOWLATENTMAN].iNum = 1;
  output[OUT_HFLOWLATENTMAN].iModuleBit = THERMINT;
  fnWrite[OUT_HFLOWLATENTMAN] = &WriteHflowLatentMan;
  /* HflowMeltMan */
  sprintf(output[OUT_HFLOWMELTMAN].cName,"HflowMeltMan");
  sprintf(output[OUT_HFLOWMELTMAN].cDescr,"Melt Heat Flow Mantle");
  sprintf(output[OUT_HFLOWMELTMAN].cNeg,"TW");
  output[OUT_HFLOWMELTMAN].bNeg = 1;
  output[OUT_HFLOWMELTMAN].dNeg = 1e-12;
  output[OUT_HFLOWMELTMAN].iNum = 1;
  output[OUT_HFLOWMELTMAN].iModuleBit = THERMINT;
  fnWrite[OUT_HFLOWMELTMAN] = &WriteHflowMeltMan;
  /* TidalPowMan
  sprintf(output[OUT_TIDALPOWMAN].cName,"TidalPowMan");
  sprintf(output[OUT_TIDALPOWMAN].cDescr,"Tidal Power Mantle");
  sprintf(output[OUT_TIDALPOWMAN].cNeg,"TW");
  output[OUT_TIDALPOWMAN].bNeg = 1;
  output[OUT_TIDALPOWMAN].dNeg = 1e-12;
  output[OUT_TIDALPOWMAN].iNum = 1;
  output[OUT_TIDALPOWMAN].iModuleBit = THERMINT;
  fnWrite[OUT_TIDALPOWMAN] = &WriteTidalPowMan; */
  /* HFlowSecMan */
  sprintf(output[OUT_HFLOWSECMAN].cName,"HflowSecMan");
  sprintf(output[OUT_HFLOWSECMAN].cDescr,"Mantle Secular Heat Flow");
  sprintf(output[OUT_HFLOWSECMAN].cNeg,"TW");
  output[OUT_HFLOWSECMAN].bNeg = 1;
  output[OUT_HFLOWSECMAN].dNeg = 1e-12;
  output[OUT_HFLOWSECMAN].iNum = 1;
  output[OUT_HFLOWSECMAN].iModuleBit = THERMINT;
  fnWrite[OUT_HFLOWSECMAN] = &WriteHflowSecMan;
  /* HFlowSurf */
  sprintf(output[OUT_HFLOWSURF].cName,"HflowSurf");
  sprintf(output[OUT_HFLOWSURF].cDescr,"Heat Flow Surface Total");
  sprintf(output[OUT_HFLOWSURF].cNeg,"TW");
  output[OUT_HFLOWSURF].bNeg = 1;
  output[OUT_HFLOWSURF].dNeg = 1e-12;
  output[OUT_HFLOWSURF].iNum = 1;
  output[OUT_HFLOWSURF].iModuleBit = THERMINT;
  fnWrite[OUT_HFLOWSURF] = &WriteHflowSurf;

  
  /* HFlowLatentIC */
  sprintf(output[OUT_HFLOWLATENTIC].cName,"HflowLatentIC");
  sprintf(output[OUT_HFLOWLATENTIC].cDescr,"Latent Heat Release at ICB");
  sprintf(output[OUT_HFLOWLATENTIC].cNeg,"TW");
  output[OUT_HFLOWLATENTIC].bNeg = 1;
  output[OUT_HFLOWLATENTIC].dNeg = 1e-12;
  output[OUT_HFLOWLATENTIC].iNum = 1;
  output[OUT_HFLOWLATENTIC].iModuleBit = THERMINT;
  fnWrite[OUT_HFLOWLATENTIC] = &WriteHflowLatentIC;
  /* PowerGravIC */
  sprintf(output[OUT_POWERGRAVIC].cName,"PowerGravIC");
  sprintf(output[OUT_POWERGRAVIC].cDescr,"Gravitational Power Release at ICB");
  sprintf(output[OUT_POWERGRAVIC].cNeg,"TW");
  output[OUT_POWERGRAVIC].bNeg = 1;
  output[OUT_POWERGRAVIC].dNeg = 1e-12;
  output[OUT_POWERGRAVIC].iNum = 1;
  output[OUT_POWERGRAVIC].iModuleBit = THERMINT;
  fnWrite[OUT_POWERGRAVIC] = &WritePowerGravIC;

  /* TDotMan */
  sprintf(output[OUT_TDOTMAN].cName,"TDotMan");
  sprintf(output[OUT_TDOTMAN].cDescr,"Change in Mantle Temperature");
  sprintf(output[OUT_TDOTMAN].cNeg,"K/s");
  output[OUT_TDOTMAN].bNeg = 1;
  output[OUT_TDOTMAN].dNeg = 1; // default units are K. 
  output[OUT_TDOTMAN].iNum = 1;
  output[OUT_TDOTMAN].iModuleBit = THERMINT;
  fnWrite[OUT_TDOTMAN] = &WriteTDotMan;
  /* TDotCore */
  sprintf(output[OUT_TDOTCORE].cName,"TDotCore");
  sprintf(output[OUT_TDOTCORE].cDescr,"Change in Core Temperature");
  sprintf(output[OUT_TDOTCORE].cNeg,"K/s");
  output[OUT_TDOTCORE].bNeg = 1;
  output[OUT_TDOTCORE].dNeg = 1; // default units are K. 
  output[OUT_TDOTCORE].iNum = 1;
  output[OUT_TDOTCORE].iModuleBit = THERMINT;
  fnWrite[OUT_TDOTCORE] = &WriteTDotCore;
  /* Constants */
  /* TrefLind */
  sprintf(output[OUT_TREFLIND].cName,"TrefLind");
  sprintf(output[OUT_TREFLIND].cDescr,"Core Liquidus Lindeman Reference");
  sprintf(output[OUT_TREFLIND].cNeg,"K");
  output[OUT_TREFLIND].bNeg = 1;
  output[OUT_TREFLIND].dNeg = 1; 
  output[OUT_TREFLIND].iNum = 1;
  output[OUT_TREFLIND].iModuleBit = THERMINT;
  fnWrite[OUT_TREFLIND] = &WriteTrefLind;

}

void FinalizeOutputFunctionThermint(OUTPUT *output,int iBody,int iModule) {
  //  output[OUT_TDOTMAN].fnOutput[iBody][iModule] = &fdTDotMan;
  //    output[OUT_SURFENFLUX].fnOutput[iBody][iModule] = &fdSurfEnFlux; //This is need to print the global var to log.  Needs to be fixed.
  //PD: I commented out the above line bc I don't know what it's for.  SURFENFLUX is called HFLOWSURF in thermint.
}

/************ THERMINT Logging Functions **************/

void LogOptionsThermint(CONTROL *control, FILE *fp) {
  /* Anything here?
  fprintf(fp,"-------- RADHEAT Options -----\n\n");
  */
}

//PED: this would be for global rad heat parameters, but this is blank bc rad is only relevant to each individual body.
void LogThermint(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UPDATE *update,fnWriteOutput fnWrite[],FILE *fp) {
  /* Anything here? 
  int iOut;
  fprintf(fp,"\n----- RADHEAT PARAMETERS ------\n");
  for (iOut=OUTSTARTRADHEAT;iOut<OUTBODYSTARTRADHEAT;iOut++) {
    if (output[iOut].iNum > 0)
      WriteLogEntry(control,output[iOut],body,system,fnWrite[iOut],fp,update,0);
  }
  */
}

void LogBodyThermint(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UPDATE *update,fnWriteOutput fnWrite[],FILE *fp,int iBody) {
  int iOut;

  fprintf(fp,"----- THERMINT PARAMETERS (%s)------\n",body[iBody].cName);
  for (iOut=OUTSTARTTHERMINT;iOut<OUTENDTHERMINT;iOut++) {
    if (output[iOut].iNum > 0) 
      WriteLogEntry(body,control,&output[iOut],system,update,fnWrite[iOut],fp,iBody);
  }
  /* Write out some global constants. */
  fprintf(fp,"EMASS=%e EMASSMAN=%e ERMAN=%e ERCORE=%e EDMAN=%e EVOL=%e EVOLCORE=%e EVOLMAN=%e\n",EMASS,EMASSMAN,ERMAN,ERCORE,EDMAN,EVOL,EVOLCORE,EVOLMAN);
  fprintf(fp,"EDENS=%e EDENSMAN=%e EDENSCORE=%e EDENSOC=%e EDENSIC=%e STIFFNESS=%e\n",EDENS,EDENSMAN,EDENSCORE,EDENSOC,EDENSIC,STIFFNESS);
  fprintf(fp,"THERMEXPANMAN=%e THERMCONDUMAN=%e THERMCONDLMAN=%e THERMDIFFUMAN=%e cube(EDMAN)=%e\n",THERMEXPANMAN,THERMCONDUMAN,THERMCONDLMAN,THERMDIFFUMAN,cube(EDMAN));
  fprintf(fp,"TrefLind=%e ViscRef=%e VISCJUMPMAN=%e \n",body[iBody].dTrefLind,body[iBody].dViscRef,VISCJUMPMAN);
  fprintf(fp,"DTCHIREF=%e CHI_OC_E=%e PARTITION=%e CHI_IC_E=%e EMASSOC_CHI=%e EMASSIC_CHI=%e EMASSCORE_CHI=%e\n",body[iBody].dDTChiRef,CHI_OC_E,PARTITION_CHI_CORE,CHI_IC_E,EMASSOC_CHI,EMASSIC_CHI,EMASSCORE_CHI);
  fprintf(fp,"body.ViscMeltB=%e Delta=%e Gamma=%e Xi=%e Phis=%e \n",body[iBody].dViscMeltB,body[iBody].dViscMeltDelta,body[iBody].dViscMeltGamma,body[iBody].dViscMeltXi,body[iBody].dViscMeltPhis);
  fprintf(fp,"body.dFixMeltfactorUMan=%f .dMeltfactorUMan=%e \n",body[iBody].dFixMeltfactorUMan,body[iBody].dMeltfactorUMan);
  fprintf(fp,"body.dStagLid=%f dManHFlowPref=%f \n",body[iBody].dStagLid,body[iBody].dManHFlowPref);
  fprintf(fp,"body.dMagMomCoef=%f \n",body[iBody].dMagMomCoef);
}

void AddModuleThermint(MODULE *module,int iBody,int iModule) {

  module->iaModule[iBody][iModule] = THERMINT;

  module->fnCountHalts[iBody][iModule] = &CountHaltsThermint;
  module->fnReadOptions[iBody][iModule] = &ReadOptionsThermint;
  module->fnLogBody[iBody][iModule] = &LogBodyThermint;
  module->fnVerify[iBody][iModule] = &VerifyThermint;
  module->fnVerifyHalt[iBody][iModule] = &VerifyHaltThermint;
  
  module->fnInitializeBody[iBody][iModule] = &InitializeBodyThermint;
  module->fnInitializeUpdate[iBody][iModule] = &InitializeUpdateThermint;

    // NEED TO ADD THERMINT VARIABLES HERE??
  module->fnFinalizeUpdateTMan[iBody][iModule] = &FinalizeUpdateTManThermint;
  module->fnFinalizeUpdateTCore[iBody][iModule] = &FinalizeUpdateTCoreThermint;

  //module->fnIntializeOutputFunction[iBody][iModule] = &InitializeOutputFunctionThermint;
  module->fnFinalizeOutputFunction[iBody][iModule] = FinalizeOutputFunctionThermint;

}

/************* THERMINT Functions ************/
/* Scalar Properties */
/* Get TUMan */
double fdTUMan(BODY *body,int iBody) {
  return ADJUMPM2UM*body[iBody].dTMan;
}
/* Get TLMan */
double fdTLMan(BODY *body,int iBody) {
    return ADJUMPM2LM*body[iBody].dTMan;
}
/* Get TCMB */
double fdTCMB(BODY *body,int iBody) {
    return ADJUMPC2CMB*body[iBody].dTCore;
}
/* Get TJumpUMan */ 
double fdTJumpUMan(BODY *body,int iBody) {
    return fabs(body[iBody].dTUMan-TSURF);
}
/* Get TJumpLMan */
double fdTJumpLMan(BODY *body,int iBody) {
    return fabs(body[iBody].dTCMB-body[iBody].dTLMan);
}
/* Get SignTJumpUMan */ 
double fdSignTJumpUMan(BODY *body,int iBody) {
    return (body[iBody].dTUMan-TSURF)/fabs(body[iBody].dTUMan-TSURF);
}
/* Get SignTJumpLMan */
double fdSignTJumpLMan(BODY *body,int iBody) {
    return (body[iBody].dTCMB-body[iBody].dTLMan)/fabs(body[iBody].dTCMB-body[iBody].dTLMan);
}
/* Get ViscUManArr Arrhenius law only */
double fdViscUManArr(BODY *body,int iBody) {  
  return body[iBody].dViscRef*exp(ACTVISCMAN/(GASCONSTANT*body[iBody].dTUMan));
}
/* Get ViscUMan */
double fdViscUMan(BODY *body,int iBody) {
  return body[iBody].dViscUManArr/body[iBody].dMeltfactorUMan;
}
double fdViscLMan(BODY *body,int iBody) {
  return body[iBody].dViscUManArr*body[iBody].dViscJumpMan/body[iBody].dMeltfactorLMan;   //w/ meltfactorLMan
}
double fdViscMMan(BODY *body,int iBody) {
  return body[iBody].dViscUMan*(VISCJUMPMMAN);
}
/* Get ViscJumpMan */
double fdViscJumpMan(BODY *body,int iBody) {
    return body[iBody].dViscJumpMan;
}
/* Get Boundary Layer Thicknesses */
double fdBLUMan(BODY *body,int iBody) {
  return (EDMAN)*pow((RACRIT)*body[iBody].dViscUMan*(THERMDIFFUMAN)/((THERMEXPANMAN)*(GRAVLMAN)*body[iBody].dTJumpLMan*cube(EDMAN)),(CONVEXPON));
}
double fdBLLMan(BODY *body,int iBody) {
  return (EDMAN)*pow((RACRIT)*body[iBody].dViscLMan*(THERMDIFFLMAN)/((THERMEXPANMAN)*(GRAVLMAN)*body[iBody].dTJumpLMan*cube(EDMAN)),(CONVEXPON));
}
double fdShmodUMan(BODY *body,int iBody) {
    return (SHMODREF)*exp((ACTSHMODMAN)/(GASCONSTANT*body[iBody].dTUMan))/body[iBody].dMeltfactorUMan;
}
double fdTsolUMan(BODY *body,int iBody) {
    return fdSolidusMan(body[iBody].dBLUMan);
}
double fdTsolLMan(BODY *body,int iBody) {
  return fdSolidusMan(ERADIUS-ERCORE-body[iBody].dBLLMan);  //Solidus fn of depth: z_LM=R-R_c-delta_LM
}
double fdTliqUMan(BODY *body,int iBody) {
    return body[iBody].dTsolUMan+DTLIQMAN;  //approx constant offset btwn sol and liq.
}
double fdTliqLMan(BODY *body,int iBody) {
    return body[iBody].dTsolLMan+DTLIQMAN;  //approx constant offset btwn sol and liq.
}
double fdFMeltUMan(BODY *body,int iBody) {
    double fmelt=(body[iBody].dTUMan-body[iBody].dTsolUMan)/(body[iBody].dTliqUMan-body[iBody].dTsolUMan);
    double fmelt2=min(fmelt,1.0);  //require fmelt<1
    fmelt2=max(fmelt2,0.0);        //require fmelt>0
    return fmelt2;
}
double fdFMeltLMan(BODY *body,int iBody) {
    double fmelt=(body[iBody].dTLMan-body[iBody].dTsolLMan)/(body[iBody].dTliqLMan-body[iBody].dTsolLMan);
    double fmelt2=min(fmelt,1.0);  //require fmelt<1
    fmelt2=max(fmelt2,0.0); //require fmelt>0
    double min1=min(1.0,2.0);  //test min
    double max1=max(1.0,2.0);  //test max
    return fmelt2;
}
double fdMeltfactorUMan(BODY *body,int iBody) {
    /* Melt factor used in viscosity and shmod.  For fmelt=1, this should give 1.20428. */
    if (body[iBody].dFixMeltfactorUMan==1) {   //if FixMeltFactorUMan=1 then return the initialized value.
      return body[iBody].dMeltfactorUMan;
    } else {                                   //if FixMeltFactorUMan not =1 then compute it.
      double bigphi=body[iBody].dFMeltUMan/body[iBody].dViscMeltPhis;
      double bigf=(1.0-body[iBody].dViscMeltXi)*erf(sqrt(PI)/(2.0*(1.0-body[iBody].dViscMeltXi))*bigphi*(1.0+pow(bigphi,body[iBody].dViscMeltGamma)));
      double meltexp=body[iBody].dViscMeltB*body[iBody].dViscMeltPhis;
      return (1.0+pow(bigphi,body[iBody].dViscMeltDelta))/pow(1.0-bigf,meltexp);
  }
}
double fdMeltfactorLMan(BODY *body,int iBody) {
    /* Melt factor used in viscosity and shmod.  For fmelt=1, this should give 1.20428. */
    double bigphi=body[iBody].dFMeltLMan/body[iBody].dViscMeltPhis;
    double bigf=(1.0-body[iBody].dViscMeltXi)*erf(sqrt(PI)/(2.0*(1.0-body[iBody].dViscMeltXi))*bigphi*(1.0+pow(bigphi,body[iBody].dViscMeltGamma)));
    double meltexp=body[iBody].dViscMeltB*body[iBody].dViscMeltPhis;
    return (1.0+pow(bigphi,body[iBody].dViscMeltDelta))/pow(1.0-bigf,meltexp);
}

/* Get DepthMeltMan */ 
double fdDepthMeltMan(BODY *body,int iBody) {
    //    double guess1=body[iBody].dBLUMan;  //lower bound of depth to bottom of UM melt layer.
    //    double guess2=1.445e6;  //(EDMAN)/2.0;  //mid-mantle= upper bound to depth of melt layer.
    //    double tol=10.0;  //root resolution is +/- 10 m?
    //    int nmax=100;  //nmax iterations of root.
    int type=0;  //types: 0=UMan, 1=LMan, 2=ICN
    //    return root(type,body,iBody,guess1,guess2,tol,nmax);
    double depthmeltman=cubicroot(type,body,iBody);
    if (depthmeltman < body[iBody].dBLUMan) {        //if solidus intersects adiabat within UMTBL then recompute it.
      depthmeltman=cubicroot(1,body,iBody);        //type=1 to find intersection within UMTBL.
    }
    return depthmeltman;
}
/* Get TDepthMeltMan */
double fdTDepthMeltMan(BODY *body,int iBody) {
    if (body[iBody].dDepthMeltMan==0) {    //if no melt layer found.
          return 0;
      } else {                         //if yes melt layer found.
          return fdSolidusMan(body[iBody].dDepthMeltMan);
      }
}
/* Get TJumpMeltMan */ 
double fdTJumpMeltMan(BODY *body,int iBody) {
  return body[iBody].dTDepthMeltMan-TSURF-(ADGRADMAN)*body[iBody].dDepthMeltMan;  //Temp jump across entire UM melt region.
}
double fdRayleighMan(BODY *body,int iBody) {
  return body[iBody].dSignTJumpUMan*(THERMEXPANMAN)*(GRAVUMAN)*(body[iBody].dTJumpUMan+body[iBody].dTJumpLMan)*pow(EDMAN,3.)/((THERMDIFFUMAN)*body[iBody].dViscMMan);  //Mantle Rayleigh number defined in terms of ViscMMan and SignTJumpUMan.
}

double fdK2Man(BODY *body,int iBody) {
    return 3./2/(1.+19./2*body[iBody].dShmodUMan/(STIFFNESS));
}
double fdImk2Man(BODY *body,int iBody) {
  double viscdyn=body[iBody].dViscUMan*(EDENSMAN); //dynamic viscosity.

  /* Peter's version. I think dRotRate should be dMeanMotion
  double denom2=pow((1.+(19./2)*(body[iBody].dShmodUMan/(STIFFNESS)))*(viscdyn*body[iBody].dRotRate/body[iBody].dShmodUMan),2.);
  double imk2=(57./4)*viscdyn*body[iBody].dRotRate/( (STIFFNESS)*(1.0+ denom2) );
  */
  double denom2=pow((1.+(19./2)*(body[iBody].dShmodUMan/(STIFFNESS)))*(viscdyn*body[iBody].dMeanMotion/body[iBody].dShmodUMan),2.);
  double imk2=(57./4)*viscdyn*body[iBody].dMeanMotion/( (STIFFNESS)*(1.0+ denom2) );
  return imk2;
}

/* Core Chemistry */
double fdMassIC(BODY *body, int iBody) {
  return 4./3*PI*pow(body[iBody].dRIC,3.)*(EDENSIC);
}
double fdMassOC(BODY *body, int iBody) {
  return EMASSCORE-body[iBody].dMassIC;
}
double fdMassChiOC(BODY *body, int iBody) {
  return EMASSCORE_CHI/( PARTITION_CHI_CORE*body[iBody].dMassIC/body[iBody].dMassOC + 1. );
}
double fdMassChiIC(BODY *body, int iBody) {
  return EMASSCORE_CHI-body[iBody].dMassChiOC;
}
double fdChiOC(BODY *body, int iBody) {
  return body[iBody].dMassChiOC/body[iBody].dMassOC;
}
double fdChiIC(BODY *body, int iBody) {
  if (body[iBody].dRIC>0.) {
    return body[iBody].dMassChiIC/body[iBody].dMassIC;
  } else {
    return 0.;
  }
}
double fdDTChi(BODY *body, int iBody) {
  return body[iBody].dDTChiRef*body[iBody].dChiOC/(CHI_OC_E);
}


/* Inner Core Size */
double fdRIC(BODY *body,int iBody) {
  double dRIC;

  /*  OLD VERSION without light element depression.
    double numerator=pow((DADCORE)/(ERCORE),2.0)*log(body[iBody].dTrefLind/body[iBody].dTCMB)-1.0;
    if (numerator>0) {    //IC Found.
        return (ERCORE)*sqrt( numerator/(2.0*(1.0-1.0/3.0/(GRUNEISEN))*pow((DADCORE)/(DLIND),2.0)-1.0) );
  */
  /* NEW VERSION with light element liquidus depression  */

  double T_fe_cen=body[iBody].dTrefLind-(body[iBody].dDTChi);     //Liquidus at center of core.
  double T_fe_cmb=(body[iBody].dTrefLind)*exp(-2.*(1.-1./(3.*(GRUNEISEN)))*pow((ERCORE)/(DLIND),2.0))-(body[iBody].dDTChi);//Liquidus@CMB
  double numerator=1.+pow((DADCORE)/(ERCORE),2.)*log(body[iBody].dTCMB/T_fe_cen);
  double denom=1.+pow((DADCORE)/(ERCORE),2.0)*log(T_fe_cmb/T_fe_cen);
  if ((numerator/denom)>0.) {    //IC exists
    dRIC = (ERCORE)*sqrt(numerator/denom);
  } else {
    dRIC = 0;        //no IC.
  }

  if (dRIC > ERCORE)
    dRIC = ERCORE;

  return dRIC;
}
double fdThermConductOC(BODY *body, int iBody) {
  return (ELECCONDCORE)*(LORENTZNUM)*body[iBody].dTCMB;
}
double fdHfluxCMBAd(BODY *body, int iBody) {
  return body[iBody].dThermConductOC*body[iBody].dTCMB*(ERCORE)/pow(DADCORE,2.);
}
double fdHfluxCMBConv(BODY *body, int iBody) {
  return body[iBody].dHfluxCMB-body[iBody].dHfluxCMBAd;
}
double fdGravICB(BODY *body, int iBody) {
  return (GRAVCMB)*body[iBody].dRIC/(ERCORE);
}
double fdRICDot(BODY *body,UPDATE *update, int iBody) {
  double denom=(2*body[iBody].dRIC*(2.*(1.-1/(3.*GRUNEISEN))*pow((DADCORE)/(DLIND),2)-1.));
  return -1*pow((DADCORE),2)/denom*(*(update[iBody].pdTDotCore))/body[iBody].dTCore;
  //  return 1/denom;
}
double fdCoreBuoyTherm(BODY *body, int iBody) {
  return (THERMEXPANCORE)*(GRAVCMB)*body[iBody].dHfluxCMBConv/((EDENSCORE)*(SPECHEATCORE));
}
double fdCoreBuoyCompo(BODY *body, int iBody) {
  return body[iBody].dGravICB*(DENSANOMICB)/(EDENSCORE)*pow(body[iBody].dRIC/(ERCORE),2)*body[iBody].dRICDot;
}
double fdCoreBuoyTotal(BODY *body, int iBody) {
  //PD: Why do we need this if statement?
  if (body[iBody].dRIC > 0.) {
    return  (body[iBody].dCoreBuoyTherm+body[iBody].dCoreBuoyCompo);
  } else {
    return body[iBody].dCoreBuoyTherm;
  }
  //  return (body[iBody].dCoreBuoyTherm+body[iBody].dCoreBuoyCompo);
}
double fdMagMom(BODY *body, int iBody) {
  return 4.*PI*pow((ERCORE),3)*body[iBody].dMagMomCoef*sqrt((EDENSCORE)/(2*(MAGPERM)))*pow(body[iBody].dCoreBuoyTotal*((ERCORE)-body[iBody].dRIC),1./3);
}

/* All tidal phenomena should exist exclusively in eqtide.c.   Heat Flows 
double fdTidalPowMan(BODY *body,int iBody) {
   Peter's version. I think dRotRate should be dMeanMotion.
  return (21./2)*body[iBody].dImk2Man*(BIGG)*pow(body[0].dMass/pow(body[iBody].dSemi,3.),2.)*pow(body[iBody].dRadius,5.)*body[iBody].dRotRate*pow(body[iBody].dEcc,2.);
  return (21./2)*body[iBody].dImk2Man*(BIGG)*pow(body[0].dMass/pow(body[iBody].dSemi,3.),2.)*pow(body[iBody].dRadius,5.)*body[iBody].dMeanMotion*pow(body[iBody].dEcc,2.);
}
*/

/* Heat Fluxes/flows */
double fdHfluxUMan(BODY *body,int iBody) {
  return (THERMCONDUMAN)*body[iBody].dSignTJumpUMan*body[iBody].dTJumpUMan/body[iBody].dBLUMan;
}
double fdHfluxLMan(BODY *body,int iBody) {
  return (THERMCONDLMAN)*body[iBody].dSignTJumpLMan*body[iBody].dTJumpLMan/body[iBody].dBLLMan;
}
double fdHfluxCMB(BODY *body,int iBody) {
  return fdHfluxLMan(body,iBody);
}
double fdHflowUMan(BODY *body,int iBody) {
  return body[iBody].dManHFlowPref*(EAREASURF)*fdHfluxUMan(body,iBody);
}
double fdHflowSurf(BODY *body,int iBody) {
  return body[iBody].dHflowUMan+body[iBody].dHflowMeltMan;
}
double fdHflowLMan(BODY *body,int iBody) {
  return (EAREACMB)*fdHfluxLMan(body,iBody);
}
double fdHflowCMB(BODY *body,int iBody) {
  return fdHflowLMan(body,iBody);
}
double fdHflowLatentMan(BODY *body,UPDATE *update,int iBody) {
    double HflowLatentMan=(-DVLIQDTEMP)*(*(update[iBody].pdTDotMan))*(EDENSMAN)*(SPECLATENTMAN);  //which structure has dTDotMan??
    HflowLatentMan=max(HflowLatentMan,0);   //ensure positive.
    return HflowLatentMan;
}
double fdMeltMassFluxMan(BODY *body,int iBody) {
  //Should crustmass be an ODE?  Or aux prop?
  return 1.16*(THERMDIFFUMAN)*(EAREASURF)/body[iBody].dBLUMan*(EDENSMAN)*body[iBody].dFMeltUMan; //DB15 (31)  =dot(M)_melt
}  
double fdHflowMeltMan(BODY *body,int iBody) {
  //    double MeltMassDot=1.16*(THERMDIFFUMAN)*(EAREASURF)/body[iBody].dBLUMan*(EDENSMAN)*body[iBody].dFMeltUMan; //DB15 (31)   
  //    return body[iBody].dEruptEff*MeltMassDot*((SPECLATENTMAN)+(SPECHEATMAN)*body[iBody].dTJumpMeltMan);
  return body[iBody].dEruptEff*body[iBody].dMeltMassFluxMan*((SPECLATENTMAN)+(SPECHEATMAN)*body[iBody].dTJumpMeltMan);
}
double fdHflowSecMan(BODY *body,int iBody) {
  return body[iBody].dHflowUMan+body[iBody].dHflowMeltMan-body[iBody].dHflowLMan-body[iBody].dHflowLatentMan-body[iBody].dTidalPowMan-body[iBody].dRadPowerMan;
}

double fdDRICDTCMB(BODY *body,int iBody) {            //=d(R_ic)/d(T_cmb)
  if (body[iBody].dRIC>0) {   //If IC exists.
    /* Old Version: from DB14 equations */
    double T_fe_cen=body[iBody].dTrefLind-(body[iBody].dDTChi);     //Liquidus at center of core.
    double T_fe_cmb=(body[iBody].dTrefLind)*exp(-2.*(1.-1./(3.*(GRUNEISEN)))*pow((ERCORE)/(DLIND),2.0))-(body[iBody].dDTChi);//Liquidus@CMB
    double denom=pow((DADCORE)/(ERCORE),2.)*log(T_fe_cmb/T_fe_cen)+1.;
    return (1./2)*pow((DADCORE),2.)/body[iBody].dRIC/body[iBody].dTCMB/denom;   //NOTES 3/16/16 -5-

    /* Newer Version: From notes on 4/23/15 */
    /*    double gamma_core2=2.*(1-1./(3.*GRUNEISEN));
    double a_drdt=-2.*gamma_core2*body[iBody].dRIC/pow(DLIND,2)*body[iBody].dTrefLind*exp(-gamma_core2*pow(body[iBody].dRIC/dl,2));
b_drdt=3.*dt_fe/chi_oc_e*mass_ic*mass_chi_total*(core_partition-1.)/(mass_core+mass_ic*(core_partition-1.))^2*(1./r_ic) ;coef
dri_dTcmb2=t_icb/t_cmb/(a_drdt+b_drdt+2.*r_ic*t_icb/dn^2)  ;notes 4/23/15.
    */
  } else {                    //If no IC.
    return 0;
  }
}
double fdMassICDot(BODY *body,UPDATE *update,int iBody) {
    if (body[iBody].dRIC>0) {   //If IC exists.
      double areaic=4.0*PI*pow(body[iBody].dRIC,2.0);
      return areaic*(EDENSIC)*(*(update[iBody].pdTDotCore))/(ADJUMPC2CMB)*body[iBody].dDRICDTCMB;  //DB14 (31)
    } else {                    //If no IC.
      return 0;
    }
}

double fdHflowLatentIC(BODY *body,UPDATE *update,int iBody) {
    if (body[iBody].dRIC>0) {   //If IC exists.
      return body[iBody].dMassICDot*(SPECLATENTICB);  //DB14 (26)
    } else {                    //If no IC.
      return 0;
    }
}
double fdPowerGravIC(BODY *body,UPDATE *update,int iBody) {
    if (body[iBody].dRIC>0) {   //If IC exists.
      return body[iBody].dMassICDot*(SPECPOWGRAVIC);  //DB14 (26)
    } else {                    //If no IC.
      return 0;
    }
}

/*** These derivatives are called from the udpate matrix, format is fixed. ***/
/* Get TDotMan */
double fdTDotMan(BODY *body,SYSTEM *system,int *iaBody) {
  int iBody=iaBody[0];
  //  return (body[iBody].dHflowCMB+body[iBody].dRadPowerMan+body[iBody].dHflowLatentMan+body[iBody].dTidalPowMan-body[iBody].dHflowUMan-body[iBody].dHflowMeltMan)/((EMASSMAN)*(SPECHEATMAN));
  return -body[iBody].dHflowSecMan/((EMASSMAN)*(SPECHEATMAN));
}

/* Get TDotCore */
double fdTDotCore(BODY *body,SYSTEM *system,int *iaBody) {
  int iBody=iaBody[0];   //Is this correct?
  double areaic=4.0*PI*pow(body[iBody].dRIC,2.0);
  return (-body[iBody].dHflowCMB+body[iBody].dRadPowerCore)/((EMASSCORE)*(SPECHEATCORE) -areaic*(EDENSIC)*(ADJUMPC2CMB)*body[iBody].dDRICDTCMB*(SPECLATENTICB+SPECPOWGRAVIC));
}



/****************************************************************************/
/****************************************************************************/
/****************************************************************************/
/*********************************   MATH   *********************************/
/****************************************************************************/
/* MATH FUNCTIONS */
double cube(double x) {
  return pow(x,3);
}
double root(int type,BODY *body,int iBody,double guess1,double guess2,double tol,int nmax) {
    double mid=0.0;  //current midpoint or root.
    double fmid=0.0;  //current value of f(mid).
    double fguess1=0.0;  //current value of f(guess1).
    int count=0.0;
    while (count <= nmax) {
      mid=(guess1+guess2)/2.0;                            //bisection method from wikipedia.
      if (type == 0) {                                    //use fdSolTempDiffMan
          fmid=fdSolTempDiffMan(mid,body,iBody);          //function at mid.
          fguess1=fdSolTempDiffMan(guess1,body,iBody);    //function at guess1.
      }
      if (fmid==0 || (guess2-guess1)/2.0<=tol) {          //solution found.
          return mid;
      }
      count++;             //increment count by 1.
      if (fmid/fabs(fmid)==fguess1/fabs(fguess1)) {guess2=mid;}
      else {guess1=mid;}
    }
    printf("method failed (nmax exceeded)\n");
    return 0;
}

double cubicroot(int type,BODY *body,int iBody) {
    double a=0,b=0,c=0,d=0;  //coefficients of cubic polynomial.    
    if (type==0){     //type=0 is melt intersection in adiabatic part of mantle, away from TBL's.
      a=ASOLIDUS;
      b=BSOLIDUS;
      c=CSOLIDUS+ADGRADMAN;
      d=DSOLIDUS-body[iBody].dTUMan-(ADGRADMAN)*((ERMAN)-body[iBody].dBLUMan);
    }
    if (type==1){    //type=1 is melt intersection within UM TBL.
      a=ASOLIDUS;
      b=BSOLIDUS;
      c=CSOLIDUS+body[iBody].dTJumpUMan/body[iBody].dBLUMan;
      d=DSOLIDUS-TSURF-body[iBody].dTJumpUMan/body[iBody].dBLUMan*(ERMAN);
    }   
    double delta0=pow(b,2.0)-3.0*a*c;                                             //cubic root component (wikip)
    double delta1=2.0*cube(b)-9.0*a*b*c+27.0*pow(a,2.0)*d;                        //cubic root component (wikip)
    double croot=pow( (delta1+sqrt(pow(delta1,2.0)-4.0*cube(delta0))) /2.0,1./3); //cubic root component (wikip)
    if (pow(delta1,2.0)-4.0*cube(delta0) < 0) {
      //        printf("imaginary cubic root!\n");
      //        exit(1);
      return 0;       //imaginary root implies no intersection, no melt layer?
    }
    double root=-1.0/(3.0*a)*(b+croot+delta0/croot);                              //real cubic root, radius of layer.
    return ERMAN-root;                                                            //Return depth.
}
double fdSolidusMan(double depth) {
    double r=(ERMAN)-depth;  //radius to bottom of region.
    double aterm=(ASOLIDUS)*pow(r,3.);
    double bterm=(BSOLIDUS)*pow(r,2.);
    double cterm=(CSOLIDUS)*r;
    return (ASOLIDUS)*pow(r,3.)+(BSOLIDUS)*pow(r,2.)+(CSOLIDUS)*r+(DSOLIDUS);
}    
double fdSolTempDiffMan(double depth,BODY *body,int iBody) { //Given a depth and BODY, return the difference between the solidus and geotherm at that depth.
    double solidus=fdSolidusMan(depth);
    double geotherm=TSURF+body[iBody].dSignTJumpUMan*body[iBody].dTJumpUMan*erf(2.0*depth/body[iBody].dBLUMan); //DB14 (16)
    return solidus-geotherm;
}

double fdSurfEnFlux(BODY *body,SYSTEM *system,UPDATE *update,int iBody,int iFoo) {

  /* dHFlowUMan is the energy flux at the top of the mantle, but includes 
     radiogenic heating. Therefore we must subtract off the radiogenic
     heating from the core and mantle, but not the crust, which is not
     part of thermint. */

  //return (body[iBody].dHflowUMan - (fdRadPowerMan(update,iBody)+fdRadPowerCore(update,iBody)))/(4*PI*body[iBody].dRadius*body[iBody].dRadius);
  //  return (body[iBody].dHflowUMan - fdRadPowerTot(update,iBody))/(4*PI*body[iBody].dRadius*body[iBody].dRadius);

  // PD: SurfEnFlux should be Total surface power/area.  HflowUMan contains all of mantle, +crust makes it total.
  return (body[iBody].dHflowUMan+body[iBody].dRadPowerCrust)/(4*PI*body[iBody].dRadius*body[iBody].dRadius);
}
