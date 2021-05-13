from benchmark import Benchmark, benchmark
import astropy.units as u
import pytest


@benchmark(
    {
        "log.final.Earth.PotTemp": {"value": 2173.620881, "unit": u.K},
        "log.final.Earth.SurfTemp": {"value": 4000.000000, "unit": u.K},
        "log.initial.Earth.SolidRadius": {"value": 0.919480, "unit": u.Rearth},
        "log.initial.Earth.WaterMassMOAtm": {"value": 4.892812, "unit": u.TO},
        "log.initial.Earth.WaterMassSol": {"value": 0.101555, "unit": u.TO},
        "log.final.Earth.OxygenMassMOAtm": {"value": 1.836760e18, "unit": u.kg},
        "log.final.Earth.OxygenMassSol": {"value": 8.739487e17, "unit": u.kg},
        "log.initial.Earth.HydrogenMassSpace": {"value": 8.761559e17, "unit": u.kg},
        "log.initial.Earth.OxygenMassSpace": {"value": 4.242889e18, "unit": u.kg},
        "log.initial.Earth.CO2MassMOAtm": {"value": 1.753694e-296, "unit": u.kg},
        "log.initial.Earth.CO2MassSol": {"value": 1.753694e-296, "unit": u.kg},
    }
)
class TestMagmOcEarth(Benchmark):
    pass
