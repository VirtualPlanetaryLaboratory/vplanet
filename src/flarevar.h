/**
  @file flarevar.h
  @brief Subroutines for coarse, mean-preserving stellar XUV variability.

  This module applies timestep-independent two-state modulation to stellar XUV
  luminosity while preserving the long-time mean. It is intended for testing
  bursty forcing effects without changing the baseline integrated XUV energy.
*/

#define FLAREVAR_MODEL_PERIODIC 0
#define FLAREVAR_MODEL_RANDOM_BIN 1

/* Options Info */
#define OPTSTARTFLAREVAR 2340 /* Start of FLAREVAR options */
#define OPTENDFLAREVAR 2400   /* End of FLAREVAR options */

#define OPT_FLAREVARMODEL 2341
#define OPT_FLAREVARDUTY 2342
#define OPT_FLAREVARAMPHIGH 2343
#define OPT_FLAREVARDTBIN 2344
#define OPT_FLAREVARSEED 2345
#define OPT_FLAREVART0 2346

/* Output Info */
#define OUTSTARTFLAREVAR 2340
#define OUTENDFLAREVAR 2400

#define OUT_FLAREVARMULT 2341
#define OUT_LXUVBASE 2342
#define OUT_LXUVTOTAL 2343

/* @cond DOXYGEN_OVERRIDE */

void AddModuleFlareVar(CONTROL *, MODULE *, int, int);
void BodyCopyFlareVar(BODY *, BODY *, int, int, int);
void InitializeBodyFlareVar(BODY *, CONTROL *, UPDATE *, int, int);
void InitializeUpdateTmpBodyFlareVar(BODY *, CONTROL *, UPDATE *, int);

void InitializeOptionsFlareVar(OPTIONS *, fnReadOption[]);
void ReadOptionsFlareVar(BODY *, CONTROL *, FILES *, OPTIONS *, SYSTEM *,
                         fnReadOption[], int);

void VerifyFlareVar(BODY *, CONTROL *, FILES *, OPTIONS *, OUTPUT *, SYSTEM *,
                    UPDATE *, int, int);
void fnPropsAuxFlareVar(BODY *, EVOLVE *, IO *, UPDATE *, int);
void fnForceBehaviorFlareVar(BODY *, MODULE *, EVOLVE *, IO *, SYSTEM *,
                             UPDATE *, fnUpdateVariable ***, int, int);
void AssignFlareVarDerivatives(BODY *, EVOLVE *, UPDATE *,
                               fnUpdateVariable ***, int);
void NullFlareVarDerivatives(BODY *, EVOLVE *, UPDATE *,
                             fnUpdateVariable ***, int);

void InitializeUpdateFlareVar(BODY *, UPDATE *, int);
void CountHaltsFlareVar(HALT *, int *);
void VerifyHaltFlareVar(BODY *, CONTROL *, OPTIONS *, int, int *);

void InitializeOutputFlareVar(OUTPUT *, fnWriteOutput[]);
void WriteFlareVarMult(BODY *, CONTROL *, OUTPUT *, SYSTEM *, UNITS *, UPDATE *,
                       int, double *, char **);
void WriteLXUVBaseFlareVar(BODY *, CONTROL *, OUTPUT *, SYSTEM *, UNITS *,
                           UPDATE *, int, double *, char **);
void WriteLXUVTotalFlareVar(BODY *, CONTROL *, OUTPUT *, SYSTEM *, UNITS *,
                            UPDATE *, int, double *, char **);

void LogOptionsFlareVar(CONTROL *, FILE *);
void LogBodyFlareVar(BODY *, CONTROL *, OUTPUT *, SYSTEM *, UPDATE *,
                     fnWriteOutput[], FILE *, int);

double fdFlareVarMultiplier(BODY *, EVOLVE *, int);

/* @endcond */
