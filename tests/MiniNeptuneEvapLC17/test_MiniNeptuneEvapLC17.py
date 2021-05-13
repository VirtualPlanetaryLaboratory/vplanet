from benchmark import Benchmark, benchmark
import astropy.units as u
import pytest


@benchmark(
    {
        "log.final.planet.EnvelopeMass": {"value": 0.029770, "unit":u.Mearth},
        "log.final.planet.DEnvMassDt": {"value": -4.923106e+08, "unit":u.kg/u.sec},

        "log.final.planet.Radius": {"value": 2.821863e+07, "unit": u.m},
        "log.final.planet.Mass": {"value":  1.969770, "unit": u.Mearth},
        "log.final.planet.ScaleHeight": {"value": 273.737851, "unit": u.km},
        "log.final.planet.PresSurf": {"value": 3.267509, "unit": u.GPa},
    }
)
class TestMiniNeptuneEvapLC17(Benchmark):
    pass

    # # Check Primary Variables
    # assert np.isclose(output.log.final.planet.EnvelopeMass, 0.029770)
    # assert np.isclose(output.log.final.planet.DEnvMassDt, -4.923106e+08)
    #
    # # Check other variables
    # assert np.isclose(output.log.final.planet.Radius, 2.821863e+07)
    # assert np.isclose(output.log.final.planet.Mass, 1.969770)
    # assert np.isclose(output.log.final.planet.ScaleHeight, 273.737851)
    # assert np.isclose(output.log.final.planet.PresSurf, 3.267509)
