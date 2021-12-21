from benchmark import Benchmark, benchmark
import astropy.units as u
import pytest


@benchmark(
    {
        "log.final.star.LXUVStellar": {
            "value": 4.988255e+21,
            "unit": u.kg * u.m ** 2 / u.sec ** 3
            },
        "log.final.ProxCenELim.EnvelopeMass": {"value": 0.000486, "unit": u.Mearth},
        "log.final.ProxCenELim.PlanetRadius": {"value": 1.336334, "unit": u.Rearth},
        "log.final.ProxCenELim.SurfWaterMass": {"value": 1.500000, "unit": u.TO},
        "log.final.ProxCenRRLim.EnvelopeMass": {"value": 0.000300, "unit": u.Mearth},
        "log.final.ProxCenRRLim.PlanetRadius": {"value": 1.339401, "unit": u.Rearth},
        "log.final.ProxCenRRLim.SurfWaterMass": {"value": 1.500000, "unit": u.TO},
        "log.final.ProxCenAuto.EnvelopeMass": {"value": 0.000493, "unit": u.Mearth},
        "log.final.ProxCenAuto.PlanetRadius": {"value": 1.336226, "unit": u.Rearth},
        "log.final.ProxCenAuto.SurfWaterMass": {"value": 1.500000, "unit": u.TO},
        "log.final.LC17ELim.EnvelopeMass": {"value": 0.000000, "unit": u.Mearth},
        "log.final.LC17ELim.PlanetRadius": {"value": 1.063513, "unit": u.Rearth},
        "log.final.LC17ELim.SurfWaterMass": {"value": 1.291071, "unit": u.TO},
        "log.final.LC17RRLim.EnvelopeMass": {"value": 0.000000, "unit": u.Mearth},
        "log.final.LC17RRLim.PlanetRadius": {"value": 1.063513, "unit": u.Rearth},
        "log.final.LC17RRLim.SurfWaterMass": {"value": 1.339746, "unit": u.TO},
        "log.final.LC17Auto.EnvelopeMass": {"value": 0.000000, "unit": u.Mearth},
        "log.final.LC17Auto.PlanetRadius": {"value": 1.063513, "unit": u.Rearth},
        "log.final.LC17Auto.SurfWaterMass": {"value": 1.339411, "unit": u.TO},
    }
)
class TestHabEvapCore(Benchmark):
    pass
