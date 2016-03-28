void InitializeModule(MODULE*,int);
void FinalizeModule(BODY*,MODULE*,int);
double fdReturnOutputZero(BODY*,SYSTEM*,UPDATE*,int,int);
void ReadModules(BODY*,CONTROL*,FILES*,OPTIONS*,int);
void InitializeBodyModules(BODY**,int);

void VerifyModuleMulti(BODY*,CONTROL*,FILES*,MODULE*,OPTIONS*,int);

void PropsAuxEqtideThermint(BODY*,UPDATE*,int);
void PropsAuxDistOrbDistRot(BODY*,UPDATE*,int);
void PropsAuxRadheatThermint(BODY*,UPDATE*,int);
void PropsAuxFlareStellar(BODY*,UPDATE*,int);

void ForceBehaviorEqtideDistOrb(BODY*,EVOLVE*,IO*,SYSTEM*,UPDATE*,int,int);
