from benchmark import Benchmark, benchmark
import astropy.units as u
import pytest


@benchmark(
    {
        "log.final.cbp.FreeEcc": {"value": 0.03},
        "log.final.cbp.Eccentricity": {"value": 0.065653},
        "log.final.cbp.SemiMajorAxis": {"value": 1.046162e11, "unit": u.m},
        "log.final.secondary.Eccentricity": {"value": 0.16048},
        "log.final.secondary.SemiMajorAxis": {"value": 3.351740e10, "unit": u.m},
    }
)
class TestCircumbinaryOrbit(Benchmark):
    pass
