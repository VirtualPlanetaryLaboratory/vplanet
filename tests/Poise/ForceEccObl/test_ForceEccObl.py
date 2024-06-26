import astropy.units as u
from benchmark import Benchmark, benchmark


@benchmark(
    {
        "log.initial.system.Age": {"value": 0.000000, "unit": u.sec},
        "log.initial.system.Time": {"value": 0.000000, "unit": u.sec},
        "log.initial.system.TotAngMom": {
            "value": 1.227033e42,
            "unit": (u.kg * u.m**2) / u.sec,
        },
        "log.initial.system.TotEnergy": {"value": -5.271115e41, "unit": u.Joule},
        "log.initial.system.PotEnergy": {"value": -5.271554e41, "unit": u.Joule},
        "log.initial.system.KinEnergy": {"value": 4.396243e37, "unit": u.Joule},
        "log.initial.system.DeltaTime": {"value": 0.000000, "unit": u.sec},
        "log.initial.sun.Mass": {"value": 1.630501e30, "unit": u.kg},
        "log.initial.sun.Radius": {"value": 2.019571e08, "unit": u.m},
        "log.initial.sun.RadGyra": {"value": 0.500000},
        "log.initial.sun.RotAngMom": {
            "value": 1.209054e42,
            "unit": (u.kg * u.m**2) / u.sec,
        },
        "log.initial.sun.RotVel": {"value": 1.468674e04, "unit": u.m / u.sec},
        "log.initial.sun.BodyType": {"value": 0.000000},
        "log.initial.sun.RotRate": {"value": 7.272205e-05, "unit": 1 / u.sec},
        "log.initial.sun.RotPer": {"value": 8.640000e04, "unit": u.sec},
        "log.initial.sun.Density": {"value": 4.725578e04, "unit": u.kg / u.m**3},
        "log.initial.sun.HZLimitDryRunaway": {"value": 1.357831e11, "unit": u.m},
        "log.initial.sun.HZLimRecVenus": {"value": 1.118929e11, "unit": u.m},
        "log.initial.sun.HZLimRunaway": {"value": 1.461108e11, "unit": u.m},
        "log.initial.sun.HZLimMoistGreenhouse": {"value": 1.480517e11, "unit": u.m},
        "log.initial.sun.HZLimMaxGreenhouse": {"value": 2.509538e11, "unit": u.m},
        "log.initial.sun.HZLimEarlyMars": {"value": 2.738109e11, "unit": u.m},
        "log.initial.sun.Instellation": {"value": -1.000000, "unit": u.kg / u.sec**3},
        "log.initial.sun.CriticalSemiMajorAxis": {"value": -1.000000, "unit": u.m},
        "log.initial.sun.LXUVTot": {"value": 3.846000e23, "unit": u.kg / u.sec**3},
        "log.initial.sun.LostEnergy": {"value": 5.562685e-309, "unit": u.Joule},
        "log.initial.sun.LostAngMom": {
            "value": 5.562685e-309,
            "unit": (u.kg * u.m**2) / u.sec,
        },
        "log.initial.sun.Luminosity": {"value": 3.846000e26, "unit": u.W},
        "log.initial.sun.LXUVStellar": {"value": 3.846000e23, "unit": u.W},
        "log.initial.sun.Temperature": {"value": 5778.000000, "unit": u.K},
        "log.initial.sun.LXUVFrac": {"value": 0.001000},
        "log.initial.sun.RossbyNumber": {"value": 0.078260},
        "log.initial.sun.DRotPerDtStellar": {"value": 9.129602e-13},
        "log.initial.earth.Mass": {"value": 5.971546e24, "unit": u.kg},
        "log.initial.earth.Obliquity": {"value": 0.855211, "unit": u.rad},
        "log.initial.earth.PrecA": {"value": 0.000000, "unit": u.rad},
        "log.initial.earth.Radius": {"value": 6.378100e06, "unit": u.m},
        "log.initial.earth.RadGyra": {"value": 0.500000},
        "log.initial.earth.BodyType": {"value": 0.000000},
        "log.initial.earth.Density": {"value": 5494.449526, "unit": u.kg / u.m**3},
        "log.initial.earth.HZLimitDryRunaway": {"value": 1.152787e11, "unit": u.m},
        "log.initial.earth.HZLimRecVenus": {"value": 1.118929e11, "unit": u.m},
        "log.initial.earth.HZLimRunaway": {"value": 1.461108e11, "unit": u.m},
        "log.initial.earth.HZLimMoistGreenhouse": {"value": 1.480517e11, "unit": u.m},
        "log.initial.earth.HZLimMaxGreenhouse": {"value": 2.509538e11, "unit": u.m},
        "log.initial.earth.HZLimEarlyMars": {"value": 2.738109e11, "unit": u.m},
        "log.initial.earth.Instellation": {
            "value": 4149.129483,
            "unit": u.kg / u.sec**3,
        },
        "log.initial.earth.Eccentricity": {"value": 0.200000},
        "log.initial.earth.MeanMotion": {"value": 4.081628e-07, "unit": 1 / u.sec},
        "log.initial.earth.OrbPeriod": {"value": 1.539382e07, "unit": u.sec},
        "log.initial.earth.SemiMajorAxis": {"value": 8.676677e10, "unit": u.m},
        "log.initial.earth.COPP": {"value": 0.000000},
        "log.initial.earth.TGlobal": {"value": 257.421264, "unit": u.deg_C},
        "log.initial.earth.AlbedoGlobal": {"value": 0.308044},
        "log.initial.earth.FluxInGlobal": {
            "value": 741.375272,
            "unit": u.kg / u.sec**3,
        },
        "log.initial.earth.FluxOutGlobal": {
            "value": 741.310442,
            "unit": u.W / u.m**2,
        },
        "log.initial.earth.TotIceMass": {"value": 0.000000, "unit": u.kg},
        "log.initial.earth.TotIceFlow": {"value": 0.000000, "unit": u.kg},
        "log.initial.earth.TotIceBalance": {"value": 0.000000, "unit": u.kg},
        "log.initial.earth.SkipSeas": {"value": 0.000000},
        "log.initial.earth.AreaIceCov": {"value": 0.000000},
        "log.initial.earth.Latitude": {"value": -83.402352, "unit": u.deg},
        "log.initial.earth.TempLat": {"value": 245.271553, "unit": u.deg_C},
        "log.initial.earth.AlbedoLat": {"value": 0.343147},
        "log.initial.earth.AnnInsol": {"value": 995.950862, "unit": u.W / u.m**2},
        "log.initial.earth.FluxMerid": {"value": -0.585888, "unit": u.PW},
        "log.initial.earth.FluxIn": {"value": 676.346321, "unit": u.W / u.m**2},
        "log.initial.earth.FluxOut": {"value": 715.917545, "unit": u.W / u.m**2},
        "log.initial.earth.DivFlux": {"value": -39.636036, "unit": u.W / u.m**2},
        "log.initial.earth.IceMass": {"value": 0.000000},
        "log.initial.earth.IceHeight": {"value": 0.000000, "unit": u.m},
        "log.initial.earth.DIceMassDt": {"value": 0.000000, "unit": u.m},
        "log.initial.earth.IceFlow": {"value": 0.000000, "unit": u.m / u.sec},
        "log.initial.earth.EnergyResL": {
            "value": -1.136868e-13,
            "unit": u.kg / u.sec**3,
        },
        "log.initial.earth.EnergyResW": {
            "value": -6.298251e-11,
            "unit": u.kg / u.sec**3,
        },
        "log.initial.earth.BedrockH": {"value": 0.000000, "unit": u.m},
        "log.initial.earth.TempLandLat": {"value": 506.642148, "unit": u.sec},
        "log.initial.earth.TempWaterLat": {"value": 524.262458, "unit": u.sec},
        "log.initial.earth.AlbedoLandLat": {"value": 0.409147},
        "log.initial.earth.AlbedoWaterLat": {"value": 0.309147},
        "log.initial.earth.TempMinLat": {"value": 199.498898, "unit": u.deg_C},
        "log.initial.earth.TempMaxLat": {"value": 298.992450, "unit": u.deg_C},
        "log.initial.earth.Snowball": {"value": 0.000000},
        "log.initial.earth.PlanckBAvg": {"value": 2.090000},
        "log.initial.earth.IceAccum": {"value": 0.000000},
        "log.initial.earth.IceAblate": {"value": 0.000000},
        "log.initial.earth.TempMaxLand": {"value": 651.017146, "unit": u.sec},
        "log.initial.earth.TempMaxWater": {"value": 533.920743, "unit": u.sec},
        "log.initial.earth.PeakInsol": {
            "value": 3546.542440,
            "unit": u.kg / u.sec**3,
        },
        "log.initial.earth.IceCapNorthLand": {"value": 0.000000},
        "log.initial.earth.IceCapNorthSea": {"value": 0.000000},
        "log.initial.earth.IceCapSouthLand": {"value": 0.000000},
        "log.initial.earth.IceCapSouthSea": {"value": 0.000000},
        "log.initial.earth.IceBeltLand": {"value": 0.000000},
        "log.initial.earth.IceBeltSea": {"value": 0.000000},
        "log.initial.earth.SnowballLand": {"value": 0.000000},
        "log.initial.earth.SnowballSea": {"value": 0.000000},
        "log.initial.earth.IceFree": {"value": 1.000000},
        "log.initial.earth.IceCapNorthLatLand": {"value": 100.000000, "unit": u.rad},
        "log.initial.earth.IceCapNorthLatSea": {"value": 100.000000, "unit": u.rad},
        "log.initial.earth.IceCapSouthLatLand": {"value": 100.000000, "unit": u.rad},
        "log.initial.earth.IceCapSouthLatSea": {"value": 100.000000, "unit": u.rad},
        "log.initial.earth.IceBeltNorthLatLand": {"value": 100.000000, "unit": u.rad},
        "log.initial.earth.IceBeltNorthLatSea": {"value": 100.000000, "unit": u.rad},
        "log.initial.earth.IceBeltSouthLatLand": {"value": 100.000000, "unit": u.rad},
        "log.initial.earth.IceBeltSouthLatSea": {"value": 100.000000, "unit": u.rad},
        "log.final.system.Age": {"value": 3.155760e10, "unit": u.sec, "rtol": 1e-4},
        "log.final.system.Time": {"value": 3.155760e10, "unit": u.sec, "rtol": 1e-4},
        "log.final.system.TotAngMom": {
            "value": 1.227033e42,
            "unit": (u.kg * u.m**2) / u.sec,
            "rtol": 1e-4,
        },
        "log.final.system.TotEnergy": {
            "value": -5.271115e41,
            "unit": u.Joule,
            "rtol": 1e-4,
        },
        "log.final.system.PotEnergy": {
            "value": -5.271554e41,
            "unit": u.Joule,
            "rtol": 1e-4,
        },
        "log.final.system.KinEnergy": {
            "value": 4.396240e37,
            "unit": u.Joule,
            "rtol": 1e-4,
        },
        "log.final.sun.Mass": {"value": 1.630501e30, "unit": u.kg, "rtol": 1e-4},
        "log.final.sun.Radius": {"value": 2.019571e08, "unit": u.m, "rtol": 1e-4},
        "log.final.sun.RadGyra": {"value": 0.500000, "rtol": 1e-4},
        "log.final.sun.RotAngMom": {
            "value": 1.209053e42,
            "unit": (u.kg * u.m**2) / u.sec,
            "rtol": 1e-4,
        },
        "log.final.sun.RotVel": {
            "value": 1.468673e04,
            "unit": u.m / u.sec,
            "rtol": 1e-4,
        },
        "log.final.sun.BodyType": {"value": 0.000000, "rtol": 1e-4},
        "log.final.sun.RotRate": {
            "value": 7.272203e-05,
            "unit": 1 / u.sec,
            "rtol": 1e-4,
        },
        "log.final.sun.RotPer": {"value": 8.640003e04, "unit": u.sec, "rtol": 1e-4},
        "log.final.sun.Density": {
            "value": 4.725578e04,
            "unit": u.kg / u.m**3,
            "rtol": 1e-4,
        },
        "log.final.sun.HZLimitDryRunaway": {
            "value": 1.357831e11,
            "unit": u.m,
            "rtol": 1e-4,
        },
        "log.final.sun.HZLimRecVenus": {
            "value": 1.118929e11,
            "unit": u.m,
            "rtol": 1e-4,
        },
        "log.final.sun.HZLimRunaway": {"value": 1.461108e11, "unit": u.m, "rtol": 1e-4},
        "log.final.sun.HZLimMoistGreenhouse": {
            "value": 1.480517e11,
            "unit": u.m,
            "rtol": 1e-4,
        },
        "log.final.sun.HZLimMaxGreenhouse": {
            "value": 2.509538e11,
            "unit": u.m,
            "rtol": 1e-4,
        },
        "log.final.sun.HZLimEarlyMars": {
            "value": 2.738109e11,
            "unit": u.m,
            "rtol": 1e-4,
        },
        "log.final.sun.Instellation": {
            "value": -1.000000,
            "unit": u.kg / u.sec**3,
            "rtol": 1e-4,
        },
        "log.final.sun.CriticalSemiMajorAxis": {
            "value": -1.000000,
            "unit": u.m,
            "rtol": 1e-4,
        },
        "log.final.sun.LXUVTot": {
            "value": 3.846000e23,
            "unit": u.kg / u.sec**3,
            "rtol": 1e-4,
        },
        "log.final.sun.LostEnergy": {
            "value": 2.931930e31,
            "unit": u.Joule,
            "rtol": 1e-4,
        },
        "log.final.sun.LostAngMom": {
            "value": 4.031694e35,
            "unit": (u.kg * u.m**2) / u.sec,
            "rtol": 1e-4,
        },
        "log.final.sun.Luminosity": {"value": 3.846000e26, "unit": u.W, "rtol": 1e-4},
        "log.final.sun.LXUVStellar": {"value": 3.846000e23, "unit": u.W, "rtol": 1e-4},
        "log.final.sun.Temperature": {"value": 5778.000000, "unit": u.K, "rtol": 1e-4},
        "log.final.sun.LXUVFrac": {"value": 0.001000, "rtol": 1e-4},
        "log.final.sun.RossbyNumber": {"value": 0.078260, "rtol": 1e-4},
        "log.final.sun.DRotPerDtStellar": {"value": 9.129605e-13, "rtol": 1e-4},
        "log.final.earth.Mass": {"value": 5.971546e24, "unit": u.kg, "rtol": 1e-4},
        "log.final.earth.Obliquity": {"value": 0.855211, "unit": u.rad, "rtol": 1e-4},
        "log.final.earth.PrecA": {"value": 0.000000, "unit": u.rad, "rtol": 1e-4},
        "log.final.earth.Radius": {"value": 6.378100e06, "unit": u.m, "rtol": 1e-4},
        "log.final.earth.RadGyra": {"value": 0.500000, "rtol": 1e-4},
        "log.final.earth.BodyType": {"value": 0.000000, "rtol": 1e-4},
        "log.final.earth.Density": {
            "value": 5494.449526,
            "unit": u.kg / u.m**3,
            "rtol": 1e-4,
        },
        "log.final.earth.HZLimitDryRunaway": {
            "value": 1.152787e11,
            "unit": u.m,
            "rtol": 1e-4,
        },
        "log.final.earth.HZLimRecVenus": {
            "value": 1.118929e11,
            "unit": u.m,
            "rtol": 1e-4,
        },
        "log.final.earth.HZLimRunaway": {
            "value": 1.461108e11,
            "unit": u.m,
            "rtol": 1e-4,
        },
        "log.final.earth.HZLimMoistGreenhouse": {
            "value": 1.480517e11,
            "unit": u.m,
            "rtol": 1e-4,
        },
        "log.final.earth.HZLimMaxGreenhouse": {
            "value": 2.509538e11,
            "unit": u.m,
            "rtol": 1e-4,
        },
        "log.final.earth.HZLimEarlyMars": {
            "value": 2.738109e11,
            "unit": u.m,
            "rtol": 1e-4,
        },
        "log.final.earth.Instellation": {
            "value": 4149.129483,
            "unit": u.kg / u.sec**3,
            "rtol": 1e-4,
        },
        "log.final.earth.Eccentricity": {"value": 0.200000, "rtol": 1e-4},
        "log.final.earth.MeanMotion": {
            "value": 4.081628e-07,
            "unit": 1 / u.sec,
            "rtol": 1e-4,
        },
        "log.final.earth.OrbPeriod": {
            "value": 1.539382e07,
            "unit": u.sec,
            "rtol": 1e-4,
        },
        "log.final.earth.SemiMajorAxis": {
            "value": 8.676677e10,
            "unit": u.m,
            "rtol": 1e-4,
        },
        "log.final.earth.COPP": {"value": 0.000000, "rtol": 1e-4},
        "log.final.earth.TGlobal": {"value": 257.433918, "unit": u.deg_C, "rtol": 1e-4},
        "log.final.earth.AlbedoGlobal": {"value": 0.308044, "rtol": 1e-4},
        "log.final.earth.FluxInGlobal": {
            "value": 741.375272,
            "unit": u.kg / u.sec**3,
            "rtol": 1e-4,
        },
        "log.final.earth.FluxOutGlobal": {
            "value": 741.336888,
            "unit": u.W / u.m**2,
            "rtol": 1e-4,
        },
        "log.final.earth.TotIceMass": {"value": 0.000000, "unit": u.kg, "rtol": 1e-4},
        "log.final.earth.TotIceFlow": {"value": 0.000000, "unit": u.kg, "rtol": 1e-4},
        "log.final.earth.TotIceBalance": {
            "value": 0.000000,
            "unit": u.kg,
            "rtol": 1e-4,
        },
        "log.final.earth.SkipSeas": {"value": 0.000000, "rtol": 1e-4},
        "log.final.earth.AreaIceCov": {"value": 0.000000, "rtol": 1e-4},
        "log.final.earth.Latitude": {"value": 83.402352, "unit": u.deg, "rtol": 1e-4},
        "log.final.earth.TempLat": {"value": 246.419100, "unit": u.deg_C, "rtol": 1e-4},
        "log.final.earth.AlbedoLat": {"value": 0.343142, "rtol": 1e-4},
        "log.final.earth.AnnInsol": {
            "value": 995.951219,
            "unit": u.W / u.m**2,
            "rtol": 1e-4,
        },
        "log.final.earth.FluxMerid": {"value": 0.575397, "unit": u.PW, "rtol": 1e-4},
        "log.final.earth.FluxIn": {
            "value": 679.428038,
            "unit": u.W / u.m**2,
            "rtol": 1e-4,
        },
        "log.final.earth.FluxOut": {
            "value": 718.315918,
            "unit": u.W / u.m**2,
            "rtol": 1e-4,
        },
        "log.final.earth.DivFlux": {
            "value": -38.926273,
            "unit": u.W / u.m**2,
            "rtol": 1e-4,
        },
        "log.final.earth.IceMass": {"value": 0.000000, "rtol": 1e-4},
        "log.final.earth.IceHeight": {"value": 0.000000, "unit": u.m, "rtol": 1e-4},
        "log.final.earth.DIceMassDt": {"value": 0.000000, "unit": u.m, "rtol": 1e-4},
        "log.final.earth.IceFlow": {
            "value": 0.000000,
            "unit": u.m / u.sec,
            "rtol": 1e-4,
        },
        "log.final.earth.EnergyResL": {
            "value": -1.705303e-12,
            "unit": u.kg / u.sec**3,
            "rtol": 1e-4,
        },
        "log.final.earth.EnergyResW": {
            "value": -4.695266e-11,
            "unit": u.kg / u.sec**3,
            "rtol": 1e-4,
        },
        "log.final.earth.BedrockH": {"value": 0.000000, "unit": u.m, "rtol": 1e-4},
        "log.final.earth.TempLandLat": {
            "value": 507.770168,
            "unit": u.sec,
            "rtol": 1e-4,
        },
        "log.final.earth.TempWaterLat": {
            "value": 525.420064,
            "unit": u.sec,
            "rtol": 1e-4,
        },
        "log.final.earth.AlbedoLandLat": {"value": 0.409142, "rtol": 1e-4},
        "log.final.earth.AlbedoWaterLat": {"value": 0.309142, "rtol": 1e-4},
        "log.final.earth.TempMinLat": {
            "value": 207.775852,
            "unit": u.deg_C,
            "rtol": 1e-4,
        },
        "log.final.earth.TempMaxLat": {
            "value": 308.077517,
            "unit": u.deg_C,
            "rtol": 1e-4,
        },
        "log.final.earth.Snowball": {"value": 0.000000, "rtol": 1e-4},
        "log.final.earth.PlanckBAvg": {"value": 2.090000, "rtol": 1e-4},
        "log.final.earth.IceAccum": {"value": 0.000000, "rtol": 1e-4},
        "log.final.earth.IceAblate": {"value": 0.000000, "rtol": 1e-4},
        "log.final.earth.TempMaxLand": {
            "value": 669.160836,
            "unit": u.sec,
            "rtol": 1e-4,
        },
        "log.final.earth.TempMaxWater": {
            "value": 536.899370,
            "unit": u.sec,
            "rtol": 1e-4,
        },
        "log.final.earth.PeakInsol": {
            "value": 3547.525214,
            "unit": u.kg / u.sec**3,
            "rtol": 1e-4,
        },
        "log.final.earth.IceCapNorthLand": {"value": 0.000000, "rtol": 1e-4},
        "log.final.earth.IceCapNorthSea": {"value": 0.000000, "rtol": 1e-4},
        "log.final.earth.IceCapSouthLand": {"value": 0.000000, "rtol": 1e-4},
        "log.final.earth.IceCapSouthSea": {"value": 0.000000, "rtol": 1e-4},
        "log.final.earth.IceBeltLand": {"value": 0.000000, "rtol": 1e-4},
        "log.final.earth.IceBeltSea": {"value": 0.000000, "rtol": 1e-4},
        "log.final.earth.SnowballLand": {"value": 0.000000, "rtol": 1e-4},
        "log.final.earth.SnowballSea": {"value": 0.000000, "rtol": 1e-4},
        "log.final.earth.IceFree": {"value": 1.000000, "rtol": 1e-4},
        "log.final.earth.IceCapNorthLatLand": {
            "value": 100.000000,
            "unit": u.rad,
            "rtol": 1e-4,
        },
        "log.final.earth.IceCapNorthLatSea": {
            "value": 100.000000,
            "unit": u.rad,
            "rtol": 1e-4,
        },
        "log.final.earth.IceCapSouthLatLand": {
            "value": 100.000000,
            "unit": u.rad,
            "rtol": 1e-4,
        },
        "log.final.earth.IceCapSouthLatSea": {
            "value": 100.000000,
            "unit": u.rad,
            "rtol": 1e-4,
        },
        "log.final.earth.IceBeltNorthLatLand": {
            "value": 100.000000,
            "unit": u.rad,
            "rtol": 1e-4,
        },
        "log.final.earth.IceBeltNorthLatSea": {
            "value": 100.000000,
            "unit": u.rad,
            "rtol": 1e-4,
        },
        "log.final.earth.IceBeltSouthLatLand": {
            "value": 100.000000,
            "unit": u.rad,
            "rtol": 1e-4,
        },
        "log.final.earth.IceBeltSouthLatSea": {
            "value": 100.000000,
            "unit": u.rad,
            "rtol": 1e-4,
        },
    }
)
class Test_ForceEccObl(Benchmark):
    pass
