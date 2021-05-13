from benchmark import Benchmark, benchmark
import astropy.units as u
import pytest


@benchmark(
    {
        "log.final.planet.EnvelopeMass": {"value": 0.032361, "unit":u.Mearth},
        "log.final.planet.DEnvMassDt": {"value": -4.568120e+09, "unit":u.kg/u.sec},

        "log.final.planet.RadXUV": {"value": 29.425995, "unit": u.Rearth},
        "log.final.planet.RocheRadius": {"value":  29.425995, "unit": u.Rearth},
        "log.final.planet.ScaleHeight": {"value": 401.210931, "unit": u.km},
        "log.final.planet.PresSurf": {"value": 3.551934, "unit": u.GPa},
    }
)
class TestMiniNeptuneEvapDyn(Benchmark):
    pass


    # # Check Primary Variables
    # assert np.isclose(output.log.final.planet.EnvelopeMass, 0.032361)
    # assert np.isclose(output.log.final.planet.DEnvMassDt, -4.568120e+09)
    #
    # # Check other variables
    # assert np.isclose(output.log.final.planet.RadXUV, 29.425995)
    # assert np.isclose(output.log.final.planet.RocheRadius, 29.425995)
    # assert np.isclose(output.log.final.planet.ScaleHeight, 401.210931)
    # assert np.isclose(output.log.final.planet.PresSurf, 3.551934)
