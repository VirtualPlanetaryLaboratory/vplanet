import astropy.units as u
from benchmark import Benchmark, benchmark


@benchmark(
    {
        "log.initial.system.Age": {"value": 3.155760e16, "unit": u.sec},
        "log.initial.system.Time": {"value": 0.0, "unit": u.sec},
        "log.initial.system.TotAngMom": {
            "value": 3.729543e40,
            "unit": (u.kg * u.m**2) / u.sec,
        },
        "log.initial.system.TotEnergy": {"value": -7.038771e40, "unit": u.Joule},
        "log.initial.star.Mass": {"value": 5.965248e29, "unit": u.kg},
        "log.initial.star.Radius": {"value": 2.024487e8, "unit": u.m},
        "log.initial.star.RadGyra": {"value": 0.458000},
        "log.initial.star.RotPer": {"value": 10.0, "unit": u.day},
        "log.initial.star.RotRate": {"value": 7.272205e-6, "unit": 1 / u.sec},
        "log.initial.star.Luminosity": {"value": 0.010328, "unit": u.LSUN},
        "log.initial.star.LXUVStellar": {"value": 6.081350e-7, "unit": u.LSUN},
        "log.initial.star.Temperature": {"value": 3415.002296, "unit": u.K},
        "log.final.system.Age": {"value": 3.155760e17, "unit": u.sec, "rtol": 1e-4},
        "log.final.system.TotEnergy": {"value": -6.865097e40, "unit": u.Joule, "rtol": 1e-4},
        "log.final.star.RotPer": {"value": 157.440880, "unit": u.day, "rtol": 1e-4},
        "log.final.star.Luminosity": {"value": 0.011350, "unit": u.LSUN, "rtol": 1e-4},
        "log.final.star.LXUVStellar": {"value": 3.935500e-8, "unit": u.LSUN, "rtol": 1e-4},
        "log.final.star.Temperature": {"value": 3416.001085, "unit": u.K, "rtol": 1e-4},
    }
)
class Test_EngleRotation(Benchmark):
    pass
