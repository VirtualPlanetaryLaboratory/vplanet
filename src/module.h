/**
 @file module.h

 @brief Functions to manipulate the MODULE struct.

 @author Rory Barnes ([RoryBarnes](https://github.com/RoryBarnes/))

 @date Jan 19 2015

*/

/* @cond DOXYGEN_OVERRIDE */

void InitializeControlNULL(CONTROL*,int);
void InitializeBodyNULL(BODY*,CONTROL*,UPDATE*,int,int);
void FinalizeUpdateNULL(BODY*,UPDATE*,int*,int,int,int);
void InitializeUpdateTmpBodyNULL(BODY*,CONTROL*,UPDATE*,int);
void InitializeModuleNULL(CONTROL*,MODULE*);
void PropsAuxNULL(BODY*,EVOLVE*,SYSTEM*,UPDATE*,int);

void PrintModuleList(FILE*,int);

double fndUpdateFunctionTiny(BODY*,SYSTEM*,int*);
void SetDerivTiny(fnUpdateVariable***,int,int,int);

void InitializeModule(BODY*,CONTROL*,MODULE*);
void FinalizeModule(BODY*,CONTROL*,MODULE*,int);
void AddModules(BODY*,CONTROL*,MODULE*);
double fdReturnOutputZero(BODY*,SYSTEM*,UPDATE*,int,int);
void ReadModules(BODY*,CONTROL*,FILES*,MODULE*,OPTIONS*,int);
void InitializeBodyModules(BODY**,int);

void VerifyModuleCompatability(BODY*,CONTROL*,FILES*,MODULE*,OPTIONS*,int);
void VerifyModuleMulti(BODY*,UPDATE*,CONTROL*,FILES*,MODULE*,OPTIONS*,int,fnUpdateVariable****);
void AssignEqtideStellarDerivatives(BODY*,EVOLVE*,UPDATE*,fnUpdateVariable***,int);
void NullEqtideStellarDerivatives(BODY*,EVOLVE*,UPDATE*,fnUpdateVariable***,int);

void PropsAuxEqtideThermint(BODY*,EVOLVE*,SYSTEM*,UPDATE*,int);
void PropsAuxAtmescEqtideThermint(BODY*,EVOLVE*,SYSTEM*,UPDATE*,int);
void PropsAuxDistOrbDistRot(BODY*,EVOLVE*,SYSTEM*,UPDATE*,int);
void PropsAuxRadheatThermint(BODY*,EVOLVE*,SYSTEM*,UPDATE*,int);
void PropsAuxFlareStellar(BODY*,EVOLVE*,SYSTEM*,UPDATE*,int);
void PropsAuxAtmescEqtide(BODY*,EVOLVE*,SYSTEM*,UPDATE*,int);
void PropsAuxEqtideDistorb(BODY*,EVOLVE*,SYSTEM*,UPDATE*,int);
void PropsAuxEqtideStellar(BODY*,EVOLVE*,SYSTEM*,UPDATE*,int);
void PropsAuxSpiNbodyEqtide(BODY*,EVOLVE*,SYSTEM*,UPDATE*,int);
void PropsAuxSpiNBodyDistOrb(BODY*,EVOLVE*,SYSTEM*,UPDATE*,int);
void PropsAuxMagmOcAtmEsc(BODY*,EVOLVE*,SYSTEM*,UPDATE*,int);

void ForceBehaviorAtmescEqtideThermint(BODY*,MODULE*,EVOLVE*,IO*,SYSTEM*,UPDATE*,fnUpdateVariable***,int,int);
void ForceBehaviorEqtideDistOrb(BODY*,MODULE*,EVOLVE*,IO*,SYSTEM*,UPDATE*,fnUpdateVariable***,int,int);
void ForceBehaviorEqtideAtmesc(BODY*,MODULE*,EVOLVE*,IO*,SYSTEM*,UPDATE*,fnUpdateVariable***,int,int);
void ForceBehaviorSpiNBodyAtmEsc(BODY*,MODULE*,EVOLVE*,IO*,SYSTEM*,UPDATE*,fnUpdateVariable***,int,int);
void ForceBehaviorSpiNBodyDistOrb(BODY*,MODULE*,EVOLVE*,IO*,SYSTEM*,UPDATE*,fnUpdateVariable***,int,int);


void InitializeUpdateMulti(BODY*,CONTROL*,MODULE*,UPDATE*,fnUpdateVariable****,int);
void FinalizeUpdateMulti(BODY*,CONTROL*,MODULE*,UPDATE*,fnUpdateVariable****,int*,int,int);

void InitializeUpdateEqStSemi(BODY*,UPDATE*,int);
void FinalizeUpdateMultiEqStSemi(BODY*,UPDATE*,int*,int,int,int,fnUpdateVariable****);

/* @endcond */
