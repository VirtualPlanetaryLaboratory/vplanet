from benchmark import Benchmark, benchmark
import astropy.units as u
import pytest
import numpy as np


@benchmark(
    {
        "log.final.a.Luminosity": {"value": 0.002445, "unit": u.LSUN},
        "log.final.a.Temperature": {"value": 2992.330343, "unit": u.K},
        "log.final.a.Radius": {"value": 0.184359, "unit": u.Rsun, "rtol": 1e-4,},
        "log.final.a.LostAngMom": {
            "value": 9.438647e+41,
            "unit": u.kg * u.m ** 2 / u.sec,
            "rtol": 1e-4,
        },
        "log.final.a.RotPer": {"value": 0.299254, "unit": u.days},
        "log.final.b.Luminosity": {"value": 0.689088, "unit": u.LSUN, "rtol": 1e-4,},
        "log.final.b.Temperature": {"value": 5539.190016, "unit": u.K},
        "log.final.b.Radius": {"value": 0.902638, "unit": u.Rsun, "rtol": 1e-4,},
        "log.final.b.LostAngMom": {
            "value": 5.085996e+43,
            "unit": u.kg * u.m ** 2 / u.sec,
            "rtol": 1e-4,
        },
        "log.final.b.RotPer": {"value": 3.075721, "unit": u.days, "rtol": 1e-4},
        "log.final.b.Density": {"value": 1916.956727, "unit": u.kg / u.m ** 3},
    }
)
class TestStellarEvol(Benchmark):
    pass


def test_energy_conservation(vplanet_output):
    Einit = vplanet_output.log.initial.system.TotEnergy
    Efinal = vplanet_output.log.initial.system.TotEnergy
    assert np.isclose(Einit, Efinal, rtol=1e-3)


def test_angmom_conservation(vplanet_output):
    Linit = vplanet_output.log.initial.system.TotAngMom
    Lfinal = vplanet_output.log.final.system.TotAngMom
    assert np.isclose(Linit, Lfinal, rtol=1e-3)
