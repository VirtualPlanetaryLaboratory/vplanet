from benchmark import Benchmark, benchmark
import astropy.units as u
import pytest


@benchmark(
    {
        "log.final.earth._40KPowerCrust": {"value": 1.168067, "unit": u.TW},
        "log.final.earth._232ThMassCore": {"value": 4.182962e+15, "unit": u.kg},
        "log.final.earth._235UPowerMan": {"value": 0.240654, "unit": u.TW},
        "log.final.earth._232ThNumCrust": {"value":  2.859381e+41},
        "log.final.earth.SurfEnFluxRadTotal": {"value": 0.047491, "unit": u.kg * u.m**2 / u.sec**2 / (u.m**2 * u.sec)},

    }
)
class TestRadHeat(Benchmark):
    pass


    # Check
    # assert np.isclose(output.log.final.earth._40KPowerCrust, 1.168067)
    # assert np.isclose(output.log.final.earth._232ThMassCore, 4.182962e+15)
    # assert np.isclose(output.log.final.earth._235UPowerMan,  0.240654)
    # assert np.isclose(output.log.final.earth._232ThNumCrust, 2.859381e+41)
    # assert np.isclose(output.log.final.earth.SurfEnFluxRadTotal, 0.047491)
