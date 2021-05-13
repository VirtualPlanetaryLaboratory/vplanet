from benchmark import Benchmark, benchmark
import astropy.units as u
import pytest


@benchmark(
    {
        "log.final.cbp.FreeEcc": {"value": 0.030000},
        "log.final.cbp.Eccentricity": {"value": 0.030249},
        "log.final.cbp.SemiMajorAxis": {"value": 1.052219e+11, "unit": u.m},

        "log.final.secondary.Eccentricity": {"value": 0.194791},
        "log.final.secondary.SemiMajorAxis": {"value": 0.072294, "unit": u.au},
        "log.final.secondary.CriticalSemiMajorAxis": {"value": 0.211427, "unit": u.au},

    }
)
class TestSteepCBPCPL(Benchmark):
    pass

    # Run our comparisons
    # assert np.isclose(output.log.final.cbp.FreeEcc, 0.030000)
    # assert np.isclose(output.log.final.cbp.Eccentricity, 0.030249)
    # assert np.isclose(output.log.final.cbp.SemiMajorAxis, 1.052219e+11)
    # assert np.isclose(output.log.final.secondary.Eccentricity, 0.194791)
    # assert np.isclose(output.log.final.secondary.SemiMajorAxis, 0.072294)
    # assert np.isclose(output.log.final.secondary.CriticalSemiMajorAxis, 0.211427)
