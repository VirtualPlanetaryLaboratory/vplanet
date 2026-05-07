/**
    @file atmesc.h
    @brief Subroutines that control the integration of the
    atmospheric escape model.
    @author Rodrigo Luger ([rodluger@gmail.com](mailto:rodluger@gmail.com>))
    @date May 12 2015
*/

#define TOMASS 1.39e21      /**< Mass of one terrestrial ocean in kg (TO) */
#define TOHMASS TOMASS / 9. /**< Hydrogen mass in one TO */
#define ATMESC_LB15                                                            \
  0 /**< Flag: Luger and Barnes (2015) atmospheric escape formulae */
#define ATMESC_LBEXACT                                                         \
  1 /**< Flag: Luger and Barnes (2015) atmospheric escape formulae w/ updated  \
       oxygen mixing ratio calculations */
#define ATMESC_TIAN                                                            \
  2                   /**< Flag: Tian (2015) atmospheric escape formulae flag  \
                       */
#define ATMESC_ELIM 3 /**< Flag: Energy-limited escape */
#define ATMESC_DIFFLIM 4   /**< Flag: Diffusion-limited escape */
#define ATMESC_BONDILIM 5  /**< Flag: Bondi-limited escape */
#define ATMESC_RRLIM 6     /**< Flag: Radiation/recombination-limited escape */
#define ATMESC_BALLISTIC 7 /**< Flag: Ballistic escape regime */
#define ATMESC_NONE 8      /**< Flag: No atmospheric escape */
// XXX Change to LOPEZ12
#define ATMESC_LOP12 9 /**< Flag: Lopez (2012) gaseous planet radius model */
#define ATMESC_PROXCENB                                                        \
  10 /**< Flag: Proxima Centauri b gaseous planet radius model */
#define ATMESC_LEHMER17                                                        \
  11 /**< Flag: Lehmer & Catling (2017) planet radius model */
#define ATMESC_BOL16                                                           \
  12 /**< Flag: Bolmont (2016) XUV absorption efficiency model */
#define ATMESC_LS2016                                                          \
  13            /**< Schaefer et al (2016) atmospheric escape model            \
                 */
#define QOH 16. /**< Atomic mass ratio oxygen/hydrogen */

/* Options Info */
#define OPTSTARTATMESC 1200     /**< Start of AtmEsc options */
#define OPTENDATMESC 1300       /**< End of AtmEsc options */
#define OPT_HALTDESICCATED 1211 /**< Halt if desiccated? */
#define OPT_XFRAC                                                              \
  1213 /**< X-ray absorption radius as a fraction of planet radius */
#define OPT_ATMXABSEFFH 1214 /**< Hydrogen Absorption efficiency (epsilon) */

#define OPT_HALTENVELOPEGONE 1216 /**< Halt if evaporated? */

#define OPT_OXYGENMASS 1218     /**< Initial oxygen mass */
#define OPT_WATERLOSSMODEL 1219 /**< Oxygen buildup / water loss model */
#define OPT_PLANETRADIUSMODEL                                                  \
  1220 /**< Gaseous planet radius model (for atmesc) */
#define OPT_INSTANTO2SINK                                                      \
  1221                         /**< Gaseous planet radius model (for atmesc)   \
                                */
#define OPT_ATMXABSEFFH2O 1222 /**< Water Absorption efficiency (epsilon) */
#define OPT_OXYGENMANTLEMASS 1223 /**< Initial oxygen mass in mantle */
#define OPT_THERMTEMP 1224        /**< thermosphere temperature */
#define OPT_PRESXUV 1225          /**< Pressure at bottom of thermosphere */
#define OPT_ATMGASCONST 1226      /**< Atmospheric Gas Constant */
#define OPT_FXUV 1227             /**< The value of the XUV flux */
#define OPT_ATMXABSEFFH2OMODEL                                                 \
  1228 /**< Model for time evolution of epsilon for H2O */
#define OPT_JEANSTIME                                                          \
  1229 /**< Time at which flow becomes ballistic (Jeans escape) */
#define OPT_FLOWTEMP 1230 /**< Flow temperature */
#define OPT_BONDILIMITED                                                       \
  1231 /**< Whether or not to use Bondi-limited escape                         \
        */
#define OPT_ENERGYLIMITED                                                      \
  1232 /**< Whether or not to use energy-limited escape */
#define OPT_RRLIMITED                                                          \
  1233 /**< Whether or not to use radiation/recombination-limited escape */
#define OPT_ATMESCAUTO                                                         \
  1234 /**< Whether or not to let atmesc determine escape regime */
#define OPT_STOPWATERLOSSINHZ                                                  \
  1235                    /**< Stop water loss once planet reaches HZ? */
#define OPT_MINKTIDE 1240 /**< Minimum KTide value */

/* Gunell+2018 magnetic-limited escape options (atmesc.c) */
#define OPT_MAGLIMITEDESCAPE 1241   /**< Enable Gunell+2018 magnetic escape */
#define OPT_MAGFIELD 1242           /**< Planet dipole moment (A m^2) */
#define OPT_STELLARWINDDENSITY 1243 /**< Solar-wind proton density at planet */
#define OPT_STELLARWINDVELOCITY 1244 /**< Solar-wind speed at planet (m/s) */
#define OPT_EXOBASETEMP 1245        /**< Exobase temperature (K) */
#define OPT_CO2MASS 1246            /**< Initial atmospheric CO2 mass (kg) */
#define OPT_N2MASS 1247             /**< Initial atmospheric N2 mass (kg) */
#define OPT_MAGLIMITEDESCAPEMODEL 1248 /**< Magnetic-limited escape model */

/* Magnetic-limited escape model selectors. Stored in
   body[iBody].iMagLimitedEscapeModel; the function pointer
   body[iBody].fdMagLimitedEscapeDt is resolved at verify time. */
#define MAG_MODEL_GUNELL18  0  /**< Gunell+2018 (4 mechanisms summed) */
#define MAG_MODEL_DRISCOLL13 1 /**< Driscoll & Bercovici 2013 (single formula) */

/* Hard-coded reference constants for the Gunell+2018 formulas. These are the
   values used in the source notebook (Combined for Paper); they appear as
   fixed scalings in the polar-cap and cusp models. */
#define MAG_OMEGA_PC_E    0.63       /**< Earth polar-cap solid angle (sr) */
#define MAG_R_EXO_E       6.871e6    /**< Earth exobase radius (m) */
#define MAG_R_IMB_E       7.647e6    /**< Earth induced magnetosphere boundary (m) */
#define MAG_FORMFACT_O    1.16       /**< Polar-cap obstacle form factor */
#define MAG_EARTH_DIPOLE  8.0e22     /**< Modern Earth dipole moment (A m^2) */
#define MAG_NSW_E         1.0e7      /**< Earth solar-wind reference density (1/m^3) */
#define MAG_VSW_E         6.04e5     /**< Earth solar-wind reference velocity (m/s) */
#define MAG_NSW_DEFAULT   1.0e7      /**< Default solar-wind density at planet */
#define MAG_VSW_DEFAULT   4.7e5      /**< Default solar-wind velocity at planet */
#define MAG_TEXO_DEFAULT  900.0      /**< Default exobase temperature (K) */
#define MAG_Q0_PICKUP     5.0e26     /**< Pickup escape coefficient (1/s) */
#define MAG_Q0_CROSSFIELD 7.7e25     /**< Cross-field escape coefficient (1/s) */
#define MAG_Q0_POLARCAP   7.8e25     /**< Polar cap escape coefficient (1/s) */
#define MAG_Q0_CUSP       5.0e24     /**< Cusp escape coefficient (1/s) */
#define MAG_QMAX_CUSP     5.0e25     /**< Maximum cusp escape rate (1/s) */
#define MAG_DRISCOLL_TIME       10.0    /**< Driscoll formula timescale (s) */
#define MAG_DRISCOLL_MASSWATER  1.0e21  /**< Driscoll water reservoir (kg) */
#define MAG_DRISCOLL_SIGMA_COLL 1.0e-17 /**< Collisional cross-section (m^2) */
#define MAG_DRISCOLL_ENERGY_EFF (1.0/10.6e44) /**< Driscoll efficiency (notebook) */
#define MAG_PROTON_MASS         1.67e-27 /**< Proton mass used in notebook (kg) */
#define MAG_VACUUM_PERMEABILITY (4.0 * PI * 1.0e-7) /**< mu_0 (H/m) */

/* Number of atmospheric species used by the well-mixed Gunell+2018 escape
   model: water, oxygen, CO2, hydrogen, N2. The total bulk mass loss rate
   is N times the H-equivalent rate from the Gunell mechanisms; each
   species loses mass at total/N. Hardcoded; matches the species set used
   in the Gunell+2018 modern-Earth setup. */
#define MAG_NUM_WELL_MIXED_SPECIES 5

/* @cond DOXYGEN_OVERRIDE */

void AddModuleAtmEsc(CONTROL *control, MODULE *, int, int);
void BodyCopyAtmEsc(BODY *, BODY *, int, int, int);

/* Options Functions */
void HelpOptionsAtmEsc(OPTIONS *);
void InitializeOptionsAtmEsc(OPTIONS *, fnReadOption[]);
void ReadOptionsAtmEsc(BODY *, CONTROL *, FILES *, OPTIONS *, SYSTEM *,
                       fnReadOption[], int);

/* Halt Functions */
#define ATMESCHALTSYSEND 5  /**< Start of AtmEsc halting functions */
#define ATMESCHALTBODYEND 5 /**< End of AtmEsc halting functions */

int fbHaltSurfaceDesiccated(BODY *, EVOLVE *, HALT *, IO *, UPDATE *,
                            fnUpdateVariable ***, int);
int fbHaltEnvelopeGone(BODY *, EVOLVE *, HALT *, IO *, UPDATE *,
                       fnUpdateVariable ***, int);
void CountHaltsAtmEsc(HALT *, int *);

/* Verify Functions */
void VerifyAtmEsc(BODY *, CONTROL *, FILES *, OPTIONS *, OUTPUT *, SYSTEM *,
                  UPDATE *, int, int);
void VerifyRotationAtmEsc(BODY *, CONTROL *, OPTIONS *, char[], int);
void InitializeVplanetAtmEsc(CONTROL *, MODULE *);
void VerifyHaltAtmEsc(BODY *, CONTROL *, OPTIONS *, int, int *);
void VerifyRotationAtmEsc(BODY *, CONTROL *, OPTIONS *, char[], int);

/* Update functions */
void InitializeUpdateAtmEsc(BODY *, UPDATE *, int);
void FinalizeUpdateEccAtmEsc(BODY *, UPDATE *, int *, int, int, int);
void FinalizeUpdateNumIsotopeAtmEsc(BODY *, UPDATE *, int *, int, int, int);
void FinalizeUpdateOblAtmEsc(BODY *, UPDATE *, int *, int, int, int);
void FinalizeUpdateRotAtmEsc(BODY *, UPDATE *, int *, int, int, int);
void FinalizeUpdateSemiAtmEsc(BODY *, UPDATE *, int *, int, int, int);
void FinalizeUpdateSurfaceWaterMassAtmEsc(BODY *, UPDATE *, int *, int, int,
                                          int);
void FinalizeUpdateOxygenMassAtmEsc(BODY *, UPDATE *, int *, int, int, int);
void FinalizeUpdateOxygenMantleMassAtmEsc(BODY *, UPDATE *, int *, int, int,
                                          int);
void FinalizeUpdateRadiusAtmEsc(BODY *, UPDATE *, int *, int, int, int);
void FinalizeUpdateEnvelopeMassAtmEsc(BODY *, UPDATE *, int *, int, int, int);
void FinalizeUpdateMassAtmEsc(BODY *, UPDATE *, int *, int, int, int);

/* Output Functinos */

/* AtmEsc 1200 - 1299 */
#define OUTSTARTATMESC 1200 /**< Start of AtmEsc output functions */
#define OUTENDATMESC 1300   /**< End of AtmEsc output functions */

/* Body Properties due to atmospheric escape */
#define OUT_SURFACEWATERMASS 1210 /**< Surface water mass remaining on body */
#define OUT_ENVELOPEMASS 1211     /**< Envelope mass remaining */
#define OUT_OXYGENMASS 1212       /**< Oxygen mass retained by body */
#define OUT_RGLIMIT 1213          /**< Runaway greenhouse limit (AU) */
#define OUT_XO 1214               /**< Oxygen mixing ratio */
#define OUT_ETAO 1215 /**< Luger & Barnes (2015) oxygen eta parameter */
#define OUT_PLANETRADIUS                                                       \
  1216 /**< Planet radius (for the Lehmer & Catling 2017 model) */
#define OUT_OXYGENMANTLEMASS                                                   \
  1217 /**< Mass of oxygen absorbed by the mantle                              \
        */
#define OUT_PLANETRADXUV                                                       \
  1218 /**< Effective planet radius in the XUV (for the Lehmer & Catling 2017  \
          model) */
#define OUT_RADSOLID                                                           \
  1219 /**< Solid radius (for the Lehmer & Catling 2017 model) */
#define OUT_PRESXUV                                                            \
  1220 /**< Pressure at the effective XUV absporption level (for the Lehmer &  \
          Catling 2017 model) */
#define OUT_SCALEHEIGHT                                                        \
  1221 /**< Atmospheric scale height (for the Lehmer & Catling 2017 model) */
#define OUT_THERMTEMP                                                          \
  1222 /**< Temperature of the thermosphere (for the Lehmer & Catling 2017     \
          model) */
#define OUT_ATMGASCONST                                                        \
  1223 /**< Gas constant (for the Lehmer & Catling 2017 model) */
#define OUT_PRESSURF                                                           \
  1224 /**< Surface pressure (for the Lehmer & Catling 2017 model) */
#define OUT_DENVMASSDT 1225    /**< Rate of change of the envelope mass */
#define OUT_FXUV 1226          /**< XUV flux */
#define OUT_EPSH2O 1227        /**< XUV Atmospheric Escape Efficiency for H2O */
#define OUT_ROCHERADIUS 1228   /**< Roche Lobe radius */
#define OUT_BONDIRADIUS 1229   /**< Bondi radius */
#define OUT_HESCAPEREGIME 1230 /**< Hydrogen envelope escape regime */
#define OUT_RRCRITICALFLUX                                                     \
  1231 /**< Critical flux between RR and energy-limited escape */
#define OUT_CROSSOVERMASS                                                      \
  1232 /**< Crossover mass, tells us whether oxygen is escaping */
#define OUT_WATERESCAPEREGIME                                                  \
  1233 /**< What water escape regime is the sim currently in (energy limited,  \
          diffusion limited, etc) */
#define OUT_FXUVCRITDRAG                                                       \
  1234 /**< The critical drag XUV Flux, FXUV > FXUVCritDrag for drag of        \
          heavier species to occur */
#define OUT_HREFFLUX                                                           \
  1235 /**< The hydrogen reference flux (I can't believe this wasn't an output \
          yet) */
#define OUT_XO2 1236  /**< The molecular oxygen mixing ratio */
#define OUT_XH2O 1237 /**< The water mixing ratio */
#define OUT_HDIFFFLUX                                                          \
  1238 /**< The diffusion limited flux (the true flux in the diffusion regime) \
        */
#define OUT_HREFODRAGMOD                                                       \
  1239 /**< Multiply by H ref flux to get H flux with drag of oxgyen */
#define OUT_KTIDE 1240 /**< Gravitational enhancement of mass loss */
#define OUT_CUMULATIVEFXUV 1250
#define OUT_RGFLUX                                                             \
  1260 /**< Incident bolometric flux to trigger a runaway greenhouse */

/* Gunell+2018 magnetic-limited escape outputs */
#define OUT_MAGPICKUPRATE     1270 /**< Pickup escape rate (particles/s) */
#define OUT_CROSSFIELDRATE    1271 /**< Cross-field ion escape rate (particles/s) */
#define OUT_POLARCAPRATE      1272 /**< Polar-cap escape rate (particles/s) */
#define OUT_CUSPRATE          1273 /**< Cusp escape rate (particles/s) */
#define OUT_DRISCOLLRATE      1274 /**< Driscoll mag-limited rate (particles/s) */
#define OUT_DMAGLIMITEDMASSDT 1275 /**< Magnetic-limited bulk mass loss rate (kg/s) */
#define OUT_MAGPAUSERADATMESC 1276 /**< Magnetopause stand-off radius (m) */
#define OUT_MAGFIELD          1277 /**< Planet dipole moment (A m^2) */
#define OUT_CO2MASS           1278 /**< Atmospheric CO2 mass (kg) */
#define OUT_N2MASS            1279 /**< Atmospheric N2 mass (kg) */

void InitializeOutputAtmEsc(OUTPUT *, fnWriteOutput[]);
void InitializeOutputFunctionAtmEsc(OUTPUT *, int, int);
void FinalizeOutputFunctionAtmEsc(OUTPUT *, int, int);

void WriteSurfaceWaterMass(BODY *, CONTROL *, OUTPUT *, SYSTEM *, UNITS *,
                           UPDATE *, int, double *, char **);
void WriteOxygenMass(BODY *, CONTROL *, OUTPUT *, SYSTEM *, UNITS *, UPDATE *,
                     int, double *, char **);
void WriteOxygenMantleMass(BODY *, CONTROL *, OUTPUT *, SYSTEM *, UNITS *,
                           UPDATE *, int, double *, char **);
void WriteEnvelopeMass(BODY *, CONTROL *, OUTPUT *, SYSTEM *, UNITS *, UPDATE *,
                       int, double *, char **);
void WriteRGLimit(BODY *, CONTROL *, OUTPUT *, SYSTEM *, UNITS *, UPDATE *, int,
                  double *, char **);
void WriteBondiRadius(BODY *, CONTROL *, OUTPUT *, SYSTEM *, UNITS *, UPDATE *,
                      int, double *, char **);
void WriteRocheRadius(BODY *, CONTROL *, OUTPUT *, SYSTEM *, UNITS *, UPDATE *,
                      int, double *, char **);
void WriteOxygenMixingRatio(BODY *, CONTROL *, OUTPUT *, SYSTEM *, UNITS *,
                            UPDATE *, int, double *, char **);
void WriteOxygenEta(BODY *, CONTROL *, OUTPUT *, SYSTEM *, UNITS *, UPDATE *,
                    int, double *, char **);
void WriteAtmXAbsEffH2O(BODY *, CONTROL *, OUTPUT *, SYSTEM *, UNITS *,
                        UPDATE *, int, double *, char **);
void WriteRRCriticalFlux(BODY *, CONTROL *, OUTPUT *, SYSTEM *, UNITS *,
                         UPDATE *, int, double *, char **);
void WriteHEscapeRegime(BODY *, CONTROL *, OUTPUT *, SYSTEM *, UNITS *,
                        UPDATE *, int, double *, char **);

/* Logging Functions */
void LogOptionsAtmEsc(CONTROL *, FILE *);
void LogAtmEsc(BODY *, CONTROL *, OUTPUT *, SYSTEM *, UPDATE *, fnWriteOutput[],
               FILE *);
void LogBodyAtmEsc(BODY *, CONTROL *, OUTPUT *, SYSTEM *, UPDATE *,
                   fnWriteOutput[], FILE *, int);

/* AtmEsc functions */
void fnForceBehaviorAtmEsc(BODY *, MODULE *, EVOLVE *, IO *, SYSTEM *, UPDATE *,
                           fnUpdateVariable ***, int, int);
void fnPropsAuxAtmEsc(BODY *, EVOLVE *, IO *, UPDATE *, int);

double fdDSurfaceWaterMassDt(BODY *, SYSTEM *, int *);
double fdDEnvelopeMassDt(BODY *, SYSTEM *, int *);
double fdDEnvelopeMassDtBondiLimited(BODY *, SYSTEM *, int *);
double fdDEnvelopeMassDtRRLimited(BODY *, SYSTEM *, int *);
double fdHZRG14(BODY *, int);
void fvLinearFit(double *, double *, int, double *);
double fdDOxygenMassDt(BODY *, SYSTEM *, int *);
double fdDOxygenMantleMassDt(BODY *, SYSTEM *, int *);
double fdAtomicOxygenMixingRatio(double, double);
double fdMolecOxygenMixingRatio(double, double);
double fdWaterAtmMixingRatio(double, double);
double fdInsolation(BODY *, int, int);
int fbDoesWaterEscape(BODY *, EVOLVE *, IO *, int);
double fdPlanetRadius(BODY *, SYSTEM *, int *);
double fdXUVEfficiencyBolmont2016(double);
double fdBondiLimitedDmDt(BODY *, int);
int fbRRCriticalFlux(BODY *, int);
int fbBondiCriticalDmDt(BODY *, int);
double fdRRCriticalFlux(BODY *, int);
void fvAtmEscRegimeChangeOutput(int, int, double);

/* Magnetic-limited escape models (atmesc.c) */
double fdMagLimitedEscapeDtGunell18(BODY *, int);
double fdMagLimitedEscapeDtDriscoll13(BODY *, int);
void VerifyMagLimitedEscape(BODY *, CONTROL *, OPTIONS *, int);
void ReadMagLimitedEscapeModel(BODY *, CONTROL *, FILES *, OPTIONS *,
                               SYSTEM *, int);
double fdMagnetopauseStandoff(double, double, double, double);
double fdGunellPickupRate(BODY *, int, double);
double fdGunellCrossFieldRate(BODY *, int, double);
double fdGunellPolarCapRate(BODY *, int);
double fdGunellCuspRate(BODY *, int, double);
double fdDriscollMagLimitedRate(BODY *, int, double);
double fdDSurfaceWaterMassDtMagLim(BODY *, SYSTEM *, int *);
double fdDOxygenMassDtMagLim(BODY *, SYSTEM *, int *);
double fdDEnvelopeMassDtMagLim(BODY *, SYSTEM *, int *);
double fdDCO2MassDtMagLim(BODY *, SYSTEM *, int *);
double fdDN2MassDtMagLim(BODY *, SYSTEM *, int *);
void FinalizeUpdateCO2MassAtmEsc(BODY *, UPDATE *, int *, int, int, int);
void FinalizeUpdateN2MassAtmEsc(BODY *, UPDATE *, int *, int, int, int);

/* Dummy functions */
double fdSurfEnFluxAtmEsc(BODY *, SYSTEM *, UPDATE *, int, int);

/* @endcond */
