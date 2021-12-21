from benchmark import Benchmark, benchmark
import astropy.units as u
import pytest


@benchmark(
    {
        "log.final.planet.EnvelopeMass": {"value": 0.022337, "unit": u.Mearth},
        "log.final.planet.DEnvMassDt": {"value": -5.634095e+08, "unit": u.kg / u.sec},
        "log.final.planet.Radius": {"value": 2.717649e+07, "unit": u.m},
        "log.final.planet.Mass": {"value": 1.962337, "unit": u.Mearth},
        "log.final.planet.ScaleHeight": {"value": 273.737851, "unit": u.km},
        "log.final.planet.PresSurf": {"value": 2.451673, "unit": u.GPa},
        "log.final.planet.RadXUV": {"value": 4.260907, "unit": u.Rearth},
        "log.final.planet.RocheRadius": {"value": 29.376060, "unit": u.Rearth},
        "log.final.planet.BondiRadius": {"value": 7.750709e+08, "unit": u.m}
    }
)
class TestMiniNeptuneEvapLC17(Benchmark):
    pass
