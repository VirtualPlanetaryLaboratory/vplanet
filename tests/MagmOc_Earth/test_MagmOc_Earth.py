from benchmark import Benchmark, benchmark
import astropy.units as u
import pytest

@benchmark(
    {
        "log.final.Earth.PotTemp": {"value": 2173.620881, "unit": u.K},
        "log.final.Earth.SurfTemp": {"value": 4000.000000, "unit": u.K},
        "log.initial.Earth.SolidRadius": {"value": 0.919480, "unit": u.REarth},
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

    #
    # # Primary Variables: Earth -- checks MagmOc parameters
    # assert np.isclose(output.log.final.Earth.PotTemp, 2173.620881)
    # assert np.isclose(output.log.final.Earth.SurfTemp, 4000.000000)
    # assert np.isclose(output.log.final.Earth.SolidRadius, 0.919480)
    # assert np.isclose(output.log.final.Earth.WaterMassMOAtm, 4.892812)
    # assert np.isclose(output.log.final.Earth.WaterMassSol, 0.101555)
    # assert np.isclose(output.log.final.Earth.OxygenMassMOAtm, 1.836760e18)
    # assert np.isclose(output.log.final.Earth.OxygenMassSol, 8.739487e17)
    # assert np.isclose(output.log.final.Earth.HydrogenMassSpace, 8.761559e17)
    # assert np.isclose(output.log.final.Earth.OxygenMassSpace, 4.242889e18)
    # assert np.isclose(output.log.final.Earth.CO2MassMOAtm, 1.753694e-296)
    # assert np.isclose(output.log.final.Earth.CO2MassSol, 1.753694e-296)
