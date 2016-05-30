void  InitializeControlNULL(CONTROL*);
void InitializeBodyNULL(BODY*,CONTROL*,UPDATE*,int,int);
void FinalizeUpdateNULL(BODY*,UPDATE*,int*,int,int,int);
void InitializeUpdateTmpBodyNULL(BODY*,CONTROL*,UPDATE*,int);
void InitializeModuleNULL(CONTROL*,MODULE*);
void PrintModuleList(FILE*,int);

double fdUpdateFunctionTiny(BODY*,SYSTEM*,int*);
void SetDerivTiny(fnUpdateVariable***,int,int,int);

void InitializeModule(MODULE*,int);
void FinalizeModule(BODY*,MODULE*,int);
double fdReturnOutputZero(BODY*,SYSTEM*,UPDATE*,int,int);
void ReadModules(BODY*,CONTROL*,FILES*,MODULE*,OPTIONS*,int);
void InitializeBodyModules(BODY**,int);

void VerifyModuleMulti(BODY*,CONTROL*,FILES*,MODULE*,OPTIONS*,int);

void PropsAuxEqtideThermint(BODY*,UPDATE*,int);
void PropsAuxDistOrbDistRot(BODY*,UPDATE*,int);
void PropsAuxRadheatThermint(BODY*,UPDATE*,int);
void PropsAuxFlareStellar(BODY*,UPDATE*,int);

void ForceBehaviorEqtideDistOrb(BODY*,EVOLVE*,IO*,SYSTEM*,UPDATE*,fnUpdateVariable***,int,int);
