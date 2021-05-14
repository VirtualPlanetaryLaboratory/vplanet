from benchmark import Benchmark, benchmark
import astropy.units as u
import pytest


@benchmark(
    {
        "log.final.Pulsar.PositionXSpiNBody": {"value": 1.72523e05},
        "log.final.Pulsar.VelZSpiNBody": {"value": 0.10003},
        "log.final.A.EnvelopeMass": {"value": 0.99913, "unit": u.Mearth,},
        "log.final.A.PlanetRadius": {"value": 59.80530, "unit": u.Rearth,},
        "log.final.A.ArgP": {"value": 2.52340, "unit": u.rad},
        "log.final.A.PositionXSpiNBody": {"value": 2.61203e10},
        "log.final.B.LongA": {"value": 6.27639, "unit": u.rad},
        "log.final.B.VelXSpiNBody": {"value": 4.40398e04},
        "log.final.C.SemiMajorAxis": {"value": 6.97244e10, "unit": u.m},
        "log.final.C.PositionZSpiNBody": {"value": 4.87735e10},
    }
)
class TestNBodyAtmEsc(Benchmark):
    pass
