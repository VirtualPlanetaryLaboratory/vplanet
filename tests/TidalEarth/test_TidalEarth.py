from benchmark import Benchmark, benchmark
import astropy.units as u
import pytest


@pytest.mark.xfail(reason="thermint currently breaking logfiles")
@benchmark(
    {
        "log.final.tidalearth.TMan": {"value": 2799.517833, "unit": u.K},
        "log.final.tidalearth.TCore": {"value": 5500.224551, "unit": u.K},
        "log.final.tidalearth.PowerEqtide": {"value": 0.005337, "unit": u.TW,},
        "log.final.tidalearth.Eccentricity": {"value": 0.490441},
        "log.final.tidalearth.SemiMajorAxis": {"value": 7.409421e09, "unit": u.m},
    }
)
class TestTidalEarth(Benchmark):
    pass
