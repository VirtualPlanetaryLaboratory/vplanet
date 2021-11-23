from benchmark import Benchmark, benchmark
import astropy.units as u
import pytest


@benchmark(
    {
        "log.final.star.LXUVFlare": {"value": 3.1720730597e-07 , "unit": u.LSUN},
        "log.final.star.LXUVStellar": {"value": 5.9101358422e-09, "unit": u.LSUN},
        "log.final.b.SurfWaterMass": {"value": 0.4504314856, "unit": u.TO},
        "log.final.b.OxygenMass": {"value": 129.0409049277, "unit": u.bars},
                "b.OxygenMass": {"index": -1, "value": 129.0409049277, "unit": u.bars},
    }
)
class TestAtmEscFlare(Benchmark):
    pass
