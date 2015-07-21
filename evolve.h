void PropsAuxNULL(BODY*,UPDATE*,int);
void PropertiesAuxiliary(BODY*,CONTROL*,UPDATE*);
double fdGetUpdateInfo(BODY*,CONTROL*,SYSTEM*,UPDATE*,fnUpdateVariable***);

void Evolve(BODY*,CONTROL*,FILES*,OUTPUT*,SYSTEM*,UPDATE*,fnUpdateVariable***,fnWriteOutput*,fnIntegrate);

void EulerStep(BODY*,CONTROL*,SYSTEM*,UPDATE*,fnUpdateVariable***,double*,int);

void RungeKutta4Step(BODY*,CONTROL*,SYSTEM*,UPDATE*,fnUpdateVariable***,double*,int);

