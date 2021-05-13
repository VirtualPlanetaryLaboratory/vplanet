from benchmark import Benchmark, benchmark
import astropy.units as u
import pytest

@benchmark(
    {
        "log.final.g.PotTemp": {"value": 2341.655847, "unit": u.K},
        "log.final.g.SurfTemp": {"value": 4000.000000, "unit": u.K},

        "log.initial.g.SolidRadius": {"value": 1.007038, "unit": u.Rearth},

        "log.initial.g.WaterMassMOAtm": {"value":  1.970302, "unit": u.TO},
        "log.initial.g.WaterMassSol": {"value": 0.027131, "unit": u.TO},

        "log.final.g.OxygenMassMOAtm": {"value": 2.229490e17, "unit": u.kg},
        "log.final.g.OxygenMassSol": {"value": 8.607177e16, "unit": u.kg},

        "log.initial.g.HydrogenMassSpace": {"value": 3.992772e17, "unit": u.kg},
        "log.initial.g.OxygenMassSpace": {"value": 2.859836e18, "unit": u.kg},

        "log.initial.g.CO2MassMOAtm": {"value": 1.755002e-297, "unit": u.kg},
        "log.initial.g.CO2MassSol": {"value": 1.755002e-297, "unit": u.kg},


    }
)
class TestMagmOcTrappist1g(Benchmark):
    pass

    # # Primary Variables: Trappist-1 g -- checks MagmOc parameters
    # assert np.isclose(output.log.final.g.PotTemp, 2341.655847)
    # assert np.isclose(output.log.final.g.SurfTemp, 4000.000000)
    #
    # assert np.isclose(output.log.final.g.SolidRadius, 1.007038)
    #
    # assert np.isclose(output.log.final.g.WaterMassMOAtm, 1.970302)
    # assert np.isclose(output.log.final.g.WaterMassSol, 0.027131)
    #
    # assert np.isclose(output.log.final.g.OxygenMassMOAtm, 2.229490e17)
    # assert np.isclose(output.log.final.g.OxygenMassSol, 8.607177e16)
    #
    # assert np.isclose(output.log.final.g.HydrogenMassSpace, 3.992772e17)
    # assert np.isclose(output.log.final.g.OxygenMassSpace, 2.859836e18)
    #
    # assert np.isclose(output.log.final.g.CO2MassMOAtm, 1.755002e-297)
    # assert np.isclose(output.log.final.g.CO2MassSol, 1.755002e-297)
