from benchmark import Benchmark, benchmark
import astropy.units as u
import pytest


@benchmark(
    {
        "log.final.Earth.TGlobal": {"value": 14.649773, "unit": u.C},
        "log.final.Earth.Eccentricity": {"value": 0.0167},
        "log.final.Earth.Obliquity": {"value": 0.409103, "unit": u.rad},
        "log.final.Earth.AlbedoGlobal": {"value": 0.344298},
        "log.final.Earth.FluxOutGlobal": {"value": 233.918026, "unit": u.W / u.m ** 2},
        "log.final.Mercury.Eccentricity": {"value": 0.205631},
        "log.final.Saturn.LongA": {"value": 1.984656, "unit": u.rad},
        "log.final.George.ArgP": {"value": 1.688334, "unit": u.rad},
    }
)
class TestEarthClimate(Benchmark):
    pass
