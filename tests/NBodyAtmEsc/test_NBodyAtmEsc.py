from benchmark import Benchmark, benchmark
import astropy.units as u
import pytest


@benchmark(
    {
        "log.final.Pulsar.PositionXSpiNBody": {"value": 1.72523e+05},
        "log.final.Pulsar.VelZSpiNBody": {"value": 0.10003},

        "log.final.A.EnvelopeMass": {"value": 0.99913, "unit": u.Mearth}, #should be earth masses but is currently dimentionless?
        "log.final.A.PlanetRadius": {"value":  59.80530, "unit": u.Rearth}, #should be earth radii but is currently dimentionless?
        "log.final.A.ArgP": {"value": 2.52340, "unit": u.rad},
        "log.final.A.PositionXSpiNBody": {"value": 2.61203e+10},

        "log.final.B.LongA": {"value": 6.27639, "unit": u.rad},
        "log.final.B.VelXSpiNBody": {"value": 4.40398e+04},

        "log.final.C.SemiMajorAxis": {"value": 6.97244e+10, "unit": u.m},
        "log.final.C.PositionZSpiNBody": {"value": 4.87735e+10},
    }
)
class TestNBodyAtmEsc(Benchmark):
    pass

    # # Check
    # assert np.isclose(output.log.final.Pulsar.PositionXSpiNBody, 1.72523e+05)
    # assert np.isclose(output.log.final.Pulsar.VelZSpiNBody, 0.10003)
    #
    # assert np.isclose(output.log.final.A.EnvelopeMass, 0.99913)
    # assert np.isclose(output.log.final.A.PlanetRadius, 59.80530)
    # assert np.isclose(output.log.final.A.ArgP, 2.52340)
    # assert np.isclose(output.log.final.A.PositionXSpiNBody, 2.61203e+10)
    #
    # assert np.isclose(output.log.final.B.LongA, 6.27639)
    # assert np.isclose(output.log.final.B.VelXSpiNBody, 4.40398e+04)
    #
    # assert np.isclose(output.log.final.C.SemiMajorAxis,  6.97244e+10)
    # assert np.isclose(output.log.final.C.PositionZSpiNBody, 4.87735e+10)
