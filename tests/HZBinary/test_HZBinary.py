from benchmark import Benchmark, benchmark
import astropy.units as u
import pytest


@benchmark(
    {
        "log.final.secondary.HZLimRecVenus": {"value": 9.828450e10, "unit": u.m},
        "log.final.secondary.HZLimEarlyMars": {"value": 2.673083e11, "unit": u.m},
        "log.final.secondary.HZLimRunaway": {"value": 1.298768e11, "unit": u.m},
        "log.final.secondary.HZLimRecVenus": {"value": 9.828450e10, "unit": u.m},
        "log.final.secondary.HZLimMoistGreenhouse": {"value": 1.300464e11, "unit": u.m},
        "log.final.secondary.HZLimMaxGreenhouse": {"value": 2.450954e11, "unit": u.m},
    }
)
class TestHZBinary(Benchmark):
    pass
