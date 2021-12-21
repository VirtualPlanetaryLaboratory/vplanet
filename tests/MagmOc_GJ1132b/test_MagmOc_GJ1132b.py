from benchmark import Benchmark, benchmark
import astropy.units as u
import pytest


@benchmark(
    {
        "log.final.GJ1132b.PotTemp": {"value": 3357.987367, "unit": u.K},
        "log.final.GJ1132b.SurfTemp": {"value": 4000.000000, "unit": u.K},
        "log.final.GJ1132b.SolidRadius": {"value": 0.750750, "unit": u.Rearth},
        "log.final.GJ1132b.WaterMassMOAtm": {"value": 99.348154, "unit": u.TO},
        "log.final.GJ1132b.WaterMassSol": {"value": 0.174994, "unit": u.TO},
        "log.final.GJ1132b.OxygenMassMOAtm": {"value": 3.945920e+18, "unit": u.kg},
        "log.final.GJ1132b.OxygenMassSol": {"value": 4.454272e+17, "unit": u.kg},
        "log.final.GJ1132b.HydrogenMassSpace": {"value": 7.416899e+19, "unit": u.kg},
        "log.final.GJ1132b.OxygenMassSpace": {"value": 5.842497e+20, "unit": u.kg},
        "log.final.GJ1132b.CO2MassMOAtm": {"value": 1.748940e-296, "unit": u.kg},
        "log.final.GJ1132b.CO2MassSol": {"value": 1.748940e-296, "unit": u.kg},
    }
)
class TestMagmOcGJ1132b(Benchmark):
    pass
