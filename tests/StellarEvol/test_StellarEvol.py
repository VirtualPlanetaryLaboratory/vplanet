from benchmark import Benchmark, benchmark
import astropy.units as u
import pytest


@benchmark(
    {
        "log.final.a.Luminosity": {"value": 0.002445, "unit": u.Lsun},
        "log.final.a.Temperature": {"value": 2992.330343, "unit": u.K},
        "log.final.a.Radius": {"value": 0.184359, "unit": u.Rsun},
        "log.final.a.LostAngMom": {
            "value": 9.438825e41,
            "unit": u.kg * u.m ** 2 / u.sec,
        },
        "log.final.a.RotPer": {"value": 0.299254, "unit": u.days},
        "log.final.b.Luminosity": {"value": 0.689088, "unit": u.Lsun},
        "log.final.b.Temperature": {"value": 5539.190016, "unit": u.K},
        "og.final.b.Radius": {"value": 0.902638, "unit": u.Rsun},
        "log.final.b.LostAngMom": {
            "value": 5.086001e43,
            "unit": u.kg * u.m ** 2 / u.sec,
        },
        "log.final.b.RotPer": {"value": 3.075721, "unit": u.days},
        "log.final.b.Density": {"value": 1916.956602, "unit": u.kg / u.m ** 3},
    }
)
class TestStellarEvol(Benchmark):
    pass
