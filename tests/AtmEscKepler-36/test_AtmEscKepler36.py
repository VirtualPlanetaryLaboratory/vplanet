from benchmark import Benchmark, benchmark
import astropy.units as u
import pytest


@benchmark(
    {
        "log.final.star.LXUVStellar": {"value": 3.120390e21, "unit": u.W},
        "log.final.b.EnvelopeMass": {"value": 0, "unit": u.Mearth},
        "log.final.b.Radius": {"value": 6.378100e06, "unit": u.m},
        "log.final.c.EnvelopeMass": {"value": 3.298788, "unit": u.Mearth},
        "log.final.c.Radius": {"value": 3.372667e+07, "unit": u.m},
        "log.final.c.KTide": {"value": 0.849139}
    }
)
class TestAtmEscKepler36(Benchmark):
    pass
