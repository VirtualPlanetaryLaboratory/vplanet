from benchmark import Benchmark, benchmark
import astropy.units as u
import pytest


@benchmark(
    {
        "log.final.planet.EnvelopeMass": {
            "value": 0.032361,
            "unit": u.Mearth,
            "marks": pytest.mark.xfail(
                reason="EnvelopeMass has units of `Earth`, which is ambiguous."
            ),
        },
        "log.final.planet.DEnvMassDt": {"value": -4.568120e09, "unit": u.kg / u.sec},
        "log.final.planet.RadXUV": {"value": 29.425995, "unit": u.Rearth},
        "log.final.planet.RocheRadius": {"value": 29.425995, "unit": u.Rearth},
        "log.final.planet.ScaleHeight": {"value": 401.210931, "unit": u.km},
        "log.final.planet.PresSurf": {"value": 3.551934, "unit": u.GPa},
    }
)
class TestMiniNeptuneEvapDyn(Benchmark):
    pass
