from benchmark import Benchmark, benchmark
import astropy.units as u
import pytest


@benchmark(
    {
        "log.final.primary.RadGyra": {"value": 0.451302},
        "log.final.primary.RotPer": {"value": 3.577599, "unit": u.day, "rtol": 1e-4},
        "log.final.secondary.RotPer": {"value": 3.577599, "unit": u.day, "rtol": 1e-4},
        "log.final.secondary.RadGyra": {"value": 0.451302},
        "log.final.secondary.Eccentricity": {"value": 0.045484, "rtol": 1e-4},
        "log.final.secondary.SemiMajorAxis": {
            "value": 0.057677,
            "unit": u.AU,
            "rtol": 1e-4,
        },
    }
)
class TestBinaryTides(Benchmark):
    pass
