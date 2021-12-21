from benchmark import Benchmark, benchmark
import astropy.units as u
import pytest


@benchmark(
    {
        "log.final.sun.LXUVStellar": {"value": 0.000427, "unit": u.LSUN},
        "log.final.venus.SurfWaterMass": {"value": 0.915086, "unit": u.TO},
        "log.final.venus.XO": {"value": 0.333333},
        "log.final.venus.EtaO": {"value": 0.093075},
        "log.final.venus.AtmXAbsEffH2O": {"value": 0.069496}
    }
)
class TestVenusWaterLoss(Benchmark):
    pass
