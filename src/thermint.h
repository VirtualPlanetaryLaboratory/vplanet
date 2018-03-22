/***************** THERMINT.H *********************** 
 *
 * Peter Driscoll 6/1/15
 *
 * This header file contains all the subroutines in
 * file thermint.c.
 *
*/

// GLOBAL VARIABLE INDEXES
#define TMAN             20                    /**< Index of TMAN (mantle temperature) global variable */
#define TCORE            21                    /**< Index of TCORE (core temperature) global variable */
#define TSURF  300.0                           /**< [K] Surface temperature (constant).  This should be set globally! */
// UNITS CONSTANTS
#define KM               1e3                   /**< [m] m in 1 km */
// FUNDAMENTAL CONSTANTS
#define GASCONSTANT      8.3144621             /**< [J/mol/K] Gas constant in SI units. */     
// E MASS RESERVOIRS
#define EMASS            5.9742e24             /**< [kg] Mass of Earth (E) */
#define EMASSFRACMAN     0.6712                /**< [nd] Earth's mantle mass fraction */
#define EMASSMAN         EMASSFRACMAN*EMASS    /**< [kg] Mass of Earth mantle */
#define EMASSFRACCORE    0.325                 /**< [nd] Earth's core mass fraction */
#define EMASSCORE        EMASSFRACCORE*EMASS   /**< [kg] Mass of Earth core */
#define EMASSIC          9.9124e22             /**< [kg] Mass of present inner core */
#define EMASSOC          EMASSCORE-EMASSIC     /**< [kg] Mass of present outer core */
#define EMASSOCEAN       1.4e21                /**< [kg] Mass of Earth oceans */
#define EMASSFRACOCEAN   EMASSOCEAN/EMASS      /**< [nd] Mass fraction of Earth's oceans */
#define EMASSFRACCRUST   1-EMASSFRACMAN-EMASSFRACCORE-EMASSFRACOCEAN  /**< [nd] Mass fraction of Earth's crust */
#define EMASSCRUST       EMASSFRACCRUST*EMASS  /**< [kg] Mass Earth crust today. */

// INTERNAL STRUCTURE E
#define ERADIUS          6371.0*KM              /**< [m] Radius of Earth */
#define ERMAN            6371.0*KM              /**< [m] Radius of Earth's mantle */
#define ERCORE           3481.0*KM              /**< [m] Radius of Earth's Core */
#define ERICB            1221.0*KM              /**< [m] Present day inner core boundary (ICB) radius */
#define EDMAN            ERADIUS-ERCORE         /**< [m] Earth's mantle shell thickness */
#define EDOC             ERCORE-ERICB           /**< [m] Earth's present day core shell thickness */
#define EVOL             (4.0)/(3)*PI*ERADIUS*ERADIUS*ERADIUS /**< [m^3] Volume of Earth */
#define EVOLIC           (4.0)/(3)*PI*ERICB*ERICB*ERICB   /**< [m^3] Volume of Earth's present-day inner core */
#define EVOLCORE         (4.0)/(3)*PI*ERCORE*ERCORE*ERCORE  /**< [m^3] Volume of Earth's total core */
#define EVOLOC           EVOLCORE-EVOLIC        /**< [m^3] Volume of Earth's outer core today */
#define EVOLMAN          EVOL-EVOLCORE          /**< [m^3] Volume of Earth's Mantle */
#define EAREASURF        4.0*PI*ERADIUS*ERADIUS /**< [m2] Area of Earth's surface */
#define EAREACMB         4.0*PI*ERCORE*ERCORE   /**< [m2] Area of Earth's core-mantle boundary (CMB) */
// DENSITIES - derived from mass and radius dfns.
#define EDENS            (EMASS)/(EVOL)         /**< [kg/m^3] Density of Earth */
#define EDENSMAN         (EMASSMAN)/(EVOLMAN)   /**< [kg/m^3] Density of Earth's mantle */
#define EDENSCORE        (EMASSCORE)/(EVOLCORE) /**< [kg/m^3] Density of Earth's core */
#define EDENSOC          (EMASSOC)/(EVOLOC)     /**< [kg/m^3] Density of Earth's outer core */
#define EDENSIC          (EMASSIC)/(EVOLIC)     /**< [kg/m^3] Density of Earth's inner core */
#define DENSANOMICB      700.0                  /**< [kg/m^3] Density difference of Earth's outer core light elements */
// Gravity
#define GRAVSURF         9.8                    /**< [m/s2] Earth's surface gravity */
#define GRAVUMAN         10.5                   /**< [m/s2] Earth's upper mantle gravity */
#define GRAVLMAN         10.5                   /**< [m/s2] Earth's lower mantle gravity  */
#define GRAVCMB          10.6                   /**< [m/s2] Earth's core-mantle boundary gravity */
					         
/* THERMAL PROPERTIES */		         
#define SPECHEATMAN      1265.0                 /**< [J/kg/K] Mantle specfic heat */
#define SPECHEATCORE     840.0                  /**< [J/kg/K] Core specific heat */
#define THERMEXPANMAN    3e-5                   /**< [K^-1] Mantle thermal expansivity */
#define THERMEXPANCORE   1e-5                   /**< [K^-1] Cpre thermal expansivity */
#define THERMCONDUMAN    4.2                    /**< [W/m/K] Upper mantle thermal conductivity */
#define THERMCONDLMAN    10.0                   /**< [W/m/K] Upper mantle thermal conductivity */
#define THERMDIFFUMAN    THERMCONDUMAN/(EDENSMAN*SPECHEATMAN) /**< [m^2/s] Upper mantle thermal diffusivity */
#define THERMDIFFLMAN    THERMDIFFUMAN          /**< [m^2/s] Lower mantle thermal diffusivity */
#define SPECLATENTICB    750e3                  /**< [J/kg] Inner core boundary specific latent heat release */
#define SPECLATENTMAN    320e3                  /**< [J/kg] specific latent heat release by mantle solidification */
#define SPECPOWGRAVIC    3e5                    /**< [J/kg] Gravitational power release by inner core solidification */
/* CONVECTION CONSTANTS */	                 	
#define RACRIT           660.                   /**< [nd] Critical rayleigh number for spherical convection */
#define CONVEXPON        1./3                   /**< [nd] Convective cooling Nusselt-Rayleigh number exponent, "beta" */
#define HFLOWREDUCTSTAG  1./25                  /**< [nd] Mantle surface heat flow reduction factor for stagnant lid */
#define STAGLID          0.                     /**< [nd] Switch to turn stagnant lid on.  (def=0, ie mobile lid) */
#define MANHFLOWPREF     1.                     /**< [nd] Upper mantle heat flow prefix. mobile lid=1. staglid=HFLOWREDUCTSTAG */
/* VISCOSITY PROPERTIES */	                 	
#define ACTVISCMAN       3e5                    /**< [J/mol] Mantle viscosity activation energy */
#define ACTSHMODMAN      2e5                    /**< [J/mol] Mantle shear modulus activation energy */
#define STIFFNESS        1e13                   /**< [Pa] Effective stiffness of mantle (calibrated to k2=0.3, Q=100) */
#define SHMODREF         6e6                    /**< [Pa] Reference kinematic mantle shear modulus */
#define VISCREF          5e7                    /**< [m^2/s] Reference kinematic mantle viscosity */
#define VISCJUMPMAN      2.7                    /**< [nd] Viscosity jump from upper to lower mantle */
#define FIXVISCJUMPMAN   0                      /**< [nd] (default) Option to fix viscjumpulm. if =0 then viscLM is computed from TLMan. */
#define VISCJUMPMMAN     10.                    /**< [nd] Viscosity jump from upper to average (mid) mantle */
#define VISCMELTB        2.5                    /**< [nd] Viscosity-melt reduction coefficient "B" (DB15 eq 8) */
#define VISCMELTPHIS	 0.8			/**< [nd] Viscosity-melt reduction coefficient "phi*" (DB15 eq 8) */
#define VISCMELTDELTA	 6.0			/**< [nd] Viscosity-melt reduction coefficient "delta" (DB15 eq 8) */
#define VISCMELTGAMMA	 6.0			/**< [nd] Viscosity-melt reduction coefficient "gamma" (DB15 eq 9) */
#define VISCMELTXI	 5e-4			/**< [nd] Viscosity-melt reduction coefficient "Xi" (DB15 eq 9) */
#define MELTFACTORLMAN   1.0                    /**< [nd] (Default) Viscosity-melt reduction factor "epsilon_phase" XXX Added by Rory -- Check! */
#define MELTFACTORUMAN   1.0                    /**< [nd] (Default) Viscosity-melt reduction factor "epsilon_phase" */
#define FIXMELTFACTORUMAN 0.0                   /**< [nd] (Default) Switch to fix MeltfactorUMan to a constant value */
/* MELTING CONSTANTS */
#define ASOLIDUS         -1.160e-16             /**< [K/m^3] Mantle solidus coefficient Tsol(r)=A*r^3+B*r^2+C*r+D */
#define BSOLIDUS         +1.708e-9              /**< [K/m^2] Solidus B coefficient */
#define CSOLIDUS         -9.074e-3              /**< [K/m] Solidus C coefficient */
#define DSOLIDUS         +1.993e4               /**< [K] Solidus D coefficient */
#define DTLIQMAN         +500.0                 /**< [K] Mantle liquidus offset, T_liq=T_sol+DTLIQMAN */
#define DLIND            7000.0*KM              /**< [m] Lindemann's law length scale for iron liquidus "D_Fe" (DB15 A23) */
#define TREFLIND         5705.0                 /**< [K] Lindemann's law reference temp. "T_Fe0" (DB15 A23) */
#define DVLIQDTEMP       8e17                   /**< [m^3/K] Approximation of change in mantle liquid volume with T_m, DV_liq/DT  (DB15) */
#define ERUPTEFF         0.1                    /**< [nd] (Default) Mantle melt heat loss eruption efficiency */
/* Continental Crust */
#define ECRUSTDENSITY    2800.                  /**< [kg/m3] Earth's continental crust density */
#define ECRUSTAREA       0.412*(EAREASURF)      /**< [m2] =41.2% of Earth surface (Taylor 95) */
#define ECRUSTDEPTH      (EMASSCRUST)/((ECRUSTDENSITY)*(ECRUSTAREA)) /**< [m] Average depth of continental crust today ~ 36 km (Taylor 95) */
#define CRUSTACCRFRAC    0.015                  /**< [nd] Constant fraction of oceanic crust accreted onto continent */
#define CRUSTINSOFACT    (65.)/(94.)            /**< [nd] Reduction in heat conducted through contintental crust compared to oceanic crust (Jaupart 07, table 3) */
/* CORE CHEMISTRY */		                 	
#define DTCHIREF         300.                   /**< [K] Core reference liquidus depression */
#define CHI_OC_E         0.18                   /**< [nd] Earth's outer core light element concentration */
#define PARTITION_CHI_CORE (ERICB)/(ERCORE)     /**< [nd] Core light element partition coefficent */
#define CHI_IC_E         PARTITION_CHI_CORE*CHI_OC_E /**< [nd] Inner core light element concentration in Earth  (CHI_IC_E=partition*CHI_OC_E) */
#define EMASSOC_CHI      CHI_OC_E*EMASSOC       /**< [kg] Mass of light elements (Chi) in Earth's outer core */
#define EMASSIC_CHI      CHI_IC_E*EMASSIC       /**< [kg] Mass of light elements (Chi) in Earth's inner core */
#define EMASSCORE_CHI    EMASSOC_CHI+EMASSIC_CHI/**< [kg] Total core light element mass of Earth (conserved) */
/* ADIABATIC PROPERTIES */
#define ADGRADMAN        (0.5)/(KM)             /**< [K/m] mantle linear adiabatic gradient =0.5K/km  (DB15 eq A18) */
#define DADCORE          6340.0*KM              /**< [m] liq iron core adiabatic length scale (DB15 eq A22) */
#define GRUNEISEN        1.3                    /**< [nd] Core gruneisen parameter */
#define ADJUMPM2UM       0.7                    /**< [nd] Adiabatic temperature jump from average to upper mantle "epsilon_UM" */
#define ADJUMPM2LM       1.3                    /**< [nd] Adiabatic temperature jump from average to lower mantle "epsilon_LM" */
#define ADJUMPC2CMB      0.8                    /**< [nd] Adiabatic temperature jump from average core to CMB "epsilon_c" */
/* MAGNETIC DYNAMO PROPERTIES */                 	
#define MAGPERM          4*PI*1e-7              /**< [H/m] Magnetic permeability constant */
#define MAGMOMCOEF       0.2                    /**< [nd] Saturation constant for fast rotating dipolar dynamos (OC2006) */
#define ELECCONDCORE     10e5                   /**< [S/m] Electrical conductivity of core  */
#define LORENTZNUM       2.5e-8                 /**< [W Ohm/K] Lorentz number, relates thermal and electrical conductivity */
#define EMAGMOM          80e21                  /**< [Am^2] Earth's present day magnetic moment */
#define EPRESSWIND       2.6761e-9              /**< [N/m^2] Earth's solar wind pressure: Psw=m_proton*n_sw*v_sw^2 (DB13) */
#define EMAGPAUSERAD     9.103*(ERADIUS)        /**< [m] Earth's magnetopause radius (DB13) */

//void InitializeControlThermint(CONTROL*);
void fvAddModuleThermint(MODULE*,int,int);
void fvBodyCopyThermint(BODY*,BODY*,int,int,int);
//void InitializeBodyEqtide(BODY*,CONTROL*,UPDATE*,int,int);
//void InitializeUpdateTmpBodyThermint(BODY*,CONTROL*,UPDATE*,int);

/* Options Info */

#define OPTSTARTTHERMINT    1700                /**< Start of THERMINT option indexes */
#define OPTENDTHERMINT      1900                /**< End of THERMINT options indexes */

/* Scalar Properties */
#define OPT_TMAN	    1710                /**< Average mantle temperature */
#define OPT_TUMAN	    1711		/**< Temperature base of upper mantle boundary layer */
#define OPT_TLMAN	    1712		/**< Temperature top of lower mantle boundary layer */
#define OPT_TCORE	    1713		/**< Average core temperature */
#define OPT_TCMB	    1714		/**< Temperature core-mantle boundary (CMB) */
#define OPT_TICB	    1715		/**< Temperature inner core boundary (ICB) */
#define OPT_BLUMAN	    1716		/**< Upper mantle thermal boundary layer thickness */
#define OPT_BLLMAN	    1717		/**< Lower mantle thermal boundary layer thickness */
#define OPT_TJUMPUMAN	    1718		/**< Temperature jump across upper mantle thermal boundary layer */
#define OPT_TJUMPLMAN	    1719		/**< Temperature jump across lower mantle thermal boundary layer */
#define OPT_VISCLMAN	    1721		/**< Viscosity of lower mantle thermal boundary layer */
#define OPT_SHMODUMAN	    1722		/**< Shear modulus of upper mantle boundary layer */
#define OPT_SHMODLMAN	    1723		/**< Shear modulus of lower mantle boundary layer */
#define OPT_FMELTUMAN	    1724		/**< Melt fraction in upper mantle boundary layer */
#define OPT_FMELTLMAN	    1725		/**< Melt fraction in lower mantle boundary layer */
#define OPT_MELTFACTORUMAN  1726		/**< Melt viscosity reduction factor in upper mantle */
#define OPT_FIXMELTFACTORUMAN 1727		/**< Option to fix melt viscosity reduction in upper mantle */
#define OPT_MELTFACTORLMAN  1728		/**< Melt viscosity reduction factor in lower mantle */
#define OPT_DEPTHMELTMAN    1729		/**< Depth to base of upper mantle melting */
#define OPT_TDEPTHMELTMAN   1730		/**< Temperature at base of upper mantle melting */
#define OPT_TJUMPMELTMAN    1731		/**< Temperature jump across upper mantle melt region */
#define OPT_K2MAN	    1732		/**< Mantle k2 Love number */
#define OPT_IMK2MAN	    1733		/**< Mantle imaginary part of the k2 Love number */
#define OPT_VISCUMANARR	    1736		/**< Arrhenius viscosity in upper mantle */
#define OPT_VISCMMAN	    1737		/**< Viscosity of mid (average) mantle */
/* Time Derivatives & Gradients */		 						   
#define OPT_TDOTMAN	    1740		/**< Time derivative of average mantle temperature */
#define OPT_TDOTCORE	    1741		/**< Time derivative of average core temperature */
#define OPT_HFLUXUMAN	    1742		/**< Heat flux across upper mantle thermal boundary layer */
#define OPT_HFLOWUMAN	    1743		/**< Heat flow across upper mantle thermal boundary layer */
#define OPT_HFLUXLMAN	    1744		/**< Heat flux across upper mantle thermal boundary layer */
#define OPT_HFLOWLMAN	    1745		/**< Heat flow across upper mantle thermal boundary layer */
#define OPT_HFLUXCMB	    1746		/**< Heat flux across core-mantle boundary and lower mantle */
#define OPT_HFLOWCMB	    1747		/**< Heat flow across core-mantle boundary and lower mantle */
#define OPT_HFLOWTIDALMAN   1748		/**< Tidal dissipation heat flow in mantle */
#define OPT_HFLOWTIDALCORE  1749		/**< Tidal dissipation heat flow in core */
#define OPT_HFLOWLATENTMAN  1750		/**< Latent heat flow from solidification of mantle */
#define OPT_HFLOWMELTMAN    1751		/**< Melt heat flow from mantle */
#define OPT_HFLOWLATENTIC   1752		/**< Latent heat flow from solidification of inner core */
#define OPT_POWERGRAVIC	    1753		/**< Gravitation power released from solidification of inner core */
#define OPT_HFLOWICB	    1754		/**< Heat flow across inner core boundary */
#define OPT_HFLUXSURF	    1760		/**< Heat flux across surface of mantle */
#define OPT_HFLOWSURF	    1761	        /**< Heat flow across surface of mantle */
#define OPT_TIDALPOWMAN	    1762                /**< Mantle tidal dissipation power */
/* Halts */			           							   
#define OPT_HALTMINTMAN	    1763                /**< Minimum mantle temperature			   */
#define OPT_HALTMINTCORE    1764                /**< Minimum core temperature			   */
/* Stagnant Lid */		                 							   
#define OPT_STAGLID	    1765                /**< Stagnant lid switch				   */
#define OPT_MANHFLOWPREF    1766                /**< Stagnant lid switch				   */
/* Stellar Wind */		                 							   
#define OPT_PRESSWIND	    1767                /**< Stellar wind pressure				   */
/* Core Variables */		                 							   
#define OPT_RIC		    1770                /**< IC radius					   */
#define OPT_DOC		    1771                /**< OC shell thickness				   */
#define OPT_CHIOC	    1780                /**< OC light element concentration chi.		   */
#define OPT_CHIIC	    1781                /**< IC light element concentration chi.		   */
#define OPT_THERMCONDUCTOC  1782                /**< Thermal conductivity OC				   */
#define OPT_THERMCONDUCTIC  1783                /**< Thermal conductivity IC				   */
#define OPT_MASSOC	    1784                /**< OC Mass						   */
#define OPT_MASSIC	    1785                /**< IC Mass						   */
#define OPT_MASSCHIOC	    1786                /**< OC chi Mass					   */
#define OPT_MASSCHIIC	    1787                /**< IC chi Mass					   */
#define OPT_DTCHI	    1788                /**< Core Liquidus Depression			   */
/* Dynamo Parameters */		                 							   
#define OPT_MAGMOMCOEF	    1789                /**< Magnetic Moment Scaling Coef (gamma_dip)	   */
/* Constants */			                 							   
#define OPT_VISCJUMPMAN	    1790                /**< Viscosity ratio UM 2 LM				   */
#define OPT_ERUPTEFF	    1791                /**< Mantle Melt Eruption Efficiency			   */
#define OPT_VISCREF	    1792                /**< Reference Viscosity				   */
#define OPT_TREFLIND	    1793                /**< Reference Temperature Lindemann Core Liquidus	   */
#define OPT_DTCHIREF	    1794                /**< Reference Core Liquidus Depression		   */
#define OPT_VISCMELTB       1795                /**< Viscosity Melt Factor B    */
#define OPT_VISCMELTPHIS    1796                /**< Viscosity Melt Factor Phi*   */
#define OPT_VISCMELTXI      1797                /**< Viscosity Melt Factor Xi   */
#define OPT_VISCMELTGAMMA   1798                /**< Viscosity Melt Factor Gamma   */
#define OPT_VISCMELTDELTA   1799                /**< Viscosity Melt Factor Delta   */

/* New Parameters for vemcee */
#define OPT_ACTVISCMAN      1800  /**< [J/mol] viscosity activation energy mantle */
#define OPT_SHMODREF        1801  /**< [Pa] reference kinematic mantle shear modulus */
#define OPT_STIFFNESS       1802  /**< [Pa] effective stiffness of mantle (calibrated to k2=0.3, Q=100) */
#define OPT_DLIND           1803  /**< [m] lindemann's law length scale for iron liquidus "D_Fe" (DB15 A23) */
#define OPT_DADCORE         1804  /**< [m] liq iron core adiabatic length scale (DB15 eq A22) */
#define OPT_ADJUMPM2UM      1805  /**< [nd] adiabatic temp jump from ave mantle to UM. "epsilon_UM" */
#define OPT_ADJUMPM2LM      1806  /**< [nd] adiabatic temp jump from ave mantle to LM. "epsilon_LM" */
#define OPT_ADJUMPC2CMB     1807  /**< [nd] adiabatic temp jump from ave core to CMB. "epsilon_c" */
#define OPT_ELECCONDCORE    1808  /**< [S/m]  electrical conductivity of core.    */
/* End vemcee parameters */

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
/* vemcee parameters */
void ReadActViscMan(BODY*,CONTROL*,FILES*,OPTIONS*,SYSTEM*,int) ;
void ReadShModRef(BODY*,CONTROL*,FILES*,OPTIONS*,SYSTEM*,int) ;
void ReadStiffness(BODY*,CONTROL*,FILES*,OPTIONS*,SYSTEM*,int) ;
void ReadDLind(BODY*,CONTROL*,FILES*,OPTIONS*,SYSTEM*,int) ;
void ReadDAdCore(BODY*,CONTROL*,FILES*,OPTIONS*,SYSTEM*,int) ;
void ReadAdJumpM2UM(BODY*,CONTROL*,FILES*,OPTIONS*,SYSTEM*,int) ;
void ReadAdJumpM2LM(BODY*,CONTROL*,FILES*,OPTIONS*,SYSTEM*,int) ;
void ReadAdJumpC2CMB(BODY*,CONTROL*,FILES*,OPTIONS*,SYSTEM*,int) ;
void ReadElecCondCore(BODY*,CONTROL*,FILES*,OPTIONS*,SYSTEM*,int) ;
/* end vemcee parameters */

void InitializeOptionsThermint(OPTIONS*,fnReadOption[]);
void ReadOptionsThermint(BODY*,CONTROL*,FILES*,OPTIONS*,SYSTEM*,fnReadOption[],int);

/* Halt Functions */
#define THERMINTHALTSYSEND       6
#define THERMINTHALTBODYEND      6

int fbHaltMinTMan(BODY*,EVOLVE*,HALT*,IO*,UPDATE*,int);  /**< syntax now is same as fnHaltModule in vplanet.h    */
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
#define OUT_TMAN            1710   /**< Temperature Mantle AVE                                
#define OUT_TUMAN           1711   /**< Temperature UMTBL				      */
#define OUT_TLMAN           1712   /**< Temperature LMTBL				      */
#define OUT_TCORE           1713   /**< Temperature Core AVE				      */
#define OUT_TCMB            1714   /**< Temperature CMB					      */
#define OUT_TICB            1715   /**< Temperature ICB					      */
#define OUT_BLUMAN          1716   /**< UM TBL thickness				      */
#define OUT_BLLMAN          1717   /**< LM TBL thickness				      */
#define OUT_TJUMPUMAN       1718   /**< Temperature Jump across UMTBL			      */
#define OUT_TJUMPLMAN       1719   /**< Temperature Jump across LMTBL			      */
#define OUT_SIGNTJUMPUMAN   1720   /**< Temperature Jump across UMTBL			      */
#define OUT_SIGNTJUMPLMAN   1721   /**< Temperature Jump across LMTBL			      */
#define OUT_VISCLMAN        1723   /**< Viscosity LMTBL					      */
#define OUT_SHMODUMAN       1725   /**< Shear modulus UMTBL				      */
#define OUT_SHMODLMAN       1726   /**< Shear modulus LMTBL				      */
#define OUT_FMELTUMAN       1727   /**< Melt fraction UMTBL				      */
#define OUT_FMELTLMAN       1728   /**< Melt fraction LMTBL				      */
#define OUT_MELTFACTORUMAN  1729   /**< Melt factor UMTBL				      */
#define OUT_MELTFACTORLMAN  1730   /**< Melt factor LMTBL				      */
#define OUT_DEPTHMELTMAN    1731   /**< Depth to base of UM melt region.		      */
#define OUT_TDEPTHMELTMAN   1732   /**< Temp at base of UM melt region.			      */
#define OUT_TJUMPMELTMAN    1733   /**< Temp jump across UM melt region.		      */
#define OUT_MELTMASSFLUXMAN 1734   /**< Mantle melt mass flux.				      */
#define OUT_VISCUMANARR     1737   /**< Viscosity UM Arrhenius				      */
#define OUT_RAYLEIGHMAN     1738   /**< Mantle Rayleigh Number				      */
#define OUT_VISCMMAN        1739   /**< Viscosity Mid (ave) mantle.			      */
/* Time Derivatives & Gradients */							      
#define OUT_TDOTMAN         1740   /**< Time deriv of mean mantle temp			      */
#define OUT_TDOTCORE        1741   /**< time deriv of mean core temp			      */
#define OUT_HFLUXUMAN       1742   /**< hflux upper mantle thermal boundary layer (UMTBL)     */
#define OUT_HFLOWUMAN       1743   /**< heat flow UMTBL                                           */
#define OUT_HFLUXLMAN       1744   /**< hflux upper mantle thermal boundary layer (UMTBL)     */
#define OUT_HFLOWLMAN       1745   /**< heat flow UMTBL					      */
#define OUT_HFLUXCMB        1746   /**< hflux lower mantle TBL = CMB			      */
#define OUT_HFLOWCMB        1747   /**< heat flow LMTBL=CMB					      */
#define OUT_HFLOWTIDALMAN   1748   /**< heat flow tidal dissipation in mantle		      */
#define OUT_HFLOWTIDALCORE  1749   /**< heat flow tidal dissipation in core			      */
#define OUT_HFLOWLATENTMAN  1750   /**< latent heat flow from solidification of mantl	      */
#define OUT_HFLOWMELTMAN    1751   /**< Melt heat flow from mantle				      */
#define OUT_HFLOWLATENTIC   1752   /**< latent heat flow from solidification of IC		      */
#define OUT_POWERGRAVIC     1753   /**< Gravitation Power release from solidification of IC   */
#define OUT_HFLOWICB        1754   /**< heat flow across ICB				      */
#define OUT_HFLUXSURF       1760   /**< hflux surface of mantle				      */
#define OUT_HFLOWSURF       1761   /**< heat flow surface of mantle				      */
#define OUT_TIDALPOWMAN     1762   /**< Tidal Power Mantle				      */
#define OUT_HFLOWSECMAN     1763   /**< Mantle Secular cooling rate.			      */
#define OUT_HFLUXCMBAD      1764   /**< CMB Adiabatic heat flux.			      */
#define OUT_HFLUXCMBCONV    1765   /**< CMB Super-Adiabatic (convective) heat flux.	      */
/* Core Variables */									      
#define OUT_RIC             1770   /**< IC radius					      */
#define OUT_DOC             1771   /**< OC shell thickness				      */
#define OUT_DRICDTCMB       1772   /**< d(R_ic)/d(T_cmb)				      */
#define OUT_RICDOT          1773   /**< d(R_ic)/d(t)					      */
#define OUT_CHIOC           1774   /**< OC light element concentration chi.		      */
#define OUT_CHIIC           1775   /**< IC light element concentration chi.		      */
#define OUT_THERMCONDUCTOC  1776   /**< Thermal conductivity OC				      */
#define OUT_THERMCONDUCTIC  1777   /**< Thermal conductivity IC				      */
#define OUT_MASSOC          1778   /**< OC Mass						      */
#define OUT_MASSIC          1779   /**< IC Mass						      */
#define OUT_MASSCHIOC       1780   /**< OC chi Mass					      */
#define OUT_MASSCHIIC       1781   /**< IC chi Mass                                           */
#define OUT_DTCHI           1782   /**< Core Liquidus Depression			      */
#define OUT_COREBUOYTHERM   1783   /**< Core Thermal buoyancy flux			      */
#define OUT_COREBUOYCOMPO   1784   /**< Core Compositional buoyancy flux		      */
#define OUT_COREBUOYTOTAL   1785   /**< Core total buoyancy flux			      */
#define OUT_GRAVICB         1786   /**< Core ICB gravity				      */
#define OUT_MAGMOM          1787   /**< Core dynamo magnetic moment			      */
#define OUT_PRESSWIND       1788   /**< Stellar wind pressure at planet			      */
#define OUT_MAGPAUSERAD     1789   /**< Magnetopause Radius				      */
/* Constants */										      
#define OUT_VISCJUMPMAN     1795   /**< Viscosity ratio UM 2 LM				      */
#define OUT_ERUPTEFF        1796   /**< Mantle Melt Eruption Efficiency			      */
#define OUT_VISCREF         1797   /**< Reference Viscosity				      */
#define OUT_TREFLIND        1798   /**< Reference Lindeman Temperature			      */
#define OUT_DTCHIREF        1799   /**< Reference Liquidus Depression			      */
											      
#define OUT_DYNVISC         1800   /**<  Dynamic viscosity of upper mantle		      */
											      
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
double fdTsolUMan(BODY*,int);  /**< solidus temp at UMBL   */
double fdTliqUMan(BODY*,int);  /**< liquidus at UMBL   */
double fdTsolLMan(BODY*,int);
double fdTliqLMan(BODY*,int);
double fdDepthMeltMan(BODY*,int);  /**< depth to bottom of UM melt region (where solidus and geother intersect).   */
double fdTDepthMeltMan(BODY*,int);  /**< T at bottom of UM melt region.   */
double fdSolTempDiffMan(double,BODY*,int);  /**< difference between solidus and geotherm at a given depth.   */
double fdSolidusMan(double);   /**< solidus at a given depth.   */
double fdTJumpMeltMan(BODY*,int);  /**< temp jump across UM melt region.   */
double fdMeltMassFluxMan(BODY*,int);  /**< upwelling mantle melt mass flux.   */
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
