from benchmark import Benchmark, benchmark 
import astropy.units as u 
 
@benchmark( 
   { 
       "log.initial.system.Age": {"value": 0.000000, "unit": u.sec}, 
       "log.initial.system.Time": {"value": 0.000000, "unit": u.sec}, 
       "log.initial.system.TotAngMom": {"value": 1.501063e+42, "unit": (u.kg * u.m ** 2) / u.sec}, 
       "log.initial.system.TotEnergy": {"value": -7.839372e+41, "unit": u.Joule}, 
       "log.initial.system.PotEnergy": {"value": -7.839908e+41, "unit": u.Joule}, 
       "log.initial.system.KinEnergy": {"value": 5.361272e+37, "unit": u.Joule}, 
       "log.initial.system.DeltaTime": {"value": 0.000000, "unit": u.sec}, 
       "log.initial.sun.Mass": {"value": 1.988416e+30, "unit": u.kg}, 
       "log.initial.sun.Radius": {"value": 2.019571e+08, "unit": u.m}, 
       "log.initial.sun.RadGyra": {"value": 0.500000}, 
       "log.initial.sun.RotAngMom": {"value": 1.474456e+42, "unit": (u.kg * u.m ** 2) / u.sec}, 
       "log.initial.sun.RotVel": {"value": 1.468674e+04, "unit": u.m / u.sec}, 
       "log.initial.sun.BodyType": {"value": 0.000000}, 
       "log.initial.sun.RotRate": {"value": 7.272205e-05, "unit": 1 / u.sec}, 
       "log.initial.sun.RotPer": {"value": 8.640000e+04, "unit": u.sec}, 
       "log.initial.sun.Density": {"value": 5.762900e+04, "unit": u.kg / u.m ** 3}, 
       "log.initial.sun.HZLimitDryRunaway": {"value": 1.357831e+11, "unit": u.m}, 
       "log.initial.sun.HZLimRecVenus": {"value": 1.118929e+11, "unit": u.m}, 
       "log.initial.sun.HZLimRunaway": {"value": 1.461108e+11, "unit": u.m}, 
       "log.initial.sun.HZLimMoistGreenhouse": {"value": 1.480517e+11, "unit": u.m}, 
       "log.initial.sun.HZLimMaxGreenhouse": {"value": 2.509538e+11, "unit": u.m}, 
       "log.initial.sun.HZLimEarlyMars": {"value": 2.738109e+11, "unit": u.m}, 
       "log.initial.sun.Instellation": {"value": -1.000000, "unit": u.kg / u.sec ** 3}, 
       "log.initial.sun.CriticalSemiMajorAxis": {"value": -1.000000, "unit": u.m}, 
       "log.initial.sun.LXUVTot": {"value": 3.846000e+23, "unit": u.kg / u.sec ** 3}, 
       "log.initial.sun.LostEnergy": {"value": 5.562685e-309, "unit": u.Joule}, 
       "log.initial.sun.LostAngMom": {"value": 5.562685e-309, "unit": (u.kg * u.m ** 2) / u.sec}, 
       "log.initial.sun.EscapeVelocity": {"value": 1.146413e+06, "unit": u.m / u.sec}, 
       "log.initial.sun.Luminosity": {"value": 3.846000e+26, "unit": u.W}, 
       "log.initial.sun.LXUVStellar": {"value": 3.846000e+23, "unit": u.W}, 
       "log.initial.sun.Temperature": {"value": 5778.000000, "unit": u.K}, 
       "log.initial.sun.LXUVFrac": {"value": 0.001000}, 
       "log.initial.sun.RossbyNumber": {"value": 0.078260}, 
       "log.initial.sun.DRotPerDtStellar": {"value": 6.558557e-13}, 
       "log.initial.sun.WindTorque": {"value": 1.119248e+25}, 
       "log.initial.equatorial.Mass": {"value": 5.971546e+24, "unit": u.kg}, 
       "log.initial.equatorial.Obliquity": {"value": 0.410152, "unit": u.rad}, 
       "log.initial.equatorial.PrecA": {"value": 0.000000, "unit": u.rad}, 
       "log.initial.equatorial.Radius": {"value": 6.378100e+06, "unit": u.m}, 
       "log.initial.equatorial.RadGyra": {"value": 0.500000}, 
       "log.initial.equatorial.BodyType": {"value": 0.000000}, 
       "log.initial.equatorial.Density": {"value": 5494.449526, "unit": u.kg / u.m ** 3}, 
       "log.initial.equatorial.HZLimitDryRunaway": {"value": 1.117992e+11, "unit": u.m}, 
       "log.initial.equatorial.HZLimRecVenus": {"value": 1.118929e+11, "unit": u.m}, 
       "log.initial.equatorial.HZLimRunaway": {"value": 1.461108e+11, "unit": u.m}, 
       "log.initial.equatorial.HZLimMoistGreenhouse": {"value": 1.480517e+11, "unit": u.m}, 
       "log.initial.equatorial.HZLimMaxGreenhouse": {"value": 2.509538e+11, "unit": u.m}, 
       "log.initial.equatorial.HZLimEarlyMars": {"value": 2.738109e+11, "unit": u.m}, 
       "log.initial.equatorial.Instellation": {"value": 1367.566935, "unit": u.kg / u.sec ** 3}, 
       "log.initial.equatorial.Eccentricity": {"value": 0.000000}, 
       "log.initial.equatorial.MeanMotion": {"value": 1.990987e-07, "unit": 1 / u.sec}, 
       "log.initial.equatorial.OrbPeriod": {"value": 3.155815e+07, "unit": u.sec}, 
       "log.initial.equatorial.SemiMajorAxis": {"value": 1.495979e+11, "unit": u.m}, 
       "log.initial.equatorial.COPP": {"value": 0.000000}, 
       "log.initial.equatorial.EscapeVelocity": {"value": 1.117931e+04, "unit": u.m / u.sec}, 
       "log.initial.equatorial.TGlobal": {"value": 16.637409, "unit": u.deg_C}, 
       "log.initial.equatorial.AlbedoGlobal": {"value": 0.322068}, 
       "log.initial.equatorial.FluxInGlobal": {"value": 237.998617, "unit": u.kg / u.sec ** 3}, 
       "log.initial.equatorial.FluxOutGlobal": {"value": 238.072185, "unit": u.W / u.m ** 2}, 
       "log.initial.equatorial.TotIceMass": {"value": 0.000000, "unit": u.kg}, 
       "log.initial.equatorial.TotIceFlow": {"value": 0.000000, "unit": u.kg}, 
       "log.initial.equatorial.TotIceBalance": {"value": 0.000000, "unit": u.kg}, 
       "log.initial.equatorial.SkipSeas": {"value": 0.000000}, 
       "log.initial.equatorial.AreaIceCov": {"value": 0.079338}, 
       "log.initial.equatorial.Latitude": {"value": -83.402352, "unit": u.deg}, 
       "log.initial.equatorial.TempLat": {"value": -10.684802, "unit": u.deg_C}, 
       "log.initial.equatorial.AlbedoLat": {"value": 0.600000}, 
       "log.initial.equatorial.AnnInsol": {"value": 176.064060, "unit": u.W / u.m ** 2}, 
       "log.initial.equatorial.FluxMerid": {"value": -1.632808, "unit": u.PW}, 
       "log.initial.equatorial.FluxIn": {"value": 70.432311, "unit": u.W / u.m ** 2}, 
       "log.initial.equatorial.FluxOut": {"value": 180.968764, "unit": u.W / u.m ** 2}, 
       "log.initial.equatorial.DivFlux": {"value": -110.461418, "unit": u.W / u.m ** 2}, 
       "log.initial.equatorial.IceMass": {"value": 0.000000}, 
       "log.initial.equatorial.IceHeight": {"value": 0.000000, "unit": u.m}, 
       "log.initial.equatorial.DIceMassDt": {"value": 0.000000, "unit": u.m}, 
       "log.initial.equatorial.IceFlow": {"value": 0.000000, "unit": u.m / u.sec}, 
       "log.initial.equatorial.EnergyResL": {"value": -15.700340, "unit": u.kg / u.sec ** 3}, 
       "log.initial.equatorial.EnergyResW": {"value": 0.154552, "unit": u.kg / u.sec ** 3}, 
       "log.initial.equatorial.BedrockH": {"value": 0.000000, "unit": u.m}, 
       "log.initial.equatorial.TempLandLat": {"value": 262.574234, "unit": u.sec}, 
       "log.initial.equatorial.TempWaterLat": {"value": 262.312582, "unit": u.sec}, 
       "log.initial.equatorial.AlbedoLandLat": {"value": 0.600000}, 
       "log.initial.equatorial.AlbedoWaterLat": {"value": 0.600000}, 
       "log.initial.equatorial.TempMinLat": {"value": -12.488589, "unit": u.deg_C}, 
       "log.initial.equatorial.TempMaxLat": {"value": -8.948297, "unit": u.deg_C}, 
       "log.initial.equatorial.Snowball": {"value": 0.000000}, 
       "log.initial.equatorial.PlanckBAvg": {"value": 2.090000}, 
       "log.initial.equatorial.IceAccum": {"value": 0.764899}, 
       "log.initial.equatorial.IceAblate": {"value": 0.000000}, 
       "log.initial.equatorial.TempMaxLand": {"value": 265.054711, "unit": u.sec}, 
       "log.initial.equatorial.TempMaxWater": {"value": 264.046713, "unit": u.sec}, 
       "log.initial.equatorial.PeakInsol": {"value": 541.704677, "unit": u.kg / u.sec ** 3}, 
       "log.initial.equatorial.IceCapNorthLand": {"value": 1.000000}, 
       "log.initial.equatorial.IceCapNorthSea": {"value": 1.000000}, 
       "log.initial.equatorial.IceCapSouthLand": {"value": 1.000000}, 
       "log.initial.equatorial.IceCapSouthSea": {"value": 1.000000}, 
       "log.initial.equatorial.IceBeltLand": {"value": 0.000000}, 
       "log.initial.equatorial.IceBeltSea": {"value": 0.000000}, 
       "log.initial.equatorial.SnowballLand": {"value": 0.000000}, 
       "log.initial.equatorial.SnowballSea": {"value": 0.000000}, 
       "log.initial.equatorial.IceFree": {"value": 0.000000}, 
       "log.initial.equatorial.IceCapNorthLatLand": {"value": 1.121291, "unit": u.rad}, 
       "log.initial.equatorial.IceCapNorthLatSea": {"value": 1.152808, "unit": u.rad}, 
       "log.initial.equatorial.IceCapSouthLatLand": {"value": -1.121291, "unit": u.rad}, 
       "log.initial.equatorial.IceCapSouthLatSea": {"value": -1.121291, "unit": u.rad}, 
       "log.initial.equatorial.IceBeltNorthLatLand": {"value": 100.000000, "unit": u.rad}, 
       "log.initial.equatorial.IceBeltNorthLatSea": {"value": 100.000000, "unit": u.rad}, 
       "log.initial.equatorial.IceBeltSouthLatLand": {"value": 100.000000, "unit": u.rad}, 
       "log.initial.equatorial.IceBeltSouthLatSea": {"value": 100.000000, "unit": u.rad}, 
       "log.initial.equatorial.LandFracGlobal": {"value": 0.289073}, 
       "log.final.system.Age": {"value": 3.155760e+07, "unit": u.sec, "rtol": 1e-4}, 
       "log.final.system.Time": {"value": 3.155760e+07, "unit": u.sec, "rtol": 1e-4}, 
       "log.final.system.TotAngMom": {"value": 1.501063e+42, "unit": (u.kg * u.m ** 2) / u.sec, "rtol": 1e-4}, 
       "log.final.system.TotEnergy": {"value": -7.839372e+41, "unit": u.Joule, "rtol": 1e-4}, 
       "log.final.system.PotEnergy": {"value": -7.839908e+41, "unit": u.Joule, "rtol": 1e-4}, 
       "log.final.system.KinEnergy": {"value": 5.361272e+37, "unit": u.Joule, "rtol": 1e-4}, 
       "log.final.sun.Mass": {"value": 1.988416e+30, "unit": u.kg, "rtol": 1e-4}, 
       "log.final.sun.Radius": {"value": 2.019571e+08, "unit": u.m, "rtol": 1e-4}, 
       "log.final.sun.RadGyra": {"value": 0.500000, "rtol": 1e-4}, 
       "log.final.sun.RotAngMom": {"value": 1.474456e+42, "unit": (u.kg * u.m ** 2) / u.sec, "rtol": 1e-4}, 
       "log.final.sun.RotVel": {"value": 1.468674e+04, "unit": u.m / u.sec, "rtol": 1e-4}, 
       "log.final.sun.BodyType": {"value": 0.000000, "rtol": 1e-4}, 
       "log.final.sun.RotRate": {"value": 7.272205e-05, "unit": 1 / u.sec, "rtol": 1e-4}, 
       "log.final.sun.RotPer": {"value": 8.640000e+04, "unit": u.sec, "rtol": 1e-4}, 
       "log.final.sun.Density": {"value": 5.762900e+04, "unit": u.kg / u.m ** 3, "rtol": 1e-4}, 
       "log.final.sun.HZLimitDryRunaway": {"value": 1.357831e+11, "unit": u.m, "rtol": 1e-4}, 
       "log.final.sun.HZLimRecVenus": {"value": 1.118929e+11, "unit": u.m, "rtol": 1e-4}, 
       "log.final.sun.HZLimRunaway": {"value": 1.461108e+11, "unit": u.m, "rtol": 1e-4}, 
       "log.final.sun.HZLimMoistGreenhouse": {"value": 1.480517e+11, "unit": u.m, "rtol": 1e-4}, 
       "log.final.sun.HZLimMaxGreenhouse": {"value": 2.509538e+11, "unit": u.m, "rtol": 1e-4}, 
       "log.final.sun.HZLimEarlyMars": {"value": 2.738109e+11, "unit": u.m, "rtol": 1e-4}, 
       "log.final.sun.Instellation": {"value": -1.000000, "unit": u.kg / u.sec ** 3, "rtol": 1e-4}, 
       "log.final.sun.CriticalSemiMajorAxis": {"value": -1.000000, "unit": u.m, "rtol": 1e-4}, 
       "log.final.sun.LXUVTot": {"value": 3.846000e+23, "unit": u.kg / u.sec ** 3, "rtol": 1e-4}, 
       "log.final.sun.LostEnergy": {"value": 2.568599e+28, "unit": u.Joule, "rtol": 1e-4}, 
       "log.final.sun.LostAngMom": {"value": 3.532078e+32, "unit": (u.kg * u.m ** 2) / u.sec, "rtol": 1e-4}, 
       "log.final.sun.EscapeVelocity": {"value": 1.146413e+06, "unit": u.m / u.sec, "rtol": 1e-4}, 
       "log.final.sun.Luminosity": {"value": 3.846000e+26, "unit": u.W, "rtol": 1e-4}, 
       "log.final.sun.LXUVStellar": {"value": 3.846000e+23, "unit": u.W, "rtol": 1e-4}, 
       "log.final.sun.Temperature": {"value": 5778.000000, "unit": u.K, "rtol": 1e-4}, 
       "log.final.sun.LXUVFrac": {"value": 0.001000, "rtol": 1e-4}, 
       "log.final.sun.RossbyNumber": {"value": 0.078260, "rtol": 1e-4}, 
       "log.final.sun.DRotPerDtStellar": {"value": 6.558557e-13, "rtol": 1e-4}, 
       "log.final.sun.WindTorque": {"value": 1.119248e+25, "rtol": 1e-4}, 
       "log.final.equatorial.Mass": {"value": 5.971546e+24, "unit": u.kg, "rtol": 1e-4}, 
       "log.final.equatorial.Obliquity": {"value": 0.410152, "unit": u.rad, "rtol": 1e-4}, 
       "log.final.equatorial.PrecA": {"value": 0.000000, "unit": u.rad, "rtol": 1e-4}, 
       "log.final.equatorial.Radius": {"value": 6.378100e+06, "unit": u.m, "rtol": 1e-4}, 
       "log.final.equatorial.RadGyra": {"value": 0.500000, "rtol": 1e-4}, 
       "log.final.equatorial.BodyType": {"value": 0.000000, "rtol": 1e-4}, 
       "log.final.equatorial.Density": {"value": 5494.449526, "unit": u.kg / u.m ** 3, "rtol": 1e-4}, 
       "log.final.equatorial.HZLimitDryRunaway": {"value": 1.117992e+11, "unit": u.m, "rtol": 1e-4}, 
       "log.final.equatorial.HZLimRecVenus": {"value": 1.118929e+11, "unit": u.m, "rtol": 1e-4}, 
       "log.final.equatorial.HZLimRunaway": {"value": 1.461108e+11, "unit": u.m, "rtol": 1e-4}, 
       "log.final.equatorial.HZLimMoistGreenhouse": {"value": 1.480517e+11, "unit": u.m, "rtol": 1e-4}, 
       "log.final.equatorial.HZLimMaxGreenhouse": {"value": 2.509538e+11, "unit": u.m, "rtol": 1e-4}, 
       "log.final.equatorial.HZLimEarlyMars": {"value": 2.738109e+11, "unit": u.m, "rtol": 1e-4}, 
       "log.final.equatorial.Instellation": {"value": 1367.566935, "unit": u.kg / u.sec ** 3, "rtol": 1e-4}, 
       "log.final.equatorial.Eccentricity": {"value": 0.000000, "rtol": 1e-4}, 
       "log.final.equatorial.MeanMotion": {"value": 1.990987e-07, "unit": 1 / u.sec, "rtol": 1e-4}, 
       "log.final.equatorial.OrbPeriod": {"value": 3.155815e+07, "unit": u.sec, "rtol": 1e-4}, 
       "log.final.equatorial.SemiMajorAxis": {"value": 1.495979e+11, "unit": u.m, "rtol": 1e-4}, 
       "log.final.equatorial.COPP": {"value": 0.000000, "rtol": 1e-4}, 
       "log.final.equatorial.EscapeVelocity": {"value": 1.117931e+04, "unit": u.m / u.sec, "rtol": 1e-4}, 
       "log.final.equatorial.TGlobal": {"value": 16.638715, "unit": u.deg_C, "rtol": 1e-4}, 
       "log.final.equatorial.AlbedoGlobal": {"value": 0.322068, "rtol": 1e-4}, 
       "log.final.equatorial.FluxInGlobal": {"value": 237.998617, "unit": u.kg / u.sec ** 3, "rtol": 1e-4}, 
       "log.final.equatorial.FluxOutGlobal": {"value": 238.074915, "unit": u.W / u.m ** 2, "rtol": 1e-4}, 
       "log.final.equatorial.TotIceMass": {"value": 8.926243e+13, "unit": u.kg, "rtol": 1e-4}, 
       "log.final.equatorial.TotIceFlow": {"value": 0.000000, "unit": u.kg, "rtol": 1e-4}, 
       "log.final.equatorial.TotIceBalance": {"value": 9.114271e-08, "unit": u.kg, "rtol": 1e-4}, 
       "log.final.equatorial.SkipSeas": {"value": 0.000000, "rtol": 1e-4}, 
       "log.final.equatorial.AreaIceCov": {"value": 0.079735, "rtol": 1e-4}, 
       "log.final.equatorial.Latitude": {"value": 83.402352, "unit": u.deg, "rtol": 1e-4}, 
       "log.final.equatorial.TempLat": {"value": -10.684156, "unit": u.deg_C, "rtol": 1e-4}, 
       "log.final.equatorial.AlbedoLat": {"value": 0.600000, "rtol": 1e-4}, 
       "log.final.equatorial.AnnInsol": {"value": 176.064060, "unit": u.W / u.m ** 2, "rtol": 1e-4}, 
       "log.final.equatorial.FluxMerid": {"value": 1.632799, "unit": u.PW, "rtol": 1e-4}, 
       "log.final.equatorial.FluxIn": {"value": 70.432311, "unit": u.W / u.m ** 2, "rtol": 1e-4}, 
       "log.final.equatorial.FluxOut": {"value": 180.970113, "unit": u.W / u.m ** 2, "rtol": 1e-4}, 
       "log.final.equatorial.DivFlux": {"value": -110.460798, "unit": u.W / u.m ** 2, "rtol": 1e-4}, 
       "log.final.equatorial.IceMass": {"value": 176.402053, "rtol": 1e-4}, 
       "log.final.equatorial.IceHeight": {"value": 0.192432, "unit": u.m, "rtol": 1e-4}, 
       "log.final.equatorial.DIceMassDt": {"value": 5.589844e-06, "unit": u.m, "rtol": 1e-4}, 
       "log.final.equatorial.IceFlow": {"value": 0.000000, "unit": u.m / u.sec, "rtol": 1e-4}, 
       "log.final.equatorial.EnergyResL": {"value": -15.700340, "unit": u.kg / u.sec ** 3, "rtol": 1e-4}, 
       "log.final.equatorial.EnergyResW": {"value": 0.154552, "unit": u.kg / u.sec ** 3, "rtol": 1e-4}, 
       "log.final.equatorial.BedrockH": {"value": -1.046896e-05, "unit": u.m, "rtol": 1e-4}, 
       "log.final.equatorial.TempLandLat": {"value": 262.578337, "unit": u.sec, "rtol": 1e-4}, 
       "log.final.equatorial.TempWaterLat": {"value": 262.313192, "unit": u.sec, "rtol": 1e-4}, 
       "log.final.equatorial.AlbedoLandLat": {"value": 0.600000, "rtol": 1e-4}, 
       "log.final.equatorial.AlbedoWaterLat": {"value": 0.600000, "rtol": 1e-4}, 
       "log.final.equatorial.TempMinLat": {"value": -12.488965, "unit": u.deg_C, "rtol": 1e-4}, 
       "log.final.equatorial.TempMaxLat": {"value": -8.947302, "unit": u.deg_C, "rtol": 1e-4}, 
       "log.final.equatorial.Snowball": {"value": 0.000000, "rtol": 1e-4}, 
       "log.final.equatorial.PlanckBAvg": {"value": 2.090000, "rtol": 1e-4}, 
       "log.final.equatorial.IceAccum": {"value": 0.774581, "rtol": 1e-4}, 
       "log.final.equatorial.IceAblate": {"value": 0.000000, "rtol": 1e-4}, 
       "log.final.equatorial.TempMaxLand": {"value": 265.050604, "unit": u.sec, "rtol": 1e-4}, 
       "log.final.equatorial.TempMaxWater": {"value": 264.047615, "unit": u.sec, "rtol": 1e-4}, 
       "log.final.equatorial.PeakInsol": {"value": 541.684612, "unit": u.kg / u.sec ** 3, "rtol": 1e-4}, 
       "log.final.equatorial.IceCapNorthLand": {"value": 1.000000, "rtol": 1e-4}, 
       "log.final.equatorial.IceCapNorthSea": {"value": 1.000000, "rtol": 1e-4}, 
       "log.final.equatorial.IceCapSouthLand": {"value": 1.000000, "rtol": 1e-4}, 
       "log.final.equatorial.IceCapSouthSea": {"value": 1.000000, "rtol": 1e-4}, 
       "log.final.equatorial.IceBeltLand": {"value": 0.000000, "rtol": 1e-4}, 
       "log.final.equatorial.IceBeltSea": {"value": 0.000000, "rtol": 1e-4}, 
       "log.final.equatorial.SnowballLand": {"value": 0.000000, "rtol": 1e-4}, 
       "log.final.equatorial.SnowballSea": {"value": 0.000000, "rtol": 1e-4}, 
       "log.final.equatorial.IceFree": {"value": 0.000000, "rtol": 1e-4}, 
       "log.final.equatorial.IceCapNorthLatLand": {"value": 1.091712, "unit": u.rad, "rtol": 1e-4}, 
       "log.final.equatorial.IceCapNorthLatSea": {"value": 1.152808, "unit": u.rad, "rtol": 1e-4}, 
       "log.final.equatorial.IceCapSouthLatLand": {"value": -1.091712, "unit": u.rad, "rtol": 1e-4}, 
       "log.final.equatorial.IceCapSouthLatSea": {"value": -1.091712, "unit": u.rad, "rtol": 1e-4}, 
       "log.final.equatorial.IceBeltNorthLatLand": {"value": 100.000000, "unit": u.rad, "rtol": 1e-4}, 
       "log.final.equatorial.IceBeltNorthLatSea": {"value": 100.000000, "unit": u.rad, "rtol": 1e-4}, 
       "log.final.equatorial.IceBeltSouthLatLand": {"value": 100.000000, "unit": u.rad, "rtol": 1e-4}, 
       "log.final.equatorial.IceBeltSouthLatSea": {"value": 100.000000, "unit": u.rad, "rtol": 1e-4}, 
       "log.final.equatorial.LandFracGlobal": {"value": 0.289073, "rtol": 1e-4}, 
   } 
)
class Test_SeasonalNC79Equatorial(Benchmark): 
   pass 