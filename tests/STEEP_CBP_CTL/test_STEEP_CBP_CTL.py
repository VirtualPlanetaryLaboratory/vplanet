from benchmark import Benchmark, benchmark
import astropy.units as u
import pytest


@benchmark(
    {
        "log.final.cbp.FreeEcc": {"value": 0.030000},
        "log.final.cbp.Eccentricity": {"value": 0.031100},
        "log.final.cbp.SemiMajorAxis": {"value": 1.048835e11, "unit": u.m},
        "log.final.secondary.Eccentricity": {"value": 0.313818},
        "log.final.secondary.SemiMajorAxis": {"value": 0.095744, "unit": u.au},
        "log.final.secondary.CriticalSemiMajorAxis": {"value": 0.307611, "unit": u.au},
    }
)
class TestSteepCBPCTL(Benchmark):
    pass
