from benchmark import Benchmark, benchmark
import astropy.units as u
import pytest


@benchmark(
    {
        "log.final.planet.EnvelopeMass": {
            "value": 0.029770,
            "unit": u.Mearth,
            "marks": pytest.mark.xfail(
                reason="EnvelopeMass has units of `Earth`, which is ambiguous."
            ),
        },
        "log.final.planet.DEnvMassDt": {"value": -4.923106e08, "unit": u.kg / u.sec},
        "log.final.planet.Radius": {"value": 2.821863e07, "unit": u.m},
        "log.final.planet.Mass": {
            "value": 1.969770,
            "unit": u.Mearth,
            "marks": pytest.mark.xfail(
                reason="Mass has units of `Earth`, which is ambiguous."
            ),
        },
        "log.final.planet.ScaleHeight": {"value": 273.737851, "unit": u.km},
        "log.final.planet.PresSurf": {"value": 3.267509, "unit": u.GPa},
    }
)
class TestMiniNeptuneEvapLC17(Benchmark):
    pass
