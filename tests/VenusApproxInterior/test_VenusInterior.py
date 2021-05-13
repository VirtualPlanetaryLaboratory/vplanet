from benchmark import Benchmark, benchmark
import astropy.units as u
import pytest


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


    # # Check
    # assert np.isclose(output.log.final.venus.TMan, 2679.27122)
    # assert np.isclose(output.log.final.venus.TCore, 6365.71258)
    # assert np.isclose(output.log.final.venus.RIC, 0.0)
    # assert np.isclose(output.log.final.venus.RadPowerTotal, 31.49126)
    # assert np.isclose(output.log.final.venus.MagMom, 0.0)
