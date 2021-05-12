from benchmark import Benchmark, benchmark
import astropy.units as u
import pytest


@benchmark(
    {
        "log.final.star.LXUVStellar": {"value": 3.120390e21, "unit": u.W},
        "log.final.b.EnvelopeMass": {
            "value": 0,
            "unit": u.Mearth,
            "marks": pytest.mark.xfail(
                reason="EnvelopeMass has units of `Earth`, which is ambiguous."
            ),
        },
        "log.final.b.Radius": {"value": 6.378100e06, "unit": u.m},
        "log.final.c.EnvelopeMass": {
            "value": 3.972871,
            "unit": u.Mearth,
            "marks": pytest.mark.xfail(
                reason="EnvelopeMass has units of `Earth`, which is ambiguous."
            ),
        },
        "log.final.c.Radius": {"value": 3.5899080e07, "unit": u.m},
    }
)
class TestAtmEscKepler36(Benchmark):
    pass
