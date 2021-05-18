from benchmark import Benchmark, benchmark
import astropy.units as u
import pytest
import numpy as np


@benchmark(
    {
        "log.final.matt.RotPer": {"value": 0.252432, "unit": u.days},
        "log.final.reiners.RotPer": {"value": 1.059182, "unit": u.days},
        "log.final.sk.RotPer": {"value": 1.486450, "unit": u.days},
    }
)
class TestMagneticBraking(Benchmark):
    pass


def test_energy_conservation(vplanet_output):
    Einit = vplanet_output.log.initial.system.TotEnergy
    Efinal = vplanet_output.log.initial.system.TotEnergy
    assert np.isclose(Einit, Efinal, rtol=1e-3)


def test_angmom_conservation(vplanet_output):
    Linit = vplanet_output.log.initial.system.TotAngMom
    Lfinal = vplanet_output.log.final.system.TotAngMom
    assert np.isclose(Linit, Lfinal, rtol=1e-3)
