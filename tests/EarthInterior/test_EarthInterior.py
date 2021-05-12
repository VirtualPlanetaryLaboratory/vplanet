from benchmark import Benchmark, benchmark
import astropy.units as u
import pytest


@pytest.mark.xfail(reason="THERMINT is spitting out garbage in the logfile!")
@benchmark(
    {
        "earth.TMan": {"index": -1, "value": 2257.8509, "unit": u.K},
        "earth.TCore": {"index": -1, "value": 4999.1318, "unit": u.K},
        "earth.RIC": {"index": -1, "value": 1224.7839, "unit": u.km},
        "earth.RadPowerTotal": {"index": -1, "value": 24.3829, "unit": u.TW},
        "earth.MagMom": {"index": -1, "value": 1.009593},
    }
)
class TestEarthInterior(Benchmark):
    pass
