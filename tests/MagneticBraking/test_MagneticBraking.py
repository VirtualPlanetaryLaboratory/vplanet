from benchmark import Benchmark, benchmark
import astropy.units as u
import pytest


@benchmark(
    {
        "log.final.matt.RotPer": {"value": 0.252432, "unit": u.days},
        "log.final.reiners.RotPer": {"value":  1.059182, "unit": u.days},
        "log.final.sk.RotPer": {"value": 1.486450, "unit": u.days},
    }
)
class TestMagneticBraking(Benchmark):
    pass

    # # Confirm
    # assert np.isclose(output.log.final.matt.RotPer, 0.252432)
    # assert np.isclose(output.log.final.reiners.RotPer, 1.059182)
    # assert np.isclose(output.log.final.sk.RotPer, 1.486450)
    #
    # # Energy
    # Einit = output.log.initial.system.TotEnergy
    # Efinal = output.log.initial.system.TotEnergy
    #
    # # Angular momentum
    # Linit = output.log.initial.system.TotAngMom
    # Lfinal = output.log.final.system.TotAngMom
    #
    # # Check conservation
    # assert np.isclose(Einit, Efinal, rtol=1e-3)
    # assert np.isclose(Linit, Lfinal, rtol=1e-3)
