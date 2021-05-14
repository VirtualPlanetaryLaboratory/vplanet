from benchmark import Benchmark, benchmark
import astropy.units as u
import pytest


@benchmark(
    {
        "log.final.sun.LXUVStellar": {"value": 0.000427, "unit": u.LSUN},
        "log.final.venus.SurfWaterMass": {"value": 0.919493, "unit": u.TO},
    }
)
class TestVenusWaterLoss(Benchmark):
    pass
