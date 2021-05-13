from benchmark import Benchmark, benchmark
import astropy.units as u
import pytest


@benchmark(
    {
        "log.final.cbp.FreeEcc": {"value": 0.030000},
        "log.final.cbp.Eccentricity": {"value": 0.031100},
        "log.final.cbp.SemiMajorAxis": {"value": 1.048835e+11, "unit": u.m},

        "log.final.secondary.Eccentricity": {"value": 0.313818},
        "log.final.secondary.SemiMajorAxis": {"value": 0.095744, "unit": u.au},
        "log.final.secondary.CriticalSemiMajorAxis": {"value": 0.307611, "unit": u.au},

    }
)
class TestSteepCBPCTL(Benchmark):
    pass

    # Run our comparisons
    # assert np.isclose(output.log.final.cbp.FreeEcc, 0.030000)
    # assert np.isclose(output.log.final.cbp.Eccentricity, 0.031100)
    # assert np.isclose(output.log.final.cbp.SemiMajorAxis, 1.048835e+11)
    # assert np.isclose(output.log.final.secondary.Eccentricity, 0.313818)
    # assert np.isclose(output.log.final.secondary.SemiMajorAxis, 0.095744)
    # assert np.isclose(output.log.final.secondary.CriticalSemiMajorAxis, 0.307611)
