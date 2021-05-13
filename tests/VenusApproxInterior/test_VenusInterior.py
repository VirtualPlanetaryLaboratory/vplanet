from benchmark import Benchmark, benchmark
import astropy.units as u
import pytest

@pytest.mark.xfail(reason="thermint currently breaking logfiles")
@benchmark(
    {
        "log.final.venus.TMan": {"value": 2679.27122, "unit":u.K},
        "log.final.venus.TCore": {"value": 6365.71258, "unit":u.K},
        "log.final.venus.RIC": {"value": 0.0, "unit": u.km},
        "log.final.venus.RadPowerTotal": {"value":  31.49126, "unit": u.TW},
        "log.final.venus.MagMom": {"value": 0.0, "unit": u.EMAGMOM},
    }
)
class TestVenusApproxInterior(Benchmark):
    pass