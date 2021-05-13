from benchmark import Benchmark, benchmark
import astropy.units as u
import pytest


@benchmark(
    {
        "log.final.tidalearth.TMan": {"value": 2799.517833, "unit":u.K},
        "log.final.tidalearth.TCore": {"value": 5500.224551, "unit":u.K},
        "log.final.tidalearth.PowerEqtide": {"value": 0.005337, "unit": u.km*u.m**2 / u.sec**3},
        "log.final.tidalearth.Eccentricity": {"value":  0.490441},
        "log.final.tidalearth.SemiMajorAxis": {"value": 7.409421e+09, "unit": u.m},
    }
)
class TestTidalEarth(Benchmark):
    pass

    # Check
    # assert np.isclose(output.log.final.tidalearth.TMan, 2799.517833)
    # assert np.isclose(output.log.final.tidalearth.TCore, 5500.224551)
    # assert np.isclose(output.log.final.tidalearth.PowerEqtide, 0.005337)
    # assert np.isclose(output.log.final.tidalearth.Eccentricity, 0.490441)
    # assert np.isclose(output.log.final.tidalearth.SemiMajorAxis, 7.409421e+09)
