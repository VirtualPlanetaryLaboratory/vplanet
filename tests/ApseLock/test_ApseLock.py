from benchmark import Benchmark, benchmark
import astropy.units as u


@benchmark(
    {
        "log.final.b.HEcc": {"value": -0.042671},
        "log.final.b.KEcc": {"value": -0.069231},
        "log.final.b.DHEccDtEqtide": {"value": 2.269581e-15, "unit": 1 / u.year},
        "log.final.b.DKEccDtEqtide": {"value": 3.682246e-15, "unit": 1 / u.year},
        "log.final.b.DHeccDtDistOrb": {"value": -2.488505e-11, "unit": 1 / u.year},
        "log.final.b.DKeccDtDistOrb": {"value": -1.344098e-11, "unit": 1 / u.year},
        "log.final.c.SemiMajorAxis": {"value": 6.911428e09, "unit": u.m},
        "log.final.b.Eccentricity": {"value": 0.081326},
        "log.final.b.ArgP": {"value": 3.693953, "unit": u.rad},
        "log.final.b.SemiMajorAxis": {"value": 2.812439e09, "unit": u.m},
        "log.final.c.Eccentricity": {"value": 0.193811},
        "log.final.c.ArgP": {"value": 5.015526, "unit": u.rad},
    }
)
class TestApseLock(Benchmark):
    pass
