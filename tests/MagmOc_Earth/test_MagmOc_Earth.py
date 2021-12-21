from benchmark import Benchmark, benchmark
import astropy.units as u
import pytest


@benchmark(
    {
        "log.final.Earth.PotTemp": {"value": 2173.620881, "unit": u.K},
        "log.final.Earth.SurfTemp": {"value": 4000.000000, "unit": u.K},
        "log.final.Earth.SolidRadius": {"value": 0.919480, "unit": u.Rearth},
        "log.final.Earth.WaterMassMOAtm": {"value": 4.892812, "unit": u.TO},
        "log.final.Earth.WaterMassSol": {"value": 0.101555, "unit": u.TO},
        "log.final.Earth.OxygenMassMOAtm": {"value": 1.836760e18, "unit": u.kg},
        "log.final.Earth.OxygenMassSol": {"value": 8.739487e17, "unit": u.kg},
        "log.final.Earth.HydrogenMassSpace": {"value": 8.798414e+17, "unit": u.kg},
        "log.final.Earth.OxygenMassSpace": {"value": 4.272139e+18, "unit": u.kg},
        "log.final.Earth.CO2MassMOAtm": {"value": 1.753694e-296, "unit": u.kg},
        "log.final.Earth.CO2MassSol": {"value": 1.753694e-296, "unit": u.kg},
        "log.final.Earth.FracFe2O3Man": {"value": 1.894182e-05},
        "log.final.Earth.NetFluxAtmo": {"value": 815.884123},
        "log.final.Earth.RadioPower": {
            "value": 9.823253e+13, 
            "unit": u.kg * u.m**2 / u.s**3
            },
        "log.final.Earth.MeltFraction": {"value": 0.376273},
        "log.final.Earth.WaterFracMelt": {"value": 0.013344}
    }
)
class TestMagmOcEarth(Benchmark):
    pass
