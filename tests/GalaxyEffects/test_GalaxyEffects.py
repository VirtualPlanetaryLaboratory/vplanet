from benchmark import Benchmark, benchmark
import astropy.units as u
import pytest


@benchmark(
    {
        "log.final.comp.SemiMajorAxis": {"value": 1.4959787070e15, "unit": u.m},
        "log.final.comp.Eccentricity": {"value": 0.7052434592},
        "log.final.comp.PeriQ": {"value": 4.4094950872e14, "unit": u.m},
        "log.final.comp.Inc": {"value": 1.3949755231, "unit": u.rad},
    }
)
class TestGalaxyEffects(Benchmark):
    pass
