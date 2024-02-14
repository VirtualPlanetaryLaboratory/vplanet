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
        "log.initial.sinewave.Mass": {"value": 1.988416e30, "unit": u.kg},
        "log.initial.sinewave.Radius": {"value": 109.065411, "unit": u.Rearth},
        "log.initial.sinewave.RadGyra": {"value": 0.500000},
        "log.initial.sinewave.RotAngMom": {
            "value": 1.749323e43,
            "unit": (u.kg * u.m**2) / u.sec,
        },
        "log.initial.sinewave.RotVel": {"value": 5.058765e04, "unit": u.m / u.sec},
        "log.initial.sinewave.BodyType": {"value": 0.000000},
        "log.initial.sinewave.RotRate": {"value": 7.272205e-05, "unit": 1 / u.sec},
        "log.initial.sinewave.RotPer": {"value": 1.000000, "unit": u.day},
        "log.initial.sinewave.Density": {"value": 1410.209204, "unit": u.kg / u.m**3},
        "log.initial.sinewave.HZLimitDryRunaway": {"value": 1.357831e11, "unit": u.m},
        "log.initial.sinewave.HZLimRecVenus": {"value": 1.118929e11, "unit": u.m},
        "log.initial.sinewave.HZLimRunaway": {"value": 1.461108e11, "unit": u.m},
        "log.initial.sinewave.HZLimMoistGreenhouse": {
            "value": 1.480517e11,
            "unit": u.m,
        },
        "log.initial.sinewave.HZLimMaxGreenhouse": {"value": 2.509538e11, "unit": u.m},
        "log.initial.sinewave.HZLimEarlyMars": {"value": 2.738109e11, "unit": u.m},
        "log.initial.sinewave.Instellation": {
            "value": -1.000000,
            "unit": u.kg / u.sec**3,
        },
        "log.initial.sinewave.CriticalSemiMajorAxis": {"value": -1.000000, "unit": u.m},
        "log.initial.sinewave.LXUVTot": {"value": 0.001000, "unit": u.LSUN},
        "log.initial.sinewave.LostEnergy": {"value": 5.562685e-309, "unit": u.Joule},
        "log.initial.sinewave.LostAngMom": {
            "value": 5.562685e-309,
            "unit": (u.kg * u.m**2) / u.sec,
        },
        "log.initial.sinewave.Luminosity": {"value": 1.000000, "unit": u.LSUN},
        "log.initial.sinewave.LXUVStellar": {"value": 3.846000e23, "unit": u.W},
        "log.initial.sinewave.Temperature": {"value": 5778.000000, "unit": u.K},
        "log.initial.sinewave.LXUVFrac": {"value": 0.001000},
        "log.initial.sinewave.RossbyNumber": {"value": 0.078260},
        "log.initial.sinewave.DRotPerDtStellar": {"value": 4.047674e-11},
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
        "log.final.sinewave.Mass": {"value": 1.988416e30, "unit": u.kg, "rtol": 1e-4},
        "log.final.sinewave.Radius": {
            "value": 109.065411,
            "unit": u.Rearth,
            "rtol": 1e-4,
        },
        "log.final.sinewave.RadGyra": {"value": 0.500000, "rtol": 1e-4},
        "log.final.sinewave.RotAngMom": {
            "value": 1.749298e43,
            "unit": (u.kg * u.m**2) / u.sec,
            "rtol": 1e-4,
        },
        "log.final.sinewave.RotVel": {
            "value": 5.058690e04,
            "unit": u.m / u.sec,
            "rtol": 1e-4,
        },
        "log.final.sinewave.BodyType": {"value": 0.000000, "rtol": 1e-4},
        "log.final.sinewave.RotRate": {
            "value": 7.272098e-05,
            "unit": 1 / u.sec,
            "rtol": 1e-4,
        },
        "log.final.sinewave.RotPer": {"value": 1.000015, "unit": u.day, "rtol": 1e-4},
        "log.final.sinewave.Density": {
            "value": 1410.209204,
            "unit": u.kg / u.m**3,
            "rtol": 1e-4,
        },
        "log.final.sinewave.HZLimitDryRunaway": {
            "value": 1.353561e11,
            "unit": u.m,
            "rtol": 1e-4,
        },
        "log.final.sinewave.HZLimRecVenus": {
            "value": 1.116179e11,
            "unit": u.m,
            "rtol": 1e-4,
        },
        "log.final.sinewave.HZLimRunaway": {
            "value": 1.458021e11,
            "unit": u.m,
            "rtol": 1e-4,
        },
        "log.final.sinewave.HZLimMoistGreenhouse": {
            "value": 1.476878e11,
            "unit": u.m,
            "rtol": 1e-4,
        },
        "log.final.sinewave.HZLimMaxGreenhouse": {
            "value": 2.505257e11,
            "unit": u.m,
            "rtol": 1e-4,
        },
        "log.final.sinewave.HZLimEarlyMars": {
            "value": 2.733430e11,
            "unit": u.m,
            "rtol": 1e-4,
        },
        "log.final.sinewave.Instellation": {
            "value": -1.000000,
            "unit": u.kg / u.sec**3,
            "rtol": 1e-4,
        },
        "log.final.sinewave.CriticalSemiMajorAxis": {
            "value": -1.000000,
            "unit": u.m,
            "rtol": 1e-4,
        },
        "log.final.sinewave.LXUVTot": {"value": 0.000994, "unit": u.LSUN, "rtol": 1e-4},
        "log.final.sinewave.LostEnergy": {
            "value": 1.880726e34,
            "unit": u.Joule,
            "rtol": 1e-4,
        },
        "log.final.sinewave.LostAngMom": {
            "value": 2.586203e38,
            "unit": (u.kg * u.m**2) / u.sec,
            "rtol": 1e-4,
        },
        "log.final.sinewave.Luminosity": {
            "value": 0.993721,
            "unit": u.LSUN,
            "rtol": 1e-4,
        },
        "log.final.sinewave.LXUVStellar": {
            "value": 3.821851e23,
            "unit": u.W,
            "rtol": 1e-4,
        },
        "log.final.sinewave.Temperature": {
            "value": 5760.875525,
            "unit": u.K,
            "rtol": 1e-4,
        },
        "log.final.sinewave.LXUVFrac": {"value": 0.001000, "rtol": 1e-4},
        "log.final.sinewave.RossbyNumber": {"value": 0.076602, "rtol": 1e-4},
        "log.final.sinewave.DRotPerDtStellar": {"value": 4.047734e-11, "rtol": 1e-4},
    }
)
class Test_SineWave(Benchmark):
    pass
