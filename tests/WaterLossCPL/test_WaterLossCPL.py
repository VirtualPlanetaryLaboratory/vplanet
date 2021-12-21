from benchmark import Benchmark, benchmark
import astropy.units as u
import pytest


@benchmark(
    {
        "log.final.b.OrbPeriod": {"value": 3.943855e05, "unit": u.sec},
        "log.final.b.Eccentricity": {"value": 0.099247},
        "log.final.b.SurfWaterMass": {"value": 9.973861, "unit": u.TO},
    }
)
class TestWaterLossCPL(Benchmark):
    pass
