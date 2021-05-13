from benchmark import Benchmark, benchmark
import astropy.units as u
import pytest

@benchmark(
    {
        "log.final.secondary.HZLimRecVenus": {"value": 9.828450e+10, "unit": u.m},
        "log.final.secondary.HZLimEarlyMars": {"value": 2.673083e+11, "unit": u.m},
        "log.final.secondary.HZLimRunaway": {"value": 1.298768e+11, "unit": u.m},
        "log.final.secondary.HZLimRecVenus": {"value": 9.828450e+10, "unit": u.m},
        "log.final.secondary.HZLimMoistGreenhouse": {"value": 1.300464e+11, "unit": u.m},
        "log.final.secondary.HZLimMaxGreenhouse": {"value": 2.450954e+11, "unit": u.m},


    }
)
class TestHZBinary(Benchmark):
    pass



    # # Check
    # assert np.isclose(output.log.final.secondary.HZLimRecVenus, 9.828450e+10)
    # assert np.isclose(output.log.final.secondary.HZLimEarlyMars, 2.673083e+11)
    # assert np.isclose(output.log.final.secondary.HZLimRunaway, 1.298768e+11)
    # assert np.isclose(output.log.final.secondary.HZLimMoistGreenhouse, 1.300464e+11)
    # assert np.isclose(output.log.final.secondary.HZLimMaxGreenhouse, 2.450954e+11)
