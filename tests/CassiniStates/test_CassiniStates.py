from benchmark import Benchmark, benchmark
import astropy.units as u
import pytest


@benchmark(
    {
        "log.final.b.Xobl": {"value": -0.706488},
        "log.final.b.Yobl": {"value": 0.046974},
        "log.final.b.Zobl": {"value": 0.706164},
        "log.final.b.HEcc": {"value": -0.062959},
        "log.final.b.KEcc": {"value": 0.010774},
        "log.final.b.RotRate": {"value": 7.270645e-05, "unit": 1 / u.s},
        "log.final.b.SemiMajorAxis": {"value": 1.869973e10, "unit": u.m},
        "log.final.b.Inc": {"value": 0.008680, "unit": u.rad},
        "log.final.b.ArgP": {"value": 4.733145, "unit": u.rad},
        "log.final.b.LongA": {"value": 0.148731, "unit": u.rad},
        "log.final.b.Obliquity": {"value": 0.786733, "unit": u.rad},
        "log.final.b.PrecA": {"value": 3.075203, "unit": u.rad},
        "log.final.b.RotAngMom": {"value": 4.415998e33, "unit": u.kg * u.m ** 2 / u.s},
        "log.final.b.RotKinEnergy": {"value": 1.605358e29, "unit": u.J},
        "log.final.c.Inc": {"value": 8.069275e-05, "unit": u.rad},
        "log.final.c.ArgP": {"value": 4.653590, "unit": u.rad},
        "log.final.c.LongA": {"value": 1.580510, "unit": u.rad},
    }
)
class TestCassiniStates(Benchmark):
    pass
