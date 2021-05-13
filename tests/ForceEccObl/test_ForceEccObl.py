from benchmark import Benchmark, benchmark
import astropy.units as u
import pytest


@benchmark(
    {
        "log.final.earth.Eccentricity": {"value": 0.200000},
        "log.final.earth.Obliquity": {"value": 0.855211, "unit": u.rad},
        "log.final.earth.AnnInsol": {"value": 310.172861, "unit": u.W / u.m ** 2},
        "log.final.earth.DivFlux": {"value": -8.595168, "unit": u.W / u.m ** 2},
        "log.final.earth.EnergyResL": {"value": -0.414500},
        "log.final.earth.EnergyResW": {"value": 0.113084},
        "log.final.earth.AlbedoLandLat": {"value": 0.508116},
        "log.final.earth.AlbedoWaterLat": {"value": 0.309142},
    }
)
class TestForceEccObl(Benchmark):
    pass
