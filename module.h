void InitializeControlNULL(CONTROL*,int);
void InitializeBodyNULL(BODY*,CONTROL*,UPDATE*,int,int);
void FinalizeUpdateNULL(BODY*,UPDATE*,int*,int,int,int);
void InitializeUpdateTmpBodyNULL(BODY*,CONTROL*,UPDATE*,int);
void InitializeModuleNULL(CONTROL*,MODULE*);
void PropsAuxNULL(BODY*,EVOLVE*,UPDATE*,int);

void PrintModuleList(FILE*,int);

double fdUpdateFunctionTiny(BODY*,SYSTEM*,int*);
void SetDerivTiny(fnUpdateVariable***,int,int,int);

void InitializeModule(MODULE*,int);
void FinalizeModule(BODY*,MODULE*,int);
double fdReturnOutputZero(BODY*,SYSTEM*,UPDATE*,int,int);
void ReadModules(BODY*,CONTROL*,FILES*,MODULE*,OPTIONS*,int);
void InitializeBodyModules(BODY**,int);

void VerifyModuleMulti(BODY*,UPDATE*,CONTROL*,FILES*,MODULE*,OPTIONS*,int,fnUpdateVariable****);

void PropsAuxEqtideThermint(BODY*,EVOLVE*,UPDATE*,int);
void PropsAuxAtmescEqtideThermint(BODY*,EVOLVE*,UPDATE*,int);
void PropsAuxDistOrbDistRot(BODY*,EVOLVE*,UPDATE*,int);
void PropsAuxRadheatThermint(BODY*,EVOLVE*,UPDATE*,int);
void PropsAuxFlareStellar(BODY*,EVOLVE*,UPDATE*,int);
void PropsAuxAtmescEqtide(BODY*,EVOLVE*,UPDATE*,int);
void PropsAuxEqtideDistorb(BODY*,EVOLVE*,UPDATE*,int);
void PropsAuxEqtideStellar(BODY*,EVOLVE*,UPDATE*,int);
void PropsAuxSpinbodyEqtide(BODY*,EVOLVE*,UPDATE*,int);

void ForceBehaviorAtmescEqtideThermint(BODY*,EVOLVE*,IO*,SYSTEM*,UPDATE*,fnUpdateVariable***,int,int);
void ForceBehaviorEqtideDistOrb(BODY*,EVOLVE*,IO*,SYSTEM*,UPDATE*,fnUpdateVariable***,int,int);

void InitializeUpdateMulti(BODY*,CONTROL*,MODULE*,UPDATE*,fnUpdateVariable****,int);
void FinalizeUpdateMulti(BODY*,CONTROL*,MODULE*,UPDATE*,fnUpdateVariable****,int*,int,int);

void InitializeUpdateEqBinStSemi(BODY*,UPDATE*,int);
void InitializeUpdateEqBinStLostEng(BODY*,UPDATE*,int);
void FinalizeUpdateMultiEqBinStSemi(BODY*,UPDATE*,int*,int,int,int,fnUpdateVariable****);
void FinalizeUpdateMultiEqBinStLostEng(BODY*,UPDATE*,int*,int,int,int,fnUpdateVariable****);
