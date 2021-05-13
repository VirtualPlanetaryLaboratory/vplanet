from benchmark import Benchmark, benchmark
import astropy.units as u
import pytest


@benchmark(
    {
        "log.initial.a.HZLimRecVenus": {"value": 2.879712e10, "unit": u.m},
        "log.initial.a.HZLimEarlyMars": {"value": 7.969376e10, "unit": u.m},
        "log.initial.a.HZLimRunaway": {"value": 3.790332e10, "unit": u.m},
        "log.initial.a.HZLimMoistGreenhouse": {"value": 3.810337e10, "unit": u.m},
        "log.initial.a.HZLimMaxGreenhouse": {"value": 7.307316e10, "unit": u.m},
    }
)
class TestHZSingle(Benchmark):
    pass
