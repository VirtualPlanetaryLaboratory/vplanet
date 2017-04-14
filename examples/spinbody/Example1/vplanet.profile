Flat profile:

Each sample counts as 0.01 seconds.
  %   cumulative   self              self     total           
 time   seconds   seconds    calls   s/call   s/call  name    
 40.80     69.49    69.49                             __ieee754_pow_sse2
 21.40    105.94    36.45                             __exp1
 10.23    123.36    17.43 21166783     0.00     0.00  fdGetUpdateInfo
  8.40    137.66    14.30                             pow
  7.42    150.31    12.64  5291616     0.00     0.00  RungeKutta4Step
  2.77    155.03     4.72 105833915     0.00     0.00  fdDVelYDt
  2.60    159.46     4.43 105833915     0.00     0.00  fdDVelXDt
  2.48    163.69     4.22 105833915     0.00     0.00  fdDVelZDt
  0.69    164.87     1.18                             sqrt
  0.53    165.76     0.90 105833915     0.00     0.00  fdDPositionYDt
  0.42    166.48     0.72  5291616     0.00     0.00  BodyCopy
  0.41    167.18     0.70 26458080     0.00     0.00  BodyCopySpiNBody
  0.31    167.71     0.53 21166467     0.00     0.00  PropertiesAuxiliary
  0.26    168.15     0.43 105833915     0.00     0.00  fdDPositionZDt
  0.24    168.56     0.41 21166467     0.00     0.00  PropsAuxGeneral
  0.20    168.90     0.34      317     0.00     0.00  WriteOutput
  0.16    169.16     0.27        1     0.27    48.39  Evolve
  0.13    169.38     0.22 105833915     0.00     0.00  fdDPositionXDt
  0.09    169.54     0.16 84665868     0.00     0.00  fdSemiToMeanMotion
  0.09    169.71     0.16                             __sqr
  0.05    169.79     0.09  5291616     0.00     0.00  fbCheckHalt
  0.05    169.88     0.08 105832335     0.00     0.00  PropertiesSpiNBody
  0.04    169.94     0.06                             __memcmp_sse4_1
  0.04    170.00     0.06                             __mul
  0.03    170.04     0.04 26458080     0.00     0.00  ForceBehaviorSpiNBody
  0.02    170.08     0.04                             strlen
  0.02    170.12     0.04  5291617     0.00     0.00  AssignDt
  0.02    170.15     0.03                             __write_nocancel
  0.01    170.16     0.02                             __mpexp
  0.01    170.18     0.01  5291934     0.00     0.00  fdNextOutput
  0.01    170.19     0.01        5     0.00     0.00  FinalizeUpdateVelXSpiNBody
  0.01    170.21     0.01        5     0.00     0.00  AddModuleSpiNBody
  0.01    170.22     0.01        5     0.00     0.00  InitializeUpdateTmpBodySpiNBody
  0.01    170.22     0.01                             BodyCopyStellar
  0.01    170.24     0.01                             _IO_file_close
  0.01    170.25     0.01                             _IO_unsave_markers
  0.01    170.25     0.01                             __add
  0.01    170.26     0.01                             __open_nocancel
  0.01    170.28     0.01                             __printf_fp_l
  0.01    170.28     0.01                             __strcpy_sse2_unaligned
  0.01    170.29     0.01                             _fxstat
  0.01    170.31     0.01                             fdOrbAngMom
  0.00    170.31     0.01                             EulerStep
  0.00    170.31     0.00   124574     0.00     0.00  sLower
  0.00    170.31     0.00    12803     0.00     0.00  fprintd
  0.00    170.31     0.00     9546     0.00     0.00  CheckComment
  0.00    170.31     0.00     3218     0.00     0.00  fdUnitsTime
  0.00    170.31     0.00     3208     0.00     0.00  fsUnitsTime
  0.00    170.31     0.00     1605     0.00     0.00  WritePositionX
  0.00    170.31     0.00     1605     0.00     0.00  WritePositionY
  0.00    170.31     0.00     1605     0.00     0.00  WritePositionZ
  0.00    170.31     0.00     1605     0.00     0.00  WriteVelX
  0.00    170.31     0.00     1605     0.00     0.00  WriteVelY
  0.00    170.31     0.00     1605     0.00     0.00  WriteVelZ
  0.00    170.31     0.00     1597     0.00     0.00  WriteDeltaTime
  0.00    170.31     0.00     1597     0.00     0.00  WriteTime
  0.00    170.31     0.00      406     0.00     0.00  GetLine
  0.00    170.31     0.00      245     0.00     0.00  AddOptionDouble
  0.00    170.31     0.00      119     0.00     0.00  CGSUnits
  0.00    170.31     0.00      118     0.00     0.00  WriteLogEntry
  0.00    170.31     0.00       79     0.00     0.00  UpdateFoundOption
  0.00    170.31     0.00       77     0.00     0.00  AddOptionInt
  0.00    170.31     0.00       75     0.00     0.00  NotPrimaryInput
  0.00    170.31     0.00       72     0.00     0.00  AddOptionString
  0.00    170.31     0.00       61     0.00     0.00  AssignDefaultDouble
  0.00    170.31     0.00       53     0.00     0.00  AddOptionBool
  0.00    170.31     0.00       45     0.00     0.00  fdUnitsLength
  0.00    170.31     0.00       36     0.00     0.00  AssignDefaultInt
  0.00    170.31     0.00       32     0.00     0.00  fsUnitsLength
  0.00    170.31     0.00       20     0.00     0.00  fdUnitsMass
  0.00    170.31     0.00       20     0.00     0.00  fsUnitsMass
  0.00    170.31     0.00       19     0.00     0.00  dNegativeDouble
  0.00    170.31     0.00       12     0.00     0.00  AddOptionStringArray
  0.00    170.31     0.00       12     0.00     0.00  GetPos
  0.00    170.31     0.00       12     0.00     0.00  GetWords
  0.00    170.31     0.00       12     0.00     0.00  xinit
  0.00    170.31     0.00       12     0.00     0.00  yinit
  0.00    170.31     0.00       11     0.00     0.00  UpdateFoundOptionMulti
  0.00    170.31     0.00       10     0.00     0.00  LogBodyRelations
  0.00    170.31     0.00       10     0.00     0.00  LogBodySpiNBody
  0.00    170.31     0.00       10     0.00     0.00  LogGridOutput
  0.00    170.31     0.00       10     0.00     0.00  LogOutputOrder
  0.00    170.31     0.00       10     0.00     0.00  PrintModuleList
  0.00    170.31     0.00       10     0.00     0.00  WriteBodyType
  0.00    170.31     0.00       10     0.00     0.00  WriteHZLimitDryRunaway
  0.00    170.31     0.00       10     0.00     0.00  WriteMass
  0.00    170.31     0.00       10     0.00     0.00  WriteRadius
  0.00    170.31     0.00        9     0.00     0.00  CheckDuplication
  0.00    170.31     0.00        8     0.00     0.00  fsUnitsEnergy
  0.00    170.31     0.00        8     0.00     0.00  xangle1
  0.00    170.31     0.00        8     0.00     0.00  xangle2
  0.00    170.31     0.00        8     0.00     0.00  yangle1
  0.00    170.31     0.00        8     0.00     0.00  yangle2
  0.00    170.31     0.00        8     0.00     0.00  zangle1
  0.00    170.31     0.00        8     0.00     0.00  zangle2
  0.00    170.31     0.00        6     0.00     0.00  InitializeInput
  0.00    170.31     0.00        6     0.00     0.00  ReadAge
  0.00    170.31     0.00        6     0.00     0.00  ReadAlbedoGlobal
  0.00    170.31     0.00        6     0.00     0.00  ReadArgP
  0.00    170.31     0.00        6     0.00     0.00  ReadBodyName
  0.00    170.31     0.00        6     0.00     0.00  ReadBodyType
  0.00    170.31     0.00        6     0.00     0.00  ReadCalcDynEllip
  0.00    170.31     0.00        6     0.00     0.00  ReadColor
  0.00    170.31     0.00        6     0.00     0.00  ReadDigits
  0.00    170.31     0.00        6     0.00     0.00  ReadDoBackward
  0.00    170.31     0.00        6     0.00     0.00  ReadDoForward
  0.00    170.31     0.00        6     0.00     0.00  ReadDoLog
  0.00    170.31     0.00        6     0.00     0.00  ReadDynEllip
  0.00    170.31     0.00        6     0.00     0.00  ReadEcc
  0.00    170.31     0.00        6     0.00     0.00  ReadEta
  0.00    170.31     0.00        6     0.00     0.00  ReadHaltMaxEcc
  0.00    170.31     0.00        6     0.00     0.00  ReadHaltMerge
  0.00    170.31     0.00        6     0.00     0.00  ReadHaltMinEcc
  0.00    170.31     0.00        6     0.00     0.00  ReadHaltMinObl
  0.00    170.31     0.00        6     0.00     0.00  ReadHaltMinSemi
  0.00    170.31     0.00        6     0.00     0.00  ReadHaltPosDeDt
  0.00    170.31     0.00        6     0.00     0.00  ReadHecc
  0.00    170.31     0.00        6     0.00     0.00  ReadInc
  0.00    170.31     0.00        6     0.00     0.00  ReadIntegrationMethod
  0.00    170.31     0.00        6     0.00     0.00  ReadKecc
  0.00    170.31     0.00        6     0.00     0.00  ReadLXUV
  0.00    170.31     0.00        6     0.00     0.00  ReadLogFile
  0.00    170.31     0.00        6     0.00     0.00  ReadLongA
  0.00    170.31     0.00        6     0.00     0.00  ReadLongP
  0.00    170.31     0.00        6     0.00     0.00  ReadMass
  0.00    170.31     0.00        6     0.00     0.00  ReadMassRad
  0.00    170.31     0.00        6     0.00     0.00  ReadMeanMotion
  0.00    170.31     0.00        6     0.00     0.00  ReadMinValue
  0.00    170.31     0.00        6     0.00     0.00  ReadModules
  0.00    170.31     0.00        6     0.00     0.00  ReadObliquity
  0.00    170.31     0.00        6     0.00     0.00  ReadOrbPeriod
  0.00    170.31     0.00        6     0.00     0.00  ReadOutFile
  0.00    170.31     0.00        6     0.00     0.00  ReadOutputTime
  0.00    170.31     0.00        6     0.00     0.00  ReadOverwrite
  0.00    170.31     0.00        6     0.00     0.00  ReadPrecA
  0.00    170.31     0.00        6     0.00     0.00  ReadRadius
  0.00    170.31     0.00        6     0.00     0.00  ReadRadiusGyration
  0.00    170.31     0.00        6     0.00     0.00  ReadRotPeriod
  0.00    170.31     0.00        6     0.00     0.00  ReadRotRate
  0.00    170.31     0.00        6     0.00     0.00  ReadRotVel
  0.00    170.31     0.00        6     0.00     0.00  ReadSciNot
  0.00    170.31     0.00        6     0.00     0.00  ReadSemiMajorAxis
  0.00    170.31     0.00        6     0.00     0.00  ReadStopTime
  0.00    170.31     0.00        6     0.00     0.00  ReadSystemName
  0.00    170.31     0.00        6     0.00     0.00  ReadTimeStep
  0.00    170.31     0.00        6     0.00     0.00  ReadUnitAngle
  0.00    170.31     0.00        6     0.00     0.00  ReadUnitLength
  0.00    170.31     0.00        6     0.00     0.00  ReadUnitMass
  0.00    170.31     0.00        6     0.00     0.00  ReadUnitTemp
  0.00    170.31     0.00        6     0.00     0.00  ReadUnitTime
  0.00    170.31     0.00        6     0.00     0.00  ReadVarDt
  0.00    170.31     0.00        6     0.00     0.00  ReadVerbose
  0.00    170.31     0.00        6     0.00     0.00  ReadViscUMan
  0.00    170.31     0.00        6     0.00     0.00  ReadXobl
  0.00    170.31     0.00        6     0.00     0.00  ReadYobl
  0.00    170.31     0.00        6     0.00     0.00  ReadZobl
  0.00    170.31     0.00        6     0.00     0.00  fdUnitsEnergy
  0.00    170.31     0.00        6     0.00     0.00  iGetNumLines
  0.00    170.31     0.00        5     0.00     0.00  AssignDefaultString
  0.00    170.31     0.00        5     0.00     0.00  CountHaltsSpiNBody
  0.00    170.31     0.00        5     0.00     0.00  FinalizeModule
  0.00    170.31     0.00        5     0.00     0.00  FinalizeUpdatePositionXSpiNBody
  0.00    170.31     0.00        5     0.00     0.00  FinalizeUpdatePositionYSpiNBody
  0.00    170.31     0.00        5     0.00     0.00  FinalizeUpdatePositionZSpiNBody
  0.00    170.31     0.00        5     0.00     0.00  FinalizeUpdateVelYSpiNBody
  0.00    170.31     0.00        5     0.00     0.00  FinalizeUpdateVelZSpiNBody
  0.00    170.31     0.00        5     0.00     0.00  Helio2Bary
  0.00    170.31     0.00        5     0.00     0.00  InitializeBodySpiNBody
  0.00    170.31     0.00        5     0.00     0.00  InitializeControlNULL
  0.00    170.31     0.00        5     0.00     0.00  InitializeUpdateBodyPerts
  0.00    170.31     0.00        5     0.00     0.00  InitializeUpdateSpiNBody
  0.00    170.31     0.00        5     0.00     0.00  InitializeUpdateTmpBody
  0.00    170.31     0.00        5     0.00     0.00  OrbElems2Helio
  0.00    170.31     0.00        5     0.00     0.00  ReadMeanA
  0.00    170.31     0.00        5     0.00     0.00  ReadOptionsSpiNBody
  0.00    170.31     0.00        5     0.00     0.00  ReadOutputOrder
  0.00    170.31     0.00        5     0.00     0.00  ReadPositionX
  0.00    170.31     0.00        5     0.00     0.00  ReadPositionY
  0.00    170.31     0.00        5     0.00     0.00  ReadPositionZ
  0.00    170.31     0.00        5     0.00     0.00  ReadUseOrbParams
  0.00    170.31     0.00        5     0.00     0.00  ReadVelX
  0.00    170.31     0.00        5     0.00     0.00  ReadVelY
  0.00    170.31     0.00        5     0.00     0.00  ReadVelZ
  0.00    170.31     0.00        5     0.00     0.00  VerifyHaltSpiNBody
  0.00    170.31     0.00        5     0.00     0.00  VerifyInterior
  0.00    170.31     0.00        5     0.00     0.00  VerifyMassRad
  0.00    170.31     0.00        5     0.00     0.00  VerifyModuleMulti
  0.00    170.31     0.00        5     0.00     0.00  VerifyModuleMultiAtmescEqtide
  0.00    170.31     0.00        5     0.00     0.00  VerifyModuleMultiAtmescEqtideThermint
  0.00    170.31     0.00        5     0.00     0.00  VerifyModuleMultiBinaryEqtide
  0.00    170.31     0.00        5     0.00     0.00  VerifyModuleMultiDistOrbDistRot
  0.00    170.31     0.00        5     0.00     0.00  VerifyModuleMultiEqtideDistOrb
  0.00    170.31     0.00        5     0.00     0.00  VerifyModuleMultiEqtideDistorb
  0.00    170.31     0.00        5     0.00     0.00  VerifyModuleMultiEqtideStellar
  0.00    170.31     0.00        5     0.00     0.00  VerifyModuleMultiEqtideThermint
  0.00    170.31     0.00        5     0.00     0.00  VerifyModuleMultiFlareStellar
  0.00    170.31     0.00        5     0.00     0.00  VerifyModuleMultiRadheatThermint
  0.00    170.31     0.00        5     0.00     0.00  VerifyPerturbersSpiNBody
  0.00    170.31     0.00        5     0.00     0.00  VerifyPositionX
  0.00    170.31     0.00        5     0.00     0.00  VerifyPositionY
  0.00    170.31     0.00        5     0.00     0.00  VerifyPositionZ
  0.00    170.31     0.00        5     0.00     0.00  VerifyRotationGeneral
  0.00    170.31     0.00        5     0.00     0.00  VerifySpiNBody
  0.00    170.31     0.00        5     0.00     0.00  VerifyVelX
  0.00    170.31     0.00        5     0.00     0.00  VerifyVelY
  0.00    170.31     0.00        5     0.00     0.00  VerifyVelZ
  0.00    170.31     0.00        5     0.00     0.00  bFileExists
  0.00    170.31     0.00        5     0.00     0.00  fdPerToFreq
  0.00    170.31     0.00        5     0.00     0.00  fiNumHalts
  0.00    170.31     0.00        5     0.00     0.00  iAssignTempUnit
  0.00    170.31     0.00        4     0.00     0.00  fdKinEnergy
  0.00    170.31     0.00        4     0.00     0.00  fdPotEnergy
  0.00    170.31     0.00        4     0.00     0.00  kepler_eqn
  0.00    170.31     0.00        4     0.00     0.00  signf
  0.00    170.31     0.00        4     0.00     0.00  vxi
  0.00    170.31     0.00        4     0.00     0.00  vyi
  0.00    170.31     0.00        2     0.00     0.00  InitializeBodyModules
  0.00    170.31     0.00        2     0.00     0.00  LogBody
  0.00    170.31     0.00        2     0.00     0.00  LogSystem
  0.00    170.31     0.00        2     0.00     0.00  WriteAge
  0.00    170.31     0.00        2     0.00     0.00  WriteKinEnergy
  0.00    170.31     0.00        2     0.00     0.00  WriteLXUVTot
  0.00    170.31     0.00        2     0.00     0.00  WriteLog
  0.00    170.31     0.00        2     0.00     0.00  WriteLongP
  0.00    170.31     0.00        2     0.00     0.00  WritePotEnergy
  0.00    170.31     0.00        2     0.00     0.00  WriteTotAngMom
  0.00    170.31     0.00        2     0.00     0.00  WriteTotEnergy
  0.00    170.31     0.00        2     0.00     0.00  fdTotAngMom
  0.00    170.31     0.00        2     0.00     0.00  fdTotEnergy
  0.00    170.31     0.00        2     0.00     0.00  fdUnitsAngle
  0.00    170.31     0.00        2     0.00     0.00  fdUnitsEnergyFlux
  0.00    170.31     0.00        2     0.00     0.00  fdUnitsPower
  0.00    170.31     0.00        2     0.00     0.00  fsUnitsAngMom
  0.00    170.31     0.00        2     0.00     0.00  fsUnitsAngle
  0.00    170.31     0.00        2     0.00     0.00  fsUnitsEnergyFlux
  0.00    170.31     0.00        1     0.00     0.00  InfileCopy
  0.00    170.31     0.00        1     0.00     0.00  InitializeControl
  0.00    170.31     0.00        1     0.00     0.00  InitializeControlEvolve
  0.00    170.31     0.00        1     0.00     0.00  InitializeHalts
  0.00    170.31     0.00        1     0.00     0.00  InitializeModule
  0.00    170.31     0.00        1     0.00     0.00  InitializeOptions
  0.00    170.31     0.00        1     0.00     0.00  InitializeOptionsAtmEsc
  0.00    170.31     0.00        1     0.00     0.00  InitializeOptionsBinary
  0.00    170.31     0.00        1     0.00     0.00  InitializeOptionsDistOrb
  0.00    170.31     0.00        1     0.00     0.00  InitializeOptionsDistRot
  0.00    170.31     0.00        1     0.00     0.00  InitializeOptionsEqtide
  0.00    170.31     0.00        1     0.00     0.00  InitializeOptionsFlare
  0.00    170.31     0.00        1     0.00     0.00  InitializeOptionsGalHabit
  0.00    170.31     0.00        1     0.00     0.00  InitializeOptionsGeneral
  0.00    170.31     0.00        1     0.00     0.00  InitializeOptionsPoise
  0.00    170.31     0.00        1     0.00     0.00  InitializeOptionsRadheat
  0.00    170.31     0.00        1     0.00     0.00  InitializeOptionsSpiNBody
  0.00    170.31     0.00        1     0.00     0.00  InitializeOptionsStellar
  0.00    170.31     0.00        1     0.00     0.00  InitializeOptionsThermint
  0.00    170.31     0.00        1     0.00     0.00  InitializeOutput
  0.00    170.31     0.00        1     0.00     0.00  InitializeOutputAtmEsc
  0.00    170.31     0.00        1     0.00     0.00  InitializeOutputBinary
  0.00    170.31     0.00        1     0.00     0.00  InitializeOutputDistOrb
  0.00    170.31     0.00        1     0.00     0.00  InitializeOutputDistRot
  0.00    170.31     0.00        1     0.00     0.00  InitializeOutputEqtide
  0.00    170.31     0.00        1     0.00     0.00  InitializeOutputFlare
  0.00    170.31     0.00        1     0.00     0.00  InitializeOutputGalHabit
  0.00    170.31     0.00        1     0.00     0.00  InitializeOutputGeneral
  0.00    170.31     0.00        1     0.00     0.00  InitializeOutputPoise
  0.00    170.31     0.00        1     0.00     0.00  InitializeOutputRadheat
  0.00    170.31     0.00        1     0.00     0.00  InitializeOutputSpiNBody
  0.00    170.31     0.00        1     0.00     0.00  InitializeOutputStellar
  0.00    170.31     0.00        1     0.00     0.00  InitializeOutputThermint
  0.00    170.31     0.00        1     0.00     0.01  InitializeUpdate
  0.00    170.31     0.00        1     0.00     0.00  LogHalt
  0.00    170.31     0.00        1     0.00     0.00  LogIntegration
  0.00    170.31     0.00        1     0.00     0.00  LogOptions
  0.00    170.31     0.00        1     0.00     0.00  LogUnits
  0.00    170.31     0.00        1     0.00     0.00  ReadBodyFileNames
  0.00    170.31     0.00        1     0.00     0.01  ReadInitialOptions
  0.00    170.31     0.00        1     0.00     0.01  ReadOptions
  0.00    170.31     0.00        1     0.00     0.00  ReadOptionsGeneral
  0.00    170.31     0.00        1     0.00     0.00  ReadOptionsModules
  0.00    170.31     0.00        1     0.00     0.00  Unrecognized
  0.00    170.31     0.00        1     0.00     0.00  UpdateCopy
  0.00    170.31     0.00        1     0.00     0.00  VerifyHalts
  0.00    170.31     0.00        1     0.00     0.00  VerifyIntegration
  0.00    170.31     0.00        1     0.00     0.03  VerifyOptions
  0.00    170.31     0.00        1     0.00     0.00  iAssignMassUnit
  0.00    170.31     0.00        1     0.00     0.00  iAssignUnitAngle
  0.00    170.31     0.00        1     0.00     0.00  iAssignUnitLength
  0.00    170.31     0.00        1     0.00     0.00  iAssignUnitTime
  0.00    170.31     0.00        1     0.00    48.42  main

 %         the percentage of the total running time of the
time       program used by this function.

cumulative a running sum of the number of seconds accounted
 seconds   for by this function and those listed above it.

 self      the number of seconds accounted for by this
seconds    function alone.  This is the major sort for this
           listing.

calls      the number of times this function was invoked, if
           this function is profiled, else blank.

 self      the average number of milliseconds spent in this
ms/call    function per call, if this function is profiled,
	   else blank.

 total     the average number of milliseconds spent in this
ms/call    function and its descendents per call, if this
	   function is profiled, else blank.

name       the name of the function.  This is the minor sort
           for this listing. The index shows the location of
	   the function in the gprof listing. If the index is
	   in parenthesis it shows where it would appear in
	   the gprof listing if it were to be printed.

Copyright (C) 2012-2016 Free Software Foundation, Inc.

Copying and distribution of this file, with or without modification,
are permitted in any medium without royalty provided the copyright
notice and this notice are preserved.

		     Call graph (explanation follows)


granularity: each sample hit covers 4 byte(s) for 0.01% of 170.31 seconds

index % time    self  children    called     name
                                                 <spontaneous>
[1]     40.8   69.49    0.00                 __ieee754_pow_sse2 [1]
-----------------------------------------------
                0.00   48.42       1/1           generic_start_main [3]
[2]     28.4    0.00   48.42       1         main [2]
                0.27   48.12       1/1           Evolve [4]
                0.00    0.03       1/1           VerifyOptions [31]
                0.00    0.01       1/1           ReadOptions [41]
                0.00    0.00       2/2           WriteLog [52]
                0.00    0.00       1/1           InitializeOptions [247]
                0.00    0.00       1/1           InitializeOutput [261]
-----------------------------------------------
                                                 <spontaneous>
[3]     28.4    0.00   48.42                 generic_start_main [3]
                0.00   48.42       1/1           main [2]
-----------------------------------------------
                0.27   48.12       1/1           main [2]
[4]     28.4    0.27   48.12       1         Evolve [4]
               12.64   34.71 5291616/5291616     RungeKutta4Step [5]
                0.34    0.00     317/317         WriteOutput [19]
                0.13    0.16 5291617/21166467     PropertiesAuxiliary [13]
                0.09    0.00 5291616/5291616     fbCheckHalt [23]
                0.04    0.00 26458080/26458080     ForceBehaviorSpiNBody [27]
                0.00    0.00     317/21166783     fdGetUpdateInfo [7]
                0.00    0.00     318/5291934     fdNextOutput [33]
                0.00    0.00       1/5291617     AssignDt [29]
                0.00    0.00       1/1           UpdateCopy [283]
-----------------------------------------------
               12.64   34.71 5291616/5291616     Evolve [4]
[5]     27.8   12.64   34.71 5291616         RungeKutta4Step [5]
               17.42   14.93 21166464/21166783     fdGetUpdateInfo [7]
                0.72    0.70 5291616/5291616     BodyCopy [12]
                0.40    0.49 15874848/21166467     PropertiesAuxiliary [13]
                0.03    0.00 5291616/5291617     AssignDt [29]
                0.01    0.00 5291616/5291934     fdNextOutput [33]
-----------------------------------------------
                                                 <spontaneous>
[6]     21.4   36.45    0.00                 __exp1 [6]
-----------------------------------------------
                0.00    0.00       2/21166783     WriteLog [52]
                0.00    0.00     317/21166783     Evolve [4]
               17.42   14.93 21166464/21166783     RungeKutta4Step [5]
[7]     19.0   17.43   14.93 21166783         fdGetUpdateInfo [7]
                4.72    0.00 105833915/105833915     fdDVelYDt [9]
                4.43    0.00 105833915/105833915     fdDVelXDt [10]
                4.22    0.00 105833915/105833915     fdDVelZDt [11]
                0.90    0.00 105833915/105833915     fdDPositionYDt [15]
                0.43    0.00 105833915/105833915     fdDPositionZDt [18]
                0.22    0.00 105833915/105833915     fdDPositionXDt [20]
-----------------------------------------------
                                                 <spontaneous>
[8]      8.4   14.30    0.00                 pow [8]
-----------------------------------------------
                4.72    0.00 105833915/105833915     fdGetUpdateInfo [7]
[9]      2.8    4.72    0.00 105833915         fdDVelYDt [9]
-----------------------------------------------
                4.43    0.00 105833915/105833915     fdGetUpdateInfo [7]
[10]     2.6    4.43    0.00 105833915         fdDVelXDt [10]
-----------------------------------------------
                4.22    0.00 105833915/105833915     fdGetUpdateInfo [7]
[11]     2.5    4.22    0.00 105833915         fdDVelZDt [11]
-----------------------------------------------
                0.72    0.70 5291616/5291616     RungeKutta4Step [5]
[12]     0.8    0.72    0.70 5291616         BodyCopy [12]
                0.70    0.00 26458080/26458080     BodyCopySpiNBody [16]
-----------------------------------------------
                0.00    0.00       2/21166467     WriteLog [52]
                0.13    0.16 5291617/21166467     Evolve [4]
                0.40    0.49 15874848/21166467     RungeKutta4Step [5]
[13]     0.7    0.53    0.65 21166467         PropertiesAuxiliary [13]
                0.41    0.16 21166467/21166467     PropsAuxGeneral [17]
                0.08    0.00 105832335/105832335     PropertiesSpiNBody [24]
-----------------------------------------------
                                                 <spontaneous>
[14]     0.7    1.18    0.00                 sqrt [14]
-----------------------------------------------
                0.90    0.00 105833915/105833915     fdGetUpdateInfo [7]
[15]     0.5    0.90    0.00 105833915         fdDPositionYDt [15]
-----------------------------------------------
                0.70    0.00 26458080/26458080     BodyCopy [12]
[16]     0.4    0.70    0.00 26458080         BodyCopySpiNBody [16]
-----------------------------------------------
                0.41    0.16 21166467/21166467     PropertiesAuxiliary [13]
[17]     0.3    0.41    0.16 21166467         PropsAuxGeneral [17]
                0.16    0.00 84665868/84665868     fdSemiToMeanMotion [21]
-----------------------------------------------
                0.43    0.00 105833915/105833915     fdGetUpdateInfo [7]
[18]     0.3    0.43    0.00 105833915         fdDPositionZDt [18]
-----------------------------------------------
                0.34    0.00     317/317         Evolve [4]
[19]     0.2    0.34    0.00     317         WriteOutput [19]
                0.00    0.00   12680/12803       fprintd [54]
                0.00    0.00    1585/1605        WriteVelZ [63]
                0.00    0.00    1585/1605        WriteVelY [62]
                0.00    0.00    1585/1605        WriteVelX [61]
                0.00    0.00    1585/1605        WritePositionZ [60]
                0.00    0.00    1585/1605        WritePositionY [59]
                0.00    0.00    1585/1605        WritePositionX [58]
                0.00    0.00    1585/1597        WriteDeltaTime [64]
                0.00    0.00    1585/1597        WriteTime [65]
-----------------------------------------------
                0.22    0.00 105833915/105833915     fdGetUpdateInfo [7]
[20]     0.1    0.22    0.00 105833915         fdDPositionXDt [20]
-----------------------------------------------
                0.16    0.00 84665868/84665868     PropsAuxGeneral [17]
[21]     0.1    0.16    0.00 84665868         fdSemiToMeanMotion [21]
-----------------------------------------------
                                                 <spontaneous>
[22]     0.1    0.16    0.00                 __sqr [22]
-----------------------------------------------
                0.09    0.00 5291616/5291616     Evolve [4]
[23]     0.1    0.09    0.00 5291616         fbCheckHalt [23]
-----------------------------------------------
                0.08    0.00 105832335/105832335     PropertiesAuxiliary [13]
[24]     0.0    0.08    0.00 105832335         PropertiesSpiNBody [24]
-----------------------------------------------
                                                 <spontaneous>
[25]     0.0    0.06    0.00                 __memcmp_sse4_1 [25]
-----------------------------------------------
                                                 <spontaneous>
[26]     0.0    0.06    0.00                 __mul [26]
-----------------------------------------------
                0.04    0.00 26458080/26458080     Evolve [4]
[27]     0.0    0.04    0.00 26458080         ForceBehaviorSpiNBody [27]
-----------------------------------------------
                                                 <spontaneous>
[28]     0.0    0.04    0.00                 strlen [28]
-----------------------------------------------
                0.00    0.00       1/5291617     Evolve [4]
                0.03    0.00 5291616/5291617     RungeKutta4Step [5]
[29]     0.0    0.04    0.00 5291617         AssignDt [29]
-----------------------------------------------
                                                 <spontaneous>
[30]     0.0    0.03    0.00                 __write_nocancel [30]
-----------------------------------------------
                0.00    0.03       1/1           main [2]
[31]     0.0    0.00    0.03       1         VerifyOptions [31]
                0.00    0.01       1/1           InitializeUpdate [35]
                0.00    0.01       5/5           InitializeUpdateTmpBody [38]
                0.00    0.00       5/5           VerifyMassRad [193]
                0.00    0.00       5/5           InitializeBodySpiNBody [176]
                0.00    0.00       5/5           VerifyRotationGeneral [209]
                0.00    0.00       5/5           VerifySpiNBody [210]
                0.00    0.00       5/5           VerifyInterior [192]
                0.00    0.00       5/5           VerifyModuleMulti [194]
                0.00    0.00       5/5           InitializeUpdateBodyPerts [178]
                0.00    0.00       1/1           VerifyIntegration [285]
                0.00    0.00       1/1           InitializeControlEvolve [244]
                0.00    0.00       1/1           InitializeHalts [245]
                0.00    0.00       1/1           VerifyHalts [284]
-----------------------------------------------
                                                 <spontaneous>
[32]     0.0    0.02    0.00                 __mpexp [32]
-----------------------------------------------
                0.00    0.00     318/5291934     Evolve [4]
                0.01    0.00 5291616/5291934     RungeKutta4Step [5]
[33]     0.0    0.01    0.00 5291934         fdNextOutput [33]
-----------------------------------------------
                0.01    0.00       5/5           InitializeUpdate [35]
[34]     0.0    0.01    0.00       5         FinalizeUpdateVelXSpiNBody [34]
-----------------------------------------------
                0.00    0.01       1/1           VerifyOptions [31]
[35]     0.0    0.00    0.01       1         InitializeUpdate [35]
                0.01    0.00       5/5           FinalizeUpdateVelXSpiNBody [34]
                0.00    0.00       5/5           InitializeUpdateSpiNBody [179]
                0.00    0.00       5/5           FinalizeUpdatePositionXSpiNBody [170]
                0.00    0.00       5/5           FinalizeUpdatePositionYSpiNBody [171]
                0.00    0.00       5/5           FinalizeUpdatePositionZSpiNBody [172]
                0.00    0.00       5/5           FinalizeUpdateVelYSpiNBody [173]
                0.00    0.00       5/5           FinalizeUpdateVelZSpiNBody [174]
-----------------------------------------------
                0.01    0.00       5/5           FinalizeModule [37]
[36]     0.0    0.01    0.00       5         AddModuleSpiNBody [36]
-----------------------------------------------
                0.00    0.01       5/5           ReadInitialOptions [40]
[37]     0.0    0.00    0.01       5         FinalizeModule [37]
                0.01    0.00       5/5           AddModuleSpiNBody [36]
-----------------------------------------------
                0.00    0.01       5/5           VerifyOptions [31]
[38]     0.0    0.00    0.01       5         InitializeUpdateTmpBody [38]
                0.01    0.00       5/5           InitializeUpdateTmpBodySpiNBody [39]
-----------------------------------------------
                0.01    0.00       5/5           InitializeUpdateTmpBody [38]
[39]     0.0    0.01    0.00       5         InitializeUpdateTmpBodySpiNBody [39]
-----------------------------------------------
                0.00    0.01       1/1           ReadOptions [41]
[40]     0.0    0.00    0.01       1         ReadInitialOptions [40]
                0.00    0.01       5/5           FinalizeModule [37]
                0.00    0.00       6/6           InitializeInput [105]
                0.00    0.00       6/6           ReadVerbose [161]
                0.00    0.00       6/6           ReadUnitMass [157]
                0.00    0.00       6/6           ReadUnitTime [159]
                0.00    0.00       6/6           ReadUnitAngle [155]
                0.00    0.00       6/6           ReadUnitLength [156]
                0.00    0.00       6/6           ReadUnitTemp [158]
                0.00    0.00       6/6           ReadSystemName [153]
                0.00    0.00       6/6           ReadModules [138]
                0.00    0.00       1/1           ReadBodyFileNames [279]
                0.00    0.00       1/2           InitializeBodyModules [224]
                0.00    0.00       1/1           InitializeModule [246]
-----------------------------------------------
                0.00    0.01       1/1           main [2]
[41]     0.0    0.00    0.01       1         ReadOptions [41]
                0.00    0.01       1/1           ReadInitialOptions [40]
                0.00    0.00       5/5           ReadOutputOrder [183]
                0.00    0.00       1/1           InitializeControl [243]
                0.00    0.00       1/1           ReadOptionsGeneral [280]
                0.00    0.00       1/1           ReadOptionsModules [281]
                0.00    0.00       1/1           Unrecognized [282]
-----------------------------------------------
                                                 <spontaneous>
[42]     0.0    0.01    0.00                 BodyCopyStellar [42]
-----------------------------------------------
                                                 <spontaneous>
[43]     0.0    0.01    0.00                 fdOrbAngMom [43]
-----------------------------------------------
                                                 <spontaneous>
[44]     0.0    0.01    0.00                 _IO_file_close [44]
-----------------------------------------------
                                                 <spontaneous>
[45]     0.0    0.01    0.00                 _IO_unsave_markers [45]
-----------------------------------------------
                                                 <spontaneous>
[46]     0.0    0.01    0.00                 __add [46]
-----------------------------------------------
                                                 <spontaneous>
[47]     0.0    0.01    0.00                 __open_nocancel [47]
-----------------------------------------------
                                                 <spontaneous>
[48]     0.0    0.01    0.00                 __printf_fp_l [48]
-----------------------------------------------
                                                 <spontaneous>
[49]     0.0    0.01    0.00                 __strcpy_sse2_unaligned [49]
-----------------------------------------------
                                                 <spontaneous>
[50]     0.0    0.01    0.00                 _fxstat [50]
-----------------------------------------------
                                                 <spontaneous>
[51]     0.0    0.01    0.00                 EulerStep [51]
-----------------------------------------------
                0.00    0.00       2/2           main [2]
[52]     0.0    0.00    0.00       2         WriteLog [52]
                0.00    0.00       2/21166783     fdGetUpdateInfo [7]
                0.00    0.00       2/21166467     PropertiesAuxiliary [13]
                0.00    0.00       2/2           LogSystem [226]
                0.00    0.00       2/2           LogBody [225]
                0.00    0.00       1/1           LogOptions [277]
-----------------------------------------------
                0.00    0.00       1/124574      iAssignUnitTime [289]
                0.00    0.00       2/124574      iAssignMassUnit [286]
                0.00    0.00       2/124574      iAssignUnitAngle [287]
                0.00    0.00       2/124574      iAssignUnitLength [288]
                0.00    0.00       2/124574      VerifyIntegration [285]
                0.00    0.00       5/124574      iAssignTempUnit [217]
                0.00    0.00      60/124574      ReadModules [138]
                0.00    0.00  124500/124574      ReadOutputOrder [183]
[53]     0.0    0.00    0.00  124574         sLower [53]
-----------------------------------------------
                0.00    0.00       1/12803       LogHalt [275]
                0.00    0.00       4/12803       LogIntegration [276]
                0.00    0.00     118/12803       WriteLogEntry [69]
                0.00    0.00   12680/12803       WriteOutput [19]
[54]     0.0    0.00    0.00   12803         fprintd [54]
-----------------------------------------------
                0.00    0.00     139/9546        InitializeInput [105]
                0.00    0.00    9407/9546        GetLine [66]
[55]     0.0    0.00    0.00    9546         CheckComment [55]
-----------------------------------------------
                0.00    0.00       1/3218        ReadOutputTime [142]
                0.00    0.00       1/3218        ReadStopTime [152]
                0.00    0.00       2/3218        WriteAge [227]
                0.00    0.00       2/3218        WriteTotAngMom [232]
                0.00    0.00       6/3218        fdUnitsPower [238]
                0.00    0.00      12/3218        fdUnitsEnergy [166]
                0.00    0.00    1597/3218        WriteDeltaTime [64]
                0.00    0.00    1597/3218        WriteTime [65]
[56]     0.0    0.00    0.00    3218         fdUnitsTime [56]
-----------------------------------------------
                0.00    0.00       2/3208        fsUnitsAngMom [239]
                0.00    0.00       2/3208        fsUnitsEnergyFlux [241]
                0.00    0.00       2/3208        WriteAge [227]
                0.00    0.00       8/3208        fsUnitsEnergy [98]
                0.00    0.00    1597/3208        WriteDeltaTime [64]
                0.00    0.00    1597/3208        WriteTime [65]
[57]     0.0    0.00    0.00    3208         fsUnitsTime [57]
-----------------------------------------------
                0.00    0.00      10/1605        WriteLogEntry [69]
                0.00    0.00      10/1605        LogOutputOrder [91]
                0.00    0.00    1585/1605        WriteOutput [19]
[58]     0.0    0.00    0.00    1605         WritePositionX [58]
-----------------------------------------------
                0.00    0.00      10/1605        WriteLogEntry [69]
                0.00    0.00      10/1605        LogOutputOrder [91]
                0.00    0.00    1585/1605        WriteOutput [19]
[59]     0.0    0.00    0.00    1605         WritePositionY [59]
-----------------------------------------------
                0.00    0.00      10/1605        WriteLogEntry [69]
                0.00    0.00      10/1605        LogOutputOrder [91]
                0.00    0.00    1585/1605        WriteOutput [19]
[60]     0.0    0.00    0.00    1605         WritePositionZ [60]
-----------------------------------------------
                0.00    0.00      10/1605        WriteLogEntry [69]
                0.00    0.00      10/1605        LogOutputOrder [91]
                0.00    0.00    1585/1605        WriteOutput [19]
[61]     0.0    0.00    0.00    1605         WriteVelX [61]
-----------------------------------------------
                0.00    0.00      10/1605        WriteLogEntry [69]
                0.00    0.00      10/1605        LogOutputOrder [91]
                0.00    0.00    1585/1605        WriteOutput [19]
[62]     0.0    0.00    0.00    1605         WriteVelY [62]
-----------------------------------------------
                0.00    0.00      10/1605        WriteLogEntry [69]
                0.00    0.00      10/1605        LogOutputOrder [91]
                0.00    0.00    1585/1605        WriteOutput [19]
[63]     0.0    0.00    0.00    1605         WriteVelZ [63]
-----------------------------------------------
                0.00    0.00       2/1597        WriteLogEntry [69]
                0.00    0.00      10/1597        LogOutputOrder [91]
                0.00    0.00    1585/1597        WriteOutput [19]
[64]     0.0    0.00    0.00    1597         WriteDeltaTime [64]
                0.00    0.00    1597/3218        fdUnitsTime [56]
                0.00    0.00    1597/3208        fsUnitsTime [57]
-----------------------------------------------
                0.00    0.00       2/1597        WriteLogEntry [69]
                0.00    0.00      10/1597        LogOutputOrder [91]
                0.00    0.00    1585/1597        WriteOutput [19]
[65]     0.0    0.00    0.00    1597         WriteTime [65]
                0.00    0.00    1597/3218        fdUnitsTime [56]
                0.00    0.00    1597/3208        fsUnitsTime [57]
-----------------------------------------------
                0.00    0.00      12/406         AddOptionStringArray [82]
                0.00    0.00      72/406         AddOptionString [73]
                0.00    0.00      77/406         AddOptionInt [71]
                0.00    0.00     245/406         AddOptionDouble [67]
[66]     0.0    0.00    0.00     406         GetLine [66]
                0.00    0.00    9407/9546        CheckComment [55]
-----------------------------------------------
                0.00    0.00       5/245         ReadPositionX [184]
                0.00    0.00       5/245         ReadPositionY [185]
                0.00    0.00       5/245         ReadPositionZ [186]
                0.00    0.00       5/245         ReadVelX [188]
                0.00    0.00       5/245         ReadVelY [189]
                0.00    0.00       5/245         ReadVelZ [190]
                0.00    0.00       5/245         ReadMeanA [181]
                0.00    0.00       6/245         ReadAge [106]
                0.00    0.00       6/245         ReadAlbedoGlobal [107]
                0.00    0.00       6/245         ReadEta [119]
                0.00    0.00       6/245         ReadOutputTime [142]
                0.00    0.00       6/245         ReadStopTime [152]
                0.00    0.00       6/245         ReadTimeStep [154]
                0.00    0.00       6/245         ReadEcc [118]
                0.00    0.00       6/245         ReadHaltMaxEcc [120]
                0.00    0.00       6/245         ReadHaltMinEcc [122]
                0.00    0.00       6/245         ReadHaltMinObl [123]
                0.00    0.00       6/245         ReadHaltMinSemi [124]
                0.00    0.00       6/245         ReadHecc [126]
                0.00    0.00       6/245         ReadKecc [129]
                0.00    0.00       6/245         ReadLongP [133]
                0.00    0.00       6/245         ReadLongA [132]
                0.00    0.00       6/245         ReadArgP [108]
                0.00    0.00       6/245         ReadInc [127]
                0.00    0.00       6/245         ReadLXUV [130]
                0.00    0.00       6/245         ReadMass [134]
                0.00    0.00       6/245         ReadMeanMotion [136]
                0.00    0.00       6/245         ReadMinValue [137]
                0.00    0.00       6/245         ReadObliquity [139]
                0.00    0.00       6/245         ReadOrbPeriod [140]
                0.00    0.00       6/245         ReadPrecA [144]
                0.00    0.00       6/245         ReadDynEllip [117]
                0.00    0.00       6/245         ReadRadius [145]
                0.00    0.00       6/245         ReadRadiusGyration [146]
                0.00    0.00       6/245         ReadRotPeriod [147]
                0.00    0.00       6/245         ReadRotRate [148]
                0.00    0.00       6/245         ReadRotVel [149]
                0.00    0.00       6/245         ReadSemiMajorAxis [151]
                0.00    0.00       6/245         ReadViscUMan [162]
                0.00    0.00       6/245         ReadXobl [163]
                0.00    0.00       6/245         ReadYobl [164]
                0.00    0.00       6/245         ReadZobl [165]
[67]     0.0    0.00    0.00     245         AddOptionDouble [67]
                0.00    0.00     245/406         GetLine [66]
-----------------------------------------------
                0.00    0.00       1/119         LogUnits [278]
                0.00    0.00     118/119         WriteLogEntry [69]
[68]     0.0    0.00    0.00     119         CGSUnits [68]
-----------------------------------------------
                0.00    0.00      18/118         LogSystem [226]
                0.00    0.00      40/118         LogBody [225]
                0.00    0.00      60/118         LogBodySpiNBody [89]
[69]     0.0    0.00    0.00     118         WriteLogEntry [69]
                0.00    0.00     118/119         CGSUnits [68]
                0.00    0.00     118/12803       fprintd [54]
                0.00    0.00      10/1605        WriteVelZ [63]
                0.00    0.00      10/1605        WriteVelY [62]
                0.00    0.00      10/1605        WriteVelX [61]
                0.00    0.00      10/1605        WritePositionZ [60]
                0.00    0.00      10/1605        WritePositionY [59]
                0.00    0.00      10/1605        WritePositionX [58]
                0.00    0.00      10/10          WriteHZLimitDryRunaway [94]
                0.00    0.00      10/10          WriteBodyType [93]
                0.00    0.00      10/10          WriteRadius [96]
                0.00    0.00      10/10          WriteMass [95]
                0.00    0.00       2/2           WriteLXUVTot [229]
                0.00    0.00       2/2           WriteLongP [230]
                0.00    0.00       2/1597        WriteDeltaTime [64]
                0.00    0.00       2/2           WriteKinEnergy [228]
                0.00    0.00       2/2           WritePotEnergy [231]
                0.00    0.00       2/2           WriteTotEnergy [233]
                0.00    0.00       2/2           WriteTotAngMom [232]
                0.00    0.00       2/1597        WriteTime [65]
                0.00    0.00       2/2           WriteAge [227]
-----------------------------------------------
                0.00    0.00       1/79          ReadUnitMass [157]
                0.00    0.00       1/79          ReadUnitTime [159]
                0.00    0.00       1/79          ReadUnitAngle [155]
                0.00    0.00       1/79          ReadUnitLength [156]
                0.00    0.00       1/79          ReadSystemName [153]
                0.00    0.00       1/79          ReadEta [119]
                0.00    0.00       1/79          ReadOutputTime [142]
                0.00    0.00       1/79          ReadStopTime [152]
                0.00    0.00       1/79          ReadVarDt [160]
                0.00    0.00       1/79          ReadDigits [113]
                0.00    0.00       1/79          ReadDoForward [115]
                0.00    0.00       1/79          ReadDoLog [116]
                0.00    0.00       1/79          ReadMinValue [137]
                0.00    0.00       1/79          ReadOverwrite [143]
                0.00    0.00       5/79          ReadBodyName [109]
                0.00    0.00       5/79          ReadEcc [118]
                0.00    0.00       5/79          ReadLongP [133]
                0.00    0.00       5/79          ReadLongA [132]
                0.00    0.00       5/79          ReadInc [127]
                0.00    0.00       5/79          ReadMass [134]
                0.00    0.00       5/79          ReadObliquity [139]
                0.00    0.00       5/79          ReadRadius [145]
                0.00    0.00       5/79          ReadRadiusGyration [146]
                0.00    0.00       5/79          ReadRotPeriod [147]
                0.00    0.00       5/79          ReadSemiMajorAxis [151]
                0.00    0.00       5/79          ReadMeanA [181]
                0.00    0.00       5/79          ReadUseOrbParams [187]
[70]     0.0    0.00    0.00      79         UpdateFoundOption [70]
-----------------------------------------------
                0.00    0.00       6/77          ReadVerbose [161]
                0.00    0.00       6/77          ReadBodyType [110]
                0.00    0.00       6/77          ReadDigits [113]
                0.00    0.00       6/77          ReadSciNot [150]
                0.00    0.00      53/77          AddOptionBool [75]
[71]     0.0    0.00    0.00      77         AddOptionInt [71]
                0.00    0.00      77/406         GetLine [66]
-----------------------------------------------
                0.00    0.00       5/75          ReadModules [138]
                0.00    0.00       5/75          ReadBodyName [109]
                0.00    0.00       5/75          ReadEcc [118]
                0.00    0.00       5/75          ReadLongP [133]
                0.00    0.00       5/75          ReadLongA [132]
                0.00    0.00       5/75          ReadInc [127]
                0.00    0.00       5/75          ReadMass [134]
                0.00    0.00       5/75          ReadObliquity [139]
                0.00    0.00       5/75          ReadOutputOrder [183]
                0.00    0.00       5/75          ReadRadius [145]
                0.00    0.00       5/75          ReadRadiusGyration [146]
                0.00    0.00       5/75          ReadRotPeriod [147]
                0.00    0.00       5/75          ReadSemiMajorAxis [151]
                0.00    0.00       5/75          ReadMeanA [181]
                0.00    0.00       5/75          ReadUseOrbParams [187]
[72]     0.0    0.00    0.00      75         NotPrimaryInput [72]
-----------------------------------------------
                0.00    0.00       6/72          ReadUnitMass [157]
                0.00    0.00       6/72          ReadUnitTime [159]
                0.00    0.00       6/72          ReadUnitAngle [155]
                0.00    0.00       6/72          ReadUnitLength [156]
                0.00    0.00       6/72          ReadUnitTemp [158]
                0.00    0.00       6/72          ReadSystemName [153]
                0.00    0.00       6/72          ReadOutFile [141]
                0.00    0.00       6/72          ReadBodyName [109]
                0.00    0.00       6/72          ReadColor [112]
                0.00    0.00       6/72          ReadIntegrationMethod [128]
                0.00    0.00       6/72          ReadLogFile [131]
                0.00    0.00       6/72          ReadMassRad [135]
[73]     0.0    0.00    0.00      72         AddOptionString [73]
                0.00    0.00      72/406         GetLine [66]
-----------------------------------------------
                0.00    0.00       5/61          ReadAge [106]
                0.00    0.00       5/61          ReadAlbedoGlobal [107]
                0.00    0.00       5/61          ReadEta [119]
                0.00    0.00       5/61          ReadOutputTime [142]
                0.00    0.00       5/61          ReadStopTime [152]
                0.00    0.00       5/61          ReadHaltMinEcc [122]
                0.00    0.00       5/61          ReadHaltMinSemi [124]
                0.00    0.00       5/61          ReadMeanMotion [136]
                0.00    0.00       5/61          ReadMinValue [137]
                0.00    0.00       5/61          ReadOrbPeriod [140]
                0.00    0.00       5/61          ReadViscUMan [162]
                0.00    0.00       6/61          ReadTimeStep [154]
[74]     0.0    0.00    0.00      61         AssignDefaultDouble [74]
-----------------------------------------------
                0.00    0.00       5/53          ReadUseOrbParams [187]
                0.00    0.00       6/53          ReadDoBackward [114]
                0.00    0.00       6/53          ReadVarDt [160]
                0.00    0.00       6/53          ReadDoForward [115]
                0.00    0.00       6/53          ReadHaltMerge [121]
                0.00    0.00       6/53          ReadHaltPosDeDt [125]
                0.00    0.00       6/53          ReadDoLog [116]
                0.00    0.00       6/53          ReadOverwrite [143]
                0.00    0.00       6/53          ReadCalcDynEllip [111]
[75]     0.0    0.00    0.00      53         AddOptionBool [75]
                0.00    0.00      53/77          AddOptionInt [71]
-----------------------------------------------
                0.00    0.00       1/45          ReadSemiMajorAxis [151]
                0.00    0.00       4/45          fdUnitsPower [238]
                0.00    0.00       4/45          fdUnitsEnergyFlux [237]
                0.00    0.00       4/45          WriteTotAngMom [232]
                0.00    0.00      10/45          WriteHZLimitDryRunaway [94]
                0.00    0.00      10/45          WriteRadius [96]
                0.00    0.00      12/45          fdUnitsEnergy [166]
[76]     0.0    0.00    0.00      45         fdUnitsLength [76]
-----------------------------------------------
                0.00    0.00       5/36          ReadBodyType [110]
                0.00    0.00       5/36          ReadVarDt [160]
                0.00    0.00       5/36          ReadDigits [113]
                0.00    0.00       5/36          ReadDoForward [115]
                0.00    0.00       5/36          ReadHaltPosDeDt [125]
                0.00    0.00       5/36          ReadDoLog [116]
                0.00    0.00       6/36          ReadSciNot [150]
[77]     0.0    0.00    0.00      36         AssignDefaultInt [77]
-----------------------------------------------
                0.00    0.00       2/32          fsUnitsAngMom [239]
                0.00    0.00       2/32          fsUnitsEnergyFlux [241]
                0.00    0.00       8/32          fsUnitsEnergy [98]
                0.00    0.00      10/32          WriteHZLimitDryRunaway [94]
                0.00    0.00      10/32          WriteRadius [96]
[78]     0.0    0.00    0.00      32         fsUnitsLength [78]
-----------------------------------------------
                0.00    0.00       2/20          fdUnitsPower [238]
                0.00    0.00       2/20          WriteTotAngMom [232]
                0.00    0.00       6/20          fdUnitsEnergy [166]
                0.00    0.00      10/20          WriteMass [95]
[79]     0.0    0.00    0.00      20         fdUnitsMass [79]
-----------------------------------------------
                0.00    0.00       2/20          fsUnitsAngMom [239]
                0.00    0.00       8/20          fsUnitsEnergy [98]
                0.00    0.00      10/20          WriteMass [95]
[80]     0.0    0.00    0.00      20         fsUnitsMass [80]
-----------------------------------------------
                0.00    0.00       4/19          ReadSemiMajorAxis [151]
                0.00    0.00       5/19          ReadMass [134]
                0.00    0.00       5/19          ReadRadius [145]
                0.00    0.00       5/19          ReadRotPeriod [147]
[81]     0.0    0.00    0.00      19         dNegativeDouble [81]
-----------------------------------------------
                0.00    0.00       1/12          ReadBodyFileNames [279]
                0.00    0.00       5/12          ReadOutputOrder [183]
                0.00    0.00       6/12          ReadModules [138]
[82]     0.0    0.00    0.00      12         AddOptionStringArray [82]
                0.00    0.00      12/406         GetLine [66]
                0.00    0.00      12/12          GetWords [84]
-----------------------------------------------
                0.00    0.00      12/12          GetWords [84]
[83]     0.0    0.00    0.00      12         GetPos [83]
-----------------------------------------------
                0.00    0.00      12/12          AddOptionStringArray [82]
[84]     0.0    0.00    0.00      12         GetWords [84]
                0.00    0.00      12/12          GetPos [83]
-----------------------------------------------
                0.00    0.00       4/12          vxi [222]
                0.00    0.00       4/12          vyi [223]
                0.00    0.00       4/12          OrbElems2Helio [180]
[85]     0.0    0.00    0.00      12         xinit [85]
-----------------------------------------------
                0.00    0.00       4/12          vxi [222]
                0.00    0.00       4/12          vyi [223]
                0.00    0.00       4/12          OrbElems2Helio [180]
[86]     0.0    0.00    0.00      12         yinit [86]
-----------------------------------------------
                0.00    0.00       1/11          ReadBodyFileNames [279]
                0.00    0.00       5/11          ReadModules [138]
                0.00    0.00       5/11          ReadOutputOrder [183]
[87]     0.0    0.00    0.00      11         UpdateFoundOptionMulti [87]
-----------------------------------------------
                0.00    0.00      10/10          LogBody [225]
[88]     0.0    0.00    0.00      10         LogBodyRelations [88]
-----------------------------------------------
                0.00    0.00      10/10          LogBody [225]
[89]     0.0    0.00    0.00      10         LogBodySpiNBody [89]
                0.00    0.00      60/118         WriteLogEntry [69]
-----------------------------------------------
                0.00    0.00      10/10          LogBody [225]
[90]     0.0    0.00    0.00      10         LogGridOutput [90]
-----------------------------------------------
                0.00    0.00      10/10          LogBody [225]
[91]     0.0    0.00    0.00      10         LogOutputOrder [91]
                0.00    0.00      10/1605        WriteVelZ [63]
                0.00    0.00      10/1605        WriteVelY [62]
                0.00    0.00      10/1605        WriteVelX [61]
                0.00    0.00      10/1605        WritePositionZ [60]
                0.00    0.00      10/1605        WritePositionY [59]
                0.00    0.00      10/1605        WritePositionX [58]
                0.00    0.00      10/1597        WriteDeltaTime [64]
                0.00    0.00      10/1597        WriteTime [65]
-----------------------------------------------
                0.00    0.00      10/10          LogBody [225]
[92]     0.0    0.00    0.00      10         PrintModuleList [92]
-----------------------------------------------
                0.00    0.00      10/10          WriteLogEntry [69]
[93]     0.0    0.00    0.00      10         WriteBodyType [93]
-----------------------------------------------
                0.00    0.00      10/10          WriteLogEntry [69]
[94]     0.0    0.00    0.00      10         WriteHZLimitDryRunaway [94]
                0.00    0.00      10/45          fdUnitsLength [76]
                0.00    0.00      10/32          fsUnitsLength [78]
-----------------------------------------------
                0.00    0.00      10/10          WriteLogEntry [69]
[95]     0.0    0.00    0.00      10         WriteMass [95]
                0.00    0.00      10/20          fdUnitsMass [79]
                0.00    0.00      10/20          fsUnitsMass [80]
-----------------------------------------------
                0.00    0.00      10/10          WriteLogEntry [69]
[96]     0.0    0.00    0.00      10         WriteRadius [96]
                0.00    0.00      10/45          fdUnitsLength [76]
                0.00    0.00      10/32          fsUnitsLength [78]
-----------------------------------------------
                0.00    0.00       1/9           ReadSystemName [153]
                0.00    0.00       1/9           ReadEta [119]
                0.00    0.00       1/9           ReadOutputTime [142]
                0.00    0.00       1/9           ReadStopTime [152]
                0.00    0.00       1/9           ReadVarDt [160]
                0.00    0.00       1/9           ReadDigits [113]
                0.00    0.00       1/9           ReadDoForward [115]
                0.00    0.00       1/9           ReadDoLog [116]
                0.00    0.00       1/9           ReadOverwrite [143]
[97]     0.0    0.00    0.00       9         CheckDuplication [97]
-----------------------------------------------
                0.00    0.00       2/8           fsUnitsEnergyFlux [241]
                0.00    0.00       2/8           WriteTotEnergy [233]
                0.00    0.00       2/8           WritePotEnergy [231]
                0.00    0.00       2/8           WriteKinEnergy [228]
[98]     0.0    0.00    0.00       8         fsUnitsEnergy [98]
                0.00    0.00       8/20          fsUnitsMass [80]
                0.00    0.00       8/32          fsUnitsLength [78]
                0.00    0.00       8/3208        fsUnitsTime [57]
-----------------------------------------------
                0.00    0.00       8/8           OrbElems2Helio [180]
[99]     0.0    0.00    0.00       8         xangle1 [99]
-----------------------------------------------
                0.00    0.00       8/8           OrbElems2Helio [180]
[100]    0.0    0.00    0.00       8         xangle2 [100]
-----------------------------------------------
                0.00    0.00       8/8           OrbElems2Helio [180]
[101]    0.0    0.00    0.00       8         yangle1 [101]
-----------------------------------------------
                0.00    0.00       8/8           OrbElems2Helio [180]
[102]    0.0    0.00    0.00       8         yangle2 [102]
-----------------------------------------------
                0.00    0.00       8/8           OrbElems2Helio [180]
[103]    0.0    0.00    0.00       8         zangle1 [103]
-----------------------------------------------
                0.00    0.00       8/8           OrbElems2Helio [180]
[104]    0.0    0.00    0.00       8         zangle2 [104]
-----------------------------------------------
                0.00    0.00       6/6           ReadInitialOptions [40]
[105]    0.0    0.00    0.00       6         InitializeInput [105]
                0.00    0.00     139/9546        CheckComment [55]
                0.00    0.00       6/6           iGetNumLines [167]
-----------------------------------------------
                0.00    0.00       6/6           ReadOptionsGeneral [280]
[106]    0.0    0.00    0.00       6         ReadAge [106]
                0.00    0.00       6/245         AddOptionDouble [67]
                0.00    0.00       5/61          AssignDefaultDouble [74]
-----------------------------------------------
                0.00    0.00       6/6           ReadOptionsGeneral [280]
[107]    0.0    0.00    0.00       6         ReadAlbedoGlobal [107]
                0.00    0.00       6/245         AddOptionDouble [67]
                0.00    0.00       5/61          AssignDefaultDouble [74]
-----------------------------------------------
                0.00    0.00       6/6           ReadOptionsGeneral [280]
[108]    0.0    0.00    0.00       6         ReadArgP [108]
                0.00    0.00       6/245         AddOptionDouble [67]
-----------------------------------------------
                0.00    0.00       6/6           ReadOptionsGeneral [280]
[109]    0.0    0.00    0.00       6         ReadBodyName [109]
                0.00    0.00       6/72          AddOptionString [73]
                0.00    0.00       5/75          NotPrimaryInput [72]
                0.00    0.00       5/79          UpdateFoundOption [70]
-----------------------------------------------
                0.00    0.00       6/6           ReadOptionsGeneral [280]
[110]    0.0    0.00    0.00       6         ReadBodyType [110]
                0.00    0.00       6/77          AddOptionInt [71]
                0.00    0.00       5/36          AssignDefaultInt [77]
-----------------------------------------------
                0.00    0.00       6/6           ReadOptionsGeneral [280]
[111]    0.0    0.00    0.00       6         ReadCalcDynEllip [111]
                0.00    0.00       6/53          AddOptionBool [75]
-----------------------------------------------
                0.00    0.00       6/6           ReadOptionsGeneral [280]
[112]    0.0    0.00    0.00       6         ReadColor [112]
                0.00    0.00       6/72          AddOptionString [73]
-----------------------------------------------
                0.00    0.00       6/6           ReadOptionsGeneral [280]
[113]    0.0    0.00    0.00       6         ReadDigits [113]
                0.00    0.00       6/77          AddOptionInt [71]
                0.00    0.00       5/36          AssignDefaultInt [77]
                0.00    0.00       1/9           CheckDuplication [97]
                0.00    0.00       1/79          UpdateFoundOption [70]
-----------------------------------------------
                0.00    0.00       6/6           ReadOptionsGeneral [280]
[114]    0.0    0.00    0.00       6         ReadDoBackward [114]
                0.00    0.00       6/53          AddOptionBool [75]
-----------------------------------------------
                0.00    0.00       6/6           ReadOptionsGeneral [280]
[115]    0.0    0.00    0.00       6         ReadDoForward [115]
                0.00    0.00       6/53          AddOptionBool [75]
                0.00    0.00       5/36          AssignDefaultInt [77]
                0.00    0.00       1/9           CheckDuplication [97]
                0.00    0.00       1/79          UpdateFoundOption [70]
-----------------------------------------------
                0.00    0.00       6/6           ReadOptionsGeneral [280]
[116]    0.0    0.00    0.00       6         ReadDoLog [116]
                0.00    0.00       6/53          AddOptionBool [75]
                0.00    0.00       5/36          AssignDefaultInt [77]
                0.00    0.00       1/9           CheckDuplication [97]
                0.00    0.00       1/79          UpdateFoundOption [70]
-----------------------------------------------
                0.00    0.00       6/6           ReadOptionsGeneral [280]
[117]    0.0    0.00    0.00       6         ReadDynEllip [117]
                0.00    0.00       6/245         AddOptionDouble [67]
-----------------------------------------------
                0.00    0.00       6/6           ReadOptionsGeneral [280]
[118]    0.0    0.00    0.00       6         ReadEcc [118]
                0.00    0.00       6/245         AddOptionDouble [67]
                0.00    0.00       5/75          NotPrimaryInput [72]
                0.00    0.00       5/79          UpdateFoundOption [70]
-----------------------------------------------
                0.00    0.00       6/6           ReadOptionsGeneral [280]
[119]    0.0    0.00    0.00       6         ReadEta [119]
                0.00    0.00       6/245         AddOptionDouble [67]
                0.00    0.00       5/61          AssignDefaultDouble [74]
                0.00    0.00       1/9           CheckDuplication [97]
                0.00    0.00       1/79          UpdateFoundOption [70]
-----------------------------------------------
                0.00    0.00       6/6           ReadOptionsGeneral [280]
[120]    0.0    0.00    0.00       6         ReadHaltMaxEcc [120]
                0.00    0.00       6/245         AddOptionDouble [67]
-----------------------------------------------
                0.00    0.00       6/6           ReadOptionsGeneral [280]
[121]    0.0    0.00    0.00       6         ReadHaltMerge [121]
                0.00    0.00       6/53          AddOptionBool [75]
-----------------------------------------------
                0.00    0.00       6/6           ReadOptionsGeneral [280]
[122]    0.0    0.00    0.00       6         ReadHaltMinEcc [122]
                0.00    0.00       6/245         AddOptionDouble [67]
                0.00    0.00       5/61          AssignDefaultDouble [74]
-----------------------------------------------
                0.00    0.00       6/6           ReadOptionsGeneral [280]
[123]    0.0    0.00    0.00       6         ReadHaltMinObl [123]
                0.00    0.00       6/245         AddOptionDouble [67]
-----------------------------------------------
                0.00    0.00       6/6           ReadOptionsGeneral [280]
[124]    0.0    0.00    0.00       6         ReadHaltMinSemi [124]
                0.00    0.00       6/245         AddOptionDouble [67]
                0.00    0.00       5/61          AssignDefaultDouble [74]
-----------------------------------------------
                0.00    0.00       6/6           ReadOptionsGeneral [280]
[125]    0.0    0.00    0.00       6         ReadHaltPosDeDt [125]
                0.00    0.00       6/53          AddOptionBool [75]
                0.00    0.00       5/36          AssignDefaultInt [77]
-----------------------------------------------
                0.00    0.00       6/6           ReadOptionsGeneral [280]
[126]    0.0    0.00    0.00       6         ReadHecc [126]
                0.00    0.00       6/245         AddOptionDouble [67]
-----------------------------------------------
                0.00    0.00       6/6           ReadOptionsGeneral [280]
[127]    0.0    0.00    0.00       6         ReadInc [127]
                0.00    0.00       6/245         AddOptionDouble [67]
                0.00    0.00       5/75          NotPrimaryInput [72]
                0.00    0.00       5/79          UpdateFoundOption [70]
-----------------------------------------------
                0.00    0.00       6/6           ReadOptionsGeneral [280]
[128]    0.0    0.00    0.00       6         ReadIntegrationMethod [128]
                0.00    0.00       6/72          AddOptionString [73]
-----------------------------------------------
                0.00    0.00       6/6           ReadOptionsGeneral [280]
[129]    0.0    0.00    0.00       6         ReadKecc [129]
                0.00    0.00       6/245         AddOptionDouble [67]
-----------------------------------------------
                0.00    0.00       6/6           ReadOptionsGeneral [280]
[130]    0.0    0.00    0.00       6         ReadLXUV [130]
                0.00    0.00       6/245         AddOptionDouble [67]
-----------------------------------------------
                0.00    0.00       6/6           ReadOptionsGeneral [280]
[131]    0.0    0.00    0.00       6         ReadLogFile [131]
                0.00    0.00       6/72          AddOptionString [73]
-----------------------------------------------
                0.00    0.00       6/6           ReadOptionsGeneral [280]
[132]    0.0    0.00    0.00       6         ReadLongA [132]
                0.00    0.00       6/245         AddOptionDouble [67]
                0.00    0.00       5/75          NotPrimaryInput [72]
                0.00    0.00       5/79          UpdateFoundOption [70]
-----------------------------------------------
                0.00    0.00       6/6           ReadOptionsGeneral [280]
[133]    0.0    0.00    0.00       6         ReadLongP [133]
                0.00    0.00       6/245         AddOptionDouble [67]
                0.00    0.00       5/75          NotPrimaryInput [72]
                0.00    0.00       5/79          UpdateFoundOption [70]
-----------------------------------------------
                0.00    0.00       6/6           ReadOptionsGeneral [280]
[134]    0.0    0.00    0.00       6         ReadMass [134]
                0.00    0.00       6/245         AddOptionDouble [67]
                0.00    0.00       5/75          NotPrimaryInput [72]
                0.00    0.00       5/19          dNegativeDouble [81]
                0.00    0.00       5/79          UpdateFoundOption [70]
-----------------------------------------------
                0.00    0.00       6/6           ReadOptionsGeneral [280]
[135]    0.0    0.00    0.00       6         ReadMassRad [135]
                0.00    0.00       6/72          AddOptionString [73]
-----------------------------------------------
                0.00    0.00       6/6           ReadOptionsGeneral [280]
[136]    0.0    0.00    0.00       6         ReadMeanMotion [136]
                0.00    0.00       6/245         AddOptionDouble [67]
                0.00    0.00       5/61          AssignDefaultDouble [74]
-----------------------------------------------
                0.00    0.00       6/6           ReadOptionsGeneral [280]
[137]    0.0    0.00    0.00       6         ReadMinValue [137]
                0.00    0.00       6/245         AddOptionDouble [67]
                0.00    0.00       5/61          AssignDefaultDouble [74]
                0.00    0.00       1/79          UpdateFoundOption [70]
-----------------------------------------------
                0.00    0.00       6/6           ReadInitialOptions [40]
[138]    0.0    0.00    0.00       6         ReadModules [138]
                0.00    0.00      60/124574      sLower [53]
                0.00    0.00       6/12          AddOptionStringArray [82]
                0.00    0.00       5/75          NotPrimaryInput [72]
                0.00    0.00       5/11          UpdateFoundOptionMulti [87]
-----------------------------------------------
                0.00    0.00       6/6           ReadOptionsGeneral [280]
[139]    0.0    0.00    0.00       6         ReadObliquity [139]
                0.00    0.00       6/245         AddOptionDouble [67]
                0.00    0.00       5/75          NotPrimaryInput [72]
                0.00    0.00       5/79          UpdateFoundOption [70]
-----------------------------------------------
                0.00    0.00       6/6           ReadOptionsGeneral [280]
[140]    0.0    0.00    0.00       6         ReadOrbPeriod [140]
                0.00    0.00       6/245         AddOptionDouble [67]
                0.00    0.00       5/61          AssignDefaultDouble [74]
-----------------------------------------------
                0.00    0.00       6/6           ReadOptionsGeneral [280]
[141]    0.0    0.00    0.00       6         ReadOutFile [141]
                0.00    0.00       6/72          AddOptionString [73]
                0.00    0.00       5/5           AssignDefaultString [168]
-----------------------------------------------
                0.00    0.00       6/6           ReadOptionsGeneral [280]
[142]    0.0    0.00    0.00       6         ReadOutputTime [142]
                0.00    0.00       6/245         AddOptionDouble [67]
                0.00    0.00       5/61          AssignDefaultDouble [74]
                0.00    0.00       1/9           CheckDuplication [97]
                0.00    0.00       1/3218        fdUnitsTime [56]
                0.00    0.00       1/79          UpdateFoundOption [70]
-----------------------------------------------
                0.00    0.00       6/6           ReadOptionsGeneral [280]
[143]    0.0    0.00    0.00       6         ReadOverwrite [143]
                0.00    0.00       6/53          AddOptionBool [75]
                0.00    0.00       1/9           CheckDuplication [97]
                0.00    0.00       1/79          UpdateFoundOption [70]
-----------------------------------------------
                0.00    0.00       6/6           ReadOptionsGeneral [280]
[144]    0.0    0.00    0.00       6         ReadPrecA [144]
                0.00    0.00       6/245         AddOptionDouble [67]
-----------------------------------------------
                0.00    0.00       6/6           ReadOptionsGeneral [280]
[145]    0.0    0.00    0.00       6         ReadRadius [145]
                0.00    0.00       6/245         AddOptionDouble [67]
                0.00    0.00       5/75          NotPrimaryInput [72]
                0.00    0.00       5/19          dNegativeDouble [81]
                0.00    0.00       5/79          UpdateFoundOption [70]
-----------------------------------------------
                0.00    0.00       6/6           ReadOptionsGeneral [280]
[146]    0.0    0.00    0.00       6         ReadRadiusGyration [146]
                0.00    0.00       6/245         AddOptionDouble [67]
                0.00    0.00       5/75          NotPrimaryInput [72]
                0.00    0.00       5/79          UpdateFoundOption [70]
-----------------------------------------------
                0.00    0.00       6/6           ReadOptionsGeneral [280]
[147]    0.0    0.00    0.00       6         ReadRotPeriod [147]
                0.00    0.00       6/245         AddOptionDouble [67]
                0.00    0.00       5/75          NotPrimaryInput [72]
                0.00    0.00       5/19          dNegativeDouble [81]
                0.00    0.00       5/79          UpdateFoundOption [70]
-----------------------------------------------
                0.00    0.00       6/6           ReadOptionsGeneral [280]
[148]    0.0    0.00    0.00       6         ReadRotRate [148]
                0.00    0.00       6/245         AddOptionDouble [67]
-----------------------------------------------
                0.00    0.00       6/6           ReadOptionsGeneral [280]
[149]    0.0    0.00    0.00       6         ReadRotVel [149]
                0.00    0.00       6/245         AddOptionDouble [67]
-----------------------------------------------
                0.00    0.00       6/6           ReadOptionsGeneral [280]
[150]    0.0    0.00    0.00       6         ReadSciNot [150]
                0.00    0.00       6/77          AddOptionInt [71]
                0.00    0.00       6/36          AssignDefaultInt [77]
-----------------------------------------------
                0.00    0.00       6/6           ReadOptionsGeneral [280]
[151]    0.0    0.00    0.00       6         ReadSemiMajorAxis [151]
                0.00    0.00       6/245         AddOptionDouble [67]
                0.00    0.00       5/75          NotPrimaryInput [72]
                0.00    0.00       5/79          UpdateFoundOption [70]
                0.00    0.00       4/19          dNegativeDouble [81]
                0.00    0.00       1/45          fdUnitsLength [76]
-----------------------------------------------
                0.00    0.00       6/6           ReadOptionsGeneral [280]
[152]    0.0    0.00    0.00       6         ReadStopTime [152]
                0.00    0.00       6/245         AddOptionDouble [67]
                0.00    0.00       5/61          AssignDefaultDouble [74]
                0.00    0.00       1/9           CheckDuplication [97]
                0.00    0.00       1/3218        fdUnitsTime [56]
                0.00    0.00       1/79          UpdateFoundOption [70]
-----------------------------------------------
                0.00    0.00       6/6           ReadInitialOptions [40]
[153]    0.0    0.00    0.00       6         ReadSystemName [153]
                0.00    0.00       6/72          AddOptionString [73]
                0.00    0.00       1/9           CheckDuplication [97]
                0.00    0.00       1/79          UpdateFoundOption [70]
-----------------------------------------------
                0.00    0.00       6/6           ReadOptionsGeneral [280]
[154]    0.0    0.00    0.00       6         ReadTimeStep [154]
                0.00    0.00       6/245         AddOptionDouble [67]
                0.00    0.00       6/61          AssignDefaultDouble [74]
-----------------------------------------------
                0.00    0.00       6/6           ReadInitialOptions [40]
[155]    0.0    0.00    0.00       6         ReadUnitAngle [155]
                0.00    0.00       6/72          AddOptionString [73]
                0.00    0.00       1/1           iAssignUnitAngle [287]
                0.00    0.00       1/79          UpdateFoundOption [70]
-----------------------------------------------
                0.00    0.00       6/6           ReadInitialOptions [40]
[156]    0.0    0.00    0.00       6         ReadUnitLength [156]
                0.00    0.00       6/72          AddOptionString [73]
                0.00    0.00       1/1           iAssignUnitLength [288]
                0.00    0.00       1/79          UpdateFoundOption [70]
-----------------------------------------------
                0.00    0.00       6/6           ReadInitialOptions [40]
[157]    0.0    0.00    0.00       6         ReadUnitMass [157]
                0.00    0.00       6/72          AddOptionString [73]
                0.00    0.00       1/1           iAssignMassUnit [286]
                0.00    0.00       1/79          UpdateFoundOption [70]
-----------------------------------------------
                0.00    0.00       6/6           ReadInitialOptions [40]
[158]    0.0    0.00    0.00       6         ReadUnitTemp [158]
                0.00    0.00       6/72          AddOptionString [73]
                0.00    0.00       5/5           iAssignTempUnit [217]
-----------------------------------------------
                0.00    0.00       6/6           ReadInitialOptions [40]
[159]    0.0    0.00    0.00       6         ReadUnitTime [159]
                0.00    0.00       6/72          AddOptionString [73]
                0.00    0.00       1/1           iAssignUnitTime [289]
                0.00    0.00       1/79          UpdateFoundOption [70]
-----------------------------------------------
                0.00    0.00       6/6           ReadOptionsGeneral [280]
[160]    0.0    0.00    0.00       6         ReadVarDt [160]
                0.00    0.00       6/53          AddOptionBool [75]
                0.00    0.00       5/36          AssignDefaultInt [77]
                0.00    0.00       1/9           CheckDuplication [97]
                0.00    0.00       1/79          UpdateFoundOption [70]
-----------------------------------------------
                0.00    0.00       6/6           ReadInitialOptions [40]
[161]    0.0    0.00    0.00       6         ReadVerbose [161]
                0.00    0.00       6/77          AddOptionInt [71]
-----------------------------------------------
                0.00    0.00       6/6           ReadOptionsGeneral [280]
[162]    0.0    0.00    0.00       6         ReadViscUMan [162]
                0.00    0.00       6/245         AddOptionDouble [67]
                0.00    0.00       5/61          AssignDefaultDouble [74]
-----------------------------------------------
                0.00    0.00       6/6           ReadOptionsGeneral [280]
[163]    0.0    0.00    0.00       6         ReadXobl [163]
                0.00    0.00       6/245         AddOptionDouble [67]
-----------------------------------------------
                0.00    0.00       6/6           ReadOptionsGeneral [280]
[164]    0.0    0.00    0.00       6         ReadYobl [164]
                0.00    0.00       6/245         AddOptionDouble [67]
-----------------------------------------------
                0.00    0.00       6/6           ReadOptionsGeneral [280]
[165]    0.0    0.00    0.00       6         ReadZobl [165]
                0.00    0.00       6/245         AddOptionDouble [67]
-----------------------------------------------
                0.00    0.00       2/6           WriteTotEnergy [233]
                0.00    0.00       2/6           WritePotEnergy [231]
                0.00    0.00       2/6           WriteKinEnergy [228]
[166]    0.0    0.00    0.00       6         fdUnitsEnergy [166]
                0.00    0.00      12/45          fdUnitsLength [76]
                0.00    0.00      12/3218        fdUnitsTime [56]
                0.00    0.00       6/20          fdUnitsMass [79]
-----------------------------------------------
                0.00    0.00       6/6           InitializeInput [105]
[167]    0.0    0.00    0.00       6         iGetNumLines [167]
-----------------------------------------------
                0.00    0.00       5/5           ReadOutFile [141]
[168]    0.0    0.00    0.00       5         AssignDefaultString [168]
-----------------------------------------------
                0.00    0.00       5/5           fiNumHalts [216]
[169]    0.0    0.00    0.00       5         CountHaltsSpiNBody [169]
-----------------------------------------------
                0.00    0.00       5/5           InitializeUpdate [35]
[170]    0.0    0.00    0.00       5         FinalizeUpdatePositionXSpiNBody [170]
-----------------------------------------------
                0.00    0.00       5/5           InitializeUpdate [35]
[171]    0.0    0.00    0.00       5         FinalizeUpdatePositionYSpiNBody [171]
-----------------------------------------------
                0.00    0.00       5/5           InitializeUpdate [35]
[172]    0.0    0.00    0.00       5         FinalizeUpdatePositionZSpiNBody [172]
-----------------------------------------------
                0.00    0.00       5/5           InitializeUpdate [35]
[173]    0.0    0.00    0.00       5         FinalizeUpdateVelYSpiNBody [173]
-----------------------------------------------
                0.00    0.00       5/5           InitializeUpdate [35]
[174]    0.0    0.00    0.00       5         FinalizeUpdateVelZSpiNBody [174]
-----------------------------------------------
                0.00    0.00       5/5           InitializeBodySpiNBody [176]
[175]    0.0    0.00    0.00       5         Helio2Bary [175]
-----------------------------------------------
                0.00    0.00       5/5           VerifyOptions [31]
[176]    0.0    0.00    0.00       5         InitializeBodySpiNBody [176]
                0.00    0.00       5/5           OrbElems2Helio [180]
                0.00    0.00       5/5           Helio2Bary [175]
-----------------------------------------------
                0.00    0.00       5/5           InitializeControl [243]
[177]    0.0    0.00    0.00       5         InitializeControlNULL [177]
-----------------------------------------------
                0.00    0.00       5/5           VerifyOptions [31]
[178]    0.0    0.00    0.00       5         InitializeUpdateBodyPerts [178]
-----------------------------------------------
                0.00    0.00       5/5           InitializeUpdate [35]
[179]    0.0    0.00    0.00       5         InitializeUpdateSpiNBody [179]
-----------------------------------------------
                0.00    0.00       5/5           InitializeBodySpiNBody [176]
[180]    0.0    0.00    0.00       5         OrbElems2Helio [180]
                0.00    0.00       8/8           xangle1 [99]
                0.00    0.00       8/8           xangle2 [100]
                0.00    0.00       8/8           yangle1 [101]
                0.00    0.00       8/8           yangle2 [102]
                0.00    0.00       8/8           zangle1 [103]
                0.00    0.00       8/8           zangle2 [104]
                0.00    0.00       4/4           kepler_eqn [220]
                0.00    0.00       4/12          xinit [85]
                0.00    0.00       4/12          yinit [86]
                0.00    0.00       4/4           vxi [222]
                0.00    0.00       4/4           vyi [223]
-----------------------------------------------
                0.00    0.00       5/5           ReadOptionsSpiNBody [182]
[181]    0.0    0.00    0.00       5         ReadMeanA [181]
                0.00    0.00       5/245         AddOptionDouble [67]
                0.00    0.00       5/75          NotPrimaryInput [72]
                0.00    0.00       5/79          UpdateFoundOption [70]
-----------------------------------------------
                0.00    0.00       5/5           ReadOptionsModules [281]
[182]    0.0    0.00    0.00       5         ReadOptionsSpiNBody [182]
                0.00    0.00       5/5           ReadUseOrbParams [187]
                0.00    0.00       5/5           ReadMeanA [181]
                0.00    0.00       5/5           ReadVelZ [190]
                0.00    0.00       5/5           ReadVelY [189]
                0.00    0.00       5/5           ReadVelX [188]
                0.00    0.00       5/5           ReadPositionZ [186]
                0.00    0.00       5/5           ReadPositionY [185]
                0.00    0.00       5/5           ReadPositionX [184]
-----------------------------------------------
                0.00    0.00       5/5           ReadOptions [41]
[183]    0.0    0.00    0.00       5         ReadOutputOrder [183]
                0.00    0.00  124500/124574      sLower [53]
                0.00    0.00       5/12          AddOptionStringArray [82]
                0.00    0.00       5/75          NotPrimaryInput [72]
                0.00    0.00       5/11          UpdateFoundOptionMulti [87]
-----------------------------------------------
                0.00    0.00       5/5           ReadOptionsSpiNBody [182]
[184]    0.0    0.00    0.00       5         ReadPositionX [184]
                0.00    0.00       5/245         AddOptionDouble [67]
-----------------------------------------------
                0.00    0.00       5/5           ReadOptionsSpiNBody [182]
[185]    0.0    0.00    0.00       5         ReadPositionY [185]
                0.00    0.00       5/245         AddOptionDouble [67]
-----------------------------------------------
                0.00    0.00       5/5           ReadOptionsSpiNBody [182]
[186]    0.0    0.00    0.00       5         ReadPositionZ [186]
                0.00    0.00       5/245         AddOptionDouble [67]
-----------------------------------------------
                0.00    0.00       5/5           ReadOptionsSpiNBody [182]
[187]    0.0    0.00    0.00       5         ReadUseOrbParams [187]
                0.00    0.00       5/53          AddOptionBool [75]
                0.00    0.00       5/75          NotPrimaryInput [72]
                0.00    0.00       5/79          UpdateFoundOption [70]
-----------------------------------------------
                0.00    0.00       5/5           ReadOptionsSpiNBody [182]
[188]    0.0    0.00    0.00       5         ReadVelX [188]
                0.00    0.00       5/245         AddOptionDouble [67]
-----------------------------------------------
                0.00    0.00       5/5           ReadOptionsSpiNBody [182]
[189]    0.0    0.00    0.00       5         ReadVelY [189]
                0.00    0.00       5/245         AddOptionDouble [67]
-----------------------------------------------
                0.00    0.00       5/5           ReadOptionsSpiNBody [182]
[190]    0.0    0.00    0.00       5         ReadVelZ [190]
                0.00    0.00       5/245         AddOptionDouble [67]
-----------------------------------------------
                0.00    0.00       5/5           VerifyHalts [284]
[191]    0.0    0.00    0.00       5         VerifyHaltSpiNBody [191]
-----------------------------------------------
                0.00    0.00       5/5           VerifyOptions [31]
[192]    0.0    0.00    0.00       5         VerifyInterior [192]
-----------------------------------------------
                0.00    0.00       5/5           VerifyOptions [31]
[193]    0.0    0.00    0.00       5         VerifyMassRad [193]
-----------------------------------------------
                0.00    0.00       5/5           VerifyOptions [31]
[194]    0.0    0.00    0.00       5         VerifyModuleMulti [194]
                0.00    0.00       5/5           VerifyModuleMultiDistOrbDistRot [198]
                0.00    0.00       5/5           VerifyModuleMultiRadheatThermint [204]
                0.00    0.00       5/5           VerifyModuleMultiEqtideDistOrb [199]
                0.00    0.00       5/5           VerifyModuleMultiAtmescEqtide [195]
                0.00    0.00       5/5           VerifyModuleMultiEqtideThermint [202]
                0.00    0.00       5/5           VerifyModuleMultiAtmescEqtideThermint [196]
                0.00    0.00       5/5           VerifyModuleMultiFlareStellar [203]
                0.00    0.00       5/5           VerifyModuleMultiBinaryEqtide [197]
                0.00    0.00       5/5           VerifyModuleMultiEqtideDistorb [200]
                0.00    0.00       5/5           VerifyModuleMultiEqtideStellar [201]
-----------------------------------------------
                0.00    0.00       5/5           VerifyModuleMulti [194]
[195]    0.0    0.00    0.00       5         VerifyModuleMultiAtmescEqtide [195]
-----------------------------------------------
                0.00    0.00       5/5           VerifyModuleMulti [194]
[196]    0.0    0.00    0.00       5         VerifyModuleMultiAtmescEqtideThermint [196]
-----------------------------------------------
                0.00    0.00       5/5           VerifyModuleMulti [194]
[197]    0.0    0.00    0.00       5         VerifyModuleMultiBinaryEqtide [197]
-----------------------------------------------
                0.00    0.00       5/5           VerifyModuleMulti [194]
[198]    0.0    0.00    0.00       5         VerifyModuleMultiDistOrbDistRot [198]
-----------------------------------------------
                0.00    0.00       5/5           VerifyModuleMulti [194]
[199]    0.0    0.00    0.00       5         VerifyModuleMultiEqtideDistOrb [199]
-----------------------------------------------
                0.00    0.00       5/5           VerifyModuleMulti [194]
[200]    0.0    0.00    0.00       5         VerifyModuleMultiEqtideDistorb [200]
-----------------------------------------------
                0.00    0.00       5/5           VerifyModuleMulti [194]
[201]    0.0    0.00    0.00       5         VerifyModuleMultiEqtideStellar [201]
-----------------------------------------------
                0.00    0.00       5/5           VerifyModuleMulti [194]
[202]    0.0    0.00    0.00       5         VerifyModuleMultiEqtideThermint [202]
-----------------------------------------------
                0.00    0.00       5/5           VerifyModuleMulti [194]
[203]    0.0    0.00    0.00       5         VerifyModuleMultiFlareStellar [203]
-----------------------------------------------
                0.00    0.00       5/5           VerifyModuleMulti [194]
[204]    0.0    0.00    0.00       5         VerifyModuleMultiRadheatThermint [204]
-----------------------------------------------
                0.00    0.00       5/5           VerifySpiNBody [210]
[205]    0.0    0.00    0.00       5         VerifyPerturbersSpiNBody [205]
-----------------------------------------------
                0.00    0.00       5/5           VerifySpiNBody [210]
[206]    0.0    0.00    0.00       5         VerifyPositionX [206]
-----------------------------------------------
                0.00    0.00       5/5           VerifySpiNBody [210]
[207]    0.0    0.00    0.00       5         VerifyPositionY [207]
-----------------------------------------------
                0.00    0.00       5/5           VerifySpiNBody [210]
[208]    0.0    0.00    0.00       5         VerifyPositionZ [208]
-----------------------------------------------
                0.00    0.00       5/5           VerifyOptions [31]
[209]    0.0    0.00    0.00       5         VerifyRotationGeneral [209]
                0.00    0.00       5/5           fdPerToFreq [215]
-----------------------------------------------
                0.00    0.00       5/5           VerifyOptions [31]
[210]    0.0    0.00    0.00       5         VerifySpiNBody [210]
                0.00    0.00       5/5           VerifyPerturbersSpiNBody [205]
                0.00    0.00       5/5           VerifyPositionX [206]
                0.00    0.00       5/5           VerifyPositionY [207]
                0.00    0.00       5/5           VerifyPositionZ [208]
                0.00    0.00       5/5           VerifyVelX [211]
                0.00    0.00       5/5           VerifyVelY [212]
                0.00    0.00       5/5           VerifyVelZ [213]
-----------------------------------------------
                0.00    0.00       5/5           VerifySpiNBody [210]
[211]    0.0    0.00    0.00       5         VerifyVelX [211]
-----------------------------------------------
                0.00    0.00       5/5           VerifySpiNBody [210]
[212]    0.0    0.00    0.00       5         VerifyVelY [212]
-----------------------------------------------
                0.00    0.00       5/5           VerifySpiNBody [210]
[213]    0.0    0.00    0.00       5         VerifyVelZ [213]
-----------------------------------------------
                0.00    0.00       5/5           VerifyIntegration [285]
[214]    0.0    0.00    0.00       5         bFileExists [214]
-----------------------------------------------
                0.00    0.00       5/5           VerifyRotationGeneral [209]
[215]    0.0    0.00    0.00       5         fdPerToFreq [215]
-----------------------------------------------
                0.00    0.00       5/5           VerifyHalts [284]
[216]    0.0    0.00    0.00       5         fiNumHalts [216]
                0.00    0.00       5/5           CountHaltsSpiNBody [169]
-----------------------------------------------
                0.00    0.00       5/5           ReadUnitTemp [158]
[217]    0.0    0.00    0.00       5         iAssignTempUnit [217]
                0.00    0.00       5/124574      sLower [53]
-----------------------------------------------
                0.00    0.00       2/4           WriteKinEnergy [228]
                0.00    0.00       2/4           fdTotEnergy [235]
[218]    0.0    0.00    0.00       4         fdKinEnergy [218]
-----------------------------------------------
                0.00    0.00       2/4           WritePotEnergy [231]
                0.00    0.00       2/4           fdTotEnergy [235]
[219]    0.0    0.00    0.00       4         fdPotEnergy [219]
-----------------------------------------------
                0.00    0.00       4/4           OrbElems2Helio [180]
[220]    0.0    0.00    0.00       4         kepler_eqn [220]
                0.00    0.00       4/4           signf [221]
-----------------------------------------------
                0.00    0.00       4/4           kepler_eqn [220]
[221]    0.0    0.00    0.00       4         signf [221]
-----------------------------------------------
                0.00    0.00       4/4           OrbElems2Helio [180]
[222]    0.0    0.00    0.00       4         vxi [222]
                0.00    0.00       4/12          xinit [85]
                0.00    0.00       4/12          yinit [86]
-----------------------------------------------
                0.00    0.00       4/4           OrbElems2Helio [180]
[223]    0.0    0.00    0.00       4         vyi [223]
                0.00    0.00       4/12          xinit [85]
                0.00    0.00       4/12          yinit [86]
-----------------------------------------------
                0.00    0.00       1/2           InitializeControlEvolve [244]
                0.00    0.00       1/2           ReadInitialOptions [40]
[224]    0.0    0.00    0.00       2         InitializeBodyModules [224]
-----------------------------------------------
                0.00    0.00       2/2           WriteLog [52]
[225]    0.0    0.00    0.00       2         LogBody [225]
                0.00    0.00      40/118         WriteLogEntry [69]
                0.00    0.00      10/10          PrintModuleList [92]
                0.00    0.00      10/10          LogBodyRelations [88]
                0.00    0.00      10/10          LogBodySpiNBody [89]
                0.00    0.00      10/10          LogOutputOrder [91]
                0.00    0.00      10/10          LogGridOutput [90]
-----------------------------------------------
                0.00    0.00       2/2           WriteLog [52]
[226]    0.0    0.00    0.00       2         LogSystem [226]
                0.00    0.00      18/118         WriteLogEntry [69]
-----------------------------------------------
                0.00    0.00       2/2           WriteLogEntry [69]
[227]    0.0    0.00    0.00       2         WriteAge [227]
                0.00    0.00       2/3218        fdUnitsTime [56]
                0.00    0.00       2/3208        fsUnitsTime [57]
-----------------------------------------------
                0.00    0.00       2/2           WriteLogEntry [69]
[228]    0.0    0.00    0.00       2         WriteKinEnergy [228]
                0.00    0.00       2/4           fdKinEnergy [218]
                0.00    0.00       2/6           fdUnitsEnergy [166]
                0.00    0.00       2/8           fsUnitsEnergy [98]
-----------------------------------------------
                0.00    0.00       2/2           WriteLogEntry [69]
[229]    0.0    0.00    0.00       2         WriteLXUVTot [229]
                0.00    0.00       2/2           fdUnitsEnergyFlux [237]
                0.00    0.00       2/2           fsUnitsEnergyFlux [241]
-----------------------------------------------
                0.00    0.00       2/2           WriteLogEntry [69]
[230]    0.0    0.00    0.00       2         WriteLongP [230]
                0.00    0.00       2/2           fdUnitsAngle [236]
                0.00    0.00       2/2           fsUnitsAngle [240]
-----------------------------------------------
                0.00    0.00       2/2           WriteLogEntry [69]
[231]    0.0    0.00    0.00       2         WritePotEnergy [231]
                0.00    0.00       2/4           fdPotEnergy [219]
                0.00    0.00       2/6           fdUnitsEnergy [166]
                0.00    0.00       2/8           fsUnitsEnergy [98]
-----------------------------------------------
                0.00    0.00       2/2           WriteLogEntry [69]
[232]    0.0    0.00    0.00       2         WriteTotAngMom [232]
                0.00    0.00       4/45          fdUnitsLength [76]
                0.00    0.00       2/2           fdTotAngMom [234]
                0.00    0.00       2/3218        fdUnitsTime [56]
                0.00    0.00       2/20          fdUnitsMass [79]
                0.00    0.00       2/2           fsUnitsAngMom [239]
-----------------------------------------------
                0.00    0.00       2/2           WriteLogEntry [69]
[233]    0.0    0.00    0.00       2         WriteTotEnergy [233]
                0.00    0.00       2/2           fdTotEnergy [235]
                0.00    0.00       2/6           fdUnitsEnergy [166]
                0.00    0.00       2/8           fsUnitsEnergy [98]
-----------------------------------------------
                0.00    0.00       2/2           WriteTotAngMom [232]
[234]    0.0    0.00    0.00       2         fdTotAngMom [234]
-----------------------------------------------
                0.00    0.00       2/2           WriteTotEnergy [233]
[235]    0.0    0.00    0.00       2         fdTotEnergy [235]
                0.00    0.00       2/4           fdKinEnergy [218]
                0.00    0.00       2/4           fdPotEnergy [219]
-----------------------------------------------
                0.00    0.00       2/2           WriteLongP [230]
[236]    0.0    0.00    0.00       2         fdUnitsAngle [236]
-----------------------------------------------
                0.00    0.00       2/2           WriteLXUVTot [229]
[237]    0.0    0.00    0.00       2         fdUnitsEnergyFlux [237]
                0.00    0.00       4/45          fdUnitsLength [76]
                0.00    0.00       2/2           fdUnitsPower [238]
-----------------------------------------------
                0.00    0.00       2/2           fdUnitsEnergyFlux [237]
[238]    0.0    0.00    0.00       2         fdUnitsPower [238]
                0.00    0.00       6/3218        fdUnitsTime [56]
                0.00    0.00       4/45          fdUnitsLength [76]
                0.00    0.00       2/20          fdUnitsMass [79]
-----------------------------------------------
                0.00    0.00       2/2           WriteTotAngMom [232]
[239]    0.0    0.00    0.00       2         fsUnitsAngMom [239]
                0.00    0.00       2/20          fsUnitsMass [80]
                0.00    0.00       2/32          fsUnitsLength [78]
                0.00    0.00       2/3208        fsUnitsTime [57]
-----------------------------------------------
                0.00    0.00       2/2           WriteLongP [230]
[240]    0.0    0.00    0.00       2         fsUnitsAngle [240]
-----------------------------------------------
                0.00    0.00       2/2           WriteLXUVTot [229]
[241]    0.0    0.00    0.00       2         fsUnitsEnergyFlux [241]
                0.00    0.00       2/8           fsUnitsEnergy [98]
                0.00    0.00       2/32          fsUnitsLength [78]
                0.00    0.00       2/3208        fsUnitsTime [57]
-----------------------------------------------
                0.00    0.00       1/1           ReadBodyFileNames [279]
[242]    0.0    0.00    0.00       1         InfileCopy [242]
-----------------------------------------------
                0.00    0.00       1/1           ReadOptions [41]
[243]    0.0    0.00    0.00       1         InitializeControl [243]
                0.00    0.00       5/5           InitializeControlNULL [177]
-----------------------------------------------
                0.00    0.00       1/1           VerifyOptions [31]
[244]    0.0    0.00    0.00       1         InitializeControlEvolve [244]
                0.00    0.00       1/2           InitializeBodyModules [224]
-----------------------------------------------
                0.00    0.00       1/1           VerifyOptions [31]
[245]    0.0    0.00    0.00       1         InitializeHalts [245]
-----------------------------------------------
                0.00    0.00       1/1           ReadInitialOptions [40]
[246]    0.0    0.00    0.00       1         InitializeModule [246]
-----------------------------------------------
                0.00    0.00       1/1           main [2]
[247]    0.0    0.00    0.00       1         InitializeOptions [247]
                0.00    0.00       1/1           InitializeOptionsGeneral [255]
                0.00    0.00       1/1           InitializeOptionsEqtide [252]
                0.00    0.00       1/1           InitializeOptionsRadheat [257]
                0.00    0.00       1/1           InitializeOptionsDistOrb [250]
                0.00    0.00       1/1           InitializeOptionsDistRot [251]
                0.00    0.00       1/1           InitializeOptionsThermint [260]
                0.00    0.00       1/1           InitializeOptionsAtmEsc [248]
                0.00    0.00       1/1           InitializeOptionsStellar [259]
                0.00    0.00       1/1           InitializeOptionsPoise [256]
                0.00    0.00       1/1           InitializeOptionsBinary [249]
                0.00    0.00       1/1           InitializeOptionsFlare [253]
                0.00    0.00       1/1           InitializeOptionsGalHabit [254]
                0.00    0.00       1/1           InitializeOptionsSpiNBody [258]
-----------------------------------------------
                0.00    0.00       1/1           InitializeOptions [247]
[248]    0.0    0.00    0.00       1         InitializeOptionsAtmEsc [248]
-----------------------------------------------
                0.00    0.00       1/1           InitializeOptions [247]
[249]    0.0    0.00    0.00       1         InitializeOptionsBinary [249]
-----------------------------------------------
                0.00    0.00       1/1           InitializeOptions [247]
[250]    0.0    0.00    0.00       1         InitializeOptionsDistOrb [250]
-----------------------------------------------
                0.00    0.00       1/1           InitializeOptions [247]
[251]    0.0    0.00    0.00       1         InitializeOptionsDistRot [251]
-----------------------------------------------
                0.00    0.00       1/1           InitializeOptions [247]
[252]    0.0    0.00    0.00       1         InitializeOptionsEqtide [252]
-----------------------------------------------
                0.00    0.00       1/1           InitializeOptions [247]
[253]    0.0    0.00    0.00       1         InitializeOptionsFlare [253]
-----------------------------------------------
                0.00    0.00       1/1           InitializeOptions [247]
[254]    0.0    0.00    0.00       1         InitializeOptionsGalHabit [254]
-----------------------------------------------
                0.00    0.00       1/1           InitializeOptions [247]
[255]    0.0    0.00    0.00       1         InitializeOptionsGeneral [255]
-----------------------------------------------
                0.00    0.00       1/1           InitializeOptions [247]
[256]    0.0    0.00    0.00       1         InitializeOptionsPoise [256]
-----------------------------------------------
                0.00    0.00       1/1           InitializeOptions [247]
[257]    0.0    0.00    0.00       1         InitializeOptionsRadheat [257]
-----------------------------------------------
                0.00    0.00       1/1           InitializeOptions [247]
[258]    0.0    0.00    0.00       1         InitializeOptionsSpiNBody [258]
-----------------------------------------------
                0.00    0.00       1/1           InitializeOptions [247]
[259]    0.0    0.00    0.00       1         InitializeOptionsStellar [259]
-----------------------------------------------
                0.00    0.00       1/1           InitializeOptions [247]
[260]    0.0    0.00    0.00       1         InitializeOptionsThermint [260]
-----------------------------------------------
                0.00    0.00       1/1           main [2]
[261]    0.0    0.00    0.00       1         InitializeOutput [261]
                0.00    0.00       1/1           InitializeOutputGeneral [269]
                0.00    0.00       1/1           InitializeOutputEqtide [266]
                0.00    0.00       1/1           InitializeOutputRadheat [271]
                0.00    0.00       1/1           InitializeOutputAtmEsc [262]
                0.00    0.00       1/1           InitializeOutputStellar [273]
                0.00    0.00       1/1           InitializeOutputDistOrb [264]
                0.00    0.00       1/1           InitializeOutputDistRot [265]
                0.00    0.00       1/1           InitializeOutputThermint [274]
                0.00    0.00       1/1           InitializeOutputPoise [270]
                0.00    0.00       1/1           InitializeOutputBinary [263]
                0.00    0.00       1/1           InitializeOutputFlare [267]
                0.00    0.00       1/1           InitializeOutputGalHabit [268]
                0.00    0.00       1/1           InitializeOutputSpiNBody [272]
-----------------------------------------------
                0.00    0.00       1/1           InitializeOutput [261]
[262]    0.0    0.00    0.00       1         InitializeOutputAtmEsc [262]
-----------------------------------------------
                0.00    0.00       1/1           InitializeOutput [261]
[263]    0.0    0.00    0.00       1         InitializeOutputBinary [263]
-----------------------------------------------
                0.00    0.00       1/1           InitializeOutput [261]
[264]    0.0    0.00    0.00       1         InitializeOutputDistOrb [264]
-----------------------------------------------
                0.00    0.00       1/1           InitializeOutput [261]
[265]    0.0    0.00    0.00       1         InitializeOutputDistRot [265]
-----------------------------------------------
                0.00    0.00       1/1           InitializeOutput [261]
[266]    0.0    0.00    0.00       1         InitializeOutputEqtide [266]
-----------------------------------------------
                0.00    0.00       1/1           InitializeOutput [261]
[267]    0.0    0.00    0.00       1         InitializeOutputFlare [267]
-----------------------------------------------
                0.00    0.00       1/1           InitializeOutput [261]
[268]    0.0    0.00    0.00       1         InitializeOutputGalHabit [268]
-----------------------------------------------
                0.00    0.00       1/1           InitializeOutput [261]
[269]    0.0    0.00    0.00       1         InitializeOutputGeneral [269]
-----------------------------------------------
                0.00    0.00       1/1           InitializeOutput [261]
[270]    0.0    0.00    0.00       1         InitializeOutputPoise [270]
-----------------------------------------------
                0.00    0.00       1/1           InitializeOutput [261]
[271]    0.0    0.00    0.00       1         InitializeOutputRadheat [271]
-----------------------------------------------
                0.00    0.00       1/1           InitializeOutput [261]
[272]    0.0    0.00    0.00       1         InitializeOutputSpiNBody [272]
-----------------------------------------------
                0.00    0.00       1/1           InitializeOutput [261]
[273]    0.0    0.00    0.00       1         InitializeOutputStellar [273]
-----------------------------------------------
                0.00    0.00       1/1           InitializeOutput [261]
[274]    0.0    0.00    0.00       1         InitializeOutputThermint [274]
-----------------------------------------------
                0.00    0.00       1/1           LogOptions [277]
[275]    0.0    0.00    0.00       1         LogHalt [275]
                0.00    0.00       1/12803       fprintd [54]
-----------------------------------------------
                0.00    0.00       1/1           LogOptions [277]
[276]    0.0    0.00    0.00       1         LogIntegration [276]
                0.00    0.00       4/12803       fprintd [54]
-----------------------------------------------
                0.00    0.00       1/1           WriteLog [52]
[277]    0.0    0.00    0.00       1         LogOptions [277]
                0.00    0.00       1/1           LogUnits [278]
                0.00    0.00       1/1           LogIntegration [276]
                0.00    0.00       1/1           LogHalt [275]
-----------------------------------------------
                0.00    0.00       1/1           LogOptions [277]
[278]    0.0    0.00    0.00       1         LogUnits [278]
                0.00    0.00       1/119         CGSUnits [68]
-----------------------------------------------
                0.00    0.00       1/1           ReadInitialOptions [40]
[279]    0.0    0.00    0.00       1         ReadBodyFileNames [279]
                0.00    0.00       1/12          AddOptionStringArray [82]
                0.00    0.00       1/1           InfileCopy [242]
                0.00    0.00       1/11          UpdateFoundOptionMulti [87]
-----------------------------------------------
                0.00    0.00       1/1           ReadOptions [41]
[280]    0.0    0.00    0.00       1         ReadOptionsGeneral [280]
                0.00    0.00       6/6           ReadZobl [165]
                0.00    0.00       6/6           ReadYobl [164]
                0.00    0.00       6/6           ReadXobl [163]
                0.00    0.00       6/6           ReadRotVel [149]
                0.00    0.00       6/6           ReadRotRate [148]
                0.00    0.00       6/6           ReadRotPeriod [147]
                0.00    0.00       6/6           ReadRadiusGyration [146]
                0.00    0.00       6/6           ReadRadius [145]
                0.00    0.00       6/6           ReadObliquity [139]
                0.00    0.00       6/6           ReadViscUMan [162]
                0.00    0.00       6/6           ReadCalcDynEllip [111]
                0.00    0.00       6/6           ReadDynEllip [117]
                0.00    0.00       6/6           ReadPrecA [144]
                0.00    0.00       6/6           ReadOverwrite [143]
                0.00    0.00       6/6           ReadSciNot [150]
                0.00    0.00       6/6           ReadDigits [113]
                0.00    0.00       6/6           ReadLongA [132]
                0.00    0.00       6/6           ReadArgP [108]
                0.00    0.00       6/6           ReadInc [127]
                0.00    0.00       6/6           ReadSemiMajorAxis [151]
                0.00    0.00       6/6           ReadOrbPeriod [140]
                0.00    0.00       6/6           ReadBodyType [110]
                0.00    0.00       6/6           ReadMeanMotion [136]
                0.00    0.00       6/6           ReadEcc [118]
                0.00    0.00       6/6           ReadMinValue [137]
                0.00    0.00       6/6           ReadMassRad [135]
                0.00    0.00       6/6           ReadMass [134]
                0.00    0.00       6/6           ReadLXUV [130]
                0.00    0.00       6/6           ReadLongP [133]
                0.00    0.00       6/6           ReadLogFile [131]
                0.00    0.00       6/6           ReadDoLog [116]
                0.00    0.00       6/6           ReadKecc [129]
                0.00    0.00       6/6           ReadIntegrationMethod [128]
                0.00    0.00       6/6           ReadHecc [126]
                0.00    0.00       6/6           ReadHaltPosDeDt [125]
                0.00    0.00       6/6           ReadHaltMinSemi [124]
                0.00    0.00       6/6           ReadHaltMinObl [123]
                0.00    0.00       6/6           ReadHaltMinEcc [122]
                0.00    0.00       6/6           ReadHaltMerge [121]
                0.00    0.00       6/6           ReadHaltMaxEcc [120]
                0.00    0.00       6/6           ReadDoForward [115]
                0.00    0.00       6/6           ReadColor [112]
                0.00    0.00       6/6           ReadBodyName [109]
                0.00    0.00       6/6           ReadVarDt [160]
                0.00    0.00       6/6           ReadTimeStep [154]
                0.00    0.00       6/6           ReadStopTime [152]
                0.00    0.00       6/6           ReadOutputTime [142]
                0.00    0.00       6/6           ReadEta [119]
                0.00    0.00       6/6           ReadOutFile [141]
                0.00    0.00       6/6           ReadDoBackward [114]
                0.00    0.00       6/6           ReadAlbedoGlobal [107]
                0.00    0.00       6/6           ReadAge [106]
-----------------------------------------------
                0.00    0.00       1/1           ReadOptions [41]
[281]    0.0    0.00    0.00       1         ReadOptionsModules [281]
                0.00    0.00       5/5           ReadOptionsSpiNBody [182]
-----------------------------------------------
                0.00    0.00       1/1           ReadOptions [41]
[282]    0.0    0.00    0.00       1         Unrecognized [282]
-----------------------------------------------
                0.00    0.00       1/1           Evolve [4]
[283]    0.0    0.00    0.00       1         UpdateCopy [283]
-----------------------------------------------
                0.00    0.00       1/1           VerifyOptions [31]
[284]    0.0    0.00    0.00       1         VerifyHalts [284]
                0.00    0.00       5/5           fiNumHalts [216]
                0.00    0.00       5/5           VerifyHaltSpiNBody [191]
-----------------------------------------------
                0.00    0.00       1/1           VerifyOptions [31]
[285]    0.0    0.00    0.00       1         VerifyIntegration [285]
                0.00    0.00       5/5           bFileExists [214]
                0.00    0.00       2/124574      sLower [53]
-----------------------------------------------
                0.00    0.00       1/1           ReadUnitMass [157]
[286]    0.0    0.00    0.00       1         iAssignMassUnit [286]
                0.00    0.00       2/124574      sLower [53]
-----------------------------------------------
                0.00    0.00       1/1           ReadUnitAngle [155]
[287]    0.0    0.00    0.00       1         iAssignUnitAngle [287]
                0.00    0.00       2/124574      sLower [53]
-----------------------------------------------
                0.00    0.00       1/1           ReadUnitLength [156]
[288]    0.0    0.00    0.00       1         iAssignUnitLength [288]
                0.00    0.00       2/124574      sLower [53]
-----------------------------------------------
                0.00    0.00       1/1           ReadUnitTime [159]
[289]    0.0    0.00    0.00       1         iAssignUnitTime [289]
                0.00    0.00       1/124574      sLower [53]
-----------------------------------------------

 This table describes the call tree of the program, and was sorted by
 the total amount of time spent in each function and its children.

 Each entry in this table consists of several lines.  The line with the
 index number at the left hand margin lists the current function.
 The lines above it list the functions that called this function,
 and the lines below it list the functions this one called.
 This line lists:
     index	A unique number given to each element of the table.
		Index numbers are sorted numerically.
		The index number is printed next to every function name so
		it is easier to look up where the function is in the table.

     % time	This is the percentage of the `total' time that was spent
		in this function and its children.  Note that due to
		different viewpoints, functions excluded by options, etc,
		these numbers will NOT add up to 100%.

     self	This is the total amount of time spent in this function.

     children	This is the total amount of time propagated into this
		function by its children.

     called	This is the number of times the function was called.
		If the function called itself recursively, the number
		only includes non-recursive calls, and is followed by
		a `+' and the number of recursive calls.

     name	The name of the current function.  The index number is
		printed after it.  If the function is a member of a
		cycle, the cycle number is printed between the
		function's name and the index number.


 For the function's parents, the fields have the following meanings:

     self	This is the amount of time that was propagated directly
		from the function into this parent.

     children	This is the amount of time that was propagated from
		the function's children into this parent.

     called	This is the number of times this parent called the
		function `/' the total number of times the function
		was called.  Recursive calls to the function are not
		included in the number after the `/'.

     name	This is the name of the parent.  The parent's index
		number is printed after it.  If the parent is a
		member of a cycle, the cycle number is printed between
		the name and the index number.

 If the parents of the function cannot be determined, the word
 `<spontaneous>' is printed in the `name' field, and all the other
 fields are blank.

 For the function's children, the fields have the following meanings:

     self	This is the amount of time that was propagated directly
		from the child into the function.

     children	This is the amount of time that was propagated from the
		child's children to the function.

     called	This is the number of times the function called
		this child `/' the total number of times the child
		was called.  Recursive calls by the child are not
		listed in the number after the `/'.

     name	This is the name of the child.  The child's index
		number is printed after it.  If the child is a
		member of a cycle, the cycle number is printed
		between the name and the index number.

 If there are any cycles (circles) in the call graph, there is an
 entry for the cycle-as-a-whole.  This entry shows who called the
 cycle (as parents) and the members of the cycle (as children.)
 The `+' recursive calls entry shows the number of function calls that
 were internal to the cycle, and the calls entry for each member shows,
 for that member, how many times it was called from other members of
 the cycle.

Copyright (C) 2012-2016 Free Software Foundation, Inc.

Copying and distribution of this file, with or without modification,
are permitted in any medium without royalty provided the copyright
notice and this notice are preserved.

Index by function name

  [36] AddModuleSpiNBody     [112] ReadColor             [211] VerifyVelX
  [75] AddOptionBool         [113] ReadDigits            [212] VerifyVelY
  [67] AddOptionDouble       [114] ReadDoBackward        [213] VerifyVelZ
  [71] AddOptionInt          [115] ReadDoForward         [227] WriteAge
  [73] AddOptionString       [116] ReadDoLog              [93] WriteBodyType
  [82] AddOptionStringArray  [117] ReadDynEllip           [64] WriteDeltaTime
  [74] AssignDefaultDouble   [118] ReadEcc                [94] WriteHZLimitDryRunaway
  [77] AssignDefaultInt      [119] ReadEta               [228] WriteKinEnergy
 [168] AssignDefaultString   [120] ReadHaltMaxEcc        [229] WriteLXUVTot
  [29] AssignDt              [121] ReadHaltMerge          [52] WriteLog
  [12] BodyCopy              [122] ReadHaltMinEcc         [69] WriteLogEntry
  [16] BodyCopySpiNBody      [123] ReadHaltMinObl        [230] WriteLongP
  [42] BodyCopyStellar       [124] ReadHaltMinSemi        [95] WriteMass
  [68] CGSUnits              [125] ReadHaltPosDeDt        [19] WriteOutput
  [55] CheckComment          [126] ReadHecc               [58] WritePositionX
  [97] CheckDuplication      [127] ReadInc                [59] WritePositionY
 [169] CountHaltsSpiNBody     [40] ReadInitialOptions     [60] WritePositionZ
  [51] EulerStep             [128] ReadIntegrationMethod [231] WritePotEnergy
   [4] Evolve                [129] ReadKecc               [96] WriteRadius
  [37] FinalizeModule        [130] ReadLXUV               [65] WriteTime
 [170] FinalizeUpdatePositionXSpiNBody [131] ReadLogFile [232] WriteTotAngMom
 [171] FinalizeUpdatePositionYSpiNBody [132] ReadLongA   [233] WriteTotEnergy
 [172] FinalizeUpdatePositionZSpiNBody [133] ReadLongP    [61] WriteVelX
  [34] FinalizeUpdateVelXSpiNBody [134] ReadMass          [62] WriteVelY
 [173] FinalizeUpdateVelYSpiNBody [135] ReadMassRad       [63] WriteVelZ
 [174] FinalizeUpdateVelZSpiNBody [181] ReadMeanA         [44] _IO_file_close
  [27] ForceBehaviorSpiNBody [136] ReadMeanMotion         [45] _IO_unsave_markers
  [66] GetLine               [137] ReadMinValue           [46] __add
  [83] GetPos                [138] ReadModules             [6] __exp1
  [84] GetWords              [139] ReadObliquity           [1] __ieee754_pow_sse2
 [175] Helio2Bary             [41] ReadOptions            [25] __memcmp_sse4_1
 [242] InfileCopy            [280] ReadOptionsGeneral     [32] __mpexp
 [224] InitializeBodyModules [281] ReadOptionsModules     [26] __mul
 [176] InitializeBodySpiNBody [182] ReadOptionsSpiNBody   [47] __open_nocancel
 [243] InitializeControl     [140] ReadOrbPeriod          [48] __printf_fp_l
 [244] InitializeControlEvolve [141] ReadOutFile          [22] __sqr
 [177] InitializeControlNULL [183] ReadOutputOrder        [49] __strcpy_sse2_unaligned
 [245] InitializeHalts       [142] ReadOutputTime         [30] __write_nocancel
 [105] InitializeInput       [143] ReadOverwrite          [50] _fxstat
 [246] InitializeModule      [184] ReadPositionX         [214] bFileExists
 [247] InitializeOptions     [185] ReadPositionY          [81] dNegativeDouble
 [248] InitializeOptionsAtmEsc [186] ReadPositionZ        [23] fbCheckHalt
 [249] InitializeOptionsBinary [144] ReadPrecA            [20] fdDPositionXDt
 [250] InitializeOptionsDistOrb [145] ReadRadius          [15] fdDPositionYDt
 [251] InitializeOptionsDistRot [146] ReadRadiusGyration  [18] fdDPositionZDt
 [252] InitializeOptionsEqtide [147] ReadRotPeriod        [10] fdDVelXDt
 [253] InitializeOptionsFlare [148] ReadRotRate            [9] fdDVelYDt
 [254] InitializeOptionsGalHabit [149] ReadRotVel         [11] fdDVelZDt
 [255] InitializeOptionsGeneral [150] ReadSciNot           [7] fdGetUpdateInfo
 [256] InitializeOptionsPoise [151] ReadSemiMajorAxis    [218] fdKinEnergy
 [257] InitializeOptionsRadheat [152] ReadStopTime        [33] fdNextOutput
 [258] InitializeOptionsSpiNBody [153] ReadSystemName     [43] fdOrbAngMom
 [259] InitializeOptionsStellar [154] ReadTimeStep       [215] fdPerToFreq
 [260] InitializeOptionsThermint [155] ReadUnitAngle     [219] fdPotEnergy
 [261] InitializeOutput      [156] ReadUnitLength         [21] fdSemiToMeanMotion
 [262] InitializeOutputAtmEsc [157] ReadUnitMass         [234] fdTotAngMom
 [263] InitializeOutputBinary [158] ReadUnitTemp         [235] fdTotEnergy
 [264] InitializeOutputDistOrb [159] ReadUnitTime        [236] fdUnitsAngle
 [265] InitializeOutputDistRot [187] ReadUseOrbParams    [166] fdUnitsEnergy
 [266] InitializeOutputEqtide [160] ReadVarDt            [237] fdUnitsEnergyFlux
 [267] InitializeOutputFlare [188] ReadVelX               [76] fdUnitsLength
 [268] InitializeOutputGalHabit [189] ReadVelY            [79] fdUnitsMass
 [269] InitializeOutputGeneral [190] ReadVelZ            [238] fdUnitsPower
 [270] InitializeOutputPoise [161] ReadVerbose            [56] fdUnitsTime
 [271] InitializeOutputRadheat [162] ReadViscUMan        [216] fiNumHalts
 [272] InitializeOutputSpiNBody [163] ReadXobl            [54] fprintd
 [273] InitializeOutputStellar [164] ReadYobl            [239] fsUnitsAngMom
 [274] InitializeOutputThermint [165] ReadZobl           [240] fsUnitsAngle
  [35] InitializeUpdate        [5] RungeKutta4Step        [98] fsUnitsEnergy
 [178] InitializeUpdateBodyPerts [282] Unrecognized      [241] fsUnitsEnergyFlux
 [179] InitializeUpdateSpiNBody [283] UpdateCopy          [78] fsUnitsLength
  [38] InitializeUpdateTmpBody [70] UpdateFoundOption     [80] fsUnitsMass
  [39] InitializeUpdateTmpBodySpiNBody [87] UpdateFoundOptionMulti [57] fsUnitsTime
 [225] LogBody               [191] VerifyHaltSpiNBody    [286] iAssignMassUnit
  [88] LogBodyRelations      [284] VerifyHalts           [217] iAssignTempUnit
  [89] LogBodySpiNBody       [285] VerifyIntegration     [287] iAssignUnitAngle
  [90] LogGridOutput         [192] VerifyInterior        [288] iAssignUnitLength
 [275] LogHalt               [193] VerifyMassRad         [289] iAssignUnitTime
 [276] LogIntegration        [194] VerifyModuleMulti     [167] iGetNumLines
 [277] LogOptions            [195] VerifyModuleMultiAtmescEqtide [220] kepler_eqn
  [91] LogOutputOrder        [196] VerifyModuleMultiAtmescEqtideThermint [2] main
 [226] LogSystem             [197] VerifyModuleMultiBinaryEqtide [8] pow
 [278] LogUnits              [198] VerifyModuleMultiDistOrbDistRot [53] sLower
  [72] NotPrimaryInput       [199] VerifyModuleMultiEqtideDistOrb [221] signf
 [180] OrbElems2Helio        [200] VerifyModuleMultiEqtideDistorb [14] sqrt
  [92] PrintModuleList       [201] VerifyModuleMultiEqtideStellar [28] strlen
  [13] PropertiesAuxiliary   [202] VerifyModuleMultiEqtideThermint [222] vxi
  [24] PropertiesSpiNBody    [203] VerifyModuleMultiFlareStellar [223] vyi
  [17] PropsAuxGeneral       [204] VerifyModuleMultiRadheatThermint [99] xangle1
 [106] ReadAge                [31] VerifyOptions         [100] xangle2
 [107] ReadAlbedoGlobal      [205] VerifyPerturbersSpiNBody [85] xinit
 [108] ReadArgP              [206] VerifyPositionX       [101] yangle1
 [279] ReadBodyFileNames     [207] VerifyPositionY       [102] yangle2
 [109] ReadBodyName          [208] VerifyPositionZ        [86] yinit
 [110] ReadBodyType          [209] VerifyRotationGeneral [103] zangle1
 [111] ReadCalcDynEllip      [210] VerifySpiNBody        [104] zangle2
