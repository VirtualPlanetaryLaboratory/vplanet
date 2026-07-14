import astropy.units as u
from benchmark import Benchmark, benchmark


@benchmark(
    {
        "log.initial.system.Age": {"value": 3.155760e16, "unit": u.sec},
        "log.initial.system.Time": {"value": 0.0, "unit": u.sec},
        "log.initial.system.TotAngMom": {
            "value": 8.694238e38,
            "unit": (u.kg * u.m**2) / u.sec,
        },
        "log.initial.system.TotEnergy": {"value": -3.754922e40, "unit": u.Joule},
        "log.initial.star.Mass": {"value": 3.599033e29, "unit": u.kg},
        "log.initial.star.Radius": {"value": 1.381422e8, "unit": u.m},
        "log.initial.star.RadGyra": {"value": 0.460833},
        "log.initial.star.RotPer": {"value": 1.054080e7, "unit": u.sec},
        "log.initial.star.RotRate": {"value": 5.960824e-7, "unit": 1 / u.sec},
        "log.initial.star.Luminosity": {"value": 0.003787, "unit": u.LSUN},
        "log.initial.star.LXUVStellar": {"value": 4.906221e-6, "unit": u.LSUN},
        "log.initial.star.LXUVTot": {"value": 4.906221e-6, "unit": u.LSUN},
        "log.initial.star.Temperature": {"value": 3212.579977, "unit": u.K},
        "log.final.system.Age": {"value": 3.108426e17, "unit": u.sec, "rtol": 1e-4},
        "log.final.system.TotAngMom": {
            "value": 8.931191e38,
            "unit": (u.kg * u.m**2) / u.sec, "rtol": 1e-4
        },
        "log.final.system.TotEnergy": {"value": -3.698928e40, "unit": u.Joule, "rtol": 1e-4},
        "log.final.star.Luminosity": {"value": 0.003956, "unit": u.LSUN, "rtol": 1e-4},
        "log.final.star.LXUVStellar": {"value": 2.529287e-7, "unit": u.LSUN, "rtol": 1e-4},
        "log.final.star.LXUVTot": {"value": 2.529287e-7, "unit": u.LSUN, "rtol": 1e-4},
        "log.final.star.Temperature": {"value": 3219.534123, "unit": u.K, "rtol": 1e-4},
        "log.final.star.RotPer": {"value": 1.193750e7, "unit": u.sec, "rtol": 1e-4},
    }
)
class Test_EngleXUV(Benchmark):
    pass
