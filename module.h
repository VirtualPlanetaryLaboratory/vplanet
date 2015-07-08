void InitializeModule(MODULE*,int);
void FinalizeModule(BODY*,MODULE*,int);
double fdReturnOutputZero(BODY*,SYSTEM*,UPDATE*,int,int);
void ReadModules(BODY*,CONTROL*,FILES*,OPTIONS*,int);

void VerifyModuleMulti(BODY*,CONTROL*,FILES*,MODULE*,OPTIONS*,int);

void PropertiesLagrangeLaskar(BODY*,UPDATE*,int);
void PropertiesRadheatThermint(BODY*,UPDATE*,int);

