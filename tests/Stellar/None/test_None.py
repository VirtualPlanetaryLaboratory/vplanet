import astropy.units as u
from benchmark import Benchmark, benchmark


@benchmark(
    {
        "log.initial.system.Age": {"value": 0.000000, "unit": u.sec},
        "log.initial.system.Time": {"value": 0.000000, "unit": u.sec},
        "log.initial.system.TotAngMom": {
            "value": 1.749323e43,
            "unit": (u.kg * u.m**2) / u.sec,
        },
        "log.initial.system.TotEnergy": {"value": -2.269742e41, "unit": u.Joule},
        "log.initial.system.PotEnergy": {"value": -2.276102e41, "unit": u.Joule},
        "log.initial.system.KinEnergy": {"value": 6.360719e38, "unit": u.Joule},
        "log.initial.system.DeltaTime": {"value": 0.000000, "unit": u.sec},
        "log.initial.none.Mass": {"value": 1.988416e30, "unit": u.kg},
        "log.initial.none.Radius": {"value": 109.065411, "unit": u.Rearth},
        "log.initial.none.RadGyra": {"value": 0.500000},
        "log.initial.none.RotAngMom": {
            "value": 1.749323e43,
            "unit": (u.kg * u.m**2) / u.sec,
        },
        "log.initial.none.RotVel": {"value": 5.058765e04, "unit": u.m / u.sec},
        "log.initial.none.BodyType": {"value": 0.000000},
        "log.initial.none.RotRate": {"value": 7.272205e-05, "unit": 1 / u.sec},
        "log.initial.none.RotPer": {"value": 1.000000, "unit": u.day},
        "log.initial.none.Density": {"value": 1410.209204, "unit": u.kg / u.m**3},
        "log.initial.none.HZLimitDryRunaway": {"value": 1.357831e11, "unit": u.m},
        "log.initial.none.HZLimRecVenus": {"value": 1.118929e11, "unit": u.m},
        "log.initial.none.HZLimRunaway": {"value": 1.461108e11, "unit": u.m},
        "log.initial.none.HZLimMoistGreenhouse": {"value": 1.480517e11, "unit": u.m},
        "log.initial.none.HZLimMaxGreenhouse": {"value": 2.509538e11, "unit": u.m},
        "log.initial.none.HZLimEarlyMars": {"value": 2.738109e11, "unit": u.m},
        "log.initial.none.Instellation": {
            "value": -1.000000,
            "unit": u.kg / u.sec**3,
        },
        "log.initial.none.CriticalSemiMajorAxis": {"value": -1.000000, "unit": u.m},
        "log.initial.none.LXUVTot": {"value": 0.001000, "unit": u.LSUN},
        "log.initial.none.LostEnergy": {"value": 5.562685e-309, "unit": u.Joule},
        "log.initial.none.LostAngMom": {
            "value": 5.562685e-309,
            "unit": (u.kg * u.m**2) / u.sec,
        },
        "log.initial.none.Luminosity": {"value": 1.000000, "unit": u.LSUN},
        "log.initial.none.LXUVStellar": {"value": 3.846000e23, "unit": u.W},
        "log.initial.none.Temperature": {"value": 5778.000000, "unit": u.K},
        "log.initial.none.LXUVFrac": {"value": 0.001000},
        "log.initial.none.RossbyNumber": {"value": 0.078260},
        "log.initial.none.DRotPerDtStellar": {"value": 4.047674e-11},
        "log.final.system.Age": {"value": 3.155760e10, "unit": u.sec, "rtol": 1e-4},
        "log.final.system.Time": {"value": 3.155760e10, "unit": u.sec, "rtol": 1e-4},
        "log.final.system.TotAngMom": {
            "value": 1.749323e43,
            "unit": (u.kg * u.m**2) / u.sec,
            "rtol": 1e-4,
        },
        "log.final.system.TotEnergy": {
            "value": -2.269742e41,
            "unit": u.Joule,
            "rtol": 1e-4,
        },
        "log.final.system.PotEnergy": {
            "value": -2.276102e41,
            "unit": u.Joule,
            "rtol": 1e-4,
        },
        "log.final.system.KinEnergy": {
            "value": 6.360531e38,
            "unit": u.Joule,
            "rtol": 1e-4,
        },
        "log.final.none.Mass": {"value": 1.988416e30, "unit": u.kg, "rtol": 1e-4},
        "log.final.none.Radius": {"value": 109.065411, "unit": u.Rearth, "rtol": 1e-4},
        "log.final.none.RadGyra": {"value": 0.500000, "rtol": 1e-4},
        "log.final.none.RotAngMom": {
            "value": 1.749298e43,
            "unit": (u.kg * u.m**2) / u.sec,
            "rtol": 1e-4,
        },
        "log.final.none.RotVel": {
            "value": 5.058690e04,
            "unit": u.m / u.sec,
            "rtol": 1e-4,
        },
        "log.final.none.BodyType": {"value": 0.000000, "rtol": 1e-4},
        "log.final.none.RotRate": {
            "value": 7.272098e-05,
            "unit": 1 / u.sec,
            "rtol": 1e-4,
        },
        "log.final.none.RotPer": {"value": 1.000015, "unit": u.day, "rtol": 1e-4},
        "log.final.none.Density": {
            "value": 1410.209204,
            "unit": u.kg / u.m**3,
            "rtol": 1e-4,
        },
        "log.final.none.HZLimitDryRunaway": {
            "value": 1.357831e11,
            "unit": u.m,
            "rtol": 1e-4,
        },
        "log.final.none.HZLimRecVenus": {
            "value": 1.118929e11,
            "unit": u.m,
            "rtol": 1e-4,
        },
        "log.final.none.HZLimRunaway": {
            "value": 1.461108e11,
            "unit": u.m,
            "rtol": 1e-4,
        },
        "log.final.none.HZLimMoistGreenhouse": {
            "value": 1.480517e11,
            "unit": u.m,
            "rtol": 1e-4,
        },
        "log.final.none.HZLimMaxGreenhouse": {
            "value": 2.509538e11,
            "unit": u.m,
            "rtol": 1e-4,
        },
        "log.final.none.HZLimEarlyMars": {
            "value": 2.738109e11,
            "unit": u.m,
            "rtol": 1e-4,
        },
        "log.final.none.Instellation": {
            "value": -1.000000,
            "unit": u.kg / u.sec**3,
            "rtol": 1e-4,
        },
        "log.final.none.CriticalSemiMajorAxis": {
            "value": -1.000000,
            "unit": u.m,
            "rtol": 1e-4,
        },
        "log.final.none.LXUVTot": {"value": 0.001000, "unit": u.LSUN, "rtol": 1e-4},
        "log.final.none.LostEnergy": {
            "value": 1.880726e34,
            "unit": u.Joule,
            "rtol": 1e-4,
        },
        "log.final.none.LostAngMom": {
            "value": 2.586203e38,
            "unit": (u.kg * u.m**2) / u.sec,
            "rtol": 1e-4,
        },
        "log.final.none.Luminosity": {"value": 1.000000, "unit": u.LSUN, "rtol": 1e-4},
        "log.final.none.LXUVStellar": {"value": 3.846000e23, "unit": u.W, "rtol": 1e-4},
        "log.final.none.Temperature": {"value": 5778.000000, "unit": u.K, "rtol": 1e-4},
        "log.final.none.LXUVFrac": {"value": 0.001000, "rtol": 1e-4},
        "log.final.none.RossbyNumber": {"value": 0.078261, "rtol": 1e-4},
        "log.final.none.DRotPerDtStellar": {"value": 4.047734e-11, "rtol": 1e-4},
    }
)
class Test_None(Benchmark):
    pass
