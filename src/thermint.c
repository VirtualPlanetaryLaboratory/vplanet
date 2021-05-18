/**
  @file thermint.c
  @brief Thermal evolution of the planet's interior (mantle + core)
  @author Peter Driscoll ([peteredriscoll](https://github.com/peteredriscoll/))
  @date June 1 2015
*/

#include "vplanet.h"

/**
  Copy values from src to dest for iBody.

  @param dest Body struct containing destination
  @param src Body struct containing source
  @param foo
  @param iNumBodies Index number of bodies
  @param iBody Body index
*/
void fvBodyCopyThermint(BODY *dest, BODY *src, int foo, int iNumBodies,
                        int iBody) {
  dest[iBody].dTSurf = src[iBody].dTSurf;
  dest[iBody].dTMan  = src[iBody].dTMan;
  dest[iBody].dTCore = src[iBody].dTCore;
  /* Constants */
  dest[iBody].dViscJumpMan   = src[iBody].dViscJumpMan;
  dest[iBody].dViscRef       = src[iBody].dViscRef;
  dest[iBody].dTrefLind      = src[iBody].dTrefLind;
  dest[iBody].dDTChiRef      = src[iBody].dDTChiRef;
  dest[iBody].dEruptEff      = src[iBody].dEruptEff;
  dest[iBody].dViscMeltDelta = src[iBody].dViscMeltDelta;
  dest[iBody].dViscMeltB     = src[iBody].dViscMeltB;
  dest[iBody].dViscMeltPhis  = src[iBody].dViscMeltPhis;
  dest[iBody].dViscMeltGamma = src[iBody].dViscMeltGamma;
  dest[iBody].dViscMeltXi    = src[iBody].dViscMeltXi;
  dest[iBody].dStagLid       = src[iBody].dStagLid;
  dest[iBody].dManHFlowPref  = src[iBody].dManHFlowPref;
  /* Aux Props Variables */
  dest[iBody].dTUMan         = src[iBody].dTUMan;
  dest[iBody].dTLMan         = src[iBody].dTLMan;
  dest[iBody].dTCMB          = src[iBody].dTCMB;
  dest[iBody].dTJumpUMan     = src[iBody].dTJumpUMan;
  dest[iBody].dTJumpLMan     = src[iBody].dTJumpLMan;
  dest[iBody].dSignTJumpUMan = src[iBody].dSignTJumpUMan;
  dest[iBody].dSignTJumpLMan = src[iBody].dSignTJumpLMan;
  dest[iBody].dViscUManArr   = src[iBody].dViscUManArr;
  dest[iBody].dViscUMan      = src[iBody].dViscUMan;
  dest[iBody].dViscLMan      = src[iBody].dViscLMan;
  dest[iBody].dViscMMan      = src[iBody].dViscMMan;
  dest[iBody].dBLUMan        = src[iBody].dBLUMan;
  dest[iBody].dBLLMan        = src[iBody].dBLLMan;
  // dShmodUMan in body.c:BodyCopy to avoid floating point exceptions with other
  // modules
  dest[iBody].dTsolUMan          = src[iBody].dTsolUMan;
  dest[iBody].dTsolLMan          = src[iBody].dTsolLMan;
  dest[iBody].dTliqUMan          = src[iBody].dTliqUMan;
  dest[iBody].dTliqLMan          = src[iBody].dTliqLMan;
  dest[iBody].dFMeltUMan         = src[iBody].dFMeltUMan;
  dest[iBody].dFMeltLMan         = src[iBody].dFMeltLMan;
  dest[iBody].dMeltfactorUMan    = src[iBody].dMeltfactorUMan;
  dest[iBody].dMeltfactorLMan    = src[iBody].dMeltfactorLMan;
  dest[iBody].dFixMeltfactorUMan = src[iBody].dFixMeltfactorUMan;
  dest[iBody].dDepthMeltMan      = src[iBody].dDepthMeltMan;
  dest[iBody].dTDepthMeltMan     = src[iBody].dTDepthMeltMan;
  dest[iBody].dTJumpMeltMan      = src[iBody].dTJumpMeltMan;
  dest[iBody].dMeltMassFluxMan   = src[iBody].dMeltMassFluxMan;
  dest[iBody].dRayleighMan       = src[iBody].dRayleighMan;
  dest[iBody].dDynamViscos       = src[iBody].dDynamViscos;
  /* Heat Flows */
  /* Mantle */
  dest[iBody].dHfluxUMan      = src[iBody].dHfluxUMan;
  dest[iBody].dHfluxLMan      = src[iBody].dHfluxLMan;
  dest[iBody].dHfluxCMB       = src[iBody].dHfluxCMB;
  dest[iBody].dHflowUMan      = src[iBody].dHflowUMan;
  dest[iBody].dHflowLMan      = src[iBody].dHflowLMan;
  dest[iBody].dHflowCMB       = src[iBody].dHflowCMB;
  dest[iBody].dHflowLatentMan = src[iBody].dHflowLatentMan;
  dest[iBody].dHflowMeltMan   = src[iBody].dHflowMeltMan;
  dest[iBody].dHflowSecMan    = src[iBody].dHflowSecMan;
  dest[iBody].dHflowSurf      = src[iBody].dHflowSurf;
  /* Core */
  dest[iBody].dRIC            = src[iBody].dRIC;
  dest[iBody].dDRICDTCMB      = src[iBody].dDRICDTCMB;
  dest[iBody].dMassICDot      = src[iBody].dMassICDot;
  dest[iBody].dHflowLatentIC  = src[iBody].dHflowLatentIC;
  dest[iBody].dPowerGravIC    = src[iBody].dPowerGravIC;
  dest[iBody].dChiOC          = src[iBody].dChiOC;
  dest[iBody].dChiIC          = src[iBody].dChiIC;
  dest[iBody].dMassOC         = src[iBody].dMassOC;
  dest[iBody].dMassIC         = src[iBody].dMassIC;
  dest[iBody].dMassChiOC      = src[iBody].dMassChiOC;
  dest[iBody].dMassChiIC      = src[iBody].dMassChiIC;
  dest[iBody].dDTChi          = src[iBody].dDTChi;
  dest[iBody].dHfluxCMBAd     = src[iBody].dHfluxCMBAd;
  dest[iBody].dHfluxCMBConv   = src[iBody].dHfluxCMBConv;
  dest[iBody].dThermConductOC = src[iBody].dThermConductOC;
  dest[iBody].dRICDot         = src[iBody].dRICDot;
  dest[iBody].dCoreBuoyTherm  = src[iBody].dCoreBuoyTherm;
  dest[iBody].dCoreBuoyCompo  = src[iBody].dCoreBuoyCompo;
  dest[iBody].dCoreBuoyTotal  = src[iBody].dCoreBuoyTotal;
  dest[iBody].dGravICB        = src[iBody].dGravICB;
  dest[iBody].dMagMomCoef     = src[iBody].dMagMomCoef;
  dest[iBody].dMagMom         = src[iBody].dMagMom;
  dest[iBody].dPresSWind      = src[iBody].dPresSWind;
  dest[iBody].dMagPauseRad    = src[iBody].dMagPauseRad;
  dest[iBody].dActViscMan     = src[iBody].dActViscMan;
  dest[iBody].dShModRef       = src[iBody].dShModRef;
  // dStiffness in body.c:BodyCopy to avoid floating point exceptions
  dest[iBody].dDLind        = src[iBody].dDLind;
  dest[iBody].dDAdCore      = src[iBody].dDAdCore;
  dest[iBody].dAdJumpM2UM   = src[iBody].dAdJumpM2UM;
  dest[iBody].dAdJumpM2LM   = src[iBody].dAdJumpM2LM;
  dest[iBody].dAdJumpC2CMB  = src[iBody].dAdJumpC2CMB;
  dest[iBody].dElecCondCore = src[iBody].dElecCondCore;
}

/**************** THERMINT options ********************/

/* Initial Mantle & Core Temperature */
/**
  Read surface temperature from input file

  @param body Body struct
  @param control Control struct
  @param files Files struct
  @param options Options struct
  @param system System struct
  @param iFile Index of file
*/
void fvReadTSurf(BODY *body, CONTROL *control, FILES *files, OPTIONS *options,
                 SYSTEM *system, int iFile) {
  int lTmp = -1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn, options->cName, &dTmp, &lTmp,
                  control->Io.iVerbose);
  if (lTmp >= 0) { // if line num of option ge 0
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                    control->Io.iVerbose);
    if (dTmp < 0) { // if input value lt 0
      body[iFile - 1].dTSurf =
            dTmp * dNegativeDouble(*options, files->Infile[iFile].cIn,
                                   control->Io.iVerbose);
    } else {
      body[iFile - 1].dTSurf =
            fdUnitsTemp(dTmp, control->Units[iFile].iTemp, 0);
    }
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else if (iFile >
             0) { // if line num not ge 0, then if iFile gt 0, then set default.
    body[iFile - 1].dTSurf = options->dDefault;
  }
}
/**
  Read mantle temperature from input file

  @param body Body struct
  @param control Control struct
  @param files Files struct
  @param options Options struct
  @param system System struct
  @param iFile Index of file
*/
void fvReadTMan(BODY *body, CONTROL *control, FILES *files, OPTIONS *options,
                SYSTEM *system, int iFile) {
  int lTmp = -1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn, options->cName, &dTmp, &lTmp,
                  control->Io.iVerbose);
  if (lTmp >= 0) { // if line num of option ge 0
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                    control->Io.iVerbose);
    if (dTmp < 0) { // if input value lt 0
      body[iFile - 1].dTMan =
            dTmp * dNegativeDouble(*options, files->Infile[iFile].cIn,
                                   control->Io.iVerbose);
    } else {
      body[iFile - 1].dTMan = fdUnitsTemp(dTmp, control->Units[iFile].iTemp, 0);
    }
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else if (iFile >
             0) { // if line num not ge 0, then if iFile gt 0, then set default.
    body[iFile - 1].dTMan = options->dDefault;
  }
}
/**
  Read core temperature from input file

  @param body Body struct
  @param control Control struct
  @param files Files struct
  @param options Options struct
  @param system System struct
  @param iFile Index of file
*/
void fvReadTCore(BODY *body, CONTROL *control, FILES *files, OPTIONS *options,
                 SYSTEM *system, int iFile) {
  int lTmp = -1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn, options->cName, &dTmp, &lTmp,
                  control->Io.iVerbose);
  if (lTmp >= 0) { // if line num of option ge 0
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                    control->Io.iVerbose);
    if (dTmp < 0) { // if input value lt 0
      body[iFile - 1].dTCore =
            dTmp * dNegativeDouble(*options, files->Infile[iFile].cIn,
                                   control->Io.iVerbose);
    } else {
      body[iFile - 1].dTCore =
            fdUnitsTemp(dTmp, control->Units[iFile].iTemp, 0);
    }
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else if (iFile >
             0) { // if line num not ge 0, then if iFile gt 0, then set default.
    body[iFile - 1].dTCore = options->dDefault;
  }
}
/**
  Read viscosity jump across mantle from input file

  @param body Body struct
  @param control Control struct
  @param files Files struct
  @param options Options struct
  @param system System struct
  @param iFile Index of file
*/
void fvReadViscJumpMan(BODY *body, CONTROL *control, FILES *files,
                       OPTIONS *options, SYSTEM *system, int iFile) {
  int lTmp = -1;
  double dTmp;
  AddOptionDouble(files->Infile[iFile].cIn, options->cName, &dTmp, &lTmp,
                  control->Io.iVerbose);
  if (lTmp >= 0) { // if line num of option ge 0
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                    control->Io.iVerbose);
    if (dTmp < 0) { // if input value lt 0
      body[iFile - 1].dViscJumpMan =
            dTmp * dNegativeDouble(*options, files->Infile[iFile].cIn,
                                   control->Io.iVerbose);
    } else {
      body[iFile - 1].dViscJumpMan = dTmp; // no units.
    }
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else if (iFile >
             0) { // if line num not ge 0, then if iFile gt 0, then set default.
    body[iFile - 1].dViscJumpMan = options->dDefault;
  }
}
/**
  Read reference viscosity from input file

  @param body Body struct
  @param control Control struct
  @param files Files struct
  @param options Options struct
  @param system System struct
  @param iFile Index of file
*/
void fvReadViscRef(BODY *body, CONTROL *control, FILES *files, OPTIONS *options,
                   SYSTEM *system, int iFile) {
  int lTmp = -1;
  double dTmp;
  AddOptionDouble(files->Infile[iFile].cIn, options->cName, &dTmp, &lTmp,
                  control->Io.iVerbose);
  if (lTmp >= 0) { // if line num of option ge 0
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                    control->Io.iVerbose);
    if (dTmp < 0) { // if input value lt 0
      body[iFile - 1].dViscRef =
            dTmp * dNegativeDouble(*options, files->Infile[iFile].cIn,
                                   control->Io.iVerbose);
    } else {
      body[iFile - 1].dViscRef = dTmp; // no units.
    }
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else if (iFile >
             0) { // if line num not ge 0, then if iFile gt 0, then set default.
    body[iFile - 1].dViscRef = options->dDefault;
  }
}
/**
  Read reference Lindemann (core liquidus) temperature from input file

  @param body Body struct
  @param control Control struct
  @param files Files struct
  @param options Options struct
  @param system System struct
  @param iFile Index of file
*/
void fvReadTrefLind(BODY *body, CONTROL *control, FILES *files,
                    OPTIONS *options, SYSTEM *system, int iFile) {
  int lTmp = -1;
  double dTmp;
  AddOptionDouble(files->Infile[iFile].cIn, options->cName, &dTmp, &lTmp,
                  control->Io.iVerbose);
  if (lTmp >= 0) { // if line num of option ge 0
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                    control->Io.iVerbose);
    if (dTmp < 0) { // if input value lt 0
      body[iFile - 1].dTrefLind =
            dTmp * dNegativeDouble(*options, files->Infile[iFile].cIn,
                                   control->Io.iVerbose);
    } else {
      body[iFile - 1].dTrefLind = dTmp; // no units.
    }
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else if (iFile >
             0) { // if line num not ge 0, then if iFile gt 0, then set default.
    body[iFile - 1].dTrefLind = options->dDefault;
  }
}
/**
  Read core liquidus light element depression from input file

  @param body Body struct
  @param control Control struct
  @param files Files struct
  @param options Options struct
  @param system System struct
  @param iFile Index of file
*/
void fvReadDTChiRef(BODY *body, CONTROL *control, FILES *files,
                    OPTIONS *options, SYSTEM *system, int iFile) {
  int lTmp = -1;
  double dTmp;
  AddOptionDouble(files->Infile[iFile].cIn, options->cName, &dTmp, &lTmp,
                  control->Io.iVerbose);
  if (lTmp >= 0) { // if line num of option ge 0
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                    control->Io.iVerbose);
    if (dTmp < 0) { // if input value lt 0
      body[iFile - 1].dDTChiRef =
            dTmp * dNegativeDouble(*options, files->Infile[iFile].cIn,
                                   control->Io.iVerbose);
    } else {
      body[iFile - 1].dDTChiRef = dTmp; // no units.
    }
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else if (iFile >
             0) { // if line num not ge 0, then if iFile gt 0, then set default.
    body[iFile - 1].dDTChiRef = options->dDefault;
  }
}
/**
  Read melt eruption efficiency from input file

  @param body Body struct
  @param control Control struct
  @param files Files struct
  @param options Options struct
  @param system System struct
  @param iFile Index of file
*/
void fvReadEruptEff(BODY *body, CONTROL *control, FILES *files,
                    OPTIONS *options, SYSTEM *system, int iFile) {
  int lTmp = -1;
  double dTmp;
  AddOptionDouble(files->Infile[iFile].cIn, options->cName, &dTmp, &lTmp,
                  control->Io.iVerbose);
  if (lTmp >= 0) { // if line num of option ge 0
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                    control->Io.iVerbose);
    if (dTmp < 0) { // if input value lt 0
      body[iFile - 1].dEruptEff =
            dTmp * dNegativeDouble(*options, files->Infile[iFile].cIn,
                                   control->Io.iVerbose);
    } else {
      body[iFile - 1].dEruptEff = dTmp; // no units.
    }
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else if (iFile >
             0) { // if line num not ge 0, then if iFile gt 0, then set default.
    body[iFile - 1].dEruptEff = options->dDefault;
  }
}
/**
  Read melt viscosity reduction "Phi^star" parameter from input file

  @param body Body struct
  @param control Control struct
  @param files Files struct
  @param options Options struct
  @param system System struct
  @param iFile Index of file
*/
void fvReadViscMeltPhis(BODY *body, CONTROL *control, FILES *files,
                        OPTIONS *options, SYSTEM *system, int iFile) {
  int lTmp = -1;
  double dTmp;
  AddOptionDouble(files->Infile[iFile].cIn, options->cName, &dTmp, &lTmp,
                  control->Io.iVerbose);
  if (lTmp >= 0) { // if line num of option ge 0
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                    control->Io.iVerbose);
    if (dTmp < 0) { // if input value lt 0
      body[iFile - 1].dViscMeltPhis =
            dTmp * dNegativeDouble(*options, files->Infile[iFile].cIn,
                                   control->Io.iVerbose);
    } else {
      body[iFile - 1].dViscMeltPhis = dTmp; // no units.
    }
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else if (iFile >
             0) { // if line num not ge 0, then if iFile gt 0, then set default.
    body[iFile - 1].dViscMeltPhis = options->dDefault;
  }
}
/**
  Read melt viscosity reduction "Xi" parameter from input file

  @param body Body struct
  @param control Control struct
  @param files Files struct
  @param options Options struct
  @param system System struct
  @param iFile Index of file
*/
void fvReadViscMeltXi(BODY *body, CONTROL *control, FILES *files,
                      OPTIONS *options, SYSTEM *system, int iFile) {
  int lTmp = -1;
  double dTmp;
  AddOptionDouble(files->Infile[iFile].cIn, options->cName, &dTmp, &lTmp,
                  control->Io.iVerbose);
  if (lTmp >= 0) { // if line num of option ge 0
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                    control->Io.iVerbose);
    if (dTmp < 0) { // if input value lt 0
      body[iFile - 1].dViscMeltXi =
            dTmp * dNegativeDouble(*options, files->Infile[iFile].cIn,
                                   control->Io.iVerbose);
    } else {
      body[iFile - 1].dViscMeltXi = dTmp; // no units.
    }
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else if (iFile >
             0) { // if line num not ge 0, then if iFile gt 0, then set default.
    body[iFile - 1].dViscMeltXi = options->dDefault;
  }
}
/**
  Read melt viscosity reduction "Gamma" parameter from input file

  @param body Body struct
  @param control Control struct
  @param files Files struct
  @param options Options struct
  @param system System struct
  @param iFile Index of file
*/
void fvReadViscMeltGamma(BODY *body, CONTROL *control, FILES *files,
                         OPTIONS *options, SYSTEM *system, int iFile) {
  int lTmp = -1;
  double dTmp;
  AddOptionDouble(files->Infile[iFile].cIn, options->cName, &dTmp, &lTmp,
                  control->Io.iVerbose);
  if (lTmp >= 0) { // if line num of option ge 0
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                    control->Io.iVerbose);
    if (dTmp < 0) { // if input value lt 0
      body[iFile - 1].dViscMeltGamma =
            dTmp * dNegativeDouble(*options, files->Infile[iFile].cIn,
                                   control->Io.iVerbose);
    } else {
      body[iFile - 1].dViscMeltGamma = dTmp; // no units.
    }
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else if (iFile >
             0) { // if line num not ge 0, then if iFile gt 0, then set default.
    body[iFile - 1].dViscMeltGamma = options->dDefault;
  }
}
/**
  Read melt viscosity reduction "delta" parameter from input file

  @param body Body struct
  @param control Control struct
  @param files Files struct
  @param options Options struct
  @param system System struct
  @param iFile Index of file
*/
void fvReadViscMeltDelta(BODY *body, CONTROL *control, FILES *files,
                         OPTIONS *options, SYSTEM *system, int iFile) {
  int lTmp = -1;
  double dTmp;
  AddOptionDouble(files->Infile[iFile].cIn, options->cName, &dTmp, &lTmp,
                  control->Io.iVerbose);
  if (lTmp >= 0) { // if line num of option ge 0
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                    control->Io.iVerbose);
    if (dTmp < 0) { // if input value lt 0
      body[iFile - 1].dViscMeltDelta =
            dTmp * dNegativeDouble(*options, files->Infile[iFile].cIn,
                                   control->Io.iVerbose);
    } else {
      body[iFile - 1].dViscMeltDelta = dTmp; // no units.
    }
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else if (iFile >
             0) { // if line num not ge 0, then if iFile gt 0, then set default.
    body[iFile - 1].dViscMeltDelta = options->dDefault;
  }
}
/**
  Read melt viscosity reduction "B" parameter from input file

  @param body Body struct
  @param control Control struct
  @param files Files struct
  @param options Options struct
  @param system System struct
  @param iFile Index of file
*/
void fvReadViscMeltB(BODY *body, CONTROL *control, FILES *files,
                     OPTIONS *options, SYSTEM *system, int iFile) {
  int lTmp = -1;
  double dTmp;
  AddOptionDouble(files->Infile[iFile].cIn, options->cName, &dTmp, &lTmp,
                  control->Io.iVerbose);
  if (lTmp >= 0) { // if line num of option ge 0
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                    control->Io.iVerbose);
    if (dTmp < 0) { // if input value lt 0
      body[iFile - 1].dViscMeltB =
            dTmp * dNegativeDouble(*options, files->Infile[iFile].cIn,
                                   control->Io.iVerbose);
    } else {
      body[iFile - 1].dViscMeltB = dTmp; // no units.
    }
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else if (iFile >
             0) { // if line num not ge 0, then if iFile gt 0, then set default.
    body[iFile - 1].dViscMeltB = options->dDefault;
  }
}
/**
  Read boolean option to fix upper mantle melt viscosity reduction from input
  file

  @param body Body struct
  @param control Control struct
  @param files Files struct
  @param options Options struct
  @param system System struct
  @param iFile Index of file
*/
void fvReadFixMeltfactorUMan(BODY *body, CONTROL *control, FILES *files,
                             OPTIONS *options, SYSTEM *system, int iFile) {
  int lTmp = -1;
  double dTmp;
  AddOptionDouble(files->Infile[iFile].cIn, options->cName, &dTmp, &lTmp,
                  control->Io.iVerbose);
  if (lTmp >= 0) { // if line num of option ge 0
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                    control->Io.iVerbose);
    if (dTmp < 0) { // if input value lt 0
      body[iFile - 1].dFixMeltfactorUMan =
            dTmp * dNegativeDouble(*options, files->Infile[iFile].cIn,
                                   control->Io.iVerbose);
    } else {
      body[iFile - 1].dFixMeltfactorUMan = dTmp; // no units.
    }
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else if (iFile >
             0) { // if line num not ge 0, then if iFile gt 0, then set default.
    body[iFile - 1].dFixMeltfactorUMan = options->dDefault;
  }
}
/**
  Read fixed lower mantle melt viscosity reduction from input file

  @param body Body struct
  @param control Control struct
  @param files Files struct
  @param options Options struct
  @param system System struct
  @param iFile Index of file
*/
void fvReadMeltfactorLMan(BODY *body, CONTROL *control, FILES *files,
                          OPTIONS *options, SYSTEM *system, int iFile) {
  /* This input option only applies when FixMeltfactorLMan=1 */
  int lTmp = -1;
  double dTmp;
  AddOptionDouble(files->Infile[iFile].cIn, options->cName, &dTmp, &lTmp,
                  control->Io.iVerbose);
  if (lTmp >= 0) { // if line num of option ge 0
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                    control->Io.iVerbose);
    if (dTmp < 0) { // XXX Rory doesn't think negatives options allowed
      body[iFile - 1].dMeltfactorLMan =
            dTmp * dNegativeDouble(*options, files->Infile[iFile].cIn,
                                   control->Io.iVerbose);
    } else {
      body[iFile - 1].dMeltfactorLMan = dTmp; // no units.
    }
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else if (iFile >
             0) { // if line num not ge 0, then if iFile gt 0, then set default.
    body[iFile - 1].dMeltfactorLMan = options->dDefault;
  }
}
/**
  Read fixed upper mantle melt viscosity reduction from input file

  @param body Body struct
  @param control Control struct
  @param files Files struct
  @param options Options struct
  @param system System struct
  @param iFile Index of file
*/
void fvReadMeltfactorUMan(BODY *body, CONTROL *control, FILES *files,
                          OPTIONS *options, SYSTEM *system, int iFile) {
  /* This input option only applies when FixMeltfactorUMan=1 */
  int lTmp = -1;
  double dTmp;
  AddOptionDouble(files->Infile[iFile].cIn, options->cName, &dTmp, &lTmp,
                  control->Io.iVerbose);
  if (lTmp >= 0) { // if line num of option ge 0
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                    control->Io.iVerbose);
    if (dTmp < 0) { // if input value lt 0
      body[iFile - 1].dMeltfactorUMan =
            dTmp * dNegativeDouble(*options, files->Infile[iFile].cIn,
                                   control->Io.iVerbose);
    } else {
      body[iFile - 1].dMeltfactorUMan = dTmp; // no units.
    }
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else if (iFile >
             0) { // if line num not ge 0, then if iFile gt 0, then set default.
    body[iFile - 1].dMeltfactorUMan = options->dDefault;
  }
}
/**
  Read option to fix stagnant lid mantle heat flow from input file

  @param body Body struct
  @param control Control struct
  @param files Files struct
  @param options Options struct
  @param system System struct
  @param iFile Index of file
*/
void fvReadStagLid(BODY *body, CONTROL *control, FILES *files, OPTIONS *options,
                   SYSTEM *system, int iFile) {
  int lTmp = -1;
  double dTmp;
  AddOptionDouble(files->Infile[iFile].cIn, options->cName, &dTmp, &lTmp,
                  control->Io.iVerbose);
  if (lTmp >= 0) { // if line num of option ge 0
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                    control->Io.iVerbose);
    if (dTmp < 0) { // if input value lt 0
      body[iFile - 1].dStagLid =
            dTmp * dNegativeDouble(*options, files->Infile[iFile].cIn,
                                   control->Io.iVerbose);
    } else {
      body[iFile - 1].dStagLid = dTmp; // no units.
    }
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else if (iFile >
             0) { // if line num not ge 0, then if iFile gt 0, then set default.
    body[iFile - 1].dStagLid = options->dDefault;
  }
}
/**
  Read mantle surface heat flow prefix coefficient from input file

  @param body Body struct
  @param control Control struct
  @param files Files struct
  @param options Options struct
  @param system System struct
  @param iFile Index of file
*/
void fvReadManHFlowPref(BODY *body, CONTROL *control, FILES *files,
                        OPTIONS *options, SYSTEM *system, int iFile) {
  int lTmp = -1;
  double dTmp;
  AddOptionDouble(files->Infile[iFile].cIn, options->cName, &dTmp, &lTmp,
                  control->Io.iVerbose);
  if (lTmp >= 0) { // if line num of option ge 0
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                    control->Io.iVerbose);
    if (dTmp < 0) { // if input value lt 0
      body[iFile - 1].dManHFlowPref =
            dTmp * dNegativeDouble(*options, files->Infile[iFile].cIn,
                                   control->Io.iVerbose);
    } else {
      body[iFile - 1].dManHFlowPref = dTmp; // no units.
    }
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else if (iFile >
             0) { // if line num not ge 0, then if iFile gt 0, then set default.
    if (body[iFile - 1].dStagLid ==
        0) { // if StagLid=0 then use plate tectonic default
      body[iFile - 1].dManHFlowPref = options->dDefault;
    }
    if (body[iFile - 1].dStagLid > 0) { // if StagLid>0 then use staglid
                                        // default.
      body[iFile - 1].dManHFlowPref = HFLOWREDUCTSTAG;
    }
  }
}
/**
  Read dipole magnetic moment coefficient from input file

  @param body Body struct
  @param control Control struct
  @param files Files struct
  @param options Options struct
  @param system System struct
  @param iFile Index of file
*/
void fvReadMagMomCoef(BODY *body, CONTROL *control, FILES *files,
                      OPTIONS *options, SYSTEM *system, int iFile) {
  int lTmp = -1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn, options->cName, &dTmp, &lTmp,
                  control->Io.iVerbose);
  if (lTmp >= 0) { // if line num of option ge 0
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                    control->Io.iVerbose);
    if (dTmp < 0) { // if input value lt 0
      body[iFile - 1].dMagMomCoef =
            dTmp * dNegativeDouble(*options, files->Infile[iFile].cIn,
                                   control->Io.iVerbose);
    } else {
      body[iFile - 1].dMagMomCoef = dTmp; // no units.
    }
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else if (iFile >
             0) { // if line num not ge 0, then if iFile gt 0, then set default.
    body[iFile - 1].dMagMomCoef = options->dDefault;
  }
}
/**
  Read solar wind pressure at body from input file

  @param body Body struct
  @param control Control struct
  @param files Files struct
  @param options Options struct
  @param system System struct
  @param iFile Index of file
*/
void fvReadPresSWind(BODY *body, CONTROL *control, FILES *files,
                     OPTIONS *options, SYSTEM *system, int iFile) {
  int lTmp = -1;
  double dTmp;
  AddOptionDouble(files->Infile[iFile].cIn, options->cName, &dTmp, &lTmp,
                  control->Io.iVerbose);
  if (lTmp >= 0) { // if line num of option ge 0
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                    control->Io.iVerbose);
    if (dTmp < 0) { // if input value lt 0
      body[iFile - 1].dPresSWind =
            dTmp * dNegativeDouble(*options, files->Infile[iFile].cIn,
                                   control->Io.iVerbose);
    } else {
      body[iFile - 1].dPresSWind = dTmp; // no units.
    }
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else if (iFile >
             0) { // if line num not ge 0, then if iFile gt 0, then set default.
    body[iFile - 1].dPresSWind = options->dDefault;
  }
}
/**
  Read minimum mantle temperature to halt from input file

  @param body Body struct
  @param control Control struct
  @param files Files struct
  @param options Options struct
  @param system System struct
  @param iFile Index of file
*/
void fvReadHaltMinTMan(BODY *body, CONTROL *control, FILES *files,
                       OPTIONS *options, SYSTEM *system, int iFile) {
  /* This parameter cannot exist in primary file */
  /* Must verify in conjuction with 235UPower and 235UMass */
  int lTmp = -1;
  double dTmp;
  AddOptionDouble(files->Infile[iFile].cIn, options->cName, &dTmp, &lTmp,
                  control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                    control->Io.iVerbose);
    if (dTmp < 0) {
      control->Halt[iFile - 1].dMinTMan =
            dTmp * dNegativeDouble(*options, files->Infile[iFile].cIn,
                                   control->Io.iVerbose);
    } else {
      control->Halt[iFile - 1].dMinTMan =
            fdUnitsTemp(dTmp, control->Units[iFile].iTemp, 0);
    }

    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else if (iFile > 0) {
    control->Halt[iFile - 1].dMinTMan = options->dDefault;
  }
}
/**
  Read minimum core temperature to halt from input file

  @param body Body struct
  @param control Control struct
  @param files Files struct
  @param options Options struct
  @param system System struct
  @param iFile Index of file
*/
void fvReadHaltMinTCore(BODY *body, CONTROL *control, FILES *files,
                        OPTIONS *options, SYSTEM *system, int iFile) {
  /* This parameter cannot exist in primary file */
  /* Must verify in conjuction with 235UPower and 235UMass */
  int lTmp = -1;
  double dTmp;
  AddOptionDouble(files->Infile[iFile].cIn, options->cName, &dTmp, &lTmp,
                  control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                    control->Io.iVerbose);
    if (dTmp < 0) {
      control->Halt[iFile - 1].dMinTCore =
            dTmp * dNegativeDouble(*options, files->Infile[iFile].cIn,
                                   control->Io.iVerbose);
    } else {
      control->Halt[iFile - 1].dMinTCore =
            fdUnitsTemp(dTmp, control->Units[iFile].iTemp, 0);
    }

    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else if (iFile > 0) {
    control->Halt[iFile - 1].dMinTCore = options->dDefault;
  }
}
/* Begin vemcee parameters */
/**
  Read mantle viscosity activation energy from input file

  @param body Body struct
  @param control Control struct
  @param files Files struct
  @param options Options struct
  @param system System struct
  @param iFile Index of file
*/
void fvReadActViscMan(BODY *body, CONTROL *control, FILES *files,
                      OPTIONS *options, SYSTEM *system, int iFile) {
  int lTmp = -1;
  double dTmp;
  AddOptionDouble(files->Infile[iFile].cIn, options->cName, &dTmp, &lTmp,
                  control->Io.iVerbose);
  if (lTmp >= 0) { // if line num of option ge 0
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                    control->Io.iVerbose);
    if (dTmp < 0) { // if input value lt 0
      body[iFile - 1].dActViscMan =
            dTmp * dNegativeDouble(*options, files->Infile[iFile].cIn,
                                   control->Io.iVerbose);
    } else {
      body[iFile - 1].dActViscMan = dTmp; // no units.
    }
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else if (iFile >
             0) { // if line num not ge 0, then if iFile gt 0, then set default.
    body[iFile - 1].dActViscMan = options->dDefault;
  }
}
/**
  Read mantle reference shear modulus from input file

  @param body Body struct
  @param control Control struct
  @param files Files struct
  @param options Options struct
  @param system System struct
  @param iFile Index of file
*/
void fvReadShModRef(BODY *body, CONTROL *control, FILES *files,
                    OPTIONS *options, SYSTEM *system, int iFile) {
  int lTmp = -1;
  double dTmp;
  AddOptionDouble(files->Infile[iFile].cIn, options->cName, &dTmp, &lTmp,
                  control->Io.iVerbose);
  if (lTmp >= 0) { // if line num of option ge 0
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                    control->Io.iVerbose);
    if (dTmp < 0) { // if input value lt 0
      body[iFile - 1].dShModRef =
            dTmp * dNegativeDouble(*options, files->Infile[iFile].cIn,
                                   control->Io.iVerbose);
    } else {
      body[iFile - 1].dShModRef = dTmp; // no units.
    }
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else if (iFile >
             0) { // if line num not ge 0, then if iFile gt 0, then set default.
    body[iFile - 1].dShModRef = options->dDefault;
  }
}
/**
  Read mantle stiffness from input file

  @param body Body struct
  @param control Control struct
  @param files Files struct
  @param options Options struct
  @param system System struct
  @param iFile Index of file
*/
void fvReadStiffness(BODY *body, CONTROL *control, FILES *files,
                     OPTIONS *options, SYSTEM *system, int iFile) {
  int lTmp = -1;
  double dTmp;
  AddOptionDouble(files->Infile[iFile].cIn, options->cName, &dTmp, &lTmp,
                  control->Io.iVerbose);
  if (lTmp >= 0) { // if line num of option ge 0
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                    control->Io.iVerbose);
    if (dTmp < 0) { // if input value lt 0
      body[iFile - 1].dStiffness =
            dTmp * dNegativeDouble(*options, files->Infile[iFile].cIn,
                                   control->Io.iVerbose);
    } else {
      body[iFile - 1].dStiffness = dTmp; // no units.
    }
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else if (iFile >
             0) { // if line num not ge 0, then if iFile gt 0, then set default.
    body[iFile - 1].dStiffness = options->dDefault;
  }
}
/**
  Read Lindemann law (core liquidus) length scale "D" from input file

  @param body Body struct
  @param control Control struct
  @param files Files struct
  @param options Options struct
  @param system System struct
  @param iFile Index of file
*/
void fvReadDLind(BODY *body, CONTROL *control, FILES *files, OPTIONS *options,
                 SYSTEM *system, int iFile) {
  int lTmp = -1;
  double dTmp;
  AddOptionDouble(files->Infile[iFile].cIn, options->cName, &dTmp, &lTmp,
                  control->Io.iVerbose);
  if (lTmp >= 0) { // if line num of option ge 0
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                    control->Io.iVerbose);
    if (dTmp < 0) { // if input value lt 0
      body[iFile - 1].dDLind =
            dTmp * dNegativeDouble(*options, files->Infile[iFile].cIn,
                                   control->Io.iVerbose);
    } else {
      body[iFile - 1].dDLind = dTmp; // no units.
    }
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else if (iFile >
             0) { // if line num not ge 0, then if iFile gt 0, then set default.
    body[iFile - 1].dDLind = options->dDefault;
  }
}
/**
  Read core adiabatic profile length scale "D" from input file

  @param body Body struct
  @param control Control struct
  @param files Files struct
  @param options Options struct
  @param system System struct
  @param iFile Index of file
*/
void fvReadDAdCore(BODY *body, CONTROL *control, FILES *files, OPTIONS *options,
                   SYSTEM *system, int iFile) {
  int lTmp = -1;
  double dTmp;
  AddOptionDouble(files->Infile[iFile].cIn, options->cName, &dTmp, &lTmp,
                  control->Io.iVerbose);
  if (lTmp >= 0) { // if line num of option ge 0
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                    control->Io.iVerbose);
    if (dTmp < 0) { // if input value lt 0
      body[iFile - 1].dDAdCore =
            dTmp * dNegativeDouble(*options, files->Infile[iFile].cIn,
                                   control->Io.iVerbose);
    } else {
      body[iFile - 1].dDAdCore = dTmp; // no units.
    }
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else if (iFile >
             0) { // if line num not ge 0, then if iFile gt 0, then set default.
    body[iFile - 1].dDAdCore = options->dDefault;
  }
}
/**
  Read adiabatic temperature jump from average mantle to upper mantle from input
  file

  @param body Body struct
  @param control Control struct
  @param files Files struct
  @param options Options struct
  @param system System struct
  @param iFile Index of file
*/
void fvReadAdJumpM2UM(BODY *body, CONTROL *control, FILES *files,
                      OPTIONS *options, SYSTEM *system, int iFile) {
  int lTmp = -1;
  double dTmp;
  AddOptionDouble(files->Infile[iFile].cIn, options->cName, &dTmp, &lTmp,
                  control->Io.iVerbose);
  if (lTmp >= 0) { // if line num of option ge 0
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                    control->Io.iVerbose);
    if (dTmp < 0) { // if input value lt 0
      body[iFile - 1].dAdJumpM2UM =
            dTmp * dNegativeDouble(*options, files->Infile[iFile].cIn,
                                   control->Io.iVerbose);
    } else {
      body[iFile - 1].dAdJumpM2UM = dTmp; // no units.
    }
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else if (iFile >
             0) { // if line num not ge 0, then if iFile gt 0, then set default.
    body[iFile - 1].dAdJumpM2UM = options->dDefault;
  }
}
/**
  Read adiabatic temperature jump from average mantle to lower mantle from input
  file

  @param body Body struct
  @param control Control struct
  @param files Files struct
  @param options Options struct
  @param system System struct
  @param iFile Index of file
*/
void fvReadAdJumpM2LM(BODY *body, CONTROL *control, FILES *files,
                      OPTIONS *options, SYSTEM *system, int iFile) {
  int lTmp = -1;
  double dTmp;
  AddOptionDouble(files->Infile[iFile].cIn, options->cName, &dTmp, &lTmp,
                  control->Io.iVerbose);
  if (lTmp >= 0) { // if line num of option ge 0
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                    control->Io.iVerbose);
    if (dTmp < 0) { // if input value lt 0
      body[iFile - 1].dAdJumpM2LM =
            dTmp * dNegativeDouble(*options, files->Infile[iFile].cIn,
                                   control->Io.iVerbose);
    } else {
      body[iFile - 1].dAdJumpM2LM = dTmp; // no units.
    }
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else if (iFile >
             0) { // if line num not ge 0, then if iFile gt 0, then set default.
    body[iFile - 1].dAdJumpM2LM = options->dDefault;
  }
}
/**
  Read adiabatic temperature jump from average core to core-mantle boundary from
  input file

  @param body Body struct
  @param control Control struct
  @param files Files struct
  @param options Options struct
  @param system System struct
  @param iFile Index of file
*/
void fvReadAdJumpC2CMB(BODY *body, CONTROL *control, FILES *files,
                       OPTIONS *options, SYSTEM *system, int iFile) {
  int lTmp = -1;
  double dTmp;
  AddOptionDouble(files->Infile[iFile].cIn, options->cName, &dTmp, &lTmp,
                  control->Io.iVerbose);
  if (lTmp >= 0) { // if line num of option ge 0
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                    control->Io.iVerbose);
    if (dTmp < 0) { // if input value lt 0
      body[iFile - 1].dAdJumpC2CMB =
            dTmp * dNegativeDouble(*options, files->Infile[iFile].cIn,
                                   control->Io.iVerbose);
    } else {
      body[iFile - 1].dAdJumpC2CMB = dTmp; // no units.
    }
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else if (iFile >
             0) { // if line num not ge 0, then if iFile gt 0, then set default.
    body[iFile - 1].dAdJumpC2CMB = options->dDefault;
  }
}
/**
  Read electrical conductivity at top of core from input file

  @param body Body struct
  @param control Control struct
  @param files Files struct
  @param options Options struct
  @param system System struct
  @param iFile Index of file
*/
void fvReadElecCondCore(BODY *body, CONTROL *control, FILES *files,
                        OPTIONS *options, SYSTEM *system, int iFile) {
  int lTmp = -1;
  double dTmp;
  AddOptionDouble(files->Infile[iFile].cIn, options->cName, &dTmp, &lTmp,
                  control->Io.iVerbose);
  if (lTmp >= 0) { // if line num of option ge 0
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                    control->Io.iVerbose);
    if (dTmp < 0) { // if input value lt 0
      body[iFile - 1].dElecCondCore =
            dTmp * dNegativeDouble(*options, files->Infile[iFile].cIn,
                                   control->Io.iVerbose);
    } else {
      body[iFile - 1].dElecCondCore = dTmp; // no units.
    }
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else if (iFile >
             0) { // if line num not ge 0, then if iFile gt 0, then set default.
    body[iFile - 1].dElecCondCore = options->dDefault;
  }
}

/**
  Read ImK2ManOrbModel from input file

  @param body Body struct
  @param control Control struct
  @param files Files struct
  @param options Options struct
  @param system System struct
  @param iFile Index of file
*/
void fvReadImK2ManOrbModel(BODY *body, CONTROL *control, FILES *files,
                           OPTIONS *options, SYSTEM *system, int iFile) {
  int lTmp = -1;
  double dTmp;
  AddOptionDouble(files->Infile[iFile].cIn, options->cName, &dTmp, &lTmp,
                  control->Io.iVerbose);
  if (lTmp >= 0) { // if line num of option ge 0
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                    control->Io.iVerbose);
    if (dTmp < 0) { // if input value lt 0
      body[iFile - 1].dImK2ManOrbModel =
            dTmp * dNegativeDouble(*options, files->Infile[iFile].cIn,
                                   control->Io.iVerbose);
    } else {
      body[iFile - 1].dImK2ManOrbModel = dTmp; // no units.
    }
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else if (iFile >
             0) { // if line num not ge 0, then if iFile gt 0, then set default.
    body[iFile - 1].dImK2ManOrbModel = options->dDefault;
  }
}

/* End vemcee parameters */


/* Initiatlize Input Options */
/**
  Initialize Thermint options

  @param options Options struct
  @param fnRead fnReadOption functions to read options
*/
void fvInitializeOptionsThermint(OPTIONS *options, fnReadOption fnRead[]) {
  int iOpt, iFile;

  /* TSurf */
  sprintf(options[OPT_TSURF].cName, "dTSurf");
  sprintf(options[OPT_TSURF].cDescr, "Initial Surface Temperature");
  sprintf(options[OPT_TSURF].cDefault, "300 K");
  sprintf(options[OPT_TSURF].cDimension, "temperature");
  options[OPT_TSURF].iType      = 2;
  options[OPT_TSURF].bMultiFile = 1;
  options[OPT_TSURF].dNeg       = 1; // Not sure about this??
  options[OPT_TSURF].dDefault   = TSURF;
  sprintf(options[OPT_TSURF].cNeg, "No negative behavior");
  fnRead[OPT_TSURF] = &fvReadTSurf;

  /* TMan */
  sprintf(options[OPT_TMAN].cName, "dTMan");
  sprintf(options[OPT_TMAN].cDescr, "Initial Mantle Temperature");
  sprintf(options[OPT_TMAN].cDefault, "3000 K");
  sprintf(options[OPT_TMAN].cDimension, "temperature");
  options[OPT_TMAN].iType      = 2;
  options[OPT_TMAN].bMultiFile = 1;
  options[OPT_TMAN].dNeg       = 3000.0; // Not sure about this??
  options[OPT_TMAN].dDefault   = 3000.0;
  sprintf(options[OPT_TMAN].cNeg, "No negative behavior");
  fnRead[OPT_TMAN] = &fvReadTMan;

  /* TCore */
  sprintf(options[OPT_TCORE].cName, "dTCore");
  sprintf(options[OPT_TCORE].cDescr, "Initial Core Temperature");
  sprintf(options[OPT_TCORE].cDefault, "6000 K");
  sprintf(options[OPT_TCORE].cDimension, "temperature");
  options[OPT_TCORE].iType      = 2;
  options[OPT_TCORE].bMultiFile = 1;
  options[OPT_TCORE].dNeg       = 6000.0; // Not sure about this??
  options[OPT_TCORE].dDefault   = 6000.0;
  sprintf(options[OPT_TCORE].cNeg, "No negative behavior");
  fnRead[OPT_TCORE] = &fvReadTCore;

  /* ViscJumpMan */
  sprintf(options[OPT_VISCJUMPMAN].cName, "dViscJumpMan");
  sprintf(options[OPT_VISCJUMPMAN].cDescr, "ViscJumpMan");
  sprintf(options[OPT_VISCJUMPMAN].cDefault, "VISCJUMPMAN");
  sprintf(options[OPT_VISCJUMPMAN].cDimension, "nd");
  options[OPT_VISCJUMPMAN].iType      = 2;
  options[OPT_VISCJUMPMAN].bMultiFile = 1;
  options[OPT_VISCJUMPMAN].dNeg       = VISCJUMPMAN;
  options[OPT_VISCJUMPMAN].dDefault   = VISCJUMPMAN;
  sprintf(options[OPT_VISCJUMPMAN].cNeg, "No negative behavior");
  fnRead[OPT_VISCJUMPMAN] = &fvReadViscJumpMan;
  // Needs a LongDescr
  sprintf(options[OPT_VISCJUMPMAN].cLongDescr,
          "Numerical factor that is used to get the lower mantle thermal "
          "boundary \n"
          "viscosity visc_UMan from the upper mantle viscosity visc_LM by \n"
          "visc_LM=ViscJumpMan*visc_UM.  See eq (163) in Barnes et al (2020).");

  /* ViscRef */
  sprintf(options[OPT_VISCREF].cName, "dViscRef");
  sprintf(options[OPT_VISCREF].cDescr, "ViscRef");
  sprintf(options[OPT_VISCREF].cDefault, "1");
  sprintf(options[OPT_VISCREF].cDimension, "length^2/time");
  options[OPT_VISCREF].iType      = 2;
  options[OPT_VISCREF].bMultiFile = 1;
  options[OPT_VISCREF].dNeg       = VISCREF;
  options[OPT_VISCREF].dDefault   = VISCREF;
  sprintf(options[OPT_VISCREF].cNeg, "No negative behavior");
  fnRead[OPT_VISCREF] = &fvReadViscRef;
  // Needs a LongDescr
  sprintf(options[OPT_VISCREF].cLongDescr,
          "Coefficient in front of viscosity equation \n"
          "see eq (159) in Barnes et al. (2020).");

  /* TrefLind */
  sprintf(options[OPT_TREFLIND].cName, "dTrefLind");
  sprintf(options[OPT_TREFLIND].cDescr, "TrefLind");
  sprintf(options[OPT_TREFLIND].cDefault, "Value in thermint.h");
  sprintf(options[OPT_TREFLIND].cDimension, "temperature");
  options[OPT_TREFLIND].iType      = 2;
  options[OPT_TREFLIND].bMultiFile = 1;
  options[OPT_TREFLIND].dNeg =
        1; // XXX If negative, is really expressed in terms of TREFLIND??
  options[OPT_TREFLIND].dDefault = TREFLIND;
  sprintf(options[OPT_TREFLIND].cNeg, "No negative behavior");
  fnRead[OPT_TREFLIND] = &fvReadTrefLind;
  sprintf(options[OPT_TREFLIND].cLongDescr,
          "Lindemann's law reference temperature, also called T_Fe0 in "
          "Driscoll \n"
          "Bercovici (2015), see their Eq. A23. Default value is set to "
          "TREFLIND,\n"
          "which is defined in thermint.h)");

  /* DTChiRef */
  sprintf(options[OPT_DTCHIREF].cName, "dDTChiRef");
  sprintf(options[OPT_DTCHIREF].cDescr, "DTChiRef");
  sprintf(options[OPT_DTCHIREF].cDefault, "Value in thermint.h");
  sprintf(options[OPT_DTCHIREF].cDimension, "temperature");
  options[OPT_DTCHIREF].iType      = 2;
  options[OPT_DTCHIREF].bMultiFile = 1;
  options[OPT_DTCHIREF].dNeg       = 1;
  options[OPT_DTCHIREF].dDefault   = DTCHIREF;
  sprintf(options[OPT_DTCHIREF].cNeg, "No negative behavior");
  fnRead[OPT_DTCHIREF] = &fvReadDTChiRef;
  // Needs a LongDescr
  sprintf(
        options[OPT_DTCHIREF].cLongDescr,
        "Core liquidus (melting temperature) depression due to the presence \n"
        "of light elements in the core.  See eq (175) in Barnes et al (2020).");

  /* EruptEff */
  sprintf(options[OPT_ERUPTEFF].cName, "dEruptEff");
  sprintf(options[OPT_ERUPTEFF].cDescr, "Melt Eruption Efficiency");
  sprintf(options[OPT_ERUPTEFF].cDefault, "ERUPTEFF");
  sprintf(options[OPT_ERUPTEFF].cDimension, "nd");
  options[OPT_ERUPTEFF].iType      = 2;
  options[OPT_ERUPTEFF].bMultiFile = 1;
  options[OPT_ERUPTEFF].dNeg       = ERUPTEFF; // XXX
  options[OPT_ERUPTEFF].dDefault   = ERUPTEFF;
  sprintf(options[OPT_ERUPTEFF].cNeg, "No negative behavior");
  fnRead[OPT_ERUPTEFF] = &fvReadEruptEff;
  // Needs a LongDescr
  sprintf(options[OPT_ERUPTEFF].cLongDescr,
          "Fraction of heat in mantle melt that escapes to the surface. \n"
          "See eq (187) in Barnes et al (2020).");

  /* ViscMeltPhis */
  sprintf(options[OPT_VISCMELTPHIS].cName, "dViscMeltPhis");
  sprintf(options[OPT_VISCMELTPHIS].cDescr, "Viscosity Melt Factor Phi star");
  sprintf(options[OPT_VISCMELTPHIS].cDefault, "Default is VISCMELTPHIS");
  sprintf(options[OPT_VISCMELTPHIS].cDimension, "nd");
  options[OPT_VISCMELTPHIS].iType      = 2;
  options[OPT_VISCMELTPHIS].bMultiFile = 1;
  options[OPT_VISCMELTPHIS].dNeg       = VISCMELTPHIS; // XXX
  options[OPT_VISCMELTPHIS].dDefault   = VISCMELTPHIS;
  sprintf(options[OPT_VISCMELTPHIS].cNeg, "No negative behavior");
  fnRead[OPT_VISCMELTPHIS] = &fvReadViscMeltPhis;
  sprintf(
        options[OPT_VISCMELTPHIS].cLongDescr,
        "Viscosity-melt reduction coefficient, \"phi*\" in Eq. 8 of Driscoll \n"
        "Bercovici (2015).");

  /* ViscMeltXi */
  sprintf(options[OPT_VISCMELTXI].cName, "dViscMeltXi");
  sprintf(options[OPT_VISCMELTXI].cDescr, "Viscosity Melt Factor Xi");
  sprintf(options[OPT_VISCMELTXI].cDefault, "Default is VISCMELTXI");
  sprintf(options[OPT_VISCMELTXI].cDimension, "nd");
  options[OPT_VISCMELTXI].iType      = 2;
  options[OPT_VISCMELTXI].bMultiFile = 1;
  options[OPT_VISCMELTXI].dNeg       = VISCMELTXI;
  options[OPT_VISCMELTXI].dDefault   = VISCMELTXI;
  sprintf(options[OPT_VISCMELTXI].cNeg, "No negative behavior");
  fnRead[OPT_VISCMELTXI] = &fvReadViscMeltXi;
  // Needs a LongDescr
  sprintf(options[OPT_VISCMELTXI].cLongDescr,
          "Constant used to compute the reduction in mantle viscosity \n"
          "due to the presence of melt.  See eq (162) in Barnes et al (2020) \n"
          "and Costa et al (2009) for values for different materials.");

  /* ViscMeltGamma */
  sprintf(options[OPT_VISCMELTGAMMA].cName, "dViscMeltGamma");
  sprintf(options[OPT_VISCMELTGAMMA].cDescr, "Viscosity Melt Factor Gamma");
  sprintf(options[OPT_VISCMELTGAMMA].cDefault, "Default is VISCMELTGAMMA");
  sprintf(options[OPT_VISCMELTGAMMA].cDimension, "nd");
  options[OPT_VISCMELTGAMMA].iType      = 2;
  options[OPT_VISCMELTGAMMA].bMultiFile = 1;
  options[OPT_VISCMELTGAMMA].dNeg       = VISCMELTGAMMA;
  options[OPT_VISCMELTGAMMA].dDefault   = VISCMELTGAMMA;
  sprintf(options[OPT_VISCMELTGAMMA].cNeg, "No negative behavior");
  fnRead[OPT_VISCMELTGAMMA] = &fvReadViscMeltGamma;
  // Needs a LongDescr
  sprintf(options[OPT_VISCMELTGAMMA].cLongDescr,
          "Constant used to compute the reduction in mantle viscosity \n"
          "due to the presence of melt.  See eq (162) in Barnes et al (2020) \n"
          "and Costa et al (2009) for values for different materials.");

  /* ViscMeltDelta */
  sprintf(options[OPT_VISCMELTDELTA].cName, "dViscMeltDelta");
  sprintf(options[OPT_VISCMELTDELTA].cDescr, "Viscosity Melt Factor Delta");
  sprintf(options[OPT_VISCMELTDELTA].cDefault, "Default is VISCMELTDELTA");
  sprintf(options[OPT_VISCMELTDELTA].cDimension, "nd");
  options[OPT_VISCMELTDELTA].iType      = 2;
  options[OPT_VISCMELTDELTA].bMultiFile = 1;
  options[OPT_VISCMELTDELTA].dNeg       = VISCMELTDELTA;
  options[OPT_VISCMELTDELTA].dDefault   = VISCMELTDELTA;
  sprintf(options[OPT_VISCMELTDELTA].cNeg, "No negative behavior");
  fnRead[OPT_VISCMELTDELTA] = &fvReadViscMeltDelta;
  // Needs a LongDescr
  sprintf(options[OPT_VISCMELTDELTA].cLongDescr,
          "Constant used to compute the reduction in mantle viscosity \n"
          "due to the presence of melt.  See eq (161) in Barnes et al (2020) \n"
          "and Costa et al (2009) for values for different materials.");

  /* ViscMeltB */
  sprintf(options[OPT_VISCMELTB].cName, "dViscMeltB");
  sprintf(options[OPT_VISCMELTB].cDescr, "Viscosity Melt Factor B");
  sprintf(options[OPT_VISCMELTB].cDefault, "Default is VISCMELTB");
  sprintf(options[OPT_VISCMELTB].cDimension, "nd");
  options[OPT_VISCMELTB].iType      = 2;
  options[OPT_VISCMELTB].bMultiFile = 1;
  options[OPT_VISCMELTB].dNeg       = VISCMELTB;
  options[OPT_VISCMELTB].dDefault   = VISCMELTB;
  sprintf(options[OPT_VISCMELTB].cNeg, "No negative behavior");
  fnRead[OPT_VISCMELTB] = &fvReadViscMeltB;
  // Needs a LongDescr
  sprintf(options[OPT_VISCMELTB].cLongDescr,
          "Constant used to compute the reduction in mantle viscosity \n"
          "due to the presence of melt.  See eq (161) in Barnes et al (2020) \n"
          "and Costa et al (2009) for values for different materials.");

  /* MeltfactorLMan XXX Added by Rory -- Check!*/
  sprintf(options[OPT_MELTFACTORLMAN].cName, "dMeltfactorLMan");
  sprintf(options[OPT_MELTFACTORLMAN].cDescr,
          "Lower Mantle Viscosity Melt Factor");
  sprintf(options[OPT_MELTFACTORLMAN].cDefault, "%f", MELTFACTORLMAN);
  sprintf(options[OPT_MELTFACTORLMAN].cDimension, "nd");
  options[OPT_MELTFACTORLMAN].iType      = 2;
  options[OPT_MELTFACTORLMAN].bMultiFile = 1;
  // XXX Rory doesn't think negative options should be allowed here
  // options[OPT_MELTFACTORLMAN].dNeg = MELTFACTORLMAN;
  options[OPT_MELTFACTORLMAN].dDefault = MELTFACTORLMAN;
  // sprintf(options[OPT_MELTFACTORLMAN].cNeg,"Default is MELTFACTORLMAN");
  fnRead[OPT_MELTFACTORLMAN] = &fvReadMeltfactorLMan;
  // Needs a LongDescr
  sprintf(
        options[OPT_MELTFACTORLMAN].cLongDescr,
        "Use to fix the lower mantle viscosity melt factor, defined as \n"
        "epsilon_phase in eq (159,160) of Barnes et al (2020).  Note that \n"
        "this is an option.  Default behavior is to compute this factor from \n"
        "the local temperature.");

  /* MeltfactorUMan */
  sprintf(options[OPT_MELTFACTORUMAN].cName, "dMeltfactorUMan");
  sprintf(options[OPT_MELTFACTORUMAN].cDescr,
          "Upper Mantle Viscosity Melt Factor");
  sprintf(options[OPT_MELTFACTORUMAN].cDefault, "%f", MELTFACTORUMAN);
  sprintf(options[OPT_MELTFACTORUMAN].cDimension, "nd");
  options[OPT_MELTFACTORUMAN].iType      = 2;
  options[OPT_MELTFACTORUMAN].bMultiFile = 1;
  options[OPT_MELTFACTORUMAN].dNeg       = MELTFACTORUMAN;
  options[OPT_MELTFACTORUMAN].dDefault   = MELTFACTORUMAN;
  sprintf(options[OPT_MELTFACTORUMAN].cNeg, "No negative behavior");
  fnRead[OPT_MELTFACTORUMAN] = &fvReadMeltfactorUMan;
  // Needs a LongDescr
  sprintf(
        options[OPT_MELTFACTORUMAN].cLongDescr,
        "Use to fix the upper mantle viscosity melt factor, defined as \n"
        "epsilon_phase in eq (159,160) of Barnes et al (2020).  Note that \n"
        "this is an option.  Default behavior is to compute this factor from \n"
        "the local temperature.");

  /* FixMeltfactorUMan */
  sprintf(options[OPT_FIXMELTFACTORUMAN].cName, "dFixMeltfactorUMan");
  sprintf(options[OPT_FIXMELTFACTORUMAN].cDescr, "Fix Viscosity Melt Factor");
  sprintf(options[OPT_FIXMELTFACTORUMAN].cDefault,
          "Default is FIXMELTFACTORUMAN");
  sprintf(options[OPT_FIXMELTFACTORUMAN].cDimension, "nd");
  options[OPT_FIXMELTFACTORUMAN].iType      = 2;
  options[OPT_FIXMELTFACTORUMAN].bMultiFile = 1;
  options[OPT_FIXMELTFACTORUMAN].dNeg       = FIXMELTFACTORUMAN;
  options[OPT_FIXMELTFACTORUMAN].dDefault   = FIXMELTFACTORUMAN;
  sprintf(options[OPT_FIXMELTFACTORUMAN].cNeg, "No negative behavior");
  fnRead[OPT_FIXMELTFACTORUMAN] = &fvReadFixMeltfactorUMan;
  // Needs a LongDescr
  sprintf(options[OPT_FIXMELTFACTORUMAN].cLongDescr,
          "Boolean: 1 tells the code to used a fixed upper mantle viscosity "
          "melt \n"
          "factor, 0 tells the code to compute it from the local temperature "
          "(default).");

  /* StagLid */
  /* RB: I don't understand this. STAGLID is 0 in thermint.h, so isn't the
    default not to use staglid? I'd also prefer to see this change to
    sThermalMode with options pt, sl, and auto. XXX */
  /* PD(4/23/21): The default value of StagLid is STAGLID=0, which means
     the planet is not in a stagnant lid so a mobile lid cooling model is
     used.  I changed the cDefault and cNeg below.  I don't think there is a
     negative value option here, right?  Should a negative value print a
     warning? */
  sprintf(options[OPT_STAGLID].cName, "dStagLid");
  sprintf(options[OPT_STAGLID].cDescr, "Stagnant Lid Switch");
  sprintf(options[OPT_STAGLID].cDefault, "Default is STAGLID");
  sprintf(options[OPT_STAGLID].cDimension, "nd");
  options[OPT_STAGLID].iType      = 2;
  options[OPT_STAGLID].bMultiFile = 1;
  options[OPT_STAGLID].dNeg       = STAGLID;
  options[OPT_STAGLID].dDefault   = STAGLID;
  sprintf(options[OPT_STAGLID].cNeg, "No negative behavior");
  fnRead[OPT_STAGLID] = &fvReadStagLid;

  /* ManHFlowPref */
  sprintf(options[OPT_MANHFLOWPREF].cName, "dManHFlowPref");
  sprintf(options[OPT_MANHFLOWPREF].cDescr, "Mantle HFlow Prefix");
  sprintf(options[OPT_MANHFLOWPREF].cDefault, "Default is MANHFLOWPREF");
  sprintf(options[OPT_MANHFLOWPREF].cDimension, "nd");
  options[OPT_MANHFLOWPREF].iType      = 2;
  options[OPT_MANHFLOWPREF].bMultiFile = 1;
  options[OPT_MANHFLOWPREF].dNeg       = MANHFLOWPREF;
  options[OPT_MANHFLOWPREF].dDefault   = MANHFLOWPREF;
  sprintf(options[OPT_MANHFLOWPREF].cNeg, "No negative behavior");
  fnRead[OPT_MANHFLOWPREF] = &fvReadManHFlowPref;
  // Needs a LongDescr
  sprintf(options[OPT_MANHFLOWPREF].cLongDescr,
          "Coefficient in front of HflowUMan, value can be anything. \n"
          "If StagLid>0 then ManHFlowPref=HFLOWREDUCTSTAG.  \n"
          "Otherwise default behavior is 1.");

  /* MagMomCoef */
  sprintf(options[OPT_MAGMOMCOEF].cName, "dMagMomCoef");
  sprintf(options[OPT_MAGMOMCOEF].cDescr, "Magnetic Moment Coefficient");
  sprintf(options[OPT_MAGMOMCOEF].cDefault, "Default is MAGMOMCOEF");
  sprintf(options[OPT_MAGMOMCOEF].cDimension, "nd");
  options[OPT_MAGMOMCOEF].iType      = 2;
  options[OPT_MAGMOMCOEF].bMultiFile = 1;
  options[OPT_MAGMOMCOEF].dNeg       = MAGMOMCOEF;
  options[OPT_MAGMOMCOEF].dDefault   = MAGMOMCOEF;
  sprintf(options[OPT_MAGMOMCOEF].cNeg, "No negative behavior");
  fnRead[OPT_MAGMOMCOEF] = &fvReadMagMomCoef;
  // Needs a LongDescr
  sprintf(options[OPT_MAGMOMCOEF].cLongDescr,
          "Coefficient in front of magnetic moment scaling law, \n"
          "gamma_d in eq (192) in Barnes et al (2020).");

  /* PresSWind */
  sprintf(options[OPT_PRESSWIND].cName, "dPresSWind");
  sprintf(options[OPT_PRESSWIND].cDescr, "Stellar Wind Pressure at body");
  sprintf(options[OPT_PRESSWIND].cDefault, "Default is EPRESSWIND");
  sprintf(options[OPT_PRESSWIND].cDimension, "pressure");
  options[OPT_PRESSWIND].iType      = 2;
  options[OPT_PRESSWIND].bMultiFile = 1;
  options[OPT_PRESSWIND].dNeg       = EPRESSWIND;
  options[OPT_PRESSWIND].dDefault   = EPRESSWIND;
  sprintf(options[OPT_PRESSWIND].cNeg, "No negative behavior");
  fnRead[OPT_PRESSWIND] = &fvReadPresSWind;

  /* Halt at Minimum Mantle Temperature */
  sprintf(options[OPT_HALTMINTMAN].cName, "dHaltMinTMan");
  sprintf(options[OPT_HALTMINTMAN].cDescr,
          "Halt at Minimum Mantle Temperature");
  sprintf(options[OPT_HALTMINTMAN].cDefault, "0 K");
  sprintf(options[OPT_HALTMINTMAN].cDimension, "temperature");
  options[OPT_HALTMINTMAN].iType      = 2;
  options[OPT_HALTMINTMAN].bMultiFile = 1;
  options[OPT_HALTMINTMAN].dNeg       = 1;
  options[OPT_HALTMINTMAN].dDefault   = 0;
  sprintf(options[OPT_HALTMINTMAN].cNeg, "No negative behavior");
  fnRead[OPT_HALTMINTMAN] = &fvReadHaltMinTMan;

  /* Halt at Minimum Core Temperature */
  sprintf(options[OPT_HALTMINTCORE].cName, "dHaltMinTCore");
  sprintf(options[OPT_HALTMINTCORE].cDescr, "Halt at Minimum Core Temperature");
  sprintf(options[OPT_HALTMINTCORE].cDefault, "0 K");
  sprintf(options[OPT_HALTMINTCORE].cDimension, "temperature");
  options[OPT_HALTMINTCORE].iType      = 2;
  options[OPT_HALTMINTCORE].bMultiFile = 1;
  options[OPT_HALTMINTCORE].dNeg       = 1;
  options[OPT_HALTMINTCORE].dDefault   = 0;
  sprintf(options[OPT_HALTMINTCORE].cNeg, "No negative behavior");
  fnRead[OPT_HALTMINTCORE] = &fvReadHaltMinTCore;

  /* Begin vemcee parameters */
  /* ActViscMan */
  sprintf(options[OPT_ACTVISCMAN].cName, "dActViscMan");
  sprintf(options[OPT_ACTVISCMAN].cDescr, "Mantle viscosity activation energy");
  sprintf(options[OPT_ACTVISCMAN].cDefault, "Default is ACTVISCMAN");
  sprintf(options[OPT_ACTVISCMAN].cDimension, "pressure");
  options[OPT_ACTVISCMAN].iType      = 2;
  options[OPT_ACTVISCMAN].bMultiFile = 1;
  options[OPT_ACTVISCMAN].dNeg       = ACTVISCMAN;
  options[OPT_ACTVISCMAN].dDefault   = ACTVISCMAN;
  sprintf(options[OPT_ACTVISCMAN].cNeg, "No negative behavior");
  fnRead[OPT_ACTVISCMAN] = &fvReadActViscMan;
  // Needs a LongDescr
  sprintf(options[OPT_ACTVISCMAN].cLongDescr,
          "Mantle viscosity activation energy, E_nu in \n"
          "eq (159) of Barnes et al (2020).  Physically it is\n"
          "a measure of how sensitive viscosity is to temperature.");

  /* ShModRef */
  sprintf(options[OPT_SHMODREF].cName, "dShModRef");
  sprintf(options[OPT_SHMODREF].cDescr,
          "Reference kinematic mantle shear modulus");
  sprintf(options[OPT_SHMODREF].cDefault, "Default is SHMODREF");
  sprintf(options[OPT_SHMODREF].cDimension, "pressure");
  options[OPT_SHMODREF].iType      = 2;
  options[OPT_SHMODREF].bMultiFile = 1;
  options[OPT_SHMODREF].dNeg       = SHMODREF;
  options[OPT_SHMODREF].dDefault   = SHMODREF;
  sprintf(options[OPT_SHMODREF].cNeg, "No negative behavior");
  fnRead[OPT_SHMODREF] = &fvReadShModRef;
  // Needs a LongDescr
  sprintf(options[OPT_SHMODREF].cLongDescr,
          "Reference kinematic mantle shear modulus coefficient, mu_ref\n"
          "in eq (160) of Barnes et al (2020).  Analogous to reference \n"
          "viscosity ViscRef.");

  /* Stiffness */
  sprintf(options[OPT_STIFFNESS].cName, "dStiffness");
  sprintf(options[OPT_STIFFNESS].cDescr, "Effective stiffness of mantle");
  sprintf(options[OPT_STIFFNESS].cDefault, "Default is STIFFNESS");
  sprintf(options[OPT_STIFFNESS].cDimension, "pressure");
  options[OPT_STIFFNESS].iType      = 2;
  options[OPT_STIFFNESS].bMultiFile = 1;
  options[OPT_STIFFNESS].dNeg       = STIFFNESS;
  options[OPT_STIFFNESS].dDefault   = STIFFNESS;
  sprintf(options[OPT_STIFFNESS].cNeg, "No negative behavior");
  fnRead[OPT_STIFFNESS] = &fvReadStiffness;

  /* DLind */
  sprintf(options[OPT_DLIND].cName, "dDLind");
  sprintf(options[OPT_DLIND].cDescr,
          "Lindemann's law length scale for iron liquidus");
  sprintf(options[OPT_DLIND].cDefault, "Default is DLIND");
  sprintf(options[OPT_DLIND].cDimension, "length");
  options[OPT_DLIND].iType      = 2;
  options[OPT_DLIND].bMultiFile = 1;
  options[OPT_DLIND].dNeg       = DLIND;
  options[OPT_DLIND].dDefault   = DLIND;
  sprintf(options[OPT_DLIND].cNeg, "No negative behavior");
  fnRead[OPT_DLIND] = &fvReadDLind;
  // Needs a LongDescr
  sprintf(options[OPT_DLIND].cLongDescr,
          "Length scale for core iron liquidus (Lindemann's law), D_Fe \n"
          "in eq (175) of Barnes et al (2020).  It determines the \n"
          "curvature of the core iron liquidus as a function of radius \n"
          "so that smaller values cause the iron liquidus to drop faster \n"
          "with radius.");

  /* DAdCore */
  sprintf(options[OPT_DADCORE].cName, "dDAdCore");
  sprintf(options[OPT_DADCORE].cDescr, "Liq iron core adiabatic length scale");
  sprintf(options[OPT_DADCORE].cDefault, "Default is DADCORE");
  sprintf(options[OPT_DADCORE].cDimension, "length");
  options[OPT_DADCORE].iType      = 2;
  options[OPT_DADCORE].bMultiFile = 1;
  options[OPT_DADCORE].dNeg       = DADCORE;
  options[OPT_DADCORE].dDefault   = DADCORE;
  sprintf(options[OPT_DADCORE].cNeg, "No negative behavior");
  fnRead[OPT_DADCORE] = &fvReadDAdCore;
  // Needs a LongDescr
  sprintf(options[OPT_DADCORE].cLongDescr,
          "Length scale for core iron adiabatic temperature profile, D_N \n"
          "in eq (174) of Barnes et al (2020).  It determines the \n"
          "curvature of the core iron adiabat as a function of radius \n"
          "so that smaller values cause the core adiabat to drop faster \n"
          "with radius.");

  /* AdJumpM2UM */
  sprintf(options[OPT_ADJUMPM2UM].cName, "dAdJumpM2UM");
  sprintf(options[OPT_ADJUMPM2UM].cDescr,
          "Adiabatic temp jump from ave mantle to UM");
  sprintf(options[OPT_ADJUMPM2UM].cDefault, "Default is ADJUMPM2UM");
  sprintf(options[OPT_ADJUMPM2UM].cDimension, "temperature");
  options[OPT_ADJUMPM2UM].iType      = 2;
  options[OPT_ADJUMPM2UM].bMultiFile = 1;
  options[OPT_ADJUMPM2UM].dNeg       = ADJUMPM2UM;
  options[OPT_ADJUMPM2UM].dDefault   = ADJUMPM2UM;
  sprintf(options[OPT_ADJUMPM2UM].cNeg, "No negative behavior");
  fnRead[OPT_ADJUMPM2UM] = &fvReadAdJumpM2UM;
  // Needs a LongDescr
  sprintf(options[OPT_ADJUMPM2UM].cLongDescr,
          "Constant used to relate TUMan to TMan (TUMan=AdJumpM2UM*TMan),\n"
          "eta_UM below eq (8) in Driscoll & Bercovici (2014).");

  /* AdJumpM2LM */
  sprintf(options[OPT_ADJUMPM2LM].cName, "dAdJumpM2LM");
  sprintf(options[OPT_ADJUMPM2LM].cDescr,
          "Adiabatic temp jump from ave mantle to LM");
  sprintf(options[OPT_ADJUMPM2LM].cDefault, "Default is ADJUMPM2LM");
  sprintf(options[OPT_ADJUMPM2LM].cDimension, "nd");
  options[OPT_ADJUMPM2LM].iType      = 2;
  options[OPT_ADJUMPM2LM].bMultiFile = 1;
  options[OPT_ADJUMPM2LM].dNeg       = ADJUMPM2LM;
  options[OPT_ADJUMPM2LM].dDefault   = ADJUMPM2LM;
  sprintf(options[OPT_ADJUMPM2LM].cNeg, "No negative behavior");
  fnRead[OPT_ADJUMPM2LM] = &fvReadAdJumpM2LM;
  // Needs a LongDescr
  sprintf(options[OPT_ADJUMPM2LM].cLongDescr,
          "Constant used to relate TLMan to TMan (TLMan=AdJumpM2LM*TMan),\n"
          "analogous to AdJumpM2UM.");

  /* AdJumpC2CMB */
  sprintf(options[OPT_ADJUMPC2CMB].cName, "dAdJumpC2CMB");
  sprintf(options[OPT_ADJUMPC2CMB].cDescr,
          "Adiabatic temp jump from ave core to CMB");
  sprintf(options[OPT_ADJUMPC2CMB].cDefault, "Default is ADJUMPC2CMB");
  sprintf(options[OPT_ADJUMPC2CMB].cDimension, "nd");
  options[OPT_ADJUMPC2CMB].iType      = 2;
  options[OPT_ADJUMPC2CMB].bMultiFile = 1;
  options[OPT_ADJUMPC2CMB].dNeg       = ADJUMPC2CMB;
  options[OPT_ADJUMPC2CMB].dDefault   = ADJUMPC2CMB;
  sprintf(options[OPT_ADJUMPC2CMB].cNeg, "No negative behavior");
  fnRead[OPT_ADJUMPC2CMB] = &fvReadAdJumpC2CMB;

  /* ElecCondCore */
  sprintf(options[OPT_ELECCONDCORE].cName, "dElecCondCore");
  sprintf(options[OPT_ELECCONDCORE].cDescr, "Electrical conductivity of core");
  sprintf(options[OPT_ELECCONDCORE].cDefault, "Default is ELECCONDCORE");
  sprintf(options[OPT_ELECCONDCORE].cDimension, "time^3*ampere^2/mass/length");
  options[OPT_ELECCONDCORE].iType      = 2;
  options[OPT_ELECCONDCORE].bMultiFile = 1;
  options[OPT_ELECCONDCORE].dNeg       = ELECCONDCORE;
  options[OPT_ELECCONDCORE].dDefault   = ELECCONDCORE;
  sprintf(options[OPT_ELECCONDCORE].cNeg, "No negative behavior");
  fnRead[OPT_ELECCONDCORE] = &fvReadElecCondCore;

  /* ImK2ManOrbModel */
  sprintf(options[OPT_IMK2MANORBMODEL].cName, "dImK2ManOrbModel");
  sprintf(options[OPT_IMK2MANORBMODEL].cDescr,
          "ImK2Man model to use in orbital equations (1=ImK2(T), 2=k2/Q(T)");
  sprintf(options[OPT_IMK2MANORBMODEL].cDefault, "Default is IMK2MANORBMODEL");
  sprintf(options[OPT_IMK2MANORBMODEL].cDimension, "nd");
  options[OPT_IMK2MANORBMODEL].iType      = 2;
  options[OPT_IMK2MANORBMODEL].bMultiFile = 1;
  options[OPT_IMK2MANORBMODEL].dNeg       = IMK2MANORBMODEL;
  options[OPT_IMK2MANORBMODEL].dDefault   = IMK2MANORBMODEL;
  sprintf(options[OPT_IMK2MANORBMODEL].cNeg, "No negative behavior");
  fnRead[OPT_IMK2MANORBMODEL] = &fvReadImK2ManOrbModel;
  // Needs a LongDescr -- XXX I'm not sure what this does! RB
  // PD(4/26/21): I don't know what this does either!  Presumably its a switch
  // to use different tidal heating models?
}
/**
  Read options in thermint

  @param body Body struct
  @param control Control struct
  @param files Files struct
  @param options Options struct
  @param system System struct
  @param fnRead fnReadOption functions to read options
  @param iBody Index of body
*/
void fvReadOptionsThermint(BODY *body, CONTROL *control, FILES *files,
                           OPTIONS *options, SYSTEM *system,
                           fnReadOption fnRead[], int iBody) {
  int iOpt;

  for (iOpt = OPTSTARTTHERMINT; iOpt < OPTENDTHERMINT; iOpt++) {
    if (options[iOpt].iType != -1) {
      fnRead[iOpt](body, control, files, &options[iOpt], system, iBody + 1);
    }
  }
}

/******************* Verify THERMINT *****************/

/**
  Verify assigning of TMan to body

  @param body Body struct
  @param options Options struct
  @param dAge Age
  @param iBody Index of body
*/
/* Should this function be deleted? */
void fvAssignTMan(BODY *body, OPTIONS *options, double dAge, int iBody) {
  /* Mantle */
  /*    if (options[OPT_TMAN].iLine[iBody+1] >= 0) {
        body[iBody].d40KNumMan=body[iBody].d40KMassMan/(MASS40K);
        printf("40KMassMan set, body[iBody].d40KNumMan=%e,
    ENUMMAN40K=%e\n",body[iBody].d40KNumMan,ENUMMAN40K);
    }
    body[iBody].dTMan = fd40KConstant(body[iBody].d40KNumMan,dAge);  //Get the
    constant given num and age.
    */
}
/**
  Verify TMan. In update struct, iaType=1 for differential equation evolution,
  iNumBodies=1, iaBody is number of bodies affected by this variable (1),
  pdTDotMan points towards derivative, fnUpdate points towards derivative
  (fdTDotMan).

  @param body Body struct
  @param options Options struct
  @param system System struct
  @param update Update struct
  @param dAge Age
  @param iBody Index of body
*/
void fvVerifyTMan(BODY *body, OPTIONS *options, SYSTEM *system, UPDATE *update,
                  double dAge, int iBody) {

  //  AssignTMan(body,options,dAge,iBody);
  /* Mantle */
  update[iBody].iaType[update[iBody].iTMan][0] =
        1; // iaType=0 for prescribed evolution, =1 for differential evolution
           // (normal)
  update[iBody].iNumBodies[update[iBody].iTMan][0] = 1;
  update[iBody].iaBody[update[iBody].iTMan][0] =
        malloc(update[iBody].iNumBodies[update[iBody].iTMan][0] *
               sizeof(int)); // iaBody is the number of bodies that are affected
                             // by this variable.
  update[iBody].iaBody[update[iBody].iTMan][0][0] = iBody;
  update[iBody].pdTDotMan = &update[iBody].daDerivProc[update[iBody].iTMan][0];

  // Initialize some values that dDTManDt depends on
  update[iBody].daDerivProc[update[iBody].iTMan][0] = 0;
}
/**
  Verify TCore. In update struct, iaType=1 for differential equation evolution,
  iNumBodies=1, iaBody is number of bodies affected by this variable (1),
  pdTDotCore points towards derivative, fnUpdate points towards derivative
  (fdTDotCore).  Initialize parameters that dDTCoreDt depends on in body struct:
  dRIC, dHfluxCMBConv, dRICDot, dGravICB, dCoreBuoyTherm, dCoreBuoyCompo,
  update. daDerivProc.

  @param body Body struct
  @param options Options struct
  @param system System struct
  @param update Update struct
  @param dAge Age
  @param iBody Index of body
*/
void fvVerifyTCore(BODY *body, OPTIONS *options, SYSTEM *system, UPDATE *update,
                   double dAge, int iBody) {
  //  AssignTCore(body,options,dAge,iBody);
  /* Core */
  update[iBody].iaType[update[iBody].iTCore][0] =
        1; // iaType=0 for prescribed evolution, =1 for differential evolution
           // (normal)
  update[iBody].iNumBodies[update[iBody].iTCore][0] = 1;
  update[iBody].iaBody[update[iBody].iTCore][0] =
        malloc(update[iBody].iNumBodies[update[iBody].iTCore][0] *
               sizeof(int)); // iaBody is the number of bodies that are affected
                             // by this variable.
  update[iBody].iaBody[update[iBody].iTCore][0][0] = iBody;
  update[iBody].pdTDotCore =
        &update[iBody].daDerivProc[update[iBody].iTCore][0];

  // Initialize parameters that dDTCoreDt depends on
  body[iBody].dRIC           = 0;
  body[iBody].dHfluxCMBConv  = 0;
  body[iBody].dRICDot        = 0;
  body[iBody].dGravICB       = fdGravICB(body, iBody);
  body[iBody].dCoreBuoyTherm = fdCoreBuoyTherm(body, iBody);
  body[iBody].dCoreBuoyCompo = fdCoreBuoyCompo(body, iBody);
  update[iBody].daDerivProc[update[iBody].iTCore][0] = 0;
}

/******************  AUX PROPS  ***************************/
/**
  Set auxiliary properties in body.  This includes all thermint parameters that
  are not computed by a time derivative (that is everything except TMan and
  TCore).

  @param body Body struct
  @param evolve Evolve struct
  @param update Update struct
  @param iBody Index of body
*/

void fvPropsAuxThermint(BODY *body, EVOLVE *evolve, IO *io, UPDATE *update,
                        int iBody) {
  /* Scalar Properties */
  body[iBody].dTUMan         = fdTUMan(body, iBody);
  body[iBody].dTLMan         = fdTLMan(body, iBody);
  body[iBody].dTCMB          = fdTCMB(body, iBody);
  body[iBody].dTJumpUMan     = fdTJumpUMan(body, iBody);
  body[iBody].dTJumpLMan     = fdTJumpLMan(body, iBody);
  body[iBody].dSignTJumpUMan = fdSignTJumpUMan(body, iBody);
  body[iBody].dSignTJumpLMan = fdSignTJumpLMan(body, iBody);
  body[iBody].dViscUManArr   = fdViscUManArr(body, iBody);
  body[iBody].dViscUMan      = fdViscUMan(body, iBody);
  body[iBody].dViscLMan      = fdViscLMan(body, iBody);
  //  body[iBody].dViscJumpMan=fdViscJumpMan(body,iBody);
  if (body[iBody].dMeltfactorUMan == 0) {
    body[iBody].dMeltfactorUMan =
          1.; // initialize to avoid fvvisc=visc/meltfactor crash.
  }
  if (body[iBody].dMeltfactorLMan == 0) {
    body[iBody].dMeltfactorLMan =
          1.; // initialize to avoid fvvisc=visc/meltfactor crash.
  }
  /* Loop through melt calculation once to get dependence of visc on melt. */
  int i = 0, nloop = 2;
  for (i = 0; i < nloop; i++) {
    body[iBody].dBLUMan         = fdBLUMan(body, iBody);
    body[iBody].dBLLMan         = fdBLLMan(body, iBody);
    body[iBody].dTsolUMan       = fdTsolUMan(body, iBody);
    body[iBody].dTliqUMan       = fdTliqUMan(body, iBody);
    body[iBody].dTsolLMan       = fdTsolLMan(body, iBody);
    body[iBody].dTliqLMan       = fdTliqLMan(body, iBody);
    body[iBody].dFMeltUMan      = fdFMeltUMan(body, iBody);
    body[iBody].dFMeltLMan      = fdFMeltLMan(body, iBody);
    body[iBody].dMeltfactorUMan = fdMeltfactorUMan(body, iBody);
    body[iBody].dMeltfactorLMan = fdMeltfactorLMan(body, iBody);
    body[iBody].dViscUManArr    = fdViscUManArr(body, iBody);
    body[iBody].dViscUMan       = fdViscUMan(body, iBody);
    body[iBody].dViscLMan       = fdViscLMan(body, iBody);
    body[iBody].dShmodUMan      = fdShmodUMan(body, iBody);

    // printf("%d TUMan=%.4f BLUMan=%.5e TsolUMan=%.4f FMeltUMan=%.4f
    // MeltfactorUMan=%e ViscUMan=%e ShmodUMan=%e ImK2=%e
    // TidalPowMan=%e\n",i,body[iBody].dTUMan,body[iBody].dBLUMan,body[iBody].dTsolUMan,body[iBody].dFMeltUMan,body[iBody].dMeltfactorUMan,body[iBody].dViscUMan,body[iBody].dShmodUMan,body[iBody].dImK2,body[iBody].dTidalPowMan);
  }
  body[iBody].dDepthMeltMan    = fdDepthMeltMan(body, iBody);
  body[iBody].dTDepthMeltMan   = fdTDepthMeltMan(body, iBody);
  body[iBody].dTJumpMeltMan    = fdTJumpMeltMan(body, iBody);
  body[iBody].dMeltMassFluxMan = fdMeltMassFluxMan(body, iBody);
  body[iBody].dViscMMan        = fdViscMMan(body, iBody);
  body[iBody].dRayleighMan     = fdRayleighMan(body, iBody);
  body[iBody].dDynamViscos     = fdDynamicViscosity(body, iBody);

  /* Heat Flows */
  /* Mantle */
  body[iBody].dHfluxUMan      = fdHfluxUMan(body, iBody);
  body[iBody].dHfluxLMan      = fdHfluxLMan(body, iBody);
  body[iBody].dHfluxCMB       = fdHfluxCMB(body, iBody);
  body[iBody].dHflowUMan      = fdHflowUMan(body, iBody);
  body[iBody].dHflowLMan      = fdHflowLMan(body, iBody);
  body[iBody].dHflowCMB       = fdHflowCMB(body, iBody);
  body[iBody].dHflowLatentMan = fdHflowLatentMan(body, update, iBody);
  body[iBody].dHflowMeltMan   = fdHflowMeltMan(body, iBody);
  body[iBody].dHflowSecMan    = fdHflowSecMan(body, evolve, iBody);
  body[iBody].dHflowSurf      = fdHflowSurf(body, iBody);

  /* Core */
  /* Iterate on Core chemistry before R_ICB */
  // The order matters here!
  body[iBody].dMassIC = fdMassIC(body, iBody);
  body[iBody].dMassOC = fdMassOC(body, iBody);
  //  body[iBody].dMassChiOC=fdMassChiOC(body,iBody);
  //  body[iBody].dMassChiIC=fdMassChiIC(body,iBody);
  body[iBody].dChiOC          = fdChiOC(body, iBody);
  body[iBody].dChiIC          = fdChiIC(body, iBody);
  body[iBody].dMassChiOC      = fdMassChiOC(body, iBody);
  body[iBody].dMassChiIC      = fdMassChiIC(body, iBody);
  body[iBody].dDTChi          = fdDTChi(body, iBody);
  body[iBody].dRIC            = fdRIC(body, iBody);
  body[iBody].dDRICDTCMB      = fdDRICDTCMB(body, iBody);
  body[iBody].dMassICDot      = fdMassICDot(body, update, iBody);
  body[iBody].dHflowLatentIC  = fdHflowLatentIC(body, update, iBody);
  body[iBody].dPowerGravIC    = fdPowerGravIC(body, update, iBody);
  body[iBody].dThermConductOC = fdThermConductOC(body, iBody);
  body[iBody].dHfluxCMBAd     = fdHfluxCMBAd(body, iBody);
  body[iBody].dHfluxCMBConv   = fdHfluxCMBConv(body, iBody);
  body[iBody].dRICDot         = fdRICDot(body, update, iBody);
  body[iBody].dGravICB        = fdGravICB(body, iBody);
  body[iBody].dCoreBuoyTherm  = fdCoreBuoyTherm(body, iBody);
  body[iBody].dCoreBuoyCompo  = fdCoreBuoyCompo(body, iBody);
  body[iBody].dCoreBuoyTotal  = fdCoreBuoyTotal(body, iBody);
  body[iBody].dMagMom         = fdMagMom(body, iBody);
  body[iBody].dPresSWind      = fdPresSWind(body, iBody);
  body[iBody].dMagPauseRad    = fdMagPauseRad(body, iBody);
}
/**
  Enforce limits to fundamental parameters: if TMan or TCore < 0.5 then set it
  to 0 (i.e. removes negatives).

  @param body Body struct
  @param evolve Evolve struct
  @param io IO struct
  @param system System struct
  @param update Update struct
  @param fnUpdate fnUpdateVariable pointer to function to update
  @param iBody Index of body
  @param iModule Index of module
*/
void fvForceBehaviorThermint(BODY *body, MODULE *module, EVOLVE *evolve, IO *io,
                             SYSTEM *system, UPDATE *update,
                             fnUpdateVariable ***fnUpdate, int iBody,
                             int iModule) {
  // XXX Reset fnUpdate functions to SetDerivTiny?
  if (body[iBody].dTMan < 0.5) {
    body[iBody].dTMan = 0;
  }
  if (body[iBody].dTCore < 0.5) {
    body[iBody].dTCore = 0;
  }
}

void fvAssignThermintDerivatives(BODY *body, EVOLVE *evolve, UPDATE *update,
                                 fnUpdateVariable ***fnUpdate, int iBody) {
  fnUpdate[iBody][update[iBody].iTMan][0]  = &fdTDotMan;
  fnUpdate[iBody][update[iBody].iTCore][0] = &fdTDotCore;
}

void fvNullThermintDerivatives(BODY *body, EVOLVE *evolve, UPDATE *update,
                               fnUpdateVariable ***fnUpdate, int iBody) {
  fnUpdate[iBody][update[iBody].iTMan][0]  = &fndUpdateFunctionTiny;
  fnUpdate[iBody][update[iBody].iTCore][0] = &fndUpdateFunctionTiny;
}

/**
  Verify thermint. Calls VerifyTMan, VerifyTCore, ForceBehaviorThermint,
  PropsAuxThermint, BodyCopyThermint.

  @param body Body struct
  @param control Control struct
  @param files Files struct
  @param options Options struct
  @param output Output struct
  @param system System struct
  @param update Update struct
  @param iBody Index of body
  @param iModule Index of module
*/
void fvVerifyThermint(BODY *body, CONTROL *control, FILES *files,
                      OPTIONS *options, OUTPUT *output, SYSTEM *system,
                      UPDATE *update, int iBody, int iModule) {

  body[iBody].bMantle = 1;

  fvVerifyTMan(body, options, system, update, body[iBody].dAge,
               iBody); // Verify Man.
  fvVerifyTCore(body, options, system, update, body[iBody].dAge,
                iBody); // Verify Core.

  control->fnForceBehavior[iBody][iModule]   = &fvForceBehaviorThermint;
  control->fnPropsAux[iBody][iModule]        = &fvPropsAuxThermint;
  control->Evolve.fnBodyCopy[iBody][iModule] = &fvBodyCopyThermint;
  //  output[OUT_TDOTMAN].fnOutput[iBody][iModule] = &fdTDotMan;
}

/**************** THERMINT update ****************/
/**
  Initialize update

  @param body Body struct
  @param update Update struct
  @param iBody Index of body
*/
void fvInitializeUpdateThermint(BODY *body, UPDATE *update, int iBody) {
  /* Initially allow all radiogenic heat sources to be present. If any are 0,
     or < dMinRadPower, they will me removed from update[iBody] in
     ForceBehavior.
  */
  if (body[iBody].dTMan > 0) {
    update[iBody].iNumVars++;
    update[iBody].iNumTMan++; // Why is iNumTMan incremented here and below?
  }
  if (body[iBody].dTCore > 0) {
    update[iBody].iNumVars++;
    update[iBody].iNumTCore++;
  }
}
/**
  Finalize update of TMan

  @param body Body struct
  @param update Update struct
  @param iEqn Index of equation to update
  @param iVar Index of variable to update
  @param iBody Index of body
  @param iFoo
*/
void fvFinalizeUpdateTManThermint(BODY *body, UPDATE *update, int *iEqn,
                                  int iVar, int iBody, int iFoo) {
  update[iBody].iaModule[iVar][*iEqn] = THERMINT;
  update[iBody].iNumTMan              = (*iEqn)++;
}
/**
  Finalize update of TCore

  @param body Body struct
  @param update Update struct
  @param iEqn Index of equation to update
  @param iVar Index of variable to update
  @param iBody Index of body
  @param iFoo
*/
void fvFinalizeUpdateTCoreThermint(BODY *body, UPDATE *update, int *iEqn,
                                   int iVar, int iBody, int iFoo) {
  update[iBody].iaModule[iVar][*iEqn] = THERMINT;
  update[iBody].iNumTCore             = (*iEqn)++;
}


/***************** RADHEAT Halts *****************/
/**
  Halt at minimum value of TMan

  @param body Body struct
  @param evolve Evolve struct
  @param halt Halt struct
  @param io IO struct
  @param update Update struct
  @param iBody Index of body
*/
/* Minimum TMan */
int fbHaltMinTMan(BODY *body, EVOLVE *evolve, HALT *halt, IO *io,
                  UPDATE *update, fnUpdateVariable ***fnUpdate, int iBody) {
  if (body[iBody].dTMan < halt->dMinTMan) {
    if (io->iVerbose >= VERBPROG) {
      printf("HALT: %s's TMan =  ", body[iBody].cName);
      fprintd(stdout, body[iBody].dTMan, io->iSciNot, io->iDigits);
      printf(" < ");
      fprintd(stdout, halt->dMinTMan, io->iSciNot, io->iDigits);
      printf(".\n");
    }
    return 1;
  }
  return 0;
}
/**
  Halt at minimum value of TCore

  @param body Body struct
  @param evolve Evolve struct
  @param halt Halt struct
  @param io IO struct
  @param update Update struct
  @param iBody Index of body
*/
int fbHaltMinTCore(BODY *body, EVOLVE *evolve, HALT *halt, IO *io,
                   UPDATE *update, fnUpdateVariable ***fnUpdate, int iBody) {
  if (body[iBody].dTCore < halt->dMinTCore) {
    if (io->iVerbose >= VERBPROG) {
      printf("HALT: %s's TCore =  ", body[iBody].cName);
      fprintd(stdout, body[iBody].dTCore, io->iSciNot, io->iDigits);
      printf(" < ");
      fprintd(stdout, halt->dMinTCore, io->iSciNot, io->iDigits);
      printf(".\n");
    }
    return 1;
  }
  return 0;
}

// XXX Should change these to bHaltTMan as there is no real need to check

/**
  Count number of halts

  @param halt Halt struct
  @param iNumHalts Index of number of halts
*/
void fvCountHaltsThermint(HALT *halt, int *iNumHalts) {
  if (halt->dMinTMan >= 0) {
    (*iNumHalts)++;
  }
  if (halt->dMinTCore >= 0) {
    (*iNumHalts)++;
  }
}
/**
  Verify halts.

  @param body Body struct
  @param control Control struct
  @param options Options struct
  @param iBody Index of body
  @param iHalt Index of halt
*/
void fvVerifyHaltThermint(BODY *body, CONTROL *control, OPTIONS *options,
                          int iBody, int *iHalt) {
  if (control->Halt[iBody].dMinTMan >= 0) {
    control->fnHalt[iBody][(*iHalt)++] = &fbHaltMinTMan;
  }
  if (control->Halt[iBody].dMinTCore >= 0) {
    control->fnHalt[iBody][(*iHalt)++] = &fbHaltMinTCore;
  }
}

/************* THERMINT Outputs ******************/

/* NOTE: If you write a new Write subroutine here you need to add the associate
   block of initialization in InitializeOutputThermint below */
/**
  Write TMan output

  @param body Body struct
  @param control Control struct
  @param output Output struct
  @param system System struct
  @param units Units struct
  @param update Update struct
  @param iBody Index of body
  @param dTmp Temporary variable
  @param cUnit Variable units
*/
void fvWriteTMan(BODY *body, CONTROL *control, OUTPUT *output, SYSTEM *system,
                 UNITS *units, UPDATE *update, int iBody, double *dTmp,
                 char cUnit[]) {
  /* Get TMan */
  *dTmp = body[iBody].dTMan;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit, output->cNeg);
  } else {
    /*      *dTmp /= fdUnitsTemp(body[iBody].dTman,0,units->iTemp);  //set
    "iOldType" to 0, second input var, arbitarily.
    fsUnitsTemp(units->iTemp,cUnit);
      */
  }
}
/**
  Write TUMan output

  @param body Body struct
  @param control Control struct
  @param output Output struct
  @param system System struct
  @param units Units struct
  @param update Update struct
  @param iBody Index of body
  @param dTmp Temporary variable
  @param cUnit Variable units
*/
void fvWriteTUMan(BODY *body, CONTROL *control, OUTPUT *output, SYSTEM *system,
                  UNITS *units, UPDATE *update, int iBody, double *dTmp,
                  char cUnit[]) {
  /* Get TUMan */
  *dTmp = body[iBody].dTUMan;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit, output->cNeg);
  } else {
  }
}
/**
  Write TLMan output

  @param body Body struct
  @param control Control struct
  @param output Output struct
  @param system System struct
  @param units Units struct
  @param update Update struct
  @param iBody Index of body
  @param dTmp Temporary variable
  @param cUnit Variable units
*/
void fvWriteTLMan(BODY *body, CONTROL *control, OUTPUT *output, SYSTEM *system,
                  UNITS *units, UPDATE *update, int iBody, double *dTmp,
                  char cUnit[]) {
  /* Get TLMan */
  *dTmp = body[iBody].dTLMan;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit, output->cNeg);
  } else {
  }
}
/**
  Write TJumpUMan output

  @param body Body struct
  @param control Control struct
  @param output Output struct
  @param system System struct
  @param units Units struct
  @param update Update struct
  @param iBody Index of body
  @param dTmp Temporary variable
  @param cUnit Variable units
*/
void fvWriteTJumpUMan(BODY *body, CONTROL *control, OUTPUT *output,
                      SYSTEM *system, UNITS *units, UPDATE *update, int iBody,
                      double *dTmp, char cUnit[]) {
  /* Get TLMan */
  *dTmp = body[iBody].dTJumpUMan;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit, output->cNeg);
  } else {
  }
}
/**
  Write TJumpLMan output

  @param body Body struct
  @param control Control struct
  @param output Output struct
  @param system System struct
  @param units Units struct
  @param update Update struct
  @param iBody Index of body
  @param dTmp Temporary variable
  @param cUnit Variable units
*/
void fvWriteTJumpLMan(BODY *body, CONTROL *control, OUTPUT *output,
                      SYSTEM *system, UNITS *units, UPDATE *update, int iBody,
                      double *dTmp, char cUnit[]) {
  /* Get TLMan */
  *dTmp = body[iBody].dTJumpLMan;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit, output->cNeg);
  } else {
  }
}
/**
  Write Sign of TJumpUMan output

  @param body Body struct
  @param control Control struct
  @param output Output struct
  @param system System struct
  @param units Units struct
  @param update Update struct
  @param iBody Index of body
  @param dTmp Temporary variable
  @param cUnit Variable units
*/
void fvWriteSignTJumpUMan(BODY *body, CONTROL *control, OUTPUT *output,
                          SYSTEM *system, UNITS *units, UPDATE *update,
                          int iBody, double *dTmp, char cUnit[]) {
  /* Get TLMan */
  *dTmp = body[iBody].dSignTJumpUMan;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit, output->cNeg);
  } else {
  }
}
/**
  Write Sign of TJumpUMan output

  @param body Body struct
  @param control Control struct
  @param output Output struct
  @param system System struct
  @param units Units struct
  @param update Update struct
  @param iBody Index of body
  @param dTmp Temporary variable
  @param cUnit Variable units
*/
void fvWriteSignTJumpLMan(BODY *body, CONTROL *control, OUTPUT *output,
                          SYSTEM *system, UNITS *units, UPDATE *update,
                          int iBody, double *dTmp, char cUnit[]) {
  /* Get TLMan */
  *dTmp = body[iBody].dSignTJumpLMan;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit, output->cNeg);
  } else {
  }
}
/**
  Write TCMB output

  @param body Body struct
  @param control Control struct
  @param output Output struct
  @param system System struct
  @param units Units struct
  @param update Update struct
  @param iBody Index of body
  @param dTmp Temporary variable
  @param cUnit Variable units
*/
void fvWriteTCMB(BODY *body, CONTROL *control, OUTPUT *output, SYSTEM *system,
                 UNITS *units, UPDATE *update, int iBody, double *dTmp,
                 char cUnit[]) {
  /* Get TCore */
  *dTmp = body[iBody].dTCMB;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit, output->cNeg);
  } else {
  }
}
/**
  Write TCore output

  @param body Body struct
  @param control Control struct
  @param output Output struct
  @param system System struct
  @param units Units struct
  @param update Update struct
  @param iBody Index of body
  @param dTmp Temporary variable
  @param cUnit Variable units
*/
void fvWriteTCore(BODY *body, CONTROL *control, OUTPUT *output, SYSTEM *system,
                  UNITS *units, UPDATE *update, int iBody, double *dTmp,
                  char cUnit[]) {
  /* Get TCore */
  *dTmp = body[iBody].dTCore;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit, output->cNeg);
  } else {
  }
}
/**
  Write viscosity UMan from Arrhenius law output

  @param body Body struct
  @param control Control struct
  @param output Output struct
  @param system System struct
  @param units Units struct
  @param update Update struct
  @param iBody Index of body
  @param dTmp Temporary variable
  @param cUnit Variable units
*/
void fvWriteViscUManArr(BODY *body, CONTROL *control, OUTPUT *output,
                        SYSTEM *system, UNITS *units, UPDATE *update, int iBody,
                        double *dTmp, char cUnit[]) {
  /* Get ViscUManArr */
  *dTmp = body[iBody].dViscUManArr;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit, output->cNeg);
  } else {
    fsUnitsViscosity(units, cUnit);
    *dTmp /= fdUnitsTime(units->iTime) /
             (fdUnitsLength(units->iLength) * fdUnitsLength(units->iLength));
  }
}
/**
  Write viscosity UMan output

  @param body Body struct
  @param control Control struct
  @param output Output struct
  @param system System struct
  @param units Units struct
  @param update Update struct
  @param iBody Index of body
  @param dTmp Temporary variable
  @param cUnit Variable units
*/
void fvWriteViscUMan(BODY *body, CONTROL *control, OUTPUT *output,
                     SYSTEM *system, UNITS *units, UPDATE *update, int iBody,
                     double *dTmp, char cUnit[]) {
  *dTmp = body[iBody].dViscUMan;

  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit, output->cNeg);
  } else {
    fsUnitsViscosity(units, cUnit);
    *dTmp /= fdUnitsTime(units->iTime) /
             (fdUnitsLength(units->iLength) * fdUnitsLength(units->iLength));
  }
}
/**
  Write dynamic mantle viscosity output

  @param body Body struct
  @param control Control struct
  @param output Output struct
  @param system System struct
  @param units Units struct
  @param update Update struct
  @param iBody Index of body
  @param dTmp Temporary variable
  @param cUnit Variable units
*/
void fvWriteDynamicViscosity(BODY *body, CONTROL *control, OUTPUT *output,
                             SYSTEM *system, UNITS *units, UPDATE *update,
                             int iBody, double *dTmp, char cUnit[]) {
  *dTmp = fdDynamicViscosity(body, iBody);

  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit, output->cNeg);
  } else {
    fsUnitsEnergy(units, cUnit);
    *dTmp /= fdUnitsEnergy(units->iTime, units->iMass, units->iLength);
  }
}
/**
  Write viscosity LMan output

  @param body Body struct
  @param control Control struct
  @param output Output struct
  @param system System struct
  @param units Units struct
  @param update Update struct
  @param iBody Index of body
  @param dTmp Temporary variable
  @param cUnit Variable units
*/
void fvWriteViscLMan(BODY *body, CONTROL *control, OUTPUT *output,
                     SYSTEM *system, UNITS *units, UPDATE *update, int iBody,
                     double *dTmp, char cUnit[]) {
  *dTmp = body[iBody].dViscLMan;

  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit, output->cNeg);
  } else {
    fsUnitsViscosity(units, cUnit);
    *dTmp /= fdUnitsTime(units->iTime) /
             (fdUnitsLength(units->iLength) * fdUnitsLength(units->iLength));
  }
}
/**
  Write viscosity MMan output

  @param body Body struct
  @param control Control struct
  @param output Output struct
  @param system System struct
  @param units Units struct
  @param update Update struct
  @param iBody Index of body
  @param dTmp Temporary variable
  @param cUnit Variable units
*/
void fvWriteViscMMan(BODY *body, CONTROL *control, OUTPUT *output,
                     SYSTEM *system, UNITS *units, UPDATE *update, int iBody,
                     double *dTmp, char cUnit[]) {
  *dTmp = body[iBody].dViscMMan;

  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit, output->cNeg);
  } else {
    fsUnitsViscosity(units, cUnit);
    *dTmp /= fdUnitsTime(units->iTime) /
             (fdUnitsLength(units->iLength) * fdUnitsLength(units->iLength));
  }
}
/**
  Write viscosity jump across mantle output

  @param body Body struct
  @param control Control struct
  @param output Output struct
  @param system System struct
  @param units Units struct
  @param update Update struct
  @param iBody Index of body
  @param dTmp Temporary variable
  @param cUnit Variable units
*/
void fvWriteViscJumpMan(BODY *body, CONTROL *control, OUTPUT *output,
                        SYSTEM *system, UNITS *units, UPDATE *update, int iBody,
                        double *dTmp, char cUnit[]) {
  /* Get TLMan */
  *dTmp = body[iBody].dViscJumpMan;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit, output->cNeg);
  } else {
  }
}
/**
  Write UMan boundary layer thickness output

  @param body Body struct
  @param control Control struct
  @param output Output struct
  @param system System struct
  @param units Units struct
  @param update Update struct
  @param iBody Index of body
  @param dTmp Temporary variable
  @param cUnit Variable units
*/
void fvWriteBLUMan(BODY *body, CONTROL *control, OUTPUT *output, SYSTEM *system,
                   UNITS *units, UPDATE *update, int iBody, double *dTmp,
                   char cUnit[]) {
  *dTmp = body[iBody].dBLUMan;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit, output->cNeg);
  } else {
  }
}
/**
  Write LMan boundary layer thickness output

  @param body Body struct
  @param control Control struct
  @param output Output struct
  @param system System struct
  @param units Units struct
  @param update Update struct
  @param iBody Index of body
  @param dTmp Temporary variable
  @param cUnit Variable units
*/
void fvWriteBLLMan(BODY *body, CONTROL *control, OUTPUT *output, SYSTEM *system,
                   UNITS *units, UPDATE *update, int iBody, double *dTmp,
                   char cUnit[]) {
  *dTmp = body[iBody].dBLLMan;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit, output->cNeg);
  } else {
  }
}
/**
  Write UMan shear modulus output

  @param body Body struct
  @param control Control struct
  @param output Output struct
  @param system System struct
  @param units Units struct
  @param update Update struct
  @param iBody Index of body
  @param dTmp Temporary variable
  @param cUnit Variable units
*/
void fvWriteShmodUMan(BODY *body, CONTROL *control, OUTPUT *output,
                      SYSTEM *system, UNITS *units, UPDATE *update, int iBody,
                      double *dTmp, char cUnit[]) {
  *dTmp = body[iBody].dShmodUMan;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit, output->cNeg);
  } else {
  }
}
/**
  Write UMan melt fraction output

  @param body Body struct
  @param control Control struct
  @param output Output struct
  @param system System struct
  @param units Units struct
  @param update Update struct
  @param iBody Index of body
  @param dTmp Temporary variable
  @param cUnit Variable units
*/
void fvWriteFMeltUMan(BODY *body, CONTROL *control, OUTPUT *output,
                      SYSTEM *system, UNITS *units, UPDATE *update, int iBody,
                      double *dTmp, char cUnit[]) {
  *dTmp = body[iBody].dFMeltUMan;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit, output->cNeg);
  } else {
  }
}
/**
  Write LMan melt fraction output

  @param body Body struct
  @param control Control struct
  @param output Output struct
  @param system System struct
  @param units Units struct
  @param update Update struct
  @param iBody Index of body
  @param dTmp Temporary variable
  @param cUnit Variable units
*/
void fvWriteFMeltLMan(BODY *body, CONTROL *control, OUTPUT *output,
                      SYSTEM *system, UNITS *units, UPDATE *update, int iBody,
                      double *dTmp, char cUnit[]) {
  *dTmp = body[iBody].dFMeltLMan;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit, output->cNeg);
  } else {
  }
}
/**
  Write UMan viscosity melt reduction output

  @param body Body struct
  @param control Control struct
  @param output Output struct
  @param system System struct
  @param units Units struct
  @param update Update struct
  @param iBody Index of body
  @param dTmp Temporary variable
  @param cUnit Variable units
*/
void fvWriteMeltfactorUMan(BODY *body, CONTROL *control, OUTPUT *output,
                           SYSTEM *system, UNITS *units, UPDATE *update,
                           int iBody, double *dTmp, char cUnit[]) {
  *dTmp = body[iBody].dMeltfactorUMan;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit, output->cNeg);
  } else {
  }
}
/**
  Write LMan viscosity melt reduction output

  @param body Body struct
  @param control Control struct
  @param output Output struct
  @param system System struct
  @param units Units struct
  @param update Update struct
  @param iBody Index of body
  @param dTmp Temporary variable
  @param cUnit Variable units
*/
void fvWriteMeltfactorLMan(BODY *body, CONTROL *control, OUTPUT *output,
                           SYSTEM *system, UNITS *units, UPDATE *update,
                           int iBody, double *dTmp, char cUnit[]) {
  *dTmp = body[iBody].dMeltfactorLMan;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit, output->cNeg);
  } else {
  }
}
/**
  Write depth of mantle melting output

  @param body Body struct
  @param control Control struct
  @param output Output struct
  @param system System struct
  @param units Units struct
  @param update Update struct
  @param iBody Index of body
  @param dTmp Temporary variable
  @param cUnit Variable units
*/
void fvWriteDepthMeltMan(BODY *body, CONTROL *control, OUTPUT *output,
                         SYSTEM *system, UNITS *units, UPDATE *update,
                         int iBody, double *dTmp, char cUnit[]) {
  *dTmp = body[iBody].dDepthMeltMan;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit, output->cNeg);
  } else {
  }
}
/**
  Write T at depth of mantle melting output

  @param body Body struct
  @param control Control struct
  @param output Output struct
  @param system System struct
  @param units Units struct
  @param update Update struct
  @param iBody Index of body
  @param dTmp Temporary variable
  @param cUnit Variable units
*/
void fvWriteTDepthMeltMan(BODY *body, CONTROL *control, OUTPUT *output,
                          SYSTEM *system, UNITS *units, UPDATE *update,
                          int iBody, double *dTmp, char cUnit[]) {
  *dTmp = body[iBody].dTDepthMeltMan;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit, output->cNeg);
  } else {
  }
}
/**
  Write TJump across depth of mantle melting output

  @param body Body struct
  @param control Control struct
  @param output Output struct
  @param system System struct
  @param units Units struct
  @param update Update struct
  @param iBody Index of body
  @param dTmp Temporary variable
  @param cUnit Variable units
*/
void fvWriteTJumpMeltMan(BODY *body, CONTROL *control, OUTPUT *output,
                         SYSTEM *system, UNITS *units, UPDATE *update,
                         int iBody, double *dTmp, char cUnit[]) {
  *dTmp = body[iBody].dTJumpMeltMan;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit, output->cNeg);
  } else {
  }
}
/**
  Write mass flux of mantle melt output

  @param body Body struct
  @param control Control struct
  @param output Output struct
  @param system System struct
  @param units Units struct
  @param update Update struct
  @param iBody Index of body
  @param dTmp Temporary variable
  @param cUnit Variable units
*/
void fvWriteMeltMassFluxMan(BODY *body, CONTROL *control, OUTPUT *output,
                            SYSTEM *system, UNITS *units, UPDATE *update,
                            int iBody, double *dTmp, char cUnit[]) {
  *dTmp = body[iBody].dMeltMassFluxMan;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit, output->cNeg);
  } else {
  }
}
/**
  Write mantle Rayleigh number output

  @param body Body struct
  @param control Control struct
  @param output Output struct
  @param system System struct
  @param units Units struct
  @param update Update struct
  @param iBody Index of body
  @param dTmp Temporary variable
  @param cUnit Variable units
*/
void fvWriteRayleighMan(BODY *body, CONTROL *control, OUTPUT *output,
                        SYSTEM *system, UNITS *units, UPDATE *update, int iBody,
                        double *dTmp, char cUnit[]) {
  *dTmp = body[iBody].dRayleighMan;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit, output->cNeg);
  } else {
  }
}
/**
  Write mantle melt eruption efficiency output

  @param body Body struct
  @param control Control struct
  @param output Output struct
  @param system System struct
  @param units Units struct
  @param update Update struct
  @param iBody Index of body
  @param dTmp Temporary variable
  @param cUnit Variable units
*/
void fvWriteEruptEff(BODY *body, CONTROL *control, OUTPUT *output,
                     SYSTEM *system, UNITS *units, UPDATE *update, int iBody,
                     double *dTmp, char cUnit[]) {
  *dTmp = body[iBody].dEruptEff;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit, output->cNeg);
  } else {
  }
}

/**
  Write inner core radius RIC output

  @param body Body struct
  @param control Control struct
  @param output Output struct
  @param system System struct
  @param units Units struct
  @param update Update struct
  @param iBody Index of body
  @param dTmp Temporary variable
  @param cUnit Variable units
*/
void fvWriteRIC(BODY *body, CONTROL *control, OUTPUT *output, SYSTEM *system,
                UNITS *units, UPDATE *update, int iBody, double *dTmp,
                char cUnit[]) {
  *dTmp = body[iBody].dRIC;

  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit, output->cNeg);
  } else {
  }
}
/**
  Write derivative of RIC wrt TCMB output

  @param body Body struct
  @param control Control struct
  @param output Output struct
  @param system System struct
  @param units Units struct
  @param update Update struct
  @param iBody Index of body
  @param dTmp Temporary variable
  @param cUnit Variable units
*/
void fvWriteDRICDTCMB(BODY *body, CONTROL *control, OUTPUT *output,
                      SYSTEM *system, UNITS *units, UPDATE *update, int iBody,
                      double *dTmp, char cUnit[]) {
  *dTmp = body[iBody].dDRICDTCMB;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit, output->cNeg);
  } else {
  }
}
/**
  Write outer core "OC" light element concentration "chi" output

  @param body Body struct
  @param control Control struct
  @param output Output struct
  @param system System struct
  @param units Units struct
  @param update Update struct
  @param iBody Index of body
  @param dTmp Temporary variable
  @param cUnit Variable units
*/
void fvWriteChiOC(BODY *body, CONTROL *control, OUTPUT *output, SYSTEM *system,
                  UNITS *units, UPDATE *update, int iBody, double *dTmp,
                  char cUnit[]) {
  *dTmp = body[iBody].dChiOC;
  //  *dTmp = body[iBody].dRIC;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit, output->cNeg);
  } else {
  }
}
/**
  Write inner core "IC" light element concentration "chi" output

  @param body Body struct
  @param control Control struct
  @param output Output struct
  @param system System struct
  @param units Units struct
  @param update Update struct
  @param iBody Index of body
  @param dTmp Temporary variable
  @param cUnit Variable units
*/
void fvWriteChiIC(BODY *body, CONTROL *control, OUTPUT *output, SYSTEM *system,
                  UNITS *units, UPDATE *update, int iBody, double *dTmp,
                  char cUnit[]) {
  *dTmp = body[iBody].dChiIC;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit, output->cNeg);
  } else {
  }
}
/**
  Write OC mass output

  @param body Body struct
  @param control Control struct
  @param output Output struct
  @param system System struct
  @param units Units struct
  @param update Update struct
  @param iBody Index of body
  @param dTmp Temporary variable
  @param cUnit Variable units
*/
void fvWriteMassOC(BODY *body, CONTROL *control, OUTPUT *output, SYSTEM *system,
                   UNITS *units, UPDATE *update, int iBody, double *dTmp,
                   char cUnit[]) {
  *dTmp = body[iBody].dMassOC;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit, output->cNeg);
  } else {
  }
}
/**
  Write IC mass output

  @param body Body struct
  @param control Control struct
  @param output Output struct
  @param system System struct
  @param units Units struct
  @param update Update struct
  @param iBody Index of body
  @param dTmp Temporary variable
  @param cUnit Variable units
*/
void fvWriteMassIC(BODY *body, CONTROL *control, OUTPUT *output, SYSTEM *system,
                   UNITS *units, UPDATE *update, int iBody, double *dTmp,
                   char cUnit[]) {
  *dTmp = body[iBody].dMassIC;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit, output->cNeg);
  } else {
  }
}
/**
  Write OC light element mass output

  @param body Body struct
  @param control Control struct
  @param output Output struct
  @param system System struct
  @param units Units struct
  @param update Update struct
  @param iBody Index of body
  @param dTmp Temporary variable
  @param cUnit Variable units
*/
void fvWriteMassChiOC(BODY *body, CONTROL *control, OUTPUT *output,
                      SYSTEM *system, UNITS *units, UPDATE *update, int iBody,
                      double *dTmp, char cUnit[]) {
  *dTmp = body[iBody].dMassChiOC;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit, output->cNeg);
  } else {
  }
}
/**
  Write IC light element mass output

  @param body Body struct
  @param control Control struct
  @param output Output struct
  @param system System struct
  @param units Units struct
  @param update Update struct
  @param iBody Index of body
  @param dTmp Temporary variable
  @param cUnit Variable units
*/
void fvWriteMassChiIC(BODY *body, CONTROL *control, OUTPUT *output,
                      SYSTEM *system, UNITS *units, UPDATE *update, int iBody,
                      double *dTmp, char cUnit[]) {
  *dTmp = body[iBody].dMassChiIC;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit, output->cNeg);
  } else {
  }
}
/**
  Write core liquidus depression due to light elements output

  @param body Body struct
  @param control Control struct
  @param output Output struct
  @param system System struct
  @param units Units struct
  @param update Update struct
  @param iBody Index of body
  @param dTmp Temporary variable
  @param cUnit Variable units
*/
void fvWriteDTChi(BODY *body, CONTROL *control, OUTPUT *output, SYSTEM *system,
                  UNITS *units, UPDATE *update, int iBody, double *dTmp,
                  char cUnit[]) {
  *dTmp = body[iBody].dDTChi;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit, output->cNeg);
  } else {
  }
}
/**
  Write thermal conductivity at top of OC output

  @param body Body struct
  @param control Control struct
  @param output Output struct
  @param system System struct
  @param units Units struct
  @param update Update struct
  @param iBody Index of body
  @param dTmp Temporary variable
  @param cUnit Variable units
*/
void fvWriteThermConductOC(BODY *body, CONTROL *control, OUTPUT *output,
                           SYSTEM *system, UNITS *units, UPDATE *update,
                           int iBody, double *dTmp, char cUnit[]) {
  *dTmp = body[iBody].dThermConductOC;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit, output->cNeg);
  } else {
  }
}
/**
  Write OC thermal buoyancy flux output

  @param body Body struct
  @param control Control struct
  @param output Output struct
  @param system System struct
  @param units Units struct
  @param update Update struct
  @param iBody Index of body
  @param dTmp Temporary variable
  @param cUnit Variable units
*/
void fvWriteCoreBuoyTherm(BODY *body, CONTROL *control, OUTPUT *output,
                          SYSTEM *system, UNITS *units, UPDATE *update,
                          int iBody, double *dTmp, char cUnit[]) {
  *dTmp = body[iBody].dCoreBuoyTherm;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit, output->cNeg);
  } else {
  }
}
/**
  Write OC compositional buoyancy flux output

  @param body Body struct
  @param control Control struct
  @param output Output struct
  @param system System struct
  @param units Units struct
  @param update Update struct
  @param iBody Index of body
  @param dTmp Temporary variable
  @param cUnit Variable units
*/
void fvWriteCoreBuoyCompo(BODY *body, CONTROL *control, OUTPUT *output,
                          SYSTEM *system, UNITS *units, UPDATE *update,
                          int iBody, double *dTmp, char cUnit[]) {
  *dTmp = body[iBody].dCoreBuoyCompo;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit, output->cNeg);
  } else {
  }
}
/**
  Write OC total buoyancy flux output

  @param body Body struct
  @param control Control struct
  @param output Output struct
  @param system System struct
  @param units Units struct
  @param update Update struct
  @param iBody Index of body
  @param dTmp Temporary variable
  @param cUnit Variable units
*/
void fvWriteCoreBuoyTotal(BODY *body, CONTROL *control, OUTPUT *output,
                          SYSTEM *system, UNITS *units, UPDATE *update,
                          int iBody, double *dTmp, char cUnit[]) {
  *dTmp = body[iBody].dCoreBuoyTotal;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit, output->cNeg);
  } else {
  }
}
/**
  Write inner core boundary (ICB) gravitational energy release output

  @param body Body struct
  @param control Control struct
  @param output Output struct
  @param system System struct
  @param units Units struct
  @param update Update struct
  @param iBody Index of body
  @param dTmp Temporary variable
  @param cUnit Variable units
*/
void fvWriteGravICB(BODY *body, CONTROL *control, OUTPUT *output,
                    SYSTEM *system, UNITS *units, UPDATE *update, int iBody,
                    double *dTmp, char cUnit[]) {
  *dTmp = body[iBody].dGravICB;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit, output->cNeg);
  } else {
  }
}
/**
  Write OC dipolar magnetic moment output

  @param body Body struct
  @param control Control struct
  @param output Output struct
  @param system System struct
  @param units Units struct
  @param update Update struct
  @param iBody Index of body
  @param dTmp Temporary variable
  @param cUnit Variable units
*/
void fvWriteMagMom(BODY *body, CONTROL *control, OUTPUT *output, SYSTEM *system,
                   UNITS *units, UPDATE *update, int iBody, double *dTmp,
                   char cUnit[]) {
  *dTmp = body[iBody].dMagMom;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit, output->cNeg);
  } else {
  }
}
/**
  Write RIC growth rate output

  @param body Body struct
  @param control Control struct
  @param output Output struct
  @param system System struct
  @param units Units struct
  @param update Update struct
  @param iBody Index of body
  @param dTmp Temporary variable
  @param cUnit Variable units
*/
void fvWriteRICDot(BODY *body, CONTROL *control, OUTPUT *output, SYSTEM *system,
                   UNITS *units, UPDATE *update, int iBody, double *dTmp,
                   char cUnit[]) {
  *dTmp = body[iBody].dRICDot;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit, output->cNeg);
  } else {
  }
}
/**
  Write solar wind pressure at planet output

  @param body Body struct
  @param control Control struct
  @param output Output struct
  @param system System struct
  @param units Units struct
  @param update Update struct
  @param iBody Index of body
  @param dTmp Temporary variable
  @param cUnit Variable units
*/
void fvWritePresSWind(BODY *body, CONTROL *control, OUTPUT *output,
                      SYSTEM *system, UNITS *units, UPDATE *update, int iBody,
                      double *dTmp, char cUnit[]) {
  *dTmp = body[iBody].dPresSWind;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit, output->cNeg);
  } else {
  }
}
/**
  Write magnetopause radius output

  @param body Body struct
  @param control Control struct
  @param output Output struct
  @param system System struct
  @param units Units struct
  @param update Update struct
  @param iBody Index of body
  @param dTmp Temporary variable
  @param cUnit Variable units
*/
void fvWriteMagPauseRad(BODY *body, CONTROL *control, OUTPUT *output,
                        SYSTEM *system, UNITS *units, UPDATE *update, int iBody,
                        double *dTmp, char cUnit[]) {
  *dTmp = body[iBody].dMagPauseRad;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit, output->cNeg);
  } else {
  }
}

/* Heat Flows/Fluxes */
/**
  Write heat flux across UMan output

  @param body Body struct
  @param control Control struct
  @param output Output struct
  @param system System struct
  @param units Units struct
  @param update Update struct
  @param iBody Index of body
  @param dTmp Temporary variable
  @param cUnit Variable units
*/
void fvWriteHfluxUMan(BODY *body, CONTROL *control, OUTPUT *output,
                      SYSTEM *system, UNITS *units, UPDATE *update, int iBody,
                      double *dTmp, char cUnit[]) {
  *dTmp = body[iBody].dHfluxUMan;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit, output->cNeg);
  } else {
  }
}
/**
  Write heat flux across LMan (same as CMB) output

  @param body Body struct
  @param control Control struct
  @param output Output struct
  @param system System struct
  @param units Units struct
  @param update Update struct
  @param iBody Index of body
  @param dTmp Temporary variable
  @param cUnit Variable units
*/
void fvWriteHfluxLMan(BODY *body, CONTROL *control, OUTPUT *output,
                      SYSTEM *system, UNITS *units, UPDATE *update, int iBody,
                      double *dTmp, char cUnit[]) {
  *dTmp = body[iBody].dHfluxLMan;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit, output->cNeg);
  } else {
  }
}
/**
  Write heat flux across CMB (same as LMan) output

  @param body Body struct
  @param control Control struct
  @param output Output struct
  @param system System struct
  @param units Units struct
  @param update Update struct
  @param iBody Index of body
  @param dTmp Temporary variable
  @param cUnit Variable units
*/
void fvWriteHfluxCMB(BODY *body, CONTROL *control, OUTPUT *output,
                     SYSTEM *system, UNITS *units, UPDATE *update, int iBody,
                     double *dTmp, char cUnit[]) {
  *dTmp = body[iBody].dHfluxCMB;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit, output->cNeg);
  } else {
  }
}
/**
  Write adiabatic heat flux at top of OC output

  @param body Body struct
  @param control Control struct
  @param output Output struct
  @param system System struct
  @param units Units struct
  @param update Update struct
  @param iBody Index of body
  @param dTmp Temporary variable
  @param cUnit Variable units
*/
void fvWriteHfluxCMBAd(BODY *body, CONTROL *control, OUTPUT *output,
                       SYSTEM *system, UNITS *units, UPDATE *update, int iBody,
                       double *dTmp, char cUnit[]) {
  *dTmp = body[iBody].dHfluxCMBAd;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit, output->cNeg);
  } else {
  }
}
/**
  Write convective heat flux at top of OC output

  @param body Body struct
  @param control Control struct
  @param output Output struct
  @param system System struct
  @param units Units struct
  @param update Update struct
  @param iBody Index of body
  @param dTmp Temporary variable
  @param cUnit Variable units
*/
void fvWriteHfluxCMBConv(BODY *body, CONTROL *control, OUTPUT *output,
                         SYSTEM *system, UNITS *units, UPDATE *update,
                         int iBody, double *dTmp, char cUnit[]) {
  *dTmp = body[iBody].dHfluxCMBConv;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit, output->cNeg);
  } else {
  }
}
/**
  Write heat flow across UMan output

  @param body Body struct
  @param control Control struct
  @param output Output struct
  @param system System struct
  @param units Units struct
  @param update Update struct
  @param iBody Index of body
  @param dTmp Temporary variable
  @param cUnit Variable units
*/
void fvWriteHflowUMan(BODY *body, CONTROL *control, OUTPUT *output,
                      SYSTEM *system, UNITS *units, UPDATE *update, int iBody,
                      double *dTmp, char cUnit[]) {
  *dTmp = body[iBody].dHflowUMan;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit, output->cNeg);
  } else {
  }
}
/**
  Write heat flow across surface of body output

  @param body Body struct
  @param control Control struct
  @param output Output struct
  @param system System struct
  @param units Units struct
  @param update Update struct
  @param iBody Index of body
  @param dTmp Temporary variable
  @param cUnit Variable units
*/
void fvWriteHflowSurf(BODY *body, CONTROL *control, OUTPUT *output,
                      SYSTEM *system, UNITS *units, UPDATE *update, int iBody,
                      double *dTmp, char cUnit[]) {
  *dTmp = body[iBody].dHflowSurf;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit, output->cNeg);
  } else {
  }
}
/**
  Write heat flow across LMan output

  @param body Body struct
  @param control Control struct
  @param output Output struct
  @param system System struct
  @param units Units struct
  @param update Update struct
  @param iBody Index of body
  @param dTmp Temporary variable
  @param cUnit Variable units
*/
void fvWriteHflowLMan(BODY *body, CONTROL *control, OUTPUT *output,
                      SYSTEM *system, UNITS *units, UPDATE *update, int iBody,
                      double *dTmp, char cUnit[]) {
  *dTmp = body[iBody].dHflowLMan;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit, output->cNeg);
  } else {
  }
}
/**
  Write heat flow across CMB output

  @param body Body struct
  @param control Control struct
  @param output Output struct
  @param system System struct
  @param units Units struct
  @param update Update struct
  @param iBody Index of body
  @param dTmp Temporary variable
  @param cUnit Variable units
*/
void fvWriteHflowCMB(BODY *body, CONTROL *control, OUTPUT *output,
                     SYSTEM *system, UNITS *units, UPDATE *update, int iBody,
                     double *dTmp, char cUnit[]) {
  *dTmp = body[iBody].dHflowCMB;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit, output->cNeg);
  } else {
  }
}
/**
  Write latent heat flow from mantle solidification output

  @param body Body struct
  @param control Control struct
  @param output Output struct
  @param system System struct
  @param units Units struct
  @param update Update struct
  @param iBody Index of body
  @param dTmp Temporary variable
  @param cUnit Variable units
*/
void fvWriteHflowLatentMan(BODY *body, CONTROL *control, OUTPUT *output,
                           SYSTEM *system, UNITS *units, UPDATE *update,
                           int iBody, double *dTmp, char cUnit[]) {
  *dTmp = body[iBody].dHflowLatentMan;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit, output->cNeg);
  } else {
  }
}
/**
  Write erupted melt heat flow from UMan output

  @param body Body struct
  @param control Control struct
  @param output Output struct
  @param system System struct
  @param units Units struct
  @param update Update struct
  @param iBody Index of body
  @param dTmp Temporary variable
  @param cUnit Variable units
*/
void fvWriteHflowMeltMan(BODY *body, CONTROL *control, OUTPUT *output,
                         SYSTEM *system, UNITS *units, UPDATE *update,
                         int iBody, double *dTmp, char cUnit[]) {
  *dTmp = body[iBody].dHflowMeltMan;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit, output->cNeg);
  } else {
  }
}
/**
  Write mantle secular cooling heat flow output

  @param body Body struct
  @param control Control struct
  @param output Output struct
  @param system System struct
  @param units Units struct
  @param update Update struct
  @param iBody Index of body
  @param dTmp Temporary variable
  @param cUnit Variable units
*/
void fvWriteHflowSecMan(BODY *body, CONTROL *control, OUTPUT *output,
                        SYSTEM *system, UNITS *units, UPDATE *update, int iBody,
                        double *dTmp, char cUnit[]) {
  *dTmp = body[iBody].dHflowSecMan;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit, output->cNeg);
  } else {
  }
}

/**
  Write latent heat flow from ICB output

  @param body Body struct
  @param control Control struct
  @param output Output struct
  @param system System struct
  @param units Units struct
  @param update Update struct
  @param iBody Index of body
  @param dTmp Temporary variable
  @param cUnit Variable units
*/
void fvWriteHflowLatentIC(BODY *body, CONTROL *control, OUTPUT *output,
                          SYSTEM *system, UNITS *units, UPDATE *update,
                          int iBody, double *dTmp, char cUnit[]) {
  *dTmp = body[iBody].dHflowLatentIC;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit, output->cNeg);
  } else {
  }
}
/**
  Write gravitational power from ICB output

  @param body Body struct
  @param control Control struct
  @param output Output struct
  @param system System struct
  @param units Units struct
  @param update Update struct
  @param iBody Index of body
  @param dTmp Temporary variable
  @param cUnit Variable units
*/
void fvWritePowerGravIC(BODY *body, CONTROL *control, OUTPUT *output,
                        SYSTEM *system, UNITS *units, UPDATE *update, int iBody,
                        double *dTmp, char cUnit[]) {
  *dTmp = body[iBody].dPowerGravIC;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit, output->cNeg);
  } else {
  }
}
/**
  Write mantle temperature cooling rate output

  @param body Body struct
  @param control Control struct
  @param output Output struct
  @param system System struct
  @param units Units struct
  @param update Update struct
  @param iBody Index of body
  @param dTmp Temporary variable
  @param cUnit Variable units
*/
void fvWriteTDotMan(BODY *body, CONTROL *control, OUTPUT *output,
                    SYSTEM *system, UNITS *units, UPDATE *update, int iBody,
                    double *dTmp, char cUnit[]) {
  /* Get TDotMan */
  *dTmp = (*(update[iBody].pdTDotMan));
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit, output->cNeg);
  } else {
    /*
      *dTmp /=
      fdUnitsTemp(body[iBody].dTman,0,units->iTemp)/fdUnitsTime(units->iTime);
      fsUnitsTempRate(units->iTemp,cUnit);  // only handles Temp/s, need to add
      yr and Gyr options.
      */
  }
}
/**
  Write core temperature cooling rate output

  @param body Body struct
  @param control Control struct
  @param output Output struct
  @param system System struct
  @param units Units struct
  @param update Update struct
  @param iBody Index of body
  @param dTmp Temporary variable
  @param cUnit Variable units
*/
void fvWriteTDotCore(BODY *body, CONTROL *control, OUTPUT *output,
                     SYSTEM *system, UNITS *units, UPDATE *update, int iBody,
                     double *dTmp, char cUnit[]) {
  /* Get TDotCore */
  *dTmp = (*(update[iBody].pdTDotCore));
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit, output->cNeg);
  } else {
    /*
      *dTmp /=
      fdUnitsTemp(body[iBody].dTman,0,units->iTemp)/fdUnitsTime(units->iTime);
      fsUnitsTempRate(units->iTemp,cUnit);  // only handles Temp/s, need to add
      yr and Gyr options.
      */
  }
}
/** Write Constants. */
/**
  Write core liquidus Lindemann reference temperature output

  @param body Body struct
  @param control Control struct
  @param output Output struct
  @param system System struct
  @param units Units struct
  @param update Update struct
  @param iBody Index of body
  @param dTmp Temporary variable
  @param cUnit Variable units
*/
void fvWriteTrefLind(BODY *body, CONTROL *control, OUTPUT *output,
                     SYSTEM *system, UNITS *units, UPDATE *update, int iBody,
                     double *dTmp, char cUnit[]) {
  *dTmp = body[iBody].dTrefLind;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit, output->cNeg);
  } else {
  }
}

/**
  Initialize output, set variable names, descriptions, default units, default
  scalings, and write functions.

  @param output Output struct
  @param fnWrite fnWriteOutput
*/

void fvInitializeOutputThermint(OUTPUT *output, fnWriteOutput fnWrite[]) {

  sprintf(output[OUT_TMAN].cName, "TMan");
  sprintf(output[OUT_TMAN].cDescr, "Mantle Temperature");
  sprintf(output[OUT_TMAN].cNeg, "K");
  output[OUT_TMAN].bNeg       = 1;
  output[OUT_TMAN].dNeg       = 1;
  output[OUT_TMAN].iNum       = 1;
  output[OUT_TMAN].iModuleBit = THERMINT;
  fnWrite[OUT_TMAN]           = &fvWriteTMan;

  sprintf(output[OUT_TUMAN].cName, "TUMan");
  sprintf(output[OUT_TUMAN].cDescr, "Upper Mantle Temperature");
  sprintf(output[OUT_TUMAN].cNeg, "K");
  output[OUT_TUMAN].bNeg       = 1;
  output[OUT_TUMAN].dNeg       = 1;
  output[OUT_TUMAN].iNum       = 1;
  output[OUT_TUMAN].iModuleBit = THERMINT;
  fnWrite[OUT_TUMAN]           = &fvWriteTUMan;

  sprintf(output[OUT_TLMAN].cName, "TLMan");
  sprintf(output[OUT_TLMAN].cDescr, "Lower Mantle Temperature");
  sprintf(output[OUT_TLMAN].cNeg, "K");
  output[OUT_TLMAN].bNeg       = 1;
  output[OUT_TLMAN].dNeg       = 1;
  output[OUT_TLMAN].iNum       = 1;
  output[OUT_TLMAN].iModuleBit = THERMINT;
  fnWrite[OUT_TLMAN]           = &fvWriteTLMan;

  sprintf(output[OUT_TJUMPUMAN].cName, "TJumpUMan");
  sprintf(output[OUT_TJUMPUMAN].cDescr, "Upper Mantle Temperature Jump");
  sprintf(output[OUT_TJUMPUMAN].cNeg, "K");
  output[OUT_TJUMPUMAN].bNeg       = 1;
  output[OUT_TJUMPUMAN].dNeg       = 1;
  output[OUT_TJUMPUMAN].iNum       = 1;
  output[OUT_TJUMPUMAN].iModuleBit = THERMINT;
  fnWrite[OUT_TJUMPUMAN]           = &fvWriteTJumpUMan;

  sprintf(output[OUT_TJUMPLMAN].cName, "TJumpLMan");
  sprintf(output[OUT_TJUMPLMAN].cDescr, "Lower Mantle Temperature Jump");
  sprintf(output[OUT_TJUMPLMAN].cNeg, "K");
  output[OUT_TJUMPLMAN].bNeg       = 1;
  output[OUT_TJUMPLMAN].dNeg       = 1;
  output[OUT_TJUMPLMAN].iNum       = 1;
  output[OUT_TJUMPLMAN].iModuleBit = THERMINT;
  fnWrite[OUT_TJUMPLMAN]           = &fvWriteTJumpLMan;

  sprintf(output[OUT_SIGNTJUMPUMAN].cName, "SignTJumpUMan");
  sprintf(output[OUT_SIGNTJUMPUMAN].cDescr,
          "Sign of Upper Mantle Temperature Jump");
  sprintf(output[OUT_SIGNTJUMPUMAN].cNeg, "K");
  output[OUT_SIGNTJUMPUMAN].bNeg       = 1;
  output[OUT_SIGNTJUMPUMAN].dNeg       = 1;
  output[OUT_SIGNTJUMPUMAN].iNum       = 1;
  output[OUT_SIGNTJUMPUMAN].iModuleBit = THERMINT;
  fnWrite[OUT_SIGNTJUMPUMAN]           = &fvWriteSignTJumpUMan;

  sprintf(output[OUT_SIGNTJUMPLMAN].cName, "SignTJumpLMan");
  sprintf(output[OUT_SIGNTJUMPLMAN].cDescr,
          "Sign of Lower Mantle Temperature Jump");
  sprintf(output[OUT_SIGNTJUMPLMAN].cNeg, "K");
  output[OUT_SIGNTJUMPLMAN].bNeg       = 1;
  output[OUT_SIGNTJUMPLMAN].dNeg       = 1;
  output[OUT_SIGNTJUMPLMAN].iNum       = 1;
  output[OUT_SIGNTJUMPLMAN].iModuleBit = THERMINT;
  fnWrite[OUT_SIGNTJUMPLMAN]           = &fvWriteSignTJumpLMan;
  // Needs a LongDescr

  sprintf(output[OUT_TCMB].cName, "TCMB");
  sprintf(output[OUT_TCMB].cDescr, "CMB Temperature");
  sprintf(output[OUT_TCMB].cNeg, "K");
  output[OUT_TCMB].bNeg       = 1;
  output[OUT_TCMB].dNeg       = 1;
  output[OUT_TCMB].iNum       = 1;
  output[OUT_TCMB].iModuleBit = THERMINT;
  fnWrite[OUT_TCMB]           = &fvWriteTCMB;

  sprintf(output[OUT_TCORE].cName, "TCore");
  sprintf(output[OUT_TCORE].cDescr, "Core Temperature");
  sprintf(output[OUT_TCORE].cNeg, "K");
  output[OUT_TCORE].bNeg       = 1;
  output[OUT_TCORE].dNeg       = 1;
  output[OUT_TCORE].iNum       = 1;
  output[OUT_TCORE].iModuleBit = THERMINT;
  fnWrite[OUT_TCORE]           = &fvWriteTCore;

  sprintf(output[OUT_VISCUMAN].cName, "ViscUMan");
  sprintf(output[OUT_VISCUMAN].cDescr, "Upper Mantle Viscosity");
  sprintf(output[OUT_VISCUMAN].cNeg, "m^2/s");
  output[OUT_VISCUMAN].bNeg       = 1;
  output[OUT_VISCUMAN].dNeg       = 1;
  output[OUT_VISCUMAN].iNum       = 1;
  output[OUT_VISCUMAN].iModuleBit = THERMINT;
  fnWrite[OUT_VISCUMAN]           = &fvWriteViscUMan;

  /* ViscUManArr Arrhenius Only */
  sprintf(output[OUT_VISCUMANARR].cName, "ViscUManArr");
  sprintf(output[OUT_VISCUMANARR].cDescr, "Upper Mantle Arrhenius Viscosity");
  sprintf(output[OUT_VISCUMANARR].cNeg, "m^2/s");
  output[OUT_VISCUMANARR].bNeg       = 1;
  output[OUT_VISCUMANARR].dNeg       = 1;
  output[OUT_VISCUMANARR].iNum       = 1;
  output[OUT_VISCUMANARR].iModuleBit = THERMINT;
  fnWrite[OUT_VISCUMANARR]           = &fvWriteViscUManArr;
  // Needs a LongDescr

  /* ViscLMan */
  sprintf(output[OUT_VISCLMAN].cName, "ViscLMan");
  sprintf(output[OUT_VISCLMAN].cDescr, "Lower Mantle Viscosity");
  sprintf(output[OUT_VISCLMAN].cNeg, "m^2/s");
  output[OUT_VISCLMAN].bNeg       = 1;
  output[OUT_VISCLMAN].dNeg       = 1;
  output[OUT_VISCLMAN].iNum       = 1;
  output[OUT_VISCLMAN].iModuleBit = THERMINT;
  fnWrite[OUT_VISCLMAN]           = &fvWriteViscLMan;

  /* ViscMMan */
  sprintf(output[OUT_VISCMMAN].cName, "ViscMMan");
  sprintf(output[OUT_VISCMMAN].cDescr, "Average (mid) Mantle Viscosity");
  sprintf(output[OUT_VISCMMAN].cNeg, "m^2/s");
  output[OUT_VISCMMAN].bNeg       = 1;
  output[OUT_VISCMMAN].dNeg       = 1;
  output[OUT_VISCMMAN].iNum       = 1;
  output[OUT_VISCMMAN].iModuleBit = THERMINT;
  fnWrite[OUT_VISCMMAN]           = &fvWriteViscMMan;

  sprintf(output[OUT_VISCJUMPMAN].cName, "ViscJumpMan");
  sprintf(output[OUT_VISCJUMPMAN].cDescr, "Mantle Viscosity Ratio");
  sprintf(output[OUT_VISCJUMPMAN].cNeg, "nd");
  output[OUT_VISCJUMPMAN].bNeg       = 1;
  output[OUT_VISCJUMPMAN].dNeg       = 1;
  output[OUT_VISCJUMPMAN].iNum       = 1;
  output[OUT_VISCJUMPMAN].iModuleBit = THERMINT;
  fnWrite[OUT_VISCJUMPMAN]           = &fvWriteViscJumpMan;
  // Needs a LongDescr

  /* BLUMan */
  sprintf(output[OUT_BLUMAN].cName, "BLUMan");
  sprintf(output[OUT_BLUMAN].cDescr, "Boundary Layer Thickness Upper Mantle");
  sprintf(output[OUT_BLUMAN].cNeg, "km");
  output[OUT_BLUMAN].bNeg       = 1;
  output[OUT_BLUMAN].dNeg       = 1e-3; // KM;
  output[OUT_BLUMAN].iNum       = 1;
  output[OUT_BLUMAN].iModuleBit = THERMINT;
  fnWrite[OUT_BLUMAN]           = &fvWriteBLUMan;
  // Needs a LongDescr

  /* BLLMan */
  sprintf(output[OUT_BLLMAN].cName, "BLLMan");
  sprintf(output[OUT_BLLMAN].cDescr, "Boundary Layer Thickness Lower Mantle");
  sprintf(output[OUT_BLLMAN].cNeg, "km");
  output[OUT_BLLMAN].bNeg       = 1;
  output[OUT_BLLMAN].dNeg       = 1e-3;
  output[OUT_BLLMAN].iNum       = 1;
  output[OUT_BLLMAN].iModuleBit = THERMINT;
  fnWrite[OUT_BLLMAN]           = &fvWriteBLLMan;
  // Needs a LongDescr

  /* ShmodUMan */
  sprintf(output[OUT_SHMODUMAN].cName, "ShmodUMan");
  sprintf(output[OUT_SHMODUMAN].cDescr, "Shear Modulus Upper Mantle");
  sprintf(output[OUT_SHMODUMAN].cNeg, "Pa");
  output[OUT_SHMODUMAN].bNeg       = 1;
  output[OUT_SHMODUMAN].dNeg       = 1;
  output[OUT_SHMODUMAN].iNum       = 1;
  output[OUT_SHMODUMAN].iModuleBit = THERMINT;
  fnWrite[OUT_SHMODUMAN]           = &fvWriteShmodUMan;

  /* FMeltUMan */
  sprintf(output[OUT_FMELTUMAN].cName, "FMeltUMan");
  sprintf(output[OUT_FMELTUMAN].cDescr, "Melt Fraction Upper Mantle");
  sprintf(output[OUT_FMELTUMAN].cNeg, "nd");
  output[OUT_FMELTUMAN].bNeg       = 1;
  output[OUT_FMELTUMAN].dNeg       = 1;
  output[OUT_FMELTUMAN].iNum       = 1;
  output[OUT_FMELTUMAN].iModuleBit = THERMINT;
  fnWrite[OUT_FMELTUMAN]           = &fvWriteFMeltUMan;

  /* FMeltLMan */
  sprintf(output[OUT_FMELTLMAN].cName, "FMeltLMan");
  sprintf(output[OUT_FMELTLMAN].cDescr, "Melt Fraction Lower Mantle");
  sprintf(output[OUT_FMELTLMAN].cNeg, "nd");
  output[OUT_FMELTLMAN].bNeg       = 1;
  output[OUT_FMELTLMAN].dNeg       = 1;
  output[OUT_FMELTLMAN].iNum       = 1;
  output[OUT_FMELTLMAN].iModuleBit = THERMINT;
  fnWrite[OUT_FMELTLMAN]           = &fvWriteFMeltLMan;

  /* MeltfactorUMan */
  sprintf(output[OUT_MELTFACTORUMAN].cName, "MeltfactorUMan");
  sprintf(output[OUT_MELTFACTORUMAN].cDescr, "Melt Factor Upper Mantle");
  sprintf(output[OUT_MELTFACTORUMAN].cNeg, "nd");
  output[OUT_MELTFACTORUMAN].bNeg       = 1;
  output[OUT_MELTFACTORUMAN].dNeg       = 1;
  output[OUT_MELTFACTORUMAN].iNum       = 1;
  output[OUT_MELTFACTORUMAN].iModuleBit = THERMINT;
  fnWrite[OUT_MELTFACTORUMAN]           = &fvWriteMeltfactorUMan;
  // Needs a LongDescr

  /* MeltfactorLMan */
  sprintf(output[OUT_MELTFACTORLMAN].cName, "MeltfactorLMan");
  sprintf(output[OUT_MELTFACTORLMAN].cDescr, "Melt Factor Lower Mantle");
  sprintf(output[OUT_MELTFACTORLMAN].cNeg, "nd");
  output[OUT_MELTFACTORLMAN].bNeg       = 1;
  output[OUT_MELTFACTORLMAN].dNeg       = 1;
  output[OUT_MELTFACTORLMAN].iNum       = 1;
  output[OUT_MELTFACTORLMAN].iModuleBit = THERMINT;
  fnWrite[OUT_MELTFACTORLMAN]           = &fvWriteMeltfactorLMan;
  // Needs a LongDescr

  /* DepthMeltMan */
  sprintf(output[OUT_DEPTHMELTMAN].cName, "DepthMeltMan");
  sprintf(output[OUT_DEPTHMELTMAN].cDescr,
          "Depth to base of Upper Mantle Melt Region");
  sprintf(output[OUT_DEPTHMELTMAN].cNeg, "m");
  output[OUT_DEPTHMELTMAN].bNeg       = 1;
  output[OUT_DEPTHMELTMAN].dNeg       = 1;
  output[OUT_DEPTHMELTMAN].iNum       = 1;
  output[OUT_DEPTHMELTMAN].iModuleBit = THERMINT;
  fnWrite[OUT_DEPTHMELTMAN]           = &fvWriteDepthMeltMan;

  /* TDepthMeltMan */
  sprintf(output[OUT_TDEPTHMELTMAN].cName, "TDepthMeltMan");
  sprintf(output[OUT_TDEPTHMELTMAN].cDescr,
          "Temp at base of Upper Mantle Melt Region");
  sprintf(output[OUT_TDEPTHMELTMAN].cNeg, "K");
  output[OUT_TDEPTHMELTMAN].bNeg       = 1;
  output[OUT_TDEPTHMELTMAN].dNeg       = 1;
  output[OUT_TDEPTHMELTMAN].iNum       = 1;
  output[OUT_TDEPTHMELTMAN].iModuleBit = THERMINT;
  fnWrite[OUT_TDEPTHMELTMAN]           = &fvWriteTDepthMeltMan;

  /* TJumpMeltMan */
  sprintf(output[OUT_TJUMPMELTMAN].cName, "TJumpMeltMan");
  sprintf(output[OUT_TJUMPMELTMAN].cDescr,
          "Temp Jump across Upper Mantle Melt Region");
  sprintf(output[OUT_TJUMPMELTMAN].cNeg, "K");
  output[OUT_TJUMPMELTMAN].bNeg       = 1;
  output[OUT_TJUMPMELTMAN].dNeg       = 1;
  output[OUT_TJUMPMELTMAN].iNum       = 1;
  output[OUT_TJUMPMELTMAN].iModuleBit = THERMINT;
  fnWrite[OUT_TJUMPMELTMAN]           = &fvWriteTJumpMeltMan;

  /* MeltMassFluxMan */
  sprintf(output[OUT_MELTMASSFLUXMAN].cName, "MeltMassFluxMan");
  sprintf(output[OUT_MELTMASSFLUXMAN].cDescr, "Mantle Melt Mass Flux");
  sprintf(output[OUT_MELTMASSFLUXMAN].cNeg, "kg/s");
  output[OUT_MELTMASSFLUXMAN].bNeg       = 1;
  output[OUT_MELTMASSFLUXMAN].dNeg       = 1;
  output[OUT_MELTMASSFLUXMAN].iNum       = 1;
  output[OUT_MELTMASSFLUXMAN].iModuleBit = THERMINT;
  fnWrite[OUT_MELTMASSFLUXMAN]           = &fvWriteMeltMassFluxMan;

  /* EruptEff */
  sprintf(output[OUT_ERUPTEFF].cName, "EruptEff");
  sprintf(output[OUT_ERUPTEFF].cDescr, "Mantle Melt Eruption Efficiency");
  sprintf(output[OUT_ERUPTEFF].cNeg, "nd");
  output[OUT_ERUPTEFF].bNeg       = 1;
  output[OUT_ERUPTEFF].dNeg       = 1;
  output[OUT_ERUPTEFF].iNum       = 1;
  output[OUT_ERUPTEFF].iModuleBit = THERMINT;
  fnWrite[OUT_ERUPTEFF]           = &fvWriteEruptEff;

  /* RayleighMan */
  sprintf(output[OUT_RAYLEIGHMAN].cName, "RayleighMan");
  sprintf(output[OUT_RAYLEIGHMAN].cDescr, "Mantle Rayleigh Number");
  sprintf(output[OUT_RAYLEIGHMAN].cNeg, "nd");
  output[OUT_RAYLEIGHMAN].bNeg       = 1;
  output[OUT_RAYLEIGHMAN].dNeg       = 1;
  output[OUT_RAYLEIGHMAN].iNum       = 1;
  output[OUT_RAYLEIGHMAN].iModuleBit = THERMINT;
  fnWrite[OUT_RAYLEIGHMAN]           = &fvWriteRayleighMan;

  /*  CORE WRITES */
  /* RIC */
  sprintf(output[OUT_RIC].cName, "RIC");
  sprintf(output[OUT_RIC].cDescr, "Inner Core Radius");
  sprintf(output[OUT_RIC].cNeg, "km");
  output[OUT_RIC].bNeg       = 1;
  output[OUT_RIC].dNeg       = 1e-3;
  output[OUT_RIC].iNum       = 1;
  output[OUT_RIC].iModuleBit = THERMINT;
  fnWrite[OUT_RIC]           = &fvWriteRIC;

  /* DRICDTCMB */
  sprintf(output[OUT_DRICDTCMB].cName, "DRICDTCMB");
  sprintf(output[OUT_DRICDTCMB].cDescr, "d(R_ic)/d(T_cmb)");
  sprintf(output[OUT_DRICDTCMB].cNeg, "m/K");
  output[OUT_DRICDTCMB].bNeg       = 1;
  output[OUT_DRICDTCMB].dNeg       = 1;
  output[OUT_DRICDTCMB].iNum       = 1;
  output[OUT_DRICDTCMB].iModuleBit = THERMINT;
  fnWrite[OUT_DRICDTCMB]           = &fvWriteDRICDTCMB;

  // Dynamic Viscosity
  sprintf(output[OUT_DYNVISC].cName, "DynViscUMan");
  sprintf(output[OUT_DYNVISC].cDescr, "Upper Mantle's Dynamic Viscosity");
  sprintf(output[OUT_DYNVISC].cNeg, "Pa");
  output[OUT_DYNVISC].bNeg       = 1;
  output[OUT_DYNVISC].dNeg       = 1;
  output[OUT_DYNVISC].iNum       = 1;
  output[OUT_DYNVISC].iModuleBit = THERMINT;
  fnWrite[OUT_DYNVISC]           = &fvWriteDynamicViscosity;

  /* ChiOC */
  sprintf(output[OUT_CHIOC].cName, "ChiOC");
  sprintf(output[OUT_CHIOC].cDescr,
          "Light Element Concentration in Outer Core");
  sprintf(output[OUT_CHIOC].cNeg, "nd");
  output[OUT_CHIOC].bNeg       = 1;
  output[OUT_CHIOC].dNeg       = 1;
  output[OUT_CHIOC].iNum       = 1;
  output[OUT_CHIOC].iModuleBit = THERMINT;
  fnWrite[OUT_CHIOC]           = &fvWriteChiOC;

  /* ChiIC */
  sprintf(output[OUT_CHIIC].cName, "ChiIC");
  sprintf(output[OUT_CHIIC].cDescr,
          "Light Element Concentration in Inner Core");
  sprintf(output[OUT_CHIIC].cNeg, "nd");
  output[OUT_CHIIC].bNeg       = 1;
  output[OUT_CHIIC].dNeg       = 1;
  output[OUT_CHIIC].iNum       = 1;
  output[OUT_CHIIC].iModuleBit = THERMINT;
  fnWrite[OUT_CHIIC]           = &fvWriteChiIC;

  /* MassOC */
  sprintf(output[OUT_MASSOC].cName, "MassOC");
  sprintf(output[OUT_MASSOC].cDescr, "Mass of Outer Core");
  sprintf(output[OUT_MASSOC].cNeg, "EMASSOC");
  output[OUT_MASSOC].bNeg       = 1;
  output[OUT_MASSOC].dNeg       = 1 / (EMASSOC);
  output[OUT_MASSOC].iNum       = 1;
  output[OUT_MASSOC].iModuleBit = THERMINT;
  fnWrite[OUT_MASSOC]           = &fvWriteMassOC;

  /* MassIC */
  sprintf(output[OUT_MASSIC].cName, "MassIC");
  sprintf(output[OUT_MASSIC].cDescr, "Mass of Inner Core");
  sprintf(output[OUT_MASSIC].cNeg, "EMASSIC");
  output[OUT_MASSIC].bNeg       = 1;
  output[OUT_MASSIC].dNeg       = 1 / (EMASSIC);
  output[OUT_MASSIC].iNum       = 1;
  output[OUT_MASSIC].iModuleBit = THERMINT;
  fnWrite[OUT_MASSIC]           = &fvWriteMassIC;

  /* MassChiOC */
  sprintf(output[OUT_MASSCHIOC].cName, "MassChiOC");
  sprintf(output[OUT_MASSCHIOC].cDescr, "Mass of Chi in Outer Core");
  sprintf(output[OUT_MASSCHIOC].cNeg, "EMASSOC_CHI");
  output[OUT_MASSCHIOC].bNeg       = 1;
  output[OUT_MASSCHIOC].dNeg       = 1 / (EMASSOC_CHI);
  output[OUT_MASSCHIOC].iNum       = 1;
  output[OUT_MASSCHIOC].iModuleBit = THERMINT;
  fnWrite[OUT_MASSCHIOC]           = &fvWriteMassChiOC;
  // Needs a LongDescr

  /* MassChiIC */
  sprintf(output[OUT_MASSCHIIC].cName, "MassChiIC");
  sprintf(output[OUT_MASSCHIIC].cDescr, "Mass of Chi in Inner Core");
  sprintf(output[OUT_MASSCHIIC].cNeg, "EMASSIC_CHI");
  output[OUT_MASSCHIIC].bNeg       = 1;
  output[OUT_MASSCHIIC].dNeg       = 1 / (EMASSIC_CHI);
  output[OUT_MASSCHIIC].iNum       = 1;
  output[OUT_MASSCHIIC].iModuleBit = THERMINT;
  fnWrite[OUT_MASSCHIIC]           = &fvWriteMassChiIC;
  // Needs a LongDescr

  /* DTChi */
  sprintf(output[OUT_DTCHI].cName, "DTChi");
  sprintf(output[OUT_DTCHI].cDescr, "Core Liquidus Depression");
  sprintf(output[OUT_DTCHI].cNeg, "K");
  output[OUT_DTCHI].bNeg       = 1;
  output[OUT_DTCHI].dNeg       = 1;
  output[OUT_DTCHI].iNum       = 1;
  output[OUT_DTCHI].iModuleBit = THERMINT;
  fnWrite[OUT_DTCHI]           = &fvWriteDTChi;
  // Needs a LongDescr

  /* CoreBuoyTherm */
  sprintf(output[OUT_COREBUOYTHERM].cName, "CoreBuoyTherm");
  sprintf(output[OUT_COREBUOYTHERM].cDescr, "Core Thermal Buoyancy Flux");
  sprintf(output[OUT_COREBUOYTHERM].cNeg, "m^2/s^3");
  output[OUT_COREBUOYTHERM].bNeg       = 1;
  output[OUT_COREBUOYTHERM].dNeg       = 1;
  output[OUT_COREBUOYTHERM].iNum       = 1;
  output[OUT_COREBUOYTHERM].iModuleBit = THERMINT;
  fnWrite[OUT_COREBUOYTHERM]           = &fvWriteCoreBuoyTherm;
  // Needs a LongDescr

  /* CoreBuoyCompo */
  sprintf(output[OUT_COREBUOYCOMPO].cName, "CoreBuoyCompo");
  sprintf(output[OUT_COREBUOYCOMPO].cDescr, "Core Compositional Buoyancy Flux");
  sprintf(output[OUT_COREBUOYCOMPO].cNeg, "m^2/s^3");
  output[OUT_COREBUOYCOMPO].bNeg       = 1;
  output[OUT_COREBUOYCOMPO].dNeg       = 1;
  output[OUT_COREBUOYCOMPO].iNum       = 1;
  output[OUT_COREBUOYCOMPO].iModuleBit = THERMINT;
  fnWrite[OUT_COREBUOYCOMPO]           = &fvWriteCoreBuoyCompo;
  // Needs a LongDescr

  /* CoreBuoyTotal */
  sprintf(output[OUT_COREBUOYTOTAL].cName, "CoreBuoyTotal");
  sprintf(output[OUT_COREBUOYTOTAL].cDescr, "Core Total Buoyancy Flux");
  sprintf(output[OUT_COREBUOYTOTAL].cNeg, "m^2/s^3");
  output[OUT_COREBUOYTOTAL].bNeg       = 1;
  output[OUT_COREBUOYTOTAL].dNeg       = 1;
  output[OUT_COREBUOYTOTAL].iNum       = 1;
  output[OUT_COREBUOYTOTAL].iModuleBit = THERMINT;
  fnWrite[OUT_COREBUOYTOTAL]           = &fvWriteCoreBuoyTotal;

  /* GravICB */
  sprintf(output[OUT_GRAVICB].cName, "GravICB");
  sprintf(output[OUT_GRAVICB].cDescr, "ICB Gravity");
  sprintf(output[OUT_GRAVICB].cNeg, "m/s^2");
  output[OUT_GRAVICB].bNeg       = 1;
  output[OUT_GRAVICB].dNeg       = 1;
  output[OUT_GRAVICB].iNum       = 1;
  output[OUT_GRAVICB].iModuleBit = THERMINT;
  fnWrite[OUT_GRAVICB]           = &fvWriteGravICB;
  // Needs a LongDescr

  /* MagMom */
  sprintf(output[OUT_MAGMOM].cName, "MagMom");
  sprintf(output[OUT_MAGMOM].cDescr, "Core Magnetic Moment");
  sprintf(output[OUT_MAGMOM].cNeg, "EMAGMOM");
  output[OUT_MAGMOM].bNeg       = 1;
  output[OUT_MAGMOM].dNeg       = 1. / (EMAGMOM);
  output[OUT_MAGMOM].iNum       = 1;
  output[OUT_MAGMOM].iModuleBit = THERMINT;
  fnWrite[OUT_MAGMOM]           = &fvWriteMagMom;

  /* RICDot */
  sprintf(output[OUT_RICDOT].cName, "RICDot");
  sprintf(output[OUT_RICDOT].cDescr, "Inner Core growth rate");
  sprintf(output[OUT_RICDOT].cNeg, "m/s");
  output[OUT_RICDOT].bNeg       = 1;
  output[OUT_RICDOT].dNeg       = 1;
  output[OUT_RICDOT].iNum       = 1;
  output[OUT_RICDOT].iModuleBit = THERMINT;
  fnWrite[OUT_RICDOT]           = &fvWriteRICDot;

  /* PresSWind */
  sprintf(output[OUT_PRESSWIND].cName, "PresSWind");
  sprintf(output[OUT_PRESSWIND].cDescr, "Stellar wind pressure at body");
  sprintf(output[OUT_PRESSWIND].cNeg, "EPRESSWIND");
  output[OUT_PRESSWIND].bNeg       = 1;
  output[OUT_PRESSWIND].dNeg       = 1. / (EPRESSWIND);
  output[OUT_PRESSWIND].iNum       = 1;
  output[OUT_PRESSWIND].iModuleBit = THERMINT;
  fnWrite[OUT_PRESSWIND]           = &fvWritePresSWind;

  /* MagPauseRad */
  sprintf(output[OUT_MAGPAUSERAD].cName, "MagPauseRad");
  sprintf(output[OUT_MAGPAUSERAD].cDescr, "Magnetopause Radius");
  sprintf(output[OUT_MAGPAUSERAD].cNeg, "EMAGPAUSERAD");
  output[OUT_MAGPAUSERAD].bNeg       = 1;
  output[OUT_MAGPAUSERAD].dNeg       = 1. / (EMAGPAUSERAD);
  output[OUT_MAGPAUSERAD].iNum       = 1;
  output[OUT_MAGPAUSERAD].iModuleBit = THERMINT;
  fnWrite[OUT_MAGPAUSERAD]           = &fvWriteMagPauseRad;

  /* Heat Fluxes/Flows */
  /* HFluxUMan */
  sprintf(output[OUT_HFLUXUMAN].cName, "HfluxUMan");
  sprintf(output[OUT_HFLUXUMAN].cDescr, "Heat Flux Upper Mantle");
  sprintf(output[OUT_HFLUXUMAN].cNeg, "W/m^2");
  output[OUT_HFLUXUMAN].bNeg       = 1;
  output[OUT_HFLUXUMAN].dNeg       = 1;
  output[OUT_HFLUXUMAN].iNum       = 1;
  output[OUT_HFLUXUMAN].iModuleBit = THERMINT;
  fnWrite[OUT_HFLUXUMAN]           = &fvWriteHfluxUMan;

  /* HFluxLMan */
  sprintf(output[OUT_HFLUXLMAN].cName, "HfluxLMan");
  sprintf(output[OUT_HFLUXLMAN].cDescr, "Heat Flux Lower Mantle");
  sprintf(output[OUT_HFLUXLMAN].cNeg, "W/m^2");
  output[OUT_HFLUXLMAN].bNeg       = 1;
  output[OUT_HFLUXLMAN].dNeg       = 1;
  output[OUT_HFLUXLMAN].iNum       = 1;
  output[OUT_HFLUXLMAN].iModuleBit = THERMINT;
  fnWrite[OUT_HFLUXLMAN]           = &fvWriteHfluxLMan;

  /* HFluxCMB */
  sprintf(output[OUT_HFLUXCMB].cName, "HfluxCMB");
  sprintf(output[OUT_HFLUXCMB].cDescr, "Heat Flux Core-Mantle Boundary");
  sprintf(output[OUT_HFLUXCMB].cNeg, "W/m^2");
  output[OUT_HFLUXCMB].bNeg       = 1;
  output[OUT_HFLUXCMB].dNeg       = 1;
  output[OUT_HFLUXCMB].iNum       = 1;
  output[OUT_HFLUXCMB].iModuleBit = THERMINT;
  fnWrite[OUT_HFLUXCMB]           = &fvWriteHfluxCMB;

  /* HfluxCMBAd */
  sprintf(output[OUT_HFLUXCMBAD].cName, "HfluxCMBAd");
  sprintf(output[OUT_HFLUXCMBAD].cDescr,
          "Adiabatic Heat Flux Core-Mantle Boundary");
  sprintf(output[OUT_HFLUXCMBAD].cNeg, "W/m^2");
  output[OUT_HFLUXCMBAD].bNeg       = 1;
  output[OUT_HFLUXCMBAD].dNeg       = 1;
  output[OUT_HFLUXCMBAD].iNum       = 1;
  output[OUT_HFLUXCMBAD].iModuleBit = THERMINT;
  fnWrite[OUT_HFLUXCMBAD]           = &fvWriteHfluxCMBAd;
  // Needs a LongDescr

  /* HfluxCMBConv */
  sprintf(output[OUT_HFLUXCMBCONV].cName, "HfluxCMBConv");
  sprintf(output[OUT_HFLUXCMBCONV].cDescr,
          "Super-Adiabatic (convective) Heat Flux Core-Mantle Boundary");
  sprintf(output[OUT_HFLUXCMBCONV].cNeg, "W/m^2");
  output[OUT_HFLUXCMBCONV].bNeg       = 1;
  output[OUT_HFLUXCMBCONV].dNeg       = 1;
  output[OUT_HFLUXCMBCONV].iNum       = 1;
  output[OUT_HFLUXCMBCONV].iModuleBit = THERMINT;
  fnWrite[OUT_HFLUXCMBCONV]           = &fvWriteHfluxCMBConv;

  /* ThermConductOC */
  sprintf(output[OUT_THERMCONDUCTOC].cName, "ThermConductOC");
  sprintf(output[OUT_THERMCONDUCTOC].cDescr,
          "Thermal Conductivity of the Outer Core");
  sprintf(output[OUT_THERMCONDUCTOC].cNeg, "W/m/K");
  output[OUT_THERMCONDUCTOC].bNeg       = 1;
  output[OUT_THERMCONDUCTOC].dNeg       = 1;
  output[OUT_THERMCONDUCTOC].iNum       = 1;
  output[OUT_THERMCONDUCTOC].iModuleBit = THERMINT;
  fnWrite[OUT_THERMCONDUCTOC]           = &fvWriteThermConductOC;

  /* HflowUMan */
  sprintf(output[OUT_HFLOWUMAN].cName, "HflowUMan");
  sprintf(output[OUT_HFLOWUMAN].cDescr, "Heat Flow Upper Mantle");
  sprintf(output[OUT_HFLOWUMAN].cNeg, "TW");
  output[OUT_HFLOWUMAN].bNeg       = 1;
  output[OUT_HFLOWUMAN].dNeg       = 1e-12;
  output[OUT_HFLOWUMAN].iNum       = 1;
  output[OUT_HFLOWUMAN].iModuleBit = THERMINT;
  fnWrite[OUT_HFLOWUMAN]           = &fvWriteHflowUMan;

  /* HFlowLMan */
  sprintf(output[OUT_HFLOWLMAN].cName, "HflowLMan");
  sprintf(output[OUT_HFLOWLMAN].cDescr, "Heat Flow Lower Mantle");
  sprintf(output[OUT_HFLOWLMAN].cNeg, "TW");
  output[OUT_HFLOWLMAN].bNeg       = 1;
  output[OUT_HFLOWLMAN].dNeg       = 1e-12;
  output[OUT_HFLOWLMAN].iNum       = 1;
  output[OUT_HFLOWLMAN].iModuleBit = THERMINT;
  fnWrite[OUT_HFLOWLMAN]           = &fvWriteHflowLMan;

  /* HFlowCMB */
  sprintf(output[OUT_HFLOWCMB].cName, "HflowCMB");
  sprintf(output[OUT_HFLOWCMB].cDescr, "Heat Flow Core-Mantle Boundary");
  sprintf(output[OUT_HFLOWCMB].cNeg, "TW");
  output[OUT_HFLOWCMB].bNeg       = 1;
  output[OUT_HFLOWCMB].dNeg       = 1e-12;
  output[OUT_HFLOWCMB].iNum       = 1;
  output[OUT_HFLOWCMB].iModuleBit = THERMINT;
  fnWrite[OUT_HFLOWCMB]           = &fvWriteHflowCMB;

  /* HflowLatentMan */
  sprintf(output[OUT_HFLOWLATENTMAN].cName, "HflowLatentMan");
  sprintf(output[OUT_HFLOWLATENTMAN].cDescr, "Latent Heat Flow Mantle");
  sprintf(output[OUT_HFLOWLATENTMAN].cNeg, "TW");
  output[OUT_HFLOWLATENTMAN].bNeg       = 1;
  output[OUT_HFLOWLATENTMAN].dNeg       = 1e-12;
  output[OUT_HFLOWLATENTMAN].iNum       = 1;
  output[OUT_HFLOWLATENTMAN].iModuleBit = THERMINT;
  fnWrite[OUT_HFLOWLATENTMAN]           = &fvWriteHflowLatentMan;

  /* HflowMeltMan */
  sprintf(output[OUT_HFLOWMELTMAN].cName, "HflowMeltMan");
  sprintf(output[OUT_HFLOWMELTMAN].cDescr, "Melt Heat Flow Mantle");
  sprintf(output[OUT_HFLOWMELTMAN].cNeg, "TW");
  output[OUT_HFLOWMELTMAN].bNeg       = 1;
  output[OUT_HFLOWMELTMAN].dNeg       = 1e-12;
  output[OUT_HFLOWMELTMAN].iNum       = 1;
  output[OUT_HFLOWMELTMAN].iModuleBit = THERMINT;
  fnWrite[OUT_HFLOWMELTMAN]           = &fvWriteHflowMeltMan;

  /* HFlowSecMan */
  sprintf(output[OUT_HFLOWSECMAN].cName, "HflowSecMan");
  sprintf(output[OUT_HFLOWSECMAN].cDescr, "Mantle Secular Heat Flow");
  sprintf(output[OUT_HFLOWSECMAN].cNeg, "TW");
  output[OUT_HFLOWSECMAN].bNeg       = 1;
  output[OUT_HFLOWSECMAN].dNeg       = 1e-12;
  output[OUT_HFLOWSECMAN].iNum       = 1;
  output[OUT_HFLOWSECMAN].iModuleBit = THERMINT;
  fnWrite[OUT_HFLOWSECMAN]           = &fvWriteHflowSecMan;
  // Needs a LongDescr

  /* HFlowSurf */
  sprintf(output[OUT_HFLOWSURF].cName, "HflowSurf");
  sprintf(output[OUT_HFLOWSURF].cDescr, "Heat Flow Surface Total");
  sprintf(output[OUT_HFLOWSURF].cNeg, "TW");
  output[OUT_HFLOWSURF].bNeg       = 1;
  output[OUT_HFLOWSURF].dNeg       = 1e-12;
  output[OUT_HFLOWSURF].iNum       = 1;
  output[OUT_HFLOWSURF].iModuleBit = THERMINT;
  fnWrite[OUT_HFLOWSURF]           = &fvWriteHflowSurf;
  // Is this redundant with SurfEnFluxTotal? XXX

  /* HFlowLatentIC */
  sprintf(output[OUT_HFLOWLATENTIC].cName, "HflowLatentIC");
  sprintf(output[OUT_HFLOWLATENTIC].cDescr, "Latent Heat Release at ICB");
  sprintf(output[OUT_HFLOWLATENTIC].cNeg, "TW");
  output[OUT_HFLOWLATENTIC].bNeg       = 1;
  output[OUT_HFLOWLATENTIC].dNeg       = 1e-12;
  output[OUT_HFLOWLATENTIC].iNum       = 1;
  output[OUT_HFLOWLATENTIC].iModuleBit = THERMINT;
  fnWrite[OUT_HFLOWLATENTIC]           = &fvWriteHflowLatentIC;

  /* PowerGravIC */
  sprintf(output[OUT_POWERGRAVIC].cName, "PowerGravIC");
  sprintf(output[OUT_POWERGRAVIC].cDescr, "Gravitational Power Release at ICB");
  sprintf(output[OUT_POWERGRAVIC].cNeg, "TW");
  output[OUT_POWERGRAVIC].bNeg       = 1;
  output[OUT_POWERGRAVIC].dNeg       = 1e-12;
  output[OUT_POWERGRAVIC].iNum       = 1;
  output[OUT_POWERGRAVIC].iModuleBit = THERMINT;
  fnWrite[OUT_POWERGRAVIC]           = &fvWritePowerGravIC;

  /* TDotMan */
  sprintf(output[OUT_TDOTMAN].cName, "TDotMan");
  sprintf(output[OUT_TDOTMAN].cDescr, "Change in Mantle Temperature");
  sprintf(output[OUT_TDOTMAN].cNeg, "K/s");
  output[OUT_TDOTMAN].bNeg       = 1;
  output[OUT_TDOTMAN].dNeg       = 1;
  output[OUT_TDOTMAN].iNum       = 1;
  output[OUT_TDOTMAN].iModuleBit = THERMINT;
  fnWrite[OUT_TDOTMAN]           = &fvWriteTDotMan;

  /* TDotCore */
  sprintf(output[OUT_TDOTCORE].cName, "TDotCore");
  sprintf(output[OUT_TDOTCORE].cDescr, "Change in Core Temperature");
  sprintf(output[OUT_TDOTCORE].cNeg, "K/s");
  output[OUT_TDOTCORE].bNeg       = 1;
  output[OUT_TDOTCORE].dNeg       = 1;
  output[OUT_TDOTCORE].iNum       = 1;
  output[OUT_TDOTCORE].iModuleBit = THERMINT;
  fnWrite[OUT_TDOTCORE]           = &fvWriteTDotCore;

  /* Constants */
  /* TrefLind */
  sprintf(output[OUT_TREFLIND].cName, "TrefLind");
  sprintf(output[OUT_TREFLIND].cDescr, "Core Liquidus Lindeman Reference");
  sprintf(output[OUT_TREFLIND].cNeg, "K");
  output[OUT_TREFLIND].bNeg       = 1;
  output[OUT_TREFLIND].dNeg       = 1;
  output[OUT_TREFLIND].iNum       = 1;
  output[OUT_TREFLIND].iModuleBit = THERMINT;
  fnWrite[OUT_TREFLIND]           = &fvWriteTrefLind;
}

/************ THERMINT Logging Functions **************/
/**
  Log options

  @param control Control struct
  @param fp File struct
*/
void fvLogOptionsThermint(CONTROL *control, FILE *fp) {
  /* Anything here?
  fprintf(fp,"-------- RADHEAT Options -----\n\n");
  */
}

// PED: this would be for global rad heat parameters, but this is blank bc rad
// is only relevant to each individual body.
/**
  Log of thermint

  @param body Body struct
  @param control Control struct
  @param output Output struct
  @param system System struct
  @param update Update struct
  @param fnWrite fnWriteOutput
  @param fp File
*/
void fvLogThermint(BODY *body, CONTROL *control, OUTPUT *output, SYSTEM *system,
                   UPDATE *update, fnWriteOutput fnWrite[], FILE *fp) {
  /* Anything here?
  int iOut;
  fprintf(fp,"\n----- RADHEAT PARAMETERS ------\n");
  for (iOut=OUTSTARTRADHEAT;iOut<OUTBODYSTARTRADHEAT;iOut++) {
    if (output[iOut].iNum > 0)
      WriteLogEntry(control,output[iOut],body,system,fnWrite[iOut],fp,update,0);
  }
  */
}
/**
  Log of body from thermint

  @param body Body struct
  @param control Control struct
  @param output Output struct
  @param system System struct
  @param update Update struct
  @param fnWrite fnWriteOutput
  @param fp File
*/
void fvLogBodyThermint(BODY *body, CONTROL *control, OUTPUT *output,
                       SYSTEM *system, UPDATE *update, fnWriteOutput fnWrite[],
                       FILE *fp, int iBody) {
  int iOut;

  fprintf(fp, "----- THERMINT PARAMETERS (%s)------\n", body[iBody].cName);
  for (iOut = OUTSTARTTHERMINT; iOut < OUTENDTHERMINT; iOut++) {
    if (output[iOut].iNum > 0) {
      WriteLogEntry(body, control, &output[iOut], system, update, fnWrite[iOut],
                    fp, iBody);
    }
  }
  /* Write out some global constants.
  fprintf(fp,"EMASS=%e EMASSMAN=%e ERMAN=%e ERCORE=%e EDMAN=%e EVOL=%e
  EVOLCORE=%e
  EVOLMAN=%e\n",EMASS,EMASSMAN,ERMAN,ERCORE,EDMAN,EVOL,EVOLCORE,EVOLMAN);
  fprintf(fp,"EDENS=%e EDENSMAN=%e EDENSCORE=%e EDENSOC=%e EDENSIC=%e
  STIFFNESS=%e\n",EDENS,EDENSMAN,EDENSCORE,EDENSOC,EDENSIC,STIFFNESS);
  fprintf(fp,"THERMEXPANMAN=%e THERMCONDUMAN=%e THERMCONDLMAN=%e
  THERMDIFFUMAN=%e
  cube(EDMAN)=%e\n",THERMEXPANMAN,THERMCONDUMAN,THERMCONDLMAN,THERMDIFFUMAN,cube(EDMAN));
  fprintf(fp,"TrefLind=%e ViscRef=%e VISCJUMPMAN=%e
  \n",body[iBody].dTrefLind,body[iBody].dViscRef,VISCJUMPMAN);
  fprintf(fp,"DTCHIREF=%e CHI_OC_E=%e PARTITION=%e CHI_IC_E=%e EMASSOC_CHI=%e
  EMASSIC_CHI=%e
  EMASSCORE_CHI=%e\n",body[iBody].dDTChiRef,CHI_OC_E,PARTITION_CHI_CORE,CHI_IC_E,EMASSOC_CHI,EMASSIC_CHI,EMASSCORE_CHI);
  fprintf(fp,"body.ViscMeltB=%e Delta=%e Gamma=%e Xi=%e Phis=%e
  \n",body[iBody].dViscMeltB,body[iBody].dViscMeltDelta,body[iBody].dViscMeltGamma,body[iBody].dViscMeltXi,body[iBody].dViscMeltPhis);
  fprintf(fp,"body.dFixMeltfactorUMan=%f .dMeltfactorUMan=%e
  \n",body[iBody].dFixMeltfactorUMan,body[iBody].dMeltfactorUMan);
  fprintf(fp,"body.dStagLid=%f dManHFlowPref=%f
  \n",body[iBody].dStagLid,body[iBody].dManHFlowPref);
  fprintf(fp,"body.dMagMomCoef=%f body.dPresSWind=%e body.dTSurf=%f
  \n",body[iBody].dMagMomCoef,body[iBody].dPresSWind,body[iBody].dTSurf);
  */
}
/**
  Add thermint module

  @param module Module struct
  @param iBody Index of body
  @param iModule Index of module
*/
void fvAddModuleThermint(CONTROL *control, MODULE *module, int iBody,
                         int iModule) {

  module->iaModule[iBody][iModule] = THERMINT;

  module->fnCountHalts[iBody][iModule]        = &fvCountHaltsThermint;
  module->fnReadOptions[iBody][iModule]       = &fvReadOptionsThermint;
  module->fnLogBody[iBody][iModule]           = &fvLogBodyThermint;
  module->fnVerify[iBody][iModule]            = &fvVerifyThermint;
  module->fnAssignDerivatives[iBody][iModule] = &fvAssignThermintDerivatives;
  module->fnNullDerivatives[iBody][iModule]   = &fvNullThermintDerivatives;
  module->fnVerifyHalt[iBody][iModule]        = &fvVerifyHaltThermint;
  module->fnVerifyHalt[iBody][iModule]        = &fvVerifyHaltThermint;

  module->fnInitializeUpdate[iBody][iModule] = &fvInitializeUpdateThermint;
  module->fnInitializeOutput[iBody][iModule] = &fvInitializeOutputThermint;

  // NEED TO ADD THERMINT VARIABLES HERE??
  module->fnFinalizeUpdateTMan[iBody][iModule] = &fvFinalizeUpdateTManThermint;
  module->fnFinalizeUpdateTCore[iBody][iModule] =
        &fvFinalizeUpdateTCoreThermint;
}

/************* THERMINT Functions ************/
/* Scalar Properties */
/* Get TUMan */
/**
  Function compute upper mantle temperature

  @param body Body struct
  @param iBody Index of body

  @return Upper mantle temperature
*/
double fdTUMan(BODY *body, int iBody) {
  return body[iBody].dAdJumpM2UM * body[iBody].dTMan;
}
/* Get TLMan */
/**
  Function compute lower mantle temperature

  @param body Body struct
  @param iBody Index of body

  @return Lower mantle temperature
*/
double fdTLMan(BODY *body, int iBody) {
  return body[iBody].dAdJumpM2LM * body[iBody].dTMan;
}
/* Get TCMB */
/**
  Function compute core-mantle boundary temperature

  @param body Body struct
  @param iBody Index of body

  @return Core-mantle boundary temperature
*/
double fdTCMB(BODY *body, int iBody) {
  return body[iBody].dAdJumpC2CMB * body[iBody].dTCore;
}
/* Get TJumpUMan */
/**
  Function compute absolute value of upper mantle temperature jump: TUman-TSURF

  @param body Body struct
  @param iBody Index of body

  @return Absolute value of upper mantle temperature jump
*/
double fdTJumpUMan(BODY *body, int iBody) {
  return fabs(body[iBody].dTUMan - body[iBody].dTSurf);
}
/* Get TJumpLMan */
/**
  Function compute absolute value of lower mantle temperature jump: TCMB-TLman

  @param body Body struct
  @param iBody Index of body

  @return Absolute value of lower mantle temperature jump
*/
double fdTJumpLMan(BODY *body, int iBody) {
  return fabs(body[iBody].dTCMB - body[iBody].dTLMan);
}
/**
  Function compute sign of upper mantle temperature jump

  @param body Body struct
  @param iBody Index of body

  @return Sign of upper mantle temperature jump
*/
double fdSignTJumpUMan(BODY *body, int iBody) {
  return (body[iBody].dTUMan - body[iBody].dTSurf) /
         fabs(body[iBody].dTUMan - body[iBody].dTSurf);
}
/**
  Function compute sign of lower mantle temperature jump

  @param body Body struct
  @param iBody Index of body

  @return Sign of lower mantle temperature jump
*/
double fdSignTJumpLMan(BODY *body, int iBody) {
  return (body[iBody].dTCMB - body[iBody].dTLMan) /
         fabs(body[iBody].dTCMB - body[iBody].dTLMan);
}
/**
  Function compute Arrhenius component of upper mantle viscosity

  @param body Body struct
  @param iBody Index of body

  @return Arrhenius component of upper mantle viscosity
*/
double fdViscUManArr(BODY *body, int iBody) {
  return body[iBody].dViscRef *
         exp(body[iBody].dActViscMan / (GASCONSTANT * body[iBody].dTUMan));
}
/**
  Function compute full upper mantle viscosity

  @param body Body struct
  @param iBody Index of body

  @return Full upper mantle viscosity
*/
double fdViscUMan(BODY *body, int iBody) {
  return body[iBody].dViscUManArr / body[iBody].dMeltfactorUMan;
}
/**
  Function compute full lower mantle viscosity

  @param body Body struct
  @param iBody Index of body

  @return Full lower mantle viscosity
*/
double fdViscLMan(BODY *body, int iBody) {
  return body[iBody].dViscUManArr * body[iBody].dViscJumpMan /
         body[iBody].dMeltfactorLMan; // w/ meltfactorLMan
}
/**
  Function compute full mid-mantle viscosity

  @param body Body struct
  @param iBody Index of body

  @return Full mid-mantle viscosity
*/
double fdViscMMan(BODY *body, int iBody) {
  return body[iBody].dViscUMan * (VISCJUMPMMAN);
}
/**
  Function compute mantle viscosity jump

  @param body Body struct
  @param iBody Index of body

  @return Mantle viscosity jump
*/
double fdViscJumpMan(BODY *body, int iBody) {
  return body[iBody].dViscJumpMan;
}
/**
  Function compute upper mantle thermal boundary layer thickness

  @param body Body struct
  @param iBody Index of body

  @return Upper mantle thermal boundary layer thickness
*/
double fdBLUMan(BODY *body, int iBody) {
  //  return
  //  (EDMAN)*pow((RACRIT)*body[iBody].dViscUMan*(THERMDIFFUMAN)/((THERMEXPANMAN)*(GRAVLMAN)*body[iBody].dTJumpLMan*cube(EDMAN)),(CONVEXPON));
  //  return
  //  (EDMAN)*pow((RACRIT)*body[iBody].dViscUMan*(THERMDIFFUMAN)/((THERMEXPANMAN)*(GRAVUMAN)*body[iBody].dTJumpUMan*cube(EDMAN)),(CONVEXPON));
  return pow((RACRIT)*body[iBody].dViscUMan * (THERMDIFFUMAN) /
                   ((THERMEXPANMAN) * (GRAVUMAN)*body[iBody].dTJumpUMan),
             (CONVEXPON));
}
/**
  Function compute lower mantle thermal boundary layer thickness

  @param body Body struct
  @param iBody Index of body

  @return Lower mantle thermal boundary layer thickness
*/
double fdBLLMan(BODY *body, int iBody) {
  return (EDMAN)*pow((RACRIT)*body[iBody].dViscLMan * (THERMDIFFLMAN) /
                           ((THERMEXPANMAN) *
                            (GRAVLMAN)*body[iBody].dTJumpLMan * cube(EDMAN)),
                     (CONVEXPON));
}
/**
  Function compute upper mantle shear modulus

  @param body Body struct
  @param iBody Index of body

  @return Upper mantle shear modulus
*/
double fdShmodUMan(BODY *body, int iBody) {
  return (body[iBody].dShModRef) *
         exp((ACTSHMODMAN) / (GASCONSTANT * body[iBody].dTUMan)) /
         body[iBody].dMeltfactorUMan;
}
/**
  Function compute solidus at bottom of upper mantle thermal boundary layer

  @param body Body struct
  @param iBody Index of body

  @return Solidus at bottom of upper mantle thermal boundary layer
*/
double fdTsolUMan(BODY *body, int iBody) {
  return fdSolidusMan(body[iBody].dBLUMan);
}
/**
  Function compute solidus at top of lower mantle thermal boundary layer

  @param body Body struct
  @param iBody Index of body

  @return Solidus at top of lower mantle thermal boundary layer
*/
double fdTsolLMan(BODY *body, int iBody) {
  return fdSolidusMan(
        ERADIUS - ERCORE -
        body[iBody].dBLLMan); // Solidus fn of depth: z_LM=R-R_c-delta_LM
}
/**
  Function compute liquidus at bottom of upper mantle thermal boundary layer

  @param body Body struct
  @param iBody Index of body

  @return Liquidus at bottom of upper mantle thermal boundary layer
*/
double fdTliqUMan(BODY *body, int iBody) {
  return body[iBody].dTsolUMan +
         DTLIQMAN; // approx constant offset btwn sol and liq.
}
/**
  Function compute liquidus at top of lower mantle thermal boundary layer

  @param body Body struct
  @param iBody Index of body

  @return Liquidus at top of lower mantle thermal boundary layer
*/
double fdTliqLMan(BODY *body, int iBody) {
  return body[iBody].dTsolLMan +
         DTLIQMAN; // approx constant offset btwn sol and liq.
}
/**
  Function compute upper mantle melt fraction

  @param body Body struct
  @param iBody Index of body

  @return Upper mantle melt fraction
*/
double fdFMeltUMan(BODY *body, int iBody) {
  double fmelt = (body[iBody].dTUMan - body[iBody].dTsolUMan) /
                 (body[iBody].dTliqUMan - body[iBody].dTsolUMan);
  double fmelt2 = min(fmelt, 1.0);  // require fmelt<1
  fmelt2        = max(fmelt2, 0.0); // require fmelt>0
  return fmelt2;
}
/**
  Function compute lower mantle melt fraction

  @param body Body struct
  @param iBody Index of body

  @return Lower mantle melt fraction
*/
double fdFMeltLMan(BODY *body, int iBody) {
  double fmelt = (body[iBody].dTLMan - body[iBody].dTsolLMan) /
                 (body[iBody].dTliqLMan - body[iBody].dTsolLMan);
  double fmelt2 = min(fmelt, 1.0);  // require fmelt<1
  fmelt2        = max(fmelt2, 0.0); // require fmelt>0
  double min1   = min(1.0, 2.0);    // test min
  double max1   = max(1.0, 2.0);    // test max
  return fmelt2;
}
/**
  Function compute upper mantle viscosity reduction factor due to melt.  If
  dFixMeltfactorUMan=1 then return dMeltfactorUMan.  Else compute it.

  @param body Body struct
  @param iBody Index of body

  @return Upper mantle viscosity reduction factor due to melt
*/
double fdMeltfactorUMan(BODY *body, int iBody) {
  /* Melt factor used in viscosity and shmod.  For fmelt=1, this should
   * give 1.20428. */
  if (body[iBody].dFixMeltfactorUMan ==
      1) { // if FixMeltFactorUMan=1 then return the initialized value.
    return body[iBody].dMeltfactorUMan;
  } else { // if FixMeltFactorUMan not =1 then compute it.
    double bigphi = body[iBody].dFMeltUMan / body[iBody].dViscMeltPhis;
    double bigf   = (1.0 - body[iBody].dViscMeltXi) *
                  erf(sqrt(PI) / (2.0 * (1.0 - body[iBody].dViscMeltXi)) *
                      bigphi * (1.0 + pow(bigphi, body[iBody].dViscMeltGamma)));
    double meltexp = body[iBody].dViscMeltB * body[iBody].dViscMeltPhis;
    return (1.0 + pow(bigphi, body[iBody].dViscMeltDelta)) /
           pow(1.0 - bigf, meltexp);
  }
}
/**
  Function compute lower mantle viscosity reduction factor due to melt.  Always
  compute it.  Only applied if ViscJumpMan is not set.

  @param body Body struct
  @param iBody Index of body

  @return Lower mantle viscosity reduction factor due to melt
*/
double fdMeltfactorLMan(BODY *body, int iBody) {
  /* Melt factor used in viscosity and shmod.  For fmelt=1, this should
   * give 1.20428. */
  double bigphi = body[iBody].dFMeltLMan / body[iBody].dViscMeltPhis;
  double bigf   = (1.0 - body[iBody].dViscMeltXi) *
                erf(sqrt(PI) / (2.0 * (1.0 - body[iBody].dViscMeltXi)) *
                    bigphi * (1.0 + pow(bigphi, body[iBody].dViscMeltGamma)));
  double meltexp = body[iBody].dViscMeltB * body[iBody].dViscMeltPhis;
  return (1.0 + pow(bigphi, body[iBody].dViscMeltDelta)) /
         pow(1.0 - bigf, meltexp);
}
/**
  Function compute depth of upper mantle melting.  Use cubic root to find
  intersection of mantle adiabat and solidus.

  @param body Body struct
  @param iBody Index of body

  @return Depth of upper mantle melting
*/
double fdDepthMeltMan(BODY *body, int iBody) {
  //    double guess1=body[iBody].dBLUMan;  //lower bound of depth to bottom of
  //    UM melt layer. double guess2=1.445e6;  //(EDMAN)/2.0;  //mid-mantle=
  //    upper bound to depth of melt layer. double tol=10.0;  //root resolution
  //    is +/- 10 m? int nmax=100;  //nmax iterations of root.
  int type = 0; // types: 0=UMan, 1=LMan, 2=ICN
  //    return root(type,body,iBody,guess1,guess2,tol,nmax);
  double depthmeltman = cubicroot(type, body, iBody);
  if (depthmeltman < body[iBody].dBLUMan) { // if solidus intersects adiabat
                                            // within UMTBL then recompute it.
    depthmeltman = cubicroot(
          1, body, iBody); // type=1 to find intersection within UMTBL.
  }
  return depthmeltman;
}
/**
  Function compute solidus temperature at depth of upper mantle melting.

  @param body Body struct
  @param iBody Index of body

  @return solidus temperature at depth of upper mantle melting
*/
double fdTDepthMeltMan(BODY *body, int iBody) {
  if (body[iBody].dDepthMeltMan == 0) { // if no melt layer found.
    return 0;
  } else { // if yes melt layer found.
    return fdSolidusMan(body[iBody].dDepthMeltMan);
  }
}
/**
  Function compute temperature jump across upper mantle melt region, excluding
  the adiabatic component (i.e. just the advective component):
  TDepthMeltMan-TSURF-ADGRADMAN*DepthMeltMan

  @param body Body struct
  @param iBody Index of body

  @return Temperature jump across upper mantle melt region
*/
double fdTJumpMeltMan(BODY *body, int iBody) {
  return body[iBody].dTDepthMeltMan - body[iBody].dTSurf -
         (ADGRADMAN)*body[iBody]
               .dDepthMeltMan; // Temp jump across entire UM melt region.
}
/**
  Function compute mantle Rayleigh number, where temperature jump is the total
  convective temperature jump across mantle (UMan+LMan) and viscosity is
  ViscMMan.

  @param body Body struct
  @param iBody Index of body

  @return Mantle Rayleigh number
*/
double fdRayleighMan(BODY *body, int iBody) {
  return body[iBody].dSignTJumpUMan * (THERMEXPANMAN) * (GRAVUMAN) *
         (body[iBody].dTJumpUMan + body[iBody].dTJumpLMan) * pow(EDMAN, 3.) /
         ((THERMDIFFUMAN)*body[iBody]
                .dViscMMan); // Mantle Rayleigh number defined in terms of
                             // ViscMMan and SignTJumpUMan.
}
/**
  Function compute upper mantle dynamic viscosity: ViscUMan*EDENSMAN.

  @param body Body struct
  @param iBody Index of body

  @return Upper mantle dynamic viscosity
*/
double fdDynamicViscosity(BODY *body, int iBody) {
  //    return body[iBody].dViscUMan*(EDENSMAN);
  return (DYNAMVISCREF)*exp(body[iBody].dActViscMan /
                            (GASCONSTANT * body[iBody].dTUMan)) /
         body[iBody].dMeltfactorUMan;
}

/* Core Chemistry */
/**
  Function compute mass of inner core

  @param body Body struct
  @param iBody Index of body

  @return Mass of inner core
*/
double fdMassIC(BODY *body, int iBody) {
  return 4. / 3 * PI * pow(body[iBody].dRIC, 3.) * (EDENSIC);
}
/**
  Function compute mass of outer core

  @param body Body struct
  @param iBody Index of body

  @return Mass of outer core
*/
double fdMassOC(BODY *body, int iBody) {
  return EMASSCORE - body[iBody].dMassIC;
}

/**
  Function compute light element concentration in outer core

  @param body Body struct
  @param iBody Index of body

  @return Light element concentration in outer core
*/
double fdChiOC(BODY *body, int iBody) {
  //  return body[iBody].dMassChiOC/body[iBody].dMassOC;
  return (EMASSCORE_CHI) /
         (body[iBody].dMassOC + body[iBody].dMassIC / (PARTITION_CHI_CORE));
}
/**
  Function compute light element concentration in inner core

  @param body Body struct
  @param iBody Index of body

  @return Light element concentration in inner core
*/
double fdChiIC(BODY *body, int iBody) {
  if (body[iBody].dRIC > 0.) {
    //    return body[iBody].dMassChiIC/body[iBody].dMassIC;
    return body[iBody].dChiOC / (PARTITION_CHI_CORE);
  } else {
    return 0.;
  }
}

/**
  Function compute light element mass in outer core

  @param body Body struct
  @param iBody Index of body

  @return Light element mass in outer core
*/
double fdMassChiOC(BODY *body, int iBody) {
  //  return EMASSCORE_CHI/(
  //  PARTITION_CHI_CORE*body[iBody].dMassIC/body[iBody].dMassOC + 1. );
  return body[iBody].dMassOC * body[iBody].dChiOC;
}
/**
  Function compute light element mass in inner core

  @param body Body struct
  @param iBody Index of body

  @return Light element mass in inner core
*/
double fdMassChiIC(BODY *body, int iBody) {
  //  return EMASSCORE_CHI-body[iBody].dMassChiOC;
  return body[iBody].dChiIC * body[iBody].dMassIC;
}


/**
  Function compute core liquidus depression due to light element concentration

  @param body Body struct
  @param iBody Index of body

  @return Core liquidus depression due to light element concentration
*/
double fdDTChi(BODY *body, int iBody) {
  return body[iBody].dDTChiRef * body[iBody].dChiOC / (CHI_OC_E);
}
/**
  Function compute inner core radius RIC

  @param body Body struct
  @param iBody Index of body

  @return Inner core radius
*/
double fdRIC(BODY *body, int iBody) {
  double dRIC;
  /* NEW VERSION with light element liquidus depression  */
  double T_fe_cen = body[iBody].dTrefLind -
                    (body[iBody].dDTChi); // Liquidus at center of core.
  double T_fe_cmb = (body[iBody].dTrefLind) *
                          exp(-2. * (1. - 1. / (3. * (GRUNEISEN))) *
                              pow((ERCORE) / (body[iBody].dDLind), 2.0)) -
                    (body[iBody].dDTChi); // Liquidus@CMB
  double numerator = 1. + pow((body[iBody].dDAdCore) / (ERCORE), 2.) *
                                log(body[iBody].dTCMB / T_fe_cen);
  if ((T_fe_cmb / T_fe_cen) < 0) {
    return 0; // for debugging only!
  }
  double denom = 1. + pow((body[iBody].dDAdCore) / (ERCORE), 2.0) *
                            log(T_fe_cmb / T_fe_cen);
  double denom2 = 1. - 2. * (1 - 1. / (3. * (GRUNEISEN))) *
                             pow(body[iBody].dDAdCore / body[iBody].dDLind, 2);
  if ((numerator / denom) > 0.) { // IC exists
    dRIC = (ERCORE)*sqrt(numerator / denom);
  } else {
    dRIC = 0; // no IC.
  }
  if (dRIC > ERCORE) {
    dRIC = ERCORE;
  }
  return dRIC;
}
/**
  Function compute thermal conductivity at top of outer core:
  ElecCondCore*LORENTZNUM*TCMB

  @param body Body struct
  @param iBody Index of body

  @return Thermal conductivity at top of outer core
*/
double fdThermConductOC(BODY *body, int iBody) {
  return (body[iBody].dElecCondCore) * (LORENTZNUM)*body[iBody].dTCMB;
}
/**
  Function compute adiabatic heat flux at top of outer core:
  ThermConductOC*TCMB*ERCORE/DAdCore^2

  @param body Body struct
  @param iBody Index of body

  @return Adiabatic heat flux at top of outer core
*/
double fdHfluxCMBAd(BODY *body, int iBody) {
  return body[iBody].dThermConductOC * body[iBody].dTCMB * (ERCORE) /
         pow(body[iBody].dDAdCore, 2.);
}
/**
  Function compute convective heat flux at top of outer core:
  HfluxCMB-HfluxCMBAd

  @param body Body struct
  @param iBody Index of body

  @return Convective heat flux at top of outer core
*/
double fdHfluxCMBConv(BODY *body, int iBody) {
  return body[iBody].dHfluxCMB - body[iBody].dHfluxCMBAd;
}
/**
  Function compute gravity at inner core boundary ICB

  @param body Body struct
  @param iBody Index of body

  @return Gravity at inner core boundary ICB
*/
double fdGravICB(BODY *body, int iBody) {
  return (GRAVCMB)*body[iBody].dRIC / (ERCORE);
}
/**
  Function compute inner core radius growth rate

  @param body Body struct
  @param iBody Index of body

  @return Inner core radius growth rate
*/
double fdRICDot(BODY *body, UPDATE *update, int iBody) {
  double denom = (2 * body[iBody].dRIC *
                  (2. * (1. - 1 / (3. * GRUNEISEN)) *
                         pow((body[iBody].dDAdCore) / (body[iBody].dDLind), 2) -
                   1.));
  if (body[iBody].dRIC > 0.) {
    return -1 * pow((body[iBody].dDAdCore), 2) / denom *
           (*(update[iBody].pdTDotCore)) / body[iBody].dTCore;
  } else {
    return 0.;
  }
}
/**
  Function compute outer core thermal buoyancy flux

  @param body Body struct
  @param iBody Index of body

  @return Outer core thermal buoyancy flux
*/
double fdCoreBuoyTherm(BODY *body, int iBody) {
  return (THERMEXPANCORE) * (GRAVCMB)*body[iBody].dHfluxCMBConv /
         ((EDENSCORE) * (SPECHEATCORE));
}
/**
  Function compute outer core compositional buoyancy flux

  @param body Body struct
  @param iBody Index of body

  @return Outer core compositional buoyancy flux
*/
double fdCoreBuoyCompo(BODY *body, int iBody) {
  return body[iBody].dGravICB * (DENSANOMICB) /
         (EDENSCORE)*pow(body[iBody].dRIC / (ERCORE), 2) * body[iBody].dRICDot;
}
/**
  Function compute outer core total buoyancy flux

  @param body Body struct
  @param iBody Index of body

  @return Outer core total buoyancy flux
*/
double fdCoreBuoyTotal(BODY *body, int iBody) {
  return body[iBody].dCoreBuoyTherm + body[iBody].dCoreBuoyCompo;
}
/**
  Function compute dipole magnetic moment

  @param body Body struct
  @param iBody Index of body

  @return Dipole magnetic moment
*/
double fdMagMom(BODY *body, int iBody) {
  double MagMom = 0.;
  if (body[iBody].dCoreBuoyTotal > 0) {
    MagMom =
          4. * PI * pow((ERCORE), 3) * body[iBody].dMagMomCoef *
          sqrt((EDENSCORE) / (2 * (MAGPERM))) *
          pow(body[iBody].dCoreBuoyTotal * ((ERCORE)-body[iBody].dRIC), 1. / 3);
  }
  return MagMom;
}
/**
  Function compute solar wind pressure

  @param body Body struct
  @param iBody Index of body

  @return Solar wind pressure
*/
double fdPresSWind(BODY *body, int iBody) {
  return body[iBody].dPresSWind; // Place holder for a proper equation later.
                                 // //(EPRESSWIND);
}
/**
  Function compute magnetopause stand-off radius from center of planet

  @param body Body struct
  @param iBody Index of body

  @return Magnetopause stand-off radius from center of planet
*/
double fdMagPauseRad(BODY *body, int iBody) {
  return pow((MAGPERM)*pow(body[iBody].dMagMom, 2) /
                   (8 * pow(PI, 2) * body[iBody].dPresSWind),
             1. / 6);
}

/* Heat Fluxes/flows */
/**
  Function compute heat flux across upper mantle thermal boundary layer

  @param body Body struct
  @param iBody Index of body

  @return Heat flux across upper mantle thermal boundary layer
*/
double fdHfluxUMan(BODY *body, int iBody) {
  return (THERMCONDUMAN)*body[iBody].dSignTJumpUMan * body[iBody].dTJumpUMan /
         body[iBody].dBLUMan;
}
/**
  Function compute heat flux across lower mantle thermal boundary layer

  @param body Body struct
  @param iBody Index of body

  @return Heat flux across lower mantle thermal boundary layer
*/
double fdHfluxLMan(BODY *body, int iBody) {
  return (THERMCONDLMAN)*body[iBody].dSignTJumpLMan * body[iBody].dTJumpLMan /
         body[iBody].dBLLMan;
}
/**
  Function compute heat flux across core-mantle boundary: same as across lower
  mantle

  @param body Body struct
  @param iBody Index of body

  @return Heat flux across core-mantle boundary
*/
double fdHfluxCMB(BODY *body, int iBody) {
  return fdHfluxLMan(body, iBody);
}
/**
  Function compute heat flow across upper mantle thermal boundary layer

  @param body Body struct
  @param iBody Index of body

  @return Heat flow across upper mantle thermal boundary layer
*/
double fdHflowUMan(BODY *body, int iBody) {
  return body[iBody].dManHFlowPref * (EAREASURF)*fdHfluxUMan(body, iBody);
}
/**
  Function compute heat flow across surface of mantle

  @param body Body struct
  @param iBody Index of body

  @return Heat flow across surface of mantle
*/
double fdHflowSurf(BODY *body, int iBody) {
  return body[iBody].dHflowUMan + body[iBody].dHflowMeltMan;
}
/**
  Function compute heat flow across lower mantle boundary layer

  @param body Body struct
  @param iBody Index of body

  @return Heat flow across lower mantle boundary layer
*/
double fdHflowLMan(BODY *body, int iBody) {
  return (EAREACMB)*fdHfluxLMan(body, iBody);
}
/**
  Function compute heat flow across core-mantle boundary (same as lower mantle)

  @param body Body struct
  @param iBody Index of body

  @return Heat flow across core-mantle boundary (same as lower mantle)
*/
double fdHflowCMB(BODY *body, int iBody) {
  return fdHflowLMan(body, iBody);
}
/**
  Function compute latent heat flow from mantle solidification

  @param body Body struct
  @param iBody Index of body

  @return Latent heat flow from mantle solidification
*/
double fdHflowLatentMan(BODY *body, UPDATE *update, int iBody) {
  double HflowLatentMan;

  // During the first WriteLog, pdTDotMan is not yet initialized! XXX

  HflowLatentMan = (-DVLIQDTEMP) * (*(update[iBody].pdTDotMan)) * (EDENSMAN) *
                   (SPECLATENTMAN);        // which structure has dTDotMan??
  HflowLatentMan = max(HflowLatentMan, 0); // ensure positive.
  return HflowLatentMan;
}
/**
  Function compute mass flux of mantle melt

  @param body Body struct
  @param iBody Index of body

  @return Mass flux of mantle melt
*/
double fdMeltMassFluxMan(BODY *body, int iBody) {
  // Should crustmass be an ODE?  Or aux prop?
  return 1.16 * (THERMDIFFUMAN) * (EAREASURF) / body[iBody].dBLUMan *
         (EDENSMAN)*body[iBody].dFMeltUMan; // DB15 (31)  =dot(M)_melt
}
/**
  Function compute heat flow of erupted mantle melt

  @param body Body struct
  @param iBody Index of body

  @return Heat flow of erupted mantle melt
*/
double fdHflowMeltMan(BODY *body, int iBody) {
  //  return
  //  body[iBody].dEruptEff*body[iBody].dMeltMassFluxMan*((SPECLATENTMAN)+(SPECHEATMAN)*body[iBody].dTJumpMeltMan);
  // PD: only specific heat lost contributes to mantle heat balance (latent heat
  // comes from melting)
  return body[iBody].dEruptEff * body[iBody].dMeltMassFluxMan *
         ((SPECHEATMAN)*body[iBody].dTJumpMeltMan);
}

/**
  Function compute secular mantle heat flow: heat sinks - sources

  @param body Body struct
  @param iBody Index of body

  @return Heat flow of mantle secular cooling
*/
/*
double fdHflowSecMan(BODY *body,int iBody) {
  return
body[iBody].dHflowUMan+body[iBody].dHflowMeltMan-body[iBody].dHflowLMan-body[iBody].dHflowLatentMan-body[iBody].dTidalPowMan-body[iBody].dRadPowerMan;
}
*/

/**
  Function compute heat flux across surface of mantle

  @param body Body struct
  @param iBody Index of body

  @return Heat flux across surface of mantle
*/
double fdHfluxSurf(BODY *body, int iBody) {
  return fdHflowSurf(body, iBody) /
         (4 * PI * body[iBody].dRadius * body[iBody].dRadius);
}
/**
  Function compute derivative of RIC wrt TCMB if IC exists.  Otherwise zero.

  @param body Body struct
  @param iBody Index of body

  @return Derivative of RIC wrt TCMB if IC exists.  Otherwise zero.
*/
double fdDRICDTCMB(BODY *body, int iBody) { //=d(R_ic)/d(T_cmb)
  if (body[iBody].dRIC > 0) {               // If IC exists.
    /* Old Version: from DB14 equations */
    double T_fe_cen = body[iBody].dTrefLind -
                      (body[iBody].dDTChi); // Liquidus at center of core.
    double T_fe_cmb = (body[iBody].dTrefLind) *
                            exp(-2. * (1. - 1. / (3. * (GRUNEISEN))) *
                                pow((ERCORE) / (body[iBody].dDLind), 2.0)) -
                      (body[iBody].dDTChi); // Liquidus@CMB
    double denom = pow((body[iBody].dDAdCore) / (ERCORE), 2.) *
                         log(T_fe_cmb / T_fe_cen) +
                   1.;
    return (1. / 2) * pow((body[iBody].dDAdCore), 2.) / body[iBody].dRIC /
           body[iBody].dTCMB / denom; // NOTES 3/16/16 -5-
  } else {                            // If no IC.
    return 0;
  }
}
/**
  Function compute time derivative of IC mass

  @param body Body struct
  @param update Update struct
  @param iBody Index of body

  @return Time derivative of IC mass if IC exists.  Otherwise zero.
*/
double fdMassICDot(BODY *body, UPDATE *update, int iBody) {
  if (body[iBody].dRIC > 0) { // If IC exists.
    double areaic = 4.0 * PI * pow(body[iBody].dRIC, 2.0);
    return areaic * (EDENSIC) * (*(update[iBody].pdTDotCore)) /
           (body[iBody].dAdJumpC2CMB) * body[iBody].dDRICDTCMB; // DB14 (31)
  } else {                                                      // If no IC.
    return 0;
  }
}


// XXX RadPowerMan should be moved to RadHeat
double fdPowerThermint(BODY *body, int iBody) {
  // double fdHflowSecManThermint(BODY *body,int iBody) {
  return body[iBody].dHflowUMan + body[iBody].dHflowMeltMan -
         body[iBody].dHflowLMan - body[iBody].dHflowLatentMan -
         body[iBody].dRadPowerMan;
}

/**
  Function compute latent heat flow from inner core solidification

  @param body Body struct
  @param update Update struct
  @param iBody Index of body

  @return Latent heat flow from inner core solidification
*/
double fdHflowLatentIC(BODY *body, UPDATE *update, int iBody) {
  if (body[iBody].dRIC > 0) {                        // If IC exists.
    return body[iBody].dMassICDot * (SPECLATENTICB); // DB14 (26)
  } else {                                           // If no IC.
    return 0;
  }
}
/**
  Function compute gravitational energy release from inner core solidification

  @param body Body struct
  @param update Update struct
  @param iBody Index of body

  @return Gravitational energy release from inner core solidification
*/
double fdPowerGravIC(BODY *body, UPDATE *update, int iBody) {
  if (body[iBody].dRIC > 0) {                        // If IC exists.
    return body[iBody].dMassICDot * (SPECPOWGRAVIC); // DB14 (26)
  } else {                                           // If no IC.
    return 0;
  }
}

/*** These derivatives are called from the udpate matrix, format is fixed. ***/
/**
  Function compute time derivative of average mantle temperature

  @param body Body struct
  @param system System struct
  @param iaBody Index of body

  @return Time derivative of average mantle temperature
*/
double fdTDotMan(BODY *body, SYSTEM *system, int *iaBody) {
  int iBody = iaBody[0];
  return -body[iBody].dHflowSecMan / ((EMASSMAN) * (SPECHEATMAN));
}
/**
  Function compute time derivative of average core temperature

  @param body Body struct
  @param system System struct
  @param iaBody Index of body

  @return Time derivative of average core temperature
*/
double fdTDotCore(BODY *body, SYSTEM *system, int *iaBody) {
  double foo;
  int iBody     = iaBody[0];
  double areaic = 4.0 * PI * pow(body[iBody].dRIC, 2.0);
  foo           = (-body[iBody].dHflowCMB + body[iBody].dRadPowerCore) /
        ((EMASSCORE) * (SPECHEATCORE)-areaic * (EDENSIC) *
         (body[iBody].dAdJumpC2CMB) * body[iBody].dDRICDTCMB *
         (SPECLATENTICB + SPECPOWGRAVIC));
  return foo;
}


/****************************************************************************/
/****************************************************************************/
/****************************************************************************/
/*********************************   MATH   *********************************/
/****************************************************************************/
/* MATH FUNCTIONS */
/**
  Function compute cube of quantity

  @param x Quantity

  @return Quantity cubed
*/
double cube(double x) {
  return pow(x, 3);
}
/**
  Function compute root of intersection of two equations using bisection method.

  @param type Type of root to compute
  @param body Body struct
  @param iBody Index of body
  @param guess1 First guess of root
  @param guess2 Second guess of root
  @param tol Tolerance of root finding
  @param nmax Maximum iteration

  @return Root
*/
double root(int type, BODY *body, int iBody, double guess1, double guess2,
            double tol, int nmax) {
  double mid     = 0.0; // current midpoint or root.
  double fmid    = 0.0; // current value of f(mid).
  double fguess1 = 0.0; // current value of f(guess1).
  int count      = 0.0;
  while (count <= nmax) {
    mid = (guess1 + guess2) / 2.0; // bisection method from wikipedia.
    if (type == 0) {               // use fdSolTempDiffMan
      fmid    = fdSolTempDiffMan(mid, body, iBody);    // function at mid.
      fguess1 = fdSolTempDiffMan(guess1, body, iBody); // function at guess1.
    }
    if (fmid == 0 || (guess2 - guess1) / 2.0 <= tol) { // solution found.
      return mid;
    }
    count++; // increment count by 1.
    if (fmid / fabs(fmid) == fguess1 / fabs(fguess1)) {
      guess2 = mid;
    } else {
      guess1 = mid;
    }
  }
  printf("method failed (nmax exceeded)\n");
  return 0;
}
/**
  Function compute cubic roots of intersection between mantle adiabat and
  solidus.

  @param type Type of root to compute
  @param body Body struct
  @param iBody Index of body

  @return Depth to solidus-adiabat intersection (melt layer)
*/
double cubicroot(int type, BODY *body, int iBody) {
  double a = 0, b = 0, c = 0, d = 0; // coefficients of cubic polynomial.
  if (type == 0) { // type=0 is melt intersection in adiabatic part of mantle,
                   // away from TBL's.
    a = ASOLIDUS;
    b = BSOLIDUS;
    c = CSOLIDUS + ADGRADMAN;
    d = DSOLIDUS - body[iBody].dTUMan -
        (ADGRADMAN) * ((ERMAN)-body[iBody].dBLUMan);
  }
  if (type == 1) { // type=1 is melt intersection within UM TBL.
    a = ASOLIDUS;
    b = BSOLIDUS;
    c = CSOLIDUS + body[iBody].dTJumpUMan / body[iBody].dBLUMan;
    d = DSOLIDUS - body[iBody].dTSurf -
        body[iBody].dTJumpUMan / body[iBody].dBLUMan * (ERMAN);
  }
  double delta0 = pow(b, 2.0) - 3.0 * a * c; // cubic root component (wikip)
  double delta1 = 2.0 * cube(b) - 9.0 * a * b * c +
                  27.0 * pow(a, 2.0) * d; // cubic root component (wikip)
  if ((pow(delta1, 2.0) - 4.0 * cube(delta0)) < 0) {
    //        printf("imaginary cubic root!\n");
    //        exit(1);
    return 0; // imaginary root implies no intersection, no melt layer?
  }
  double croot =
        pow((delta1 + sqrt(pow(delta1, 2.0) - 4.0 * cube(delta0))) / 2.0,
            1. / 3); // cubic root component (wikip)
  double root =
        -1.0 / (3.0 * a) *
        (b + croot + delta0 / croot); // real cubic root, radius of layer.
  return ERMAN - root;                // Return depth.
}
/**
  Function compute mantle solidus at a given depth

  @param depth

  @return Mantle solidus temperature at this depth
*/
double fdSolidusMan(double depth) {
  double r     = (ERMAN)-depth; // radius to bottom of region.
  double aterm = (ASOLIDUS)*pow(r, 3.);
  double bterm = (BSOLIDUS)*pow(r, 2.);
  double cterm = (CSOLIDUS)*r;
  return (ASOLIDUS)*pow(r, 3.) + (BSOLIDUS)*pow(r, 2.) + (CSOLIDUS)*r +
         (DSOLIDUS);
}
/**
  Function compute temperature difference between solidus and geotherm at a
  given depth

  @param depth
  @param body Body struct
  @param iBody Index of body

  @return Temperature difference between solidus and geotherm at a given depth
*/
double
fdSolTempDiffMan(double depth, BODY *body,
                 int iBody) { // Given a depth and BODY, return the difference
                              // between the solidus and geotherm at that depth.
  double solidus  = fdSolidusMan(depth);
  double geotherm = body[iBody].dTSurf +
                    body[iBody].dSignTJumpUMan * body[iBody].dTJumpUMan *
                          erf(2.0 * depth / body[iBody].dBLUMan); // DB14 (16)
  return solidus - geotherm;
}
/**
  Function compute surface energy flux including crustal radiogenic power:
  hflowUMan+RadPowerCrust

  @param body Body struct
  @param update Update struct
  @param system System struct
  @param iBody Index of body
  @param iFoo

  @return Surface energy flux including crustal radiogenic power
*/
double fdSurfEnFlux(BODY *body, SYSTEM *system, UPDATE *update, int iBody,
                    int iFoo) {

  /* dHFlowUMan is the energy flux at the top of the mantle, but includes
     radiogenic heating. Therefore we must subtract off the radiogenic
     heating from the core and mantle, but not the crust, which is not
     part of thermint. */

  // PD: SurfEnFlux should be Total surface power/area.  HflowUMan contains all
  // of mantle, +crust makes it total.
  return (body[iBody].dHflowUMan + body[iBody].dRadPowerCrust) /
         (4 * PI * body[iBody].dRadius * body[iBody].dRadius);
}
