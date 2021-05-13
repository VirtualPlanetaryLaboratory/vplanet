from benchmark import Benchmark, benchmark
import astropy.units as u
import pytest


@benchmark(
    {
        "log.final.Star. PositionXSpiNBody": {"value": -7.2996045904859365e+05},
        "log.final.Star.VelZSpiNBody": {"value": -0.0074053658185989},

        "log.final.Mercury.Inc": {"value": 0.1222430915966078, "unit": u.rad},
        "log.final.Mercury.VelXSpiNBody": {"value":  -5.6172614113858515e+04},

        "log.final.Venus.ArgP": {"value": 0.9555745288739304, "unit": u.rad},
        "log.final.Venus.PositionXSpiNBody": {"value": 1.0815757150556880e+11},

        "log.final.Earth.LongA": {"value": 6.0858921788484137, "unit": u.rad},
        "log.final.Earth.VelZSpiNBody": {"value": 1751.8263488484312802},

        "log.final.Mars.SemiMajorAxis": {"value": 2.2794298912149789e+11, "unit": u.m},
        "log.final.Mars.PositionYSpiNBody": {"value": -1.9293056341095392e+11},
    }
)
class TestSS_SpiNBody(Benchmark):
    pass


    # Check
    # assert np.isclose(output.log.final.Star. PositionXSpiNBody, -7.2996045904859365e+05)
    # assert np.isclose(output.log.final.Star.VelZSpiNBody, -0.0074053658185989)
    #
    # assert np.isclose(output.log.final.Mercury.Inc, 0.1222430915966078)
    # assert np.isclose(output.log.final.Mercury.VelXSpiNBody, -5.6172614113858515e+04)
    #
    # assert np.isclose(output.log.final.Venus.ArgP, 0.9555745288739304)
    # assert np.isclose(output.log.final.Venus.PositionXSpiNBody, 1.0815757150556880e+11)
    #
    # assert np.isclose(output.log.final.Earth.LongA, 6.0858921788484137)
    # assert np.isclose(output.log.final.Earth.VelZSpiNBody, 1751.8263488484312802)
    #
    # assert np.isclose(output.log.final.Mars.SemiMajorAxis,  2.2794298912149789e+11)
    # assert np.isclose(output.log.final.Mars.PositionYSpiNBody, -1.9293056341095392e+11)
