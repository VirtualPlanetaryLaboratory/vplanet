from benchmark import Benchmark, benchmark
import astropy.units as u
import pytest


@benchmark(
    {
        "log.final.GJ1132b.PotTemp": {"value": 3358.229436, "unit": u.K},
        "log.final.GJ1132b.SurfTemp": {"value": 4000.000000, "unit": u.K},
        "log.final.GJ1132b.SolidRadius": {"value": 0.750604, "unit": u.Rearth},
        "log.final.GJ1132b.WaterMassMOAtm": {"value": 99.456333, "unit": u.TO},
        "log.final.GJ1132b.WaterMassSol": {"value": 0.174876, "unit": u.TO},
        "log.final.GJ1132b.OxygenMassMOAtm": {"value": 3.946430e18, "unit": u.kg},
        "log.final.GJ1132b.OxygenMassSol": {"value": 4.449172e17, "unit": u.kg},
        "log.final.GJ1132b.HydrogenMassSpace": {"value": 5.736131e19, "unit": u.kg},
        "log.final.GJ1132b.OxygenMassSpace": {"value": 4.508558e20, "unit": u.kg},
        "log.final.GJ1132b.CO2MassMOAtm": {"value": 1.748940e-296, "unit": u.kg},
        "log.final.GJ1132b.CO2MassSol": {"value": 1.748940e-296, "unit": u.kg},
    }
)
class TestMagmOcGJ1132b(Benchmark):
    pass
