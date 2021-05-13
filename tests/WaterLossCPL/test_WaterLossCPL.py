from benchmark import Benchmark, benchmark
import astropy.units as u
import pytest


@benchmark(
    {
        "log.final.b.OrbPeriod": {"value": 3.943855e+05, "unit":u.sec},
        "log.final.b.Eccentricity": {"value": 0.099247},
        "log.final.b.SurfWaterMass": {"value": 9.974648, "unit": u.TO},

    }
)
class TestWaterLossCPL(Benchmark):
    pass


    # Check
    # assert np.isclose(output.log.final.b.OrbPeriod, 3.943855e+05)
    # assert np.isclose(output.log.final.b.Eccentricity, 0.099247)
    # assert np.isclose(output.log.final.b.SurfWaterMass, 9.974648)
