from benchmark import Benchmark, benchmark
import astropy.units as u
import pytest


@benchmark(
    {
        "log.final.g.PotTemp": {"value": 2341.655847, "unit": u.K},
        "log.final.g.SurfTemp": {"value": 4000.000000, "unit": u.K},
        "log.final.g.SolidRadius": {"value": 1.007038, "unit": u.Rearth},
        "log.final.g.WaterMassMOAtm": {"value": 1.970302, "unit": u.TO},
        "log.final.g.WaterMassSol": {"value": 0.027131, "unit": u.TO},
        "log.final.g.OxygenMassMOAtm": {"value": 2.229490e17, "unit": u.kg},
        "log.final.g.OxygenMassSol": {"value": 8.607177e16, "unit": u.kg},
        "log.final.g.HydrogenMassSpace": {"value": 3.992772e17, "unit": u.kg},
        "log.final.g.OxygenMassSpace": {"value": 2.859836e18, "unit": u.kg},
        "log.final.g.CO2MassMOAtm": {"value": 1.755002e-297, "unit": u.kg},
        "log.final.g.CO2MassSol": {"value": 1.755002e-297, "unit": u.kg},
    }
)
class TestMagmOcTrappist1g(Benchmark):
    pass
