from benchmark import Benchmark, benchmark
import astropy.units as u
import pytest


@benchmark(
    {
        "log.final.comp.SemiMajorAxis": {"value": 1.4959787070e+15, "unit": u.m},
        "log.final.comp.Eccentricity": {"value": 0.7052434592},
        "log.final.comp.PeriQ": {"value": 4.4094950872e+14, "unit": u.m},
        "log.final.comp.Inc": {"value": 1.3949755231,"unit": u.rad},
    }
)
class TestGalaxyEffects(Benchmark):
    pass


    # Check
    # assert np.isclose(output.log.final.comp.SemiMajorAxis, 1.4959787070e+15)
    # assert np.isclose(output.log.final.comp.Eccentricity, 0.7052434592)
    # assert np.isclose(output.log.final.comp.PeriQ, 4.4094950872e+14)
    # assert np.isclose(output.log.final.comp.Inc, 1.3949755231)
