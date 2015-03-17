void PropertiesAuxiliary(BODY*,CONTROL*);
double fdGetUpdateInfo(BODY*,CONTROL*,SYSTEM*,UPDATE*,fnUpdateVariable***);

void Evolve(BODY*,CONTROL*,FILES*,OUTPUT*,SYSTEM*,UPDATE*,fnUpdateVariable***,fnWriteOutput*,fnIntegrate);

void EulerStep(BODY*,CONTROL*,SYSTEM*,UPDATE*,fnUpdateVariable***,int,double *);

void RungeKutta4Step(BODY*,CONTROL*,SYSTEM*,UPDATE*,fnUpdateVariable***,int,double *);

