/***************** THERMINT.H *********************** 
 *
 * Peter Driscoll 6/1/15
 *
 * This header file contains all the subroutines in
 * file thermint.c.
 *
*/

// GLOBAL VARIABLE INDEXES
#define TMAN             20                    //index of TMAN variable.
#define TCORE            21                    //index of TCORE variable.

// !!!HACK!!!  Assume a constant surf temp for now.
#define TSURF  300.0  
// UNITS CONSTANTS
#define KM               1e3       //[m] 1 km in m

// FUNDAMENTAL CONSTANTS
#define GASCONSTANT      8.3144621             //[J/mol/K] gas constant in SI units.      

// E MASS RESERVOIRS
#define EMASS            5.9742e24             //[kg] mass of E
#define EMASSFRACMAN     0.67                  //mass fraction of E mantle
#define EMASSMAN         EMASSFRACMAN*EMASS    //[kg] mass of E mantle
#define EMASSFRACCORE    0.32                  //mass frac E core
#define EMASSCORE        EMASSFRACCORE*EMASS   //[kg] mass of E core
#define EMASSIC          9.9124e22             //[kg] mass of present IC.
#define EMASSOC          EMASSCORE-EMASSIC     //[kg] mass of present OC.
#define EMASSOCEAN       1.4e21                //[kg] mass of E oceans
#define EMASSFRACOCEAN   EMASSOCEAN/EMASS      //ocean mass frac
#define EMASSFRACCRUST   1-EMASSFRACMAN-EMASSFRACCORE-EMASSFRACOCEAN  //crust is remaining bit of mass.
#define EMASSCRUST       EMASSFRACCRUST*EMASS  //[kg] mass E crust today.

// INTERNAL STRUCTURE E
#define ERADIUS          6371.0*KM          //[m] radius of E.
#define ERMAN            6371.0*KM          //[m] radius of E mantle.
#define ERCORE           3481.0*KM          //[m] radius Core
#define ERICB            1221.0*KM          //[m] present day ICB radius.
#define EDMAN            ERADIUS-ERCORE     //[m] mantle shell thickness
#define EDOC             ERCORE-ERICB       //[m] present day core shell thickness
#define EVOL             (4.0)/(3)*PI*ERADIUS*ERADIUS*ERADIUS //[m^3] volume of E.
#define EVOLIC           (4.0)/(3)*PI*ERICB*ERICB*ERICB   //[m^3] volume of present-day IC.
#define EVOLCORE         (4.0)/(3)*PI*ERCORE*ERCORE*ERCORE  //[m^3] volume of total core.
#define EVOLOC           EVOLCORE-EVOLIC    //[m^3] volume of OC today.
#define EVOLMAN          EVOL-EVOLCORE      //[m^3] volume of Mantle.
#define EAREASURF        4.0*PI*ERADIUS*ERADIUS //[m2] area of surface
#define EAREACMB         4.0*PI*ERCORE*ERCORE   //[m2] area of CMB.
// DENSITIES - derived from mass and radius dfns.
#define EDENS            (EMASS)/(EVOL)         //[kg/m^3] density of E
#define EDENSMAN         (EMASSMAN)/(EVOLMAN)   //[kg/m^3] density of E mantle.
#define EDENSCORE        (EMASSCORE)/(EVOLCORE) //[kg/m^3] density of E core.
#define EDENSOC          (EMASSOC)/(EVOLOC)     //[kg/m^3] density of E core.
#define EDENSIC          (EMASSIC)/(EVOLIC)     //[kg/m^3] density of E IC.
#define DENSANOMICB      700.0                  //[kg/m^3] density diff of OC light elements.
// Gravity
#define GRAVSURF         9.8                //[m/s2] Surf gravity in SI
#define GRAVUMAN         10.5               //[m/s2] UM gravity in SI
#define GRAVLMAN         10.5               //[m/s2] LM gravity in SI
#define GRAVCMB          10.6               //[m/s2] CMB gravity

/* THERMAL PROPERTIES */
#define SPECHEATMAN      1265.0             //[J/kg/K] specfic heat mantle.
#define SPECHEATCORE     840.0              //[J/kg/K] sp heat core.
#define THERMEXPANMAN    3e-5               //[K^-1] thermal expansivity of mantle.
#define THERMEXPANCORE   1e-5               //[K^-1] thermal expansivity of core.
#define THERMCONDUMAN    4.2                //[W/m/K] thermal conductivity of Upper mantle.
#define THERMCONDLMAN    10.0               //[W/m/K] thermal conductivity of Lower mantle.
#define THERMDIFFUMAN    THERMCONDUMAN/(EDENSMAN*SPECHEATMAN) //[m^2/s] thermal diffusivity mantle.
#define THERMDIFFLMAN    THERMDIFFUMAN
#define SPECLATENTICB    750e3              //[J/kg] specific latent heat release at ICB
#define SPECLATENTMAN    320e3              //[J/kg] specific latent heat release by mantle solidification.
#define SPECPOWGRAVIC    3e5           //[J/kg]  Gravitational power release during IC growth.
/* CONVECTION CONSTANTS */
#define RACRIT           660.          //[-] critical rayleigh number for spherical convection.
#define CONVEXPON        1./3          //[nd] convective cooling nusselt exponent "beta"
#define HFLOWREDUCTSTAG  1./25         //[nd] mantle surface heat flow reduction factor for stagnant lid.
#define STAGLID          0.            //[nd] switch to turn stagnant lid on.  (def=0, ie mobile lid)
#define MANHFLOWPREF     1.            //[nd] UM heat flow prefix.  mobile lid=1.  staglid=HFLOWREDUCTSTAG. 
/* VISCOSITY PROPERTIES */
#define ACTVISCMAN       3e5           //[J/mol] viscosity activation energy mantle
#define ACTSHMODMAN      2e5           //[J/mol] shear modulus activation energy mantle
#define VISCREF          5e7           //[m2/s] reference kinematic mantle viscosity, def ViscRef.
#define VISCJUMPMAN      2.7           //[nd] viscosity jump from upper to lower mantle, def ViscJumpMan.
#define FIXVISCJUMPMAN   0             //[nd] (default) option to fix viscjumpulm. if =0 then viscLM is computed from TLMan.
#define VISCJUMPMMAN     10.           //[nd] viscosity jump from upper to average (mid) mantle.
#define SHMODREF         6.24e4        //[Pa] reference kinematic mantle shear modulus
#define VISCMELTB        2.5           //[nd] viscosity-melt reduction coefficient "B" (DB15 eq 8)
#define VISCMELTPHIS     0.8           //[nd] viscosity-melt reduction coefficient "phi*" (DB15 eq 8)
#define VISCMELTDELTA    6.0           //[nd] viscosity-melt reduction coefficient "delta" (DB15 eq 8)
#define VISCMELTGAMMA    6.0           //[nd] viscosity-melt reduction coefficient "gamma" (DB15 eq 9)
#define VISCMELTXI       5e-4          //[nd] viscosity-melt reduction coefficient "Xi" (DB15 eq 9)
#define MELTFACTORUMAN   1.0           //[nd] (Default) viscosity-melt reduction factor "epsilon_phase"
#define FIXMELTFACTORUMAN 0.0          //[nd] (Default) switch to fix MeltfactorUMan to a constant value.
/* TIDAL PROPERTIES */
#define STIFFNESS        3.1217e11     //1.71e4*1e9    //[Pa] effective stiffness of mantle (calibrated to k2=0.3, Q=100)
/* MELTING CONSTANTS */
#define ASOLIDUS         -1.160e-16    //[K/m3] mantle solidus coefficient Tsol(r)=A*r^3+B*r^2+C*r+D
#define BSOLIDUS         +1.708e-9     //[K/m2] B coefficient
#define CSOLIDUS         -9.074e-3     //[K/m] C coefficient
#define DSOLIDUS         +1.993e4      //[K] D coefficient
#define DTLIQMAN         +500.0        //[K] mantle liquidus offset, T_liq=T_sol+DTLIQMAN
#define DVLIQDTM         +8e17         //[m3/K] change in mantle liquid volume with T_m approximated (Driscoll&B 2015)
#define DLIND            7000.0*KM     //[m] lindemann's law length scale for iron liquidus "D_Fe" (DB15 A23)
#define TREFLIND         5705.0        //[K] lindemann's law reference temp. "T_Fe0" (DB15 A23)
#define DVLIQDTEMP       8e17          //[m^3/K] approximation of mantle DV_liq/DT.
#define ERUPTEFF         0.1           //[nd] Default eruption efficiency.
/* Continental Crust */
#define ECRUSTMASS       0.0035*(MEARTH)  //[kg]  =0.35% of M_earth (Taylor 95)
#define ECRUSTDENSITY    2800.         //[kg/m3]
#define ECRUSTAREA       0.412*(EAREASURF)  //[m2] =41.2% of Earth surface (Taylor 95)
#define ECRUSTDEPTH      (ECRUSTMASS)/((ECRUSTDENSITY)*(ECRUSTAREA)) //[m] ave depth of CC today ~ 36 km (Taylor 95)
#define CRUSTACCRFRAC    0.015         //[nd] constant fraction of oceanic crust accreted onto continent.
#define CRUSTINSOFACT    (65.)/(94.)   //[nd] reduction in hflux through contintental crust today (Jaupart 07, table 3)
/* CORE CHEMISTRY */
#define DTCHIREF         300.          //[K] DT_chi>0. liquidus depression present E.
#define CHI_OC_E         0.18          //[nd] OCore light element concentration present E.
#define PARTITION_CHI_CORE (ERICB)/(ERCORE) //[nd] Core Le Partition coefficent=R_ic_e/r_e
#define CHI_IC_E         PARTITION_CHI_CORE*CHI_OC_E //[nd] IC Le concentration in E  (CHI_IC_E=partition*CHI_OC_E)
#define EMASSOC_CHI      CHI_OC_E*EMASSOC //[kg] Mass of Chi in OC in E
#define EMASSIC_CHI      CHI_IC_E*EMASSIC //[kg] Mass of Chi in IC in E
#define EMASSCORE_CHI    EMASSOC_CHI+EMASSIC_CHI  //[kg] total Chi mass OC+IC in E (conserved)
/* ADIABATIC PROPERTIES */
#define ADGRADMAN        (0.5)/(KM)    //[K/m] mantle linear adiabatic gradient =0.5K/km  (DB15 eq A18)
#define DADCORE          6340.0*KM     //[m] liq iron core adiabatic length scale (DB15 eq A22)
#define GRUNEISEN        1.3           //[nd] core gruneisen parameter
#define ADJUMPM2UM       0.7           //[nd] adiabatic temp jump from ave mantle to UM. "epsilon_UM"
#define ADJUMPM2LM       1.3           //[nd] adiabatic temp jump from ave mantle to LM. "epsilon_LM"
#define ADJUMPC2CMB      0.8           //[nd] adiabatic temp jump from ave core to CMB. "epsilon_c"
/* MAGNETIC DYNAMO PROPERTIES */
#define MAGPERM          4*PI*1e-7     //[H/m] magnetic permeability constant in SI units.
#define MAGMOMCOEF       0.2           //[nd] saturation constant for fast rot dipolar dynamos (OC2006)
#define ELECCONDCORE     10e5          //[S/m]  electrical conductivity of core. 
#define LORENTZNUM       2.5e-8        //[W Ohm/K] lorentz number, relates thermal and electrical conductivity.
#define EMAGMOM          80e21         //[Am2] Earth's present day magnetic moment.
#define EPRESSWIND       2.6761e-9     //[N/m2] Earth's SW pressure: Psw=m_proton*n_sw*v_sw^2 (DB13)
#define EMAGPAUSERAD     9.103*(ERADIUS)  //[m] Earth's magnetopause radius (DB13)

void InitializeControlThermint(CONTROL*);
void AddModuleThermint(MODULE*,int,int);
void BodyCopyThermint(BODY*,BODY*,int,int,int);
void InitializeBodyEqtide(BODY*,CONTROL*,UPDATE*,int,int);
void InitializeUpdateTmpBodyThermint(BODY*,CONTROL*,UPDATE*,int);

/* Options Info */

#define OPTSTARTTHERMINT    1700 /* Start of THERMINT options */
#define OPTENDTHERMINT      1900 /* End of THERMINT options */

/* Scalar Properties */
#define OPT_TMAN            1710   //Temperature Mantle AVE
#define OPT_TUMAN           1711   //Temperature UMTBL
#define OPT_TLMAN           1712   //Temperature LMTBL
#define OPT_TCORE           1713   //Temperature Core AVE
#define OPT_TCMB            1714   //Temperature CMB
#define OPT_TICB            1715   //Temperature ICB
#define OPT_BLUMAN          1716   //UM TBL thickness
#define OPT_BLLMAN          1717   //LM TBL thickness
#define OPT_TJUMPUMAN       1718   //Temperature Jump across UMTBL
#define OPT_TJUMPLMAN       1719   //Temperature Jump across LMTBL
#define OPT_VISCUMAN        1720   //Viscosity UMTBL
#define OPT_VISCLMAN        1721   //Viscosity LMTBL
#define OPT_SHMODUMAN       1722   //Shear modulus UMTBL
#define OPT_SHMODLMAN       1723   //Shear modulus LMTBL
#define OPT_FMELTUMAN       1724   //Melt fraction UMTBL
#define OPT_FMELTLMAN       1725   //Melt fraction LMTBL
#define OPT_MELTFACTORUMAN  1726   //Melt fraction UMTB
#define OPT_FIXMELTFACTORUMAN 1727   //Option to Fix Melt fraction UMTBLL
#define OPT_MELTFACTORLMAN  1728   //Melt fraction LMTBL
#define OPT_DEPTHMELTMAN    1729   //Depth to base of UM melt region.
#define OPT_TDEPTHMELTMAN   1730   //Temp at base of UM melt region.
#define OPT_TJUMPMELTMAN    1731   //Temp jump across UM melt region.
#define OPT_K2MAN           1732   //Mantle k2 love number
#define OPT_IMK2MAN         1733   //Mantle Im(k2) love number
#define OPT_VISCUMANARR     1736   //Viscosity UM Arrhenius
#define OPT_VISCMMAN        1737   //Viscosity Mid (ave) mantle.
/* Time Derivatives & Gradients */
#define OPT_TDOTMAN         1740   //Time deriv of mean mantle temp
#define OPT_TDOTCORE        1741   //time deriv of mean core temp
#define OPT_HFLUXUMAN       1742   //hflux upper mantle thermal boundary layer (UMTBL)
#define OPT_HFLOWUMAN       1743   //hflow UMTBL
#define OPT_HFLUXLMAN       1744   //hflux upper mantle thermal boundary layer (UMTBL)
#define OPT_HFLOWLMAN       1745   //hflow UMTBL
#define OPT_HFLUXCMB        1746   //hflux lower mantle TBL = CMB
#define OPT_HFLOWCMB        1747   //hflow LMTBL=CMB
#define OPT_HFLOWTIDALMAN   1748   //hflow tidal dissipation in mantle
#define OPT_HFLOWTIDALCORE  1749   //hflow tidal dissipation in core
#define OPT_HFLOWLATENTMAN  1750   //latent hflow from solidification of mantle
#define OPT_HFLOWMELTMAN    1751   //Melt hflow from mantle
#define OPT_HFLOWLATENTIC   1752   //latent hflow from solidification of IC
#define OPT_POWERGRAVIC     1753   //Gravitation Power release from solidification of IC
#define OPT_HFLOWICB        1754   //hflow across ICB
#define OPT_HFLUXSURF       1760   //hflux surface of mantle
#define OPT_HFLOWSURF       1761   //hflow surface of mantle
#define OPT_TIDALPOWMAN     1762   //Tidal Power Mantle
/* Halts */
#define OPT_HALTMINTMAN     1763   //Minimum mantle temperature
#define OPT_HALTMINTCORE    1764   //Minimum core temperature
/* Stagnant Lid */
#define OPT_STAGLID         1765   //Stagnant lid switch
#define OPT_MANHFLOWPREF    1766   //Stagnant lid switch
/* Stellar Wind */
#define OPT_PRESSWIND       1767   //Stellar wind pressure
/* Core Variables */
#define OPT_RIC             1770   //IC radius
#define OPT_DOC             1771   //OC shell thickness
#define OPT_CHIOC           1780   //OC light element concentration chi.
#define OPT_CHIIC           1781   //IC light element concentration chi.
#define OPT_THERMCONDUCTOC  1782   //Thermal conductivity OC
#define OPT_THERMCONDUCTIC  1783   //Thermal conductivity IC
#define OPT_MASSOC          1784   //OC Mass
#define OPT_MASSIC          1785   //IC Mass
#define OPT_MASSCHIOC       1786   //OC chi Mass
#define OPT_MASSCHIIC       1787   //IC chi Mass
#define OPT_DTCHI           1788   //Core Liquidus Depression
/* Dynamo Parameters */
#define OPT_MAGMOMCOEF      1789   //Magnetic Moment Scaling Coef (gamma_dip)
/* Constants */
#define OPT_VISCJUMPMAN     1790   //Viscosity ratio UM 2 LM
#define OPT_ERUPTEFF        1791   //Mantle Melt Eruption Efficiency
#define OPT_VISCREF         1792   //Reference Viscosity
#define OPT_TREFLIND        1793   //Reference Temperature Lindemann Core Liquidus
#define OPT_DTCHIREF        1794   //Reference Core Liquidus Depression
#define OPT_VISCMELTB       1795   //Viscosity Melt Factor B
#define OPT_VISCMELTPHIS    1796   //Viscosity Melt Factor Phi*
#define OPT_VISCMELTXI      1797   //Viscosity Melt Factor Xi
#define OPT_VISCMELTGAMMA   1798   //Viscosity Melt Factor Gamma
#define OPT_VISCMELTDELTA   1799   //Viscosity Melt Factor Delta

/* Options Functions */
void HelpOptionsThermint(OPTIONS*);
void ReadTMan(BODY*,CONTROL*,FILES*,OPTIONS*,SYSTEM*,int) ;
void ReadTCore(BODY*,CONTROL*,FILES*,OPTIONS*,SYSTEM*,int) ;
void ReadViscJumpMan(BODY*,CONTROL*,FILES*,OPTIONS*,SYSTEM*,int) ;
void ReadViscRef(BODY*,CONTROL*,FILES*,OPTIONS*,SYSTEM*,int) ;
void ReadViscMeltB(BODY*,CONTROL*,FILES*,OPTIONS*,SYSTEM*,int) ;
void ReadViscMeltPhis(BODY*,CONTROL*,FILES*,OPTIONS*,SYSTEM*,int) ;
void ReadViscMelGamma(BODY*,CONTROL*,FILES*,OPTIONS*,SYSTEM*,int) ;
void ReadViscMeltXi(BODY*,CONTROL*,FILES*,OPTIONS*,SYSTEM*,int) ;
void ReadViscMeltDelta(BODY*,CONTROL*,FILES*,OPTIONS*,SYSTEM*,int) ;
void ReadMeltfactorUMan(BODY*,CONTROL*,FILES*,OPTIONS*,SYSTEM*,int) ;
void ReadFixMeltfactorUMan(BODY*,CONTROL*,FILES*,OPTIONS*,SYSTEM*,int) ;
void ReadTrefLind(BODY*,CONTROL*,FILES*,OPTIONS*,SYSTEM*,int) ;
void ReadDTChiRef(BODY*,CONTROL*,FILES*,OPTIONS*,SYSTEM*,int) ;
void ReadEruptEff(BODY*,CONTROL*,FILES*,OPTIONS*,SYSTEM*,int) ;
void ReadHaltMinTMan(BODY*,CONTROL*,FILES*,OPTIONS*,SYSTEM*,int) ;
void ReadHaltMinTCore(BODY*,CONTROL*,FILES*,OPTIONS*,SYSTEM*,int) ;
void ReadStagLid(BODY*,CONTROL*,FILES*,OPTIONS*,SYSTEM*,int) ;
void ReadManHFlowPref(BODY*,CONTROL*,FILES*,OPTIONS*,SYSTEM*,int) ;
void ReadMagMomCoef(BODY*,CONTROL*,FILES*,OPTIONS*,SYSTEM*,int) ;
void ReadPresSWind(BODY*,CONTROL*,FILES*,OPTIONS*,SYSTEM*,int) ;

void InitializeOptionsThermint(OPTIONS*,fnReadOption[]);
void ReadOptionsThermint(BODY*,CONTROL*,FILES*,OPTIONS*,SYSTEM*,fnReadOption[],int);

/* Halt Functions */
#define THERMINTHALTSYSEND       6
#define THERMINTHALTBODYEND      6

int fbHaltMinTMan(BODY*,EVOLVE*,HALT*,IO*,UPDATE*,int);  //syntax now is same as fnHaltModule in vplanet.h
int fbHaltMinTCore(BODY*,EVOLVE*,HALT*,IO*,UPDATE*,int);
void CountHaltsThermint(HALT*,int*);

/* Verify Functions */

void VerifyThermint(BODY*,CONTROL*,FILES*,OPTIONS*,OUTPUT*,SYSTEM*,UPDATE*,fnUpdateVariable***,int,int);
void VerifyRotationThermint(BODY*,CONTROL*,OPTIONS*,char[],int);
void InitializeVplanetThermint(CONTROL*,MODULE*);
void VerifyHaltThermint(BODY*,CONTROL*,OPTIONS*,int,int*);
void VerifyRotationThermint(BODY*,CONTROL*,OPTIONS*,char[],int);

/* Update functions */
void InitializeUpdateThermint(BODY*,UPDATE*,int);

/* Auxiliary Properties */
void PropsAuxThermint(BODY*,EVOLVE*,UPDATE*,int);

/* Output Functinos */

/* THERMINT */
#define OUTSTARTTHERMINT         1700
#define OUTENDTHERMINT           1900

/* Output Body Properties */
/* Scalar Properties */
#define OUT_TMAN            1710   //Temperature Mantle AVE
#define OUT_TUMAN           1711   //Temperature UMTBL
#define OUT_TLMAN           1712   //Temperature LMTBL
#define OUT_TCORE           1713   //Temperature Core AVE
#define OUT_TCMB            1714   //Temperature CMB
#define OUT_TICB            1715   //Temperature ICB
#define OUT_BLUMAN          1716   //UM TBL thickness
#define OUT_BLLMAN          1717   //LM TBL thickness
#define OUT_TJUMPUMAN       1718   //Temperature Jump across UMTBL
#define OUT_TJUMPLMAN       1719   //Temperature Jump across LMTBL
#define OUT_SIGNTJUMPUMAN   1720   //Temperature Jump across UMTBL
#define OUT_SIGNTJUMPLMAN   1721   //Temperature Jump across LMTBL
#define OUT_VISCUMAN        1722   //Viscosity UMTBL
#define OUT_VISCLMAN        1723   //Viscosity LMTBL
#define OUT_SHMODUMAN       1725   //Shear modulus UMTBL
#define OUT_SHMODLMAN       1726   //Shear modulus LMTBL
#define OUT_FMELTUMAN       1727   //Melt fraction UMTBL
#define OUT_FMELTLMAN       1728   //Melt fraction LMTBL
#define OUT_MELTFACTORUMAN  1729   //Melt factor UMTBL
#define OUT_MELTFACTORLMAN  1730   //Melt factor LMTBL
#define OUT_DEPTHMELTMAN    1731   //Depth to base of UM melt region.
#define OUT_TDEPTHMELTMAN   1732   //Temp at base of UM melt region.
#define OUT_TJUMPMELTMAN    1733   //Temp jump across UM melt region.
#define OUT_MELTMASSFLUXMAN 1734   //Mantle melt mass flux.
#define OUT_K2MAN           1735   //Mantle k2 love number
#define OUT_IMK2MAN         1736   //Mantle Im(k2) love number
#define OUT_VISCUMANARR     1737   //Viscosity UM Arrhenius
#define OUT_RAYLEIGHMAN     1738   //Mantle Rayleigh Number
#define OUT_VISCMMAN        1739   //Viscosity Mid (ave) mantle.
/* Time Derivatives & Gradients */
#define OUT_TDOTMAN         1740   //Time deriv of mean mantle temp
#define OUT_TDOTCORE        1741   //time deriv of mean core temp
#define OUT_HFLUXUMAN       1742   //hflux upper mantle thermal boundary layer (UMTBL)
#define OUT_HFLOWUMAN       1743   //hflow UMTBL
#define OUT_HFLUXLMAN       1744   //hflux upper mantle thermal boundary layer (UMTBL)
#define OUT_HFLOWLMAN       1745   //hflow UMTBL
#define OUT_HFLUXCMB        1746   //hflux lower mantle TBL = CMB
#define OUT_HFLOWCMB        1747   //hflow LMTBL=CMB
#define OUT_HFLOWTIDALMAN   1748   //hflow tidal dissipation in mantle
#define OUT_HFLOWTIDALCORE  1749   //hflow tidal dissipation in core
#define OUT_HFLOWLATENTMAN  1750   //latent hflow from solidification of mantl
#define OUT_HFLOWMELTMAN    1751   //Melt hflow from mantle
#define OUT_HFLOWLATENTIC   1752   //latent hflow from solidification of IC
#define OUT_POWERGRAVIC     1753   //Gravitation Power release from solidification of IC
#define OUT_HFLOWICB        1754   //hflow across ICB
#define OUT_HFLUXSURF       1760   //hflux surface of mantle
#define OUT_HFLOWSURF       1761   //hflow surface of mantle
#define OUT_TIDALPOWMAN     1762   //Tidal Power Mantle
#define OUT_HFLOWSECMAN     1763   //Mantle Secular cooling rate.
#define OUT_HFLUXCMBAD      1764   //CMB Adiabatic heat flux.
#define OUT_HFLUXCMBCONV    1765   //CMB Super-Adiabatic (convective) heat flux.
/* Core Variables */
#define OUT_RIC             1770   //IC radius
#define OUT_DOC             1771   //OC shell thickness
#define OUT_DRICDTCMB       1772   //d(R_ic)/d(T_cmb)
#define OUT_RICDOT          1773   //d(R_ic)/d(t)
#define OUT_CHIOC           1774   //OC light element concentration chi.
#define OUT_CHIIC           1775   //IC light element concentration chi.
#define OUT_THERMCONDUCTOC  1776   //Thermal conductivity OC
#define OUT_THERMCONDUCTIC  1777   //Thermal conductivity IC
#define OUT_MASSOC          1778   //OC Mass
#define OUT_MASSIC          1779   //IC Mass
#define OUT_MASSCHIOC       1780   //OC chi Mass
#define OUT_MASSCHIIC       1781   //IC chi Mass
#define OUT_DTCHI           1782   //Core Liquidus Depression
#define OUT_COREBUOYTHERM   1783   //Core Thermal buoyancy flux
#define OUT_COREBUOYCOMPO   1784   //Core Compositional buoyancy flux
#define OUT_COREBUOYTOTAL   1785   //Core total buoyancy flux
#define OUT_GRAVICB         1786   //Core ICB gravity
#define OUT_MAGMOM          1787   //Core dynamo magnetic moment
#define OUT_PRESSWIND       1788   //Stellar wind pressure at planet
#define OUT_MAGPAUSERAD     1789   //Magnetopause Radius
/* Constants */
#define OUT_VISCJUMPMAN     1795   //Viscosity ratio UM 2 LM
#define OUT_ERUPTEFF        1796   //Mantle Melt Eruption Efficiency
#define OUT_VISCREF         1797   //Reference Viscosity
#define OUT_TREFLIND        1798   //Reference Lindeman Temperature
#define OUT_DTCHIREF        1799   //Reference Liquidus Depression

#define OUT_DYNVISC         1800   // Dynamic viscosity of upper mantle

void HelpOutputThermint(OUTPUT*);
void InitializeOutputThermint(OUTPUT*,fnWriteOutput[]);
void InitializeOutputFunctionThermint(OUTPUT*,int,int);
void FinalizeOutputFunctionThermint(OUTPUT*,int,int);

void WriteTMan(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void WriteTUMan(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void WriteTLMan(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void WriteTJumpUMan(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void WriteTJumpLMan(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void WriteSignTJumpUMan(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void WriteSignTJumpLMan(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void WriteTCore(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void WriteTCMB(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void WriteTICB(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void WriteViscUManArr(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void WriteViscUMan(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void WriteViscLMan(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void WriteViscMMan(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void WriteShmodUMan(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void WriteShmodLMan(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void WriteFMeltUMan(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void WriteFMeltLMan(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void WriteMeltfactorUMan(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void WriteMeltfactorLMan(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void WriteDepthMeltMan(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void WriteTDepthMeltMan(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void WriteTJumpMeltMan(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void WriteMeltMassFluxMan(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void WriteRayleighMan(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void WriteTDotMan(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void WriteTDotCore(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void WriteHfluxUMan(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void WriteHflowUMan(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void WriteHflowSecMan(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void WriteHfluxLMan(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void WriteHflowLMan(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void WriteHflowTidalMan(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void WriteHflowTidalCore(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void WriteHflowLatentMan(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void WriteHflowMeltMan(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void WriteHflowLatentIC(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void WritePowerGravIC(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void WriteHflowICB(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void WriteHflowSurf(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void WriteTidalPowMan(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void WriteRIC(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void WriteDRICDTCMB(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void WriteChiOC(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void WriteChiIC(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void WriteMassOC(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void WriteMassIC(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void WriteMassChiOC(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void WriteMassChiIC(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void WriteDTChi(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void WriteThermConductOC(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void WriteHfluxCMBAd(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void WriteHfluxCMBConv(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void WriteCoreBuoyTherm(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void WriteCoreBuoyCompo(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void WriteCoreBuoyTotal(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void WriteGravICB(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void WriteRICDot(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void WriteMagMom(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void WritePresSWind(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void WriteMagPauseRad(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
/* Logging Functions */
void LogOptionsThermint(CONTROL*,FILE*);
void LogThermint(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UPDATE*,fnWriteOutput[],FILE*);
void LogBodyThermint(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UPDATE*,fnWriteOutput[],FILE*,int);

/* Thermal Functions */
double fdTDotMan(BODY*,SYSTEM*,int*);
double fdTDotCore(BODY*,SYSTEM*,int*);
double fdTUMan(BODY*,int);
double fdTLMan(BODY*,int);
double fdTCMB(BODY*,int);
double fdTICB(BODY*,int);
double fdTJumpUMan(BODY*,int);
double fdTJumpLMan(BODY*,int);
double fdSignTJumpUMan(BODY*,int);
double fdSignTJumpLMan(BODY*,int);
double fdViscUManArr(BODY*,int);
double fdViscUMan(BODY*,int);
double fdViscLMan(BODY*,int);
double fdViscMMan(BODY*,int);
double fdViscJumpMan(BODY*,int);
double fdBLUMan(BODY*,int);
double fdBLLMan(BODY*,int);
double fdShmodUMan(BODY*,int);
double fdShmodLMan(BODY*,int);
double fdFMeltUMan(BODY*,int);
double fdFMeltLMan(BODY*,int);
double fdMeltfactorUMan(BODY*,int);
double fdMeltfactorLMan(BODY*,int);
double fdTsolUMan(BODY*,int);  //solidus temp at UMBL
double fdTliqUMan(BODY*,int);  //liquidus at UMBL
double fdTsolLMan(BODY*,int);
double fdTliqLMan(BODY*,int);
double fdDepthMeltMan(BODY*,int);  //depth to bottom of UM melt region (where solidus and geother intersect).
double fdTDepthMeltMan(BODY*,int);  //T at bottom of UM melt region.
double fdSolTempDiffMan(double,BODY*,int);  //difference between solidus and geotherm at a given depth.
double fdSolidusMan(double);   //solidus at a given depth.
double fdTJumpMeltMan(BODY*,int);  //temp jump across UM melt region.
double fdMeltMassFluxMan(BODY*,int);  //upwelling mantle melt mass flux.
double fdDynamicViscosity(BODY*,int);
double fdImk2Man(BODY*,int);
double fdK2Man(BODY*,int);
double fdRayleighMan(BODY*,int);
double fdHfluxUMan(BODY*,int);
double fdHfluxLMan(BODY*,int);
double fdHfluxCMB(BODY*,int);
double fdHflowUMan(BODY*,int);
double fdHflowSecMan(BODY*,int);
double fdHflowLMan(BODY*,int);
double fdHflowCMB(BODY*,int);
double fdHflowMeltMan(BODY*,int);
double fdHflowLatentMan(BODY*,UPDATE*,int);
double fdMassICDot(BODY*,UPDATE*,int);
double fdHflowLatentIC(BODY*,UPDATE*,int);
double fdPowerGravIC(BODY*,UPDATE*,int);
double fdTidalPowMan(BODY*,int);
double fdHflowSurf(BODY*,int);
double fdHfluxSurf(BODY*,int);
double fdRIC(BODY*,int);
double fdDRICDTCMB(BODY*,int);
double fdChiOC(BODY*,int);
double fdChiIC(BODY*,int);
double fdMassIC(BODY*,int);
double fdMassOC(BODY*,int);
double fdMassChiIC(BODY*,int);
double fdMassChiOC(BODY*,int);
double fdDTChi(BODY*,int);
double fdManHFlowPref(BODY*,int);
double fdHfluxCMBAd(BODY*,int);
double fdHfluxCMBConv(BODY*,int);
double fdThermConductOC(BODY*,int);
double fdCoreBuoyTherm(BODY*,int);
double fdCoreBuoyCompo(BODY*,int);
double fdCoreBuoyTotal(BODY*,int);
double fdGravICB(BODY*,int);
double fdRICDot(BODY*,UPDATE*,int);
double fdMagMom(BODY*,int);
double fdPresSWind(BODY*,int);
double fdMagPauseRad(BODY*,int);

void fnForceBehaviorThermint(BODY*,EVOLVE*,IO*,SYSTEM*,UPDATE*,fnUpdateVariable ***fnUpdate,int,int);
double fdSurfEnFlux(BODY*,SYSTEM*,UPDATE*,int,int);


/* MATH  FUNCTIONS */
double cube(double);
double root(int,BODY*,int,double,double,double,int);
double cubicroot(int,BODY*,int);
#define max(a,b)               \
   ({ __typeof__ (a) _a = (a); \
       __typeof__ (b) _b = (b); \
     _a > _b ? _a : _b; })
#define min(a,b) \
   ({ __typeof__ (a) _a = (a); \
       __typeof__ (b) _b = (b); \
     _a < _b ? _a : _b; })
