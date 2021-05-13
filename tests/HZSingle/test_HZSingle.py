from benchmark import Benchmark, benchmark
import astropy.units as u
import pytest

@benchmark(
    {
        "log.initial.a.HZLimRecVenus": {"value": 2.879712e+10, "unit": u.m},
        "log.initial.a.HZLimEarlyMars": {"value": 7.969376e+10, "unit": u.m},
        "log.initial.a.HZLimRunaway": {"value": 3.790332e+10, "unit": u.m},
        "log.initial.a.HZLimMoistGreenhouse": {"value": 3.810337e+10, "unit": u.m},
        "log.initial.a.HZLimMaxGreenhouse": {"value": 7.307316e+10, "unit": u.m},


    }
)
class TestHZSingle(Benchmark):
    pass


    # # Check
    # assert np.isclose(output.log.initial.a.HZLimRecVenus, 2.879712e+10)
    # assert np.isclose(output.log.initial.a.HZLimEarlyMars, 7.969376e+10)
    # assert np.isclose(output.log.initial.a.HZLimRunaway, 3.790332e+10)
    # assert np.isclose(output.log.initial.a.HZLimMoistGreenhouse, 3.810337e+10)
    # assert np.isclose(output.log.initial.a.HZLimMaxGreenhouse, 7.307316e+10)
