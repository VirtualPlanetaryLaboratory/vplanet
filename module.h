void InitializeModule(MODULE*,int);
void FinalizeModule(BODY*,MODULE*,int);
double fdReturnOutputZero(BODY*,SYSTEM*,UPDATE*,int,int);
void ReadModules(BODY*,CONTROL*,FILES*,OPTIONS*,int);

void VerifyModuleMulti(BODY*,CONTROL*,FILES*,MODULE*,OPTIONS*,int);

void PropsAuxEqtideThermint(BODY*,UPDATE*,int);
void PropsAuxLagrangeLaskar(BODY*,UPDATE*,int);
void PropsAuxRadheatThermint(BODY*,UPDATE*,int);

